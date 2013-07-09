#ifndef CSAMPLER_H
#define CSAMPLER_H

#include "softsynthsclasses.h"
#include "csamplerdevice.h"

class CSampler : public IDevice
{
private:
    enum JackNames
    {jnMIDIIn,jnModulation,jnOut};
    enum ParameterNames
    {pnMIDIChannel,pnTranspose,pnTune,pnModulation};
    void inline CalcParams();
    CSamplerDevice SamplerDevice;
    int LastMod;
    float CurrentMod;
    float VolumeFactor;
    void Process();
public:
    CSampler();
    void Play(const bool FromStart);
    void Pause();
    void Init(const int Index,void* MainWindow);
};

#endif // CSAMPLER_H
