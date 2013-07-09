#include "csequenserform.h"
#include "ui_csequenserform.h"
#include "csequenser.h"
#include "cinsertpatternform.h"
#include "crepeatform.h"

CSequenserForm::CSequenserForm(IDevice* Device, QWidget *parent) :
    CSoftSynthsForm(Device,true,parent),
    ui(new Ui::CSequenserForm)
{
    ui->setupUi(this);
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
    connect(ui->PatternPlayList,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(PlayListPopup(QPoint)));
    connect(ui->PatternPlayList,SIGNAL(currentRowChanged(int)),this,SLOT(ChangeListIndex(int)));

    UpdatePatterns();
    UpdateBeats();
    UpdatePatternlist();
}

CSequenserForm::~CSequenserForm()
{
    delete ui;
}

void CSequenserForm::UpdatePatterns()
{
    CSequenser* m_DM=(CSequenser*)m_Device;
    ui->PatternList->blockSignals(true);
    ui->PatternList->clear();
    ui->RemovePattern->setEnabled(m_DM->Patterns.count()>1);
    for (int i=0;i<m_DM->Patterns.count();i++)
    {
        ui->PatternList->addItem(m_DM->Patterns[i]->Name);
    }
    ui->PatternList->blockSignals(false);
}

void CSequenserForm::UpdateBeats()
{
    if (ui->PatternList->currentRow()<0)
    {
        if (ui->PatternList->count()==0) return;
        ui->PatternList->blockSignals(true);
        ui->PatternList->setCurrentRow(0);
        ui->PatternList->blockSignals(false);
    }
    CSequenser* m_DM=(CSequenser*)m_Device;
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
        Beat->Init(CP->Beat(i),i,0,false,false,false);
        Beat->show();
    }
    ui->BeatsSpin->blockSignals(false);
    ui->TempoSpin->blockSignals(false);
    ui->NameEdit->blockSignals(false);
}

void CSequenserForm::UpdatePatternlist()
{
    CSequenser* m_DM=(CSequenser*)m_Device;
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

void CSequenserForm::RemovePatternInList(int Index)
{
    CSequenser* m_DM=(CSequenser*)m_Device;
    if (m_DM->PatternsInList.count()>0)
    {
        PatternListType* PLI=m_DM->PatternsInList[Index];
        m_DM->PatternsInList.removeOne(PLI);
        delete PLI;
        UpdatePatternlist();
    }
}

void CSequenserForm::AddPatternToList(int NewIndex, int PatternIndex, int Repeats)
{
    CSequenser* m_DM=(CSequenser*)m_Device;
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

void CSequenserForm::AddPatternClick()
{
    CSequenser* m_DM=(CSequenser*)m_Device;
    PatternType* P=new PatternType("New Pattern",16,1,100,0,0);
    m_DM->Patterns.append(P);
    UpdatePatterns();
    ui->PatternList->setCurrentRow(ui->PatternList->count()-1);
    //TabControl1->TabIndex=m_DM->Patterns->Count-1;
    //TabControl1->OnChange(this);
}
//---------------------------------------------------------------------------

void CSequenserForm::RemovePatternClick()
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

void CSequenserForm::RemovePattern(int Index)
{
    CSequenser* m_DM=(CSequenser*)m_Device;
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

void CSequenserForm::PlayListPopup(QPoint Pos)
{
    PlayListMenu->popup(ui->PatternPlayList->mapToGlobal(Pos));
}

void CSequenserForm::MenuAddPatternClick()
{
    CSequenser* m_DM=(CSequenser*)m_Device;
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

void CSequenserForm::MenuRemovePatternClick()
{
    if (ui->PatternPlayList->currentRow()>-1)
    {
        RemovePatternInList(ui->PatternPlayList->currentRow());
    }
}
//---------------------------------------------------------------------------

void CSequenserForm::MenuEditPatternClick()
{
    CSequenser* m_DM=(CSequenser*)m_Device;
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

void CSequenserForm::ChangePatternIndex()
{
    //CurrentPattern=ui->PatternList->currentRow();
    UpdateBeats();
}
//---------------------------------------------------------------------------

/*
void CSequenserForm::FormResize(TObject *Sender)
{
    ListView1->Columns->Items[0]->Width=ListView1->ClientWidth;
}
*/
void CSequenserForm::ChangeName()
{
    CSequenser* m_DM=(CSequenser*)m_Device;
    m_DM->Patterns[ui->PatternList->currentRow()]->Name=ui->NameEdit->text();
    UpdatePatterns();
}

void CSequenserForm::ChangeNumOfBeats(int Value)
{
    CSequenser* m_DM=(CSequenser*)m_Device;
    m_DM->Patterns[ui->PatternList->currentRow()]->SetNumOfBeats(Value,100,0,0);
    UpdateBeats();
}
//---------------------------------------------------------------------------

void CSequenserForm::ChangeTempo(int Value)
{
    CSequenser* m_DM=(CSequenser*)m_Device;
    m_DM->Patterns[ui->PatternList->currentRow()]->Tempo=Value;
}

void CSequenserForm::CustomLoad(const QString &XML)
{
    CSequenser* m_DM=(CSequenser*)m_Device;
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
            //P->Polyphony= Pattern->attributeValue("Sounds");
            QDomLiteElementList XMLBeats = Pattern->elementsByTag("Beat");
            for (unsigned int i=0;i<XMLBeats.size();i++)
            {
                QDomLiteElement* Beat=XMLBeats[i];
                if (i<NOB)
                {
                    BeatType* B = P->Beat(i);
                        B->Pitch[0]=Beat->attributeValue("Pitch");
                        B->Length[0]=Beat->attributeValue("Length");
                        B->Volume[0]=Beat->attributeValue("Volume");
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

const QString CSequenserForm::CustomSave()
{
    CSequenser* m_DM=(CSequenser*)m_Device;
    QDomLiteElement xml("Custom");
    for (int i=0; i<m_DM->Patterns.count(); i++)
    {
        PatternType* P=m_DM->Patterns[i];
        QDomLiteElement* Pattern = xml.appendChild("Pattern");
        Pattern->setAttribute("Name",P->Name);
        Pattern->setAttribute("NumOfBeats",P->NumOfBeats());
        Pattern->setAttribute("Tempo",P->Tempo);
        //Pattern->setAttribute("Sounds",P->Polyphony);
        for (int i1=0;i1<P->NumOfBeats();i1++)
        {
            BeatType* B=P->Beat(i1);
            QDomLiteElement* Beat = Pattern->appendChild("Beat");
                Beat->setAttribute("Pitch",B->Pitch[0]);
                Beat->setAttribute("Length",B->Length[0]);
                Beat->setAttribute("Volume",B->Volume[0]);
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

void CSequenserForm::Flash(int Pattern, int Beat)
{
    if (Pattern != ui->PatternPlayList->currentRow()) ui->PatternPlayList->setCurrentRow(Pattern);
    m_Beats[Beat]->Flash();
}

void CSequenserForm::ChangeListIndex(int index)
{
    CSequenser* m_DM=(CSequenser*)m_Device;
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
