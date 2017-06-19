#pragma once

#include "render.h"

//////////////////////////////////////////////////////////////////////////
// definition ("Renderable")
class	ENGINE_API	IRenderable			{
public:
	struct 
	{
		Fmatrix							xform						;
		IRender_Visual*					visual						;
		IRender_ObjectSpecific*			pROS						;
		BOOL							pROS_Allowed				;
	}	renderable;
public:
										IRenderable					();
	virtual								~IRenderable				();
	IRender_ObjectSpecific*				renderable_ROS				()	;
	virtual	void						renderable_Render			()	= 0;
	virtual	BOOL						renderable_ShadowGenerate	()	{ return FALSE; };
	virtual	BOOL						renderable_ShadowReceive	()	{ return FALSE; };
};
