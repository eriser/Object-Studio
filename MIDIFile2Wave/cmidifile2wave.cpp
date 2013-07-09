#include "cmidifile2wave.h"
#include <QDesktopWidget>

CDeviceListLite::CDeviceListLite()
{
}

void CDeviceListLite::Play(const bool FromStart)
{
    foreach (IDevice* d, Devices) d->Play(FromStart);
}

void CDeviceListLite::Pause()
{
    foreach (IDevice* d, Devices) d->Pause();
}

void CDeviceListLite::Tick()
{
    foreach (IDevice* d, Devices) d->Tick();
}

void CDeviceListLite::DisconnectAll()
{
    foreach (IJack* j, Jacks)
    {
        if (j != 0)
        {
            if (j->Direction==IJack::In)
            {
                CInJack* j1=(CInJack*)j;
                for (int i=j1->OutJackCount()-1;i>=0;i--)
                {
                    qDebug() << "Disconnect" << j1->Owner << j1->Name << j1->OutJack(i)->Name;
                    j1->DisconnectFromOut(j1->OutJack(i));
                }
            }
        }
    }
}

void CDeviceListLite::DisconnectDevice(IDevice *device)
{
    foreach (IJack* j, Jacks)
    {
        if (j != 0)
        {
            if (j->Direction==IJack::In)
            {
                CInJack* j1=(CInJack*)j;
                for (int i=j1->OutJackCount()-1;i>=0;i--)
                {
                    COutJack* j2=j1->OutJack(i);
                    if ((j1->Owner==device->DeviceID()) || (j2->Owner==device->DeviceID()))
                    {
                        qDebug() << "Disconnect" << j1->Owner << j1->Name << j1->OutJack(i)->Name;
                        j1->DisconnectFromOut(j2);
                    }
                }
            }
        }
    }
}

void CDeviceListLite::AddDevice(IDevice *device, int index, void* MainWindow)
{
    device->Init(index,MainWindow);
    device->SetHost((IHost*)MainWindow);
    Devices.append(device);
    AddJacks(device);
}

void CDeviceListLite::RemoveDevice(IDevice *device)
{
    for (int i=0;i<device->JackCount();i++)
    {
        Jacks.remove(device->DeviceID()+" "+device->GetJack(i)->Name);
    }
    Devices.removeOne(device);
    delete device;
}

void CDeviceListLite::Clear()
{
    Jacks.clear();
    foreach (IDevice* d,Devices)
    {
        Devices.removeOne(d);
        delete d;
    }
}

void CDeviceListLite::AddJacks(IDevice *device)
{
    for (int i=0;i<device->JackCount();i++)
    {
        IJack* j=device->GetJack(i);
        Jacks.insert(device->DeviceID() +" "+j->Name,j);
    }
}

void CDeviceListLite::ConnectJacks(QString InJack, QString OutJack)
{
    CInJack* J1=(CInJack*)Jacks[InJack];
    COutJack* J2=(COutJack*)Jacks[OutJack];
    (J1)->ConnectToOut(J2);
}

#define devicename "MIDIFile2Wave"

CMIDI2WavForm::CMIDI2WavForm(IDevice* Device,QWidget* Parent) :
    CSoftSynthsForm(Device,true,Parent)
{
    MW=new CMixerWidget(this);
    QHBoxLayout* ly=new QHBoxLayout(this);
    ly->setMargin(0);
    ly->setSpacing(0);
    ly->addWidget(MW);
}

CMIDI2WavForm::~CMIDI2WavForm()
{
    qDebug() << "Exit CMIDI2WavForm";
}

CMIDIFile2Wave::CMIDIFile2Wave()
{
    Mx=NULL;
}

void CMIDIFile2Wave::Play(const bool FromStart)
{
    if (FromStart) MW->resetPeak();
    DeviceList.Play(FromStart);
}

void CMIDIFile2Wave::Pause()
{
    DeviceList.Pause();
}

void CMIDIFile2Wave::Tick()
{
    DeviceList.Tick();
}

float* CMIDIFile2Wave::GetNextA(const int ProcIndex)
{
    if (Mx != NULL) return Mx->GetNextA(ProcIndex+CStereoMixer::jnOut);
    return NULL;
}

void CMIDIFile2Wave::LoadFile(QString filename)
{
    try
    {
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly))
        {
            OpenPtr((char*)f.map(0,f.size()),f.size());
            f.close();
            m_FileName=filename;
        }
    }
    catch (...)
    {
    }
}

