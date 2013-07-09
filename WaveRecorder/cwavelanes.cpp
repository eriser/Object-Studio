#include "cwavelanes.h"
#include "ui_cwavelanes.h"
#include "softsynthsclasses.h"
#include "cwaveeditwidget.h"
#include <QListWidget>
#include <QScrollBar>
#include <QHBoxLayout>

CWaveTrack::CWaveTrack(const QString &Filename, size_t StartPointer)
{
    Name=Filename;
    Start=StartPointer;
    Valid=W.open(Filename,CPresets::Presets.SampleRate,CPresets::Presets.ModulationRate);
    LP.Start=0;
    LP.End=W.GetLength();
    LP.FadeIn=0;
    LP.FadeOut=0;
    LP.Volume=100;
}

size_t CWaveTrack::PlayLength()
{
    return LP.End-LP.Start;
}

float CWaveTrack::Volume(size_t Counter)
{
    float Vol=(float)LP.Volume*0.01;
    if (Counter < LP.FadeIn)
    {
        Vol*=(double)Counter/(float)LP.FadeIn;
    }
    if (Counter>PlayLength()-LP.FadeOut)
    {
        Vol*=(double)(PlayLength()-Counter)/(float)LP.FadeOut;
    }
    return Vol;
}

void CWaveTrack::Paint(QCanvasLayer* Canvas, float ZoomFactor)
{
    QPen p;
    QLinearGradient lg(0,Geometry.top(),0,Geometry.height()+Geometry.top());
    if (Active)
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
    Canvas->SetBrush(lg);
    Canvas->SetPen(p);
    Canvas->Rectangle(Geometry);
    Canvas->SetBrush(QBrush(Qt::NoBrush));
    Canvas->SetPen(Qt::black);
    QFont f("",9);
    QFontMetrics fm(f);
    QString Caption=QFileInfo(Name).baseName();
    while (fm.width(Caption)>Geometry.width())
    {
        Caption.chop(1);
    }
    Canvas->SetFont(f);
    Canvas->Text(Geometry.topLeft(),Caption);

    float YFactor=Geometry.height()/2;
    int HalfHeight=Geometry.top()+(Geometry.height()/2);
    float ZoomValue=1.0/ZoomFactor;
    Canvas->MoveTo(Geometry.left(),HalfHeight);
    Canvas->SetPen(Qt::black);
    for (int Channel=0;Channel<W.Channels;Channel++)
    {
        float* Buffer=W.BufferPointer(Channel);
        double i=LP.Start;
        for (int i1=Geometry.left();i1<Geometry.right();i1++)
        {
            if (i>=LP.End) break;
            int Val=Buffer[(size_t)i]*YFactor;
            if (Val==0)
            {
                Canvas->LineTo(i1,HalfHeight);
            }
            else
            {
                Canvas->MoveTo(i1,HalfHeight-Val);
                Canvas->LineTo(i1,HalfHeight+Val);
            }
            i=i+ZoomValue;
        }
    }
    Canvas->SetPen(Qt::red);
    Canvas->MoveTo(Geometry.left(),Geometry.bottom());
    Canvas->LineTo(Geometry.bottomLeft()+QPoint(LP.FadeIn*ZoomFactor,-Geometry.height()*LP.Volume*0.01));
    Canvas->LineTo(Geometry.right()-(LP.FadeOut*ZoomFactor),Geometry.bottom()-(Geometry.height()*LP.Volume*0.01));
    Canvas->LineTo(Geometry.right(),Geometry.bottom());
}

CWaveLane::CWaveLane()
{
    Vol=0;
}

CWaveLane::~CWaveLane()
{
    qDeleteAll(Tracks);
}

