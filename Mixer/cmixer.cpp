#include "cmixer.h"
#include "cmixerform.h"

CMixer::CMixer()
{
}

void inline CalcPeak(float Val,float* Peak)
{
    if (Val<0)
    {
        Val=-Val;
    }
    if (Val>*Peak)
    {
        *Peak=Val;
    }
}

void CMixer::Process()
{
    float* Signal[Mixer::mixerchannels];
    int ActiveBuffers=0;
    int OrigChannel[Mixer::mixerchannels];
    CStereoBuffer* OutBuffer=(CStereoBuffer*)AudioBuffers[jnOut];
    CStereoBuffer* SendBuffer=(CStereoBuffer*)AudioBuffers[jnSend];
    float* OutL=OutBuffer->Buffer;
    float* OutR=OutBuffer->BufferR;
    float* SendL=SendBuffer->Buffer;
    float* SendR=SendBuffer->BufferR;

    for (int i =0; i < Mixer::mixerchannels; i++)
    {
        float* TempBuffer=FetchA(i+jnIn);
        if (TempBuffer)
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
            else if (!Mute[i])
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
        SendBuffer->ZeroBuffer();
    }
    else
    {
        int C=OrigChannel[0];
        if (!EffectMute[C])
        {
            for (int i=0; i < m_BufferSize; i++)
            {
                float Sig=*(Signal[0]+i)*Level[C];
                float L=Sig*PanL[C];
                float R=Sig*PanR[C];
                OutL[i]=L;
                OutR[i]=R;
                SendL[i]=(L*Effect[C]);
                SendR[i]=(R*Effect[C]);
                CalcPeak(Sig,&Peak[C]);
            }
        }
        else
        {
            SendBuffer->ZeroBuffer();
            for (int i=0; i < m_BufferSize; i++)
            {
                float Sig=*(Signal[0]+i)*Level[C];
                OutL[i]=Sig*PanL[C];
                OutR[i]=Sig*PanR[C];
                CalcPeak(Sig,&Peak[C]);
            }
        }
    }
    for (int InChannel=1;InChannel<ActiveBuffers;InChannel++)
    {
        int C=OrigChannel[InChannel];
        if (!EffectMute[C])
        {
            for (int i=0; i < m_BufferSize; i++)
            {
                float Sig=*(Signal[InChannel]+i)*Level[C];
                float L=Sig*PanL[C];
                float R=Sig*PanR[C];
                OutL[i]+=L;
                OutR[i]+=R;
                SendL[i]+=(L*Effect[C]);
                SendR[i]+=(R*Effect[C]);
                CalcPeak(Sig,&Peak[C]);
            }
        }
        else
        {
            for (int i=0; i < m_BufferSize; i++)
            {
                float Sig=*(Signal[InChannel]+i)*Level[C];
                OutL[i]+=Sig*PanL[C];
                OutR[i]+=Sig*PanR[C];
                CalcPeak(Sig,&Peak[C]);
            }
        }
    }
    SendBuffer->Multiply(MixFactor);
    OutBuffer->AddBuffer(FetchA(jnReturn));
    OutBuffer->Multiply(MasterLeft*MixFactor,MasterRight*MixFactor);
    for (int i = 0; i < m_BufferSize; i++)
    {
        CalcPeak(OutL[i],&PeakL);
        CalcPeak(OutR[i],&PeakR);
    }
}

void CMixer::Init(const int Index, void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);

    AddJack("Return",IJack::Stereo,IJack::In,jnReturn);
    AddJack("Out",IJack::Stereo,IJack::Out,jnOut);
    AddJack("Send",IJack::Stereo,IJack::Out,jnSend);

    for (int i=0;i<Mixer::mixerchannels;i++)
    {
        AddJack("In " + QString::number(i+1),IJack::Wave,IJack::In,jnIn+i);
        Level[i]=1;
        Effect[i]=0;
        PanL[i]=1;
        PanR[i]=1;
        Mute[i]=false;
        EffectMute[i]=false;
    }
    PeakL=0;
    PeakR=0;
    ZeroMemory(Peak,Mixer::mixerchannels*sizeof(float));
    SoloChannel=-1;
    MasterLeft=1;
    MasterRight=1;
    MixFactor=1.0/sqrt(Mixer::mixerchannels);
    CalcParams();
    m_Form=new CMixerForm(this,(QWidget*)MainWindow);

}

void CMixer::GetPeak(float* P,float* L,float* R)
{
    CopyMemory(P,Peak,Mixer::mixerchannels*sizeof(float));
    *L=PeakL;
    *R=PeakR;
    PeakL=0;
    PeakR=0;
    ZeroMemory(Peak,Mixer::mixerchannels*sizeof(float));
}

void CMixer::Play(bool /*FromStart*/)
{
    PeakL=0;
    PeakR=0;
    ZeroMemory(Peak,Mixer::mixerchannels*sizeof(float));
    ((CMixerForm*)m_Form)->Reset();
}
