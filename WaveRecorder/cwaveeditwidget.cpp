#include "cwaveeditwidget.h"
#include "ui_cwaveeditwidget.h"

CWaveEditWidget::CWaveEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWaveEditWidget)
{
    ui->setupUi(this);
    connect(ui->StartSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->EndSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->FadeInSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->FadeOutSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->VolSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));

    connect(ui->ScrollBar,SIGNAL(valueChanged(int)),this,SLOT(SetStart()));

    connect(ui->ZoomInButton,SIGNAL(clicked()),this,SLOT(ZoomIn()));
    connect(ui->ZoomOutButton,SIGNAL(clicked()),this,SLOT(ZoomOut()));
    connect(ui->ZoomMinButton,SIGNAL(clicked()),this,SLOT(ZoomMin()));
    connect(ui->ZoomMaxButton,SIGNAL(clicked()),this,SLOT(ZoomMax()));

    connect(ui->WaveEdit,SIGNAL(ParameterChanged(CWaveGenerator::LoopParameters)),this,SLOT(UpdateControls(CWaveGenerator::LoopParameters)));
    connect(ui->WaveEdit,SIGNAL(ParameterChanged(CWaveGenerator::LoopParameters)),this,SIGNAL(UpdateHost(CWaveGenerator::LoopParameters)));
}

CWaveEditWidget::~CWaveEditWidget()
{
    delete ui;
}

void CWaveEditWidget::Init(CWaveGenerator *WG, CWaveGenerator::LoopParameters LP, bool LoopOn, bool Enabled)
{
    m_WG=WG;
    m_LoopOn=LoopOn;
    ui->WaveEdit->Enabled=Enabled;
    foreach(QWidget* w,findChildren<QWidget*>()) w->setEnabled(Enabled);
    if (!Enabled) return;
    if (LP.End>WG->GetLength())
    {
        LP.End=WG->GetLength();
    }
    if (LP.Start>WG->GetLength())
    {
        LP.Start=WG->GetLength();
    }
    if (LP.LoopEnd>WG->GetLength())
    {
        LP.LoopEnd=WG->GetLength();
    }
    if (LP.LoopStart>WG->GetLength())
    {
        LP.LoopStart=WG->GetLength();
    }
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(true);
    ui->VolSpin->setValue(LP.Volume);
    ui->StartSpin->setMaximum(WG->GetLength());
    ui->EndSpin->setMaximum(WG->GetLength());
    ui->FadeInSpin->setMaximum(WG->GetLength());
    ui->FadeOutSpin->setMaximum(WG->GetLength());
    ui->StartSpin->setValue(LP.Start);
    ui->EndSpin->setValue(LP.End);
    if (LoopOn)
    {
        ui->FadeInSpin->setValue(LP.LoopStart);
        ui->FadeOutSpin->setValue(LP.LoopEnd);
        ui->FadeInLabel->setText("Loop Start");
        ui->FadeOutLabel->setText("Loop End");
        ui->VolSpin->setVisible(false);
        ui->VolLabel->setText("");
    }
    else
    {
        ui->FadeInSpin->setValue(LP.FadeIn);
        ui->FadeOutSpin->setValue(LP.FadeOut);
        ui->VolSpin->setVisible(true);
        ui->VolLabel->setText("Volume");
    }
    ui->WaveEdit->Init(WG,LP,LoopOn);
    ui->ScrollBar->setMaximum(ui->WaveEdit->ScrollMax());
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(false);
}

void CWaveEditWidget::SetScrollMax()
{
    ui->ScrollBar->blockSignals(true);
    if (ui->ScrollBar->value()>ui->WaveEdit->ScrollMax())
    {
        ui->ScrollBar->setValue(ui->WaveEdit->ScrollMax());
    }
    ui->ScrollBar->setMaximum(ui->WaveEdit->ScrollMax());
    int SampleCount=ui->WaveEdit->Zoom*ui->WaveEdit->width();
    ui->ScrollBar->setSingleStep(ui->WaveEdit->Zoom);
    ui->ScrollBar->setPageStep(SampleCount);
    ui->ScrollBar->blockSignals(false);
}

void CWaveEditWidget::SetStart()
{
    SetScrollMax();
    ui->WaveEdit->SetStart(ui->ScrollBar->value());
}

void CWaveEditWidget::ZoomIn()
{
    ui->WaveEdit->ZoomIn();
    SetScrollMax();
}

void CWaveEditWidget::ZoomOut()
{
    ui->WaveEdit->ZoomOut();
    SetScrollMax();
}

void CWaveEditWidget::ZoomMax()
{
    ui->WaveEdit->ZoomMax();
    SetScrollMax();
}

void CWaveEditWidget::ZoomMin()
{
    ui->WaveEdit->ZoomMin();
    SetScrollMax();
}

void CWaveEditWidget::UpdateGraph()
{
    CWaveGenerator::LoopParameters LP=m_WG->LP;
    LP.Volume=ui->VolSpin->value();
    if (m_LoopOn)
    {
        LP.Start=ui->StartSpin->value();
        LP.End=ui->EndSpin->value();
        LP.LoopStart=ui->FadeInSpin->value();
        LP.LoopEnd=ui->FadeOutSpin->value();
    }
    else
    {
        ui->StartSpin->setMaximum(ui->EndSpin->value()-(ui->FadeInSpin->value()+ui->FadeOutSpin->value()));
        LP.Start=ui->StartSpin->value();
        ui->EndSpin->setMinimum(ui->StartSpin->value()+ui->FadeInSpin->value()+ui->FadeOutSpin->value());
        LP.End=ui->EndSpin->value();
        ui->FadeInSpin->setMaximum((ui->EndSpin->value()-ui->StartSpin->value())-ui->FadeOutSpin->value());
        LP.FadeIn=ui->FadeInSpin->value();
        ui->FadeOutSpin->setMaximum((ui->EndSpin->value()-ui->StartSpin->value())-ui->FadeInSpin->value());
        LP.FadeOut=ui->FadeOutSpin->value();
    }
    ui->WaveEdit->Draw(LP);
    emit UpdateHost(LP);
}

void CWaveEditWidget::UpdateControls(CWaveGenerator::LoopParameters LP)
{
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(true);
    ui->VolSpin->setValue(LP.Volume);
    ui->StartSpin->setValue(LP.Start);
    ui->EndSpin->setValue(LP.End);
    if (m_LoopOn)
    {
        ui->FadeInSpin->setValue(LP.LoopStart);
        ui->FadeOutSpin->setValue(LP.LoopEnd);
    }
    else
    {
        ui->FadeInSpin->setValue(LP.FadeIn);
        ui->FadeOutSpin->setValue(LP.FadeOut);
    }
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(false);
}
