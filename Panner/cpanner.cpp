#include "cpanner.h"

CPanner::CPanner()
{
}

void CPanner::Init(const int Index, void *MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("In",IJack::Wave,IJack::In,jnIn);
    AddJack("Out",IJack::Stereo,IJack::Out,jnOut);
    AddJack("Out Left",IJack::Wave,IJack::Out,jnOutLeft);
    AddJack("Out Right",IJack::Wave,IJack::Out,jnOutRight);
    AddJack("Modulation In",(IJack::AttachModes)(IJack::Pitch | IJack::Amplitude),IJack::In,jnModulation);
    AddParameter(ParameterType::Numeric,"Pan","%",-100,100,0,"",0);
    AddParameter(ParameterType::Numeric,"Modulation","%",0,100,0,"",0);
    LastMod=0;
    CurrentMod=0;
    LeftModFactor=1;
    RightModFactor=1;
    InSignal=NULL;
    CalcParams();
}

void CPanner::Process()
{
    InSignal=FetchA(jnIn);
    if (!InSignal) return;
    float lTemp= Fetch(jnModulation);
    if (lTemp != LastMod)
    {
        LastMod = lTemp;
        CurrentMod = lTemp* ModFactor;
        if (CurrentMod==0)
        {
            LeftModFactor=1;
            RightModFactor=1;
        }
        else if (CurrentMod>0)
        {
            LeftModFactor=1-CurrentMod;
            RightModFactor=1;
        }
        else
        {
            RightModFactor=1+CurrentMod;
            LeftModFactor=1;
        }
    }
    CStereoBuffer* OutBuffer=(CStereoBuffer*)AudioBuffers[jnOut];
    OutBuffer->FromMono(InSignal);
    OutBuffer->Multiply(LeftFactor*LeftModFactor,RightFactor*RightModFactor);
}

float *CPanner::GetNextA(const int ProcIndex)
{
    if (m_Process)
    {
        m_Process=false;
        Process();
    }
    if (!InSignal) return NULL;
    CStereoBuffer* OutBuffer=(CStereoBuffer*)AudioBuffers[jnOut];
    if (ProcIndex==jnOutRight) return OutBuffer->BufferR;
    return OutBuffer->Buffer;
}

void CPanner::CalcParams()
{
    if (m_ParameterValues[pnPan]==0)
    {
        LeftFactor=1;
        RightFactor=1;
    }
    else if (m_ParameterValues[pnPan]>0)
    {
        LeftFactor=(100-m_ParameterValues[pnPan])*0.01;
        RightFactor=1;
    }
    else
    {
        RightFactor=(100+m_ParameterValues[pnPan])*0.01;
        LeftFactor=1;
    }
    ModFactor = m_ParameterValues[pnModulation] * 0.01;
}
