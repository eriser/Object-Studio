#include "cstereomixerform.h"
#include "ui_cstereomixerform.h"
#include "softsynthsclasses.h"
#include <QHBoxLayout>

CStereoMixerForm::CStereoMixerForm(IDevice *Device, QWidget *parent) :
    CSoftSynthsForm(Device, true, parent),
    ui(new Ui::CStereoMixerForm)
{
    ui->setupUi(this);

    connect(&peakTimer,SIGNAL(timeout()),this,SLOT(peak()));
    mapper=new QSignalMapper(this);
    connect(mapper,SIGNAL(mapped(int)),this,SLOT(setSoloChannel(int)));

    this->setLayout(new QHBoxLayout(this));
    this->layout()->setMargin(1);
    this->layout()->setSpacing(1);
    for (int i=0;i<((CStereoMixer*)Device)->channelCount;i++)
    {
        CStereoChannelWidget* ch=new CStereoChannelWidget(this);
        channels.append(ch);
        ch->Init(((CStereoMixer*)Device)->channels[i],QString::number(i+1));
        connect(ch,SIGNAL(solo()),mapper,SLOT(map()));
        mapper->setMapping(ch,i);
        this->layout()->addWidget(ch);
    }
    master=new CMasterWidget(this);
    master->Init((CStereoMixer*)Device);
    this->layout()->addWidget(master);
    adjustSize();
    setFixedSize(size());
    peakTimer.start(40+channels.count());
}

CStereoMixerForm::~CStereoMixerForm()
{
    delete ui;
}

void CStereoMixerForm::Reset()
{
    master->resetPeak();
    foreach (CStereoChannelWidget* ch,channels)
    {
        ch->resetPeak();
    }
}

void CStereoMixerForm::peak()
{
    master->checkPeak();
    foreach (CStereoChannelWidget* ch,channels)
    {
        ch->checkPeak();
    }
}

void CStereoMixerForm::setSoloChannel(int channel)
{
    QToolButton* b = channels[channel]->findChild<QToolButton*>("Solo");
    if (b->isChecked())
    {
        for (int i=0;i<channels.count();i++)
        {
            if (i != channel) channels[i]->soloButton(false);
        }
        master->setSoloChannel(channel);
    }
    else
    {
        master->setSoloChannel(-1);
    }
}

void CStereoMixerForm::CustomLoad(const QString &XML)
{
    QDomLiteElement Custom("Custom");
    Custom.fromString(XML);
    for (int i=0;i<channels.count();i++)
    {
        QDomLiteElement* ch=Custom.elementByTag("Channel"+QString::number(i));
        if (ch != 0)
        {
            channels[i]->Load(ch->firstChild()->toString());
        }
    }
    QDomLiteElement* Master=Custom.elementByTag("Master");
    if (Master != 0)
    {
        master->Load(Master->toString());
    }
}

const QString CStereoMixerForm::CustomSave()
{
    QDomLiteElement Custom("Custom");
    for (int i=0;i<channels.count();i++)
    {
        QDomLiteElement* ch=Custom.appendChild("Channel"+QString::number(i));
        ch->appendChildFromString(channels[i]->Save());
    }
    Custom.appendChildFromString(master->Save());
    return Custom.toString();
}
