#include "cdesktopcomponent.h"
#include "ui_cdesktopcomponent.h"
#include <QGraphicsSimpleTextItem>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>

QList<QGraphicsItem*> CJackContainer::Paint(QGraphicsScene* Scene)
{
    QList<QGraphicsItem*> items;
    for (int i=0;i<JackRects.count();i++)
    {
        QPen p(JackRects.at(i).Jack->JackColor());
        JackRects[i].setSize(QSize(8,8));
        p.setWidth(2);
        items.append(Scene->addEllipse(JackRects.at(i).translated(Geometry.topLeft()),p,QBrush(QColor(0,0,0,80))));
    }
    return items;
}

const bool CJackContainer::InsideMe(const QPoint& Pos)
{
    return Geometry.contains(Pos);
}

QPoint CJackContainer::JackPos(const int Index)
{
    if (Index>=JackRects.count())
    {
        return QPoint();
    }
    return JackRects[Index].topLeft()+QPoint(4,4);
}

const int CJackContainer::InsideJack(const QPoint& Pos)
{
    for (int i=0;i<JackRects.count();i++)
    {
        if (JackRects.at(i).translated(Geometry.topLeft()).contains(Pos)) return i;
    }
    return -1;
}

const int CJackContainer::JackCount()
{
    return JackRects.count();
}

IJack* CJackContainer::GetJack(const int Index)
{
    return JackRects[Index].Jack;
}

//---------------------------------------------------------------------------------------

CDeviceComponent::CDeviceComponent()
{
    m_Device=NULL;
    m_Active=false;
}

void CDeviceComponent::Init(IDevice* Device,const QString& ClassName)
{
    m_ClassName=ClassName;
    m_Device=Device;
    for (int i=0;i<Device->JackCount();i++)
    {
        JackRect JR;
        JR.Index=i;
        JR.Jack=Device->GetJack(i);
        JackRects.append(JR);
    }
}

CDeviceComponent::~CDeviceComponent()
{
}

IDevice* CDeviceComponent::Device()
{
    return m_Device;
}

QList<QGraphicsItem*> CDeviceComponent::Paint(QGraphicsScene* Scene)
{
    QList<QGraphicsItem*> items;
    int InCount=1;
    int OutCount=1;
    int InIndex=0;
    int OutIndex=0;
    float InFactor=1;
    float OutFactor=1;
    if (m_Device != NULL)
    {
        if (Geometry.left()<1) Geometry.setLeft(1);
        if (Geometry.top()<1) Geometry.setTop(1);
        Geometry.setSize(QSize(120,60));
        QPainterPath path(QPoint(0,0));
        path.addRoundedRect(Geometry.translated(5,5),5,5);
        Scene->addPath(path,Qt::NoPen,QBrush(QColor(0,0,0,40)));

        QPen p(Qt::NoPen);
        p.setWidth(1);
        QLinearGradient lg(0,Geometry.top(),0,Geometry.height()+Geometry.top());
        if (m_Active)
        {
            p=QPen(Qt::black);
            lg.setColorAt(0,"#eee");
            lg.setColorAt(0.49999,"#bbb");
            lg.setColorAt(0.5,"#afafaf");
            lg.setColorAt(1,"#999");
        }
        else
        {
            p=QPen(Qt::gray);
            lg.setColorAt(0,"#ddd");
            lg.setColorAt(0.49999,"#aaa");
            lg.setColorAt(0.5,"#8f8f8f");
            lg.setColorAt(1,"#777");
        }
        QBrush b(lg);
        path=QPainterPath(QPoint(0,0));
        path.addRoundedRect(Geometry,5,5);
        items.append(Scene->addPath(path,p,b));
        QString Caption=m_Device->Name() + " " + QString::number(m_Device->Index());
        QString FileName;
        if (!m_Device->FileName().isEmpty()) FileName="("+QFileInfo(m_Device->FileName()).fileName()+")";
        QFont f;
        QFontMetrics fm(f);
        if (fm.width(Caption)>Geometry.width()) Caption=Caption.left(7)+"..."+Caption.right(7);
        QGraphicsSimpleTextItem* item = Scene->addSimpleText(Caption,f);
        QPoint TextOffset((Geometry.width()-fm.width(Caption))/2,(Geometry.height()-fm.height())/2);
        if (!FileName.isEmpty()) TextOffset.setY(TextOffset.y()-(fm.height()/2));
        item->setPos(Geometry.topLeft()+TextOffset);
        item->setBrush(QBrush("#ddd"));
        item->setPen(Qt::NoPen);
        items.append(item);
        item = Scene->addSimpleText(Caption,f);
        item->setPos(Geometry.topLeft()+TextOffset+QPoint(-1,-1));
        item->setBrush(QBrush("#222"));
        item->setPen(Qt::NoPen);
        items.append(item);
        if (!FileName.isEmpty())
        {
            if (fm.width(FileName)>Geometry.width()) FileName=FileName.left(7)+"..."+FileName.right(7);
            item = Scene->addSimpleText(FileName,f);
            TextOffset.setX((Geometry.width()-fm.width(FileName))/2);
            TextOffset.setY(TextOffset.y()+fm.height());
            item->setPos(Geometry.topLeft()+TextOffset);
            item->setBrush(QBrush("#ddd"));
            item->setPen(Qt::NoPen);
            items.append(item);
            item = Scene->addSimpleText(FileName,f);
            item->setPos(Geometry.topLeft()+TextOffset+QPoint(-1,-1));
            item->setBrush(QBrush("#222"));
            item->setPen(Qt::NoPen);
            items.append(item);
        }

        for (int i=0;i<JackRects.count();i++)
        {
            if (JackRects.at(i).Jack->Direction==IJack::In)
            {
                InCount++;
            }
            else
            {
                OutCount++;
            }
        }
        InFactor=Geometry.width()/InCount;
        OutFactor=Geometry.width()/OutCount;

        for (int i=0;i<JackRects.count();i++)
        {
            JackRect* JR=&JackRects[i];
            if (JR->Jack->Direction==IJack::In)
            {
                JR->setTopLeft(QPoint((InIndex*InFactor)+4,1));
                InIndex++;
            }
            else
            {
                JR->setTopLeft(QPoint((OutIndex*OutFactor)+4,Geometry.height()-9));
                OutIndex++;
            }
        }
        items.append(CJackContainer::Paint(Scene));
    }
    return items;
}

QString CDeviceComponent::ClassName()
{
    return m_ClassName;
}

void CDeviceComponent::Select(const bool Active)
{
    m_Active=Active;
}

//---------------------------------------------------------------------------

CJackBar::CJackBar()
{
}

CJackBar::~CJackBar()
{
}

IJack* CJackBar::AddJack(IJack* J)
{
    JackRect JR;
    JR.Index=JackRects.count();
    JR.Jack=J;
    JackRects.append(JR);
    return J;
}

