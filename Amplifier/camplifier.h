#ifndef CAMPLIFIER_H
#define CAMPLIFIER_H

#include "softsynthsclasses.h"

class CAmplifier : public IDevice
{
public:
    CAmplifier();
    void Init(const int Index, void *MainWindow);
    float* GetNextA(const int ProcIndex);
private:
    enum JackNames
    {jnIn,jnOut,jnModulation};
    enum ParameterNames
    {pnModulation};
    float ModFactor;
    float CurrentMod;
    int LastMod;
    void inline CalcParams();
};

#endif // CAMPLIFIER_H
