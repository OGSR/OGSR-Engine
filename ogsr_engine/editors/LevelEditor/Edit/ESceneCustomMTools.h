#ifndef ESceneCustomMToolsH
#define ESceneCustomMToolsH

#include "ESceneClassList.h"
// refs
struct SSceneSummary;
class TUI_CustomControl;
class ESceneCustomMTools;
class SceneBuilder;
struct SExportStreams;

DEFINE_VECTOR(TUI_CustomControl*,ControlsVec,ControlsIt);

class ESceneCustomMTools
{
	ObjClassID 			FClassID;
protected:
	// controls
    ControlsVec 		m_Controls;
    int             	action;
    int					sub_target;
public:
	enum{
    	flEnable		= (1<<0),
    	flReadonly		= (1<<1),
    	flForceReadonly	= (1<<2),
    };
    Flags32				m_EditFlags;
    BOOL				IsEnabled				(){return m_EditFlags.is(flEnable);}
    BOOL				IsEditable				(){return !m_EditFlags.is_any(flReadonly|flForceReadonly);}
    BOOL				IsReadonly				(){return m_EditFlags.is(flReadonly);}
    BOOL				IsForceReadonly			(){return m_EditFlags.is(flForceReadonly);}
public:
	// modifiers
    shared_str			m_ModifName;
    time_t				m_ModifTime;
	// frame & Controls
    TUI_CustomControl* 	pCurControl;
    TForm*				pFrame;
protected:
    void            	AddControl				(TUI_CustomControl* c);
    TUI_CustomControl* 	FindControl				(int subtarget, int action);
    void            	UpdateControl			();
public:
    void            	SetAction   			(int action);
    void            	SetSubTarget			(int target);
    void            	ResetSubTarget			();
protected:
    void				CreateDefaultControls	(u32 sub_target_id);
    virtual void		CreateControls			()=0;
    virtual void		RemoveControls			();
public:
    virtual void    	OnActivate  			();
    virtual void    	OnDeactivate			();
    virtual void    	OnObjectsUpdate			(){;}
public:
    PropertyGP			(FClassID,FClassID) 	ObjClassID ClassID;
	// definition
    virtual LPCSTR		ClassName				()=0;
    virtual LPCSTR		ClassDesc				()=0;
    virtual int			RenderPriority			()=0;
public:
						ESceneCustomMTools		(ObjClassID cls);
	virtual				~ESceneCustomMTools		();

    virtual void		OnCreate				();
    virtual void		OnDestroy				();
    
	virtual	bool		AllowEnabling    		()=0;
    // snap 
	virtual ObjectList* GetSnapList				()=0;
    virtual void		UpdateSnapList			()=0;
    
	// selection manipulate
    // flags: [0 - FALSE, 1 - TRUE, -1 - INVERT]
	virtual int			RaySelect				(int flag, float& distance, const Fvector& start, const Fvector& direction, BOOL bDistanceOnly)=0;
	virtual int			FrustumSelect			(int flag, const CFrustum& frustum)=0;
	virtual void 		SelectObjects           (bool flag)=0;
	virtual void		InvertSelection         ()=0;
	virtual void		RemoveSelection         ()=0;
	virtual int 		SelectionCount          (bool testflag)=0;
	virtual void 		ShowObjects				(bool flag, bool bAllowSelectionFlag=false, bool bSelFlag=true)=0;

    virtual void		Clear					(bool bSpecific=false)=0;       
    virtual void		Reset					();

    // validation
    virtual bool		Valid					()=0;
    virtual bool		Validate				(bool)=0;

    // events
	virtual void		OnDeviceCreate			()=0;
	virtual void		OnDeviceDestroy			()=0;
	virtual void		OnSynchronize			()=0;
	virtual void		OnSceneUpdate			(){;}
    virtual void		OnObjectRemove			(CCustomObject* O)=0;
    virtual void 		OnBeforeObjectChange	(CCustomObject* O){};
	virtual void		OnFrame					()=0;

    // render
    virtual void		BeforeRender			(){;}
    virtual void		OnRender				(int priority, bool strictB2F)=0;
    virtual void		AfterRender				(){;}

    // IO
    virtual bool   		IsNeedSave				()=0;
    virtual bool		Load            		(IReader&)=0;
    virtual void		Save            		(IWriter&)=0;
    virtual bool		LoadSelection      		(IReader&)=0;
    virtual void		SaveSelection      		(IWriter&)=0;

    virtual bool		Export          		(LPCSTR path){return true;}
    virtual bool		ExportGame         		(SExportStreams* F){return true;}

    virtual bool		ExportStatic			(SceneBuilder* B){return true;}
    virtual void		GetStaticDesc			(int& v_cnt, int& f_cnt){}

    // properties
    virtual void		FillProp          		(LPCSTR pref, PropItemVec& items)=0;

    // utils
	virtual bool 		GetSummaryInfo			(SSceneSummary* inf)=0;
	virtual void 		HighlightTexture		(LPCSTR tex_name, bool allow_ratio, u32 t_width, u32 t_height, BOOL mark){}
    virtual void		GetBBox 				(Fbox& bb, bool bSelOnly)=0;
};

DEFINE_MAP(ObjClassID,ESceneCustomMTools*,SceneToolsMap,SceneToolsMapPairIt);
#endif //ESceneCustomMToolsH
 