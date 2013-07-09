#include "cstereobox.h"
#include "cmacroboxform.h"

CStereoBox::CStereoBox()
{
}

CStereoBox::~CStereoBox()
{
    if (m_Initialized)
    {
        ((CMacroBoxForm*)m_Form)->DesktopComponent->Clear();
        qDeleteAll(JacksCreated);
    }
}

void CStereoBox::Init(const int Index, void *MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    m_Form=new CMacroBoxForm(this,(QWidget*)MainWindow);
    CDesktopComponent* d=((CMacroBoxForm*)m_Form)->DesktopComponent;
    AddJack("Out",IJack::Stereo,IJack::Out,jnOut);
    AddJack("Out Left",IJack::Wave,IJack::Out,jnOutLeft);
    AddJack("Out Right",IJack::Wave,IJack::Out,jnOutRight);
    AddJack("In",IJack::Stereo,IJack::In,jnIn);
    AddJack("In Left",IJack::Wave,IJack::In,jnInLeft);
    AddJack("In Right",IJack::Wave,IJack::In,jnInRight);
    d->SetPoly(1);

    WaveOutL=new CInJack("Out","This",IJack::Wave,IJack::In,this);
    JacksCreated.push_back(d->AddJack(WaveOutL,0));
    WaveOutR=new CInJack("Out","This",IJack::Wave,IJack::In,this);
    JacksCreated.push_back(d->AddJack(WaveOutR,1));
    JacksCreated.push_back(d->AddJack(new COutJack("In","This",IJack::Wave,IJack::Out,this,jnInsideInLeft),0));
    JacksCreated.push_back(d->AddJack(new COutJack("In","This",IJack::Wave,IJack::Out,this,jnInsideInRight),1));
}

void CStereoBox::Tick()
{
    ((CMacroBoxForm*)m_Form)->DesktopComponent->Tick();
    m_Process=true;
}

void CStereoBox::HideForm()
{
    ((CMacroBoxForm*)m_Form)->DesktopComponent->HideForms();
    m_Form->setVisible(false);
}

void CStereoBox::Process()
{
    float* B=FetchA(jnIn);
    float* BL=FetchA(jnInLeft);
    float* BR=FetchA(jnInRight);
    if (B == NULL)
    {
        InL=BL;
        InR=BR;
    }
    else if ((BL == NULL) & (BR == NULL))
    {
        InL=B;
        InR=B+m_BufferSize;
    }
    else
    {
        InBuffer.FromBuffer(B);
        InBuffer.AddMono(BL,BR);
        InBuffer.Multiply(SQRT2MULTIPLY);
        InL=InBuffer.Buffer;
        InR=InBuffer.BufferR;
    }
    ((CStereoBuffer*)AudioBuffers[jnOut])->FromMono(WaveOutL->GetNextA(),WaveOutR->GetNextA());
}

float* CStereoBox::GetNextA(const int ProcIndex)
{
    if (m_Process)
    {
        m_Process=false;
        Process();
    }
    if (ProcIndex==jnInsideInLeft) return InL;
    if (ProcIndex==jnInsideInRight) return InR;
    CStereoBuffer* OutBuffer=(CStereoBuffer*)AudioBuffers[jnOut];
    if (ProcIndex==jnOutRight) return OutBuffer->BufferR;
    return OutBuffer->Buffer;
}

void CStereoBox::Play(const bool FromStart)
{
    ((CMacroBoxForm*)m_Form)->DesktopComponent->Play(FromStart);
}

void CStereoBox::Pause()
{
    ((CMacroBoxForm*)m_Form)->DesktopComponent->Pause();
}
