#ifndef CENVELOPE_H
#define CENVELOPE_H

#include "softsynthsclasses.h"
#include "cadsr.h"

class CEnvelope : public IDevice
{
public:
    CEnvelope();
    void Init(const int Index,void* MainWindow);
    const float GetNext(const int ProcIndex);
    void UpdateHost();
private:
    enum JackNames
    {jnTriggerIn,jnOut};
    enum ParameterNames
    {pnAttackTime,pnDecayTime,pnSustainLevel,pnReleaseTime,pnVolume,pnMode};
    CADSR ADSR;
    float VolumeFactor;
    void inline CalcParams();
};

#endif // CENVELOPE_H
