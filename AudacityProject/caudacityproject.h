#ifndef CAUDACITYPROJECT_H
#define CAUDACITYPROJECT_H

#include "softsynthsclasses.h"
#include "cwavegenerator.h"

class AudacityBlock
{
private:
    int Channel;
    int Buffersize;
    unsigned int AliasStart;
    int Rate;
    CWaveGenerator wa;
public:
    unsigned int Start;
    AudacityBlock();
    float* GetNext();
    void Reset();
    bool Init(QString Filename, unsigned int StartPtr, int Channels, unsigned int AliasPointer,int RateOverride);
};

class AudacityClip
{
private:
    unsigned int Counter;
    std::vector<AudacityBlock*> Blocks;
    float* Buffer;
    float* auBuffer;
    int Buffersize;
    unsigned int BlockIndex;
    int BufferPointer;
public:
    AudacityClip();
    ~AudacityClip();
    float Offset;
    void Reset();
    void AddBlock(QString Filename,unsigned int Start,int RateOverride);
    void AddAliasBlock(QString Filename,unsigned int Start,int Channel,unsigned int AliasStart);
    void LoadClip(const QString& XML,QString ProjectPath,int RateOverride);
    void LoadSequence(const QString& XML,QString ProjectPath,int RateOverride);
    float* GetNext();
};

class AudacityTrack
{
private:
    float Time;
    std::vector<AudacityClip*> Clips;
    float TimeFactor;
    int Buffersize;
public:
    QString Name;
    int Channel;
    bool Linked;
    float Offset;
    bool Mute;
    bool Solo;
    float Rate;
    float Gain;
    float Pan;
    float TimeAdd;
    unsigned int ClipIndex;
    bool Playing;
    float FactorL;
    float FactorR;
    AudacityTrack();
    ~AudacityTrack();
    void Reset();
    void Load(const QString& XML,QString ProjectPath);
    float* GetNext();
};

class CAudacityProject : public IDevice
{
private:
    enum JackNames
    {jnOut};
    enum ParameterNames
    {pnVolume};
    float ModFactor;
    float TimeAdd;
    float Time;
    std::vector<AudacityTrack*> Tracks;
    bool Playing;
    bool Loading;
    void inline CalcParams();
    void inline LoadProject(const QString& ProjectFile);
    void Process();
public:
    CAudacityProject();
    ~CAudacityProject();
    void Execute(const bool Show);
    const QString Save();
    void Load(const QString& XML);
    void Init(const int Index,void* MainWindow);
    void Play(const bool FromStart);
    void Pause();
    float* GetNextA(const int ProcIndex);
};

#endif // CAUDACITYPROJECT_H
