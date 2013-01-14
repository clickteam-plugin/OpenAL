//Extension class (part of RUNDATA).
class Extension
{
public:

	LPRDATA rdPtr;
	LPRH    rhPtr;

	Edif::Runtime Runtime;

	static const int MinimumBuild = 251;

	static const int OEFLAGS = OEFLAG_VALUES;
	static const int OEPREFS = 0;

	static const int Version = 2;
	
	static const int WindowProcPriority = 100;

	Extension(LPRDATA rdPtr, LPEDATA edPtr, fpcob cobPtr);
	~Extension();

	//Handles to the currently selected objects.
	ALSource*	Source;
	ALuint		Effect;
	ALuint		Filter;
	ALuint		AuxSlot;

	//Object arrays that keep track of all OpenAL handles.
	list<ALSource>          SourceList;
	map<string,ALBuffer>	BufferMap;
	map<int,ALuint>			EffectMap;
	map<int,ALuint>			FilterMap;
	map<int,ALuint>			AuxSlotMap;
	int						LastEffectIndex;
	int						LastFilterIndex;

	//A list of all devices used when opening one.
	const ALchar*	DeviceList;
	//Current device and its name.
	ALCdevice*		Device;
	const ALchar*	DeviceName;
	const ALchar*	EnumeratedDevice;
	//The context that the device is working in (holds sources etc).
	ALCcontext*		Context;

	//Microphone input settings.
	struct InputStruct
	{
		const ALchar*	DeviceList;
		//Current recording device.
		ALCdevice*      Device;
		const ALchar*	DeviceName;
		const ALchar*	EnumeratedDevice;
		//To keep track. No way to ask OpenAL!
		bool            Recording;
		//Numbers used for recording.
		ALuint          Frequency;
		ALuint          Channels;
		ALuint          Bits;
		ALuint          BufferSamples;
		ALsizei			FFTSize;
		//Calculated from above settings.
		ALuint          BlockAlign;
		ALenum          Format;
	} Input;

	//Stores the meters per unit.
	ALfloat		Unit;

	//Callback stream.
	ALubyte*    CallbackBuffer;
	ALuint      CallbackAvailableBytes;
	ALuint      CallbackWrittenBytes;

	//Microphone capture callback.
	ALubyte*	CaptureBuffer;
	ALuint		CaptureAvailableBytes;


	//Helpers

	void		CaptureRecalculateSettings();
	ALBuffer*	BufferGetFromName(const char* Name);
	ALuint		AuxEffectSlotGetFromID(int ID);
	void		AuxEffectSlotLoadEffect(ALuint Handle);
	ALuint		FilterGetFromID(int ID);
	ALuint		EffectGetFromID(int ID);
	ALenum		EffectGetParameterIndex(const char* Name);
	ALSource*	SourceGetFromName(const char* Name);
	void        SourceCreateEffectSlot(int Slot);
	bool		IsSource();

	void		LinkACE();
	bool		GenericTrigger();
	static void	Assert(ALboolean Statement);


	//Actions

	void SetDistanceModel(int DistanceModel);
	void SetDopplerFactor(float DopplerFctor);
	void SetSpeedOfSound(float SpeedOfSound);

	void DeviceOpen(const char* Name = "");
	void DeviceClose();
	void DeviceEnumerate();

	void CaptureSetFrequency(int Frequency);
	void CaptureSetChannels(int Channels);
	void CaptureSetBits(int Bits);
	void CaptureSetBufferSize(int BufferSamples);
	void CaptureDeviceOpen(const char* Name);
	void CaptureDeviceClose();
	void CaptureDeviceEnumerate();
	void CaptureStart();
	void CaptureStop();

	void BufferCreateSampleForFile(const char* Name, const char* FilePath);
	void BufferCreateStreamForFile(const char* Name, const char* FilePath);
	void BufferCreateSampleForMemory(const char* Name, const ALubyte* Address, unsigned int Size);
	void BufferCreateSampleForMMF(const char* Name, const char* MMFName);
	void BufferCreateStreamForCapture(const char* Name);
	void BufferCreateStreamForSignal(const char* Name, ALuint SampleRate, ALuint Channels, ALuint Bits, const char* Type, ALfloat Frequency, ALfloat ModulatorAmplitude, ALfloat ModulatorFrequency);
	void BufferCreateStreamForCallback(const char* Name, ALuint SampleRate, ALuint Channels, ALuint Bits, ALuint BufferSize);
	void BufferCallbackWriteBytes(int Bytes);

