#ifndef CFREQGLIDER_H
#define CFREQGLIDER_H

#include "softsynthsclasses.h"

class CFreqGlider
{
private:
    int LastGlideFreq;
    float GlideFactor;
    int ResponseFactor;
    int CurrentGlide;
    float TargetFreq;
    int CurrentSpeed;
public:
    CFreqGlider();
    void SetGlide(const int Glide);
    void SetTargetFreq(const float Freq);
    const float GetCurrentFreq(void);
    void SetSpeed(const int Speed);
};

#endif // CFREQGLIDER_H
