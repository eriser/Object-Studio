#ifndef IDEVICEBASE_H
#define IDEVICEBASE_H

#include "softsynthsdefines.h"

class IDeviceBase
{
public:
    virtual const float GetNext(const int /*ProcIndex*/)
    {
        return 0;
    }
    virtual void* GetNextP(const int /*ProcIndex*/)
    {
        return NULL;
    }
    virtual float* GetNextA(const int /*ProcIndex*/)
    {
        return NULL;
    }
};

#endif // IDEVICEBASE_H
