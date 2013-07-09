#include "csf2generator.h"
#include "softsynthsclasses.h"

QMap<QString, CSF2File*> CSF2Generator::SF2Files=QMap<QString, CSF2File*>();

bool CSF2Generator::LoadFile(const QString& Path)
{
    QString m_Path=QFileInfo(Path).absoluteFilePath();
    Ready=false;
    if (SFFile!=NULL)
    {
        qDebug() << SFFile->ReferenceCount << SF2Files.count();
        if (--SFFile->ReferenceCount==0)
        {
            sfUnloadSFBank(SFFile->BankID);
            SF2Files.remove(SFFile->Path.toLower());
            qDebug() << SF2Files.count();
            delete SFFile;
        }
        SFFile=NULL;
    }
    if (!SF2Files.contains(m_Path.toLower()))
    {
        if (!QFileInfo(m_Path).exists())
        {
            return false;
        }
        SFFile=new CSF2File;
        if (!SFFile->Open(m_Path))
        {
            delete SFFile;
            SFFile=NULL;
            return false;
        }
        SF2Files.insert(m_Path.toLower(),SFFile);
    }
    else
    {
        SFFile=SF2Files[m_Path.toLower()];
    }
    MidiBank=0;
    MidiPreset=0;
    pitchWheel=0;
    transpose=0;
    SFFile->ReferenceCount++;
    qDebug() << SFFile->ReferenceCount;
    FinishedPlaying=true;
    ID=0;
    Channel=0;
    return true;
}

CSF2Generator::CSF2Generator()
{
    Ready=false;
    SFFile=NULL;
    OscCount=0;
    FinishedPlaying=true;
    ID=0;
    Channel=0;
    MidiBank=0;
    MidiPreset=0;
    pitchWheel=0;
    transpose=0;
    m_ModulationRate=CPresets::Presets.ModulationRate;
    AudioL=new float[m_ModulationRate*2];
    AudioR=AudioL +m_ModulationRate;
    ZeroMemory(AudioL,m_ModulationRate*sizeof(float)*2);
}

CSF2Generator::~CSF2Generator()
{
    delete [] AudioL;
    if (SFFile!=NULL)
    {
        qDebug() << SFFile->ReferenceCount << SF2Files.count();
        if (--SFFile->ReferenceCount==0)
        {
            sfUnloadSFBank(SFFile->BankID);
            SF2Files.remove(SFFile->Path.toLower());
            qDebug() << SF2Files.count();
            delete SFFile;
        }
        SFFile=NULL;
    }
    qDeleteAll(Osc);
}

void CSF2Generator::setPitchWheel(int cent)
{
    pitchWheel=cent;
    foreach (OscType* O, Osc) O->setPitchWheel(cent);
}

void CSF2Generator::addTranspose(int steps)
{
    transpose+=steps;
    foreach (OscType* O, Osc) O->setTranspose(transpose);
}

void CSF2Generator::setAftertouch(short value)
{
    float val=(value*0.001)+1;
    foreach (OscType* O, Osc) O->setAftertouch(val);
}

void CSF2Generator::resetTranspose()
{
    transpose=0;
    foreach (OscType* O, Osc) O->setTranspose(0);
}

void CSF2Generator::ResetSample(const short MidiNote, const short MidiVelo)
{
    Ready=false;
    if (!SFFile) return;
    PlayEnd=false;
    FinishedPlaying=false;
    sfData* OscData = sfNav(SFFile->BankID,MidiBank,MidiPreset,MidiNote,MidiVelo,&OscCount);
    //    SmpHdrs=sfGetSampHdrs(MidiBank,&SmpCnt);

    if (OscCount <= 0) return;
    /*
    Debug("Osc count " + QString::number(OscCount) +
        " Offset " + QString::number(SFFile->Offset) +
        " Size " + QString::number(SFFile->SFData->Size) +
        " MIDI Bank " + QString::number(MidiBank) +
        " MIDI Preset " + QString::number(MidiPreset) +
        " SF Bank " + QString::number(SFFile->BankID)
    );
    */
    //OscFactor=1/sqrt(OscCount);
    for (int i=Osc.size();i<OscCount;i++)
    {
        Osc.push_back(new OscType);
    }
    for (int i=0;i<OscCount;i++)
    {
        OscType* O=Osc[i];
        O->Init(&OscData[i],SFFile,MidiNote,MidiVelo);
        O->setPitchWheel(pitchWheel);
        O->setTranspose(transpose);
        O->Start();
    }
    Ready=true;
}

