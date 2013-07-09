#include "csamplergenerator.h"

CSamplerGenerator::CSamplerGenerator()
{
    m_ModulationRate=CPresets::Presets.ModulationRate;
    AudioL=new float[m_ModulationRate*2];
    AudioR=AudioL + m_ModulationRate;
    ZeroMemory(AudioL,m_ModulationRate*sizeof(float)*2);
    Tune=440;
    ID=0;
    Channel=0;
    FinishedPlaying=true;
    pitchWheel=0;
    transpose=0;
    CurrentFrequency=0;
    CurrentVelocity=0;
}

CSamplerGenerator::~CSamplerGenerator()
{
    qDeleteAll(Layers);
    delete [] AudioL;
}

float* CSamplerGenerator::GetNext(float modulation)
{
    float VolFact=ADSR.GetVol(ADSR.HoldTrigger);
    if (ADSR.State==CADSR::esSilent)
    {
        FinishedPlaying=true;
        return NULL;
    }
    if (VolFact==0) return NULL;
    if (ActiveLayers.count()==0) return NULL;
    ZeroMemory(AudioL,m_ModulationRate*sizeof(float)*2);
    foreach (CLayer* L,ActiveLayers)
    {
        float TempTune=pow(2.0,(float)(L->LP.Tune)*0.001);
        float Freq=CurrentFrequency*Cent_to_Percent(((transpose+L->LP.Transpose)*100)+pitchWheel);
        L->ModifyBuffer(AudioL,AudioR,m_ModulationRate,Freq,CurrentVelocity,TempTune*modulation,VolFact*L->PlayVol);
    }
    return AudioL;
}

void CSamplerGenerator::setPitchWheel(int cent)
{
    pitchWheel=cent;
}

void CSamplerGenerator::addTranspose(int steps)
{
    transpose+=steps;
}

void CSamplerGenerator::setAftertouch(short value)
{
    float val=(value*0.001)+1;
}

void CSamplerGenerator::resetTranspose()
{
    transpose=0;
}

void CSamplerGenerator::ResetSample(short MidiNote, short MidiVelo)
{
    CurrentFrequency=MIDItoFreq(MidiNote,Tune);
    CurrentVelocity=MidiVelo/127.0;
    ADSR.HoldTrigger=1.0;
    ActiveLayers.clear();
    ADSR.GetVol(0);
    foreach (CLayer* L,Layers)
    {
        float Vol=L->GetVolume(MidiVelo);
        if (Vol > 0)
        {
            ActiveLayers.append(L);
            L->PlayVol=Vol;
            L->ResetSample(MidiNote);
        }
    }
    FinishedPlaying=false;
}

void CSamplerGenerator::EndSample()
{
    ID=0;
    ADSR.HoldTrigger=0;
    foreach (CLayer* L,ActiveLayers) L->EndSample();
}

const QString CSamplerGenerator::Save()
{
    QDomLiteElement xml("Custom");

    foreach (CLayer* L,Layers)
    {
        QDomLiteElement* Layer = xml.appendChild("Layer");
        QDomLiteElement* Custom = Layer->appendChildFromString(L->Save());

        Custom->setAttribute("UpperTop",L->LP.UpperTop);
        Custom->setAttribute("LowerTop",L->LP.LowerTop);
        Custom->setAttribute("UpperZero",L->LP.UpperZero);
        Custom->setAttribute("LowerZero",L->LP.LowerZero);
        Custom->setAttribute("Volume",L->LP.Volume);
        Custom->setAttribute("Transpose",L->LP.Transpose);
        Custom->setAttribute("Tune",L->LP.Tune);

    }
    QDomLiteElement* ADSRelement = xml.appendChild("ADSR");
    ADSRelement->appendChildFromString(ADSR.Save());
    return xml.toString();
}

void CSamplerGenerator::Load(const QString &XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    QDomLiteElementList XMLLayers = xml.elementsByTag("Layer");
    for (int i=XMLLayers.size();i<Layers.count();i++)
    {
        CLayer* L=Layers.last();
        Layers.removeOne(L);
        ActiveLayers.removeOne(L);
        delete L;
    }
    int i=0;
    foreach(QDomLiteElement* Layer,XMLLayers)
    {
        CLayer* L;
        if (i>=Layers.count())
        {
            L=new CLayer(0,0);
            Layers.append(L);
        }
        else
        {
            L=Layers[i];
        }
        QDomLiteElement* Custom = Layer->elementByTag("Custom");

        L->LP.UpperTop=Custom->attributeValue("UpperTop");
        L->LP.LowerTop=Custom->attributeValue("LowerTop");
        L->LP.UpperZero=Custom->attributeValue("UpperZero");
        L->LP.LowerZero=Custom->attributeValue("LowerZero");
        L->LP.Volume=Custom->attributeValue("Volume");
        L->LP.Transpose=Custom->attributeValue("Transpose");
        L->LP.Tune=Custom->attributeValue("Tune");

        L->Load(Layer->firstChild()->toString());
        i++;
    }
    QDomLiteElement* ADSRelement = xml.elementByTag("ADSR");
    ADSR.Load(ADSRelement->firstChild()->toString());
}

CSampleKeyRange::RangeParams CSamplerGenerator::RangeParams(int Layer, int Range)
{
    return Layers[Layer]->RangeParams(Range);
}

void CSamplerGenerator::setRangeParams(CSampleKeyRange::RangeParams RangeParams, int Layer, int Range)
{
    Layers[Layer]->setRangeParams(RangeParams,Range);
}

CWaveGenerator::LoopParameters CSamplerGenerator::LoopParams(int Layer, int Range)
{
    return Layers[Layer]->LoopParams(Range);
}

void CSamplerGenerator::setLoopParams(CWaveGenerator::LoopParameters LoopParams, int Layer, int Range)
{
    Layers[Layer]->setLoopParams(LoopParams,Range);
}

CLayer::LayerParams CSamplerGenerator::LayerParams(int Layer)
{
    return Layers[Layer]->LP;
}

void CSamplerGenerator::setLayerParams(CLayer::LayerParams LayerParams, int Layer)
{
    Layers[Layer]->LP=LayerParams;
}

CADSR::ADSRParams CSamplerGenerator::ADSRParameters()
{
    return ADSR.AP;
}

void CSamplerGenerator::setADSRParams(CADSR::ADSRParams ADSRParams)
{
    ADSR.AP=ADSRParams;
}

void CSamplerGenerator::AddRange(int Layer, const QString &WavePath, int Upper, int Lower)
{
    Layers[Layer]->AddRange(WavePath,Upper,Lower);
}

void CSamplerGenerator::ChangePath(int Layer, int Range, const QString &WavePath)
{
    Layers[Layer]->ChangePath(Range,WavePath);
}

void CSamplerGenerator::RemoveRange(int Layer, int Index)
{
    CSampleKeyRange* KR=Layers[Layer]->Range(Index);
    Layers[Layer]->RemoveRange(KR);
}

void CSamplerGenerator::AddLayer(int Upper, int Lower)
{
    CLayer* L=new CLayer(Upper,Lower);
    L->AddRange();
    Layers.append(L);
}

void CSamplerGenerator::RemoveLayer(int index)
{
    CLayer* L=Layers[index];
    Layers.removeOne(L);
    ActiveLayers.removeOne(L);
    delete L;
}

CLayer* CSamplerGenerator::Layer(int Index)
{
    return Layers[Index];
}

int CSamplerGenerator::LayerCount()
{
    return Layers.count();
}

int CSamplerGenerator::RangeCount(int Layer)
{
    return Layers[Layer]->RangeCount();
}
