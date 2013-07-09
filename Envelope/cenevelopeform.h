#ifndef CENEVELOPEFORM_H
#define CENEVELOPEFORM_H

#include "softsynthsclasses.h"
#include "cadsrwidget.h"

namespace Ui {
    class CEnevelopeForm;
}

class CEnvelopeForm : public CSoftSynthsForm
{
    Q_OBJECT

public:
    explicit CEnvelopeForm(IDevice* Device, QWidget *parent = 0);
    ~CEnvelopeForm();
    CADSRWidget* ADSRWidget;
    CADSR::ADSRParams AP;
private slots:
    void UpdateDevice(CADSR::ADSRParams ADSRParams);
private:
    Ui::CEnevelopeForm *ui;
};

#endif // CENEVELOPEFORM_H
