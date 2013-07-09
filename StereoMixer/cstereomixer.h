#ifndef STEREOMIXER_H
#define STEREOMIXER_H

#include "softsynthsclasses.h"

class CStereoMixerChannel
{
public:
    CStereoMixerChannel(int sends=3);
    ~CStereoMixerChannel();
    void MixChannel(float* Signal, CStereoBuffer* Out, CStereoBuffer** Send, const int BufferSize, const bool First, CStereoBuffer* WorkBuffer);
    float Level;
    float PanL;
    float PanR;
    bool Mute;
    bool EffectMute;
    float* Effect;
    float PeakL;
    float PeakR;
    int sendCount;
};

class CStereoMixer : public IDevice
{
public:
    enum JackNames
    {jnReturn,jnOut,jnSend};
    CStereoMixer(int channels=12, int sends=3);
    ~CStereoMixer();
    void Init(const int Index, void *MainWindow);
    float* GetNextA(const int ProcIndex);
    void Play(const bool FromStart);

    CStereoMixerChannel** channels;
    int SoloChannel;
    float MasterLeft;
    float MasterRight;
    float PeakL;
    float PeakR;
    float* Sends;
    int sendCount;
    int channelCount;
    bool Disabled;
private:
    float MixFactor;
    void Process();
    CStereoBuffer WorkBuffer;
};

#endif // STEREOMIXER_H