void CWaveLane::ModifyBuffers(float *L, float *R, float MixFactor)
{
    CWaveTrack* PlayingTrack=NULL;
    CWaveTrack* StartingTrack=NULL;
    int ModRate=CPresets::Presets.ModulationRate;
    for (int i=0;i<Tracks.count();i++)
    {
        if (Tracks[i]->PlayLength()+Tracks[i]->Start>Counter)
        {
            if (Tracks[i]->Start<Counter)
            {
                PlayingTrack=Tracks[i];
            }
            else if (Tracks[i]->Start<Counter+ModRate)
            {
                StartingTrack=Tracks[i];
            }
        }
    }
    if ((StartingTrack==NULL) && (PlayingTrack==NULL))
    {
        CurrentBuffer=NULL;
    }
    for (int i=0;i<ModRate;i++)
    {
        if (PlayingTrack)
        {
            if (Counter>=PlayingTrack->Start+PlayingTrack->PlayLength())
            {
                CurrentBuffer=NULL;
            }
            else
            {
                if (ModulationCounter>=ModRate)
                {
                    PlayingTrack->W.SetPointer((Counter-PlayingTrack->Start)+PlayingTrack->LP.Start);
                    CurrentBuffer=PlayingTrack->W.GetNext();
                    CurrentChannels=PlayingTrack->W.Channels;
                    ModulationCounter=0;
                    Vol=PlayingTrack->Volume(Counter-(PlayingTrack->Start))*MixFactor;
                }
            }
        }
        if (StartingTrack)
        {
            if (Counter>=StartingTrack->Start+StartingTrack->PlayLength())
            {
                CurrentBuffer=NULL;
            }
            else
            {
                if (Counter==StartingTrack->Start)
                {
                    StartingTrack->W.Reset();
                    StartingTrack->W.SetPointer(StartingTrack->LP.Start);
                    CurrentBuffer=StartingTrack->W.GetNext();
                    CurrentChannels=StartingTrack->W.Channels;
                    ModulationCounter=0;
                    Vol=StartingTrack->Volume(Counter-(StartingTrack->Start))*MixFactor;
                }
            }
        }
        if (CurrentBuffer)
        {
            L[i]+=CurrentBuffer[ModulationCounter]*Vol;
            if (CurrentChannels==1)
            {
                R[i]+=CurrentBuffer[ModulationCounter]*Vol;
            }
            else
            {
                R[i]+=CurrentBuffer[ModulationCounter+ModRate]*Vol;
            }
       }
        ModulationCounter++;
        Counter++;
    }
}

void CWaveLane::Reset()
{
    Counter=0;
    ModulationCounter=0;
    CurrentBuffer=0;
    CurrentChannels=1;
    foreach (CWaveTrack* T,Tracks)
    {
        if (!T->Valid)
        {
            Tracks.removeOne(T);
            delete T;
        }
    }
}

void CWaveLane::UpdateGeometry(float ZoomFactor, int CanvasRight, int BorderWidth)
{
    int MaxLen=CanvasRight;
    foreach (CWaveTrack* T,Tracks)
    {
        if (!T->Valid)
        {
            Tracks.removeOne(T);
            delete T;
        }
        else
        {
            T->Geometry=QRect(BorderWidth+(((float)T->Start)*ZoomFactor),Geometry.top(),(float)T->PlayLength()*ZoomFactor,Geometry.height());
            if (T->Geometry.right()+400>MaxLen) MaxLen=T->Geometry.right()+400;
        }
    }
    Geometry.setRight(MaxLen);
}

void CWaveLane::Paint(QCanvasLayer* Canvas, float ZoomFactor)
{
    Canvas->SetPenBrush(Qt::darkGray,Qt::lightGray);
    Canvas->Rectangle(Geometry);
    foreach (CWaveTrack* T,Tracks)
    {
        T->Paint(Canvas,ZoomFactor);
    }
}

void CWaveLane::RemoveFile(const QString &Filename)
{
    foreach(CWaveTrack* T,Tracks)
    {
        if (T->Name==Filename)
        {
            Tracks.removeOne(T);
            delete T;
        }
    }
}

void CWaveLane::Load(const QString &XML,float ZoomFactor)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    QDomLiteElement* xmlLane=xml.firstChild();
    if (xmlLane)
    {
        if (xmlLane->tag=="Lane")
        {
            foreach (QDomLiteElement* xmlTrack,xmlLane->childElements)
            {
                int Position=xmlTrack->attributeValue("Position");
                quint64 Start=xmlTrack->attributeValue("Start");
                if (Start==0) Start=(float)Position/ZoomFactor;
                QString CurrentPath = xmlTrack->attribute("Path");
                QString Name=CPresets::ResolveFilename(CurrentPath);

                CWaveTrack* WT=new CWaveTrack(Name,Start);
                if (WT->Valid)
                {
                    WT->LP.Start=xmlTrack->attributeValue("StartPoint");
                    WT->LP.End=xmlTrack->attributeValue("EndPoint");
                    WT->LP.FadeIn=xmlTrack->attributeValue("FadeIn");
                    WT->LP.FadeOut=xmlTrack->attributeValue("FadeOut");
                    WT->LP.Volume=xmlTrack->attributeValue("Volume");
                    Tracks.append(WT);
                }
            }
        }
    }
}

