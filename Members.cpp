
#include "Common.h"

//OpenGL decoder function
unsigned long Extension::DecodeOggVorbis(OggVorbis_File *VorbisFile, char *DecodeBuffer, unsigned long BufferSize, unsigned long Channels)
{
	int CurrentSection;
	long DecodeSize;
	short *Samples;

	unsigned long BytesDone = 0;
	while(true)
	{
		DecodeSize = ov_read(VorbisFile,DecodeBuffer+BytesDone,BufferSize-BytesDone,0,2,1,&CurrentSection);
		if(DecodeSize > 0)
		{
			BytesDone += DecodeSize;

			if (BytesDone >= BufferSize)
				break;
		}
		else
		{
			break;
		}
	}

	// Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
	// however 6-Channels files need to be re-ordered
	if (Channels == 6)
	{		
		Samples = (short*)DecodeBuffer;
		for (unsigned long i = 0; i < (BufferSize>>1); i += 6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
			swap(Samples[i+1],Samples[i+2]);
			swap(Samples[i+3],Samples[i+5]);
			swap(Samples[i+4],Samples[i+5]);
		}
	}

	return BytesDone;
}