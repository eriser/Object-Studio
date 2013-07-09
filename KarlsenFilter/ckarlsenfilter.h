#ifndef CKARLSENFILTER_H
#define CKARLSENFILTER_H

#include "softsynthsclasses.h"
#include "cfreqglider.h"

class CKarlsenFilter : public IDevice
{
public:
    CKarlsenFilter();
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
    float rezamount;
    float cutoffreq;
    float pole1;
    float pole2;
    float pole3;
    float pole4;
    float ModulationFactor;
    float InVolumeFactor;
    float OutVolumeFactor;
    float MixFactor;
    CFreqGlider FreqGlider;
    void inline CalcParams();
    void inline CalcExpResonance(float CutOff);
};

#endif // CKARLSENFILTER_H
