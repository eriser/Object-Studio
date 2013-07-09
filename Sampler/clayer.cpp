#include "clayer.h"
#include "softsynthsclasses.h"

CLayer::CLayer(int Upper,int Lower)
{
    qDebug() << "Create CLayer";
    LP.UpperTop=Upper;
    LP.UpperZero=Upper;
    LP.LowerTop=Lower;
    LP.LowerZero=Lower;
    LP.Volume=100;
    LP.Tune=0;
    LP.Transpose=0;
    PlayVol=1;
}

CLayer::CLayer(LayerParams LayerParams)
{
    qDebug() << "Create CLayer";
    LP=LayerParams;
    PlayVol=1;
}

CLayer::~CLayer()
{
    qDebug() << "Delete CLayer";
    for (int i = Ranges.count()-1; i >= 0; i--)
    {
        RemoveRange(Ranges[i]);
    }
}

void CLayer::ResetSample(short MidiNote)
{
    ActiveRanges.clear();
    foreach (CSampleKeyRange* KR,Ranges)
    {
        float Vol=KR->GetVolume(MidiNote+LP.Transpose);
        if (Vol > 0)
        {
            ActiveRanges.append(KR);
            KR->PlayVol=Vol;
            KR->WG.Reset();
        }
    }
}

void CLayer::EndSample()
{
    foreach (CSampleKeyRange* KR,ActiveRanges) KR->WG.Release();
}

void CLayer::ModifyBuffer(float *BufferL, float *BufferR, int Samples, float Frequency, float Velocity, float ModulationTune, float MixFactor)
{
    float CurrentFreq=Frequency*ModulationTune;
    float VelocityFactor=MixFactor*Velocity;
    foreach (CSampleKeyRange* KR,ActiveRanges)
    {
        float PV=KR->PlayVol*VelocityFactor;
        float TempTune=pow(2.0,(float)(KR->WG.LP.Tune)*0.001);
        if (KR->WG.Channels==1)
        {
            float* B=KR->WG.GetNext(CurrentFreq*TempTune);
            if (B)
            {
                for (int i=0;i<Samples;i++)
                {
                    float Signal=B[i]*PV;
                    BufferL[i]+=Signal;
                    BufferR[i]+=Signal;
                }
            }
        }
        else
        {
            float* BL=KR->WG.GetNext(CurrentFreq*TempTune);
            if (BL)
            {
                float* BR=BL+Samples;
                for (int i=0;i<Samples;i++)
                {
                    BufferL[i]+=BL[i]*PV;
                    BufferR[i]+=BR[i]*PV;
                }
            }
        }
    }
}

float CLayer::GetVolume(int Velocity)
{
    if ((Velocity<LP.LowerZero+1) || (Velocity>LP.UpperZero))
    {
        return 0;
    }
    if (Velocity<LP.LowerTop+1)
    {
        float diff=LP.LowerTop-LP.LowerZero;
        float Val=Velocity-(LP.LowerZero+1);
        return Val*(float)LP.Volume/diff*0.01;
    }
    if (Velocity>LP.UpperTop)
    {
        float diff=-(LP.UpperTop-LP.UpperZero);
        float Val=-(Velocity-(LP.UpperZero-1));
        return Val*(float)LP.Volume/diff*0.01;
    }
    return (float)LP.Volume*0.01;
}

CSampleKeyRange* CLayer::AddRange(const QString& WavePath,int Upper,int Lower)
{
    if (Upper<Lower)
    {
        int Temp=Lower;
        Lower=Upper;
        Upper=Temp;
    }
    qDebug() << "Create Range" << WavePath;
    CSampleKeyRange* KR=new CSampleKeyRange(WavePath,Upper,Lower);
    Ranges.append(KR);
    return KR;
}

void CLayer::ChangePath(int Range, const QString &WavePath)
{
    Ranges[Range]->ChangePath(WavePath);
}

void CLayer::RemoveRange(CSampleKeyRange* KR)
{
    Ranges.removeOne(KR);
    ActiveRanges.removeOne(KR);
    delete KR;
}

