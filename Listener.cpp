
#include "Common.h"

/* Actions */

void Extension::ListenerSetGain(float Gain)
{
	alListenerf(AL_GAIN, Gain);
}

void Extension::ListenerSetPosition(float X, float Y, float Z)
{
	alListener3f(AL_POSITION, X*Unit, Y*Unit, Z*Unit);
}

void Extension::ListenerSetVelocity(float X, float Y, float Z)
{
	alListener3f(AL_VELOCITY, X*Unit, Y*Unit, Z*Unit);
}

void Extension::ListenerSetOrientation(float XAt, float YAt, float ZAt, float XUp, float YUp, float ZUp)
{
	const ALfloat Orientation[] = {XAt, YAt, ZAt, XUp, YUp, ZUp};
	alListenerfv(AL_ORIENTATION, &Orientation[0]);
}

void Extension::ListenerSetMetersPerUnit(float Meters)
{
	alListenerf(AL_METERS_PER_UNIT, Meters);
	alGetListenerf(AL_METERS_PER_UNIT, &Unit);
}