#ifndef CDELAY_H
#define CDELAY_H

#include "softsynthsclasses.h"
#include "cwavebank.h"

class CDelay : public IDevice
{
private:
    enum JackNames
    {jnOut,jnEffectOut,jnIn};
    enum ParameterNames
    {pnFrequency,pnAmplitude,pnDelay,pnRegen,pnMix};
    float* Buffer;
    int ReadPosition;
    float WavePosition;
    float CleanMix;
    float EffectMix;
    float RegenCleanMix;
    float RegenEffectMix;
    int DelayRate;
    float CurrentMod;
    CWaveBank WaveBank;
    void Process(void);
    void inline CalcParams(void);
public:
    CDelay();
    ~CDelay();
    void Init(const int Index,void* MainWindow);
    void Tick(void);
};

#endif // CDELAY_H
