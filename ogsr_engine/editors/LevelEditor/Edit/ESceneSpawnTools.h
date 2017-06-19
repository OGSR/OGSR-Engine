//---------------------------------------------------------------------------
#ifndef ESceneSpawnToolsH
#define ESceneSpawnToolsH

#include "ESceneCustomOTools.h"

class ESceneSpawnTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
    friend class 		CSpawnPoint;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
	enum{
    	flPickSpawnType	= (1<<30),
    	flShowSpawnType	= (1<<31),
    };
    Flags32				m_Flags;

    // class 
    DEFINE_VECTOR		(SChooseItem,SSVec,SSVecIt);
    DEFINE_MAP			(CLASS_ID,SSVec,ClassSpawnMap,ClassSpawnMapIt);
    ClassSpawnMap		m_Classes;

    // icon list
    DEFINE_MAP			(shared_str,ref_shader,ShaderMap,ShaderPairIt);
    ShaderMap 			m_Icons;
    ref_shader 			CreateIcon	(shared_str name);
    ref_shader 			GetIcon		(shared_str name);
public:
						ESceneSpawnTools		();
	virtual				~ESceneSpawnTools		();

	// definition
    IC LPCSTR			ClassName				(){return "spawn";}
    IC LPCSTR			ClassDesc				(){return "Spawn";}
    IC int				RenderPriority			(){return 1;}

    void 				FillProp				(LPCSTR pref, PropItemVec& items);

    virtual void		Clear					(bool bSpecific=false){inherited::Clear(bSpecific);m_Flags.zero();}
    // IO
    virtual bool   		IsNeedSave				(){return true;}
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);

	virtual int 		MultiRenameObjects		();
    
    virtual CCustomObject* CreateObject			(LPVOID data, LPCSTR name);
};
//---------------------------------------------------------------------------
// refs 
class ISE_Abstract;

typedef ISE_Abstract* 	(__stdcall *Tcreate_entity)		(LPCSTR section);
typedef void		  	(__stdcall *Tdestroy_entity)	(ISE_Abstract *&);

extern	Tcreate_entity 	create_entity;
extern	Tdestroy_entity destroy_entity;
//---------------------------------------------------------------------------
#endif
