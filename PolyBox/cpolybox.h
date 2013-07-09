#ifndef CPOLYBOX_H
#define CPOLYBOX_H

#include "softsynthsclasses.h"
#include "ccvdevice.h"

class CPolyBox : public IDevice
{
public:
    CPolyBox();
    ~CPolyBox();
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
    {pnMIDIChannel,pnTranspose,pnTune};
    void Reset();
    void inline CalcParams();
    std::vector<IJack*> JacksCreated;
    CInJack* WaveOut;
    CCVDevice CVDevice;
};

#endif // CPOLYBOX_H
