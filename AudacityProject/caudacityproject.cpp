#include "caudacityproject.h"
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>

AudacityBlock::AudacityBlock()
{
    Start=0;
    Buffersize=CPresets::Presets.ModulationRate;
    AliasStart=0;
    Channel=0;
}

float* AudacityBlock::GetNext()
{
    float* Audio=wa.GetNext(Rate);
    if (Audio)
    {
        return Audio+(Channel*Buffersize);
    }
    return NULL;
}

void AudacityBlock::Reset()
{
    wa.Reset();
    wa.SetPointer(AliasStart);
}

bool AudacityBlock::Init(QString Filename, unsigned int StartPtr, int Channels, unsigned int AliasPointer,int RateOverride)
{
    if (wa.open(Filename,CPresets::Presets.SampleRate,CPresets::Presets.ModulationRate))
    {
        Start=StartPtr;
        AliasStart=AliasPointer;
        Rate=RateOverride;
        if (Channels<wa.Channels)
        {
            Channel=Channels;
        }
        Reset();
        return true;
    }
    return false;
}

AudacityClip::AudacityClip()
{
    Buffersize=CPresets::Presets.ModulationRate;
    Buffer=new float[Buffersize];
    auBuffer=NULL;
    Counter=0;
    BlockIndex=0;
    BufferPointer=0;
}

AudacityClip::~AudacityClip()
{
    qDeleteAll(Blocks);
    delete[] Buffer;
}

void AudacityClip::Reset()
{
    BlockIndex=0;
    Counter=0;
    BufferPointer=0;
    auBuffer=NULL;
    foreach (AudacityBlock* AB, Blocks) AB->Reset();
}

void AudacityClip::AddBlock(QString Filename,unsigned int Start,int RateOverride)
{
    QString FN=CPresets::ResolveFilename(Filename);
    if (!QFileInfo(FN).exists()) return;
    AudacityBlock* AB=new AudacityBlock();
    if (AB->Init(FN,Start,0,0,RateOverride))
    {
        Blocks.push_back(AB);
    }
}

void AudacityClip::AddAliasBlock(QString Filename,unsigned int Start,int Channel,unsigned int AliasStart)
{
    QString FN=CPresets::ResolveFilename(Filename);
    if (!QFileInfo(FN).exists()) return;
    AudacityBlock* AB=new AudacityBlock();
    if (AB->Init(FN,Start,Channel,AliasStart,0))
    {
        Blocks.push_back(AB);
    }
}

void AudacityClip::LoadClip(const QString& XML,QString ProjectPath,int RateOverride)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="waveclip")
    {
        Offset=xml.attributeValue("offset");
        QDomLiteElement* sequence = xml.elementByTag("sequence");
        QDomLiteElementList blocks = sequence->elementsByTag("waveblock");
        foreach (QDomLiteElement* block,blocks)
        {
            int Start=block->attributeValue("start");
            QDomLiteElement* file = block->elementByTag("simpleblockfile");
            if (file)
            {
                QString Filename=file->attribute("filename");
                QString FilePath=ProjectPath + "/" + Filename;
                if (QFileInfo(FilePath).exists())
                {
                    AddBlock(FilePath,Start,RateOverride);
                }
                else
                {
                    //QString Path1=Filename.SubString(1,3);
                    //QString Path2="d" + Filename.SubString(4,2);
                    QString Path1=Filename.mid(1,3);
                    QString Path2="d" + Filename.mid(4,2);
                    FilePath=ProjectPath + "/" + Path1 + "/" + Path2 + "/" + Filename;
                    AddBlock(FilePath,Start,RateOverride);
                }
            }
            file=NULL;
            file = block->elementByTag("pcmaliasblockfile");
            if (file)
            {
                QString Filename=file->attribute("aliasfile");
                int AliasChannel=file->attributeValue("aliaschannel");
                int AliasStart=file->attributeValue("aliasstart");
                AddAliasBlock(Filename,Start,AliasChannel,AliasStart);
            }
            file=NULL;
        }
    }
}

