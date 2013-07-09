#include "cdevicelist.h"

CJackCollection::CJackCollection()
{
}

CJackCollection::~CJackCollection()
{
}

void CJackCollection::Add(IJack* Jack, const QString& Key)
{
    m_Jacks.append(Jack);
    m_Keys.append(Key);
}

void CJackCollection::Remove(const QString& Key)
{
    int Index=m_Keys.indexOf(Key);
    if (Index>-1)
    {
        m_Jacks.removeAt(Index);
        m_Keys.removeAt(Index);
    }
}

void CJackCollection::Remove(const int Index)
{
    if (Index<m_Jacks.count())
    {
        m_Jacks.removeAt(Index);
        m_Keys.removeAt(Index);
    }
}

IJack* CJackCollection::Item(const QString& Key)
{
    int Index=m_Keys.indexOf(Key);
    if (Index>-1) return m_Jacks[Index];
    return NULL;
}

IJack* CJackCollection::Item(const int Index)
{

    if (Index<m_Jacks.count()) return m_Jacks[Index];
    return NULL;
}

const int CJackCollection::Count()
{
    return m_Jacks.count();
}

void CJackCollection::Clear()
{
    m_Jacks.clear();
    m_Keys.clear();
}


CDeviceList::CDeviceList()
{
}

CDeviceList::~CDeviceList()
{
}

QList<IDevice*>* CDeviceList::Devices()
{
    return &m_Devices;
}

QString CDeviceList::DeviceID(IDevice* D)
{
    if (D->Index()==0) return D->Name();
    return D->Name() + " " + QString::number(D->Index());
}

QString CDeviceList::JackID(IDevice* D, const int JackIndex)
{
    return DeviceID(D) + " " + D->GetJack(JackIndex)->Name;
}

QString CDeviceList::JackID(IJack* J)
{
    return J->Owner + " " + J->Name;
}

IDevice* CDeviceList::AddDevice(IDevice *D)
{
    for (int i=0;i<D->JackCount();i++)
    {
        m_Jacks.Add(D->GetJack(i),JackID(D,i));
    }
    m_Devices.append(D);
    return D;

}

IDevice* CDeviceList::AddDevice(voidinstancefunc InstanceFunction, const int ID, void* MainWindow)
{
    instancefunc initializer=(instancefunc)InstanceFunction;
    IDevice* D=initializer();
    D->Init(ID,MainWindow);
    return AddDevice(D);
}

const int CDeviceList::FindFreeID(const QString& Name)
{
    int i=0;
    bool Taken;
    do
    {
        Taken=false;
        i++;
        foreach (IDevice* D, m_Devices)
        {
            if (Name==D->Name())
            {
                if (D->Index()==i)
                {
                    Taken=true;
                    break;
                }
            }
        }
    }
    while (Taken);
    return i;
}

void CDeviceList::DisconnectDevice(IDevice* Device)
{
    for (int i=0;i<m_Jacks.Count();i++)
    {
        IJack* J=m_Jacks.Item(i);
        if (J->Direction==IJack::In)
        {
            for (int i1=0;i1<Device->JackCount();i1++)
            {
                if (Device->GetJack(i1)->Direction==IJack::Out)
                {
                    if (Device->GetJack(i1)->Owner==DeviceID(Device))
                    {
                        CInJack* IJ=(CInJack*)J;
                        if (IsConnected(IJ,Device->GetJack(i1))) IJ->DisconnectFromOut((COutJack*)Device->GetJack(i1));
                    }
                }
            }
        }
    }
    for (int i=0;i<Device->JackCount();i++)
    {
        if (Device->GetJack(i)->Direction==IJack::In)
        {
            CInJack* IJ=(CInJack*)Device->GetJack(i);
            for (int i1=0;i1<IJ->OutJackCount();i1++)
            {
                if (IsConnected(IJ,IJ->OutJack(i1))) IJ->DisconnectFromOut(IJ->OutJack(i1));
            }
        }
    }
}

void CDeviceList::DisconnectDevice(const int Device)
{
    DisconnectDevice(m_Devices[Device]);
}

void CDeviceList::RemoveDevice(IDevice* Device)
{
    DisconnectDevice(Device);
    for (int i=0;i<Device->JackCount();i++)
    {
        m_Jacks.Remove(JackID(Device,i));
    }
    m_Devices.removeOne(Device);
}

void CDeviceList::RemoveDevice(const int Device)
{
    RemoveDevice(m_Devices[Device]);
}

void CDeviceList::Clear()
{
    foreach (IDevice* D, m_Devices)
    {
        RemoveDevice(D);
        delete D;
    }
}

void CDeviceList::SaveParameters(QDomLiteElement* Device, IDevice* D)
{
    for (int i1=0;i1<D->ParameterCount();i1++)
    {
        QDomLiteElement* Parameter = Device->appendChild("Parameter");
        Parameter->setAttribute("Index",i1);
        Parameter->setAttribute("Name",D->Parameter(i1).Name);
        Parameter->setAttribute("Value",D->GetParameterValue(i1));
    }
    QString CustomString=D->Save();
    if (!CustomString.isEmpty())
    {
        Device->appendChildFromString(CustomString);
    }
}

void CDeviceList::LoadParameters(QDomLiteElement* Device, const int DeviceIndex, const bool Poly)
{
    IDevice* D=m_Devices[DeviceIndex];
    QDomLiteElementList XMLParameters = Device->elementsByTag("Parameter");
    foreach (QDomLiteElement* XMLParameter, XMLParameters)
    {
        QString ParameterName=XMLParameter->attribute("Name");
        for (int i=0;i<D->ParameterCount();i++)
        {
            if (D->Parameter(i).Name==ParameterName)
            {
                D->SetParameterValue(i,XMLParameter->attributeValue("Value"));
            }
        }
    }
    QDomLiteElement* Custom = Device->elementByTag("Custom");
    if (Custom)
    {
        D->Load(Custom->toString());
        if (Poly) D->Execute(false);
    }
}

