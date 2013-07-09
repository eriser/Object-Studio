#include "cvsthostclass.h"
#include "cvsthostform.h"
#include <dlfcn.h>
#include <QFileDialog>

//QList<AEffect*> Plugs;
//void* HostDialog=NULL;

void* loadDynamicLibrary (const QString& name)
{
    return dlopen ((const char*) name.toUtf8().constData(), RTLD_LOCAL | RTLD_NOW);
}

void freeDynamicLibrary (void* handle)
{
    dlclose(handle);
}

void* getProcedureEntryPoint (void* libraryHandle, const QString& procedureName)
{
    return dlsym (libraryHandle, (const char*) procedureName.toUtf8().constData());
}

TVSTHost::TVSTHost(void *MainWindow,IDevice* Device)
{
    ptrPlug=NULL;
    ptrInputBuffers=NULL;
    ptrOutputBuffers=NULL;
    ptrEvents=NULL;
    ptrEventBuffer=NULL;
    MIDIChannel=0;
    m_MainWindow=(QWidget*)MainWindow;
    m_Device=Device;
}

TVSTHost::~TVSTHost()
{
    if (ptrPlug)
    {
        KillPlug();
        delete m_EditForm;
    }
    qDebug() << "Exit TVSTHost";
}

bool TVSTHost::Load(QString FN)
{
    //find and load the DLL and get a pointer to its main function
    //this has a protoype like this: AEffect *main (audioMasterCallback audioMaster)
    if (FileName==FN) return true;
    FileName.clear();
    AEffect* TempPlug;
    if (ptrPlug)
    {
        KillPlug();
        delete m_EditForm;
    }

    nEvents=0;

    QString filepath=FN+"/Contents/MacOS/"+QFileInfo(FN).baseName();
    qDebug() << filepath << QFileInfo(filepath).exists();
    if (!QFileInfo(filepath).exists()) filepath=FN;
    void* sdl_library = loadDynamicLibrary(filepath);
    libhandle=sdl_library;
    if(sdl_library == NULL)
    {
        qDebug() << "Not found!" << dlerror();
        return false;
    }
    else
    {
        // use the result in a call to dlsym
        qDebug() << "Found";
        //DLL was loaded OK
        AEffect* (VSTCALLBACK* getNewPlugInstance)(audioMasterCallback);

        getNewPlugInstance = (AEffect* (VSTCALLBACK*)(audioMasterCallback)) getProcedureEntryPoint (sdl_library, "VSTPluginMain");

        if (getNewPlugInstance == NULL)
        {
            getNewPlugInstance = (AEffect* (VSTCALLBACK*)(audioMasterCallback)) getProcedureEntryPoint (sdl_library, "main");
        }
        if (getNewPlugInstance != NULL)
        {
            //main function located OK
            try
            {
                TempPlug=getNewPlugInstance(host);
            }
            catch (...)
            {
                qDebug() << "Load error";
                dlclose(sdl_library);
                return false;
            }

            if (TempPlug!=NULL)
            {
                //plugin instantiated OK
                qDebug() << "Plugin was loaded OK";

                if (TempPlug->magic==kEffectMagic)
                {
                    qDebug() << "It's a valid VST plugin";
                }
                else
                {
                    qDebug() << "Not a VST plugin";
                    dlclose(sdl_library);
                    return false;
                }
            }
            else
            {
                qDebug() << "Plugin could not be instantiated";
                dlclose(sdl_library);
                return false;
            }
        }
        else
        {
            qDebug() << "Plugin main function could not be located";
            dlclose(sdl_library);
            return false;
        }
    }
    //Plugs.append(TempPlug);

    FileName=FN;
    //switch the plugin off (calls Suspend)
    TempPlug->dispatcher(TempPlug,effMainsChanged,0,0,NULL,0.0f);

    //Must have handle of new window!
    m_EditForm=new CVSTHostForm(m_Device,m_MainWindow);

    char strEffName[32];
    if (TempPlug->dispatcher(TempPlug,effGetEffectName,0,0,strEffName,0.0f))
    {
        //m_EditForm->Caption=strEffName;
    }
    else
    {
        // m_EditForm->Caption=ExtractFileName(FN);
    }


    ((CVSTHostForm*)m_EditForm)->Init(TempPlug,this);
    //m_EditForm->show();

    ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Plug-In Loaded, OK");
    //set sample rate and block size
    TempPlug->dispatcher(TempPlug,effSetSampleRate,0,0,NULL,CPresets::Presets.SampleRate);
    TempPlug->dispatcher(TempPlug,effSetBlockSize,0,CPresets::Presets.ModulationRate,NULL,0.0f);

    if ((TempPlug->dispatcher(TempPlug,effGetVstVersion,0,0,NULL,0.0f)==2) &&
            (TempPlug->flags & effFlagsIsSynth))
    {
        //get I/O configuration for synth plugins - they will declare their
        //own output and input channels
        for (int i=0; i<TempPlug->numInputs+TempPlug->numOutputs;i++)
        {
            if (i<TempPlug->numInputs)
            {
                //input pin
                VstPinProperties temp;

                if (TempPlug->dispatcher(TempPlug,effGetInputProperties,i,0,&temp,0.0f)==1)
                {
                    ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Input pin " + QString::number(i+1) + " label " + QString(temp.label));

                    if (temp.flags & kVstPinIsActive)
                    {
                        ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Input pin " + QString::number(i+1) + " is active");
                    }

                    if (temp.flags & kVstPinIsStereo)
                    {
                        // is index even or zero?
                        if (i%2==0 || i==0)
                        {
                            ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Input pin " + QString::number(i+1) + " is left channel of a stereo pair");
                        }
                        else
                        {
                            ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Input pin " + QString::number(i+1) + " is right channel of a stereo pair");
                        }
                    }
                }
            }
            else
            {
                //output pin
                VstPinProperties temp;

                if (TempPlug->dispatcher(TempPlug,effGetOutputProperties,i,0,&temp,0.0f)==1)
                {
                    ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Output pin " + QString::number(i-TempPlug->numInputs+1) + " label " + QString(temp.label));

                    if (temp.flags & kVstPinIsActive)
                    {
                        ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Output pin " + QString::number(i-TempPlug->numInputs+1) + " is active");
                    }
                    else
                    {
                        ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Output pin " + QString::number(i-TempPlug->numInputs+1) + " is inactive");
                    }

                    if (temp.flags & kVstPinIsStereo)
                    {
                        // is index even or zero?
                        if ((i-TempPlug->numInputs)%2==0 || (i-TempPlug->numInputs)==0)
                        {
                            ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Output pin " + QString::number(i+1) + " is left channel of a stereo pair");
                        }
                        else
                        {
                            ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Output pin " + QString::number(i+1) + " is right channel of a stereo pair");
                        }
                    }
                    else
                    {
                        ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Output pin " + QString::number(i+1) + " is mono");
                    }
                }
            }
        }
    }	//end VST2 specific

    //switch the plugin back on (calls Resume)
    TempPlug->dispatcher(TempPlug,effMainsChanged,0,1,NULL,0.0f);

    //SetUp Buffers
    if (TempPlug->numInputs)
    {
        //Plug requires independent input signals
        ptrInputBuffers=new float*[TempPlug->numInputs];

        //create the input buffers
        for (int i=0;i<TempPlug->numInputs;i++)
        {
            ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Input buffer " + QString::number(i+1) + " created");
            ptrInputBuffers[i]=new float[CPresets::Presets.ModulationRate];
            ZeroMemory(ptrInputBuffers[i],CPresets::Presets.ModulationRate*sizeof(float));
        }
    }

    if (TempPlug->numOutputs)
    {
        ptrOutputBuffers=new float*[TempPlug->numOutputs];

        //create the output buffers
        for (int i=0;i<TempPlug->numOutputs;i++)
        {
            ((CVSTHostForm*)m_EditForm)->AddStatusInfo("Output buffer " + QString::number(i+1) + " created");
            ptrOutputBuffers[i]=new float[CPresets::Presets.ModulationRate];
            ZeroMemory(ptrOutputBuffers[i],CPresets::Presets.ModulationRate*sizeof(float));
        }
    }

    ptrPlug=TempPlug;
    return true;
}

