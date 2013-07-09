#include "csf2channelwidget.h"
#include "ui_csf2channelwidget.h"

CSF2ChannelWidget::CSF2ChannelWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CSF2ChannelWidget)
{
    ui->setupUi(this);
    mapper=new QSignalMapper(this);
    preset=-1;
    bank=-1;
    m_Ch=NULL;
    ui->VolLabel->setText("0.00 dB");
    ui->Bank->setEffect(EffectLabel::Raised);
    ui->Bank->setShadowColor(Qt::white);
    ui->Patch->setEffect(EffectLabel::Raised);
    ui->Patch->setShadowColor(Qt::white);
    ui->label_3->setEffect(EffectLabel::Raised);
    ui->label_3->setShadowColor(Qt::white);
    ui->Name->setEffect(EffectLabel::Raised);
    ui->Name->setShadowColor(Qt::darkGray);
    ui->VolLabel->setEffect(EffectLabel::Raised);
    ui->VolLabel->setShadowColor(Qt::darkGray);
    connect(ui->SF2,SIGNAL(clicked()),this,SLOT(loadDialog()));
    connect(ui->verticalSlider,SIGNAL(valueChanged(int)),this,SLOT(setVolume(int)));
    connect(ui->Pan,SIGNAL(valueChanged(int)),this,SLOT(setPan(int)));
    connect(ui->Bypass,SIGNAL(toggled(bool)),this,SLOT(setBypass(bool)));
    connect(ui->Mute,SIGNAL(toggled(bool)),this,SLOT(setMute(bool)));
    connect(ui->Solo,SIGNAL(toggled(bool)),this,SIGNAL(solo()));
    connect(mapper,SIGNAL(mapped(int)),this,SLOT(setEffect(int)));
}

CSF2ChannelWidget::~CSF2ChannelWidget()
{
    delete ui;
}

void CSF2ChannelWidget::Init(CStereoMixerChannel *ch, CSF2Player *SF2, short MIDIChannel, QString Name)
{
    ui->Name->setText(Name);
    m_Ch=ch;
    m_SF2=SF2;
    m_MIDIChannel=MIDIChannel;
    for (int i=Effect.count();i<ch->sendCount;i++)
    {
        QDial* d=new QDial(this);
        Effect.append(d);
        d->setMaximumSize(36,36);
        d->setMaximum(100);
        d->setValue(100);
        d->setNotchesVisible(true);
        EffectLabel* l=new EffectLabel(this);
        l->setEffect(EffectLabel::Raised);
        l->setShadowColor(Qt::white);
        l->setMaximumHeight(13);
        QFont f=l->font();
        f.setPointSize(9);
        l->setFont(f);
        l->setText("Effect "+QString::number(i+1));
        l->setStyleSheet("background:transparent;");
        ui->EffectLayout->addWidget(d,0,Qt::AlignHCenter);
        ui->EffectLayout->addWidget(l,0,Qt::AlignHCenter);
        connect(d,SIGNAL(sliderReleased()),mapper,SLOT(map()));
        mapper->setMapping(d,i);
    }
    this->setMinimumHeight(424+(50*ch->sendCount));
    ui->verticalSlider->setValue(100);
    ui->Pan->setValue(100);
    ui->Mute->setChecked(false);
    ui->Solo->setChecked(false);
    ui->Bypass->setChecked(false);
    foreach (QDial* d,Effect) d->setValue(100);
}


void CSF2ChannelWidget::checkPeak()
{
    ui->PeakLeft->SetValue(m_Ch->PeakL);
    ui->PeakRight->SetValue(m_Ch->PeakR);
    m_Ch->PeakL=0;
    m_Ch->PeakR=0;
}

void CSF2ChannelWidget::resetPeak()
{
    ui->PeakLeft->Reset();
    ui->PeakRight->Reset();
    m_Ch->PeakL=0;
    m_Ch->PeakR=0;
}

