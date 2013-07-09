#include "cscopeform.h"
#include "ui_cscopeform.h"

CScopeForm::CScopeForm(IDevice* Device, QWidget *parent) :
    CSoftSynthsForm(Device,true,parent),
    ui(new Ui::CScopeForm)
{
    ui->setupUi(this);
    Scope=ui->ScopeControl;
    Spectrum=ui->SpectrumControl;
    Tab=ui->tabWidget;
    startTimer(50);
}

CScopeForm::~CScopeForm()
{
    delete ui;
}

void CScopeForm::timerEvent(QTimerEvent* /*event*/)
{
    if (isVisible())
    {
        if (ui->tabWidget->currentIndex()==0)
        {
            Scope->repaint();
        }
        else
        {
            Spectrum->repaint();
        }
    }
}

void CScopeForm::CustomLoad(const QString &XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        QDomLiteElement* Tabs=xml.elementByTag("Tabs");
        if (Tabs) ui->tabWidget->setCurrentIndex(Tabs->attributeValue("TabIndex"));
    }
}

const QString CScopeForm::CustomSave()
{
    QDomLiteElement xml("Custom");
    QDomLiteElement* Tabs=xml.appendChild("Tabs");
    Tabs->setAttribute("TabIndex",ui->tabWidget->currentIndex());
    return xml.toString();
}
