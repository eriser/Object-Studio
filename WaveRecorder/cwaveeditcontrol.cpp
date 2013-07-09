#include "cwaveeditcontrol.h"
#include "ui_cwaveeditcontrol.h"

CWaveEditControl::CWaveEditControl(QWidget *parent) :
    QCanvas(parent,1),
    ui(new Ui::CWaveEditControl)
{
    ui->setupUi(this);
    MD=false;
    setMouseTracking(true);
    Zoom=1;
    isMaxZoom=false;
    m_Start=0;
    Enabled=false;
    m_Length=0;
    m_Buffer=0;
}

CWaveEditControl::~CWaveEditControl()
{
    delete ui;
}

void CWaveEditControl::Init(CWaveGenerator *WG, CWaveGenerator::LoopParameters LP,bool LoopOn)
{
    m_LP=LP;
    m_WG=WG;
    m_LoopOn=LoopOn;
    if (size() != BackImage.size()) SetSize(size());
    if (m_Buffer != m_WG->BufferPointer(0))
    {
        m_Buffer=m_WG->BufferPointer(0);
        m_Length=m_WG->GetLength();
        m_Start=0;
        ZoomMax();
    }
    else
    {
        Draw(m_LP);
    }
}

void CWaveEditControl::Draw(CWaveGenerator::LoopParameters LP)
{
    m_LP=LP;
    if (Enabled)
    {
        if (height())
        {
            DrawWave();
            DrawLines(m_LP,m_LoopOn);
        }
    }
}

void CWaveEditControl::DrawLines(CWaveGenerator::LoopParameters LP, bool LoopOn)
{
    QCanvasLayer* L=CanvasLayers[0];
    L->ClearTransparent();
    DrawLine(LP.Start);
    DrawLine(LP.End);
    if (LoopOn)
    {
        DrawLine(LP.LoopStart);
        DrawLine(LP.LoopEnd);
    }
    else
    {
        int Vol=Vol2Pos(LP.Volume);//height()-(((LinePic->ClientHeight-2)*LP.Volume)/100)-2;
        L->SetPen(Qt::red);
        L->MoveTo(PicPos(LP.Start),Vol2Pos(0));
        L->LineTo(PicPos(LP.Start+LP.FadeIn),Vol);
        L->LineTo(PicPos(LP.End-LP.FadeOut),Vol);
        L->LineTo(PicPos(LP.End),Vol2Pos(0));

        L->Circle(PicPos(LP.Start+LP.FadeIn),Vol,3);
        L->Circle(PicPos(LP.End-LP.FadeOut),Vol,3);

    }
    update();
}

void CWaveEditControl::resizeEvent(QResizeEvent* event)
{
    QCanvas::resizeEvent(event);
    if (isMaxZoom)
    {
        ZoomMax();
    }
    else
    {
        Draw(m_LP);
    }
}

void CWaveEditControl::DrawWave()
{
    float XInc=0.01;
    float PosInc=Zoom*XInc;
    if (PosInc < 1.0)
    {
        PosInc=1;
        XInc=PosInc/Zoom;
    }
    qDebug() << PosInc << XInc << Zoom;
    float YFactor=height()*0.5;
    int HalfHeight=height()*0.5;
    int Width=width();
    if (m_Length-m_Start<(Width/XInc)) m_Start=m_Length-(Width/XInc);
    ClearGradient();
    SetPen(Qt::black);
    if (m_Length)
    {
        for (int Channel=0;Channel<m_WG->Channels;Channel++)
        {
            float* Buffer=m_WG->BufferPointer(Channel);
            float X=0;
            double Pos=m_Start;
            MoveTo((int)X,HalfHeight+(Buffer[(size_t)Pos]*YFactor));
            while (X<Width)
            {
                LineTo((int)X,HalfHeight+(Buffer[(size_t)Pos]*YFactor));
                Pos+=PosInc;
                if (Pos>m_Length) break;
                X+=XInc;
            }
        }
    }
}

void inline CWaveEditControl::DrawLine(size_t Position)
{
    if (Position<=m_Length)
    {
        QCanvasLayer* L=CanvasLayers[0];
        L->SetPen(Qt::red);
        int Pos=PicPos(Position);
        L->Line(Pos,Vol2Pos(0),Pos,Vol2Pos(100));
    }
}

