#ifndef CSTEREOMIXERFORM_H
#define CSTEREOMIXERFORM_H

#include "softsynthsclasses.h"
#include "cstereochannelwidget.h"
#include "cmasterwidget.h"

namespace Ui {
class CStereoMixerForm;
}

class CStereoMixerForm : public CSoftSynthsForm
{
    Q_OBJECT
    
public:
    explicit CStereoMixerForm(IDevice* Device, QWidget *parent = 0);
    ~CStereoMixerForm();
    void Reset();
    const QString CustomSave();
    void CustomLoad(const QString &XML);
private:
    Ui::CStereoMixerForm *ui;
    QList<CStereoChannelWidget*> channels;
    CMasterWidget* master;
    QTimer peakTimer;
    QSignalMapper* mapper;
private slots:
    void peak();
    void setSoloChannel(int channel);
};

#endif // CSTEREOMIXERFORM_H
