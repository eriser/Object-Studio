#include "cpitchtracker.h"


CPitchTracker::CPitchTracker()
{
}

CPitchTracker::~CPitchTracker() {
    if (m_Initialized)
    {
        delete PT;
    }
}

void CPitchTracker::Init(const int Index, void *MainWindow) {
    m_Name=devicename;
    PT=new CPitchTrackerClass(m_BufferSize*BufferCount,m_Presets.SampleRate);
    BufferFill=0;
    LastNote=0;
    BufferDivide=2;
    NewBufferDivide=2;
    IDevice::Init(Index,MainWindow);
    AddJack("In",IJack::Wave,IJack::In,0);
    AddJack("Frequency Out",IJack::Frequency,IJack::Out,1);
    AddJack("MIDI Frequency Out",IJack::Frequency,IJack::Out,2);
    AddJack("MIDI Out",IJack::MIDI,IJack::Out,3);
    AddJack("Difference Out",IJack::Pitch,IJack::Out,4);
    AddParameter(ParameterType::Numeric,"Threshold","%",0,100,0,"",0);
    AddParameter(ParameterType::Numeric,"Tune","Hz",43600,44800,100,"",44000);
    AddParameter(ParameterType::SelectBox,"Range Priority","",0,3,0,"Very High§High§Low§Very Low",1);
    CalcParams();
}

const float CPitchTracker::GetNext(const int ProcIndex) {
    float Retval=0;
    if (m_Process)
    {
        m_Process=false;
        Process(FetchA(jnIn));
    }
    if (ProcIndex==jnFrequencyOut)
    {
        Retval=PT->CurrentFreq/BufferDivide;
    }
    if (ProcIndex==jnMIDIFreqOut)
    {
        Retval=PT->CurrentMIDIFreq/BufferDivide;
    }
    if (ProcIndex==jnDiffOut)
    {
        Retval=PT->CurrentDiff;
    }
    if (BufferFill==0)
    {
        if (NewBufferDivide!=BufferDivide)
        {
            BufferDivide=NewBufferDivide;
        }
    }
    return Retval;
}

void CPitchTracker::Process(float *Input) {
    if (!Input)
    {
        BufferFill=0;
        return;
    }
    // Get coefficients
    int BSize=m_BufferSize/BufferDivide;
    int Offset=BufferFill*BSize;
    for (int i=0; i<BSize; i++)
    {
        *(PT->coeffs+i+Offset) = (double)*(Input+(i*BufferDivide));//*Factor1;
    }
    BufferFill++;
    if (BufferFill<BufferDivide*BufferCount)
    {
        return;
    }


    BufferFill=0;
    PT->Process();
}

void *CPitchTracker::GetNextP(int) {
    if (m_Process)
    {
        m_Process=false;
        Process(FetchA(jnIn));
    }
    MIDIBuffer.Reset();
    if (PT->CurrentVel)
    {
        if (PT->CurrentNote != LastNote)
        {
            if (LastNote)
            {
                //LastNote Off
                MIDIBuffer.Push(0x80);
                MIDIBuffer.Push(LastNote-(12*(BufferDivide >>1)));
                MIDIBuffer.Push(0);
            }
            if (PT->CurrentNote)
            {
                MIDIBuffer.Push(0x90);
                MIDIBuffer.Push(PT->CurrentNote-(12*(BufferDivide >>1)));
                MIDIBuffer.Push(PT->CurrentVel);
            }
        }

        LastNote=PT->CurrentNote;
    }
    else
    {
        if (LastNote)
        {
            MIDIBuffer.Push(0x80);
            MIDIBuffer.Push(LastNote-(12*(BufferDivide >>1)));
            MIDIBuffer.Push(0);

            LastNote=0;
        }
    }
    if (BufferFill==0)
    {
        if (NewBufferDivide!=BufferDivide)
        {
            BufferDivide=NewBufferDivide;
        }
    }
    return (void*)&MIDIBuffer;
}

void CPitchTracker::CalcParams() {
    PT->Threshold=m_ParameterValues[pnThreshold]*0.01;
    PT->InTune=m_ParameterValues[pnTune]*0.01;
    PT->OutTune=PT->InTune;
    switch (m_ParameterValues[pnPriority])
    {
    case 0:
        NewBufferDivide=1;
        break;
    case 1:
        NewBufferDivide=2;
        break;
    case 2:
        NewBufferDivide=4;
        break;
    case 3:
        NewBufferDivide=8;
        break;
    }
}
