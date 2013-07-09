#include "ckeyrangescontrol.h"
#include "ui_ckeyrangescontrol.h"

CKeyRangesControl::CKeyRangesControl(QWidget *parent) :
    QCanvas(parent,1),
    ui(new Ui::CKeyRangesControl)
{
    ui->setupUi(this);
    MD=false;
    setMouseTracking(true);
    m_Sampler=NULL;
}

CKeyRangesControl::~CKeyRangesControl()
{
    delete ui;
}

void CKeyRangesControl::Init(CSamplerDevice *D)
{
    m_Sampler=D;
    Draw();
}

void CKeyRangesControl::resizeEvent(QResizeEvent* event)
{
    QCanvas::resizeEvent(event);
    Draw();
}

void inline CKeyRangesControl::MoveLines(QPoint P)
{
    if (m_Sampler->RangeCount()) PutPoint(m_Sampler->CurrentRange(),QPoint(Graph2X(P.x()),Graph2Vol(P.y())),SplitValue);
}

void inline CKeyRangesControl::PutPoint(CSampleKeyRange* Range,QPoint P,SplitterValues SV)
{
    if (SV==svUpper)
    {
        if ((P.x()>Range->RP.LowerTop) && (P.x()<128))
        {
            Range->RP.UpperTop=P.x();
            if (Range->RP.UpperZero<P.x()) Range->RP.UpperZero=P.x();
            if (Range->RP.UpperZero>127) Range->RP.UpperZero=127;
            int Vol=P.y();
            if (Vol>100) Vol=100;
            if (Vol<5) Vol=5;
            Range->RP.Volume=Vol;

            DrawLines();
            emit CurrentRangeChanged(Range->RP);
        }
    }
    else if (SV==svLower)
    {
        if ((P.x()<Range->RP.UpperTop) && (P.x()>=0))
        {
            Range->RP.LowerTop=P.x();
            if (Range->RP.LowerZero>P.x()) Range->RP.LowerZero=P.x();
            if (Range->RP.LowerZero<0) Range->RP.LowerZero=0;
            int Vol=P.y();
            if (Vol>100) Vol=100;
            if (Vol<5) Vol=5;
            Range->RP.Volume=Vol;

            DrawLines();
            emit CurrentRangeChanged(Range->RP);
        }
    }
    else if (SV==svUpperZero)
    {
        if ((P.x()>=Range->RP.UpperTop) && (P.x()<128))
        {
            Range->RP.UpperZero=P.x();
            DrawLines();
            emit CurrentRangeChanged(Range->RP);
        }
        else
        {
            Range->RP.UpperZero=P.x();
            P.setY(Range->RP.Volume);
            PutPoint(Range,P,svUpper);
        }
    }
    else if (SV==svLowerZero)
    {
        if ((P.x()<=Range->RP.LowerTop) && (P.x()>=0))
        {
            Range->RP.LowerZero=P.x();
            DrawLines();
            emit CurrentRangeChanged(Range->RP);
        }
        else
        {
            Range->RP.LowerZero=P.x();
            P.setY(Range->RP.Volume);
            PutPoint(Range,P,svLower);
        }
    }
}


bool inline InsidePoint(int X,int Y,QPoint P)
{
    QRect r(X-6,Y-6,12,12);
    return r.contains(P);
}


void CKeyRangesControl::Draw()
{    
    DrawKeys();
    DrawLines();
}

void CKeyRangesControl::DrawLines()
{
    if (m_Sampler==NULL) return;
    CanvasLayers[0]->ClearTransparent();
    for (int i=0;i<m_Sampler->RangeCount();i++)
    {
        CSampleKeyRange::RangeParams RP=m_Sampler->RangeParams(m_Sampler->CurrentLayerIndex,i);
        int GrafVol=Vol2Graph(RP.Volume);
        CanvasLayers[0]->SetPen(QPen(Qt::black));
        if (m_Sampler->CurrentRangeIndex==i) CanvasLayers[0]->SetPen(QPen(Qt::red));
        CanvasLayers[0]->Line(X2Graph(RP.LowerZero),Vol2Graph(0),X2Graph(RP.LowerTop),GrafVol);
        CanvasLayers[0]->Line(X2Graph(RP.LowerTop),GrafVol,X2Graph(RP.UpperTop),GrafVol);
        CanvasLayers[0]->Line(X2Graph(RP.UpperTop),GrafVol,X2Graph(RP.UpperZero),Vol2Graph(0));
        if (m_Sampler->CurrentRangeIndex==i)
        {
            CanvasLayers[0]->SetBrush(QBrush(Qt::NoBrush));
            CanvasLayers[0]->Circle(X2Graph(RP.LowerZero),Vol2Graph(0),3);
            CanvasLayers[0]->Circle(X2Graph(RP.LowerTop),GrafVol,3);
            CanvasLayers[0]->Circle(X2Graph(RP.UpperTop),GrafVol,3);
            CanvasLayers[0]->Circle(X2Graph(RP.UpperZero),Vol2Graph(0),3);
        }
    }
    update();
}

