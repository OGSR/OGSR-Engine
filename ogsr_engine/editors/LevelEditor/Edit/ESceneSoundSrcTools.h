//---------------------------------------------------------------------------
#ifndef ESceneSoundSrcToolsH
#define ESceneSoundSrcToolsH

#include "ESceneCustomOTools.h"

class ESceneSoundSrcTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneSoundSrcTools		():ESceneCustomOTools(OBJCLASS_SOUND_SRC){;}
	// definition
    IC LPCSTR			ClassName				(){return "sound_src";}
    IC LPCSTR			ClassDesc				(){return "Sound Source";}
    IC int				RenderPriority			(){return 10;}

    virtual void		Clear					(bool bSpecific=false){inherited::Clear(bSpecific);}
    // IO
    virtual bool   		IsNeedSave				(){return inherited::IsNeedSave();}
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);

    virtual CCustomObject* CreateObject			(LPVOID data, LPCSTR name);
};
//---------------------------------------------------------------------------
#endif
