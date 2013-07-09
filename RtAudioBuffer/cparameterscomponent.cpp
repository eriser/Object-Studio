#include "cparameterscomponent.h"
#include "ui_cparameterscomponent.h"

CParametersComponent::CParametersComponent(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CParametersComponent)
{
    ui->setupUi(this);
    mapper=new QSignalMapper(this);
    connect(mapper,SIGNAL(mapped(int)),this,SLOT(ValueChanged(int)));
    Spacer=new QWidget(this);
    Spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->horizontalLayout_2->addWidget(Spacer);
    ui->label->setEffect(EffectLabel::Raised);
    ui->label->setShadowColor(Qt::white);
}

CParametersComponent::~CParametersComponent()
{
    disconnect(mapper);
    delete ui;
}

void CParametersComponent::ShowParameters(IDevice *Device,QString Title)
{
    setUpdatesEnabled(false);
    ui->DialsFrame->hide();
    foreach (CKnobControl* w,Dials) w->hide();
    Parameters.clear();
    ui->horizontalLayout_2->removeWidget(Spacer);
    m_D=Device;
    if (Device != NULL)
    {
        for (int i=0;i<Device->ParameterCount();i++)
        {
            ParameterType p=Device->Parameter(i);
            Parameters.append(p);
            CKnobControl* d;
            if (i>Dials.count()-1)
            {
                d=new CKnobControl(this);
                ui->horizontalLayout_2->addWidget(d);
                connect(d,SIGNAL(ValueChanged(int)),mapper,SLOT(map()));
                mapper->setMapping(d,i);
                Dials.append(d);
            }
            else
            {
                d=Dials[i];
            }
            d->SetValue(Device->GetParameterValue(i),p);
            d->show();
        }
    }
    ui->horizontalLayout_2->addWidget(Spacer);
    ui->DialsFrame->show();
    setUpdatesEnabled(true);
    ui->label->setText(Title);
}

void CParametersComponent::ValueChanged(int i)
{
        ParameterType p=Parameters[i];
        Dials[i]->SetTexts(Dials[i]->GetValue(),p);
        m_D->SetParameterValue(i,Dials[i]->GetValue());
        emit ParameterChanged(m_D,i,Dials[i]->GetValue());
}

void CParametersComponent::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::RightButton)
    {
        emit Popup(mapToGlobal(event->pos()));
    }
}
