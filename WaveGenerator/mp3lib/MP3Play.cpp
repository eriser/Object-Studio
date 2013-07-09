//
// MPEG1 Audio Layer-3 Class
// Copyright(C) 2000 by TOx2RO / O2 Software All rights reserved.
//
// MP3Play.cpp
//

#include "MP3Play.h"
#include <cwavefile.h>

CMP3File::CMP3File()
{
    Init();
}

CMP3File::~CMP3File()
{
    if(State != Closed) { Close(); }
}

void CMP3File::Init()
{
    Bitrate			= 0;
    Channel			= 0;
    Buffer[0]		= NULL;
    Buffer[1]		= NULL;
    BufferingNumber = MaxBufferingNumber;
    sw				= 0;
    State			= Closed;
    FileSize		= 0;
    pSrcFile		= NULL;
    pFrameData		= NULL;
    FrameSize		= 0;
    Frequency		= 0;
    NowPos			= 0;
    PartitionNumber	= 0;
    Partition		= 0;
    startSec		= 0;
    endSec			= 0;
    memset(&PCMwf,0,sizeof(WAVEFORMATEX));
    memset(&PCMwh[0],0,sizeof(WAVEHDR));
    memset(&PCMwh[1],0,sizeof(WAVEHDR));
    memset(&MP3DI,0,sizeof(MPEG_DECODE_INFO));
    memset(&MP3DP[0],0,sizeof(MPEG_DECODE_PARAM));
    memset(&MP3DP[1],0,sizeof(MPEG_DECODE_PARAM));
}

const bool CMP3File::Open(unsigned char* pSrc, const int Size)
{
    pSrcFile=pSrc;
    FileSize = Size;//GetFileSize(FileHandle,NULL);
    if(!memcmp(&pSrcFile[0],"RIFF",4))
    {
        if(!memcmp(&pSrcFile[8],"WAVEfmt",7))
        {
            if(!memcmp(&pSrcFile[50],"fact",4)) { pFrameData = pSrcFile + 70; }
            if(!memcmp(&pSrcFile[52],"fact",4)) { pFrameData = pSrcFile + 72; }
            if(!memcmp(&pSrcFile[50],"data",4)) { pFrameData = pSrcFile + 58; }
        }
        if(!memcmp(&pSrcFile[8],"RMP3",4))
        {
            if(!memcmp(&pSrcFile[12],"data",4)) { pFrameData = pSrcFile + 16; }
        }
    }
    else if (!memcmp(&pSrcFile[0],"TAG+",4))
    {
        pFrameData=pSrcFile+227;
    }
    else if (!memcmp(&pSrcFile[0],"TAG",3))
    {
        pFrameData=pSrcFile+128;
    }
    else if (!memcmp(&pSrcFile[0],"ID3",3))
    {

        //const int supportedVersion = 3;

        int size = ( pSrcFile[9] & 0x7F       ) |
                ((pSrcFile[8] & 0x7F) << 7 ) |
                ((pSrcFile[7] & 0x7F) << 14) |
                ((pSrcFile[6] & 0x7F) << 21);
        if (pSrcFile[5] & (1 << 6))         //flags
        {
            long lNet=*(long*)pSrcFile+16;
            size+= ntohl(lNet)+6;
        }
        int xSize=size;//StrToSize(pSrcFile+6);

        if (pSrcFile[xSize+10]==0xFF)
        {
            pFrameData=pSrcFile+xSize+10;
        }
        else
        {
            //int maxsize=GetFileSize(FileHandle,NULL);
            for (int i = xSize+10; i <Size-1; i++)
            {
                if (pSrcFile[i]==0xFF)
                {
                    if (pSrcFile[i+1]==0xFB)
                    {
                        pFrameData=pSrcFile+i;
                        break;
                    }
                }
            }
        }

    }
    else
    {
        pFrameData = pSrcFile;
    }

    FileSize -= (unsigned int)(pFrameData - pSrcFile);
    if((pSrcFile[FileSize - 128] == 'T') && (pSrcFile[FileSize - 127] == 'A') && (pSrcFile[FileSize - 126] == 'G'))
    {
        FileSize -= 128;
    }

    int	aBitrate[2][16] = {
        { 0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0 },
        { 0, 8,16,24,32,64,80,56, 64,128,160,112,128,256,320,0 }
    };

    int aFrequency[2][4] = {
        { 44100,48000,32000,0 },
        { 22050,24000,16000,0 }
    };

    MpegSpec	= ((pFrameData[1] & 8) == 0) ? 1 : 0;
    Bitrate		= aBitrate[ MpegSpec ][ pFrameData[2] >> 4 ];
    Frequency	= aFrequency[ MpegSpec ][ (pFrameData[2] >> 2) & 3 ];
    Channel		= (pFrameData[3] >> 6 == 3) ? 1 : 2;
    FrameSize	= 144000 * Bitrate / Frequency;

    // PCM
    PCMwf.wFormatTag			= WAVE_FORMAT_PCM;
    PCMwf.nChannels				= Channel;
    PCMwf.nSamplesPerSec		= Frequency;
    PCMwf.nAvgBytesPerSec		= Frequency * Channel * 2;
    PCMwf.nBlockAlign			= Channel * 2;
    PCMwf.wBitsPerSample		= 16;

    mp3DecodeInit();
    mp3GetDecodeInfo(pFrameData,FrameSize,&MP3DI,1);
    mp3DecodeStart(pFrameData,FileSize);

    Buffer[0] = new float[MP3DI.outputSize * MaxBufferingNumber];
    Buffer[1] = new float[MP3DI.outputSize * MaxBufferingNumber];
    if(!Buffer[0]) { return false; }
    if(!Buffer[1]) { return false; }
    State = Opened;
    return true;
}

