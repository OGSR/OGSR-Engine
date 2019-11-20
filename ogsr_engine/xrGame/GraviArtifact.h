///////////////////////////////////////////////////////////////
// GraviArtifact.h
// GraviArtefact - гравитационный артефакт, прыгает на месте
// и парит над землей
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CGraviArtefact : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CGraviArtefact(void);
	virtual ~CGraviArtefact(void);

	virtual BOOL					net_Spawn						(CSE_Abstract* DC);
	virtual void Load				(LPCSTR section);
	virtual void					PhDataUpdate						(dReal step);
	virtual void					OnH_B_Independent				(bool just_before_destroy);

protected:
	virtual void	UpdateCLChild	();
	//параметры артефакта
	float m_fJumpHeight;
	float m_fEnergy;

	bool  m_jump_jump, m_keep, m_raise, m_jump_debug;
	float m_jump_min_height, m_keep_height;
	float m_jump_raise_speed, m_jump_keep_speed, m_jump_under_speed;
	u32   m_jump_time, m_jump_time_end;

	void process_gravity();
	void process_jump();
};
