#ifndef _AL_EAX_H_
#define _AL_EAX_H_

#include "AL/al.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	EAXNONE	= 0x00,

	// Buffer / Source properties
	EAX1B	= 0x01,
	EAX2B	= 0x02,
	EAX3B	= 0x04,
	EAX4S	= 0x08,

	// Listener / Global properties
	EAX1L	= 0x8000,
	EAX2L	= 0x10000,
	EAX3L	= 0x20000,
	EAX4FX0	= 0x40000,
	EAX4FX1 = 0x80000,
	EAX4FX2 = 0x100000,
	EAX4FX3 = 0x200000,
	EAX4CTX = 0x400000,
} EAXGUID;


#define EAXBUFFERGUID	(EAX1B | EAX2B | EAX3B | EAX4S)
#define EAXLISTENERGUID	(EAX1L | EAX2L | EAX3L | EAX4FX0 | EAX4FX1 | EAX4FX2 | EAX4FX3 | EAX4CTX)

ALboolean		CheckEAXSupport(const ALchar *szEAXName);
ALAPI ALenum    ALAPIENTRY EAXGet(const struct _GUID *propertySetID,ALuint property,ALuint source,ALvoid *value,ALuint size);
ALAPI ALenum    ALAPIENTRY EAXSet(const struct _GUID *propertySetID,ALuint property,ALuint source,ALvoid *value,ALuint size);


#ifdef __cplusplus
}
#endif

#endif
