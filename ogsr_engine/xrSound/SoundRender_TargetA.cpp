#include "stdafx.h"


#include <efx.h>
#include "soundrender_TargetA.h"
#include "soundrender_emitter.h"
#include "soundrender_source.h"

xr_vector<u8> g_target_temp_data;

CSoundRender_TargetA::CSoundRender_TargetA():CSoundRender_Target()
{
    cache_gain			= 0.f;
    cache_pitch			= 1.f;
    pSource				= 0;
    efx_env_slot = AL_EFFECTSLOT_NULL;
}

CSoundRender_TargetA::~CSoundRender_TargetA()
{
}

BOOL	CSoundRender_TargetA::_initialize		()
{
	inherited::_initialize();
    // initialize buffer
    alGetError();
	A_CHK(alGenBuffers	(sdef_target_count, pBuffers));	
    if ( ALenum error = alGetError(); error != AL_NO_ERROR ) {
      Msg( "!![%s] alGenBuffers: %s", __FUNCTION__, alGetString( error ) );
      return FALSE;
    }
    alGenSources		(1, &pSource);
    ALenum error		= alGetError();
    if (AL_NO_ERROR==error){
        A_CHK(alSourcei	(pSource, AL_LOOPING, AL_FALSE));
        A_CHK(alSourcef	(pSource, AL_MIN_GAIN, 0.f));
        A_CHK(alSourcef	(pSource, AL_MAX_GAIN, 1.f));
        A_CHK(alSourcef	(pSource, AL_GAIN, 	cache_gain));
        A_CHK(alSourcef	(pSource, AL_PITCH,	cache_pitch));
        return			TRUE;
    }else{
    	Msg				("! [OpenAL] Can't create source. Error: [%s]",(LPCSTR)alGetString(error));
        return 			FALSE;
    }
}


void CSoundRender_TargetA::alAuxInit( ALuint slot ) {
  if ( slot != efx_env_slot )
    A_CHK( alSource3i( pSource, AL_AUXILIARY_SEND_FILTER, slot, 0, AL_FILTER_NULL ) );
    efx_env_slot = slot;
}


void	CSoundRender_TargetA::_destroy		()
{
	// clean up target
	if (alIsSource(pSource))	alDeleteSources	(1, &pSource);
	A_CHK(alDeleteBuffers		(sdef_target_count, pBuffers));
}

void	CSoundRender_TargetA::start			(CSoundRender_Emitter* E)
{
    inherited::start(E);

	// Calc storage
	buf_block		= sdef_target_block*wfx.nAvgBytesPerSec/1000;
    g_target_temp_data.resize(buf_block);
}

void	CSoundRender_TargetA::render		()
{
	alGetError();
	for (u32 buf_idx=0; buf_idx<sdef_target_count; buf_idx++)
		fill_block	(pBuffers[buf_idx]);
        if ( ALenum error = alGetError(); error != AL_NO_ERROR ) {
          Msg( "!![%s] alBufferData: %s", __FUNCTION__, alGetString( error ) );
          return;
        }

	A_CHK			(alSourceQueueBuffers	(pSource, sdef_target_count, pBuffers));	
	A_CHK			(alSourcePlay			(pSource));
        if ( ALenum error = alGetError(); error != AL_NO_ERROR ) {
          Msg( "!![%s] alSourcePlay: %s", __FUNCTION__, alGetString( error ) );
          return;
        }

    inherited::render();
}

void	CSoundRender_TargetA::stop			()
{
	if (rendering)
	{
		A_CHK		(alSourceStop(pSource));
		A_CHK		(alSourcei	(pSource, AL_BUFFER,   NULL));
		A_CHK		(alSourcei	(pSource, AL_SOURCE_RELATIVE,	TRUE));
		alAuxInit( AL_EFFECTSLOT_NULL );
	}
    inherited::stop	();
}

void	CSoundRender_TargetA::rewind			()
{
	inherited::rewind();

	alGetError();
	A_CHK			(alSourceStop(pSource));
	A_CHK			(alSourcei	(pSource, AL_BUFFER,   NULL));
        if ( ALenum error = alGetError(); error != AL_NO_ERROR ) {
          Msg( "!![%s] alSourceStop: %s", __FUNCTION__, alGetString( error ) );
          return;
        }
	for (u32 buf_idx=0; buf_idx<sdef_target_count; buf_idx++)
		fill_block	(pBuffers[buf_idx]);
        if ( ALenum error = alGetError(); error != AL_NO_ERROR ) {
          Msg( "!![%s] alBufferData: %s", __FUNCTION__, alGetString( error ) );
          return;
        }
	A_CHK			(alSourceQueueBuffers	(pSource, sdef_target_count, pBuffers));	
	A_CHK			(alSourcePlay			(pSource));
        if ( ALenum error = alGetError(); error != AL_NO_ERROR )
          Msg( "!![%s] alSourcePlay: %s", __FUNCTION__, alGetString( error ) );
}

