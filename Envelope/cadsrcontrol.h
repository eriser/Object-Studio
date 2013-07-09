#ifndef CADSRCONTROL_H
#define CADSRCONTROL_H

#include "qcanvas.h"
#include "cadsr.h"

namespace ADSRControl
{
const int ADSR_MaxTime=2000;
const int ADSR_ReleaseStart=ADSR_MaxTime*3;
const int ADSR_MaxWidth=ADSR_MaxTime*4;
}

namespace Ui {
    class CADSRControl;
}

class CADSRControl : public QCanvas
{
    Q_OBJECT

public:
    enum SplitterValuesADSR
    {svADSRNone,svAttack,svDecay,svSustain,svRelease};
    explicit CADSRControl(QWidget *parent = 0);
    ~CADSRControl();
public slots:
    void Draw(CADSR::ADSRParams ADSRParams);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
signals:
    void Changed(CADSR::ADSRParams ADSRParams);
private:
    Ui::CADSRControl *ui;
    int inline Time2X(int Time);
    int inline Vol2Y(int Vol);
    int inline X2Time(int X);
    int inline Y2Vol(int Y);
    void inline MoveLines(QPoint P);
    void inline PutPoint(QPoint P,SplitterValuesADSR SV);
    bool MD;
    QPoint OldPoint;
    SplitterValuesADSR SplitValue;
    CADSR::ADSRParams AP;
};

#endif // CADSRCONTROL_H
