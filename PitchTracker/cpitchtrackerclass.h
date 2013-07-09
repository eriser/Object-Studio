#ifndef CPITCHTRACKERCLASS_H
#define CPITCHTRACKERCLASS_H

#include "softsynthsclasses.h"

class CPitchTrackerClass
{
private:
    double* coeffs1;
    double* coeffs2;
    double* coeffs3;
    double* Product;
    int note_num[5];
    int note_num3[5];
    int BufferSize;
        int m_SampleRate;
public:
    double* coeffs;
        float CurrentFreq;
    int CurrentVel;
        int CurrentNote;
    float CurrentMIDIFreq;
        float CurrentDiff;
        float Threshold;
        float InTune;
        float OutTune;
        CPitchTrackerClass(int BufferSz,int SampleRate);
    ~CPitchTrackerClass();
    void Process();
};

#endif // CPITCHTRACKERCLASS_H
