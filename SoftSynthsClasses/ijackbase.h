#ifndef IJACKBASE_H
#define IJACKBASE_H

#include <QColor>

class IJackBase
{
public:
    enum AttachModes
    {Amplitude=1,Pitch=2,Modulation=3,Frequency=4,Trigger=8,Wave=16,Audio=16,Stereo=24,MIDI=32};
    enum Directions
    {In,Out};
    IJackBase(IJackBase::AttachModes tAttachMode, IJackBase::Directions tDirection):AttachMode(tAttachMode),Direction(tDirection){}
    inline const QColor JackColor()
    {
        switch (AttachMode)
        {
        case Amplitude:
            return Qt::green;
        case Frequency:
            return Qt::yellow;
        case Pitch:
        case Modulation:
            return Qt::blue;
        case Trigger:
            return Qt::black;
        case Wave:
            return Qt::darkRed;
        case Stereo:
            return Qt::red;
        case MIDI:
            return Qt::white;
        }
        return Qt::black;
    }
    AttachModes AttachMode;
    Directions Direction;
};

#endif // IJACKBASE_H
