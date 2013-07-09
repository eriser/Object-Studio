#include "cmasterwidget.h"
#include "ui_cmasterwidget.h"

CMasterWidget::CMasterWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CMasterWidget)
{
    ui->setupUi(this);
    mapper=new QSignalMapper(this);
    dialMapper=new QSignalMapper(this);
    ui->label->setEffect(EffectLabel::Raised);
    ui->label->setShadowColor(Qt::darkGray);
    ui->VolLabelL->setEffect(EffectLabel::Raised);
    ui->VolLabelL->setShadowColor(Qt::darkGray);
    ui->VolLabelR->setEffect(EffectLabel::Raised);
    ui->VolLabelR->setShadowColor(Qt::darkGray);

    ui->VolLabelL->setText("0.00 dB");
    ui->VolLabelR->setText("0.00 dB");
    connect(ui->VolL,SIGNAL(valueChanged(int)),this,SLOT(setVolL(int)));
    connect(ui->VolR,SIGNAL(valueChanged(int)),this,SLOT(setVolR(int)));
    connect(dialMapper,SIGNAL(mapped(int)),this,SLOT(effectVol(int)));
    connect(mapper,SIGNAL(mapped(int)),this,SLOT(showEffect(int)));
}

CMasterWidget::~CMasterWidget()
{
    delete ui;
}

void CMasterWidget::Init(CStereoMixer *mx, QList<IDevice *>* effects)
{
    m_Mx=mx;
    m_Fx=effects;
    for (int i=dials.count();i<m_Mx->sendCount;i++)
    {
        QDial* d=new QDial(this);
        d->setMaximumSize(36,36);
        d->setMaximum(200);
        d->setValue(100);
        d->setNotchesVisible(true);
        dials.append(d);
        connect(d,SIGNAL(sliderReleased()),dialMapper,SLOT(map()));
        dialMapper->setMapping(d,i);
        ui->EffectLayout->addWidget(d,0,Qt::AlignHCenter);
        if (m_Fx==NULL)
        {
            EffectLabel* e=new EffectLabel(this);
            e->setEffect(EffectLabel::Raised);
            e->setShadowColor(Qt::white);
            e->setStyleSheet("background:transparent;");
            e->setText("Effect "+QString::number(i+1));
            e->setMaximumHeight(13);
            QFont f=e->font();
            f.setPixelSize(9);
            e->setFont(f);
            ui->EffectLayout->addWidget(e,0,Qt::AlignHCenter);
        }
        else
        {
            QToolButton* b=new QToolButton(this);
            m_Buttons.append(b);
            m_Names.append(m_Fx->at(i)->FileName());
            b->setText("Effect "+QString::number(i+1));
            b->setMaximumHeight(13);
            //b->setFlat(true);
            b->setStyleSheet("background:transparent;color:blue;");
            b->setCursor(Qt::PointingHandCursor);
            QFont f=b->font();
            f.setPixelSize(9);
            f.setUnderline(true);
            b->setFont(f);
            if (i<m_Fx->count())
            {
                b->setText(QFileInfo(m_Fx->at(i)->FileName()).completeBaseName());
            }
            if (b->text().isEmpty()) b->setText("Effect "+QString::number(i+1));
            connect(b,SIGNAL(clicked()),mapper,SLOT(map()));
            mapper->setMapping(b,i);
            ui->EffectLayout->addWidget(b,0,Qt::AlignHCenter);
        }
    }
    ui->Lock->setChecked(true);
    ui->VolL->setValue(100);
    foreach (QDial* d,dials) d->setValue(100);
}

void CMasterWidget::showEffect(int eff)
{
    m_Fx->at(eff)->Execute(true);
    m_Fx->at(eff)->RaiseForm();
}

void CMasterWidget::effectVol(int eff)
{
    m_Mx->Sends[eff]=dials[eff]->value()*0.01;
}

void CMasterWidget::checkPeak()
{
    ui->Peak->SetValues(m_Mx->PeakL,m_Mx->PeakR);
    m_Mx->PeakL=0;
    m_Mx->PeakR=0;
}

void CMasterWidget::resetPeak()
{
    ui->Peak->Reset();
    m_Mx->PeakL=0;
    m_Mx->PeakR=0;
}

void CMasterWidget::setVolL(int vol)
{
    m_Mx->MasterLeft=vol*0.01;
    ui->VolLabelL->setText(QString::number(lin2db((float)vol*0.01),'f',2)+" dB");
    if (ui->Lock->isChecked())
    {
        ui->VolR->blockSignals(true);
        ui->VolR->setValue(vol);
        m_Mx->MasterRight=vol*0.01;
        ui->VolLabelR->setText(QString::number(lin2db((float)vol*0.01),'f',2)+" dB");
        ui->VolR->blockSignals(false);
    }
}

void CMasterWidget::setVolR(int vol)
{
    m_Mx->MasterRight=vol*0.01;
    ui->VolLabelR->setText(QString::number(lin2db((float)vol*0.01),'f',2)+" dB");
    if (ui->Lock->isChecked())
    {
        ui->VolL->blockSignals(true);
        ui->VolL->setValue(vol);
        m_Mx->MasterLeft=vol*0.01;
        ui->VolLabelL->setText(QString::number(lin2db((float)vol*0.01),'f',2)+" dB");
        ui->VolL->blockSignals(false);
    }
}

void CMasterWidget::checkEffects()
{
    for (int i=0;i<m_Fx->count();i++)
    {
        if (m_Fx->at(i)->FileName() != m_Names[i])
        {
            m_Names[i]=m_Fx->at(i)->FileName();
            m_Buttons[i]->setText(QFileInfo(m_Names[i]).completeBaseName());
            if (m_Buttons[i]->text().isEmpty()) m_Buttons[i]->setText("Effect "+QString::number(i+1));
        }
    }
}

void CMasterWidget::setSoloChannel(int channel)
{
    m_Mx->SoloChannel=channel;
}

const QString CMasterWidget::Save()
{
    QDomLiteElement Master("Master");
    Master.setAttribute("Lock",ui->Lock->isChecked());
    Master.setAttribute("Volume Left",ui->VolL->value());
    Master.setAttribute("Volume Right",ui->VolR->value());
    for (int i=0;i<dials.count();i++) Master.setAttribute("Effect "+QString::number(i+1),dials[i]->value());
    return Master.toString();
}

void CMasterWidget::Load(const QString& XML)
{
    QDomLiteElement Master("Master");
    Master.fromString(XML);
    ui->Lock->setChecked(Master.attributeValue("Lock"));
    ui->VolL->setValue(Master.attributeValue("Volume Left"));
    ui->VolR->setValue(Master.attributeValue("Volume Right"));
    for (int i=0;i<dials.count();i++) dials[i]->setValue(Master.attributeValue("Effect "+QString::number(i+1),100));
}
