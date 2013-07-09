#include "ckeylayerscontrol.h"
#include "ui_ckeylayerscontrol.h"

CKeyLayersControl::CKeyLayersControl(QWidget *parent) :
    QCanvas(parent),
    ui(new Ui::CKeyLayersControl)
{
    ui->setupUi(this);
    MD=false;
    setMouseTracking(true);
    m_Sampler=NULL;
}

CKeyLayersControl::~CKeyLayersControl()
{
    delete ui;
}

void CKeyLayersControl::Init(CSamplerDevice *D)
{
    m_Sampler=D;
    Draw();
}

void CKeyLayersControl::resizeEvent(QResizeEvent* event)
{
    QCanvas::resizeEvent(event);
    Draw();
}

void inline CKeyLayersControl::MoveLines(QPoint P)
{
    if (m_Sampler->LayerCount()) PutPoint(m_Sampler->CurrentLayer(),QPoint(Graph2Vol(P.x()),Graph2Y(P.y())),SplitValue);
}

void inline CKeyLayersControl::PutPoint(CLayer *Layer, QPoint P, SplitterValues SV)
{
    if (SV==svUpper)
    {
        if ((P.y()>Layer->LP.LowerTop) && (P.y()<128))
        {
            Layer->LP.UpperTop=P.y();
            if (Layer->LP.UpperZero<P.y())
            {
                (Layer->LP.UpperZero=P.y());
            }
            int Vol=P.x();
            if (Vol>100){Vol=100;}
            if (Vol<5){Vol=5;}
            Layer->LP.Volume=Vol;
            Draw();
            emit CurrentLayerChanged(Layer->LP);
        }
    }
    else if (SV==svLower)
    {
        if ((P.y()<Layer->LP.UpperTop) && (P.y()>0))
        {
            Layer->LP.LowerTop=P.y();
            if (Layer->LP.LowerZero>P.y())
            {
                (Layer->LP.LowerZero=P.y());
            }

            int Vol=P.x();
            if (Vol>100){Vol=100;}
            if (Vol<5){Vol=5;}
            Layer->LP.Volume=Vol;
            Draw();
            emit CurrentLayerChanged(Layer->LP);
        }
    }
    else if (SV==svUpperZero)
    {
        if (P.y()>=Layer->LP.UpperTop && P.y()<128)
        {
            Layer->LP.UpperZero=P.y();
            if (Layer->LP.UpperZero>127) Layer->LP.UpperZero=127;
            Draw();
            emit CurrentLayerChanged(Layer->LP);
        }
        else
        {
            Layer->LP.UpperZero=P.y();
            if (Layer->LP.UpperZero>127) Layer->LP.UpperZero=127;
            P.setX(Layer->LP.Volume);
            PutPoint(Layer,P,svUpper);
        }
    }
    else if (SV==svLowerZero)
    {
        if (P.y()<=Layer->LP.LowerTop && P.y()>0)
        {
            Layer->LP.LowerZero=P.y();
            if (Layer->LP.LowerZero<0) Layer->LP.LowerZero=0;
            Draw();
            emit CurrentLayerChanged(Layer->LP);
        }
        else
        {
            Layer->LP.LowerZero=P.y();
            if (Layer->LP.LowerZero<0) Layer->LP.LowerZero=0;
            P.setX(Layer->LP.Volume);
            PutPoint(Layer,P,svLower);
        }

    }
}

bool inline InsidePoint(int X,int Y,QPoint P)
{
    return QRect(X,Y,0,0).adjusted(-4,-4,4,4).contains(P);
}


