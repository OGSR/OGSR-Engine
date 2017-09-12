#ifndef _AL_SOURCE_H_
#define _AL_SOURCE_H_

#define AL_NUM_SOURCE_PARAMS	128

#include "AL/al.h"

#include "eax.h"

#ifdef __cplusplus
extern "C" {
#endif

// Flags indicating what Direct Sound parameters need to be updated in the UpdateContext call
#define VOLUME					1
#define FREQUENCY				2
#define POSITION				4
#define VELOCITY				8
#define MODE					16
#define MINDIST					32
#define MAXDIST					64
#define ORIENTATION				128
#define CONEANGLES				256
#define CONEOUTSIDEVOLUME		512
#define LOOPED					1024
#define STATE					2048
#define ROLLOFFFACTOR			4096
#define SDELETE					8192
#define SGENERATESOURCE			16384
#define SQUEUE					32768
#define SUNQUEUE				65536
#define MINGAIN					131072
#define MAXGAIN					262144
#define OFFSET					524288

#define ALSOURCE		1
#define ALLISTENER		2
#define ALBUFFER		3

#define SOURCE3D		0
#define SOURCE2D		1

typedef struct ALbufferlistitem
{
    ALuint              buffer;
	ALuint				bufferstate;
	ALuint				flag;
	struct ALbufferlistitem	*next;
} ALbufferlistitem;

typedef struct ALsource_struct
{
	ALfloat		flPitch;
	ALfloat		flGain;
	ALfloat		flOuterGain;
	ALfloat		flMinGain;
	ALfloat		flMaxGain;
	ALfloat		flInnerAngle;
	ALfloat		flOuterAngle;
	ALfloat		flRefDistance;
	ALfloat		flMaxDistance;
	ALfloat		flRollOffFactor;
	ALfloat		vPosition[3];
	ALfloat		vVelocity[3];
	ALfloat		vOrientation[3];
	ALboolean	bHeadRelative;
	ALboolean	bLooping;

	ALuint		ulBufferID;

	ALboolean	inuse;
	ALboolean	play;
	ALenum		state;
	ALuint		position;
	ALuint		position_fraction;
	struct ALbufferlistitem	*queue;		// Linked list of buffers in queue
	ALuint		BuffersInQueue;			// Number of buffers in queue
	ALuint		BuffersProcessed;		// Number of buffers already processed (played)
	union
	{
		ALuint		SizeOfBufferDataAddedToQueue;	// Total size of data added to queue by alSourceQueueBuffers
		ALuint		SizeOfBufferDataRemovedFromQueue;	// Total size of data removed from queue by alSourceUnqueueBuffers
	};
	union
	{
		ALuint		NumBuffersAddedToQueue;		// Number of buffers added to queue by alSourceQueueBuffers
		ALuint		NumBuffersRemovedFromQueue;	// Number of buffers removed from queue by alSourceUnqueueBuffers
	};
	ALuint		TotalBufferDataSize;	// Total amount of data contained in the buffers queued for this source
	ALuint		BuffersAddedToDSBuffer;	// Number of buffers whose data has been copied into DS buffer
	ALuint		update1;				// Store changes that need to be made in alUpdateContext
	ALvoid		*uservalue1;
	ALvoid		*uservalue2;
	ALvoid		*uservalue3;
	ALuint		SourceType;				// Stores type of Source (SOURCE3D or SOURCE2D)
	ALuint		BufferPosition;			// Read position in audio data of current buffer
	ALboolean	FinishedQueue;			// Indicates if all the buffer data has been copied to the source
	ALuint		OldPlayCursor;			// Previous position of Play Cursor
	ALuint		OldWriteCursor;			// Previous position of the Write Cursor
	ALuint		SilenceAdded;			// Number of bytes of silence added to buffer
	ALfloat		BufferDuration;			// Length in seconds of the DS circular buffer
	ALuint		OldTime;				// Last time Source was serviced by timer

	ALuint		DSFrequency;			// Frequency of DS Buffer
	ALuint		Silence;				// Bytes of silence added to a Paused or Stopped source

    ALuint      source;                 // Index to itself

	ALuint		CurrentState;
	ALboolean	DSBufferPlaying;

	ALint		lBytesPlayed;

	ALint		lOffset;
	ALint		lOffsetType;

	ALint		lSourceType;			// Source Type (Static, Streaming, or Undetermined)

	ALfloat		flDistance;				// Distance of Source from Listener
	ALint		lAttenuationVolume;		// Attenuation from Distance (mB)
	ALint		lVolume;				// Source volume (mB) (converted from Gain)
	ALint		lMaxVolume;				// Source Max volume (mB) (converted from Max Gain)
	ALint		lMinVolume;				// Source Min volume (mB) (converted from Min Gain)
	ALint		lFinalVolume;			// Last Volume actually set
	EAXBUFFERPROPERTIES EAX20BP;

	struct ALsource_struct *previous;
	struct ALsource_struct *next;
} ALsource;

#ifdef __cplusplus
}
#endif

#endif
