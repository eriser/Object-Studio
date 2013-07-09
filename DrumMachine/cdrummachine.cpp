#include "cdrummachine.h"
#include "cdrummachineform.h"

void CDrumMachine::Init(const int Index,void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Wave,IJack::Out,0);
    AddParameter(ParameterType::Numeric,"Tempo","BPM",20,300,0,"",100);
    AddParameter(ParameterType::dB,"Volume","dB",0,200,0,"",100);
    BeatInterval=0;
    PatternLength=0;
    SamplesPerTick=0;
    Playing=false;
    PatternType* DefaultPattern=new PatternType("Default",16,DrumMachine::SoundCount,100,0,0);
    Patterns.append(DefaultPattern);
    PatternListType* DefaultList=new PatternListType();
    DefaultList->Pattern=DefaultPattern;
    DefaultList->Repeats=4;
    PatternsInList.append(DefaultList);
    AddSound("kick02.wav","Kick",&WG[0],&ST[0]);
    AddSound("snr01.wav","Snare",&WG[1],&ST[1]);
    AddSound("hat01.wav","Hi-Hat",&WG[2],&ST[2]);
    AddSound("hat19.wav","Open Hi-Hat",&WG[3],&ST[3]);
    AddSound("cym01.wav","Cymbal",&WG[4],&ST[4]);
    AddSound("tom01.wav","Tom 1",&WG[5],&ST[5]);
    AddSound("tom02.wav","Tom 2",&WG[6],&ST[6]);
    Reset();
    CalcParams();
    m_Form=new CDrumMachineForm(this,(QWidget*)MainWindow);
}

void inline CDrumMachine::AddSound(const QString &Path, const QString &Name, CWaveGenerator *WG, SoundType *ST)
{
    if (WG->open(":/sounds/"+Path,m_Presets.SampleRate,m_Presets.ModulationRate))
    {
        ST->Generator=WG;
        ST->Name=Name;
        ST->Volume=0;
    }
}

void CDrumMachine::Tick()
{
    if (Playing)
    {
        SampleCount+=m_BufferSize;
        while (SampleCount>=m_BufferSize)
        {
            SampleCount=SampleCount - SamplesPerTick;
            if (Counter==BeatInterval * BeatCount)
            {
                //Debug("Dr " + AnsiString(SampleCount) + " " + AnsiString(SamplesPerTick) + " " + AnsiString(Counter) );
                if (PatternIndex<PatternsInList.count())
                {
                    PatternType* P=PatternsInList[PatternIndex]->Pattern;
                    BeatType* B=P->Beat(BeatCount);
                    for (int i = 0;i<P->Polyphony;i++)
                    {
                        if (B->Volume[i] > 0)
                        {
                            ST[i].Generator->Reset();// .Volume(iTemp)
                            ST[i].Volume=B->Volume[i]*0.01;
                        }
                    }
                    ((CDrumMachineForm*)m_Form)->Flash(PatternIndex,BeatCount);
                    BeatCount++;
                    if (BeatCount>=Patterns[PatternIndex]->NumOfBeats())
                    {
                        BeatCount=0;
                        if (PatternsInList[PatternIndex]->Repeats>0)
                        {
                            PatternRepeatCount++;
                            if (PatternRepeatCount==PatternsInList[PatternIndex]->Repeats)
                            {
                                PatternIndex++;
                                if (PatternIndex==PatternsInList.count())
                                {
                                    PatternLength=0;
                                }
                                else
                                {
                                    PatternLength=PatternsInList[PatternIndex]->Pattern->NumOfBeats()*BeatInterval;
                                }
                            }
                        }
                    }
                }
            }
            Counter++;
            if (Counter>=PatternLength)
            {
                Counter=0;
            }
        }
    }
}

float* CDrumMachine::GetNextA(const int ProcIndex)
{
    if (Playing)
    {
        AudioBuffers[ProcIndex]->ZeroBuffer();
        for (int i=0;i<DrumMachine::SoundCount;i++)
        {
            AudioBuffers[ProcIndex]->AddBuffer(ST[i].Generator->GetNext(), ST[i].Volume*VolumeFactor);
        }
        return AudioBuffers[ProcIndex]->Buffer;
    }
    return NULL;
}

void inline CDrumMachine::CalcParams()
{
    int Ticks=100;
    int uSPQ=(60000000/4) / m_ParameterValues[pnTempo];
    float uSperTick=(float)uSPQ/(float)Ticks;
    SamplesPerTick = uSperTick / m_Presets.uSPerSample;
    BeatInterval = Ticks;
    float MixFactor = sqrtf(DrumMachine::SoundCount);
    VolumeFactor =  ((float)m_ParameterValues[pnVolume]*0.01)/MixFactor;
    int temp=PatternsInList[PatternIndex]->Pattern->NumOfBeats();
    PatternLength = temp * BeatInterval;
}

void CDrumMachine::Reset()
{
    PatternIndex = 0;
    PatternRepeatCount = 0;
    BeatCount = 0;
    NextBeat = 0;
    NextStop = 0;
    Counter = 0;
    SampleCount = 0;//m_Presets.ModulationRate-1;
    CalcParams();
}

void CDrumMachine::Play(const bool FromStart)
{
    if (FromStart) Reset();
    Playing=true;
}

void CDrumMachine::Pause()
{
    Playing=false;
}
