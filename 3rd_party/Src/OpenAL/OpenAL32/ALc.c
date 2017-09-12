/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2000 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */ 

#ifndef __MINGW32__
#define _CRT_SECURE_NO_DEPRECATE // get rid of sprintf security warnings on VS2005
#pragma comment(lib, "winmm.lib")
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "include/alMain.h"
#include "AL/al.h"
#include "AL/alc.h"

///////////////////////////////////////////////////////
// DEBUG INFORMATION

#ifdef _DEBUG
 char szDebug[256];
#endif

///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// DEFINITIONS

#define SPEEDOFSOUNDMETRESPERSEC	(343.3f)
#define MAX_NUM_SOURCES				64
#define OUTPUT_BUFFER_SIZE			(32768*SWMIXER_OUTPUT_RATE/22050)

#define	TIMERINTERVAL	50

#ifndef DWORD_PTR_DEFINED
#define DWORD_PTR DWORD
#endif

#define sqr(a) ((a) * (a))
#define CLAMP(x, min, max) if (x < min)	x = min; else if (x > max) x = max;

#define LEVEL_TOLERANCE 100

///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// FUNCTION PROTOTYPES

// EAX Related
ALvoid ResetLocalFXParams(ALCcontext * ALCContext, ALuint ulSlotID);
void EAXFix(ALCcontext *context);

// Multimedia Timer Callback function prototype
static void CALLBACK WaveOutProc(HWAVEOUT hDevice,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2);
static void CALLBACK DirectSoundProc(UINT uID,UINT uReserved,DWORD_PTR dwUser,DWORD_PTR dwReserved1,DWORD_PTR dwReserved2);
void CALLBACK DirectSound3DProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
DWORD WINAPI ThreadProc(LPVOID lpParameter);

// Capture functions
static void CALLBACK WaveInProc(HWAVEIN hDevice,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2);
DWORD WINAPI CaptureThreadProc(LPVOID lpParameter);

ALvoid FillBuffer(ALsource *ALSource, ALubyte *pDestData, ALuint ulDestSize);

// Update Context functions
void UpdateSource(ALCcontext *ALContext, ALsource *ALSource);
void UpdateListener(ALCcontext *ALContext);

// DirectSound3D Device functions
ALuint GetMaxNumStereoBuffers(LPDIRECTSOUND lpDS);
ALuint GetMaxNum3DMonoBuffers(LPDIRECTSOUND lpDS);

// RollOff functions
void InitializeManualAttenuation(ALCcontext *pContext);
void SetDistanceModel(ALCcontext *pContext);
void SetNonEAXSourceLevels(ALCcontext *pContext, ALsource *pSource, ALuint ulFlags);

///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// STRING and EXTENSIONS

typedef struct ALCextension_struct
{
	ALCchar		*extName;
	ALvoid		*address;
} ALCextension;

typedef struct ALCfunction_struct
{
	ALCchar		*funcName;
	ALvoid		*address;
} ALCfunction;

static ALCextension alcExtensions[] = {
	{ "ALC_ENUMERATION_EXT",		(ALvoid *) NULL				},
	{ "ALC_EXT_CAPTURE",			(ALvoid *) NULL				},
	{ NULL,							(ALvoid *) NULL				} };

static ALCfunction  alcFunctions[] = {
	{ "alcCreateContext",           (ALvoid *) alcCreateContext         },
	{ "alcMakeContextCurrent",      (ALvoid *) alcMakeContextCurrent    },
	{ "alcProcessContext",          (ALvoid *) alcProcessContext        },
	{ "alcSuspendContext",          (ALvoid *) alcSuspendContext        },
	{ "alcDestroyContext",          (ALvoid *) alcDestroyContext        },
	{ "alcGetCurrentContext",       (ALvoid *) alcGetCurrentContext     },
	{ "alcGetContextsDevice",       (ALvoid *) alcGetContextsDevice     },
	{ "alcOpenDevice",              (ALvoid *) alcOpenDevice            },
	{ "alcCloseDevice",             (ALvoid *) alcCloseDevice           },
	{ "alcGetError",                (ALvoid *) alcGetError              },
	{ "alcIsExtensionPresent",      (ALvoid *) alcIsExtensionPresent    },
	{ "alcGetProcAddress",          (ALvoid *) alcGetProcAddress        },
	{ "alcGetEnumValue",            (ALvoid *) alcGetEnumValue          },
	{ "alcGetString",               (ALvoid *) alcGetString             },
	{ "alcGetIntegerv",             (ALvoid *) alcGetIntegerv           },
	{ "alcCaptureOpenDevice",		(ALvoid *) alcCaptureOpenDevice		},
	{ "alcCaptureCloseDevice",		(ALvoid *) alcCaptureCloseDevice	},
	{ "alcCaptureStart",			(ALvoid *) alcCaptureStart			},
	{ "alcCaptureStop",				(ALvoid *) alcCaptureStop			},
	{ "alcCaptureSamples",			(ALvoid *) alcCaptureSamples		},
	{ NULL,							(ALvoid *) NULL				} };

// Error strings
static ALCchar alcNoError[] = "No Error";
static ALCchar alcErrInvalidDevice[] = "Invalid Device";
static ALCchar alcErrInvalidContext[] = "Invalid Context";
static ALCchar alcErrInvalidEnum[] = "Invalid Enum";
static ALCchar alcErrInvalidValue[] = "Invalid Value";

// Context strings
static ALCchar alcDefaultDeviceSpecifier[] = "Generic Hardware";
static ALCchar alcDeviceList[] = "Generic Hardware\0Generic Software\0";
static ALCchar alcExtensionList[] = "ALC_ENUMERATION_EXT ALC_EXT_CAPTURE";

static ALCchar alcCaptureDefaultDeviceSpecifier[2048] = { 0 }; //  = "MMSYSTEM";
static ALCchar alcCaptureDeviceList[2048] = { 0 }; //  = "MMSYSTEM\0";

static ALCint alcMajorVersion = 1;
static ALCint alcMinorVersion = 1;

///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// Global Variables

// Critical Section data
extern CRITICAL_SECTION g_mutex;

// Context List
ALCcontext	*g_pContextList = NULL;
ALCuint		g_ulContextCount = 0;

// Context Error
static ALCenum g_eLastContextError = ALC_NO_ERROR;

// Capture Device List
ALCdevice *g_pCaptureDevice = NULL;

// EAX Related (need to be reset in alcOpenDevice)

typedef enum { NONE, EAX1, EAX2, EAX25, EAX3, EAX4 } EAXVERSION;

extern ALboolean bEAX2Initialized;
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// ALC Related helper functions

/*
	IsContext

	Check pContext is a valid Context pointer
*/
ALCboolean IsContext(ALCcontext *pContext)
{
	ALCcontext *pTempContext;
	ALCboolean bReturn = AL_FALSE;

	pTempContext = g_pContextList;
	while (pTempContext)
	{
		if (pTempContext == pContext)
		{
			bReturn = AL_TRUE;
			break;
		}

		pTempContext = pTempContext->next;
	}

	return bReturn;
}


/*
	SetALCError

	Store latest ALC Error
*/
ALCvoid SetALCError(ALenum errorCode)
{
	g_eLastContextError = errorCode;
}


/*
	SuspendContext

	Thread-safe entry
*/
ALCvoid SuspendContext(ALCcontext *pContext)
{
	EnterCriticalSection(&g_mutex);
}


/*
	ProcessContext

	Thread-safe exit
*/
ALCvoid ProcessContext(ALCcontext *pContext)
{
	LeaveCriticalSection(&g_mutex);
}


/*
	InitContext

	Initialize Context variables
*/
ALvoid InitContext(ALCcontext *pContext)
{
	if (pContext)
	{
		//Lock context
		SuspendContext(pContext);
		//Initialise listener
		pContext->Listener.Gain=1.0f;
		pContext->Listener.Position[0]=0.0f;
		pContext->Listener.Position[1]=0.0f;
		pContext->Listener.Position[2]=0.0f;
		pContext->Listener.Velocity[0]=0.0f;
		pContext->Listener.Velocity[1]=0.0f;
		pContext->Listener.Velocity[2]=0.0f;
		pContext->Listener.Forward[0]=0.0f;
		pContext->Listener.Forward[1]=0.0f;
		pContext->Listener.Forward[2]=-1.0f;
		pContext->Listener.Up[0]=0.0f;
		pContext->Listener.Up[1]=1.0f;
		pContext->Listener.Up[2]=0.0f;

		pContext->Listener.EAX20LP.dwEnvironment = EAX_ENVIRONMENT_GENERIC;
		pContext->Listener.EAX20LP.flEnvironmentSize = EAXLISTENER_DEFAULTENVIRONMENTSIZE;
		pContext->Listener.EAX20LP.flEnvironmentDiffusion = EAXLISTENER_DEFAULTENVIRONMENTDIFFUSION;
		pContext->Listener.EAX20LP.lRoom = EAXLISTENER_MINROOM;
		pContext->Listener.EAX20LP.lRoomHF = EAXLISTENER_DEFAULTROOMHF;
		pContext->Listener.EAX20LP.flDecayTime = EAXLISTENER_DEFAULTDECAYTIME;
		pContext->Listener.EAX20LP.flDecayHFRatio = EAXLISTENER_DEFAULTDECAYHFRATIO;
		pContext->Listener.EAX20LP.lReflections = EAXLISTENER_DEFAULTREFLECTIONS;
		pContext->Listener.EAX20LP.flReflectionsDelay = EAXLISTENER_DEFAULTREFLECTIONSDELAY;
		pContext->Listener.EAX20LP.lReverb = EAXLISTENER_DEFAULTREVERB;
		pContext->Listener.EAX20LP.flReverbDelay = EAXLISTENER_DEFAULTREVERBDELAY;
		pContext->Listener.EAX20LP.flAirAbsorptionHF = EAXLISTENER_DEFAULTAIRABSORPTIONHF;
		pContext->Listener.EAX20LP.flRoomRolloffFactor = EAXLISTENER_DEFAULTROOMROLLOFFFACTOR;
		pContext->Listener.EAX20LP.dwFlags = EAXLISTENER_DEFAULTFLAGS;

		//Validate pContext
		pContext->LastError=AL_NO_ERROR;
		pContext->InUse=AL_FALSE;
		//Set output format
		pContext->Frequency=pContext->Device->Frequency;
		pContext->Channels=pContext->Device->Channels;
		pContext->Format=pContext->Device->Format;
		//Set globals
		pContext->DistanceModel=AL_INVERSE_DISTANCE_CLAMPED;
		pContext->DopplerFactor = 1.0f;
		pContext->DopplerVelocity = 1.0f;
		pContext->flSpeedOfSound = SPEEDOFSOUNDMETRESPERSEC;

		if (pContext->Device)
		{
			pContext->lNumStereoSources = 1;
			pContext->lNumMonoSources = pContext->Device->MaxNoOfSources - pContext->lNumStereoSources;
		}

		pContext->bUseManualAttenuation = AL_FALSE;
		
		// Initialize update to set all the Listener parameters
		pContext->Listener.update1 = LPOSITION | LVELOCITY | LORIENTATION | LDOPPLERFACTOR | LDOPPLERVELOCITY | LDISTANCEMODEL | LSPEEDOFSOUND;
		UpdateContext(pContext, ALLISTENER, 0);
			
		//Unlock pContext
		ProcessContext(pContext);
	}
}