int inline CKeyRangesControl::Vol2Graph(int Vol)
{
    return (((100-Vol)*(height()-(Range_FrameWidth*2)))/100)+Range_FrameWidth;
}

int inline CKeyRangesControl::Graph2Vol(int Y)
{
    return 100-(((Y-Range_FrameWidth)*100)/(height()-(Range_FrameWidth*2)));
}

int inline CKeyRangesControl::X2Graph(int X)
{
    return (((float)X*(float)(width()-(Range_FrameWidth*2)))/127.0)+Range_FrameWidth;
}

int inline CKeyRangesControl::Graph2X(int X)
{
    return ((float)(X-Range_FrameWidth)*127.0)/((float)(width()-(Range_FrameWidth*2)));
}

void CKeyRangesControl::DrawKeys()
{
    ClearGradient();
    SetPen(QPen(Qt::darkGray));
    for (int i=0;i<128;i++)
    {
        int XG=X2Graph(i);
        Line(XG,0,XG,height()-1);
        int Key=i % 12;
        int KeyHeight=(height()*3)/5;
        if (Key==1 || Key==3 || Key==6 || Key==8 || Key==10)
        {
            Line(XG+1,KeyHeight-5,XG+1,KeyHeight);
            for (int i1=2;i1<5;i1++)
            {
                Line(XG+i1,0,XG+i1,KeyHeight);
            }
        }
    }
}

void CKeyRangesControl::mousePressEvent(QMouseEvent *event)
{
    OldPoint=event->pos();
    MD=true;
    if (event->modifiers() & Qt::SHIFT)
    {
        StartMark=Graph2X(OldPoint.x());
    }
}

void CKeyRangesControl::mouseMoveEvent(QMouseEvent *event)
{
    if (m_Sampler->RangeCount())
    {
        QPoint P=event->pos();
        if (MD)
        {
            if (P != OldPoint)
            {
                MoveLines(P);
                OldPoint=P;
            }
        }
        else
        {
            CSampleKeyRange::RangeParams RP=m_Sampler->RangeParams();
            if (InsidePoint(X2Graph(RP.UpperTop),Vol2Graph(RP.Volume),P))
            {
                setCursor(Qt::SizeAllCursor);
                SplitValue=svUpper;
            }
            else if (InsidePoint(X2Graph(RP.LowerTop),Vol2Graph(RP.Volume),P))
            {
                setCursor(Qt::SizeAllCursor);
                SplitValue=svLower;
            }
            else if (InsidePoint(X2Graph(RP.LowerZero),Vol2Graph(0),P))
            {
                setCursor(Qt::SizeHorCursor);
                SplitValue=svLowerZero;
            }
            else if (InsidePoint(X2Graph(RP.UpperZero),Vol2Graph(0),P))
            {
                setCursor(Qt::SizeHorCursor);
                SplitValue=svUpperZero;
            }
            else
            {
                setCursor(Qt::ArrowCursor);
                SplitValue=svNone;
            }
        }
    }
}

void CKeyRangesControl::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint P=event->pos();
    if (event->modifiers() & Qt::SHIFT)
    {
        if (MD)
        {
            if (StartMark < Graph2X(P.x())) emit Add(Graph2X(P.x()),StartMark);
            if (StartMark > Graph2X(P.x())) emit Add(StartMark,Graph2X(P.x()));
            emit LoadWaveFile();
        }
    }
    else
    {
        if (MD)
        {
            if (SplitValue==svNone)
            {
                for (int i=0;i<m_Sampler->RangeCount();i++)
                {
                    CSampleKeyRange::RangeParams RP=m_Sampler->RangeParams(m_Sampler->CurrentLayerIndex,i);
                    if ((P.x()>(X2Graph(RP.LowerTop))) && (P.x()<X2Graph(RP.UpperTop)))
                    {
                        if (i != m_Sampler->CurrentRangeIndex)
                        {
                            emit RangeIndexChanged(i);
                            MD=false;
                            return;
                        }
                    }
                }
            }
            else
            {
                MoveLines(P);
            }
        }
    }
    MD=false;
}
