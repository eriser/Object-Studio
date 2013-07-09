#include "crepeatform.h"
#include "ui_crepeatform.h"

CRepeatForm::CRepeatForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CRepeatForm)
{
    ui->setupUi(this);
    setWindowTitle("Repeats");
}

CRepeatForm::~CRepeatForm()
{
    delete ui;
}

void CRepeatForm::SetRepeats(int Repeats)
{
    ui->checkBox->setChecked(Repeats==0);
    ui->spinBox->setValue(Repeats);
}

int CRepeatForm::GetRepeats()
{
    if (ui->checkBox->isChecked()) return 0;
    return ui->spinBox->value();
}
