#include "cfilter.h"

CFilter::CFilter()
{
}

void CFilter::Init(const int Index, void *MainWindow) {
    m_Name=devicename;
    Maxcutoff=m_Presets.SampleRate * 0.425;
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
    FiltCoefTab0=0;
    FiltCoefTab1=0;
    FiltCoefTab2=0;
    FiltCoefTab3=0;
    FiltCoefTab4=0;
    ly1=0;
    ly2=0;
    lx1=0;
    lx2=0;
    m_ExpResonance=0;
    MixFactor=0;
    ModulationFactor=0;
    InVolumeFactor=0;
    LastResonance=0;
    LastCO=0;
    CalcExpResonance();
    CalcParams();
}

float *CFilter::GetNextA(const int ProcIndex) {
    float* InSignal=FetchA(jnIn);
    if (!InSignal) return NULL;
    bool Recalc=false;
    float CutOff=m_ParameterValues[pnCutOffFrequency];
    if (ModulationFactor) CutOff*= pow(2,Fetch(jnModulation)*ModulationFactor);
    if (m_ParameterValues[pnResonance] != LastResonance)
    {
        CalcExpResonance();
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
    if (Recalc)
    {
        float Omega=(DoublePi * CurrentFreq) / m_Presets.HalfRate;
        float sn=sin(Omega);
        float cs=cos(Omega);
        float Alpha=sn / m_ExpResonance;
        float b1= 1-cs;
        float b0= b1*0.5;
        float b2= b0;
        float a0=1+Alpha;
        float a1=-2*cs;
        float a2=1-Alpha;
        FiltCoefTab0=b0/a0;
        FiltCoefTab1=b1/a0;
        FiltCoefTab2=b2/a0;
        FiltCoefTab3=-a1/a0;
        FiltCoefTab4=-a2/a0;
        MixFactor=((float)Maxcutoff / (float)CurrentFreq) * 0.004;
        MixFactor=0.01 / ((m_ExpResonance*MixFactor)+(1-MixFactor));
    }
    float* Buffer=AudioBuffers[ProcIndex]->Buffer;
    for (int i=0;i<m_BufferSize;i++)
    {
        float Signal=*(InSignal+i) * InVolumeFactor;
        float Temp_y=(FiltCoefTab0 * Signal) + (FiltCoefTab1 * lx1) + (FiltCoefTab2 * lx2) + (FiltCoefTab3 * ly1) + (FiltCoefTab4 * ly2);
        ly2=ly1;
        ly1=Temp_y;
        lx2=lx1;
        lx1=Signal;
        Buffer[i]=(Temp_y * MixFactor) * (float)m_ParameterValues[pnOutVolume];
    }
    return Buffer;
}

void CFilter::CalcExpResonance()
{
    m_ExpResonance=exp((float)m_ParameterValues[pnResonance]/16);
}

void CFilter::CalcParams() {
    ModulationFactor=(float)m_ParameterValues[pnCutOffModulation]* 0.01;
    InVolumeFactor=(float)m_ParameterValues[pnInVolume]*0.01;
    FreqGlider.SetGlide(m_ParameterValues[pnResponse]);
}
