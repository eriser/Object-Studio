#include "cmidibuffer.h"

CMIDIBuffer::CMIDIBuffer()
{
    Buffer=new BYTE[MIDIBuffer::MIDIBufferSize];
    Size=MIDIBuffer::MIDIBufferSize;
    Reset();
}

CMIDIBuffer::~CMIDIBuffer()
{
    delete [] Buffer;
}

void CMIDIBuffer::Push(const BYTE MIDIData)
{
    Buffer[PushCount]=MIDIData;
    PushCount++;
    if (PushCount>=Size)
    {
        BYTE* temp=new BYTE[Size+MIDIBuffer::MIDIBufferSize];
        memcpy(temp,Buffer,Size);
        delete [] Buffer;
        Buffer=temp;
        Size+=MIDIBuffer::MIDIBufferSize;
    }
}

const short CMIDIBuffer::Pop()
{
    if (PushCount != PopCount)
    {
        BYTE P = Buffer[PopCount];
        PopCount++;
        if (PopCount>=Size)
        {
            PopCount=0;
        }
        return P;
    }
    else
    {
        return -1;
    }
}

void CMIDIBuffer::StartRead()
{
    ReadCount=PopCount;
}

const short CMIDIBuffer::Read()
{
    if (PushCount != ReadCount)
    {
        BYTE P = Buffer[ReadCount];
        ReadCount++;
        if (ReadCount>=Size)
        {
            ReadCount=0;
        }
        return P;
    }
    else
    {
        return -1;
    }
}

void CMIDIBuffer::Reset()
{
    if (Size > MIDIBuffer::MIDIBufferSize)
    {
        delete [] Buffer;
        Buffer=new BYTE[MIDIBuffer::MIDIBufferSize];
        Size=MIDIBuffer::MIDIBufferSize;
    }
    PopCount=0;
    PushCount=0;
    ReadCount=0;
}

const bool CMIDIBuffer::IsEmpty()
{
    return (PushCount == PopCount);
}

const bool CMIDIBuffer::IsRead()
{
    return (PushCount == ReadCount);
}

void CMIDIBuffer::Append(CMIDIBuffer *MB)
{
    MB->StartRead();
    short lTemp=MB->Read();
    while (lTemp>-1)
    {
        Push(lTemp);
        lTemp=MB->Read();
    }
}