void CSoundRender_TargetA::update()
{
	inherited::update();

	ALint processed, state;

	/* Get relevant source info */
	alGetError();
	alGetSourcei(pSource, AL_SOURCE_STATE, &state);
	ALenum err = alGetError();
	if ( err != AL_NO_ERROR ) {
	  Msg( "!![%s] Error checking source state: %s", __FUNCTION__, alGetString( err ) );
	  return;
	}
	alGetSourcei(pSource, AL_BUFFERS_PROCESSED, &processed);
	err = alGetError();
	if ( err != AL_NO_ERROR ) {
	  Msg( "!![%s] Error checking buffers processed: %s", __FUNCTION__, alGetString( err ) );
	  return;
	}

	while (processed > 0)
	{
		ALuint BufferID;
		A_CHK(alSourceUnqueueBuffers(pSource, 1, &BufferID));
		fill_block(BufferID);
		A_CHK(alSourceQueueBuffers(pSource, 1, &BufferID));
		processed--;
		if (alGetError() != AL_NO_ERROR)
		{
			Msg("!![%s]Error buffering data", __FUNCTION__);
			return;
		}
	}

	/* Make sure the source hasn't underrun */
	if (state != AL_PLAYING && state != AL_PAUSED)
	{
		ALint queued;

		/* If no buffers are queued, playback is finished */
		alGetSourcei(pSource, AL_BUFFERS_QUEUED, &queued);
		if (queued == 0)
			return;

		alSourcePlay(pSource);
		if (alGetError() != AL_NO_ERROR)
		{
			Msg("!![%s]Error restarting playback", __FUNCTION__);
			return;
		}
	}
}

void	CSoundRender_TargetA::fill_parameters()
{
#ifdef DEBUG
	CSoundRender_Emitter* SE = pEmitter; VERIFY(SE);
#endif
	inherited::fill_parameters();
	alGetError();

    // 3D params
	VERIFY2(pEmitter,SE->source->file_name());
    A_CHK(alSourcef	(pSource, AL_REFERENCE_DISTANCE, 	pEmitter->p_source.min_distance));

	VERIFY2(pEmitter,SE->source->file_name());
    A_CHK(alSourcef	(pSource, AL_MAX_DISTANCE, 			pEmitter->p_source.max_distance));

	VERIFY2(pEmitter,SE->source->file_name                                       ());
	A_CHK(alSource3f(pSource, AL_POSITION,	 			pEmitter->p_source.position.x,pEmitter->p_source.position.y,-pEmitter->p_source.position.z));

#ifdef SND_DOPPLER_EFFECT
	VERIFY2(pEmitter, SE->source->file_name());
	A_CHK(alSource3f(pSource, AL_VELOCITY, pEmitter->p_source.velocity.x, pEmitter->p_source.velocity.y, -pEmitter->p_source.velocity.z));
	//A_CHK(alSource3f(pSource, AL_VELOCITY, 0.,0.,0.));
#endif

	VERIFY2(pEmitter,SE->source->file_name());
    A_CHK(alSourcei	(pSource, AL_SOURCE_RELATIVE,		pEmitter->b2D));

	A_CHK(alSourcef	(pSource, AL_ROLLOFF_FACTOR,		psSoundRolloff));

	VERIFY2(pEmitter,SE->source->file_name());
    float	_gain	= pEmitter->smooth_volume;			clamp	(_gain,EPS_S,1.f);
    if (!fsimilar(_gain,cache_gain)){
        cache_gain	= _gain;
        A_CHK(alSourcef	(pSource, AL_GAIN,				_gain));
    }

	VERIFY2(pEmitter,SE->source->file_name());
    float	_pitch	= pEmitter->p_source.freq;			clamp	(_pitch,EPS_L,2.f);
    if (!fsimilar(_pitch,cache_pitch)){
        cache_pitch	= _pitch;
        A_CHK(alSourcef	(pSource, AL_PITCH,				_pitch));
    }
	VERIFY2(pEmitter,SE->source->file_name());

    if ( ALenum error = alGetError(); error != AL_NO_ERROR )
      Msg( "!![%s]: %s", __FUNCTION__, alGetString( error ) );
}

void	CSoundRender_TargetA::fill_block	(ALuint BufferID)
{
#pragma todo("check why pEmitter is NULL")
	if (0==pEmitter)	return;

	pEmitter->fill_block(&g_target_temp_data.front(),buf_block);

	ALuint format 		= (wfx.nChannels==1)?AL_FORMAT_MONO16:AL_FORMAT_STEREO16;
    A_CHK				(alBufferData(BufferID, format, &g_target_temp_data.front(), buf_block, wfx.nSamplesPerSec));
}
