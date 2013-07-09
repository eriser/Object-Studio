#ifndef CKNOBCONTROL_H
#define CKNOBCONTROL_H

#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QMenu>
#include <QMouseEvent>
#include <QWidgetAction>
#include <QSignalMapper>
#include "softsynthsdefines.h"
#include "softsynthsclasses.h"

namespace Ui {
    class CKnobControl;
}

class CKnobControl : public QWidget
{
    Q_OBJECT

public:
    explicit CKnobControl(QWidget *parent = 0);
    ~CKnobControl();
    void SetValue(int Value, const ParameterType& p);
    void SetTexts(int Value, const ParameterType &p);
    int GetValue();
protected:
    void mousePressEvent(QMouseEvent *);
signals:
    void ValueChanged(int Value);
private:
    ParameterType Parameter;
    Ui::CKnobControl *ui;
    QMenu* popup;
    QWidgetAction* spinboxAction;
    QDoubleSpinBox* spinbox;
    QSignalMapper* mapper;

private slots:
    void SetNumericValue(double Value);
    void SetdBValue(double Value);
};

#endif // CKNOBCONTROL_H
