#ifndef CMIDIBUFFER_H
#define CMIDIBUFFER_H

#include <memory.h>
typedef unsigned char BYTE;

namespace MIDIBuffer
{
const unsigned int MIDIBufferSize=1000;
}

class CMIDIBuffer
{
private:
    BYTE* Buffer;
    unsigned int Size;
    unsigned int PopCount;
    unsigned int PushCount;
    unsigned int ReadCount;
public:
    CMIDIBuffer();
    ~CMIDIBuffer();
    void Push(const BYTE MIDIData);
    const short Pop(void);
    const short Read(void);
    void StartRead(void);
    void Reset(void);
    const bool IsEmpty(void);
    const bool IsRead(void);
    void Append(CMIDIBuffer* MB);
};

#endif // CMIDIBUFFER_H
