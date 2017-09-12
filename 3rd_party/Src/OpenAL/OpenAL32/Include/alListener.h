#ifndef _AL_LISTENER_H_
#define _AL_LISTENER_H_

#include "AL/al.h"

#include "eax.h"

#ifdef __cplusplus
extern "C" {
#endif

// Flags indicating what Direct Sound parameters need to be updated in the UpdateContext call
#define LVOLUME				1
#define LPOSITION			2
#define LVELOCITY			4
#define LORIENTATION		8
#define LDOPPLERFACTOR		16
#define LDOPPLERVELOCITY	32
#define LDISTANCEMODEL		64
#define LSPEEDOFSOUND		128

typedef struct ALlistener_struct
{
	ALfloat					Position[3];
	ALfloat					Velocity[3];
	ALfloat					Forward[3];
	ALfloat					Up[3];
	ALfloat					Gain;
	ALuint					update1;	// Store changes that need to be made in UpdateContext
	ALint					lVolume;	// Listener Volume (mb) (converted from Listener Gain)
	EAXLISTENERPROPERTIES	EAX20LP;
} ALlistener;

#ifdef __cplusplus
}
#endif

#endif
