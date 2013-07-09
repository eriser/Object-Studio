#ifndef CWAVEBANK_H
#define CWAVEBANK_H

#include <QtCore>

class CWaveBank
{
private:
    float HoldFloat;
    short HoldInt;
    bool HoldSet1;
    bool HoldSet2;
    static int RefCount;
    static float HalfRate;
    static unsigned int SampleRate;
    static float* SineBufferFloat;
    //static float* SquareBufferFloat;
    static float* TriangleBufferFloat;
    static float* SawToothBufferFloat;
    void FillBuffers();
    float wPos;
    static float RAND_MAX_DIV;
public:
    enum WaveForms
    {Sine,Square,Triangle,Sawtooth,Noise,SampleAndHold};
    CWaveBank();
    ~CWaveBank();
    const float GetNext(const int& Position,const WaveForms& WaveForm);
    const float GetNextFreq(const float& Frequency,const WaveForms& WaveForm);
};

#endif // CWAVEBANK_H
