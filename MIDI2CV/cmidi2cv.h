#ifndef CMIDI2CV_H
#define CMIDI2CV_H

#include "softsynthsclasses.h"
#include "ccvdevice.h"

class CMIDI2CV : public IDevice
{
private:
    enum JackNames
    {jnFrequency,jnVelocity=8,jnIn=16};
    enum ParameterNames
    {pnMIDIChannel,pnTranspose,pnTune};
    void inline CalcParams();
    CCVDevice CVDevice;
public:
    CMIDI2CV();
    void Init(const int Index,void* MainWindow);
    const float GetNext(const int ProcIndex);
    void Play(const bool FromStart);
    void Pause();
};

#endif // CMIDI2CV_H
