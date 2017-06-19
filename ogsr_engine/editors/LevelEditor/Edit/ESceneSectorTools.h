//---------------------------------------------------------------------------
#ifndef ESceneSectorToolsH
#define ESceneSectorToolsH

#include "ESceneCustomOTools.h"

class ESceneSectorTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
    friend class 		CSector;
protected:
	enum{
    	flDrawSolid		= (1<<31),
    };
    Flags32				m_Flags;
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneSectorTools		():ESceneCustomOTools(OBJCLASS_SECTOR){m_Flags.zero();}
	// definition
    IC LPCSTR			ClassName				(){return "sector";}
    IC LPCSTR			ClassDesc				(){return "Sector";}
    IC int				RenderPriority			(){return 20;}

    virtual void 		OnObjectRemove			(CCustomObject* O);
    virtual void 		OnBeforeObjectChange	(CCustomObject* O);

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
