#ifndef CSAMPLERFORM_H
#define CSAMPLERFORM_H

#include "softsynthsclasses.h"
#include "csamplerdevice.h"
#include <QDialog>

namespace Ui {
    class CSamplerForm;
}

class CSamplerForm : public CSoftSynthsForm
{
    Q_OBJECT

public:
    explicit CSamplerForm(IDevice* Device, QWidget *parent = 0);
    ~CSamplerForm();
    void Init(CSamplerDevice* Device);
    void CustomLoad(const QString &XML);
    const QString CustomSave();
    void ReleaseLoop();
private:
    Ui::CSamplerForm *ui;
    CSamplerDevice* m_Sampler;
};

#endif // CSAMPLERFORM_H