QList<QGraphicsItem*> CJackBar::Paint(QGraphicsScene* Scene)
{
    QList<QGraphicsItem*> items;
    if (Geometry.topLeft()==QPoint(0,0)) Scene->addRect(Geometry.translated(0,5),Qt::NoPen,QColor(0,0,0,40));

    QLinearGradient lg(0,Geometry.top(),0,Geometry.height()+Geometry.top());
    lg.setColorAt(0,"#ddd");
    lg.setColorAt(0.49999,"#bbb");
    lg.setColorAt(0.5,"#9f9f9f");
    lg.setColorAt(1,"#787878");
    QBrush b(lg);

    items.append(Scene->addRect(Geometry,Qt::NoPen,b));

    for (int i=0;i<JackRects.count();i++)
    {
        JackRect* JR=&JackRects[i];
        if (JR->Jack->Direction==IJack::In)
        {
            JR->setTopLeft(QPoint((i*20)+12,2));
        }
        else
        {
            JR->setTopLeft(QPoint((i*20)+12,2));
        }
    }
    items.append(CJackContainer::Paint(Scene));
    return items;
}

//---------------------------------------------------------------------------

CDesktopComponent::CDesktopComponent(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::CDesktopComponent)
{
    ui->setupUi(this);
    setStyleSheet("QGraphicsView{background:qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ddd, stop:1 #999);}");
    setDragMode(NoDrag);
    Rubberband=new QiPhotoRubberband(this);
    Rubberband->hide();
    Marked=false;
    m_MD=false;
    Dragging=false;
    MouseDown=false;
    m_DeviceIndex=-1;
    setScene(&Scene);
    setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    Scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::HighQualityAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setFrameStyle(0);
    setLineWidth(0);

    RecentMenu=new QMenu("Recent",this);
    RecentPopup=new QMenu("Recent",this);
    RecentMapper=new QSignalMapper(this);
    RecentPopupMapper=new QSignalMapper(this);
    connect(RecentMapper,SIGNAL(mapped(QString)),this,SLOT(OpenFile(QString)));
    connect(RecentPopupMapper,SIGNAL(mapped(QString)),this,SLOT(OpenFile(QString)));
    CreateRecentMenu(RecentMenu,RecentMapper);
    CreateRecentMenu(RecentPopup,RecentPopupMapper);

    JackPopup=new QMenu(this);
    mapper=new QSignalMapper(this);
    connect(mapper,SIGNAL(mapped(QString)),this,SLOT(ToggleConnection(QString)));
    PluginsPopup=new QMenu("Add",this);
    pluginmapper=new QSignalMapper(this);
    connect(pluginmapper,SIGNAL(mapped(QString)),this,SLOT(PluginMenuClicked(QString)));

    actionPaste=new QAction("Paste",this);
    connect(actionPaste,SIGNAL(triggered()),this,SLOT(Paste()));
    actionPaste->setShortcut(QKeySequence::Paste);
    actionPasteParameters=new QAction("Paste Parameters",this);
    connect(actionPasteParameters,SIGNAL(triggered()),this,SLOT(PasteParameters()));

    ParameterPresetsMenu=new QMenu("Load",this);
    ParameterPresetsMapper=new QSignalMapper(this);
    connect(ParameterPresetsMapper,SIGNAL(mapped(QString)),this,SLOT(OpenPreset(QString)));
    ParametersMenu=new QMenu("Parameters",this);
    ParametersMenu->addMenu(ParameterPresetsMenu);
    ParametersMenu->addAction("Save as Preset",this,SLOT(SavePresetAs()));
    ParametersMenu->addAction("Copy Parameters",this,SLOT(CopyParameters()));
    ParametersMenu->addAction(actionPasteParameters);
    MainWindow=NULL;

    DesktopMenu=new QMenu(this);
    DesktopMenu->addMenu(PluginsPopup);
    DesktopMenu->addSeparator();
    DesktopMenu->addAction("New",this,SLOT(New()),QKeySequence(QKeySequence::New));
    DesktopMenu->addAction("Open...",this,SLOT(Open()),QKeySequence(QKeySequence::Open));
    DesktopMenu->addAction("Save",this,SLOT(Save()),QKeySequence(QKeySequence::Save));
    DesktopMenu->addAction("Save As...",this,SLOT(SaveAs()),QKeySequence(QKeySequence::SaveAs));
    DesktopMenu->addMenu(RecentPopup);
    DesktopMenu->addSeparator();
    DesktopMenu->addAction(actionPaste);

    QAction* aCopy=new QAction("Copy",this);
    aCopy->setShortcut(QKeySequence::Copy);
    connect(aCopy,SIGNAL(triggered()),this,SLOT(Copy()));
    QAction* aCut=new QAction("Cut",this);
    aCopy->setShortcut(QKeySequence::Cut);
    connect(aCut,SIGNAL(triggered()),this,SLOT(Cut()));
    QAction* aDisconnect=new QAction("Disconnect",this);
    connect(aDisconnect,SIGNAL(triggered()),this,SLOT(RemoveConnections()));

    DeviceMenu=new QMenu(this);
    DeviceMenu->addAction(aCut);
    DeviceMenu->addAction(aCopy);
    DeviceMenu->addSeparator();
    DeviceMenu->addMenu(ParametersMenu);
    DeviceMenu->addSeparator();
    DeviceMenu->addAction(aDisconnect);
    DeviceMenu->addAction("Show UI",this,SLOT(Execute()));

    MarkMenu=new QMenu(this);
    MarkMenu->addAction(aCut);
    MarkMenu->addAction(aCopy);
    MarkMenu->addAction(actionPasteParameters);
    MarkMenu->addSeparator();
    MarkMenu->addAction(aDisconnect);
}

CDesktopComponent::~CDesktopComponent()
{
    delete ui;
}

void CDesktopComponent::SetFileMenu(QMenu* Menu)
{
    Menu->addMenu(RecentMenu);
}

IJack* CDesktopComponent::CreateInsideJack(int ProcIndex, IJack *ConnectTo, IDeviceBase *DeviceClass)
{
    if (ConnectTo->Direction==IJack::In) return new COutJack(ConnectTo->Name,"This",ConnectTo->AttachMode,IJack::Out,DeviceClass,ProcIndex);
    return new CInJack(ConnectTo->Name,"This",ConnectTo->AttachMode,IJack::In,DeviceClass);

}

IJack* CDesktopComponent::AddJack(IJack* Jack, int PolyIndex)
{
    if (!PolyIndex)
    {
        DeviceList.AddJack(Jack);
        if (Jack->Direction==IJack::In)
        {
            JackBar2.AddJack(Jack);
        }
        else
        {
            JackBar1.AddJack(Jack);
        }
    }
    else
    {
        PolyDevices[PolyIndex-1].AddJack(Jack);
    }
    return Jack;
}

void CDesktopComponent::SetPoly(const int Count)
{
    int Before=PolyDevices.count();
    if (Count>Before)
    {
        for (int i=Before;i<Count;i++)
        {
            CDeviceList DL;
            PolyDevices.append(DL);
        }
    }
    else
    {
        for (int i=Before;i>Count;i--)
        {
            PolyDevices.removeAt(i);
        }
    }
}

