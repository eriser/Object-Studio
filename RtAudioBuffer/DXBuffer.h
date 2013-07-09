//---------------------------------------------------------------------------

#ifndef DXBufferH
#define DXBufferH

#include "SoftSynthsClasses.h"
//#include "PeakComponent.h"
//#include <mmsystem.h>
//#include "c:\dxsdk\include\dsound.h"

#include <syncobjs.hpp>
#include "DesktopControl.h"
#include "DXInput.h"
#include <stdio.h>


	void __fastcall CreateBuffer(TDXInput* WaveIn,TDesktopComponent* DC,CInJack* InJackL,CInJack* InJackR,CInJack* InJackMIDI,TVemodPeakMeter* ProgressBarL,TVemodPeakMeter* ProgressBarR);
    void __fastcall Finish(void);
    void __fastcall FillDeviceCombo(TComboBox* ComboBox);
    void __fastcall SelectDevice(int Device,bool Restart);
    void __fastcall FillMIDIDeviceCombo(TComboBox* ComboBox,Directions Direction);
	void __fastcall SelectMIDIDevice(int Device,Directions Direction,bool Restart);
    void __fastcall StartRecord(AnsiString FileName);
	void __fastcall StopRecord(void);
	void __fastcall Pause(void);
	void __fastcall Resume(void);

//---------------------------------------------------------------------------
#endif


