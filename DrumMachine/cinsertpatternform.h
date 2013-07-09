#ifndef CINSERTPATTERNFORM_H
#define CINSERTPATTERNFORM_H

#include <QDialog>
#include "softsynthsclasses.h"
#include "sequenserclasses.h"

namespace Ui {
    class CInsertPatternForm;
}

class CInsertPatternForm : public QDialog
{
    Q_OBJECT

public:
    explicit CInsertPatternForm(QWidget *parent = 0);
    ~CInsertPatternForm();
    void SelectPattern(QList<PatternType*>& Patterns,int PatternIndex,int Repeats,bool InsertBefore);
    void GetValues(int& PatternIndex,int& Repeats,bool& InsertBefore);

private:
    Ui::CInsertPatternForm *ui;
};

#endif // CINSERTPATTERNFORM_H
