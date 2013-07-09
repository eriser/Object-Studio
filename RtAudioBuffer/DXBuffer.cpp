//---------------------------------------------------------------------------

#include <vcl.h>
#include "DXBuffer.h"
#include "RtAudio.h"
#include "RtMidi.h"
#include <fastmath.h>
#include "WaveFile.h"
#pragma hdrstop
//---------------------------------------------------------------------------

#pragma package(smart_init)

	enum BufferStates
	{
		Ready=0,
		Working=1,
		Stopped=2
	};
	int SampleRate=Presets().SampleRate;
	int ModulationRate=Presets().ModulationRate;
	int BufferSize=Presets().BufferSize;

	BufferStates BufferState=Ready;
	//bool Stopped;
	CInJack* m_InJackL;
	CInJack* m_InJackR;
	CInJack* m_InJackMIDI;
	TVemodPeakMeter* PGL;
	TVemodPeakMeter* PGR;
	TDesktopComponent* m_DC;
	TDXInput* m_WI;
	int OldPos[2]={0,0};
	int MaxCount[2]={0,0};
	//bool InHere=false;
    RtAudio *m_audio=NULL;
    RtMidiOut *m_MIDI=NULL;
    int m_Device=-1;
    int m_AsioDevices=0;
    bool Duplex[20];
    int m_MIDIDevice[2]={-1,-1};
	float* ChannelBuffer[2]={NULL,NULL};
    int TickCount=0;
	unsigned int RunningStatusLength=0;
	unsigned char RunningStatus=0;
	int WaveInTick=Presets().ModulationRate/4;
    int DeviceTick=Presets().ModulationRate/2;
	int MIDIOutTick=WaveInTick+DeviceTick;
    bool m_Recording=false;
    TFileStream* m_RecordFile;
    int AsioIndex[20];
    int DXIndex[20];

int __fastcall Driver(void)
{
    if (m_Device>=m_AsioDevices)
    {
        return RtAudio::WINDOWS_DS;
    }
    return RtAudio::WINDOWS_ASIO;
}

int __fastcall Device(void)
{
    if (m_Device>=m_AsioDevices)
    {
		//return (m_Device-m_AsioDevices)+1;
		return DXIndex[m_Device-m_AsioDevices];
    }
	//return m_Device+1;
	return AsioIndex[m_Device];
}

short inline TruncateVal(float* Buf, float* Peak)
{
	float Val=*Buf;
	if (Val<0)
	{
		Val=-Val;
	}
	if (Val>*Peak)
	{
		*Peak=Val;
	}
	return (*Buf)*MAXSHORT;
}

void inline ParseMidi(CMIDIBuffer* MIDIBuffer)
{
	std::vector<unsigned char> message;
	MIDIBuffer->StartRead();
	while (!MIDIBuffer->IsRead())
	{
		unsigned int MessageLength;
		unsigned char MessageByte=MIDIBuffer->Read();
		message.clear();
		if (MessageByte==0xFF)
		{
			unsigned char Data1=MIDIBuffer->Read();
			MessageLength=MIDIBuffer->Read();
			message.push_back( MessageByte );
			message.push_back( Data1 );
			message.push_back( MessageLength );
			for (unsigned int i=0;i<MessageLength;i++)
			{
				message.push_back( (unsigned char)MIDIBuffer->Read() );
			}
			m_MIDI->sendMessage( &message );
		}
		else if ((MessageByte ==0xF0) | (MessageByte ==0xF7))
		{
			//Buffer chunck
			MessageLength=MIDIBuffer->Read();
			message.push_back( MessageByte );
			message.push_back( MessageLength );
			for (unsigned int i=0;i<MessageLength;i++)
			{
				message.push_back( (unsigned char)MIDIBuffer->Read() );
			}
			m_MIDI->sendMessage( &message );
		}
		else if (MessageByte<0x80)
		{
			//This i running status
			if (RunningStatusLength==3)
			{
				MessageLength=1;
			}
			else
			{
				MessageLength=0;
			}
			message.push_back( RunningStatus );
			message.push_back( MessageByte );
			for (unsigned int i=0;i<MessageLength;i++)
			{
				message.push_back( (unsigned char)MIDIBuffer->Read() );
			}
			m_MIDI->sendMessage( &message );
		}
		else
		{
			if ((MessageByte >=0xC0) & (MessageByte <=0xDF))
			{
				//2 byte chunck
				RunningStatus=MessageByte;
				RunningStatusLength=2;
				MessageLength=1;
			}
			else if ((MessageByte >=0x80) & (MessageByte <=0xEF))
			{
				//3 byte chunck
				RunningStatus=MessageByte;
				RunningStatusLength=3;
				MessageLength=2;
			}
			message.push_back( MessageByte );
			for (unsigned int i=0;i<MessageLength;i++)
			{
				message.push_back( (unsigned char)MIDIBuffer->Read() );
			}
			m_MIDI->sendMessage( &message );
		}
	}
}

