#ifndef CSAMPLERDEVICE_H
#define CSAMPLERDEVICE_H

#include "csamplergenerator.h"
#include "csounddevice.h"
#include "cwavebank.h"

namespace Sampler
{
const int samplervoices=16;
}

class CSamplerDevice : public ISoundDevice
{
public:
    enum SamplerTestModes
    {
        st_NoTest,
        st_LoopTest,
        st_TuneTest
    };
    CSamplerDevice();
    void NoteOn(const short channel, const short pitch, const short velocity);
    void NoteOff(const short channel, const short pitch);
    void Aftertouch(const short channel, const short pitch, const short value);
    float* getNext(const int voice, const float modulation=1);
    void TuneTest(float* BufferL, float* BufferR, int Samples);
    void LoopTest(float* BufferL, float* BufferR, int Samples);
    const short voiceChannel(const int voice);
    const int voiceCount();
    void reset();
    void allNotesOff();
    void setTune(float tune);
    void AddRange(int Layer,const QString& WavePath=QString(),int Upper=127,int Lower=0);
    void AddRange(const QString& WavePath=QString(),int Upper=127,int Lower=0);
    void ChangePath(int Layer,int Range,const QString& WavePath);
    void ChangePath(const QString& WavePath);
    void RemoveRange(int Layer,int Index);
    void RemoveRange(int Index);
    void RemoveRange();
    void AddLayer(int Upper=127, int Lower=0);
    void RemoveLayer(int index);
    void RemoveLayer();
    int LayerCount();
    int RangeCount(int Layer);
    int RangeCount();
    CSampleKeyRange::RangeParams RangeParams(int Layer, int Range);
    CSampleKeyRange::RangeParams RangeParams();
    void setRangeParams(CSampleKeyRange::RangeParams RangeParams,int Layer, int Range);
    void setRangeParams(CSampleKeyRange::RangeParams);
    CWaveGenerator::LoopParameters LoopParams(int Layer, int Range);
    CWaveGenerator::LoopParameters LoopParams();
    void setLoopParams(CWaveGenerator::LoopParameters LoopParams, int Layer, int Range);
    void setLoopParams(CWaveGenerator::LoopParameters LoopParams);
    CLayer::LayerParams LayerParams(int Layer);
    CLayer::LayerParams LayerParams();
    void setLayerParams(CLayer::LayerParams LayerParams,int Layer);
    void setLayerParams(CLayer::LayerParams LayerParams);
    CADSR::ADSRParams ADSRParams();
    void setADSRParams(CADSR::ADSRParams ADSRParams);
    const QString Save();
    void Load(const QString& XML);
    CLayer* Layer(int Index);
    CSampleKeyRange* Range(int Layer, int Index);
    CLayer* CurrentLayer();
    CSampleKeyRange* CurrentRange();
    int CurrentLayerIndex;
    int CurrentRangeIndex;
    SamplerTestModes TestMode;
    bool Looping;
    CWaveBank WB;
    float WavePos;
private:
    CSamplerGenerator SamplerGenerator[Sampler::samplervoices];
};

#endif // CSAMPLERDEVICE_H
