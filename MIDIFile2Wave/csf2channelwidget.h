#ifndef CCHANNELWIDGET_H
#define CSF2CHANNELWIDGET_H

#include <QFrame>
#include <cstereomixer.h>
#include <csf2player.h>
#include <QDial>

namespace Ui {
class CSF2ChannelWidget;
}

class CSF2ChannelWidget : public QFrame
{
    Q_OBJECT
    
public:
    explicit CSF2ChannelWidget(QWidget *parent = 0);
    ~CSF2ChannelWidget();
    void Init(CStereoMixerChannel* ch, CSF2Player* SF2, short MIDIChannel, QString Name);
    void loadSF(QString filename);
    void checkPreset();
    void checkPeak();
    void resetPeak();
    void soloButton(bool pressed);
    const QString Save();
    void Load(const QString& XML);
private:
    Ui::CSF2ChannelWidget *ui;
    CStereoMixerChannel* m_Ch;
    CSF2Player* m_SF2;
    short m_MIDIChannel;
    short preset;
    short bank;
    QList<QDial*> Effect;
    QSignalMapper* mapper;
private slots:
    void setVolume(int Vol);
    void setPan(int Pan);
    void setMute(bool Mute);
    void setBypass(bool Bypass);
    void setEffect(int effNumber);
    void loadDialog();
signals:
    void solo();
};

#endif // CCHANNELWIDGET_H
