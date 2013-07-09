#include "csampler.h"
#include "csamplerform.h"

CSampler::CSampler()
{
}

void CSampler::Init(const int Index,void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("MIDI In",IJack::MIDI,IJack::In,jnMIDIIn);
    AddJack("Modulation",IJack::Pitch,IJack::In,jnModulation);
    AddJack("Out",IJack::Stereo,IJack::Out,jnOut);

    AddParameter(ParameterType::SelectBox,"MIDI Channel","",0,16,0,"All§1§2§3§4§5§6§7§8§9§10§11§12§13§14§15§16",0);
    AddParameter(ParameterType::Numeric,"Transpose","Half tones",-24,24,0,"",0);
    AddParameter(ParameterType::Numeric,"Tune","Hz",43600,44800,100,"",44000);
    AddParameter(ParameterType::Numeric,"Modulation","%",0,100,0,"",0);
    VolumeFactor=1.0*(1.0/sqrtf(Sampler::samplervoices));
    LastMod=0;
    CurrentMod=1;
    SamplerDevice.ChangePath(0,0,":/test.wav");
    SamplerDevice.reset();
    m_Form=new CSamplerForm(this,(QWidget*)MainWindow);
    ((CSamplerForm*)m_Form)->Init(&SamplerDevice);
    CalcParams();
}

void CSampler::Process()
{
    CStereoBuffer* OutBuffer=(CStereoBuffer*)AudioBuffers[jnOut];
    if (SamplerDevice.TestMode==CSamplerDevice::st_NoTest)
    {
        float ModIn=0;
        if (m_ParameterValues[pnModulation]) ModIn=Fetch(jnModulation);
        if (ModIn != LastMod)
        {
            LastMod=ModIn;
            CurrentMod=pow(2.0,(float)ModIn * (float)m_ParameterValues[pnModulation] * 0.01);
        }
        SamplerDevice.parseMIDI((CMIDIBuffer*)FetchP(jnMIDIIn));
        bool First=true;
        for (int i1=0;i1<SamplerDevice.voiceCount();i1++)
        {
            float* BufferL=SamplerDevice.getNext(i1,CurrentMod);
            if (BufferL)
            {
                float volL=VolumeFactor*SamplerDevice.volL(SamplerDevice.voiceChannel(i1));
                float volR=VolumeFactor*SamplerDevice.volR(SamplerDevice.voiceChannel(i1));
                if (First)
                {
                    First=false;
                    OutBuffer->FromBuffer(BufferL,volL,volR);
                }
                else
                {
                    OutBuffer->AddBuffer(BufferL,volL,volR);
                }
            }
        }
        if (First) OutBuffer->ZeroBuffer();
    }
    else if (SamplerDevice.TestMode==CSamplerDevice::st_LoopTest)
    {
        AudioBuffers[jnOut]->ZeroBuffer();
        SamplerDevice.LoopTest(OutBuffer->Buffer,OutBuffer->BufferR,m_BufferSize);
    }
    else if (SamplerDevice.TestMode==CSamplerDevice::st_TuneTest)
    {
        AudioBuffers[jnOut]->ZeroBuffer();
        SamplerDevice.TuneTest(OutBuffer->Buffer,OutBuffer->BufferR,m_BufferSize);
    }
}

void inline CSampler::CalcParams()
{
    VolumeFactor=1.0*(1.0/sqrtf(Sampler::samplervoices));
    SamplerDevice.setTune(m_ParameterValues[pnTune]*0.01);
    SamplerDevice.setTranspose(m_ParameterValues[pnTranspose]);
    SamplerDevice.setChannel(m_ParameterValues[pnMIDIChannel]);
}

void CSampler::Play(const bool FromStart)
{
    if (FromStart)
    {
        SamplerDevice.reset();
        ((CSamplerForm*)m_Form)->ReleaseLoop();
        CalcParams();
    }
}

void CSampler::Pause()
{
    SamplerDevice.allNotesOff();
    CalcParams();
}
