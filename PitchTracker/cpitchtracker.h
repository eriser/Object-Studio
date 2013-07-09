#ifndef CPITCHTRACKER_H
#define CPITCHTRACKER_H

#include "softsynthsclasses.h"
#include "cpitchtrackerclass.h"

#define BufferCount 2

class CPitchTracker : public IDevice
{
private:
    enum JackNames
    {jnIn,jnFrequencyOut,jnMIDIFreqOut,jnMIDIOut,jnDiffOut};
    enum ParameterNames
    {pnThreshold,pnTune,pnPriority};
    int BufferFill;
    int LastNote;
    int BufferDivide;
    int NewBufferDivide;
    CMIDIBuffer MIDIBuffer;
    CPitchTrackerClass* PT;
    void inline CalcParams();
public:
    CPitchTracker();
    ~CPitchTracker();
    void Init(const int Index,void* MainWindow);
    const float GetNext(const int ProcIndex);
    void Process(float* Input);
    void* GetNextP(int /*ProcIndex*/);
};

#endif // CPITCHTRACKER_H