/*
	ExitContext

	Clean up Context, destroy any remaining Sources
*/
ALCvoid ExitContext(ALCcontext *pContext)
{
	unsigned int i;
	ALsource *ALSource;
	ALsource *ALTempSource;

	if (IsContext(pContext))
	{
		//Lock context
		SuspendContext(pContext);

#ifdef _DEBUG
		if (pContext->SourceCount>0)
		{
			sprintf(szDebug,"OpenAL32 : alcDestroyContext() %d Source(s) NOT deleted\n", pContext->SourceCount);
			OutputDebugString(szDebug);
		}
#endif

		// Free all the Sources still remaining
		ALSource = pContext->Source;
		for (i = 0; i < pContext->SourceCount; i++)
		{
			if (ALSource->uservalue1)
			{
				IDirectSoundBuffer_Stop((LPDIRECTSOUNDBUFFER)ALSource->uservalue1);
				if (ALSource->uservalue3)
				{
					IKsPropertySet_Release((LPKSPROPERTYSET)ALSource->uservalue3);
					ALSource->uservalue3 = NULL;
				}
				if (ALSource->uservalue2)
				{
					IDirectSound3DBuffer_Release((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2);
					ALSource->uservalue2 = NULL;
				}
				IDirectSoundBuffer_Release((LPDIRECTSOUNDBUFFER)ALSource->uservalue1);
				ALSource->uservalue1=NULL;
			}

			ALTempSource = ALSource->next;
            ALTHUNK_REMOVEENTRY(ALSource->source);
            memset(ALSource,0,sizeof(ALsource));
			free(ALSource);
			ALSource = ALTempSource;
		}

		// If we created the Permanent Source ( in EAXFix() ), then manually delete it now
		if (pContext->alPrivateSource)
		{
            ALSource=((ALsource *)ALTHUNK_LOOKUPENTRY(pContext->alPrivateSource));

			// Delete the Source
			if (ALSource->uservalue1)
			{
				IDirectSoundBuffer_Stop((LPDIRECTSOUNDBUFFER)ALSource->uservalue1);
				if (ALSource->uservalue3)
				{
					IKsPropertySet_Release((LPKSPROPERTYSET)ALSource->uservalue3);
					ALSource->uservalue3 = NULL;
				}
				if (ALSource->uservalue2)
				{
					IDirectSound3DBuffer_Release((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2);
					ALSource->uservalue2 = NULL;
				}
				IDirectSoundBuffer_Release((LPDIRECTSOUNDBUFFER)ALSource->uservalue1);
				ALSource->uservalue1=NULL;
			}
            ALTHUNK_REMOVEENTRY(ALSource->source);
			memset(ALSource,0,sizeof(ALsource));
			free(ALSource);
		}

		//Invalidate context
		pContext->LastError=AL_NO_ERROR;
		pContext->InUse=AL_FALSE;

		//Unlock context
		ProcessContext(pContext);
	}
	else
		SetALCError(ALC_INVALID_CONTEXT);
}

///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// ALC Functions calls


// This should probably move to another c file but for now ...
ALCAPI ALCdevice* ALCAPIENTRY alcCaptureOpenDevice(const ALCchar *deviceName, ALCuint frequency, ALCenum format, ALCsizei SampleSize)
{
	ALCdevice *pDevice = NULL;
	WAVEINCAPS WaveInCaps;
	ALint i;
	ALint lNumDevs, lBufferSize;
	ALint lDeviceID = 0;

	if (!g_pCaptureDevice)
	{
		pDevice = malloc(sizeof(ALCdevice));
		if (pDevice)
		{
			if (SampleSize > 0)
			{
				//Initialise device structure
				memset(pDevice,0,sizeof(ALCdevice));
				//Validate device
				pDevice->bInUse=AL_TRUE;
				pDevice->bIsCaptureDevice=AL_TRUE;

				memset(&pDevice->wfexCaptureFormat,0,sizeof(WAVEFORMATEX));
				pDevice->wfexCaptureFormat.wFormatTag = WAVE_FORMAT_PCM;
				pDevice->wfexCaptureFormat.nChannels = (((format==AL_FORMAT_STEREO8)||(format==AL_FORMAT_STEREO16))?2:1);
				pDevice->wfexCaptureFormat.wBitsPerSample = (((format==AL_FORMAT_MONO16)||(format==AL_FORMAT_STEREO16))?16:8);
				pDevice->wfexCaptureFormat.nBlockAlign = pDevice->wfexCaptureFormat.nChannels * pDevice->wfexCaptureFormat.wBitsPerSample/8;
				pDevice->wfexCaptureFormat.nSamplesPerSec = frequency;
				pDevice->wfexCaptureFormat.nAvgBytesPerSec = pDevice->wfexCaptureFormat.nSamplesPerSec * pDevice->wfexCaptureFormat.nBlockAlign;
				pDevice->wfexCaptureFormat.cbSize=0;

				// Find the Device ID matching the deviceName if valid
				if (deviceName)
				{
					lNumDevs = waveInGetNumDevs();
					for (i = 0; i < lNumDevs; i++)
					{
						if (waveInGetDevCaps(i, &WaveInCaps, sizeof(WAVEINCAPS)) == MMSYSERR_NOERROR)
						{
							if (!strcmp(deviceName, WaveInCaps.szPname))
							{
								lDeviceID = i;
								break;
							}
						}
					}
				}

				if (waveInOpen(&pDevice->hWaveInHandle,lDeviceID,&pDevice->wfexCaptureFormat,(DWORD_PTR)&WaveInProc,(DWORD_PTR)pDevice,CALLBACK_FUNCTION)==MMSYSERR_NOERROR)
				{
					pDevice->hWaveInHdrEvent = CreateEvent(NULL, AL_TRUE, AL_FALSE, "WaveInAllHeadersReturned");
					if (pDevice->hWaveInHdrEvent != NULL)
					{
						pDevice->hWaveInThreadEvent = CreateEvent(NULL, AL_TRUE, AL_FALSE, "WaveInThreadDestroyed");
						if (pDevice->hWaveInThreadEvent != NULL)
						{
							pDevice->hWaveInThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CaptureThreadProc,(LPVOID)pDevice,0,&pDevice->ulWaveInThreadID);
							if (pDevice->hWaveInThread != NULL)
							{
								// Allocate circular memory buffer for the captured audio
								pDevice->ulCapturedDataSize = SampleSize * pDevice->wfexCaptureFormat.nBlockAlign;

								// Make sure circular buffer is at least 100ms in size (and an exact multiple of
								// the block alignment
								if (pDevice->ulCapturedDataSize < (pDevice->wfexCaptureFormat.nAvgBytesPerSec / 10))
								{
									pDevice->ulCapturedDataSize = pDevice->wfexCaptureFormat.nAvgBytesPerSec / 10;
									pDevice->ulCapturedDataSize -= (pDevice->ulCapturedDataSize % pDevice->wfexCaptureFormat.nBlockAlign);
								}

								pDevice->pCapturedSampleData = (ALCchar*)malloc(pDevice->ulCapturedDataSize);
								pDevice->lWaveInBuffersCommitted=0;

								// Create 4 Buffers of 50ms each
								lBufferSize = pDevice->wfexCaptureFormat.nAvgBytesPerSec / 20;
								lBufferSize -= (lBufferSize % pDevice->wfexCaptureFormat.nBlockAlign);

								for (i=0;i<4;i++)
								{
									memset(&pDevice->WaveInBuffer[i],0,sizeof(WAVEHDR));
									pDevice->WaveInBuffer[i].dwBufferLength=lBufferSize;
									pDevice->WaveInBuffer[i].lpData=malloc(pDevice->WaveInBuffer[i].dwBufferLength);
									if (pDevice->WaveInBuffer[i].lpData)
										memset(pDevice->WaveInBuffer[i].lpData, 0, pDevice->WaveInBuffer[i].dwBufferLength);
									pDevice->WaveInBuffer[i].dwFlags=0;
									pDevice->WaveInBuffer[i].dwLoops=0;
									waveInPrepareHeader(pDevice->hWaveInHandle, &pDevice->WaveInBuffer[i], sizeof(WAVEHDR));
									waveInAddBuffer(pDevice->hWaveInHandle, &pDevice->WaveInBuffer[i], sizeof(WAVEHDR));
									pDevice->lWaveInBuffersCommitted++;
								}

								pDevice->ulReadCapturedDataPos = 0;
								pDevice->ulWriteCapturedDataPos = 0;

								g_pCaptureDevice = pDevice;

								if (waveInGetDevCaps(lDeviceID, &WaveInCaps, sizeof(WAVEINCAPS)) == MMSYSERR_NOERROR)
									strcpy(g_pCaptureDevice->szDeviceName, WaveInCaps.szPname);
							}
						}
					}
				}

				if (!g_pCaptureDevice)
				{
					if (pDevice->hWaveInThreadEvent)
					{
						CloseHandle(pDevice->hWaveInThreadEvent);
						pDevice->hWaveInThreadEvent = NULL;
					}

					if (pDevice->hWaveInHdrEvent)
					{
						CloseHandle(pDevice->hWaveInHdrEvent);
						pDevice->hWaveInHdrEvent = NULL;
					}

					if (pDevice->hWaveInHandle)
					{
						waveInClose(pDevice->hWaveInHandle);
						pDevice->hWaveInHandle = NULL;
					}
				}
			}
			else
			{
				SetALCError(ALC_INVALID_VALUE);
			}
		}
		else
		{
			SetALCError(ALC_OUT_OF_MEMORY);
		}
	}

	return pDevice;
}

ALCAPI ALCboolean ALCAPIENTRY alcCaptureCloseDevice(ALCdevice *pDevice)
{
	ALint i;
	ALCboolean bReturn = ALC_FALSE;

	if ((pDevice)&&(pDevice->bIsCaptureDevice))
	{		
		// Call waveOutReset to shutdown wave device
		pDevice->bWaveInShutdown = AL_TRUE;
		waveInReset(pDevice->hWaveInHandle);

		// Wait for signal that all Wave Buffers have returned
		WaitForSingleObjectEx(pDevice->hWaveInHdrEvent, 5000, FALSE);

		// Wait for signal that Wave Thread has been destroyed
		WaitForSingleObjectEx(pDevice->hWaveInThreadEvent, 5000, FALSE);

		// Release the wave buffers
		for (i=0;i<4;i++)
		{
			waveInUnprepareHeader(pDevice->hWaveInHandle, &pDevice->WaveInBuffer[i], sizeof(WAVEHDR));
			free(pDevice->WaveInBuffer[i].lpData);
		}
	
		// Free Audio Buffer data
		free(pDevice->pCapturedSampleData);
		pDevice->pCapturedSampleData = NULL;

		// Close the Wave device
		waveInClose(pDevice->hWaveInHandle);
		pDevice->hWaveInHandle = 0;

		CloseHandle(pDevice->hWaveInThread);
		pDevice->hWaveInThread = 0;

		if (pDevice->hWaveInHdrEvent)
		{
			CloseHandle(pDevice->hWaveInHdrEvent);
			pDevice->hWaveInHdrEvent = 0;
		}

		if (pDevice->hWaveInThreadEvent)
		{
			CloseHandle(pDevice->hWaveInThreadEvent);
			pDevice->hWaveInThreadEvent = 0;
		}

		g_pCaptureDevice = NULL;

		bReturn = ALC_TRUE;
	}
	else
	{
		SetALCError(ALC_INVALID_DEVICE);
	}

	return bReturn;
}

ALCAPI void ALCAPIENTRY alcCaptureStart(ALCdevice *pDevice)
{
	if ((pDevice)&&(pDevice->bIsCaptureDevice))
		waveInStart(pDevice->hWaveInHandle);
	else
		SetALCError(ALC_INVALID_DEVICE);
}

ALCAPI void ALCAPIENTRY alcCaptureStop(ALCdevice *pDevice)
{
	if ((pDevice)&&(pDevice->bIsCaptureDevice))
		waveInStop(pDevice->hWaveInHandle);
	else
		SetALCError(ALC_INVALID_DEVICE);
}

ALCAPI void ALCAPIENTRY alcCaptureSamples(ALCdevice *pDevice, ALCvoid *pBuffer, ALCsizei lSamples)
{
	ALuint	ulCapturedSamples;
	ALuint	ulBytes, ulBytesToCopy;
	ALuint	ulReadOffset;
	ALuint	ulSamples = (unsigned long)lSamples;

	if ((pDevice) && (pDevice->bIsCaptureDevice))
	{
		SuspendContext(NULL);

		// Check that we have the requested numbers of Samples
		ulCapturedSamples = (pDevice->ulWriteCapturedDataPos - pDevice->ulReadCapturedDataPos) / pDevice->wfexCaptureFormat.nBlockAlign;

		if (ulSamples <= ulCapturedSamples)
		{
			ulBytes = ulSamples * pDevice->wfexCaptureFormat.nBlockAlign;

			// Get Read Offset
			ulReadOffset = (pDevice->ulReadCapturedDataPos % pDevice->ulCapturedDataSize);

			// Check for wrap-around condition
			if ((ulReadOffset + ulBytes) > pDevice->ulCapturedDataSize)
			{
				// Copy data from last Read position to end of data
				ulBytesToCopy = pDevice->ulCapturedDataSize - ulReadOffset;
				memcpy(pBuffer, pDevice->pCapturedSampleData + ulReadOffset, ulBytesToCopy);

				// Copy rest of the data from the start of the captured data
				memcpy(((char *)pBuffer) + ulBytesToCopy, pDevice->pCapturedSampleData, ulBytes - ulBytesToCopy);
			}
			else
			{
				// Copy data from the read position in the captured data
				memcpy(pBuffer, pDevice->pCapturedSampleData + ulReadOffset, ulBytes);
			}

			// Update Read Position
			pDevice->ulReadCapturedDataPos += ulBytes;
		}
		else
		{
			SetALCError(ALC_INVALID_VALUE);
		}

		ProcessContext(NULL);
	}
	else
	{
		SetALCError(ALC_INVALID_DEVICE);
	}
}

/*
	WaveInProc

	Posts a message to 'CaptureThreadProc' everytime a WaveIn Buffer is completed and
	returns to the application (with more data)
*/
static void CALLBACK WaveInProc(HWAVEIN hDevice,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2)
{
	ALCdevice *pDevice;

	pDevice = (ALCdevice *)dwInstance;
	if ((uMsg==WIM_DATA) && (pDevice))
	{
		// Decrement number of buffers in use
		pDevice->lWaveInBuffersCommitted--;

		if (pDevice->bWaveInShutdown == AL_FALSE)
		{
			// Notify Wave Processor Thread that a Wave Header has returned
			PostThreadMessage(pDevice->ulWaveInThreadID,uMsg,0,dwParam1);
		}
		else
		{
			if (pDevice->lWaveInBuffersCommitted == 0)
			{
				// Signal Wave Buffers Returned event
				if (pDevice->hWaveInHdrEvent)
					SetEvent(pDevice->hWaveInHdrEvent);
				
				// Post 'Quit' Message to WaveIn Processor Thread
				PostThreadMessage(pDevice->ulWaveInThreadID,WM_QUIT,0,0);
			}
		}
	}
}

/*
	CaptureThreadProc

	Used by "MMSYSTEM" Device.  Called when a WaveIn buffer had been filled with new
	audio data.
*/
DWORD WINAPI CaptureThreadProc(LPVOID lpParameter)
{
	ALCdevice *pDevice;
	LPWAVEHDR pWaveHdr;
	MSG msg;
	ALuint ulOffset, ulMaxSize, ulSection;

	pDevice = (ALCdevice*)lpParameter;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if ((msg.message==WIM_DATA)&&(!pDevice->bWaveInShutdown))
		{
			SuspendContext(NULL);

			pWaveHdr = ((LPWAVEHDR)msg.lParam);

			// Calculate offset in local buffer to write data to
			ulOffset = pDevice->ulWriteCapturedDataPos % pDevice->ulCapturedDataSize;

			if ((ulOffset + pWaveHdr->dwBytesRecorded) > pDevice->ulCapturedDataSize)
			{
				ulSection = pDevice->ulCapturedDataSize - ulOffset;
				memcpy(pDevice->pCapturedSampleData + ulOffset, pWaveHdr->lpData, ulSection);
				memcpy(pDevice->pCapturedSampleData, pWaveHdr->lpData + ulSection, pWaveHdr->dwBytesRecorded - ulSection);
			}
			else
			{
				memcpy(pDevice->pCapturedSampleData + ulOffset, pWaveHdr->lpData, pWaveHdr->dwBytesRecorded);
			}

			pDevice->ulWriteCapturedDataPos += pWaveHdr->dwBytesRecorded;

			if (pDevice->ulWriteCapturedDataPos > (pDevice->ulReadCapturedDataPos + pDevice->ulCapturedDataSize))
			{
				// Application has not read enough audio data from the capture buffer so data has been
				// overwritten.  Reset ReadPosition.
				pDevice->ulReadCapturedDataPos = pDevice->ulWriteCapturedDataPos - pDevice->ulCapturedDataSize;
			}

			// To prevent an over-flow prevent the offset values from getting too large
			ulMaxSize = pDevice->ulCapturedDataSize << 4;
			if ((pDevice->ulReadCapturedDataPos > ulMaxSize) && (pDevice->ulWriteCapturedDataPos > ulMaxSize))
			{
				pDevice->ulReadCapturedDataPos -= ulMaxSize;
				pDevice->ulWriteCapturedDataPos -= ulMaxSize;
			}

			// Send buffer back to capture more data
			waveInAddBuffer(pDevice->hWaveInHandle,pWaveHdr,sizeof(WAVEHDR));
			pDevice->lWaveInBuffersCommitted++;

			ProcessContext(NULL);
		}
	}

	// Signal Wave Thread completed event
	if (pDevice->hWaveInThreadEvent)
		SetEvent(pDevice->hWaveInThreadEvent);

	ExitThread(0);

	return 0;
}

/*
	alcGetError

	Return last ALC generated error code
*/
ALCAPI ALCenum ALCAPIENTRY alcGetError(ALCdevice *device)
{
	ALCenum errorCode;

	errorCode = g_eLastContextError;
	g_eLastContextError = ALC_NO_ERROR;
	return errorCode;
}


/*
	alcSuspendContext

	Not functional
*/
ALCAPI ALCvoid ALCAPIENTRY alcSuspendContext(ALCcontext *pContext)
{
	// Not a lot happens here !
}


/*
	alcProcessContext

	Not functional
*/
ALCAPI ALCvoid ALCAPIENTRY alcProcessContext(ALCcontext *pContext)
{
	// Not a lot happens here !
}


/*
	alcGetString

	Returns information about the Device, and error strings
*/
ALCAPI const ALCchar* ALCAPIENTRY alcGetString(ALCdevice *pDevice,ALCenum param)
{
	WAVEINCAPS WaveInCaps;
	const ALCchar *value = NULL;
	ALint lNumDevs, lLoop;
	ALCchar *szDeviceList;

	switch (param)
	{
	case ALC_NO_ERROR:
		value=alcNoError;
		break;

	case ALC_INVALID_ENUM:
		value=alcErrInvalidEnum;
		break;

	case ALC_INVALID_VALUE:
		value=alcErrInvalidValue;
		break;

	case ALC_INVALID_DEVICE:
		value=alcErrInvalidDevice;
		break;

	case ALC_INVALID_CONTEXT:
		value=alcErrInvalidContext;
		break;

	case ALC_DEFAULT_DEVICE_SPECIFIER:
		value = alcDefaultDeviceSpecifier;
		break;

	case ALC_DEVICE_SPECIFIER:
		if (pDevice)
			value = pDevice->szDeviceName;
		else
			value = alcDeviceList;
		break;

	case ALC_CAPTURE_DEVICE_SPECIFIER:
		if (pDevice)
		{
			value = pDevice->szDeviceName;
		}
		else
		{
			szDeviceList = alcCaptureDeviceList;

			lNumDevs = waveInGetNumDevs();
			for (lLoop = 0; lLoop < lNumDevs; lLoop++)
			{
				if (waveInGetDevCaps(lLoop, &WaveInCaps, sizeof(WAVEINCAPS)) == MMSYSERR_NOERROR)
				{
					sprintf(szDeviceList, "%s", WaveInCaps.szPname);
					szDeviceList += strlen(szDeviceList) + 1;
				}
			}

			value = alcCaptureDeviceList;
		}
		break;

	case ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER:
		if (waveInGetNumDevs())
		{
			if (waveInGetDevCaps(0, &WaveInCaps, sizeof(WAVEINCAPS)) == MMSYSERR_NOERROR)
				sprintf(alcCaptureDefaultDeviceSpecifier, "%s", WaveInCaps.szPname);
		}
		value = alcCaptureDefaultDeviceSpecifier;
		break;

	case ALC_EXTENSIONS:
		value = alcExtensionList;
		break;

	default:
		SetALCError(ALC_INVALID_ENUM);
		break;
	}

	return value;
}


/*
	alcGetIntegerv

	Returns information about the Device and the version of Open AL
*/
ALCAPI ALCvoid ALCAPIENTRY alcGetIntegerv(ALCdevice *device,ALCenum param,ALsizei size,ALCint *data)
{
	ALCcontext *Context;
	ALint lCapturedBytes;

	if ((device)&&(device->bIsCaptureDevice))
	{
		SuspendContext(NULL);

		// Capture device
		switch (param)
		{
		case ALC_CAPTURE_SAMPLES:
			if ((size) && (data))
			{
				lCapturedBytes = (device->ulWriteCapturedDataPos - device->ulReadCapturedDataPos);
				*data = lCapturedBytes / device->wfexCaptureFormat.nBlockAlign;
			}
			else
			{
				SetALCError(ALC_INVALID_VALUE);
			}
			break;

		default:
			SetALCError(ALC_INVALID_ENUM);
			break;
		}

		ProcessContext(NULL);
	}
	else
	{
		Context=alcGetCurrentContext();
		if (Context)
		{
			SuspendContext(Context);

			if (data)
			{
				// Playback Device
				switch (param)
				{
				case ALC_MAJOR_VERSION:
					if (size)
						*data = alcMajorVersion;
					else
						SetALCError(ALC_INVALID_VALUE);
					break;

				case ALC_MINOR_VERSION:
					if (size)
						*data = alcMinorVersion;
					else
						SetALCError(ALC_INVALID_VALUE);
					break;

				case ALC_ATTRIBUTES_SIZE:
					if (device)
					{
						if (size)
							*data = 11;
						else
							SetALCError(ALC_INVALID_VALUE);
					}
					else
					{
						SetALCError(ALC_INVALID_DEVICE);
					}
					break;

				case ALC_ALL_ATTRIBUTES:
					if (device)
					{
						if (size >= 11)
						{
							data[0] = ALC_FREQUENCY;
							if (device->lpDS3DListener)
								data[1] = 44100;
							else
								data[1] = SWMIXER_OUTPUT_RATE;

							data[2] = ALC_REFRESH;
							if (device->lpDS3DListener)
								data[3] = 20;
							else
								data[3] = 40;

							data[4] = ALC_SYNC;
							data[5] = AL_FALSE;

							data[6] = ALC_MONO_SOURCES;
							data[7] = Context->lNumMonoSources;

							data[8] = ALC_STEREO_SOURCES;
							data[9] = Context->lNumStereoSources;

							data[10] = 0;
						}
						else
						{
							SetALCError(ALC_INVALID_VALUE);
						}
					}
					else
					{
						SetALCError(ALC_INVALID_DEVICE);
					}
					break;

				default:
					SetALCError(ALC_INVALID_ENUM);
					break;
				}
			}
			else
			{
				SetALCError(ALC_INVALID_VALUE);
			}

			ProcessContext(Context);
		}
		else
		{
			// Invalid Context
			SetALCError(ALC_INVALID_CONTEXT);
		}
	}

	return;
}