const QString CWaveLane::Save()
{
    QDomLiteElement xml("Lane");
    foreach(CWaveTrack* WT,Tracks)
    {
        QDomLiteElement* xmlTrack=xml.appendChild("Track");
        QString Relpath=QDir().relativeFilePath(WT->Name);
        xmlTrack->setAttribute("Path",Relpath);
        xmlTrack->setAttribute("Start",QVariant().fromValue(WT->Start));
        xmlTrack->setAttribute("StartPoint",QVariant().fromValue(WT->LP.Start));
        xmlTrack->setAttribute("EndPoint",QVariant().fromValue(WT->LP.End));
        xmlTrack->setAttribute("FadeIn",QVariant().fromValue(WT->LP.FadeIn));
        xmlTrack->setAttribute("FadeOut",QVariant().fromValue(WT->LP.FadeOut));
        xmlTrack->setAttribute("Volume",WT->LP.Volume);
    }
    return xml.toString();
}

CWaveLanes::CWaveLanes(QWidget *parent) :
    QCanvas(parent,2),
    ui(new Ui::CWaveLanes)
{
    ui->setupUi(this);
    setAcceptDrops(true);
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
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    /*
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setFrameStyle(0);
    setLineWidth(0);
    */
    InfoLabel=new QLabel(this);
    InfoLabel->setAutoFillBackground(true);
    InfoLabel->setFrameStyle(QFrame::Box | QFrame::Plain);
    InfoLabel->hide();

    Popup=new QMenu(this);
    Popup->addAction("Add Track",this,SLOT(AddLane()));
    Popup->addAction("Remove Track",this,SLOT(RemoveLane()));
    CurrentLane=-1;
    CurrentTrack=-1;
    DragTrack=-1;
    RulerTempo=120;
    RulerBeats=4;
    MixFactor=1;
    ZoomFactor=0.001;
    Lanes.append(new CWaveLane);
    Loading=false;
    Paint();
    TimerID=0;
}

CWaveLanes::~CWaveLanes()
{
    qDeleteAll(Lanes);
    delete ui;
}

void CWaveLanes::timerEvent(QTimerEvent *)
{
    int LineX=BorderWidth+((double)(BufferCount*CPresets::Presets.ModulationRate)*ZoomFactor);
    if (LineX != CurrentLineX)
    {
        CurrentLineX=LineX;
        QCanvasLayer* L=CanvasLayers[1];
        int LineHeight=height();
        QRect EraseRect(LineX-20,0,22,LineHeight);
        L->EraseTransparent(EraseRect);
        L->SetPen(Qt::red);
        L->Line(LineX,0,LineX,LineHeight);
        if (!visibleRegion().intersects(EraseRect))
        {
            emit RequestVisible(LineX,0);
            return;
        }
        repaint(EraseRect);
    }
}

void CWaveLanes::ModifyBuffers(float *L, float *R)
{
    if (Loading) return;
    foreach (CWaveLane* Lane, Lanes) Lane->ModifyBuffers(L,R,MixFactor);
    BufferCount++;
}

void CWaveLanes::Reset()
{
    for (int i=0;i<Lanes.count();i++) Lanes[i]->Reset();
    BufferCount=0;
    CurrentLineX=-1;
    TimerID=startTimer(50);
    CanvasLayers[1]->ClearTransparent();
    repaint();
    CalcMixFactor();
}

void CWaveLanes::CalcMixFactor()
{
    MixFactor=1.0/sqrtf(Lanes.count());
}

