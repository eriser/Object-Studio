#ifndef CMIDIFILE2WAVE_H
#define CMIDIFILE2WAVE_H

#include <QWidget>
#include <softsynthsclasses.h>
#include <cmidifileplayer.h>
#include <csf2player.h>
#include <cvsthost.h>
#include <cstereomixer.h>
#include <cmixerwidget.h>
#include <cmasterwidget.h>
#include <QGridLayout>

class CDeviceListLite
{
public:
    CDeviceListLite();
    void Play(const bool FromStart);
    void Pause();
    void Tick();
    void DisconnectAll();
    void Clear();
    void AddJacks(IDevice* device);
    void ConnectJacks(QString InJack, QString OutJack);
    void DisconnectDevice(IDevice* device);
    void AddDevice(IDevice* device, int index, void* MainWindow);
    void RemoveDevice(IDevice* device);
    QList<IDevice*> Devices;
    QHash<QString,IJack*> Jacks;
};

class CMIDI2WavForm : public CSoftSynthsForm
{
    Q_OBJECT
public:
    explicit CMIDI2WavForm(IDevice* Device, QWidget *parent = 0);
    ~CMIDI2WavForm();
    /*
    const QString Save(){}
    void Load(const QString& XML){}
    void CustomLoad(const QString &XML){}
    const QString CustomSave(){}
    */
    CMixerWidget* MW;
};

namespace MIDIFile2Wave
{
const int effectCount=3;
}

class CMIDIFile2Wave : public IDevice
{
public:
    CMIDIFile2Wave();
    ~CMIDIFile2Wave();
    void Tick();
    void Play(const bool FromStart);
    void Pause();
    float* GetNextA(const int ProcIndex);
    void Init(const int Index, void* MainWindow);
    void Load(const QString &XML);
    const QString Save();
    void Execute(const bool Show);
    void HideForm();
    void Skip(unsigned long MilliSeconds);
    bool IsPlaying();
    unsigned long CurrentTick();
    unsigned long Duration();
    unsigned long CurrentmSec();
    unsigned long MilliSeconds();
    void clear();
    bool isEmpty();
    bool isVisible();
    void OpenPtr(const char* Pnt, int Length);
    void Center();
    void SetTitle(const QString& t);
    CMixerWidget* MW;
private:
    enum JackNames
    {jnOut};
    CMIDIFileReader MFR;
    QList<IDevice*> Effects;
    QList<IDevice*> MFPs;
    QList<IDevice*> SF2s;
    void LoadFile(QString filename);
    void LoadEffect(int index, QString filename);
    CStereoMixer* Mx;
    CDeviceListLite DeviceList;
};

#endif // CMIDIFILE2WAVE_H
