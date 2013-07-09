#ifndef CSTEREOCHANNELWIDGET_H
#define CSTEREOCHANNELWIDGET_H

#include <QFrame>
#include "cstereomixer.h"
#include <QDial>

namespace Ui {
class CStereoChannelWidget;
}

class CStereoChannelWidget : public QFrame
{
    Q_OBJECT
    
public:
    explicit CStereoChannelWidget(QWidget *parent = 0);
    ~CStereoChannelWidget();
    void Init(CStereoMixerChannel* ch, const QString& Name);
    void checkPeak();
    void resetPeak();
    void soloButton(bool pressed);
    const QString Save();
    void Load(const QString& XML);
private:
    Ui::CStereoChannelWidget *ui;
    CStereoMixerChannel* m_Ch;
    QList<QDial*> Effect;
    QSignalMapper* mapper;
private slots:
    void setVolume(int Vol);
    void setPan(int Pan);
    void setMute(bool Mute);
    void setBypass(bool Bypass);
    void setEffect(int effNumber);
signals:
    void solo();
};

#endif // CSTEREOCHANNELWIDGET_H
