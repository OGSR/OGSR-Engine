#ifndef _ALU_H_
#define _ALU_H_

#define ALUAPI
#define ALUAPIENTRY __cdecl

#define BUFFERSIZE 48000
#define FRACTIONBITS 14
#define FRACTIONMASK ((1L<<FRACTIONBITS)-1)
#define MAX_PITCH 4
#define OUTPUTCHANNELS 2

#include "AL/al.h"

#ifdef __cplusplus
extern "C" {
#endif

ALUAPI ALint	ALUAPIENTRY aluF2L(ALfloat value);
ALUAPI ALshort	ALUAPIENTRY aluF2S(ALfloat value);
ALUAPI ALvoid	ALUAPIENTRY aluCrossproduct(ALfloat *inVector1,ALfloat *inVector2,ALfloat *outVector);
ALUAPI ALfloat	ALUAPIENTRY aluDotproduct(ALfloat *inVector1,ALfloat *inVector2);
ALUAPI ALvoid	ALUAPIENTRY aluNormalize(ALfloat *inVector);
ALUAPI ALvoid	ALUAPIENTRY aluMatrixVector(ALfloat *vector,ALfloat matrix[3][3]);
ALUAPI ALvoid	ALUAPIENTRY aluCalculateSourceParameters(ALuint source,ALuint frequency,ALuint channels,ALfloat *drysend,ALfloat *wetsend,ALfloat *pitch);
ALUAPI ALvoid	ALUAPIENTRY aluMixData(ALvoid *context,ALvoid *buffer,ALsizei size,ALenum format);

#ifdef __cplusplus
}
#endif

#endif

