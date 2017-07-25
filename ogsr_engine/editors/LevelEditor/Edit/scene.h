//----------------------------------------------------
// file: Scene.h
//----------------------------------------------------
#ifndef SceneH
#define SceneH

#include "SceneGraph.h"
#include "Communicate.h"
#include "pure.h"
#include "ElTree.hpp"

#include "ESceneCustomMTools.h"
#include "ESceneCustomOTools.h"
#include "xrLevel.h"
#include "../ECore/Editor/pick_defs.h"
//refs
struct FSChunkDef;
class PropValue;
struct SPBItem;
//----------------------------------------------------

#pragma pack( push,1 )
struct UndoItem {
	char m_FileName[MAX_PATH];
};
#pragma pack( pop )

class TProperties;
/*
int __cdecl _CompareByDist( const void *a, const void *b)
{
    return ((SRayPickInfo*)a)->rp_inf.range - ((SRayPickInfo*)b)->rp_inf.range;
}
*/

struct st_LevelOptions{
	shared_str		m_FNLevelPath;
    shared_str		m_LevelPrefix;
	shared_str 		m_BOPText;
    
    u8	 			m_LightHemiQuality;
    u8 				m_LightSunQuality;

    b_params		m_BuildParams;

    BOOL			m_bDeathmatch;
    BOOL			m_bTeamDeathmatch;
    BOOL			m_bArtefacthunt;

    				st_LevelOptions	();

    void			SetCustomQuality();
    void			SetDraftQuality	();
    void			SetHighQuality	();
    
	void 			Save			(IWriter&);
	void 			Read			(IReader&);
    void			Reset			();
};

