#ifndef COREBUFFER_H
#define COREBUFFER_H

#include "/Developer/Library/rtaudio-4.0.11/RtAudio.h"
#include "/Developer/Library/rtmidi-2.0.1/RtMidi.h"
#include "softsynthsclasses.h"
#include "cwavefile.h"
#include <QDebug>
#include <QStringList>
#include <QProgressBar>
#include <QThread>

class CCoreMainBuffers : public IDevice
{
public:
    CInJack* InAudio;
    CInJack* InMIDI;
    COutJack* OutAudio;
    COutJack* OutMIDI;
    CCoreMainBuffers();
    void Init(const int Index, void* MainWindow, IHost* Host);
    void* GetNextP(const int ProcIndex);
    void MainAudioLoop(float* OutBuffer, float* InBuffer, const int BufferSize);
    void CreateBuffer();
    void Finish();
    void Panic();
    void Wait();
    void StartRecording();
    void StopRecording();
    const bool SaveRecording(const QString& fileName);
    void getPeak(float& L,float& R);
    QStringList DeviceList(int Direction);
    static int AudioCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData );
    float outputVol;
private:
    enum BufferStates
    {
        Ready=0,
        Working=1,
        Stopped=2
    };
    enum JackNames { jnIn,jnMIDIIn,jnOut,jnMIDIOut };
    BufferStates BufferState;

    RtMidiIn m_MidiIn;
    RtMidiOut m_MidiOut;
    RtAudio m_Audio;
    unsigned int m_ModulationRate;
    unsigned int m_SampleRate;
    unsigned int m_BufferSize;

    float* ChannelBufferL;
    float* ChannelBufferR;

    float* InBufferL;
    float* InBufferR;
    unsigned int TickCount;

    float PeakL;
    float PeakR;

    CMIDIBuffer MIDIBuffer;
    const float inline TruncateVal(float Buf, float& Peak);
    void inline ParseMidi(CMIDIBuffer* MIDIBuffer);

    CWaveFile WaveFile;
    bool m_Recording;
};

#endif // COREBUFFER_H
