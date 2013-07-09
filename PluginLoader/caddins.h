#ifndef CADDINS_H
#define CADDINS_H

#include <QtCore>
#include <stdlib.h>

typedef void*(*voidinstancefunc)();

class CAddIns
{
public:
    struct AddInType
    {
        QString Path;
        QString ClassName;
        voidinstancefunc InstanceFunction;
        void* Instance;
    };
    CAddIns();
    ~CAddIns();
    static std::vector<AddInType> AddInList;
    static const QStringList AddInNames();
    static const unsigned int AddInIndex(const QString& Name);
    static int instances;
private:
    void LoadAddIns(QDir& pluginsDir);
};

#endif // CADDINS_H
