#include "corebuffer.h"
#include <QApplication>

int CCoreMainBuffers::AudioCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData )
{
    ((CCoreMainBuffers*)userData)->MainAudioLoop((float*)outputBuffer, (float*)inputBuffer, nBufferFrames);
    return 0;
}

CCoreMainBuffers::CCoreMainBuffers()
{
    m_ModulationRate=m_Presets.ModulationRate;

    m_BufferSize=m_Presets.BufferSize;
    m_SampleRate=m_Presets.SampleRate;

    ChannelBufferL=NULL;
    TickCount=0;

    PeakL=0;
    PeakR=0;

    BufferState=Ready;

    outputVol=1;
    m_Recording=false;
}

void CCoreMainBuffers::Init(const int Index, void *MainWindow, IHost* Host)
{
    m_Name="This";
    IDevice::Init(Index, MainWindow);
    IDevice::SetHost(Host);

    OutAudio=(COutJack*)AddJack("In",IJack::Stereo,IJack::Out,jnIn);
    OutMIDI=(COutJack*)AddJack("MIDI In",IJack::MIDI,IJack::Out,jnMIDIIn);

    InAudio=(CInJack*)AddJack("Out",IJack::Stereo,IJack::In,jnOut);
    InMIDI=(CInJack*)AddJack("MIDI Out",IJack::MIDI,IJack::In,jnMIDIOut);

    CStereoBuffer* Buffer=(CStereoBuffer*)AudioBuffers[jnIn];
    InBufferL=Buffer->Buffer;
    InBufferR=Buffer->BufferR;

    CalcParams();
}

const float inline CCoreMainBuffers::TruncateVal(float Buf, float& Peak)
{
    Buf*=outputVol;
    float Val = Buf;
    if (Val < 0)
    {
        Val = -Val;
    }
    if (Val > Peak)
    {
        Peak = Val;
    }
    return Buf;
}

void inline CCoreMainBuffers::ParseMidi(CMIDIBuffer* MIDIBuffer)
{
    std::vector<BYTE> message;
    MIDIBuffer->StartRead();
    int lTemp=MIDIBuffer->Read();
    while (lTemp > -1)
    {
        if (lTemp >= 0x80)
        {
            message.clear();
            message.push_back(lTemp);
        }
        forever
        {
            lTemp=MIDIBuffer->Read();
            if ((lTemp >= 0x80) | (lTemp < 0)) break;
            message.push_back(lTemp);
        }
        if (message.size() > 0) m_MidiOut.sendMessage(&message);
    }
}

void CCoreMainBuffers::Wait()
{
    while (BufferState != Ready){}
}

void CCoreMainBuffers::MainAudioLoop(float* OutBuffer, float* InBuffer, const int BufferSize)
{
    if (BufferState==Ready)
    {
        BufferState= (BufferStates)(BufferState | Working);
        float* outBufferPointer=OutBuffer;
        float* inBufferPointer=InBuffer;
        for (int i=0; i<BufferSize; i++)
        {
            InBufferL[TickCount]=*inBufferPointer++;
            InBufferR[TickCount]=*inBufferPointer++;
            if (TickCount>=m_ModulationRate)
            {
                TickCount=0;
                //Tick All Devices!!!
                if (m_Host) m_Host->Tick();
                CMIDIBuffer* MIDIBuffer=(CMIDIBuffer*)FetchP(jnMIDIOut);
                if (MIDIBuffer != NULL) ParseMidi(MIDIBuffer);
                ChannelBufferL=FetchA(jnOut);
                if (ChannelBufferL) ChannelBufferR=ChannelBufferL+m_ModulationRate;
                if (m_Recording) WaveFile.pushBuffer(ChannelBufferL,m_ModulationRate);
            }

            if (ChannelBufferL)
            {
                *outBufferPointer++=TruncateVal(ChannelBufferL[TickCount], PeakL);
                *outBufferPointer++=TruncateVal(ChannelBufferR[TickCount], PeakR);
            }
            else
            {
                *outBufferPointer++=0;
                *outBufferPointer++=0;
            }
            TickCount++;
        }
        BufferState =(BufferStates)(BufferState & (!Working));
    }
}

