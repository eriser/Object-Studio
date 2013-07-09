#ifndef CMIXERWIDGET_H
#define CMIXERWIDGET_H

#include <QWidget>
#include <csf2channelwidget.h>
#include <cmasterwidget.h>
#include <QGridLayout>

namespace Ui {
class CMixerWidget;
}

class CMixerWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CMixerWidget(QWidget *parent = 0);
    ~CMixerWidget();
    CSF2ChannelWidget* appendChannel(int index);
    void removeChannel(int index);
    void showMaster(CStereoMixer *mx, QList<IDevice *>& effects);
    void hideMaster();
    void clear();
    QList<CSF2ChannelWidget*> channels;
    CMasterWidget* master;
    void resetPeak();
public slots:
    void start();
    void stop();
private slots:
    void peak();
    void setSoloChannel(int channel);
private:
    Ui::CMixerWidget *ui;
    QTimer peakTimer;
    QGridLayout* lo;
    QSignalMapper* mapper;
    int timercounter;
};

#endif // CMIXERWIDGET_H
