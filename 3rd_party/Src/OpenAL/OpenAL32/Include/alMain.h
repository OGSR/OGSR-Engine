#ifndef _AL_MAIN_H_
#define _AL_MAIN_H_

#define AL_MAX_CHANNELS		4
#define AL_MAX_SOURCES		32

#include "alu.h"
#include "windows.h"
#include "mmsystem.h"
#include "dsound.h"
#include "alBuffer.h"
#include "alError.h"
#include "alExtension.h"
#include "alListener.h"
#include "alSource.h"
#include "alState.h"
#include "alThunk.h"

#define NUMWAVEBUFFERS	4

#define SWMIXER_OUTPUT_RATE		44100

#define AL_FORMAT_MONO_IMA4                      0x1300
#define AL_FORMAT_STEREO_IMA4                    0x1301

#define SPEEDOFSOUNDMETRESPERSEC	(343.3f)

typedef struct ALCdevice_struct
{
 	ALboolean	bInUse;
	ALboolean	bIsCaptureDevice;

	ALuint		Frequency;
	ALuint		Channels;
	ALenum		Format;

	ALCchar		szDeviceName[256];

	// Maximum number of sources that can be created
	ALuint		MaxNoOfSources;

	// MMSYSTEM Capture Device
	ALboolean		bWaveInShutdown;
	HANDLE			hWaveInHdrEvent;
	HANDLE			hWaveInThreadEvent;
	HANDLE			hWaveInThread;
	ALuint			ulWaveInThreadID;
	ALint			lWaveInBuffersCommitted;
	HWAVEIN			hWaveInHandle;
	WAVEHDR			WaveInBuffer[4];
	WAVEFORMATEX	wfexCaptureFormat;
	ALCchar			*pCapturedSampleData;
	ALuint			ulCapturedDataSize;
	ALuint			ulReadCapturedDataPos;
	ALuint			ulWriteCapturedDataPos;

	// MMSYSTEM Device
	ALboolean	bWaveShutdown;
	HANDLE		hWaveHdrEvent;
	HANDLE		hWaveThreadEvent;
	HANDLE		hWaveThread;
	ALuint		ulWaveThreadID;
	ALint		lWaveBuffersCommitted;
	HWAVEOUT	hWaveHandle;
	WAVEHDR		buffer[NUMWAVEBUFFERS];

	// DirectSound and DirectSound3D Devices
	LPDIRECTSOUND			lpDS;

	// DirectSound Device
	LPDIRECTSOUNDBUFFER		DSpbuffer;
	LPDIRECTSOUNDBUFFER		DSsbuffer;
	MMRESULT				ulDSTimerID;

	// DirectSound3D Device
	LPDIRECTSOUND3DLISTENER lpDS3DListener;
	ALuint					ulDS3DTimerInterval;
	ALuint					ulDS3DTimerID;
#ifdef __MINGW32__
} WINdevice;
#undef ALCdevice
#define ALCdevice WINdevice
#else
} ALCdevice;
#endif

typedef struct ALCcontext_struct
{
	ALlistener	Listener;

	ALsource *	Source;
	ALuint		SourceCount;

    ALuint      alPrivateSource;    // Guarantees that there is always one Source in existence

	ALenum		LastError;
	ALboolean	InUse;

	ALuint		Frequency;
	ALuint		Channels;
	ALenum		Format;

	ALenum		DistanceModel;

	ALfloat		DopplerFactor;
	ALfloat		DopplerVelocity;
	ALfloat		flSpeedOfSound;

	ALint		lNumMonoSources;
	ALint		lNumStereoSources;

	ALCdevice * Device;

	ALboolean	bUseManualAttenuation;

	struct ALCcontext_struct *previous;
	struct ALCcontext_struct *next;
	
#ifdef __MINGW32__
}  WINcontext;
#undef ALCcontext
#define ALCcontext WINcontext
#else
}  ALCcontext;
#endif

ALCvoid UpdateContext(ALCcontext *context,ALuint type,ALuint name);
ALint LinearGainToMB(float flGain);

#define LEVELFLAG_RECALCULATE_ATTENUATION	0x01
#define LEVELFLAG_FORCE_EAX_CALL			0x02
#define LEVELFLAG_USE_DEFERRED				0x80000000

void InitializeManualAttenuation(ALCcontext *pContext);
void SetSourceLevel(ALsource *pSource, ALuint ulFlags);

#ifdef __cplusplus
extern "C"
{
#endif

ALCvoid SuspendContext(ALCcontext *context);
ALCvoid ProcessContext(ALCcontext *context);

#ifdef __cplusplus
}
#endif

#endif

