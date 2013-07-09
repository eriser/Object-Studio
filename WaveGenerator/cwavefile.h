#ifndef CWAVEFILE_H
#define CWAVEFILE_H

#include <QtCore/qendian.h>
#include <QFile>
#include <QDebug>
#include "softsynthsdefines.h"
#include "iwavefile.h"

namespace WaveFile
{
const QString WaveFilter("Sound Files (*.wav;*.au;*.mp3;*.aiff)");
}

#pragma pack(push,1)
class CAiffFile : public IWaveFile
{
public:
    const size_t CreateFloatBuffer(float *&OutBuffer, const int Samplerate);
    const bool Open(BYTE* pSrc, const size_t Size);
private:
    struct FormChunk
    {
        chunk descriptor;
        char formType[4];
    };
    struct CommonChunk
    {
      chunk descriptor;
      short numChannels; /* # audio channels */
      unsigned int numSampleFrames; /* # sample frames = samples/channel */
      short sampleSize; /* # bits/sample */
      BYTE sampleRate[10]; /* sample_frames/sec */
      char compressionType[4]; /* compression type ID code */
      char compressionNameLen; /* human-readable compression type name */
      char compressionName[];
    };
    struct SoundDataChunk
    {
        chunk descriptor;
        unsigned int       offset;
        unsigned int       blockSize;
    };
    QString AiffEncoding;
};

class CAuFile : public IWaveFile
{
public:
    const bool Open(BYTE* pSrc, const size_t Size);
    const bool Save(const QString &filename, float *&data, const int Channels, const size_t Length, const unsigned int SampleRate);
private:
    struct Audio_filehdr
    {
        unsigned int	magic;	/* magic number */
        unsigned int	hdr_size;	/* size of this header */
        unsigned int 	data_size;	/* length of data (optional) */
        unsigned int 	encoding;	/* data encoding format */
        unsigned int 	sample_rate;	/* samples per second */
        unsigned int 	channels;	/* number of interleaved channels */
    } ;
    static const int AUDIO_FILE_MAGIC=(int)0x2e736e64;
};

class CWavFile : public IWaveFile
{
public:
    struct RIFFHeader
    {
        chunk       descriptor;     // "RIFF"
        char        type[4];        // "WAVE"
    };
    struct WAVEHeader
    {
        chunk       descriptor;
        quint16     audioFormat;
        quint16     numChannels;
        quint32     sampleRate;
        quint32     byteRate;
        quint16     blockAlign;
        quint16     bitsPerSample;
    };
    struct _PPEAK
    {
      float Value;    // peak value
      quint32 Position;    // sample frame for peak
    };
    struct PEAKheader
    {
       chunk descriptor;
       quint32 Version;    // peak chunk version
       quint32 timestamp;  // UNIX timestamp of creation
      _PPEAK *peak;    // one for each channel
    };
    struct FACTHeader
    {
        chunk descriptor;
        qint32 dwSampleLength;
    };
    struct DATAHeader
    {
        chunk       descriptor;
    };
    struct CombinedHeader
    {
        RIFFHeader  riff;
        WAVEHeader  wave;
    };
    const bool Open(BYTE* pSrc, const size_t Size);
    const bool Save(const QString &filename, float *&data, const int Channels, const size_t Length, const unsigned int SampleRate);
private:
    enum SampleType { Unknown, SignedInt, UnSignedInt, Float };
};

class CWaveFile
{
public:
    CWaveFile();
    ~CWaveFile();
    const bool open(const QString &fileName, const unsigned int SampleRate);
    void startRecording(const int Channels, const unsigned int SampleRate);
    void finishRecording();
    void pushBuffer(float* &buffer, const size_t Size);
    int refCount;
    float* data;
    size_t Length;
    int channels;
    int frequency;
    const bool save(const QString &fileName);
private:
    unsigned int m_SampleRate;
    size_t m_PushBufferSize;
};

#pragma pack(pop)

#endif // CWAVEFILE_H
