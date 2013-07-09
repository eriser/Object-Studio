#include "csf2player.h"
#include "csf2playerform.h"

#define devicename "SF2Player"

CSF2Player::CSF2Player()
{
}

void CSF2Player::Init(const int Index,void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("MIDI In",IJack::MIDI,IJack::In,jnIn);
    AddJack("Out",IJack::Stereo,IJack::Out,jnOut);
    AddParameter(ParameterType::SelectBox,"MIDI Channel","",0,16,0,"All§1§2§3§4§5§6§7§8§9§10§11§12§13§14§15§16",0);
    AddParameter(ParameterType::dB,"Volume","dB",0,200,0,"",100);
    AddParameter(ParameterType::Numeric,"Transpose","Semitones",-24,24,0,"",0);
    AddParameter(ParameterType::SelectBox,"Patch Change","",0,1,0,"Off§On",0);
    LastTrigger=0;
    LastFreq=0;
    VolumeFactor=1.0*(1.0/sqrtf(SF2Device::sf2voices));
    CalcParams();
    SF2Device.reset();
    m_Form=new CSF2PlayerForm(this,(QWidget*)MainWindow);
}

void CSF2Player::Process()
{
    SF2Device.parseMIDI((CMIDIBuffer*)FetchP(jnIn));
    bool First=true;
    CStereoBuffer* OutBuffer=(CStereoBuffer*)AudioBuffers[jnOut];
    for (int i1=0;i1<SF2Device.voiceCount();i1++)
    {
        float* BufferL=SF2Device.getNext(i1);
        if (BufferL)
        {
            float volL=VolumeFactor*SF2Device.volL(SF2Device.voiceChannel(i1));
            float volR=VolumeFactor*SF2Device.volR(SF2Device.voiceChannel(i1));
            if (First)
            {
                First=false;
                OutBuffer->FromBuffer(BufferL,volL,volR);
            }
            else
            {
                OutBuffer->AddBuffer(BufferL,volL,volR);
            }
        }
    }
    if (First) OutBuffer->ZeroBuffer();
}

void CSF2Player::Pause()
{
    SF2Device.allNotesOff();
    //Reset();
}

void CSF2Player::Play(const bool FromStart)
{
    if (FromStart) SF2Device.reset();
}

void inline CSF2Player::CalcParams()
{
    VolumeFactor=(float)m_ParameterValues[pnVolume]*0.01*(1.0/sqrtf(SF2Device::sf2voices));
    SF2Device.patchResponse=m_ParameterValues[pnPatchChange];
    SF2Device.setTranspose(m_ParameterValues[pnTranspose]);
    SF2Device.setChannel(m_ParameterValues[pnMIDIChannel]);
}

void CSF2Player::SetFilename(const QString &FileName)
{
    m_FileName=FileName;
}
