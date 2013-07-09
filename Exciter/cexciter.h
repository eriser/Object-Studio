#ifndef CEXCITER_H
#define CEXCITER_H

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

class CExciter : public IDevice
{
private:
    enum JackNames
    {jnOut,jnEffOut,jnIn};
    enum ParameterNames
    {pnType,pnInVolume,pnAmount,pnCutOffFrequency,pnOutVolume};
    CFxRbjFilter RBJFilter;
    float InVolFactor;
    float OutVolFactor;
    float EffFactor;
    float k2;
    void inline CalcParams();
    float inline clip (float x, float a, float b);
public:
    CExciter();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
};


#endif // CEXCITER_H
