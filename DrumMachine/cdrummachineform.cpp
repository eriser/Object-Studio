#include "cdrummachineform.h"
#include "ui_cdrummachineform.h"
#include "cinsertpatternform.h"
#include "crepeatform.h"

CDrumMachineForm::CDrumMachineForm(IDevice* Device, QWidget *parent) :
    CSoftSynthsForm(Device,true,parent),
    ui(new Ui::CDrumMachineForm)
{
    ui->setupUi(this);
    ui->InstrList->setDrawBase(false);
    PlayListMenu=new QMenu(this);
    PlayListMenu->addAction("Add...",this,SLOT(MenuAddPatternClick()));
    PlayListMenu->addAction("Remove",this,SLOT(MenuRemovePatternClick()));
    PlayListMenu->addAction("Edit...",this,SLOT(MenuEditPatternClick()));

    connect(ui->AddPattern,SIGNAL(clicked()),this,SLOT(AddPatternClick()));
    connect(ui->RemovePattern,SIGNAL(clicked()),this,SLOT(RemovePatternClick()));

    connect(ui->NameEdit,SIGNAL(textEdited(QString)),this,SLOT(ChangeName()));
    connect(ui->TempoSpin,SIGNAL(valueChanged(int)),this,SLOT(ChangeTempo(int)));
    connect(ui->BeatsSpin,SIGNAL(valueChanged(int)),this,SLOT(ChangeNumOfBeats(int)));

    connect(ui->PatternList,SIGNAL(currentRowChanged(int)),this,SLOT(ChangePatternIndex()));
    connect(ui->InstrList,SIGNAL(currentChanged(int)),this,SLOT(ChangeInstrument()));
    connect(ui->PatternPlayList,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(PlayListPopup(QPoint)));
    connect(ui->PatternPlayList,SIGNAL(currentRowChanged(int)),this,SLOT(ChangeListIndex(int)));

    UpdateSounds();
    UpdatePatterns();
    UpdateBeats();
    UpdatePatternlist();
}

CDrumMachineForm::~CDrumMachineForm()
{
    delete ui;
}

void CDrumMachineForm::UpdateSounds()
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    ui->InstrList->blockSignals(true);
    for (int i=ui->InstrList->count()-1;i>-1;i--) ui->InstrList->removeTab(i);
    for (int i=0;i<DrumMachine::SoundCount;i++)
    {
        ui->InstrList->addTab(m_DM->ST[i].Name);
    }
    ui->InstrList->blockSignals(false);
}

void CDrumMachineForm::UpdatePatterns()
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    ui->PatternList->blockSignals(true);
    ui->PatternList->clear();
    ui->RemovePattern->setEnabled(m_DM->Patterns.count()>1);
    for (int i=0;i<m_DM->Patterns.count();i++)
    {
        ui->PatternList->addItem(m_DM->Patterns[i]->Name);
    }
    ui->PatternList->blockSignals(false);
}

void CDrumMachineForm::UpdateBeats()
{
    if (ui->PatternList->currentRow()<0)
    {
        if (ui->PatternList->count()==0) return;
        ui->PatternList->blockSignals(true);
        ui->InstrList->blockSignals(true);
        ui->PatternList->setCurrentRow(0);
        ui->InstrList->setCurrentIndex(0);
        ui->PatternList->blockSignals(false);
        ui->InstrList->blockSignals(false);
    }
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    ui->BeatsSpin->blockSignals(true);
    ui->TempoSpin->blockSignals(true);
    ui->NameEdit->blockSignals(true);
    CBeatFrame* Beat;
    PatternType* CP=m_DM->Patterns[ui->PatternList->currentRow()];
    for (int i=0;i<m_Beats.count();i++)
    {
        m_Beats[i]->hide();
    }
    ui->BeatsSpin->setValue(CP->NumOfBeats());
    ui->NameEdit->setText(CP->Name);
    ui->TempoSpin->setValue(CP->Tempo);
    for (int i=0;i<CP->NumOfBeats();i++)
    {
        if (i>=m_Beats.count())
        {
            Beat=new CBeatFrame(this);
            Beat->hide();
            m_Beats.append(Beat);
            ui->BeatFrameLayout->addWidget(Beat);
        }
        Beat=m_Beats[i];
        Beat->Init(CP->Beat(i),i,ui->InstrList->currentIndex(),false,false,true);
        Beat->show();
    }
    ui->BeatsSpin->blockSignals(false);
    ui->TempoSpin->blockSignals(false);
    ui->NameEdit->blockSignals(false);
}

void CDrumMachineForm::UpdatePatternlist()
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    ui->PatternPlayList->blockSignals(true);
    ui->PatternPlayList->clear();
    for (int i=0;i<m_DM->PatternsInList.count();i++)
    {
        PatternListType* PL=m_DM->PatternsInList[i];
        PatternType* P=PL->Pattern;
        if (PL->Repeats>0)
        {
            ui->PatternPlayList->addItem(P->Name + " " + QString::number(PL->Repeats) + "x");
        }
        else
        {
            ui->PatternPlayList->addItem(P->Name + " Loop");
        }
    }
    ui->PatternPlayList->blockSignals(false);
}