const QString CLayer::Save()
{
    QDomLiteElement xml("Custom");
    foreach (CSampleKeyRange* KR,Ranges)
    {
        QDomLiteElement* Range = xml.appendChild("Range");

        QString Relpath=QDir().relativeFilePath(KR->FileName);
        Range->setAttribute("WaveFile",Relpath);
        Range->setAttribute("UpperZero",KR->RP.UpperZero);
        Range->setAttribute("UpperTop",KR->RP.UpperTop);
        Range->setAttribute("LowerZero",KR->RP.LowerZero);
        Range->setAttribute("LowerTop",KR->RP.LowerTop);
        Range->setAttribute("Volume",KR->RP.Volume);
        Range->setAttribute("Start",QVariant::fromValue(KR->WG.LP.Start));
        Range->setAttribute("End",QVariant::fromValue(KR->WG.LP.End));
        Range->setAttribute("LoopStart",QVariant::fromValue(KR->WG.LP.LoopStart));
        Range->setAttribute("LoopEnd",QVariant::fromValue(KR->WG.LP.LoopEnd));
        Range->setAttribute("MIDINote",KR->WG.LP.MIDINote);
        Range->setAttribute("Tune",KR->WG.LP.Tune);
        Range->setAttribute("LoopType",(int)KR->WG.LP.LoopType);
        Range->setAttribute("XFade",KR->WG.LP.XFade);
    }
    return xml.toString();
}

void CLayer::Load(const QString& XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        QDomLiteElementList XMLRanges = xml.elementsByTag("Range");
        for (int i=XMLRanges.size();i<Ranges.count();i++)
        {
            CSampleKeyRange* KR=Ranges.last();
            Ranges.removeOne(KR);
            ActiveRanges.removeOne(KR);
            delete KR;
        }
        int i=0;
        foreach(QDomLiteElement* Range,XMLRanges)
        {
            CSampleKeyRange* KR;
            if (i>=Ranges.count())
            {
                KR=AddRange();
            }
            else
            {
                KR=Ranges[i];
            }
            QString FileName=CPresets::ResolveFilename(Range->attribute("WaveFile"));
            KR->ChangePath(FileName);
            KR->RP.UpperZero=Range->attributeValue("UpperZero");
            KR->RP.UpperTop=Range->attributeValue("UpperTop");
            KR->RP.LowerZero=Range->attributeValue("LowerZero");
            KR->RP.LowerTop=Range->attributeValue("LowerTop");
            KR->RP.Volume=Range->attributeValue("Volume");
            KR->WG.LP.Start=Range->attributeValue("Start");
            KR->WG.LP.End=Range->attributeValue("End");
            KR->WG.LP.LoopStart=Range->attributeValue("LoopStart");
            KR->WG.LP.LoopEnd=Range->attributeValue("LoopEnd");
            KR->WG.LP.MIDINote=Range->attributeValue("MIDINote");
            KR->WG.LP.Tune=Range->attributeValue("Tune");
            KR->WG.LP.LoopType=(CWaveGenerator::LoopTypeEnum)Range->attributeValue("LoopType");
            KR->WG.LP.XFade=Range->attributeValue("XFade");
            i++;
        }
    }
}

CSampleKeyRange* CLayer::Range(int Index)
{
    return Ranges[Index];
}

int CLayer::RangeCount()
{
    return Ranges.count();
}

CSampleKeyRange::RangeParams CLayer::RangeParams(int Range)
{
    return Ranges[Range]->RP;
}

void CLayer::setRangeParams(CSampleKeyRange::RangeParams RangeParams, int Range)
{
    Ranges[Range]->RP=RangeParams;
}

CWaveGenerator::LoopParameters CLayer::LoopParams(int Range)
{
    return Ranges[Range]->WG.LP;
}

void CLayer::setLoopParams(CWaveGenerator::LoopParameters LoopParams, int Range)
{
    Ranges[Range]->WG.LP=LoopParams;
}
