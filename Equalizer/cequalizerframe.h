#ifndef CEQUALIZERFRAME_H
#define CEQUALIZERFRAME_H

#include <QFrame>
#include "cequalizer.h"

namespace Ui {
    class CEqualizerFrame;
}

class CEqualizerFrame : public QFrame
{
    Q_OBJECT

public:
    explicit CEqualizerFrame(QWidget *parent = 0);
    ~CEqualizerFrame();
    void Init(CEqualizer* EQ, int BandIndex, int FqMin, int FqMax, int FqDefault);
    const QString Save();
    void Load(const QString& XML);
private:
    Ui::CEqualizerFrame *ui;
    CEqualizer* m_Device;
    int Index;
private slots:
    void VolChanged(int Value);
    void FreqChanged(int Freq);
};

#endif // CEQUALIZERFRAME_H
