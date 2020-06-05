#pragma once

#include "../../../xr_3da/SkeletonMotions.h"
#include "../../../Include\xrRender\Kinematics.h"
#include "../../../Include\xrRender\animation_motion.h"
#include "../../../Include\xrRender\animation_blend.h"

class CAI_Trader;

namespace MonsterSpace {
	enum EMonsterHeadAnimType;
};

class CTraderAnimation {
	CAI_Trader			*m_trader;

	LPCSTR				m_anim_global;
	LPCSTR				m_anim_head;
	
	MotionID			m_motion_head;	
	MotionID			m_motion_global;

	ref_sound			*m_sound;

	bool				m_external_sound;

public:
					CTraderAnimation		(CAI_Trader *trader) : m_trader(trader) {}

	void			reinit					();

	void			set_animation			(LPCSTR anim);
	void			set_head_animation		(LPCSTR anim);
	void			set_sound				(LPCSTR sound, LPCSTR head_anim);

	// Callbacks
	static void		global_callback			(CBlend* B);
	static void		head_callback			(CBlend* B);

	void			update_frame			();

	void			external_sound_start	(LPCSTR phrase);
	void			external_sound_stop		();

private:
	void			remove_sound			();	


};

