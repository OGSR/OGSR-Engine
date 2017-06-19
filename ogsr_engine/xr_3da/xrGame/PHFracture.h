//
//
#ifndef PH_FRACTURE_H
#define PH_FRACTURE_H

#include "PHDefs.h"
#include "PHImpact.h"

class CPHFracture;
class CPHElement;

DEFINE_VECTOR(dJointFeedback,CFEEDBACK_STORAGE,CFEEDBACK_I)



class CPHFracturesHolder 			//stored in CPHElement
{
friend class CPHElement;
friend class CPHShellSplitterHolder;
bool			 m_has_breaks;

FRACTURE_STORAGE m_fractures;
PH_IMPACT_STORAGE m_impacts;		//filled in anytime from CPHElement applyImpulseTrace cleared in PhDataUpdate
CFEEDBACK_STORAGE m_feedbacks;		//this store feedbacks for non contact joints 
public:
CPHFracturesHolder			();

~CPHFracturesHolder			();
void				DistributeAdditionalMass	(u16 geom_num,const dMass& m);//
void				SubFractureMass				(u16 fracture_num);
void				AddImpact		(const Fvector& force,const Fvector& point,u16 id);
PH_IMPACT_STORAGE&	Impacts			(){return m_impacts;}

CPHFracture&		LastFracture	(){return m_fractures.back();}
protected:
private:

u16 				CheckFractured	();										//returns first breaked fracture

element_fracture	SplitFromEnd	(CPHElement* element,u16 geom_num);
void				InitNewElement	(CPHElement* element,const Fmatrix &shift_pivot,float density);
void				PassEndFractures(u16 from,CPHElement* dest);
public:
void				SplitProcess	(CPHElement* element,ELEMENT_PAIR_VECTOR &new_elements);
u16					AddFracture		(const CPHFracture& fracture);
CPHFracture&		Fracture		(u16 num);
void				PhTune			(dBodyID body);										//set feedback for joints called from PhTune of ShellSplitterHolder
bool				PhDataUpdate	(CPHElement* element);										//collect joints and external impacts in fractures Update which set m_fractured; called from PhDataUpdate of ShellSplitterHolder returns true if has breaks
void				ApplyImpactsToElement(CPHElement* element);
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

#endif  PH_FRACTURE_H