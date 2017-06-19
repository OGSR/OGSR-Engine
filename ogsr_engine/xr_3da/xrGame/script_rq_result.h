#pragma once

#include "script_export_space.h"

class script_rq_result
{
	float				range;
	CScriptGameObject	 *object;

public:
	script_rq_result						()							{ object = nullptr; range = 0.f; }
	void					set_result		(collide::rq_result _res);
/*	IC CScriptGameObject	*get_object		()		const				{ return object; };
	IC float				get_range		()		const				{ return range; };*/

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(script_rq_result)
#undef script_type_list
#define script_type_list save_type_list(script_rq_result)