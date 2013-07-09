#include "cadsr.h"

CADSR::CADSR()
{
    HoldTrigger=0;
    m_Presets=CPresets::Presets;
    LastTrigger=0;
    LastGet=0;
    State=esSilent;
    TimeCount=0;
    CurrentVolume=0;
    AP.Attack=0;
    AP.Decay=0;
    AP.Sustain=100;
    AP.Release=0;
    Mode=0;
    CalcParams();
}

float CADSR::GetVol(float Trigger)
{
    if (Trigger != LastTrigger)
    {
        if (Trigger==0)
        {
            LastTrigger = Trigger;
            Finish();
        }
        else
        {
            if (Mode==1)
            {
                if (LastTrigger==0)
                {
                    LastTrigger = Trigger;
                    Start();
                }
            }
            else
            {
                LastTrigger = Trigger;
                Start();
            }
        }
        LastTrigger = Trigger;
    }
    if (State==esPlaying)
    {
        if (TimeCount <= AttackLen)
        {
            CurrentVolume = AttackOffset + (TimeCount * AttackStep);
        }
        if (TimeCount > AttackLen)
        {
            if (TimeCount <= AttackLen + DecayLen)
            {
                CurrentVolume = 1.0 - ((TimeCount - AttackLen) * DecayStep);
            }
        }
        if (TimeCount > AttackLen + DecayLen)
        {
            CurrentVolume = SustainVol;
        }
    }
    else if (State==esReleasing)
    {
        if (TimeCount <= Length + ReleaseLen)
        {
            CurrentVolume = SustainVol - ((TimeCount - Length) * ReleaseStep);
        }
        else
        {
            CurrentVolume = 0;
            State=esSilent;
        }
    }
    LastGet = CurrentVolume;
    TimeCount++;
    return CurrentVolume * VolumeFactor;
}

void CADSR::CalcParams()
{
    if (Mode==0)
    {
        VelocityFactor=LastTrigger;
    }
    else
    {
        VelocityFactor=1;
    }
    VolumeFactor = VelocityFactor;
    AttackLen = TimeConv(AP.Attack);
    DecayLen = TimeConv(AP.Decay);
    ReleaseLen = TimeConv(AP.Release);
    SustainVol = VolConv(AP.Sustain);
    //AttackOffset = LastGet;
    AttackOffset=0;
    if (AttackLen>0)
    {
        AttackStep = (1.0 - AttackOffset) / AttackLen;
    }
    else
    {
        AttackStep = 1.0;
    }
    if (DecayLen>0)
    {
        DecayStep = (1.0 - SustainVol) / DecayLen;
    }
    else
    {
        DecayStep = 1.0 - SustainVol;
    }
}

size_t inline CADSR::TimeConv(size_t mSec)
{
    return (mSec * (m_Presets.SampleRate / m_Presets.ModulationRate)) * 0.001;
}

float inline CADSR::VolConv(float Percent)
{
    return Percent * 0.01;
}

void inline CADSR::Start()
{
    //if (!State==esPlaying)
    //{
        State=esPlaying;
        CalcParams();
        TimeCount = 0;
    //}
}

void inline CADSR::Finish()
{
    //if (State==esPlaying)
    //{
        Length = TimeCount;
        State=esReleasing;
        SustainVol = LastGet;
        if (ReleaseLen > 0)
        {
            ReleaseStep = SustainVol / ReleaseLen;
        }
        else
        {
            ReleaseStep = SustainVol;
        }
    //}
}

//---------------------------------------------------------------------------
const QString CADSR::Save()
{
    QDomLiteElement xml("Custom");
    xml.setAttribute("Attack",QVariant::fromValue(AP.Attack));
    xml.setAttribute("Decay",QVariant::fromValue(AP.Decay));
    xml.setAttribute("Sustain",QVariant::fromValue(AP.Sustain));
    xml.setAttribute("Release",QVariant::fromValue(AP.Release));

    return xml.toString();
}

void CADSR::Load(const QString &XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        AP.Attack=xml.attributeValue("Attack");
        AP.Decay=xml.attributeValue("Decay");
        AP.Sustain=xml.attributeValue("Sustain");
        AP.Release=xml.attributeValue("Release");
    }
    //CalcParams();
}
