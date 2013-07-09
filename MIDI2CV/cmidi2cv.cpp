#include "cmidi2cv.h"

CMIDI2CV::CMIDI2CV()
{
}

void CMIDI2CV::CalcParams()
{
    CVDevice.Tune=m_ParameterValues[pnTune]*0.01;
    CVDevice.setTranspose(m_ParameterValues[pnTranspose]);
    CVDevice.setChannel(m_ParameterValues[pnMIDIChannel]);
}

void CMIDI2CV::Init(const int Index,void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    for (int i=0;i<8;i++)
    {
        AddJack("Frequency Out " + QString::number(i+1),IJack::Frequency,IJack::Out,jnFrequency+i);
    }
    for (int i=0;i<8;i++)
    {
        AddJack("Velocity Out " + QString::number(i+1),IJack::Amplitude,IJack::Out,jnVelocity+i);
    }
    AddJack("MIDI In",IJack::MIDI,IJack::In,0);
    AddParameter(ParameterType::SelectBox,"MIDI Channel","",0,16,0,"All§1§2§3§4§5§6§7§8§9§10§11§12§13§14§15§16",0);
    AddParameter(ParameterType::Numeric,"Transpose","Half tones",-24,24,0,"",0);
    AddParameter(ParameterType::Numeric,"Tune","Hz",43600,44800,100,"",44000);
    CalcParams();
}
const float CMIDI2CV::GetNext(const int ProcIndex)
{
    if (m_Process)
    {
        m_Process=false;
        CVDevice.parseMIDI((CMIDIBuffer*)FetchP(jnIn));
    }
    if ((ProcIndex>=jnFrequency) & (ProcIndex<=jnFrequency+7))
    {
        return CVDevice.Notes[ProcIndex-jnFrequency].Frequency*CVDevice.getPitchbend(ProcIndex-jnFrequency);
    }
    else if ((ProcIndex>=jnVelocity) & (ProcIndex<=jnVelocity+7))
    {
        return (float)CVDevice.Notes[ProcIndex-jnVelocity].Velocity*CVDevice.Vol(ProcIndex-jnVelocity);
    }
    return 0;
}
void CMIDI2CV::Play(const bool FromStart)
{
    if (FromStart)
    {
        CVDevice.reset();
        CalcParams();
    }
}
void CMIDI2CV::Pause()
{
    CVDevice.allNotesOff();
    CalcParams();
}
