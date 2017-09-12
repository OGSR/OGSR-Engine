#ifndef _AL_THUNK_H_
#define _AL_THUNK_H_

#include "AL/al.h"
#include "AL/alc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_M_AMD64) || defined(_M_IA64)

void alThunkInit(void);
void alThunkExit(void);
ALuint alThunkAddEntry(ALvoid * ptr);
void alThunkRemoveEntry(ALuint index);
ALvoid * alThunkLookupEntry(ALuint index);

#define ALTHUNK_INIT()          alThunkInit()
#define ALTHUNK_EXIT()          alThunkExit()
#define ALTHUNK_ADDENTRY(p)     alThunkAddEntry(p)
#define ALTHUNK_REMOVEENTRY(i)  alThunkRemoveEntry(i)
#define ALTHUNK_LOOKUPENTRY(i)  alThunkLookupEntry(i)

#else

#define ALTHUNK_INIT()
#define ALTHUNK_EXIT()
#define ALTHUNK_ADDENTRY(p)     p
#define ALTHUNK_REMOVEENTRY(i)
#define ALTHUNK_LOOKUPENTRY(i)  ((ALvoid*)(i))

#endif // defined(_M_AMD64) || defined(_M_IA64)

#ifdef __cplusplus
}
#endif

#endif //_AL_THUNK_H_

