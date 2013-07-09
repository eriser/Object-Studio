#include "cvsthostform.h"
#include "ui_cvsthostform.h"
#include <QFileDialog>
#include <QDesktopWidget>
//#include "qt_mac_p.h"
#include "cmacwindow.h"
#include <mouseevents.h>

CVSTHostForm::CVSTHostForm(IDevice* Device, QWidget *parent) :
    CSoftSynthsForm(Device,true,parent),
    ui(new Ui::CVSTHostForm)
{
    ui->setupUi(this);
    m_Effect=NULL;
    Popup=new QMenu(this);
    PresetAction=Popup->addAction("Presets",this,SLOT(TogglePresets()));
    PresetAction->setCheckable(true);
    StatusAction=Popup->addAction("Status",this,SLOT(ToggleStatus()));
    StatusAction->setCheckable(true);
    Popup->addSeparator();
    Popup->addAction("Load Bank",this,SLOT(LoadBank()));
    Popup->addAction("Save Bank",this,SLOT(SaveBank()));
    Popup->addAction("Load Preset",this,SLOT(LoadPreset()));
    Popup->addAction("Save Preset",this,SLOT(SavePreset()));
    Popup->addSeparator();
    Popup->addAction("Unload",this,SLOT(Unload()));

    ui->PresetList->setFocusPolicy(Qt::ClickFocus);
    ui->PresetList->setAttribute(Qt::WA_MacShowFocusRect,false);
    ui->StatusEdit->setFocusPolicy(Qt::NoFocus);
    ui->ParameterList->setFocusPolicy(Qt::ClickFocus);
    ui->ParameterList->setAttribute(Qt::WA_MacShowFocusRect,false);

    connect(ui->PresetList,SIGNAL(currentRowChanged(int)),this,SLOT(PresetChange(int)));
    connect(ui->ParameterList,SIGNAL(currentRowChanged(int)),this,SLOT(ParameterIndexChange(int)));
    connect(ui->dial,SIGNAL(valueChanged(int)),this,SLOT(ParameterChange(int)));

    MouseEvents* e=new MouseEvents;
    ui->PresetList->viewport()->installEventFilter(e);
    ui->StatusEdit->viewport()->installEventFilter(e);
    connect(e,SIGNAL(MousePress(QMouseEvent*)),ui->View,SLOT(update()));

    m_MD=false;
    HasEditor=false;
    EffRect=QRect(0,0,400,300);
    wasActivated=false;
    startTimer(0);
}

CVSTHostForm::~CVSTHostForm()
{
    delete ui;
    qDebug() << "Exit CVSTHostForm";
}

void CVSTHostForm::Init(AEffect *E, TVSTHost *OwnerClass)
{
    ui->PresetList->setVisible(false);
    ui->ParameterList->setVisible(false);
    ui->StatusEdit->setVisible(false);
    ui->PresetList->setVisible(false);
    ui->dial->setVisible(false);
    ui->Label1->setVisible(false);
    ui->Label2->setVisible(false);
    ui->View->setVisible(false);

    m_Effect=E;
    m_OwnerClass=OwnerClass;
    if (m_Effect->flags & effFlagsHasEditor)
    {
        ui->View->setVisible(true);
        HasEditor=true;
        EffRect=GetEffRect();
        resize(EffRect.size());
        ui->View->setGeometry(EffRect);
        ui->View->Init();
        m_Effect->dispatcher(m_Effect,effEditOpen,0,0,ui->View->WindowReference(),0.0f);
    }
    else
    {
        HasEditor=false;
        EffRect=QRect(0,0,400,300);
        ui->PresetList->setVisible(true);
        ui->ParameterList->setVisible(true);
        ui->StatusEdit->setVisible(true);
        ui->PresetList->setVisible(true);
        ui->dial->setVisible(true);
        ui->Label1->setVisible(true);
        ui->Label2->setVisible(true);
        for (int i=0;i<m_Effect->numParams;i++)
        {
            char strName[24];
            m_Effect->dispatcher(m_Effect,effGetParamName,i,0,strName,0.0f);
            ui->ParameterList->addItem(strName);
        }
        ui->ParameterList->setCurrentRow(0);
        CurrentParameter=0;
        UpdateParam();
        ViewResized();
    }

    long nPrograms=m_Effect->numPrograms;
    for (long i=0;i<nPrograms;i++)
    {
        char strProgramName[24+2];
        m_Effect->dispatcher(m_Effect,effSetProgram,0,i,NULL,0.0f);
        m_Effect->dispatcher(m_Effect,effGetProgramName,0,0,strProgramName,0.0f);

        ui->PresetList->addItem(strProgramName);
    }
    m_Effect->dispatcher(m_Effect,effSetProgram,0,0,NULL,0.0f);
    ui->PresetList->setCurrentRow(0);
}

