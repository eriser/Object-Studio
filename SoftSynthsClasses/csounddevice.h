#ifndef CSOUNDDEVICE_H
#define CSOUNDDEVICE_H

#include "softsynthsclasses.h"

class channelData
{
private:
    float inline panL(float pan);
    float inline panR(float pan);
public:
    channelData();
    void reset();
    float volL();
    float volR();
    short portNote;
    short patch;
    float expression;
    float volume;
    float pan;
    float balance;
    float pitchWheel;
    float pressure;
    short bank;
    bool pedal;
    QList<short> pedalnotes;
};

class ISoundDevice
{
public:
    ISoundDevice();
    virtual void NoteOn(const short channel, const short pitch, const short velocity);
    virtual void NoteOff(const short channel, const short pitch);
    virtual void Aftertouch(short channel,short pitch,short value);
    virtual void Controller(const short channel, const short controller, const short value);
    virtual void PitchBend(const short channel, const short value);
    virtual void ChannelPressure(const short channel, const short value);
    virtual void Patch(const short channel, const short value);
    virtual void SysEx(char* data, const short datalen);
    virtual float* getNext(const int voice);
    virtual const short voiceChannel(const int voice);
    virtual const int voiceCount();
    virtual const float volL(const short channel);
    virtual const float volR(const short channel);
    virtual void reset();
    virtual void allNotesOff();
    virtual void parseMIDI(CMIDIBuffer* MB);
    void setTranspose(const short transpose);
    void setChannel(const short channel);
protected:
    channelData channelSettings[16];
    short m_Channel;
    short m_Transpose;
    bool isGM;
private:
    QByteArray data;
    int datalen;
};


#endif // CSOUNDDEVICE_H
