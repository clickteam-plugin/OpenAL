
#include "Common.h"


///
/// EXTENSION CONSTRUCTOR/DESTRUCTOR
///

Extension::Extension(LPRDATA _rdPtr, LPEDATA edPtr, fpcob cobPtr)
	: rdPtr(_rdPtr), rhPtr(_rdPtr->rHo.hoAdRunHeader), Runtime(_rdPtr)
{

#ifdef _DEBUG
	AllocConsole();
	freopen("conin$","r", stdin);
	freopen("conout$","w", stdout);
	freopen("conout$","w", stderr);
#endif

	Source = 0;
	Effect = 0;
	Filter = 0;
	AuxSlot = 0;
	
	//Initialize device variables.
	DeviceList = alcGetString(0, ALC_DEVICE_SPECIFIER);
	Device = 0;
	Context = 0;
	EnumeratedDevice = "";

	//Initialize input device variables.
	Input.DeviceList = alcGetString(0, ALC_CAPTURE_DEVICE_SPECIFIER);
	Input.Device = 0;
	Input.Recording = false;
	Input.EnumeratedDevice = "";

	//Set up the microphone input default settings.
	Input.Bits = 16;
	Input.Channels = 1;
	Input.Frequency = 22050;
	Input.BufferSamples = 1024;
	Input.FFTSize = 256;

	//Open the default device.
	DeviceOpen();

	//Get the meters per unit so we can scale the positions.
	alGetListenerf(AL_METERS_PER_UNIT, &Unit);

	//Link the ACE indices to their functions.
	LinkACE();
}

Extension::~Extension()
{
	//Free the global filter.
	if(Filter)
		alDeleteFilters(1, &Filter);

	//Free all effect objects.
	for(map<int, ALuint>::iterator it = EffectMap.begin(); it != EffectMap.end(); ++it)
	{
		alDeleteEffects(1, &it->second);
	}

	//Free all filter objects.
	for(map<int, ALuint>::iterator it = FilterMap.begin(); it != FilterMap.end(); ++it)
	{
		alDeleteFilters(1, &it->second);
	}

	//Stop all running sources and free them.
	for(list<ALSource>::iterator it = SourceList.begin(); it != SourceList.end(); ++it)
	{
		alureStopSource(it->Handle, false);
		alDeleteSources(1, &it->Handle);
	}

	//Free all auxiliary effect slots.
	for(map<int, ALuint>::iterator it = AuxSlotMap.begin(); it != AuxSlotMap.end(); ++it)
	{
		alDeleteAuxiliaryEffectSlots(1, &it->second);
	}

	//Delete all buffers and their attached streams.
	for(map<string, ALBuffer>::iterator it = BufferMap.begin(); it != BufferMap.end(); ++it)
	{
		DeleteBuffer(&it->second);
	}

	//Close all open devices.
	CaptureDeviceClose();
	DeviceClose();
}

short Extension::Handle()
{
	//Handle streams, make context current etc.
	if(Device && Context)
		alureUpdate();

	return 0;
}

short Extension::Display()
{
	return 0;
}

short Extension::Pause()
{
	alcSuspendContext(Context);
	return 0;
}

short Extension::Continue()
{
	alcProcessContext(Context);
	return 0;
}

bool Extension::Save(HANDLE File)
{
	bool OK = false;

	#ifndef VITALIZE

		// Save the object's data here

		OK = true;

	#endif

	return OK;
}

bool Extension::Load(HANDLE File)
{
	bool OK = false;

	#ifndef VITALIZE

		// Load the object's data here

		OK = true;

	#endif

	return OK;
}


// These are called if there's no function linked to an ID

void Extension::Action(int ID, LPRDATA rdPtr, long param1, long param2)
{
	char meh[100] = {0};
	sprintf_s(meh, 100, "Action %d unlinked (params: %d, %d)", ID, param1, param2);
	MessageBox(0, meh, "Error", 0);
}

long Extension::Condition(int ID, LPRDATA rdPtr, long param1, long param2)
{
	char meh[100] = {0};
	sprintf_s(meh, 100, "Condition %d unlinked (params: %d, %d)", ID, param1, param2);
	MessageBox(0, meh, "Error", 0);
	return false;
}

long Extension::Expression(int ID, LPRDATA rdPtr, long param)
{
	char meh[100] = {0};
	sprintf_s(meh, 100, "Expression %d unlinked (params %d)", ID, param);
	MessageBox(0, meh, "Error", 0);
	return 0;
}

bool Extension::GetError(const char* File, unsigned int Line)
{
#ifdef _DEBUG
	printf("%s:%04d\n\t", File, Line);
	ALenum Error = alGetError();
	if(Error)
		printf("OpenAL: %s (%#x)\n", alGetString(Error), Error);
	else if(strcmp("No error", alureGetErrorString()))
		printf("Alure: %s\n", alureGetErrorString());
	else
	{
		printf("No error\n");
		return true;
	}

	return false;
#endif
	
	return true;
}

void Extension::Assert(ALboolean Statement)
{
	if(!Statement)
		MessageBox(0,"Assertion failed!","OpenAL Object", MB_ICONWARNING);
}

bool Extension::GenericTrigger()
{
	return true;
}

