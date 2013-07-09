#ifndef CDRUMMACHINE_H
#define CDRUMMACHINE_H

#include "softsynthsclasses.h"
#include "cwavegenerator.h"
#include "sequenserclasses.h"

namespace DrumMachine
{
const int SoundCount=7;
}

class CDrumMachine : public IDevice
{
private:
    enum JackNames
    {jnOut};
    enum ParameterNames
    {pnTempo,pnVolume};
    struct SoundType
    {
        CWaveGenerator* Generator;
        QString Name;
        float Volume;
    };
    float VolumeFactor;
    void inline CalcParams();
    int Counter;
    int BeatInterval;
    int BeatCount;
    int PatternIndex;
    int PatternRepeatCount;
    int CurrentPitch;
    int CurrentLength;
    int CurrentVolume;
    int PatternLength;
    int NextBeat;
    int NextStop;
    //float MixFactor;
    float SamplesPerTick;
    float SampleCount;
    bool Playing;
    void Reset();
    void inline AddSound(const QString& Path,const QString& Name,CWaveGenerator* WG, SoundType* ST);
    CWaveGenerator WG[DrumMachine::SoundCount];
public:
    void Init(const int Index,void* MaionWindow);
    float* GetNextA(const int ProcIndex);
    void Tick();
    SoundType ST[DrumMachine::SoundCount];
    QList<PatternType*> Patterns;
    QList<PatternListType*> PatternsInList;
    //TList* Sounds;
    void Play(const bool FromStart);
    void Pause();
};

#endif // CDRUMMACHINE_H
