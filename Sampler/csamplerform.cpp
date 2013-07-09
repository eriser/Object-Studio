#include "csamplerform.h"
#include "ui_csamplerform.h"

CSamplerForm::CSamplerForm(IDevice* Device, QWidget *parent) :
    CSoftSynthsForm(Device,true,parent),
    ui(new Ui::CSamplerForm)
{
    ui->setupUi(this);
}

CSamplerForm::~CSamplerForm()
{
    delete ui;
}

void CSamplerForm::Init(CSamplerDevice *Device)
{
    m_Sampler=Device;
    ui->WaveLayers->Init(Device);
    ui->WaveLayers->Update();
}

void CSamplerForm::CustomLoad(const QString &XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        QDomLiteElement* Layers = xml.elementByTag("Layers");
        m_Sampler->Load(Layers->firstChild()->toString());
    }
    ui->WaveLayers->Update();
}

const QString CSamplerForm::CustomSave()
{
    QDomLiteElement xml("Custom");
    QDomLiteElement* Layers = xml.appendChild("Layers");
    Layers->appendChildFromString(m_Sampler->Save());

    return xml.toString();
}

void CSamplerForm::ReleaseLoop()
{
    ui->WaveLayers->ReleaseLoop();
}
