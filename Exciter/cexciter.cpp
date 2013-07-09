#include "cexciter.h"

CFxRbjFilter::CFxRbjFilter() {
    // reset filter coeffs
    b0a0=b1a0=b2a0=a1a0=a2a0=0.0;

    // reset in/out history
    ou1=ou2=in1=in2=0.0f;
}

float CFxRbjFilter::filter(float in0) {
    // filter
    float const yn = b0a0*in0 + b1a0*in1 + b2a0*in2 - a1a0*ou1 - a2a0*ou2;

    // push in/out buffers
    in2=in1;
    in1=in0;
    ou2=ou1;
    ou1=yn;

    // return output
    return yn;
}

void CFxRbjFilter::calc_filter_coeffs(const int type, const double frequency, const double sample_rate, const double q, const double db_gain, bool q_is_bandwidth) {
    //double const temp_pi=3.1415926535897932384626433832795;
    // temp coef vars
    double alpha,a0,a1,a2,b0,b1,b2;

    // peaking, lowshelf and hishelf
    if(type>=6)
    {
        double const A		=	db_gain;//pow(10.0,(db_gain/40.0));
        double const omega	=	2.0*DoublePi*frequency/sample_rate;
        double const tsin	=	sin(omega);
        double const tcos	=	cos(omega);

        if(q_is_bandwidth)
            alpha=tsin*sinh(log(2.0)/2.0*q*omega/tsin);
        else
            alpha=tsin/(2.0*q);

        double const beta	=	sqrt(A)/q;

        // peaking
        if(type==6)
        {
            b0=float(1.0+alpha*A);
            b1=float(-2.0*tcos);
            b2=float(1.0-alpha*A);
            a0=float(1.0+alpha/A);
            a1=float(-2.0*tcos);
            a2=float(1.0-alpha/A);
        }

        // lowshelf
        if(type==7)
        {
            b0=float(A*((A+1.0)-(A-1.0)*tcos+beta*tsin));
            b1=float(2.0*A*((A-1.0)-(A+1.0)*tcos));
            b2=float(A*((A+1.0)-(A-1.0)*tcos-beta*tsin));
            a0=float((A+1.0)+(A-1.0)*tcos+beta*tsin);
            a1=float(-2.0*((A-1.0)+(A+1.0)*tcos));
            a2=float((A+1.0)+(A-1.0)*tcos-beta*tsin);
        }

        // hishelf
        if(type==8)
        {
            b0=float(A*((A+1.0)+(A-1.0)*tcos+beta*tsin));
            b1=float(-2.0*A*((A-1.0)+(A+1.0)*tcos));
            b2=float(A*((A+1.0)+(A-1.0)*tcos-beta*tsin));
            a0=float((A+1.0)-(A-1.0)*tcos+beta*tsin);
            a1=float(2.0*((A-1.0)-(A+1.0)*tcos));
            a2=float((A+1.0)-(A-1.0)*tcos-beta*tsin);
        }
    }
    else
    {
        // other filters
        double const omega	=	2.0*DoublePi*frequency/sample_rate;
        double const tsin	=	sin(omega);
        double const tcos	=	cos(omega);

        if(q_is_bandwidth)
            alpha=tsin*sinh(log(2.0)/2.0*q*omega/tsin);
        else
            alpha=tsin/(2.0*q);


        // lowpass
        if(type==0)
        {
            b0=(1.0-tcos)/2.0;
            b1=1.0-tcos;
            b2=(1.0-tcos)/2.0;
            a0=1.0+alpha;
            a1=-2.0*tcos;
            a2=1.0-alpha;
        }

        // hipass
        if(type==1)
        {
            b0=(1.0+tcos)/2.0;
            b1=-(1.0+tcos);
            b2=(1.0+tcos)/2.0;
            a0=1.0+ alpha;
            a1=-2.0*tcos;
            a2=1.0-alpha;
        }

        // bandpass csg
        if(type==2)
        {
            b0=tsin/2.0;
            b1=0.0;
            b2=-tsin/2;
            a0=1.0+alpha;
            a1=-2.0*tcos;
            a2=1.0-alpha;
        }

        // bandpass czpg
        if(type==3)
        {
            b0=alpha;
            b1=0.0;
            b2=-alpha;
            a0=1.0+alpha;
            a1=-2.0*tcos;
            a2=1.0-alpha;
        }

        // notch
        if(type==4)
        {
            b0=1.0;
            b1=-2.0*tcos;
            b2=1.0;
            a0=1.0+alpha;
            a1=-2.0*tcos;
            a2=1.0-alpha;
        }

        // allpass
        if(type==5)
        {
            b0=1.0-alpha;
            b1=-2.0*tcos;
            b2=1.0+alpha;
            a0=1.0+alpha;
            a1=-2.0*tcos;
            a2=1.0-alpha;
        }
    }

    // set filter coeffs
    b0a0=float(b0/a0);
    b1a0=float(b1/a0);
    b2a0=float(b2/a0);
    a1a0=float(a1/a0);
    a2a0=float(a2/a0);
}

