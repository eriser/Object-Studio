#ifndef CBEATFRAME_H
#define CBEATFRAME_H

#include <QFrame>
#include "sequenserclasses.h"

namespace Ui {
    class CBeatFrame;
}

class CBeatFrame : public QFrame
{
    Q_OBJECT

public:
    explicit CBeatFrame(QWidget *parent = 0);
    ~CBeatFrame();
    void Init(BeatType* Beat,int Index,int SoundIndex,bool HideLength,bool HideVolume,bool HidePitch);
    void Flash();
private:
    Ui::CBeatFrame *ui;
        QString Pitch2Note(int Pitch);
        int Note2Pitch(const QString& Note);
        bool DontChange;
        int Text2Pitch(const QString& Txt);
        QString Pitch2Text(int Pitch);
        void UpdateBeat();	// User declarations
        BeatType* m_Beat;
        int m_SoundIndex;
        int m_TimerID;
private slots:
        void LenChanged(int Value);
        void PitchChanged(int Value);
        void VolChanged(int Value);
        void TextChanged(QString Txt);
protected:
        void timerEvent(QTimerEvent *);
};

#endif // CBEATFRAME_H
