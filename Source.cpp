
#include "Common.h"
#include "AluStuff.h"

//Quick function that checks if a source is selected.
bool Extension::IsSource()
{
	return Source && Source->Handle;//&& alIsSource(Source->Handle);
}

ALSource* Extension::SourceGetFromName(const char* Name)
{
	for(list<ALSource>::iterator it = SourceList.begin(); it != SourceList.end(); ++it)
	{
		//A source with the name was found, return its handle.
		if(it->Name == Name)
			return &(*it);
	}


	//Nothing found, return zero.
	return 0;
}

/* Actions */

void Extension::SourceCreate()
{
	//Simply create a source with no name.
	SourceCreateNamed("");
}

void Extension::SourceCreateNamed(const char* Name)
{
	//Create a new source.
	ALSource NewSource;
	NewSource.Name = Name;
	alGenSources(1, &NewSource.Handle);

	//If successful, store the source in the map and select it.
	if(NewSource.Handle && alGetError() == AL_NO_ERROR)
	{
		SourceList.push_back(NewSource);
		Source = &SourceList.back();
	}
} 

void Extension::SourceCreateWithBuffer(const char* Buffer)
{	
	SourceCreateNamed("");
	SourceSetBuffer(Buffer);
}

void Extension::SourceCreateNamedWithBuffer(const char* Name, const char* Buffer)
{
	SourceCreateNamed(Name);
	SourceSetBuffer(Buffer);
}

void Extension::SourceSelectByName(const char* Name)
{
	//Loop through all sources and find one with a matching name.
	for(list<ALSource>::iterator it = SourceList.begin(); it != SourceList.end(); ++it)
	{
		if(!strcmp(it->Name.c_str(), Name))
		{
			Source = &*it;
			return;
		}
	}

	//Nothing found, unset source.
	Source = 0;
}

void Extension::SourceSelectByHandle(int Handle)
{
	//Loop through all sources and find one with a matching handle.
	for(list<ALSource>::iterator it = SourceList.begin(); it != SourceList.end(); ++it)
	{
		if(it->Handle == Handle)
		{
			Source = &*it;
			return;
		}
	}

	//Nothing found, unset source.
	Source = 0;
}

void Extension::SourcePlay()
{
	if(IsSource() && Source->Buffer)
	{
		//Buffer holds a stream.
		if(Source->Buffer->Type == BUFFER_TYPE_STREAM)
		{
			//Streams have a separate loop parameter.
			ALint Looping;
			alGetSourcei(Source->Handle, AL_LOOPING, &Looping);

			alurePlaySourceStream(Source->Handle, Source->Buffer->Stream, 3, Looping ? -1 : 0, PlaySourceCallback, this);
		}
		//Buffer must be a sample.
		else
			alurePlaySource(Source->Handle, PlaySourceCallback, this);
	}
}

void Extension::SourcePause()
{
	 if(IsSource())
		alurePauseSource(Source->Handle);
}

void Extension::SourceStop()
{
	 if(IsSource())
		alureStopSource(Source->Handle, true);
}

void Extension::SourceResume()
{
	 if(IsSource() && Source->Buffer)
		alureResumeSource(Source->Handle);
}

void Extension::SourceRewind()
{
	if(IsSource() && Source->Buffer)
	{
		//Buffer holds a stream.
		if(alureStream* Stream = Source->Buffer->Stream)
			alureRewindStream(Stream);

		//Buffer has a handle - play sample
		else if(alIsBuffer(Source->Buffer->Handle))
			alSourceRewind(Source->Handle);
	}
}

void Extension::SourceSetBuffer(const char* BufferName)
{
	if(IsSource())
	{
		//Find the buffer based on its name
		ALBuffer* Buffer = BufferGetFromName(BufferName);

		//Store a pointer in the internal selected source.
		Source->Buffer = Buffer;

		//If found, assign it. Otherwise, assign null.
		if(Buffer && Buffer->Type == BUFFER_TYPE_SAMPLE)
		{
			//Assign it to the source.
			alSourcei(Source->Handle, AL_BUFFER, Buffer->Handle);
		}
		else
		{
			//This should be the case for streams.
			alSourcei(Source->Handle, AL_BUFFER, 0);
		}
	}
}

void Extension::SourceSetPermanent(int Permanent)
{
	if(IsSource())
	{
		//If this is true, the source won't be automatically deleted after playback finished.
		Source->Permanent = Permanent != 0;
	}
}