const bool CDeviceList::CanConnect(IJack* J1, IJack* J2)
{
    if (J1 != J2)
    {
        if (J1->Owner != J2->Owner)
        {
            if (J1->Direction != J2->Direction)
            {
                if (J1->AttachMode & J2->AttachMode)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

const bool CDeviceList::CanConnect(const int J1, const int J2)
{
    return CanConnect(m_Jacks.Item(J1),m_Jacks.Item(J2));
}

const bool CDeviceList::Connect(IJack* J1, IJack* J2)
{
    if (!(J1 && J2))
    {
        return false;
    }
    COutJack* OJ;
    CInJack* IJ;
    if (J1->Direction==IJack::In)
    {
        OJ=(COutJack*)J2;
        IJ=(CInJack*)J1;
    }
    else
    {
        OJ=(COutJack*)J1;
        IJ=(CInJack*)J2;
    }
    if (!IJ->ConnectionState(OJ))
    {
        IJ->ConnectToOut(OJ);
        return true;
    }
    return false;
}

const bool CDeviceList::Connect(const int J1, const int J2)
{
    return Connect(m_Jacks.Item(J1),m_Jacks.Item(J2));
}

const bool CDeviceList::Connect(const QString& J1, const QString& J2)
{
    return Connect(m_Jacks.Item(J1),m_Jacks.Item(J2));
}

void CDeviceList::Disconnect(IJack* J1, IJack* J2)
{
    if (!(J1 && J2))
    {
        return;
    }
    COutJack* OJ;
    CInJack* IJ;
    if (J1->Direction==IJack::In)
    {
        OJ=(COutJack*)J2;
        IJ=(CInJack*)J1;
    }
    else
    {
        OJ=(COutJack*)J1;
        IJ=(CInJack*)J2;
    }
    if (IJ->ConnectionState(OJ))
    {
        IJ->DisconnectFromOut(OJ);
        return;
    }
    return;
}

void CDeviceList::Disconnect(const int J1, const int J2)
{
    Disconnect(m_Jacks.Item(J1),m_Jacks.Item(J2));
}

void CDeviceList::Disconnect(const QString& J1, const QString& J2)
{
    Disconnect(m_Jacks.Item(J1),m_Jacks.Item(J2));
}

const bool CDeviceList::IsConnected(IJack* J1, IJack* J2)
{
    if (!(J1 && J2))
    {
        return false;
    }
    if (!CanConnect(J1,J2))
    {
        return false;
    }
    COutJack* OJ;
    CInJack* IJ;
    if (J1->Direction==IJack::In)
    {
        OJ=(COutJack*)J2;
        IJ=(CInJack*)J1;
    }
    else
    {
        OJ=(COutJack*)J1;
        IJ=(CInJack*)J2;
    }
    if (IJ->ConnectionState(OJ))
    {
        return true;
    }
    return false;
}

const bool CDeviceList::IsConnected(const int J1, const int J2)
{
    return IsConnected(m_Jacks.Item(J1),m_Jacks.Item(J2));
}

const bool CDeviceList::IsConnected(const QString& J1, const QString& J2)
{
    return IsConnected(m_Jacks.Item(J1),m_Jacks.Item(J2));
}

void CDeviceList::AddJack(IJack* Jack)
{
    m_Jacks.Add(Jack,JackID(Jack));
}

void CDeviceList::DisconnectJack(IJack* Jack)
{
    if (Jack->Direction==IJack::In)
    {
        CInJack* IJ=(CInJack*)Jack;
        for (int i=0;i<IJ->OutJackCount();i++)
        {
            IJ->DisconnectFromOut(IJ->OutJack(i));

        }
    }
    else
    {
        COutJack* OJ=(COutJack*)Jack;
        for (int i=0;i<m_Jacks.Count();i++)
        {
            IJack* J1=m_Jacks.Item(i);
            if (J1->Direction==IJack::In)
            {
                CInJack* IJ=(CInJack*)J1;
                if (IJ->ConnectionState(OJ))
                {
                    IJ->DisconnectFromOut(OJ);
                }
            }
        }

    }
}

void CDeviceList::DisconnectJack(const int Jack)
{
    DisconnectJack(m_Jacks.Item(Jack));
}

void CDeviceList::DisconnectJack(const QString& Jack)
{
    DisconnectJack(m_Jacks.Item(Jack));
}

void CDeviceList::Play(const bool FromStart)
{
    foreach (IDevice* D, m_Devices) D->Play(FromStart);
}

void CDeviceList::Pause()
{
    foreach (IDevice* D, m_Devices) D->Pause();
}

void CDeviceList::Tick()
{
    foreach (IDevice* D, m_Devices) D->Tick();
}

void CDeviceList::HideForms()
{
    foreach (IDevice* D, m_Devices) D->HideForm();
}

IJack* CDeviceList::Jacks(const int Index)
{
    return (IJack*)m_Jacks.Item(Index);
}

IJack* CDeviceList::Jacks(const QString& DeviceID)
{
    return m_Jacks.Item(DeviceID);
}

IDevice* CDeviceList::Device(const int Index)
{
    return m_Devices[Index];
}

const int CDeviceList::JackCount()
{
    return m_Jacks.Count();
}

const int CDeviceList::DeviceCount()
{
    return m_Devices.count();
}

const int CDeviceList::IndexOfDevice(IDevice* D)
{
    return m_Devices.indexOf(D);
}