void CDesktopComponent::UpdatePoly()
{
    Load(Save("Custom"));
}

void CDesktopComponent::ChangeParameter(IDevice* Device, int ParameterIndex, int Value)
{
    int Index=DeviceList.IndexOfDevice(Device);
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].Device(Index)->SetParameterValue(ParameterIndex,Value);
    }
}

void CDesktopComponent::ParameterChange()
{
    if ((m_DeviceIndex>-1) & (Devices.count() > 0))
    {
        IDevice* D=Devices[m_DeviceIndex].Device();
        ShowParameters(D);
        for (int i=0;i<PolyDevices.count();i++)
        {
            for (int p=0;p<D->ParameterCount();p++)
            {
                PolyDevices[i].Device(m_DeviceIndex)->SetParameterValue(p,D->GetParameterValue(p));
            }
        }
        DrawConnections();
    }
}

void CDesktopComponent::PluginMenuClicked(QString ClassName)
{
    AddDevice(ClassName,this);
    Devices.last().Geometry.setTopLeft(StartPoint);
    DrawConnections();
}

const bool CDesktopComponent::AddDevice(const QString &ClassName, void *MainWindow)
{
    return AddDevice(ClassName,DeviceList.FindFreeID(ClassName),MainWindow);
}

const bool CDesktopComponent::AddDevice(const QString& ClassName, const int ID, void* MainWindow)
{
    int MenuIndex=CAddIns::AddInIndex(ClassName);
    if (MenuIndex<0) return false;
    IDevice* D=DeviceList.AddDevice(CAddIns::AddInList[MenuIndex].InstanceFunction,ID,MainWindow);
    AddDevice(D,ClassName);
    D->SetHost(this);
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].AddDevice(CAddIns::AddInList[MenuIndex].InstanceFunction,ID,MainWindow);
    }
    return true;
}

void CDesktopComponent::AddDevice(IDevice *Device, const QString& ClassName)
{
    CDeviceComponent DC;
    DC.Name=Device->Name() + QString::number(Device->Index());
    DC.Init(Device,ClassName);
    DC.Geometry.setTopLeft(QPoint(100,100));
    Devices.append(DC);
    SelectDevice(Devices.count()-1);
}

void CDesktopComponent::RemoveDevice(IDevice* Device)
{
    int Index=DeviceList.IndexOfDevice(Device);
    DeviceList.RemoveDevice(Index);
    for (int i=0;i<PolyDevices.count();i++)
    {
        IDevice* D1=PolyDevices[i].Device(Index);
        PolyDevices[i].RemoveDevice(Index);
        delete D1;
    }
    Devices.removeAt(Index);
    delete Device;
}

void CDesktopComponent::DisconnectDevice(IDevice* Device)
{
    int Index=DeviceList.IndexOfDevice(Device);
    DeviceList.DisconnectDevice(Index);
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].DisconnectDevice(Index);
    }
}

void CDesktopComponent::Clear()
{
    emit StopPlaying();
    m_DeviceIndex=-1;
    ShowParameters(NULL);
    DisconnectJackBar(JackBar1);
    DisconnectJackBar(JackBar2);
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].Clear();
    }
    DeviceList.Clear();
    Devices.clear();
}

void CDesktopComponent::DisconnectJackBar(CJackBar& JackBar)
{
    for (int i=0;i<DeviceList.JackCount();i++)
    {
        IJack* J=DeviceList.Jacks(i);
        if (J->Direction==IJack::In)
        {
            for (int i1=0;i1<JackBar.JackCount();i1++)
            {
                if (JackBar.GetJack(i1)->Direction==IJack::Out)
                {
                    if (JackBar.GetJack(i1)->Owner=="This")
                    {
                        CInJack* IJ=(CInJack*)J;
                        IJ->DisconnectFromOut((COutJack*)JackBar.GetJack(i1));
                    }
                }
            }
        }
    }
    for (int i=0;i<JackBar.JackCount();i++)
    {
        if (JackBar.GetJack(i)->Direction==IJack::In)
        {
            CInJack* IJ=(CInJack*)JackBar.GetJack(i);
            for (int i1=0;i1<IJ->OutJackCount();i1++)
            {
                IJ->DisconnectFromOut(IJ->OutJack(i1));
            }
        }
    }

}

Qt::CursorShape CDesktopComponent::CanConnect(IJack* J1,IJack* J2)
{
    if (J1 != J2)
    {
        if ((DeviceList.CanConnect(J1,J2)) && (!DeviceList.IsConnected(J1,J2)))
        {
            setToolTip(DeviceList.JackID(J2));
            return Qt::PointingHandCursor;
        }
        setToolTip(DeviceList.JackID(J2));
        return Qt::ForbiddenCursor;
    }
    setToolTip(QString());
    return Qt::OpenHandCursor;
}

void CDesktopComponent::Connect(const QString& J1,const QString& J2)
{
    if (DeviceList.IsConnected(J1,J2)) return;
    DeviceList.Connect(J1,J2);
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].Connect(J1,J2);
    }
}

void CDesktopComponent::Disconnect(const QString& J1,const QString& J2)
{
    DeviceList.Disconnect(J1,J2);
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].Disconnect(J1,J2);
    }
}

void CDesktopComponent::Connect(IJack* J1,IJack* J2)
{
    Connect(DeviceList.JackID(J1),DeviceList.JackID(J2));
}

void CDesktopComponent::SetConnectCursor(const QPoint& Pos)
{
    IJack* HoverJack=MouseOverJack(Pos);
    if (HoverJack)
    {
        setCursor(CanConnect(DragJack,HoverJack));
        return;
    }
    setCursor(Qt::OpenHandCursor);
    setToolTip(QString());
}

void CDesktopComponent::ConnectDrop(const QPoint& Pos)
{
    if (Dragging)
    {
        setToolTip(QString());
        Dragging=false;
        setCursor(Qt::ArrowCursor);
        IJack* HoverJack=MouseOverJack(Pos);
        if (HoverJack)
        {
            if (CanConnect(DragJack,HoverJack)==Qt::PointingHandCursor)
            {
                Connect(DragJack,HoverJack);
            }
        }
        DrawConnections();
    }
}

void CDesktopComponent::resizeEvent(QResizeEvent* /*event*/)
{
    DrawConnections();
}

void CDesktopComponent::moveEvent(QMoveEvent* /*event*/)
{
    DrawConnections();
}

void CDesktopComponent::showEvent(QShowEvent* /*event*/)
{
    DrawConnections();
}

void CDesktopComponent::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx,dy);
    DrawConnections();
}

