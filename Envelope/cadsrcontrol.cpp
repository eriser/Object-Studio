#include "cadsrcontrol.h"
#include "ui_cadsrcontrol.h"

#define ADSR_FrameWidth 4

CADSRControl::CADSRControl(QWidget *parent) :
    QCanvas(parent),
    ui(new Ui::CADSRControl)
{
    ui->setupUi(this);
    MD=false;
    /*
    setScene(&Scene);
    setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    Scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::HighQualityAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    */
    setMouseTracking(true);
    /*
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(0);
    setLineWidth(0);
    */
}

CADSRControl::~CADSRControl()
{
    delete ui;
}

int inline CADSRControl::Time2X(int Time)
{
    return ((Time*(width()-(ADSR_FrameWidth*2)))/ADSRControl::ADSR_MaxWidth)+ADSR_FrameWidth;
}

int inline CADSRControl::Vol2Y(int Vol)
{
    return ((100-Vol)*(height()-(ADSR_FrameWidth*2))*0.01)+ADSR_FrameWidth;
}

int inline CADSRControl::X2Time(int X)
{
    return ((X-ADSR_FrameWidth)*ADSRControl::ADSR_MaxWidth)/(width()-(ADSR_FrameWidth*2));
}

int inline CADSRControl::Y2Vol(int Y)
{
    return 100-(((Y-ADSR_FrameWidth)*100)/(height()-(ADSR_FrameWidth*2)));
}

void CADSRControl::Draw(CADSR::ADSRParams ADSRParams)
{
    AP=ADSRParams;
    ClearGradient();
    SetPen(Qt::black);
    Line(Time2X(0),Vol2Y(0),Time2X(AP.Attack),Vol2Y(100));
    Line(Time2X(AP.Attack),Vol2Y(100),Time2X(AP.Attack+AP.Decay),Vol2Y(AP.Sustain));
    SetPen(QPen(Qt::darkGray,0,Qt::DotLine));
    Line(Time2X(AP.Attack+AP.Decay),Vol2Y(AP.Sustain),Time2X(ADSRControl::ADSR_ReleaseStart),Vol2Y(AP.Sustain));
    SetPen(Qt::black);
    Line(Time2X(ADSRControl::ADSR_ReleaseStart),Vol2Y(AP.Sustain),Time2X(ADSRControl::ADSR_ReleaseStart+AP.Release),Vol2Y(0));

    SetPenBrush(Qt::red);
    SetBrush(QBrush(Qt::NoBrush));
    Circle(Time2X(AP.Attack),Vol2Y(100),3);
    Circle(Time2X(AP.Attack+AP.Decay),Vol2Y(AP.Sustain),3);
    Circle(Time2X(ADSRControl::ADSR_ReleaseStart),Vol2Y(AP.Sustain),3);
    Circle(Time2X(ADSRControl::ADSR_ReleaseStart+AP.Release),Vol2Y(0),3);
    update();
}

bool InsidePoint(int X,int Y,QPoint P)
{
    return QRect(X,Y,0,0).adjusted(-4,-4,4,4).contains(P);
}

void inline CADSRControl::MoveLines(QPoint P)
{
    P.setY(Y2Vol(P.y()));
    P.setX(X2Time(P.x()));
    PutPoint(P,SplitValue);
}

void inline CADSRControl::PutPoint(QPoint P,SplitterValuesADSR SV)
{
    if (SV==svDecay)
    {
        int D=P.x()-AP.Attack;
        if (D>ADSRControl::ADSR_MaxTime){D=ADSRControl::ADSR_MaxTime;}
        if (D<0){D=0;}
        AP.Decay=D;
        int Vol=P.y();
        if (Vol>100){Vol=100;}
        if (Vol<0){Vol=0;}
        AP.Sustain=Vol;
        Draw(AP);
        emit Changed(AP);
    }
    else if (SV==svSustain)
    {
        int Vol=P.y();
        if (Vol>100){Vol=100;}
        if (Vol<0){Vol=0;}
        AP.Sustain=Vol;
        Draw(AP);
        emit Changed(AP);
    }
    else if (SV==svAttack)
    {
        if ((P.x()>=0) && (P.x()<=ADSRControl::ADSR_MaxTime))
        {
            AP.Attack=P.x();
            Draw(AP);
            emit Changed(AP);
        }
    }
    else if (SV==svRelease)
    {
        if ((P.x()>=ADSRControl::ADSR_ReleaseStart) && (P.x()<=ADSRControl::ADSR_ReleaseStart+ADSRControl::ADSR_MaxTime))
        {
            AP.Release=P.x()-ADSRControl::ADSR_ReleaseStart;
            Draw(AP);
            emit Changed(AP);
        }
    }

}

void CADSRControl::mousePressEvent(QMouseEvent* /*event*/)
{
    MD=true;
}

void CADSRControl::mouseMoveEvent(QMouseEvent *event)
{
    //QPoint P=mapToScene(event->pos().x(),event->pos().y()).toPoint();
    QPoint P=event->pos();
    if (!MD)
    {
        if (InsidePoint(Time2X(AP.Attack),Vol2Y(100),P))
        {
            setCursor(Qt::SizeHorCursor);
            SplitValue=svAttack;
        }
        else if (InsidePoint(Time2X(AP.Attack+AP.Decay),Vol2Y(AP.Sustain),P))
        {
            setCursor(Qt::SizeAllCursor);
            SplitValue=svDecay;
        }
        else if (InsidePoint(Time2X(ADSRControl::ADSR_ReleaseStart),Vol2Y(AP.Sustain),P))
        {
            setCursor(Qt::SizeVerCursor);
            SplitValue=svSustain;
        }
        else if (InsidePoint(Time2X(ADSRControl::ADSR_ReleaseStart+AP.Release),Vol2Y(0),P))
        {
            setCursor(Qt::SizeHorCursor);
            SplitValue=svRelease;
        }
        else
        {
            setCursor(Qt::ArrowCursor);
            SplitValue=svADSRNone;
        }
    }
    else
    {
        if (P != OldPoint)
        {
            MoveLines(P);
            OldPoint=P;
        }
    }
}

void CADSRControl::mouseReleaseEvent(QMouseEvent *event)
{
    //QPoint P=mapToScene(event->pos().x(),event->pos().y()).toPoint();
    QPoint P=event->pos();
    if (MD)
    {
        if (SplitValue!=svADSRNone)
        {
            MoveLines(P);
        }
    }
    MD=false;
}

void CADSRControl::resizeEvent(QResizeEvent* event)
{
    QCanvas::resizeEvent(event);
    Draw(AP);
}
