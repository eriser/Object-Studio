#ifndef CUNIFILTER_H
#define CUNIFILTER_H

#include "softsynthsclasses.h"

class CFxRbjFilter
{
public:

    CFxRbjFilter();

    float filter(float in0);

    void calc_filter_coeffs(int const type,double const frequency,double const sample_rate,double const q,double const db_gain,bool q_is_bandwidth);

private:

    // filter coeffs
    float b0a0,b1a0,b2a0,a1a0,a2a0;

    // in/out history
    float ou1,ou2,in1,in2;
};

class CUnifilter : public IDevice
{
private:
    enum JackNames
    {jnOut,jnIn,jnModulation};
    enum ParameterNames
    {pnFilterType,pnInVolume,pnCutOffModulation,pnCutOffFrequency,pnResonance,pnOutVolume};
    CFxRbjFilter RBJFilter;
    float InVolFactor;
    float OutVolFactor;
    void inline CalcParams();
public:
    CUnifilter();
    ~CUnifilter();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
};

#endif // CUNIFILTER_H
