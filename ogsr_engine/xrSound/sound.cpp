#include "stdafx.h"
#pragma hdrstop

#include "SoundRender_CoreA.h"
#include "SoundRender_CoreD.h"

void CSound_manager_interface::_create		(u64 window)
{
	if (strstr( Core.Params,"-dsound"))
	{
		SoundRenderD	= xr_new<CSoundRender_CoreD>();
		SoundRender		= SoundRenderD;
		Sound			= SoundRender;
	}else{
		SoundRenderA	= xr_new<CSoundRender_CoreA>();
		SoundRender		= SoundRenderA;
		Sound			= SoundRender;
	}
	if (strstr			( Core.Params,"-nosound")){
		SoundRender->bPresent = FALSE;
		return;
	}
	Sound->_initialize	(window);
}

void CSound_manager_interface::_destroy	()
{
	Sound->_clear		();
    xr_delete			(SoundRender);
    Sound				= 0;
}

