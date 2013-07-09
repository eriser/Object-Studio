#ifndef CPARAMETERSCOMPONENT_H
#define CPARAMETERSCOMPONENT_H

#include <QWidget>
#include <QGridLayout>
#include "softsynthsclasses.h"
#include "cknobcontrol.h"

namespace Ui {
    class CParametersComponent;
}

class CParametersComponent : public QWidget
{
    Q_OBJECT

public:
    explicit CParametersComponent(QWidget *parent = 0);
    ~CParametersComponent();
public slots:
    void ShowParameters(IDevice* Device, QString Title);
protected:
    void mousePressEvent(QMouseEvent *event);
private:
    Ui::CParametersComponent *ui;
    QList<CKnobControl*> Dials;
    QList<ParameterType> Parameters;
    QSignalMapper* mapper;
    IDevice* m_D;
    QWidget* Spacer;
private slots:
    void ValueChanged(int i);
signals:
    void ParameterChanged(IDevice* Device, int Index, int Value);
    void Popup(QPoint Pos);
};

#endif // CPARAMETERSCOMPONENT_H
