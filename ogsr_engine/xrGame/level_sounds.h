//---------------------------------------------------------------------------
#ifndef LevelSoundsH
#define LevelSoundsH
#pragma once

struct SStaticSound
{
	ref_sound		m_Source;
	Ivector2		m_ActiveTime;
	Ivector2		m_PlayTime;
	Ivector2		m_PauseTime;
	u32				m_NextTime;
	u32				m_StopTime;
	Fvector			m_Position;
	float			m_Volume;
	float			m_Freq;
public:
	void 			Load			(IReader& F);
	void 			LoadIni			(CInifile::Sect& section);
	void 			Update			(u32 gt, u32 rt);
};

// music interface
struct	SMusicTrack
{
#ifdef DEBUG
	shared_str		m_DbgName;
#endif
	ref_sound		m_SourceLeft;
	ref_sound		m_SourceRight;
	Ivector2		m_ActiveTime;
	Ivector2		m_PauseTime;
	float			m_Volume;
public:
	void			Load			(LPCSTR fn, LPCSTR params);
	BOOL			IsPlaying		(){return m_SourceLeft._feedback() || m_SourceRight._feedback();}
	void			Play			();
	void			Stop			();
	void			SetVolume		(float volume);
};

class CLevelSoundManager
{
	DEFINE_VECTOR(SStaticSound,StaticSoundsVec,StaticSoundsVecIt);
	StaticSoundsVec	m_StaticSounds;
	DEFINE_VECTOR(SMusicTrack,MusicTrackVec,MusicTrackVecIt);
	MusicTrackVec	m_MusicTracks;
	u32				m_NextTrackTime;
	int				m_CurrentTrack{};
public:
					CLevelSoundManager();
	void			Load			();
	void			Unload			();
	void Update();
};

#endif