void CVSTHostForm::TogglePresets()
{
    ui->PresetList->setVisible(!ui->PresetList->isVisible());
    ViewResized();
}

void CVSTHostForm::ToggleStatus()
{
    ui->StatusEdit->setVisible(!ui->StatusEdit->isVisible());
    ViewResized();
}

void CVSTHostForm::PresetChange(int Index)
{
    long i = m_Effect->dispatcher(m_Effect,effGetProgram,0,0,NULL,0.0f);
    if (i != Index && Index>-1)
    {
        i=Index;
        m_Effect->dispatcher(m_Effect,effSetProgram,0,i,NULL,0.0f);
        if (ui->ParameterList->isVisible())
        {
            UpdateParam();
        }
    }
    ui->View->update();
}

void CVSTHostForm::AddStatusInfo(QString Info)
{
    ui->StatusEdit->appendPlainText(Info);
}

const QString CVSTHostForm::Save()
{
    QDomLiteElement xml("Settings");
    QString RelPath=QDir(CPresets::Presets.VSTPath).relativeFilePath(m_OwnerClass->CurrentBank);
    xml.setAttribute("BankPath",RelPath);

    RelPath=QDir(CPresets::Presets.VSTPath).relativeFilePath(m_OwnerClass->CurrentPreset);

    xml.setAttribute("PresetPath",RelPath);
    int p = m_Effect->dispatcher(m_Effect,effGetProgram,0,0,NULL,0.0f);
    xml.setAttribute("Preset",p);
    xml.setAttribute("NumParams",m_Effect->numParams);
    QDomLiteElement* Params = xml.appendChild("Parameters");
    for (int i=0;i<m_Effect->numParams;i++)
    {
        float newval=m_Effect->getParameter(m_Effect,i);
        Params->setAttribute("Param" + QString::number(i),newval);
    }

    QDomLiteElement* Position=xml.appendChild("Position");
    Position->setAttribute("Top",this->pos().y());
    Position->setAttribute("Left",this->pos().x());
    Position->setAttribute("Height",this->height());
    Position->setAttribute("Width",this->width());
    Position->setAttribute("Visible",this->isVisible());
    Position->setAttribute("Status",ui->StatusEdit->isVisible());
    Position->setAttribute("Presets",ui->PresetList->isVisible());
    return xml.toString();
}

