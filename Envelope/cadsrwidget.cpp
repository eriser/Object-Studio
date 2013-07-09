#include "cadsrwidget.h"
#include "ui_cadsrwidget.h"

CADSRWidget::CADSRWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CADSRWidget)
{
    ui->setupUi(this);
    ui->Attack->setMaximum(ADSRControl::ADSR_MaxTime);
    ui->Decay->setMaximum(ADSRControl::ADSR_MaxTime);
    ui->Release->setMaximum(ADSRControl::ADSR_MaxTime);

    connect(ui->ADSRControl,SIGNAL(Changed(CADSR::ADSRParams)),this,SIGNAL(Changed(CADSR::ADSRParams)));
    connect(ui->ADSRControl,SIGNAL(Changed(CADSR::ADSRParams)),this,SLOT(UpdateControls(CADSR::ADSRParams)));
    connect(ui->Attack,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->Decay,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->Sustain,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->Release,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
}

CADSRWidget::~CADSRWidget()
{
    delete ui;
}

void CADSRWidget::Update(CADSR::ADSRParams ADSRParams)
{
    UpdateControls(ADSRParams);
    ui->ADSRControl->Draw(ADSRParams);
}

void CADSRWidget::UpdateGraph()
{
    AP.Attack=ui->Attack->value();
    AP.Decay=ui->Decay->value();
    AP.Sustain=ui->Sustain->value();
    AP.Release=ui->Release->value();
    ui->ADSRControl->Draw(AP);
    emit Changed(AP);
}

void CADSRWidget::UpdateControls(CADSR::ADSRParams ADSRParams)
{
    AP=ADSRParams;
    ui->Attack->blockSignals(true);
    ui->Decay->blockSignals(true);
    ui->Sustain->blockSignals(true);
    ui->Release->blockSignals(true);
    ui->Attack->setValue(AP.Attack);
    ui->Decay->setValue(AP.Decay);
    ui->Sustain->setValue(AP.Sustain);
    ui->Release->setValue(AP.Release);

    ui->Attack->blockSignals(false);
    ui->Decay->blockSignals(false);
    ui->Sustain->blockSignals(false);
    ui->Release->blockSignals(false);
}
