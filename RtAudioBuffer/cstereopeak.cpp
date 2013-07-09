#include "cstereopeak.h"
#include "ui_cstereopeak.h"

CStereoPeak::CStereoPeak(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CStereoPeak)
{
    ui->setupUi(this);
}

CStereoPeak::~CStereoPeak()
{
    delete ui;
}

void CStereoPeak::SetValues(const float L, const float R)
{
    ui->PeakL->SetValue(L);
    ui->PeakR->SetValue(R);
}

void CStereoPeak::Reset()
{
    ui->PeakL->Reset();
    ui->PeakR->Reset();
}
