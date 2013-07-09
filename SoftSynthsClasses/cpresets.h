#ifndef CPRESETS_H
#define CPRESETS_H

#include <QtCore>

struct PresetsType
{
    int SampleRate;
    int HalfRate;
    int DoubleRate;
    int ModulationRate;
    int ModulationHalfRate;
    float uSPerSample;
    int BufferSize;
    float ResponseFactor;
    QString VSTPath;
};

class CPresets
{
public:
    CPresets();
    ~CPresets();
    static PresetsType Presets;
    static QStringList RecentFiles;
    static const QString ResolveFilename(const QString& Filename);
private:
    static unsigned int instancecount;
    static QMap<QString,QVariant> ReplacementFiles;
};

#endif // CPRESETS_H
