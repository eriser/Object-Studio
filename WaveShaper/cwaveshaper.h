#ifndef CWAVESHAPER_H
#define CWAVESHAPER_H

#include "softsynthsclasses.h"

class CWaveShaper : public IDevice
{
private:
    enum JackNames
    {jnIn,jnOut};
    enum ParameterNames
    {pnGain,pnAmount,pnType};
    float m_Gain;
    float k;
    float k1;
    float k2;
    int a;
    float a1;
    void inline CalcParams();
    int inline sign(float x);
    float inline max (float x, float a);
    float inline min (float x, float b);
    float inline clip (float x, float a, float b);
public:
    CWaveShaper();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
};

#endif // CWAVESHAPER_H
