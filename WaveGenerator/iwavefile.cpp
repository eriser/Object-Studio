#include "iwavefile.h"

static int32_t IMA_INDEX_TABLE[16] =
{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};

static int32_t IMA_STEP_TABLE[89] =
{
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350,  22385, 24623, 27086, 29794, 32767
};

void IWaveFile::DecompressIMAPacket( const uint8_t* pSrc, float* pDst, const int stride )
{
    // Read packet header
    uint16_t value  = *(uint16_t*)pSrc;
    uint16_t header = ( value >> 8 ) | ( value << 8 );
    int32_t predictor  = header & 0xff80;
    int32_t step_index = header & 0x007f;
    int32_t step, nibble, diff;

    // Sign extend predictor
    if( predictor & 0x8000 )
        predictor |= 0xffff0000;

    // Skip header
    pSrc += 2;

    // Read 64 nibbles, 2 at a time
    uint32_t byteCount = 32;
    while( byteCount-- )
    {
        // Read 2 nibbles
        uint8_t byte = *pSrc++;

        // Process low nibble
        nibble = byte & 0x0f;
        if( step_index < 0 ) step_index = 0;
        else if( step_index > 88 ) step_index = 88;
        step = IMA_STEP_TABLE[ step_index ];
        step_index += IMA_INDEX_TABLE[ nibble ];
        diff = step >> 3;
        if (nibble & 4) diff += step;
        if (nibble & 2) diff += (step >> 1);
        if (nibble & 1) diff += (step >> 2);
        if (nibble & 8) predictor -= diff;
        else predictor += diff;
        if( predictor < -32768 ) predictor = -32768;
        else if( predictor > 32767 ) predictor = 32767;
        *pDst = (float)predictor*MAXSHORTMULTIPLY;
        pDst += stride;

        // Process high nibble
        nibble = byte >> 4;
        if( step_index < 0 ) step_index = 0;
        else if( step_index > 88 ) step_index = 88;
        step = IMA_STEP_TABLE[ step_index ];
        step_index += IMA_INDEX_TABLE[ nibble ];
        diff = step >> 3;
        if (nibble & 4) diff += step;
        if (nibble & 2) diff += (step >> 1);
        if (nibble & 1) diff += (step >> 2);
        if (nibble & 8) predictor -= diff;
        else predictor += diff;
        if( predictor < -32768 ) predictor = -32768;
        else if( predictor > 32767 ) predictor = 32767;
        *pDst = (float)predictor*MAXSHORTMULTIPLY;
        pDst += stride;
    }
}

const size_t IWaveFile::DecompressIMA(const int channels, const uint8_t* pSrc, float *&pDst, const size_t srcSize)
{
    int packetCount = ((float)srcSize / 34.0) / (float)channels;
    unsigned int Length=packetCount*64.0;
    pDst=new float[Length*channels];
    for (int pck=0;pck<packetCount;pck++)
    {
        for (int c=0;c<channels;c++)
        {
            DecompressIMAPacket( pSrc+(((pck*channels)+c)*34), pDst+((int)(64.0*pck*channels))+c, channels );
        }
    }
    return Length;
}


IWaveFile::IWaveFile()
{
    AuEncoding=0;
    byteOrder=LittleEndian;
    channels=1;
    frequency=44100;
}

const int16_t IWaveFile::MuLaw_Decode(int8_t number)
{
   const uint16_t MULAW_BIAS = 33;
   uint8_t sign = 0, position = 0;
   int16_t decoded = 0;
   number = ~number;
   if (number & 0x80)
   {
      number &= ~(1 << 7);
      sign = -1;
   }
   position = ((number & 0xF0) >> 4) + 5;
   decoded = ((1 << position) | ((number & 0x0F) << (position - 4))
             | (1 << (position - 5))) - MULAW_BIAS;
   return (sign == 0) ? (decoded) : (-(decoded));
}

const int16_t IWaveFile::ALaw_Decode(int8_t number)
{
   uint8_t sign = 0x00;
   uint8_t position = 0;
   int16_t decoded = 0;
   number^=0x55;
   if(number&0x80)
   {
      number&=~(1<<7);
      sign = -1;
   }
   position = ((number & 0xF0) >>4) + 4;
   if(position!=4)
   {
      decoded = ((1<<position)|((number&0x0F)<<(position-4))
                |(1<<(position-5)));
   }
   else
   {
      decoded = (number<<1)|1;
   }
   return (sign==0)?(decoded):(-decoded);
}