void CDesktopComponent::DrawConnections()
{
    Scene.clear();
    QList<QGraphicsItem*> items;
    QRect MaxRect;
    for (int i=0;i<Devices.count();i++) MaxRect=MaxRect.united(Devices[i].Geometry);
    MaxRect=MaxRect.united(rect());
    JackBar1.Geometry=QRect(0,0,MaxRect.width(),12);
    items.append(JackBar1.Paint(&Scene));
    JackBar2.Geometry=QRect(0,MaxRect.height()-12,MaxRect.width(),12);
    items.append(JackBar2.Paint(&Scene));
    QList<CJackContainer*> paintedContainers;
    paintedContainers.append(&JackBar1);
    paintedContainers.append(&JackBar2);

    for (int i=0;i<Devices.count();i++)
    {
        if (i!=m_DeviceIndex)
        {
            items.append(Devices[i].Paint(&Scene));
            items.append(DrawDeviceConnections(Devices[i],paintedContainers));
        }
    }

    if (m_DeviceIndex>-1)
    {
        items.append(Devices[m_DeviceIndex].Paint(&Scene));
        items.append(DrawDeviceConnections(Devices[m_DeviceIndex],paintedContainers));
    }
    foreach(QGraphicsItem* i,items) i->setZValue(1);
    setSceneRect(MaxRect);
}

QList<QGraphicsItem*> CDesktopComponent::DrawDeviceConnections(CDeviceComponent& Device,QList<CJackContainer*>& paintedContainers)
{
    QList<QGraphicsItem*> items;
    for (int j=0;j<Device.JackCount();j++)
    {
        for (int k=0;k<paintedContainers.count();k++)
        {
            for (int l=0;l<paintedContainers[k]->JackCount();l++)
            {
                if (DeviceList.IsConnected(Device.GetJack(j),paintedContainers[k]->GetJack(l)))
                {
                    QPoint Pos1=paintedContainers[k]->JackPos(l)+paintedContainers[k]->Geometry.topLeft();
                    QPoint Pos2=Device.JackPos(j)+Device.Geometry.topLeft();
                    if (!Device.Geometry.contains(Pos1))
                    {
                        if (Device.GetJack(j)->Direction==IJack::In)
                        {
                            items.append(DrawArrow(Pos1,Pos2,Device.GetJack(j)->JackColor()));
                            DrawArrow(Pos1+QPoint(5,5),Pos2+QPoint(5,5),QColor(0,0,0,40));
                        }
                        else
                        {
                            items.append(DrawArrow(Pos2,Pos1,Device.GetJack(j)->JackColor()));
                            DrawArrow(Pos2+QPoint(5,5),Pos1+QPoint(5,5),QColor(0,0,0,40));
                        }
                    }
                }
            }
        }
    }
    paintedContainers.append(&Device);
    return items;
}

QList<QGraphicsItem*> CDesktopComponent::DrawArrow(const QPoint& OutPoint, const QPoint& InPoint, QColor Color)
{
    QList<QGraphicsItem*> items;
    float theta;
    const float pi = DoublePi;
    int L = 6;
    QPointF s(InPoint-OutPoint);
    QPoint Mid(InPoint -((InPoint-OutPoint)/2));
    QPoint Mid14(InPoint-((InPoint-OutPoint)/4));
    QPoint Mid34(InPoint-(((InPoint-OutPoint)*3)/4));

    QPoint Dist(InPoint-OutPoint);
    float Distance=sqrt((Dist.x()*Dist.x())+(Dist.y()*Dist.y()))/10;

    if ((InPoint.x()<OutPoint.x()) && (InPoint.y()>OutPoint.y())) Distance=-Distance;
    if ((InPoint.x()<OutPoint.x()) && (InPoint.y()<OutPoint.y())) Distance=-Distance;

    if (s.x() != 0)
    {
        theta = atan(s.y() / s.x());
        if (s.x() < 0) theta = theta + pi;
    }
    else
    {
        if (s.y() < 0)
        {
            theta = 3 * pi / 2;
        }
        else
        {
            theta = pi / 2;
        }
    }
    //'rotate direction
    float theta1 = theta - 3 * pi / 4;
    //'find end of one side of arrow:
    QPoint p3((L * cos(theta1)) + Mid.x(),(L * sin(theta1)) + Mid.y());
    //'rotate other way for other arrow line
    theta1 = theta1 - pi / 2;
    QPoint p4((L * cos(theta1)) + Mid.x(),(L * sin(theta1)) + Mid.y());

    theta1 = theta - 2 * pi / 4;
    QPoint p14((Distance * cos(theta1)) + Mid14.x(),(Distance * sin(theta1)) + Mid14.y());
    theta1 = theta1 - pi;
    QPoint p34((Distance * cos(theta1)) + Mid34.x(),(Distance * sin(theta1)) + Mid34.y());

    //'draw the lines
    QPainterPath path(QPoint(0,0));
    path.moveTo(OutPoint);
    path.cubicTo(p34,p14,InPoint);
    items.append(Scene.addPath(path,Color,Qt::NoBrush));
    path=QPainterPath(QPoint(0,0));
    path.moveTo(p3);
    path.lineTo(Mid);
    path.lineTo(p4);
    items.append(Scene.addPath(path,Color,Color));
    return items;
}

const bool CDesktopComponent::XMLCompare(QDomLiteElement* xml1,QDomLiteElement* xml2)
{
    if (xml1->attributeCount() != xml2->attributeCount()) return false;
    if (xml1->attributesString() != xml2->attributesString()) return false;
    if (xml1->childCount() != xml2->childCount()) return false;
    for (int i=0;i<xml1->childCount();i++)
    {
        if (!XMLCompare(xml1->childElement(i),xml2->childElement(i))) return false;
    }
    return true;
}

void CDesktopComponent::ShowParameters(IDevice* Device)
{
    foreach (QAction* a, ParameterPresetsMenu->actions())
    {
        ParameterPresetsMapper->removeMappings(a);
        delete a;
    }
    ParameterPresetsMenu->actions().clear();
    ParameterPresetsMenu->clear();
    QString Preset;
    if (Device != NULL)
    {
        QString fn=QFileInfo(qApp->applicationDirPath()+"/"+Devices[m_DeviceIndex].Device()->Name()+".ssd").absoluteFilePath();
        if (QFileInfo(fn).exists())
        {
            QDomLiteDocument TempDoc(fn);
            QDomLiteElement xml1("Preset");
            DeviceList.SaveParameters(&xml1,Devices[m_DeviceIndex].Device());
            foreach (QDomLiteElement* e,TempDoc.documentElement->elementsByTag("Preset"))
            {
                QString s=e->attribute("PresetName");
                QAction* a=ParameterPresetsMenu->addAction(s);
                connect(a,SIGNAL(triggered()),ParameterPresetsMapper,SLOT(map()));
                ParameterPresetsMapper->setMapping(a,s);
                a->setCheckable(true);
                xml1.setAttribute("PresetName",s);
                if (XMLCompare(&xml1,e))
                {
                    a->setChecked(true);
                    Preset=s;
                }
            }
        }
    }
    if (ParameterPresetsMenu->isEmpty())
    {
        QAction* a=ParameterPresetsMenu->addAction("No Presets");
        a->setEnabled(false);
    }
    QString Title;
    if (Device != NULL)
    {
        Title=DeviceList.DeviceID(Device);
        if (!Preset.isEmpty()) Title+=+" ("+Preset+")";
    }
    emit UpdateParameters(Device,Title);
}

