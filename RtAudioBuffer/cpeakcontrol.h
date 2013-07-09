#ifndef CPEAKCONTROL_H
#define CPEAKCONTROL_H

#include "qcanvas.h"

namespace Ui {
    class CPeakControl;
}

class CPeakControl : public QCanvas
{
    Q_OBJECT

public:
    explicit CPeakControl(QWidget *parent = 0);
    ~CPeakControl();
    void SetValue(const float Value);
    void Reset();
protected:
    void resizeEvent(QResizeEvent *event);
private:
    Ui::CPeakControl *ui;
    float m_Value;
    float m_OldValue;
    float m_Max;
    void SetSize();
    QLinearGradient lgBlack;
    QLinearGradient lgRed;
    QLinearGradient lgYellow;
    QLinearGradient lgGreen;
    QLinearGradient lgDarkRed;
    const int val2y(const float val, const float height);
    const QLinearGradient y2col(const int y, const int height);
    static const int Border=4;
    static const int HalfBorder=2;
};

#endif // CPEAKCONTROL_H
