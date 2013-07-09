#ifndef CAUDIOBUFFER_H
#define CAUDIOBUFFER_H

#include "ijackbase.h"
#include "softsynthsdefines.h"

class CAudioBuffer
{
protected:
    IJackBase::AttachModes AttachMode;
    int m_BufferSize;
    int m_WaveBufferSize;
public:
    CAudioBuffer(IJackBase::AttachModes a);
    ~CAudioBuffer();
    inline float* ZeroBuffer() {
        ZeroMemory(Buffer,m_WaveBufferSize*sizeof(float));
        return Buffer;
    }
    inline float* FromBuffer(float* b) {
        if (!b) return NULL;
        CopyMemory(Buffer,b,m_WaveBufferSize*sizeof(float));
        return Buffer;
    }
    inline float* FromBuffer(float *b, float factor) {
        if (!b) return NULL;
        if (factor==0) return ZeroBuffer();
        CopyMemory(Buffer,b,m_WaveBufferSize*sizeof(float));
        return Multiply(factor);
    }
    inline float* FromBuffer(float* b, IJackBase::AttachModes a) {
        if (!b) return NULL;
        if (a==AttachMode) return FromBuffer(b);
        if (AttachMode==IJackBase::Wave)
        {
            FromBuffer(b);
            AddBuffer(b+m_BufferSize);
            return Multiply(SQRT2MULTIPLY);
        }
        CopyMemory(Buffer,b,m_BufferSize*sizeof(float));
        float* BufferR=Buffer+m_BufferSize;
        CopyMemory(BufferR,b,m_BufferSize*sizeof(float));
        return Buffer;
    }
    inline float* AddBuffer(float* b) {
        if (b) for (int i=0;i<m_WaveBufferSize;i++) Buffer[i]+=b[i];
        return Buffer;
    }
    inline float* AddBuffer(float *b, float factor) {
        if (b) if (factor > 0) for (int i=0;i<m_WaveBufferSize;i++) Buffer[i]+=b[i]*factor;
        return Buffer;
    }
    inline float* AddBuffer(float* b, IJackBase::AttachModes a) {
        if (b)
        {
            if (a==AttachMode) return AddBuffer(b);
            if (AttachMode==IJackBase::Wave)
            {
                AddBuffer(b,SQRT2MULTIPLY);
                return AddBuffer(b+m_BufferSize,SQRT2MULTIPLY);
            }
            float* BufferR=Buffer+m_BufferSize;
            for (int i=0;i<m_BufferSize;i++)
            {
                Buffer[i]+=b[i];
                BufferR[i]+=b[i];
            }
        }
        return Buffer;
    }
    inline float* Multiply(float factor) {
        if (factor==1) return Buffer;
        if (factor==0) return ZeroBuffer();
        for (int i=0;i<m_WaveBufferSize;i++) Buffer[i]*=factor;
        return Buffer;
    }
    float* Buffer;
};

class CMonoBuffer : public CAudioBuffer
{
public:
    CMonoBuffer() : CAudioBuffer(IJackBase::Wave){}
    inline float* FromStereo(float* b) {
        if (!b) return NULL;
        FromBuffer(b);
        AddBuffer(b+m_BufferSize);
        return Multiply(SQRT2MULTIPLY);
    }
    inline float* AddStereo(float* b) {
        AddBuffer(b,SQRT2MULTIPLY);
        return AddBuffer(b+m_BufferSize,SQRT2MULTIPLY);
    }
};

class CStereoBuffer : public CAudioBuffer
{
public:
    CStereoBuffer() : CAudioBuffer(IJackBase::Stereo), BufferR(Buffer+m_BufferSize){}
    inline float* FromBuffer(float* b) { return CAudioBuffer::FromBuffer(b); }
    inline float* FromBuffer(float* b, float factor) { return CAudioBuffer::FromBuffer(b,factor); }
    inline float* FromBuffer(float* b, float factorL, float factorR) {
        if (!b) return NULL;
        CopyMemory(Buffer,b,m_WaveBufferSize*sizeof(float));
        return Multiply(factorL,factorR);
    }
    inline float* FromMono(float* L, float* R) {
        if (L) CopyMemory(Buffer,L,m_BufferSize*sizeof(float));
        else ZeroMemory(Buffer,m_BufferSize*sizeof(float));
        if (R) CopyMemory(BufferR,R,m_BufferSize*sizeof(float));
        else ZeroMemory(BufferR,m_BufferSize*sizeof(float));
        return Buffer;
    }
    inline float* FromMono(float* b) {
        if (!b) return NULL;
        CopyMemory(Buffer,b,m_BufferSize*sizeof(float));
        CopyMemory(BufferR,b,m_BufferSize*sizeof(float));
        return Buffer;
    }
    inline float* AddBuffer(float *b) { return CAudioBuffer::AddBuffer(b); }
    inline float* AddBuffer(float *b, float factor) { return CAudioBuffer::AddBuffer(b,factor); }
    inline float* AddBuffer(float *b, float factorL, float factorR) {
        AddLeftBuffer(b, factorL);
        AddRightBuffer(b+m_BufferSize, factorR);
        return Buffer;
    }
    inline void AddLeftBuffer(float* b) {
        if (!b) return;
        for (int i=0;i<m_BufferSize;i++) Buffer[i]+=b[i];
    }
    inline void AddLeftBuffer(float* b, float factor) {
        if (!b) return;
        if (factor > 0) for (int i=0;i<m_BufferSize;i++) Buffer[i]+=b[i]*factor;
    }
    inline void AddRightBuffer(float* b) {
        if (!b) return;
        for (int i=0;i<m_BufferSize;i++) BufferR[i]+=b[i];
    }
    inline void AddRightBuffer(float* b, float factor) {
        if (!b) return;
        if (factor > 0) for (int i=0;i<m_BufferSize;i++) BufferR[i]+=b[i]*factor;
    }
    inline float* AddMono(float* L, float* R) {
        if (L) for (int i=0;i<m_BufferSize;i++) Buffer[i]+=L[i];
        if (R) for (int i=0;i<m_BufferSize;i++) BufferR[i]+=R[i];
        return Buffer;
    }
    inline float* AddMono(float* b) {
        if (!b) return Buffer;
        for (int i=0;i<m_BufferSize;i++)
        {
            Buffer[i]+=b[i];
            BufferR[i]+=b[i];
        }
        return Buffer;
    }
    inline float* Multiply(float factor) { return CAudioBuffer::Multiply(factor); }
    inline float* Multiply(float factorL,float factorR) {
        if (factorL==0) ZeroMemory(Buffer,m_BufferSize*sizeof(float));
        if (factorL != 1) for (int i=0;i<m_BufferSize;i++) Buffer[i]*=factorL;
        if (factorR==0) ZeroMemory(BufferR,m_BufferSize*sizeof(float));
        if (factorR != 1) for (int i=0;i<m_BufferSize;i++) BufferR[i]*=factorR;
        return Buffer;

    }
    float* BufferR;
};

#endif // CAUDIOBUFFER_H
