#include "caudiobuffer.h"
#include "cpresets.h"

CAudioBuffer::CAudioBuffer(IJackBase::AttachModes a) : AttachMode(a), m_BufferSize(CPresets::Presets.ModulationRate)
{
    m_WaveBufferSize=m_BufferSize;
    if (AttachMode==IJackBase::Stereo) m_WaveBufferSize=m_BufferSize*2;
    Buffer=new float[m_WaveBufferSize];
    ZeroBuffer();
}

CAudioBuffer::~CAudioBuffer()
{
    delete [] Buffer;
}
