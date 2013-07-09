#include "cpolybox.h"
#include "cmacroboxform.h"

CPolyBox::CPolyBox()
{
}

CPolyBox::~CPolyBox()
{
    if (m_Initialized)
    {
        ((CMacroBoxForm*)m_Form)->DesktopComponent->Clear();
        qDeleteAll(JacksCreated);
    }
}

void CPolyBox::Init(const int Index, void *MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    m_Form=new CMacroBoxForm(this,(QWidget*)MainWindow);
    CDesktopComponent* d=((CMacroBoxForm*)m_Form)->DesktopComponent;
    AddJack("Out",IJack::Wave,IJack::Out,0);
    AddJack("MIDI In",IJack::MIDI,IJack::In,1);

    d->SetPoly(CVDevice::CVVoices-1);

    WaveOut=(CInJack*)d->CreateInsideJack(0,(IJack*)m_Jacks.front(),this);
    JacksCreated.push_back(WaveOut);

    for (int i=0;i<CVDevice::CVVoices;i++)
    {
        d->AddJack(WaveOut,i);
        JacksCreated.push_back(d->AddJack(new COutJack("Frequency In","This",IJack::Frequency,IJack::Out,this,2+i),i));
        JacksCreated.push_back(d->AddJack(new COutJack("Trigger In","This",IJack::Amplitude,IJack::Out,this,2+i+CVDevice::CVVoices),i));
    }
    AddParameter(ParameterType::SelectBox,"MIDI Channel","",0,16,0,"All§1§2§3§4§5§6§7§8§9§10§11§12§13§14§15§16",0);
    AddParameter(ParameterType::Numeric,"Transpose","Half tones",-24,24,0,"",0);
    AddParameter(ParameterType::Numeric,"Tune","Hz",43600,44800,100,"",44000);
    CalcParams();
}

void CPolyBox::Tick()
{
    m_Process=true;
    ((CMacroBoxForm*)m_Form)->DesktopComponent->Tick();
}

void CPolyBox::HideForm()
{
    ((CMacroBoxForm*)m_Form)->DesktopComponent->HideForms();
    m_Form->setVisible(false);
}

const float CPolyBox::GetNext(const int ProcIndex)
{
    if (m_Process)
    {
        m_Process=false;
        CVDevice.parseMIDI((CMIDIBuffer*)FetchP(1));
    }
    if ((ProcIndex>=2) & (ProcIndex<(2+CVDevice::CVVoices)))
    {
        return CVDevice.Notes[ProcIndex-2].Frequency*CVDevice.getPitchbend(ProcIndex-2);
    }
    else if (ProcIndex>=2+CVDevice::CVVoices)
    {
        return CVDevice.Notes[ProcIndex-(2+CVDevice::CVVoices)].Velocity*CVDevice.Vol(ProcIndex-(2+CVDevice::CVVoices));
    }
    return 0;
}

void* CPolyBox::GetNextP(int /*ProcIndex*/)
{
    return NULL;
}

float* CPolyBox::GetNextA(const int ProcIndex)
{
    if (ProcIndex==0) return WaveOut->GetNextA();
    return NULL;
}

void inline CPolyBox::CalcParams()
{
    CVDevice.Tune=m_ParameterValues[pnTune]*0.01;
    CVDevice.setTranspose(m_ParameterValues[pnTranspose]);
    CVDevice.setChannel(m_ParameterValues[pnMIDIChannel]);
}

void CPolyBox::Reset()
{
    CalcParams();
}

void CPolyBox::Play(const bool FromStart)
{
    if (FromStart)
    {
        CVDevice.reset();
        CalcParams();
    }
    ((CMacroBoxForm*)m_Form)->DesktopComponent->Play(FromStart);
}

void CPolyBox::Pause()
{
    CVDevice.allNotesOff();
    CalcParams();
    ((CMacroBoxForm*)m_Form)->DesktopComponent->Pause();
}
