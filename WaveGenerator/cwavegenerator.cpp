#include "cwavegenerator.h"
#include <QFileInfo>

QMap<QString, CWaveFile*> CWaveGenerator::WaveFiles=QMap<QString, CWaveFile*>();

CWaveGenerator::CWaveGenerator()
{
    Audio=0;
    WF=0;
    Init();
}

CWaveGenerator::~CWaveGenerator()
{
    if (WaveFiles.contains(m_Path))
    {
        if (--WF->refCount==0)
        {
            delete WF;
            WaveFiles.remove(m_Path.toLower());
        }
    }
    if (Audio)
    {
        delete [] Audio;
    }
}

bool CWaveGenerator::open(const QString& path, unsigned int SampleRate, unsigned int BufferSize)
{
    Buffer=0;
    Length=0;
    m_Path=QFileInfo(path).absoluteFilePath();
    m_SampleRate=SampleRate;
    m_BufferSize=BufferSize;
    if (WaveFiles.contains(m_Path.toLower()))
    {
        WF=WaveFiles[m_Path.toLower()];
        WF->refCount++;
    }
    else
    {
        WF=new CWaveFile;
        if (WF->open(m_Path,SampleRate))
        {
            WaveFiles.insert(m_Path.toLower(),WF);
            WF->refCount++;
        }
        else
        {
            m_Path="";
            delete WF;
            return false;
        }
    }
    Init();
    Buffer=WF->data;
    Channels=WF->channels;
    Audio=new float[BufferSize*Channels];
    ZeroMemory(Audio,BufferSize*sizeof(float)*Channels);
    Length=WF->Length;
    LP.End=Length;
    return true;
}

void inline CWaveGenerator::Init()
{
    Pointer=0;
    Finished=true;
    Buffer=0;
    Length=0;
    LP.Start=0;
    LP.End=0;
    LP.LoopStart=0;
    LP.LoopEnd=0;
    LP.MIDINote=69;
    LP.Tune=0;
    LP.XFade=0;
    LP.LoopType=ltForward;
    Position=0;
    OrigFreq=440;
    SampleState=ssSilent;
}

float* CWaveGenerator::GetNext(void)
{
    if (Finished)
    {
        return NULL;
    }
    for (unsigned int i = 0; i < m_BufferSize; i++)
    {
        if (Pointer>=Length)
        {
            Finished=true;
            for (int c=0; c < Channels; c++)
            {
                Audio[i+(m_BufferSize*c)]=0;
            }
        }
        else
        {
            for (int c=0; c < Channels; c++)
            {
                Audio[i+(m_BufferSize*c)]=*(Buffer+(size_t)Pointer+(Length*c));
            }
            Pointer++;
        }
    }
    return Audio;
}

float* CWaveGenerator::GetNext(int RateOverride)
{
    if (Finished)
    {
        return NULL;
    }
    float OverrideFactor=(float)RateOverride/(float)WF->frequency;
    if (OverrideFactor==0) OverrideFactor=1;
    for (unsigned int i = 0; i < m_BufferSize; i++)
    {
        if (Pointer>=Length)
        {
            Finished=true;
            for (int c=0; c < Channels; c++)
            {
                Audio[i+(m_BufferSize*c)]=0;
            }
        }
        else
        {
            for (int c=0; c < Channels; c++)
            {
                Audio[i+(m_BufferSize*c)]=*(Buffer+(size_t)Pointer+(Length*c));
            }
            Pointer+=OverrideFactor;
        }
    }
    return Audio;
}

