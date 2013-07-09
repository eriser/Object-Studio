#ifndef CLAYERCLASS_H
#define CLAYERCLASS_H

#include "crange.h"

class CLayer
{
public:
    struct LayerParams
    {
        int UpperTop;
        int LowerTop;
        int UpperZero;
        int LowerZero;
        int Volume;
        int Transpose;
        int Tune;
    };
    LayerParams LP;
    float PlayVol;
    CLayer(int Upper=127,int Lower=0);
    CLayer(LayerParams LayerParams);
    ~CLayer();
    CSampleKeyRange* AddRange(const QString& WavePath=QString(),int Upper=127,int Lower=0);
    void ChangePath(int Range, const QString &WavePath);
    void RemoveRange(CSampleKeyRange* KR);
    void ModifyBuffer(float* BufferL,float* BufferR,int Samples, float Frequency,float Velocity,float ModulationTune,float MixFactor);
    float GetVolume(int Velocity);
    const QString Save();
    void Load(const QString& XML);
    void ResetSample(short MidiNote);
    void EndSample(void);
    CSampleKeyRange* Range(int Index);
    int RangeCount();
    CSampleKeyRange::RangeParams RangeParams(int Range);
    void setRangeParams(CSampleKeyRange::RangeParams RangeParams,int Range);
    CWaveGenerator::LoopParameters LoopParams(int Range);
    void setLoopParams(CWaveGenerator::LoopParameters LoopParams,int Range);
private:	// User declarations
    QList<CSampleKeyRange*> Ranges;
    QList<CSampleKeyRange*> ActiveRanges;
    float CurrentFrequency;
};

#endif // CLAYERCLASS_H
