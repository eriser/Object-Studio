#include "csequenser.h"
#include "csequenserform.h"

CSequenser::CSequenser()
{
}

void CSequenser::Init(const int Index,void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("MIDI Out",IJack::MIDI,IJack::Out,0);
    AddParameter(ParameterType::Numeric,"Tempo","BPM",20,300,0,"",100);
    AddParameter(ParameterType::Numeric,"MIDI Channel","",1,16,0,"",1);
    MIDIBuffer=new CMIDIBuffer();
    BeatInterval=0;
    PatternLength=0;
    SamplesPerTick=0;
    Playing=false;
    PatternType* DefaultPattern=new PatternType("Default",16);
    Patterns.append(DefaultPattern);
    PatternListType* DefaultList=new PatternListType();
    DefaultList->Pattern=DefaultPattern;
    DefaultList->Repeats=4;
    PatternsInList.append(DefaultList);
    Reset();
    CalcParams();
    m_Form=new CSequenserForm(this,(QWidget*)MainWindow);
}

void CSequenser::Tick()
{
    if (!Playing)
    {
        return;
    }
    MIDIBuffer->Reset();
    SampleCount=SampleCount+m_BufferSize;
    while (SampleCount>=m_BufferSize)
    {
        SampleCount=SampleCount-SamplesPerTick;
        if (Counter==NextBeat)
        {
            //Debug("Se " + AnsiString(SampleCount) + " " + AnsiString(SamplesPerTick) + " " + AnsiString(Counter) );
            if (PatternIndex<PatternsInList.count())
            {
                PatternListType* PLI=PatternsInList[PatternIndex];
                PatternType* Pattern=PLI->Pattern;
                BeatType* Beat=Pattern->Beat(BeatCount);
                if (Beat->Length[0]>0)
                {
                    CurrentLength=Beat->Length[0];
                    CurrentPitch=Beat->Pitch[0];
                    if (CurrentPitch>0)
                    {
                        MIDIBuffer->Push(m_ParameterValues[pnMIDIChannel] + 0x90 - 1);
                        MIDIBuffer->Push(CurrentPitch);
                        MIDIBuffer->Push(Beat->Volume[0] * 127 / 100);
                    }
                }
                else
                {
                    CurrentLength=0;
                }
                ((CSequenserForm*)m_Form)->Flash(PatternIndex,BeatCount);
                BeatCount++;
                NextBeat=BeatInterval * BeatCount;
                NextStop=(BeatInterval * (BeatCount - 1)) + CurrentLength;
                if (BeatCount>=Pattern->NumOfBeats())
                {
                    BeatCount=0;
                    NextBeat=0;
                    if (PLI->Repeats>0)
                    {
                        PatternRepeatCount++;
                        if (PatternRepeatCount>=PLI->Repeats)
                        {
                            PatternRepeatCount=0;
                            PatternIndex++;
                        }
                    }
                }
            }
        }
        if (Counter==NextStop)
        {
            MIDIBuffer->Push(m_ParameterValues[pnMIDIChannel] + 0x90 - 1);
            MIDIBuffer->Push(CurrentPitch);
            MIDIBuffer->Push(0);
        }
        Counter++;
        if (Counter>=PatternLength)
        {
            Counter=0;
            CalcParams();
        }
    }
}

void* CSequenser::GetNextP(int /*ProcIndex*/)
{
    if (!Playing)
    {
        return NULL;
    }
    return (void*)MIDIBuffer;
}

void inline CSequenser::CalcParams()
{
    int Ticks=100;
    PatternLength=0;
    if (PatternIndex<PatternsInList.count())
    {
        PatternListType* PL=PatternsInList[PatternIndex];
        PatternType* P=PL->Pattern;
        int uSPQ=(60000000/4) / (m_ParameterValues[pnTempo]*P->Tempo*0.01);
        float uSperTick=(float)uSPQ / (float)Ticks;
        SamplesPerTick= uSperTick / m_Presets.uSPerSample;
        BeatInterval=Ticks;
        PatternLength=P->NumOfBeats() * BeatInterval;
    }
}

void CSequenser::Reset()
{
    CurrentPitch = 0;
    CurrentLength = 0;
    PatternIndex = 0;
    PatternRepeatCount = 0;
    BeatCount = 0;
    NextBeat = 0;
    NextStop = 0;
    Counter = 0;
    SampleCount = 0;
    CalcParams();
    MIDIBuffer->Reset();
}

void CSequenser::Play(const bool FromStart)
{
    if (FromStart)
    {
        Reset();
    }
    Playing=true;
}

void CSequenser::Pause()
{
    Playing=false;
}
