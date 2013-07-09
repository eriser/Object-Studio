#include "cscope.h"
#include "cscopeform.h"

CScope::CScope()
{
}

CScope::~CScope()
{
    if (m_Initialized)
    {
        for (int i=0;i<MaxBuffers;i++)
        {
            delete[] m_Buffer[i];
        }
    }
}

void CScope::DrawBuffers()
{
    if (m_Form)
    {
        if (m_Form->isVisible())
        {
            for (int i=0;i<CurrentBuffer;i++)
            {
                if (((CScopeForm*)m_Form)->Tab->currentIndex()==0)
                {
                    ((CScopeForm*)m_Form)->Scope->Process(PlotBuffer[i]);
                }
                else
                {
                    ((CScopeForm*)m_Form)->Spectrum->Process(PlotBuffer[i]);
                }

            }
            CurrentBuffer=0;
            for (int i=0;i<NullBuffers;i++)
            {
                if (((CScopeForm*)m_Form)->Tab->currentIndex()==0)
                {
                    ((CScopeForm*)m_Form)->Scope->Process(NULL);
                }
                else
                {
                    ((CScopeForm*)m_Form)->Spectrum->Process(NULL);
                }
            }
            NullBuffers=0;
        }
    }
}

void CScope::Init(const int Index,void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    for (int i=0;i<MaxBuffers;i++)
    {
        m_Buffer[i]=new float[m_BufferSize];
    }
    CurrentBuffer=0;
    NullBuffers=0;
    m_Form=new CScopeForm(this,(QWidget*)MainWindow);
    //m_Form=new TfrmScope(CallingApp,this,AnsiString(m_Name) + " " + AnsiString(Index));
    AddJack("In",IJack::Wave,IJack::In,0);
    AddParameter(ParameterType::dB,"Input Volume","dB",0,200,0,"",100);
    AddParameter(ParameterType::Numeric,"Scope Frequency","Hz",100,100000,100,"",44000);
    AddParameter(ParameterType::SelectBox,"Spectrum Mode","",0,2,0,"Circular§Continuous§Diagram",0);
    Reset();
    CalcParams();
}

void CScope::Tick()
{
    if (m_Form->isVisible())
    {

        float* Signal=FetchA(jnIn);
        if (CurrentBuffer<MaxBuffers)
        {
            if (!Signal)
            {
                PlotBuffer[CurrentBuffer]=NULL;
            }
            else
            {
                CopyMemory(m_Buffer[CurrentBuffer],Signal,m_BufferSize*sizeof(float));
                PlotBuffer[CurrentBuffer]=m_Buffer[CurrentBuffer];
            }
            CurrentBuffer++;
        }
        else
        {
            NullBuffers++;
        }
        if (CurrentBuffer>=MaxBuffers)
        {
            DrawBuffers();
        }
    }
}

void inline CScope::CalcParams()
{
    ((CScopeForm*)m_Form)->Scope->SetFreq(m_ParameterValues[pnFrequency]*0.01,m_BufferSize);
    ((CScopeForm*)m_Form)->Scope->SetVol(m_ParameterValues[pnVolume]);
    ((CScopeForm*)m_Form)->Spectrum->SetVol(m_ParameterValues[pnVolume]);
    ((CScopeForm*)m_Form)->Spectrum->SetMode(m_ParameterValues[pnMode]);
    /*
    ((TfrmScope*)m_Form)->TScopeComponent1->SetFreq(m_ParameterValues[pnFrequency]*0.01,m_BufferSize);
    ((TfrmScope*)m_Form)->TScopeComponent1->SetVol(m_ParameterValues[pnVolume]);
    */
}

void CScope::Reset()
{
}