void AudacityClip::LoadSequence(const QString& XML,QString ProjectPath,int RateOverride)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="sequence")
    {
        Offset=0;

        QDomLiteElementList blocks = xml.elementsByTag("waveblock");
        foreach(QDomLiteElement* block,blocks)
        {
            int Start=block->attributeValue("start");
            QDomLiteElement* file = block->elementByTag("simpleblockfile");
            if (file)
            {
                QString Filename=file->attribute("filename");
                QString FilePath=ProjectPath + "/" + Filename;
                if (QFileInfo(FilePath).exists())
                {
                    AddBlock(FilePath,Start,RateOverride);
                }
                else
                {
                    //AnsiString Path1=Filename.SubString(1,3);
                    //AnsiString Path2="d" + Filename.SubString(4,2);
                    QString Path1=Filename.mid(1,3);
                    QString Path2="d" + Filename.mid(4,2);
                    FilePath=ProjectPath + "/" + Path1 + "/" + Path2 + "/" + Filename;
                    AddBlock(FilePath,Start,RateOverride);
                }
            }
            file=NULL;
            file = block->elementByTag("pcmaliasblockfile");
            if (file)
            {
                QString Filename=file->attribute("aliasfile");
                int AliasChannel=file->attributeValue("aliaschannel");
                int AliasStart=file->attributeValue("aliasstart");
                AddAliasBlock(Filename,Start,AliasChannel,AliasStart);
            }
            file=NULL;
        }
    }
}

float* AudacityClip::GetNext()
{
    if (BlockIndex > Blocks.size()-1)
    {
        return NULL;
    }
    AudacityBlock* AB=Blocks[BlockIndex];
    AudacityBlock* NB=NULL;
    if (BlockIndex<Blocks.size()-1) NB=Blocks[BlockIndex+1];
    for (int i=0; i < Buffersize; i++)
    {
        if (Counter==AB->Start)
        {
            auBuffer=AB->GetNext();
            BufferPointer=0;
        }
        if (NB)
        {
            if (Counter==NB->Start)
            {
                AB=NB;
                auBuffer=AB->GetNext();
                BufferPointer=0;
                BlockIndex++;
                NB=NULL;
                if (BlockIndex < Blocks.size()-1) NB=Blocks[BlockIndex+1];
            }
        }
        if (Counter>AB->Start)
        {
            if (BufferPointer>=Buffersize)
            {
                auBuffer=AB->GetNext();
                BufferPointer=0;
                if (!NB)
                {
                    if (!auBuffer)
                    {
                        BlockIndex++;
                    }
                }
            }
        }
        if (auBuffer)
        {
            Buffer[i]=auBuffer[BufferPointer];
        }
        else
        {
            Buffer[i]=0;
        }
        BufferPointer++;
        Counter++;
    }
    return Buffer;
}

AudacityTrack::AudacityTrack()
{
    Buffersize=CPresets::Presets.ModulationRate;
    Time=0;
    TimeAdd=(float)Buffersize / (float)CPresets::Presets.SampleRate;
    ClipIndex=0;
    Playing=false;
    Mute=false;
    Solo=false;
    Gain=0;
}

AudacityTrack::~AudacityTrack()
{
    qDeleteAll(Clips);
}

void AudacityTrack::Reset()
{
    Playing=false;
    Time=0;
    ClipIndex=0;
    foreach (AudacityClip* AC, Clips) AC->Reset();
}

void AudacityTrack::Load(const QString& XML,QString ProjectPath)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="wavetrack")
    {
        Channel = xml.attributeValue("channel");
        Name = xml.attribute("name");
        Linked = xml.attributeValue("linked");
        Offset=xml.attributeValue("offset");
        //Mute = StrToInt("0"+AnsiString(xmldoc->GetAttribute("mute")));
        //Solo = StrToInt("0"+AnsiString(xmldoc->GetAttribute("solo")));
        Rate = xml.attributeValue("rate");
        Gain = xml.attributeValue("gain");
        Pan = xml.attributeValue("pan");

        FactorL=Gain;
        FactorR=Gain;
        if (Pan<0)
        {
            FactorR*=(1.0+Pan);
        }
        if (Pan>0)
        {
            FactorL*=(1.0-Pan);
        }

        QDomLiteElementList clips = xml.elementsByTag("waveclip");
        foreach(QDomLiteElement* clip,clips)
        {
            AudacityClip* AC=new AudacityClip();
            AC->LoadClip(clip->toString(),ProjectPath,Rate);
            Clips.push_back(AC);
        }
        clips = xml.elementsByTag("sequence");
        foreach(QDomLiteElement* clip,clips)
        {
            AudacityClip* AC=new AudacityClip();
            AC->LoadSequence(clip->toString(),ProjectPath,Rate);
            Clips.push_back(AC);
        }
    }
}

float* AudacityTrack::GetNext()
{
    if (ClipIndex > Clips.size()-1)
    {
        return NULL;
    }
    AudacityClip* AC=Clips[ClipIndex];
    if (Time<AC->Offset)
    {
        Time+=TimeAdd;
        return NULL;
    }
    AudacityClip* NC=NULL;
    if (ClipIndex < Clips.size()-1)
    {
        NC=Clips[ClipIndex+1];
        if (Time>=NC->Offset)
        {
            ClipIndex++;
            AC=NC;
        }
    }
    Time+=TimeAdd;
    float* auBuffer=AC->GetNext();
    if (auBuffer)
    {
        return auBuffer;
    }
    return NULL;
}