void Extension::SourceSetLooping(int Looping)
{
	if(IsSource())
	{
		Looping = Looping ? AL_TRUE : AL_FALSE;
		alSourcei(Source->Handle, AL_LOOPING, Looping);
	}
}

void Extension::SourceSetGain(float Gain)
{
	if(IsSource())
	{
		Gain = max(0, min(1, Gain));
		alSourcef(Source->Handle, AL_GAIN, Gain);
	}
}

void Extension::SourceSetPitch(float Pitch)
{
	if(IsSource())
	{
		Pitch = max(0.00001f, Pitch);
		alSourcef(Source->Handle, AL_PITCH, Pitch);
	}
}

void Extension::SourceSetSampleOffset(int Sample)
{
	if(IsSource() && Source->Buffer)
	{    
		Sample = max(0, Sample);
		//Source plays a sample.
		if(Source->Buffer->Type == BUFFER_TYPE_SAMPLE)
		{
			alSourcei(Source->Handle, AL_SAMPLE_OFFSET, Sample);
		}
		//Source plays a stream.
		else
		{
			//TODO MATHIAS
			//alureSeekStream(Source->Buffer->Stream, Sample);
		}
	}
}

void Extension::SourceSetSecondOffset(float Second)
{
	if(IsSource() && Source->Buffer)
	{
		Second = max(0, Second);
		//Source plays a sample.
		if(Source->Buffer->Type == BUFFER_TYPE_SAMPLE)
		{
			alSourcef(Source->Handle, AL_SEC_OFFSET, Second);
		}
		//Source plays a stream.
		else
		{
			ALsizei Frequency = alureGetStreamFrequency(Source->Buffer->Stream);
			//TODO MATHIAS
			//alureSeekStream(Source->Buffer->Stream, (alureInt64)(Second*Frequency));
		}
	}
}

void Extension::SourceSetPosition(float X, float Y, float Z)
{
	if(IsSource())
		alSource3f(Source->Handle, AL_POSITION, X*Unit, Y*Unit, Z*Unit);
}

void Extension::SourceSetVelocity(float X, float Y, float Z)
{
	if(IsSource())
		alSource3f(Source->Handle, AL_VELOCITY, X*Unit, Y*Unit, Z*Unit);
}

void Extension::SourceSetDirection(float X, float Y, float Z)
{
	if(IsSource())
		alSource3f(Source->Handle, AL_DIRECTION, X, Y, Z);
}
   
void Extension::SourceSetConeAngles(float Inner, float Outer)
{
	if(IsSource())
	{
		alSourcef(Source->Handle, AL_CONE_INNER_ANGLE, Inner);
		alSourcef(Source->Handle, AL_CONE_OUTER_ANGLE, Outer);
	}
}

void Extension::SourceSetConeOuterGain(float Gain)
{
	if(IsSource())
	{
		Gain = max(0, min(1, Gain));
		alSourcef(Source->Handle, AL_CONE_OUTER_GAIN, Gain);
   }
}

void Extension::SourceSetConeOuterGainHF(float GainHF)
{
	if(IsSource())
	{
		GainHF = max(0, min(10, GainHF));
		alSourcef(Source->Handle, AL_CONE_OUTER_GAINHF, GainHF);
	}
}

void Extension::SourceSetRelative(int Relative)
{
	if(IsSource())
	{
		Relative = Relative ? AL_TRUE : AL_FALSE;
		alSourcei(Source->Handle, AL_SOURCE_RELATIVE, Relative);
	}
}

void Extension::SourceSetMinimumGain(float Gain)
{
	if(IsSource())
	{
		Gain = max(0, min(1, Gain));
		if(IsSource())
			alSourcef(Source->Handle, AL_MIN_GAIN, Gain);
	}
}

void Extension::SourceSetMaximumGain(float Gain)
{
	if(IsSource())
	{
		Gain = max(0, min(1, Gain));
		alSourcef(Source->Handle, AL_MAX_GAIN, Gain);
	}
}

void Extension::SourceSetReferenceDistance(float Distance)
{
	if(IsSource())
	{
		Distance = max(0, Distance*Unit);
		alSourcef(Source->Handle, AL_REFERENCE_DISTANCE, Distance);
	}
}

void Extension::SourceSetMaximumDistance(float Distance)
{
	if(IsSource())
	{
		Distance = max(0, Distance*Unit);
		alSourcef(Source->Handle, AL_MAX_DISTANCE, Distance);
	}
}

