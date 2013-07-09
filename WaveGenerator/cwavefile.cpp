#include "cwavefile.h"
#include "mp3lib/MP3Play.h"

/* ************************* ConvertFloat() *****************************
 * Converts an 80 bit IEEE Standard 754 floating point number to an unsigned
 * long.
 ********************************************************************** */

unsigned int ConvertFloat(BYTE* buffer)
{
   unsigned int mantissa;
   unsigned int last = 0;
   BYTE exp;

   mantissa = qFromBigEndian<unsigned int>(*(unsigned int *)(buffer+2));
   exp = 30 - *(buffer+1);
   while (exp--)
   {
     last = mantissa;
     mantissa >>= 1;
   }
   if (last & 0x00000001) mantissa++;
   return(mantissa);
}

const bool CAiffFile::Open(BYTE* pSrc, const size_t Size)
{
    FormChunk* header=(FormChunk*)pSrc;
    if (!descriptorMatch(header->descriptor.id, "FORM"))
    {
        return false;
    }
    if (descriptorMatch(header->formType,"AIFF"))
    {
        byteOrder=IWaveFile::BigEndian;
    }
    else if (descriptorMatch(header->formType,"AIFC"))
    {
        byteOrder=IWaveFile::BigEndian;
    }
    else
    {
        return false;
    }
    size_t ptr=sizeof(FormChunk);
    if (findChunk("COMM",ptr,pSrc,Size))
    {
        CommonChunk* CommonHeader=(CommonChunk*)(pSrc + ptr);
        if (descriptorMatch(CommonHeader->descriptor.id,"COMM"))
        {
            ptr += sizeof(chunk)+getInt(CommonHeader->descriptor.size);
            qDebug() << ptr << getInt(CommonHeader->descriptor.size) << CommonHeader->descriptor.size;
            if (findChunk("SSND",ptr,pSrc,Size))
            {
                // Read off remaining header information
                SoundDataChunk* dataheader=(SoundDataChunk*)(pSrc + ptr);
                chunkSize=getInt(dataheader->descriptor.size)-8;
                channels = getShort(CommonHeader->numChannels);
                frequency = ConvertFloat(CommonHeader->sampleRate);
                sampleSize = getShort(CommonHeader->sampleSize);
                AiffEncoding=QString(QByteArray(CommonHeader->compressionType,4));
                WaveStart=pSrc + ptr + sizeof(SoundDataChunk);
                if (AiffEncoding.toLower()=="fl32")
                {
                    sampleSize=32;
                    AuEncoding=AUDIO_FILE_ENCODING_FLOAT;
                }
                if (AiffEncoding.toLower()=="fl64")
                {
                    sampleSize=32;
                    AuEncoding=AUDIO_FILE_ENCODING_DOUBLE;
                }
                if (AiffEncoding.toLower()=="alaw")
                {
                    sampleSize=8;
                    AuEncoding=AUDIO_FILE_ENCODING_ALAW_8;
                }
                if (AiffEncoding.toLower()=="ulaw")
                {
                    sampleSize=8;
                    AuEncoding=AUDIO_FILE_ENCODING_MULAW_8;
                }
                return true;
            }
        }
    }
    return false;
}

const size_t CAiffFile::CreateFloatBuffer(float *&OutBuffer, const int Samplerate)
{
    size_t Length=0;
    float* TempBuffer=NULL;
    if (AiffEncoding=="ima4")
    {
        Length = DecompressIMA(channels, WaveStart, TempBuffer, chunkSize);
        WaveStart=(BYTE*)TempBuffer;
        AuEncoding=AUDIO_FILE_ENCODING_FLOAT;
        sampleSize=32;
        chunkSize=Length*channels*sizeof(float);
    }
    if (sampleSize)
    {
        if (AiffEncoding.toLower()=="sowt") byteOrder=LittleEndian;
        Length = IWaveFile::CreateFloatBuffer(OutBuffer,Samplerate);
    }
    if (TempBuffer) delete [] TempBuffer;
    return Length;
}

const bool CAuFile::Open(BYTE* pSrc, const size_t Size)
{
    Audio_filehdr* auHeader=(Audio_filehdr*)pSrc;
    int HeaderSize=0;
    if (qFromLittleEndian<qint32>(auHeader->magic) == AUDIO_FILE_MAGIC)
    {
        byteOrder=IWaveFile::LittleEndian;
    }
    else if (qFromBigEndian<qint32>(auHeader->magic) == AUDIO_FILE_MAGIC)
    {
        byteOrder=IWaveFile::BigEndian;
    }
    else
    {
        return false;
    }
    WaveStart=pSrc + getInt(auHeader->hdr_size);
    channels=getInt(auHeader->channels);
    AuEncoding=getInt(auHeader->encoding);
    frequency=getInt(auHeader->sample_rate);
    HeaderSize=getInt(auHeader->hdr_size);
    switch (AuEncoding)
    {
    case AUDIO_FILE_ENCODING_ADPCM_G723_3:
        sampleSize=3;
        break;
    case AUDIO_FILE_ENCODING_ADPCM_G723_5:
        sampleSize=5;
        break;
    case AUDIO_FILE_ENCODING_ADPCM_G721:
        sampleSize=4;
        break;
    case AUDIO_FILE_ENCODING_MULAW_8:
    case AUDIO_FILE_ENCODING_LINEAR_8:
    case AUDIO_FILE_ENCODING_ALAW_8:
    case AUDIO_FILE_ENCODING_ADPCM_G722:
        sampleSize=8;
        break;
    case AUDIO_FILE_ENCODING_LINEAR_16:
        sampleSize=16;
        break;
    case AUDIO_FILE_ENCODING_LINEAR_24:
        sampleSize=24;
        break;
    case AUDIO_FILE_ENCODING_LINEAR_32:
    case AUDIO_FILE_ENCODING_FLOAT:
        sampleSize=32;
        break;
    case AUDIO_FILE_ENCODING_DOUBLE:
        sampleSize=64;
        break;
    default:
        sampleSize=8;
        break;
    }
    chunkSize=Size-HeaderSize;
    return true;
}