void CVSTHostForm::Load(const QString &XML)
{
    //_di_IXMLDocument TempDoc = LoadXMLData(XML);
    //_di_IXMLNode xmldoc = TempDoc->ChildNodes->FindNode("Settings");
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Settings")
    {
        //if (!xmldoc->GetAttribute("BankPath").IsNull())
        //{
        m_OwnerClass->CurrentBank=xml.attribute("BankPath");
        //}
        //else
        //{
        //        m_OwnerClass->CurrentBank="";
        //}
        //if (!xmldoc->GetAttribute("PresetPath").IsNull())
        //{
        m_OwnerClass->CurrentPreset=xml.attribute("PresetPath");
        //}
        //else
        //{
        //       m_OwnerClass->CurrentPreset="";
        //}
        if (!m_OwnerClass->CurrentBank.isEmpty())
        {
            QString Expath=QDir(CPresets::Presets.VSTPath).absoluteFilePath(m_OwnerClass->CurrentBank);
            m_OwnerClass->LoadBank(Expath);
        }
        if (!m_OwnerClass->CurrentPreset.isEmpty())
        {
            QString Expath=QDir(CPresets::Presets.VSTPath).absoluteFilePath(m_OwnerClass->CurrentPreset);

            m_OwnerClass->LoadPreset(Expath);
        }
        int nParams=xml.attributeValue("NumParams");
        int p=xml.attributeValue("Preset");
        m_Effect->dispatcher(m_Effect,effSetProgram,0,p,NULL,0.0f);
        ui->PresetList->setCurrentRow(p);
        QDomLiteElement* Params = xml.elementByTag("Parameters");
        if (Params)
        {
            for (int i=nParams-1;i>-1;i--)
            {
                float Param=Params->attributeValue("Param" + QString::number(i));
                m_Effect->setParameter(m_Effect,i,Param);
                char strLabel[24];
                m_Effect->dispatcher(m_Effect,effGetParamName,i,0,strLabel,0.0f);

                qDebug() << i << Param << m_Effect->getParameter(m_Effect,0) << strLabel;
            }
        }
        QDomLiteElement* Position = xml.elementByTag("Position");
        if (Position)
        {
            this->move(Position->attributeValue("Left"),Position->attributeValue("Top"));
            this->setVisible(Position->attributeValue("Visible"));
            ui->PresetList->setVisible(Position->attributeValue("Presets"));
            ui->StatusEdit->setVisible(Position->attributeValue("Status"));
        }
    }
    if (m_Effect)
    {
        ViewResized();
    }
}

void CVSTHostForm::ParameterIndexChange(int Index)
{
    long i=Index;
    if (i>-1 && i !=CurrentParameter)
    {
        CurrentParameter=i;
        UpdateParam();
    }
}

void CVSTHostForm::ParameterChange(int Value)
{
    float newval=(float)Value*0.0001;
    if (newval<0)
    {
        newval=0;
    }
    if (newval>1)
    {
        newval=1;
    }
    m_Effect->setParameter(m_Effect,CurrentParameter,newval);
    UpdateParam();
}

void inline CVSTHostForm::UpdateParam()
{
    ui->dial->blockSignals(true);
    ui->dial->setValue(m_Effect->getParameter(m_Effect,CurrentParameter)*10000);
    ui->dial->blockSignals(false);
    char strDisplay[24];
    char strLabel[24];
    m_Effect->dispatcher(m_Effect,effGetParamLabel,CurrentParameter,0,strLabel,0.0f);
    m_Effect->dispatcher(m_Effect,effGetParamDisplay,CurrentParameter,0,strDisplay,0.0f);
    ui->Label1->setText(QString(strDisplay).trimmed());
    ui->Label2->setText(QString(strLabel).trimmed());
}

void CVSTHostForm::LoadBank()
{
    QString FN=QFileDialog::getOpenFileName(this,"Load Bank",CPresets::Presets.VSTPath,"Effect Bank (*.fxb)");
    if (!FN.isEmpty())
    {
        m_OwnerClass->LoadBank(FN);
    }
}

void CVSTHostForm::SaveBank()
{
    QString FN=QFileDialog::getSaveFileName(this,"Save Bank",CPresets::Presets.VSTPath,"Effect Bank (*.fxb)");
    if (!FN.isEmpty())
    {
        m_OwnerClass->SaveBank(FN);
    }
}

void CVSTHostForm::LoadPreset()
{
    QString FN=QFileDialog::getOpenFileName(this,"Load Preset",CPresets::Presets.VSTPath,"Effect Program (*.fxp)");
    if (!FN.isEmpty())
    {
        m_OwnerClass->LoadPreset(FN);
    }
}

void CVSTHostForm::SavePreset()
{
    QString FN=QFileDialog::getSaveFileName(this,"Save Preset",CPresets::Presets.VSTPath,"Effect Program (*.fxp)");
    if (!FN.isEmpty())
    {
        m_OwnerClass->SavePreset(FN);
    }
}

