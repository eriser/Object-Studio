#include "qcanvas.h"
#include <QDebug>

QCanvasLayer::QCanvasLayer()
{
    BackPainter=NULL;
    SetSize(QSize(1,1));
}

QCanvasLayer::~QCanvasLayer()
{
    if (BackPainter != NULL)
    {
        delete BackPainter;
        //delete BackImage;
    }
}

void QCanvasLayer::Clear(const QBrush &Brush)
{
    SetBrush(Brush);
    if (Brush.color().alpha()<255) BackImage.fill(qRgba(0,0,0,0));
    BackPainter->fillRect(BackImage.rect(),Brush);
}

void QCanvasLayer::ClearGradient()
{
    QLinearGradient lg(QPoint(0,0),QPoint(BackImage.width(),BackImage.height()));
    lg.setColorAt(0,"#ddd");
    lg.setColorAt(1,"#999");
    Clear(lg);
}

void QCanvasLayer::ClearTransparent()
{
    Clear(Qt::transparent);
}

void QCanvasLayer::MoveTo(QPoint Pos)
{
    CurrentPos=Pos;
}

void QCanvasLayer::MoveTo(int x, int y)
{
    MoveTo(QPoint(x,y));
}

void QCanvasLayer::LineTo(QPoint Pos)
{
    BackPainter->drawLine(CurrentPos,Pos);
    CurrentPos=Pos;
}

void QCanvasLayer::LineTo(int x, int y)
{
    LineTo(QPoint(x,y));
}

void QCanvasLayer::Circle(QPoint Pos, int Radius)
{
    BackPainter->drawEllipse(Pos,Radius,Radius);
    CurrentPos=Pos;
}

void QCanvasLayer::Circle(int x, int y, int Radius)
{
    Circle(QPoint(x,y),Radius);
}

void QCanvasLayer::Line(QPoint Start, QPoint End)
{
    BackPainter->drawLine(Start,End);
    CurrentPos=End;
}

void QCanvasLayer::Line(int x1, int y1, int x2, int y2)
{
    Line(QPoint(x1,y1),QPoint(x2,y2));
}

void QCanvasLayer::Rectangle(int X, int Y, int Width, int Height)
{
    Rectangle(QRect(X,Y,Width,Height));
}

void QCanvasLayer::Rectangle(const QRect &Rect)
{
    BackPainter->drawRect(Rect);
}

void QCanvasLayer::Path(const QPainterPath &P)
{
    BackPainter->drawPath(P);
}

void QCanvasLayer::EraseTransparent(int X, int Y, int Width, int Height)
{
    EraseTransparent(QRect(X,Y,Width,Height));
}

void QCanvasLayer::EraseTransparent(const QRect &Rect)
{
    BackPainter->setCompositionMode(QPainter::CompositionMode_Clear);
    BackPainter->fillRect(Rect,Qt::transparent);
    BackPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
}

void QCanvasLayer::Text(int X, int Y, const QString &s)
{
    QFontMetrics fm(BackPainter->font());
    BackPainter->drawText(X+1,Y+fm.ascent()+1,s);
}

void QCanvasLayer::Text(QPoint Pos, const QString &s)
{
    Text(Pos.x(),Pos.y(),s);
}

void QCanvasLayer::grabWidget(QWidget *w, float smoke)
{
    grabWidget(w,w->rect(),smoke);
}

void QCanvasLayer::grabWidget(QWidget *w, QRect sourceRect, float smoke)
{
    SetSize(sourceRect.size());
    w->render(BackPainter,QPoint(0,0),QRegion(sourceRect));
    if (smoke>0)
    {
        BackPainter->setBrush(QColor(0,0,0,255.0*smoke));
        BackPainter->setPen(Qt::NoPen);
        BackPainter->drawRect(QRect(QPoint(0,0),sourceRect.size()));
    }
}

void QCanvasLayer::SetPen(const QPen &Pen)
{
    BackPainter->setPen(Pen);
}

void QCanvasLayer::SetPen(const QColor &Col)
{
    BackPainter->setPen(QPen(Col));
}

void QCanvasLayer::SetBrush(const QBrush &Brush)
{
    BackPainter->setBrush(Brush);
}

void QCanvasLayer::SetBrush(const QColor &Col)
{
    BackPainter->setBrush(QBrush(Col));
}

void QCanvasLayer::SetPenBrush(const QColor &Col)
{
    SetPenBrush(Col,Col);
}

void QCanvasLayer::SetPenBrush(const QColor &PenCol, const QColor &BrushCol)
{
    BackPainter->setPen(QPen(PenCol));
    BackPainter->setBrush(QBrush(BrushCol));
}

void QCanvasLayer::SetFont(const QFont &Font)
{
    BackPainter->setFont(Font);
}

void QCanvasLayer::SetSize(QSize Size)
{
    if (Size.width())
    {
        if (Size.height())
        {
            if (BackPainter != NULL)
            {
                delete BackPainter;
                //delete BackImage;
            }
            BackImage=QImage(Size,QImage::Format_ARGB32_Premultiplied);
            BackPainter=new QPainter(&BackImage);
            //BackPainter->setWorldMatrixEnabled(false);
            //BackPainter->setViewTransformEnabled(false);
            BackPainter->setBackgroundMode(Qt::TransparentMode);
            BackPainter->setRenderHint(QPainter::Antialiasing);
            //BackPainter->setRenderHint(QPainter::TextAntialiasing);
            //BackPainter->setRenderHint(QPainter::HighQualityAntialiasing);
            //BackPainter->setRenderHint(QPainter::SmoothPixmapTransform);
        }
    }
}

void QCanvasLayer::Render(const QRect &r, QPainter& p)
{
    p.drawImage(r,BackImage,r);
}

QCanvas::QCanvas(QWidget *parent, int Layers) :
    QFrame(parent), QCanvasLayer()
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_StaticContents);
    setAttribute(Qt::WA_NoSystemBackground);
//#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    //setAttribute(Qt::WA_PaintOnScreen);
//#endif
    SetLayers(Layers);
}

QCanvas::~QCanvas()
{
    qDeleteAll(CanvasLayers);
}

void QCanvas::SetLayers(int Layers)
{
    for (int i=0;i<Layers;i++)
    {
        CanvasLayers.append(new QCanvasLayer);
    }
}

void QCanvas::Clear(const QBrush &Brush)
{
    if (size() != BackImage.size())
    {
        SetSize(size());
        foreach(QCanvasLayer* L,CanvasLayers) L->SetSize(size());
    }
    QCanvasLayer::Clear(Brush);
}

void QCanvas::resizeEvent(QResizeEvent* event)
{
    if (event->size() != BackImage.size())
    {
        SetSize(size());
        foreach(QCanvasLayer* L,CanvasLayers) L->SetSize(event->size());
    }
}

void QCanvas::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    //p.setRenderHint(QPainter::SmoothPixmapTransform);
    //p.setWorldMatrixEnabled(false);
    //p.setViewTransformEnabled(false);
    QRect r=event->rect();
    p.setClipRect(r);
    Render(r,p);
    //p.drawImage(r,BackImage,r);
    foreach(QCanvasLayer* L,CanvasLayers)
    {
        L->Render(r,p);
        //p.drawImage(r,L->BackImage,r);
    }
}