const unsigned int CMP3File::Close()
{
    if(State == Closed) { return NoError; }

    if(Buffer[0]) { delete [] Buffer[0]; }
    if(Buffer[1]) { delete [] Buffer[1]; }

    Init();

    State = Closed;

    return NoError;
}

const unsigned int CMP3File::PCMBuffer(short*& OutBuffer)
{
    if(State != Opened) { return NoError; }

    QBuffer Mem;
    Mem.open(QIODevice::WriteOnly);

    sw = 0;
    Partition = 0;

    State = Playing;

    startSec = 0; endSec = (FileSize / Bitrate * 8);

    BufferingNumber = MaxBufferingNumber;
    if(BufferingNumber > (msec2frame(endSec) - msec2frame(startSec)) + 1) { BufferingNumber = msec2frame(endSec) - msec2frame(startSec) + 1; }

    PartitionNumber = (((msec2frame(endSec) - msec2frame(startSec)) + 1) / BufferingNumber);
    if(((msec2frame(endSec) - msec2frame(startSec)) + 1) % BufferingNumber == 0) { PartitionNumber--; }

    memset(&MP3DP[sw],0,sizeof(MPEG_DECODE_PARAM));

    unsigned int MemPtr=0;

    for (unsigned int i = 0 ; i < BufferingNumber ; i++)
    {
        mp3GetDecodeInfo(pFrameData + NowPos,FrameSize,&MP3DI,1);
        MP3DP[sw].header			=	MP3DI.header;
        MP3DP[sw].bitRate			=	MP3DI.bitRate;
        MP3DP[sw].inputBuf			=	pFrameData + NowPos;
        MP3DP[sw].inputSize			=	MP3DI.header.padding ? MP3DI.maxInputSize : MP3DI.minInputSize;
        MP3DP[sw].outputBuf			=	(unsigned char*)Buffer[sw] + (MP3DI.outputSize * i);
        MP3DP[sw].outputSize		=	MP3DI.outputSize;
        mp3DecodeFrame(&MP3DP[sw]);

        NowPos += MP3DI.header.padding ? MP3DI.maxInputSize : MP3DI.minInputSize;
    }

    memset(&PCMwh[sw],0,sizeof(WAVEHDR));
    PCMwh[sw].lpData			= (char*)Buffer[sw] + (msec2bytes(startSec) - (msec2frame(startSec) * MP3DI.outputSize));
    PCMwh[sw].dwBufferLength	= MP3DI.outputSize * BufferingNumber - (msec2bytes(startSec) - (msec2frame(startSec) * MP3DI.outputSize));

    Mem.write(PCMwh[sw].lpData,PCMwh[sw].dwBufferLength);
    MemPtr+=PCMwh[sw].dwBufferLength;

    while(1)
    {
        if(Partition == PartitionNumber - 1)
        {
            sw ^= 1;

            Partition++;
            memset(Buffer[sw],0,MP3DI.outputSize * BufferingNumber);

            for(unsigned int i = 0 ; i < msec2frame(endSec) - msec2frame(startSec) - (BufferingNumber * Partition) + 1 ; i++)
            {
                mp3GetDecodeInfo(pFrameData + NowPos,FrameSize,&MP3DI,1);
                MP3DP[sw].header			=	MP3DI.header;
                MP3DP[sw].bitRate			=	MP3DI.bitRate;
                MP3DP[sw].inputBuf			=	pFrameData + NowPos;
                MP3DP[sw].inputSize			=	MP3DI.header.padding ? MP3DI.maxInputSize : MP3DI.minInputSize;
                MP3DP[sw].outputBuf			=	(unsigned char*)Buffer[sw] + (MP3DI.outputSize * i);
                MP3DP[sw].outputSize		=	MP3DI.outputSize;
                mp3DecodeFrame(&MP3DP[sw]);

                NowPos += MP3DI.header.padding ? MP3DI.maxInputSize : MP3DI.minInputSize;
            }

            memset(&PCMwh[sw],0,sizeof(WAVEHDR));
            PCMwh[sw].lpData			= (char*)Buffer[sw];
            PCMwh[sw].dwBufferLength	= msec2bytes(endSec) - msec2bytes(startSec) - MP3DI.outputSize * (BufferingNumber * Partition);

            Mem.write(PCMwh[sw].lpData,PCMwh[sw].dwBufferLength);
            MemPtr+=PCMwh[sw].dwBufferLength;
        }

        if(Partition < PartitionNumber)
        {
            sw ^= 1;

            Partition++;

            for(unsigned int i = 0 ; i < BufferingNumber ; i++)
            {
                mp3GetDecodeInfo(pFrameData + NowPos,FrameSize,&MP3DI,1);
                MP3DP[sw].header			=	MP3DI.header;
                MP3DP[sw].bitRate			=	MP3DI.bitRate;
                MP3DP[sw].inputBuf			=	pFrameData + NowPos;
                MP3DP[sw].inputSize			=	MP3DI.header.padding ? MP3DI.maxInputSize : MP3DI.minInputSize;
                MP3DP[sw].outputBuf			=	(unsigned char*)Buffer[sw] + (MP3DI.outputSize * i);
                MP3DP[sw].outputSize		=	MP3DI.outputSize;
                mp3DecodeFrame(&MP3DP[sw]);

                NowPos += MP3DI.header.padding ? MP3DI.maxInputSize : MP3DI.minInputSize;
            }

            memset(&PCMwh[sw],0,sizeof(WAVEHDR));
            PCMwh[sw].lpData			= (char*)Buffer[sw];
            PCMwh[sw].dwBufferLength	= MP3DI.outputSize * BufferingNumber;

            Mem.write(PCMwh[sw].lpData,PCMwh[sw].dwBufferLength);
            MemPtr+=PCMwh[sw].dwBufferLength;
        }

        if (Partition == PartitionNumber)
        {
            Partition++;
        }
        if (Partition > PartitionNumber)
        {
            State = Opened;
            break;
        }
    }
    OutBuffer=new short[MemPtr/sizeof(short)];
    CopyMemory(OutBuffer,Mem.buffer(),MemPtr);
    Mem.close();
    return MemPtr/sizeof(short);
}
/*
void CMP3File::WaveFile()
{
    CWavFile::CombinedHeader WH;

    short* WaveBuffer;
    unsigned int PCMSize=PCMBuffer(WaveBuffer);

    setDescriptor(WH.riff.descriptor.id,"RIFF");
    setDescriptor(WH.riff.type,"WAVE");
    setDescriptor(WH.wave.descriptor.id,"fmt ");
    WH.wave.descriptor.size=qToLittleEndian<qint32>(sizeof(CWavFile::WAVEHeader)-8);

    WH.wave.audioFormat=PCMwf.wFormatTag;//WAVE_FORMAT_PCM;         // Format category
    WH.wave.numChannels=qToLittleEndian<qint16>(PCMwf.nChannels);          // Number of channels
    WH.wave.sampleRate=qToLittleEndian<qint32>(PCMwf.nSamplesPerSec);
    WH.wave.bitsPerSample=qToLittleEndian<qint16>(PCMwf.wBitsPerSample);
    WH.wave.blockAlign=qToLittleEndian<qint16>((PCMwf.nChannels* PCMwf.wBitsPerSample)/8);        // Data block size
    WH.wave.byteRate=qToLittleEndian<qint32>(PCMwf.nSamplesPerSec*((PCMwf.nChannels* PCMwf.wBitsPerSample)/8));   // For buffer estimation

    CWavFile::DATAHeader WDI;
    setDescriptor(WDI.descriptor.id,"data");

    QFile m_RecordFile("test.wav");
    if (m_RecordFile.open(QIODevice::WriteOnly))
    {
        m_RecordFile.write((char*)&WH,sizeof(WH));
        m_RecordFile.write((char*)&WDI,sizeof(WDI));

        memset(WaveBuffer,0,PCMSize*sizeof(short));

        m_RecordFile.write((char*)WaveBuffer,PCMSize*sizeof(short));

        m_RecordFile.seek(4);
        int FileSize=m_RecordFile.size()-8;
        m_RecordFile.write((char*)&FileSize,sizeof(int));
        m_RecordFile.seek(sizeof(CWavFile::CombinedHeader)+4);
        int Size=m_RecordFile.size()-(sizeof(CWavFile::CombinedHeader)+sizeof(CWavFile::DATAHeader));
        m_RecordFile.write((char*)&Size,sizeof(int));
        m_RecordFile.close();
    }
    delete [] WaveBuffer;
}
*/
const int CMP3File::Channels()
{
    return PCMwf.nChannels;
}

