#ifndef CFILTER_H
#define CFILTER_H

#include "softsynthsclasses.h"
#include "cfreqglider.h"

class CFilter : public IDevice
{
private:
    enum JackNames
    {jnOut,jnIn,jnModulation};
    enum ParameterNames
    {pnInVolume,pnCutOffModulation,pnCutOffFrequency,pnResponse,pnResonance,pnOutVolume};
    int Maxcutoff;
    float FiltCoefTab0;
    float FiltCoefTab1;
    float FiltCoefTab2;
    float FiltCoefTab3;
    float FiltCoefTab4;
    float ly1;
    float ly2;
    float lx1;
    float lx2;
    float m_ExpResonance;
    int LastResonance;
    float LastCO;
    //int TargetFreq;
    float MixFactor;
    float ModulationFactor;
    float InVolumeFactor;
    CFreqGlider FreqGlider;
    void inline CalcParams();
    void inline CalcExpResonance();
public:
    CFilter();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
};

#endif // CFILTER_H
