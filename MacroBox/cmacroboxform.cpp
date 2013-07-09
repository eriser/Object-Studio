#include "cmacroboxform.h"
#include "ui_cmacroboxform.h"

CMacroBoxForm::CMacroBoxForm(IDevice* Device, QWidget *parent) :
    CSoftSynthsForm(Device,false,parent),
    ui(new Ui::CMacroBoxForm)
{
    ui->setupUi(this);
    DesktopComponent=ui->DesktopContainer->Desktop;
}

CMacroBoxForm::~CMacroBoxForm()
{
    delete ui;
}

void CMacroBoxForm::CustomLoad(const QString &XML)
{
    DesktopComponent->Load(XML);
}

const QString CMacroBoxForm::CustomSave()
{
    return DesktopComponent->Save("Custom");
}