//host callback function
//this is called directly by the plug-in!!
//
VstIntPtr VSTCALLBACK host(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr /*value*/, void* ptr, float opt)
{
    char S[1024];
    char** FileStrings=NULL;
    int nFileStrings=0;
    VstTimeInfo VTI;
    long retval=0;
    VstFileSelect* FS=(VstFileSelect*)ptr;
    //QFileDialog OD;
    QStringList FileNames;
    QString FN;
    QString Filter;
    VstFileType* FT;

    switch (opcode)
    {
    //VST 1.0 opcodes
    case audioMasterVersion:
        //Input values:
        //none

        //Return Value:
        //0 or 1 for old version
        //2 or higher for VST2.0 host?
        //Debug("plug called audioMasterVersion");
        retval=2400;
        break;

    case audioMasterAutomate:
        //Input values:
        //<index> parameter that has changed
        //<opt> new value

        //Return value:
        //not tested, always return 0

        //NB - this is called when the plug calls
        //setParameterAutomated

        //Debug("plug called audioMasterAutomate");
        effect->setParameter(effect,index,opt);
        break;

    case audioMasterCurrentId:
        //Input values:
        //none

        //Return Value
        //the unique id of a plug that's currently loading
        //zero is a default value and can be safely returned if not known
        //Debug("plug called audioMasterCurrentId");
        retval=effect->uniqueID;
        break;
    case audioMasterIdle:
        //Input values:
        //none

        //Return Value
        //not tested, always return 0

        //NB - idle routine should also call effEditIdle for all open editors
        //Sleep(1);
        //Debug("plug called audioMasterIdle");
        /* 2.4
                        effect->dispatcher(effect,effIdle,0,0,NULL,0.0f);
                        for (int i=0;i<Plugs.count();i++)
                        {
                            AEffect* E=Plugs[i];
                            if (E->flags & effFlagsHasEditor)
                            {
                                E->dispatcher(E,effEditIdle,0,0,NULL,0.0f);
                            }
                        }
                        */
        break;
        /* 2.4
                case audioMasterPinConnected:
                        //Input values:
                        //<index> pin to be checked
                        //<value> 0=input pin, non-zero value=output pin

                        //Return values:
                        //0=true, non-zero=false
                        //Debug("plug called audioMasterPinConnected");
                        break;

                //VST 2.0 opcodes
                case audioMasterWantMidi:
                        //Input Values:
                        //<value> filter flags (which is currently ignored, no defined flags?)

                        //Return Value:
                        //not tested, always return 0
                        //Debug("plug called audioMasterWantMidi");
                        break;
*/
    case audioMasterGetTime:
        //Input Values:
        //<value> should contain a mask indicating which fields are required
        //...from the following list?
        //kVstNanosValid
        //kVstPpqPosValid
        //kVstTempoValid
        //kVstBarsValid
        //kVstCyclePosValid
        //kVstTimeSigValid
        //kVstSmpteValid
        //kVstClockValid

        //Return Value:
        //ptr to populated const VstTimeInfo structure (or 0 if not supported)

        VTI.sampleRate =  CPresets::Presets.SampleRate;
        VTI.timeSigNumerator = 4;
        VTI.timeSigDenominator = 4;
        VTI.smpteFrameRate = 1;
        VTI.samplePos = 0;
        VTI.ppqPos = 0;
        //Flags := [vtiNanosValid, vtiPpqPosValid, vtiTempoValid, vtiBarsValid,
        // vtiCyclePosValid, vtiTimeSigValid, vtiSmpteValid, vtiClockValid];
        retval=(long)&VTI;
        //NB - this structure will have to be held in memory for long enough
        //for the plug to safely make use of it
        //Debug("plug called audioMasterGetTime");
        break;

    case audioMasterProcessEvents:
        //Input Values:
        //<ptr> Pointer to a populated VstEvents structure

        //Return value:
        //0 if error
        //1 if OK
        //Debug("plug called audioMasterProcessEvents");
        break;
        /* 2.4
                case audioMasterSetTime:
                        //IGNORE!
                        break;

                case audioMasterTempoAt:
                        //Input Values:
                        //<value> sample frame location to be checked

                        //Return Value:
                        //tempo (in bpm * 10000)
                        //Debug("plug called audioMasterTempoAt");
                        retval= 120*10000;
                        break;

                case audioMasterGetNumAutomatableParameters:
                        //Input Values:
                        //None

                        //Return Value:
                        //number of automatable parameters
                        //zero is a default value and can be safely returned if not known

                        //NB - what exactly does this mean? can the host set a limit to the
                        //number of parameters that can be automated?
                        //Debug("plug called audioMasterGetNumAutomatableParameters");
                        break;

                case audioMasterGetParameterQuantization:
                        //Input Values:
                        //None

                        //Return Value:
                        //integer value for +1.0 representation,
                        //or 1 if full single float precision is maintained
                        //in automation.

                        //NB - ***possibly bugged***
                        //Steinberg notes say "parameter index in <value> (-1: all, any)"
                        //but in aeffectx.cpp no parameters are taken or passed
                        //Debug("plug called audioMasterGetParameterQuantization");
                        break;
*/
    case audioMasterIOChanged:
        //Input Values:
        //None

        //Return Value:
        //0 if error
        //non-zero value if OK
        //Debug("plug called audioMasterIOChanged");
        break;
        /* 2.4
                case audioMasterNeedIdle:
                        //Input Values:
                        //None

                        //Return Value:
                        //0 if error
                        //non-zero value if OK

                        //NB plug needs idle calls (outside its editor window)
                        //this means that effIdle must be dispatched to the plug
                        //during host idle process and not effEditIdle calls only when its
                        //editor is open
                        //Check despatcher notes for any return codes from effIdle
                        //Debug("plug called audioMasterNeedIdle");
                        effect->dispatcher(effect,effIdle,0,0,NULL,0.0f);
                        if (effect->flags & effFlagsHasEditor)
                        {
                            effect->dispatcher(effect,effEditIdle,0,0,NULL,0.0f);
                        }
                        retval=1;
                        break;
*/
    case audioMasterSizeWindow:
        //Input Values:
        //<index> width
        //<value> height

        //Return Value:
        //0 if error
        //non-zero value if OK
        //Debug("plug called audioMasterSizeWindow");
        break;

    case audioMasterGetSampleRate:
        //Input Values:
        //None

        //Return Value:
        //not tested, always return 0

        //NB - Host must despatch effSetSampleRate to the plug in response
        //to this call
        //Check despatcher notes for any return codes from effSetSampleRate
        //Debug("plug called audioMasterGetSampleRate");
        //TempF=Presets().SampleRate;
        //effect->dispatcher(effect,effSetSampleRate,0,0,NULL,TempF);
        retval=CPresets::Presets.SampleRate;
        break;

    case audioMasterGetBlockSize:
        //Input Values:
        //None

        //Return Value:
        //not tested, always return 0

        //NB - Host must despatch effSetBlockSize to the plug in response
        //to this call
        //Check despatcher notes for any return codes from effSetBlockSize
        //Debug("plug called audioMasterGetBlockSize");
        //TempL=Presets().ModulationRate;
        //effect->dispatcher(effect,effSetBlockSize,0,TempL,NULL,0.0f);
        retval=CPresets::Presets.ModulationRate;
        break;

    case audioMasterGetInputLatency:
        //Input Values:
        //None

        //Return Value:
        //input latency (in sampleframes?)
        //Debug("plug called audioMasterGetInputLatency");
        break;

    case audioMasterGetOutputLatency:
        //Input Values:
        //None

        //Return Value:
        //output latency (in sampleframes?)
        //Debug("plug called audioMasterGetOutputLatency");
        break;
        /* 2.4
                case audioMasterGetPreviousPlug:
                        //Input Values:
                        //None

                        //Return Value:
                        //pointer to AEffect structure or NULL if not known?

                        //NB - ***possibly bugged***
                        //Steinberg notes say "input pin in <value> (-1: first to come)"
                        //but in aeffectx.cpp no parameters are taken or passed
                        //Debug("plug called audioMasterGetPreviousPlug");
                        break;

                case audioMasterGetNextPlug:
                        //Input Values:
                        //None

                        //Return Value:
                        //pointer to AEffect structure or NULL if not known?

                        //NB - ***possibly bugged***
                        //Steinberg notes say "output pin in <value> (-1: first to come)"
                        //but in aeffectx.cpp no parameters are taken or passed
                        //Debug("plug called audioMasterGetNextPlug");
                        break;

                case audioMasterWillReplaceOrAccumulate:
                        //Input Values:
                        //None

                        //Return Value:
                        //0: not supported
                        //1: replace
                        //2: accumulate
                        //Debug("plug called audioMasterWillReplaceOrAccumulate");
                        retval=1;
                        break;
*/
    case audioMasterGetCurrentProcessLevel:
        //Input Values:
        //None

        //Return Value:
        //0: not supported,
        //1: currently in user thread (gui)
        //2: currently in audio thread (where process is called)
        //3: currently in 'sequencer' thread (midi, timer etc)
        //4: currently offline processing and thus in user thread
        //other: not defined, but probably pre-empting user thread.
        //Debug("plug called audioMasterGetCurrentProcessLevel");
        break;

    case audioMasterGetAutomationState:
        //Input Values:
        //None

        //Return Value:
        //0: not supported
        //1: off
        //2:read
        //3:write
        //4:read/write
        //Debug("plug called audioMasterGetAutomationState");
        break;

    case audioMasterGetVendorString:
        //Input Values:
        //<ptr> string (max 64 chars) to be populated

        //Return Value:
        //0 if error
        //non-zero value if OK
        //Debug("plug called audioMasterGetVendorString");
        ZeroMemory(S,1024);
        strcpy (S, "Veinge Musik och Data");
        ptr=S;
        retval=1;
        break;

    case audioMasterGetProductString:
        //Input Values:
        //<ptr> string (max 64 chars) to be populated

        //Return Value:
        //0 if error
        //non-zero value if OK
        //Debug("plug called audioMasterGetProductString");
        ZeroMemory(S,1024);
        strcpy (S, "Object Studio");
        ptr=S;
        retval=1;
        break;

    case audioMasterGetVendorVersion:
        //Input Values:
        //None

        //Return Value:
        //Vendor specific host version as integer
        //Debug("plug called audioMasterGetVendorVersion");
        retval=1;
        break;

    case audioMasterVendorSpecific:
        //Input Values:
        //<index> lArg1
        //<value> lArg2
        //<ptr> ptrArg
        //<opt>	floatArg

        //Return Values:
        //Vendor specific response as integer
        //Debug("plug called audioMasterVendorSpecific");
        break;
        /* 2.4
                case audioMasterSetIcon:
                        //IGNORE
                        break;
*/
    case audioMasterCanDo:
        //Input Values:
        //<ptr> predefined "canDo" string

        //Return Value:
        //0 = Not Supported
        //non-zero value if host supports that feature

        //NB - Possible Can Do strings are:
        //"sendVstEvents",
        //"sendVstMidiEvent",
        //"sendVstTimeInfo",
        //"receiveVstEvents",
        //"receiveVstMidiEvent",
        //"receiveVstTimeInfo",
        //"reportConnectionChanges",
        //"acceptIOChanges",
        //"sizeWindow",
        //"asyncProcessing",
        //"offline",
        //"supplyIdle",
        //"supportShell"
        //Debug("plug called audioMasterCanDo" + AnsiString((char*)ptr));

        if (strcmp((char*)ptr,"sendVstEvents")==0 ||
                strcmp((char*)ptr,"sendVstMidiEvent")==0 ||
                strcmp((char*)ptr,"sendVstTimeInfo")==0 ||
                strcmp((char*)ptr,"asyncProcessing")==0 ||
                strcmp((char*)ptr,"offline")==0 ||
                strcmp((char*)ptr,"supplyIdle")==0)
        {
            retval=1;
        }
        else
        {
            retval=0;
        }

        break;

    case audioMasterGetLanguage:
        //Input Values:
        //None

        //Return Value:
        //kVstLangEnglish
        //kVstLangGerman
        //kVstLangFrench
        //kVstLangItalian
        //kVstLangSpanish
        //kVstLangJapanese
        //Debug("plug called audioMasterGetLanguage");
        retval=kVstLangEnglish;
        break;
        /*
                MAC SPECIFIC?

                case audioMasterOpenWindow:
                        //Input Values:
                        //<ptr> pointer to a VstWindow structure

                        //Return Value:
                        //0 if error
                        //else platform specific ptr
                        Debug("plug called audioMasterOpenWindow");
                        break;

                case audioMasterCloseWindow:
                        //Input Values:
                        //<ptr> pointer to a VstWindow structure

                        //Return Value:
                        //0 if error
                        //Non-zero value if OK
                        Debug("plug called audioMasterCloseWindow");
                        break;
*/
    case audioMasterGetDirectory:
        //Input Values:
        //None

        //Return Value:
        //0 if error
        //FSSpec on MAC, else char* as integer

        //NB Refers to which directory, exactly?
        //Debug("plug called audioMasterGetDirectory");
        ZeroMemory(S,1024);
        strcpy (S, CPresets::Presets.VSTPath.toUtf8().constData());
        retval=(long)&S[0];
        break;

    case audioMasterUpdateDisplay:
        //Input Values:
        //None

        //Return Value:
        //Unknown
        //Debug("plug called audioMasterUpdateDisplay");
        if (effect->flags & effFlagsHasEditor)
        {
            effect->dispatcher(effect,effEditIdle,0,0,NULL,0.0f);
        }
        break;
        //---from here VST 2.1 extension opcodes------------------------------------------------------
    case audioMasterBeginEdit:
        // begin of automation session (when mouse down), parameter index in <index>
        //Debug("plug called audioMasterBeginEdit");
        break;
    case audioMasterEndEdit:
        // end of automation session (when mouse up),     parameter index in <index>
        //Debug("plug called audioMasterEndEdit");
        break;
    case audioMasterOpenFileSelector:
        // open a fileselector window with VstFileSelect* in <ptr>
        if (ptr) // && !HostDialog)
        {
            switch (FS->type)
            {
            case kVstFileLoad:
                FT=(VstFileType*)FS->fileTypes;
                for (int i=0;i<FS->nbFileTypes;i++)
                {
                    Filter = QString(QString(FT[i].name) + " (*." +
                                     QString(FT[i].dosType) + ")|*." +
                                     QString(FT[i].dosType) + "|");
                }
                FN=QFileDialog::getOpenFileName(0,FS->title,FS->initialPath,Filter);
                if (!FN.isEmpty())
                {
                    ZeroMemory(S,1024);
                    strcpy(S,FN.toUtf8().constData());
                    FS->returnPath=S;
                    FS->sizeReturnPath=FN.length();
                }
                break;
            case kVstFileSave:
                FT=(VstFileType*)FS->fileTypes;
                for (int i=0;i<FS->nbFileTypes;i++)
                {
                    Filter = QString(QString(FT[i].name) + " (*." +
                                     QString(FT[i].dosType) + ")|*." +
                                     QString(FT[i].dosType) + "|");
                }
                FN=QFileDialog::getSaveFileName(0,FS->title,FS->initialPath,Filter);
                if (!FN.isEmpty())
                {
                    ZeroMemory(S,1024);
                    strcpy(S,FN.toUtf8().constData());
                    FS->returnPath=S;
                    FS->sizeReturnPath=FN.length();
                }
                break;
            case kVstMultipleFilesLoad:
                FT=(VstFileType*)FS->fileTypes;
                for (int i=0;i<FS->nbFileTypes;i++)
                {
                    Filter = QString(QString(FT[i].name) + " (*." +
                                     QString(FT[i].dosType) + ")|*." +
                                     QString(FT[i].dosType) + "|");
                }
                FileNames=QFileDialog::getOpenFileNames(0,FS->title,FS->initialPath,Filter);
                if (FileNames.count())
                {
                    nFileStrings=FileNames.count();
                    FileStrings=new char*[nFileStrings];
                    for (int i=0;i<nFileStrings;i++)
                    {
                        FileStrings[i]=new char[FileNames[i].length()];
                        strcpy(FileStrings[i],FileNames[i].toUtf8().constData());
                    }
                    FS->nbReturnPath=nFileStrings;
                    FS->returnMultiplePaths=FileStrings;
                }
                break;
            case kVstDirectorySelect:
                FN=QFileDialog::getExistingDirectory(0,FS->title,FS->initialPath);
                if (!FN.isEmpty())
                {
                    ZeroMemory(S,1024);
                    strcpy(S,FN.toUtf8().constData());
                    FS->returnPath=S;
                    FS->sizeReturnPath=FN.length();
                }
                break;

            }
            if (!FS->returnPath)
            {
                retval=1;
            }
        }
        //Debug("plug called audioMasterOpenFileSelector");
        break;
        //---from here VST 2.2 extension opcodes------------------------------------------------------
    case audioMasterCloseFileSelector:
        // close a fileselector operation with VstFileSelect* in <ptr>
        // Must be always called after an open !
        /*
                        if (HostDialog && ptr)
                        {
                            switch (FS->type)
                            {
                                case kVstFileLoad:
                                    OD=(TOpenDialog*)HostDialog;
                                    if (OD->Title.AnsiCompare(FS->title)==0)
                                    {
                                        delete OD;
                                        //OD=NULL;
                                    }
                                    break;
                                case kVstFileSave:
                                    SD=(TSaveDialog*)HostDialog;
                                    if (SD->Title.AnsiCompare(FS->title)==0)
                                    {
                                        delete SD;
                                        //SD=NULL;
                                    }
                                    break;
                                case kVstMultipleFilesLoad:
                                    OD=(TOpenDialog*)HostDialog;
                                    if (OD->Title.AnsiCompare(FS->title)==0)
                                    {
                                        delete OD;
                                        //OD=NULL;
                                    }
                                    break;
                                case kVstDirectorySelect:
                                    BD=(TLMDBrowseDlg*)HostDialog;
                                    if (BD->CaptionTitle.AnsiCompare(FS->title)==0)
                                    {
                                        delete BD;
                                        //BD=NULL;
                                    }
                                    break;
                            }
                        }
                        */
        if (FileStrings)
        {
            for (int i=0;i<nFileStrings;i++)
            {
                delete[] FileStrings[i];
            }
            delete[] FileStrings;
            FileStrings=NULL;
        }

        //Debug("plug called audioMasterCloseFileSelector");
        break;
        /* 2.4
                case audioMasterEditFile:
                                                // open an editor for audio (defined by XML text in ptr)
                        //Debug("plug called audioMasterEditFile");
                        break;
                case audioMasterGetChunkFile:
                                        // get the native path of currently loading bank or project
                                        // (called from writeChunk) void* in <ptr> (char[2048], or sizeof(FSSpec))
                        //Debug("plug called audioMasterGetChunkFile");
                        break;
*/
    }

    return retval;
};


