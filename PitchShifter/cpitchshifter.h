#ifndef CPITCHSHIFTER_H
#define CPITCHSHIFTER_H

#include "softsynthsclasses.h"

//#define M_PI 3.14159265358979323846
#define MAX_FRAME_LENGTH 4096
void smbFft(float *fftBuffer, long fftFrameSize, long sign);
double smbAtan2(double x, double y);

class CPitchShifter : public IDevice
{
private:
    enum JackNames
    {jnIn,jnModulation,jnOut};
    enum ParameterNames
    {pnShift,pnOverSampling,pnFrameSize,pnModulation};
    float PitchShift;
    int OverSampling;
    int FrameSize;
    float ModFactor;
    float gInFIFO[MAX_FRAME_LENGTH];
    float gOutFIFO[MAX_FRAME_LENGTH];
    float gFFTworksp[2*MAX_FRAME_LENGTH];
    float gLastPhase[MAX_FRAME_LENGTH/2+1];
    float gSumPhase[MAX_FRAME_LENGTH/2+1];
    float gOutputAccum[2*MAX_FRAME_LENGTH];
    float gAnaFreq[MAX_FRAME_LENGTH];
    float gAnaMagn[MAX_FRAME_LENGTH];
    float gSynFreq[MAX_FRAME_LENGTH];
    float gSynMagn[MAX_FRAME_LENGTH];
    long gRover;
    long gInit;
    void smbPitchShift(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata)
    /*
        Routine smbPitchShift(). See top of file for explanation
        Purpose: doing pitch shifting while maintaining duration using the Short
        Time Fourier Transform.
        Author: (c)1999-2002 Stephan M. Bernsee <smb@dspdimension.com>
*/;
    void inline CalcParams();
public:
    CPitchShifter();
    void Init(const int Index,void* MainWindow);
    float* GetNextA(const int ProcIndex);
};

#endif // CPITCHSHIFTER_H
