#pragma once

// #define TGFEXT       // TGF2, MMF2, MMF2 Dev
   #define MMFEXT       // MMF2, MMF2 Dev
// #define PROEXT       // MMF2 Dev only

//OpenAL includes
#define AL_LIBTYPE_STATIC
#define AL_ALEXT_PROTOTYPES
#include <al.h>
#include <alc.h>
#include <efx.h>
#ifdef _DEBUG
#pragma comment(lib,"OpenAL32d.lib")
#else
#pragma comment(lib,"OpenAL32.lib")
#endif

#define GetALError() Extension::GetError(__FILE__, __LINE__)

//Alure for OGG support
#define ALURE_STATIC_LIBRARY
#include <alure.h>
#ifdef _DEBUG
#pragma comment(lib,"c:\\C++\\src\\alure-1.2\\build\\Debug\\Alured.lib")
#else
#pragma comment(lib,"Alure.lib")
#endif

#pragma comment(lib,"libogg_static.lib")
#pragma comment(lib,"libvorbis_static.lib")
#pragma comment(lib,"libvorbisfile_static.lib")

//OpenAL-Soft requires WinMM
#pragma comment(lib,"winmm.lib")

//Custom includes
#include "Structs.h"
#include "EFXPresets.h"

//Global functions in Globals.cpp
void DeleteBuffer(ALBuffer* Buffer);
void PlaySourceCallback(void* UserData, ALuint SourceHandle);
ALuint StreamSignalCallback(void* UserData, ALubyte* Buffer, ALuint AvailableBytes);
ALuint StreamCaptureCallback(void* UserData, ALubyte* Buffer, ALuint AvailableBytes);
ALuint StreamCustomCallback(void* UserData, ALubyte* Buffer, ALuint AvailableBytes);
void LoadReverbPreset(ALuint Effect, const char* Preset);

//EDIF
#include "Edif.h"
#include "Resource.h"

// edPtr : Used at edittime and saved in the MFA/CCN/EXE files

struct EDITDATA
{
	// Header - required
	extHeader		eHeader;

	// Object's data

    // Give menu items a tag (first parameter of a menu item in the
    // JSON) and you'll be able to show/hide them with these functions.
    // This is useful when the properties enable/disable features.

    void ShowMenuItems(const char * Tag);
    void HideMenuItems(const char * Tag);

    // At the end to save screwing up the offset of your variables

    //Edif::Edittime Edif;
};

class Extension;

struct RUNDATA
{
	// Main header - required
	headerObject rHo;

	// Optional headers - depend on the OEFLAGS value, see documentation and examples for more info
//	rCom			rc;				// Common structure for movements & animations
//	rMvt			rm;				// Movements
//	rSpr			rs;				// Sprite (displayable objects)
	rVal			rv;				// Alterable values

    // Required
	Extension * pExtension;

    /*
        You shouldn't add any variables or anything here. Add them as members
        of the Extension class (Extension.h) instead.
    */
};

#include "Extension.h"
