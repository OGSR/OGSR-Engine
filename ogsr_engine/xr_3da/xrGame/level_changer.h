////////////////////////////////////////////////////////////////////////////
//	Module 		: level_changer.h
//	Created 	: 10.07.2003
//  Modified 	: 10.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "../feel_touch.h"
#include "game_graph_space.h"

class CLevelChanger : public CGameObject, public Feel::Touch {
private:
	typedef	CGameObject	inherited;

private:
	GameGraph::_GRAPH_ID	m_game_vertex_id;
	u32						m_level_vertex_id;
	Fvector					m_position;
	Fvector					m_angles;
	float					m_entrance_time;
	void					update_actor_invitation	();
	bool					m_bSilentMode;
	bool					get_reject_pos(Fvector& p, Fvector& r);
public:
	virtual				~CLevelChanger		();
	virtual BOOL		net_Spawn			(CSE_Abstract* DC);
	virtual void		net_Destroy			();
	virtual void		Center				(Fvector& C) const;
	virtual float		Radius				() const;
	virtual void		shedule_Update		(u32 dt);
	virtual void		feel_touch_new		(CObject* O);
	virtual BOOL		feel_touch_contact	(CObject* O);

	virtual bool		IsVisibleForZones() { return false;		}
};
