//---------------------------------------------------------------------------
#ifndef ESceneWayToolsH
#define ESceneWayToolsH

#include "ESceneCustomOTools.h"

class ESceneWayTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneWayTools			():ESceneCustomOTools(OBJCLASS_WAY){;}
	// definition
    IC LPCSTR			ClassName				(){return "way";}
    IC LPCSTR			ClassDesc				(){return "Way";}
    IC int				RenderPriority			(){return 1;}

    virtual void		Clear					(bool bSpecific=false){inherited::Clear(bSpecific);}
    // IO
    virtual bool   		IsNeedSave				(){return inherited::IsNeedSave();}
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);

    virtual void    	OnActivate  			();

    virtual CCustomObject* CreateObject			(LPVOID data, LPCSTR name);
};
//---------------------------------------------------------------------------
#endif