class EScene
//	,public pureDeviceCreate,
//	public pureDeviceDestroy
{
    CMemoryWriter 	m_SaveCache;
public:
	typedef	FixedMAP<float,CCustomObject*>	mapObject_D;
	typedef mapObject_D::TNode	 	    	mapObject_Node;
	mapObject_D						    	mapRenderObjects;
public:
	st_LevelOptions	m_LevelOp;
protected:
	bool m_Valid;
	int m_Locked;

    // version control 
    xrGUID			m_GUID;
    shared_str		m_OwnerName;
    time_t			m_CreateTime;

    // 
	int m_LastAvailObject;

    SceneToolsMap   m_SceneTools;

	xr_deque<UndoItem> m_UndoStack;
	xr_deque<UndoItem> m_RedoStack;

	TProperties* m_SummaryInfo;

    ObjectList		m_ESO_SnapObjects; // временно здесь а вообще нужно перенести в ESceneTools
protected:
    bool 			OnLoadAppendObject			(CCustomObject* O);
    bool 			OnLoadSelectionAppendObject(CCustomObject* O);
protected:
	void			RegisterSceneTools			(ESceneCustomMTools* mt);
	void			CreateSceneTools			();
	void			DestroySceneTools			();

    void 			FindObjectByNameCB			(LPCSTR new_name, bool& res){res=!!FindObjectByName(new_name,(CCustomObject*)0);}

	void __stdcall 	OnBuildControlClick			(ButtonValue* sender, bool& bModif, bool& bSafe);
	void __stdcall 	OnRTFlagsChange				(PropValue* sender);
public:
	enum{
    	flRT_Unsaved 			= (1<<0),
    	flRT_Modified 			= (1<<1),
    };
    Flags32			m_RTFlags;
public:

	typedef bool (__closure *TAppendObject)(CCustomObject* object);

	bool 			ReadObject			(IReader& F, CCustomObject*& O);
	bool 			ReadObjects			(IReader& F, u32 chunk_id, TAppendObject on_append, SPBItem* pb);
	void 			SaveObject			(CCustomObject* O,IWriter& F);
	void 			SaveObjects			(ObjectList& lst, u32 chunk_id, IWriter& F);

    xr_string		LevelPartPath		(LPCSTR map_name);
    xr_string		LevelPartName		(LPCSTR map_name, ObjClassID cls);

    BOOL			LoadLevelPart		(ESceneCustomMTools* M, LPCSTR map_name);
    BOOL			LoadLevelPart		(LPCSTR map_name, ObjClassID cls, bool lock);
    BOOL		 	UnloadLevelPart		(ESceneCustomMTools* M);
    BOOL			UnloadLevelPart		(LPCSTR map_name, ObjClassID cls, bool unlock);
	void 			LockLevel			(LPCSTR map_name);
	void 			UnlockLevel			(LPCSTR map_name);
public:
	bool			ExportGame			(SExportStreams* F);
	bool 			Load				(LPCSTR map_name, bool bUndo);
	void 			Save				(LPCSTR map_name, bool bUndo);
	bool 			LoadSelection		(LPCSTR fname);
	void 			SaveSelection		(ObjClassID classfilter, LPCSTR fname);
	void 			Unload				(BOOL bEditableToolsOnly);
	void 			Clear				(BOOL bEditableToolsOnly);    
    void			Reset				();
	void 			LoadCompilerError	(LPCSTR fn);
    void			SaveCompilerError	(LPCSTR fn);
    void			HighlightTexture	(LPCSTR t_name, bool allow_ratio, u32 t_width, u32 t_height, bool leave_previous);

    int				MultiRenameObjects	();

	IC bool 		valid				()           	{ return m_Valid; }

	IC bool 		locked				()          	{ return m_Locked!=0; }
	IC void 		lock				()            	{ m_Locked++; }
	IC void 		unlock				()          	{ m_Locked--; }
	IC void 		waitlock			()        		{ while( locked() ) Sleep(0); }

	IC ESceneCustomMTools* GetMTools	(ObjClassID cat)	{ return m_SceneTools[cat]; }
	IC ESceneCustomOTools* GetOTools	(ObjClassID cat)	{ return dynamic_cast<ESceneCustomOTools*>(GetMTools(cat)); }
	IC SceneToolsMapPairIt FirstTools	()				{ return m_SceneTools.begin(); }
	IC SceneToolsMapPairIt LastTools	()				{ return m_SceneTools.end(); }

	IC ObjectList&	ListObj    			(ObjClassID cat)	{ VERIFY(GetOTools(cat)); return GetOTools(cat)->GetObjects(); }
	IC ObjectIt 	FirstObj      		(ObjClassID cat)	{ VERIFY(GetOTools(cat)); return ListObj(cat).begin(); }
	IC ObjectIt 	LastObj       		(ObjClassID cat)	{ VERIFY(GetOTools(cat)); return ListObj(cat).end(); }
	IC int 			ObjCount           	(ObjClassID cat)	{ return ListObj(cat).size(); }
	int 			ObjCount 			();

	void 			RenderSky			(const Fmatrix& camera);
	void 			Render              (const Fmatrix& camera);
	void 			OnFrame				(float dT);

	virtual void 	AppendObject		(CCustomObject* object, bool bExecUndo=true);
	virtual bool 	RemoveObject		(CCustomObject* object, bool bExecUndo=true);
    void			BeforeObjectChange	(CCustomObject* object);
    bool 			ContainsObject		(CCustomObject* object, ObjClassID classfilter);

    // Snap List Part
	bool 			FindObjectInSnapList(CCustomObject* O);
	bool 			AddToSnapList		(CCustomObject* O, bool bUpdateScene=true);
    bool			DelFromSnapList		(CCustomObject* O, bool bUpdateScene=true);
	int 			AddSelToSnapList	();
    int 			DelSelFromSnapList	();
    int 			SetSnapList			();
    void 			RenderSnapList		();
    void 			ClearSnapList		(bool bCurrentOnly);
    void 			SelectSnapList		();
    void 			UpdateSnapList 	   	();
	virtual ObjectList* 	GetSnapList			(bool bIgnoreUse);

	virtual CCustomObject*	RayPickObject 		(float dist, const Fvector& start, const Fvector& dir, ObjClassID classfilter, SRayPickInfo* pinf, ObjectList* from_list);
	int 			BoxPickObjects		(const Fbox& box, SBoxPickInfoVec& pinf, ObjectList* from_list);
    int				RayQuery			(SPickQuery& RQ, const Fvector& start, const Fvector& dir, float dist, u32 flags, ObjectList* snap_list);
    int 			BoxQuery			(SPickQuery& RQ, const Fbox& bb, u32 flags, ObjectList* snap_list);
    int				RayQuery			(SPickQuery& RQ, const Fvector& start, const Fvector& dir, float dist, u32 flags, CDB::MODEL* model);
    int 			BoxQuery			(SPickQuery& RQ, const Fbox& bb, u32 flags, CDB::MODEL* model);

	int 			RaySelect           (int flag, ObjClassID classfilter=OBJCLASS_DUMMY); // flag=0,1,-1 (-1 invert)
	int 			FrustumSelect       (int flag, ObjClassID classfilter=OBJCLASS_DUMMY);
	void			SelectObjects       (bool flag, ObjClassID classfilter=OBJCLASS_DUMMY);
	int 			LockObjects         (bool flag, ObjClassID classfilter=OBJCLASS_DUMMY, bool bAllowSelectionFlag=false, bool bSelFlag=true);
	void 			ShowObjects         (bool flag, ObjClassID classfilter=OBJCLASS_DUMMY, bool bAllowSelectionFlag=false, bool bSelFlag=true);
	void			InvertSelection     (ObjClassID classfilter);
	int 			SelectionCount      (bool testflag, ObjClassID classfilter);
	void			RemoveSelection     (ObjClassID classfilter);
	void 			CutSelection        (ObjClassID classfilter);
	void			CopySelection       (ObjClassID classfilter);
	void			PasteSelection      ();

	void 			SelectLightsForObject(CCustomObject* obj);

    void 			ZoomExtents			(ObjClassID cls, BOOL bSelectedOnly);

	int 			FrustumPick			(const CFrustum& frustum, ObjClassID classfilter, ObjectList& ol);
	int 			SpherePick			(const Fvector& center, float radius, ObjClassID classfilter, ObjectList& ol);

	virtual void			GenObjectName		(ObjClassID cls_id, char *buffer, const char* prefix=NULL);
	virtual CCustomObject* 	FindObjectByName	(LPCSTR name, ObjClassID classfilter);
    virtual CCustomObject* 	FindObjectByName	(LPCSTR name, CCustomObject* pass_object);
    bool 			FindDuplicateName   ();

	void 			UndoClear			();
	void 			UndoSave			();
	bool 			Undo				();
	bool 			Redo				();

    bool 			GetBox				(Fbox& box, ObjClassID classfilter);
    bool 			GetBox				(Fbox& box, ObjectList& lst);

public:
	int  			GetQueryObjects		(ObjectList& objset, ObjClassID classfilter, int iSel=1, int iVis=1, int iLock=0);
    template <class Predicate>
    int  GetQueryObjects_if			(ObjectList& dest, ObjClassID classfilter, Predicate cmp){
        for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
            ObjectList& lst = it->second;
            if ((classfilter==OBJCLASS_DUMMY)||(classfilter==it->first)){
                for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
        			if (cmp(_F)) dest.push_back(*_F);
                }
            }
        }
        return dest.size();
    }
