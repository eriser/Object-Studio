#include "cdelay.h"

CDelay::CDelay():ReadPosition(0),WavePosition(0),CleanMix(0),EffectMix(0), RegenCleanMix(0),RegenEffectMix(0),DelayRate(0)
{
}

CDelay::~CDelay() {
    if (m_Initialized) delete[] Buffer;
}

void CDelay::Init(const int Index, void *MainWindow) {
    m_Name=devicename;
    Buffer=new float[m_Presets.DoubleRate];
    ZeroMemory(Buffer,m_Presets.DoubleRate*sizeof(float));
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Wave,IJack::Out,jnOut);
    AddJack("EffectOut",IJack::Wave,IJack::Out,jnEffectOut);
    AddJack("In",IJack::Wave,IJack::In,0);
    AddParameter(ParameterType::Numeric,"Modulation Frequency","Hz",1,1000,100,"",100);
    AddParameter(ParameterType::Numeric,"Modulation Amplitude","%",0,100,0,"",0);
    AddParameter(ParameterType::Numeric,"Delay Time","ms",1,2000,0,"",10);
    AddParameter(ParameterType::Numeric,"Delay Regeneration","%",0,100,0,"",0);
    AddParameter(ParameterType::Numeric,"Effect","%",0,100,0,"",100);
    CalcParams();
}

void CDelay::Tick() {
    if (m_ParameterValues[pnAmplitude]>0)
    {
        WavePosition=WavePosition+((float)m_ParameterValues[pnFrequency]*m_BufferSize*0.01);
        while (WavePosition>=m_Presets.SampleRate) WavePosition=WavePosition-m_Presets.SampleRate;
        CurrentMod=(WaveBank.GetNext(WavePosition,CWaveBank::Triangle))*(float)m_ParameterValues[pnAmplitude];
    }
    m_Process=true;
}

void CDelay::CalcParams() {
    CleanMix=(float)(100 - m_ParameterValues[pnMix])*0.01;
    EffectMix=(float)m_ParameterValues[pnMix]*0.012;
    RegenCleanMix=(float)(200-m_ParameterValues[pnRegen])* 0.002;
    RegenEffectMix=(float)m_ParameterValues[pnRegen]*0.008;
    DelayRate=(float)m_ParameterValues[pnDelay]*(float)m_Presets.DoubleRate*0.001;
}

void CDelay::Process() {
    float* Signal=FetchA(jnIn);
    float* OutBuffer=AudioBuffers[jnOut]->Buffer;
    float* EffectBuffer=AudioBuffers[jnEffectOut]->Buffer;
    for (int i=0;i<m_BufferSize;i++)
    {
        ReadPosition++;
        float WritePositionF=DelayRate+(float)ReadPosition;
        if (m_ParameterValues[pnAmplitude]>0) WritePositionF+=CurrentMod;
        int WritePosition=WritePositionF;
        while (ReadPosition>=m_Presets.DoubleRate) ReadPosition=ReadPosition-m_Presets.DoubleRate;
        while (WritePosition<0) WritePosition=WritePosition+m_Presets.DoubleRate;
        while (WritePosition>=m_Presets.DoubleRate) WritePosition=WritePosition-m_Presets.DoubleRate;
        if (!Signal)
        {
            float fTemp=(float)Buffer[ReadPosition]*RegenEffectMix;
            EffectBuffer[i]=Buffer[ReadPosition];
            Buffer[WritePosition]=fTemp;
            OutBuffer[i]=fTemp*EffectMix;
        }
        else
        {
            float fTemp=((float)Buffer[ReadPosition]*RegenEffectMix)+((float)Signal[i]*RegenCleanMix);
            EffectBuffer[i]=Buffer[ReadPosition];
            Buffer[WritePosition]=fTemp;
            OutBuffer[i]=(fTemp*EffectMix)+((float)Signal[i]*CleanMix);
        }
    }
}
