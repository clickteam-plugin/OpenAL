
#include "Common.h"
#include <limits.h>

//Delete a buffer object and its assiociated handles.
void DeleteBuffer(ALBuffer* Buffer)
{
	//Delete the buffer handle.
	if(Buffer->Type == BUFFER_TYPE_SAMPLE && Buffer->Handle)
		alDeleteBuffers(1, &Buffer->Handle);
	//Delete the assiociated stream.
	else if(Buffer->Type == BUFFER_TYPE_STREAM && Buffer->Stream)
		alureDestroyStream(Buffer->Stream, 0, 0);

	//If a signal object is available, delete it.
	if(Buffer->Signal)
	{
		delete Buffer->Signal;
		Buffer->Signal = 0;
	}
}

//Called when a source has finished playing.
void PlaySourceCallback(void* UserData, ALuint SourceHandle)
{
	Extension* Ext = (Extension*)UserData;
	for(list<ALSource>::iterator it = Ext->SourceList.begin(); it != Ext->SourceList.end(); ++it)
	{
		//Found the source in the list.
		if(it->Handle == SourceHandle)
		{
			//Source stopped condition.
			Ext->Runtime.GenerateEvent(2);

			//If it's not permanent, delete it.
			if(!it->Permanent)
			{
				alDeleteSources(1, &SourceHandle);
				Ext->SourceList.erase(it);
			}
			break;
		}
	}
}
//Callback for MMF user-specific data.
ALuint StreamCustomCallback(void* UserData, ALubyte* Buffer, ALuint AvailableBytes)
{
    Extension* Ext = (Extension*)UserData;

    Ext->CallbackBuffer = Buffer;
    Ext->CallbackAvailableBytes = AvailableBytes;
    Ext->CallbackWrittenBytes = 0;

    //Trigger an MMF callback.
    Ext->Runtime.GenerateEvent(4);

    Ext->CallbackBuffer = 0;
    Ext->CallbackAvailableBytes = 0;
    return Ext->CallbackWrittenBytes;
}

//Callback for generic signal streams.
ALuint StreamSignalCallback(void* UserData, ALubyte* Buffer, ALuint AvailableBytes)
{
	ALSignal* Signal = (ALSignal*)UserData;
	ALuint Channels = Signal->Channels;
	ALfloat SampleRate = (ALfloat)Signal->SampleRate;
	ALuint Bits = Signal->Bits;
	ALuint Type = Signal->Type;
	ALfloat Frequency = Signal->Frequency;
	ALfloat ModulatorAmplitude = Signal->ModulatorAmplitude;
	ALfloat ModulatorFrequency = Signal->ModulatorFrequency;

	ALfloat Value;
	const ALfloat TwoPi = 2*3.1415926535f;
	
	//Calculate the number of samples based on the bit depth.
	ALuint AvailableSamples = AvailableBytes*8/Bits;

	for(ALuint i = 0; i < AvailableSamples; ++i)
	{
		//The phase is stored externally to generate a continuous signal between buffers.
		ALfloat Phase = Signal->Phase;

		switch(Type)
		{
			case SIGNAL_TYPE_SINE:
				Value = sin(TwoPi*Phase);
				break;
			case SIGNAL_TYPE_SAW:
				Value = -1 + 2*fmodf(Phase, 1);
				break;
			case SIGNAL_TYPE_TRIANGLE:
				Value = -1 + 4*abs(fmodf(Phase+0.5f, 1)-0.5f);
				break;
			case SIGNAL_TYPE_SQUARE:
				Value = -1 + 4*abs(fmodf(Phase+0.5f, 1)-0.5f);
				Value = max(-1, min(1, Value*1000));
				break;
			case SIGNAL_TYPE_WHITENOISE:
				Value = 2*((ALfloat)rand()/RAND_MAX-0.5f);
				break;
		}
		
		//Increase the phase.
		Signal->Phase += (Frequency+ModulatorAmplitude*sin(TwoPi*Signal->ModulatorPhase))/SampleRate/Channels;
		Signal->ModulatorPhase += ModulatorFrequency/SampleRate/Channels;

		//Avoid clipping.
		Value *= 0.95f;

		//Store the calculated value as 16-bit.
		if(Bits == 16)
			((ALshort*)Buffer)[i] = (ALshort)(Value*SHRT_MAX);
		else
			Buffer[i] = 128+(ALubyte)(Value*127);
	}

	//Avoid overflow.
	Signal->Phase = fmodf(Signal->Phase, 1.0f);

	return AvailableBytes;
}

