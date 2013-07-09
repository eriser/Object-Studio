#ifndef SOFTSYNTHSDEFINES_H
#define SOFTSYNTHSDEFINES_H

#include <qmath.h>

typedef unsigned char BYTE;

const int MAXCHAR=127;

const int MAXSHORT=32767;

const int MAXINT=2147483647;

const float MAXCHARMULTIPLY = 0.00787401574;//1.0/MAXCHAR

const float MAXSHORTMULTIPLY = 0.0000305185094759972;//MAXSHORTMULTIPLY 1.0/MAXSHORT

const float MAXINTMULTIPLY=4.6566129e-10;

const int FreqResolution=100000;

const float FreqResolutionMultiply=0.00001;//1/FreqResolution

const double DoublePi=3.1415926535897932384626433832795;

const float SQRT2MULTIPLY=1.0f / sqrt(2.0f); //1.414213562373095f

const float LOG2MULTIPLY=1.0f / logf(2.0f);

const float LOG10MULTIPLY=1.0f / logf(10.0f);

const float pitchBendFactor=1.0f / (0x2000/200.0f);

const float inline MIDItoFreq(const BYTE keynum ,const float A440)
{
    return A440 * powf(2.0f, (keynum - 69) / 12.0f);
}

const int inline FreqtoMIDI(const float freq, const float A440)
{
    return floorf((12*log2f(freq/A440)) + 0.5f)+69;
}

void inline ZeroMemory(void* mem, const size_t size)
{
    memset(mem,0,size);
}

void inline CopyMemory(const void* dest, const void* source, const size_t size)
{
    memcpy((void*)dest,source,size);
}

const inline unsigned short from14bit(const BYTE lsb, const BYTE msb)
{
   unsigned short _14bit;
   _14bit = (unsigned short)msb;
   _14bit <<= 7;
   _14bit |= (unsigned short)lsb;
   return(_14bit);
}

const float inline Cent_to_Percent(const float Cent)
{
    static float LastPercentReturn=1;
    static float LastCent=0;
    if (Cent==LastCent) return LastPercentReturn;
    if (Cent==0) LastPercentReturn=1;
    else LastPercentReturn=powf(2.0f,Cent/1200.0f);
    LastCent=Cent;
    return LastPercentReturn;
}
/*
 * convert timecents to msec
 */
const int inline awe_timecent_to_msec(const float timecent)
{
    return 1000 * powf(2.0f, timecent / 1200.0f);
}
/*
 * convert msec to timecents
 */
const int inline awe_msec_to_timecent(float msec)
{
    if (msec <= 0) msec = 1;
    return logf(msec / 1000.0f) * LOG2MULTIPLY * 1200.0f;
}

const float inline cB_to_Percent(const float cB)
{
    if (cB>=960) return 0;
    if (cB==0) return 1;
    return powf(10.0f,cB/-200.0f);
}

const float inline calc_filterQ_adip(const int gain_cB)
{
    float Q = (gain_cB * 2) / 30;
    if (Q < 0)
        Q = 0;
    else if (Q > 15)
        Q = 15;
    return Q/15.0f;
}

const bool inline descriptorMatch(const void *descriptor, const void *s)
{
    return (memcmp(descriptor,s,4)==0);
}

void inline setDescriptor(const void* descriptor, const void* s)
{
    CopyMemory(descriptor,s,4);
}

const float inline db2lin(const float x)
{
    if (x > -96.0f) return powf(10.0f, x * 0.05f);
    return 0;
}

const float inline lin2db(const float x)
{
    if (x > 0) return 20.0f*(logf(x)*LOG10MULTIPLY);
    return -96;
}

#endif // SOFTSYNTHSDEFINES_H
