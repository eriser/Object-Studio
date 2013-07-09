#ifndef CSCOPECONTROL_H
#define CSCOPECONTROL_H

#include "qcanvas.h"
#include "softsynthsclasses.h"

namespace Ui {
class CScopeControl;
}

class CScopeControl : public QCanvas
{
    Q_OBJECT

public:
    explicit CScopeControl(QWidget *parent = 0);
    ~CScopeControl();
    void SetVol(int Vol);
    void SetFreq(float Freq,int BufferSize);
    void Process(float* Buffer);
protected:
    void resizeEvent(QResizeEvent *event);
private:
    Ui::CScopeControl *ui;
    void inline CalcHeight();
    int inline ScaleY(float Y);

    float ScopeCounter;
    float OldCounter;
    int m_Amplitude;
    int MiddleY;
    float CurrentHeight;
    int PixWidth;
    int PixHeight;
    int ImgHeight;
    int LastY;
    int LastX;
    PresetsType m_Presets;
    bool Working;
    float m_Frequency;
    float DisplayFactor;
    int BufferDisplaySize;
    int m_BufferSize;
};

#endif // CSCOPECONTROL_H
