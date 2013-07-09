#include "cequalizerframe.h"
#include "ui_cequalizerframe.h"

CEqualizerFrame::CEqualizerFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CEqualizerFrame)
{
    ui->setupUi(this);
    connect(ui->VolSlider,SIGNAL(valueChanged(int)),this,SLOT(VolChanged(int)));
    connect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(FreqChanged(int)));
    m_Device=NULL;
}

CEqualizerFrame::~CEqualizerFrame()
{
    delete ui;
}

void CEqualizerFrame::Init(CEqualizer *EQ, int BandIndex, int FqMin, int FqMax, int FqDefault)
{
    m_Device=EQ;
    Index=BandIndex;
    ui->FreqLabel->setText("Freq.");
    ui->dial->setMinimum(FqMin);
    ui->dial->setMaximum(FqMax);
    ui->dial->setValue(FqDefault);
    ui->VolSlider->setValue(100);
    VolChanged(100);
    ui->IndexLabel->setText("Band " + QString::number(Index+1));
}

void CEqualizerFrame::VolChanged(int Value)
{
    if (!m_Device)
    {
        return;
    }
    float dB=lin2db((float)Value*0.01);
    m_Device->SetLevel(Index,dB);
    ui->VolLabel->setText(QString::number(dB,'f',2)+"dB");
}

void CEqualizerFrame::FreqChanged(int Freq)
{
    if (!m_Device)
    {
        return;
    }
    m_Device->SetFreq(Index,Freq);
    ui->ValueLabel->setText(QString::number(Freq)+"Hz");
}

const QString CEqualizerFrame::Save()
{
    QDomLiteElement xml("Custom");
    xml.setAttribute("Volume",ui->VolSlider->value());
    xml.setAttribute("Frequency",ui->dial->value());
    return xml.toString();
}

void CEqualizerFrame::Load(const QString& XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        ui->VolSlider->setValue(xml.attributeValue("Volume"));
        ui->dial->setValue(xml.attributeValue("Frequency"));
        m_Device->SetLevel(Index,lin2db((float)(ui->VolSlider->value())*0.01));
        m_Device->SetFreq(Index,ui->dial->value());
    }
}
