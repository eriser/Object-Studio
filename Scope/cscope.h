#ifndef CSCOPE_H
#define CSCOPE_H

#include "softsynthsclasses.h"

#define MaxBuffers 8

class CScope : public IDevice
{
public:
    CScope();
    ~CScope();
    void Init(const int Index,void* MainWindow);
    void Tick();
    void DrawBuffers();
private:
    enum JackNames
    {jnIn};
    enum ParameterNames
    {pnVolume,pnFrequency,pnMode};
    void Reset();
    void inline CalcParams();
    int SpectrumCount;
    float* m_Buffer[MaxBuffers];
    float* PlotBuffer[MaxBuffers];
    int CurrentBuffer;
    int NullBuffers;
};

#endif // CSCOPE_H
