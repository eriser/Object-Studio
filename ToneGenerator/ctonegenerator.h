#ifndef CTONEGENERATOR_H
#define CTONEGENERATOR_H

#include "softsynthsclasses.h"
#include "cwavebank.h"
#include "cfreqglider.h"

class CToneGenerator : public IDevice
{
private:
    enum JackNames
    {jnOut,jnFrequency,jnModulation,jnPulseModulation};
    enum ParameterNames
    {pnFrequency,pnGlide,pnModulation,pnTuning,pnDetune,pnWaveForm,pnPulse,pnPulseModulation,pnVolume};
    float FreqValue;
    float WavePosition;
    float DetunePosition;
    float CurrentTune;
    float CurrentDetune;
    float CurrentMod;
    float CurrentFreq;
    float DetuneFreq;
    int LastFreq;
    float LastMod;
    float LastFreqValue;
    float VolumeFactor;
    float PulseFactor;
    float PulseModulationFactor;
    CWaveBank WaveBank;
    CFreqGlider FreqGlider;
    int inline PulseCalc(int Pos,float Modulation);
    void inline CalcParams(void);
public:
    CToneGenerator();
    void Init(const int Index,void * MainWindow);
    float* GetNextA(const int ProcIndex);
};

#endif // CTONEGENERATOR_H
