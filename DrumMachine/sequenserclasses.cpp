#include "sequenserclasses.h"

PatternType::PatternType(const QString& sName,int NumOfBeats)
{
    Name=sName;
    Tempo=100;
    Polyphony=1;
    for (int i=0;i<NumOfBeats;i++)
    {
        Beats.append(new BeatType());
    }
}

PatternType::PatternType(const QString& sName,int NumOfBeats,int Poly,BYTE Length,BYTE Pitch,BYTE Volume)
{
    Name=sName;
    Tempo=100;
    Polyphony=Poly;
    for (int i=0;i<NumOfBeats;i++)
    {
        Beats.append(new BeatType(Length,Pitch,Volume));
    }
}

void PatternType::SetNumOfBeats(int NewNumOfBeats)
{
    for (int i=Beats.count()-1;i>=NewNumOfBeats;i--)
    {
        delete Beats[i];
        Beats.removeAt(i);
    }
    for (int i=Beats.count();i<NewNumOfBeats;i++)
    {
        Beats.append(new BeatType());
    }
}

void PatternType::SetNumOfBeats(int NewNumOfBeats,BYTE Length,BYTE Pitch,BYTE Volume)
{
    for (int i=Beats.count()-1;i>=NewNumOfBeats;i--)
    {
        delete Beats[i];
        Beats.removeAt(i);
    }
    for (int i=Beats.count();i<NewNumOfBeats;i++)
    {
        Beats.append(new BeatType(Length,Pitch,Volume));
    }
}

int PatternType::NumOfBeats()
{
    return Beats.count();
}

BeatType* PatternType::Beat(int Index)
{
    return Beats[Index];
}

BeatType::BeatType()
{
    for (int i=0;i<24;i++)
    {
        Length[i]=100;
        Pitch[i]=0;
        Volume[i]=100;
    }
}

BeatType::BeatType(BYTE Length,BYTE Pitch,BYTE Volume)
{
    for (int i=0;i<24;i++)
    {
        this->Length[i]=Length;
        this->Pitch[i]=Pitch;
        this->Volume[i]=Volume;
    }
}
