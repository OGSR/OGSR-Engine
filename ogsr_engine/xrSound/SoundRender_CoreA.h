#pragma once

#include "SoundRender_Core.h"            

#include "OpenALDeviceList.h"

#include <eax.h>

#if 0//def DEBUG //KRodin: оно вот совсем не нужно, ну правда. ќшибки-то могут быть, тем более после обновлени€ ќпенјЋ - это не фатально всЄ.
#	define A_CHK(expr)		{ alGetError(); 		expr; ALenum error=alGetError(); 			VERIFY2(error==AL_NO_ERROR, (LPCSTR)alGetString(error)); }
#	define AC_CHK(expr)		{ alcGetError(pDevice); expr; ALCenum error=alcGetError(pDevice); 	VERIFY2(error==ALC_NO_ERROR,(LPCSTR)alcGetString(pDevice,error)); }
#else
#	define A_CHK(expr)		{ expr; }
#	define AC_CHK(expr)		{ expr; }
#endif

class CSoundRender_CoreA: public CSoundRender_Core
{
	typedef CSoundRender_Core inherited;
	EAXSet					eaxSet;					// EAXSet function, retrieved if EAX Extension is supported
	EAXGet					eaxGet;					// EAXGet function, retrieved if EAX Extension is supported
	ALCdevice* 				pDevice;
    ALCcontext*				pContext;
	ALDeviceList*			pDeviceList;

	struct SListener{
		Fvector				position;
		Fvector				orientation[2];
#ifdef SND_DOPPLER_EFFECT
		Fvector                         curVelocity;
		Fvector                         prevVelocity;
		Fvector                         accVelocity;
#endif
	};
	SListener				Listener;

    BOOL 					EAXQuerySupport			(BOOL bDeferred, const GUID* guid, u32 prop, void* val, u32 sz);
	BOOL 					EAXTestSupport			(BOOL bDeferred);
protected:
	virtual void			i_eax_set				(const GUID* guid, u32 prop, void* val, u32 sz);
	virtual void			i_eax_get				(const GUID* guid, u32 prop, void* val, u32 sz);
	virtual void			update_listener			( const Fvector& P, const Fvector& D, const Fvector& N, float dt );
public:	
						    CSoundRender_CoreA		();
    virtual					~CSoundRender_CoreA		();

	virtual void			_initialize				( u64 window );
	virtual void			_clear					( );
    
	virtual void			set_master_volume		( float f		);

	virtual const Fvector&	listener_position		( ){return Listener.position;}
};
extern CSoundRender_CoreA* SoundRenderA;
