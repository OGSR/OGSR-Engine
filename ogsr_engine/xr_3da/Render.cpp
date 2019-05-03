#include "stdafx.h"
#include "render.h"

IRender_interface::~IRender_interface()		{};

//ENGINE_API	IRender_interface*	Render		= NULL;

// resources
IRender_Light::~IRender_Light	()			{	
	::Render->light_destroy		(this	);
}
IRender_Glow::~IRender_Glow		()			{	
	::Render->glow_destroy		(this	);	
}

ENGINE_API Fvector4 w_states = { 0,0,0,1 };
ENGINE_API Fvector3 w_timers = { 0,0,0 };
ENGINE_API ShExports shader_exports = {};
