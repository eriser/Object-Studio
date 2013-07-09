#ifndef IDEVICE_H
#define IDEVICE_H

#include "ijack.h"
#include "csoftsynthsform.h"
#include "ihost.h"
#include "cpresets.h"

namespace ParameterList
{
const QString ParameterListSeparator("§");
}

struct ParameterType
{
    enum ParameterTypes
    {Numeric,SelectBox,dB};
    int Min;
    int Max;
    ParameterTypes Type;
    QString Name;
    QString List;
    QString Unit;
    int DecimalFactor;
};

class IDevice : public IDeviceBase
{
public:
    IDevice() : m_Initialized(false), m_DeviceID("No ID"), m_Name("Generic Device Class"), m_ParameterCount(0),
        m_BufferSize(CPresets::Presets.ModulationRate), m_Host(NULL), m_Presets(PresetsType(CPresets::Presets)), m_Form(NULL) {}
    virtual ~IDevice();
    inline const ParameterType Parameter(const int Index) { return m_Parameters[Index]; }
    inline const int GetParameterValue(const int Index) { return m_ParameterValues[Index]; }
    inline void SetParameterValue(const int Index, const int Value)
    {
        if (m_ParameterValues[Index]!=Value)
        {
            m_ParameterValues[Index]=Value;
            CalcParams();
        }
    }
    inline const int ParameterCount(void) { return m_ParameterCount; }
    void SetHost(IHost* Host) { m_Host=Host; }
    const QString OpenFile(const QString& Filter);
    virtual void RaiseForm() {
        if (m_Form != NULL)
        {
            if (m_Form->isVisible())
            {
                m_Form->raise();
                m_Form->activateWindow();
            }
        }
    }
    virtual void HideForm() { if (m_Form != NULL) m_Form->setVisible(false); }
    IJack* GetJack(const int Index) { return (IJack*)m_Jacks[Index]; }
    const int JackCount(void) { return m_Jacks.size(); }
    const int Index(void) { return m_Index; }
    const QString Name(void) { return m_Name; }
    const QString Picture(void) { return QString(); }
    virtual const QString FileName(void) { return m_FileName; }
    const QString DeviceID(void) { return m_DeviceID; }

    virtual float* GetNextA(const int ProcIndex) {
        if (m_Process)
        {
            m_Process=false;
            Process();
        }
        return AudioBuffers[ProcIndex]->Buffer;
    }
    virtual void Init(const int Index, void* MainWindow);
    virtual void Load(const QString& XML) { if (m_Form != NULL) m_Form->Load(XML); }
    virtual const QString Save(void) {
        if (m_Form != NULL) return m_Form->Save();
        return QString();
    }
    virtual void Execute(const bool Show) {
        if (m_Form != NULL)
        {
            if (Show) m_Form->show();
            else m_Form->setVisible(false);
        }
    }
    virtual void Play(const bool /*FromStart*/) {}
    virtual void Pause(void) {}
    virtual void Tick(void) { m_Process=true; }
    virtual void UpdateHost() { if (m_Host != NULL) m_Host->ParameterChange(); }
    virtual void Activate() { if (m_Host != NULL) m_Host->Activate(this); }
protected:
    bool m_Initialized;
    QString m_DeviceID;
    int m_Index;
    void* m_MainWindow;
    QString m_FileName;
    QString m_Name;
    CFastPointerList m_Jacks;
    int m_ParameterCount;
    std::vector<ParameterType> m_Parameters;
    std::vector<int> m_ParameterValues;
    inline float Fetch(const int ProcIndex) { return ((CInJack*)m_Jacks[ProcIndex])->GetNext(); }
    inline void* FetchP(const int ProcIndex) { return ((CInJack*)m_Jacks[ProcIndex])->GetNextP(); }
    inline float* FetchA(const int ProcIndex) { return ((CInJack*)m_Jacks[ProcIndex])->GetNextA(); }
    std::vector<CAudioBuffer*> AudioBuffers;
    int m_BufferSize;
    IHost* m_Host;
    PresetsType m_Presets;
    CSoftSynthsForm* m_Form;
    void AddParameter(ParameterType::ParameterTypes Type,const QString& Name,const QString& Unit,int Min,int Max,int DecimalFactor,const QString& ListString,int Value);
    void AddJack(IJack* J,IJack::Directions tDirection);
    IJack* AddJack(const QString& sName,IJack::AttachModes tAttachMode,IJack::Directions tDirection,int ProcIndex=-1);
    virtual void CalcParams() {}
    virtual void Process() {}
    bool m_Process;
};

typedef IDevice*(*instancefunc)();

#endif // IDEVICE_H
