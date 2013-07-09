#ifndef CMASTERWIDGET_H
#define CMASTERWIDGET_H

#include <QFrame>
#include <cstereomixer.h>
#include <softsynthsclasses.h>
#include <QDial>
#include <QToolButton>

namespace Ui {
class CMasterWidget;
}

class CMasterWidget : public QFrame
{
    Q_OBJECT
    
public:
    explicit CMasterWidget(QWidget *parent = 0);
    ~CMasterWidget();
    void Init(CStereoMixer* mx, QList<IDevice*>* effects=NULL);
    void checkPeak();
    void checkEffects();
    const QString Save();
    void Load(const QString& XML);
    void setSoloChannel(int channel);
    void resetPeak();
private:
    Ui::CMasterWidget *ui;
    CStereoMixer* m_Mx;
    QList<IDevice*>* m_Fx;
    QList<QToolButton*> m_Buttons;
    QStringList m_Names;
    QSignalMapper* mapper;
    QSignalMapper* dialMapper;
    QList<QDial*> dials;
private slots:
    void setVolL(int vol);
    void setVolR(int vol);
    void showEffect(int eff);
    void effectVol(int eff);
};

#endif // CMASTERWIDGET_H