CExciter::CExciter()
{
}

void CExciter::Init(const int Index, void *MainWindow)
{
    int Maxcutoff=m_Presets.SampleRate * 0.425;
    m_Name="Exciter";
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Wave,IJack::Out,0);
    AddJack("Effect Out",IJack::Wave,IJack::Out,0);
    AddJack("In",IJack::Wave,IJack::In,0);
    AddParameter(ParameterType::SelectBox,"Type","",0,1,0,"Soft§Clipping",0);
    AddParameter(ParameterType::dB,"In Volume","dB",0,200,0,"",100);
    AddParameter(ParameterType::Numeric,"Amount","%",0,100,0,"",100);
    AddParameter(ParameterType::Numeric,"Cutoff Frequency","Hz",20,Maxcutoff,0,"",Maxcutoff/2);
    AddParameter(ParameterType::Numeric,"Effect","%",0,100,0,"",50);
    CalcParams();
}

float *CExciter::GetNextA(const int ProcIndex)
{
    float* InSignal=FetchA(jnIn);
    if (!InSignal) return NULL;
    float* Buffer=AudioBuffers[ProcIndex]->Buffer;
    if (ProcIndex==jnOut)
    {
        if (m_ParameterValues[pnType]==0)
        {
            for (int i=0;i<m_BufferSize;i++)
            {
                float x=(float)InSignal[i]*InVolFactor;
                x=RBJFilter.filter(x);
                x=(x*(fabs(x) + m_ParameterValues[pnAmount])/(pow(x,2) + (m_ParameterValues[pnAmount]-1)*fabs(x) + 1));
                Buffer[i] = (x*EffFactor)+(InSignal[i]*OutVolFactor);
            }
        }
        else
        {
            for (int i=0;i<m_BufferSize;i++)
            {
                float x=(float)InSignal[i]*InVolFactor;
                x=RBJFilter.filter(x);
                x=clip(x*k2,1,-1);
                Buffer[i] = (x*EffFactor)+(InSignal[i]*OutVolFactor);
            }
        }
    }
    else if (ProcIndex==jnEffOut)
    {
        if (m_ParameterValues[pnType]==0)
        {
            for (int i=0;i<m_BufferSize;i++)
            {
                float x=(float)InSignal[i]*InVolFactor;
                x=RBJFilter.filter(x);
                x=(x*(fabs(x) + m_ParameterValues[pnAmount])/(pow(x,2) + (m_ParameterValues[pnAmount]-1)*fabs(x) + 1));
                Buffer[i] = x*EffFactor;
            }
        }
        else
        {
            for (int i=0;i<m_BufferSize;i++)
            {
                float x=(float)InSignal[i]*InVolFactor;
                x=RBJFilter.filter(x);
                x=clip(x*k2,1,-1);
                Buffer[i] = x*EffFactor;
            }
        }
    }
    return Buffer;
}

void CExciter::CalcParams()
{
    float Amount=m_ParameterValues[pnAmount]*0.00999999;
    k2=0.9999+(Amount*100);
    InVolFactor=(float)m_ParameterValues[pnInVolume]*0.01;
    EffFactor=(float)m_ParameterValues[pnOutVolume]*0.002;
    OutVolFactor=1.0-pow((EffFactor*0.01)/0.002,2);
    RBJFilter.calc_filter_coeffs(1,m_ParameterValues[pnCutOffFrequency],m_Presets.SampleRate,1,1,false);
}

float CExciter::clip(float x, float a, float b)
{
    float x1 = fabs (x-a);
    float x2 = fabs (x-b);
    x = x1 + (a+b);
    x -= x2;
    x *= 0.5;
    return (x);
}
