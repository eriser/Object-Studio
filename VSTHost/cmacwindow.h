#ifndef CMACWINDOW_H
#define CMACWINDOW_H

#include <QtCore>
#include <QWidget>
#include <QObject>
#include <QPaintEvent>
#include <QHideEvent>
#include <QShowEvent>

class CMacWindow : public QWidget
{
    Q_OBJECT
public:
    CMacWindow(QWidget* parent=0);
    ~CMacWindow();
    void Init();
    void* WindowReference();
    void Move();
    void Size();
    void Activate();
    void Hide();
    void Show();
    void Grab();
    void ToFront();
protected:
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
private:
    void* CreateMacWindow();
    void DestroyMacWindow();
    //void* winGroup;
    void * cocoaWin;
    QPixmap pixmap;
};

#endif // CMACWINDOW_H
