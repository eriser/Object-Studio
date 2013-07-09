#include "cwavebank.h"
#include "softsynthsclasses.h"

int CWaveBank::RefCount=0;
float* CWaveBank::SineBufferFloat=NULL;
//float* CWaveBank::SquareBufferFloat=NULL;
float* CWaveBank::TriangleBufferFloat=NULL;
float* CWaveBank::SawToothBufferFloat=NULL;
float CWaveBank::HalfRate=0;
float CWaveBank::RAND_MAX_DIV=1.0/(float)RAND_MAX;
unsigned int CWaveBank::SampleRate=0;

void CWaveBank::FillBuffers()
{
    SampleRate=CPresets::Presets.SampleRate;
    HalfRate=CPresets::Presets.HalfRate;
    float HalfRateDivPi=(float)HalfRate / DoublePi;

    SineBufferFloat=new float[SampleRate];
    //SquareBufferFloat=new float[SampleRate];
    TriangleBufferFloat=new float[SampleRate];
    SawToothBufferFloat=new float[SampleRate];
    qDebug() << "Wavebank buffers created";
    //randomize;
    float P=0;
    for (unsigned int Position=0;Position<SampleRate;Position++)
    {
        //Sine
        SineBufferFloat[Position]=sin(P / HalfRateDivPi);

        //Triangle
        if (Position<HalfRate)
        {
            TriangleBufferFloat[Position]=((P * 2.0)/HalfRate)-1.0;
        }
        else
        {
            TriangleBufferFloat[Position]=(((P - HalfRate) * -2.0) / HalfRate) + 1.0;
        }
/*
        //Square
        if (Position<HalfRate)
        {
            SquareBufferFloat[Position]=1.0;
        }
        else
        {
            SquareBufferFloat[Position]=-1.0;
        }
*/
        //SawTooth
        SawToothBufferFloat[Position]=(P/HalfRate)-1.0;
        P++;
    }
}

//---------------------------------------------------------------------------

CWaveBank::CWaveBank()
{
    if (RefCount++==0) FillBuffers();
    wPos=0;
}

CWaveBank::~CWaveBank()
{
    if (--RefCount==0)
    {
        delete[] SineBufferFloat;
        //delete[] SquareBufferFloat;
        delete[] TriangleBufferFloat;
        delete[] SawToothBufferFloat;
    }
}

const float CWaveBank::GetNextFreq(const float &Frequency, const WaveForms &WaveForm)
{
    float retVal=GetNext(wPos,WaveForm);
    wPos+=Frequency;
    while (wPos>=SampleRate) wPos-=SampleRate;
    return retVal;
}

const float CWaveBank::GetNext(const int& Position,const WaveForms& WaveForm)
{
    switch (WaveForm)
    {
    case Sine:
        return *(SineBufferFloat+Position);
    case Square:
        //return *(SquareBufferFloat+Position);
        if (Position < HalfRate) return 1;
        return -1;
    case Triangle:
        return *(TriangleBufferFloat+Position);
    case Sawtooth:
        return *(SawToothBufferFloat+Position);
    case Noise:
        return ((qrand()*2)-RAND_MAX)*RAND_MAX_DIV;
    case SampleAndHold:
        if (Position < HalfRate)
        {
            if (!HoldSet1)
            {
                HoldFloat = ((qrand()*2)-RAND_MAX)*RAND_MAX_DIV;
                HoldSet1 = true;
                HoldSet2 = false;
            }
        }
        else
        {
            if (!HoldSet2)
            {
                HoldFloat = ((qrand()*2)-RAND_MAX)*RAND_MAX_DIV;
                HoldSet2 = true;
                HoldSet1 = false;
            }
        }
        return HoldFloat;
    }
    return 0;
}
