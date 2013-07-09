#include "cwaverecorderform.h"
#include "ui_cwaverecorderform.h"
#include "cwavefile.h"
#include <QFileDialog>
#include <QScrollBar>

CWaveRecorderForm::CWaveRecorderForm(IDevice* Device, QWidget *parent) :
    CSoftSynthsForm(Device,false,parent),
    ui(new Ui::CWaveRecorderForm)
{
    ui->setupUi(this);
    Popup=new QMenu(this);
    Popup->addAction("Import...",this,SLOT(Import()));
    Popup->addAction("Remove..",this,SLOT(Remove()));
    ui->FileList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->FileList->setDragEnabled(true);
    ui->FileList->setDragDropMode(QListWidget::DragOnly);
    connect(ui->FileList,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(FileMenuPopup(QPoint)));
    connect(ui->AddButton,SIGNAL(clicked()),this,SLOT(Import()));
    connect(ui->RemoveButton,SIGNAL(clicked()),this,SLOT(Remove()));
    connect(ui->WaveLanes,SIGNAL(RequestVisible(int,int)),this,SLOT(EnsureVisible(int,int)));
    //ui->scrollArea->setBackgroundRole(QPalette::Dark);
}

CWaveRecorderForm::~CWaveRecorderForm()
{
    delete ui;
}

void CWaveRecorderForm::EnsureVisible(int x, int y)
{
    ui->scrollArea->ensureVisible(x,y,ui->scrollArea->width()-50);
}

void CWaveRecorderForm::Import()
{
    QFileDialog d(this);
    d.setFileMode(QFileDialog::ExistingFile);
    d.setNameFilter(WaveFile::WaveFilter);
    d.setWindowTitle("Import Wave");
    d.setDirectory(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    if (d.exec() != QFileDialog::Accepted) return;
    QString FN=d.selectedFiles().first();
    if (!FN.isEmpty())
    {
        QListWidgetItem* item=new QListWidgetItem;
        item->setText(QFileInfo(FN).baseName());
        item->setData(34,FN);
        item->setFlags(Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->FileList->addItem(item);
        ui->WaveLanes->Paint();
    }
}

void CWaveRecorderForm::Remove()
{
    int Index=ui->FileList->currentRow();
    if (Index>-1)
    {
        ui->WaveLanes->RemoveFile(ui->FileList->item(Index)->data(34).toString());
        delete ui->FileList->takeItem(Index);
    }
}

void CWaveRecorderForm::FileMenuPopup(QPoint Pos)
{
    Popup->popup(mapToGlobal(Pos));
}

const QString CWaveRecorderForm::CustomSave()
{
    QDomLiteElement xml("Custom");
    xml.appendChildFromString(ui->WaveLanes->Save());
    xml.setAttribute("RulerBeats",ui->WaveLanes->RulerBeats);
    xml.setAttribute("RulerTempo",ui->WaveLanes->RulerTempo);

    xml.setAttribute("Paths",ui->FileList->count());
    for (int i = 0; i < ui->FileList->count(); i++)
    {
        xml.setAttribute("Path" + QString::number(i+1),QDir().relativeFilePath(ui->FileList->item(i)->data(34).toString()));
    }
    return xml.toString();
}

void CWaveRecorderForm::CustomLoad(const QString &XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        QDomLiteElement* Lanes=xml.elementByTag("Lanes");
        if (Lanes)
        {
            ui->WaveLanes->Load(Lanes->toString());

            //LMDSpinEdit2->Value=StrToInt(AnsiString(xml->GetAttribute("RulerBeats")));
            //LMDSpinEdit1->Value=StrToInt(AnsiString(xml->GetAttribute("RulerTempo")));

            ui->FileList->clear();
            int PathCount=xml.attributeValue("Paths");
            for (int i = 0; i < PathCount; i++)
            {
                QString Path=CPresets::ResolveFilename(xml.attribute("Path"+QString::number(i+1)));
                if (!Path.isEmpty())
                {
                    QListWidgetItem* item=new QListWidgetItem;
                    item->setText(QFileInfo(Path).baseName());
                    item->setData(34,Path);
                    item->setFlags(Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                    ui->FileList->addItem(item);
                }
            }
        }
    }
}

void CWaveRecorderForm::Reset()
{
    ui->WaveLanes->Reset();
}

void CWaveRecorderForm::Stop()
{
    ui->WaveLanes->Stop();
}

void CWaveRecorderForm::ModifyBuffers(float *L, float *R)
{
    ui->WaveLanes->ModifyBuffers(L,R);
}
/*
void CWaveRecorderForm::resizeEvent(QResizeEvent *e)
{
    if ((ui->scrollArea->width()>ui->WaveLanes->width()) | (ui->scrollArea->height()>ui->WaveLanes->height()))ui->WaveLanes->setFixedSize(ui->scrollArea->size());
}
*/