int TVSTHost::NumPrograms()
{
    if (ptrPlug)
    {
        return ptrPlug->numPrograms;
    }
    return 0;
}

int TVSTHost::NumParams()
{
    if (ptrPlug)
    {
        return ptrPlug->numParams;
    }
    return 0;
}

int TVSTHost::NumInputs()
{
    if (ptrPlug)
    {
        return ptrPlug->numInputs;
    }
    return 0;
}

int TVSTHost::NumOutputs()
{
    if (ptrPlug)
    {
        return ptrPlug->numOutputs;
    }
    return 0;
}

float TVSTHost::VSTVersion()
{
    if  (!ptrPlug){return 0;}
    return ptrPlug->dispatcher(ptrPlug,effGetVstVersion,0,0,NULL,0.0f);
}

bool TVSTHost::IsSynth()
{
    //if ((ptrPlug->dispatcher(ptrPlug,effGetVstVersion,0,0,NULL,0.0f)==2) && (ptrPlug->flags & effFlagsIsSynth))
    //{
    return true;
    //}
    //return false;
}

void TVSTHost::Process()
{
    if (ptrPlug)
    {
        ptrPlug->dispatcher(ptrPlug,effStartProcess,0,0,NULL,0.0f);
        // Called before the start of process call

        //ProcessEvents
        if (nEvents)
        {
            //Some plugs can receive but doesn't return non zero on receiveVstEvents, so send anyway
            //if ((ptrPlug->dispatcher(ptrPlug,effGetVstVersion,0,0,NULL,0.0f)==2) &&
            //   ((ptrPlug->flags & effFlagsIsSynth) ||
            //	(ptrPlug->dispatcher(ptrPlug,effCanDo,0,0,"receiveVstEvents",0.0f)>0)))
            //{
            if (ptrPlug->dispatcher(ptrPlug,effProcessEvents,0,0,(VstEvents*)ptrEventBuffer,0.0f)==1)
            {
                //Debug("plug processed events OK and wants more");
            }
            //else
            //{
            //Debug("plug does not want any more events");
            //}
            //}
        }

        //Some plugs don't replace even if processReplacing is called so we must flush buffers
        //Some people don't do that for you !!!
        /*
                for (int i=0;i<ptrPlug->numOutputs;i++)
                {
                        ZeroMemory(ptrOutputBuffers[i],ModRate*sizeof(float));
                }
                */
        //process (replacing)
        //if (ptrPlug->flags & effFlagsCanReplacing)
        //{
        ptrPlug->processReplacing(ptrPlug,ptrInputBuffers,ptrOutputBuffers,CPresets::Presets.ModulationRate);
        /* 2.4
                }
                else
                {
                        ptrPlug->process(ptrPlug,ptrInputBuffers,ptrOutputBuffers,ModRate);
                }
                */
        //((TfrmVSTHost*)m_EditForm)->EditIdle();
        // Called after the stop of process call
        ptrPlug->dispatcher(ptrPlug,effStopProcess,0,0,NULL,0.0f);
    }
}

