#include "csf2file.h"
#include "softsynthsclasses.h"
#include "softsynthsdefines.h"

CSF2File::CSF2File()
{
    ReferenceCount=0;
    Size=0;
    SFData=NULL;
}

CSF2File::~CSF2File()
{
    if (SFData) delete [] SFData;
}

bool CSF2File::Open(const QString &filePath)
{
    Path=filePath;
    BankID=sfReadSFBFile((char*)filePath.toUtf8().constData());
    if (BankID==-1) return false;
    Offset=sfGetSMPLOffset(BankID);
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) return false;
    Size=f.size();
    SFData=new BYTE[Size];
    CopyMemory(SFData,f.map(0,Size),Size);
    Mem=(short*)(SFData+Offset);
    return true;
}

const short CSF2File::ReadMem(const size_t& Pos)
{
    return Mem[Pos];
}

OscType::~OscType()
{
}

OscType::OscType()
{
}

void OscType::Init(sfData* Data,CSF2File* SFFile, const short MidiNote, const short MidiVelo)
{
    m_SFFile=SFFile;
    CopyMemory(&OscData,Data,sizeof(sfData));
    Silent=true;
    if ((OscData.dwEnd*sizeof(short)) + m_SFFile->Offset > m_SFFile->Size)
    {
        return;
    }
    //ModCount=0;
    EnvMod=1;
    EnvVol=1;
    pitchWheel=0;
    transpose=0;
    //ModCount=ModRate;
    float ModFactor=(float)CPresets::Presets.SampleRate/(float)CPresets::Presets.ModulationRate;
    VolEnv.Init(OscData.shDelayVolEnv,OscData.shAttackVolEnv,OscData.shHoldVolEnv,
                OscData.shDecayVolEnv,OscData.shSustainVolEnv,OscData.shReleaseVolEnv,OscData.shAutoHoldVolEnv,
                OscData.shAutoDecayVolEnv,MidiNote,ModFactor);
    ModEnv.Init(OscData.shDelayModEnv,OscData.shAttackModEnv,OscData.shHoldModEnv,
                OscData.shDecayModEnv,OscData.shSustainModEnv,OscData.shReleaseModEnv,OscData.shAutoHoldModEnv,
                OscData.shAutoDecayModEnv,MidiNote,ModFactor);
    ModLFO.Init(OscData.shFreqModLfo,OscData.shDelayModLfo,MidiNote,ModFactor);
    VibLFO.Init(OscData.shFreqVibLfo,OscData.shDelayVibLfo,MidiNote,ModFactor);
    Filter.Init(OscData.shInitialFilterFc,OscData.shInitialFilterQ);
    Silent=false;
    float Frequency=MIDItoFreq(MidiNote,440);
    if (OscData.shOverridingRootKey!=-1)
    {
        Frequency=MIDItoFreq(OscData.shOverridingRootKey,440);
    }
    RelEndLoop=OscData.dwEndloop-OscData.dwStart;
    LoopSize=OscData.dwEndloop-OscData.dwStartloop;
    RealStart=OscData.dwStart;
    SampleSize=OscData.dwEnd-OscData.dwStart;
    VolumeFactor=((cB_to_Percent(OscData.shInstVol)*(float)MidiVelo)/127.0)*MAXSHORTMULTIPLY;
    float RateFactor=((unsigned int)OscData.dwSampleRate)/(float)CPresets::Presets.SampleRate;
    char OrigKey=OscData.shOrigKeyAndCorr/0x100;
    char Correction=OscData.shOrigKeyAndCorr & 0xFF;
    float SampleFrequency=MIDItoFreq(OrigKey-OscData.shCoarseTune,440);
    SampleFrequency=SampleFrequency*Cent_to_Percent(-Correction);
    SampleFrequency=SampleFrequency*Cent_to_Percent(-OscData.shFineTune);
    PosRate=(Frequency/SampleFrequency)*RateFactor;
    SampleMode=OscData.shSampleModes;
    LeftPan=1;
    RightPan=1;
    Aftertouch=1;
    if (OscData.shPanEffectsSend>0)
    {
        if (OscData.shPanEffectsSend>500)
        {
            LeftPan=0;
        }
        else
        {
            LeftPan=(500-OscData.shPanEffectsSend)*0.002;
        }
    }
    if (OscData.shPanEffectsSend<0)
    {
        if (OscData.shPanEffectsSend<-500)
        {
            RightPan=0;
        }
        else
        {
            RightPan=(OscData.shPanEffectsSend+500)*0.002;
        }
    }
    if (OscData.shSampleModes & 4)
    {
        if (OscData.shSampleModes & 8)
        {
            Stereo=StereoR;
        }
        else
        {
            Stereo=StereoL;
        }
    }
    else
    {
        Stereo=Mono;
    }

    /*
                        qDebug() << "dwStart " + QString::number(OscData.dwStart) +
                                " dwEnd " + QString::number(OscData.dwEnd) +
                                " dwSampleRate " + QString::number(OscData.dwSampleRate) +
                                " dwStartloop " + QString::number(OscData.dwStartloop) +
                                " dwEndloop " + QString::number(OscData.dwEndloop) +
                                " Orig Key " + QString::number((short)OrigKey) +
                                " Correction " + QString::number((short)Correction) +
                                " shCoarseTune " + QString::number(OscData.shCoarseTune) +
                                " shFineTune " + QString::number(OscData.shFineTune) +
                                " shInstVol " + QString::number(OscData.shInstVol) +
                                " SampleFreq " + QString::number(SampleFrequency) +
                                " RateFactor " + QString::number(RateFactor) +
                                " InitialFilterFc " + QString::number(OscData.shInitialFilterFc) +
                                " Pan " + QString::number(OscData.shPanEffectsSend) +
                                " Mode " + QString::number(OscData.shSampleModes) +
                                " Link " + QString::number(OscData.shSampleLink) +
                                " VolumeFactor " + QString::number(VolumeFactor) +
                                " Override root key " + QString::number(OscData.shOverridingRootKey)
                        ;
                        */
}

