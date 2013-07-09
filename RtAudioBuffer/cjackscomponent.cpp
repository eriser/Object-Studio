#include "cjackscomponent.h"
#include "ui_cjackscomponent.h"
#include <QGraphicsSimpleTextItem>
#include <QMenu>

CJacksComponent::CJacksComponent(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::CJacksComponent)
{
    ui->setupUi(this);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::HighQualityAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    setScene(&Scene);
    setFrameStyle(0);
    setLineWidth(0);
    m_D=NULL;
    JackPopup=new QMenu(this);
    mapper=new QSignalMapper(this);
    connect(mapper,SIGNAL(mapped(QString)),this,SLOT(ToggleConnection(QString)));
}

CJacksComponent::~CJacksComponent()
{
    delete JackPopup;
    delete ui;
}

void CJacksComponent::Init(CDeviceList *DeviceList)
{
    m_DL=DeviceList;
}

void CJacksComponent::ShowConnections(IDevice *Device)
{
    Scene.clear();
    InJacks.clear();
    OutJacks.clear();
    m_D=Device;
    if (Device==NULL) return;
    for (int i=0;i<Device->JackCount();i++)
    {
        if (Device->GetJack(i)->Direction==IJack::In)
        {
            InJacks.append(i);
        }
        else
        {
            OutJacks.append(i);
        }
    }
    DrawConnections();
}

void CJacksComponent::DrawConnections()
{
    Scene.clear();
    JackRects.clear();
    if (m_D==NULL) return;
    for (int i=0;i<m_D->JackCount();i++)
    {
        QRect r;
        QString txt=m_D->GetJack(i)->Name;
        QFont f(QString(),11);
        if (InJacks.contains(i))
        {
            r=QRect(50,InJacks.indexOf(i)*15,12,12);
            JackRects.append(r);
            QFontMetrics fm(f);
            QGraphicsSimpleTextItem* item = Scene.addSimpleText(txt,f);
            item->setPos(QPoint(48-fm.width(txt),InJacks.indexOf(i)*15));
            item->setPen(Qt::NoPen);
        }
        else
        {
            r=QRect(70,OutJacks.indexOf(i)*15,12,12);
            JackRects.append(r);
            QGraphicsSimpleTextItem* item = Scene.addSimpleText(txt,f);
            item->setPos(QPoint(84,OutJacks.indexOf(i)*15));
            item->setPen(Qt::NoPen);
        }
        QPen p(m_D->GetJack(i)->JackColor());
        /*
        switch (m_D->GetJack(i)->AttachMode)
        {
        case IJack::Amplitude:
            //Green
            //ImageList2->Draw(PaintBox1->Canvas,JR->Position.left-4,JR->Position.top-4,2,true);
            p.setColor(Qt::green);
            break;
        case IJack::Frequency:
            //Yellow
            //ImageList2->Draw(PaintBox1->Canvas,JR->Position.left-4,JR->Position.top-4,0,true);
            p.setColor(Qt::yellow);
            break;
        case IJack::Pitch:
        case (IJack::Amplitude | IJack::Pitch):
            //Blue
            //ImageList2->Draw(PaintBox1->Canvas,JR->Position.left-4,JR->Position.top-4,1,true);
            p.setColor(Qt::blue);
            break;
        case IJack::Trigger:
            //Black
            //ImageList2->Draw(PaintBox1->Canvas,JR->Position.left-4,JR->Position.top-4,5,true);
            p.setColor(Qt::black);
            break;
        case IJack::Wave:
            //Red
            //ImageList2->Draw(PaintBox1->Canvas,JR->Position.left-4,JR->Position.top-4,3,true);
            p.setColor(Qt::red);
            break;
        case IJack::MIDI:
            //White
            //ImageList2->Draw(PaintBox1->Canvas,JR->Position.left-4,JR->Position.top-4,4,true);
            p.setColor(Qt::white);
            break;
        }
        */
        p.setWidth(2);
        Scene.addEllipse(r,p,QBrush(QColor(0,0,0,80)));
    }
}

void CJacksComponent::paintEvent(QPaintEvent *event)
{
    DrawConnections();
    QGraphicsView::paintEvent(event);
}

int CJacksComponent::MouseOverJack(const QPoint &Pos)
{
    for (int i=0;i<JackRects.count();i++)
    {
        if (JackRects[i].contains(Pos))
        {
            return i;
        }
    }
    return -1;
}

void CJacksComponent::ToggleConnection(QString JackID)
{
    if (m_DL->IsConnected(JackID,MenuJackID))
    {
        m_DL->Disconnect(JackID,MenuJackID);
    }
    else
    {
        m_DL->Connect(JackID,MenuJackID);
    }
}

void CJacksComponent::mousePressEvent(QMouseEvent *event)
{
    QPoint Pos=mapToScene(event->pos().x(),event->pos().y()).toPoint();
    int JackIndex=MouseOverJack(Pos);
    MenuJackID.clear();
    if (JackIndex>-1)
    {
        IJack* Jack=m_D->GetJack(JackIndex);
        MenuJackID=m_DL->JackID(Jack);
        JackPopup->clear();
        for (int i=0;i<m_DL->JackCount();i++)
        {
            IJack* J=m_DL->Jacks(i);
            if (m_DL->CanConnect(Jack,J))
            {
                QAction* a=JackPopup->addAction(m_DL->JackID(J));
                a->setCheckable(true);
                a->setChecked(m_DL->IsConnected(Jack,J));
                connect(a,SIGNAL(triggered()),mapper,SLOT(map()));
                mapper->setMapping(a,m_DL->JackID(J));
            }
        }
        if (JackPopup->actions().count()==0)
        {
            QAction* a=JackPopup->addAction("(No Available Connections)");
            a->setEnabled(false);
        }
        JackPopup->popup(mapToGlobal(event->pos()));
    }
}
