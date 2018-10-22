//////////////////////////////////////////////////////////////////////
// HudSound.h:		структура для работы со звуками применяемыми в 
//					HUD-объектах (обычные звуки, но с доп. параметрами)
//////////////////////////////////////////////////////////////////////

#pragma once


struct HUD_SOUND
{
	HUD_SOUND()		{ m_activeSnd=NULL; }
	~HUD_SOUND()	{ m_activeSnd=NULL; }

	////////////////////////////////////
	// работа со звуками
	/////////////////////////////////////
	static void		LoadSound		(	LPCSTR section, LPCSTR line,
		ref_sound& hud_snd,
		int type = sg_SourceType,
		float* volume = NULL,
		float* delay = NULL);

	static void		LoadSound		(	LPCSTR section, LPCSTR line,
		HUD_SOUND& hud_snd,  int type = sg_SourceType);

	static void		DestroySound	(	HUD_SOUND& hud_snd);

	static void		PlaySound		(	HUD_SOUND& snd,
									const Fvector& position,
									const CObject* parent,
									bool hud_mode,
									bool looped = false);

	static void		StopSound		(	HUD_SOUND& snd);

	ICF BOOL		playing			()
	{
		if (m_activeSnd)	return	m_activeSnd->snd._feedback()?TRUE:FALSE;
		else				return	FALSE;
	}

	ICF void		set_position	(const Fvector& pos)
	{
		if(m_activeSnd)	{ 
			if (m_activeSnd->snd._feedback()&&!m_activeSnd->snd._feedback()->is_2D())	
									m_activeSnd->snd.set_position	(pos);
			else					m_activeSnd	= NULL;
		}
	}

	struct SSnd		{
		ref_sound	snd;
		float		delay;		//задержка перед проигрыванием
		float		volume;		//громкость
	};
	SSnd*			m_activeSnd;
	xr_vector<SSnd> sounds;
};

