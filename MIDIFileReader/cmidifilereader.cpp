#include "cmidifilereader.h"
#include "../SoftSynthsClasses/softsynthsdefines.h"
/*
const unsigned int inline ReadData(const size_t Size, const void* Pos)
{
    BYTE* TempChar=(BYTE*)Pos;
    switch (Size)
    {
    case 1:
        return TempChar[0];
    case 2:
        return (TempChar[0]<<8) | TempChar[1];
    case 3:
        return (TempChar[0]<<16) | (TempChar[1]<<8) | TempChar[2];
    case 4:
        return (TempChar[0]<<24) | (TempChar[1]<<16) | (TempChar[2]<<8) | TempChar[3];
    }
    return 0;
}
*/
CMIDIFileTrack::CMIDIFileTrack()
{
    m_Length=0;
    m_Time=0;
    m_Time1=0;
    m_Time2=0;
    m_SharpFlat=0;
    m_Key=0;
    m_Message=0;
    m_Tempo=0;
    Counter=0;
    m_MoreMessages=MFRUnknown;
    DataPointer=0;
    TimePointer=0;
    NextPointer=0;
    StartPointer=0;
    NoteCount=0;
}

CMIDIFileTrack::~CMIDIFileTrack()
{
}

const size_t CMIDIFileTrack::Fill(const char *Data, const size_t Pointer)
{
    MIDIFileTrackHeader* header=(MIDIFileTrackHeader*)(Data+Pointer);
    m_Length=qFromBigEndian<qint32>(header->descriptor.size);
    StartPointer=((BYTE*)Data)+Pointer+sizeof(MIDIFileTrackHeader);
    Reset();
    return Pointer+m_Length+sizeof(MIDIFileTrackHeader);
}

const unsigned long CMIDIFileTrack::GetTime()
{
    unsigned long value;
    BYTE c;
    NextPointer=TimePointer;
    if ( (value = *NextPointer++) & 0x80 )
    {
        value &= 0x7F;
        do
        {
            value = (value << 7) + ((c = *NextPointer++) & 0x7F);
        } while (c & 0x80);
    }
    return value;
}

const MessageType CMIDIFileTrack::MoreMessages()
{
    TimePointer=NextPointer;
    BYTE Message=*NextPointer;
    TimePointer++;
    if (Message < 0x80)
    {
        //This i running status
        DataPointer=TimePointer-1;
        if (m_MoreMessages==MFR3Bytes)
        {
            DataSize=2;
            TimePointer++;
        }
        else
        {
            DataSize=1;
        }
        m_Time=GetTime();
        return m_MoreMessages;
    }
    else if ((Message >= 0xC0) & (Message <= 0xDF))
    {
        //2 byte chunk
        DataPointer=TimePointer;
        m_Message=Message;
        DataSize=1;
        TimePointer++;
        m_MoreMessages=MFR2Bytes;
        m_Time=GetTime();
        return MFR2Bytes;
    }
    else if ((Message >= 0x80) & (Message <= 0xEF))
    {
        //3 byte chunk
        DataPointer=TimePointer;
        m_Message=Message;
        DataSize=2;
        TimePointer+=2;
        m_MoreMessages=MFR3Bytes;
        m_Time=GetTime();
        return MFR3Bytes;
    }
    else if ((Message == 0xF0) | (Message == 0xF7))
    {
        //Buffer chunk
        m_Message=Message;
        int i=GetTime();     //length of buffer
        TimePointer=NextPointer;
        DataPointer=TimePointer;
        DataSize=i;
        TimePointer+=i;
        m_MoreMessages=MFRBuffer;
        m_Time=GetTime();
        return MFRBuffer;
    }
    else if (Message==0xFF)
    {
        m_Message=Message;
        DataPointer=TimePointer;
        BYTE m_Data1=DataPointer[0];//ReadByte(TrackData);
        DataSize=DataPointer[1]; //ReadByte(TrackData);//length of message
        DataPointer+=2;
        TimePointer+=2+DataSize;
        if (m_Data1==0x2F)
        {
            m_MoreMessages=MFREndOfTrack;
            m_Time=0;
            NextPointer=TimePointer;
            Finished=true;
            return MFREndOfTrack;
        }
        else if ((m_Data1 <= 7) | (m_Data1==0x7F))
        {
            m_Time=GetTime();
            m_MoreMessages=MFRBuffer;
            return MFRBuffer;
        }
        else
        {
            switch (m_Data1)
            {
            case 0x51:
                //m_Tempo = ReadData(DataSize,DataPointer);
                m_Tempo=qFromBigEndian<qint32>(*(int*)(DataPointer-1)) & 0xFFFFFF;
                m_Time=GetTime();
                m_MoreMessages=MFRTempo;
                return MFRTempo;
            case 0x54:
                m_Time=GetTime();
                m_MoreMessages=MFRSMPTEOffset;
                return MFRSMPTEOffset;
            case 0x58:
                m_Time1=DataPointer[0];//ReadByte(TrackData);
                m_Time2=DataPointer[1];//ReadByte(TrackData);
                m_Time=GetTime();
                m_MoreMessages=MFRTime;
                return MFRTime;
            case 0x59:
                m_SharpFlat=DataPointer[0];//ReadByte(TrackData);
                m_Key=DataPointer[1];//ReadByte(TrackData);
                m_Time=GetTime();
                m_MoreMessages=MFRKey;
                return MFRKey;
            default:
                m_Time=GetTime();
                m_MoreMessages=MFRUnknown;
                return MFRUnknown;
            }
        }
    }
    DataPointer=TimePointer;
    DataSize=0;
    m_MoreMessages=MFRUnknown;
    m_Time=GetTime();
    return MFRUnknown;
}

