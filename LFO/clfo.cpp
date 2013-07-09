#include "clfo.h"


CLFO::CLFO():WavePosition (0),FreqValue(0)
{

}

void CLFO::Init(const int Index, void *MainWindow) {
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Pitch,IJack::Out,jnOutPitch);
    AddJack("Out Amplitude",IJack::Amplitude,IJack::Out,jnOutAmplitude);
    AddParameter(ParameterType::Numeric,"Frequency","Hz",1,10000,100,"",400);
    AddParameter(ParameterType::SelectBox,"WaveForm","",0,5,0,"Sine§Square§Triangle§Sawtooth§Noise§Sample and Hold",0);
    AddParameter(ParameterType::dB,"Volume","dB",0,200,0,"",100);
    CalcParams();
}

const float CLFO::GetNext(const int ProcIndex) {
    if (ProcIndex==jnOutPitch) return ReturnValue;
    return (ReturnValue+1.0)*0.5;
}

void CLFO::Tick() {
    WavePosition=WavePosition+FreqValue;
    while (WavePosition>=m_Presets.SampleRate)
    {
        WavePosition=WavePosition-m_Presets.SampleRate;
    }
    ReturnValue = WaveBank.GetNext(WavePosition,(CWaveBank::WaveForms)m_ParameterValues[pnWaveForm])*VolumeFactor;
}

void CLFO::CalcParams() {
    VolumeFactor=m_ParameterValues[pnVolume]*0.01;
    FreqValue=m_ParameterValues[pnFrequency]*m_BufferSize*0.01;
}
