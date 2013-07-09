#include "csamplerdevice.h"

CSamplerDevice::CSamplerDevice()
{
    CurrentLayerIndex=0;
    CurrentRangeIndex=0;
    TestMode=st_NoTest;
    Looping=false;
    AddLayer();
}

void CSamplerDevice::NoteOn(short channel, short pitch, short velocity)
{
    if (channelSettings[channel].portNote)
    {
        for (int i=0;i<Sampler::samplervoices;i++)
        {
            if ((channel) == SamplerGenerator[i].Channel)
            {
                if (SamplerGenerator[i].ID==channelSettings[channel].portNote)
                {
                    SamplerGenerator[i].ID=pitch;
                    SamplerGenerator[i].addTranspose(pitch-channelSettings[channel].portNote);
                    break;
                }
            }
        }
    }
    else
    {
        int FreeIndex=-1;
        for (int i=0;i<Sampler::samplervoices;i++)
        {
            if (SamplerGenerator[i].ID==0)
            {
                FreeIndex=i;
                break;
            }
        }
        if (FreeIndex>-1)
        {
            SamplerGenerator[FreeIndex].ID=pitch;
            SamplerGenerator[FreeIndex].Channel=channel;
            SamplerGenerator[FreeIndex].resetTranspose();
            SamplerGenerator[FreeIndex].ResetSample(pitch+m_Transpose,velocity & 0x7F);
        }
    }
    channelSettings[channel].portNote=0;
}

void CSamplerDevice::NoteOff(short channel, short pitch)
{
    for (int i=0;i<Sampler::samplervoices;i++)
    {
        if ((channel) == SamplerGenerator[i].Channel)
        {
            if (channelSettings[channel].pedal)
            {
                channelSettings[channel].pedalnotes.append(pitch);
            }
            else if (pitch==SamplerGenerator[i].ID)
            {
                SamplerGenerator[i].EndSample();
                break;
            }
        }
    }
}

void CSamplerDevice::Aftertouch(const short channel, const short pitch, const short value)
{
    for (int i=0;i<Sampler::samplervoices;i++)
    {
        if ((channel) == SamplerGenerator[i].Channel)
        {
            if (pitch==SamplerGenerator[i].ID)
            {
                SamplerGenerator[i].setAftertouch(value);
            }
        }
    }
}

void CSamplerDevice::allNotesOff()
{
    TestMode=st_NoTest;
    Looping=false;
    for (int i=0;i<Sampler::samplervoices;i++)
    {
        if (!SamplerGenerator[i].FinishedPlaying)
        {
            SamplerGenerator[i].EndSample();
        }
    }
}

float* CSamplerDevice::getNext(const int voice, const float modulation)
{
    SamplerGenerator[voice].setPitchWheel(channelSettings[voiceChannel(voice)].pitchWheel);
    return SamplerGenerator[voice].GetNext(modulation);
}

void CSamplerDevice::LoopTest(float* BufferL,float* BufferR,int Samples)
{
    CWaveGenerator* WG=&CurrentRange()->WG;
    if (!Looping)
    {
        Looping=true;
        WG->Reset();
    }
    if (WG->Channels==1)
    {
        float* B=WG->GetNext((float)0);
        if (B)
        {
            for (int i=0;i<Samples;i++)
            {
                float Signal=B[i]*0.5;
                BufferL[i]+=Signal;
                BufferR[i]+=Signal;
            }
        }
    }
    else
    {
        float* BL=WG->GetNext((float)0);
        if (BL)
        {
            float* BR=BL+Samples;
            for (int i=0;i<Samples;i++)
            {
                BufferL[i]+=BL[i]*0.5;
                BufferR[i]+=BR[i]*0.5;
            }
        }
    }
}

void CSamplerDevice::TuneTest(float* BufferL,float* BufferR,int Samples)
{
    CWaveGenerator* WG=&CurrentRange()->WG;
    int SmpRate=CPresets::Presets.SampleRate;
    if (!Looping)
    {
        Looping=true;
        int TempTune=WG->LP.Tune;
        WG->LP.Tune=0;
        WG->Reset();
        WG->LP.Tune=TempTune;
        WavePos=0;
    }

    float PlayFreq=pow(2.0,(float)WG->LP.Tune*0.001)*440.0;

    if (WG->Channels==1)
    {
        float* B=WG->GetNext(PlayFreq);
        if (B)
        {
            for (int i=0;i<Samples;i++)
            {
                WavePos=WavePos+440;
                while (WavePos>=SmpRate)
                {
                    WavePos-=SmpRate;
                }

                float Signal=B[i];
                Signal=(B[i]+(WB.GetNext(WavePos,CWaveBank::Sawtooth)*0.1))*0.5;
                BufferL[i]+=Signal;
                BufferR[i]+=Signal;
            }
        }
    }
    else
    {
        float* BL=WG->GetNext(PlayFreq);
        if (BL)
        {
            float* BR=BL+Samples;
            for (int i=0;i<Samples;i++)
            {
                WavePos=WavePos+440;
                while (WavePos>=SmpRate)
                {
                    WavePos-=SmpRate;
                }
                float SawWave=WB.GetNext(WavePos,CWaveBank::Sawtooth)*0.1;
                BufferL[i]+=(SawWave+BL[i])*0.5;
                BufferR[i]+=(SawWave+BR[i])*0.5;
            }
        }
    }
}

const short CSamplerDevice::voiceChannel(const int voice)
{
    return SamplerGenerator[voice].Channel;
}

