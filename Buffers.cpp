
#include "Common.h"

ALBuffer* Extension::BufferGetFromName(const char* Name)
{
	map<string, ALBuffer>::iterator FoundBuffer = BufferMap.find(Name);

	//A buffer with the name was found, return its handle.
	if(FoundBuffer != BufferMap.end())
		return &FoundBuffer->second;

	//Nothing found, return zero.
	return 0;
}

/* Actions */

void Extension::BufferCreateSampleForFile(const char* Name, const char* FilePath)
{
	//Do not allow empty names.
	if(*Name)
	{
		//Select the buffer for editing.
		ALBuffer* Buffer = &BufferMap[Name];
		DeleteBuffer(Buffer);

		//Let Alure load the file and generate the buffer object.
		Buffer->Handle = alureCreateBufferFromFile(FilePath);
		Buffer->Type = BUFFER_TYPE_SAMPLE;
	}
}	

void Extension::BufferCreateStreamForFile(const char* Name, const char* FilePath)
{
	//Do not allow empty names.
	if(*Name)
	{
		//Select the buffer for editing.
		ALBuffer* Buffer = &BufferMap[Name];
		DeleteBuffer(Buffer);

		//Let Alure load the file and generate the buffer object.
	    alureStreamSizeIsMicroSec(true);
		Buffer->Stream = alureCreateStreamFromFile(FilePath, 250000, 0, 0);
		Buffer->Type = BUFFER_TYPE_STREAM;
	}
}

void Extension::BufferCreateStreamForCapture(const char* Name)
{
	//Do not allow empty names.
	if(*Name)
	{
		//Select the buffer for editing.
		ALBuffer*Buffer = &BufferMap[Name];
		DeleteBuffer(Buffer);

		CaptureRecalculateSettings();

		//Let Alure load the file and generate the buffer object.
	    alureStreamSizeIsMicroSec(false);
        Buffer->Stream = alureCreateStreamFromCallback(StreamCaptureCallback, this, Input.Format, Input.Frequency, Input.BufferBytes, 0, 0);
		Buffer->Type = BUFFER_TYPE_STREAM;
	}
}

void Extension::BufferCreateStreamForSignal(const char* Name, ALuint SampleRate, ALuint Channels, ALuint Bits, const char* Type, ALfloat Frequency, ALfloat ModulatorAmplitude, ALfloat ModulatorFrequency)
{
	if(!*Name || Channels < 1 || Channels > 2 || (Bits != 8 && Bits != 16) || SampleRate < 0)
		return;

	//Get the signal type enum based on the name.
	ALuint IntType = SIGNAL_TYPE_WHITENOISE;
	if(!_strnicmp("sine", Type, 4))
		IntType = SIGNAL_TYPE_SINE;
	else if(!_strnicmp("saw", Type, 3))
		IntType = SIGNAL_TYPE_SAW;
	else if(!_strnicmp("square", Type, 6))
		IntType = SIGNAL_TYPE_SQUARE;
	else if(!_strnicmp("triangle", Type, 8))
		IntType = SIGNAL_TYPE_TRIANGLE;

	//Select the buffer for editing.
	ALBuffer* Buffer = &BufferMap[Name];
	DeleteBuffer(Buffer);

	//Store the parameters in the signal object.
	Buffer->Signal = new ALSignal(SampleRate, Channels, Bits, IntType, Frequency, ModulatorAmplitude, ModulatorFrequency);

	//Let Alure load the file and generate the buffer object.
    alureStreamSizeIsMicroSec(false);
	GetALError();
	Buffer->Stream = alureCreateStreamFromCallback(StreamSignalCallback, Buffer->Signal, alureGetSampleFormat(Channels, Bits, 0), SampleRate, 1024*Channels*Bits/8, 0, 0);
	Buffer->Type = BUFFER_TYPE_STREAM;
}

void Extension::BufferCreateStreamForCallback(const char* Name, ALuint SampleRate, ALuint Channels, ALuint Bits, ALuint BufferSize)
{
	if(!*Name || Channels < 1 || Channels > 2 || (Bits != 8 && Bits != 16) || SampleRate < 0 || BufferSize <= 0)
		return;

	//Select the buffer for editing.
	ALBuffer* Buffer = &BufferMap[Name];
	DeleteBuffer(Buffer);
	
	//Let Alure load the file and generate the buffer object.
	Buffer->Stream = alureCreateStreamFromCallback(StreamCustomCallback, this, alureGetSampleFormat(Channels, Bits, 0), SampleRate, BufferSize, 0, 0);
	Buffer->Type = BUFFER_TYPE_STREAM;
}

void Extension::BufferCallbackWriteBytes(int Bytes)
{
	CallbackWrittenBytes = max(0, Bytes);
}

/* Expressions */

int Extension::BufferCallbackGetAddress()
{
	return (int)CallbackBuffer;
}

int Extension::BufferCallbackGetBytes()
{
	return CallbackAvailableBytes;
}