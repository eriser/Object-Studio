#ifndef QCANVAS_H
#define QCANVAS_H

#include <QFrame>
#include <QImage>
#include <QPainter>
#include <QResizeEvent>

class QCanvasLayer
{
public:
    QCanvasLayer();
    ~QCanvasLayer();
    void Clear(const QBrush &Brush=QBrush(Qt::lightGray));
    void ClearGradient();
    void ClearTransparent();
    void MoveTo(QPoint Pos);
    void MoveTo(int x, int y);
    void LineTo(QPoint Pos);
    void LineTo(int x, int y);
    void Circle(QPoint Pos, int Radius);
    void Circle(int x, int y, int Radius);
    void Line(QPoint Start, QPoint End);
    void Line(int x1, int y1, int x2, int y2);
    void Rectangle(const QRect &Rect);
    void Rectangle(int X, int Y, int Width, int Height);
    void Path(const QPainterPath &P);
    void EraseTransparent(const QRect &Rect);
    void EraseTransparent(int X, int Y, int Width, int Height);
    void Text(int X, int Y, const QString &s);
    void Text(QPoint Pos, const QString &s);
    void SetPen(const QPen &Pen);
    void SetPen(const QColor &Col);
    void SetBrush(const QBrush &Brush);
    void SetBrush(const QColor &Col);
    void SetPenBrush(const QColor &Col);
    void SetPenBrush(const QColor &PenCol, const QColor &BrushCol);
    void SetFont(const QFont &Font);
    void grabWidget(QWidget* w,float smoke=0);
    void grabWidget(QWidget* w,QRect sourceRect,float smoke=0);
    QPoint CurrentPos;
    QImage BackImage;
    QPainter* BackPainter;
    void SetSize(QSize Size);
    void Render(const QRect& r, QPainter& p);
};

class QCanvas : public QFrame, public QCanvasLayer
{
    Q_OBJECT
public:
    explicit QCanvas(QWidget *parent = 0, int Layers=0);
    ~QCanvas();
    void Clear(const QBrush &Brush=QBrush(Qt::lightGray));
    void SetLayers(int Layers);
    QList<QCanvasLayer*> CanvasLayers;
protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
private:
signals:
public slots:
};

#endif // QCANVAS_H
