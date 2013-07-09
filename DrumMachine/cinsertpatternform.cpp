#include "cinsertpatternform.h"
#include "ui_cinsertpatternform.h"

CInsertPatternForm::CInsertPatternForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInsertPatternForm)
{
    ui->setupUi(this);
}

CInsertPatternForm::~CInsertPatternForm()
{
    delete ui;
}

void CInsertPatternForm::SelectPattern(QList<PatternType*>& Patterns,int PatternIndex,int Repeats,bool InsertBefore)
{
    ui->comboBox->clear();
    for (int i=0;i<Patterns.count();i++)
    {
        ui->comboBox->addItem(Patterns[i]->Name);
    }
    ui->comboBox->setCurrentIndex(PatternIndex);
    ui->spinBox->setValue(Repeats);
    ui->checkBox->setChecked(Repeats==0);
    ui->checkBox_2->setChecked(InsertBefore);
}

void CInsertPatternForm::GetValues(int& PatternIndex,int& Repeats,bool& InsertBefore)
{
    PatternIndex=ui->comboBox->currentIndex();
    Repeats=ui->spinBox->value();
    if (ui->checkBox->isChecked()) Repeats=0;
    InsertBefore=ui->checkBox_2->isChecked();
 }
