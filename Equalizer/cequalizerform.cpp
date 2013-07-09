#include "cequalizerform.h"
#include "ui_cequalizerform.h"
#include <QPainter>

CEqualizerForm::CEqualizerForm(IDevice* Device, QWidget *parent) :
    CSoftSynthsForm(Device,true,parent),
    ui(new Ui::CEqualizerForm)
{
    ui->setupUi(this);
    m_Device=NULL;
    connect(ui->GraphFrame,SIGNAL(RefreshMe()),this,SLOT(Draw()));
    Canvas=ui->GraphFrame;
}

CEqualizerForm::~CEqualizerForm()
{
    delete ui;
}

void CEqualizerForm::Init(CEqualizer *EQ)
{
    m_Device=EQ;
    ui->frame_0->Init(EQ,0,40,280,100);
    ui->frame_1->Init(EQ,1,100,500,200);
    ui->frame_2->Init(EQ,2,200,1000,400);
    ui->frame_3->Init(EQ,3,400,2800,1000);
    ui->frame_4->Init(EQ,4,1000,5000,3000);
    ui->frame_5->Init(EQ,5,3000,9000,6000);
    ui->frame_6->Init(EQ,6,6000,18000,12000);
    ui->frame_7->Init(EQ,7,10000,20000,15000);
}

const QString CEqualizerForm::CustomSave()
{
    QDomLiteElement xml("Custom");

    for (int i=0;i<8;i++)
    {
        QDomLiteElement* Channel = xml.appendChild("Band" + QString::number(i+1));
        CEqualizerFrame* f=findChild<CEqualizerFrame*>("frame_"+QString::number(i));
        Channel->appendChildFromString(f->Save());
    }

    return xml.toString();
}

void CEqualizerForm::CustomLoad(const QString& XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        for (int i=0;i<8;i++)
        {
            QDomLiteElement* Channel = xml.elementByTag("Band" + QString::number(i+1));
            CEqualizerFrame* f=findChild<CEqualizerFrame*>("frame_"+QString::number(i));
            f->Load(Channel->firstChild()->toString());
        }
    }
}

int inline freqtopoint(float freq)
{
    return floor( (12.0*log2(freq/440.0)) + 0.5 )+69.0;
}

int inline PointtoFreq( int keynum )
{
    return 440 * pow( 2.0, ((float)keynum - 69.0) / 12.0 );
}

void CEqualizerForm::Draw()
{
    DrawBg();
    DrawGraph();
}

void CEqualizerForm::DrawBg()
{
    Canvas->ClearGradient();
    Canvas->SetBrush(QBrush(Qt::NoBrush));
    Canvas->SetPen(Qt::black);
    Canvas->SetFont(QFont("",10));
    QFontMetrics fm(QFont("",10));
    int P=(float)((freqtopoint(100)-23)*ui->GraphFrame->width()*1.56)/(200.0-22.0);
    Canvas->Line(P,ui->GraphFrame->height(),P,ui->GraphFrame->height()-5);
    Canvas->Text(P-(fm.width("100Hz")/2),ui->GraphFrame->height()-14,"100Hz");
    P=(float)((freqtopoint(1000)-23)*ui->GraphFrame->width()*1.56)/(200.0-22.0);
    Canvas->Line(P,ui->GraphFrame->height(),P,ui->GraphFrame->height()-5);
    Canvas->Text(P-(fm.width("1000Hz")/2),ui->GraphFrame->height()-14,"1000Hz");
    P=(float)((freqtopoint(10000)-23)*ui->GraphFrame->width()*1.56)/(200.0-22.0);
    Canvas->Line(P,ui->GraphFrame->height(),P,ui->GraphFrame->height()-5);
    Canvas->Text(P-(fm.width("10kHz")/2),ui->GraphFrame->height()-14,"10kHz");
    P=(float)((freqtopoint(20000)-23)*ui->GraphFrame->width()*1.56)/(200.0-22.0);
    Canvas->Line(P,ui->GraphFrame->height(),P,ui->GraphFrame->height()-5);
    Canvas->Text(P-(fm.width("20kHz")/2),ui->GraphFrame->height()-14,"20kHz");
}

void CEqualizerForm::DrawGraph()
{
    if (m_Device==NULL) return;
    QCanvasLayer* CanvasLayer=Canvas->CanvasLayers[0];
    CanvasLayer->ClearTransparent();
    CanvasLayer->SetPen(Qt::red);
    QPoint LastPoint(-1,40);
    int SR=CPresets::Presets.SampleRate;
    for (int i=0;i<8;i++)
    {
        eq_set_params(&Fi[i], m_Device->Freq[i], m_Device->Level[i], BWIDTH, SR);
    }
    for (int x = 22;x < 200;x++)
    {
        int F=PointtoFreq(x);
        float WPos=0;
        float Max=0;
        for (int i=0;i<8;i++)
        {
            biquad_init(&Fi[i]);
        }
        for (int i=0;i<512;i++)
        {
            WPos=WPos+F;
            while (WPos>SR)
            {
                WPos-=SR;
            }
            float samp=W.GetNext(WPos,CWaveBank::Sine);
            for (int i1=0;i1<8;i1++)
            {
                if (m_Device->Level[i1] != 0)
                    samp = biquad_run(&Fi[i1], samp);
            }
            if (samp<0)
            {
                if (-samp>Max)
                {
                    Max=-samp;
                }
            }
            else if (samp>0)
            {
                if (samp>Max)
                {
                    Max=samp;
                }
            }
        }
        int X1=(float)((x-23)*ui->GraphFrame->width()*1.56)/(200.0-22.0);
        QPoint EndPoint(X1,40+((1.0-Max)*30.0));
        CanvasLayer->Line(LastPoint,EndPoint);
        LastPoint=EndPoint;
        if (X1>ui->GraphFrame->width())
        {
            break;
        }
    }
    ui->GraphFrame->update();
}