void OscType::setPitchWheel(const float cent)
{
    pitchWheel=cent;
}

void OscType::setTranspose(const int steps)
{
    transpose=steps*100;
}

void OscType::setAftertouch(const float value)
{
    Aftertouch=value;
}

void OscType::Modulate()
{
    //Debug("Modulate " + QString::number(ModCount));
    //if (ModCount>=ModRate)
    //{
    //ModCount=0;
    float ModLFOVal=ModLFO.GetNext()*Aftertouch;
    float VibLFOVal=VibLFO.GetNext()*Aftertouch;
    float ModEnvVal=ModEnv.GetNext();
    float VolEnvVal=cB_to_Percent((1-VolEnv.GetNext())*960);
    EnvMod=Cent_to_Percent((ModEnvVal*OscData.shModEnvToPitch)+(ModLFOVal*OscData.shModLfoToPitch)+(VibLFOVal*OscData.shVibLfoToPitch) + pitchWheel+transpose);
    EnvVol=VolEnvVal*cB_to_Percent(ModLFOVal*OscData.shModLfoToVolume);
    Filter.SetAmount(Cent_to_Percent((ModEnvVal*OscData.shModEnvToFilterFc)+(ModLFOVal*OscData.shModLfoToFilterFc)));
    if (VolEnv.Silent)
    {
        Silent=true;
    }
    //}
    //ModCount++;
}

const float OscType::UpdatePos(void)
{
    float Data = m_SFFile->ReadMem(RealPos());
    Position += PosRate * EnvMod;
    if (OscData.shInitialFilterFc >= 13500) return Data * VolumeFactor * EnvVol;
    return Filter.GetNext(Data * VolumeFactor * EnvVol);
}

const unsigned int OscType::RealPos(void)
{
    return RealStart+Position;
}

void OscType::Loop()
{
    while (Position>=RelEndLoop) Position-=LoopSize;
}
const bool OscType::NoLoop()
{
    if (Position>=SampleSize)
    {
        Silent=true;
        return true;
    }
    return false;
}

void OscType::Start()
{
    Position=0;
    VolEnv.Start();
    ModEnv.Start();
    ModLFO.Start();
    VibLFO.Start();
}

void OscType::End()
{
    VolEnv.End();
    ModEnv.End();
}

int inline TimeConv(const int mSec, const float RateFactor)
{
    //qDebug() << "mSec " + QString::number(mSec) + " RateFactor " + QString::number(RateFactor) + " Time " + QString::number((mSec *RateFactor)*0.001);
    return ((float)mSec * RateFactor) * 0.001;
}

