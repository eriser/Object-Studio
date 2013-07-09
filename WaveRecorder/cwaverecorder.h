#ifndef CWAVERECORDER_H
#define CWAVERECORDER_H

#include "softsynthsclasses.h"

class CWaveRecorder : public IDevice
{
private:
    enum JackNames
    {jnOut,jnIn};
    enum ParameterNames
    {};
    bool Playing;
public:
    CWaveRecorder();
    void Play(const bool FromStart);
    void Pause();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
    void Tick();
};

#endif // CWAVERECORDER_H
