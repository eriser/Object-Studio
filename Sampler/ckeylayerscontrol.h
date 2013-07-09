#ifndef CKEYLAYERSCONTROL_H
#define CKEYLAYERSCONTROL_H

#include "qcanvas.h"
#include "csamplerdevice.h"
#include <QMouseEvent>

namespace Ui {
class CKeyLayersControl;
}

class CKeyLayersControl : public QCanvas
{
    Q_OBJECT
    
public:
    explicit CKeyLayersControl(QWidget *parent = 0);
    ~CKeyLayersControl();
    void Init(CSamplerDevice* D);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
public slots:
    void Draw();
signals:
    void CurrentLayerChanged(CLayer::LayerParams LP);
    void Add(int Upper, int Lower);
    void LayerIndexChanged(int LayerIndex);
private:
    enum SplitterValues
    {
        svNone,
        svUpper,
        svLower,
        svUpperZero,
        svLowerZero
    };
    Ui::CKeyLayersControl *ui;
    bool MD;
    CSamplerDevice* m_Sampler;
    int inline Vol2Graph(int Vol);
    int inline Graph2Vol(int X);
    int inline Y2Graph(int Y);
    int inline Graph2Y(int Y);
    SplitterValues SplitValue;
    int StartMark;
    QPoint OldPoint;
    void inline MoveLines(QPoint P);
    void inline PutPoint(CLayer* Layer,QPoint P,SplitterValues SV);
    static const int Layer_FrameWidth=4;
};

#endif // CKEYLAYERSCONTROL_H
