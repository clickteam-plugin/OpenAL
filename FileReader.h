#pragma once
#include "Extension.h"

class FileReader
{
	ALenum Format;
	ALvoid* Data;
	ALsizei Size;
	ALsizei Frequency;

public:

	FileReader()
	{
		Reset();
	}

	~FileReader()
	{
		//Deleted the allocated buffer.
		if(Data)
			delete[] Data;
	}

	void Reset()
	{
		Format = AL_FORMAT_MONO8;
		Data = 0;
		Size = 0;
		Frequency = 0;
	}

	virtual bool LoadFile(const char* FilePath) = 0;
};