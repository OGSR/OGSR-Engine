#ifndef PHDEFS_H
#define PHDEFS_H
class CPHElement;
class CPHJoint;
class CPhysicsShell;
//class CPHFracture;
//class CShellSplitInfo;

#include "ode_include.h"

IC	void sub_diapasones(u16 &from1,u16 &to1,const u16 &from0,const u16 &to0)
{
	if(from0==to0 ||from1==to1|| to1<=from0||to1==u16(-1)) return;
	R_ASSERT(from0>=from1&&to0<=to1);
	u16 dip=to0-from0;
	to1=to1-dip;
}

class CShellSplitInfo 
{
friend class  CPHFracturesHolder;
friend class  CPHShellSplitterHolder;
friend class CPHElement;
IC bool HaveElements		()		{return m_end_el_num!=m_start_el_num;}
IC bool HaveJoints			()		{return m_start_jt_num!=m_end_jt_num;}
public:
IC void sub_diapasone(const CShellSplitInfo& sub)
{
	sub_diapasones(m_start_el_num,m_end_el_num,sub.m_start_el_num,sub.m_end_el_num);
	sub_diapasones(m_start_jt_num,m_end_jt_num,sub.m_start_jt_num,sub.m_end_jt_num);
}
protected:
	u16				m_start_el_num;
	u16				m_end_el_num;
	u16				m_start_jt_num;
	u16				m_end_jt_num;
	u16				m_start_geom_num;
	u16				m_end_geom_num;
	u16				m_bone_id;
};


class CPHFracture : public CShellSplitInfo
{
friend class  CPHFracturesHolder;
friend class CPHElement;
friend class CPHShell;
bool			m_breaked;
dMass			m_firstM;
dMass			m_secondM;
//when breaked m_pos_in_element-additional force m_break_force-additional torque -x additional torque-y add_torque_z - additional torque z
float			m_break_force;
float			m_break_torque;
Fvector			m_pos_in_element;
float			m_add_torque_z;
				CPHFracture();
public:
bool			Update(CPHElement* element);
IC bool			Breaked(){return m_breaked;}
void			SetMassParts(const dMass& first,const dMass& second);
void			MassSetZerro();
void			MassAddToFirst(const dMass& m);
void			MassAddToSecond(const dMass& m);
void			MassSubFromFirst(const dMass& m);
void			MassSubFromSecond(const dMass& m);
void			MassSetFirst(const dMass& m);
void			MassSetSecond(const dMass& m);
const dMass&	MassFirst(){return m_firstM;}
const dMass&	MassSecond(){return m_secondM;}
void			MassUnsplitFromFirstToSecond(const dMass& m);
};

typedef std::pair<CPhysicsShell*,u16>	shell_root;
typedef std::pair<CPHElement*,CShellSplitInfo>		element_fracture;
DEFINE_VECTOR(CPHElement*,ELEMENT_STORAGE,ELEMENT_I)
DEFINE_VECTOR(CPHJoint*,JOINT_STORAGE,JOINT_I)
DEFINE_VECTOR(shell_root,PHSHELL_PAIR_VECTOR,SHELL_PAIR_I)
typedef xr_vector<shell_root>::reverse_iterator SHELL_PAIR_RI;
DEFINE_VECTOR(element_fracture,ELEMENT_PAIR_VECTOR,ELEMENT_PAIR_I)
typedef		xr_vector<CPHElement*>::reverse_iterator	ELEMENT_RI;
typedef		xr_vector<element_fracture>::reverse_iterator	ELEMENT_PAIR_RI;
DEFINE_VECTOR(CPHFracture,FRACTURE_STORAGE,FRACTURE_I)
typedef		xr_vector<CPHFracture>::reverse_iterator	FRACTURE_RI;
#endif