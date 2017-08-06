#pragma once

#include "light.h"
#include "light_package.h"

class	CLight_DB
{
private:
	xr_vector<ref_light>	v_static;
public:
	ref_light				sun_original;
	ref_light				sun_adapted;
	light_Package			package;
public:
	void					add_light			(light*		L	);

	void					Load				(IReader*	fs	);
	void					Unload				();

	light*					Create				();
	void					Update				();

	CLight_DB				();
	~CLight_DB				();
};