void Extension::SourceSetRolloffFactor(float RolloffFactor)
{
	if(IsSource())
	{
		RolloffFactor = max(0, RolloffFactor);
		alSourcef(Source->Handle, AL_ROLLOFF_FACTOR, RolloffFactor);
	}
}

void Extension::SourceSetRoomRolloffFactor(float RolloffFactor)
{
	if(IsSource())
	{
		RolloffFactor = max(0, min(10, RolloffFactor));
		alSourcef(Source->Handle, AL_ROOM_ROLLOFF_FACTOR, RolloffFactor);
	}
}

void Extension::SourceSetAirAbsorptionFactor(float AirAbsorption)
{
	if(IsSource())
	{
		AirAbsorption = max(0, min(10, AirAbsorption));
		alSourcef(Source->Handle, AL_AIR_ABSORPTION_FACTOR, AirAbsorption);
	}
}

void Extension::SourceLoadDirectFilter()
{
	if(IsSource())
	{
		ALuint FoundFilter = FilterGetFromID(LastFilterIndex);

		//Assign the direct (dry) filter to the source, otherwise set none.
		if(FoundFilter)
			alSourcei(Source->Handle, AL_DIRECT_FILTER, FoundFilter);
	}
}

void Extension::SourceSetAuxiliarySend(int Send, int AuxSlot)
{
	SourceSetAuxiliarySendWithFilter(Send, AuxSlot, 0);
}

void Extension::SourceSetAuxiliarySendWithFilter(int Send, int AuxSlot, int Filter)
{
	if(!IsSource() || Send < 0 || Send > 3)
		return;

	ALuint FoundAux = AuxEffectSlotGetFromID(AuxSlot);
	ALuint FoundFilter = FilterGetFromID(Filter);

	//Assign, if any, filters and auxiliary slots to the source send ID.
	alSource3i(Source->Handle, AL_AUXILIARY_SEND_FILTER, FoundAux, Send, FoundFilter);
}

void Extension::SourceSetAuxiliarySendGainAutoAdjust(int Adjust)
{
	if(!IsSource())
		return;

	Adjust = Adjust ? AL_TRUE : AL_FALSE;
	alSourcei(Source->Handle, AL_AUXILIARY_SEND_FILTER_GAIN_AUTO, Adjust);
}

void Extension::SourceSetAuxiliarySendGainHFAutoAdjust(int Adjust)
{
	if(!IsSource())
		return;

	Adjust = Adjust ? AL_TRUE : AL_FALSE;
	alSourcei(Source->Handle, AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO, Adjust);
}

/* Conditions */

bool Extension::SourceOnStopped()
{
	return true;
}

bool Extension::SourceIsPlaying(const char* Name)
{
	ALSource* FoundSource = SourceGetFromName(Name);

	if(!FoundSource)
		return false;

	ALint State;
	alGetSourcei(FoundSource->Handle, AL_SOURCE_STATE, &State);

	return State == AL_PLAYING;
}

bool Extension::SourceExists(const char* Name)
{
	return SourceGetFromName(Name) != 0;
}

/* Expressions */

int Extension::SourceSelectedGetHandle()
{
	return IsSource() ? Source->Handle : 0;
}

char* Extension::SourceSelectedGetName()
{
	return IsSource() ? Runtime.CopyString(Source->Name.c_str()) : "";
}

int Extension::SourceByNameGetHandle(const char* Name)
{
	ALSource* FoundSource = SourceGetFromName(Name);

	if(!FoundSource)
		return 0;

	return FoundSource->Handle;
}

float Extension::SourceByNameGetSecondOffset(const char* Name)
{
	ALSource* FoundSource = SourceGetFromName(Name);

	if(!FoundSource)
		return 0.0f;

	//Source is a sample.
	if(FoundSource->Buffer->Type == BUFFER_TYPE_SAMPLE)
	{
		float Pos;
		alGetSourcef(FoundSource->Handle, AL_SEC_OFFSET, &Pos);
		return Pos;
	}
	//Source is a stream.
	else
	{
		alureStream* Stream = FoundSource->Buffer->Stream;
		if(!Stream)
			return 0.0f;
		//Get the sample offset and convert it to seconds.
		//TODO MATHIAS
		alureInt64 Pos = 1;// alureGetStreamPos(Stream);
		ALsizei Frequency = alureGetStreamFrequency(Source->Buffer->Stream);
		return (float)Pos/Frequency;
	}
}