public:

	void 			OnCreate		();
	void 			OnDestroy		();
    void 			Modified		();
    bool 			IfModified		();
	bool 			IsUnsaved		();
	bool 			IsModified		();

    int 			GetUndoCount	(){return m_UndoStack.size();}
    int 			GetRedoCount	(){return m_RedoStack.size();}

    bool 			Validate		(bool bNeedOkMsg, bool bTestPortal, bool bTestHOM, bool bTestGlow, bool bTestShaderCompatible, bool bFullTest);
    void 			OnObjectsUpdate	();

					EScene			();
	virtual 		~EScene			();

	virtual	void	OnDeviceCreate	();
	virtual	void	OnDeviceDestroy	();

	void 			OnShowHint		(AStringVec& dest);

    void			SynchronizeObjects();

    void			ClearSummaryInfo	();
    void			CollectSummaryInfo	();
    void			ShowSummaryInfo		();
    void			ExportSummaryInfo	(LPCSTR f_name);

    xr_string		LevelPath		();
    shared_str 		LevelPrefix		(){return m_LevelOp.m_LevelPrefix; }

	void 			FillProp		(LPCSTR pref, PropItemVec& items, ObjClassID cls_id);
protected:
    typedef std::pair<xr_string,xr_string>  TSubstPair;
    typedef xr_vector<TSubstPair>           TSubstPairs;
    typedef TSubstPairs::iterator           TSubstPairs_it;
    typedef TSubstPairs::const_iterator     TSubstPairs_cit;
    TSubstPairs                             m_subst_pairs;
public:
    void            RegisterSubstObjectName  (const xr_string& from, const xr_string& to );
    bool            GetSubstObjectName       (const xr_string& from, xr_string& to) const;
};

//----------------------------------------------------
extern EScene* Scene;
//----------------------------------------------------

#endif /*_INCDEF_Scene_H_*/

