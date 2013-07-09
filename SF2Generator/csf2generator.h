#ifndef CSF2GENERATOR_H
#define CSF2GENERATOR_H

#include "csf2file.h"

class CSF2Generator
{

private:

    bool PlayEnd;
    bool Ready;
    std::vector<OscType*> Osc;
    unsigned short OscCount;
    CSF2File* SFFile;
    float* AudioL;
    float* AudioR;
    unsigned int m_ModulationRate;
    int pitchWheel;
    int transpose;
public:
    CSF2Generator();
    ~CSF2Generator();
    bool FinishedPlaying;
    short MidiBank;
    short MidiPreset;
    short ID;
    short Channel;
    bool LoadFile(const QString& Path);
    void ResetSample(const short MidiNote, const short MidiVelo);
    void EndSample(void);
    float* GetNext(void);
    short PresetCount(void);
    void setPitchWheel(int cent);
    void addTranspose(int steps);
    void setAftertouch(short value);
    void resetTranspose();
    SFPRESETHDRPTR Preset(short Index);
protected:
    static QMap<QString, CSF2File*> SF2Files;
};


#endif // CSF2GENERATOR_H