int __fastcall FillBuffer(char *buffer, int bufferSize, void *data)
{
	if (BufferState==Ready)
	{
			BufferState= (BufferStates)(BufferState | Working);
			short *my_buffer = (short*) buffer;
			float PeakL=0;
			float PeakR=0;
			m_WI->CopyBuffer(my_buffer,bufferSize);
			for (int i=0; i<bufferSize; i++)
			{
				if (TickCount==WaveInTick)
				{
					m_WI->Tick();
				}
				else if (TickCount>=ModulationRate)
				{
					TickCount=0;
					ChannelBuffer[0]=m_InJackL->GetNextA();
					ChannelBuffer[1]=m_InJackR->GetNextA();
					TickGraphics();
				}
				else if (TickCount==MIDIOutTick)
				{
					CMIDIBuffer* MIDIBuffer=(CMIDIBuffer*)m_InJackMIDI->GetNextP();
					if (MIDIBuffer)
					{
						ParseMidi(MIDIBuffer);
					}
				}
				else if (TickCount==DeviceTick)
				{
					m_DC->Tick();
				}
				if (ChannelBuffer[0])
				{
					*my_buffer++=TruncateVal(ChannelBuffer[0]+TickCount,&PeakL);
				}
				else
				{
					*my_buffer++=0;
				}
				if (ChannelBuffer[1])
				{
					*my_buffer++=TruncateVal(ChannelBuffer[1]+TickCount,&PeakR);
				}
				else
				{
					*my_buffer++=0;
				}
				TickCount++;
			}
			PGL->Peak(PeakL);
			PGR->Peak(PeakR);
			if (m_Recording)
			{
				m_RecordFile->Write(buffer,bufferSize*4);
			}
			BufferState =(BufferStates)(BufferState & (!Working));
	
	}
	return 0;
}

void __fastcall StartRecord(AnsiString FileName)
{
        m_RecordFile=NULL;
        WaveHeader WH;
        memcpy(WH.GroupID,"RIFF",4);
        memcpy(WH.RiffType,"WAVE",4);
        WaveFormatID WFI;
        memcpy(WFI.chunkID,"fmt ",4);
        WFI.chunkSize=sizeof(WFI)-8;

        WFI.wFormatTag=WAVE_FORMAT_PCM;         // Format category
        WFI.wChannels=2;          // Number of channels
        WFI.dwSamplesPerSec=SampleRate;    // Sampling rate
        WFI.wBitsPerSample=16;
        WFI.wBlockAlign=(WFI.wChannels* WFI.wBitsPerSample)/8;        // Data block size
        WFI.dwAvgBytesPerSec=SampleRate*WFI.wBlockAlign;   // For buffer estimation

        WaveDataID WDI;
        memcpy(WDI.ID,"data",4);

        try
        {
            m_RecordFile=new TFileStream(FileName,fmCreate | fmShareExclusive);
        }
        catch (...)
        {
            ShowMessage("Can't create file");
            return;
        }
        m_RecordFile->Write(&WH,sizeof(WH));
        m_RecordFile->Write(&WFI,sizeof(WFI));
        m_RecordFile->Write(&WDI,sizeof(WDI));
        if (m_RecordFile)
        {
            m_Recording = true;
        }    
}

void __fastcall StopRecord(void)
{
    if (m_Recording)
    {
		m_Recording=false;
		m_RecordFile->Seek(4,0);
		int FileSize=m_RecordFile->Size-8;
        m_RecordFile->Write(&FileSize,sizeof(int));
        m_RecordFile->Seek(sizeof(WaveHeader)+sizeof(WaveFormatID)+4,0);
        int Size=m_RecordFile->Size-(sizeof(WaveHeader)+sizeof(WaveFormatID)+sizeof(WaveDataID));
        m_RecordFile->Write(&Size,sizeof(int));
        delete m_RecordFile;
    }
}

