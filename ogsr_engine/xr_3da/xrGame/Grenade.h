#pragma once
#include "missile.h"
#include "explosive.h"
#include "../feel_touch.h"

#define SND_RIC_COUNT 5

class CGrenade :
	public CMissile,
	public CExplosive
{
	typedef CMissile		inherited;
public:
							CGrenade							();
	virtual					~CGrenade							();


	virtual void			Load								(LPCSTR section);
	
	virtual BOOL 			net_Spawn							(CSE_Abstract* DC);
	virtual void 			net_Destroy							();
	virtual void 			net_Relcase							(CObject* O );

	virtual void 			OnH_B_Independent					(bool just_before_destroy);
	virtual void 			OnH_A_Independent					();
	virtual void 			OnH_A_Chield						();
	
	virtual void 			OnEvent								(NET_Packet& P, u16 type);
	
	virtual void 			OnAnimationEnd						(u32 state);
	virtual void 			UpdateCL							();

	virtual void 			Throw();
	virtual void 			Destroy();

	
	virtual bool			Action								(s32 cmd, u32 flags);
	virtual bool			Useful								() const;
	virtual void			State								(u32 state);

	virtual void			OnH_B_Chield						()				{inherited::OnH_B_Chield();}

	virtual	void			Hit									(SHit* pHDS);

	virtual bool			NeedToDestroyObject					() const; 
	virtual ALife::_TIME_ID	TimePassedAfterIndependant			() const;

			void			PutNextToSlot						();

	virtual void			Deactivate							();
	virtual void			GetBriefInfo						(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count);
protected:
	ALife::_TIME_ID			m_dwGrenadeRemoveTime;
	ALife::_TIME_ID			m_dwGrenadeIndependencyTime;
protected:
	HUD_SOUND				sndCheckout;
	ESoundTypes				m_eSoundCheckout;
private:
	float					m_grenade_detonation_threshold_hit;
	bool					m_thrown;
protected:
	virtual	void			UpdateXForm							()		{ CMissile::UpdateXForm(); };
public:

	virtual BOOL			UsedAI_Locations					();
	virtual CExplosive		*cast_explosive						()	{return this;}
	virtual CMissile		*cast_missile						()	{return this;}
	virtual CHudItem		*cast_hud_item						()	{return this;}
	virtual CGameObject		*cast_game_object					()	{return this;}
	virtual IDamageSource	*cast_IDamageSource					()	{return CExplosive::cast_IDamageSource();}
};
