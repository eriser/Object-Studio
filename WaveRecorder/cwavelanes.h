#ifndef CWAVELANES_H
#define CWAVELANES_H

#include "qcanvas.h"
#include <QMenu>
#include <QLabel>
#include "cwavegenerator.h"

namespace Ui {
    class CWaveLanes;
}

class CWaveTrack
{
public:
    CWaveTrack(const QString& Filename, size_t StartPointer=0);
    QRect Geometry;
    QString Name;
    CWaveGenerator W;
    CWaveGenerator::LoopParameters LP;
    float Volume(size_t Counter);
    size_t PlayLength();
    void Paint(QCanvasLayer* Canvas, float ZoomFactor);
    size_t Start;
    bool Valid;
    bool Active;
private:

};

class CWaveLane
{
public:
    CWaveLane();
    ~CWaveLane();
    QRect Geometry;
    void ModifyBuffers(float* L, float* R, float MixFactor);
    void Reset();
    void UpdateGeometry(float ZoomFactor, int CanvasRight, int BorderWidth);
    void Paint(QCanvasLayer* Canvas, float ZoomFactor);
    QList<CWaveTrack*> Tracks;
    void RemoveFile(const QString& Filename);
    const QString Save();
    void Load(const QString& XML,float ZoomFactor);
private:
    size_t Counter;
    int ModulationCounter;
    float* CurrentBuffer;
    int CurrentChannels;
    float Vol;
};

class CWaveLanes : public QCanvas
{
    Q_OBJECT
public:
    explicit CWaveLanes(QWidget *parent = 0);
    ~CWaveLanes();
    void ModifyBuffers(float* L,float* R);
    void Reset();
    void Stop();
    void Paint();
    void RemoveFile(const QString& Filename);
    const QString Save();
    void Load(const QString& XML);
    int RulerBeats;
    float RulerTempo;
    QList<CWaveLane*> Lanes;
protected:
    //void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
    void timerEvent(QTimerEvent *);
    void mouseDoubleClickEvent(QMouseEvent *event);
signals:
    void RequestVisible(int x, int y);
private slots:
    void AddLane();
    void RemoveLane();
    void UpdateEditTrack(CWaveGenerator::LoopParameters LP);
private:
    Ui::CWaveLanes *ui;
    float MixFactor;
    float ZoomFactor;
    size_t BufferCount;
    int CurrentLineX;
    int MouseOverLane(QPoint Pos);
    int MouseOverTrack(QPoint Pos, int Lane);
    void CalcMixFactor();
    void ShowInfoLabel(quint64 Start,int Lane);
    QPoint StartPos;
    int CurrentLane;
    int CurrentTrack;
    int DragTrack;
    quint64 DragTrackStart;
    QMenu* Popup;
    int TimerID;
    bool Loading;
    QLabel* InfoLabel;
    CWaveTrack* EditTrack;
    static const int LaneHeight=48;
    static const int LaneGap=4;
    static const int RulerHeight=32;
    static const int BorderWidth=8;
};

#endif // CWAVELANES_H