void CKeyLayersControl::Draw()
{
    ClearGradient();
    if (m_Sampler==NULL) return;
    for (int i=0;i<m_Sampler->LayerCount();i++)
    {
        CLayer::LayerParams LP=m_Sampler->LayerParams(i);
        int GrafVol=Vol2Graph(LP.Volume);
        SetPen(QPen(Qt::black));
        if (m_Sampler->CurrentLayerIndex==i) SetPen(QPen(Qt::red));
        Line(Vol2Graph(0),Y2Graph(LP.LowerZero),GrafVol,Y2Graph(LP.LowerTop));
        Line(GrafVol,Y2Graph(LP.LowerTop),GrafVol,Y2Graph(LP.UpperTop));
        Line(GrafVol,Y2Graph(LP.UpperTop),Vol2Graph(0),Y2Graph(LP.UpperZero));
        if (m_Sampler->CurrentLayerIndex==i)
        {
            SetBrush(QBrush(Qt::NoBrush));
            Circle(Vol2Graph(0),Y2Graph(LP.LowerZero),3);
            Circle(GrafVol,Y2Graph(LP.LowerTop),3);
            Circle(GrafVol,Y2Graph(LP.UpperTop),3);
            Circle(Vol2Graph(0),Y2Graph(LP.UpperZero),3);
        }
    }
    update();
}

int inline CKeyLayersControl::Vol2Graph(int Vol)
{
    return ((float)Vol*(float)(width()-(Layer_FrameWidth*2))*0.01)+Layer_FrameWidth;
}

int inline CKeyLayersControl::Graph2Vol(int X)
{
    return (float)(X-Layer_FrameWidth)/(float)(width()-(Layer_FrameWidth*2))*100.0;
}

int inline CKeyLayersControl::Y2Graph(int Y)
{
    return (((127.0-(float)Y)*(float)(height()-(Layer_FrameWidth*2)))/127.0)+Layer_FrameWidth;
}

int inline CKeyLayersControl::Graph2Y(int Y)
{
    return 127.0-(((float)(Y-Layer_FrameWidth)*127.0)/(float)(height()-(Layer_FrameWidth*2)));
}

void CKeyLayersControl::mousePressEvent(QMouseEvent *event)
{
    OldPoint=event->pos();
    MD=true;
    if (event->modifiers() & Qt::SHIFT)
    {
        StartMark=Graph2Y(OldPoint.y());
    }
}

void CKeyLayersControl::mouseMoveEvent(QMouseEvent *event)
{
    if (m_Sampler->LayerCount())
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
            CLayer::LayerParams LP=m_Sampler->LayerParams();
            if (InsidePoint(Vol2Graph(LP.Volume),Y2Graph(LP.UpperTop),P))
            {
                setCursor(Qt::SizeAllCursor);
                SplitValue=svUpper;
            }
            else if (InsidePoint(Vol2Graph(LP.Volume),Y2Graph(LP.LowerTop),P))
            {
                setCursor(Qt::SizeAllCursor);
                SplitValue=svLower;
            }
            else if (InsidePoint(Vol2Graph(0),Y2Graph(LP.LowerZero),P))
            {
                setCursor(Qt::SizeVerCursor);
                SplitValue=svLowerZero;
            }
            else if (InsidePoint(Vol2Graph(0),Y2Graph(LP.UpperZero),P))
            {
                setCursor(Qt::SizeVerCursor);
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

void CKeyLayersControl::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint P=event->pos();
    if (event->modifiers() & Qt::SHIFT)
    {
        if (MD)
        {
            if (StartMark < Graph2Y(P.y()))
            {
                emit Add(Graph2Y(P.y()),StartMark);
            }
            else
            {
                emit Add(StartMark,Graph2Y(P.y()));
            }
        }
    }
    else
    {
        if (MD)
        {
            if (SplitValue==svNone)
            {
                for (int i=0;i<m_Sampler->LayerCount();i++)
                {
                    CLayer::LayerParams LP=m_Sampler->LayerParams(i);
                    if ((P.y()>(Y2Graph(LP.UpperTop))) && (P.y()<Y2Graph(LP.LowerTop)))
                    {
                        emit LayerIndexChanged(i);
                        //Draw();
                        //emit CurrentLayerChanged(LP);
                        MD=false;
                        return;
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
