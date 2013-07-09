#ifndef CSCOPEFORM_H
#define CSCOPEFORM_H

#include "softsynthsclasses.h"
#include "cscopecontrol.h"
#include "cspectrumcontrol.h"
#include <QDialog>
#include <QTabWidget>

namespace Ui {
    class CScopeForm;
}

class CScopeForm : public CSoftSynthsForm
{
    Q_OBJECT

public:
    explicit CScopeForm(IDevice* Device, QWidget *parent = 0);
    ~CScopeForm();
    CScopeControl* Scope;
    CSpectrumControl* Spectrum;
    QTabWidget* Tab;
    void CustomLoad(const QString &XML);
    const QString CustomSave();
protected:
    void timerEvent(QTimerEvent *event);
private:
    Ui::CScopeForm *ui;
};

#endif // CSCOPEFORM_H
