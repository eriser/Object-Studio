#ifndef SEQUENSERCLASSES_H
#define SEQUENSERCLASSES_H

#include <QtCore>
typedef unsigned char BYTE;

class BeatType
{
public:
    BeatType();
    BeatType(BYTE Length,BYTE Pitch,BYTE Volume);
    BYTE Length[24];
    BYTE Pitch[24];
    BYTE Volume[24];
};

class PatternType
{
public:
        PatternType(const QString& sName,int NumOfBeats);
        PatternType(const QString& sName,int NumOfBeats,int Poly,BYTE Length,BYTE Pitch,BYTE Volume);
        QString Name;
        int NumOfBeats();
        int Tempo;
        int Polyphony;
        BeatType* Beat(int Index);
        void SetNumOfBeats(int NewNumOfBeats);
        void SetNumOfBeats(int NewNumOfBeats,BYTE Length,BYTE Pitch,BYTE Volume);
private:
        QList<BeatType*> Beats;
};

struct PatternListType
{
        PatternType* Pattern;
        int Repeats;
};

#endif // SEQUENSERCLASSES_H
