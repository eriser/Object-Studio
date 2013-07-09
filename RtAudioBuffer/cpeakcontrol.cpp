#include "cpeakcontrol.h"
#include "ui_cpeakcontrol.h"

CPeakControl::CPeakControl(QWidget *parent) :
    QCanvas(parent,1),
    ui(new Ui::CPeakControl)
{
    ui->setupUi(this);
    m_Value=0;
    m_OldValue=0;
}

void CPeakControl::Reset()
{
    m_Max=0;
    m_Value=0;
    m_OldValue=0;
    QLinearGradient gradient;
    gradient.setStart(0,0);
    gradient.setFinalStop(0,height());
    gradient.setColorAt(0, QColor(60,60,60));
    gradient.setColorAt(0.8, Qt::black);
    SetPen(QPen(Qt::NoPen));
    SetBrush(gradient);
    Rectangle(rect());
    SetPen(Qt::darkGray);
    SetBrush(QBrush(Qt::NoBrush));
    Rectangle(HalfBorder,HalfBorder,width()-Border-1,height()-Border-1);
    CanvasLayers[0]->ClearTransparent();
    update();
}

void CPeakControl::SetSize()
{
    Reset();
    int Left=Border;
    int Right=width()-Border;
    int Width=Right-Left;
    lgBlack.setStart(Left,0);
    lgBlack.setFinalStop(Width,0);
    lgBlack.setColorAt(0,Qt::darkYellow);
    lgBlack.setColorAt(0.5,Qt::black);

    lgYellow.setStart(Left,0);
    lgYellow.setFinalStop(Width,0);
    lgYellow.setColorAt(0,Qt::white);
    lgYellow.setColorAt(0.3,Qt::yellow);
    lgYellow.setColorAt(1,Qt::darkYellow);

    lgDarkRed.setStart(Left,0);
    lgDarkRed.setFinalStop(Width,0);
    lgDarkRed.setColorAt(0,Qt::darkRed);
    lgDarkRed.setColorAt(0.5,Qt::black);

    lgRed.setStart(Left,0);
    lgRed.setFinalStop(Width,0);
    lgRed.setColorAt(0,Qt::white);
    lgRed.setColorAt(0.3,Qt::red);
    lgRed.setColorAt(1,Qt::darkRed);

    lgGreen.setStart(Left,0);
    lgGreen.setFinalStop(Width,0);
    lgGreen.setColorAt(0,Qt::white);
    lgGreen.setColorAt(0.3,Qt::green);
    lgGreen.setColorAt(1,Qt::darkGreen);
}

CPeakControl::~CPeakControl()
{
    delete ui;
}

const int CPeakControl::val2y(const float val, const float height)
{
    return height-((val*height*2)/3);
}

const QLinearGradient CPeakControl::y2col(const int y, const int height)
{
    if (height-y<((height*2)/3)) return lgGreen;
    if (height-y<((height*3)/4)) return lgYellow;
    return lgRed;
}

void CPeakControl::SetValue(const float Value)
{
    QCanvasLayer* L=CanvasLayers[0];
    if (Value>=m_Value-0.02)
    {
        m_Value=Value;
        if (m_Value > m_Max) m_Max=m_Value;
    }
    else
    {
        if (m_Value>0) m_Value-=0.02;
    }
    int HalfHeight=(height()/2)-Border;
    int val=val2y(m_Value,HalfHeight)+HalfBorder;
    if (val<HalfBorder) val=HalfBorder;
    int oldval=val2y(m_OldValue,HalfHeight)+HalfBorder;
    if (oldval<HalfBorder) oldval=HalfBorder;
    int Left=Border;
    int Right=width()-Border;
    int Width=Right-Left;
    L->SetPen(QPen(Qt::NoPen));
    QRect UpdateRect;
    if (m_Value>m_OldValue)
    {
        for (int i=oldval;i>=val;i--)
        {
            L->SetBrush(y2col(i-HalfBorder,HalfHeight));
            L->Rectangle(Left,i*2,Width,1);
        }
        UpdateRect.setRect(Left,val*2,Width,((oldval-val)*2)+1);
    }
    else if (m_Value<m_OldValue)
    {
        UpdateRect.setRect(Left,oldval*2,Width,((val-oldval)*2)+1);
        L->EraseTransparent(UpdateRect);
    }
    if (m_Max>0)
    {
        val=val2y(m_Max,HalfHeight)+HalfBorder;
        if (val<HalfBorder) val=HalfBorder;
        L->SetBrush(y2col(val-HalfBorder,HalfHeight));
        L->Rectangle(Left,val*2,Width,1);
    }
    m_OldValue=m_Value;
    repaint(UpdateRect);
}

void CPeakControl::resizeEvent(QResizeEvent *event)
{
    QCanvas::resizeEvent(event);
    SetSize();
}