/*
	alcIsExtensionPresent

	Determines if there is support for a particular extension
*/
ALCAPI ALCboolean ALCAPIENTRY alcIsExtensionPresent(ALCdevice *device, const ALCchar *extName)
{
	ALsizei i = 0;
	ALCboolean bResult = ALC_FALSE;

	if (extName)
	{
		while ((alcExtensions[i].extName)&&(_stricmp(alcExtensions[i].extName,extName)))
			i++;

		if (alcExtensions[i].extName)
			bResult = ALC_TRUE;
	}
	else
	{
		SetALCError(ALC_INVALID_VALUE);
	}

	return bResult;
}


/*
	alcGetProcAddress

	Retrieves the function address for a particular extension function
*/
ALCAPI ALCvoid *  ALCAPIENTRY alcGetProcAddress(ALCdevice *device, const ALCchar *funcName)
{
	ALsizei i = 0;
	ALCvoid *pFunction = NULL;

	if (funcName)
	{
		while ((alcFunctions[i].funcName)&&(strcmp(alcFunctions[i].funcName,funcName)))
			i++;
		pFunction = alcFunctions[i].address;
	}
	else
	{
		SetALCError(ALC_INVALID_VALUE);
	}

	return pFunction;
}


/*
	alcGetEnumValue

	Get the value for a particular ALC Enumerated Value
	Calls alGetEnumValue in alExtension.c to process request
*/
ALCAPI ALCenum ALCAPIENTRY alcGetEnumValue(ALCdevice *device, const ALCchar *enumName)
{
	ALCenum eResult = AL_NONE;

	if (enumName)
		eResult = alGetEnumValue(enumName);
	else
		SetALCError(ALC_INVALID_VALUE);

	return eResult;
}


/*
	alcCreateContext

	Create and attach a Context to a particular Device.
*/
ALCAPI ALCcontext* ALCAPIENTRY alcCreateContext(ALCdevice *device, const ALCint *attrList)
{
	ALCcontext *ALContext = NULL;
	ALboolean	bDeviceHasContext = AL_FALSE;
	ALuint		ulAttributeIndex, ulRequestedStereoSources;

	if ((device)&&(!device->bIsCaptureDevice))
	{
		// Reset Context Last Error code
		g_eLastContextError = ALC_NO_ERROR;

		// Check if this Device already has been assigned a Context (current implementation
		// only allows one Context per Device)
		ALContext = g_pContextList;
		while (ALContext)
		{
			if (ALContext->Device == device)
			{
				bDeviceHasContext = AL_TRUE;
				break;
			}
			ALContext = ALContext->next;
		}

		if (!bDeviceHasContext)
		{
			if (!g_pContextList)
			{
				g_pContextList = malloc(sizeof(ALCcontext));
				
				if (g_pContextList)
				{
					memset(g_pContextList, 0, sizeof(ALCcontext));
					g_pContextList->Device = device;
					InitContext(g_pContextList);
					g_ulContextCount++;
				}

				ALContext = g_pContextList;
			}
			else
			{
				ALContext = g_pContextList;
				while (ALContext->next)
					ALContext=ALContext->next;
				if (ALContext)
				{
					ALContext->next=malloc(sizeof(ALCcontext));
					if (ALContext->next)
					{
						memset(ALContext->next,0,sizeof(ALCcontext));
						ALContext->next->previous=ALContext;
						ALContext->next->Device=device;
						InitContext(ALContext->next);
						g_ulContextCount++;
					}
					ALContext=ALContext->next;
				}
			}

			// Check for Voice Count attributes
			if (attrList)
			{
				ulAttributeIndex = 0;
				while ((ulAttributeIndex < 10) && (attrList[ulAttributeIndex]))
				{
					if (attrList[ulAttributeIndex] == ALC_STEREO_SOURCES)
					{
						ulRequestedStereoSources = attrList[ulAttributeIndex + 1];

						if (ALContext->Device->lpDS3DListener)
						{
							// As we have already reserved a voice, add it back to the max Source count
							ALContext->Device->MaxNoOfSources++;

							if ((ulRequestedStereoSources * 2) > ALContext->Device->MaxNoOfSources)
								ulRequestedStereoSources = ALContext->Device->MaxNoOfSources / 2;

							ALContext->Device->MaxNoOfSources -= ulRequestedStereoSources;
							ALContext->lNumStereoSources = ulRequestedStereoSources;
							ALContext->lNumMonoSources = ALContext->Device->MaxNoOfSources - ALContext->lNumStereoSources;
						}
						else
						{
							if (ulRequestedStereoSources > ALContext->Device->MaxNoOfSources)
								ulRequestedStereoSources = ALContext->Device->MaxNoOfSources;

							ALContext->lNumStereoSources = ulRequestedStereoSources;
							ALContext->lNumMonoSources = ALContext->Device->MaxNoOfSources - ALContext->lNumStereoSources;
						}
						break;
					}

					ulAttributeIndex += 2;
				}
			}
		}
		else
		{
			SetALCError(ALC_INVALID_VALUE);
			ALContext = NULL;
		}
	}
	else
	{
		SetALCError(ALC_INVALID_DEVICE);
	}

	return ALContext;
}


/*
	alcDestroyContext

	Remove a Context
*/
ALCAPI ALCvoid ALCAPIENTRY alcDestroyContext(ALCcontext *context)
{
	ALCcontext *ALContext;

	if (IsContext(context))
	{
		// Lock context
		SuspendContext(context);
		ALContext=((ALCcontext *)context);
		ExitContext(ALContext);
		if (ALContext->previous)
			ALContext->previous->next=ALContext->next;
		else
			g_pContextList = ALContext->next;
		if (ALContext->next)
			ALContext->next->previous=ALContext->previous;
		memset(ALContext,0,sizeof(ALCcontext));
		g_ulContextCount--;
		// Unlock context
		ProcessContext(context);
		// Free memory (MUST do this after ProcessContext)
		free(ALContext);
	}
	else
	{
		SetALCError(ALC_INVALID_CONTEXT);
	}
}


/*
	alcGetCurrentContext

	Returns the currently active Context
*/
ALCAPI ALCcontext * ALCAPIENTRY alcGetCurrentContext(ALCvoid)
{
	ALCcontext *pContext = NULL;

	pContext = g_pContextList;
	while ((pContext) && (!pContext->InUse))
		pContext = pContext->next;

	return pContext;
}


/*
	alcGetContextsDevice

	Returns the Device that a particular Context is attached to
*/
ALCAPI ALCdevice* ALCAPIENTRY alcGetContextsDevice(ALCcontext *pContext)
{
	ALCdevice *pDevice = NULL;
	ALCcontext *pTempContext = NULL;

	pTempContext = pContext;
	if (IsContext(pTempContext))
	{
		SuspendContext(pTempContext);
		pDevice = pTempContext->Device;
		ProcessContext(pTempContext);
	}
	else
	{
		SetALCError(ALC_INVALID_CONTEXT);
	}

	return pDevice;
}


/*
	alcMakeContextCurrent

	Makes the given Context the active Context
*/
ALCAPI ALCboolean ALCAPIENTRY alcMakeContextCurrent(ALCcontext *context)
{
	ALCcontext *ALContext;
	ALboolean bReturn = AL_TRUE;
	
	// context must be a valid Context or NULL
	if ((IsContext(context)) || (context == NULL))
	{
		if (ALContext=alcGetCurrentContext())
		{
			SuspendContext(ALContext);
			ALContext->InUse=AL_FALSE;
			ProcessContext(ALContext);
		}

		if ((ALContext=context) && (ALContext->Device))
		{
			SuspendContext(ALContext);
			ALContext->InUse=AL_TRUE;

			// If we are using the DS3D Device then we may need to work-around a problem with the
			// FCS Audigy WDM drivers (see EAXFix() for more details)
			if (ALContext->Device->lpDS3DListener)
			{
				EAXFix(context);
			}

			ProcessContext(ALContext);
		}
	}
	else
	{
		SetALCError(ALC_INVALID_CONTEXT);
		bReturn = AL_FALSE;
	}

	return bReturn;
}


/*
	alcOpenDevice

	Open the Device specified.  Current options are "Generic Hardware", "Generic Software", and for legacy
	"DirectSound3D", "DirectSound" and "MMSYSTEM"
*/
ALCAPI ALCdevice* ALCAPIENTRY alcOpenDevice(const ALCchar *deviceName)
{
	DSBUFFERDESC DSBDescription;
	WAVEFORMATEX OutputType;
	TIMECAPS timeCaps;
	ALCdevice *device=NULL;
	ALint vmode=1;
	ALint i;
	DSCAPS dsCaps;
	ALvoid *lpPart1, *lpPart2;
	ALuint dwSize1, dwSize2;
	ALboolean bUseDS;
	ALboolean bUseDS3D;
	ALboolean bDeviceFound = AL_FALSE;
	ALint numSources;

	bUseDS = AL_FALSE;
	bUseDS3D = AL_FALSE;

	if (deviceName)
	{
		if ((!strcmp(deviceName,"DirectSound3D")) || (!strcmp(deviceName,"Generic Hardware")))
			bUseDS3D = AL_TRUE;
		else if ((!strcmp(deviceName, "DirectSound")) || (!strcmp(deviceName, "Generic Software")))
			bUseDS = AL_TRUE;
	}
	else
	{
		// If no device name is specified, we will attempt to use DS3D
		bUseDS3D = AL_TRUE;
	}

	device=malloc(sizeof(ALCdevice));
	if (device)
	{
		//Initialise device structure
		memset(device,0,sizeof(ALCdevice));
		//Validate device
		device->bInUse=AL_TRUE;
		device->bIsCaptureDevice=AL_FALSE;
		//Set output format
		device->Frequency=SWMIXER_OUTPUT_RATE;
		device->Channels=2;
		device->Format=AL_FORMAT_STEREO16;
		//Wave-Out properties
		device->bWaveShutdown = AL_FALSE;
		device->hWaveHdrEvent = NULL;
		device->hWaveThreadEvent = NULL;
		device->hWaveThread = NULL;
		device->ulWaveThreadID = 0;
		device->lWaveBuffersCommitted = 0;
		//Reset EAX Variables
		bEAX2Initialized = AL_FALSE;
		//Platform specific
		memset(&OutputType,0,sizeof(WAVEFORMATEX));
		OutputType.wFormatTag=WAVE_FORMAT_PCM;
        OutputType.nChannels=(WORD)device->Channels;
		OutputType.wBitsPerSample=(((device->Format==AL_FORMAT_MONO16)||(device->Format==AL_FORMAT_STEREO16))?16:8);
		OutputType.nBlockAlign=OutputType.nChannels*OutputType.wBitsPerSample/8;
		OutputType.nSamplesPerSec=device->Frequency;
		OutputType.nAvgBytesPerSec=OutputType.nSamplesPerSec*OutputType.nBlockAlign;
		OutputType.cbSize=0;
		//Initialise requested device

		if (bUseDS3D)
		{
			//Init COM
			CoInitialize(NULL);
			//DirectSound Init code
			if (CoCreateInstance(&CLSID_DirectSound,NULL,CLSCTX_INPROC_SERVER,&IID_IDirectSound,&(device->lpDS))==S_OK)
			{
				if (IDirectSound_Initialize(device->lpDS,NULL)==DS_OK)
				{
					if (IDirectSound_SetCooperativeLevel(device->lpDS,GetForegroundWindow(),DSSCL_PRIORITY)==DS_OK)
					{
						memset(&DSBDescription,0,sizeof(DSBUFFERDESC));
						DSBDescription.dwSize=sizeof(DSBUFFERDESC);
						DSBDescription.dwFlags=DSBCAPS_PRIMARYBUFFER|DSBCAPS_CTRL3D|DSBCAPS_CTRLVOLUME;
						if (IDirectSound_CreateSoundBuffer(device->lpDS,&DSBDescription,&device->DSpbuffer,NULL)==DS_OK)
						{
							if (IDirectSoundBuffer_SetFormat(device->DSpbuffer,&OutputType)==DS_OK)
							{
								if (IDirectSoundBuffer_QueryInterface(device->DSpbuffer,&IID_IDirectSound3DListener,&device->lpDS3DListener)==DS_OK)
								{
									memset(&dsCaps, 0, sizeof(DSCAPS));
									dsCaps.dwSize = sizeof(DSCAPS);
									if (IDirectSound_GetCaps(device->lpDS, &dsCaps) == DS_OK)
									{
										// Check that is an accelerated DS device
										if (!(dsCaps.dwFlags & DSCAPS_EMULDRIVER))
										{
											numSources = GetMaxNum3DMonoBuffers(device->lpDS);

											// To enable the 'DirectSound3D' device, the audio card MUST support
											// at least 16 voices.  (If not, then the device selection drops through
											// to the 'DirectSound' device).
											if (numSources >= 16)
											{
												device->MaxNoOfSources = numSources - 2; // Reserved one for a Stereo Source, and one for EAXFix

												if ((deviceName) && (!strcmp(deviceName,"DirectSound3D")))
													strcpy(device->szDeviceName, "DirectSound3D");
												else
													strcpy(device->szDeviceName, "Generic Hardware");

												// Set-up Timer for serving streaming buffers
												device->ulDS3DTimerInterval = TIMERINTERVAL;
												device->ulDS3DTimerID = 0;

												// Get the Timer Capabilities of the system
												timeGetDevCaps(&timeCaps, sizeof(TIMECAPS));

												// If desired accuracy is not available, then just go with the best that we have
												if (timeCaps.wPeriodMin > device->ulDS3DTimerInterval)
													device->ulDS3DTimerInterval = timeCaps.wPeriodMin;

												// Begin Time Period
												timeBeginPeriod(device->ulDS3DTimerInterval);

												bDeviceFound = AL_TRUE;
											}
										}
									}
								}
							}
						}
					}
				}
			}

			if (!bDeviceFound)
			{
				if (device->lpDS3DListener)
				{
					IDirectSound3DListener_Release(device->lpDS3DListener);
					device->lpDS3DListener=NULL;
				}

				if (device->DSpbuffer)
				{
					IDirectSoundBuffer_Release(device->DSpbuffer);
					device->DSpbuffer=NULL;
				}

				if (device->lpDS)
				{
					IDirectSound_Release(device->lpDS);
					device->lpDS = NULL;
				}

				// Failed to initialize DirectSound3D device - so fall back to DirectSound device
				bUseDS = AL_TRUE;
			}
		}

		if (bUseDS)
		{
			//Init COM
			CoInitialize(NULL);
			//DirectSound Init code
			if (CoCreateInstance(&CLSID_DirectSound,NULL,CLSCTX_INPROC_SERVER,&IID_IDirectSound,&device->lpDS)==S_OK)
			{
				if (IDirectSound_Initialize(device->lpDS,NULL)==DS_OK)
				{
					if (IDirectSound_SetCooperativeLevel(device->lpDS,GetForegroundWindow(),DSSCL_PRIORITY)==DS_OK)
					{
						memset(&DSBDescription,0,sizeof(DSBUFFERDESC));
						DSBDescription.dwSize=sizeof(DSBUFFERDESC);
						DSBDescription.dwFlags=DSBCAPS_PRIMARYBUFFER;
						if (IDirectSound_CreateSoundBuffer(device->lpDS,&DSBDescription,&device->DSpbuffer,NULL)==DS_OK)
						{
							if (IDirectSoundBuffer_SetFormat(device->DSpbuffer,&OutputType)==DS_OK)
							{
								memset(&DSBDescription,0,sizeof(DSBUFFERDESC));
								DSBDescription.dwSize=sizeof(DSBUFFERDESC);
								DSBDescription.dwFlags=DSBCAPS_GLOBALFOCUS|DSBCAPS_GETCURRENTPOSITION2;
								DSBDescription.dwBufferBytes=OUTPUT_BUFFER_SIZE;
								DSBDescription.lpwfxFormat=&OutputType;
								if (IDirectSound_CreateSoundBuffer(device->lpDS,&DSBDescription,&device->DSsbuffer,NULL)==DS_OK)
								{
									// Check that is a hardware accelerated DS device
									memset(&dsCaps, 0, sizeof(DSCAPS));
									dsCaps.dwSize = sizeof(DSCAPS);
									if (IDirectSound_GetCaps(device->lpDS, &dsCaps) == DS_OK)
									{
										// Check that is an accelerated DS device
										if (!(dsCaps.dwFlags & DSCAPS_EMULDRIVER))
										{
											if (IDirectSoundBuffer_Lock(device->DSsbuffer,0,0,&lpPart1, &dwSize1, &lpPart2, &dwSize2, DSBLOCK_ENTIREBUFFER ) == DS_OK)
											{
												memset(lpPart1, 0, dwSize1);
												IDirectSoundBuffer_Unlock(device->DSsbuffer,lpPart1, dwSize1, lpPart1, dwSize2);
											}
											if (IDirectSoundBuffer_Play(device->DSsbuffer,0,0,DSBPLAY_LOOPING)==DS_OK)
											{
                                                device->ulDSTimerID = timeSetEvent(25,0,(LPTIMECALLBACK)DirectSoundProc,(DWORD)device,(UINT)TIME_CALLBACK_FUNCTION|TIME_PERIODIC);
												device->MaxNoOfSources = 256;

												if ((deviceName) && (!strcmp(deviceName,"DirectSound")))
													strcpy(device->szDeviceName, "DirectSound");
												else
													strcpy(device->szDeviceName, "Generic Software");

												bDeviceFound = AL_TRUE;
											}
										}
									}
								}
							}
						}
					}
				}
			}

			if (!bDeviceFound)
			{
				if (device->DSsbuffer)
				{
					IDirectSoundBuffer_Release(device->DSsbuffer);
					device->DSsbuffer=NULL;
				}

				if (device->DSpbuffer)
				{
					IDirectSoundBuffer_Release(device->DSpbuffer);
					device->DSpbuffer = NULL;
				}
				
				if (device->lpDS)
				{
					IDirectSound_Release(device->lpDS);
					device->lpDS = NULL;
				}
			}
		}
		
		if (!bDeviceFound)
		{
			// Fallback to WaveOut code
			if (waveOutOpen(&device->hWaveHandle,WAVE_MAPPER,&OutputType,0,0,WAVE_FORMAT_DIRECT_QUERY)==MMSYSERR_NOERROR)
			{
                if (waveOutOpen(&device->hWaveHandle,WAVE_MAPPER,&OutputType,(DWORD_PTR)&WaveOutProc,(DWORD_PTR)device,CALLBACK_FUNCTION)==MMSYSERR_NOERROR)
				{
					device->hWaveHdrEvent = CreateEvent(NULL, AL_TRUE, AL_FALSE, "WaveOutAllHeadersReturned");
					if (device->hWaveHdrEvent != NULL)
					{
						device->hWaveThreadEvent = CreateEvent(NULL, AL_TRUE, AL_FALSE, "WaveOutThreadDestroyed");
						if (device->hWaveThreadEvent != NULL)
						{
							device->hWaveThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadProc,(LPVOID)device,0,&device->ulWaveThreadID);
							if (device->hWaveThread != NULL)
							{
								device->MaxNoOfSources = 256;
								// Setup Windows Multimedia driver buffers and start playing
								for (i=0;i<NUMWAVEBUFFERS;i++)
								{
									memset(&device->buffer[i],0,sizeof(WAVEHDR));
									device->buffer[i].dwBufferLength=((OutputType.nAvgBytesPerSec/16)&0xfffffff0);
									device->buffer[i].lpData=malloc(device->buffer[i].dwBufferLength);
									if (device->buffer[i].lpData)
										memset(device->buffer[i].lpData, 0, device->buffer[i].dwBufferLength);
									device->buffer[i].dwFlags=0;
									device->buffer[i].dwLoops=0;
									waveOutPrepareHeader(device->hWaveHandle,&device->buffer[i],sizeof(WAVEHDR));
									if (waveOutWrite(device->hWaveHandle,&device->buffer[i],sizeof(WAVEHDR))!=MMSYSERR_NOERROR)
									{
										waveOutUnprepareHeader(device->hWaveHandle,&device->buffer[i],sizeof(WAVEHDR));
										free(device->buffer[i].lpData);
									}
									else
										device->lWaveBuffersCommitted++;
								}
								strcpy(device->szDeviceName, "Generic Software Compatibility Mode");
								bDeviceFound = AL_TRUE;
							}
						}
					}
				}
			}

			if (!bDeviceFound)
			{
				if (device->hWaveThreadEvent)
				{
					CloseHandle(device->hWaveThreadEvent);
					device->hWaveThreadEvent = NULL;
				}

				if (device->hWaveHdrEvent)
				{
					CloseHandle(device->hWaveHdrEvent);
					device->hWaveHdrEvent = NULL;
				}

				if (device->hWaveHandle)
				{
					waveOutClose(device->hWaveHandle);
					device->hWaveHandle = NULL;
				}
			}
		}

		if (!bDeviceFound)
		{
			// No suitable output device found
			free(device);
			device = NULL;
		}
	}

	return device;
}


