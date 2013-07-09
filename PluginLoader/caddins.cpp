#include "caddins.h"
#include <dlfcn.h>

std::vector<CAddIns::AddInType> CAddIns::AddInList=std::vector<CAddIns::AddInType>();
int CAddIns::instances=0;

CAddIns::CAddIns()
{

    if (instances++ > 0) return;
    qDebug() << "PluginLoader created";
#if defined(Q_OS_WIN)
    QDir pluginsDir("../");
#elif defined(Q_OS_MAC)
    QDir pluginsDir("../../../");
#endif
    LoadAddIns(pluginsDir);
}

CAddIns::~CAddIns()
{
    if (--instances > 0) return;
    foreach (AddInType AI, AddInList) dlclose(AI.Instance);
    qDebug() << "PluginLoader destroyed";
}

const QStringList CAddIns::AddInNames()
{
    QStringList l;
    foreach (AddInType AI, AddInList) l.append(AI.ClassName);
    return l;
}

const unsigned int CAddIns::AddInIndex(const QString &Name)
{
    return AddInNames().indexOf(Name);
}

void CAddIns::LoadAddIns(QDir& pluginsDir)
{
    foreach (QString fileName, pluginsDir.entryList(QStringList() << "*.dylib",QDir::Files | QDir::Hidden | QDir::NoSymLinks))
    {
        QString filepath=pluginsDir.absolutePath()+"/"+fileName;
        qDebug() << filepath;
        void* sdl_library = dlopen(filepath.toUtf8().constData(), RTLD_LAZY);
        if(sdl_library == NULL)
        {
            qDebug() << "Not found!";
        }
        else
        {
           // use the result in a call to dlsym
            qDebug() << "It is here!";
            void* initializer = dlsym(sdl_library,"createinstance");
            void* name = dlsym(sdl_library,"name");
            if (initializer == NULL)
            {
               // report error ...
                qDebug() << dlerror();
                qDebug() << "no initializer";
                dlclose(sdl_library);
            }
            else if (name == NULL)
            {
               // report error ...
                qDebug() << dlerror();
                qDebug() << "no name";
                dlclose(sdl_library);
            }
            else
            {
                char* classname=*(char**)name;
                AddInType addin;
                addin.ClassName = classname;
                addin.Instance = sdl_library;
                addin.InstanceFunction = *(voidinstancefunc*)initializer;
                addin.Path = filepath;
                AddInList.push_back(addin);
                qDebug() << "success" << addin.ClassName << addin.Path;
            }
        }
    }
    foreach (QString dirName, pluginsDir.entryList(QDir::AllDirs | QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot))
    {
        QDir dir(pluginsDir.absolutePath()+"/"+dirName);
        qDebug() << "sub dir "+dirName;
        if (dir.exists()) LoadAddIns(dir);
    }
}

CAddIns Inst;