void CWaveEditControl::ZoomIn()
{
    int MaxZoom=m_Length/width();
    if (Zoom<MaxZoom)
    {
        isMaxZoom=false;
        Zoom=Zoom*2;
        if (Zoom>MaxZoom)
        {
            Zoom=MaxZoom;
            isMaxZoom=true;
        }
        Draw(m_LP);
    }
}

void CWaveEditControl::ZoomOut()
{
    if (Zoom>1)
    {
        Zoom=Zoom/2;
        if (Zoom<1) Zoom=1;
        isMaxZoom=false;
        Draw(m_LP);
    }
}

void CWaveEditControl::ZoomMax()
{
    float MaxZoom=((double)m_Length)/(float)width();
    if (MaxZoom<1)
    {
        MaxZoom=1;
    }
    Zoom=MaxZoom;
    isMaxZoom=true;
    Draw(m_LP);
}

void CWaveEditControl::ZoomMin()
{
    Zoom=1;
    isMaxZoom=false;
    Draw(m_LP);
}

size_t CWaveEditControl::BufferPos(int X)
{
    size_t Pos= m_Start+((double)X*Zoom);
    if (Pos>m_Length) return m_Length;
    return Pos;
}

int CWaveEditControl::PicPos(size_t Position)
{
    return (double)(Position-m_Start)/Zoom;
}

int CWaveEditControl::Vol2Pos(int Vol)
{
    float Height=height();
    return height()-(Height*Vol*0.01);
}

int CWaveEditControl::Pos2Vol(int Pos)
{
    float Height=height();
    int Vol= ((Height-Pos)*100)/Height;
    if (Vol<0)
    {
        Vol=0;
    }
    if (Vol>100)
    {
        Vol=100;
    }
    return Vol;
}

int CWaveEditControl::ScrollMax()
{
    if (m_Length>(width()*Zoom))
    {
        return m_Length-(width()*Zoom);
    }
    else
    {
        return 0;
    }
}

void CWaveEditControl::SetStart(int Start)
{
    m_Start=Start;
    Draw(m_LP);
}

void inline CWaveEditControl::MoveWaveLines(QPoint p)
{
    size_t Pos=BufferPos(p.x());
    if (Pos>m_Length) Pos=m_Length;
    if (WaveLines==wlStart)
    {
        if (m_LoopOn)
        {
            if (Pos>m_LP.LoopStart) Pos=m_LP.LoopStart;
        }
        else
        {
            if (Pos>m_LP.End-(m_LP.FadeIn+m_LP.FadeOut)) Pos=m_LP.End-(m_LP.FadeIn+m_LP.FadeOut);
        }
        m_LP.Start=Pos;
    }
    else if (WaveLines==wlEnd)
    {
        if (m_LoopOn)
        {
            if (Pos<m_LP.LoopEnd) Pos=m_LP.LoopEnd;
        }
        else
        {
            if (Pos<m_LP.Start+m_LP.FadeIn+m_LP.FadeOut) Pos=m_LP.Start+m_LP.FadeIn+m_LP.FadeOut;
        }
        m_LP.End=Pos;
    }
    else if (WaveLines==wlLoopStart)
    {
        if (Pos<m_LP.Start) Pos=m_LP.Start;
        if (Pos>m_LP.LoopEnd) Pos=m_LP.LoopEnd;
        m_LP.LoopStart=Pos;
    }
    else if (WaveLines==wlLoopEnd)
    {
        if (Pos<m_LP.LoopStart) Pos=m_LP.LoopStart;
        if (Pos>m_LP.End) Pos=m_LP.End;
        m_LP.LoopEnd=Pos;
    }
    else if (WaveLines==wlFadeIn)
    {
        if (Pos<m_LP.Start) Pos=m_LP.Start;
        if (Pos>m_LP.End-m_LP.FadeOut) Pos=m_LP.End-m_LP.FadeOut;
            m_LP.FadeIn=Pos-m_LP.Start;
            m_LP.Volume=Pos2Vol(p.y());
    }
    else if (WaveLines==wlFadeOut)
    {
        if (Pos<m_LP.Start+m_LP.FadeIn) Pos=m_LP.Start+m_LP.FadeIn;
        if (Pos>m_LP.End) Pos=m_LP.End;
        m_LP.FadeOut=m_LP.End-Pos;
        m_LP.Volume=Pos2Vol(p.y());
    }
    DrawLines(m_LP,m_LoopOn);
    emit ParameterChanged(m_LP);
}

