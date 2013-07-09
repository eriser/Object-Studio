#ifndef CRANGE_H
#define CRANGE_H

#include "cwavegenerator.h"

class CSampleKeyRange
{
public:
    struct RangeParams
    {
        int UpperZero;
        int UpperTop;
        int LowerZero;
        int LowerTop;
        int Volume;
    };
    CWaveGenerator WG;
    QString FileName;
    float GetVolume(int MIDIKey);
    float PlayVol;
    RangeParams RP;
    CSampleKeyRange(const QString& WavePath=QString(),int Upper=127,int Lower=0);
    CSampleKeyRange(const QString& WavePath,CSampleKeyRange::RangeParams RangeParams);
    ~CSampleKeyRange();
    void ChangePath(const QString& WavePath);
    void PitchDetect(int Tune);
    void AutoLoop(int Cycles);
    void AutoTune();
    void AutoFix(int Cycles, int Tune);
private:
};

#endif // CRANGE_H