void SFEnvelope::Init(const int Delay, const int Attack, const int Hold, const int Decay, const int Sustain, const int Release, const int AutoHold, const int AutoRelease, const int KeyNum, const float RateFactor)
{
    mDelay=TimeConv(awe_timecent_to_msec(Delay),RateFactor);
    mAttack=TimeConv(awe_timecent_to_msec(Attack),RateFactor);
    mHold=TimeConv(awe_timecent_to_msec(Hold),RateFactor)*pow(2.0,((AutoHold*(KeyNum-60))/-1200));
    mDecay=TimeConv(awe_timecent_to_msec(Decay),RateFactor);
    mSustain=cB_to_Percent(Sustain);
    mRelease=TimeConv(awe_timecent_to_msec(Release),RateFactor)*pow(2.0,((AutoRelease*(KeyNum-60))/-1200));
    /*
        qDebug() << "Delay " + QString::number(Delay) + " " +QString::number(mDelay) + " " + QString::number(awe_timecent_to_msec(Delay)) +
                " Attack " + QString::number(Attack) + " " +QString::number(mAttack) + " " + QString::number(awe_timecent_to_msec(Attack)) +
                " Hold " + QString::number(Hold) + " " +QString::number(mHold) + " " + QString::number(awe_timecent_to_msec(Hold)) +
                " Decay " + QString::number(Decay) + " " +QString::number(mDecay) + " " + QString::number(awe_timecent_to_msec(Decay)) +
                " Sustain " + QString::number(Sustain) + " " +QString::number(mSustain) +
                " Release " + QString::number(Release) + " " +QString::number(mRelease) + " " + QString::number(awe_timecent_to_msec(Release)) +
                " AutoHold " + QString::number(AutoHold) + " " + " " + QString::number(pow(2.0,((AutoHold*(KeyNum-60))/-1200))) +
                " AutoRelease " + QString::number(AutoRelease) + " " + QString::number(pow(2.0,((AutoRelease*(KeyNum-60))/-1200)))
        ;
        */
    if (mAttack>0)
    {
        AttackStep = 1.0 / mAttack;
    }
    else
    {
        AttackStep=1;
    }
    if (mDecay>0)
    {
        DecayStep = (1.0 - mSustain) / mDecay;
    }
    else
    {
        DecayStep = 1.0 - mSustain;
    }
    //qDebug() << "Decay " + QString::number(DecayStep) + " " + QString::number(mDecay) + " " + QString::number(mDecay*DecayStep) ;
    CurrentVol=0;
    CurrentAction=evSilent;
    Counter=0;
    Silent=true;
}

const float inline SFEnvelope::GetNext()
{
    if (CurrentAction==evSilent)
    {
        return 0;
    }
    if (CurrentAction==evDelay)
    {
        //TimeCount=GetTickCount();
        if (Counter<mDelay)
        {
            Counter++;
            return 0;
        }
        CurrentAction=evAttack;
        CurrentVol=0;
        //qDebug() << "-> Attack " + QString::number(Counter);
    }
    if (CurrentAction==evAttack)
    {
        if (Counter<mDelay+mAttack)
            //if (Counter<mDelay)
        {
            Counter++;
            CurrentVol+=AttackStep;
            return CurrentVol;
        }
        CurrentAction=evHold;
        CurrentVol=1.0;
        //qDebug() << "-> Hold " + QString::number(Counter);
    }
    if (CurrentAction==evHold)
    {
        if (Counter<mDelay+mAttack+mHold)
            //if (Counter<mHold)
        {
            Counter++;
            return CurrentVol;
        }
        CurrentAction=evDecay;
        //qDebug() << "-> Decay " + QString::number(DecayStep) + " " + QString::number(Counter);
    }
    if (CurrentAction==evDecay)
    {
        if (Counter<mDelay+mAttack+mHold+mDecay)
            //if (Counter<mDecay)
        {
            Counter++;
            CurrentVol-=DecayStep;
            //qDebug() << QString::number(CurrentVol);
            return CurrentVol;
        }
        CurrentAction=evSustain;
        CurrentVol=mSustain;
        //qDebug() << QString::number(GetTickCount()-TimeCount);
        //qDebug() << "-> Sustain " + QString::number(CurrentVol) + " " + QString::number(Counter);
    }
    if (CurrentAction==evSustain)
    {
        return CurrentVol;
    }
    else if (CurrentAction==evRelease)
    {
        if (CurrentVol > 0)
        {
            CurrentVol-=ReleaseStep;
        }
        else
        {
            CurrentVol=0;
            Silent=true;
            CurrentAction=evSilent;
        }
        //qDebug() << CurrentVol;
        return CurrentVol;
    }
    return 0;
}

