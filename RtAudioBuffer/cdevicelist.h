#ifndef CDEVICELIST_H
#define CDEVICELIST_H

#include <QtCore>
#include "softsynthsclasses.h"
#include "caddins.h"

class CJackCollection
{
private:
    QList<IJack*> m_Jacks;
    QStringList m_Keys;
public:
    CJackCollection();
    ~CJackCollection();
    void Add(IJack* Jack, const QString& Key);
    void Remove(const QString& Key);
    void Remove(const int Index);
    IJack* Item(const QString& Key);
    IJack* Item(const int Index);
    const int Count(void);
    void Clear(void);
};

class CDeviceList
{
    private:
        CJackCollection m_Jacks;
        QList<IDevice*> m_Devices;
        void DisconnectDevice(IDevice* Device);
        void RemoveDevice(IDevice* Device);
        void Disconnect(IJack* J1, IJack* J2);
    public:
        IDevice* AddDevice(IDevice* D); // test purposes only! Must be private!!!
        CDeviceList();
        ~CDeviceList();
        QList<IDevice*>* Devices();
        QString DeviceID(IDevice* D);
        QString JackID(IDevice* D, const int JackIndex);
        QString JackID(IJack* J);
        void AddJack(IJack* Jack); // for the JackBar!!!
        IDevice* AddDevice(voidinstancefunc InstanceFunction, const int ID, void* MainWindow);
        const int FindFreeID(const QString& Name);
        void DisconnectDevice(const int Device);
        void RemoveDevice(const int Device);
        void Clear(void);
        void SaveParameters(QDomLiteElement* Device, IDevice* D);
        void LoadParameters(QDomLiteElement* Device, const int DeviceIndex, const bool Poly);
        const bool CanConnect(const int J1, const int J2);
        const bool CanConnect(IJack* J1, IJack* J2);
        const bool Connect(IJack* J1, IJack* J2);
        const bool Connect(const int J1, const int J2);
        const bool Connect(const QString& J1, const QString& J2);
        void Disconnect(const int J1, const int J2);
        void Disconnect(const QString& J1, const QString& J2);
        const bool IsConnected(const int J1, const int J2);
        const bool IsConnected(const QString& J1, const QString& J2);
        const bool IsConnected(IJack* J1, IJack* J2);
        void DisconnectJack(IJack* Jack);
        void DisconnectJack(const int Jack);
        void DisconnectJack(const QString& Jack);
        void Play(const bool FromStart);
        void Pause(void);
        void Tick(void);
        void HideForms(void);
        IJack* Jacks(const int Index);
        IJack* Jacks(const QString& DeviceID);
        IDevice* Device(const int Index);
        const int JackCount(void);
        const int DeviceCount(void);
        const int IndexOfDevice(IDevice* D);
};
#endif // CDEVICELIST_H
