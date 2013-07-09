#ifndef CSTEREOBOX_H
#define CSTEREOBOX_H

#include "softsynthsclasses.h"

class CStereoBox : public IDevice
{
public:
    CStereoBox();
    ~CStereoBox();
    void Init(const int Index, void *MainWindow);
    float* GetNextA(const int ProcIndex);
    void Tick();
    void HideForm();
    void Play(const bool FromStart);
    void Pause();
private:
    enum JackNames
    {jnOut,jnOutLeft,jnOutRight,jnIn,jnInLeft,jnInRight,jnInsideInLeft,jnInsideInRight};
    enum ParameterNames
    {};
    void Process();
    std::vector<IJack*> JacksCreated;
    CInJack* WaveOutL;
    CInJack* WaveOutR;
    float* InL;
    float* InR;
    CStereoBuffer InBuffer;
};


#endif // CSTEREOBOX_H