void TVSTHost::DumpMIDI(CMIDIBuffer* MB)
{
    if (!ptrPlug)
    {
        return;
    }
    QList<VstMidiEvent*> Events;
    VstMidiEvent* ptrWrite;
    short Message;
    QByteArray data;
    MB->StartRead();
    int lTemp=MB->Read();
    while (lTemp > -1)
    {
        if (lTemp >= 0x80)
        {
            Message=lTemp;
            data.clear();
        }
        forever
        {
            lTemp=MB->Read();
            if ((lTemp >= 0x80) | (lTemp < 0)) break;
            data.append(lTemp);
        }
        short Channel=Message & 0x0F;
        if ((Message & 0xF0) == 0x90)
        {
            if (MIDIChannel==0 || MIDIChannel-1==Channel)
            {
                ptrWrite=new VstMidiEvent;
                ptrWrite->type=kVstMidiType;
                ptrWrite->byteSize=24L;
                ptrWrite->deltaFrames=0L;
                ptrWrite->flags=0L;
                ptrWrite->noteLength=0L;
                ptrWrite->noteOffset=0L;

                ptrWrite->midiData[0]=(char)Message;	//status & channel
                ptrWrite->midiData[1]=(char)data[0];	//MIDI byte #2
                ptrWrite->midiData[2]=(char)data[1];	//MIDI byte #3
                ptrWrite->midiData[3]=(char)0x00;	//MIDI byte #4 - blank

                ptrWrite->detune=0x00;
                ptrWrite->noteOffVelocity=0x00;
                ptrWrite->reserved1=0x00;
                ptrWrite->reserved2=0x00;
                Events.append(ptrWrite);
                /*
                                        OutBuffer->Push(((TfrmMIDIChannelRouter*)FM)->RouteTo[Message-0x90]+0x90);
                                        OutBuffer->Push(Data1);
                                        float Vel=(float)Data2;
                                        Vel=Vel*(float)(((TfrmMIDIChannelRouter*)FM)->VelScale[Message-0x90]);
                                        Vel=Vel*0.01;
                                        OutBuffer->Push(Vel);
                                        */
            }
        }
        else if ((Message & 0xF0)==0x80)
        {
            if (MIDIChannel==0 || MIDIChannel-1==Channel)
            {
                ptrWrite=new VstMidiEvent;
                ptrWrite->type=kVstMidiType;
                ptrWrite->byteSize=24L;
                ptrWrite->deltaFrames=0L;
                ptrWrite->flags=0L;
                ptrWrite->noteLength=0L;
                ptrWrite->noteOffset=0L;

                ptrWrite->midiData[0]=(char)Message;	//status & channel
                ptrWrite->midiData[1]=(char)data[0];	//MIDI byte #2
                ptrWrite->midiData[2]=(char)data[1];	//MIDI byte #3
                ptrWrite->midiData[3]=(char)0x00;	//MIDI byte #4 - blank

                ptrWrite->detune=0x00;
                ptrWrite->noteOffVelocity=0x00;
                ptrWrite->reserved1=0x00;
                ptrWrite->reserved2=0x00;
                Events.append(ptrWrite);
                /*
                                        OutBuffer->Push(((TfrmMIDIChannelRouter*)FM)->RouteTo[Message-0x80]+0x80);
                                        OutBuffer->Push(Data1);
                                        float Vel=(float)Data2;
                                        Vel=Vel*(float)(((TfrmMIDIChannelRouter*)FM)->VelScale[Message-0x80]);
                                        Vel=Vel*0.01;
                                        OutBuffer->Push(Vel);
                                        */
            }
        }
    }

    //create a block of appropriate size
    nEvents=Events.count();
    int pointersize=sizeof(VstEvent*);
    int bufferSize=sizeof(VstEvents)-(pointersize*2);
    bufferSize+=pointersize*(nEvents);
    //create the buffer
    ptrEventBuffer=new char[bufferSize+1];

    //now, create some memory for the events themselves
    VstMidiEvent* ptrEvents=new VstMidiEvent[nEvents];
    ptrWrite=ptrEvents;
    for (int i=0;i<nEvents;i++)
    {
        *ptrWrite=*(Events[i]);
        ptrWrite++;
    }
    //copy the addresses of our events into the eventlist structure
    VstEvents* ev=(VstEvents*)ptrEventBuffer;
    for (int i=0;i<nEvents;i++)
    {
        ev->events[i]=(VstEvent*)(ptrEvents+i);
    }

    //do the block header
    ev->numEvents=nEvents;
    ev->reserved=0L;
    qDeleteAll(Events);
    /*
        for (int i=nEvents-1;i>-1;i--)
        {
                ptrWrite=Events[i];
                Events.removeOne(ptrWrite);
                delete ptrWrite;
        }
        */

}

