#ifndef CREPEATFORM_H
#define CREPEATFORM_H

#include <QDialog>

namespace Ui {
    class CRepeatForm;
}

class CRepeatForm : public QDialog
{
    Q_OBJECT

public:
    explicit CRepeatForm(QWidget *parent = 0);
    ~CRepeatForm();
    void SetRepeats(int Repeats);
    int GetRepeats();
private:
    Ui::CRepeatForm *ui;
};

#endif // CREPEATFORM_H
