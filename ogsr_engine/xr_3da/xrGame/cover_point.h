////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_point.h
//	Created 	: 24.03.2004
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover point class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

class CCoverPoint {
public:
	Fvector				m_position;
	u32					m_level_vertex_id;

public:
	IC					CCoverPoint		(const Fvector &point, u32 level_vertex_id);
	IC	const Fvector	&position		() const;
	IC	u32				level_vertex_id	() const;
	IC	bool			operator==		(const CCoverPoint &point) const;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CCoverPoint)
#undef script_type_list
#define script_type_list save_type_list(CCoverPoint)

#include "cover_point_inline.h"