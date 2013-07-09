#include "crange.h"
#include "softsynthsclasses.h"
#include <QFileInfo>
#include "cpitchtrackerclass.h"

int FindZeroXing(float* Buffer,int Samples,int Position,bool BackwardsOnly,int NearestValuePointer)
{
    int BSteps=0;
    int FSteps=0;
    int BPos=0;
    int FPos=0;
    float Prev=0;
    bool FActive=false;
    bool BActive=false;
    float NearestValue=0;
    if (NearestValuePointer>-1) NearestValue=Buffer[NearestValuePointer];
    for (int i=Position;i>0;i--)
    {
        BActive=true;
        if (Prev>0 && Buffer[i]<=0)
        {
            if (fabs(Prev-NearestValue)>fabs(Buffer[i]-NearestValue))
            {
                BPos=i;
                if (BackwardsOnly) return BPos;
            }
            else
            {
                BPos=i+1;
            }
            break;
        }
        BSteps++;
        Prev=Buffer[i];
    }
    Prev=1;
    for (int i=Position;i<Samples;i++)
    {
        FActive=true;
        if (Prev<=0 && Buffer[i]>0)
        {
            if (fabs(Prev-NearestValue)>fabs(Buffer[i]-NearestValue))
            {
                FPos=i;
            }
            else
            {
                FPos=i-1;
            }
            break;
        }
        FSteps++;
        Prev=Buffer[i];
    }
    if (!FActive)
    {
        if (BActive) return BPos;
    }
    if (!BActive)
    {
        if (FActive) return FPos;
    }
    if (FActive & BActive)
    {
        if (FSteps>BSteps)
        {
            return BPos;
        }
        else
        {
            return FPos;
        }
    }
    return Position;
}

void CSampleKeyRange::PitchDetect(int Tune)
{
    float* Buffer=WG.BufferPointer(0);
    int HalfLength=16384*2;
    int StartPos=WG.GetLength()-(HalfLength*6);
    while (StartPos<0)
    {
        HalfLength=HalfLength/2;
        StartPos=WG.GetLength()-(HalfLength*6);
    }
    qDebug() << "Halflength" << HalfLength << WG.GetLength() << WG.LP.End << StartPos+(HalfLength*4);
    CPitchTrackerClass* PT=new CPitchTrackerClass(HalfLength,CPresets::Presets.SampleRate);
    PT->InTune=Tune;
    for (int i=0;i<HalfLength;i++)
    {
        PT->coeffs[i]=Buffer[StartPos+(i*4)];
    }
    PT->Process();
    WG.LP.MIDINote=PT->CurrentNote-24;
    WG.LP.Tune=PT->CurrentDiff*1000.0;
    delete PT;
}

void CSampleKeyRange::AutoLoop(int Cycles)
{
    float Freq=MIDItoFreq(WG.LP.MIDINote,440.0);
    float TempTune=pow(2.0,(float)(WG.LP.Tune)*0.001);
    float CycleLength=(float)CPresets::Presets.SampleRate/(Freq*TempTune);
    float Length=CycleLength*Cycles;
    WG.LP.LoopType=CWaveGenerator::ltForward;
    WG.LP.LoopEnd=FindZeroXing(WG.BufferPointer(0),WG.GetLength(),WG.LP.End-CycleLength,true,-1);
    WG.LP.LoopStart=FindZeroXing(WG.BufferPointer(0),WG.GetLength(),WG.LP.LoopEnd-Length,false,WG.LP.LoopEnd);
}

void CSampleKeyRange::AutoTune()
{
    float Length=WG.LP.LoopEnd-WG.LP.LoopStart;
    if (Length<=0)
    {
        return;
    }
    float Freq=MIDItoFreq(WG.LP.MIDINote,440.0);
    float CycleLength=(float)CPresets::Presets.SampleRate/Freq;
    float Cycles=qRound(Length/CycleLength);
    float ActualFreq=((float)CPresets::Presets.SampleRate/Length)*Cycles;
    float FreqDiff=(Freq-ActualFreq)/Freq;
    qDebug() << "Freq" << Freq << "CycleLength" << CycleLength << "Cycles" << Cycles << "ActualFreq" << ActualFreq << "FreqDiff" << FreqDiff;
    int TuneVar=FreqDiff*1000.0;
    WG.LP.Tune=TuneVar;
}

void CSampleKeyRange::AutoFix(int Cycles, int Tune)
{
    PitchDetect(Tune);
    AutoLoop(Cycles);
    AutoTune();
}

CSampleKeyRange::CSampleKeyRange(const QString& WavePath,int Upper,int Lower)
{
    qDebug() << "Create TCSampleKeyRange";
    RP.UpperZero=Upper;
    RP.UpperTop=Upper;
    RP.LowerZero=Lower;
    RP.LowerTop=Lower;
    RP.Volume=100;
    PlayVol=1;
    ChangePath(WavePath);
}

CSampleKeyRange::CSampleKeyRange(const QString& WavePath,CSampleKeyRange::RangeParams RangeParams)
{
    qDebug() << "Create TCSampleKeyRange";
    RP=RangeParams;
    PlayVol=1;
    ChangePath(WavePath);
}

CSampleKeyRange::~CSampleKeyRange()
{
    qDebug() << "Delete TCSampleKeyRange";
}

void CSampleKeyRange::ChangePath(const QString& WavePath)
{
    if (WavePath==FileName) return;
    if (QFileInfo(WavePath).exists())
    {
        WG.open(WavePath,CPresets::Presets.SampleRate,CPresets::Presets.ModulationRate);
        WG.LP.Start=0;
        WG.LP.End=WG.GetLength();
        WG.LP.LoopStart=0;
        WG.LP.LoopEnd=0;
        WG.LP.MIDINote=69;
        WG.LP.Tune=0;
        WG.LP.LoopType=CWaveGenerator::ltForward;
        WG.LP.FadeIn=0;
        WG.LP.FadeOut=0;
        WG.LP.XFade=0;
        WG.LP.Volume=100;
        FileName=WavePath;
    }
}

float CSampleKeyRange::GetVolume(int MIDIKey)
{
    if ((MIDIKey<RP.LowerZero+1) || (MIDIKey>RP.UpperZero))
    {
        return 0;
    }
    if (MIDIKey<RP.LowerTop+1)
    {
        float diff=RP.LowerTop-RP.LowerZero;
        float Val=MIDIKey-(RP.LowerZero+1);
        return 0.01 * Val*(float)RP.Volume/diff;
    }
    if (MIDIKey>RP.UpperTop)
    {
        float diff=-(RP.UpperTop-RP.UpperZero);
        float Val=-(MIDIKey-(RP.UpperZero-1));
        return 0.01 * Val*(float)RP.Volume/diff;
    }
    return (float)RP.Volume*0.01;
}