void CDrumMachineForm::RemovePatternInList(int Index)
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    if (m_DM->PatternsInList.count()>0)
    {
        PatternListType* PLI=m_DM->PatternsInList[Index];
        m_DM->PatternsInList.removeOne(PLI);
        delete PLI;
        UpdatePatternlist();
    }
}

void CDrumMachineForm::AddPatternToList(int NewIndex, int PatternIndex, int Repeats)
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    PatternListType* PLI=new PatternListType;
    PLI->Pattern=m_DM->Patterns[PatternIndex];
    PLI->Repeats=Repeats;
    if (NewIndex>m_DM->PatternsInList.count())
    {
        NewIndex=m_DM->PatternsInList.count();
    }
    m_DM->PatternsInList.insert(NewIndex,PLI);
    UpdatePatternlist();
}

void CDrumMachineForm::AddPatternClick()
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    PatternType* P=new PatternType("New Pattern",16,7,100,0,0);
    P->Polyphony=7;
    m_DM->Patterns.append(P);
    UpdatePatterns();
    ui->PatternList->setCurrentRow(ui->PatternList->count()-1);
    //TabControl1->TabIndex=m_DM->Patterns->Count-1;
    //TabControl1->OnChange(this);
}
//---------------------------------------------------------------------------

void CDrumMachineForm::RemovePatternClick()
{
    if (ui->PatternList->count()>0)
    {
        int Curr=ui->PatternList->currentRow();
        RemovePattern(Curr);
        UpdatePatterns();
        if (Curr>ui->PatternList->count()-1)
        {
            ui->PatternList->setCurrentRow(ui->PatternList->count()-1);
        }
        else
        {
            ui->PatternList->setCurrentRow(Curr);
        }
    }
}
//---------------------------------------------------------------------------

void CDrumMachineForm::RemovePattern(int Index)
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    for (int i=0;i<m_DM->PatternsInList.count();i++)
    {
        PatternListType* PLI=m_DM->PatternsInList[i];
        PatternType* CP=PLI->Pattern;
        if (m_DM->Patterns.indexOf(CP)==Index)
        {
            RemovePatternInList(i);
        }
    }
    PatternType* P=m_DM->Patterns[Index];
    P->SetNumOfBeats(0);
    m_DM->Patterns.removeOne(P);
    delete P;

}

void CDrumMachineForm::PlayListPopup(QPoint Pos)
{
    PlayListMenu->popup(ui->PatternPlayList->mapToGlobal(Pos));
}

void CDrumMachineForm::MenuAddPatternClick()
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    int PIndex=0;
    if (ui->PatternPlayList->currentRow()>-1)
    {
        PIndex=ui->PatternPlayList->currentRow();
    }
    int NewPattern=0;
    int Repeats=4;
    bool InsertBefore=true;
    CInsertPatternForm d(this);
    d.SelectPattern(m_DM->Patterns,NewPattern,Repeats,InsertBefore);
    if (d.exec() == QDialog::Accepted)
    {
        d.GetValues(NewPattern,Repeats,InsertBefore);
        if (!InsertBefore)
        {
            PIndex++;
        }
        AddPatternToList(PIndex,NewPattern,Repeats);
    }
}
//---------------------------------------------------------------------------

void CDrumMachineForm::MenuRemovePatternClick()
{
    if (ui->PatternPlayList->currentRow()>-1)
    {
        RemovePatternInList(ui->PatternPlayList->currentRow());
    }
}
//---------------------------------------------------------------------------

void CDrumMachineForm::MenuEditPatternClick()
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    if (ui->PatternPlayList->currentRow()>-1)
    {
        PatternListType* PL=m_DM->PatternsInList[ui->PatternPlayList->currentRow()];
        CRepeatForm d(this);
        d.SetRepeats(PL->Repeats);
        if (d.exec()==QDialog::Accepted)
        {
            PL->Repeats=d.GetRepeats();
            UpdatePatternlist();
        }
    }
}

void CDrumMachineForm::ChangePatternIndex()
{
    //CurrentPattern=ui->PatternList->currentRow();
    UpdateBeats();
}
//---------------------------------------------------------------------------

/*
void CDrumMachineForm::FormResize(TObject *Sender)
{
    ListView1->Columns->Items[0]->Width=ListView1->ClientWidth;
}
*/
void CDrumMachineForm::ChangeName()
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    m_DM->Patterns[ui->PatternList->currentRow()]->Name=ui->NameEdit->text();
    UpdatePatterns();
}

void CDrumMachineForm::ChangeInstrument()
{
    //CurrentSound=ui->InstrList->currentRow();
    UpdateBeats();
}
//---------------------------------------------------------------------------