//Callback for capture stream.
ALuint StreamCaptureCallback(void* UserData, ALubyte* Buffer, ALuint AvailableBytes)
{
    Extension* Ext = (Extension*)UserData;
    //Store the size of a sample in bytes. Equivalent to the block alignment.
    const int SampleSize = Ext->Input.BlockAlign;

    //Read how many samples were captured from the device.
    ALuint CapturedBytes;
    alcGetIntegerv(Ext->Input.Device, ALC_CAPTURE_SAMPLES, 4, (ALint*)&CapturedBytes);
    //Convert samples to bytes.
    CapturedBytes *= SampleSize;
   
	//Didn't capture anything yet.
	if(!CapturedBytes)
	{
		//Fill the buffer with zeros which keeps the stream alive.
		memset(Buffer, 0, AvailableBytes);
		return AvailableBytes;
	}
	
    //Make sure we don't read more than possible.
    ALuint ReadBytes = min(CapturedBytes, AvailableBytes);

    //Convert bytes to samples.
    ALuint ReadSamples = ReadBytes / SampleSize;

    //Write input into stream output buffer.

    alcCaptureSamples(Ext->Input.Device, Buffer, ReadSamples);
	Ext->CaptureBuffer = Buffer;
	Ext->CaptureAvailableBytes = ReadBytes;
    Ext->Runtime.GenerateEvent(3);

    //Return the number of written bytes - block aligned.
    return ReadBytes;
}

#define REVERB_PRESET(XXX)  if(!_stricmp(Preset,#XXX)) { EFXEAXREVERBPROPERTIES Temp = EFX_EAX_REVERB_ ## XXX; Reverb = Temp; break; }
void LoadReverbPreset(ALuint Effect, const char* Preset)
{
	EFXEAXREVERBPROPERTIES Reverb = EFX_EAX_REVERB_GENERIC;

	do
	{
		REVERB_PRESET(GENERIC)
		REVERB_PRESET(PADDEDCELL)
		REVERB_PRESET(ROOM)
		REVERB_PRESET(BATHROOM)
		REVERB_PRESET(LIVINGROOM)
		REVERB_PRESET(STONEROOM)
		REVERB_PRESET(AUDITORIUM)
		REVERB_PRESET(CONCERTHALL)
		REVERB_PRESET(CAVE)
		REVERB_PRESET(ARENA)
		REVERB_PRESET(HANGAR)
		REVERB_PRESET(CARPETTEDHALLWAY)
		REVERB_PRESET(HALLWAY)
		REVERB_PRESET(STONECORRIDOR)
		REVERB_PRESET(ALLEY)
		REVERB_PRESET(FOREST)
		REVERB_PRESET(CITY)
		REVERB_PRESET(MOUNTAINS)
		REVERB_PRESET(QUARRY)
		REVERB_PRESET(PLAIN)
		REVERB_PRESET(PARKINGLOT)
		REVERB_PRESET(SEWERPIPE)
		REVERB_PRESET(UNDERWATER)
		REVERB_PRESET(DRUGGED)
		REVERB_PRESET(DIZZY)
		REVERB_PRESET(PSYCHOTIC)
	} while(0);

	alEffectf(Effect, AL_EAXREVERB_DENSITY, Reverb.flDensity);
	alEffectf(Effect, AL_EAXREVERB_DIFFUSION, Reverb.flDiffusion);
	alEffectf(Effect, AL_EAXREVERB_GAIN, Reverb.flGain);
	alEffectf(Effect, AL_EAXREVERB_GAINHF, Reverb.flGainHF);
	alEffectf(Effect, AL_EAXREVERB_GAINLF, Reverb.flGainLF);
	alEffectf(Effect, AL_EAXREVERB_DECAY_TIME, Reverb.flDecayTime);
	alEffectf(Effect, AL_EAXREVERB_DECAY_HFRATIO, Reverb.flDecayHFRatio);
	alEffectf(Effect, AL_EAXREVERB_DECAY_LFRATIO, Reverb.flDecayLFRatio);
	alEffectf(Effect, AL_EAXREVERB_REFLECTIONS_GAIN, Reverb.flReflectionsGain);
	alEffectf(Effect, AL_EAXREVERB_REFLECTIONS_DELAY, Reverb.flReflectionsDelay);
	alEffectfv(Effect, AL_EAXREVERB_REFLECTIONS_PAN, Reverb.flReflectionsPan);
	alEffectf(Effect, AL_EAXREVERB_LATE_REVERB_GAIN, Reverb.flLateReverbGain);
	alEffectf(Effect, AL_EAXREVERB_LATE_REVERB_DELAY, Reverb.flLateReverbDelay);
	alEffectfv(Effect, AL_EAXREVERB_LATE_REVERB_PAN, Reverb.flLateReverbPan);
	alEffectf(Effect, AL_EAXREVERB_ECHO_TIME, Reverb.flEchoTime);
	alEffectf(Effect, AL_EAXREVERB_ECHO_DEPTH, Reverb.flEchoDepth);
	alEffectf(Effect, AL_EAXREVERB_MODULATION_TIME, Reverb.flModulationTime);
	alEffectf(Effect, AL_EAXREVERB_MODULATION_DEPTH, Reverb.flModulationDepth);
	alEffectf(Effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, Reverb.flAirAbsorptionGainHF);
	alEffectf(Effect, AL_EAXREVERB_HFREFERENCE, Reverb.flHFReference);
	alEffectf(Effect, AL_EAXREVERB_LFREFERENCE, Reverb.flLFReference);
	alEffectf(Effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, Reverb.flRoomRolloffFactor);
	alEffecti(Effect, AL_EAXREVERB_DECAY_HFLIMIT, Reverb.iDecayHFLimit);
}

