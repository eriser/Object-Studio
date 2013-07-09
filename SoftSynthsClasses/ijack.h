#ifndef IJACK_H
#define IJACK_H

#include "ijackbase.h"
#include "idevicebase.h"
#include "caudiobuffer.h"
#include "cmidibuffer.h"
#include "cfastpointerlist.h"

//bool ModulationZero(void);

class IJack : public IJackBase
{
protected:
    IDeviceBase* m_OwnerClass;
    int m_BufferSize;
public:
    IJack(const QString& sName,const QString& sOwner,AttachModes tAttachMode,Directions tDirection,IDeviceBase* OwnerClass);
    virtual ~IJack();
    QString Name;
    QString Owner;
    CAudioBuffer* AudioBuffer;
};

class COutJack : public IJack
{
public:
    COutJack(const QString& sName,const QString& sOwner,AttachModes tAttachMode,Directions tDirection,IDeviceBase* OwnerClass,const int tProcIndex) :IJack(sName,sOwner,tAttachMode,tDirection,OwnerClass), ProcIndex(tProcIndex), ConnectCount(0), LastGet(0), LastGetP(0), LastGetA(NULL) {}
    int ProcIndex;
    inline void Connect(void) { ConnectCount++; }
    inline void Disconnect(void) { ConnectCount--; }
    inline const float GetNext(IJack* JackID)
    {
        if (ConnectCount==1) return Fetch(ProcIndex);
        else if (InJackCalls.size()==0) LastGet=Fetch(ProcIndex);
        else if (InJackCalls.Exist(JackID))
        {
            InJackCalls.clear();
            LastGet = Fetch(ProcIndex);
        }
        InJackCalls.push_back(JackID);
        return LastGet;
    }
    inline void* GetNextP(IJack* JackID)
    {
        if (ConnectCount==1) return FetchP(ProcIndex);
        else if (InJackCalls.size()==0) LastGetP=FetchP(ProcIndex);
        else if (InJackCalls.Exist(JackID))
        {
            InJackCalls.clear();
            LastGetP=FetchP(ProcIndex);
        }
        InJackCalls.push_back(JackID);
        return LastGetP;
    }
    inline float* GetNextA(IJack* JackID)
    {
        if (ConnectCount==1) return FetchA(ProcIndex);
        else if (InJackCalls.size()==0) LastGetA=FetchA(ProcIndex);
        else if (InJackCalls.Exist(JackID))
        {
            InJackCalls.clear();
            LastGetA=FetchA(ProcIndex);
        }
        InJackCalls.push_back(JackID);
        return LastGetA;
    }
private:
    int ConnectCount;
    float LastGet;
    void* LastGetP;
    float* LastGetA;
    CFastPointerList InJackCalls;
    inline float Fetch(const int ProcIndex) { return m_OwnerClass->GetNext(ProcIndex); }
    inline void* FetchP(const int ProcIndex) { return m_OwnerClass->GetNextP(ProcIndex); }
    inline float* FetchA(const int ProcIndex) { return m_OwnerClass->GetNextA(ProcIndex); }
};

class CInJack : public IJack
{
public:
    CInJack(const QString& sName,const QString& sOwner,AttachModes tAttachMode,Directions tDirection,IDeviceBase* OwnerClass) :IJack(sName,sOwner,tAttachMode,tDirection,OwnerClass), LastGetNext(0), m_OutJackCount(0), m_MIDIBuffer(NULL) {}
    ~CInJack();
    const float GetNext(void);
    void* GetNextP(void);
    inline float* GetNextA(void)
    {
        if (m_OutJackCount==0) return NULL;
        if (m_OutJackCount==1) return AudioBuffer->FromBuffer(FetchA(0, this),((COutJack*)m_OutJacks[0])->AttachMode);
        int FetchCount=0;
        for (int lTemp1=0;lTemp1<m_OutJackCount;lTemp1++)
        {
            float* B=FetchA(lTemp1, this);
            if (B != NULL)
            {
                if (FetchCount==0) AudioBuffer->FromBuffer(B,((COutJack*)m_OutJacks[lTemp1])->AttachMode);
                else AudioBuffer->AddBuffer(B,((COutJack*)m_OutJacks[lTemp1])->AttachMode);
                FetchCount++;
            }
        }
        if (FetchCount==0) return NULL;
        return AudioBuffer->Multiply(MixFactor);
    }
    inline const int OutJackCount(void) { return m_OutJackCount; }
    inline COutJack* OutJack(const int Index) { return (COutJack*)m_OutJacks[Index]; }
    void ConnectToOut(COutJack* OutJack);
    void DisconnectFromOut(COutJack* OutJack);
    bool ConnectionState(COutJack* OutJack) { return m_OutJacks.Exist(OutJack); }
private:
    CFastPointerList m_OutJacks;
    float LastGetNext;
    int m_OutJackCount;
    float MixFactor;
    inline float Fetch(const int ProcIndex, IJack *JackID) { return ((COutJack*)m_OutJacks[ProcIndex])->GetNext(JackID); }
    inline void* FetchP(const int ProcIndex, IJack *JackID) { return ((COutJack*)m_OutJacks[ProcIndex])->GetNextP(JackID); }
    inline float* FetchA(const int ProcIndex, IJack *JackID) { return ((COutJack*)m_OutJacks[ProcIndex])->GetNextA(JackID); }
    CMIDIBuffer* m_MIDIBuffer;
};


#endif // IJACK_H
