
#include "Common.h"

/* Actions */

void Extension::SetDistanceModel(int DistanceModel)
{
	alDistanceModel(DistanceModel ? AL_DISTANCE_MODEL+DistanceModel : AL_NONE);
}

void Extension::SetDopplerFactor(float DopplerFactor)
{
	alDopplerFactor(DopplerFactor);
}

void Extension::SetSpeedOfSound(float SpeedOfSound)
{
	alSpeedOfSound(SpeedOfSound);
}