void TVSTHost::DumpAudio(int Index,float* Buffer,int Samples)
{
    if (ptrPlug)
    {
        if (Index < ptrPlug->numInputs )
        {
            if (Buffer)
            {
                //We don't have to do this for synths, obviously
                CopyMemory(ptrInputBuffers[Index],Buffer,Samples*sizeof(float));
            }
            else
            {
                ZeroMemory(ptrInputBuffers[Index],Samples*sizeof(float));
                return;
            }

        }
    }
}

void TVSTHost::GetAudio(int Index, float* Buffer, int Samples,float Volume)
{
    if (ptrPlug)
    {
        if (Index < ptrPlug->numOutputs)
        {
            for (int j=0;j<Samples;j++)
            {
                Buffer[j]= ptrOutputBuffers[Index][j]*Volume;
            }
        }
    }
    else
    {
        ZeroMemory(Buffer,Samples*sizeof(float));
    }
}

void TVSTHost::KillPlug()
{
    if (!ptrPlug){return;}
    AEffect* TempPlug=ptrPlug;
    ptrPlug=NULL;
    //Plugs.removeOne(TempPlug);
    if (TempPlug->flags & effFlagsHasEditor)
    {
        TempPlug->dispatcher(TempPlug,effEditClose,0,0,NULL,0.0f);
    }
    qDebug() << 1;
    //m_EditForm->Hide();
    m_Device->Execute(false);
    m_Device->Load(QString());
    ((CVSTHostForm*)m_EditForm)->StopTimer();
    //m_EditForm->deleteLater();
    qDebug() << 2;

    ////////////////////////////////////////////////////////////////////////////

    TempPlug->dispatcher(TempPlug,effMainsChanged,0,0,NULL,0.0f);	//calls suspend

    ////////////////////////////////////////////////////////////////////////////

    //delete the MIDI data
    if (ptrEventBuffer!=NULL)
    {
        delete[] ptrEventBuffer;
    }

    if (ptrEvents!=NULL)
    {
        delete[] ptrEvents;
    }
    qDebug() << 3;

    //delete the input buffers
    if (TempPlug->numInputs>0)
    {
        //if (!(TempPlug->flags & effFlagsCanMono))
        //{
        //independent buffers have been assigned, delete them
        for (int i=0;i<TempPlug->numInputs;i++)
        {
            delete[] ptrInputBuffers[i];
        }
        /* 2.4
                }
                else
                {
                        //there's only one buffer, delete it
                        delete[] ptrInputBuffers[0];
                }
*/
        //remove the pointers to the buffers
        delete[] ptrInputBuffers;
    }

    qDebug() << 4;

    //delete the output buffers
    if (TempPlug->numOutputs>0)
    {
        for (int i=0;i<TempPlug->numOutputs;i++)
        {
            delete[] ptrOutputBuffers[i];
        }

        //remove the pointers to the buffers
        delete[] ptrOutputBuffers;
    }

    qDebug() << 5;

    //Shut the plugin down and free the library (this deletes the C++ class
    //memory and calls the appropriate destructors...)
    TempPlug->dispatcher(TempPlug,effClose,0,0,NULL,0.0f);
    ptrInputBuffers=NULL;
    ptrOutputBuffers=NULL;
    ptrEvents=NULL;
    ptrEventBuffer=NULL;
    CurrentBank.clear();
    CurrentPreset.clear();
    FileName.clear();
    dlclose(libhandle);

    m_Device->UpdateHost();
    qDebug() << 6;
    ////////////////////////////////////////////////////////////////////////////
}

