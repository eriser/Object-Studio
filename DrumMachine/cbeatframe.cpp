#include "cbeatframe.h"
#include "ui_cbeatframe.h"

QStringList Pitches=QStringList() << "B#" << "H#" << "C" << "C#" << "DB" << "D" << "D#" << "EB" << "E" << "FB" << "E#" << "F" << "F#" << "GB" << "G" << "G#" << "AB" << "A" << "A#" << "BB" << "B" << "H" << "CB";

CBeatFrame::CBeatFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CBeatFrame)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    connect( ui->LenSlider,SIGNAL(valueChanged(int)),this,SLOT(LenChanged(int)));
    connect(ui->PitchSlider,SIGNAL(valueChanged(int)),this,SLOT(PitchChanged(int)));
    connect(ui->VolSlider,SIGNAL(valueChanged(int)),this,SLOT(VolChanged(int)));
    connect(ui->PitchEdit,SIGNAL(textEdited(QString)),this,SLOT(TextChanged(QString)));
}

CBeatFrame::~CBeatFrame()
{
    delete ui;
}

QString CBeatFrame::Pitch2Note(int Pitch)
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

int CBeatFrame::Note2Pitch(const QString& Note)
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

int CBeatFrame::Text2Pitch(const QString& Txt)
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

    if (Txt.length()>0)
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

QString CBeatFrame::Pitch2Text(int Pitch)
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

void CBeatFrame::LenChanged(int Value)
{
    m_Beat->Length[m_SoundIndex]=Value;
    UpdateBeat();
}
//---------------------------------------------------------------------------


void CBeatFrame::UpdateBeat()
{
    ui->LenProgress->setValue(m_Beat->Length[m_SoundIndex]);
    //Label1->Color=clBlue + (m_Beat->Length * 2);
    //if (m_Beat->Length)
    //{
    //    Label1->Color=clRed;
    //}
    //else
    //{
    //    Label1->Color=clBlue;
    //}
}



void CBeatFrame::Init(BeatType* Beat,int Index,int SoundIndex,bool HideLength,bool HideVolume,bool HidePitch)
{
    DontChange=false;
    m_SoundIndex=SoundIndex;
    ui->LenSlider->setVisible(true);
    ui->LenProgress->setVisible(true);
    ui->VolSlider->setVisible(true);
    ui->VolProgress->setVisible(true);
    ui->PitchEdit->setVisible(true);
    ui->PitchSlider->setVisible(true);
    /*
    LMDScrollBar1->Visible=true;
    LMDProgress1->Visible=true;
    LMDScrollBar3->Visible=true;
    LMDProgress2->Visible=true;
    Edit1->Visible=true;
    LMDScrollBar2->Visible=true;

    LMDScrollBar3->Top=76;
    LMDProgress2->Top=76;
    Edit1->Top=135;
    LMDScrollBar2->Top=158;
    this->Height=212;
    */
    if (HideLength)
    {
        /*
        LMDScrollBar1->Visible=false;
        LMDProgress1->Visible=false;
        LMDScrollBar3->Top=76-76;
        LMDProgress2->Top=76-76;
        Edit1->Top=135-76;
        LMDScrollBar2->Top=158-76;
        this->Height=this->Height-76;
        */
        ui->LenProgress->setVisible(false);
        ui->LenSlider->setVisible(false);
    }
    if (HideVolume)
    {
        /*
        LMDScrollBar3->Visible=false;
        LMDProgress2->Visible=false;
        Edit1->Top=Edit1->Top-(135-76);
        LMDScrollBar2->Top= LMDScrollBar2->Top -(135-76);
        this->Height=this->Height-58;
        */
        ui->VolSlider->setVisible(false);
        ui->VolProgress->setVisible(false);
    }
    if (HidePitch)
    {
        /*
        Edit1->Visible=false;
        LMDScrollBar2->Visible=false;
        this->Height=this->Height-76;
        */
        ui->PitchEdit->setVisible(false);
        ui->PitchSlider->setVisible(false);
    }


    //Label1->Caption=AnsiString(Index+1);
    //LMDProgress1->Caption=AnsiString(Index+1);
    //ui->LenProgress->setFormat(QString::number(Index+1));
    ui->label->setText(QString::number(Index+1));
    m_Beat=Beat;
    ui->PitchEdit->blockSignals(true);
    ui->LenSlider->blockSignals(true);
    ui->VolSlider->blockSignals(true);
    ui->VolProgress->blockSignals(true);
    ui->PitchEdit->setText(Pitch2Text(m_Beat->Pitch[m_SoundIndex]));
    ui->LenSlider->setValue(m_Beat->Length[m_SoundIndex]);
    ui->VolSlider->setValue(m_Beat->Volume[m_SoundIndex]);
    ui->PitchSlider->setValue(m_Beat->Pitch[m_SoundIndex]);
    ui->VolProgress->setValue(m_Beat->Volume[m_SoundIndex]);
    ui->PitchEdit->blockSignals(false);
    ui->LenSlider->blockSignals(false);
    ui->VolSlider->blockSignals(false);
    ui->VolProgress->blockSignals(false);
    UpdateBeat();
}

void CBeatFrame::TextChanged(QString Txt)
{
    m_Beat->Pitch[m_SoundIndex]=Text2Pitch(Txt);
    ui->PitchSlider->setValue(m_Beat->Pitch[m_SoundIndex]);
    UpdateBeat();
}
//---------------------------------------------------------------------------

void CBeatFrame::PitchChanged(int Value)
{
    m_Beat->Pitch[m_SoundIndex]=Value;
    ui->PitchEdit->setText(Pitch2Text(Value));
}
//---------------------------------------------------------------------------


void CBeatFrame::VolChanged(int Value)
{
    m_Beat->Volume[m_SoundIndex]=Value;
    ui->VolProgress->setValue(Value);
}
//---------------------------------------------------------------------------
void CBeatFrame::Flash()
{
    QPalette p=ui->label->palette();
    p.setColor(QPalette::Background,Qt::yellow);
    ui->label->setPalette(p);
    update();
    m_TimerID = startTimer(200);
}

void CBeatFrame::timerEvent(QTimerEvent *)
{
    QPalette p=ui->label->palette();
    p.setColor(QPalette::Background,Qt::transparent);
    ui->label->setPalette(p);
    update();
    killTimer(m_TimerID);
}
