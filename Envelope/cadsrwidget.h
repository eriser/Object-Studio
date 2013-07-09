#ifndef CADSRWIDGET_H
#define CADSRWIDGET_H

#include "softsynthsclasses.h"
#include "cadsrcontrol.h"
#include <QFrame>

namespace Ui {
    class CADSRWidget;
}

class CADSRWidget : public QFrame
{
    Q_OBJECT

public:
    explicit CADSRWidget(QWidget *parent = 0);
    ~CADSRWidget();
    void Update(CADSR::ADSRParams ADSRParams);
private slots:
    void UpdateControls(CADSR::ADSRParams ADSRParams);
    void UpdateGraph();
signals:
    void Changed(CADSR::ADSRParams ADSRParams);
private:
    Ui::CADSRWidget *ui;
    CADSR::ADSRParams AP;
};

#endif // CADSRWIDGET_H
