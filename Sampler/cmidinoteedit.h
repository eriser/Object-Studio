#ifndef CMIDINOTEEDIT_H
#define CMIDINOTEEDIT_H

#include <QWidget>

namespace Ui {
class CMIDINoteEdit;
}

class CMIDINoteEdit : public QWidget
{
    Q_OBJECT   
public:
    explicit CMIDINoteEdit(QWidget *parent = 0);
    ~CMIDINoteEdit();
    int value();
    void setValue(int val);
    void setMaximum(int val);
    void setMinimum(int val);
signals:
    void Changed(int val);
private:
    Ui::CMIDINoteEdit *ui;
private slots:
    void fromText();
    void fromSpin();
};

#endif // CMIDINOTEEDIT_H
