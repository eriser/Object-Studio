#ifndef CEQUALIZERFORM_H
#define CEQUALIZERFORM_H

#include "softsynthsclasses.h"
#include "cequalizer.h"
#include "cwavebank.h"
#include "qcanvas.h"

namespace Ui {
    class CEqualizerForm;
}

class CEqualizerForm : public CSoftSynthsForm
{
    Q_OBJECT

public:
    explicit CEqualizerForm(IDevice* Device, QWidget *parent = 0);
    ~CEqualizerForm();
    void Init(CEqualizer* EQ);
    const QString CustomSave();
    void CustomLoad(const QString& XML);
public slots:
    void Draw();
    void DrawGraph();
private:
    Ui::CEqualizerForm *ui;
    CEqualizer* m_Device;
    CWaveBank W;
    biquad Fi[8];
    QCanvas* Canvas;
    void DrawBg();
};

#endif // CEQUALIZERFORM_H
