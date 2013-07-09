#ifndef CVSTHOSTCLASS_H
#define CVSTHOSTCLASS_H

#include "aeffectx.h"
#include "softsynthsclasses.h"

typedef unsigned char BYTE;

#pragma pack(push, 1)
struct
        //--------------------------------------------------------------------
        // For Preset (Program) (.fxp) without chunk (magic = 'FxCk')
        //--------------------------------------------------------------------
        fxPreset// = packed record
{
    char chunkMagic[4];// : longint;   // 'CcnK'
    int byteSize;//   : longint;   // of this chunk, excl. magic + byteSize

    char fxMagic[4]; //   : longint;   // 'FxCk'
    int version; //   : longint;
    int fxID; //      : longint;   // fx unique id
    int fxVersion; // : longint;

    int numParams; //  : longint;
    char prgName[28];//    : array[0..27] of char;
    void* params;  //   : pointer; //array[0..0] of single;    // variable no. of parameters
};

//--------------------------------------------------------------------
// For Preset (Program) (.fxp) with chunk (magic = 'FPCh')
//--------------------------------------------------------------------
struct fxChunkSet // = packed record
{
    char chunkMagic[4]; // : longint;                // 'CcnK'
    int byteSize;  //  : longint;                // of this chunk, excl. magic + byteSize

    char fxMagic[4];  //   : longint;                // 'FPCh'
    int version;   //  : longint;
    int fxID;      //  : longint;                // fx unique id
    int fxVersion;  // : longint;

    int numPrograms;// : longint;
    char prgName[28];//     : array[0..27] of char;

    int chunkSize; //  : longint;
    void* chunk;    //   : pointer; //array[0..7] of char;    // variable
};

//--------------------------------------------------------------------
// For Bank (.fxb) without chunk (magic = 'FxBk')
//--------------------------------------------------------------------
struct fxSet //= packed record
{
    char chunkMagic[4]; // : longint;                   // 'CcnK'
    int byteSize;    //: longint;                   // of this chunk, excl. magic + byteSize

    char fxMagic[4];   //  : longint;                   // 'FxBk'
    int version;   //  : longint;
    int fxID;      //  : longint;                   // fx unique id
    int fxVersion; //  : longint;

    int numPrograms; //: longint;
    BYTE future[128]; //     : array[0..127] of byte;

    void* programs;  //  : pointer;//array[0..0] of fxPreset;  // variable no. of programs
};


//--------------------------------------------------------------------
// For Bank (.fxb) with chunk (magic = 'FBCh')
//--------------------------------------------------------------------
struct fxChunkBank //= packed record
{
    char chunkMagic[4]; // : longint;                // 'CcnK'
    int byteSize;  //  : longint;                // of this chunk, excl. magic + byteSize

    char fxMagic[4];   //  : longint;                // 'FBCh'
    int version;   //  : longint;
    int fxID;      //  : longint;                // fx unique id
    int fxVersion; //  : longint;

    int numPrograms; //: longint;
    BYTE future[128]; //     : array[0..127] of byte;

    int chunkSize; //  : longint;
    void* chunk;   //    : pointer; //array[0..7] of char;    // variable
};

struct VSTRect
{
    short top;
    short left;
    short bottom;
    short right;
};
#pragma pack(pop)
class TVSTHost
{
private:
    AEffect* ptrPlug;
    float** ptrInputBuffers;
    float** ptrOutputBuffers;
    VstEvent* ptrEvents;
    //HINSTANCE libhandle;
    char* ptrEventBuffer;
    int nEvents;
    CSoftSynthsForm* m_EditForm;
    int GetChunk(void* pntr,bool isPreset);
    int SetChunk(void* data,long byteSize,bool isPreset);
    void SavePreset(QFile& str);
    void LoadBank(QFile& str);
    fxPreset GetPreset(long i);
    void LoadPreset(QFile& str);
    void SaveBank(QFile& str);
    QWidget* m_MainWindow;
    IDevice* m_Device;
    void* libhandle;
public:
    TVSTHost(void* MainWindow,IDevice* Device);
    ~TVSTHost();
    bool Load(QString FN);
    void KillPlug();
    QString FileName;
    int NumPrograms();
    int NumParams();
    int NumInputs();
    int NumOutputs();
    void Process();
    float VSTVersion();
    bool IsSynth();
    void DumpMIDI(CMIDIBuffer* MB);
    void DumpAudio(int Index,float* Buffer,int Samples);
    void GetAudio(int Index, float* Buffer, int Samples,float Volume);
    void AllNotesOff();
    bool ShowForm(bool Show);
    void RaiseForm();
    QString CurrentBank;
    QString CurrentPreset;
    int MIDIChannel;
    QString SaveXML();
    void LoadXML(QString xml);
    void LoadBank(QString FileName);
    void LoadPreset(QString FileName);
    void SaveBank(QString FileName);
    void SavePreset(QString FileName);
};

VstIntPtr VSTCALLBACK host(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);

#endif // CVSTHOSTCLASS_H