void CWaveLanes::Stop()
{
    if (TimerID) killTimer(TimerID);
    TimerID=0;
}
/*
void CWaveLanes::resizeEvent(QResizeEvent* event)
{
}
*/
void CWaveLanes::Paint()
{
    for (int i=0;i<Lanes.count();i++)
    {
        for(int j=0;j<Lanes[i]->Tracks.count();j++)
        {
            if ((i==CurrentLane) && (j==CurrentTrack))
            {
                Lanes[i]->Tracks[j]->Active=true;
            }
            else
            {
                Lanes[i]->Tracks[j]->Active=false;
            }
        }
    }
    int MaxLen=0;
    for (int i=0;i<Lanes.count();i++)
    {
        CWaveLane* Lane=Lanes[i];
        Lane->Geometry=QRect(BorderWidth,BorderWidth+RulerHeight+(LaneHeight*i),200,LaneHeight-LaneGap);
        Lane->UpdateGeometry(ZoomFactor,width()-BorderWidth,BorderWidth);
        if (Lane->Geometry.width()>MaxLen) MaxLen=Lane->Geometry.width();
    }
    QSize NewSize((BorderWidth*2)+MaxLen,(Lanes.count()*LaneHeight)+(BorderWidth*2)+RulerHeight);
    if (NewSize != size()) setFixedSize(NewSize);
    if (NewSize != BackImage.size()) SetSize(NewSize);
    ClearGradient();
    CanvasLayers[0]->ClearTransparent();
    for (int i=0;i<Lanes.count();i++)
    {
        Lanes[i]->UpdateGeometry(ZoomFactor,width()-BorderWidth,BorderWidth);
        if (i==CurrentLane)
        {
            Lanes[i]->Paint(CanvasLayers[0],ZoomFactor);
        }
        else
        {
            Lanes[i]->Paint(this,ZoomFactor);
        }
    }

    int BarCount=1;
    int BeatCount=1;
    float X=BorderWidth;
    QFont f("",8);
    SetFont(f);
    SetPen(Qt::black);
    SetBrush(QBrush(Qt::NoBrush));
    while (X<MaxLen+BorderWidth)
    {
        if (BeatCount>RulerBeats)
        {
            BeatCount=1;
        }
        if (BeatCount==1)
        {
            Text(X-2,(RulerHeight+BorderWidth)-22,QString::number(BarCount));
            BarCount++;
        }
        Text(X-2,(RulerHeight+BorderWidth)-14,QString::number(BeatCount));
        BeatCount++;
        Line(X,RulerHeight+BorderWidth-2,X,RulerHeight+BorderWidth-6);
        X=X+(((float)(CPresets::Presets.SampleRate*60)/RulerTempo)*ZoomFactor);
    }
    int SecCount=0;
    int MinCount=0;
    X=BorderWidth;
    while (X<MaxLen+BorderWidth)
    {
        if (SecCount>=60)
        {
            MinCount++;
            SecCount=0;
        }
        Text(X-2,(RulerHeight+BorderWidth)-34,QString::number(MinCount)+":"+QString::number(SecCount));
        SecCount++;
        Line(X,RulerHeight+BorderWidth-22,X,RulerHeight+BorderWidth-26);
        X=X+((float)(CPresets::Presets.SampleRate)*ZoomFactor);
    }
    CalcMixFactor();
    update();
}

void CWaveLanes::RemoveFile(const QString &Filename)
{
    for (int i=0;i<Lanes.count();i++)
    {
        Lanes[i]->RemoveFile(Filename);
    }
    Paint();
}

const QString CWaveLanes::Save()
{
    QDomLiteElement xml("Lanes");
    for (int i = 0; i < Lanes.count(); i++)
    {
        QDomLiteElement* Lane = xml.appendChild("Lane" + QString::number(i+1));
        Lane->appendChildFromString(Lanes[i]->Save());
    }
    return xml.toString();
}

void CWaveLanes::Load(const QString &XML)
{
    Loading=true;
    qDeleteAll(Lanes);
    Lanes.clear();
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Lanes")
    {
        int i=0;
        QDomLiteElement* Lane=xml.elementByTag("Lane"+QString::number(i+1));
        while (Lane)
        {
            CWaveLane* L=new CWaveLane;
            Lanes.append(L);
            L->Load(Lane->toString(),ZoomFactor);
            i++;
            Lane=xml.elementByTag("Lane"+QString::number(i+1));
        }
    }
    if (Lanes.isEmpty())
    {
        Lanes.append(new CWaveLane);
    }
    Paint();
    Loading=false;
}

int CWaveLanes::MouseOverLane(QPoint Pos)
{
    for(int i=0;i<Lanes.count();i++)
    {
        if (Lanes[i]->Geometry.contains(Pos)) return i;
    }
    return -1;
}

