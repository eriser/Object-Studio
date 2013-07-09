#include "cenvelope.h"
#include "cenevelopeform.h"

CEnvelope::CEnvelope()
{
}

void CEnvelope::Init(const int Index,void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("Trigger In",IJack::Amplitude,IJack::In,0);
    AddJack("Out",IJack::Amplitude,IJack::Out,0);
    AddParameter(ParameterType::Numeric,"Attack Time","mSec",0,2000,0,"",0);
    AddParameter(ParameterType::Numeric,"Decay Time","mSec",0,2000,0,"",0);
    AddParameter(ParameterType::Numeric,"Sustain Level","%",0,100,0,"",100);
    AddParameter(ParameterType::Numeric,"Release Time","mSec",0,2000,0,"",0);
    AddParameter(ParameterType::dB,"Volume","dB",0,200,0,"",100);
    AddParameter(ParameterType::SelectBox,"Input Mode","",0,1,0,"Analog§Binary",0);
    m_Form=new CEnvelopeForm(this,(QWidget*)MainWindow);
    CalcParams();
}

const float CEnvelope::GetNext(int /*ProcIndex*/)
{
    return ADSR.GetVol(Fetch(jnTriggerIn))*VolumeFactor;
}

void inline CEnvelope::CalcParams()
{
    ADSR.AP.Attack=m_ParameterValues[pnAttackTime];
    ADSR.AP.Decay=m_ParameterValues[pnDecayTime];
    ADSR.AP.Sustain=m_ParameterValues[pnSustainLevel];
    ADSR.AP.Release=m_ParameterValues[pnReleaseTime];
    ADSR.Mode=m_ParameterValues[pnMode];
    VolumeFactor=m_ParameterValues[pnVolume]*0.01;
    ((CEnvelopeForm*)m_Form)->ADSRWidget->Update(ADSR.AP);
    ADSR.CalcParams();
}

void CEnvelope::UpdateHost()
{
    ADSR.AP=((CEnvelopeForm*)m_Form)->AP;
    m_ParameterValues[pnAttackTime]=ADSR.AP.Attack;
    m_ParameterValues[pnDecayTime]=ADSR.AP.Decay;
    m_ParameterValues[pnSustainLevel]=ADSR.AP.Sustain;
    m_ParameterValues[pnReleaseTime]=ADSR.AP.Release;
    ADSR.CalcParams();
    IDevice::UpdateHost();
}
