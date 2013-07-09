#include "ijack.h"
#include "cpresets.h"

/*
int ModulationCounter=0;

bool ModulationZero()
{
    return (ModulationCounter==0);
}
*/

IJack::IJack(const QString& sName,const QString& sOwner,AttachModes tAttachMode,Directions tDirection,IDeviceBase* OwnerClass)
    : IJackBase(tAttachMode,tDirection), m_BufferSize(CPresets::Presets.BufferSize), Name(sName), Owner(sOwner), m_OwnerClass(OwnerClass), AudioBuffer(NULL)
{
    qDebug() << "Jack Created " << sOwner << sName;
    if (AttachMode==Wave)
    {
        AudioBuffer=new CMonoBuffer;
        qDebug() << "MonoBuffer Created" << sName << sOwner;
    }
    if (AttachMode==Stereo)
    {
        AudioBuffer=new CStereoBuffer;
        qDebug() << "StereoBuffer Created" << sName << sOwner;
    }
}

IJack::~IJack()
{
    if (AttachMode & Audio)
    {
        delete AudioBuffer;
        qDebug() << "AudioBuffer Deleted" << Name << Owner;
    }
    m_OwnerClass=NULL;
    qDebug() << "Jack Deleted " << Owner << Name;
}

CInJack::~CInJack()
{
    if (m_MIDIBuffer != NULL) delete m_MIDIBuffer;
}

void* CInJack::GetNextP()
{
    //if (ModulationZero())
    //{
        if (m_OutJackCount==1) return FetchP(0,this);
        else if (m_OutJackCount==0) return NULL;
        else
        {
            std::vector<CMIDIBuffer*> MIDIBuffers;
            for (int lTemp=0;lTemp<m_OutJackCount;lTemp++)
            {
                CMIDIBuffer* MB=(CMIDIBuffer*)FetchP(lTemp, this);
                if (MB != NULL)
                {
                    if (!MB->IsEmpty()) MIDIBuffers.push_back(MB);
                }
            }
            if (MIDIBuffers.size()==0) return NULL;
            else if (MIDIBuffers.size()==1) return (void*)MIDIBuffers[0];
            else
            {
                if (m_MIDIBuffer == NULL) m_MIDIBuffer=new CMIDIBuffer;
                m_MIDIBuffer->Reset();
                foreach (CMIDIBuffer* MB, MIDIBuffers) m_MIDIBuffer->Append(MB);
                return (void*)m_MIDIBuffer;
            }
        }
    //}
    return NULL;
}

const float CInJack::GetNext()
{
    if (m_OutJackCount==0) return 0;
    if (m_OutJackCount==1)
    {
        LastGetNext=Fetch(0, this);
        return LastGetNext;
    }
    float TempPitch=0;
    int PitchCount=0;
    float TempAmp=0;
    int AmpCount=0;
    float TempFreq=0;
    int FreqCount=0;
    float GetAmp;
    float GetPitch=0;
    foreach (void* j,m_OutJacks)
    {
        COutJack* OJ=(COutJack*)j;
        switch (OJ->AttachMode)
        {
        case Amplitude:
            AmpCount++;
            TempAmp=TempAmp+OJ->GetNext(this);
            break;
        case Pitch:
            PitchCount++;
            TempPitch=TempPitch+OJ->GetNext(this);
            break;
        case Frequency:
            FreqCount++;
            TempFreq=TempFreq+OJ->GetNext(this);
            break;
        default:
            break;
        }
    }
    switch (AttachMode)
    {
    case Amplitude:
        if (AmpCount>0) LastGetNext=TempAmp/AmpCount;
        break;
    case Pitch:
        if (PitchCount>0) LastGetNext=TempPitch/PitchCount;
        break;
    case Frequency:
        if (FreqCount>0) LastGetNext=TempFreq/FreqCount;
        break;
    case (Amplitude | Pitch):
        if (PitchCount>0) GetPitch=-TempPitch/PitchCount;
        if (AmpCount>0)
        {
            GetAmp=TempAmp/AmpCount;
            if (PitchCount>0) LastGetNext=(GetPitch+GetAmp)*0.5;
            else LastGetNext=GetAmp;
        }
        else if (PitchCount>0) LastGetNext=GetPitch;
        break;
    default:
        break;
    }
    return LastGetNext;
}

void CInJack::ConnectToOut(COutJack* OutJack)
{
    if (OutJack!=NULL)
    {
        if (ConnectionState(OutJack)) return;
        m_OutJacks.push_back(OutJack);
        m_OutJackCount++;
        if (m_OutJackCount > 1) MixFactor=1.0/sqrtf(m_OutJackCount);
        else MixFactor=1;
        OutJack->Connect();
    }
}

void CInJack::DisconnectFromOut(COutJack* OutJack)
{
    int Index=m_OutJacks.IndexOf(OutJack);
    if (Index>-1)
    {
        OutJack->Disconnect();
        m_OutJacks.Remove(Index);
        m_OutJackCount--;
        if (m_OutJackCount > 1) MixFactor=1.0/sqrtf(m_OutJackCount);
        else MixFactor=1;
    }
}
