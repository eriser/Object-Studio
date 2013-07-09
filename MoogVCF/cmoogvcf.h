#ifndef CMOOGVCF_H
#define CMOOGVCF_H

#include "softsynthsclasses.h"
#include "cfreqglider.h"

class  CMoogVCF : public IDevice
{
public:
    CMoogVCF();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
private:
    enum JackNames
    {jnOut,jnIn,jnModulation};
    enum ParameterNames
    {pnInVolume,pnCutOffModulation,pnCutOffFrequency,pnResponse,pnResonance,pnOutVolume};
    int Maxcutoff;
    int LastResonance;
    float LastCO;
    //int TargetFreq;
    float ModulationFactor;
    float InVolumeFactor;
    float OutVolumeFactor;
    void inline CalcParams();
    void inline CalcExpResonance(float CutOff,float Resonance);
    //float fc;
    //float res;
    float f;
    float fb;
    float fa;
    float In1;
    float In2;
    float In3;
    float In4;
    float Out1;
    float Out2;
    float Out3;
    float Out4;
    CFreqGlider FreqGlider;
};

#endif // CMOOGVCF_H
