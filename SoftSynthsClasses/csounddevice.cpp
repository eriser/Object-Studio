#include "csounddevice.h"

float channelData::panL(float pan)
{
    if (pan<=1) return 1;
    return 1-(pan-1);
}

float channelData::panR(float pan)
{
    if (pan>=1) return 1;
    return pan;
}

channelData::channelData()
{
    reset();
}

void channelData::reset()
{
    portNote=0;
    patch=0;
    expression=1;
    volume=1;
    pan=1;
    balance=1;
    pitchWheel=0;
    pressure=1;
    bank=0;
    pedal=false;
    pedalnotes.clear();
}

float channelData::volL()
{
    return expression*volume*panL(pan)*panL(balance)*pressure;
}

float channelData::volR()
{
    return expression*volume*panR(pan)*panR(balance)*pressure;
}

ISoundDevice::ISoundDevice()
{
    m_Transpose=0;
    m_Channel=0;
    isGM=false;
}

void ISoundDevice::NoteOn(short /*channel*/,short /*pitch*/,short /*velocity*/)
{
}

void ISoundDevice::NoteOff(short /*channel*/,short /*pitch*/)
{
}

void ISoundDevice::allNotesOff()
{}

void ISoundDevice::Aftertouch(short /*channel*/,short /*pitch*/,short /*value*/)
{
}

float* ISoundDevice::getNext(const int /*voice*/)
{
    return NULL;
}

const short ISoundDevice::voiceChannel(const int /*voice*/)
{
    return 0;
}

const int ISoundDevice::voiceCount()
{
    return 0;
}

void ISoundDevice::Patch(const short channel, const short value)
{
    channelSettings[channel].patch=value;
}

void ISoundDevice::Controller(short channel, short controller, short value)
{
    switch (controller)
    {
    case 0: // Bank select
        channelSettings[channel].bank=value;
        break;
    case 32: // Bank select
        //channel[Message & 0xF].bank=Data2;
        //qDebug() << "Bank select" << value;
        break;
    case 84: // portamento note
        channelSettings[channel].portNote=value;
        break;
    case 7: // volume
        channelSettings[channel].volume=(float)value/127.0;
        //qDebug() << "volume" << channelSettings[channel].volume;
        break;
    case 8: // balance
        channelSettings[channel].balance=(float)value/64.0;
        //qDebug() << "balance" << channelSettings[channel].balance;
        break;
    case 10: // pan
        channelSettings[channel].pan=(float)value/64.0;
        //qDebug() << "pan" << channelSettings[channel].pan;
        break;
    case 11: // expression
        channelSettings[channel].expression=(float)value/127.0;
        //qDebug() << "expression" << channelSettings[channel].expression;
        break;
    case 64: // pedal
    case 66:
    case 89:
        channelSettings[channel].pedal=(value > 63);
        if (!channelSettings[channel].pedal)
        {
            foreach (short pitch,channelSettings[channel].pedalnotes)
            {
                NoteOff(channel,pitch);
            }
            channelSettings[channel].pedalnotes.clear();
        }
        break;
    case 121: // All controllers off
        channelSettings[channel].reset();
        break;
    default:
        break;
        //qDebug() << "Controller" << controller << value;
    }
}

void ISoundDevice::PitchBend(short channel, short value)
{
    channelSettings[channel].pitchWheel=value*pitchBendFactor;
}

void ISoundDevice::ChannelPressure(short channel, short value)
{
    channelSettings[channel].pressure=((float)value*0.001)+1;
}

void ISoundDevice::SysEx(char* data, const short datalen)
{
    if (datalen > 3)
    {
        if (data[2]==9)
        {
            isGM=data[3];
            reset();
        }
    }
}

const float ISoundDevice::volL(const short channel)
{
    return channelSettings[channel].volL();
}

const float ISoundDevice::volR(const short channel)
{
    return channelSettings[channel].volR();
}

void ISoundDevice::reset()
{
    //MessageLength=0;
}

void ISoundDevice::parseMIDI(CMIDIBuffer* MB)
{
    short Message=0;
    datalen=0;
    if (MB!=NULL)
    {
        MB->StartRead();
        int lTemp=MB->Read();
        while (lTemp > -1)
        {
            if (lTemp >= 0x80)
            {
                Message=lTemp;
                datalen=0;
            }
            forever
            {
                lTemp=MB->Read();
                if ((lTemp >= 0x80) | (lTemp < 0)) break;
                if (datalen < data.count())
                {
                    data[datalen]=lTemp;
                }
                else
                {
                    data.append(lTemp);
                }
                datalen++;
            }
            if (Message >= 0x80)
            {
                short Channel=Message & 0x0F;
                Message=Message & 0xF0;
                if ((m_Channel==0) | (Channel==m_Channel-1))
                {
                    if (datalen > 0)
                    {
                        if (Message==0x80)
                        {
                            NoteOff(Channel,data[0]);
                        }
                        else if (Message==0xC0)
                        {
                            Patch(Channel,data[0]);
                        }
                        else if (Message==0xD0)
                        {
                            ChannelPressure(Channel,data[0]);
                        }
                        else if (Message==0xF0)
                        {
                            if ((Channel==0) | (Channel==7))
                            {
                                SysEx(data.data(),datalen);
                            }
                            if (Channel==0xF)
                            {
                                QString MetaText=QString(data.left(datalen)).trimmed().simplified();
                                if (!MetaText.isEmpty()) qDebug() << "Meta" << MetaText;
                            }
                        }
                        if (datalen > 1)
                        {
                            if (Message==0x90)
                            {
                                if ((int)data[1] == 0)
                                {
                                    NoteOff(Channel,data[0]);
                                }
                                else
                                {
                                    NoteOn(Channel,data[0],data[1]);
                                }
                            }
                            else if (Message==0xA0)
                            {
                                Aftertouch(Channel,data[0],data[1]);
                            }
                            else if (Message==0xB0)
                            {
                                Controller(Channel,data[0],data[1]);
                            }
                            else if (Message==0xE0)
                            {
                                PitchBend(Channel,from14bit(data[0],data[1])-0x2000);
                            }
                        }
                    }
                }
            }
        }
    }
}

void ISoundDevice::setTranspose(const short transpose)
{
    m_Transpose=transpose;
}

void ISoundDevice::setChannel(const short channel)
{
    m_Channel=channel;
}
