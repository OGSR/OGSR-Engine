#ifndef SoundRenderH
#define SoundRenderH
#pragma once

#include "sound.h"

class CSoundRender_Core;
class CSoundRender_Source;
class CSoundRender_Emitter;
class CSoundRender_EmitterParams;
class CSoundRender_Target;
class CSoundRender_Environment;
class SoundEnvironment_LIB;

const 	u32 	sdef_target_count		= 3;									//
const	u32		sdef_target_block		= 400;									// ms
const	u32		sdef_target_size		= sdef_target_count*sdef_target_block;	// ms
const	u32		sdef_source_footer		= 0;									// ms of silence after buffer data, just for rendering
const	u32		sdef_env_version		= 4;									// current version of env-def
const	u32		sdef_level_version		= 1;									// current version of level-def
const	u32		sdef_event_pulse		= 500;									// ms

#endif