float* CWaveGenerator::GetNext(const float& Frequency)
{
    if (SampleState==ssSilent) return NULL;
    float XFadeVol=0;
    for (unsigned int i = 0; i < m_BufferSize; i++)
    {
        float Vol=1.0;
        switch (LP.LoopType)
        {
        case ltForward:
            if (SampleState==ssStarting)
            {
                Position=LP.Start;
                if (LP.LoopStart<LP.LoopEnd)
                {
                    SampleState=ssLooping;
                }
                else
                {
                    SampleState=ssEnding;
                }
            }
            else if (SampleState==ssLooping)
            {
                while (Position>LP.LoopEnd) Position-=LP.LoopEnd-LP.LoopStart;
            }
            else
            {
                if ((Position >= LP.End) | (Position >= Length)) SampleState=ssSilent;
            }
            if (Position<0)
            {
                for (int c=0; c < Channels; c++) Audio[i+(m_BufferSize*c)]=0;
            }
            else if ((Position >= LP.End) | (Position >= Length))
            {
                SampleState=ssSilent;
                for (int c=0; c < Channels; c++) Audio[i+(m_BufferSize*c)]=0;
            }
            else
            {
                for (int c=0; c < Channels; c++) Audio[i+(m_BufferSize*c)]=*(Buffer+(size_t)Position+(Length*c));
            }
            if (Frequency>0)
            {
                Position+=Frequency/OrigFreq;
            }
            else
            {
                Position++;
            }
            break;
        case ltAlternate:
            if (SampleState==ssStarting)
            {
                Position=LP.Start;
                if (LP.LoopStart<LP.LoopEnd)
                {
                    SampleState=ssLooping;
                }
                else
                {
                    SampleState=ssEnding;
                }
            }
            else if (SampleState==ssLooping)
            {
                if (AlternateDirection==1)
                {
                    if (Position>=LP.LoopEnd) AlternateDirection=-1;
                }
                else if (AlternateDirection==-1)
                {
                    if (Position<=LP.LoopStart) AlternateDirection=1;
                }
            }
            else
            {
                if (AlternateDirection==-1)
                {
                    if (Position<=LP.LoopStart) AlternateDirection=1;
                }
                if ((Position >= LP.End) | (Position >= Length)) SampleState=ssSilent;
            }
            if (Position<0)
            {
                for (int c=0; c < Channels; c++) Audio[i+(m_BufferSize*c)]=0;
            }
            else if ((Position >= LP.End) | (Position >= Length))
            {
                SampleState=ssSilent;
                for (int c=0; c < Channels; c++) Audio[i+(m_BufferSize*c)]=0;
            }
            else
            {
                for (int c=0; c < Channels; c++) Audio[i+(m_BufferSize*c)]=*(Buffer+(size_t)Position+(Length*c));
            }
            if (Frequency>0)
            {
                Position+=(Frequency/OrigFreq)*AlternateDirection;
            }
            else
            {
                Position+=AlternateDirection;
            }
            break;
        case ltXFade:
            if (SampleState==ssStarting)
            {
                Position=LP.Start;
                XFadePosition=LP.Start;
                if (LP.LoopStart<LP.LoopEnd)
                {
                    SampleState=ssLooping;
                }
                else
                {
                    SampleState=ssEnding;
                }
            }
            else if (SampleState==ssLooping)
            {
                if (XFadeFactor)
                {
                    if (!XFadeStarted)
                    {
                        if (Position>XFadeEnd) XFadeStarted=true;
                    }
                    if (XFadeStarted)
                    {
                        XFadeVol=0;
                        if (Position>XFadeEnd && Position<LP.LoopEnd)
                        {
                            float diff=LP.LoopEnd-Position;
                            XFadeVol=(LP.XFade-diff)*XFadeFactor;
                            XFadePosition=LP.LoopStart-diff;
                        }
                        if (Position<XFadeStart && Position>LP.LoopStart)
                        {
                            float diff=Position-LP.LoopStart;
                            XFadeVol=(LP.XFade-diff)*XFadeFactor;
                            XFadePosition=LP.LoopEnd+diff;
                        }
                        Vol=1.0-XFadeVol;
                    }
                    while (Position>LP.LoopEnd) Position-=LP.LoopEnd-LP.LoopStart;
                }
                else
                {
                    while (Position>LP.LoopEnd) Position-=LP.LoopEnd-LP.LoopStart;
                }
            }
            else
            {
                if (XFadeFactor)
                {
                    if (XFadeStarted)
                    {
                        XFadeVol=0;
                        if (Position>XFadeEnd && Position<LP.LoopEnd)
                        {
                            float diff=LP.LoopEnd-Position;
                            XFadeVol=(LP.XFade-diff)*XFadeFactor;
                            XFadePosition=LP.LoopStart-diff;
                        }
                        if (Position<XFadeStart && Position>LP.LoopStart)
                        {
                            float diff=Position-LP.LoopStart;
                            XFadeVol=(LP.XFade-diff)*XFadeFactor;
                            XFadePosition=LP.LoopEnd+diff;
                        }
                        if (Position<(LP.LoopEnd+LP.XFade) && Position>LP.LoopEnd)
                        {
                            float diff=Position-LP.LoopEnd;
                            XFadeVol=(LP.XFade-diff)*XFadeFactor;
                            XFadePosition=LP.LoopStart+diff;
                        }
                        Vol=1.0-XFadeVol;
                    }
                }
                if ((Position >= LP.End) | (Position >= Length)) SampleState=ssSilent;
            }
            if (Position<0)
            {
                for (int c=0; c < Channels; c++) Audio[i+(m_BufferSize*c)]=0;
            }
            else if ((Position >= LP.End) | (Position >= Length))
            {
                SampleState=ssSilent;
                for (int c=0; c < Channels; c++) Audio[i+(m_BufferSize*c)]=0;
            }
            else
            {
                for (int c=0; c < Channels; c++) Audio[i+(m_BufferSize*c)]=*(Buffer+(size_t)Position+(Length*c))*Vol;
            }

            if (XFadeFactor)
            {
                if (XFadeVol)
                {
                    if (XFadePosition>=0 && XFadePosition<Length)
                    {
                        for (int c=0; c < Channels; c++) Audio[i+(m_BufferSize*c)]+=*(Buffer+(size_t)XFadePosition+(Length*c))*XFadeVol;
                    }
                }
            }

            if (Frequency>0)
            {
                Position+=Frequency/OrigFreq;
            }
            else
            {
                Position++;
            }
            break;
        default:
            return NULL;
        }
    }
    return Audio;
}

size_t CWaveGenerator::GetLength()
{
    return Length;
}

void CWaveGenerator::Reset()
{
    Pointer=0;
    Finished=false;
    SampleState=ssStarting;
    AlternateDirection=1;
    XFadeFactor=0;
    if (LP.XFade) XFadeFactor=(1.0/LP.XFade)*0.5;
    XFadeStart=LP.LoopStart+LP.XFade;
    XFadeEnd=LP.LoopEnd-LP.XFade;

    //float TempTune=(float)(1000-LP.Tune)*0.001;
    float TempTune=pow(2.0,(float)(LP.Tune)*0.001);
    //if (TempTune<1) TempTune=((TempTune-1)*0.5)+1;
    OrigFreq=MIDItoFreq(LP.MIDINote,440.0*TempTune);
    XFadeStarted=false;
}

void CWaveGenerator::Release()
{
    if (SampleState != ssSilent)
    {
        SampleState=ssEnding;
    }
}

float* CWaveGenerator::BufferPointer(const int& Channel)
{
    return Buffer+(Length*Channel);
}

void CWaveGenerator::SetPointer(const size_t& Ptr)
{
    Pointer=Ptr;
}

