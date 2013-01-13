
#include "Common.h"

ALuint Extension::FilterGetFromID(int ID)
{
	map<int, ALuint>::iterator FoundFilter = FilterMap.find(ID);

	//A filter with the ID was found, return	its	handle.
	if(FoundFilter != FilterMap.end())
		return FoundFilter->second;

	//Nothing found, return	zero.
	return 0;
}

/* Actions */

void Extension::FilterCreateLowPass()
{
	//Generate the filter object.
	alGenFilters(1, &Filter);

	int ID = ++LastFilterIndex;

	if(Filter)
	{
		//Delete the old filter in the map.
		//if(FilterMap.find(Name) != FilterMap.end())
		//	alDeleteFilters(1, &FilterMap[Name]);

		//Store the filter in the map. 
		FilterMap[ID] = Filter;

		//Attach a low-pass.
		alFilteri(Filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
	}
}

void Extension::FilterSelectByID(int ID)
{
	Filter = FilterGetFromID(ID);
}

void Extension::FilterSetGain(float Gain)
{
	if(alIsFilter(Filter))
		alFilterf(Filter, AL_LOWPASS_GAIN, Gain);
}

void Extension::FilterSetGainHF(float GainHF)
{
	if(alIsFilter(Filter))
		alFilterf(Filter, AL_LOWPASS_GAINHF, GainHF);
}

/* Expressions */

int Extension::FilterGetLast()
{
	return LastFilterIndex;
}