int CWaveLanes::MouseOverTrack(QPoint Pos, int Lane)
{
    if (Lane>-1)
    {
        for(int i=0;i<Lanes[Lane]->Tracks.count();i++)
        {
            if (Lanes[Lane]->Tracks[i]->Geometry.contains(Pos)) return i;
        }
    }
    return -1;
}

void CWaveLanes::ShowInfoLabel(quint64 Start,int Lane)
{
    int Minutes=(Start/CPresets::Presets.SampleRate)/60.0;
    int Seconds=(Start/CPresets::Presets.SampleRate)%60;
    int MSeconds=(int)((double)Start/((float)CPresets::Presets.SampleRate/1000.0))%1000;
    int Beat=(int)(((Start*RulerTempo)/CPresets::Presets.SampleRate)/60)%RulerBeats;
    int Bar=(int)(((Start*RulerTempo)/CPresets::Presets.SampleRate)/60)/RulerBeats;
    int BeatRemains=((Start%(int)((60*CPresets::Presets.SampleRate)/RulerTempo))*100)/((60*CPresets::Presets.SampleRate)/RulerTempo);
    //QPoint Zero(0,0);//=mapToScene(0,0).toPoint();
    InfoLabel->hide();
    InfoLabel->setFont(QFont("",11));
    QFontMetrics fm(InfoLabel->font());
    InfoLabel->setText("Sample "+QString::number(Start)+" \n"+QString::number(Minutes)+":"+QString::number(Seconds)+":"+QString::number(MSeconds)+" \nBar "+QString::number(Bar+1)+":"+QString::number(Beat+1)+":"+QString::number(BeatRemains));
    InfoLabel->setGeometry(((double)Start*ZoomFactor)+BorderWidth,(Lane*LaneHeight)+BorderWidth+RulerHeight-InfoLabel->height(),fm.width("Sample "+QString::number(Start)+" \n")+4,(fm.height()*3)+4);
    //InfoLabel->resize(fm.width("Sample "+QString::number(Start)+" \n")+4,(fm.height()*3)+4);
    //InfoLabel->move(((double)Start*ZoomFactor)+BorderWidth,(Lane*LaneHeight)+BorderWidth+RulerHeight-InfoLabel->height());
    InfoLabel->show();
}

void CWaveLanes::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint Pos=event->pos();
    int Lane=MouseOverLane(Pos);
    int Track=MouseOverTrack(Pos,Lane);
    if (Track>-1)
    {
        QDialog d(this,Qt::Tool | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        d.resize(600,200);
        CWaveEditWidget* e=new CWaveEditWidget(&d);
        connect(e,SIGNAL(UpdateHost(CWaveGenerator::LoopParameters)),this,SLOT(UpdateEditTrack(CWaveGenerator::LoopParameters)));
        EditTrack=Lanes[Lane]->Tracks[Track];
        QHBoxLayout* l=new QHBoxLayout(&d);
        l->setMargin(0);
        d.setLayout(l);
        d.setWindowTitle(QFileInfo(EditTrack->Name).baseName());
        l->addWidget(e);
        e->Init(&EditTrack->W,EditTrack->LP,false);
        d.exec();
        Paint();
    }
}

void CWaveLanes::UpdateEditTrack(CWaveGenerator::LoopParameters LP)
{
    EditTrack->LP=LP;
}

void CWaveLanes::mousePressEvent(QMouseEvent *event)
{
    StartPos=event->pos();
    int Lane=MouseOverLane(StartPos);
    int Track=MouseOverTrack(StartPos,Lane);
    if (Track>-1)
    {
        if ((Lane != CurrentLane) || (Track != CurrentTrack))
        {
            CurrentLane=Lane;
            CurrentTrack=Track;
            Paint();
        }
    }
    if (event->button()==Qt::RightButton)
    {
        Popup->popup(mapToGlobal(event->pos()));
        return;
    }
    DragTrack=Track;
    if (DragTrack>-1)
    {
        CWaveLane* Lane=Lanes[CurrentLane];
        DragTrackStart=Lane->Tracks[DragTrack]->Start;
        ShowInfoLabel(Lane->Tracks[DragTrack]->Start,CurrentLane);
    }
}

