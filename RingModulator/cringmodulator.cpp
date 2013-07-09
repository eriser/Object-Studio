#include "cringmodulator.h"

CRingModulator::CRingModulator():ModulationFactor(0)
{

}

void CRingModulator::Init(const int Index, void *MainWindow) {
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Wave,IJack::Out,0);
    AddJack("In",IJack::Wave,IJack::In,0);
    AddJack("Modulation",IJack::Wave,IJack::In,0);
    AddParameter(ParameterType::Numeric,"Modulation","%",0,100,0,"",0);
    CalcParams();
}

float *CRingModulator::GetNextA(const int ProcIndex) {
    float* InSignal=FetchA(jnIn);
    float* InModulation=FetchA(jnModulation);
    if (!InSignal)
    {
        return NULL;
    }
    if (!InModulation)
    {
        return InSignal;
    }
    for (int i=0;i<m_BufferSize;i++)
    {
        //float Modulation=1-((((float)*(InModulation+i))*ModulationFactor)+(m_ParameterValues[pnModulation] * 0.005));
        float Modulation=(InModulation[i]*ModulationFactor)*InSignal[i];
        *(AudioBuffers[ProcIndex]->Buffer+i)= InSignal[i]*CleanFactor + Modulation;//Modulation;
    }
    return AudioBuffers[ProcIndex]->Buffer;
}

void CRingModulator::CalcParams() {
    ModulationFactor=m_ParameterValues[pnModulation] * 0.01;
    CleanFactor=1-ModulationFactor;
}