const int CSamplerDevice::voiceCount()
{
    return Sampler::samplervoices;
}

void CSamplerDevice::reset()
{
    TestMode=st_NoTest;
    Looping=false;
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].resetTranspose();
        SamplerGenerator[i].ID=0;
        SamplerGenerator[i].Channel=0;
    }
    for (int i = 0; i < 16; i++)
    {
        channelSettings[i].reset();
    }
}

CSampleKeyRange::RangeParams CSamplerDevice::RangeParams(int Layer, int Range)
{
    return SamplerGenerator[0].RangeParams(Layer,Range);
}

CSampleKeyRange::RangeParams CSamplerDevice::RangeParams()
{
    return RangeParams(CurrentLayerIndex,CurrentRangeIndex);
}

void CSamplerDevice::setRangeParams(CSampleKeyRange::RangeParams RangeParams, int Layer, int Range)
{
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].setRangeParams(RangeParams,Layer,Range);
    }
}

void CSamplerDevice::setRangeParams(CSampleKeyRange::RangeParams RangeParams)
{
    setRangeParams(RangeParams,CurrentLayerIndex,CurrentRangeIndex);
}

CWaveGenerator::LoopParameters CSamplerDevice::LoopParams(int Layer, int Range)
{
    return SamplerGenerator[0].LoopParams(Layer,Range);
}

CWaveGenerator::LoopParameters CSamplerDevice::LoopParams()
{
    return LoopParams(CurrentLayerIndex,CurrentRangeIndex);
}

void CSamplerDevice::setLoopParams(CWaveGenerator::LoopParameters LoopParams, int Layer, int Range)
{
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].setLoopParams(LoopParams,Layer,Range);
    }
}

void CSamplerDevice::setLoopParams(CWaveGenerator::LoopParameters LoopParams)
{
    setLoopParams(LoopParams,CurrentLayerIndex,CurrentRangeIndex);
}

CLayer::LayerParams CSamplerDevice::LayerParams(int Layer)
{
    return SamplerGenerator[0].LayerParams(Layer);
}

CLayer::LayerParams CSamplerDevice::LayerParams()
{
    return LayerParams(CurrentLayerIndex);
}

void CSamplerDevice::setLayerParams(CLayer::LayerParams LayerParams, int Layer)
{
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].setLayerParams(LayerParams,Layer);
    }
}

void CSamplerDevice::setLayerParams(CLayer::LayerParams LayerParams)
{
    setLayerParams(LayerParams,CurrentLayerIndex);
}

CADSR::ADSRParams CSamplerDevice::ADSRParams()
{
    return SamplerGenerator[0].ADSRParameters();
}

void CSamplerDevice::setADSRParams(CADSR::ADSRParams ADSRParams)
{
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].setADSRParams(ADSRParams);
    }
}

void CSamplerDevice::setTune(float tune)
{
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].Tune=tune;
    }
}

void CSamplerDevice::AddRange(int Layer, const QString &WavePath, int Upper, int Lower)
{
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].AddRange(Layer,WavePath,Upper,Lower);
    }
}

void CSamplerDevice::AddRange(const QString &WavePath, int Upper, int Lower)
{
    AddRange(CurrentLayerIndex,WavePath,Upper,Lower);
}

void CSamplerDevice::ChangePath(int Layer, int Range, const QString &WavePath)
{
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].ChangePath(Layer,Range,WavePath);
    }
}

void CSamplerDevice::ChangePath(const QString &WavePath)
{
    ChangePath(CurrentLayerIndex,CurrentRangeIndex,WavePath);
}

void CSamplerDevice::RemoveRange(int Layer, int Index)
{
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].RemoveRange(Layer,Index);
    }
}

void CSamplerDevice::RemoveRange(int Index)
{
    RemoveRange(CurrentLayerIndex,Index);
}

void CSamplerDevice::RemoveRange()
{
    RemoveRange(CurrentLayerIndex,CurrentRangeIndex);
}

void CSamplerDevice::AddLayer(int Upper, int Lower)
{
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].AddLayer(Upper,Lower);
    }
}

void CSamplerDevice::RemoveLayer(int index)
{
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].RemoveLayer(index);
    }
}

void CSamplerDevice::RemoveLayer()
{
    RemoveLayer(CurrentLayerIndex);
}

int CSamplerDevice::RangeCount(int Layer)
{
    return SamplerGenerator[0].RangeCount(Layer);
}

int CSamplerDevice::RangeCount()
{
    return RangeCount(CurrentLayerIndex);
}

int CSamplerDevice::LayerCount()
{
    return SamplerGenerator[0].LayerCount();
}

CLayer* CSamplerDevice::Layer(int Index)
{
    return SamplerGenerator[0].Layer(Index);
}

CSampleKeyRange* CSamplerDevice::Range(int Layer, int Index)
{
    return this->Layer(Layer)->Range(Index);
}

CLayer* CSamplerDevice::CurrentLayer()
{
    return Layer(CurrentLayerIndex);
}

CSampleKeyRange* CSamplerDevice::CurrentRange()
{
    return Range(CurrentLayerIndex,CurrentRangeIndex);
}

const QString CSamplerDevice::Save()
{
    return SamplerGenerator[0].Save();
}

void CSamplerDevice::Load(const QString &XML)
{
    TestMode=st_NoTest;
    Looping=false;
    for (int i=0; i < Sampler::samplervoices; i++)
    {
        SamplerGenerator[i].Load(XML);
    }
}
