#include "cwavelayers.h"
#include "ui_cwavelayers.h"

CWaveLayers::CWaveLayers(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWaveLayers)
{
    ui->setupUi(this);
    connect(ui->VolSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->TransposeSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->TuneSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->LowVelFullSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->HighVelFullSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->LowVelXSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->HighVelXSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateGraph()));
    connect(ui->ADSRWidget,SIGNAL(Changed(CADSR::ADSRParams)),this,SLOT(UpdateADSRs(CADSR::ADSRParams)));
    connect(ui->LayersControl,SIGNAL(CurrentLayerChanged(CLayer::LayerParams)),this,SLOT(UpdateControls(CLayer::LayerParams)));
    connect(ui->LayersControl,SIGNAL(LayerIndexChanged(int)),this,SLOT(SelectLayer(int)));
    connect(ui->LayersControl,SIGNAL(Add(int,int)),this,SLOT(AddLayer(int,int)));
    //connect(ui->KeyLayoutControl,SIGNAL(ReleaseLoop()),this,SLOT(ReleaseLoop()));
    connect(ui->FixLayerButton,SIGNAL(clicked()),this,SLOT(FixLayer()));
    connect(ui->FixAllButton,SIGNAL(clicked()),this,SLOT(FixAll()));
    connect(ui->PitchLayerButton,SIGNAL(clicked()),this,SLOT(PitchLayer()));
    connect(ui->PitchAllButton,SIGNAL(clicked()),this,SLOT(PitchAll()));

    connect(ui->DeleteLayerButton,SIGNAL(clicked()),this,SLOT(DeleteLayer()));

    MD=false;
    Working=false;
    //Update();
}

CWaveLayers::~CWaveLayers()
{
    qDebug() << "Delete TWaveLayersControl";
    delete ui;
}

void CWaveLayers::Init(CSamplerDevice *Device)
{
    m_Sampler=Device;
    ui->ADSRWidget->Update(m_Sampler->ADSRParams());
    ui->LayersControl->Init(m_Sampler);
    ui->KeyLayoutControl->Init(m_Sampler);
    SelectLayer(m_Sampler->CurrentLayerIndex);
}

void CWaveLayers::SelectLayer(int LayerIndex)
{
    ReleaseLoop();
    m_Sampler->CurrentLayerIndex=LayerIndex;
    if (m_Sampler->CurrentRangeIndex >= m_Sampler->RangeCount()) m_Sampler->CurrentRangeIndex=m_Sampler->RangeCount()-1;
    Update();
}

void CWaveLayers::DeleteLayer()
{
    ReleaseLoop();
    if (m_Sampler->LayerCount() > 1)
    {
        m_Sampler->RemoveLayer();
        int L=m_Sampler->CurrentLayerIndex-1;
        if (L < 0) L=0;
        SelectLayer(L);
    }
}

void CWaveLayers::UpdateGraph()
{
    CLayer::LayerParams LP;
    LP.Volume=ui->VolSpin->value();
    if (ui->HighVelFullSpin->value() > ui->HighVelXSpin->value()) ui->HighVelXSpin->setValue(ui->HighVelFullSpin->value());
    if (ui->LowVelFullSpin->value() < ui->LowVelXSpin->value()) ui->LowVelXSpin->setValue(ui->LowVelFullSpin->value());
    LP.LowerTop=ui->LowVelFullSpin->value();
    LP.UpperTop=ui->HighVelFullSpin->value();
    LP.LowerZero=ui->LowVelXSpin->value();
    LP.UpperZero=ui->HighVelXSpin->value();
    LP.Transpose=ui->TransposeSpin->value();
    LP.Tune=ui->TuneSpin->value();
    m_Sampler->setLayerParams(LP);
    setControlBounds(LP);
    ui->LayersControl->Draw();
}

void CWaveLayers::UpdateControls(CLayer::LayerParams LP)
{
    setControlBounds(LP);
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(true);
    ui->VolSpin->setValue(LP.Volume);
    ui->LowVelFullSpin->setValue(LP.LowerTop);
    ui->HighVelFullSpin->setValue(LP.UpperTop);
    ui->LowVelXSpin->setValue(LP.LowerZero);
    ui->HighVelXSpin->setValue(LP.UpperZero);
    ui->TransposeSpin->setValue(LP.Transpose);
    ui->TuneSpin->setValue(LP.Tune);
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(false);
    m_Sampler->setLayerParams(LP);
}

void CWaveLayers::setControlBounds(CLayer::LayerParams LP)
{
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(true);
    ui->LowVelFullSpin->setMaximum(LP.UpperTop);
    ui->HighVelFullSpin->setMinimum(LP.LowerTop);
    ui->LowVelXSpin->setMaximum(LP.LowerTop);
    ui->HighVelXSpin->setMinimum(LP.UpperTop);
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(false);
}

void CWaveLayers::Update()
{
    ui->ADSRWidget->Update(m_Sampler->ADSRParams());
    ui->LayersControl->Draw();
    UpdateControls(m_Sampler->LayerParams());
    ui->KeyLayoutControl->Update();
    ui->DeleteLayerButton->setEnabled(m_Sampler->LayerCount() > 1);
}

void CWaveLayers::UpdateADSRs(CADSR::ADSRParams ADSRParams)
{
    m_Sampler->setADSRParams(ADSRParams);
}

void CWaveLayers::AddLayer(int Upper, int Lower)
{
    m_Sampler->AddLayer(Upper,Lower);
    m_Sampler->CurrentLayerIndex=m_Sampler->LayerCount()-1;
    Update();
}

void CWaveLayers::ReleaseLoop()
{
    ui->KeyLayoutControl->ReleaseLoop();
}

void CWaveLayers::FixLayer()
{
    for (int i=0;i<m_Sampler->RangeCount();i++)
    {
        CSampleKeyRange* KR=m_Sampler->Range(m_Sampler->CurrentLayerIndex,i);
        KR->AutoFix(ui->KeyLayoutControl->LoopCycles(),ui->KeyLayoutControl->Tune_A440());
        m_Sampler->setLoopParams(KR->WG.LP,m_Sampler->CurrentLayerIndex,i);
    }
    Update();
}

void CWaveLayers::PitchLayer()
{
    for (int i=0;i<m_Sampler->RangeCount();i++)
    {
        CSampleKeyRange* KR=m_Sampler->Range(m_Sampler->CurrentLayerIndex,i);
        KR->PitchDetect(ui->KeyLayoutControl->Tune_A440());
        m_Sampler->setLoopParams(KR->WG.LP,m_Sampler->CurrentLayerIndex,i);
    }
    Update();
}

void CWaveLayers::FixAll()
{
    for (int l=0;l<m_Sampler->LayerCount();l++)
    {
        for (int i=0;i<m_Sampler->RangeCount(l);i++)
        {
            CSampleKeyRange* KR=m_Sampler->Range(l,i);
            KR->AutoFix(ui->KeyLayoutControl->LoopCycles(),ui->KeyLayoutControl->Tune_A440());
            m_Sampler->setLoopParams(KR->WG.LP,l,i);
        }
    }
    Update();
}

void CWaveLayers::PitchAll()
{
    for (int l=0;l<m_Sampler->LayerCount();l++)
    {
        for (int i=0;i<m_Sampler->RangeCount(l);i++)
        {
            CSampleKeyRange* KR=m_Sampler->Range(l,i);
            KR->PitchDetect(ui->KeyLayoutControl->Tune_A440());
            m_Sampler->setLoopParams(KR->WG.LP,l,i);
        }
    }
    Update();
}
