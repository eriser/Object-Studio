#ifndef CSEQUENSER_H
#define CSEQUENSER_H

#include "softsynthsclasses.h"
#include "sequenserclasses.h"

class CSequenser : public IDevice
{
public:
    CSequenser();
    QList<PatternType*> Patterns;
    QList<PatternListType*> PatternsInList;
    void Play(const bool FromStart);
    void Pause();
    void Init(const int Index,void* MainWindow);
    void* GetNextP(const int ProcIndex);
    void Tick();
private:
    enum JackNames
    {jnMIDIOut};
    enum ParameterNames
    {pnTempo,pnMIDIChannel};
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
    float SamplesPerTick;
    float SampleCount;
    bool Playing;
    CMIDIBuffer* MIDIBuffer;
    void Reset();
    void inline CalcParams();
};

#endif // CSEQUENSER_H
