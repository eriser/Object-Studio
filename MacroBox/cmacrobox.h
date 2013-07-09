#ifndef CMACROBOX_H
#define CMACROBOX_H

#include "softsynthsclasses.h"

class CMacroBox : public IDevice
{
public:
    CMacroBox();
    ~CMacroBox();
    void Play(const bool FromStart);
    void Pause();
    void Init(const int Index,void* MainWindow);
    const float GetNext(const int ProcIndex);
    void* GetNextP(const int ProcIndex);
    float* GetNextA(const int ProcIndex);
    void Tick();
    void HideForm();
private:
    enum JackNames
    {jnMIDIIn,jnOut};
    enum ParameterNames
    {pnMIDIChannel,pnVolume};
    CFastPointerList InsideJacks;
    std::vector<IJack*> JacksCreated;
};

#endif // CMACROBOX_H
