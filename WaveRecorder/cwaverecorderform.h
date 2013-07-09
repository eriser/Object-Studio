#ifndef CWAVERECORDERFORM_H
#define CWAVERECORDERFORM_H

#include "softsynthsclasses.h"
#include <QMenu>

namespace Ui {
    class CWaveRecorderForm;
}

class CWaveRecorderForm : public CSoftSynthsForm
{
    Q_OBJECT

public:
    explicit CWaveRecorderForm(IDevice* Device, QWidget *parent = 0);
    ~CWaveRecorderForm();
    const QString CustomSave();
    void CustomLoad(const QString& XML);
    void Reset();
    void Stop();
    void ModifyBuffers(float* L,float* R);
private slots:
    void Import();
    void Remove();
    void FileMenuPopup(QPoint Pos);
    void EnsureVisible(int x, int y);
protected:
    //void resizeEvent(QResizeEvent * e);
private:
    Ui::CWaveRecorderForm *ui;
    QMenu* Popup;
};

#endif // CWAVERECORDERFORM_H