void inline CAudacityProject::CalcParams()
{
    ModFactor = (float)m_ParameterValues[pnVolume] * 0.01;
}

void inline CAudacityProject::LoadProject(const QString& ProjectFile)
{
    Loading=true;
    qDeleteAll(Tracks);
    Tracks.clear();
    QDomLiteDocument doc(ProjectFile);
    QString projname = doc.documentElement->attribute("projname");
    QString OrigPath=QFileInfo(ProjectFile).absolutePath() + "/" + projname;
    QDomLiteElementList tracks = doc.documentElement->elementsByTag("wavetrack");
    qDebug() << projname << OrigPath << tracks.size();
    Time=0;
    foreach(QDomLiteElement* track,tracks)
    {
        AudacityTrack* AT=new AudacityTrack();
        AT->Load(track->toString(),OrigPath);
        Tracks.push_back(AT);
    }
    Loading=false;
}

CAudacityProject::CAudacityProject()
{

}

CAudacityProject::~CAudacityProject()
{
    qDeleteAll(Tracks);
}

void CAudacityProject::Execute(const bool Show)
{
    if (Show)
    {
        QString fn=OpenFile("Audacity projects (*.aup)");
        if (!fn.isEmpty())
        {
            LoadProject(fn);
            m_FileName=fn;
            return;
        }
        else
        {
            QDesktopServices::openUrl(QUrl("file://"+m_FileName));
        }
    }
}

const QString CAudacityProject::Save()
{
    QDomLiteElement xml("Custom");
    xml.setAttribute("File",m_FileName);
    return xml.toString();
}

void CAudacityProject::Load(const QString& XML)
{
    m_FileName.clear();
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        QString CurrentPath = CPresets::ResolveFilename(xml.attribute("File"));
        if (!CurrentPath.isEmpty())
        {
            if (QFileInfo(CurrentPath).exists())
            {
                LoadProject(CurrentPath);
                m_FileName=CurrentPath;
                return;
            }
        }
    }
}

void CAudacityProject::Init(const int Index,void* MainWindow)
{
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Stereo,IJack::Out,0);
    AddParameter(ParameterType::dB,"Volume","dB",0,200,0,"",100);
    CalcParams();
    TimeAdd=(float)CPresets::Presets.ModulationRate / (float)CPresets::Presets.SampleRate;
    Time=0;
    Playing=false;
    Loading=false;
}

void CAudacityProject::Play(const bool FromStart)
{
    if (FromStart)
    {
        if (m_Initialized)
        {
            Time=0;
            foreach (AudacityTrack* AT, Tracks) AT->Reset();
        }
    }
    Playing=true;
}

void CAudacityProject::Pause()
{
    Playing=false;
}

void CAudacityProject::Process()
{
    CStereoBuffer* OutBuffer=(CStereoBuffer*)AudioBuffers[jnOut];
    OutBuffer->ZeroBuffer();
    foreach (AudacityTrack* AT, Tracks)
    {
        if (!AT->Playing)
        {
            if (AT->Offset<=Time)  AT->Playing=true;
        }
        if (AT->Playing)
        {
            float* auBuffer=AT->GetNext();
            //float volL=AT->FactorL*ModFactor;
            //float volR=AT->FactorR*ModFactor;
            //float* BufferL=AudioBuffers[jnOut]->Buffer;
            //float* BufferR=AudioBuffers[jnOut]->BufferR;

            if (auBuffer)
            {
                if (AT->Linked)
                {
                    OutBuffer->AddLeftBuffer(auBuffer,AT->FactorL*ModFactor);
                    //for (int i=0;i<m_BufferSize;i++) BufferL[i]+=auBuffer[i]*volL;
                }
                else if (AT->Channel==1)
                {
                    OutBuffer->AddRightBuffer(auBuffer,AT->FactorR*ModFactor);
                    //for (int i=0;i<m_BufferSize;i++) BufferR[i]+=auBuffer[i]*volR;
                }
                else
                {
                    for (int i=0;i<m_BufferSize;i++)
                    {
                        OutBuffer->AddLeftBuffer(auBuffer,AT->FactorL*ModFactor);
                        OutBuffer->AddRightBuffer(auBuffer,AT->FactorR*ModFactor);
                        //BufferL[i]+=auBuffer[i]*volL;
                        //BufferR[i]+=auBuffer[i]*volR;
                    }
                }
            }
        }
    }
    Time+=TimeAdd;
}

float* CAudacityProject::GetNextA(const int ProcIndex)
{
    if (!Playing) return NULL;
    if (Loading) return NULL;
    if (m_Process)
    {
        m_Process=false;
        Process();
    }
    return AudioBuffers[ProcIndex]->Buffer;
}
