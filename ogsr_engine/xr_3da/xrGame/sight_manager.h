////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sight manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "setup_manager.h"
#include "sight_control_action.h"

namespace LevelGraph {
	class CVertex;
};

class CAI_Stalker;

class CSightManager : public CSetupManager<CSightControlAction,CAI_Stalker,u32> {
public:
	typedef CSetupManager<CSightControlAction,CAI_Stalker,u32> inherited;

private:
	bool			m_enabled;
	bool			m_turning_in_place;
	float			m_max_left_angle;
	float			m_max_right_angle;

public:
					CSightManager						(CAI_Stalker *object);
	virtual			~CSightManager						();
	virtual	void	Load								(LPCSTR section);
	virtual	void	reinit								();
	virtual	void	reload								(LPCSTR section);
			void	remove_links						(CObject *object);
			void	Exec_Look							(float dt);
			bool	bfIf_I_SeePosition					(Fvector tPosition) const;
			void	SetPointLookAngles					(const Fvector &tPosition, float &yaw, float &pitch, const CGameObject *object = 0);
			void	SetFirePointLookAngles				(const Fvector &tPosition, float &yaw, float &pitch, const CGameObject *object = 0);
			void	SetDirectionLook					();
			void	SetLessCoverLook					(const LevelGraph::CVertex *tpNode, bool bDifferenceLook);
			void	SetLessCoverLook					(const LevelGraph::CVertex *tpNode, float fMaxHeadTurnAngle, bool bDifferenceLook);
			void	vfValidateAngleDependency			(float x1, float &x2, float x3);
			bool	need_correction						(float x1, float x2, float x3);
	IC		bool	GetDirectionAnglesByPrevPositions	(float &yaw, float &pitch);
			bool	GetDirectionAngles					(float &yaw, float &pitch);
	IC		bool	use_torso_look						() const;
	template <typename T1, typename T2, typename T3>
	IC		void	setup								(T1 _1, T2 _2, T3 _3);
	template <typename T1, typename T2>
	IC		void	setup								(T1 _1, T2 _2);
	template <typename T1>
	IC		void	setup								(T1 _1);
			void	setup								(const CSightAction &sight_action);
	virtual void	update								();
	IC		bool	turning_in_place					() const;
	IC		bool	enabled								() const;
	IC		void	enable								(bool value);
};

#include "sight_manager_inline.h"