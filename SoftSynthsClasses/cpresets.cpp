#include "cpresets.h"
#include <QFileDialog>
#include <QDebug>

PresetsType CPresets::Presets=PresetsType();
QStringList CPresets::RecentFiles=QStringList();
unsigned int CPresets::instancecount=0;
QMap<QString,QVariant> CPresets::ReplacementFiles=QMap<QString,QVariant>();

CPresets::CPresets()
{
    if (instancecount++ >0) return;
    qDebug() << "Presets loaded";
    QSettings s("http://www.musiker.nu/objectstudio","ObjectStudio");
    //load presets;
    Presets.SampleRate=s.value("SampleRate",44100).toInt();
    //qDebug() << Presets.SampleRate;
    Presets.HalfRate=Presets.SampleRate/2;
    Presets.ModulationRate=s.value("ModulationRate",256).toInt();
    Presets.BufferSize=s.value("BufferSize",512).toInt();
    Presets.uSPerSample=(float)1000000/(float)Presets.SampleRate;
    Presets.DoubleRate=Presets.SampleRate*2;
    Presets.ModulationHalfRate=Presets.ModulationRate/2;
    Presets.ResponseFactor=(float)((float)Presets.ModulationRate/256.f)*(22050.f/Presets.SampleRate)*5;
    Presets.VSTPath=s.value("VSTPath",QDir("/Library/Audio/Plug-Ins/VST").absolutePath()).toString();
    RecentFiles=s.value("RecentFiles").toStringList();
    ReplacementFiles=s.value("ReplacementFiles").toMap();
}

CPresets::~CPresets()
{
    if (--instancecount >0) return;
    //save presets
    qDebug() << "Presets saved";
    QSettings s("http://www.musiker.nu/objectstudio","ObjectStudio");
    s.setValue("SampleRate",Presets.SampleRate);
    s.setValue("ModulationRate",Presets.ModulationRate);
    s.setValue("BufferSize",Presets.BufferSize);
    s.setValue("VSTPath",Presets.VSTPath);
    s.setValue("RecentFiles",RecentFiles);
    s.setValue("ReplacementFiles",ReplacementFiles);
}

const QString CPresets::ResolveFilename(const QString& Filename)
{
    QFileInfo fi(Filename);
    if (fi.exists()) return Filename;
    QString absoluteFilePath=fi.canonicalFilePath();
    if (absoluteFilePath.isEmpty()) absoluteFilePath=fi.absoluteFilePath();
    if (ReplacementFiles.contains(absoluteFilePath.toLower())) return ReplacementFiles[absoluteFilePath.toLower()].toString();
    QString FN=QFileDialog::getOpenFileName(0,"Replace file "+fi.fileName(),fi.absolutePath());
    if (!FN.isEmpty()) ReplacementFiles.insert(absoluteFilePath.toLower(),FN);
    return FN;
}

static CPresets Presets;
