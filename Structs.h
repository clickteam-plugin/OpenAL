#include <map>
#include <list>
#include <string>
using namespace std;

enum ALSignalType
{
	SIGNAL_TYPE_SINE,
	SIGNAL_TYPE_SQUARE,
	SIGNAL_TYPE_SAW,
	SIGNAL_TYPE_TRIANGLE,
	SIGNAL_TYPE_WHITENOISE,
};

struct ALSignal
{
	//Audio format settings.
	ALsizei		SampleRate;
	ALsizei		Channels;
	ALsizei		Bits;

	//Signal related settings.
	ALuint		Type;
	ALfloat		Frequency;
	ALfloat		ModulatorAmplitude;
	ALfloat		ModulatorFrequency;
	//Pre calculated based on samplerate and channel count.
	ALfloat		FrequencyReal;
	//For keeping the signal continuous.
	ALfloat		Phase;
	ALfloat		ModulatorPhase;

	ALSignal(ALuint SampleRate, ALuint Channels, ALuint Bits, ALuint Type, ALfloat Frequency, ALfloat ModulatorAmplitude, ALfloat ModulatorFrequency)
		: SampleRate(SampleRate), Channels(Channels), Bits(Bits), Type(Type), Frequency(Frequency),
		ModulatorAmplitude(ModulatorAmplitude), ModulatorFrequency(ModulatorFrequency), Phase(0), ModulatorPhase(0)
	{
	}
};

enum ALBufferType
{
	BUFFER_TYPE_SAMPLE,
	BUFFER_TYPE_STREAM,
};

struct ALBuffer
{
	//One of ALBufferType. Either sample or stream.
	ALuint	Type;
	union
	{
		//If type is sample.
		ALuint			Handle;
		//If type is stream.
		alureStream*	Stream;
	};
	//For signal callbacks.
	ALSignal*	Signal;

	ALBuffer()
		: Type(0), Handle(0), Signal(0)
	{
	}
};

struct ALSource
{
	//The assiociated OpenAL handle.
	ALuint			Handle;
	//An optional name to indentify the source.
	string			Name;
	//If true, the source will stay until it is manually deleted.
	bool			Permanent;
	//The assiociated buffer for playback.
	ALBuffer*		Buffer;

	ALSource()
		: Permanent(false), Handle(0), Buffer(0)
	{
	}
};

/* 
	Useless crap that I spend WAY too much time on. Keep it simple, duh
*/ 
//struct ALEffect
//{
//	ALuint			Handle;
//};

