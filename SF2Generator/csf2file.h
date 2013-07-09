#ifndef CSF2FILE_H
#define CSF2FILE_H

#include "softsynthsdefines.h"
#include "enabler/enab.h"
#include "cwavebank.h"
#include <QtCore>
#include <QByteArray>

namespace SF2File
{
const QString SF2Filter("Sound font files (*.sf2)");
}

class CSF2File
{
public:
    CSF2File();
    ~CSF2File();
    bool Open(const QString& filePath);
    QString Path;
    int ReferenceCount;
    short BankID;
    unsigned int Offset;
    BYTE* SFData;
    size_t Size;
    const short ReadMem(const size_t& Pos);
    short* Mem;
};

class SFEnvelope
{
public:
    enum SFEnvelopeValues
    {
        evSilent,evDelay,evAttack,evHold,evDecay,evSustain,evRelease
    };
    void Init(const int Delay, const int Attack, const int Hold, const int Decay, const int Sustain, const int Release, const int AutoHold, const int AutoRelease, const int KeyNum, const float RateFactor);
    const float inline GetNext(void);
    void Start(void);
    void End(void);
    bool Silent;
private:
    int mDelay;
    int mAttack;
    int mHold;
    int mDecay;
    float mSustain;
    int mRelease;
    int mAutoHold;
    int mAutoRelease;
    SFEnvelopeValues CurrentAction;
    float AttackStep;
    float DecayStep;
    float ReleaseStep;
    float CurrentVol;
    long Counter;
    long TimeCount;
};

class SFLFO
{
private:
    int mDelay;
    long Counter;
    CWaveBank LFO;
    SFEnvelope::SFEnvelopeValues CurrentAction;
    float WavePosition;
    float FreqValue;
    int SampleRate;

public:
    void Init(const int Freq, const int Delay, const int KeyNum, const float RateFactor);
    SFLFO();
    ~SFLFO();
    const float inline GetNext(void);
    void Start(void);
};

class SFFilter
{
private:
    int Maxcutoff;
    float FiltCoefTab0;
    float FiltCoefTab1;
    float FiltCoefTab2;
    float FiltCoefTab3;
    float FiltCoefTab4;
    float ly1;
    float ly2;
    float lx1;
    float lx2;
    float m_ExpResonance;
    //int LastResonance;
    int LastCO;
    int TargetFreq;
    float MixFactor;
    //float ModulationFactor;
    //float InVolumeFactor;
    int CurrentCutOff;
    int CurrentHiQ;
    float CurrentAmount;
    int HalfRate;
    //void inline CalcParams();
    void inline CalcExpResonance(void);
public:
    void Init(const int CutOff, const int HiQ);
    const float inline GetNext(const float Signal);
    void inline SetAmount(const float Amount);
};

class OscType
{
private:
    //long ModCount;
    float EnvMod;
    float EnvVol;
    float PosRate;
    float VolumeFactor;
    float pitchWheel;
    int transpose;
    float Aftertouch;
    SFEnvelope VolEnv;
    SFEnvelope ModEnv;
    SFLFO ModLFO;
    SFLFO VibLFO;
    SFFilter Filter;
    sfData OscData;
    CSF2File* m_SFFile;
public:
    enum StereoType
    {
        Mono,StereoL,StereoR
    };
    float LeftPan;
    float RightPan;
    int RelEndLoop;
    int LoopSize;
    int RealStart;
    int SampleSize;
    short SampleMode;
    //bool Valid;
    bool Silent;
    float Position;
    StereoType Stereo;
    void Init(sfData* Data,CSF2File* SFFile, const short MidiNote, const short MidiVelo);
    void setPitchWheel(const float cent);
    void setTranspose(const int steps);
    void setAftertouch(const float value);
    OscType();
    ~OscType();
    void Modulate(void);
    const float UpdatePos(void);
    const unsigned int RealPos(void);
    void Loop();
    const bool NoLoop();
    void Start(void);
    void End(void);
};

#endif // CSF2FILE_H