void CDrumMachineForm::ChangeNumOfBeats(int Value)
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    m_DM->Patterns[ui->PatternList->currentRow()]->SetNumOfBeats(Value,100,0,0);
    UpdateBeats();
}
//---------------------------------------------------------------------------

void CDrumMachineForm::ChangeTempo(int Value)
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    m_DM->Patterns[ui->PatternList->currentRow()]->Tempo=Value;
}

void CDrumMachineForm::CustomLoad(const QString &XML)
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    for (int i=m_DM->Patterns.count()-1;i>-1;i--)
    {
        RemovePattern(i);
    }
    m_DM->Patterns.clear();
    m_DM->PatternsInList.clear();
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        QDomLiteElementList XMLPatterns=xml.elementsByTag("Pattern");
        foreach (QDomLiteElement* Pattern, XMLPatterns)
        {
            unsigned int NOB = Pattern->attributeValue("NumOfBeats");
            QString Name = Pattern->attribute("Name");
            PatternType* P=new PatternType(Name,NOB);
            m_DM->Patterns.append(P);
            P->Tempo= Pattern->attributeValue("Tempo");
            P->Polyphony= Pattern->attributeValue("Sounds");
            QDomLiteElementList XMLBeats = Pattern->elementsByTag("Beat");
            for (unsigned int i=0;i<XMLBeats.size();i++)
            {
                QDomLiteElement* Beat=XMLBeats[i];
                if (i < NOB)
                {
                    BeatType* B = P->Beat(i);
                    for (int i1=0;i1<P->Polyphony;i1++)
                    {
                        B->Pitch[i1]=Beat->attributeValue("Pitch"+QString::number(i1));
                        B->Length[i1]=Beat->attributeValue("Length"+QString::number(i1));
                        B->Volume[i1]=Beat->attributeValue("Volume"+QString::number(i1));
                    }
                }
            }
        }
        //ui->PatternList->setCurrentRow(0);
        //ui->InstrList->setCurrentRow(0);
        XMLPatterns = xml.elementsByTag("PatternInList");
        foreach(QDomLiteElement* PatternInList,XMLPatterns)
        {
            int Index = PatternInList->attributeValue("PatternIndex");
            int Repeats = PatternInList->attributeValue("Repeats");
            PatternListType* PLI=new PatternListType();
            PLI->Repeats=Repeats;
            PLI->Pattern=m_DM->Patterns[Index];
            m_DM->PatternsInList.append(PLI);
        }
    }
    UpdatePatterns();
    UpdateBeats();
    UpdatePatternlist();
}

const QString CDrumMachineForm::CustomSave()
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    QDomLiteElement xml("Custom");
    for (int i=0; i<m_DM->Patterns.count(); i++)
    {
        PatternType* P=m_DM->Patterns[i];
        QDomLiteElement* Pattern = xml.appendChild("Pattern");
        Pattern->setAttribute("Name",P->Name);
        Pattern->setAttribute("NumOfBeats",P->NumOfBeats());
        Pattern->setAttribute("Tempo",P->Tempo);
        Pattern->setAttribute("Sounds",P->Polyphony);
        for (int i1=0;i1<P->NumOfBeats();i1++)
        {
            BeatType* B=P->Beat(i1);
            QDomLiteElement* Beat = Pattern->appendChild("Beat");
            for (int i2=0;i2<P->Polyphony;i2++)
            {
                Beat->setAttribute("Pitch"+QString::number(i2),B->Pitch[i2]);
                Beat->setAttribute("Length"+QString::number(i2),B->Length[i2]);
                Beat->setAttribute("Volume"+QString::number(i2),B->Volume[i2]);
            }
        }
    }

    for (int i=0; i<m_DM->PatternsInList.count(); i++)
    {
        PatternListType* PLI=m_DM->PatternsInList[i];
        int Index=m_DM->Patterns.indexOf(PLI->Pattern);
        QDomLiteElement* PatternInList = xml.appendChild("PatternInList");
        PatternInList->setAttribute("PatternIndex",Index);
        PatternInList->setAttribute("Repeats",PLI->Repeats);
    }
    return xml.toString();
}

void CDrumMachineForm::Flash(int Pattern, int Beat)
{
    if (Pattern != ui->PatternPlayList->currentRow()) ui->PatternPlayList->setCurrentRow(Pattern);
    m_Beats[Beat]->Flash();
}

void CDrumMachineForm::ChangeListIndex(int index)
{
    CDrumMachine* m_DM=(CDrumMachine*)m_Device;
    PatternListType* PLI=m_DM->PatternsInList[index];
    for (int i=0;i<ui->PatternList->count();i++)
    {
        QListWidgetItem* l = ui->PatternList->item(i);
        if (l->text()==PLI->Pattern->Name)
        {
            ui->PatternList->setCurrentRow(i);
            break;
        }
    }
}
