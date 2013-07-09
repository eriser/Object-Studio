#ifndef CFASTPOINTERLIST_H
#define CFASTPOINTERLIST_H

#include <QtCore>
#include <stdlib.h>

class CFastPointerList : public std::vector<void*>
{
public:
    const bool Exist(void* Ptr)
    {
        return (IndexOf(Ptr) > -1);
    }
    const int IndexOf(void* Ptr)
    {
        for (unsigned int i = 0; i < size(); i++)
        {
            if (at(i)==Ptr) return i;
        }
        return -1;
    }
    void Remove(void* Ptr)
    {
        int Index=IndexOf(Ptr);
        if (Index>-1) erase(begin() + Index);
    }
    void Remove(const int Index)
    {
        erase(begin() + Index);
    }
};

#endif // CFASTPOINTERLIST_H
