#include "cscopecontrol.h"
#include "ui_cscopecontrol.h"

CScopeControl::CScopeControl(QWidget *parent) :
    QCanvas(parent,1),
    ui(new Ui::CScopeControl)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    m_Presets=CPresets::Presets;
    ScopeCounter=0;
    LastX=0;

    m_Frequency = 440;
    m_Amplitude = 100;
}

CScopeControl::~CScopeControl()
{
    delete ui;
}

void inline CScopeControl::CalcHeight()
{
    CurrentHeight=m_Amplitude*(MiddleY)*0.01;
}

int inline CScopeControl::ScaleY(float Y)
{
    float Temp;
    Temp=Y*CurrentHeight;
    Temp+=MiddleY;
    return Temp;
}

void CScopeControl::Process(float* Buffer)
{
    OldCounter=ScopeCounter;
    QCanvasLayer* CanvasLayer=CanvasLayers[0];
    if (!Buffer)
    {
        CanvasLayer->MoveTo(ScopeCounter,MiddleY);
        if (ScopeCounter + BufferDisplaySize > PixWidth)
        {
            CanvasLayer->EraseTransparent(ScopeCounter,0,PixWidth-ScopeCounter,ImgHeight);
            CanvasLayer->LineTo(PixWidth,MiddleY);
            CanvasLayer->EraseTransparent(0,0,(ScopeCounter+BufferDisplaySize)-PixWidth,ImgHeight);
            CanvasLayer->MoveTo(0,MiddleY);
            ScopeCounter=(ScopeCounter+BufferDisplaySize)-PixWidth;
        }
        else
        {
            CanvasLayer->EraseTransparent(ScopeCounter,0,BufferDisplaySize,ImgHeight);
            ScopeCounter+=BufferDisplaySize;
        }
        CanvasLayer->LineTo(ScopeCounter,MiddleY);
        LastY=MiddleY;
        LastX=ScopeCounter;
    }
    else
    {
        if (ScopeCounter + BufferDisplaySize > PixWidth)
        {
            CanvasLayer->EraseTransparent(ScopeCounter,0,PixWidth-ScopeCounter,ImgHeight);
            CanvasLayer->EraseTransparent(0,0,(ScopeCounter+BufferDisplaySize)-PixWidth,ImgHeight);
        }
        else
        {
            CanvasLayer->EraseTransparent(ScopeCounter,0,BufferDisplaySize,ImgHeight);
        }
        CanvasLayer->MoveTo(ScopeCounter,LastY);
        for (int i = 0;i<m_BufferSize;i++)
        {
            if ((int)ScopeCounter != LastX)
            {
                LastY = ScaleY(Buffer[i]);
                CanvasLayer->LineTo(ScopeCounter,LastY);
                LastX = ScopeCounter;
            }
            ScopeCounter+=DisplayFactor;
            if (ScopeCounter > PixWidth)
            {
                ScopeCounter-=PixWidth;
                CanvasLayer->MoveTo(ScopeCounter,LastY);
            }
        }
    }
}

void CScopeControl::resizeEvent(QResizeEvent *event)
{
    //setUpdatesEnabled(false);
    QCanvas::resizeEvent(event);
    PixWidth=width();
    ImgHeight=height();
    PixHeight=ImgHeight-24;
    MiddleY = PixHeight * 0.5;
    CalcHeight();
    LastY=MiddleY;
    SetVol(m_Amplitude);
    SetFreq(m_Frequency,m_BufferSize);
    ClearGradient();
    CanvasLayers[0]->ClearTransparent();
    CanvasLayers[0]->SetPen(Qt::yellow);
    //setUpdatesEnabled(true);
}

void CScopeControl::SetVol(int Vol)
{
    m_Amplitude=Vol;
    CalcHeight();
    ui->VolLabel->setText("A "+QString::number(Vol)+" Percent");
}

void CScopeControl::SetFreq(float Freq,int BufferSize)
{
    m_Frequency=Freq;
    QString Time="T " + QString::number((1.0/m_Frequency)*1000,'f',2) + " US  ";
    QString Frequency="F " +  QString::number(m_Frequency,'f',2) + " HZ";
    ui->FreqLabel->setText(Time + Frequency);

    DisplayFactor = (float)PixWidth/((float)m_Presets.SampleRate/m_Frequency);//20.0 / (float)m_Frequency;
    BufferDisplaySize = (float)BufferSize*DisplayFactor;
    m_BufferSize=BufferSize;
}