void TVSTHost::AllNotesOff()
{
    if (ptrPlug)
    {
        CMIDIBuffer b;
        b.Reset();
        for (int j=0;j<16;j++)
        {
            for (int i=0;i<127;i++)
            {
                b.Push(0x80+j);
                b.Push(i);
                b.Push(0);
            }
        }
        DumpMIDI(&b);
        /*
                //switch the plugin off (calls Suspend)
                ptrPlug->dispatcher(ptrPlug,effMainsChanged,0,0,NULL,0.0f);
                for (int i=0;i<ptrPlug->numOutputs;i++)
                {
                        ZeroMemory(ptrOutputBuffers[i],ModRate*sizeof(float));
                }
                ptrPlug->dispatcher(ptrPlug,effMainsChanged,0,1,NULL,0.0f);
                */
    }
}

void TVSTHost::RaiseForm()
{
    if (ptrPlug)
    {
        if (m_EditForm->isVisible())
        {
            m_EditForm->raise();
            m_EditForm->activateWindow();
        }
    }
}

bool TVSTHost::ShowForm(bool Show)
{
    if (ptrPlug)
    {
        if (Show)
        {
            m_EditForm->show();
        }
        else
        {
            m_EditForm->setVisible(false);
        }
        return true;
    }
    return false;
}

