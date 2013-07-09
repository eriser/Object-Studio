#include "ccvdevice.h"

CCVDevice::CCVDevice()
{
    Notes=new CVNote[CVDevice::CVVoices];
    reset();
    Tune=440;
}

CCVDevice::~CCVDevice()
{
    delete [] Notes;
}

void CCVDevice::NoteOn(const short channel, const short pitch, const short velocity)
{
    if (channelSettings[channel].portNote)
    {
        for (int i=0;i<CVDevice::CVVoices;i++)
        {
            if ((channel) == Notes[i].Channel)
            {
                if (Notes[i].MIDIPitch==channelSettings[channel].portNote)
                {
                    Notes[i].MIDIPitch=pitch;
                    Notes[i].Frequency*=Cent_to_Percent((pitch-channelSettings[channel].portNote)*100);
                    break;
                }
            }
        }
    }
    else
    {
        int FreeIndex=-1;
        for (int i=0;i<CVDevice::CVVoices;i++)
        {
            if (Notes[i].Velocity==0)
            {
                FreeIndex=i;
                break;
            }
        }
        if (FreeIndex>-1)
        {
            Notes[FreeIndex].Channel=channel;
            Notes[FreeIndex].MIDIPitch=pitch;
            Notes[FreeIndex].Frequency=MIDItoFreq(pitch+m_Transpose,Tune)*FreqResolution;
            Notes[FreeIndex].Velocity=(float)(velocity & 0x7F) / (float)0x7F;
        }
    }
    channelSettings[channel].portNote=0;
}

void CCVDevice::NoteOff(const short channel, const short pitch)
{
    for (int i=0;i<CVDevice::CVVoices;i++)
    {
        if (channel==Notes[i].Channel)
        {
            if (channelSettings[channel].pedal)
            {
                channelSettings[channel].pedalnotes.append(pitch);
            }
            else if (pitch==Notes[i].MIDIPitch)
            {
                Notes[i].Frequency=0;
                Notes[i].Velocity=0;
                break;
            }
        }
    }
}

const int CCVDevice::voiceCount()
{
    return CVDevice::CVVoices;
}

void CCVDevice::reset()
{
    ZeroMemory(Notes,CVDevice::CVVoices*sizeof(CVNote));
    for (int i = 0; i < 16; i++)
    {
        channelSettings[i].reset();
    }
}

void CCVDevice::allNotesOff()
{
    for (int i=0;i<CVDevice::CVVoices;i++)
    {
        Notes[i].MIDIPitch=0;
        Notes[i].Velocity=0;
    }
}

const short CCVDevice::voiceChannel(const int voice)
{
    return Notes[voice].Channel;
}

const float CCVDevice::getPitchbend(const int voice)
{
    return Cent_to_Percent(channelSettings[voiceChannel(voice)].pitchWheel);
}

const float CCVDevice::Vol(const int voice)
{
    int ch=voiceChannel(voice);
    return channelSettings[ch].volume*channelSettings[ch].expression;
}