void Extension::LinkACE()
{
	//ACE's are linked here.
	LinkAction(0, ListenerSetPosition);
	LinkAction(1, SourceCreate);
	LinkAction(2, SourceCreateNamed);
	LinkAction(3, BufferCreateSampleForFile);
	LinkAction(4, SourcePlay);
	LinkAction(5, SourceSetBuffer);
	LinkAction(6, SourceSetPosition);
	LinkAction(7, SourceSelectByHandle);
	LinkAction(8, SourceSetLooping);
	LinkAction(9, ListenerSetGain);
	LinkAction(10, EffectCreateReverb);
	LinkAction(11, AuxEffectSlotLoadEffectByID);
	LinkAction(12, SourceSetGain);
	LinkAction(13, SourcePause);
	LinkAction(14, SourceStop);
	LinkAction(15, SourceRewind);
	LinkAction(16, EffectLoadReverbPreset);
	LinkAction(17, EffectCreateEcho)
	LinkAction(18, EffectCreateRingModulator);
	LinkAction(19, EffectSetIntParameter);
	LinkAction(20, AuxEffectSlotSetGain);
	LinkAction(21, FilterSetGain);
	LinkAction(22, BufferCreateStreamForFile);
	LinkAction(23, SourceResume);
	LinkAction(24, SourceSetPitch);
	LinkAction(25, ListenerSetOrientation);
	LinkAction(26, ListenerSetVelocity);
	LinkAction(27, SourceSetVelocity);
	LinkAction(28, SetDopplerFactor);
	LinkAction(29, SetSpeedOfSound);
	LinkAction(30, SetDistanceModel);
	LinkAction(31, CaptureDeviceOpen);
	LinkAction(32, EffectSetFloatParameter);
	LinkAction(33, BufferCreateStreamForCapture);
	LinkAction(34, CaptureStart);
	LinkAction(35, CaptureStop);
	LinkAction(36, AuxEffectSlotSetAutoAdjust);
	LinkAction(37, SourceSetSampleOffset);
	LinkAction(38, SourceSetSecondOffset);
	LinkAction(39, SourceSetDirection);
	LinkAction(40, SourceSetConeAngles);
	LinkAction(41, SourceSetConeOuterGain);
	LinkAction(42, SourceSetRelative);
	LinkAction(43, SourceSetMinimumGain);
	LinkAction(44, SourceSetMaximumGain);
	LinkAction(45, SourceSetReferenceDistance);
	LinkAction(46, SourceSetMaximumDistance);
	LinkAction(47, SourceSetRolloffFactor);
	LinkAction(48, CaptureDeviceClose);
	LinkAction(49, CaptureSetFrequency);
	LinkAction(50, CaptureSetChannels);
	LinkAction(51, CaptureSetBits);
	LinkAction(52, CaptureSetBufferSize);
	LinkAction(53, DeviceOpen);
	LinkAction(54, DeviceClose);
	LinkAction(55, EffectSelectByID);
	LinkAction(56, SourceCreateWithBuffer);
	LinkAction(57, SourceCreateNamedWithBuffer);
	LinkAction(58, ListenerSetMetersPerUnit);
	LinkAction(59, SourceSetConeOuterGainHF);
	LinkAction(60, SourceSetRoomRolloffFactor);
	LinkAction(61, SourceSetAirAbsorptionFactor);
	LinkAction(62, SourceSelectByHandle);
	LinkAction(63, SourceLoadDirectFilter);
	LinkAction(64, FilterCreateLowPass);
	LinkAction(65, FilterSetGainHF)
	LinkAction(66, AuxEffectSlotCreate);
	LinkAction(67, AuxEffectSlotSelectByID);
	LinkAction(68, SourceSetAuxiliarySendWithFilter);
	LinkAction(69, SourceSetAuxiliarySendGainAutoAdjust);
	LinkAction(70, SourceSetAuxiliarySendGainHFAutoAdjust);
	LinkAction(71, SourceSetAuxiliarySend);
	LinkAction(72, BufferCreateStreamForSignal);
	LinkAction(73, SourceSetPermanent);
	LinkAction(74, BufferCreateStreamForCallback);
	LinkAction(75, BufferCallbackWriteBytes);
	LinkAction(76, DeviceEnumerate);
	LinkAction(77, CaptureDeviceEnumerate);
	LinkAction(78, FilterSelectByID);
	LinkAction(79, BufferCreateSampleForMemory);
	LinkAction(80, BufferCreateSampleForMMF);
	
	LinkCondition(0, SourceExists);
	LinkCondition(1, SourceIsPlaying);
	LinkCondition(2, SourceOnStopped);
	LinkCondition(3, GenericTrigger);
	LinkCondition(4, GenericTrigger);
	LinkCondition(5, GenericTrigger);
	LinkCondition(6, GenericTrigger);

	LinkExpression(0, SourceByNameGetHandle);
	LinkExpression(1, SourceByNameGetSecondOffset);
	LinkExpression(2, SourceByNameGetSecondLength);
	LinkExpression(3, SourceSelectedGetName);
	LinkExpression(4, SourceSelectedGetHandle);
	LinkExpression(5, BufferCallbackGetAddress);
	LinkExpression(6, BufferCallbackGetBytes);
	LinkExpression(7, CaptureDataGetAddress);
	LinkExpression(8, CaptureDataGetBytes);
	LinkExpression(9, DeviceGetEnumerated);
	LinkExpression(10, CaptureDeviceGetEnumerated);
	LinkExpression(11, SourceByNameGetDistance);
	LinkExpression(12, EffectGetLast);
	LinkExpression(13, FilterGetLast);
	LinkExpression(14, SourceGetCount);
	LinkExpression(15, SourceByIndexGetName);
	LinkExpression(16, SourceByIndexGetHandle);
}