#include "cstereomixer.h"
#ifdef STEREOMIXER_LIBRARY
#include "cstereomixerform.h"
#endif
#define devicename "StereoMixer"

void inline CalcPeak(float Val,float* Peak)
{
    if (Val < 0)
    {
        Val = -Val;
    }
    if (Val > *Peak)
    {
        *Peak=Val;
    }
}

CStereoMixerChannel::CStereoMixerChannel(int sends)
{
    sendCount=sends;
    Effect=new float[sends];
    for (int i=0;i<sends;i++) Effect[i]=1;
    Level=1;
    PanL=1;
    PanR=1;
    Mute=false;
    EffectMute=false;
    PeakL=0;
    PeakR=0;
}

CStereoMixerChannel::~CStereoMixerChannel()
{
    delete [] Effect;
}

void CStereoMixerChannel::MixChannel(float *Signal, CStereoBuffer *Out, CStereoBuffer **Send, const int BufferSize, const bool First, CStereoBuffer *WorkBuffer)
{
    if (First)
    {
        if (Level != 0)
        {
            Out->FromBuffer(Signal,Level*PanL,Level*PanR);
            if (!EffectMute)
            {
                for (int j=0;j<sendCount;j++) Send[j]->FromBuffer(Out->Buffer,Effect[j]);
            }
            else
            {
                for (int j=0;j<sendCount;j++) Send[j]->ZeroBuffer();
            }
            for (int i=0; i < BufferSize; i++)
            {
                CalcPeak(Out->Buffer[i],&PeakL);
                CalcPeak(Out->BufferR[i],&PeakR);
            }
        }
        else
        {
            Out->ZeroBuffer();
            for (int j=0;j<sendCount;j++) Send[j]->ZeroBuffer();
            PeakL=0;
            PeakR=0;
        }
    }
    else
    {
        WorkBuffer->FromBuffer(Signal,Level*PanL,Level*PanR);
        if (Level != 0)
        {
            Out->AddBuffer(WorkBuffer->Buffer);
            if (!EffectMute) for (int j=0;j<sendCount;j++) Send[j]->AddBuffer(WorkBuffer->Buffer,Effect[j]);
            for (int i=0; i < BufferSize; i++)
            {
                CalcPeak(WorkBuffer->Buffer[i],&PeakL);
                CalcPeak(WorkBuffer->BufferR[i],&PeakR);
            }
        }
        else
        {
            PeakL=0;
            PeakR=0;
        }
    }
}

CStereoMixer::CStereoMixer(int channels, int sends)
{
    Disabled=false;
    channelCount=channels;
    sendCount=sends;
    Sends=new float[sends];
    for (int i=0;i<sends;i++) Sends[i]=1;
    this->channels=new CStereoMixerChannel*[channelCount];
    for (int i=0;i<channelCount;i++)
    {
        this->channels[i]=new CStereoMixerChannel(sends);
    }
}

CStereoMixer::~CStereoMixer()
{
    Disabled=true;
    for (int i=0;i<channelCount;i++)
    {
        delete channels[i];
    }
    delete []  channels;
    delete [] Sends;
}

void CStereoMixer::Process()
{
    float* Signal[channelCount];
    int ActiveBuffers=0;
    int OrigChannel[channelCount];
    CStereoBuffer* OutBuffer=(CStereoBuffer*)AudioBuffers[jnOut];
    CStereoBuffer** SendBuffers=(CStereoBuffer**)&AudioBuffers[jnSend];

    for (int i =0; i < channelCount; i++)
    {
        CStereoMixerChannel* ch=channels[i];
        float* TempBuffer=FetchA(jnSend+sendCount+i);
        if (TempBuffer != NULL)
        {
            if (SoloChannel>-1)
            {
                if (SoloChannel==i)
                {
                    Signal[ActiveBuffers]=TempBuffer;
                    OrigChannel[ActiveBuffers]=SoloChannel;
                    ActiveBuffers++;
                }
            }
            else if (!ch->Mute)
            {
                Signal[ActiveBuffers]=TempBuffer;
                OrigChannel[ActiveBuffers]=i;
                ActiveBuffers++;
            }
        }
    }
    if (ActiveBuffers==0)
    {
        OutBuffer->ZeroBuffer();
        for (int i=0;i<sendCount;i++) SendBuffers[i]->ZeroBuffer();
    }
    else
    {
        channels[OrigChannel[0]]->MixChannel(Signal[0],OutBuffer,SendBuffers,m_BufferSize,true,&WorkBuffer);
    }
    for (int InChannel=1;InChannel<ActiveBuffers;InChannel++)
    {
        channels[OrigChannel[InChannel]]->MixChannel(Signal[InChannel],OutBuffer,SendBuffers,m_BufferSize,false,&WorkBuffer);
    }
    for (int j=0;j<sendCount;j++)
    {
        float SendVol=MixFactor*Sends[j];
        SendBuffers[j]->Multiply(SendVol);
    }
    OutBuffer->AddBuffer(FetchA(jnReturn));
    OutBuffer->Multiply(MasterLeft*MixFactor,MasterRight*MixFactor);
    for (int i = 0; i < m_BufferSize; i++)
    {
        CalcPeak(*(OutBuffer->Buffer+i),&PeakL);
        CalcPeak(*(OutBuffer->BufferR+i),&PeakR);
    }
}

void CStereoMixer::Init(const int Index, void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("Return",IJack::Stereo,IJack::In,jnReturn);

    AddJack("Out",IJack::Stereo,IJack::Out,jnOut);
    for (int i=0;i<sendCount;i++)
    {
        AddJack("Send " + QString::number(i+1),IJack::Stereo,IJack::Out,jnSend + i);
    }

    for (int i=0;i<channelCount;i++)
    {
        AddJack("In " + QString::number(i+1),IJack::Stereo,IJack::In,jnSend+sendCount+i);
    }
    PeakL=0;
    PeakR=0;
    SoloChannel=-1;
    MasterLeft=1;
    MasterRight=1;
    MixFactor=1.0/sqrtf(channelCount);
    CalcParams();
#ifdef STEREOMIXER_LIBRARY
    m_Form=new CStereoMixerForm(this,(QWidget*)MainWindow);
#endif
}

float* CStereoMixer::GetNextA(const int ProcIndex)
{
    if (Disabled) return NULL;
    if (m_Process)
    {
        m_Process=false;
        Process();
    }
    return AudioBuffers[ProcIndex]->Buffer;
}

void CStereoMixer::Play(bool /*FromStart*/)
{
    PeakL=0;
    PeakR=0;
    for (int i=0;i<channelCount;i++)
    {
        CStereoMixerChannel* ch=channels[i];
        ch->PeakL=0;
        ch->PeakR=0;
    }
#ifdef STEREOMIXER_LIBRARY
    ((CStereoMixerForm*)m_Form)->Reset();
#endif
}
