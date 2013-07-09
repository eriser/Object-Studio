#include "cknobcontrol.h"
#include "ui_cknobcontrol.h"

CKnobControl::CKnobControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CKnobControl)
{
    ui->setupUi(this);
    popup=new QMenu(this);
    spinbox=new QDoubleSpinBox(this);
    spinboxAction=new QWidgetAction(this);
    spinboxAction->setDefaultWidget(spinbox);
    connect(ui->dial,SIGNAL(valueChanged(int)),this,SIGNAL(ValueChanged(int)));
    mapper=new QSignalMapper(this);
    connect(mapper,SIGNAL(mapped(int)),ui->dial,SLOT(setValue(int)));
    ui->label->setEffect(EffectLabel::Raised);
    ui->label->setShadowColor(Qt::white);
}

CKnobControl::~CKnobControl()
{
    delete ui;
}

int CKnobControl::GetValue()
{
    return ui->dial->value();
}

void CKnobControl::SetValue(int Value, const ParameterType &p)
{
    Parameter=p;
    ui->dial->blockSignals(true);
    ui->dial->setMinimum(p.Min);
    ui->dial->setMaximum(p.Max);
    ui->dial->setValue(Value);
    ui->dial->blockSignals(false);
    SetTexts(Value,p);
}

void CKnobControl::SetTexts(int Value, const ParameterType &p)
{
    ui->label->setText(p.Name);
    if (p.Type==ParameterType::dB)
    {
        ui->label_2->setText(QString::number(lin2db((float)Value*0.01),'f',2)+" "+p.Unit);
    }
    else if (p.Type==ParameterType::SelectBox)
    {
        QStringList l=p.List.split(ParameterList::ParameterListSeparator);
        ui->label_2->setText(l[Value]);
    }
    else
    {
        float DecFactor=p.DecimalFactor;
        if (DecFactor==0) DecFactor=1;
        ui->label_2->setText(QString::number((float)Value/DecFactor,'f',QString::number(DecFactor).length()-1)+" "+p.Unit);
    }
}

void CKnobControl::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::RightButton)
    {
        popup->clear();
        if (Parameter.Type==ParameterType::Numeric)
        {
            float DecFactor=Parameter.DecimalFactor;
            if (DecFactor==0) DecFactor=1;
            disconnect(spinbox);
            spinbox->setAttribute(Qt::WA_MacShowFocusRect, 0);
            spinbox->setMinimum(Parameter.Min);
            spinbox->setMaximum(Parameter.Max);
            spinbox->setDecimals(QString::number(DecFactor).length()-1);
            spinbox->setValue((float)ui->dial->value()/DecFactor);
            spinbox->selectAll();
            popup->addAction(spinboxAction);
            connect(spinbox,SIGNAL(valueChanged(double)),this,SLOT(SetNumericValue(double)));
            popup->popup(mapToGlobal(event->pos()));
            spinbox->setFocus();
        }
        if (Parameter.Type==ParameterType::SelectBox)
        {
            QStringList l=Parameter.List.split(ParameterList::ParameterListSeparator);
            for (int i=0;i<l.count();i++)
            {
                QString s=l[i];
                QAction* a=popup->addAction(s);
                a->setCheckable(true);
                a->setChecked(ui->dial->value()==i);
                if (ui->dial->value()==i) popup->setActiveAction(a);
                mapper->setMapping(a,i);
                connect(a,SIGNAL(triggered()),mapper,SLOT(map()));
            }
            popup->popup(mapToGlobal(event->pos()));
        }
        if (Parameter.Type==ParameterType::dB)
        {
            disconnect(spinbox);
            spinbox->setAttribute(Qt::WA_MacShowFocusRect, 0);
            spinbox->setMinimum(lin2db((float)Parameter.Min*0.01));
            spinbox->setMaximum(lin2db((float)Parameter.Max*0.01));
            spinbox->setDecimals(2);
            spinbox->setValue(lin2db((float)ui->dial->value()*0.01));
            spinbox->selectAll();
            popup->addAction(spinboxAction);
            connect(spinbox,SIGNAL(valueChanged(double)),this,SLOT(SetdBValue(double)));
            popup->popup(mapToGlobal(event->pos()));
            spinbox->setFocus();
        }
    }
}

void CKnobControl::SetNumericValue(double Value)
{
    float DecFactor=Parameter.DecimalFactor;
    if (DecFactor==0) DecFactor=1;
    int Val=Value*DecFactor;
    ui->dial->setValue(Val);
    spinbox->selectAll();
    spinbox->setFocus();
}

void CKnobControl::SetdBValue(double Value)
{
    double Val=db2lin(Value)*100.0;
    ui->dial->setValue(round(Val));
    spinbox->selectAll();
    spinbox->setFocus();
}
