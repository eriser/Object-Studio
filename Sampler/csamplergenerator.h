#ifndef CSAMPLERGENERATOR_H
#define CSAMPLERGENERATOR_H

#include "cadsr.h"
#include "clayer.h"

class CSamplerGenerator
{
public:
    CSamplerGenerator();
    ~CSamplerGenerator();
    float* GetNext(float tune);
    void setPitchWheel(int cent);
    void addTranspose(int steps);
    void setAftertouch(short value);
    void resetTranspose();
    void ResetSample(short MidiNote,short MidiVelo);
    void EndSample(void);
    const QString Save();
    void Load(const QString& XML);
    short ID;
    short Channel;
    bool FinishedPlaying;
    float Tune;
    void AddRange(int Layer,const QString& WavePath=QString(),int Upper=127,int Lower=0);
    void ChangePath(int Layer,int Range,const QString& WavePath);
    void RemoveRange(int Layer,int Index);
    void AddLayer(int Upper=127, int Lower=0);
    void RemoveLayer(int index);
    CLayer* Layer(int Index);
    int LayerCount();
    int RangeCount(int Layer);
    CSampleKeyRange::RangeParams RangeParams(int Layer, int Range);
    void setRangeParams(CSampleKeyRange::RangeParams RangeParams,int Layer, int Range);
    CWaveGenerator::LoopParameters LoopParams(int Layer, int Range);
    void setLoopParams(CWaveGenerator::LoopParameters LoopParams, int Layer, int Range);
    CLayer::LayerParams LayerParams(int Layer);
    void setLayerParams(CLayer::LayerParams LayerParams,int Layer);
    CADSR::ADSRParams ADSRParameters();
    void setADSRParams(CADSR::ADSRParams ADSRParams);
private:
    QList<CLayer*> Layers;
    QList<CLayer*> ActiveLayers;
    CADSR ADSR;
    int pitchWheel;
    int transpose;
    float CurrentFrequency;
    float CurrentVelocity;
    float* AudioL;
    float* AudioR;
    int m_ModulationRate;
};

#endif // CSAMPLERGENERATOR_H