const float IWaveFile::ReadAuMem(const void* pSrc, const bool HalfByte)
{
    if (AuEncoding==AUDIO_FILE_ENCODING_DOUBLE) return *((double*)(pSrc));
    if (AuEncoding==AUDIO_FILE_ENCODING_FLOAT) return *((float*)(pSrc));
    if (sampleSize==16) return getShort(*(short*)pSrc)*MAXSHORTMULTIPLY;
    if (sampleSize==32) return getInt(*(int*)pSrc)*MAXINTMULTIPLY;
    if (sampleSize==8)
    {
        if (AuEncoding==AUDIO_FILE_ENCODING_MULAW_8) return MuLaw_Decode(*(int8_t*)pSrc)*MAXSHORTMULTIPLY;
        if (AuEncoding==AUDIO_FILE_ENCODING_ALAW_8) return ALaw_Decode(*(int8_t*)pSrc)*MAXSHORTMULTIPLY;
        return (*(char*)pSrc)*MAXCHARMULTIPLY;
    }
    if (sampleSize==4)
    {
        if (HalfByte) return (((*(char*)pSrc) & 0xF) << 4)*MAXCHARMULTIPLY;
        return ((*(char*)pSrc) & 0xF0)*MAXCHARMULTIPLY;
    }
    if (sampleSize==12)
    {
        if (byteOrder==BigEndian) return qFromBigEndian<qint16>((*(short*)pSrc) & 0xFFF)*MAXSHORTMULTIPLY;
        else return (((*(short*)pSrc) & 0xFFF) << 4)*MAXSHORTMULTIPLY;
    }
    if (sampleSize==24)
    {
        if (byteOrder==BigEndian) return qFromBigEndian<qint16>(*(short*)pSrc)*MAXSHORTMULTIPLY;
        else return (*(short*)pSrc+1)*MAXSHORTMULTIPLY;
    }
    return 0;
}

const size_t IWaveFile::CreateFloatBuffer(float *&OutBuffer, const int Samplerate)
{
    float PointerInc = ((float)sampleSize / 8.0) * channels;
    float RateFactor=(float)frequency/(float)Samplerate;
    size_t Length=(chunkSize/RateFactor)/PointerInc;
    int ByteCount=ceilf(sampleSize/8.0);

    OutBuffer=new float[Length*channels];
    qDebug() << "Samplesize" << sampleSize << "Bytecount" << ByteCount << "channels" << channels << "Frequency" << frequency << "SampleRate" << Samplerate << "chunksize" << chunkSize << "Ratefactor" << RateFactor << "pointerinc" << PointerInc << "Length" << Length << "Auencoding" << AuEncoding << "Byteorder" << byteOrder;

    if (ByteCount==(float)sampleSize/8.0)
    {
        int UnitSize=ByteCount*channels;
        if (fmodf(RateFactor,1)==0)
        {
            qDebug() << "No float!";
            int PtrInc=UnitSize*RateFactor;
            for (int c=0;c<channels;c++)
            {
                size_t Ptr=0;
                size_t ChannelStart=Length*c;
                BYTE* ChannelPtr=WaveStart+(c*ByteCount);
                for (size_t i=ChannelStart;i<ChannelStart+Length;i++) OutBuffer[i]=ReadAuMem(ChannelPtr+(Ptr+=PtrInc));
            }
            return Length;
        }
        qDebug() << "Float RateFactor!";
        for (int c=0;c<channels;c++)
        {
            long double Ptr=0;
            size_t ChannelStart=Length*c;
            BYTE* ChannelPtr=WaveStart+(c*ByteCount);
            for (size_t i=ChannelStart;i<ChannelStart+Length;i++) OutBuffer[i]=ReadAuMem(ChannelPtr+((size_t)(Ptr+=RateFactor)*UnitSize));
        }
        return Length;
    }
    float stridef=RateFactor*PointerInc;
    bool HalfByte=false;
    qDebug() << "Float all!" << stridef << RateFactor;
    for (int c = 0; c < channels; c++)
    {
        size_t ChannelStart=Length*c;
        BYTE* ChannelPtr=WaveStart+(c*ByteCount);
        for (long double i=0;i< Length;i++)
        {
            if (sampleSize==4) HalfByte=(fmodf(i*stridef,1)>=0.5);
            size_t TempPos=(i*stridef);
            if (ByteCount > 1) TempPos = (TempPos / ByteCount) * ByteCount;
            *(OutBuffer+(size_t)i+ChannelStart)=ReadAuMem(ChannelPtr+TempPos,HalfByte);
        }
    }
    return Length;
}

const int IWaveFile::Rate()
{
    return frequency;
}

const int IWaveFile::Channels()
{
    return channels;
}

const bool IWaveFile::OpenFile(QFile &f)
{
    qDebug() << f.fileName();
    return Open(f.map(0,f.size()),f.size());
}

const bool IWaveFile::Open(BYTE* /*pSrc*/, size_t /*Size*/)
{
    return false;
}

const bool IWaveFile::Save(const QString &/*filename*/, float *&/*data*/, const int /*Channels*/, const size_t /*Length*/, const unsigned int /*SampleRate*/)
{
    return false;
}

const bool IWaveFile::findChunk(const char *s, size_t &ptr, BYTE* pSrc, const size_t filesize)
{
    chunk* Chnk=(chunk*)(pSrc + ptr);
    while (!descriptorMatch(Chnk->id, s))
    {
        if (ptr > filesize) return false;
        ptr+=getInt(Chnk->size)+sizeof(chunk);
        Chnk=(chunk*)(pSrc + ptr);
    }
    return true;
}
