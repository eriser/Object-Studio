#ifndef CMIDIFILEPLAYER_H
#define CMIDIFILEPLAYER_H

#include "softsynthsclasses.h"
#include "cmidifilereader.h"

namespace MIDIFilePlayer
{
const QString MIDIFilter("MIDI files (*.mid;*.kar)");
}

class CMIDIFilePlayer : public IDevice
{
public:
    CMIDIFilePlayer();
    ~CMIDIFilePlayer()
    {
    }

    unsigned long CurrentTick;
    unsigned long CurrentMilliSecond;

    void Init(const int Index,void* MainWindow);
    void Tick();
    void Skip(const unsigned long mSec);
    void* GetNextP(const int ProcIndex);
    void Play(const bool FromStart);
    void Pause();
    void Execute(const bool Show);
    const QString Save();
    void Load(const QString& XML);
    const bool IsPlaying();
    const unsigned long Duration();
    const unsigned long MilliSeconds();
    void OpenPtr(const char* Pnt, const int Length);
private:
    enum JackNames
    {jnMIDI};
    enum ParameterNames
    {pnTrack};
    int Ticks;
    int uSPQ;
    float SamplesPerTick;
    double SampleCount;
    float SamplesPermSec;
    float mSecCount;
    bool Playing;
    bool SkipBuffer;
    CMIDIBuffer MIDIBuffer;
    CMIDIFileReader MFR;
    float uSPerTick;
    QList<CMIDIFileTrack*> PlayingTracks;

    void OpenMidiFile(const QString& fn);
    void inline CalcParams();
    void inline Reset();
    void inline CalcTempo();
};

#endif // CMIDIFILEPLAYER_H