const bool CAuFile::Save(const QString &filename, float *&data, const int Channels, const size_t Length, const unsigned int SampleRate)
{
    Audio_filehdr WH;
    int* WaveBuffer=new int[Length*Channels];
    size_t t=0;
    for (size_t i=0;i<Length;i++)
    {
        for (int c=0;c<Channels;c++)
        {
            WaveBuffer[t++]=qToBigEndian<qint32>(data[i+(Length*c)]*MAXINT);
        }
    }
    size_t PCMSize = Length*Channels;

    WH.magic=qToBigEndian<qint32>(AUDIO_FILE_MAGIC);
    WH.channels=qToBigEndian<qint32>(Channels);
    WH.data_size=qToBigEndian<qint32>(PCMSize*sizeof(int));
    WH.encoding=qToBigEndian<qint32>(AUDIO_FILE_ENCODING_LINEAR_32);
    WH.hdr_size=qToBigEndian<qint32>(sizeof(Audio_filehdr));
    WH.sample_rate=qToBigEndian<qint32>(SampleRate);

    QFile m_RecordFile(filename);
    if (m_RecordFile.open(QIODevice::WriteOnly))
    {
        m_RecordFile.write((char*)&WH,sizeof(WH));
        m_RecordFile.write((char*)WaveBuffer,PCMSize*sizeof(int));
        m_RecordFile.close();
    }
    delete [] WaveBuffer;
    return true;

}

const bool CWavFile::Open(BYTE* pSrc, const size_t Size)
{
    CombinedHeader* header=(CombinedHeader*)pSrc;
    short audioFormat=-1;
    if (descriptorMatch(header->riff.descriptor.id, "RIFF"))
    {
        byteOrder = IWaveFile::LittleEndian;
    }
    else if (descriptorMatch(header->riff.descriptor.id, "RIFX"))
    {
        byteOrder = IWaveFile::BigEndian;
    }
    else
    {
        return false;
    }
    audioFormat=getShort(header->wave.audioFormat);
    if (descriptorMatch(header->riff.type, "WAVE") && descriptorMatch(header->wave.descriptor.id,"fmt ") && (audioFormat == 1 || audioFormat == 0 || audioFormat == 3))
    {
        size_t ptr = getInt(header->wave.descriptor.size)+sizeof(RIFFHeader)+sizeof(chunk);
        if (findChunk("data",ptr,pSrc,Size))
        {
            DATAHeader* dataheader=(DATAHeader*)(pSrc + ptr);
            chunkSize=getInt(dataheader->descriptor.size);
            channels = getShort(header->wave.numChannels);
            frequency = getInt(header->wave.sampleRate);
            sampleSize = getShort(header->wave.bitsPerSample);
            if (audioFormat==3)
            {
                AuEncoding=AUDIO_FILE_ENCODING_FLOAT;
                sampleSize=32;
            }
            if (audioFormat==6)
            {
                AuEncoding=AUDIO_FILE_ENCODING_ALAW_8;
                sampleSize=8;
            }
            if (audioFormat==7)
            {
                AuEncoding=AUDIO_FILE_ENCODING_MULAW_8;
                sampleSize=8;
            }
            WaveStart=pSrc + ptr + sizeof(DATAHeader);
            return true;
        }
    }
    return false;
}