void CWaveEditControl::mousePressEvent(QMouseEvent* /*event*/)
{
    MD=true;
}

bool inline InsideLine(int X,int LineX)
{
    if (X>=LineX-4 && X<=LineX+4)
    {
        return true;
    }
    return false;
}

void CWaveEditControl::mouseMoveEvent(QMouseEvent *event)
{

    if (Enabled)
    {
        //QPoint Pos(mapToScene(event->pos().x(),event->pos().y()).toPoint());
        QPoint Pos(event->pos());
        if (!MD)
        {
            if (m_LoopOn)
            {
                if (InsideLine(Pos.x(),PicPos(m_LP.Start)))
                {
                    //Image3->Cursor=crHSplit;
                    setCursor(Qt::SizeHorCursor);
                    WaveLines=wlStart;
                }
                else if (InsideLine(Pos.x(),PicPos(m_LP.End)))
                {
                    //Image3->Cursor=crHSplit;
                    setCursor(Qt::SizeHorCursor);
                    WaveLines=wlEnd;
                }
                else if (InsideLine(Pos.x(),PicPos(m_LP.LoopStart)))
                {
                    //Image3->Cursor=crHSplit;
                    setCursor(Qt::SizeHorCursor);
                    WaveLines=wlLoopStart;
                }
                else if (InsideLine(Pos.x(),PicPos(m_LP.LoopEnd)))
                {
                    //Image3->Cursor=crHSplit;
                    setCursor(Qt::SizeHorCursor);
                    WaveLines=wlLoopEnd;
                }
                else
                {
                    //Image3->Cursor=crDefault;
                    setCursor(Qt::ArrowCursor);
                    WaveLines=wlNone;
                }
            }
            else
            {
                if ((InsideLine(Pos.x(),PicPos(m_LP.Start+m_LP.FadeIn))) &&  (InsideLine(Pos.y(),Vol2Pos(m_LP.Volume))))
                {
                    //Image3->Cursor=crSizeAll;
                    setCursor(Qt::SizeAllCursor);
                    WaveLines=wlFadeIn;
                }
                else if ((InsideLine(Pos.x(),PicPos(m_LP.End-m_LP.FadeOut))) &&  (InsideLine(Pos.y(),Vol2Pos(m_LP.Volume))))
                {
                    //Image3->Cursor=crSizeAll;
                    setCursor(Qt::SizeAllCursor);
                    WaveLines=wlFadeOut;
                }
                else if (InsideLine(Pos.x(),PicPos(m_LP.Start)))
                {
                    //Image3->Cursor=crHSplit;
                    setCursor(Qt::SizeHorCursor);
                    WaveLines=wlStart;
                }
                else if (InsideLine(Pos.x(),PicPos(m_LP.End)))
                {
                    //Image3->Cursor=crHSplit;
                    setCursor(Qt::SizeHorCursor);
                    WaveLines=wlEnd;
                }
                else
                {
                    //Image3->Cursor=crDefault;
                    setCursor(Qt::ArrowCursor);
                    WaveLines=wlNone;
                }
            }
        }
        else
        {
            if ((WaveLines==wlFadeIn) || (WaveLines==wlFadeOut))
            {
                if (Pos != OldPos)
                {
                    MoveWaveLines(Pos);
                    OldPos=Pos;
                }
            }
            else
            {
                if (Pos.x() != OldPos.x())
                {
                    MoveWaveLines(Pos);
                    OldPos.setX(Pos.x());
                }
            }
        }
    }

}
//---------------------------------------------------------------------------
void CWaveEditControl::mouseReleaseEvent(QMouseEvent *event)
{
    if (Enabled)
    {
        //QPoint Pos(mapToScene(event->pos().x(),event->pos().y()).toPoint());
        if (MD)
        {
            MoveWaveLines(event->pos());
        }
    }
    MD=false;

}
