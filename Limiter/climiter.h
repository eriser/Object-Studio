#ifndef CLIMITER_H
#define CLIMITER_H

#include "softsynthsclasses.h"

class CLimiter : public IDevice
{
private:
    enum JackNames
    {jnOut,jnIn};
    enum ParameterNames
    {pnLimitVol,pnOutVol};
    float* ringbuffer;
    unsigned int buflen;
    unsigned int ready_num;
    unsigned int pos;
    float out_vol;
    float limit_vol;
    float read_buffer(float* buffer, unsigned int buflen, unsigned int pos, unsigned int n);
    void write_buffer(float insample, float* buffer, unsigned int buflen, unsigned int pos, unsigned int n);
    float inline push_buffer(float insample, float* buffer, unsigned int buflen, unsigned int * pos);
    void inline CalcParams();
public:
    CLimiter();
    ~CLimiter();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
};

#endif // CLIMITER_H
