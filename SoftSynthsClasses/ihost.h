#ifndef IHOST_H
#define IHOST_H

class IDevice;

class IHost
{
public:
    IHost(){}
    virtual void ParameterChange(){}
    virtual void Tick(){}
    virtual void Activate(IDevice* /*Device*/){}
};

#endif // IHOST_H
