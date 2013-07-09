#ifndef CMIXER_H
#define CMIXER_H

#include "softsynthsclasses.h"

namespace Mixer
{
const int mixerchannels=8;
}

class CMixer : public IDevice
{
public:
    CMixer();
    void Init(const int Index, void *MainWindow);
    void Play(const bool FromStart);

    float Level[Mixer::mixerchannels];
    float Effect[Mixer::mixerchannels];
    float PanL[Mixer::mixerchannels];
    float PanR[Mixer::mixerchannels];
    bool Mute[Mixer::mixerchannels];
    bool EffectMute[Mixer::mixerchannels];
    int SoloChannel;
    float MasterLeft;
    float MasterRight;
    void GetPeak(float* P,float* L,float* R);
private:
    enum JackNames
    {jnReturn,jnOut,jnSend,jnIn};
    float MixFactor;
    void Process();
    float Peak[Mixer::mixerchannels];
    float PeakL;
    float PeakR;
};

#endif // CMIXER_H
