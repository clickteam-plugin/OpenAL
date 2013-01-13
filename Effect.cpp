
#include "Common.h"

ALuint Extension::EffectGetFromID(int ID)
{
	map<int, ALuint>::iterator FoundEffect = EffectMap.find(ID);

	//An effect	with the ID was found, return its handle.
	if(FoundEffect != EffectMap.end())
		return FoundEffect->second;

	//Nothing found, return	zero.
	return 0;
}

ALenum Extension::EffectGetParameterIndex(const	char* Name)
{
	if(!Effect)
		return 0;

	const char*	EffectName = 0;

	//Find the current effect type.
	ALint EffectType;
	alGetEffecti(Effect, AL_EFFECT_TYPE, &EffectType);

	switch(EffectType)
	{
		case AL_EFFECT_ECHO: EffectName	= "ECHO"; break;
		case AL_EFFECT_EAXREVERB: EffectName = "EAXREVERB";	break;
		case AL_EFFECT_RING_MODULATOR: EffectName =	"RING_MODULATOR"; break;
	}

	if(!EffectName)
		return 0;

	//Make parameter name CONSTANT_LIKE.
	char CapitalizedName[24] = {0};
	for(int	i =	0; Name[i];	++i)
		CapitalizedName[i] = Name[i] ==	' '	? '_' :	toupper(Name[i]);

	//Construct	OpenAL enum	string.
	char EnumName[64] =	{'A','L','_', 0};
	strcat(EnumName, EffectName);
	strcat(EnumName,"_");
	strcat(EnumName, CapitalizedName);

	return alGetEnumValue(EnumName);
}

void Extension::EffectCreate(int Type)
{
	//Generate an object.
	alGenEffects(1, &Effect);

	int ID = ++LastEffectIndex;

	//Store the effect in the map and assign the effect type to it.
	if(Effect)
	{

		//Delete the old effect in the map.
		//if(EffectMap.find(ID) != EffectMap.end())
		//	alDeleteEffects(1, &EffectMap[ID]);

		EffectMap[ID] = Effect;
		alEffecti(Effect, AL_EFFECT_TYPE, Type);
	}

	//Effect and aux slots are merged for now.
	AuxEffectSlotCreate(ID);
	AuxEffectSlotLoadEffect(Effect);
}

/* Actions */

void Extension::EffectSelectByID(int ID)
{
	Effect = EffectGetFromID(ID);

	//Effect and aux slots are merged for now.
	AuxEffectSlotSelectByID(ID);
}

void Extension::EffectCreateReverb()
{
	EffectCreate(AL_EFFECT_EAXREVERB);
}

void Extension::EffectCreateEcho()
{
	EffectCreate(AL_EFFECT_ECHO);
}

void Extension::EffectCreateRingModulator()
{
	EffectCreate(AL_EFFECT_RING_MODULATOR);
}

void Extension::EffectSetFloatParameter(const char*	Name, float	Value)
{
	if(!Effect || !*Name ||	strlen(Name) >=	24)
		return;

	//Try to find the parameter	enum.
	ALenum Parameter = EffectGetParameterIndex(Name);

	//On success, set the parameter.
	if(Parameter)
	{
		alEffectf(Effect, Parameter, Value);

		//Effect and aux slots are merged for now.
		AuxEffectSlotLoadEffect(Effect);
	}
}

void Extension::EffectSetIntParameter(const	char* Name,	int	Value)
{
	if(!Effect || !*Name ||	strlen(Name) >=	24)
		return;

	//Try to find the parameter	enum.
	ALenum Parameter = EffectGetParameterIndex(Name);

	//On success, set the parameter.
	if(Parameter)
	{
		alEffecti(Effect, Parameter, Value);
		
		//Effect and aux slots are merged for now.
		AuxEffectSlotLoadEffect(Effect);
	}
}


void Extension::EffectLoadReverbPreset(const char* Preset)
{
	if(!Effect)
		return;

	//This function	is so ugly that	it's in	Globals.cpp!
	LoadReverbPreset(Effect, Preset);

	//Effect and aux slots are merged for now.
	AuxEffectSlotLoadEffect(Effect);
}

/* Expressions */

int Extension::EffectGetLast()
{
	return LastEffectIndex;
}