QString TVSTHost::SaveXML()
{
    if (ptrPlug)
    {
        return m_EditForm->Save();
    }
    return "<Custom></Custom>";
}

void TVSTHost::LoadXML(QString xml)
{
    if (ptrPlug)
    {
        m_EditForm->Load(xml);
    }
}

void TVSTHost::LoadBank(QString FileName)
{
    QFile f(FileName);
    if (!f.exists())
    {
        qDebug() << "Bank does not exist";
        return;
    }
    if (f.open(QIODevice::ReadOnly))
    {
        LoadBank(f);
        if (CurrentBank.isEmpty())
        {
            CurrentBank=FileName;
        }
    }
}

void TVSTHost::LoadPreset(QString FileName)
{
    QFile f(FileName);
    if (!f.exists())
    {
        qDebug() << "Preset does not exist";
        return;
    }
    if (f.open(QIODevice::ReadOnly))
    {
        LoadPreset(f);
        if (CurrentPreset.isEmpty())
        {
            CurrentPreset=FileName;
        }
    }
}

fxPreset TVSTHost::GetPreset(long Index)
{
    fxPreset result;
    ptrPlug->dispatcher(ptrPlug,effSetProgram,0,Index,NULL,0.0f);
    setDescriptor(result.chunkMagic,"CcnK");
    setDescriptor(result.fxMagic,"FxCk");
    result.version=qToBigEndian<qint32>(1);
    result.fxID=qToBigEndian<qint32>(ptrPlug->uniqueID);
    result.fxVersion=qToBigEndian<qint32>((int)ptrPlug->version);
    result.numParams=qToBigEndian<qint32>((int)ptrPlug->numParams);
    char temp[256];
    ptrPlug->dispatcher(ptrPlug,effGetProgramName,0,0,temp,0.0f);
    CopyMemory(result.prgName,temp,256);
    result.params=new float[ptrPlug->numParams];//calloc(ptrPlug->numParams,sizeof(float));
    int* pc=(int*)result.params;
    for (long i=0;i<ptrPlug->numParams;i++)
    {
        float si=ptrPlug->getParameter(ptrPlug,i);
        int x=*((int*)&si);
        x=qToBigEndian<qint32>(x);
        *pc=x;
        pc++;
    }
    result.byteSize=qToBigEndian<qint32>(sizeof(result)-sizeof(int)*2+(NumParams()-1)*sizeof(float));
    return result;
}

void TVSTHost::SaveBank(QString FileName)
{
    QFile f(FileName);
    if (f.open(QIODevice::WriteOnly))
    {
        SaveBank(f);
        CurrentBank=FileName;
    }
}

void TVSTHost::SavePreset(QString FileName)
{
    QFile f(FileName);
    if (f.open(QIODevice::WriteOnly))
    {
        SavePreset(f);
    }
}

int TVSTHost::GetChunk(void* pntr,bool isPreset)
{
    return ptrPlug->dispatcher(ptrPlug,effGetChunk,long(isPreset),0,pntr,0.0f);
}

int TVSTHost::SetChunk(void* data,long byteSize,bool isPreset)
{
    return ptrPlug->dispatcher(ptrPlug,effSetChunk, long(isPreset), byteSize, data,0.0f);
}


void TVSTHost::SavePreset(QFile& str)
{
    if (!ptrPlug){return;}
    if (ptrPlug->flags & effFlagsProgramChunks)
    {
        fxChunkSet p2;
        setDescriptor(p2.chunkMagic,"CcnK");
        setDescriptor(p2.fxMagic,"FPCh");
        p2.version=qToBigEndian<qint32>(1);
        p2.fxID=qToBigEndian<qint32>(ptrPlug->uniqueID);
        p2.fxVersion=qToBigEndian<qint32>(ptrPlug->version);
        p2.numPrograms=qToBigEndian<qint32>(ptrPlug->numPrograms);
        char temp[256];
        ptrPlug->dispatcher(ptrPlug,effGetProgramName,0,0,temp,0.0f);
        CopyMemory(p2.prgName,temp,256);
        void* PBuffer;
        int x=GetChunk(&PBuffer,true);
        p2.chunkSize=qToBigEndian<qint32>(x);
        p2.byteSize=qToBigEndian<qint32>(sizeof(p2) - sizeof(int) * 2 + x - 8);
        str.write((char*)&p2,sizeof(p2)-sizeof(void*));
        str.write((char*)PBuffer, x);
    }
    else
    {
        long i=ptrPlug->dispatcher(ptrPlug,effGetProgram,0,0,NULL,0.0f);
        fxPreset pp=GetPreset(i);
        str.write((char*)&pp,sizeof(pp)-sizeof(float));
        str.write((char*)pp.params, sizeof(float) * ptrPlug->numParams);
        free(pp.params);
    }
}

