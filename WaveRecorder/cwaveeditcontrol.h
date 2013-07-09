#ifndef CWAVEEDITCONTROL_H
#define CWAVEEDITCONTROL_H

#include "qcanvas.h"
#include <QMouseEvent>
#include "cwavegenerator.h"

namespace Ui {
    class CWaveEditControl;
}

class CWaveEditControl : public QCanvas
{
    Q_OBJECT

public:
    explicit CWaveEditControl(QWidget *parent = 0);
    ~CWaveEditControl();
    void Init(CWaveGenerator* WG,CWaveGenerator::LoopParameters LP,bool LoopOn);
    void Draw(CWaveGenerator::LoopParameters LP);
    int ScrollMax();
    float Zoom;
    bool Enabled;
public slots:
    void SetStart(int Start);
    void ZoomIn();
    void ZoomOut();
    void ZoomMax();
    void ZoomMin();
protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void ParameterChanged(CWaveGenerator::LoopParameters LP);
private:
    enum WaveLineValues
    {wlNone,wlStart,wlEnd,wlLoopStart,wlLoopEnd,wlFadeIn,wlFadeOut};
    Ui::CWaveEditControl *ui;
    CWaveGenerator* m_WG;
    CWaveGenerator::LoopParameters m_LP;
    bool m_LoopOn;
    int m_Start;
    bool MD;
    void Scroll();
    int PicPos(size_t Position);
    size_t BufferPos(int X);
    int Pos2Vol(int Pos);
    int Vol2Pos(int Vol);
    void DrawLines(CWaveGenerator::LoopParameters LP,bool LoopOn);
    void DrawWave();
    void inline DrawLine(size_t Position);
    void inline MoveWaveLines(QPoint p);
    WaveLineValues WaveLines;
    QPoint OldPos;
    bool isMaxZoom;
    size_t m_Length;
    float* m_Buffer;
};

#endif // CWAVEEDITCONTROL_H