const QString CDesktopComponent::Save(const QString& Mode)
{
    QDomLiteElement xml(Mode);
    QDomLiteElement* Items=xml.appendChild("Items");
    for (int i=0;i<Devices.count();i++)
    {
        QDomLiteElement* Device = Items->appendChild("Device");
        Device->setAttribute("Index",Devices[i].Device()->Index());
        Device->setAttribute("Top",Devices[i].Geometry.top());
        Device->setAttribute("Left",Devices[i].Geometry.left());
        Device->setAttribute("Type",Devices[i].ClassName());
        Device->setAttribute("ClassName",Devices[i].ClassName()+".dll");
        DeviceList.SaveParameters(Device,Devices[i].Device());
    }
    for (int i=0;i<DeviceList.JackCount();i++)
    {
        if (DeviceList.Jacks(i)->Direction==IJack::In)
        {
            CInJack* IJ=(CInJack*)DeviceList.Jacks(i);

            for (int i1=0;i1<IJ->OutJackCount();i1++)
            {
                COutJack* OJ=IJ->OutJack(i1);
                QDomLiteElement* Connection = Items->appendChild("Connection");
                Connection->setAttribute("InJack",DeviceList.JackID((IJack*)IJ));
                Connection->setAttribute("OutJack",DeviceList.JackID((IJack*)OJ));
            }
        }
    }
    if (MainWindow != 0)
    {
        QDomLiteElement* Position = Items->appendChild("Position");
        Position->setAttribute("Top",MainWindow->pos().y());
        Position->setAttribute("Left",MainWindow->pos().x());
        Position->setAttribute("Height",MainWindow->height());
        Position->setAttribute("Width",MainWindow->width());
        Position->setAttribute("Visible",MainWindow->isVisible());
    }
    return xml.toString();
}

void CDesktopComponent::Load(const QString& XML)
{
    Clear();
    QDomLiteElement xml;
    xml.fromString(XML);
    qDebug() << XML;
    QDomLiteElement* Items=xml.elementByTag("Items");
    if (Items)
    {
        QDomLiteElementList XMLDevices=Items->elementsByTag("Device");
        foreach (QDomLiteElement* XMLDevice, XMLDevices)
        {
            QString ClassName=XMLDevice->attribute("Type");
            int Index=XMLDevice->attributeValue("Index");
            if (AddDevice(ClassName,Index,this))
            {
                Devices.last().Geometry.setTopLeft(QPoint(XMLDevice->attributeValue("Left"),XMLDevice->attributeValue("Top")));
                LoadParameters(XMLDevice,Devices.last().Device());
                ShowParameters(Devices.last().Device());
            }
        }
        QDomLiteElementList XMLConnections=Items->elementsByTag("Connection");
        foreach (QDomLiteElement* XMLConnection,XMLConnections)
        {
            Connect(XMLConnection->attribute("InJack"),XMLConnection->attribute("OutJack"));
        }
        if (MainWindow != NULL)
        {
            QDomLiteElement* XMLPosition=Items->elementByTag("Position");
            if (XMLPosition)
            {
                MainWindow->move(QPoint(XMLPosition->attributeValue("Left"),XMLPosition->attributeValue("Top")));
                MainWindow->resize(QSize(XMLPosition->attributeValue("Width"),XMLPosition->attributeValue("Height")));
                MainWindow->setVisible((bool)XMLPosition->attributeValue("Visible"));
            }
        }
    }
    SelectDevice(0);
}

void CDesktopComponent::LoadParameters(QDomLiteElement* Device,IDevice* D)
{
    int Index=DeviceList.IndexOfDevice(D);
    DeviceList.LoadParameters(Device,Index,false);
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].LoadParameters(Device,Index,true);
    }
}

void CDesktopComponent::OpenFile(const QString& Path)
{
    emit StopPlaying();
    QDomLiteDocument Doc(Path);
    Load(Doc.documentElement->toString());
    FileName=Path;
    AddRecentFile(Path);
}

void CDesktopComponent::SaveFile(const QString& Path)
{
    QString SavePath=Path;
    if (SavePath.isEmpty()) SavePath=FileName;
    if (SavePath.isEmpty()) return;
    QDomLiteDocument Doc("ObjectStudioProject","Custom");
    Doc.documentElement->fromString(Save("Custom"));
    Doc.save(Path);
    FileName=Path;
    AddRecentFile(Path);
}

void CDesktopComponent::AddRecentFile(const QString &Path)
{
    CPresets::RecentFiles.removeOne(Path);
    CPresets::RecentFiles.prepend(Path);
    while (CPresets::RecentFiles.count()>20) CPresets::RecentFiles.removeLast();
    CreateRecentMenu(RecentMenu,RecentMapper);
    CreateRecentMenu(RecentPopup,RecentPopupMapper);
}

void CDesktopComponent::CreateRecentMenu(QMenu* m, QSignalMapper* sm)
{
    foreach (QAction* a, m->actions())
    {
        sm->removeMappings(a);
        delete a;
    }
    m->actions().clear();
    m->clear();
    foreach(QString s,CPresets::RecentFiles)
    {
        QString DisplayString=s;
        if (s.length()>60) DisplayString="..."+s.right(57);
        QAction* a=new QAction(DisplayString,m);
        connect(a,SIGNAL(triggered()),sm,SLOT(map()));
        sm->setMapping(a,s);
        m->addAction(a);
    }
}

void CDesktopComponent::Play(const bool FromStart)
{
    DeviceList.Play(FromStart);
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].Play(FromStart);
    }
}

void CDesktopComponent::Pause()
{
    DeviceList.Pause();
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].Pause();
    }
}

const bool CDesktopComponent::DeviceInsideRect(CDeviceComponent& D)
{
    return Rubberband->windowGeometry().translated(mapToScene(0,0).toPoint()).contains(D.Geometry);
}

const int CDesktopComponent::DeviceIndex(const QPoint& Pos)
{
    if (m_DeviceIndex>-1)
    {
        if (Devices[m_DeviceIndex].InsideMe(Pos)) return m_DeviceIndex;
    }
    for (int i=Devices.count()-1;i>=0;i--)
    {
        if (Devices[i].InsideMe(Pos))
        {
            return i;
        }
    }
    return -1;
}

void CDesktopComponent::Tick()
{
    DeviceList.Tick();
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].Tick();
    }
}

void CDesktopComponent::HideForms()
{
    DeviceList.HideForms();
    for (int i=0;i<PolyDevices.count();i++)
    {
        PolyDevices[i].HideForms();
    }
}

IJack* CDesktopComponent::MouseOverJack(const QPoint &Pos)
{
    QPoint dummy;
    return MouseOverJack(Pos,dummy);
}