bool CVSTHostForm::event(QEvent *e)
{
    if (HasEditor)
    {
        if (e->type()==QEvent::Move)
        {
            ui->View->Move();
        }
    }
    if (e->type()==QEvent::Show)
    {
        m_MD=false;
    }
    if (e->type()==QEvent::NonClientAreaMouseButtonPress)
    {
        if (((QMouseEvent*)e)->button()==Qt::RightButton)
        {
            Popup->popup(mapToGlobal(((QMouseEvent*)e)->pos()));
        }
        else
        {
            if (HasEditor)
            {
                CursorStart=cursor().pos();
                PosStart=pos();
                qDebug() << "Press" << pos() << CursorStart;
                m_MD=true;
                ui->View->Hide();
            }
        }
    }
    if (e->type()==QEvent::NonClientAreaMouseMove)
    {
        if (HasEditor)
        {
            if (m_MD)
            {
                ui->View->Move();
            }
        }
    }
    if (e->type()==QEvent::NonClientAreaMouseButtonRelease)
    {
        if (HasEditor)
        {
            if (m_MD)
            {
                qDebug() << "Release" << pos() << cursor().pos()-CursorStart;
                move(PosStart+(cursor().pos()-CursorStart));
                ui->View->Move();
            }
            m_MD=false;
            ui->View->Show();
            return false;
        }
    }
    return CSoftSynthsForm::event(e);
}

void CVSTHostForm::timerEvent(QTimerEvent* /*e*/)
{
    //m_Effect->dispatcher(m_Effect,effEditIdle,0,0,NULL,0.0f);
    if (HasEditor)
    {
        if (!m_MD)
        {
            if (EffRect.size() != GetEffRect().size())
            {
                ViewResized();
            }
            /*
            else
            {
                if (ui->PresetList->hasFocus())
                {
                    ui->View->setFocus();
                    ui->View->repaint();
                }
            }
            */
        }
    }

}

QRect CVSTHostForm::GetEffRect()
{
    VSTRect* FormRect;
    m_Effect->dispatcher(m_Effect,effEditGetRect,0,0,&FormRect,0.0f);
    return QRect(0,0,FormRect->right-FormRect->left,FormRect->bottom-FormRect->top);
}

void CVSTHostForm::ViewResized()
{
    if (HasEditor)
    {
        EffRect=GetEffRect();
        ui->View->resize(EffRect.size());
    }
    else
    {
        ui->ParameterList->setGeometry(0,0,EffRect.width()/2,EffRect.height());
        ui->Label1->setGeometry(EffRect.width()/2,0,EffRect.width()/2,ui->Label1->height());
        ui->dial->setGeometry(EffRect.width()/2+((EffRect.width()/2-ui->dial->width())/2),ui->Label1->height(),ui->dial->width(),ui->dial->height());
        ui->Label1->setGeometry(EffRect.width()/2,ui->Label1->height()+ui->dial->height(),EffRect.width()/2,ui->Label2->height());
    }
    QRect r(EffRect);
    StatusAction->setChecked(ui->StatusEdit->isVisible());
    PresetAction->setChecked(ui->PresetList->isVisible());
    if (ui->StatusEdit->isVisible())
    {
        r.adjust(0,0,0,ui->StatusEdit->height());
    }
    if (ui->PresetList->isVisible())
    {
        r.adjust(0,0,ui->PresetList->width(),0);
    }
    ui->PresetList->setGeometry(EffRect.width(),0,ui->PresetList->width(),r.height());
    ui->StatusEdit->setGeometry(0,EffRect.height(),EffRect.width(),ui->StatusEdit->height());
    this->setFixedSize(r.size());
    ui->View->update();
}

void CVSTHostForm::StopTimer()
{
    HasEditor=false;
}

void CVSTHostForm::Unload()
{
    qDebug() << "CVSTHostForm Unload 1";
    m_OwnerClass->KillPlug();
    qDebug() << "CVSTHostForm Unload 2";
    HasEditor=false;
    qDebug() << "CVSTHostForm Unload 3";
    this->deleteLater();
    qDebug() << "CVSTHostForm Unload 4";
}
