#include "csf2device.h"

CSF2Device::CSF2Device()
{
    lastChannel=0;
    drumBank=0;
    loaded=false;
    patchResponse=false;
    isGM=true;
}

CSF2Device::~CSF2Device()
{
    foreach (SF2Bank bank,banks)
    {
        bank.presets.clear();
    }
    banks.clear();
}

void CSF2Device::NoteOn(short channel, short pitch, short velocity)
{
    if (channelSettings[channel].portNote)
    {
        for (int i=0;i<SF2Device::sf2voices;i++)
        {
            if ((channel) == SF2Generator[i].Channel)
            {
                if (SF2Generator[i].ID==channelSettings[channel].portNote)
                {
                    SF2Generator[i].ID=pitch;
                    SF2Generator[i].addTranspose(pitch-channelSettings[channel].portNote);
                    break;
                }
            }
        }
    }
    else
    {
        int FreeIndex=-1;
        for (int i=0;i<SF2Device::sf2voices;i++)
        {
            if (SF2Generator[i].ID==0)
            {
                FreeIndex=i;
                break;
            }
        }
        if (FreeIndex>-1)
        {
            SF2Generator[FreeIndex].ID=pitch;
            SF2Generator[FreeIndex].Channel=channel;
            SF2Generator[FreeIndex].resetTranspose();
            SF2Generator[FreeIndex].MidiBank=channelSettings[channel].bank;
            SF2Generator[FreeIndex].MidiPreset=channelSettings[channel].patch;
            SF2Generator[FreeIndex].ResetSample(pitch+m_Transpose,velocity & 0x7F);
        }
    }
    channelSettings[channel].portNote=0;
}

void CSF2Device::NoteOff(short channel, short pitch)
{
    for (int i=0;i<SF2Device::sf2voices;i++)
    {
        if ((channel) == SF2Generator[i].Channel)
        {
            if (channelSettings[channel].pedal)
            {
                channelSettings[channel].pedalnotes.append(pitch);
            }
            else if (pitch==SF2Generator[i].ID)
            {
                SF2Generator[i].EndSample();
                break;
            }
        }
    }
}

void CSF2Device::Aftertouch(const short channel, const short pitch, const short value)
{
    for (int i=0;i<SF2Device::sf2voices;i++)
    {
        if ((channel) == SF2Generator[i].Channel)
        {
            if (pitch==SF2Generator[i].ID)
            {
                SF2Generator[i].setAftertouch(value);
            }
        }
    }
}

void CSF2Device::Patch(const short channel, const short value)
{
    if (patchResponse)
    {
        lastChannel=channel;
        channelSettings[channel].patch=value;
    }
}

void CSF2Device::Controller(short channel, short controller, short value)
{
    short bank;
    short patch;
    switch (controller)
    {
    case 0: // Bank select
        if (patchResponse)
        {
            if ((channel != 9) | (!isGM))
            {
                if (banks.contains(value))
                {
                    channelSettings[channel].bank=value;
                }
            }
        }
        break;
    case 121: // All controllers off
        bank=channelSettings[channel].bank;
        patch=channelSettings[channel].patch;
        channelSettings[channel].reset();
        if (patchResponse | isGM)
        {
            channelSettings[9].bank=drumBank;
        }
        else
        {
            channelSettings[channel].bank=bank;
            channelSettings[channel].patch=patch;
        }
        break;
    default:
        ISoundDevice::Controller(channel,controller,value);
        break;
    }
}

void CSF2Device::allNotesOff()
{
    for (int i=0;i<SF2Device::sf2voices;i++)
    {
        if (!SF2Generator[i].FinishedPlaying)
        {
            SF2Generator[i].EndSample();
        }
    }
}

float* CSF2Device::getNext(const int voice)
{
    SF2Generator[voice].setPitchWheel(channelSettings[voiceChannel(voice)].pitchWheel);
    return SF2Generator[voice].GetNext();
}

const short CSF2Device::voiceChannel(const int voice)
{
    return SF2Generator[voice].Channel;
}

const int CSF2Device::voiceCount()
{
    return SF2Device::sf2voices;
}

void CSF2Device::reset()
{
    //MessageLength=0;
    drumBank=0;
    if (loaded)
    {
        for (int i=0;i<SF2Generator[0].PresetCount();i++)
        {
            if (SF2Generator[0].Preset(i)->wPresetBank>drumBank)
            {
                drumBank=SF2Generator[0].Preset(i)->wPresetBank;
            }
        }
    }
    for (int i=0; i < SF2Device::sf2voices; i++)
    {
        SF2Generator[i].resetTranspose();
        SF2Generator[i].ID=0;
        SF2Generator[i].Channel=0;
    }
    for (int i = 0; i < 16; i++)
    {
        channelSettings[i].reset();
    }
    if (isGM) channelSettings[9].bank=drumBank;
}

const int CSF2Device::presetcount()
{
    return SF2Generator[0].PresetCount();
}

const QString CSF2Device::presetname(const int preset)
{
    return SF2Generator[0].Preset(preset)->achPresetName;
}

const int CSF2Device::banknumber(const int preset)
{
    return SF2Generator[0].Preset(preset)->wPresetBank;
}

const int CSF2Device::presetnumber(const int preset)
{
    return SF2Generator[0].Preset(preset)->wPresetNum;
}

const int CSF2Device::currentBank(const short channel)
{
    short ch=channel;
    if (ch<0) ch=lastChannel;
    return channelSettings[ch].bank;
}

const int CSF2Device::currentPreset(const short channel)
{
    short ch=channel;
    if (ch<0) ch=lastChannel;
    return channelSettings[ch].patch;
}

void CSF2Device::setBank(const int bank)
{
    if (patchResponse) return;
    for (int i=0;i<16;i++)
    {
        channelSettings[i].bank=bank;
    }
}

void CSF2Device::setPreset(const int preset)
{
    if (patchResponse) return;
    for (int i=0;i<16;i++)
    {
        channelSettings[i].patch=preset;
    }
}

const bool CSF2Device::loadFile(const QString& filename)
{
    allNotesOff();
    loaded=false;
    CSF2Generator* sf=&SF2Generator[0];
    if (sf->LoadFile(filename))
    {
        for (int i=1;i<SF2Device::sf2voices;i++)
        {
            SF2Generator[i].LoadFile(filename);
        }
        banks.clear();
        for (int i=0;i<sf->PresetCount();i++)
        {
            SF2Preset p;
            p.name=sf->Preset(i)->achPresetName;
            p.number=i;
            if (banks.keys().indexOf(sf->Preset(i)->wPresetBank)<0)
            {
                SF2Bank b;
                b.presets[sf->Preset(i)->wPresetNum]=p;
                banks[sf->Preset(i)->wPresetBank]=b;
            }
            else
            {
                banks[sf->Preset(i)->wPresetBank].presets[sf->Preset(i)->wPresetNum]=p;
            }
        }
        loaded=true;
    }
    return loaded;
}