void CWaveLanes::mouseMoveEvent(QMouseEvent *event)
{
    QPoint Pos=event->pos();
    if (DragTrack>-1)
    {
        if (Pos.x() != StartPos.x())
        {
            CWaveLane* Lane=Lanes[CurrentLane];
            long Start=DragTrackStart-(((float)(StartPos.x()-Pos.x()))/ZoomFactor);
            if (Start<0) Start=0;
            Lane->Tracks[DragTrack]->Start=Start;
            ShowInfoLabel(Lane->Tracks[DragTrack]->Start,CurrentLane);
            //Paint();
            Lane->UpdateGeometry(ZoomFactor,width()-BorderWidth,BorderWidth);
            CanvasLayers[0]->ClearTransparent();
            Lane->Paint(CanvasLayers[0],ZoomFactor);
            QRegion UpdateRect=visibleRegion().intersected(Lane->Geometry.adjusted(0,-2,0,2));
            update(UpdateRect);
        }
    }
}

void CWaveLanes::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint Pos=event->pos();
    if (DragTrack>-1)
    {
        InfoLabel->hide();
        if (Pos.x() != StartPos.x())
        {
            CWaveLane* Lane=Lanes[CurrentLane];
            long Start=DragTrackStart-(((float)(StartPos.x()-Pos.x()))/ZoomFactor);
            if (Start<0) Start=0;
            Lane->Tracks[DragTrack]->Start=Start;
            Lane->UpdateGeometry(ZoomFactor,width()-BorderWidth,BorderWidth);
            CanvasLayers[0]->ClearTransparent();
            Lane->Paint(CanvasLayers[0],ZoomFactor);
            QRegion UpdateRect=visibleRegion().intersected(Lane->Geometry.adjusted(0,-2,0,2));
            update(UpdateRect);
        }
    }
    DragTrack=-1;
}

void CWaveLanes::AddLane()
{
    Lanes.append(new CWaveLane);
    Paint();
}

void CWaveLanes::RemoveLane()
{
    if (Lanes.count()>0)
    {
        if (CurrentLane>-1)
        {
            CWaveLane* Lane=Lanes[CurrentLane];
            Lanes.removeOne(Lane);
            delete Lane;
            CurrentLane=-1;
            CurrentTrack=-1;
            Paint();
        }
    }
}

void CWaveLanes::dragEnterEvent(QDragEnterEvent *e)
{
    qDebug() << "Drag enter";
    e->acceptProposedAction();
}

void CWaveLanes::dragMoveEvent(QDragMoveEvent *e)
{
    qDebug() << "Drag move";
    int Lane=MouseOverLane(e->pos());
    int Track=MouseOverTrack(e->pos(),Lane);
    ShowInfoLabel((float)(e->pos().x()-BorderWidth)/ZoomFactor,Lane);
    if (Lane<0)
    {
        e->ignore();
        return;
    }
    if (Track>-1)
    {
        e->ignore();
        return;
    }
    e->acceptProposedAction();
}

void CWaveLanes::dropEvent(QDropEvent *e)
{
    qDebug() << "drop";
    InfoLabel->hide();
    int Lane=MouseOverLane(e->pos());
    int Track=MouseOverTrack(e->pos(),Lane);
    if (Lane<0) return;
    if (Track>-1) return;
    QListWidget* l=(QListWidget*)e->source();
    QListWidgetItem* i=l->item(l->currentRow());
    QString FN=i->data(34).toString();
    quint64 Start=(float)(e->pos().x()-BorderWidth)/ZoomFactor;
    CWaveTrack* T=new CWaveTrack(FN,Start);
    CWaveLane* L=Lanes[Lane];
    if (T->Valid)
    {
        Loading=true;
        int MaxOverlap=0;
        for (int i=0;i<L->Tracks.count();i++)
        {
            if ((L->Tracks[i]->Start>=T->Start) && (L->Tracks[i]->Start<T->Start+T->W.GetLength()))
            {
                int Overlap=(T->Start+T->W.GetLength())-Lanes[Lane]->Tracks[i]->Start;
                if (Overlap>MaxOverlap) MaxOverlap=Overlap;
            }
        }
        if (MaxOverlap)
        {
            for (int i=0;i<L->Tracks.count();i++)
            {
                if (L->Tracks[i]->Start>=T->Start) L->Tracks[i]->Start+=MaxOverlap;
            }
        }
        L->Tracks.append(T);

        Paint();
        Loading=false;
        e->acceptProposedAction();
    }
}
