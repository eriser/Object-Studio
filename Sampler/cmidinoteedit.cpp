#include "cmidinoteedit.h"
#include "ui_cmidinoteedit.h"

QStringList Pitches=QStringList() << "B#" << "H#" << "C" << "C#" << "DB" << "D" << "D#" << "EB" << "E" << "FB" << "E#" << "F" << "F#" << "GB" << "G" << "G#" << "AB" << "A" << "A#" << "BB" << "B" << "H" << "CB";

QString Pitch2Note(int Pitch)
{
    switch (Pitch)
    {
    case 0: return "C";
    case 1: return "C#";
    case 2: return "D";
    case 3: return "D#";
    case 4: return "E";
    case 5: return "F";
    case 6: return "F#";
    case 7: return "G";
    case 8: return "G#";
    case 9: return "A";
    case 10: return "Bb";
    case 11: return "B";
    }
    return "";
}

int Note2Pitch(const QString& Note)
{
    switch (Pitches.indexOf(Note.toUpper()))
    {
    case 0: return 0;
    case 1: return 0;
    case 2: return 0;
    case 3: return 1;
    case 4: return 1;
    case 5: return 2;
    case 6: return 3;
    case 7: return 3;
    case 8: return 4;
    case 9: return 4;
    case 10: return 5;
    case 11: return 5;
    case 12: return 6;
    case 13: return 6;
    case 14: return 7;
    case 15: return 8;
    case 16: return 8;
    case 17: return 9;
    case 18: return 10;
    case 19: return 10;
    case 20: return 11;
    case 21: return 11;
    case 22: return 11;
    }
    return 0;
}

int Text2Pitch(const QString& Txt)
{
    int Note;
    int Octave=0;
    int StrLen=Txt.length();
    if (StrLen==0)
    {
        return 0;
    }
    if (Txt.contains("?"))
    {
        return 0;
    }
    int LastChar=0;

    if (!Txt.isEmpty())
    {
        QString Oct;
        for (int i=0;i<StrLen;i++)
        {
            if (Txt[i].isDigit())
            {
                Oct=Oct + Txt[i];
            }
            else
            {
                LastChar=i+1;
            }
        }
        if (Oct.length())
        {
            Octave=Oct.toInt();
        }
        /*
        try
        {
            Octave=StrToInt(Txt->SubString(StrLen,StrLen-1));
        }
        catch (...)
        {
            Octave=0;
        }
        */
    }
    Note=Note2Pitch(Txt.left(LastChar));
    return Note + (Octave * 12);
}

QString Pitch2Text(int Pitch)
{
    QString Text;
    int Note;
    int Octave;
    if (Pitch==0)
    {
        Text="?";
    }
    else
    {
        Note=Pitch % 12;
        Octave= Pitch/12;
        Text=Pitch2Note(Note);
        if (Text.length()==0)
        {
            Text="?";
        }
        else
        {
            Text += QString::number(Octave);
        }
    }
    return Text;
}

CMIDINoteEdit::CMIDINoteEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMIDINoteEdit)
{
    ui->setupUi(this);
    connect(ui->lineEdit,SIGNAL(editingFinished()),this,SLOT(fromText()));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(fromSpin()));
}

CMIDINoteEdit::~CMIDINoteEdit()
{
    delete ui;
}

int CMIDINoteEdit::value()
{
    return ui->spinBox->value();
}

void CMIDINoteEdit::setValue(int val)
{
    ui->spinBox->blockSignals(true);
    ui->lineEdit->blockSignals(true);
    ui->spinBox->setValue(val);
    ui->lineEdit->setText(Pitch2Text(val));
    ui->spinBox->blockSignals(false);
    ui->lineEdit->blockSignals(false);
}

void CMIDINoteEdit::fromSpin()
{
    ui->lineEdit->blockSignals(true);
    ui->lineEdit->setText(Pitch2Text(ui->spinBox->value()));
    ui->lineEdit->blockSignals(false);
    emit Changed(ui->spinBox->value());
}

void CMIDINoteEdit::fromText()
{
    ui->spinBox->blockSignals(true);
    ui->spinBox->setValue(Text2Pitch(ui->lineEdit->text()));
    ui->spinBox->blockSignals(false);
    emit Changed(ui->spinBox->value());
}

void CMIDINoteEdit::setMaximum(int val)
{
    ui->spinBox->blockSignals(true);
    ui->lineEdit->blockSignals(true);
    ui->spinBox->setMaximum(val);
    ui->lineEdit->setText(Pitch2Text(ui->spinBox->value()));
    ui->spinBox->blockSignals(false);
    ui->lineEdit->blockSignals(false);
}

void CMIDINoteEdit::setMinimum(int val)
{
    ui->spinBox->blockSignals(true);
    ui->lineEdit->blockSignals(true);
    ui->spinBox->setMinimum(val);
    ui->lineEdit->setText(Pitch2Text(ui->spinBox->value()));
    ui->spinBox->blockSignals(false);
    ui->lineEdit->blockSignals(false);
}
