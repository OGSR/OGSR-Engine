// icon_params.h
#pragma once



class CIconParams
{
protected:
	bool			loaded;
public:
	int				icon_group;
	float			grid_x;
	float			grid_y;
	float			grid_width;
	float			grid_height;
	bool			show_condition;
	shared_str		name;
	shared_str		section;

	CIconParams()  
	{
		loaded = false;
		icon_group = 0;
		grid_x = grid_y = 0.f;
		grid_width = grid_height = 0.f;
		show_condition = false;
	}
	CIconParams				(const shared_str section)  { Load(section); }

	void					Load(const shared_str item_section);

	ref_shader&				get_shader()		const;
	Frect&					original_rect()		const;  // used static variable!

};