IJack* CDesktopComponent::MouseOverJack(const QPoint& Pos, QPoint& JackPoint)
{
    int JackIndex = JackBar1.InsideJack(Pos);
    if (JackIndex > -1)
    {
        JackPoint=JackBar1.JackPos(JackIndex)+JackBar1.Geometry.topLeft();
        return JackBar1.GetJack(JackIndex);
    }
    JackIndex = JackBar2.InsideJack(Pos);
    if (JackIndex > -1)
    {
        JackPoint=JackBar2.JackPos(JackIndex)+JackBar2.Geometry.topLeft();
        return JackBar2.GetJack(JackIndex);
    }
    if (m_DeviceIndex>-1)
    {
        JackIndex = Devices[m_DeviceIndex].InsideJack(Pos);
        if (JackIndex > -1)
        {
            JackPoint=Devices[m_DeviceIndex].JackPos(JackIndex)+Devices[m_DeviceIndex].Geometry.topLeft();
            return Devices[m_DeviceIndex].Device()->GetJack(JackIndex);
        }
    }
    for (int i=0;i<Devices.count();i++)
    {
        JackIndex = Devices[i].InsideJack(Pos);
        if (JackIndex > -1)
        {
            JackPoint=Devices[i].JackPos(JackIndex)+Devices[i].Geometry.topLeft();
            return Devices[i].Device()->GetJack(JackIndex);
        }
    }
    return NULL;
}

void CDesktopComponent::Activate(IDevice *Device)
{
    int Index=DeviceList.IndexOfDevice(Device);
    SelectDevice(Index);
}

void CDesktopComponent::SelectDevice(const int Index)
{
    m_DeviceIndex=Index;
    for (int i=0;i<Devices.count();i++) Devices[i].Select(false);
    if (Index==-1)
    {
        ShowParameters(NULL);
    }
    else
    {
        if (Devices.isEmpty())
        {
            SelectDevice(-1);
            return;
        }
        Devices[Index].Select(true);
        ShowParameters(Devices[Index].Device());
        Devices[Index].Device()->RaiseForm();
    }
    DrawConnections();
}

void CDesktopComponent::ToggleConnection(QString JackID)
{
    if (DeviceList.IsConnected(JackID,MenuJackID))
    {
        DeviceList.Disconnect(JackID,MenuJackID);
        for (int i=0;i<PolyDevices.count();i++)
        {
            PolyDevices[i].Disconnect(JackID,MenuJackID);
        }
    }
    else
    {
        DeviceList.Connect(JackID,MenuJackID);
        for (int i=0;i<PolyDevices.count();i++)
        {
            PolyDevices[i].Connect(JackID,MenuJackID);
        }
    }
    DrawConnections();
}

void CDesktopComponent::mousePressEvent(QMouseEvent *event)
{
    Rubberband->hide();
    Dragging=false;
    DragJack=NULL;
    MouseDown=true;
    StartPoint=mapToScene(event->pos().x(),event->pos().y()).toPoint();
    int DI=DeviceIndex(StartPoint);
    if (DI > -1)
    {
        if (m_DeviceIndex!=DI)
        {
            SelectDevice(DI);
        }
    }
    DragJack=MouseOverJack(StartPoint,DragJackPos);
    if (DragJack)
    {
        if (event->button()==Qt::LeftButton)
        {
            setCursor(Qt::OpenHandCursor);
            Dragging=true;
            return;
        }
        if (event->button()==Qt::RightButton)
        {
            MenuJackID=DeviceList.JackID(DragJack);
            JackPopup->clear();
            for (int i=0;i<DeviceList.JackCount();i++)
            {
                IJack* J=DeviceList.Jacks(i);
                if (DeviceList.CanConnect(DragJack,J))
                {
                    QAction* a=JackPopup->addAction(DeviceList.JackID(J));
                    a->setCheckable(true);
                    a->setChecked(DeviceList.IsConnected(DragJack,J));
                    connect(a,SIGNAL(triggered()),mapper,SLOT(map()));
                    mapper->setMapping(a,DeviceList.JackID(J));
                }
            }
            if (JackPopup->actions().count()==0)
            {
                QAction* a=JackPopup->addAction("(No Available Connections)");
                a->setEnabled(false);
            }
            JackPopup->popup(mapToGlobal(event->pos()));
            MouseDown=false;
            return;
        }
    }
    if (DI > -1)
    {
        if (event->button()==Qt::LeftButton)
        {
            m_MD=true;
            setCursor(Qt::PointingHandCursor);
            Start=StartPoint;
            StartPos=Devices[DI].Geometry.topLeft();
            MouseDown=false;
            return;
        }
        if (event->button()==Qt::RightButton)
        {
            MarkList.clear();
            MarkList.append(Devices[DI].Device());
            actionPasteParameters->setEnabled(QApplication::clipboard()->text().startsWith("<Parameters"));
            DeviceMenu->popup(mapToGlobal(event->pos()));
            //emit DevicePopup(mapToGlobal(event->pos()));
            MouseDown=false;
            return;
        }
    }
    else
    {
        if (event->button()==Qt::RightButton)
        {
            MarkList.clear();
            MouseDown=false;
            PluginsPopup->clear();
            QStringList plugs=CAddIns::AddInNames();
            for (int i=0;i<plugs.count();i++)
            {
                QAction* a=PluginsPopup->addAction(plugs[i]);
                pluginmapper->setMapping(a,plugs[i]);
                connect(a,SIGNAL(triggered()),pluginmapper,SLOT(map()));
            }
            CreateRecentMenu(RecentMenu,RecentMapper);
            CreateRecentMenu(RecentPopup,RecentPopupMapper);
            actionPaste->setEnabled(QApplication::clipboard()->text().startsWith("<Devices>"));
            DesktopMenu->popup(mapToGlobal(event->pos()));
            return;
        }
    }
}

void CDesktopComponent::mouseMoveEvent(QMouseEvent *event)
{
    QPoint Pos=mapToScene(event->pos().x(),event->pos().y()).toPoint();
    if (Pos==MousePos) return;
    MousePos=Pos;
    if (m_MD)
    {
        if (Pos != Start)
        {
            Devices[m_DeviceIndex].Geometry.setTopLeft(StartPos+(Pos-StartPoint));
            Start=Pos;
            DrawConnections();
        }
        return;
    }
    if (Dragging)
    {
        SetConnectCursor(Pos);
        /*
        for (int i=0;i<DragList.count();i++)
        {
            if (Scene.items().contains(DragList[i])) Scene.removeItem(DragList[i]);
        }
        */
        DragList.clear();
        DrawConnections();
        if (DragJack->Direction==IJack::Out)
        {
            DragList.append(DrawArrow(DragJackPos,Pos,DragJack->JackColor()));
            for (int i=0;i<DragList.count();i++) DragList[i]->setZValue(3);
            DragList.append(DrawArrow(DragJackPos+QPoint(5,5),Pos+QPoint(5,5),QColor(0,0,0,40)));
        }
        else
        {
            DragList.append(DrawArrow(Pos,DragJackPos,DragJack->JackColor()));
            for (int i=0;i<DragList.count();i++) DragList[i]->setZValue(3);
            DragList.append(DrawArrow(Pos+QPoint(5,5),DragJackPos+QPoint(5,5),QColor(0,0,0,40)));
        }

        return;
    }
    else
    {
        IJack* HoverJack=MouseOverJack(Pos);
        if (HoverJack)
        {
            setToolTip(DeviceList.JackID(HoverJack));
        }
        else
        {
            setToolTip(QString());
        }
        if (MouseDown)
        {
            if (DeviceIndex(Pos) == -1)
            {
                Rubberband->setGeometry(this->rect());
                Rubberband->setWindowGeometry(QRect(StartPoint,Pos).normalized().translated(-mapToScene(0,0).toPoint()));
                if (!Rubberband->isVisible()) Rubberband->show();
            }
        }

    }
}