void TVSTHost::LoadBank(QFile& str)
{
    if (!ptrPlug){return;}
    if (ptrPlug->flags & effFlagsProgramChunks)
    {
        fxChunkBank p2;
        str.read((char*)&p2,sizeof(fxChunkBank)-sizeof(void*));
        if (qFromBigEndian<qint32>(p2.fxID) != ptrPlug->uniqueID)
        {
            qDebug() << "Bank file not for this Plug-in";
            return;
        }
        int x=str.size() - str.pos();
        void* pb2=calloc(1,x+1);
        long j=str.read((char*)pb2,x);
        SetChunk(pb2,j,false);
        free(pb2);
    }
    else
    {
        fxSet p;
        str.read((char*)&p,sizeof(fxSet)-sizeof(void*));
        if (qFromBigEndian<qint32>(p.fxID) != ptrPlug->uniqueID)
        {
            qDebug() << "Bank file not for this Plug-in";
            return;
        }
        VstPatchChunkInfo pci;
        pci.version=1;
        pci.pluginUniqueID=ptrPlug->uniqueID;
        pci.pluginVersion=ptrPlug->version;
        pci.numElements=ptrPlug->numPrograms;
        ptrPlug->dispatcher(ptrPlug,effBeginLoadBank,0,0,&pci,0.0f);
        p.numPrograms=qFromBigEndian<qint32>(p.numPrograms);
        for (long j=0;j<p.numPrograms;j++)
        {
            fxPreset pp;
            str.read((char*)&pp,sizeof(fxPreset)-sizeof(void*));
            ptrPlug->dispatcher(ptrPlug,effBeginSetProgram,0,0,NULL,0.0f);
            ptrPlug->dispatcher(ptrPlug,effSetProgram,0,j,NULL,0.0f);
            ptrPlug->dispatcher(ptrPlug,effEndSetProgram,0,0,NULL,0.0f);
            ptrPlug->dispatcher(ptrPlug,effSetProgramName,0,0,pp.prgName,0.0f);
            pp.numParams=qFromBigEndian<qint32>(pp.numParams);
            int x;
            for (long i=0;i<pp.numParams;i++)
            {
                str.read((char*)&x,sizeof(int));
                x=qFromBigEndian<qint32>(x);
                float s=*((float*)&x);
                ptrPlug->setParameter(ptrPlug,i,s);
            }
        }
    }
    ptrPlug->dispatcher(ptrPlug,effBeginSetProgram,0,0,NULL,0.0f);
    ptrPlug->dispatcher(ptrPlug,effSetProgram,0,0,NULL,0.0f);
    ptrPlug->dispatcher(ptrPlug,effEndSetProgram,0,0,NULL,0.0f);
}

void TVSTHost::LoadPreset(QFile& str)
{
    if (!ptrPlug){return;}
    if (ptrPlug->flags & effFlagsProgramChunks)
    {
        fxChunkSet p2;
        str.read((char*)&p2,sizeof(fxChunkSet)-sizeof(void*));
        if (qFromBigEndian<qint32>(p2.fxID) != ptrPlug->uniqueID)
        {
            qDebug() << "Preset file not for this Plug-in";
            return;
        }
        ptrPlug->dispatcher(ptrPlug,effSetProgramName,0,0,p2.prgName,0.0f);
        int x=str.size()-str.pos();
        void* pb2=calloc(1,x+1);
        long j=str.read((char*)pb2,x);
        SetChunk(pb2,j,true);
        free(pb2);
    }
    else
    {
        fxPreset p;
        str.read((char*)&p,sizeof(fxPreset)-sizeof(void*));
        if (qFromBigEndian<qint32>(p.fxID) != ptrPlug->uniqueID)
        {
            qDebug() << "Preset file not for this Plug-in";
            return;
        }
        VstPatchChunkInfo pci;
        pci.version=1;
        pci.pluginUniqueID=ptrPlug->uniqueID;
        pci.pluginVersion=ptrPlug->version;
        pci.numElements=ptrPlug->numParams;
        ptrPlug->dispatcher(ptrPlug,effBeginLoadProgram,0,0,&pci,0.0f);
        ptrPlug->dispatcher(ptrPlug,effSetProgramName,0,0,p.prgName,0.0f);
        p.numParams=qFromBigEndian<qint32>(p.numParams);
        int x;
        for (long i=0;i<p.numParams;i++)
        {
            str.read((char*)&x,sizeof(int));
            x=qFromBigEndian<qint32>(x);
            float s=*((float*)&x);
            ptrPlug->setParameter(ptrPlug,i,s);
        }
    }
    ptrPlug->dispatcher(ptrPlug,effBeginSetProgram,0,0,NULL,0.0f);
    ptrPlug->dispatcher(ptrPlug,effSetProgram,0,0,NULL,0.0f);
    ptrPlug->dispatcher(ptrPlug,effEndSetProgram,0,0,NULL,0.0f);
}

void TVSTHost::SaveBank(QFile& str)
{
    if (!ptrPlug){return;}
    if (ptrPlug->flags & effFlagsProgramChunks)
    {
        fxChunkBank p2;
        setDescriptor(p2.chunkMagic,"Ccnk");
        setDescriptor(p2.fxMagic,"FBCh");
        p2.version=qToBigEndian<qint32>(1);
        p2.fxID=qToBigEndian<qint32>(ptrPlug->uniqueID);
        p2.fxVersion=qToBigEndian<qint32>(ptrPlug->version);
        p2.numPrograms=qToBigEndian<qint32>(ptrPlug->numPrograms);
        void* PBuffer;
        int x=GetChunk(PBuffer,false);
        p2.chunkSize=qToBigEndian<qint32>(x);
        p2.byteSize=qToBigEndian<qint32>(sizeof(p2) - sizeof(int) * 3 + x + 8);
        str.write((char*)&p2,sizeof(p2)-sizeof(void*));
        str.write((char*)PBuffer,x);
    }
    else
    {
        fxSet p;
        setDescriptor(p.chunkMagic,"Ccnk");
        setDescriptor(p.fxMagic,"FxBk");
        p.version=qToBigEndian<qint32>(1);
        p.fxID=qToBigEndian<qint32>(ptrPlug->uniqueID);
        p.fxVersion=qToBigEndian<qint32>(ptrPlug->version);
        p.numPrograms=qToBigEndian<qint32>(ptrPlug->numPrograms);
        p.byteSize=qToBigEndian<qint32>(sizeof(p) - sizeof(int) + (sizeof(fxPreset) + (ptrPlug->numParams-1) * sizeof(float)) * ptrPlug->numParams);
        str.write((char*)&p,sizeof(p)-sizeof(float));
        for (int j=0;j<ptrPlug->numPrograms;j++)
        {
            fxPreset pp =GetPreset(j);
            str.write((char*)&pp,sizeof(pp)-sizeof(float));
            str.write((char*)pp.params,sizeof(float)*ptrPlug->numParams);
            free(pp.params);
        }
    }
}


