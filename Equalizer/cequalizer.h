#ifndef CEQUALIZER_H
#define CEQUALIZER_H

#include "softsynthsclasses.h"

#include "biquad.h"

/* Bandwidth of EQ filters in octaves */
#define BWIDTH        1.0f

class CEqualizer : public IDevice
{
private:
    enum JackNames
    {jnOut,jnIn};
    enum ParameterNames
    {};
    biquad filters[8];
    void inline CalcParams();
public:
    CEqualizer();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
    float Level[8];
    int Freq[8];
    void SetLevel(int Index, int Level);
    void SetFreq(int Index, int Freq);
};


#endif // CEQUALIZER_H
