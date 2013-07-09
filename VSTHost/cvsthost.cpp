#include "cvsthost.h"
#include <QFileDialog>

#define devicename "VSTHost"
#define BufferCount 8

CVSTHost::CVSTHost()
{
}

CVSTHost::~CVSTHost()
{
    if (m_Initialized) delete VST;
    qDebug() << "Exit CVSTHost";
}

void CVSTHost::Init(const int Index, void *MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("In",IJack::Stereo,IJack::In,jnIn);
    AddJack("MIDI In",IJack::MIDI,IJack::In,jnMIDIIn);
    for (int i=0;i<BufferCount/2;i++)
    {
        AddJack("Out " + QString::number(i),IJack::Stereo,IJack::Out,jnOut+i);
    }
    AddParameter(ParameterType::dB,"Volume","dB",0,200,0,"",100);
    AddParameter(ParameterType::SelectBox,"MIDI Channel","",0,16,0,"All§1§2§3§4§5§6§7§8§9§10§11§12§13§14§15§16",0);
    VolFactor=1.0;
    OldBuffers=0;
    InBufferCount=0;
    OutBufferCount=0;
    VST=new TVSTHost(MainWindow,this);
    Playing=false;
    uSPQ = 500000;
    CalcParams();
}

void inline CVSTHost::CalcParams()
{
    VolFactor=m_ParameterValues[pnVolume]*0.01;
    VST->MIDIChannel=m_ParameterValues[pnMIDIChannel];
}

void CVSTHost::Pause()
{
    Playing=false;
    VST->AllNotesOff();
}

void CVSTHost::RaiseForm()
{
    VST->RaiseForm();
}

const QString CVSTHost::FileName()
{
    return VST->FileName;
}

void CVSTHost::Execute(const bool Show)
{
    if (Show)
    {
        if (VST->FileName.isEmpty())
        {
            QString FN=QFileDialog::getOpenFileName((QWidget*)m_MainWindow,"Open VST Plug-in",CPresets::Presets.VSTPath);
            //OpenDialog1->Filter="VST Plug-in files (*.dll)|*.DLL";
            if (!FN.isEmpty())
            {
                if (QFileInfo(FN).exists())
                {
                    if (VST->Load(FN))
                    {
                        InBufferCount=VST->NumInputs();
                        OutBufferCount=VST->NumOutputs();
                        VST->ShowForm(true);
                        return;
                    }
                    qDebug() << "Could not open " + FN;
                }
            }
        }
        else
        {
            VST->ShowForm(true);
        }
    }
    else
    {
        VST->ShowForm(false);
    }
}

void CVSTHost::HideForm()
{
    VST->ShowForm(false);
}

const QString CVSTHost::Save()
{
    QDomLiteElement xml("Custom");
    QString Relpath=QDir(CPresets::Presets.VSTPath).relativeFilePath(VST->FileName);
    xml.setAttribute("File",Relpath);
    xml.appendChildFromString(VST->SaveXML());
    return xml.toString();
}

void CVSTHost::Load(const QString& XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        QString CurrentPath = xml.attribute("File");
        if (!CurrentPath.isEmpty())
        {
            CurrentPath = CPresets::ResolveFilename(QDir(CPresets::Presets.VSTPath).absoluteFilePath(CurrentPath));
            if (QFileInfo(CurrentPath).exists())
            {
                if (VST->Load(CurrentPath))
                {
                    QDomLiteElement* Custom=xml.elementByTag("Settings");
                    if (Custom)
                    {

                        VST->LoadXML(Custom->toString());
                    }
                    InBufferCount=VST->NumInputs();
                    OutBufferCount=VST->NumOutputs();
                    return;

                }
                qDebug() << "Could not open " + CurrentPath;

            }
        }
    }

}

void CVSTHost::Process()
{
    CMIDIBuffer* MB=(CMIDIBuffer*)FetchP(jnMIDIIn);
    if (MB)
    {
        VST->DumpMIDI(MB);
    }
    if (InBufferCount>1)
    {
        float* Buffer=FetchA(jnIn);
        VST->DumpAudio(1,Buffer+m_BufferSize,m_BufferSize);
        VST->DumpAudio(0,Buffer,m_BufferSize);
    }
    else if (InBufferCount)
    {
        float* Buffer=FetchA(jnIn);
        VST->DumpAudio(0,Buffer,m_BufferSize);
    }
    VST->Process();
    if (OutBufferCount>BufferCount)
    {
        OutBufferCount=BufferCount;
    }
    if (OldBuffers>OutBufferCount)
    {
        for (int i=OutBufferCount/2;i<OldBuffers/2;i++)
        {
            AudioBuffers[jnOut + i]->ZeroBuffer();
        }
    }
    OldBuffers=OutBufferCount;

    for (int i=0;i<OutBufferCount/2;i++)
    {
        VST->GetAudio(i*2,AudioBuffers[jnOut + i]->Buffer,m_BufferSize,VolFactor);
        VST->GetAudio((i*2)+1,((CStereoBuffer*)AudioBuffers[jnOut + i])->BufferR,m_BufferSize,VolFactor);

    }
}
