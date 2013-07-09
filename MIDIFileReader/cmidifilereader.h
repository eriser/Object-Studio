#ifndef CMIDIFILEREADER_H
#define CMIDIFILEREADER_H

#include <QtCore>

typedef unsigned char BYTE;
typedef unsigned short* PWORD;

enum MessageType
{
    MFREndOfTrack,MFR3Bytes,MFR2Bytes,MFRBuffer,MFRTempo,MFRTime,MFRKey,MFRSMPTEOffset,MFRUnknown
};

#pragma pack(push,1)

class MIDIFileMemoryData
{
public:
    MIDIFileMemoryData(const char* Pnt, const size_t Length)
    {
        data=new char[Length];
        memcpy(data,Pnt,Length);
        refcount=0;
    }
    ~MIDIFileMemoryData()
    {
        delete [] data;
    }
    char* data;
    int refcount;
};

struct chunk
{
    char        id[4];
    quint32     size;
};

struct MIDIFileHeader
{
    chunk descriptor;
    short fileType;
    short numTracks;
    short ticks;
};

struct MIDIFileTrackHeader
{
    chunk descriptor;
};

class CMIDIFileTrack
{
private:
    char* m_TrackName;
    char* m_trackText;
    char* m_CopyRight;
    unsigned int m_Length;
    unsigned long m_Time;
    short m_Time1;
    short m_Time2;
    short m_SharpFlat;
    short m_Key;
    BYTE m_Message;
    unsigned int m_Tempo;
    MessageType m_MoreMessages;
    BYTE* NextPointer;
    BYTE* TimePointer;
    BYTE* StartPointer;
    BYTE* DataPointer;
public:
    int Index;
    bool Finished;
    unsigned long Counter;
    unsigned long Duration;
    unsigned long NoteCount;
    CMIDIFileTrack();
    ~CMIDIFileTrack();
    const size_t Fill(const char* Data, const size_t Pointer);
    const unsigned long inline GetTime();
    const MessageType MoreMessages();
    const short GetData();
    const unsigned long Time();
    const BYTE Message();
    const unsigned int Tempo();
    const BYTE Time1();
    const BYTE Time2();
    const BYTE SharpFlat();
    const BYTE Key();
    BYTE* Data();
    int DataIndex;
    int DataSize;
    void Reset();
    const bool MessageReady();
};

class CMIDIFileReader
{
private:
    short m_FileType;
    short m_NumOfTracks;
    void GetDuration();
    unsigned long m_MilliSeconds;
    unsigned long m_Ticks;
    char* m_Pnt;
public:
    CMIDIFileReader();
    ~CMIDIFileReader();
    QList<CMIDIFileTrack*> Tracks;
    short Ticks;
    const bool Open(const QString& Path);
    const bool OpenPtr(const char* Pnt, const size_t Length);
    const short TrackCount();
    const short FileType();
    const unsigned long MilliSeconds();
    const unsigned long Duration(const int Track=-1);
    const unsigned long NoteCount(const int Track);
    void Reset();
protected:
    static QMap<const char*,MIDIFileMemoryData*> MIDIFiles;
};

#pragma pack(pop)

#endif // CMIDIFILEREADER_H