/* FAST FOURIER STUFF */

inline bool IsPowerOfTwo( unsigned int p_nX )
{
    return (p_nX >= 2) && !(p_nX & (p_nX-1));
}

unsigned int NumberOfBitsNeeded( unsigned int p_nSamples )
{

    int i;

    if( p_nSamples < 2 )
    {
        return 0;
    }

    for ( i=0; ; i++ )
    {
        if( p_nSamples & (1 << i) ) return i;
    }

}

unsigned int ReverseBits(unsigned int p_nIndex, unsigned int p_nBits)
{

    unsigned int i, rev;

    for(i=rev=0; i < p_nBits; i++)
    {
        rev = (rev << 1) | (p_nIndex & 1);
        p_nIndex >>= 1;
    }

    return rev;
}

double Index_to_frequency(unsigned int p_nBaseFreq, 
    unsigned int p_nSamples, unsigned int p_nIndex)
{

    if(p_nIndex >= p_nSamples)
    {
        return 0.0;
    }
    else if(p_nIndex <= p_nSamples/2)
    {
        return ( (double)p_nIndex / 
                 (double)p_nSamples * p_nBaseFreq );
    }
    else
    {
        return ( -(double)(p_nSamples-p_nIndex) / 
                  (double)p_nSamples * p_nBaseFreq );
    }

}

void fft_double (unsigned int p_nSamples, bool p_bInverseTransform, 
    double *p_lpRealIn, double *p_lpImagIn, 
    double *p_lpRealOut, double *p_lpImagOut)
{
    unsigned int NumBits;
    unsigned int i, j, k, n;
    unsigned int BlockSize, BlockEnd;

    double angle_numerator = 2.0f * 3.1415926535f;
    double tr, ti;

    if(!p_lpRealIn || !p_lpRealOut || !p_lpImagOut) return;

    if( !IsPowerOfTwo(p_nSamples) )
    {
        return;
    }

    if( p_bInverseTransform ) angle_numerator = -angle_numerator;

    NumBits = NumberOfBitsNeeded ( p_nSamples );


    for( i=0; i < p_nSamples; i++ )
    {
        j = ReverseBits ( i, NumBits );
        p_lpRealOut[j] = p_lpRealIn[i];
        p_lpImagOut[j] = (p_lpImagIn == NULL) ? 0.0f : p_lpImagIn[i];
    }

    BlockEnd = 1;
    for( BlockSize = 2; BlockSize <= p_nSamples; BlockSize <<= 1 )
    {
        double delta_angle = angle_numerator / (double)BlockSize;
        double sm2 = sin ( -2 * delta_angle );
        double sm1 = sin ( -delta_angle );
        double cm2 = cos ( -2 * delta_angle );
        double cm1 = cos ( -delta_angle );
        double w = 2 * cm1;
        double ar[3], ai[3];

        for( i=0; i < p_nSamples; i += BlockSize )
        {

            ar[2] = cm2;
            ar[1] = cm1;

            ai[2] = sm2;
            ai[1] = sm1;

            for ( j=i, n=0; n < BlockEnd; j++, n++ )
            {

                ar[0] = w*ar[1] - ar[2];
                ar[2] = ar[1];
                ar[1] = ar[0];

                ai[0] = w*ai[1] - ai[2];
                ai[2] = ai[1];
                ai[1] = ai[0];

                k = j + BlockEnd;
                tr = ar[0]*p_lpRealOut[k] - ai[0]*p_lpImagOut[k];
                ti = ar[0]*p_lpImagOut[k] + ai[0]*p_lpRealOut[k];

                p_lpRealOut[k] = p_lpRealOut[j] - tr;
                p_lpImagOut[k] = p_lpImagOut[j] - ti;

                p_lpRealOut[j] += tr;
                p_lpImagOut[j] += ti;

            }
        }

        BlockEnd = BlockSize;

    }


    if( p_bInverseTransform )
    {
        double denom = (double)p_nSamples;

        for ( i=0; i < p_nSamples; i++ )
        {
            p_lpRealOut[i] /= denom;
            p_lpImagOut[i] /= denom;
        }
    }
}