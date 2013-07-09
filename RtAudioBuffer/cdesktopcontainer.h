#ifndef CDESKTOPCONTAINER_H
#define CDESKTOPCONTAINER_H

#include <QWidget>
#include "cdesktopcomponent.h"
#include "cparameterscomponent.h"

namespace Ui {
    class CDesktopContainer;
}

class CDesktopContainer : public QWidget
{
    Q_OBJECT

public:
    explicit CDesktopContainer(QWidget *parent = 0);
    ~CDesktopContainer();
    CDesktopComponent* Desktop;
    CParametersComponent* Parameters;
private:
    Ui::CDesktopContainer *ui;
};

#endif // CDESKTOPCONTAINER_H
