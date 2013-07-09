#include "cwaverecorder.h"
#include "cwaverecorderform.h"

CWaveRecorder::CWaveRecorder()
{
}

void CWaveRecorder::Init(const int Index,void* MainWindow)
{
     m_Name=devicename;
     IDevice::Init(Index,MainWindow);
     AddJack("Out",IJack::Stereo,IJack::Out,jnOut);
     AddJack("In",IJack::Wave,IJack::In,jnIn);
     m_Form=new CWaveRecorderForm(this,(QWidget*)MainWindow);
     Playing=false;
}

void CWaveRecorder::Tick()
{
    if (Playing)
    {
        CStereoBuffer* OutBuffer=(CStereoBuffer*)AudioBuffers[jnOut];
        OutBuffer->ZeroBuffer();
        ((CWaveRecorderForm*)m_Form)->ModifyBuffers(OutBuffer->Buffer,OutBuffer->BufferR);
    }
}

float* CWaveRecorder::GetNextA(const int ProcIndex)
{
    if (!Playing)
    {
        return NULL;
    }
    return AudioBuffers[ProcIndex]->Buffer;
}

void CWaveRecorder::Play(const bool FromStart)
{
    Playing=true;
    if (FromStart)
    {
        ((CWaveRecorderForm*)m_Form)->Reset();
    }
}

void CWaveRecorder::Pause()
{
    Playing=false;
    ((CWaveRecorderForm*)m_Form)->Stop();
}
