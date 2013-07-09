#ifndef CKEYLAYOUTCONTROL_H
#define CKEYLAYOUTCONTROL_H

#include <QFrame>
#include "csamplerdevice.h"

enum SplitterValues
{svNone,svLower,svUpper,svVolume,svLowerZero,svUpperZero};

namespace Ui {
class CKeyLayoutControl;
}

class CKeyLayoutControl : public QFrame
{
    Q_OBJECT

public:
    explicit CKeyLayoutControl(QWidget *parent = 0);
    ~CKeyLayoutControl();
    void Init(CSamplerDevice* D);
    void Update();
    int Tune_A440();
    int LoopCycles();
private slots:
    void DoUpdateHost();
    void UpdateRangeGraph();
    void UpdateWaveGraph();
    void UpdateWaveControls(CWaveGenerator::LoopParameters LP);
    void UpdateRangeControls(CSampleKeyRange::RangeParams RP);
    void OpenFile();
    void SelectRange(int RangeIndex);
    void DeleteRange();
    void AddRange(int Upper=127,int Lower=0);
public slots:
    void ReleaseLoop();
    void ToggleTuneTest(bool ButtonDown);
    void ToggleLoopTest(bool ButtonDown);
    void Autoloop();
    void Autotune();
    void Pitchdetect();
    void FixRange();
signals:
    void UpdateHost(bool Erase);
private:
    Ui::CKeyLayoutControl *ui;
    CSamplerDevice* m_Sampler;
    /*
    bool MD;
    int StartMark;
    void inline MoveLines(QPoint P);
    void inline PutPoint(CSampleKeyRange* Range,QPoint P,SplitterValues SV);
    //        void AddRange(AnsiString WavePath,int Upper,int Lower);
    int inline Vol2Graph(int Vol);
    int inline Graph2Vol(int Y);
    int inline X2Graph(int X);
    int inline Graph2X(int X);
    */
    void inline WEInit();
    /*
    void inline EnableEdit(bool Enabled);
    //CSampleKeyRange* SelectedRange;
    SplitterValues SplitValue;
    QPoint OldPoint;
    int OldX;
    */
};

#endif // CKEYLAYOUTCONTROL_H