void CMIDIFile2Wave::OpenPtr(const char* Pnt, int Length)
{
    //QPicFrame* pf=new QPicFrame(m_Form);
    //pf->setFixedSize(m_Form->size());
    //pf->grabWidget(MW,m_Form->rect());
    if (Mx != NULL) Mx->Disabled=true;
    MW->stop();
    DeviceList.DisconnectAll();

    for (int i=Effects.count();i<MIDIFile2Wave::effectCount;i++)
    {
        CVSTHost* VSTH=new CVSTHost;
        DeviceList.AddDevice(VSTH,i+1,m_MainWindow);
        Effects.append(VSTH);
    }
    LoadEffect(0,"/Library/Audio/Plug-Ins/VST/LexHall.vst");

    MFR.OpenPtr(Pnt,Length);
    int MFPCount=1;
    int channelcount=16;
    if (MFR.FileType() != 0)
    {
        int count=0;
        for (int i=MFR.TrackCount()-1;i>=0;i--)
        {
            count=i+1;
            qDebug() << MFR.NoteCount(i);
            if (MFR.NoteCount(i) != 0) break;
        }
        MFPCount=count;
        channelcount=count;
    }
    for (int i=0;i<MFPCount;i++)
    {
        CMIDIFilePlayer* MFP;
        if (i>=MFPs.count())
        {
            MFP=new CMIDIFilePlayer;
            DeviceList.AddDevice(MFP,i+1,m_MainWindow);
            MFPs.append(MFP);
        }
        else
        {
            MFP=(CMIDIFilePlayer*)MFPs[i];
        }
        MFP->OpenPtr(Pnt,Length);
        MFP->SetParameterValue(0,0);
        if (MFR.FileType() != 0) MFP->SetParameterValue(0,i+1);
    }
    for (int i=MFPs.count()-1;i>=MFPCount;i--)
    {
        IDevice* d=MFPs[i];
        MFPs.removeOne(d);
        DeviceList.RemoveDevice(d);
    }
    if (Mx != NULL)
    {
        if (Mx->channelCount != channelcount)
        {
            DeviceList.RemoveDevice(Mx);
            Mx=new CStereoMixer(channelcount,MIDIFile2Wave::effectCount);
            DeviceList.AddDevice(Mx,1,m_MainWindow);
        }
    }
    else
    {
        Mx=new CStereoMixer(channelcount,MIDIFile2Wave::effectCount);
        DeviceList.AddDevice(Mx,1,m_MainWindow);
    }
    //if (m_Form->isVisible())
    //{
        //pf->move(0,0);
        //pf->show();
        //pf->raise();
    //}
    m_Form->setUpdatesEnabled(false);
    if (!m_Form->isVisible())
    {
        MW->hide();
        MW->hideMaster();
    }
    for (int i =0;i<channelcount;i++)
    {
        CSF2Player* SF2P;
        CSF2ChannelWidget* ch;
        if (i>=MW->channels.count())
        {
            SF2P=new CSF2Player;
            DeviceList.AddDevice(SF2P,i+1,m_MainWindow);
            SF2s.append(SF2P);
            ch=MW->appendChannel(i);
        }
        else
        {
            SF2P=(CSF2Player*)SF2s[i];
            ch=MW->channels[i];
        }
        bool chVisible=true;
        if (MFR.FileType() != 0)
        {
            qDebug() << i << MFR.NoteCount(i);
            if (MFR.NoteCount(i) == 0) chVisible=false;
        }
        if (chVisible)
        {
            DeviceList.ConnectJacks("StereoMixer 1 In "+QString::number(i+1),"SF2Player "+ QString::number(i+1) +" Out");
        }
        if (MFR.FileType() == 0)
        {
            DeviceList.ConnectJacks("SF2Player "+ QString::number(i+1) +" MIDI In","MIDIFile 1 MIDI Out");
            ch->Init(Mx->channels[i], SF2P, i, "Channel "+QString::number(i+1));
        }
        else
        {
            if (chVisible) DeviceList.ConnectJacks("SF2Player "+ QString::number(i+1) +" MIDI In","MIDIFile "+ QString::number(i+1) +" MIDI Out");
            ch->Init(Mx->channels[i], SF2P, -1, "Track "+QString::number(i+1));
        }
        //qDebug() << QFileInfo(":/028.5mg Masterpiece GM Bank.sf2").size();
        if (chVisible) ch->loadSF("/Users/thomasallin/desktop/028.5mg Masterpiece GM Bank.sf2");
        //if (chVisible) ch->loadSF("/Users/thomasallin/desktop/jnsgm2-mini-sf2/jnsgm2 mini.sf2");

        ch->setVisible(chVisible);
    }
    for (int i=MW->channels.count()-1;i>=channelcount;i--)
    {
        IDevice* d=SF2s[i];
        SF2s.removeOne(d);
        DeviceList.RemoveDevice(d);
        MW->removeChannel(i);
    }
    MW->showMaster(Mx,Effects);
    for (int i=0;i<MIDIFile2Wave::effectCount;i++)
    {
        DeviceList.ConnectJacks("VSTHost "+ QString::number(i+1) +" In","StereoMixer 1 Send "+ QString::number(i+1));
        DeviceList.ConnectJacks("StereoMixer 1 Return","VSTHost "+ QString::number(i+1) +" Out 0");
    }
    //delete pf;
    m_Form->adjustSize();
    m_Form->setFixedSize(m_Form->size());
    Mx->Disabled=false;
    MW->start();
    MW->show();
    m_Form->setUpdatesEnabled(true);
}

