#ifndef CMACROBOXFORM_H
#define CMACROBOXFORM_H

#include <QDialog>
#include "cdesktopcontainer.h"

namespace Ui {
    class CMacroBoxForm;
}

class CMacroBoxForm : public CSoftSynthsForm
{
    Q_OBJECT

public:
    explicit CMacroBoxForm(IDevice* Device, QWidget *parent = 0);
    ~CMacroBoxForm();
    CDesktopComponent* DesktopComponent;
    void CustomLoad(const QString& XML);
    const QString CustomSave();
private:
    Ui::CMacroBoxForm *ui;
};

#endif // CMACROBOXFORM_H
