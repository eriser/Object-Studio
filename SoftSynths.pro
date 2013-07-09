cache()
QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
CONFIG += qt x86
CONFIG -= x86_64 ppc64 ppc

QT -= network opengl sql svg xml xmlpatterns qt3support

TEMPLATE = subdirs

SUBDIRS = PluginLoader \
    SoftSynthsClasses \
    WaveBank \
    WaveGenerator \
    SF2Generator \
    MIDIFileReader \
    Amplifier \
    RtAudioBuffer \
    MIDIFilePlayer \
    ToneGenerator \
    Delay \
    LFO \
    Filter \
    Panner \
    Scope \
    Envelope \
    MacroBox \
    PolyBox \
    DrumMachine \
    SF2Player \
    Sequenser \
    MIDI2CV \
    RingModulator \
    Limiter \
    Exciter \
    Mixer \
    PitchShifter \
    Chorus \
    KarlsenFilter \
    MoogVCF \
    NoiseGate \
    Phaser \
    VSTHost \
    Equalizer \
    AudacityProject \
    WaveRecorder \
    PitchTracker \
    Unifilter \
    WaveShaper \
    Sampler \
    StereoMixer \
    MIDIFile2Wave \
    StereoBox \
    StereoSplitBox








