float Extension::SourceByNameGetSecondLength(const char* Name)
{
	ALSource* FoundSource = SourceGetFromName(Name);

	if(!FoundSource)
		return 0.0f;

	//Source is a sample.
	if(FoundSource->Buffer->Type == BUFFER_TYPE_SAMPLE)
	{
		//Get the buffer size in bytes and convert it to seconds.
		ALint Size, Bits, Channels, Frequency;
		alGetBufferi(FoundSource->Buffer->Handle, AL_SIZE, &Size);
		alGetBufferi(FoundSource->Buffer->Handle, AL_BITS, &Bits);
		alGetBufferi(FoundSource->Buffer->Handle, AL_CHANNELS, &Channels);
		alGetBufferi(FoundSource->Buffer->Handle, AL_FREQUENCY, &Frequency);
		return (float)Size/(Bits*Channels*Frequency/8);
	}
	//Source is a stream.
	else
	{
		alureStream* Stream = FoundSource->Buffer->Stream;
		if(!Stream)
			return 0.0f;
		//Get the sample offset and convert it to seconds.
		alureInt64 Length = alureGetStreamLength(Stream);
		ALsizei Frequency = alureGetStreamFrequency(Source->Buffer->Stream);
		return (float)Length/Frequency;
	}
}

float Extension::SourceByNameGetDistance(const char* Name)
{
	ALSource* FoundSource = SourceGetFromName(Name);

	if(!FoundSource)
		return 0.0f;
		
	ALint DistanceModel;
	ALint IsRelative;
	ALfloat Distance, ClampedDist;
	ALfloat Position[3];
	ALfloat ListenerPos[3];
	ALfloat MinVolume, MaxVolume, MinDist, MaxDist, Rolloff;	
	ALfloat Attenuation;

	DistanceModel = alGetInteger(AL_DISTANCE_MODEL);
	alGetListenerfv(AL_POSITION, ListenerPos);

	alGetSourcefv(FoundSource->Handle, AL_POSITION, Position);
	alGetSourcef(FoundSource->Handle, AL_MIN_GAIN, &MinVolume);
	alGetSourcef(FoundSource->Handle, AL_MAX_GAIN, &MaxVolume);
	alGetSourcef(FoundSource->Handle, AL_REFERENCE_DISTANCE, &MinDist);
	alGetSourcef(FoundSource->Handle, AL_MAX_DISTANCE, &MaxDist);
	alGetSourcef(FoundSource->Handle, AL_ROLLOFF_FACTOR, &Rolloff);
	alGetSourcei(FoundSource->Handle, AL_SOURCE_RELATIVE, &IsRelative);

	if(!IsRelative)
	{
		Position[0] -= ListenerPos[0];
		Position[1] -= ListenerPos[1];
		Position[2] -= ListenerPos[2];
	}

	//2. Calculate distance attenuation
	Distance = sqrt(aluDotproduct(Position, Position));
	ClampedDist = Distance;

	switch(DistanceModel)
	{
		case AL_INVERSE_DISTANCE_CLAMPED:
			ClampedDist = clampf(ClampedDist, MinDist, MaxDist);
			if(MaxDist < MinDist)
				break;
			//fall-through
		case AL_INVERSE_DISTANCE:
			if(MinDist > 0.0f)
			{
				if((MinDist + (Rolloff * (ClampedDist - MinDist))) > 0.0f)
					Attenuation = MinDist / (MinDist + (Rolloff * (ClampedDist - MinDist)));
			}
			break;

		case AL_LINEAR_DISTANCE_CLAMPED:
			ClampedDist = clampf(ClampedDist, MinDist, MaxDist);
			if(MaxDist < MinDist)
				break;
			//fall-through
		case AL_LINEAR_DISTANCE:
			if(MaxDist != MinDist)
			{
				Attenuation = 1.0f - (Rolloff*(ClampedDist-MinDist)/(MaxDist - MinDist));
				Attenuation = max(Attenuation, 0.0f);
			}
			break;

		case AL_EXPONENT_DISTANCE_CLAMPED:
			ClampedDist = clampf(ClampedDist, MinDist, MaxDist);
			if(MaxDist < MinDist)
				break;
			//fall-through
		case AL_EXPONENT_DISTANCE:
			if(ClampedDist > 0.0f && MinDist > 0.0f)
			{
				Attenuation = pow(ClampedDist/MinDist, -Rolloff);
			}
			break;

	}

	return (MinDist/Attenuation - MinDist)*Unit;
}