/*
	alcCloseDevice

	Close the specified Device
*/
ALCAPI ALCboolean ALCAPIENTRY alcCloseDevice(ALCdevice *pDevice)
{
	ALint i;
	ALCboolean bReturn = ALC_FALSE;

	if ((pDevice)&&(!pDevice->bIsCaptureDevice))
	{
		// Stop and release the DS3D timer
		if (pDevice->ulDS3DTimerID != 0)
			timeKillEvent(pDevice->ulDS3DTimerID);

		// End Timer Period
		timeEndPeriod(pDevice->ulDS3DTimerInterval);

		// Stop and release the DS timer
		if (pDevice->ulDSTimerID)
			timeKillEvent(pDevice->ulDSTimerID);

		// Wait ... just in case any timer events happen
		Sleep(100);

		EnterCriticalSection(&g_mutex);

		//Platform specific exit
		if (pDevice->lpDS)
		{
			if (pDevice->lpDS3DListener)
				IDirectSound3DListener_Release(pDevice->lpDS3DListener);
			if (pDevice->DSsbuffer)
				IDirectSoundBuffer_Release(pDevice->DSsbuffer);
			if (pDevice->DSpbuffer)
				IDirectSoundBuffer_Release(pDevice->DSpbuffer);
			if (pDevice->lpDS)
				IDirectSound_Release(pDevice->lpDS);
			//Deinit COM
			CoUninitialize();
		}
		else
		{
			pDevice->bWaveShutdown = AL_TRUE;

			// Wait for signal that all Wave Buffers have returned
			WaitForSingleObjectEx(pDevice->hWaveHdrEvent, 5000, FALSE);

			// Wait for signal that Wave Thread has been destroyed
			WaitForSingleObjectEx(pDevice->hWaveThreadEvent, 5000, FALSE);
			
			// Call waveOutReset to shutdown wave device
			waveOutReset(pDevice->hWaveHandle);

			// Release them all
			for (i=0;i<NUMWAVEBUFFERS;i++)
			{
				waveOutUnprepareHeader(pDevice->hWaveHandle,&pDevice->buffer[i],sizeof(WAVEHDR));
				free(pDevice->buffer[i].lpData);
			}

			// Close the Wave device
			waveOutClose(pDevice->hWaveHandle);
			pDevice->hWaveHandle = 0;

			CloseHandle(pDevice->hWaveThread);
			pDevice->hWaveThread = 0;

			if (pDevice->hWaveHdrEvent)
			{
				CloseHandle(pDevice->hWaveHdrEvent);
				pDevice->hWaveHdrEvent = 0;
			}

			if (pDevice->hWaveThreadEvent)
			{
				CloseHandle(pDevice->hWaveThreadEvent);
				pDevice->hWaveThreadEvent = 0;
			}
		}

		//Release device structure
		LeaveCriticalSection(&g_mutex);
		memset(pDevice,0,sizeof(ALCdevice));
		free(pDevice);
		
		bReturn = ALC_TRUE;
	}
	else
	{
		SetALCError(ALC_INVALID_DEVICE);
	}

	return bReturn;
}
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// ALC Device Specific Functions


/*
	WaveOutProc

	Used by "MMSYSTEM" device.  Posts a message to 'ThreadProc' everytime a WaveOut Buffer is completed and
	returns to the application (for more data)
*/
static void CALLBACK WaveOutProc(HWAVEOUT hDevice,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2)
{
	ALCdevice *pDevice;

	pDevice = (ALCdevice *)dwInstance;
	if ((uMsg==WOM_DONE) && (pDevice))
	{
		// Decrement number of buffers in use
		pDevice->lWaveBuffersCommitted--;

		if (pDevice->bWaveShutdown == AL_FALSE)
		{
			// Notify Wave Processor Thread that a Wave Header has returned
			PostThreadMessage(pDevice->ulWaveThreadID,uMsg,0,dwParam1);
		}
		else
		{
			if (pDevice->lWaveBuffersCommitted == 0)
			{
				// Signal Wave Buffers Returned event
				if (pDevice->hWaveHdrEvent)
					SetEvent(pDevice->hWaveHdrEvent);
				
				// Post 'Quit' Message to Wave Processor Thread
				PostThreadMessage(pDevice->ulWaveThreadID,WM_QUIT,0,0);
			}
		}
	}
}


/*
	ThreadProc

	Used by "MMSYSTEM" Device.  Called when a WaveOut buffer needs to be filled with new
	audio data.
*/
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	ALCcontext *pContext;
	ALCdevice *pDevice;
	LPWAVEHDR WaveHdr;
	MSG msg;

	pDevice = (ALCdevice*)lpParameter;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if ((msg.message==WOM_DONE)&&(!pDevice->bWaveShutdown))
		{
			// See if there is a Context using the WaveOut Device
			pContext = g_pContextList;
			while (pContext)
			{
				if (pContext->Device == pDevice)
					break;

				pContext = pContext->next;
			}

			WaveHdr=((LPWAVEHDR)msg.lParam);
			if (pContext)
			{
				SuspendContext(pContext);
				aluMixData(pContext,WaveHdr->lpData,WaveHdr->dwBufferLength,pContext->Format);
				ProcessContext(pContext);
			}
			else
			{
				memset(WaveHdr->lpData,0,WaveHdr->dwBufferLength);
			}
			
			waveOutWrite(pDevice->hWaveHandle,WaveHdr,sizeof(WAVEHDR));
			pDevice->lWaveBuffersCommitted++;
		}
	}

	// Signal Wave Thread completed event
	if (pDevice->hWaveThreadEvent)
		SetEvent(pDevice->hWaveThreadEvent);

	ExitThread(0);

	return 0;
}


/*
	DirectSoundProc

	Used by the "DirectSound" device to service the streaming DS Buffer.  Mixes new audio data to the
	buffer.
*/
static void CALLBACK DirectSoundProc(UINT uID,UINT uReserved,DWORD_PTR dwUser,DWORD_PTR dwReserved1,DWORD_PTR dwReserved2)
{
	static DWORD OldWriteCursor=0;
	DWORD PlayCursor,WriteCursor;
	BYTE *WritePtr1,*WritePtr2;
	DWORD WriteCnt1,WriteCnt2;
	WAVEFORMATEX OutputType;
	ALCcontext *pContext;
	ALCdevice *pDevice;
	DWORD BytesPlayed;
	HRESULT DSRes;

	pContext = alcGetCurrentContext();
	SuspendContext(pContext);

	// Get Device pointer from dwUser data
	pDevice = (ALCdevice *)dwUser;

	// See if there is a Context using this Device
	pContext = g_pContextList;
	while (pContext)
	{
		if (pContext->Device == pDevice)
			break;
		pContext = pContext->next;
	}

	// Get current play and write cursors
	IDirectSoundBuffer_GetCurrentPosition(pDevice->DSsbuffer,&PlayCursor,&WriteCursor);
	if (!OldWriteCursor) OldWriteCursor=WriteCursor-PlayCursor;

	// Get the output format and figure the number of bytes played (block aligned)
	IDirectSoundBuffer_GetFormat(pDevice->DSsbuffer,&OutputType,sizeof(WAVEFORMATEX),NULL);
	BytesPlayed=((((WriteCursor<OldWriteCursor)?(OUTPUT_BUFFER_SIZE+WriteCursor-OldWriteCursor):(WriteCursor-OldWriteCursor))/OutputType.nBlockAlign)*OutputType.nBlockAlign);

	// Lock output buffer started at 40msec in front of the old write cursor (15msec in front of the actual write cursor)
	DSRes=IDirectSoundBuffer_Lock(pDevice->DSsbuffer,(OldWriteCursor+(OutputType.nSamplesPerSec/25)*OutputType.nBlockAlign)%OUTPUT_BUFFER_SIZE,BytesPlayed,&WritePtr1,&WriteCnt1,&WritePtr2,&WriteCnt2,0);

	// If the buffer is lost, restore it, play and lock
	if (DSRes==DSERR_BUFFERLOST)
	{
		IDirectSoundBuffer_Restore(pDevice->DSsbuffer);
		IDirectSoundBuffer_Play(pDevice->DSsbuffer,0,0,DSBPLAY_LOOPING);
		DSRes=IDirectSoundBuffer_Lock(pDevice->DSsbuffer,(OldWriteCursor+(OutputType.nSamplesPerSec/25)*OutputType.nBlockAlign)%OUTPUT_BUFFER_SIZE,BytesPlayed,&WritePtr1,&WriteCnt1,&WritePtr2,&WriteCnt2,0);
	}

	// Successfully locked the output buffer
	if (DSRes==DS_OK)
	{
		// If we have a active context, mix data directly into output buffer otherwise fill with silence
		if (pContext)
		{
			if (WritePtr1)
				aluMixData(pContext,WritePtr1,WriteCnt1,pContext->Format);
			if (WritePtr2)
				aluMixData(pContext,WritePtr2,WriteCnt2,pContext->Format);
		}
		else
		{
			if (WritePtr1)
				memset(WritePtr1,0,WriteCnt1);
			if (WritePtr2)
				memset(WritePtr2,0,WriteCnt2);
		}

		// Unlock output buffer only when successfully locked
		IDirectSoundBuffer_Unlock(pDevice->DSsbuffer,WritePtr1,WriteCnt1,WritePtr2,WriteCnt2);
	}
	
	// Update old write cursor location
	OldWriteCursor=((OldWriteCursor+BytesPlayed)%OUTPUT_BUFFER_SIZE);

	ProcessContext(pContext);
}