void __fastcall CreateBuffer(TDXInput* WaveIn,TDesktopComponent* DC,CInJack* InJackL,CInJack* InJackR,CInJack* InJackMIDI,TVemodPeakMeter* ProgressBarL,TVemodPeakMeter* ProgressBarR)
{
    //Application->ProcessMessages();
    m_DC=DC;
    m_WI=WaveIn;
    m_InJackL = InJackL;
    m_InJackR = InJackR;
    m_InJackMIDI=InJackMIDI;
    PGL=ProgressBarL;
    PGR=ProgressBarR;
	BufferSize=Presets().BufferSize;
	TickCount=0;	
    //Application->ProcessMessages();
    try
    {
        if (Duplex[m_Device])
        {
			m_audio = new RtAudio(Device(), 2, Device(), 2,RTAUDIO_SINT16, SampleRate, &BufferSize, Presets().Buffers,(RtAudio::RtAudioApi)Driver());
        }
        else
        {
			m_audio = new RtAudio(Device(), 2, 0, 0,RTAUDIO_SINT16, SampleRate, &BufferSize, Presets().Buffers,(RtAudio::RtAudioApi)Driver());
        }
    }
    catch (RtError &error)
    {
        error.printMessage();
        ShowMessage(error.getMessage().c_str());
        //exit(EXIT_FAILURE);
        m_audio=NULL;
        return;
    }
    m_WI->CreateBuffer(m_MIDIDevice[In],Duplex[m_Device],Device(),Driver(),BufferSize);
    //Application->ProcessMessages();
    try
    {
        // Set the stream callback function
        m_audio->setStreamCallback(&FillBuffer, NULL);
        //m_audio->getStreamBuffer();
        //Application->ProcessMessages();
		BufferState = (BufferStates)(BufferState & (!Stopped));
        // Start the stream
        m_audio->startStream();
        //Application->ProcessMessages();
    }
    catch (RtError &error)
    {
        error.printMessage();
        ShowMessage(error.getMessage().c_str());
        Finish();
    }
    try
    {
        m_MIDI = new RtMidiOut();
        // Check available ports.
        unsigned int nPorts = m_MIDI->getPortCount();
        if ( nPorts > 0 )
        {
            // Open first available port.
            m_MIDI->openPort( m_MIDIDevice[Out] );
        }
    }
    catch (RtError &error)
    {
        error.printMessage();
        ShowMessage(error.getMessage().c_str());
        delete m_MIDI;
        m_MIDI=NULL;
    }
    m_WI->StartStream();
	m_WI->Tick();
	m_DC->Tick();
}

void __fastcall Pause(void)
{
	if (m_audio)
	{
		try
		{
			// Stop and close the stream
			m_audio->stopStream();
		}
		catch (...)
		{
			//error.printMessage();
		}
		m_WI->StopStream();
	}
	BufferState = (BufferStates)(BufferState | Stopped);
}

void __fastcall Resume(void)
{
	if (m_audio)
	{
		try
		{
			// Stop and close the stream
			m_audio->startStream();
		}
		catch (...)
		{
			//error.printMessage();
		}
		m_WI->StartStream();
	}
	BufferState = (BufferStates)(BufferState & (!Stopped));
}

void __fastcall Finish(void)
{
	StopRecord();
	BufferState = (BufferStates)(BufferState | Stopped);
	if (m_audio)
	{
		try
		{
			// Stop and close the stream
			m_audio->stopStream();
		}
		catch (...)
		{
			//error.printMessage();
		}
		m_WI->StopStream();
		//Application->ProcessMessages();
		try
		{
			// Stop and close the stream
			m_audio->closeStream();
		}
		catch (...)
		{
			//error.printMessage();
		}
		//Application->ProcessMessages();
		try
		{
			delete m_audio;
		}
		catch (...)
		{
			//error.printMessage();
		}
		m_WI->Finish();
		//Application->ProcessMessages();
	}
	m_audio=NULL;
	if (m_MIDI)
	{
		delete m_MIDI;
	}
	m_MIDI=0;
}

