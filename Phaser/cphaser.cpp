#include "cphaser.h"

CPhaser::CPhaser()
{
}

Phaser::Phaser() : _fb( .7f )
  , _lfoPhase( 0.f )
  , _depth( 1.f )
  , _zm1( 0.f )
  , _SampleRate(CPresets::Presets.SampleRate)
  , _pi(DoublePi) {
    Range( 440.f, 1600.f );
    Rate( .5f );
}

void Phaser::Range(float fMin, float fMax) { // Hz
    _dmin = fMin / (_SampleRate/2.f);
    _dmax = fMax / (_SampleRate/2.f);
}

void Phaser::Rate(float rate) { // cps
    _lfoInc = 2.f * _pi * (rate / _SampleRate);
}

void Phaser::Feedback(float fb) { // 0 -> <1.
    _fb = fb;
}

void Phaser::Depth(float depth) {  // 0 -> 1.
    _depth = depth;
}

float Phaser::Update(float inSamp) {
    //calculate and update phaser sweep lfo...
    float d  = _dmin + (_dmax-_dmin) * ((sin( _lfoPhase ) + 1.f)/2.f);
    _lfoPhase += _lfoInc;
    if( _lfoPhase >= _pi * 2.f )
    {
        _lfoPhase -= _pi * 2.f;
    }

    //update filter coeffs
    for( int i=0; i<6; i++ )
    {
        _alps[i].Delay( d );
    }

    //calculate output
    float y = 	_alps[0].Update(
                _alps[1].Update(
                _alps[2].Update(
                _alps[3].Update(
                _alps[4].Update(
                _alps[5].Update( inSamp + _zm1 * _fb ))))));
    _zm1 = y;

    return inSamp + y * _depth;
}

Phaser::AllpassDelay::AllpassDelay() : _a1( 0.f )
  , _zm1( 0.f ) {}

void Phaser::AllpassDelay::Delay(float delay) { //sample delay time
    _a1 = (1.f - delay) / (1.f + delay);
}

float Phaser::AllpassDelay::Update(float inSamp) {
    float y = inSamp * -_a1 + _zm1;
    _zm1 = y * _a1 + inSamp;

    return y;
}

void CPhaser::Init(const int Index, void *MainWindow) {
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Wave,IJack::Out,0);
    AddJack("In",IJack::Wave,IJack::In,0);
    AddParameter(ParameterType::Numeric,"Range Min","Hz",20,m_Presets.SampleRate / 2.f,0,"",440);
    AddParameter(ParameterType::Numeric,"Range Max","Hz",20,m_Presets.SampleRate / 2.f,0,"",1600);
    AddParameter(ParameterType::Numeric,"Rate","Sweeps/sec",5,200,100,"",50);
    AddParameter(ParameterType::Numeric,"Feedback","%",0,100,0,"",70);
    AddParameter(ParameterType::Numeric,"Depth","%",0,100,0,"",100);
    CalcParams();
}

float *CPhaser::GetNextA(const int ProcIndex) {
    float* InSignal=FetchA(jnIn);
    if (!InSignal) return NULL;
    float* Buffer=AudioBuffers[ProcIndex]->Buffer;
    for (int i=0;i<m_BufferSize;i++)
    {
        Buffer[i]=P.Update(InSignal[i]);
    }
    return Buffer;
}

void CPhaser::CalcParams() {
    P.Range(m_ParameterValues[pnRangeMin],m_ParameterValues[pnRangeMax]);
    P.Rate(m_ParameterValues[pnRate]*0.01);
    P.Feedback(m_ParameterValues[pnFeedback]*0.0098);
    P.Depth(m_ParameterValues[pnDepth]*0.01);
}
