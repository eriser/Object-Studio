#ifndef CNOISEGATE_H
#define CNOISEGATE_H

#include "softsynthsclasses.h"

class CNoiseGate : public IDevice
{
public:
    CNoiseGate();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
    const float GetNext(int /*ProcIndex*/);
private:
    enum JackNames
    {jnIn,jnOut,jnEnvOut};
    enum ParameterNames
    {pnThreshold,pnResponse,pnDecay};
    float Threshold;
    float TargetVol;
    float LastGlideVol;
    float GlideFactor;
    float DecayFactor;
    float CurrentVol;
    void inline CalcParams();
    void Process();
};

#endif // CNOISEGATE_H
