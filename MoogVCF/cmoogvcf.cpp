#include "cmoogvcf.h"


CMoogVCF::CMoogVCF()
{
}

void CMoogVCF::Init(const int Index,void* MainWindow)
{
    m_Name=devicename;
    Maxcutoff=m_Presets.SampleRate * 0.1;
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Wave,IJack::Out,0);
    AddJack("In",IJack::Wave,IJack::In,0);
    AddJack("Modulation",(IJack::AttachModes)(IJack::Amplitude | IJack::Pitch),IJack::In,0);
    AddParameter(ParameterType::dB,"In Volume","dB",0,200,0,"",100);
    AddParameter(ParameterType::Numeric,"Cutoff Modulation","%",0,100,0,"",0);
    AddParameter(ParameterType::Numeric,"Cutoff Frequency","Hz",20,Maxcutoff,0,"",Maxcutoff);
    AddParameter(ParameterType::Numeric,"Response Time","%",0,100,0,"",50);
    AddParameter(ParameterType::Numeric,"Resonance","%",0,100,0,"",0);
    AddParameter(ParameterType::dB,"Out Volume","dB",0,200,0,"",100);
    FreqGlider.SetSpeed(5);
    ModulationFactor=0;
    InVolumeFactor=0;
    LastResonance=0;
    LastCO=0;
    In1=0;
    In2=0;
    In3=0;
    In4=4;
    Out1=0;
    Out2=0;
    Out3=0;
    Out4=0;
    CalcExpResonance(0,0);
    CalcParams();
}

float* CMoogVCF::GetNextA(const int ProcIndex)
{
    float* InSignal=FetchA(jnIn);
    if (!InSignal) return NULL;
    bool Recalc=false;
    float CutOff=m_ParameterValues[pnCutOffFrequency];
    if (ModulationFactor) CutOff *= pow(2.0,(float)Fetch(jnModulation)*ModulationFactor);
    if (m_ParameterValues[pnResonance] != LastResonance)
    {
        LastResonance=m_ParameterValues[pnResonance];
        Recalc=true;
    }
    if (CutOff>Maxcutoff) CutOff=Maxcutoff;
    if (CutOff<20) CutOff=20;
    if (LastCO!=CutOff)
    {
        FreqGlider.SetTargetFreq(CutOff);
        LastCO=CutOff;
        Recalc=true;
    }
    float CurrentFreq=FreqGlider.GetCurrentFreq();
    if (LastCO != CurrentFreq) Recalc=true;
    if (Recalc) CalcExpResonance(CurrentFreq,LastResonance);
    float* Buffer=AudioBuffers[ProcIndex]->Buffer;
    for (int i=0;i<m_BufferSize;i++)
    {
        float Signal=*(InSignal+i) * InVolumeFactor;
        Signal -= Out4 * fb;
        Signal *= fa;
        Out1 = Signal + 0.3 * In1 + (1 - f) * Out1; // Pole 1
        In1 = Signal;
        Out2 = Out1 + 0.3 * In2 + (1 - f) * Out2; // Pole 2
        In2 = Out1;
        Out3 = Out2 + 0.3 * In3 + (1 - f) * Out3; // Pole 3
        In3 = Out2;
        Out4 = Out3 + 0.3 * In4 + (1 - f) * Out4; // Pole 4
        In4 = Out3;
        Buffer[i]= Out4 * OutVolumeFactor;
    }
    return Buffer;
}

void inline CMoogVCF::CalcParams()
{
    ModulationFactor=(float)m_ParameterValues[pnCutOffModulation]*0.01;
    InVolumeFactor=(float)m_ParameterValues[pnInVolume]*0.01;
    OutVolumeFactor=(float)m_ParameterValues[pnOutVolume]*0.01;
    FreqGlider.SetGlide(m_ParameterValues[pnResponse]);
}

void inline CMoogVCF::CalcExpResonance(float CutOff,float Resonance)
{
    float fc=CutOff/Maxcutoff;
    float res=Resonance/25;
    f = fc * 1.16;
    fb = res * (1.0 - 0.15 * f * f);
    fa=0.35013 * (f*f)*(f*f);
}