/*
	DirectSound3DProc

	Used by "DirectSound3D" device.  Used to service all the streaming Direct Sound 3D Buffers.
*/
void CALLBACK DirectSound3DProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	ALuint loop;
	ALuint PlayCursor, WriteCursor, DataToLock;
    ALuint DataSize;
	ALuint Part1Size, Part2Size;
	ALuint DataCount;
	ALuint BytesPlayed, BufferSize;
	ALuint NewTime;
	ALint  BytesPlayedSinceLastTimer;
	ALvoid *lpPart1, *lpPart2;
	ALsource *ALSource;
	ALuint	ulSourceCount;
	ALCdevice *pDevice;
	ALCcontext *pContext;
	ALbufferlistitem *ALBufferListItem;

	pContext = alcGetCurrentContext();
	SuspendContext(pContext);

	// Get Device pointer from dwUser data
	pDevice = (ALCdevice *)dwUser;

	// See if there is a Context using the "DirectSound3D" Device
	pContext = g_pContextList;
	while (pContext)
	{
		if (pContext->Device == pDevice)
			break;
		pContext = pContext->next;
	}

	if (pContext)
	{
		ALSource = pContext->Source;
		ulSourceCount = pContext->SourceCount;
	}
	else
	{
		ulSourceCount = 0;
	}

	// Process each playing source
	for (loop=0;loop < ulSourceCount;loop++)
	{
		if (ALSource->DSBufferPlaying)
		{
			// Get position in DS Buffer
			IDirectSoundBuffer_GetCurrentPosition((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, &PlayCursor, &WriteCursor);

			// Get current time
			// if time elapsed > duration of circular buffer
			//	 bytesplayed += 88200
			//   oldwritecursor = writecursor
			// oldtime = current time
			NewTime = timeGetTime();

			if ((NewTime - ALSource->OldTime) > ALSource->BufferDuration)
			{
				// Buffer has wrapped around due to lack of Time Callbacks !
				BytesPlayedSinceLastTimer = 88200;
				ALSource->OldWriteCursor = WriteCursor;
			}
			else
			{
				// Calculate amount of data played since last Timer event
				if (ALSource->OldPlayCursor > PlayCursor)
					BytesPlayedSinceLastTimer = ((88200 - ALSource->OldPlayCursor) + PlayCursor);
				else
					BytesPlayedSinceLastTimer = (PlayCursor - ALSource->OldPlayCursor);
			}

			ALSource->lBytesPlayed += BytesPlayedSinceLastTimer;
			ALSource->OldTime = NewTime;

			// Lock buffer from Old Write cursor to current Play cursor
			if (ALSource->OldWriteCursor > PlayCursor)
				DataToLock = (88200 - ALSource->OldWriteCursor) + PlayCursor;
			else
				DataToLock = PlayCursor - ALSource->OldWriteCursor;

			// If Source is STOPPED, copy silence into DS Buffer, PAUSED buffers are physically stopped
			if (ALSource->CurrentState == AL_STOPPED)
			{
				// Check if we have already filled the buffer with silence
				if (ALSource->Silence < 88200)
				{
					IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, ALSource->OldWriteCursor, DataToLock, &lpPart1, &Part1Size, &lpPart2, &Part2Size, 0);

					if (lpPart1)
					{
						memset(lpPart1, 0, Part1Size);
						ALSource->Silence += Part1Size;
					}

					if (lpPart2)
					{
						memset(lpPart2, 0, Part2Size);
						ALSource->Silence += Part2Size;
					}

					IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, lpPart1, Part1Size, lpPart2, Part2Size);
				}

				// Update Old Write Cursor
				ALSource->OldWriteCursor += DataToLock;
				if (ALSource->OldWriteCursor >= 88200)
					ALSource->OldWriteCursor -= 88200;

				// Update Old Play Cursor
				ALSource->OldPlayCursor = PlayCursor;

				// Move on to next Source
				ALSource = ALSource->next;
				continue;
			}

			// Update current buffer variable

			if (ALSource->lBytesPlayed > 0)
			{
				// Find position in queue
				BytesPlayed = ALSource->lBytesPlayed;

				if (BytesPlayed >= ALSource->TotalBufferDataSize)
				{
					if (ALSource->bLooping == AL_TRUE)
					{
						BytesPlayed = (BytesPlayed % ALSource->TotalBufferDataSize);
					}
					else
					{
						// Not looping ... must have played too much data !
						BytesPlayed = ALSource->TotalBufferDataSize;
					}
				}

				ALBufferListItem = ALSource->queue;
				DataSize = 0;
				while (ALBufferListItem != NULL)
				{
					if (ALBufferListItem->buffer)
						BufferSize = ((ALbuffer*)(ALTHUNK_LOOKUPENTRY(ALBufferListItem->buffer)))->size;
					else
						BufferSize = 0;
					DataSize += BufferSize;
					if (DataSize >= BytesPlayed)
						break;
					else
						ALBufferListItem = ALBufferListItem->next;
				}

				// Record current BufferID
				ALSource->ulBufferID = ALBufferListItem->buffer;
			}

			// If we are not looping, check if any buffers in the queue have finished playing
			// Also check if the Source has now finished playing
			if (ALSource->bLooping == AL_FALSE)
			{
				DataCount = 0;
				ALSource->BuffersProcessed = 0;
				ALBufferListItem = ALSource->queue;
				while (ALBufferListItem)
				{
					if (ALBufferListItem->buffer)
                        DataSize = ((ALbuffer*)ALTHUNK_LOOKUPENTRY(ALBufferListItem->buffer))->size;
					else
						DataSize = 0;

					DataCount += DataSize;
					if ((ALint)DataCount <= ALSource->lBytesPlayed)
					{
						// Buffer has been played
						ALBufferListItem->bufferstate = PROCESSED;
						ALSource->BuffersProcessed++;
						ALBufferListItem = ALBufferListItem->next;
					}
					else
						ALBufferListItem = NULL;
				}

				// Check if finished - if so stop source !
				if (ALSource->lBytesPlayed >= (ALint)ALSource->TotalBufferDataSize)
				{
					ALSource->state = AL_STOPPED;
					ALSource->CurrentState = AL_STOPPED;

					// Make sure current buffer points to last queued buffer
					ALBufferListItem = ALSource->queue;
					while (ALBufferListItem)
					{
						if (ALBufferListItem->next == NULL)
							ALSource->ulBufferID = ALBufferListItem->buffer;
		
						ALBufferListItem = ALBufferListItem->next;
					}

					// Move on to next source
					ALSource = ALSource->next;
					continue;
				}
			}

			if (SUCCEEDED(IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, ALSource->OldWriteCursor, DataToLock, &lpPart1, &Part1Size, &lpPart2, &Part2Size, 0)))
			{
				FillBuffer(ALSource, (ALubyte *)lpPart1, Part1Size);
				FillBuffer(ALSource, (ALubyte *)lpPart2, Part2Size);

				ALSource->OldPlayCursor = PlayCursor;

				IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, lpPart1, Part1Size, lpPart2, Part2Size);
			}
		}

		ALSource = ALSource->next;
	}

	ProcessContext(pContext);

	return;
}


/*
	FillBuffer

	Used by "DirectSound3D" device.  Fills the given AL Source with new audio data from its
	queue of AL Buffers, or silence if the queue is completed
*/
ALvoid FillBuffer(ALsource *ALSource, ALubyte *pDestData, ALuint ulDestSize)
{
	ALbufferlistitem *ALBufferListItem;
	ALuint BytesWritten, BuffersToSkip, BufferID;
	ALuint DataSize, DataLeft;
	ALuint i;
	ALvoid *Data;

	if ((pDestData) && (ulDestSize > 0) && (ulDestSize <= 88200))
	{
		if (ALSource->FinishedQueue)
		{
			memset(pDestData, 0, ulDestSize);
			ALSource->SilenceAdded += ulDestSize;
			ALSource->OldWriteCursor += ulDestSize;
			if (ALSource->OldWriteCursor >= 88200)
				ALSource->OldWriteCursor -= 88200;
		}
		else
		{
			// Find position in buffer queue
			BuffersToSkip = ALSource->BuffersAddedToDSBuffer;

			if (BuffersToSkip >= ALSource->BuffersInQueue)
				BuffersToSkip = BuffersToSkip % ALSource->BuffersInQueue;

			ALBufferListItem = ALSource->queue;
			for (i = 0; i < BuffersToSkip; i++)
			{
				ALBufferListItem = ALBufferListItem->next;
			}

			BytesWritten = 0;
			BufferID = ALBufferListItem->buffer;

			while (AL_TRUE)
			{
				// Copy audio data from Open AL Buffer(s) into DS buffer
							
				// Find out how much data is left in current Open AL Buffer
				if (BufferID)
				{
					Data = (ALvoid*)(((ALbuffer*)ALTHUNK_LOOKUPENTRY(BufferID))->data);
					DataSize = ((ALbuffer*)ALTHUNK_LOOKUPENTRY(BufferID))->size;
				}
				else
				{
					Data = 0;
					DataSize = 0;
				}

				if (DataSize == 0)
				{
					DataLeft = 0;
				}
				else
				{
					if (ALSource->BufferPosition > DataSize)
						ALSource->BufferPosition = 0;

					DataLeft = DataSize - ALSource->BufferPosition;
				}

				if (DataLeft > (ulDestSize - BytesWritten))
				{
					// Copy (ulDestSize - BytesWritten) bytes to Direct Sound buffer
					memcpy(pDestData + BytesWritten, (ALubyte*)Data + ALSource->BufferPosition, ulDestSize - BytesWritten);

					ALSource->FinishedQueue = AL_FALSE;	// More data to follow ...
					ALSource->BufferPosition += (ulDestSize - BytesWritten);		// Record position in buffer data
					BytesWritten += (ulDestSize - BytesWritten);
					break;
				}
				else
				{
					// Not enough data in buffer to fill DS buffer so just copy as much data as possible
					if ((Data) && (DataLeft > 0))
					{
						memcpy(pDestData + BytesWritten, (ALubyte*)Data + ALSource->BufferPosition, DataLeft);
					}
					BytesWritten += DataLeft;

					ALSource->BuffersAddedToDSBuffer++;
					ALSource->BufferPosition = 0;

					// Get next valid buffer ID
					ALBufferListItem = ALBufferListItem->next;
							
					if (ALBufferListItem == NULL)
					{
						// No more buffers - check for looping flag
						if (ALSource->bLooping == AL_TRUE)
						{
							// Looping
							ALBufferListItem = ALSource->queue;
						}
						else
						{
							// Not looping and no more buffers
							ALSource->FinishedQueue = AL_TRUE;
							break;
						}
					}

					BufferID = ALBufferListItem->buffer;
				}
			}

			if (BytesWritten < ulDestSize)
			{
				// Fill the rest of the buffer with silence
				memset(pDestData + BytesWritten, 0, ulDestSize - BytesWritten);

				ALSource->SilenceAdded += (ulDestSize - BytesWritten);
			}

			ALSource->OldWriteCursor += ulDestSize;
			if (ALSource->OldWriteCursor >= 88200)
				ALSource->OldWriteCursor -= 88200;
		}
	}
}


/*
	UpdateContext

	Used by Direct Sound 3D device for any updates to the Listener or any of the Sources
*/
ALCvoid UpdateContext(ALCcontext *context, ALuint type, ALuint name)
{
	ALCcontext *ALContext;
	ALsource *ALSource;
	
	ALContext = context;
    ALSource = (ALsource*)ALTHUNK_LOOKUPENTRY(name);

	SuspendContext(ALContext);

	//Platform specific context updating
	if ((ALContext->Device->lpDS)&&(ALContext->Device->lpDS3DListener))
	{
		// Check if we need to update a Source
		if ( (type == ALSOURCE) && (alIsSource(name)) && (ALSource->update1) )
		{
			// First check for any Open AL Updates (e.g Position, Velocity, Looping etc ...)
			if (ALSource->update1)
				UpdateSource(ALContext, ALSource);

			// If we need to actually start playing the sound, do it now
			if ((ALSource->play) && (ALSource->uservalue1))
			{
				// Start playing the DS Streaming buffer (always looping)
				ALSource->OldTime = timeGetTime();
				IDirectSoundBuffer_Play((LPDIRECTSOUNDBUFFER)ALSource->uservalue1,0,0,DSBPLAY_LOOPING);
				ALSource->play = AL_FALSE;
				ALSource->DSBufferPlaying = AL_TRUE;
			}
		}

		// Check for Listener related updates
		if ((type == ALLISTENER) && (ALContext->Device->lpDS3DListener))
		{
			// Update any Open AL Listener Properties (e.g Position, Velocity, Orientation etc ...)
			if (ALContext->Listener.update1)
				UpdateListener(ALContext);
		}

	}

	ProcessContext(ALContext);
}