const bool CWavFile::Save(const QString &filename, float *&data, const int Channels, const size_t Length, const unsigned int SampleRate)
{
    CombinedHeader WH;

    short* WaveBuffer=new short[Length*Channels];
    size_t t=0;
    for (size_t i=0;i<Length;i++)
    {
        for (int c=0;c<Channels;c++)
        {
            WaveBuffer[t++]=data[i+(Length*c)]*MAXSHORT;
        }
    }
    size_t PCMSize = Length*Channels;

    setDescriptor(WH.riff.descriptor.id,"RIFF");
    WH.riff.descriptor.size=qToLittleEndian<qint32>(sizeof(WAVEHeader)+sizeof(DATAHeader)+(PCMSize*sizeof(short)));
    setDescriptor(WH.riff.type,"WAVE");
    setDescriptor(WH.wave.descriptor.id,"fmt ");
    WH.wave.descriptor.size=qToLittleEndian<qint32>(sizeof(WAVEHeader)-sizeof(WH.wave.descriptor));

    WH.wave.audioFormat=WAVE_FORMAT_PCM;         // Format category
    WH.wave.numChannels=qToLittleEndian<qint16>(Channels);          // Number of channels
    WH.wave.sampleRate=qToLittleEndian<qint32>(SampleRate);
    WH.wave.bitsPerSample=qToLittleEndian<qint16>(sizeof(short)*8);
    WH.wave.blockAlign=qToLittleEndian<qint16>(Channels*sizeof(short));        // Data block size
    WH.wave.byteRate=qToLittleEndian<qint32>(SampleRate*Channels*sizeof(short));   // For buffer estimation

    DATAHeader WDI;
    setDescriptor(WDI.descriptor.id,"data");
    WDI.descriptor.size=qToLittleEndian<qint32>(PCMSize*sizeof(short));

    QFile m_RecordFile(filename);
    if (m_RecordFile.open(QIODevice::WriteOnly))
    {
        m_RecordFile.write((char*)&WH,sizeof(WH));
        m_RecordFile.write((char*)&WDI,sizeof(WDI));
        m_RecordFile.write((char*)WaveBuffer,PCMSize*sizeof(short));
        m_RecordFile.close();
    }
    delete [] WaveBuffer;
    return true;
}

CWaveFile::CWaveFile()
{
    refCount=0;
    data=NULL;
    m_PushBufferSize=0;
}

const bool CWaveFile::open(const QString &fileName, const unsigned int SampleRate)
{
    m_SampleRate=SampleRate;
    IWaveFile* WF=NULL;
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly))
    {
        if (fileName.toLower().endsWith(".wav")) WF=new CWavFile;
        if (fileName.toLower().endsWith(".au")) WF=new CAuFile;
        if (fileName.toLower().endsWith(".mp3")) WF=new CMP3File;
        if (fileName.toLower().endsWith(".aif") || fileName.toLower().endsWith(".aiff")) WF=new CAiffFile;
        if (WF)
        {
            if (WF->OpenFile(f))
            {
                if (data)
                {
                    delete [] data;
                    data=NULL;
                }
                Length=WF->CreateFloatBuffer(data,m_SampleRate);
                channels=WF->Channels();
                frequency=WF->Rate();
                qDebug() << "Length" << Length << "Channels" << channels << "Frequency" << frequency << "Data" << data;
                delete WF;
                return true;
            }
            delete WF;
        }
    }
    return false;
}

void CWaveFile::pushBuffer(float *&buffer, const size_t Size)
{
    if (m_PushBufferSize==0)
    {
        if (data)
        {
            delete [] data;
            data=NULL;
        }
        Length=0;
    }
    while (Length+Size>m_PushBufferSize)
    {
        size_t OldSize=m_PushBufferSize;
        m_PushBufferSize+=0xFFFF;
        float* t=new float[channels*m_PushBufferSize];
        if (data)
        {
            for (int c=0;c<channels;c++) CopyMemory(t+(c*m_PushBufferSize),data+(c*OldSize),Length*sizeof(float));
            delete [] data;
        }
        data=t;
    }
    for (int c=0;c<channels;c++)
    {
        if (buffer != NULL)
        {
            CopyMemory(data+Length+(m_PushBufferSize*c),buffer+(Size*c),Size*sizeof(float));
        }
        else
        {
            ZeroMemory(data+Length+(m_PushBufferSize*c),Size*sizeof(float));
        }
    }
    Length+=Size;
}

void CWaveFile::startRecording(const int Channels, const unsigned int SampleRate)
{
    m_PushBufferSize=0;
    channels=Channels;
    frequency=SampleRate;
    Length=0;
    m_SampleRate=SampleRate;
    if (data)
    {
        delete [] data;
        data=NULL;
    }
}

void CWaveFile::finishRecording()
{
    if (Length != m_PushBufferSize)
    {
        float* t=new float[channels*Length];
        for (int c=0;c<channels;c++) CopyMemory(t+(Length*c),data+(m_PushBufferSize*c),Length*sizeof(float));
        m_PushBufferSize=Length;
        delete [] data;
        data=t;
    }
}

const bool CWaveFile::save(const QString &fileName)
{
    if (!data) return false;
    finishRecording();
    IWaveFile* WF=NULL;
    if (fileName.toLower().endsWith(".wav")) WF=new CWavFile;
    if (fileName.toLower().endsWith(".au")) WF=new CAuFile;
    if (fileName.toLower().endsWith(".mp3")) WF=new CMP3File;
    if (fileName.toLower().endsWith(".aif") || fileName.toLower().endsWith(".aiff")) WF=new CAiffFile;
    if (WF)
    {
        bool Result = WF->Save(fileName,data,channels,Length,m_SampleRate);
        delete WF;
        return Result;
    }
    return false;
}

CWaveFile::~CWaveFile()
{
    if (data != NULL) delete [] data;
}
