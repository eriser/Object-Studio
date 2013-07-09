#include "ckeylayoutcontrol.h"
#include "ui_ckeylayoutcontrol.h"
#include "cwavefile.h"
#include <QFileDialog>

CKeyLayoutControl::CKeyLayoutControl(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CKeyLayoutControl)
{
    ui->setupUi(this);

    ui->LoopTypeCombo->addItems(QStringList() << "Forward" << "Alternate" << "X-fade");

    connect(ui->VolSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateRangeGraph()));
    connect(ui->TuneSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateWaveGraph()));
    connect(ui->XFadeCyclesSpin,SIGNAL(valueChanged(int)),this,SLOT(UpdateWaveGraph()));
    connect(ui->LoopTypeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(UpdateWaveGraph()));

    connect(ui->LowKeyFullEdit,SIGNAL(Changed(int)),this,SLOT(UpdateRangeGraph()));
    connect(ui->HighKeyFullEdit,SIGNAL(Changed(int)),this,SLOT(UpdateRangeGraph()));
    connect(ui->LowKeyXEdit,SIGNAL(Changed(int)),this,SLOT(UpdateRangeGraph()));
    connect(ui->HighKeyXEdit,SIGNAL(Changed(int)),this,SLOT(UpdateRangeGraph()));
    connect(ui->MIDINoteEdit,SIGNAL(Changed(int)),this,SLOT(UpdateWaveGraph()));

    connect(ui->WaveEditWidget,SIGNAL(UpdateHost(CWaveGenerator::LoopParameters)),this,SLOT(UpdateWaveControls(CWaveGenerator::LoopParameters)));
    connect(ui->KeyLayout,SIGNAL(CurrentRangeChanged(CSampleKeyRange::RangeParams)),this,SLOT(UpdateRangeControls(CSampleKeyRange::RangeParams)));
    connect(ui->KeyLayout,SIGNAL(RangeIndexChanged(int)),this,SLOT(SelectRange(int)));
    connect(ui->KeyLayout,SIGNAL(LoadWaveFile()),this,SLOT(OpenFile()));
    connect(ui->KeyLayout,SIGNAL(Add(int,int)),this,SLOT(AddRange(int,int)));
    connect(ui->OpenButton,SIGNAL(clicked()),this,SLOT(OpenFile()));
    connect(ui->DeleteButton,SIGNAL(clicked()),this,SLOT(DeleteRange()));

    connect(ui->LoopTestButton,SIGNAL(toggled(bool)),this,SLOT(ToggleLoopTest(bool)));
    connect(ui->TuneTestButton,SIGNAL(toggled(bool)),this,SLOT(ToggleTuneTest(bool)));

    connect(ui->AutoLoopButton,SIGNAL(clicked()),this,SLOT(Autoloop()));
    connect(ui->TuneLoopButton,SIGNAL(clicked()),this,SLOT(Autotune()));
    connect(ui->PitchDetectButton,SIGNAL(clicked()),this,SLOT(Pitchdetect()));
    connect(ui->FixRangeButton,SIGNAL(clicked()),this,SLOT(FixRange()));


}

void CKeyLayoutControl::Init(CSamplerDevice* D)
{
    m_Sampler=D;
    ui->KeyLayout->Init(D);
    SelectRange(m_Sampler->CurrentRangeIndex);
}

CKeyLayoutControl::~CKeyLayoutControl()
{
    delete ui;
}

void CKeyLayoutControl::AddRange(int Upper, int Lower)
{
    m_Sampler->AddRange(QString(),Upper,Lower);
    SelectRange(m_Sampler->RangeCount()-1);
    Update();
}

void CKeyLayoutControl::DoUpdateHost()
{
    emit UpdateHost(false);
}

void CKeyLayoutControl::UpdateRangeGraph()
{
    CSampleKeyRange::RangeParams RP;
    RP.Volume=ui->VolSpin->value();
    if (ui->HighKeyFullEdit->value() > ui->HighKeyXEdit->value()) ui->HighKeyXEdit->setValue(ui->HighKeyFullEdit->value());
    if (ui->LowKeyFullEdit->value() < ui->LowKeyXEdit->value()) ui->LowKeyXEdit->setValue(ui->LowKeyFullEdit->value());
    RP.LowerTop=ui->LowKeyFullEdit->value()-1;
    RP.UpperTop=ui->HighKeyFullEdit->value();
    RP.LowerZero=ui->LowKeyXEdit->value()-1;
    RP.UpperZero=ui->HighKeyXEdit->value();

    m_Sampler->setRangeParams(RP);
    ui->KeyLayout->Draw();
}

void CKeyLayoutControl::UpdateWaveGraph()
{
    CWaveGenerator::LoopParameters LP=m_Sampler->LoopParams();
    LP.Tune=ui->TuneSpin->value();
    LP.LoopType=(CWaveGenerator::LoopTypeEnum)ui->LoopTypeCombo->currentIndex();
    LP.XFade=ui->XFadeCyclesSpin->value();
    LP.MIDINote=ui->MIDINoteEdit->value();
    m_Sampler->setLoopParams(LP);
}

void CKeyLayoutControl::UpdateWaveControls(CWaveGenerator::LoopParameters LP)
{
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(true);
    ui->MIDINoteEdit->setValue(LP.MIDINote);
    ui->TuneSpin->setValue(LP.Tune);
    ui->LoopTypeCombo->setCurrentIndex((int)LP.LoopType);
    ui->XFadeCyclesSpin->setValue(LP.XFade);
    m_Sampler->setLoopParams(LP);
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(false);
}