void CSF2Generator::EndSample()
{
    PlayEnd=true;
    ID=0;
    foreach (OscType* O, Osc) O->End();
}

float* CSF2Generator::GetNext()
{
    //short I;
    if ((!Ready) | FinishedPlaying)
    {
        return NULL;
    }
    short SilentCount=0;
    foreach (OscType* O, Osc)
    {
        if (O->Silent)
        {
            SilentCount++;
        }
        else
        {
            break;
        }
    }
    if (SilentCount==OscCount)
    {
        FinishedPlaying=true;
        return NULL;
    }
    ZeroMemory(AudioL,m_ModulationRate*sizeof(float)*2);
    foreach (OscType* O, Osc)
    {
        if (!O->Silent)
        {
            O->Modulate();
            char SmpMd=O->SampleMode & 3;
            if (O->Stereo==OscType::StereoL)
            {
                if ((SmpMd & 1) | (!PlayEnd & (SmpMd & 2)))
                {
                    for (unsigned int s=0;s<m_ModulationRate;s++)
                    {
                        O->Loop();
                        AudioL[s]+=O->UpdatePos();
                    }
                }
                else //if (SmpMd==0)
                {
                    for (unsigned int s=0;s<m_ModulationRate;s++)
                    {
                        if (O->NoLoop()) break;
                        AudioL[s]+=O->UpdatePos();
                    }
                }
            }
            else if (O->Stereo==OscType::StereoR)
            {
                if ((SmpMd & 1) | (!PlayEnd & (SmpMd & 2)))
                {
                    for (unsigned int s=0;s<m_ModulationRate;s++)
                    {
                        O->Loop();
                        AudioR[s]+=O->UpdatePos();
                    }
                }
                else //if (SmpMd==0)
                {
                    for (unsigned int s=0;s<m_ModulationRate;s++)
                    {
                        if (O->NoLoop()) break;
                        AudioR[s]+=O->UpdatePos();
                    }
                }
            }
            else
            {
                float LeftPan=O->LeftPan;
                float RightPan=O->RightPan;
                if ((SmpMd & 1) | (!PlayEnd & (SmpMd & 2)))
                {
                    //Debug("Buffer");
                    for (unsigned int s=0;s<m_ModulationRate;s++)
                    {
                        O->Loop();
                        float a=O->UpdatePos();
                        AudioL[s]+=a*LeftPan;
                        AudioR[s]+=a*RightPan;
                    }
                }
                else //if (SmpMd==0)
                {
                    for (unsigned int s=0;s<m_ModulationRate;s++)
                    {
                        if (O->NoLoop()) break;
                        float a=O->UpdatePos();
                        AudioL[s]+=a*LeftPan;
                        AudioR[s]+=a*RightPan;
                    }
                }
            }
        }
    }
    return AudioL;
}

short CSF2Generator::PresetCount()
{
    unsigned short cnt;
    sfGetPresetHdrs(SFFile->BankID,&cnt );
    return cnt;
}

SFPRESETHDRPTR CSF2Generator::Preset(short Index)
{
    unsigned short cnt;
    SFPRESETHDRPTR PresetHeaders=sfGetPresetHdrs(SFFile->BankID,&cnt );
    if (Index < cnt) return &PresetHeaders[Index];
    return NULL;
}

