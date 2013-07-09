#ifndef CJACKSCOMPONENT_H
#define CJACKSCOMPONENT_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QMenu>
#include "cdevicelist.h"

namespace Ui {
    class CJacksComponent;
}

class CJacksComponent : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CJacksComponent(QWidget *parent = 0);
    ~CJacksComponent();
    void Init(CDeviceList* DeviceList);
public slots:
    void ShowConnections(IDevice* Device);
signals:
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
private:
    Ui::CJacksComponent *ui;
    QGraphicsScene Scene;
    IDevice* m_D;
    QList<int> InJacks;
    QList<int> OutJacks;
    QList<QRect> JackRects;
    void DrawConnections();
    int MouseOverJack(const QPoint& Pos);
    CDeviceList* m_DL;
    QMenu* JackPopup;
    QString MenuJackID;
    QSignalMapper* mapper;
private slots:
    void ToggleConnection(QString JackID);
};

#endif // CJACKSCOMPONENT_H
