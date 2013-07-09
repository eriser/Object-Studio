#ifndef CSF2PLAYER_H
#define CSF2PLAYER_H

#include "softsynthsclasses.h"
#include "csf2device.h"

class CSF2Player : public IDevice
{
public:
    CSF2Player();
    void Init(const int Index,void* MainWindow);
    void Pause();
    void Play(const bool FromStart);
    void SetFilename(const QString& FileName);
    CSF2Device SF2Device;
private:
    enum JackNames
    {jnIn,jnOut};
    enum ParameterNames
    {pnMIDIChannel,pnVolume,pnTranspose,pnPatchChange};
    int LastTrigger;
    float LastFreq;
    float VolumeFactor;
    void Process();
    void inline CalcParams();
    bool inline matchChannel(int message,int command);
};

#endif // CSF2PLAYER_H
