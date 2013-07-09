#include "cfreqglider.h"

CFreqGlider::CFreqGlider()
{
    ResponseFactor=CPresets::Presets.ResponseFactor;
    LastGlideFreq=0;
    CurrentGlide=0;
    SetSpeed(1);
    TargetFreq=0;
}

void CFreqGlider::SetSpeed(const int Speed)
{
    CurrentSpeed=Speed;
    SetGlide(CurrentGlide);
}

void CFreqGlider::SetGlide(const int Glide)
{
    CurrentGlide=Glide;
    GlideFactor=(101-CurrentGlide)*((float)ResponseFactor/(float)CurrentSpeed);
}

void CFreqGlider::SetTargetFreq(const float Freq)
{
    TargetFreq=Freq;
}

const float CFreqGlider::GetCurrentFreq(void)
{
    float CurrentFreq;
    if (CurrentGlide)
    {
        float GF;
        if (LastGlideFreq>TargetFreq)
        {
            GF=GlideFactor;
        }
        else
        {
            GF=-GlideFactor;
        }
        if (fabs(LastGlideFreq - TargetFreq)>fabs(GF))
        {
            CurrentFreq=LastGlideFreq-GF;
        }
        else
        {
            CurrentFreq=TargetFreq;
        }
    }
    else
    {
        CurrentFreq=TargetFreq;
    }
    LastGlideFreq=CurrentFreq;
    return CurrentFreq;
}
