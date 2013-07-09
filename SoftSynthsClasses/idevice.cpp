#include "idevice.h"
#include <QFileDialog>

IDevice::~IDevice()
{
    if (m_Form != NULL)
    {
        m_Form->hide();
        try
        {
            delete m_Form;
            qDebug() << m_DeviceID << "Form Deleted";
        }
        catch (...)
        {}
        m_Form=NULL;
    }
    foreach (void* j,m_Jacks) delete (IJack*)j;
    qDebug() << "Exit " + m_DeviceID;
}

void IDevice::Init(const int Index, void *MainWindow)
{
    m_Index=Index;
    m_MainWindow=MainWindow;
    if (m_Index==0) m_DeviceID=m_Name;
    else m_DeviceID=m_Name + " " + QString::number(m_Index);
    qDebug() << ("Create " + m_DeviceID);
    m_Initialized=true;
    m_Process=false;
}

const QString IDevice::OpenFile(const QString& Filter)
{
    QFileDialog d((QWidget*)m_MainWindow);
    d.setFileMode(QFileDialog::ExistingFile);
    d.setNameFilter(Filter);
    d.setDirectory(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    if (!m_FileName.isEmpty()) d.selectFile(m_FileName);
    if (d.exec()!=QDialog::Accepted) return QString();
    return d.selectedFiles().first();
}

void IDevice::AddParameter(ParameterType::ParameterTypes Type,const QString& Name,const QString& Unit,int Min,int Max,int DecimalFactor,const QString& ListString,int Value)
{
    m_Parameters.resize(m_ParameterCount+1);
    m_ParameterValues.resize(m_ParameterCount+1);
    m_Parameters[m_ParameterCount].Type=Type;
    m_Parameters[m_ParameterCount].Name=Name;
    m_Parameters[m_ParameterCount].Unit=Unit;
    m_Parameters[m_ParameterCount].Min=Min;
    m_Parameters[m_ParameterCount].Max=Max;
    m_Parameters[m_ParameterCount].DecimalFactor=DecimalFactor;
    m_Parameters[m_ParameterCount].List=ListString;
    m_ParameterValues[m_ParameterCount]=Value;
    m_ParameterCount++;
}

void IDevice::AddJack(IJack* J,IJack::Directions tDirection)
{
    if (tDirection==IJack::Out)
    {
        COutJack* OJ=(COutJack*)J;
        OJ->ProcIndex=m_Jacks.size();
        AudioBuffers.resize(OJ->ProcIndex+1);
        AudioBuffers[OJ->ProcIndex]=OJ->AudioBuffer;
    }
    m_Jacks.push_back(J);
}

IJack* IDevice::AddJack(const QString& sName,IJack::AttachModes tAttachMode,IJack::Directions tDirection,int ProcIndex)
{
    if (ProcIndex==-1) ProcIndex=m_Jacks.size();
    if (tDirection==IJack::In)
    {
        CInJack* IJ=new CInJack(sName,m_DeviceID,tAttachMode,tDirection,this);
        m_Jacks.push_back(IJ);
        return IJ;
    }
    COutJack* OJ=new COutJack(sName,m_DeviceID,tAttachMode,tDirection,this,ProcIndex);
    if ((unsigned int)ProcIndex >= AudioBuffers.size()) AudioBuffers.resize(ProcIndex+1);
    AudioBuffers[ProcIndex]=OJ->AudioBuffer;
    m_Jacks.push_back(OJ);
    return OJ;
}