void CDesktopComponent::mouseReleaseEvent(QMouseEvent *event)
{
    MouseDown=false;
    QPoint Pos=mapToScene(event->pos().x(),event->pos().y()).toPoint();
    if (event->button()==Qt::LeftButton)
    {
        if (m_MD)
        {
            setCursor(Qt::ArrowCursor);
            if (Pos != Start)
            {
                Devices[m_DeviceIndex].Geometry.setTopLeft(StartPos+(Pos-StartPoint));
                Start=Pos;
                DrawConnections();
            }
            m_MD=false;
        }
    }
    ConnectDrop(Pos);
    if (Rubberband->isVisible())
    {
        MarkList.clear();
        for (int i=0;i<Devices.count();i++)
        {
            if (DeviceInsideRect(Devices[i])) MarkList.append(Devices[i].Device());
        }
        if (MarkList.count())
        {
            //emit EditPopup(mapToGlobal(event->pos()));
            qDebug() << QApplication::clipboard()->text();
            actionPasteParameters->setEnabled(QApplication::clipboard()->text().startsWith("<Parameters"));
            MarkMenu->popup(mapToGlobal(event->pos()));
        }
    }
}

void CDesktopComponent::mouseDoubleClickEvent(QMouseEvent *event)
{
    Dragging=false;
    DragJack=NULL;

    QPoint Pos=mapToScene(event->pos().x(),event->pos().y()).toPoint();
    IJack* J=MouseOverJack(Pos);
    if (J)
    {
        DeviceList.DisconnectJack(J);
        for (int i=0;i<PolyDevices.count();i++)
        {
            PolyDevices[i].DisconnectJack(J);
        }
        setToolTip(DeviceList.JackID(J));
        setCursor(Qt::ArrowCursor);
        DrawConnections();
        return;
    }
    int DI=DeviceIndex(Pos);
    SelectDevice(DI);
    Execute();
}

void CDesktopComponent::New()
{
    Clear();
    SelectDevice(0);
}

void CDesktopComponent::Open()
{
    QFileDialog d(this);
    d.setFileMode(QFileDialog::ExistingFile);
    d.setNameFilter("XML files (*.xml)");
    if (!FileName.isEmpty())
    {
        d.selectFile(FileName);
    }
    if (d.exec()!=QDialog::Accepted) return;
    QString fn=d.selectedFiles().first();
    if (!fn.isEmpty())
    {
        if (QFileInfo(fn).exists())
        {
            OpenFile(fn);
        }
    }
    SelectDevice(0);
}

void CDesktopComponent::Save()
{
    if (FileName.isEmpty())
    {
        SaveAs();
    }
    else
    {
        SaveFile(FileName);
    }
}

void CDesktopComponent::SaveAs()
{
    QString FilePath = QFileDialog::getSaveFileName(this,"Save XML file","","XML files (*.xml)");
    if(FilePath.isEmpty()) return;
    SaveFile(FilePath);
}

void CDesktopComponent::Cut()
{
    if ((!MarkList.count()) && (m_DeviceIndex>-1)) MarkList.append(DeviceList.Device(m_DeviceIndex));
    Copy();
    for (int i=0;i<MarkList.count();i++)
    {
        RemoveDevice(MarkList[i]);
    }
    SelectDevice(Devices.count()-1);
}

void CDesktopComponent::SavePresetAs()
{
    QString FileName=QFileInfo(qApp->applicationDirPath()+"/"+Devices[m_DeviceIndex].Device()->Name()+".ssd").absoluteFilePath();
    QDomLiteDocument* TempDoc;
    if (QFileInfo(FileName).exists())
    {
        TempDoc=new QDomLiteDocument(FileName);
    }
    else
    {
        TempDoc=new QDomLiteDocument("Presets",Devices[m_DeviceIndex].Device()->Name());
    }
    QDomLiteElement* Parameters=NULL;
    forever
    {
        QString InputString=QInputDialog::getText(this,"Name","Enter Preset Name",QLineEdit::Normal,"New Preset");
        foreach (QDomLiteElement* e,TempDoc->documentElement->elementsByTag("Preset"))
        {
            if (e->attribute("PresetName")==InputString)
            {
                Parameters=e;
                break;
            }
        }
        if (!Parameters)
        {
            Parameters=TempDoc->documentElement->appendChild("Preset");
            Parameters->setAttribute("PresetName",InputString);
            break;
        }
        if (QMessageBox::question(this,"Replace Preset?","Replace the Preset "+InputString,QMessageBox::No,QMessageBox::Yes)==QMessageBox::Yes)
        {
            Parameters=TempDoc->documentElement->replaceChild(Parameters,"Preset");
            Parameters->setAttribute("PresetName",InputString);
            break;
        }
        else
        {
            return;
        }
    }
    DeviceList.SaveParameters(Parameters,Devices[m_DeviceIndex].Device());
    TempDoc->save(FileName);
    ShowParameters(Devices[m_DeviceIndex].Device());
}

void CDesktopComponent::Copy()
{
    if ((!MarkList.count()) && (m_DeviceIndex>-1)) MarkList.append(DeviceList.Device(m_DeviceIndex));
    if (MarkList.count())
    {
        QDomLiteElement xml("Devices");
        for (int i=0;i<MarkList.count();i++)
        {
            QDomLiteElement* XMLDevice=xml.appendChild("Device");
            XMLDevice->setAttribute("Type",MarkList[i]->Name());
            XMLDevice->setAttribute("ClassName",MarkList[i]->Name()+".dll");
            QRect r=Devices[DeviceList.IndexOfDevice(MarkList[i])].Geometry;
            if (MarkList.count()>1)
            {
                r.translate(-Rubberband->windowGeometry().topLeft());
            }
            else
            {
                r.setTopLeft(QPoint(0,0));
            }
            XMLDevice->setAttribute("Top",r.top());
            XMLDevice->setAttribute("Left",r.left());
            XMLDevice->setAttribute("Index",MarkList[i]->Index());
            DeviceList.SaveParameters(XMLDevice,MarkList[i]);
        }
        for (int i=0;i<MarkList.count();i++)
        {
            for (int i1=0;i1<MarkList[i]->JackCount();i1++)
            {
                if (MarkList[i]->GetJack(i1)->Direction==IJack::In)
                {
                    CInJack* IJ=(CInJack*)MarkList[i]->GetJack(i1);
                    for (int i2=0;i2<IJ->OutJackCount();i2++)
                    {
                        COutJack* OJ=IJ->OutJack(i2);
                        QDomLiteElement* XMLConnection=xml.appendChild("Connection");
                        XMLConnection->setAttribute("InJack",DeviceList.JackID((IJack*)IJ));
                        XMLConnection->setAttribute("OutJack",DeviceList.JackID((IJack*)OJ));
                    }
                }
            }
        }
        QApplication::clipboard()->setText(xml.toString());
    }
}

