#ifndef CLFO_H
#define CLFO_H

#include "softsynthsclasses.h"
#include "cwavebank.h"

class CLFO : public IDevice
{
private:
    enum JackNames
    {jnOutPitch,jnOutAmplitude};
    enum ParameterNames
    {pnFrequency,pnWaveForm,pnVolume};
    float WavePosition;
    float FreqValue;
    float VolumeFactor;
    float ReturnValue;
    CWaveBank WaveBank;
    void inline CalcParams();
public:
    CLFO();
    void Init(const int Index,void* MainWindow);
    void Tick();
    const float GetNext(const int ProcIndex);
};

#endif // CLFO_H
