#include "stdafx.h"

#ifdef DEBUG
	ECORE_API BOOL bDebug	= FALSE;
#endif

// Video
//. u32			psCurrentMode		= 1024;
u32			psCurrentVidMode[2] = {1024,768};
u32			psCurrentBPP		= 32;
// release version always has "mt_*" enabled
Flags32		psDeviceFlags = { rsDetails | rsDrawStatic | rsDrawDynamic | rsRefresh60hz | rsFullscreen };

// textures 
int			psTextureLOD		= 0;