	void ListenerSetGain(float Gain);
	void ListenerSetPosition(float X, float Y, float Z);
	void ListenerSetVelocity(float X, float Y, float Z);
	void ListenerSetOrientation(float XAt, float YAt, float ZAt, float XUp, float YUp, float ZUp);
	void ListenerSetMetersPerUnit(float Meters);

	void SourceCreate();
	void SourceCreateNamed(const char* Name);
	void SourceCreateWithBuffer(const char* Buffer);
	void SourceCreateNamedWithBuffer(const char* Name, const char* Buffer);
	void SourceSelectByName(const char* Name);
	void SourceSelectByHandle(int Handle);
	void SourcePlay();
	void SourcePause();
	void SourceResume();
	void SourceStop();
	void SourceRewind();
	void SourceSetPermanent(int Permanent);
	void SourceSetBuffer(const char* BufferName);
	void SourceSetGain(float Gain);
	void SourceSetPitch(float Pitch);
	void SourceSetLooping(int Looping);
	void SourceSetSampleOffset(int Sample);
	void SourceSetSecondOffset(float Second);
	void SourceSetPosition(float X, float Y, float Z);
	void SourceSetVelocity(float X, float Y, float Z);
	void SourceSetDirection(float X, float Y, float Z);
	void SourceSetConeAngles(float Inner, float Outer);
	void SourceSetConeOuterGain(float Gain);
	void SourceSetConeOuterGainHF(float GainHF);
	void SourceSetRelative(int Relative);
	void SourceSetMinimumGain(float Gain);
	void SourceSetMaximumGain(float Gain);
	void SourceSetReferenceDistance(float Distance);
	void SourceSetMaximumDistance(float Distance);
	void SourceSetRolloffFactor(float RolloffFactor);
	void SourceSetRoomRolloffFactor(float RolloffFactor);
	void SourceSetAirAbsorptionFactor(float AirAbsorption);
	void SourceLoadDirectFilter();
	void SourceSetAuxiliarySend(int Send, int AuxSlot);
	void SourceSetAuxiliarySendWithFilter(int Send, int AuxSlot, int Filter);
	void SourceSetAuxiliarySendGainAutoAdjust(int Adjust);
	void SourceSetAuxiliarySendGainHFAutoAdjust(int Adjust);
	
	void EffectCreate(int Type);
	void EffectCreateReverb();
	void EffectCreateEcho();
	void EffectCreateRingModulator();
	void EffectSelectByID(int ID);
	void EffectSetFloatParameter(const char* Name, float Value);
	void EffectSetIntParameter(const char* Name, int Value);
	void EffectLoadReverbPreset(const char* Preset);

	void FilterCreateLowPass();
	void FilterSelectByID(int ID);
	void FilterSetGain(float Gain);
	void FilterSetGainHF(float GainHF);

	void AuxEffectSlotCreate(int ID);
	void AuxEffectSlotSelectByID(int ID);
	void AuxEffectSlotSetGain(float Gain);
	void AuxEffectSlotSetAutoAdjust(int Adjust);
	void AuxEffectSlotLoadEffectByID(int ID);

	//Conditions

	bool SourceOnStopped();
	bool SourceExists(const char* Name);
	bool SourceIsPlaying(const char* Name);

	//Expressions

	char* DeviceGetEnumerated();

	int CaptureDataGetAddress();
	int CaptureDataGetBytes();
	char* CaptureDeviceGetEnumerated();

	int BufferCallbackGetAddress();
	int BufferCallbackGetBytes();

	int SourceGetCount();
	int SourceSelectedGetHandle();
	char* SourceSelectedGetName();
	int SourceByNameGetHandle(const char* Name);
	float SourceByNameGetSecondOffset(const char* Name);
	float SourceByNameGetSecondLength(const char* Name);
	float SourceByNameGetDistance(const char* Name);
	int SourceByIndexGetHandle(unsigned int Index);
	char* SourceByIndexGetName(unsigned int Index);

	int EffectGetLast();

	int FilterGetLast();

	//Edif stuff

	void Action(int ID, LPRDATA rdPtr, long param1, long param2);
	long Condition(int ID, LPRDATA rdPtr, long param1, long param2);
	long Expression(int ID, LPRDATA rdPtr, long param);

	short Handle();
	short Display();

	short Pause();
	short Continue();

	bool Save(HANDLE File);
	bool Load(HANDLE File);

	static inline bool Extension::GetError(const char* File, unsigned int Line)
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
};