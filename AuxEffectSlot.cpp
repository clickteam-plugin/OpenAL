
#include "Common.h"

ALuint Extension::AuxEffectSlotGetFromID(int ID)
{
	map<int, ALuint>::iterator FoundAux = AuxSlotMap.find(ID);

	//An auxiliary slot with the ID was found, return its handle.
	if(FoundAux != AuxSlotMap.end())
		return FoundAux->second;

	//Nothing found, return	zero.
	return 0;
}

void Extension::AuxEffectSlotLoadEffect(ALuint Handle)
{
	if(!alIsAuxiliaryEffectSlot(AuxSlot))
		return;

	alAuxiliaryEffectSloti(AuxSlot, AL_EFFECTSLOT_EFFECT, Handle);
}

/* Actions */

void Extension::AuxEffectSlotCreate(int ID)
{
	//Generate an object.
	alGenAuxiliaryEffectSlots(1, &AuxSlot);

	//Store the auxiliary slot in the map.
	if(AuxSlot)
	{
		//Delete the old effect in the map.
		if(AuxSlotMap.find(ID) != AuxSlotMap.end())
			alDeleteAuxiliaryEffectSlots(1, &AuxSlotMap[ID]);

		AuxSlotMap[ID] = AuxSlot;
	}
}

void Extension::AuxEffectSlotSelectByID(int ID)
{
	AuxSlot = AuxEffectSlotGetFromID(ID);
}


void Extension::AuxEffectSlotSetGain(float Gain)
{
	if(!alIsAuxiliaryEffectSlot(AuxSlot))
		return;

	Gain = max(0, min(1, Gain));
	alAuxiliaryEffectSlotf(AuxSlot, AL_EFFECTSLOT_GAIN, Gain);
}

void Extension::AuxEffectSlotSetAutoAdjust(int Adjust)
{
	if(!alIsAuxiliaryEffectSlot(AuxSlot))
		return;

	Adjust = Adjust ? AL_TRUE : AL_FALSE;
	alAuxiliaryEffectSloti(AuxSlot, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, Adjust);
}

void Extension::AuxEffectSlotLoadEffectByID(int ID)
{
	AuxEffectSlotLoadEffect(EffectGetFromID(ID));
}