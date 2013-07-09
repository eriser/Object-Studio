#ifndef CPANNER_H
#define CPANNER_H

#include "softsynthsclasses.h"

class CPanner : public IDevice
{
private:
    enum JackNames
    {jnIn,jnOut,jnOutLeft,jnOutRight,jnModulation};
    enum ParameterNames
    {pnPan,pnModulation};
    float LeftModFactor;
    float RightModFactor;
    float CurrentMod;
    float LastMod;
    float LeftFactor;
    float RightFactor;
    float ModFactor;
    float* InSignal;
    void inline CalcParams();
    void Process();
public:
    CPanner();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
};

#endif // CPANNER_H
