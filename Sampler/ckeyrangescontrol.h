#ifndef CKEYRANGESCONTROL_H
#define CKEYRANGESCONTROL_H

#include "qcanvas.h"
#include "csamplerdevice.h"
#include <QMouseEvent>

namespace Ui {
    class CKeyRangesControl;
}

class CKeyRangesControl : public QCanvas
{
    Q_OBJECT

public:
    explicit CKeyRangesControl(QWidget *parent = 0);
    ~CKeyRangesControl();
    void Init(CSamplerDevice* D);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
public slots:
    void Draw();
signals:
    void CurrentRangeChanged(CSampleKeyRange::RangeParams RP);
    void RangeIndexChanged(int Index);
    void LoadWaveFile();
    void Add(int Upper, int Lower);
private:
    enum SplitterValues
    {
        svNone,
        svUpper,
        svLower,
        svUpperZero,
        svLowerZero
    };
    Ui::CKeyRangesControl *ui;
    bool MD;
    CSamplerDevice* m_Sampler;
    int inline Vol2Graph(int Vol);
    int inline Graph2Vol(int X);
    int inline X2Graph(int X);
    int inline Graph2X(int X);
    SplitterValues SplitValue;
    int StartMark;
    QPoint OldPoint;
    void DrawKeys();
    void DrawLines();
    void inline MoveLines(QPoint P);
    void inline PutPoint(CSampleKeyRange* Range,QPoint P,SplitterValues SV);
    static const int Range_FrameWidth=4;
};

#endif // CKEYRANGESCONTROL_H
