#include "cdesktopcontainer.h"
#include "ui_cdesktopcontainer.h"

CDesktopContainer::CDesktopContainer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDesktopContainer)
{
    ui->setupUi(this);

    Desktop=ui->DesktopComponent;
    Parameters=ui->ParametersComponent;

    connect(ui->DesktopComponent,SIGNAL(UpdateParameters(IDevice*,QString)),ui->ParametersComponent,SLOT(ShowParameters(IDevice*,QString)));
    connect(ui->ParametersComponent,SIGNAL(ParameterChanged(IDevice*,int,int)),ui->DesktopComponent,SLOT(ChangeParameter(IDevice*,int,int)));
}

CDesktopContainer::~CDesktopContainer()
{
    delete ui;
}