void CKeyLayoutControl::UpdateRangeControls(CSampleKeyRange::RangeParams RP)
{
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(true);
    ui->VolSpin->setValue(RP.Volume);

    //LowerTop
    ui->LowKeyFullEdit->setMaximum(RP.UpperTop);
    ui->LowKeyFullEdit->setValue(RP.LowerTop+1);
    //UpperTop
    ui->HighKeyFullEdit->setMinimum(RP.LowerTop+1);
    ui->HighKeyFullEdit->setValue(RP.UpperTop);
    //LowerZero
    ui->LowKeyXEdit->setMaximum(RP.UpperTop);
    ui->LowKeyXEdit->setValue(RP.LowerZero+1);
    //UpperZero
    ui->HighKeyXEdit->setMinimum(RP.LowerTop+1);
    ui->HighKeyXEdit->setValue(RP.UpperZero);

    m_Sampler->setRangeParams(RP);
    foreach(QWidget* w,findChildren<QWidget*>()) w->blockSignals(false);
}

void CKeyLayoutControl::Update()
{
    /*
    if (!SelectedRange)
    {
        InitSelected();
    }
    if (!SelectedRange)
    {
        ui->groupBox->setTitle("No Range");
        EnableEdit(false);
        WEInit(NULL);
        return;
    }
    EnableEdit(true);
    */
    if (m_Sampler->RangeCount())
    {
        UpdateWaveControls(m_Sampler->LoopParams());
        UpdateRangeControls(m_Sampler->RangeParams());
        WEInit();
        UpdateRangeGraph();
    }
    ui->DeleteButton->setEnabled(m_Sampler->RangeCount() > 1);
}

void inline CKeyLayoutControl::WEInit()
{
    CSampleKeyRange* KR=m_Sampler->CurrentRange();
    ui->WaveEditWidget->Init(&KR->WG,KR->WG.LP,true);
    ui->FilenameEdit->blockSignals(true);
    ui->FilenameEdit->setText(KR->FileName);
    ui->FilenameEdit->blockSignals(false);
}

void CKeyLayoutControl::OpenFile()
{
    QString FN=QFileDialog::getOpenFileName(this,"Open",QStandardPaths::writableLocation(QStandardPaths::MusicLocation),WaveFile::WaveFilter);
    if (FN.isEmpty()) return;
    emit ReleaseLoop();
    if (!m_Sampler->RangeCount())
    {
        m_Sampler->AddRange(FN,127,1);
        SelectRange(m_Sampler->RangeCount()-1);
        return;
    }
    m_Sampler->ChangePath(FN);
    Update();
}

void CKeyLayoutControl::SelectRange(int RangeIndex)
{
    emit ReleaseLoop();
    if (m_Sampler->RangeCount())
    {
        m_Sampler->CurrentRangeIndex=RangeIndex;
    }
    Update();
}

void CKeyLayoutControl::DeleteRange()
{
    emit ReleaseLoop();
    if (m_Sampler->RangeCount() > 1)
    {
        m_Sampler->RemoveRange();
        int R=m_Sampler->CurrentRangeIndex-1;
        if (R < 0) R=0;
        SelectRange(R);
    }
}

void CKeyLayoutControl::ReleaseLoop()
{
    ui->LoopTestButton->setChecked(false);
    ui->TuneTestButton->setChecked(false);
}

void CKeyLayoutControl::ToggleTuneTest(bool ButtonDown)
{
    if (ButtonDown)
    {
        m_Sampler->TestMode=CSamplerDevice::st_TuneTest;
        m_Sampler->Looping=false;
        ui->LoopTestButton->setEnabled(false);
    }
    else
    {
        m_Sampler->TestMode=CSamplerDevice::st_NoTest;
        m_Sampler->Looping=false;
        ui->LoopTestButton->setEnabled(true);
    }
}

void CKeyLayoutControl::ToggleLoopTest(bool ButtonDown)
{
    if (ButtonDown)
    {
        m_Sampler->TestMode=CSamplerDevice::st_LoopTest;
        m_Sampler->Looping=false;
        ui->TuneTestButton->setEnabled(false);
    }
    else
    {
        m_Sampler->TestMode=CSamplerDevice::st_NoTest;
        m_Sampler->Looping=false;
        ui->TuneTestButton->setEnabled(true);
    }
}

void CKeyLayoutControl::Autoloop()
{
    CSampleKeyRange* KR=m_Sampler->CurrentRange();
    KR->AutoLoop(ui->LoopCyclesSpin->value());
    m_Sampler->setLoopParams(KR->WG.LP);
    Update();
}

void CKeyLayoutControl::Autotune()
{
    CSampleKeyRange* KR=m_Sampler->CurrentRange();
    KR->AutoTune();
    m_Sampler->setLoopParams(KR->WG.LP);
    Update();
}

void CKeyLayoutControl::Pitchdetect()
{
    CSampleKeyRange* KR=m_Sampler->CurrentRange();
    KR->PitchDetect(ui->PitchDetectSpin->value());
    m_Sampler->setLoopParams(KR->WG.LP);
    Update();
}

void CKeyLayoutControl::FixRange()
{
    CSampleKeyRange* KR=m_Sampler->CurrentRange();
    KR->AutoFix(ui->LoopCyclesSpin->value(),ui->PitchDetectSpin->value());
    m_Sampler->setLoopParams(KR->WG.LP);
    Update();
}

int CKeyLayoutControl::Tune_A440()
{
    return ui->PitchDetectSpin->value();
}

int CKeyLayoutControl::LoopCycles()
{
    return ui->LoopCyclesSpin->value();
}