////Typedefs for alGenerate* and alDelete* function pointers.
//typedef void (__cdecl *ALPointer)(ALsizei Count, ALuint* Pointer);
//
////Abstract class which wraps an arbitrary OpenAL object.
//struct ALWrapper
//{
//	//A handle to an object such as a buffer or source.
//	ALuint	Handle;
//
//	void Generate()
//	{
//		printf("%s::Generate()\n",ClassName());
//		//Get the necessary generation function.
//		ALPointer GenerateFunc = PointerGenerate();
//
//		//Function is null - we can't generate an object. Most likely an extension that's missing.
//		if(!GenerateFunc)
//			return;
//
//		//Clear error status
//		alGetError();
//
//		//Allocate an object.
//		GenerateFunc(1,&Handle);
//
//		//Error occured - make sure the handle is null.
//		if(alGetError())
//		{
//			#ifdef _DEBUG
//			printf("Unable to generate %s.\n",ClassName());
//			#endif
//			Handle = 0;
//		}
//		#ifdef _DEBUG
//		else
//		{
//			printf("Generated %s(%d).\n",ClassName(),Handle);
//		}
//		#endif
//	}
//
//	void Delete()
//	{
//		printf("%s\n",ClassName());
//		if(Handle)
//		{
//			//Get the necessary deletion function.
//			ALPointer DeleteFunc = PointerDelete();
//
//			//Delete the generated object.
//			DeleteFunc(1,&Handle);
//
//			#ifdef _DEBUG
//			if(int Error = alGetError())
//				printf("Unable to delete %s(%d). Error code %#x\n",ClassName(),Handle,Error);
//			else
//				printf("Deleted %s(%d).\n",ClassName(),Handle);
//			#endif
//
//			//Unset the handle so there's no way it can get deleted again.
//			Handle = 0;
//		}	
//	}
//
//	void Trivialize()
//	{
//		//Set the handle to zero so its object won't get freed when Delete is called.
//		Handle = 0;
//	}
//
//	//This function should return a generator function such as alGenBuffers.
//	virtual ALPointer PointerGenerate() = 0;
//
//	//Same but for deletion.
//	virtual ALPointer PointerDelete() = 0;
//
//	//Returns the name of the class for debugging
//	virtual const char* ClassName() = 0;
//};
//
////Buffer objects that hold audio data.
//struct ALBuffer : ALWrapper
//{
//	ALBuffer() { Handle = 0;  }
//	//~ALBuffer() { Delete(); }
//
//	ALPointer PointerGenerate() { return alGenBuffers; }
//	ALPointer PointerDelete() { return (ALPointer)alDeleteBuffers; }
//	const char* ClassName() { return "ALBuffer"; }
//};
//
////Effect objects that hold an EFX extension effect.
//struct ALEffect : ALWrapper
//{
//	ALEffect() { Handle = 0; }
//	//~ALEffect() { Delete(); }
//
//	ALPointer PointerGenerate() { return alGenEffects; }
//	ALPointer PointerDelete() { return alDeleteEffects; }
//	const char* ClassName() { return "ALEffect"; }
//};
//
////Effect objects that hold an EFX extension effect.
//struct ALEffectSlot : ALWrapper
//{
//	ALEffectSlot() { Handle = 0; }
//	//~ALEffectSlot() { Delete(); }
//
//	ALPointer PointerGenerate() { return alGenAuxiliaryEffectSlots; }
//	ALPointer PointerDelete() { return alDeleteAuxiliaryEffectSlots; }
//	const char* ClassName() { return "ALEffectSlot"; }
//};
//
////Filter objects that hold an EFX extension filter.
//struct ALFilter : ALWrapper
//{
//	ALFilter() { Handle = 0; }
//	//~ALFilter() { Delete(); }
//
//	ALPointer PointerGenerate() { return alGenFilters; }
//	ALPointer PointerDelete() { return alDeleteFilters; }
//	const char* ClassName() { return "ALFilter"; }
//};
//
////Source objects that define an audio source in the world.
//struct ALSource : ALWrapper
//{
//	//The name of the source - if empty it is anonymous.
//	string Name;
//	//The effect slots assigned to this source (for EFX).
//	ALEffectSlot* EffectSlot;
//	//A (low-pass) filter that can be enabled when needed.
//	ALFilter* Filter;
//	
//	//Assign the effect slot to the source.
//	void AssignEffectSlot()
//	{
//		if(!Handle || EffectSlot)
//			return;
//
//		//Allocate a new effect slot.
//		EffectSlot = new ALEffectSlot;
//
//		if(!EffectSlot)
//			return;
//		EffectSlot->Generate();
//
//		if(EffectSlot->Handle)
//		{
//			alSource3i(Handle,AL_AUXILIARY_SEND_FILTER,EffectSlot->Handle,0,0);
//			#ifdef _DEBUG
//			if(ALenum Error = alGetError())
//				printf("Unable to assign %s(%d) to %s(%d). Error code %#x.\n",
//					EffectSlot->ClassName(),EffectSlot->Handle,ClassName(),Handle,Error);
//			else
//				printf("Assigned %s(%d) to %s(%d).\n",
//					EffectSlot->ClassName(),EffectSlot->Handle,ClassName(),Handle);
//			#endif
//		}
//	}
//
//	//Assign the filter to the source.
//	void AssignFilter()
//	{
//		if(!Handle || Filter)
//			return;
//
//		//Allocate a new effect slot.
//		Filter = new ALFilter; 
//
//		if(!Filter)
//			return;
//		Filter->Generate();
//
//		if(Filter->Handle)
//		{
//			alSourcei(Handle,AL_DIRECT_FILTER,Filter->Handle);
//			#ifdef _DEBUG
//			if(ALenum Error = alGetError())
//				printf("Unable to assign %s(%d) to %s(%d). Error code %#x.\n",
//					Filter->ClassName(),Filter->Handle,ClassName(),Handle,Error);
//			else
//				printf("Assigned %s(%d) to %s(%d).\n",
//					Filter->ClassName(),Filter->Handle,ClassName(),Handle);
//			#endif
//		}
//	}
//
//	ALSource(string Name) : Name(Name)
//	{
//		Handle = 0;
//		EffectSlot = 0;
//		Filter = 0;
//	}
//
//	void DeleteAll()
//	{
//		//Delete the effect slot.
//		if(EffectSlot)
//		{
//			//Unassign the effect slot from the source.
//			if(Handle)
//				alSource3i(Handle,AL_AUXILIARY_SEND_FILTER,AL_EFFECTSLOT_NULL,0,0);
//			EffectSlot->Delete();
//			delete EffectSlot;
//			EffectSlot = 0;
//		}
//
//		//Delete the assigned filter.
//		if(Filter)
//		{
//			//Unassign the filter from the source.
//			if(Handle)
//				alSourcei(Handle,AL_DIRECT_FILTER,AL_FILTER_NULL);
//			Filter->Delete();
//			delete Filter;
//			Filter = 0;
//		}
//
//		//Delete the main handle.
//		Delete();
//
//		//Set the handle to zero so it can't get freed again.
//		Handle = 0;
//	}
//
//	//Stuff.
//	ALPointer PointerGenerate() { return alGenSources; }
//	ALPointer PointerDelete() { return (ALPointer)alDeleteSources; }
//	const char* ClassName() { return "ALSource"; }
//};