BYTE* CMIDIFileTrack::Data()
{
    return DataPointer;
}

const short CMIDIFileTrack::GetData()
{
    if (DataIndex<DataSize)
    {
        return DataPointer[DataIndex++];
    }
    else
    {
        return -1;
    }
}

const bool CMIDIFileTrack::MessageReady()
{
    if (m_Time != Counter)
    {
        Counter++;
        return false;
    }
    Counter=1;
    return true;
}

const unsigned long CMIDIFileTrack::Time()
{
    return m_Time;
}

const BYTE CMIDIFileTrack::Message()
{
    DataIndex=0;
    return m_Message;
}

const unsigned int CMIDIFileTrack::Tempo()
{
    return m_Tempo;
}

const BYTE CMIDIFileTrack::Time1()
{
    return m_Time1;
}

const BYTE CMIDIFileTrack::Time2()
{
    return m_Time2;
}

const BYTE CMIDIFileTrack::SharpFlat()
{
    return m_SharpFlat;
}

const BYTE CMIDIFileTrack::Key()
{
    return m_Key;
}

void CMIDIFileTrack::Reset()
{
    DataSize=0;
    TimePointer=StartPointer;
    m_Time=GetTime();
    Counter=0;
    Finished=false;
}

QMap<const char*,MIDIFileMemoryData*> CMIDIFileReader::MIDIFiles=QMap<const char*,MIDIFileMemoryData*>();

CMIDIFileReader::CMIDIFileReader()
{
    m_Pnt=NULL;
    m_FileType=0;
    m_NumOfTracks=0;
    Ticks=240;
}

CMIDIFileReader::~CMIDIFileReader()
{
    qDeleteAll(Tracks);
    MIDIFileMemoryData* Data;
    if (MIDIFiles.contains(m_Pnt))
    {
        Data=MIDIFiles[m_Pnt];
        if (--Data->refcount==0)
        {
            qDebug() << "Delete MIDIFileRef";
            MIDIFiles.remove(m_Pnt);
            delete Data;
        }
    }
}

const bool CMIDIFileReader::Open(const QString& Path)
{
    try
    {
        QFile f(Path);
        if (f.open(QIODevice::ReadOnly))
        {
            bool retVal=OpenPtr((char*)f.map(0,f.size()),f.size());
            f.close();
            return retVal;
        }
        else
        {
            return false;
        }
    }
    catch (...)
    {
        return false;
    }
}

