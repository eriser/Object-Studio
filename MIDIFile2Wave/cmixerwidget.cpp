#include "cmixerwidget.h"
#include "ui_cmixerwidget.h"

CMixerWidget::CMixerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMixerWidget)
{
    ui->setupUi(this);
    timercounter=0;
    connect(&peakTimer,SIGNAL(timeout()),this,SLOT(peak()));
    mapper=new QSignalMapper(this);
    connect(mapper,SIGNAL(mapped(int)),this,SLOT(setSoloChannel(int)));
    master=new CMasterWidget(this);
    master->hide();
    lo=new QGridLayout(this);
    lo->setSpacing(1);
    lo->setMargin(1);
    setLayout(lo);
}

CMixerWidget::~CMixerWidget()
{
    delete ui;
    qDebug() << "Exit CMixerWidget";
}

void CMixerWidget::peak()
{
    timercounter++;
    master->checkPeak();
    if (timercounter>=channels.count())
    {
        master->checkEffects();
        foreach (CSF2ChannelWidget* ch,channels)
        {
            ch->checkPreset();
            ch->checkPeak();
        }
        timercounter=0;
        return;
    }
    foreach (CSF2ChannelWidget* ch,channels)
    {
        ch->checkPeak();
    }
}

void CMixerWidget::setSoloChannel(int channel)
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

CSF2ChannelWidget* CMixerWidget::appendChannel(int index)
{
    CSF2ChannelWidget* ch=new CSF2ChannelWidget(this);
    connect(ch,SIGNAL(solo()),mapper,SLOT(map()));
    mapper->setMapping(ch,index);
    lo->addWidget(ch,0,index);
    channels.append(ch);
    return ch;
}

void CMixerWidget::removeChannel(int index)
{
    CSF2ChannelWidget* ch=channels[index];
    lo->removeWidget(ch);
    channels.removeOne(ch);
    delete ch;
}

void CMixerWidget::hideMaster()
{
    master->hide();
    lo->removeWidget(master);
}

void CMixerWidget::showMaster(CStereoMixer *mx, QList<IDevice *>& effects)
{
    master->Init(mx,&effects);
    master->show();
    lo->addWidget(master,0,channels.count());
}

void CMixerWidget::stop()
{
    peakTimer.stop();
}

void CMixerWidget::start()
{
    peakTimer.start(40+channels.count());
}

void CMixerWidget::clear()
{
    foreach (CSF2ChannelWidget* ch,channels)
    {
        lo->removeWidget(ch);
        channels.removeOne(ch);
        delete ch;
    }
    lo->removeWidget(master);
}

void CMixerWidget::resetPeak()
{
    master->resetPeak();
    foreach (CSF2ChannelWidget* ch,channels)
    {
        ch->resetPeak();
    }
}
