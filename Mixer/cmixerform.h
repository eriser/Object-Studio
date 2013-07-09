#ifndef CMIXERFORM_H
#define CMIXERFORM_H

#include "softsynthsclasses.h"
#include "cmixerframe.h"

namespace Ui {
    class CMixerForm;
}

class CMixerForm : public CSoftSynthsForm
{
    Q_OBJECT

public:
    explicit CMixerForm(IDevice* Device, QWidget *parent = 0);
    ~CMixerForm();
    void CustomLoad(const QString &XML);
    const QString CustomSave();
    void Reset();
private:
    Ui::CMixerForm *ui;
    QList<CMixerFrame*> MF;
protected:
    void timerEvent(QTimerEvent *event);
private slots:
    void LeftChanged(int Value);
    void RightChanged(int Value);
    void SoloClicked(bool Pressed, int Index);
};

#endif // CMIXERFORM_H
