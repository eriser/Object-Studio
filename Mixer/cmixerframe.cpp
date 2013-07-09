#include "cmixerframe.h"
#include "softsynthsclasses.h"
#include "ui_cmixerframe.h"

CMixerFrame::CMixerFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CMixerFrame)
{
    ui->setupUi(this);
    Mixer=NULL;
    connect(ui->VolSlider,SIGNAL(valueChanged(int)),this,SLOT(VolChanged(int)));
    connect(ui->PanDial,SIGNAL(valueChanged(int)),this,SLOT(PanChanged(int)));
    connect(ui->EffectDial,SIGNAL(valueChanged(int)),this,SLOT(EffectChanged(int)));
    connect(ui->BypassButton,SIGNAL(clicked(bool)),this,SLOT(BypassButtonClicked(bool)));
    connect(ui->SoloButton,SIGNAL(clicked(bool)),this,SLOT(SoloButtonClicked(bool)));
    connect(ui->MuteButton,SIGNAL(clicked(bool)),this,SLOT(MuteButtonClicked(bool)));
    ui->VolLabel->setText(QString::number(lin2db((float)ui->VolSlider->value()*0.01),'f',2)+" dB");
}

CMixerFrame::~CMixerFrame()
{
    delete ui;
}

void CMixerFrame::Init(CMixer* MixerClass,int ChannelIndex)
{
    Mixer=MixerClass;
    Index=ChannelIndex;
    ui->VolSlider->setValue(Mixer->Level[Index]*100);

    //VemodKnob1->Init(Numeric,"Pan","",-100,100,0,"",0);
    //VemodKnob2->Init(Numeric,"Send","",0,100,0,"",0);
    ui->IndexLabel->setText(QString::number(Index+1));
}

void CMixerFrame::Peak(float Value)
{
        ui->Peak->SetValue(Value);
}

void CMixerFrame::Reset()
{
    ui->Peak->Reset();
}

void CMixerFrame::SetSolo(bool Value)
{
    ui->SoloButton->setChecked(Value);
}

void CMixerFrame::PanChanged(int Value)
{
    if (!Mixer)
    {
        return;
    }
    float PanValue=(float)Value*0.01;
    if (PanValue<0)
    {
        Mixer->PanL[Index]=1;
        Mixer->PanR[Index]=1+PanValue;
    }
    else
    {
        Mixer->PanR[Index]=1;
        Mixer->PanL[Index]=1-PanValue;
    }
}
//---------------------------------------------------------------------------

void CMixerFrame::EffectChanged(int Value)
{
    if (!Mixer)
    {
        return;
    }
    Mixer->Effect[Index]=(float)Value*0.01;
}
//---------------------------------------------------------------------------

const QString CMixerFrame::Save()
{
    QDomLiteElement xml("Custom");
                xml.setAttribute("Volume",ui->VolSlider->value());
                xml.setAttribute("Pan",ui->PanDial->value());
                xml.setAttribute("Send",ui->EffectDial->value());
                xml.setAttribute("Mute",(int)Mixer->Mute[Index]);
                xml.setAttribute("EffectMute",(int)Mixer->EffectMute[Index]);
        return xml.toString();
}

void CMixerFrame::Load(const QString& XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
        if (xml.tag=="Custom")
        {
                ui->VolSlider->setValue(xml.attributeValue("Volume"));
                Mixer->Level[Index]=(float)ui->VolSlider->value()*0.01;
                ui->PanDial->setValue(xml.attributeValue("Pan"));
                ui->EffectDial->setValue(xml.attributeValue("Send"));
                Mixer->Mute[Index]=xml.attributeValue("Mute");
                Mixer->EffectMute[Index]=xml.attributeValue("EffectMute");
        }

    ui->SoloButton->setChecked(Mixer->SoloChannel==Index);
    ui->MuteButton->setChecked(Mixer->Mute[Index]);
    ui->BypassButton->setChecked(Mixer->EffectMute[Index]);
}


void CMixerFrame::VolChanged(int Value)
{
    ui->VolLabel->setText(QString::number(lin2db((float)Value*0.01),'f',2)+" dB");
    if (!Mixer)
    {
        return;
    }
    Mixer->Level[Index]=(float)Value*0.01;
}
//---------------------------------------------------------------------------

void CMixerFrame::MuteButtonClicked(bool Value)
{
    if (!Mixer)
    {
        return;
    }
    Mixer->Mute[Index]=Value;
}
//---------------------------------------------------------------------------

void CMixerFrame::SoloButtonClicked(bool Value)
{
    if (!Mixer)
    {
        return;
    }
    emit SoloClicked(Value,Index);
}
//---------------------------------------------------------------------------

void CMixerFrame::BypassButtonClicked(bool Value)
{
    if (!Mixer)
    {
        return;
    }
    Mixer->EffectMute[Index]=Value;
}
