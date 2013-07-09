#include "cwaveshaper.h"

CWaveShaper::CWaveShaper()
{
}

void CWaveShaper::CalcParams() {
    float Amount=m_ParameterValues[pnAmount]*0.00999999;
    k = 2*Amount/(1-Amount);
    k1=-1+Amount;
    k2=0.9999+(Amount*100);
    a=m_ParameterValues[pnAmount];
    a1=m_ParameterValues[pnAmount]*0.02;
    m_Gain=m_ParameterValues[pnGain]*0.01;
}

int CWaveShaper::sign(float x) {
    if (x !=0) return x/fabs(x);
    return 0;
}

float CWaveShaper::max(float x, float a) {
    x -= a;
    x += fabs (x);
    x *= 0.5;
    x += a;
    return (x);
}

float CWaveShaper::min(float x, float b) {
    x = b - x;
    x += fabs (x);
    x *= 0.5;
    x = b - x;
    return (x);
}

float CWaveShaper::clip(float x, float a, float b) {
    float x1 = fabs (x-a);
    float x2 = fabs (x-b);
    x = x1 + (a+b);
    x -= x2;
    x *= 0.5;
    return (x);
}

void CWaveShaper::Init(const int Index, void *MainWindow) {
    m_Name="WaveShaper";
    IDevice::Init(Index,MainWindow);
    AddJack("In",IJack::Wave,IJack::In,0);
    AddJack("Out",IJack::Wave,IJack::Out,0);
    AddParameter(ParameterType::Numeric,"Gain","%",0,200,0,"",100);
    AddParameter(ParameterType::Numeric,"Amount","%",1,100,0,"",1);
    AddParameter(ParameterType::SelectBox,"Type","",0,4,0,"Hard§Softer§Sinus§Gloubi-boulga§Clipping",0);
    CalcParams();
}

float *CWaveShaper::GetNextA(const int ProcIndex) {
    float* Signal=FetchA(jnIn);
    if (!Signal) return NULL;
    float* Buffer=AudioBuffers[ProcIndex]->Buffer;
    switch (m_ParameterValues[pnType])
    {
    case 0:
        for (int i=0;i<m_BufferSize;i++)
        {
            float x=Signal[i]*m_Gain;
            Buffer[i]=((1+k)*x/(1+k*fabs(x)));
        }
        break;
    case 1:
        for (int i=0;i<m_BufferSize;i++)
        {
            float x=Signal[i]*m_Gain;
            Buffer[i]=(x*(fabs(x) + a)/(pow(x,2) + (a-1)*fabs(x) + 1));
        }
        break;
    case 2:
    {
        float y;
        float z = DoublePi * a1;
        float s = 1/sin(z);
        float b = 1/a1;

        for (int i=0;i<m_BufferSize;i++)
        {
            float x=Signal[i]*m_Gain;
            if (x > b)
            {
                y = 1;
            }
            else
            {
                y = sin(z*x)*s;
            }
            Buffer[i]=y;
        }
    }
        break;
    case 3:
        for (int i=0;i<m_BufferSize;i++)
        {
            float y=Signal[i]*m_Gain;

            double x = y * 0.686306;
            double z = 1 + exp (sqrt (fabs (x)) * k1);//-0.75);
            Buffer[i]=((exp (x) - exp (-x * z)) / (exp (x) + exp (-x)));
        }
        break;
    case 4:
        for (int i=0;i<m_BufferSize;i++)
        {
            float x=Signal[i]*m_Gain;
            //float y=pow(fabs(x), 1 / k2);
            Buffer[i]=clip(x*k2,1,-1);
        }
        break;
    }
    return Buffer;
}