void SFEnvelope::Start()
{
    CurrentAction=evDelay;
    Counter=0;
    CurrentVol=0;
    Silent=false;
}

void SFEnvelope::End()
{
    CurrentAction=evRelease;
    if (mRelease > 0)
    {
        ReleaseStep = CurrentVol / mRelease;
    }
    else
    {
        ReleaseStep = CurrentVol;
    }
}

void SFLFO::Init(const int Freq, const int Delay, const int /*KeyNum*/, const float RateFactor)
{
    mDelay=TimeConv(awe_timecent_to_msec(Delay),RateFactor);
    FreqValue=8.176*Cent_to_Percent(Freq)*CPresets::Presets.ModulationRate;
    Counter=0;
    CurrentAction=SFEnvelope::evSilent;
    SampleRate=CPresets::Presets.SampleRate;
    WavePosition=SampleRate/4;
}

SFLFO::SFLFO()
{
    //LFO=new TWaveBank();
}

SFLFO::~SFLFO()
{
    //delete LFO;
}

const float inline SFLFO::GetNext()
{
    if (CurrentAction==SFEnvelope::evSilent)
    {
        return 0;
    }
    if (CurrentAction==SFEnvelope::evDelay)
    {
        if (Counter<mDelay)
        {
            Counter++;
            return 0;
        }
        CurrentAction=SFEnvelope::evAttack;
    }
    WavePosition+=FreqValue;
    while (WavePosition>=SampleRate)
    {
        WavePosition=WavePosition-SampleRate;
    }
    return LFO.GetNext(WavePosition,CWaveBank::Triangle);
}

void SFLFO::Start()
{
    Counter=0;
    CurrentAction=SFEnvelope::evDelay;
    WavePosition=SampleRate/4;
}

void SFFilter::Init(const int CutOff, const int HiQ)
{
    Maxcutoff=CPresets::Presets.SampleRate * 0.425;
    CurrentCutOff=8.176*Cent_to_Percent(CutOff);
    CurrentHiQ=(10*calc_filterQ_adip(HiQ))+1;
    //Debug(QString::number(CurrentCutOff) + " " + QString::number(CurrentHiQ));
    CurrentAmount=1;
    FiltCoefTab0=0;
    FiltCoefTab1=0;
    FiltCoefTab2=0;
    FiltCoefTab3=0;
    FiltCoefTab4=0;
    ly1=0;
    ly2=0;
    lx1=0;
    lx2=0;
    m_ExpResonance=0;
    MixFactor=0;
    LastCO=0;
    HalfRate=CPresets::Presets.SampleRate/2;
    //CalcExpResonance();
    SetAmount(1);
}

void inline SFFilter::SetAmount(const float Amount)
{
    CurrentAmount=Amount;
    int CutOff=CurrentCutOff;
    CutOff=(float)CutOff * CurrentAmount;
    if (CutOff>Maxcutoff)
    {
        CutOff=Maxcutoff;
    }
    if (CutOff<20)
    {
        CutOff=20;
    }
    if (LastCO != CutOff)
    {
        //Debug(QString::number(CurrentAmount));
        LastCO=CutOff;
        float Omega=(DoublePi * (float)LastCO) / (float)HalfRate;
        float sn=sin(Omega);
        float cs=cos(Omega);
        float Alpha=sn / CurrentHiQ;
        float b1= 1-cs;
        float b0= b1*0.5;
        float b2= b0;
        float a0=1+Alpha;
        float a1=-2*cs;
        float a2=1-Alpha;
        FiltCoefTab0=b0/a0;
        FiltCoefTab1=b1/a0;
        FiltCoefTab2=b2/a0;
        FiltCoefTab3=-a1/a0;
        FiltCoefTab4=-a2/a0;
        MixFactor=((float)Maxcutoff / (float)LastCO) * 0.004;
        MixFactor=1.00 / ((CurrentHiQ*MixFactor)+(1-MixFactor));
    }

}

const float inline SFFilter::GetNext(const float Signal)
{
    //float Signal=Value;
    float Temp_y=(FiltCoefTab0 * Signal) + (FiltCoefTab1 * lx1) + (FiltCoefTab2 * lx2) + (FiltCoefTab3 * ly1) + (FiltCoefTab4 * ly2);
    ly2=ly1;
    ly1=Temp_y;
    lx2=lx1;
    lx1=Signal;
    return Temp_y * MixFactor;
}

void inline SFFilter::CalcExpResonance()
{
    m_ExpResonance=exp((float)CurrentHiQ/16);
}
