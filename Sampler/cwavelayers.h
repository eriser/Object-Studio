#ifndef CWAVELAYERS_H
#define CWAVELAYERS_H

#include <QFrame>
#include "ckeylayoutcontrol.h"
#include "csamplerdevice.h"

namespace Ui {
class CWaveLayers;
}

class CWaveLayers : public QFrame
{
    Q_OBJECT

public:
    explicit CWaveLayers(QWidget *parent = 0);
    ~CWaveLayers();
    void Init(CSamplerDevice* Device);
    void Update();
public slots:
    void AddLayer(int Upper, int Lower);
    void ReleaseLoop();
private slots:
    void UpdateGraph();
    void UpdateControls(CLayer::LayerParams LP);
    void UpdateADSRs(CADSR::ADSRParams ADSRParams);
    void SelectLayer(int LayerIndex);
    void FixLayer();
    void FixAll();
    void DeleteLayer();
    void PitchLayer();
    void PitchAll();
private:
    Ui::CWaveLayers *ui;
    void setControlBounds(CLayer::LayerParams LP);
    void Add(int Upper,int Lower);
    int inline Vol2Graph(int Vol);
    int inline Graph2Vol(int X);
    int inline Y2Graph(int Y);
    int inline Graph2Y(int Y);
    int StartMark;
    bool MD;
    bool Working;
    SplitterValues SplitValue;
    void inline MoveLines(QPoint P);
    void inline PutPoint(CLayer* Range,QPoint P,SplitterValues SV);
    float MixFactor;
    QPoint OldPoint;
    CSamplerDevice* m_Sampler;
};

#endif // CWAVELAYERS_H
