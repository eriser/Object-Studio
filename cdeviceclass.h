#ifndef CDEVICECLASS_H
#define CDEVICECLASS_H

#include "softsynthsclasses.h"
#include headerfile

IDevice* _createinstance();
instancefunc Q_DECL_EXPORT createinstance=_createinstance;

const char* Q_DECL_EXPORT name=devicename;

#endif // CDEVICECLASS_H