const int CMP3File::Rate()
{
    return PCMwf.nSamplesPerSec;
}

const size_t CMP3File::CreateFloatBuffer(float*& OutBuffer, const int Samplerate)
{
    short* WaveBuffer;
    size_t size=PCMBuffer(WaveBuffer);
    float RateFactor=(float)Samplerate/(float)PCMwf.nSamplesPerSec;
    float PtrInc=PCMwf.nChannels/RateFactor;
    size_t OutSize=size*RateFactor;
    size_t Length=OutSize/PCMwf.nChannels;
    OutBuffer=new float[OutSize];
    for (int c = 0; c < PCMwf.nChannels; c++)
    {
        float Ptr=0;
        size_t ChannelPtr=c*Length;
        for (size_t i=0; i < Length; i++)
        {
            OutBuffer[ChannelPtr+i]=((float)WaveBuffer[c+(((size_t)Ptr/2)*2)])*MAXSHORTMULTIPLY;
            Ptr+=PtrInc;
        }
    }
    delete [] WaveBuffer;
    return Length;
}

const unsigned int CMP3File::Equalize(EQ eq)
{
    int tEQ[10];

    if(State == Closed) { return NoError; }

    tEQ[0] = eq._60;
    tEQ[1] = eq._170;
    tEQ[2] = eq._310;
    tEQ[3] = eq._600;
    tEQ[4] = eq._1k;
    tEQ[5] = eq._3k;
    tEQ[6] = eq._6k;
    tEQ[7] = eq._12k;
    tEQ[8] = eq._14k;
    tEQ[9] = eq._16k;

    mp3SetEqualizer(&tEQ[0]);

    return NoError;
}

inline unsigned int CMP3File::msec2frame(unsigned int msec)
{
    float spf[] = {26.12245f, 24.0f, 36.0f};
    return (unsigned int)(msec / spf[ (pFrameData[2] >> 2) & 3 ]);
}

inline unsigned int CMP3File::frame2msec(unsigned int frame)
{
    return frame * ((FrameSize / Bitrate) * 8);
}

inline unsigned int CMP3File::msec2bytes(unsigned int msec)
{
    return (unsigned int)((msec * (Frequency * Channel * 2 / 1000.0f)) + ((unsigned int)(msec * (Frequency * Channel * 2 / 1000.0f)) % (unsigned int)(Frequency * Channel * 2 / 1000.0f)));
}
