#ifndef IWAVEFILE_H
#define IWAVEFILE_H

#include <QtCore>
#include <QFile>
#include "softsynthsdefines.h"

#pragma pack(push,1)

class IWaveFile
{
public:
    enum Endian { BigEndian = QSysInfo::BigEndian, LittleEndian = QSysInfo::LittleEndian };
    IWaveFile();
    virtual const int Channels();
    virtual const int Rate();
    virtual const bool OpenFile(QFile& f);
    virtual const bool Open(BYTE* pSrc, const size_t Size);
    virtual const bool Save(const QString &filename,float*& data,const int Channels,const size_t Length,const unsigned int SampleRate);
    virtual const size_t CreateFloatBuffer(float*& OutBuffer, const int Samplerate);
protected:
    struct chunk
    {
        char        id[4];
        quint32     size;
    };
    const qint16 inline getShort(const qint16 val)
    {
        if (byteOrder==LittleEndian) return qFromLittleEndian<qint16>(val);
        if (byteOrder==BigEndian) return qFromBigEndian<qint16>(val);
        return val;
    }
    const qint32 inline getInt(const qint32 val)
    {
        if (byteOrder==LittleEndian) return qFromLittleEndian<qint32>(val);
        if (byteOrder==BigEndian) return qFromBigEndian<qint32>(val);
        return val;
    }
    const bool findChunk(const char* s, size_t &ptr, BYTE* pSrc, const size_t filesize);

    const int16_t inline MuLaw_Decode(int8_t number);
    const int16_t inline ALaw_Decode(int8_t number);
    const float inline ReadAuMem(const void* pSrc, const bool HalfByte=false);
    void inline DecompressIMAPacket( const uint8_t* pSrc, float* pDst, const int stride );
    const size_t DecompressIMA(const int channels, const uint8_t* pSrc, float *&pDst, const size_t srcSize);

    int channels;
    int frequency;
    Endian byteOrder;
    int AuEncoding;
    BYTE* WaveStart;
    int sampleSize;
    size_t chunkSize;
    /* Define the encoding fields */
    static const int AUDIO_FILE_ENCODING_MULAW_8=1; 	/* 8-bit ISDN u-law */
    static const int AUDIO_FILE_ENCODING_LINEAR_8=2; 	/* 8-bit linear PCM */
    static const int AUDIO_FILE_ENCODING_LINEAR_16=3; 	/* 16-bit linear PCM */
    static const int AUDIO_FILE_ENCODING_LINEAR_24=4; 	/* 24-bit linear PCM */
    static const int AUDIO_FILE_ENCODING_LINEAR_32=5; 	/* 32-bit linear PCM */
    static const int AUDIO_FILE_ENCODING_FLOAT=6; 	/* 32-bit IEEE floating point */
    static const int AUDIO_FILE_ENCODING_DOUBLE=7; 	/* 64-bit IEEE floating point */
    static const int AUDIO_FILE_ENCODING_ADPCM_G721=23; 	/* 4-bit CCITT g.721 ADPCM */
    static const int AUDIO_FILE_ENCODING_ADPCM_G722=24; 	/* CCITT g.722 ADPCM */
    static const int AUDIO_FILE_ENCODING_ADPCM_G723_3=25; 	/* CCITT g.723 3-bit ADPCM */
    static const int AUDIO_FILE_ENCODING_ADPCM_G723_5=26; 	/* CCITT g.723 5-bit ADPCM */
    static const int AUDIO_FILE_ENCODING_ALAW_8=27; 	/* 8-bit ISDN A-law */
};

#pragma pack(pop)

#endif // IWAVEFILE_H
