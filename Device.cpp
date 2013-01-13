
#include "Common.h"

/* Actions */

void Extension::DeviceOpen(const char* Name)
{
	//Can't open more than one device at once.
	if(Device || Context)
		return;

	//Attributes for the context: Use 4 auxiliary sends.
	const ALint Attributes[4] = {ALC_MAX_AUXILIARY_SENDS, 4, 0, 0};

	//Open a device and create a contex for it
	if(Device = alcOpenDevice(Name))
	{
		Context = alcCreateContext(Device, Attributes);
		alcMakeContextCurrent(Context);
	}
}

void Extension::DeviceClose()
{
	if(Device && Context)
	{
		alcDestroyContext(Context);
		alcCloseDevice(Device);
		Device = 0;
		Context = 0;
	}
}

void Extension::DeviceEnumerate()
{
	for(EnumeratedDevice = DeviceList; *EnumeratedDevice; EnumeratedDevice += strlen(EnumeratedDevice))
		Runtime.GenerateEvent(5);
	EnumeratedDevice = "";
}

/* Expressions */

char* Extension::DeviceGetEnumerated()
{
	return (char*)EnumeratedDevice;
}