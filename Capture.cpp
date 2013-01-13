
#include "Common.h"

void Extension::CaptureRecalculateSettings()
{
    Input.BlockAlign = Input.Channels * Input.Bits / 8;
    Input.Format = alureGetSampleFormat(Input.Channels, Input.Bits, 0);
}

/* Actions */

void Extension::CaptureSetFrequency(int Frequency)
{
	Input.Frequency = Frequency;
}

void Extension::CaptureSetChannels(int Channels)
{
	Input.Channels = Channels == 2 ? 2 : 1;
}

void Extension::CaptureSetBits(int Bits)
{
	Input.Bits = Bits == 16 ? 16 : 8;
}

void Extension::CaptureSetBufferSize(int BufferSamples)
{
	Input.BufferSamples = max(32, BufferSamples);
}

void Extension::CaptureDeviceOpen(const char* Name)
{
	//Make sure that we are not recording right now.
	if(Input.Recording || Input.Device)
		return;

	CaptureRecalculateSettings();

	//Open a recording device based on the settings.
	Input.Device = alcCaptureOpenDevice(Name, Input.Frequency, Input.Format, Input.BufferSamples*Input.Bits/8*Input.Channels);
}

void Extension::CaptureDeviceClose()
{
	if(Input.Device)
	{
		CaptureStop();
		alcCaptureCloseDevice(Input.Device);
		Input.Device = 0;
	}
}

void Extension::CaptureDeviceEnumerate()
{
	for(Input.EnumeratedDevice = Input.DeviceList; *Input.EnumeratedDevice; Input.EnumeratedDevice += strlen(Input.EnumeratedDevice))
		Runtime.GenerateEvent(6);
	Input.EnumeratedDevice = "";
}

void Extension::CaptureStart()
{
    if(Input.Device && !Input.Recording)
	{
        alcCaptureStart(Input.Device);
		Input.Recording = true;
	}
}

void Extension::CaptureStop()
{
    if(Input.Device && Input.Recording)
	{
		alcCaptureStop(Input.Device);
		Input.Recording = false;
	}
}

/* Expressions */ 

int Extension::CaptureDataGetAddress()
{
	return (int)CaptureBuffer;
}

int Extension::CaptureDataGetBytes()
{
	return CaptureAvailableBytes;
}

char* Extension::CaptureDeviceGetEnumerated()
{
	return (char*)Input.EnumeratedDevice;
}