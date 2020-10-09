#ifndef PHSHELL_SPLITTER_H
#define PHSHELL_SPLITTER_H
#include "PHDefs.h"
#include "PHObject.h"

class CPHShellSplitter 
{
	friend	class CPHShellSplitterHolder;
	friend	class CPHShell;

public:
	enum EType				  {splElement,splJoint}										;
private:
	bool						m_breaked												;
	EType						m_type													;
	u16							m_element												;
	u16							m_joint													;
public:
	CPHShellSplitter			(CPHShellSplitter::EType type,u16 element,u16 joint)	;
	CPHShellSplitter			()														;
};

//class CPHShellSplitter;
class CPHShell;
class CODEGeom;
typedef std::pair<u16,CODEGeom*>	id_geom;
DEFINE_MAP(u16,CODEGeom*,GEOM_MAP,GEOM_MAP_I)
DEFINE_VECTOR(CPHShellSplitter,SPLITTER_STORAGE,SPLITTER_I)
typedef		xr_vector<CPHShellSplitter>::reverse_iterator	SPLITTER_RI;

class CPHShellSplitterHolder : public CPHUpdateObject			//call all Fractures and Breakable Joints Updates
{
friend class CPHShell;
bool							m_has_breaks																																;
bool							m_unbreakable																																;
CPHShell*						m_pShell																																	;			//purpose: to extract elements and joints corresponded splitters
SPLITTER_STORAGE				m_splitters																																	;			//
GEOM_MAP						m_geom_root_map																																;			//to find geom pointer by bone id
virtual	void					PhTune						(dReal step)																									;						//call fractures PhTune for element splitters m_pShell->m_elements[m_splitters[i]->m_element]->m_pFracturesHolder->PhTune()
virtual void					PhDataUpdate				(dReal step)																									;				//call fractures PhDataUpdate for element splitters m_pShell->m_elements[m_splitters[i]->m_element]->m_pFracturesHolder->PhDataUpdate()
		bool					CheckSplitter				(u16 aspl)																										;		//
		shell_root				SplitJoint					(u16 aspl)																										;		//create new shell moving into it departed elements and joints 
		shell_root				ElementSingleSplit			(const element_fracture &split_elem,const CPHElement* source_element)											;
		void					SplitElement				(u16 aspl,PHSHELL_PAIR_VECTOR &out_shels)																		;//
		void					PassEndSplitters			(const CShellSplitInfo& spl_inf,CPHShell* dest,u16 jt_add_shift,u16 el_add_shift)								;
		void					InitNewShell				(CPHShell* shell)																								;			//inits new active shell
public:
								CPHShellSplitterHolder		(CPHShell* shell)																								;
virtual							~CPHShellSplitterHolder		()																												;
		void					Activate					()																												;
		void					Deactivate					()																												;
		void					AddSplitter					(CPHShellSplitter::EType type,u16 element,u16 joint)															;
		void					AddSplitter					(CPHShellSplitter::EType type,u16 element,u16 joint,u16 position)												;
		void					SplitProcess				(PHSHELL_PAIR_VECTOR &out_shels)																				;
		void					AddToGeomMap				(const id_geom& id_rootgeom)																					;
		u16						FindRootGeom				(u16 bone_id)																									;
IC      bool					Breaked						()																							{return m_has_breaks;}
IC		bool					isEmpty						()																					{return m_splitters.empty()	;}
		void					SetUnbreakable				()																												;
		void					SetBreakable				()																												;
		bool					IsUnbreakable				()																					{return m_unbreakable		;}					
};



#endif