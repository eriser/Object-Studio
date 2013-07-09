#ifndef CMIXERFRAME_H
#define CMIXERFRAME_H

#include <QFrame>
#include "cmixer.h"

namespace Ui {
    class CMixerFrame;
}

class CMixerFrame : public QFrame
{
    Q_OBJECT

public:
    explicit CMixerFrame(QWidget *parent = 0);
    ~CMixerFrame();
    void Init(CMixer* MixerClass,int ChannelIndex);
    int Index;
    const QString Save();
    void Load(const QString& XML);
    void Peak(float Value);
    void Reset();
    void SetSolo(bool Value);
signals:
    void SoloClicked(bool Pressed, int Index);
private:
    Ui::CMixerFrame *ui;
    CMixer* Mixer;
private slots:
    void PanChanged(int Value);
    void EffectChanged(int Value);
    void VolChanged(int Value);
    void MuteButtonClicked(bool Value);
    void SoloButtonClicked(bool Value);
    void BypassButtonClicked(bool Value);
};

#endif // CMIXERFRAME_H
