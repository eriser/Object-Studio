#ifndef CCVDEVICE_H
#define CCVDEVICE_H

#include "softsynthsclasses.h"
#include "csounddevice.h"

namespace CVDevice
{
const int CVVoices=8;
}

class CCVDevice : public ISoundDevice
{
public:
    struct CVNote
    {
        float Frequency;
        float Velocity;
        int MIDIPitch;
        int Channel;
    };
    CCVDevice();
    ~CCVDevice();
    void NoteOn(const short channel, const short pitch, const short velocity);
    void NoteOff(const short channel, const short pitch);
    const int voiceCount();
    void reset();
    void allNotesOff();
    float Tune;
    CVNote* Notes;
    const short voiceChannel(const int voice);
    const float getPitchbend(const int voice);
    const float Vol(const int voice);
};


#endif // CCVDEVICE_H
