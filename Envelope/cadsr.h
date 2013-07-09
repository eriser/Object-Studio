#ifndef CADSR_H
#define CADSR_H

#include <QtCore>
#include "softsynthsclasses.h"

class CADSR
{
public:
    enum EnvelopeStates
    {
        esSilent,
        esPlaying,
        esReleasing
    };
    struct ADSRParams
    {
        size_t Attack;
        size_t Decay;
        size_t Sustain;
        size_t Release;
    };
    CADSR();
    float GetVol(float Trigger);
    const QString Save();
    void Load(const QString& XML);
    ADSRParams AP;
    int Mode;
    float HoldTrigger;
    EnvelopeStates State;
    void CalcParams();
private:
    float AttackStep;
    float DecayStep;
    float ReleaseStep;
    size_t AttackLen;
    size_t DecayLen;
    size_t ReleaseLen;
    float SustainVol;
    size_t Length;
    float LastGet;
    float AttackOffset;
    float LastTrigger;
    size_t TimeCount;
    float CurrentVolume;
    float VolumeFactor;
    float VelocityFactor;
    size_t inline TimeConv(size_t mSec);
    float inline VolConv(float Percent);
    void inline Start();
    void inline Finish();
    PresetsType m_Presets;
};

#endif // CADSR_H