void __fastcall FillDevice(TComboBox* ComboBox, int Driver)
{
    RtAudio *audio;
    // Default RtAudio constructor
    try
    {
        audio = new RtAudio((RtAudio::RtAudioApi)Driver);
    }
    catch (RtError &error)
    {
        //error.printMessage();
        //ShowMessage(error.getMessage().c_str());
        //exit(EXIT_FAILURE);
        return;
    }
    // Determine the number of devices available
    int devices = audio->getDeviceCount();
    // Scan through devices for various capabilities
    RtAudioDeviceInfo info;
    for (int i=1; i<=devices; i++)
    {
		try
		{
			info = audio->getDeviceInfo(i);
			bool Match1=false;
			int OutputChannels=info.outputChannels;
			int InputChannels=info.inputChannels;			
			if ((OutputChannels>=2) & (InputChannels>=2))
			{
				for (unsigned int i1=0;i1<info.sampleRates.size();i1++)
				{
					if (info.sampleRates[i1]==Presets().SampleRate)
					{
						Match1=true;
					}
				}
				if (Match1)
				{
					unsigned long BufferFormat=RTAUDIO_SINT16;
					unsigned long NativeFormat=info.nativeFormats;
					if ((NativeFormat & BufferFormat)!=0)
					{
						AnsiString DrvName=AnsiString(info.name.c_str());
						AnsiString Ext="";
						int ExistCount=0;
						while (ComboBox->Items->IndexOf(DrvName+Ext)>-1)
						{
							ExistCount++;
							Ext=" (" + AnsiString(ExistCount)+ ")";
						}
						ComboBox->Items->Add(DrvName+Ext);
						if (Driver==RtAudio::WINDOWS_ASIO)
						{
							AsioIndex[ComboBox->Items->Count-1]=i;
						}
						if (Driver==RtAudio::WINDOWS_DS)
						{
							DXIndex[(ComboBox->Items->Count-1)-m_AsioDevices]=i;
						}
//						ComboBox->Items->Add(AnsiString(info.name.c_str()));
						//Debug (AnsiString(info.name.c_str()) + " " + AnsiString(info.outputChannels) + " " + AnsiString(info.inputChannels) + " " + AnsiString(info.duplexChannels) + " " + AnsiString(info.nativeFormats) + " " + AnsiString((int)info.probed) + " " + AnsiString((int)info.isDefault));
						int DuplexChannels=info.duplexChannels;
						if (DuplexChannels>=2)
						{
							Duplex[ComboBox->Items->Count-1]=true;
						}
					}
				}
			}
		}
		//catch (RtError &error)
		catch (...)
		{
			//error.printMessage();
			//ShowMessage(error.getMessage().c_str());
			//break;
		}
	}
	// Clean up
	delete audio;
}

void __fastcall FillDeviceCombo(TComboBox* ComboBox)
{
    ComboBox->Clear();
    FillDevice(ComboBox,RtAudio::WINDOWS_ASIO);
    m_AsioDevices=ComboBox->Items->Count;
    FillDevice(ComboBox,RtAudio::WINDOWS_DS);
    SendMessage( ComboBox->Handle,   CB_SETCURSEL, 0, 0 );
    if (ComboBox->Items->Count)
    {
        m_Device=0;
    }

}

void __fastcall SelectDevice(int Device,bool Restart)
{
	m_Device=Device;
	if (!Restart)
	{
		return;
	}
	if (!(BufferState & Stopped))
	{
		Finish();
		CreateBuffer(m_WI,m_DC,m_InJackL,m_InJackR,m_InJackMIDI,PGL,PGR);
		return;
	}
	CreateBuffer(m_WI,m_DC,m_InJackL,m_InJackR,m_InJackMIDI,PGL,PGR);
}

void __fastcall FillMIDIDeviceCombo(TComboBox* ComboBox,Directions Direction)
{
    ComboBox->Clear();
    if (Direction==In)
    {
        RtMidiIn* midiin;
        try
        {
            midiin = new RtMidiIn();
        }
        catch (RtError &error)
        {
            error.printMessage();
            return;
        }

        // Check inputs.
        unsigned int nPorts = midiin->getPortCount();
        std::string portName;
        for ( unsigned int i=0; i<nPorts; i++ )
        {
            try
            {
                portName = midiin->getPortName(i);
                ComboBox->Items->Add(portName.c_str());
            }
            catch (RtError &error)
            {
                error.printMessage();
            }
        }
        delete midiin;
    }
    else
    {
        RtMidiOut* midiout;
        try
        {
            midiout = new RtMidiOut();
        }
        catch (RtError &error)
        {
            error.printMessage();
            return;
        }

        // Check inputs.
        unsigned int nPorts = midiout->getPortCount();
        std::string portName;
        for ( unsigned int i=0; i<nPorts; i++ )
        {
            try
            {
                portName = midiout->getPortName(i);
                ComboBox->Items->Add(portName.c_str());
            }
            catch (RtError &error)
            {
                error.printMessage();
            }
        }
        delete midiout;
    }


    SendMessage( ComboBox->Handle,   CB_SETCURSEL, 0, 0 );
    if (ComboBox->Items->Count)
    {
        m_MIDIDevice[Direction]=0;
    }
}

void __fastcall SelectMIDIDevice(int Device,Directions Direction,bool Restart)
{
	m_MIDIDevice[Direction]=Device;
	if (!Restart)
	{
		 return;
	}
	if (!(BufferState & Stopped))
	{
		Finish();
		CreateBuffer(m_WI,m_DC,m_InJackL,m_InJackR,m_InJackMIDI,PGL,PGR);
		return;
	}
	CreateBuffer(m_WI,m_DC,m_InJackL,m_InJackR,m_InJackMIDI,PGL,PGR);
}