const bool CMIDIFileReader::OpenPtr(const char *Pnt, const size_t Length)
{
    MIDIFileHeader* header=(MIDIFileHeader*)Pnt;
    if (!descriptorMatch(header->descriptor.id,"MThd")) return false;
    MIDIFileMemoryData* Data;
    if (MIDIFiles.contains(m_Pnt))
    {
        Data=MIDIFiles[m_Pnt];
        if (--Data->refcount==0)
        {
            qDebug() << "Delete MIDIFileRef";
            MIDIFiles.remove(m_Pnt);
            delete Data;
        }
    }
    if (!MIDIFiles.contains(Pnt))
    {
        Data=new MIDIFileMemoryData(Pnt,Length);
        MIDIFiles.insert(Pnt,Data);
        qDebug() << "New MIDIFileRef";
    }
    else
    {
        Data=MIDIFiles[Pnt];
    }
    Data->refcount++;
    m_Pnt=(char*)Pnt;
    qDeleteAll(Tracks);
    Tracks.clear();
    size_t len=qFromBigEndian<qint32>(header->descriptor.size);
    m_FileType=qFromBigEndian<qint16>(header->fileType);
    m_NumOfTracks=qFromBigEndian<qint16>(header->numTracks);
    Ticks=qFromBigEndian<qint16>(header->ticks);
    size_t Pointer=(sizeof(chunk) + len);
    for (int i=0;i<m_NumOfTracks;i++)
    {
        CMIDIFileTrack* T=new CMIDIFileTrack();
        Tracks.append(T);
        Pointer=T->Fill(Data->data,Pointer);
        T->Index=i;
    }
    GetDuration();
    return true;
}

const short CMIDIFileReader::TrackCount()
{
    return m_NumOfTracks;
}

const short CMIDIFileReader::FileType()
{
    return m_FileType;
}

void CMIDIFileReader::Reset()
{
    foreach (CMIDIFileTrack* T,Tracks) T->Reset();
}

void CMIDIFileReader::GetDuration()
{
    m_MilliSeconds=0;
    m_Ticks=0;
    if (m_NumOfTracks==0) return;
    double uSCount=0;
    Reset();
    float uSPQ=500000;
    float uSPerTick=uSPQ/(float)Ticks;
    bool TickPlay=true;
    unsigned long SkipTicks=0;
    QList<CMIDIFileTrack*> PlayingTracks(Tracks);
    while (TickPlay)
    {
        uSCount+=1000;
        while (uSCount>=uSPerTick)
        {
            if (SkipTicks==0)
            {
                SkipTicks=1000000000;
                m_Ticks++;
                uSCount-=uSPerTick;
                foreach (CMIDIFileTrack* T, PlayingTracks)
                {
                    if (T->MessageReady())
                    {
                        do
                        {
                            MessageType mt=T->MoreMessages();
                            if (mt==MFRTempo)
                            {
                                uSPQ=T->Tempo();
                                uSPerTick=uSPQ/(float)Ticks;
                            }
                            else if (mt==MFREndOfTrack)
                            {
                                T->Duration=m_Ticks-1;
                                PlayingTracks.removeOne(T);
                                if (PlayingTracks.isEmpty()) TickPlay=false;
                                break;
                            }
                            else if (mt==MFR3Bytes)
                            {
                                if (T->Message() & 0x90)
                                {
                                    if (T->Data()[1] != 0) T->NoteCount++;
                                }
                            }
                        }
                        while (T->Time()==0);
                    }
                    unsigned long TickDiff=T->Time()-T->Counter;
                    if (TickDiff < SkipTicks) SkipTicks=TickDiff;
                }
            }
            else if (SkipTicks != 1000000000)
            {
                foreach (CMIDIFileTrack* T, PlayingTracks) T->Counter+=SkipTicks;
                m_Ticks+=SkipTicks;
                uSCount-=(float)SkipTicks*uSPerTick;
                unsigned long elapsedmSec=qMax(ceil((-uSCount+uSPerTick)*0.001),0.0);
                uSCount+=(1000*elapsedmSec);
                m_MilliSeconds+=elapsedmSec;
                SkipTicks=0;
            }
            else
            {
                TickPlay=false;
                break;
            }
        }
        m_MilliSeconds++;
    }
    Reset();
    qDebug() << m_MilliSeconds << m_Ticks;
}

const unsigned long CMIDIFileReader::MilliSeconds()
{
    return m_MilliSeconds;
}

const unsigned long CMIDIFileReader::Duration(const int Track)
{
    if (Track==-1) return m_Ticks;
    return Tracks[Track]->Duration;
}

const unsigned long CMIDIFileReader::NoteCount(const int Track)
{
    return Tracks[Track]->NoteCount;
}
