#include "cenevelopeform.h"
#include "cadsrcontrol.h"
#include "ui_cenevelopeform.h"

CEnvelopeForm::CEnvelopeForm(IDevice* Device, QWidget *parent) :
    CSoftSynthsForm(Device,false,parent),
    ui(new Ui::CEnevelopeForm)
{
    ui->setupUi(this);
    ADSRWidget=ui->ADSRWidget;
    connect(ui->ADSRWidget,SIGNAL(Changed(CADSR::ADSRParams)),this,SLOT(UpdateDevice(CADSR::ADSRParams)));
}

CEnvelopeForm::~CEnvelopeForm()
{
    delete ui;
}

void CEnvelopeForm::UpdateDevice(CADSR::ADSRParams ADSRParams)
{
    AP=ADSRParams;
    m_Device->UpdateHost();
}

