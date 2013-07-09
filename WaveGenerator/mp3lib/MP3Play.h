//
// MPEG1 Audio Layer-3 Class Header
// Copyright(C) 2000 by TOx2RO / O2 Software All rights reserved.
//
// MP3Play.h
//

#ifndef __MP3PLAY_H__
#define __MP3PLAY_H__
#include "iwavefile.h"

#define WAVE_FORMAT_PCM                 0x0001 /* Microsoft Corporation */

typedef char* PSTR, *LPSTR;

typedef struct wavehdr_tag {
    LPSTR              lpData;
    unsigned int              dwBufferLength;
    unsigned int              dwBytesRecorded;
    unsigned int*          dwUser;
    unsigned int              dwFlags;
    unsigned int              dwLoops;
    struct wavehdr_tag  *lpNext;
    unsigned int*          reserved;
} WAVEHDR, *LPWAVEHDR;

typedef struct {
    unsigned short  wFormatTag;
    unsigned short  nChannels;
    unsigned int nSamplesPerSec;
    unsigned int nAvgBytesPerSec;
    unsigned short  nBlockAlign;
    unsigned short  wBitsPerSample;
    unsigned short  cbSize;
} WAVEFORMATEX;

#include <QtCore>
#include "include/mp3dec.h"

const unsigned short MaxBufferingNumber = 64;

enum PlayState
{
    Closed,
    Opened,
    Playing,
    Paused
};

enum Error
{
    NoError,
    NoFile,
    NoMP3,
    NoMemory,
    UnknownError
};

// 10-Bands Equalizer
struct EQ
{
    signed char _60;							// 60Hz
    signed char _170;							// 170Hz
    signed char _310;							// 310Hz
    signed char _600;							// 600Hz
    signed char _1k;							// 1kHz
    signed char _3k;							// 3kHz
    signed char _6k;							// 6kHz
    signed char _12k;							// 12kHz
    signed char _14k;							// 14kHz
    signed char _16k;							// 16kHz
};

class CMP3File : public IWaveFile
{
public:
    const int Channels();
    const int Rate();
    CMP3File();
    ~CMP3File();
    const bool Open(unsigned char* pSrc, const int Size);
    const unsigned int Close();
    const unsigned int Equalize(EQ eq);
    const unsigned int PCMBuffer(short*& OutBuffer);
    const size_t CreateFloatBuffer(float*& OutBuffer, const int Samplerate);
    //void WaveFile();
private:
    void Init();
    inline unsigned int msec2frame(unsigned int msec);
    inline unsigned int frame2msec(unsigned int frame);
    inline unsigned int msec2bytes(unsigned int bytes);
private:
    PlayState State;
    unsigned int Partition;
    unsigned char sw;
    unsigned int BufferingNumber;

    unsigned char* pFrameData;
    unsigned int NowPos;
    WAVEFORMATEX PCMwf;

    MPEG_DECODE_PARAM MP3DP[2];
    MPEG_DECODE_INFO MP3DI;

    WAVEHDR PCMwh[2];
    float* Buffer[2];

    unsigned char* pSrcFile;
    size_t FileSize;
    unsigned int PartitionNumber;
    unsigned int startSec;
    unsigned int endSec;

    unsigned short MpegSpec;
    unsigned short Bitrate;
    unsigned short Frequency;
    unsigned short Channel;
    unsigned short FrameSize;
};

#endif