void CSF2ChannelWidget::checkPreset()
{
    short p=m_SF2->SF2Device.currentPreset(m_MIDIChannel);
    short b=m_SF2->SF2Device.currentBank(m_MIDIChannel);
    if (bank != b)
    {
        bank=b;
        ui->Bank->setText("Bank "+QString::number(b));
        preset=p;
        ui->Patch->setText(m_SF2->SF2Device.banks[b].presets[p].name);
        return;
    }
    if (preset != p)
    {
        preset=p;
        ui->Patch->setText(m_SF2->SF2Device.banks[b].presets[p].name);
    }
}

void CSF2ChannelWidget::setVolume(int Vol)
{
    m_Ch->Level=(float)Vol*0.01;
    ui->VolLabel->setText(QString::number(lin2db((float)Vol*0.01),'f',2)+" dB");
}

void CSF2ChannelWidget::setPan(int Pan)
{
    if (Pan<=100)
    {
        m_Ch->PanL=1;
        m_Ch->PanR=Pan*0.01;
    }
    else
    {
        m_Ch->PanR=1;
        m_Ch->PanL=(100-(Pan-100))*0.01;
    }
}

void CSF2ChannelWidget::setMute(bool Mute)
{
    m_Ch->Mute=Mute;
}

void CSF2ChannelWidget::setBypass(bool Bypass)
{
    m_Ch->EffectMute=Bypass;
}

void CSF2ChannelWidget::setEffect(int effNumber)
{
    m_Ch->Effect[effNumber]=Effect[effNumber]->value()*0.01;
}

void CSF2ChannelWidget::loadSF(QString filename)
{
    if (m_SF2->FileName() != filename) m_SF2->Load("<Custom File=\""+filename+"\"/>");
    if (m_MIDIChannel>-1) m_SF2->SetParameterValue(0,m_MIDIChannel+1);
    m_SF2->SetParameterValue(3,1);
    ui->SF2->setText(QFileInfo(m_SF2->FileName()).completeBaseName());
    checkPreset();
}

void CSF2ChannelWidget::loadDialog()
{
    QString fn=m_SF2->OpenFile(SF2File::SF2Filter);
    qDebug() << fn;
    if (!fn.isEmpty()) loadSF(fn);
}

void CSF2ChannelWidget::soloButton(bool pressed)
{
    ui->Solo->blockSignals(true);
    ui->Solo->setChecked(pressed);
    ui->Solo->blockSignals(false);
}

const QString CSF2ChannelWidget::Save()
{
    QDomLiteElement Channel("Channel");
    Channel.setAttribute("Volume",ui->verticalSlider->value());
    Channel.setAttribute("Pan",ui->Pan->value());
    Channel.setAttribute("Mute",ui->Mute->isChecked());
    Channel.setAttribute("Bypass",ui->Bypass->isChecked());
    Channel.setAttribute("Solo",ui->Solo->isChecked());
    for (int i=0;i<Effect.count();i++) Channel.setAttribute("Effect"+QString::number(i+1),Effect[i]->value());
    Channel.appendChildFromString(m_SF2->Save());
    return Channel.toString();
}

void CSF2ChannelWidget::Load(const QString& XML)
{
    QDomLiteElement Channel("Channel");
    Channel.fromString(XML);
    ui->verticalSlider->setValue(Channel.attributeValue("Volume"));
    ui->Pan->setValue(Channel.attributeValue("Pan"));
    ui->Mute->setChecked(Channel.attributeValue("Mute"));
    ui->Bypass->setChecked(Channel.attributeValue("Bypass"));
    ui->Solo->setChecked(Channel.attributeValue("Solo"));
    m_SF2->Load(Channel.firstChild()->toString());
    ui->SF2->setText(QFileInfo(m_SF2->FileName()).completeBaseName());
    for (int i=0;i<Effect.count();i++) Effect[i]->setValue(Channel.attributeValue("Effect"+QString::number(i+1),100));
    checkPeak();
    checkPreset();
}
