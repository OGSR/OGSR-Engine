#pragma once

#include "../CustomHUD.h"
#include "HitMarker.h"
#include "UI.h"

class CContextMenu;
class CHUDTarget;

struct CFontManager :public pureDeviceReset			{
							CFontManager			();
							~CFontManager			();

	typedef xr_vector<CGameFont**>					FONTS_VEC;
	typedef FONTS_VEC::iterator						FONTS_VEC_IT;
	FONTS_VEC				m_all_fonts;
	void					Render					();

	// hud font
	CGameFont*				pFontMedium;
	CGameFont*				pFontDI;

	CGameFont*				pFontArial14;
	CGameFont*				pFontGraffiti19Russian;
	CGameFont*				pFontGraffiti22Russian;
	CGameFont*				pFontLetterica16Russian;
	CGameFont*				pFontLetterica18Russian;
	CGameFont*				pFontGraffiti32Russian;
	CGameFont*				pFontGraffiti50Russian;
	CGameFont*				pFontLetterica25;
	CGameFont*				pFontStat;

	void					InitializeFonts			();
	void					InitializeFont			(CGameFont*& F, LPCSTR section, u32 flags = 0);
	LPCSTR					GetFontTexName			(LPCSTR section);				

	virtual void			OnDeviceReset			();
};

class CHUDManager :
	public CCustomHUD
{
	friend class CUI;
private:
	CUI*					pUI;
	CHitMarker				HitMarker;
	CHUDTarget*				m_pHUDTarget;
	bool					b_online;
public:
							CHUDManager			();
	virtual					~CHUDManager		();
	virtual		void		OnEvent				(EVENT E, u64 P1, u64 P2);

	virtual		void		Load				();
	
	virtual		void		Render_First		();
	virtual		void		Render_Last			();	   
	virtual		void		Render_Actor_Shadow	();	// added by KD
	virtual		void		OnFrame				();

	virtual		void		RenderUI			();

	virtual		IC CUI*		GetUI				(){return pUI;}

				void		Hit					(int idx, float power, const Fvector& dir);
	CFontManager&			Font				()							{return *(UI()->Font());}
	//текущий предмет на который смотрит HUD
	collide::rq_result&		GetCurrentRayQuery	();


	//устанвка внешнего вида прицела в зависимости от текущей дисперсии
	void					SetCrosshairDisp	(float dispf, float disps = 0.f);
	void					ShowCrosshair		(bool show);

	void					SetHitmarkType		(LPCSTR tex_name);
	virtual void			OnScreenRatioChanged();
	virtual void			OnDisconnected		();
	virtual void			OnConnected			();
	virtual void			net_Relcase			(CObject *object);
	CHUDTarget				*GetTarget			()	const { return m_pHUDTarget; };
};