void CDesktopComponent::Paste()
{
    QString TempClipBoard=QApplication::clipboard()->text();
    if (!TempClipBoard.isEmpty())
    {
        QDomLiteElement xml;
        xml.fromString(TempClipBoard);
        if (xml.tag=="Devices")
        {
            if (xml.childCount())
            {
                //QPoint Pos=QPointF(mapFromGlobal(this->cursor().pos())).toPoint();
                //qDebug() << cursor().pos() << Pos;
                QDomLiteElementList XMLDevices=xml.elementsByTag("Device");
                QStringList Before;
                QStringList After;
                foreach(QDomLiteElement* XMLDevice,XMLDevices)
                {
                    QString Name=XMLDevice->attribute("Type");
                    int Index=XMLDevice->attributeValue("Index");
                    Before.append(Name + " " + QString::number(Index));

                    int FreeIndex=DeviceList.FindFreeID(Name);
                    After.append(Name + " " + QString::number(FreeIndex));
                    if (AddDevice(Name,FreeIndex,this))
                    {
                        Devices.last().Geometry.setTopLeft(QPoint(XMLDevice->attributeValue("Left"),XMLDevice->attributeValue("Top"))+StartPoint);
                        LoadParameters(XMLDevice,Devices.last().Device());
                        ShowParameters(Devices.last().Device());
                        //D->Visible=true;
                    }
                }
                QDomLiteElementList XMLConnections=xml.elementsByTag("Connection");
                foreach(QDomLiteElement* XMLConnection,XMLConnections)
                {
                    QString InJack=XMLConnection->attribute("InJack");
                    QString OutJack=XMLConnection->attribute("OutJack");
                    for (int i=0; i < Before.count(); i++)
                    {
                        if (InJack.contains(Before[i]))
                        {
                            InJack=After[i] + InJack.mid(Before[i].length());
                        }
                        if (OutJack.contains(Before[i]))
                        {
                            OutJack=After[i] + OutJack.mid(Before[i].length());
                        }
                    }
                    Connect(InJack,OutJack);

                }
            }
        }
    }
    if (m_DeviceIndex < 0)
    {
        SelectDevice(0);
    }
    else
    {
        DrawConnections();
    }
}

void CDesktopComponent::RemovePreset(QString PresetName)
{
    QString FileName=QFileInfo(qApp->applicationDirPath()+"/"+Devices[m_DeviceIndex].Device()->Name()+".ssd").absoluteFilePath();
    QDomLiteDocument* TempDoc;
    if (QFileInfo(FileName).exists())
    {
        TempDoc=new QDomLiteDocument(FileName);
    }
    else
    {
        TempDoc=new QDomLiteDocument("Presets",Devices[m_DeviceIndex].Device()->Name());
    }
    QDomLiteElement* Parameters=NULL;
    foreach (QDomLiteElement* e,TempDoc->documentElement->elementsByTag("Preset"))
    {
        if (e->attribute("PresetName")==PresetName)
        {
            Parameters=e;
            break;
        }
    }
    if (Parameters)
    {
        TempDoc->documentElement->removeChild(Parameters);
        TempDoc->save(FileName);
        ShowParameters(Devices[m_DeviceIndex].Device());
    }
}

void CDesktopComponent::OpenPreset(QString PresetName)
{
    QString FileName=QFileInfo(qApp->applicationDirPath()+"/"+Devices[m_DeviceIndex].Device()->Name()+".ssd").absoluteFilePath();
    QDomLiteDocument* TempDoc;
    if (QFileInfo(FileName).exists())
    {
        TempDoc=new QDomLiteDocument(FileName);
    }
    else
    {
        TempDoc=new QDomLiteDocument("Presets",Devices[m_DeviceIndex].Device()->Name());
    }
    QDomLiteElement* Parameters=NULL;
    foreach (QDomLiteElement* e,TempDoc->documentElement->elementsByTag("Preset"))
    {
        if (e->attribute("PresetName")==PresetName)
        {
            Parameters=e;
            break;
        }
    }
    if (Parameters)
    {
        LoadParameters(Parameters,Devices[m_DeviceIndex].Device());
        DrawConnections();
        ShowParameters(Devices[m_DeviceIndex].Device());
    }
}

void CDesktopComponent::CopyParameters()
{
    QDomLiteElement xml("Parameters");
    xml.setAttribute("Type",Devices[m_DeviceIndex].Device()->Name());
    DeviceList.SaveParameters(&xml,Devices[m_DeviceIndex].Device());
    QApplication::clipboard()->setText(xml.toString());
}

void CDesktopComponent::PasteParameters()
{
    QString TempClipBoard=QApplication::clipboard()->text();
    if (!TempClipBoard.isEmpty())
    {
        QDomLiteElement xml;
        xml.fromString(TempClipBoard);

        if (xml.childCount())
        {
            if (xml.tag=="Parameters")
            {
                QString ClassName=xml.attribute("Type");
                if (!ClassName.isEmpty())
                {
                    if (ClassName==Devices[m_DeviceIndex].Device()->Name())
                    {
                        LoadParameters(&xml,Devices[m_DeviceIndex].Device());
                        ShowParameters(Devices[m_DeviceIndex].Device());
                    }
                }
            }
        }
    }
    DrawConnections();
}

void CDesktopComponent::RemoveConnections()
{
    DesktopMenu->hide();
    if ((!MarkList.count()) && (m_DeviceIndex>-1)) MarkList.append(DeviceList.Device(m_DeviceIndex));
    for (int i=0;i<MarkList.count();i++)
    {
        DisconnectDevice(MarkList[i]);
    }
    DrawConnections();
}

void CDesktopComponent::Execute()
{
    if (m_DeviceIndex>-1)
    {
        for (int i=0;i<PolyDevices.count();i++)
        {
            PolyDevices[i].Device(m_DeviceIndex)->Execute(false);
        }
        DeviceList.Device(m_DeviceIndex)->Execute(true);
    }
    DrawConnections();
}

void CDesktopComponent::ParameterPopup(QPoint Pos)
{
    ParametersMenu->popup(Pos);
}
