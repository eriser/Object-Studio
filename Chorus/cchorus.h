#ifndef CCHORUS_H
#define CCHORUS_H

#include "softsynthsclasses.h"
#include "Biquad.h"

#define DEPTH_BUFLEN 450
#define DELAY_BUFLEN 19200

/* Max. frequency setting */
#define MAX_FREQ 5.0f

/* bandwidth of highpass filters (in octaves) */
#define HP_BW 1

/* cosine table for fast computations */
#define COS_TABLE_SIZE 1024

class CChorus : public IDevice
{
public:
    CChorus();
    ~CChorus();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
private:
    enum JackNames
    {jnOut,jnIn};
    enum ParameterNames
    {pnFrequency,pnPhase,pnDepth,pnDelay,pnContour,pnDryLevel,pnWetLevel};
    float* ring;
    unsigned int buflen;
    unsigned int pos;
    float Frequency;
    int Phase;
    float Depth;
    int Delay;
    int Contour;
    float DryLevel;
    float WetLevel;
    float cm_phase;
    float dm_phase;
    biquad highpass;
    float cos_table[COS_TABLE_SIZE];
    float inline read_buffer(float* buffer, unsigned int buflen, unsigned int pos, unsigned int n);
    void inline write_buffer(float insample, float* buffer, unsigned int buflen, unsigned int pos, unsigned int n);
    float inline push_buffer(float insample, float* buffer, unsigned int buflen, unsigned int * pos);
    void inline CalcParams();
};

#endif // CCHORUS_H
