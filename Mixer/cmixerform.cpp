#include "cmixerform.h"
#include "ui_cmixerform.h"
#include "softsynthsclasses.h"

CMixerForm::CMixerForm(IDevice* Device, QWidget *parent) :
    CSoftSynthsForm(Device,true,parent),
    ui(new Ui::CMixerForm)
{
    ui->setupUi(this);
    for (int i=0;i<Mixer::mixerchannels;i++)
    {
        MF=findChildren<CMixerFrame*>();
        MF[i]->Init((CMixer*)m_Device,i);
        connect(MF[i],SIGNAL(SoloClicked(bool,int)),this,SLOT(SoloClicked(bool,int)));
    }
    startTimer(50);
    connect(ui->LeftSlider,SIGNAL(valueChanged(int)),this,SLOT(LeftChanged(int)));
    connect(ui->RightSlider,SIGNAL(valueChanged(int)),this,SLOT(RightChanged(int)));
    ui->LeftSlider->setValue(((CMixer*)m_Device)->MasterLeft*100);
    ui->RightSlider->setValue(((CMixer*)m_Device)->MasterRight*100);

}

CMixerForm::~CMixerForm()
{
    delete ui;
}

void CMixerForm::timerEvent(QTimerEvent* /*event*/)
{
    if (isVisible())
    {
        float P[Mixer::mixerchannels];
        float L;
        float R;
        ((CMixer*)m_Device)->GetPeak(P,&L,&R);
        ui->StereoPeak->SetValues(L,R);
        for (int i=0;i<Mixer::mixerchannels;i++)
        {
            MF[i]->Peak(P[i]);
        }
    }
}

void CMixerForm::Reset()
{
    ui->StereoPeak->Reset();
    for (int i=0;i<Mixer::mixerchannels;i++)
    {
        MF[i]->Reset();
    }
}

void CMixerForm::CustomLoad(const QString &XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        QDomLiteElement* Master = xml.elementByTag("Master");
        if (Master)
        {
            ui->RightSlider->setValue(Master->attributeValue("VolumeRight"));
            ui->LeftSlider->setValue(Master->attributeValue("VolumeLeft"));
            ((CMixer*)m_Device)->SoloChannel = Master->attributeValue("SoloChannel");
            ui->LockButton->setChecked(Master->attributeValue("ChannelLock"));
        }
        for (int i=0;i<Mixer::mixerchannels;i++)
        {
            QDomLiteElement* Channel = xml.elementByTag("Channel" + QString::number(i+1));
            if (Channel) MF[i]->Load(Channel->firstChild()->toString());
        }
    }
}

const QString CMixerForm::CustomSave()
{
    QDomLiteElement xml("Custom");
    QDomLiteElement* Master = xml.appendChild("Master");
    Master->setAttribute("VolumeRight",ui->RightSlider->value());
    Master->setAttribute("VolumeLeft",ui->LeftSlider->value());
    Master->setAttribute("ChannelLock",ui->LockButton->isChecked());
    Master->setAttribute("SoloChannel",((CMixer*)m_Device)->SoloChannel);

    for (int i=0;i<Mixer::mixerchannels;i++)
    {
        QDomLiteElement* Channel = xml.appendChild("Channel" + QString::number(i+1));
        Channel->appendChildFromString(MF[i]->Save());
    }
    return xml.toString();
}

void CMixerForm::LeftChanged(int Value)
{
    if (!(CMixer*)m_Device)
    {
        return;
    }
    ((CMixer*)m_Device)->MasterLeft=(float)Value*0.01;

    if (ui->LockButton->isChecked())
    {
        ui->RightSlider->blockSignals(true);
        ui->RightSlider->setValue(Value);
        ui->RightSlider->blockSignals(false);
        ((CMixer*)m_Device)->MasterRight=(float)Value*0.01;
    }
    ui->LeftLabel->setText(QString::number(lin2db((float)ui->LeftSlider->value()*0.01),'f',2)+" dB");
    ui->RightLabel->setText(QString::number(lin2db((float)ui->RightSlider->value()*0.01),'f',2)+" dB");

}

//---------------------------------------------------------------------------

void CMixerForm::RightChanged(int Value)
{
    if (!(CMixer*)m_Device)
    {
        return;
    }
    ((CMixer*)m_Device)->MasterRight=(float)Value*0.01;
    if (ui->LockButton->isChecked())
    {
        ui->LeftSlider->blockSignals(true);
        ui->LeftSlider->setValue(Value);
        ui->LeftSlider->blockSignals(false);
        ((CMixer*)m_Device)->MasterLeft=(float)Value*0.01;
    }
    ui->LeftLabel->setText(QString::number(lin2db((float)ui->LeftSlider->value()*0.01),'f',2)+" dB");
    ui->RightLabel->setText(QString::number(lin2db((float)ui->RightSlider->value()*0.01),'f',2)+" dB");

}

void CMixerForm::SoloClicked(bool Pressed, int Index)
{
    if (Pressed)
    {
        for (int i=0;i<Mixer::mixerchannels;i++)
        {
            if (i != Index) MF[i]->SetSolo(false);
        }
        ((CMixer*)m_Device)->SoloChannel=Index;
    }
    else
    {
        ((CMixer*)m_Device)->SoloChannel=-1;
    }
}
