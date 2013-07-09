#ifndef CRINGMODULATOR_H
#define CRINGMODULATOR_H

#include "softsynthsclasses.h"

class CRingModulator : public IDevice
{
private:
    enum JackNames
    {jnOut,jnIn,jnModulation};
    enum ParameterNames
    {pnModulation};
    float ModulationFactor;
    float CleanFactor;
    void inline CalcParams();
public:
    CRingModulator();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
};

#endif // CRINGMODULATOR_H