/*
	UpdateSource

	Used by "DirectSound3D" device to handles updates to AL Sources (including generation and destruction)
*/
void UpdateSource(ALCcontext *ALContext, ALsource *ALSource)
{
	WAVEFORMATEX OutputType;
	DSBUFFERDESC DSBDescription;
	ALfloat Dir[3], Pos[3], Vel[3];
	ALuint	DataPlayed, DataCount, TotalDataSize;
	ALint	BufferSize, DataCommitted;
	ALint	Relative;
    ALuint	Freq, State, outerAngle, innerAngle;
	ALfloat maxDist, minDist;
	ALvoid	*lpPart1, *lpPart2;
	ALuint	Part1Size, Part2Size, DataSize;
	ALuint	DataToCopy, PlayCursor, WriteCursor;
    ALuint	BufferID;
	ALbufferlistitem *ALBufferListItem;
	ALbufferlistitem *ALBufferListTemp;
	ALint	volume;
	ALuint BytesPlayedSinceLastTime, CursorGap, DataToLock;
	ALboolean bServiceNow;
	ALfloat	flVelFactor;

	// Check if the Source is being Destroyed
	if (ALSource->update1 == SDELETE)
	{
		// Destroy source
		if (ALSource->uservalue1)
		{
			IDirectSoundBuffer_Stop((LPDIRECTSOUNDBUFFER)ALSource->uservalue1);
			if (ALSource->uservalue3)
			{
				IKsPropertySet_Release((LPKSPROPERTYSET)ALSource->uservalue3);
				ALSource->uservalue3 = NULL;
			}
			if (ALSource->uservalue2)
			{
				IDirectSound3DBuffer_Release((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2);
				ALSource->uservalue2 = NULL;
			}
			IDirectSoundBuffer_Release((LPDIRECTSOUNDBUFFER)ALSource->uservalue1);
			ALSource->uservalue1=NULL;
		}

		ALSource->update1 &= ~SDELETE;
		if (ALSource->update1 == 0)
			return;
	}

	// Check if we need to generate a new Source
	if (ALSource->update1 & SGENERATESOURCE)
	{
		// Create a streaming DS buffer - 16bit mono 44.1KHz, 1 second in length
		memset(&DSBDescription,0,sizeof(DSBUFFERDESC));
		DSBDescription.dwSize=sizeof(DSBUFFERDESC);
		DSBDescription.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRL3D|DSBCAPS_GLOBALFOCUS|
			DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_LOCHARDWARE;
		DSBDescription.dwBufferBytes=88200;
		DSBDescription.lpwfxFormat=&OutputType;
		memset(&OutputType,0,sizeof(WAVEFORMATEX));
		OutputType.wFormatTag=WAVE_FORMAT_PCM;
		OutputType.nChannels=1;
		OutputType.wBitsPerSample=16;
		OutputType.nBlockAlign=2;
		OutputType.nSamplesPerSec=44100;
		OutputType.nAvgBytesPerSec=88200;
		OutputType.cbSize=0;
		ALSource->DSFrequency=44100;

		if (IDirectSound_CreateSoundBuffer(ALContext->Device->lpDS,&DSBDescription,(LPDIRECTSOUNDBUFFER *)&ALSource->uservalue1,NULL)==DS_OK)
		{
			IDirectSoundBuffer_SetCurrentPosition((LPDIRECTSOUNDBUFFER)ALSource->uservalue1,0);

			// Get 3D Interface
			if (IDirectSoundBuffer_QueryInterface((LPDIRECTSOUNDBUFFER)ALSource->uservalue1,&IID_IDirectSound3DBuffer,(LPUNKNOWN *)&ALSource->uservalue2)==DS_OK)
			{
				// Get Property Set Interface
				IDirectSound3DBuffer_QueryInterface((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2,&IID_IKsPropertySet,(LPUNKNOWN *)&ALSource->uservalue3);

				ALSource->SourceType = SOURCE3D;

				// Set Volume
				if (ALContext->bUseManualAttenuation)
				{
					SetSourceLevel(ALSource, 0);
				}
				else
				{
					volume = ALSource->lVolume + ALContext->Listener.lVolume;
					CLAMP(volume, -10000, 0)
					IDirectSoundBuffer_SetVolume((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, volume);
				}
			}
			else
			{
				// Failed creation of 3D interface, so release the buffer
				IDirectSoundBuffer_Release((LPDIRECTSOUNDBUFFER)ALSource->uservalue1);
				ALSource->uservalue1 = NULL;
			}
		}

		ALSource->update1 &= ~SGENERATESOURCE;
		if (ALSource->update1 == 0)
				return;
	}

	// Check if we need to Stop, Start, Pause, or Resume a Source
	if (ALSource->update1 & STATE)
	{
		State = ALSource->state;

		switch (State)
		{
			case AL_INITIAL:
				break;

			case AL_PLAYING:
				if (ALSource->uservalue1 == NULL)
				{
					ALSource->play = AL_FALSE;
					ALSource->state = AL_STOPPED;
					break;
				}

				if (ALSource->play)
				{
					if (ALSource->CurrentState == AL_PAUSED)
					{
						// If paused, just restart the DS buffer, and nothing else
						IDirectSoundBuffer_Play((LPDIRECTSOUNDBUFFER)ALSource->uservalue1,0,0,DSBPLAY_LOOPING);
						ALSource->DSBufferPlaying = AL_TRUE;
						ALSource->CurrentState = AL_PLAYING;
						ALSource->play = AL_FALSE;
						// Make sure we reset the LastTime
						ALSource->OldTime = timeGetTime();
						break;
					}

					// Skip over any Buffers marked as PROCESSED (this can happen if the Source has been given
					// an Offset position)
					ALBufferListItem = ALSource->queue;
					while ((ALBufferListItem) && (ALBufferListItem->bufferstate == PROCESSED))
						ALBufferListItem = ALBufferListItem->next;
					
					// Mark any NULL or zero-length buffers at the current play position as PROCESSED
					while (ALBufferListItem)
					{
						if (ALBufferListItem->buffer)
                            BufferSize = ((ALbuffer*)ALTHUNK_LOOKUPENTRY(ALBufferListItem->buffer))->size;
						else
							BufferSize = 0;
						if (BufferSize == 0)
						{
							// Skip over this buffer (and mark as processed)
							ALBufferListItem->bufferstate = PROCESSED;
							ALSource->BuffersProcessed++;
							ALBufferListItem = ALBufferListItem->next;
							ALSource->BuffersAddedToDSBuffer++;
						}
						else
						{
							// Found a valid buffer
							break;
						}
					}

					// Start multimedia timer (if not already in progress)
					if (ALContext->Device->ulDS3DTimerID == 0)
                        ALContext->Device->ulDS3DTimerID = timeSetEvent(ALContext->Device->ulDS3DTimerInterval, 0, (LPTIMECALLBACK)&DirectSound3DProc, (DWORD)ALContext->Device, (UINT)TIME_CALLBACK_FUNCTION | TIME_PERIODIC);

					// Update current buffer variable
					BufferID = ALBufferListItem->buffer;
                    ALSource->ulBufferID = BufferID;

					// Set Direct Sound buffer to frequency of current Open AL buffer multiplied by desired Pitch
                    Freq = ((ALbuffer*)ALTHUNK_LOOKUPENTRY(BufferID))->frequency;
					
					if (ALSource->DSFrequency != (unsigned long)(Freq*ALSource->flPitch))
					{
						if (SUCCEEDED(IDirectSoundBuffer_SetFrequency((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, (unsigned long)(Freq*ALSource->flPitch))))
							ALSource->DSFrequency = (unsigned long)(Freq*ALSource->flPitch);
					}

					// Record duration of the DS circular buffer
					if (ALSource->SourceType == SOURCE3D)
						ALSource->BufferDuration = 44100000.f / (float)(ALSource->DSFrequency);
					else
						ALSource->BufferDuration = 22050000.f / (float)(ALSource->DSFrequency);


					if (ALSource->DSBufferPlaying)
					{
						// Lock as much of the Buffer as possible (from write cursor to play cursor)
						IDirectSoundBuffer_GetCurrentPosition((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, &PlayCursor, &WriteCursor);
						
						if (PlayCursor > WriteCursor)
						{
							DataToCopy = PlayCursor - WriteCursor;
							ALSource->lBytesPlayed -= ((88200 - PlayCursor) + WriteCursor);
						}
						else
						{
							DataToCopy = ((88200 - WriteCursor) + PlayCursor);
							ALSource->lBytesPlayed -= WriteCursor - PlayCursor;
						}

						if (SUCCEEDED(IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1,WriteCursor,DataToCopy,&lpPart1,&Part1Size,&lpPart2,&Part2Size,0)))
						{
							// This is just for the debug output in the FillBuffer routine
							ALSource->OldWriteCursor = WriteCursor;

							FillBuffer(ALSource, lpPart1, Part1Size);
							FillBuffer(ALSource, lpPart2, Part2Size);

							// We will have filled the whole buffer (minus gap between play and write cursors) with data
							// up to the new Play Cursor.  So set OldWrite and OldPlay to current Play cursor, for the next
							// timer callback to correctly service this source.
							ALSource->OldPlayCursor = PlayCursor;
							ALSource->OldWriteCursor = PlayCursor;

							IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, lpPart1, Part1Size, lpPart2, Part2Size);
						}

						ALSource->play = AL_FALSE;
						// Make sure we reset the LastTime
						ALSource->OldTime = timeGetTime();
					}
					else
					{
						if (SUCCEEDED(IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1,0,0,&lpPart1,&Part1Size,0,0,DSBLOCK_ENTIREBUFFER)))
						{
							FillBuffer(ALSource, lpPart1, Part1Size);

							IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1,lpPart1,Part1Size,0,0);
						}
					}
				}

				ALSource->CurrentState = AL_PLAYING;
				break;

			case AL_PAUSED:
				if (ALSource->uservalue1)
				{
					if (ALSource->CurrentState != AL_PAUSED)
					{
						ALSource->CurrentState = AL_PAUSED;
						IDirectSoundBuffer_Stop((LPDIRECTSOUNDBUFFER)ALSource->uservalue1);
						ALSource->DSBufferPlaying = AL_FALSE;
					}
				}
				break;

			case AL_STOPPED:
				if (ALSource->uservalue1)
				{
					if (ALSource->CurrentState != AL_STOPPED)
					{
						if (ALSource->CurrentState == AL_PAUSED)
						{
							// DSBuffer is already stopped, so just re-set position to 0
							IDirectSoundBuffer_SetCurrentPosition((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, 0);
							ALSource->OldWriteCursor = 0;
							ALSource->OldPlayCursor = 0;
						}
						else
						{
							// Lock as much of buffer as possible, and fill with silence 
							// So that the DSBuffer does not need to be stopped
							IDirectSoundBuffer_GetCurrentPosition((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, &PlayCursor, &WriteCursor);
							
							if (PlayCursor > WriteCursor)
								DataToCopy = PlayCursor - WriteCursor;
							else
								DataToCopy = ((88200 - WriteCursor) + PlayCursor);

							if (SUCCEEDED(IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1,WriteCursor,DataToCopy,&lpPart1,&Part1Size,&lpPart2,&Part2Size,0)))
							{
								if (lpPart1)
								{
									memset(lpPart1, 0, Part1Size);
									ALSource->OldWriteCursor += Part1Size;
								}
								
								if (lpPart2)
								{
									memset(lpPart2, 0, Part2Size);
									ALSource->OldWriteCursor += Part2Size;
								}

								// Update Old Play and Old Write Cursors
								if (ALSource->OldWriteCursor >= 88200)
									ALSource->OldWriteCursor -= 88200;
								
								ALSource->OldPlayCursor = PlayCursor;

								IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, lpPart1, Part1Size, lpPart2, Part2Size);
							}
						}

						ALSource->CurrentState = AL_STOPPED;

						// Mark all buffers in queue as PROCESSED
						ALSource->BuffersProcessed = ALSource->BuffersInQueue;

						ALBufferListItem= ALSource->queue;
						while (ALBufferListItem != NULL)
						{
							ALBufferListItem->bufferstate = PROCESSED;
							ALBufferListItem = ALBufferListItem->next;
						}
					}
				}
				break;
		}

		// End of STATE update
		ALSource->update1 &= ~STATE;
		if (ALSource->update1 == 0)
			return;
	}


	if (ALSource->update1 & OFFSET)
	{
		ALSource->FinishedQueue = AL_FALSE;

		// Lock as much of buffer as possible, and fill with data from the new Byte Offset
		IDirectSoundBuffer_GetCurrentPosition((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, &PlayCursor, &WriteCursor);
		
		if (PlayCursor > WriteCursor)
			DataToCopy = PlayCursor - WriteCursor;
		else
			DataToCopy = ((88200 - WriteCursor) + PlayCursor);

		ALSource->lBytesPlayed -= (88200 - DataToCopy);

		if (SUCCEEDED(IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1,WriteCursor,DataToCopy,&lpPart1,&Part1Size,&lpPart2,&Part2Size,0)))
		{
			// This is just for the debug output in the FillBuffer routine
			ALSource->OldWriteCursor = WriteCursor;

			FillBuffer(ALSource, lpPart1, Part1Size);
			FillBuffer(ALSource, lpPart2, Part2Size);

			// We will have filled the whole buffer (minus gap between play and write cursors) with data
			// up to the new Play Cursor.  So set OldWrite and OldPlay to current Play cursor, for the next
			// timer callback to correctly service this source.
			ALSource->OldPlayCursor = PlayCursor;
			ALSource->OldWriteCursor = PlayCursor;

			IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, lpPart1, Part1Size, lpPart2, Part2Size);
		}

		ALSource->update1 &= ~OFFSET;
		if (ALSource->update1 == 0)
			return;
	}

	// Check if we need to update the 3D Position of the Source
	if (ALSource->update1 & POSITION)
	{
		if (ALSource->uservalue2)
		{
			Pos[0] = ALSource->vPosition[0];
			Pos[1] = ALSource->vPosition[1];
			Pos[2] = -ALSource->vPosition[2];
			IDirectSound3DBuffer_SetPosition((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2,Pos[0],Pos[1],Pos[2],DS3D_IMMEDIATE);

			// Manual attenation
			if (ALContext->bUseManualAttenuation)
			{
				// Calculate distance between Source and Listener
				if (ALSource->bHeadRelative)
				{
					ALSource->flDistance =
						(float)sqrt(sqr(ALSource->vPosition[0]) +
									sqr(ALSource->vPosition[1]) +
									sqr(ALSource->vPosition[2]));
				}
				else
				{
					ALSource->flDistance =
						(float)sqrt(sqr(ALContext->Listener.Position[0] - ALSource->vPosition[0]) +
									sqr(ALContext->Listener.Position[1] - ALSource->vPosition[1]) +
									sqr(ALContext->Listener.Position[2] - ALSource->vPosition[2]));
				}
				
				SetSourceLevel(ALSource, LEVELFLAG_RECALCULATE_ATTENUATION);
			}
		}

		ALSource->update1 &= ~POSITION;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if we need to adjust the velocity of the Source
	if (ALSource->update1 & VELOCITY)
	{
		if (ALSource->uservalue2)
		{
			flVelFactor = SPEEDOFSOUNDMETRESPERSEC / ALContext->flSpeedOfSound;

			Vel[0] = ALSource->vVelocity[0] * flVelFactor;
			Vel[1] = ALSource->vVelocity[1] * flVelFactor;
			Vel[2] = -ALSource->vVelocity[2] * flVelFactor;
			IDirectSound3DBuffer_SetVelocity((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2,Vel[0],Vel[1],Vel[2],DS3D_IMMEDIATE);
		}

		ALSource->update1 &= ~VELOCITY;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if we need to adjust the Orientation of the Source
	if (ALSource->update1 & ORIENTATION)
	{
		if (ALSource->uservalue2)
		{
			Dir[0] = ALSource->vOrientation[0];
			Dir[1] = ALSource->vOrientation[1];
			Dir[2] = -ALSource->vOrientation[2];
			IDirectSound3DBuffer_SetConeOrientation((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2,Dir[0],Dir[1],Dir[2],DS3D_IMMEDIATE);
		}

		ALSource->update1 &= ~ORIENTATION;
		if (ALSource->update1 == 0)
			return;
	}

	
	// Check if any Buffers have been added to this Source's queue
	if (ALSource->update1 & SQUEUE)
	{
		bServiceNow = AL_FALSE;

		if ((ALSource->uservalue1) && ((ALSource->state == AL_PLAYING) || (ALSource->state == AL_PAUSED)))
		{
			// Some buffer(s) have been added to the queue

			// If silence has been added, then we need to overwrite the silence with new audio
			// data from the buffers in the queue
			if (ALSource->SilenceAdded > 0)
			{
				// Need to determine if it is safe / possible to write the new queued data on to the end of the
				// last valid data.   This is only possible, if the Play Cursor has not gone past the end of the
				// valid data minus the gap between the Play and Write cursors.
				// If the data can't be added at the end, it should be written to the current Write Cursor and
				// BytesPlayed adjusted accordingly.

				IDirectSoundBuffer_GetCurrentPosition((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, &PlayCursor, &WriteCursor);

				if (WriteCursor >= PlayCursor)
					CursorGap = WriteCursor - PlayCursor;
				else
					CursorGap = (88200 - PlayCursor) + WriteCursor;

				if (PlayCursor >= ALSource->OldPlayCursor)
					BytesPlayedSinceLastTime = PlayCursor - ALSource->OldPlayCursor;
				else
					BytesPlayedSinceLastTime = (88200 - ALSource->OldPlayCursor) + PlayCursor;

				ALSource->lBytesPlayed += BytesPlayedSinceLastTime;

				if (ALSource->lBytesPlayed >= (ALint)(ALSource->TotalBufferDataSize - CursorGap))
				{
					ALSource->OldWriteCursor = WriteCursor;

					// If we have played more than the TotalBufferDataSize then set lBytesPlayed
					// to the total size minus the gap between the Play and Write cursors (because new data will
					// be written to Write Cursor)

					// Otherwise, we need a minor correction to BytesPlayed equal to the gap between the WriteCursor
					// and the end of the valid data (otherwise this will be included in BytesPlayed)

					if (ALSource->lBytesPlayed > (ALint)ALSource->TotalBufferDataSize)
						ALSource->lBytesPlayed = ALSource->TotalBufferDataSize - CursorGap;
					else
						ALSource->lBytesPlayed -= (CursorGap - (ALSource->TotalBufferDataSize - ALSource->lBytesPlayed));
				}
				else
				{
					if (ALSource->SilenceAdded > ALSource->OldWriteCursor)
					{
						ALSource->OldWriteCursor = (88200 - ALSource->SilenceAdded + ALSource->OldWriteCursor);
					}
					else
						ALSource->OldWriteCursor -= ALSource->SilenceAdded;
				}

				// Read position from next buffer should be set to 0
				ALSource->BufferPosition = 0;

				// We have overwritten the silent data, so reset variable
				ALSource->SilenceAdded = 0;

				// Make sure that the we haven't finished processing the queue !
				ALSource->FinishedQueue = AL_FALSE;

				// Need to copy new data to the (new) Write cursor immediately
				bServiceNow = AL_TRUE;
			}
			else if (ALSource->bLooping == AL_TRUE)
			{
				// Need to copy new data to the (new) Write cursor immediately
				bServiceNow = AL_TRUE;

				// Get position in DS Buffer
				IDirectSoundBuffer_GetCurrentPosition((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, &PlayCursor, &WriteCursor);

				// Calculate amount of data played
				if (ALSource->OldPlayCursor > PlayCursor)
					ALSource->lBytesPlayed += ((88200 - ALSource->OldPlayCursor) + PlayCursor);
				else
					ALSource->lBytesPlayed += (PlayCursor - ALSource->OldPlayCursor);

				// BytesPlayed and BuffersAddedToDSBuffer are always MOD'ed against the total data size, and
				// number of buffers in queue respectively.  These calculates will go wrong if the queue has completed
				// a loop, so we need to re-set them here (using the old values of TotalBufferData and BuffersInQueue)
				// so they effectively are set to values as if the queue hasn't looped (yet)
				if (ALSource->lBytesPlayed > 0)
					ALSource->lBytesPlayed = (ALSource->lBytesPlayed % ALSource->TotalBufferDataSize);

				ALSource->BuffersAddedToDSBuffer = (ALSource->BuffersAddedToDSBuffer % (ALSource->BuffersInQueue - ALSource->NumBuffersAddedToQueue));
				
				// If the current queue is smaller than 88200, it will be necessary to overwrite some
				// data that has been copied into the DS Buffer.   Re-set OldWriteCursor to end of current
				// queue loop, and update BufferPosition and BuffersAddedToDSBuffer, so in the TimerCallback
				// the new queued buffers will be added at the correct location
				if ((ALSource->TotalBufferDataSize - ALSource->lBytesPlayed) < 88200)
				{
					// Need to find position to write new data
					ALSource->OldWriteCursor = PlayCursor + (ALSource->TotalBufferDataSize - ALSource->lBytesPlayed);

					if (ALSource->OldWriteCursor >= 88200)
						ALSource->OldWriteCursor -= 88200;

					// Make sure that ALSource->OldWriteCursor is NOT between the current PlayCursor and WriteCursor
					if (WriteCursor > PlayCursor)
					{
						while ((ALSource->OldWriteCursor >= PlayCursor) && (ALSource->OldWriteCursor <= WriteCursor))
						{
							ALSource->OldWriteCursor += ALSource->TotalBufferDataSize;
							if (ALSource->OldWriteCursor >= 88200)
								ALSource->OldWriteCursor -= 88200;

							ALSource->lBytesPlayed -= ALSource->TotalBufferDataSize;
						}
					}
					else
					{
						while ((ALSource->OldWriteCursor >= PlayCursor) && (ALSource->OldWriteCursor <= WriteCursor))
						{
							ALSource->OldWriteCursor += ALSource->TotalBufferDataSize;
							if (ALSource->OldWriteCursor >= 88200)
								ALSource->OldWriteCursor -= 88200;

							ALSource->lBytesPlayed -= ALSource->TotalBufferDataSize;
						}
					}

					// Read position from next buffer should be set to 0
					ALSource->BufferPosition = 0;

					ALSource->BuffersAddedToDSBuffer = ALSource->BuffersInQueue - ALSource->NumBuffersAddedToQueue;

					// Make sure that the we haven't finished processing the queue !
					ALSource->FinishedQueue = AL_FALSE;
				}
			}
		}

		ALSource->TotalBufferDataSize += ALSource->SizeOfBufferDataAddedToQueue;
		ALSource->SizeOfBufferDataAddedToQueue = 0;
		ALSource->NumBuffersAddedToQueue = 0;

		if (bServiceNow)
		{
			// Lock buffer from Old Write cursor to current Play cursor
			if (ALSource->OldWriteCursor > PlayCursor)
				DataToLock = (88200 - ALSource->OldWriteCursor) + PlayCursor;
			else
				DataToLock = PlayCursor - ALSource->OldWriteCursor;

			if (SUCCEEDED(IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, ALSource->OldWriteCursor, DataToLock, &lpPart1, &Part1Size, &lpPart2, &Part2Size, 0)))
			{
				FillBuffer(ALSource, (ALubyte *)lpPart1, Part1Size);
				FillBuffer(ALSource, (ALubyte *)lpPart2, Part2Size);

				ALSource->OldPlayCursor = PlayCursor;

				IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, lpPart1, Part1Size, lpPart2, Part2Size);
			}
		}

		ALSource->update1 &= ~SQUEUE;
		if (ALSource->update1 == 0)
			return;
	}

	// Check if any Buffers have been removed from this Source's Queue
	if (ALSource->update1 & SUNQUEUE)
	{
		// Some number of buffers have been removed from the queue

		// We need to update some variables to correctly reflect the new queue

		// The number of BuffersAddedToDSBuffers must be decreased by the number of buffers
		// removed from the queue (or else the Timer function will think we are further through
		// the list than we are)

		// The amount of DataPlayed must be decreased by the total size of the data in the buffers
		// removed from the queue (or the amount of data still to play (TotalDataSize - DataPlayed)
		// will be incorrect)
		if ((ALSource->uservalue1) && (ALSource->state == AL_PLAYING))
		{
			ALSource->lBytesPlayed -= ALSource->SizeOfBufferDataRemovedFromQueue;
		}
		ALSource->TotalBufferDataSize -= ALSource->SizeOfBufferDataRemovedFromQueue;
		ALSource->NumBuffersRemovedFromQueue = 0;
		ALSource->SizeOfBufferDataRemovedFromQueue = 0;

		// If we're not playing then reset current buffer (it may have changed)
		if (ALSource->state != AL_PLAYING)
		{
			if (ALSource->queue)
				BufferID = ALSource->queue->buffer;
			else
				BufferID = 0;

            ALSource->ulBufferID = BufferID;
		}

		ALSource->update1 &= ~SUNQUEUE;
		if (ALSource->update1 == 0)
			return;
	}

	// Check if we need to adjust the volume of the Source
	if (ALSource->update1 & VOLUME)
	{
		if (ALSource->uservalue1)
		{
			ALSource->lVolume = LinearGainToMB(ALSource->flGain);

			if (ALContext->bUseManualAttenuation)
			{
				SetSourceLevel(ALSource, 0);
			}
			else
			{
				volume = ALSource->lVolume + ALContext->Listener.lVolume;
				CLAMP(volume, -10000, 0)
				IDirectSoundBuffer_SetVolume((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, volume);
			}
		}

		ALSource->update1 &= ~VOLUME;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if we need to adjust the frequency of the Source
	if (ALSource->update1 & FREQUENCY)
	{
		if (ALSource->uservalue1)
		{
            BufferID = ALSource->ulBufferID;
			if (BufferID == 0)
				Freq = 44100;
			else
                Freq = ((ALbuffer*)ALTHUNK_LOOKUPENTRY(BufferID))->frequency;

			if (SUCCEEDED(IDirectSoundBuffer_SetFrequency((LPDIRECTSOUNDBUFFER)ALSource->uservalue1,(unsigned long)(Freq*ALSource->flPitch))))
				ALSource->DSFrequency = (unsigned long)(Freq*ALSource->flPitch);
			
			// Update duration of the DS circular buffer
			if (ALSource->SourceType == SOURCE3D)
				ALSource->BufferDuration = 44100000.f / (float)(ALSource->DSFrequency);
			else
				ALSource->BufferDuration = 22050000.f / (float)(ALSource->DSFrequency);
		}

		ALSource->update1 &= ~FREQUENCY;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if we need to adjust the Min Distance of the Source
	if (ALSource->update1 & MINDIST)
	{
		if (ALSource->uservalue2)
		{
			minDist = ALSource->flRefDistance;
			IDirectSound3DBuffer_SetMinDistance((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2,minDist,DS3D_IMMEDIATE);

			// Manual attenuation
			if (ALContext->bUseManualAttenuation)
				SetSourceLevel(ALSource, LEVELFLAG_RECALCULATE_ATTENUATION);
		}

		ALSource->update1 &= ~MINDIST;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if we need to adjust the Max Distance of the Source
	if (ALSource->update1 & MAXDIST)
	{
		if (ALSource->uservalue2)
		{
			maxDist = ALSource->flMaxDistance;
			IDirectSound3DBuffer_SetMaxDistance((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2,maxDist,DS3D_IMMEDIATE);

			// Manual attenuation
			if (ALContext->bUseManualAttenuation)
				SetSourceLevel(ALSource, LEVELFLAG_RECALCULATE_ATTENUATION);
		}

		ALSource->update1 &= ~MAXDIST;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if we need to adjust the Cone Outside Volume of the Source
	if (ALSource->update1 & CONEOUTSIDEVOLUME)
	{
		if (ALSource->uservalue2)
		{
			volume = LinearGainToMB(ALSource->flOuterGain);

			IDirectSound3DBuffer_SetConeOutsideVolume((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2, volume,DS3D_IMMEDIATE);
		}

		ALSource->update1 &= ~CONEOUTSIDEVOLUME;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if we need to update the Roll-Off Factor of the Source
	if (ALSource->update1 & ROLLOFFFACTOR)
	{
		if (ALSource->uservalue2)
		{
			// Manual attenuation
			SetSourceLevel(ALSource, LEVELFLAG_RECALCULATE_ATTENUATION);
		}

		ALSource->update1 &= ~ROLLOFFFACTOR;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if we need to update Min Gain
	if (ALSource->update1 & MINGAIN)
	{
		ALSource->lMinVolume = LinearGainToMB(ALSource->flMinGain);

		// Manual attenuation
		SetSourceLevel(ALSource, 0);

		ALSource->update1 &= ~MINGAIN;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if we need to update Max Gain
	if (ALSource->update1 & MAXGAIN)
	{
		ALSource->lMaxVolume = LinearGainToMB(ALSource->flMaxGain);

		// Manual attenuation
		SetSourceLevel(ALSource, 0);

		ALSource->update1 &= ~MAXGAIN;
		if (ALSource->update1 == 0)
			return;
	}
	

	// Check if we need to update the 3D Processing Mode (Head Relative)
	if (ALSource->update1 & MODE)
	{
		if (ALSource->uservalue2)
		{
			Relative = ALSource->bHeadRelative ? DS3DMODE_HEADRELATIVE : DS3DMODE_NORMAL;
			IDirectSound3DBuffer_SetMode((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2,Relative,DS3D_IMMEDIATE);

			// Manual attenation
			if (ALContext->bUseManualAttenuation)
			{
				// Calculate distance between Source and Listener
				if (ALSource->bHeadRelative)
				{
					ALSource->flDistance =
						(float)sqrt(sqr(ALSource->vPosition[0]) +
									sqr(ALSource->vPosition[1]) +
									sqr(ALSource->vPosition[2]));
				}
				else
				{
					ALSource->flDistance =
						(float)sqrt(sqr(ALContext->Listener.Position[0] - ALSource->vPosition[0]) +
									sqr(ALContext->Listener.Position[1] - ALSource->vPosition[1]) +
									sqr(ALContext->Listener.Position[2] - ALSource->vPosition[2]));
				}
				
				SetSourceLevel(ALSource, LEVELFLAG_RECALCULATE_ATTENUATION);
			}
		}

		ALSource->update1 &= ~MODE;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if we ne need to update the Cone Angles for the Source
	if (ALSource->update1 & CONEANGLES)
	{
		if (ALSource->uservalue2)
		{
			innerAngle = (ALuint)ALSource->flInnerAngle;
			outerAngle = (ALuint)ALSource->flOuterAngle;

			IDirectSound3DBuffer_SetConeAngles((LPDIRECTSOUND3DBUFFER)ALSource->uservalue2,innerAngle,outerAngle,DS3D_IMMEDIATE);
		}

		ALSource->update1 &= ~CONEANGLES;
		if (ALSource->update1 == 0)
			return;
	}


	// Check if Looping has been enabled / disabled
	if (ALSource->update1 & LOOPED)
	{
		bServiceNow = AL_FALSE;

		// Only has an effect if the Source is playing
		if ((ALSource->uservalue1) && ((ALSource->state == AL_PLAYING) || (ALSource->state == AL_PAUSED)))
		{
			// Find out whether Looping has been enabled or disabled
			if (ALSource->bLooping == AL_TRUE)
			{
				// Looping enabled !

				// All buffers in queue will be needed again, so their state needs to be upgraded
				// to PENDING, and the number of buffers processed set to 0
				ALSource->BuffersProcessed = 0;

				// Set all Buffer states to PENDING
				ALBufferListTemp = ALSource->queue;
				while (ALBufferListTemp != NULL)
				{
					ALBufferListTemp->bufferstate = PENDING;
					ALBufferListTemp = ALBufferListTemp->next;
				}

				// If we have added silence after the valid data, then we need to set the new
				// write position back to the end of the valid data
				if (ALSource->SilenceAdded > 0)
				{
					if (ALSource->OldWriteCursor < ALSource->SilenceAdded)
						ALSource->OldWriteCursor += 88200;

					ALSource->OldWriteCursor -= ALSource->SilenceAdded;

					ALSource->BufferPosition = 0;

					ALSource->SilenceAdded = 0;

					IDirectSoundBuffer_GetCurrentPosition((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, &PlayCursor, &WriteCursor);

					// Calculate amount of data played
					if (ALSource->OldPlayCursor > PlayCursor)
						ALSource->lBytesPlayed += ((88200 - ALSource->OldPlayCursor) + PlayCursor);
					else
						ALSource->lBytesPlayed += (PlayCursor - ALSource->OldPlayCursor);
					
					bServiceNow = AL_TRUE;
				}

				ALSource->FinishedQueue = AL_FALSE;
			}
			else
			{
				// Looping disabled !

				// We need to calculate how much data is still to be played
				IDirectSoundBuffer_GetCurrentPosition((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, &PlayCursor, &WriteCursor);
				
				// Calculate amount of data played
				if (ALSource->OldPlayCursor > PlayCursor)
					ALSource->lBytesPlayed += ((88200 - ALSource->OldPlayCursor) + PlayCursor);
				else
					ALSource->lBytesPlayed += (PlayCursor - ALSource->OldPlayCursor);

				// Calculate how much data is left to play for the current iteration of the looping data
				TotalDataSize = ALSource->TotalBufferDataSize;

				if (ALSource->lBytesPlayed > (ALint)TotalDataSize)
					ALSource->lBytesPlayed = (ALSource->lBytesPlayed % TotalDataSize);

				ALSource->BuffersAddedToDSBuffer = (ALSource->BuffersAddedToDSBuffer % ALSource->BuffersInQueue);

				if (WriteCursor >= PlayCursor)
					DataCommitted = WriteCursor - PlayCursor;
				else
					DataCommitted = (88200 - PlayCursor) + WriteCursor;

				if (DataCommitted > (ALint)(ALSource->TotalBufferDataSize - ALSource->lBytesPlayed))
				{
					// Data for the next iteration of the loop has already been committed
					ALSource->lBytesPlayed -= TotalDataSize;
				}
				else
				{
					DataPlayed = ALSource->lBytesPlayed;
					DataCount = 0;
					ALSource->BuffersProcessed = 0;

					ALBufferListItem = ALSource->queue;
					while (ALBufferListItem != NULL)
					{
						if (ALBufferListItem->buffer)
	                        DataSize = ((ALbuffer*)ALTHUNK_LOOKUPENTRY(ALBufferListItem->buffer))->size;
						else
							DataSize = 0;

						DataCount += DataSize;
						if (DataCount < DataPlayed)
						{
							ALBufferListItem->bufferstate = PROCESSED;
							ALBufferListItem = ALBufferListItem->next;
							ALSource->BuffersProcessed++;
						}
						else
							ALBufferListItem = NULL;
					}
				}

				if ((ALSource->TotalBufferDataSize - ALSource->lBytesPlayed) < 88200)
				{
					// Need to move Write Cursor to end of valid data (so silence can be added after it)
					ALSource->OldWriteCursor = PlayCursor + (ALSource->TotalBufferDataSize - ALSource->lBytesPlayed);

					if (ALSource->OldWriteCursor >= 88200)
						ALSource->OldWriteCursor -= 88200;

					ALSource->FinishedQueue = AL_TRUE;
				}

				bServiceNow = AL_TRUE;
			}

			if (bServiceNow)
			{
				// Lock buffer from Old Write cursor to current Play cursor
				if (ALSource->OldWriteCursor > PlayCursor)
					DataToLock = (88200 - ALSource->OldWriteCursor) + PlayCursor;
				else
					DataToLock = PlayCursor - ALSource->OldWriteCursor;

				if (SUCCEEDED(IDirectSoundBuffer_Lock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, ALSource->OldWriteCursor, DataToLock, &lpPart1, &Part1Size, &lpPart2, &Part2Size, 0)))
				{
					FillBuffer(ALSource, (ALubyte *)lpPart1, Part1Size);
					FillBuffer(ALSource, (ALubyte *)lpPart2, Part2Size);

					ALSource->OldPlayCursor = PlayCursor;

					IDirectSoundBuffer_Unlock((LPDIRECTSOUNDBUFFER)ALSource->uservalue1, lpPart1, Part1Size, lpPart2, Part2Size);
				}
			}
		}

		ALSource->update1 &= ~LOOPED;
		if (ALSource->update1 == 0)
			return;
	}

	return;
}


/*
	UpdateListener

	Used by "DirectSound3D" device to handle 3D Listener updates
*/
void UpdateListener(ALCcontext *ALContext)
{
	ALfloat		Pos[3],Vel[3], Ori[6];
	ALfloat		flDistanceFactor, flVelFactor;
	ALint		volume;
	ALsource	*pSourceList;

	if (ALContext->Listener.update1 & LVOLUME)
	{
		ALContext->Listener.lVolume = LinearGainToMB(ALContext->Listener.Gain);

		if (ALContext->bUseManualAttenuation)
		{
			pSourceList = ALContext->Source;
			while (pSourceList)
			{
				if (pSourceList->uservalue1)
					SetSourceLevel(pSourceList, 0);

				pSourceList = pSourceList->next;
			}
		}
		else
		{
			pSourceList = ALContext->Source;
			while (pSourceList)
			{
				if (pSourceList->uservalue1)
				{
					volume = pSourceList->lVolume + ALContext->Listener.lVolume;
					CLAMP(volume, -10000, 0)
					IDirectSoundBuffer_SetVolume((LPDIRECTSOUNDBUFFER)pSourceList->uservalue1, volume);
				}

				pSourceList = pSourceList->next;
			}
		}

		ALContext->Listener.update1 &= ~LVOLUME;
		if (ALContext->Listener.update1 == 0)
			return;
	}

	if (ALContext->Listener.update1 & LPOSITION)
	{
		Pos[0] = ALContext->Listener.Position[0];
		Pos[1] = ALContext->Listener.Position[1];
		Pos[2] = -ALContext->Listener.Position[2];
		IDirectSound3DListener_SetPosition(ALContext->Device->lpDS3DListener, Pos[0], Pos[1], Pos[2],DS3D_IMMEDIATE);

		// Manual attenuation
		if (ALContext->bUseManualAttenuation)
		{
			pSourceList = ALContext->Source;
			while (pSourceList)
			{
				// Calculate distance between Source and Listener (only affects 3D non-Head Relative sounds)
				if ((pSourceList->uservalue2) && (pSourceList->bHeadRelative == AL_FALSE))
				{
					pSourceList->flDistance =
						(float)sqrt(sqr(ALContext->Listener.Position[0] - pSourceList->vPosition[0]) +
									sqr(ALContext->Listener.Position[1] - pSourceList->vPosition[1]) +
									sqr(ALContext->Listener.Position[2] - pSourceList->vPosition[2]));
				}

				SetSourceLevel(pSourceList, LEVELFLAG_RECALCULATE_ATTENUATION);
				pSourceList = pSourceList->next;
			}
		}

		ALContext->Listener.update1 &= ~LPOSITION;
		if (ALContext->Listener.update1 == 0)
			return;
	}

	if (ALContext->Listener.update1 & LVELOCITY)
	{
		flVelFactor = SPEEDOFSOUNDMETRESPERSEC / ALContext->flSpeedOfSound;
		Vel[0] = ALContext->Listener.Velocity[0] * flVelFactor;
		Vel[1] = ALContext->Listener.Velocity[1] * flVelFactor;
		Vel[2] = -ALContext->Listener.Velocity[2] * flVelFactor;
		IDirectSound3DListener_SetVelocity(ALContext->Device->lpDS3DListener, Vel[0], Vel[1], Vel[2], DS3D_IMMEDIATE);
		ALContext->Listener.update1 &= ~LVELOCITY;
		if (ALContext->Listener.update1 == 0)
			return;
	}

	if (ALContext->Listener.update1 & LORIENTATION)
	{
		Ori[0] = ALContext->Listener.Forward[0];
		Ori[1] = ALContext->Listener.Forward[1];
		Ori[2] = -ALContext->Listener.Forward[2];
		Ori[3] = ALContext->Listener.Up[0];
		Ori[4] = ALContext->Listener.Up[1];
		Ori[5] = -ALContext->Listener.Up[2];
		IDirectSound3DListener_SetOrientation(ALContext->Device->lpDS3DListener, Ori[0], Ori[1], Ori[2], Ori[3], Ori[4], Ori[5], DS3D_IMMEDIATE);
		ALContext->Listener.update1 &= ~LORIENTATION;
		if (ALContext->Listener.update1 == 0)
			return;
	}

	if (ALContext->Listener.update1 & LDOPPLERFACTOR)
	{
		IDirectSound3DListener_SetDopplerFactor(ALContext->Device->lpDS3DListener,ALContext->DopplerFactor,DS3D_IMMEDIATE);
		ALContext->Listener.update1 &= ~LDOPPLERFACTOR;
		if (ALContext->Listener.update1 == 0)
			return;
	}

	if (ALContext->Listener.update1 & LDOPPLERVELOCITY)
	{
		// Doppler Velocity is used to set the speed of sound in units per second
		// DS3D uses Distance Factor to relate units to real world coordinates (metres)
		// Therefore need to convert Doppler Velocity into DS3D Distance Factor
		flDistanceFactor = 1.0f / ALContext->DopplerVelocity;
		IDirectSound3DListener_SetDistanceFactor(ALContext->Device->lpDS3DListener, flDistanceFactor, DS3D_IMMEDIATE);
		ALContext->Listener.update1 &= ~LDOPPLERVELOCITY;
		if (ALContext->Listener.update1 == 0)
			return;
	}

	if (ALContext->Listener.update1 & LSPEEDOFSOUND)
	{
		// Multiply the Listener and all Source velocities by 343.3 / SpeedOfSound
		flVelFactor = SPEEDOFSOUNDMETRESPERSEC / ALContext->flSpeedOfSound;

		// Listener
		Vel[0] = ALContext->Listener.Velocity[0] * flVelFactor;
		Vel[1] = ALContext->Listener.Velocity[1] * flVelFactor;
		Vel[2] = -ALContext->Listener.Velocity[2] * flVelFactor;
		IDirectSound3DListener_SetVelocity(ALContext->Device->lpDS3DListener, Vel[0], Vel[1], Vel[2], DS3D_IMMEDIATE);

		// Sources
		pSourceList = ALContext->Source;
		while (pSourceList)
		{
			if (pSourceList->uservalue2)
			{
				Vel[0] = pSourceList->vVelocity[0] * flVelFactor;
				Vel[1] = pSourceList->vVelocity[1] * flVelFactor;
				Vel[2] = -pSourceList->vVelocity[2] * flVelFactor;
				IDirectSound3DBuffer_SetVelocity((LPDIRECTSOUND3DBUFFER)pSourceList->uservalue2, Vel[0], Vel[1], Vel[2], DS3D_IMMEDIATE);
			}
			pSourceList = pSourceList->next;
		}

		ALContext->Listener.update1 &= ~LSPEEDOFSOUND;
		if (ALContext->Listener.update1 == 0)
			return;
	}

	if (ALContext->Listener.update1 & LDISTANCEMODEL)
	{
		SetDistanceModel(ALContext);
		ALContext->Listener.update1 &= ~LDISTANCEMODEL;
		if (ALContext->Listener.update1 == 0)
			return;
	}

	return;
}


/*
	EAXFix

	Used by "DirectSound3D" device to fix an Audigy & Live Driver bug
*/
void EAXFix(ALCcontext *context)
{
	ALuint alDummySource;
	ALboolean bEAX = AL_FALSE;

	if (context->alPrivateSource)
		return;

	// Generate a dummy Source
	alGenSources(1, &alDummySource);

	// Query for EAX 2.0 support (this function will make an EAX 2.0 Set() call if EAX 2.0 is detected)
	bEAX = alIsExtensionPresent((ALubyte*)"EAX2.0");

	if (bEAX)
	{
		// Need to generate Permanent Source
		alGenSources(1, &(context->alPrivateSource));
	}

	// Delete dummy Source
	alDeleteSources(1, &alDummySource);

	// Update Context Source variables
	if (context->alPrivateSource)
	{
		context->Source = NULL;
		context->SourceCount = 0;
	}
}

void InitializeManualAttenuation(ALCcontext *pContext)
{
	ALsource	*pSourceList;
	ALfloat		flRollOffFactor;
	ALuint		i;

#ifdef _DEBUG
	OutputDebugString("Initializing Manual Attenuation !\n");
#endif
		
	pContext->bUseManualAttenuation = AL_TRUE;

	// Set Global RollOff Factor to 0
	flRollOffFactor = 0.0f;
	IDirectSound3DListener_SetRolloffFactor (pContext->Device->lpDS3DListener, flRollOffFactor, DS3D_IMMEDIATE);

	// Calculate level for every Source
	pSourceList = pContext->Source;
	for (i = 0; i < pContext->SourceCount; i++)
	{
		// Calculate distance between Source and Listener
		if (pSourceList->SourceType == SOURCE2D)
		{
			pSourceList->flDistance = 0;
		}
		else if (pSourceList->bHeadRelative)
		{
			pSourceList->flDistance =
				(float)sqrt(sqr(pSourceList->vPosition[0]) +
							sqr(pSourceList->vPosition[1]) +
							sqr(pSourceList->vPosition[2]));
		}
		else
		{
			pSourceList->flDistance =
				(float)sqrt(sqr(pContext->Listener.Position[0] - pSourceList->vPosition[0]) +
							sqr(pContext->Listener.Position[1] - pSourceList->vPosition[1]) +
							sqr(pContext->Listener.Position[2] - pSourceList->vPosition[2]));
		}

		SetSourceLevel(pSourceList, LEVELFLAG_RECALCULATE_ATTENUATION | LEVELFLAG_FORCE_EAX_CALL);
		pSourceList = pSourceList->next;
	}
}

void SetDistanceModel(ALCcontext *pContext)
{
	ALsource	*pSourceList;
	ALfloat		flRollOffFactor;
	ALuint		i;

	if (pContext->bUseManualAttenuation)
	{
		pSourceList = pContext->Source;
		for (i = 0; i < pContext->SourceCount; i++)
		{
			SetSourceLevel(pSourceList, LEVELFLAG_RECALCULATE_ATTENUATION);
			pSourceList = pSourceList->next;
		}
	}
	else
	{
		if (pContext->DistanceModel == AL_INVERSE_DISTANCE_CLAMPED)
		{
			// Set Global RollOff Factor to 1.0
			flRollOffFactor = 1.0f;
			IDirectSound3DListener_SetRolloffFactor (pContext->Device->lpDS3DListener, flRollOffFactor, DS3D_IMMEDIATE);
		}
		else
		{
			// Has to be emulated manually
			InitializeManualAttenuation(pContext);
		}
	}
}

void SetSourceLevel(ALsource *pSource, ALuint ulFlags)
{
	ALCcontext	*pContext;

	pContext = alcGetCurrentContext();
	if (pContext)
	{
		if (pContext->bUseManualAttenuation)
		{
			if (pSource->uservalue1)
				SetNonEAXSourceLevels(pContext, pSource, ulFlags);
		}
		else if ((pSource) && ((pSource->flRollOffFactor != 1.0f) ||
			(pSource->flMinGain != 0.0f) ||
			(pSource->flMaxGain != 1.0f)))
		{
			InitializeManualAttenuation(pContext);
		}
	}
}

void SetNonEAXSourceLevels(ALCcontext *pContext, ALsource *pSource, ALuint ulFlags)
{
	long lVolume;
	float flDistance;

	if (ulFlags & LEVELFLAG_RECALCULATE_ATTENUATION)
	{
		if (pSource->SourceType == SOURCE3D)
		{
			// Get the Source->Listener Distance (and clamp if appropriate)
			switch (pContext->DistanceModel)
			{
			case AL_INVERSE_DISTANCE_CLAMPED:
			case AL_LINEAR_DISTANCE_CLAMPED:
			case AL_EXPONENT_DISTANCE_CLAMPED:
				flDistance = max(pSource->flDistance, pSource->flRefDistance);
				flDistance = min(flDistance, pSource->flMaxDistance);
				break;

			case AL_INVERSE_DISTANCE:
			case AL_LINEAR_DISTANCE:
			case AL_EXPONENT_DISTANCE:
				flDistance = pSource->flDistance;
				break;

			case AL_NONE:
			default:
				flDistance = 0.0f;
				break;
			}

			// Default is no attenuation
			pSource->lAttenuationVolume = 0;

			// Calculate attenuation based on Distance and DistanceModel
			switch (pContext->DistanceModel)
			{
			case AL_INVERSE_DISTANCE:
				if (pSource->flRefDistance > 0.0f)
				{
					if ((((flDistance - pSource->flRefDistance) / pSource->flRefDistance) * pSource->flRollOffFactor) > -1.0f)
						pSource->lAttenuationVolume = (ALint)(-2000 * log10(1.0f + (((flDistance - pSource->flRefDistance) / pSource->flRefDistance) * pSource->flRollOffFactor)));
					else
						pSource->lAttenuationVolume = 10000;
				}
				break;

			case AL_INVERSE_DISTANCE_CLAMPED:
				if ((pSource->flMaxDistance >= pSource->flRefDistance) && (pSource->flRefDistance > 0.0f))
				{
					if ((((flDistance - pSource->flRefDistance) / pSource->flRefDistance) * pSource->flRollOffFactor) > -1.0f)
						pSource->lAttenuationVolume = (ALint)(-2000 * log10(1.0f + (((flDistance - pSource->flRefDistance) / pSource->flRefDistance) * pSource->flRollOffFactor)));
					else
						pSource->lAttenuationVolume = 10000;
				}
				break;

			case AL_LINEAR_DISTANCE:
				if (pSource->flMaxDistance != pSource->flRefDistance)
					pSource->lAttenuationVolume = LinearGainToMB(1.0f - (pSource->flRollOffFactor * (flDistance - pSource->flRefDistance) / (pSource->flMaxDistance - pSource->flRefDistance)));
				break;

			case AL_LINEAR_DISTANCE_CLAMPED:
				// NOTE : For clamped models, Max Distance must be greater than or equal to Ref Distance, but in this case
				//        the values cannot be equal (otherwise a divide by zero will occur)
				if (pSource->flMaxDistance > pSource->flRefDistance)
					pSource->lAttenuationVolume = LinearGainToMB(1.0f - (pSource->flRollOffFactor * (flDistance - pSource->flRefDistance) / (pSource->flMaxDistance - pSource->flRefDistance)));
				break;

			case AL_EXPONENT_DISTANCE:
				if ((flDistance > 0.0f) && (pSource->flRefDistance > 0.0f))
					pSource->lAttenuationVolume = LinearGainToMB((ALfloat)pow(flDistance / pSource->flRefDistance, -pSource->flRollOffFactor));
				break;

			case AL_EXPONENT_DISTANCE_CLAMPED:
				if ((pSource->flMaxDistance >= pSource->flRefDistance) && (flDistance > 0.0f) && (pSource->flRefDistance > 0.0f))
					pSource->lAttenuationVolume = LinearGainToMB((ALfloat)pow(flDistance / pSource->flRefDistance, -pSource->flRollOffFactor));
				break;

			case AL_NONE:
			default:
				pSource->lAttenuationVolume = 0;
				break;
			}
		}
		else
		{
			pSource->lAttenuationVolume = 0;
		}
	}

	// Add Source's Volume (preserving a silent source)
	lVolume = (pSource->lVolume == -10000) ? -10000 : pSource->lVolume + pSource->lAttenuationVolume;

	// Clamp to Min/Max Volume
	lVolume = min(lVolume, pSource->lMaxVolume);
	lVolume = max(lVolume, pSource->lMinVolume);

	// Add Listener Volume
	lVolume += pContext->Listener.lVolume;

	// Clamp in range
	CLAMP(lVolume, -10000, 0)

	// Set if actually different by more than LEVEL_TOLERANCE
	if ((ulFlags & LEVELFLAG_FORCE_EAX_CALL) || (abs(lVolume - pSource->lFinalVolume) > LEVEL_TOLERANCE))
	{
		pSource->lFinalVolume = lVolume;
		IDirectSoundBuffer_SetVolume((LPDIRECTSOUNDBUFFER)pSource->uservalue1, lVolume);
	}
}


/*
	LinearGainToMB

	Helper function to convert a floating point amplitude (range 0.0 to 1.0) to Millibels
*/
ALint LinearGainToMB(float flGain)
{
	if (flGain > 0)
		return (long)(2000.0*log10(flGain));
	else
		return -10000;
}


/*
	GetMaxNumStereoBuffers
	
	Determine how many HARDWARE Stereo buffers can be successfully created
*/
ALuint GetMaxNumStereoBuffers(LPDIRECTSOUND lpDS)
{
	LPDIRECTSOUNDBUFFER lpDSB[MAX_NUM_SOURCES];
	DSBUFFERDESC DSBDescription;
	WAVEFORMATEX	OutputType;
	ALuint loop;
	ALuint numBuffers = 0;

	// Set Caps
	memset(&DSBDescription,0,sizeof(DSBUFFERDESC));
	DSBDescription.dwSize=sizeof(DSBUFFERDESC);
	DSBDescription.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_GLOBALFOCUS|
		DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_LOCHARDWARE;
	DSBDescription.dwBufferBytes=88200;
	DSBDescription.lpwfxFormat=&OutputType;
	memset(&OutputType,0,sizeof(WAVEFORMATEX));
	OutputType.wFormatTag=WAVE_FORMAT_PCM;
	OutputType.nChannels=2;
	OutputType.wBitsPerSample=16;
	OutputType.nBlockAlign=4;
	OutputType.nSamplesPerSec=44100;
	OutputType.nAvgBytesPerSec=176400;
	OutputType.cbSize=0;

	for (loop = 0; loop < MAX_NUM_SOURCES; loop++)
	{
		if (IDirectSound_CreateSoundBuffer(lpDS,&DSBDescription,&lpDSB[loop],NULL)==DS_OK)
			numBuffers++;
		else
			break;
	}

	// Release all the buffers
	for (loop = 0; loop < numBuffers; loop++)
	{
		if (lpDSB[loop])
		{
			IDirectSoundBuffer_Release(lpDSB[loop]);
			lpDSB[loop] = NULL;
		}
	}

	return numBuffers;
}


/*
	GetMaxNum3DMonoBuffers

	Determine how many HARDWARE mono 3D buffers can be successfully created
*/
ALuint GetMaxNum3DMonoBuffers(LPDIRECTSOUND lpDS)
{
	LPDIRECTSOUNDBUFFER lpDSB[MAX_NUM_SOURCES] = { NULL };
	LPDIRECTSOUND3DBUFFER lpDS3DB[MAX_NUM_SOURCES] = { NULL };
	DSBUFFERDESC DSBDescription;
	WAVEFORMATEX	OutputType;
	ALuint loop;
	ALuint numBuffers = 0;

	// Set Caps
	memset(&DSBDescription,0,sizeof(DSBUFFERDESC));
	DSBDescription.dwSize=sizeof(DSBUFFERDESC);
	DSBDescription.dwFlags=DSBCAPS_CTRL3D|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_GLOBALFOCUS|
		DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_LOCHARDWARE;
	DSBDescription.dwBufferBytes=88200;
	DSBDescription.lpwfxFormat=&OutputType;
	memset(&OutputType,0,sizeof(WAVEFORMATEX));
	OutputType.wFormatTag=WAVE_FORMAT_PCM;
	OutputType.nChannels=1;
	OutputType.wBitsPerSample=16;
	OutputType.nBlockAlign=2;
	OutputType.nSamplesPerSec=44100;
	OutputType.nAvgBytesPerSec=88200;
	OutputType.cbSize=0;
	
	for (loop = 0; loop < MAX_NUM_SOURCES; loop++)
	{
		if (IDirectSound_CreateSoundBuffer(lpDS,&DSBDescription,&lpDSB[loop],NULL)==DS_OK)
		{
			if (IDirectSoundBuffer_QueryInterface(lpDSB[loop], &IID_IDirectSound3DBuffer, &lpDS3DB[loop])==DS_OK)
			{
				numBuffers++;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	// Release all the buffers
	for (loop = 0; loop < numBuffers; loop++)
	{
		if (lpDS3DB[loop])
		{
			IDirectSound3DBuffer_Release(lpDS3DB[loop]);
			lpDS3DB[loop] = NULL;
		}

		if (lpDSB[loop])
		{
			IDirectSoundBuffer_Release(lpDSB[loop]);
			lpDSB[loop] = NULL;
		}
	}

	return numBuffers;
}
