#ifndef CSF2DEVICE_H
#define CSF2DEVICE_H

#include "csf2generator.h"
#include "csounddevice.h"

namespace SF2Device
{
const int sf2voices=16;
}

class CSF2Device : public ISoundDevice
{
public:
    struct SF2Preset
    {
        int number;
        QString name;
    };
    struct SF2Bank
    {
        QHash<int,SF2Preset> presets;
    };
    CSF2Device();
    ~CSF2Device();
    void NoteOn(const short channel, const short pitch, const short velocity);
    void NoteOff(const short channel, const short pitch);
    void Aftertouch(const short channel, const short pitch, const short value);
    void Patch(const short channel, const short value);
    void Controller(const short channel, const short controller, const short value);
    float* getNext(const int voice);
    const short voiceChannel(const int voice);
    const int voiceCount();
    void reset();
    const int presetcount();
    const QString presetname(const int preset);
    const int banknumber(const int preset);
    const int presetnumber(const int preset);
    const int currentPreset(const short channel);
    const int currentBank(const short channel);
    void setBank(const int bank);
    void setPreset(const int preset);
    const bool loadFile(const QString& filename);
    bool patchResponse;
    void allNotesOff();
    QHash<int,SF2Bank> banks;
private:
    short lastChannel;
    bool isGM;
    int drumBank;
    CSF2Generator SF2Generator[SF2Device::sf2voices];
    bool loaded;
};


#endif // CSF2DEVICE_H
