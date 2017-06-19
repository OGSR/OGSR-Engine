//---------------------------------------------------------------------------
#ifndef EScenePortalToolsH
#define EScenePortalToolsH

#include "ESceneCustomOTools.h"

class EScenePortalTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
    friend class 		CPortal;
protected:
	enum{
    	flDrawSimpleModel	= (1<<31),
    };
    Flags32				m_Flags;
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						EScenePortalTools		():ESceneCustomOTools(OBJCLASS_PORTAL){m_Flags.zero();}
	// definition
    IC LPCSTR			ClassName				(){return "portal";}
    IC LPCSTR			ClassDesc				(){return "Portal";}
    IC int				RenderPriority			(){return 20;}

	virtual void 		FillProp				(LPCSTR pref, PropItemVec& items);

    virtual void		Clear					(bool bSpecific=false){inherited::Clear(bSpecific);m_Flags.zero();}
    // IO
    virtual bool   		IsNeedSave				(){return true;}
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);

    virtual CCustomObject* CreateObject			(LPVOID data, LPCSTR name);
};
//---------------------------------------------------------------------------
#endif
