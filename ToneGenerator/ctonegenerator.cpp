#include "ctonegenerator.h"

CToneGenerator::CToneGenerator():WavePosition(0),DetunePosition(0),CurrentMod(0),LastFreq(0),LastMod(0)
{
}

void CToneGenerator::Init(const int Index, void *MainWindow) {
    m_Name="ToneGenerator";
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Wave,IJack::Out,0);
    AddJack("Frequency",IJack::Frequency,IJack::In,0);
    AddJack("Modulation",IJack::Pitch,IJack::In,0);
    AddJack("Pulse Modulation",IJack::Pitch,IJack::In,0);
    AddParameter(ParameterType::Numeric,"Frequency","Hz",100,m_Presets.HalfRate*100,100,"",44000);
    AddParameter(ParameterType::Numeric,"Glide","%",0,100,0,"",0);
    AddParameter(ParameterType::Numeric,"Modulation","%",0,100,0,"",0);
    AddParameter(ParameterType::Numeric,"Tuning","%",-100,100,0,"",0);
    AddParameter(ParameterType::Numeric,"Detune","%",-100,100,0,"",0);
    AddParameter(ParameterType::SelectBox,"WaveForm","",0,5,0,"Sine§Square§Triangle§Sawtooth§Noise§Sample and Hold",0);
    AddParameter(ParameterType::Numeric,"PulseWave","",-100,100,0,"",0);
    AddParameter(ParameterType::Numeric,"PulseWave Modulation","",0,100,0,"",0);
    AddParameter(ParameterType::dB,"Volume","dB",0,200,0,"",100);
    CalcParams();
}

float *CToneGenerator::GetNextA(const int ProcIndex) {
    float FreqIn=Fetch(jnFrequency) * FreqResolutionMultiply;
    float ModIn=0;
    float PulseModIn=0;
    if (m_ParameterValues[pnModulation]) ModIn=Fetch(jnModulation);
    if (m_ParameterValues[pnPulseModulation]) PulseModIn=Fetch(jnPulseModulation);
    if (FreqIn>0)
    {
        if (FreqIn != LastFreqValue)
        {
            m_ParameterValues[pnFrequency]=FreqIn*100;
            FreqValue=FreqIn;
        }
        FreqGlider.SetTargetFreq(FreqIn);
        LastFreqValue=FreqIn;
    }
    else
    {
        FreqGlider.SetTargetFreq(FreqValue);
        LastFreqValue=FreqValue;
    }
    CurrentFreq=FreqGlider.GetCurrentFreq();
    if (ModIn != LastMod)
    {
        LastMod=ModIn;
        CurrentMod=pow(2.0,(float)ModIn * (float)m_ParameterValues[pnModulation] *0.01);
    }
    float CurrentFrequency=CurrentFreq;
    if (CurrentTune != 0) CurrentFrequency*=CurrentTune;
    if (CurrentMod != 0) CurrentFrequency*=CurrentMod;
    float* Buffer=AudioBuffers[ProcIndex]->Buffer;
    for (int i=0;i<m_BufferSize;i++)
    {
        WavePosition+=CurrentFrequency;
        while (WavePosition>=m_Presets.SampleRate) WavePosition-=m_Presets.SampleRate;
        while (WavePosition<0) WavePosition+=m_Presets.SampleRate;
        if (CurrentDetune==0)
        {
            Buffer[i]=(WaveBank.GetNext(PulseCalc(WavePosition,PulseModIn),(CWaveBank::WaveForms)m_ParameterValues[pnWaveForm]))*VolumeFactor;
        }
        else
        {
            DetunePosition=DetunePosition+(CurrentFrequency*CurrentDetune);
            while (DetunePosition>=m_Presets.SampleRate) DetunePosition-=m_Presets.SampleRate;
            while (DetunePosition<0) DetunePosition+=m_Presets.SampleRate;
            Buffer[i]=((WaveBank.GetNext(PulseCalc(WavePosition,PulseModIn),(CWaveBank::WaveForms)m_ParameterValues[pnWaveForm]))+(WaveBank.GetNext(PulseCalc(DetunePosition,PulseModIn),(CWaveBank::WaveForms)m_ParameterValues[pnWaveForm])))*VolumeFactor*0.5;
        }
    }
    return Buffer;
}

int CToneGenerator::PulseCalc(int Pos, float Modulation) {
    float Mod=PulseFactor;
    if (Modulation)
    {
        float ModIn=(float)Modulation*m_ParameterValues[pnPulseModulation]*0.01;
        Mod=ModIn+Mod;
        if (Mod>0.99) Mod=0.99;
        if (Mod<-0.99) Mod=-0.99;
    }
    if (Mod==0) return Pos;
    int T=((float)m_Presets.HalfRate*Mod)+m_Presets.HalfRate;
    if (Pos<T) return (m_Presets.HalfRate*Pos)/T;
    return m_Presets.HalfRate+((m_Presets.HalfRate*(Pos-T))/(m_Presets.SampleRate-T));
}

void CToneGenerator::CalcParams() {
    CurrentTune=pow(2.0,m_ParameterValues[pnTuning]*0.01);
    CurrentDetune=pow(2.0,m_ParameterValues[pnDetune]*0.01);
    VolumeFactor=(float)m_ParameterValues[pnVolume]*0.01;
    FreqValue=(float)m_ParameterValues[pnFrequency]*0.01;
    FreqGlider.SetGlide(m_ParameterValues[pnGlide]);
    PulseFactor=(m_ParameterValues[pnPulse]) * 0.0099;
}
