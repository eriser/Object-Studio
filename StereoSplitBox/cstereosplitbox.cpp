#include "cstereosplitbox.h"
#include "cmacroboxform.h"

CStereoSplitBox::CStereoSplitBox()
{
}

CStereoSplitBox::~CStereoSplitBox()
{
    if (m_Initialized)
    {
        ((CMacroBoxForm*)m_Form)->DesktopComponent->Clear();
        qDeleteAll(JacksCreated);
    }
}

void CStereoSplitBox::Init(const int Index, void *MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    m_Form=new CMacroBoxForm(this,(QWidget*)MainWindow);
    CDesktopComponent* d=((CMacroBoxForm*)m_Form)->DesktopComponent;
    AddJack("Out",IJack::Stereo,IJack::Out,jnOut);
    AddJack("In",IJack::Stereo,IJack::In,jnIn);

    WaveOutL=new CInJack("Out Left","This",IJack::Wave,IJack::In,this);
    JacksCreated.push_back(d->AddJack(WaveOutL,0));
    WaveOutR=new CInJack("Out Right","This",IJack::Wave,IJack::In,this);
    JacksCreated.push_back(d->AddJack(WaveOutR,0));
    JacksCreated.push_back(d->AddJack(new COutJack("In Left","This",IJack::Wave,IJack::Out,this,jnInLeft),0));
    JacksCreated.push_back(d->AddJack(new COutJack("In Right","This",IJack::Wave,IJack::Out,this,jnInRight),0));
}

void CStereoSplitBox::Tick()
{
    ((CMacroBoxForm*)m_Form)->DesktopComponent->Tick();
    m_Process=true;
}

void CStereoSplitBox::HideForm()
{
    ((CMacroBoxForm*)m_Form)->DesktopComponent->HideForms();
    m_Form->setVisible(false);
}

void CStereoSplitBox::Process()
{
    float* B=FetchA(jnIn);
    if (B != NULL)
    {
        InL=B;
        InR=B+m_BufferSize;
    }
    else
    {
        InL=NULL;
        InR=NULL;
    }
}

float* CStereoSplitBox::GetNextA(const int ProcIndex)
{
    if (ProcIndex==jnOut) ((CStereoBuffer*)AudioBuffers[jnOut])->FromMono(WaveOutL->GetNextA(),WaveOutR->GetNextA());
    if (ProcIndex==jnInLeft)
    {
        if (m_Process)
        {
            m_Process=false;
            Process();
        }
        return InL;
    }
    if (ProcIndex==jnInRight)
    {
        if (m_Process)
        {
            m_Process=false;
            Process();
        }
        return InR;
    }
    return AudioBuffers[ProcIndex]->Buffer;
}

void CStereoSplitBox::Play(const bool FromStart)
{
    ((CMacroBoxForm*)m_Form)->DesktopComponent->Play(FromStart);
}

void CStereoSplitBox::Pause()
{
    ((CMacroBoxForm*)m_Form)->DesktopComponent->Pause();
}
