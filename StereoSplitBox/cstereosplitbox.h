#ifndef CSTEREOSPLITBOX_H
#define CSTEREOSPLITBOX_H

#include "softsynthsclasses.h"

class CStereoSplitBox  : public IDevice
{
public:
    CStereoSplitBox();
    ~CStereoSplitBox();
    void Init(const int Index, void *MainWindow);
    float* GetNextA(const int ProcIndex);
    void Tick();
    void HideForm();
    void Play(const bool FromStart);
    void Pause();
private:
    enum JackNames
    {jnOut,jnIn,jnOutLeft,jnOutRight,jnInLeft,jnInRight};
    enum ParameterNames
    {};
    void Process();
    std::vector<IJack*> JacksCreated;
    CInJack* WaveOutL;
    CInJack* WaveOutR;
    float* InL;
    float* InR;
};

#endif // CSTEREOSPLITBOX_H
