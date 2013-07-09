#ifndef CSTEREOPEAK_H
#define CSTEREOPEAK_H

#include <QFrame>
#include "cpeakcontrol.h"

namespace Ui {
    class CStereoPeak;
}

class CStereoPeak : public QFrame
{
    Q_OBJECT

public:
    explicit CStereoPeak(QWidget *parent = 0);
    ~CStereoPeak();
    void SetValues(const float L, const float R);
    void Reset();
private:
    Ui::CStereoPeak *ui;
};

#endif // CSTEREOPEAK_H