void CCoreMainBuffers::StartRecording()
{
    m_Recording=true;
    WaveFile.startRecording(2,m_SampleRate);
}

void CCoreMainBuffers::StopRecording()
{
    if (m_Recording)
    {
        m_Recording=false;
        WaveFile.finishRecording();
    }
}

const bool CCoreMainBuffers::SaveRecording(const QString &fileName)
{
    return WaveFile.save(fileName);
}

void CCoreMainBuffers::getPeak(float &L, float &R)
{
    L=PeakL;
    R=PeakR;
    PeakL=0;
    PeakR=0;
}

void* CCoreMainBuffers::GetNextP(const int /*ProcIndex*/)
{
    MIDIBuffer.Reset();
    std::vector<BYTE> message;
    m_MidiIn.getMessage( &message );
    foreach (BYTE c, message) MIDIBuffer.Push(c);
    return (void*)&MIDIBuffer;
}

void CCoreMainBuffers::CreateBuffer()
{
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = m_Audio.getDefaultInputDevice(); // first available device
    iParams.nChannels = 2;
    iParams.firstChannel = 0;
    oParams.deviceId = m_Audio.getDefaultOutputDevice(); // first available device
    oParams.nChannels = 2;
    oParams.firstChannel = 0;

    try
    {
        m_Audio.openStream( &oParams, &iParams, RTAUDIO_FLOAT32, m_SampleRate, &m_BufferSize, &AudioCallback, this );
        m_Audio.startStream();
    }
    catch ( RtError& e )
    {
        qDebug() << e.what();
        Finish();
        exit( 0 );
    }
    try
    {
        if ( m_MidiOut.getPortCount() > 0 )
        {
            m_MidiOut.openPort(0);
        }
    }
    catch ( RtError& e )
    {
        qDebug() << e.what();
    }

    m_MidiIn.openVirtualPort("Object Studio MIDI In");
    m_MidiIn.ignoreTypes( true, true, true );
}

void CCoreMainBuffers::Panic()
{
    for (int i=0;i<16;i++)
    {
        std::vector<BYTE> message;
        message.push_back(0xB0+i);
        message.push_back(0x7B);
        message.push_back(00);
        m_MidiOut.sendMessage(&message);
    }
    //Bn 7B 00 All notes off!
}

void CCoreMainBuffers::Finish()
{
    Panic();
    m_Audio.stopStream();
    m_MidiOut.closePort();
    m_MidiIn.closePort();
}

QStringList CCoreMainBuffers::DeviceList(int Direction)
{
    QStringList retVal;
    RtAudio audio;
    for (unsigned int i=0; i<audio.getDeviceCount(); i++)
    {
        try
        {
            RtAudio::DeviceInfo info(audio.getDeviceInfo(i));
            if (((info.outputChannels>=2) && (Direction==0)) || ((info.inputChannels>=2) && (Direction==1)))
            {
                bool Match1=false;
                for (unsigned int i1=0;i1<info.sampleRates.size();i1++)
                {
                    if (info.sampleRates[i1]==m_SampleRate)
                    {
                        Match1=true;
                        break;
                    }
                }
                if (Match1)
                {
                    if (info.nativeFormats & RTAUDIO_FLOAT32)
                    {
                        QString DrvName = QString::fromStdString(info.name);
                        /*
                        for (int c=0;c<info.name.size();c++)
                        {
                            qDebug() << QChar(info.name[c]).unicode() << QChar(info.name[c]);
                        }
                        foreach (QChar c,DrvName) qDebug() << c.unicode() << c;
                        qDebug() << DrvName;
                        */
                        QString Ext;
                        int ExistCount=0;
                        while (retVal.contains(DrvName+Ext))
                        {
                            ExistCount++;
                            Ext=" (" + QString::number(ExistCount)+ ")";
                        }
                        retVal.append(DrvName+Ext);
                    }
                }
            }
        }
        catch (RtError &error)
        {
            qDebug() << error.what();
        }
    }
    return retVal;
}
