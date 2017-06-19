//---------------------------------------------------------------------------
#ifndef ESceneGroupToolsH
#define ESceneGroupToolsH

#include "ESceneCustomOTools.h"

class ESceneGroupTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
    xr_string			m_CurrentObject;
protected:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						ESceneGroupTools		():ESceneCustomOTools(OBJCLASS_GROUP){;}
	// definition
    IC LPCSTR			ClassName				(){return "group";}
    IC LPCSTR			ClassDesc				(){return "Group";}
    IC int				RenderPriority			(){return 1;}

    virtual void		Clear					(bool bSpecific=false){inherited::Clear(bSpecific);}
    // IO
    virtual bool   		IsNeedSave				(){return inherited::IsNeedSave();}
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);

    virtual void		GetStaticDesc			(int& v_cnt, int& f_cnt);

	virtual void 		OnActivate				();

    // group function
    void				UngroupObjects			(bool bUndo=true);
	void 				GroupObjects			(bool bUndo=true);
    void				OpenGroups				(bool bUndo=true);
	void 				CloseGroups				(bool bUndo=true);
    
	void 				CenterToGroup			();
    void 				AlignToObject			();

    void 				MakeThumbnail			();

    void 				SaveSelectedObject		();
    void 				ReloadRefsSelectedObject();
    void 				SetCurrentObject		(LPCSTR nm);
    LPCSTR				GetCurrentObject		(){return m_CurrentObject.c_str();}
    
    virtual CCustomObject* CreateObject			(LPVOID data, LPCSTR name);
};
//---------------------------------------------------------------------------
#endif
