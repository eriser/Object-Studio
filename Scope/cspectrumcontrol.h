#ifndef CSPECTRUMCONTROL_H
#define CSPECTRUMCONTROL_H

#include <QFrame>
#include <QPainter>
#include "softsynthsclasses.h"

namespace Ui {
    class CSpectrumControl;
}

class CSpectrumControl : public QFrame
{
    Q_OBJECT

public:
    explicit CSpectrumControl(QWidget *parent = 0);
    ~CSpectrumControl();
    void Process(float* Buffer);
    void SetVol(int Vol);
    void SetMode(int Mode);
protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    Ui::CSpectrumControl *ui;
    void* m_ViewFreq;
    void* m_Fft;
};

#endif // CSPECTRUMCONTROL_H