void CMIDIFile2Wave::Center()
{
    m_Form->adjustSize();
    m_Form->setFixedSize(m_Form->size());
    m_Form->move(QDesktopWidget().rect().center()-m_Form->rect().center());
}

void CMIDIFile2Wave::SetTitle(const QString& t)
{
    m_Form->setWindowTitle(t);
}

void CMIDIFile2Wave::Load(const QString &XML)
{
    QDomLiteElement Custom("Custom");
    Custom.fromString(XML);
    QString Path=CPresets::ResolveFilename(Custom.attribute("File"));
    LoadFile(Path);
    QDomLiteElement* Mixer=Custom.elementByTag("Mixer");
    if (Mixer != 0)
    {
        for (int i=0;i<MW->channels.count();i++)
        {
            QDomLiteElement* ch=Mixer->elementByTag("Channel"+QString::number(i));
            if (ch != 0)
            {
                MW->channels[i]->Load(ch->firstChild()->toString());
            }
        }
        for (int i=0;i<Effects.count();i++)
        {
            QDomLiteElement* e=Mixer->elementByTag("Effect"+QString::number(i));
            if (e != 0)
            {
                Effects[i]->Load(e->firstChild()->toString());
            }
        }
        QDomLiteElement* Master=Mixer->elementByTag("Master");
        if (Master != 0)
        {
            MW->master->Load(Master->toString());
        }
    }
    m_Form->Load(XML);
}

const QString CMIDIFile2Wave::Save()
{
    QDomLiteElement Custom("Custom");
    Custom.fromString(m_Form->Save());
    Custom.setAttribute("File",QDir().relativeFilePath(FileName()));
    QDomLiteElement* Mixer=Custom.appendChild("Mixer");
    for (int i=0;i<MW->channels.count();i++)
    {
        QDomLiteElement* ch=Mixer->appendChild("Channel"+QString::number(i));
        ch->appendChildFromString(MW->channels[i]->Save());
    }
    for (int i=0;i<Effects.count();i++)
    {
        QDomLiteElement* e=Mixer->appendChild("Effect"+QString::number(i));
        e->appendChildFromString(Effects[i]->Save());
    }
    Mixer->appendChildFromString(MW->master->Save());
    return Custom.toString();
}

void CMIDIFile2Wave::Execute(const bool Show)
{
    if (Show)
    {
        if (m_FileName.isEmpty())
        {
            QString fn=OpenFile(MIDIFilePlayer::MIDIFilter);
            if (fn.isEmpty()) return;
            LoadFile(fn);
            m_FileName=fn;
        }
    }
    IDevice::Execute(Show);
}

void CMIDIFile2Wave::HideForm()
{
    foreach (IDevice* d, DeviceList.Devices)
    {
        d->HideForm();
    }
    m_Form->hide();
}

void CMIDIFile2Wave::Skip(unsigned long MilliSeconds)
{
    foreach (IDevice* d, DeviceList.Devices)
    {
        if (d->Name()=="MIDIFile") ((CMIDIFilePlayer*)d)->Skip(MilliSeconds);
    }
    foreach (IDevice* d, DeviceList.Devices)
    {
        if (d->Name()=="MIDIFile")
        {
            d->Play(false);
        }
        else
        {
            d->Play(true);
        }
    }
}

CMIDIFile2Wave::~CMIDIFile2Wave()
{
    if (m_Initialized) clear();
    qDebug() << "Exit CMIDIFile2Wave";
}

void CMIDIFile2Wave::Init(const int Index, void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Stereo,IJack::Out,jnOut);
    m_Form=new CMIDI2WavForm(this,(QWidget*)MainWindow);
    MW=((CMIDI2WavForm*)m_Form)->MW;
}

bool CMIDIFile2Wave::isEmpty()
{
    return MFPs.isEmpty();
}

bool CMIDIFile2Wave::isVisible()
{
    return m_Form->isVisible();
}

void CMIDIFile2Wave::clear()
{
    if (Mx != NULL) Mx->Disabled=true;
    DeviceList.Pause();
    MW->stop();
    DeviceList.DisconnectAll();
    MW->clear();
    Effects.clear();
    DeviceList.Clear();
}

void CMIDIFile2Wave::LoadEffect(int index, QString filename)
{
    if (Effects[index]->FileName() != filename) Effects[index]->Load("<Custom File=\""+filename+"\"/>");
}

bool CMIDIFile2Wave::IsPlaying()
{
    if (!MFPs.isEmpty()) return ((CMIDIFilePlayer*)MFPs[0])->IsPlaying();
    return false;
}

unsigned long CMIDIFile2Wave::CurrentTick()
{
    if (MFPs.count()) return ((CMIDIFilePlayer*)MFPs[0])->CurrentTick;
    return 0;
}

unsigned long CMIDIFile2Wave::Duration()
{
    return MFR.Duration();
}

unsigned long CMIDIFile2Wave::CurrentmSec()
{
    if (MFPs.count()) return ((CMIDIFilePlayer*)MFPs[0])->CurrentMilliSecond;
    return 0;
}

unsigned long CMIDIFile2Wave::MilliSeconds()
{
    return MFR.MilliSeconds();
}
