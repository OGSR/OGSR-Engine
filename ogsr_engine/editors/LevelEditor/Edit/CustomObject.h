//----------------------------------------------------
// file: CustomObject.h
//----------------------------------------------------
#ifndef CustomObjectH
#define CustomObjectH

#include "ESceneClassList.h"
#include "SceneSummaryInfo.h"

#define CHUNK_OBJECT_BODY   0x7777
//----------------------------------------------------
struct SRayPickInfo;
struct FSChunkDef;
class CFrustum;
class IReader;
class IWriter;
class COMotion;
class CCustomMotion;
class SAnimParams;
struct SSceneSummary;
class ESceneCustomOTools;

struct SExportStreamItem{
	int					chunk;
	CMemoryWriter		stream;
    SExportStreamItem	():chunk(0){;}
};

struct SExportStreams{
	SExportStreamItem	spawn;
	SExportStreamItem	patrolpath;
	SExportStreamItem	rpoint;
	SExportStreamItem	sound_static;
	SExportStreamItem	sound_env_geom;
	SExportStreamItem	pe_static;
	SExportStreamItem	envmodif;
};

class ECORE_API CCustomObject {
	ObjClassID		FClassID;
    ESceneCustomOTools* FParentTools;

	SAnimParams*	m_MotionParams;
    COMotion*		m_Motion;

    // private animation methods
    void 			AnimationOnFrame	();
	void 			AnimationDrawPath	();
    void			AnimationCreateKey	(float t);
    void			AnimationDeleteKey	(float t);
    void			AnimationUpdate		(float t);
protected:
	enum{
    	flSelected	= (1<<0),
    	flVisible	= (1<<1),
    	flLocked	= (1<<2),
    	flMotion	= (1<<3),

    	flAutoKey	= (1<<30),
    	flCameraView= (1<<31),
    };
    Flags32			m_CO_Flags;
	enum{
        flRT_Valid			= (1<<0),
        flRT_UpdateTransform= (1<<1),
        flRT_NeedSelfDelete	= (1<<2),
    };
    Flags32			m_RT_Flags;
public:
	shared_str		FName;

    // orientation
    Fvector 		FPosition;
    Fvector 		FScale;
    Fvector 		FRotation;
    Fmatrix			FTransformP;
    Fmatrix			FTransformR;
    Fmatrix			FTransformS;
    Fmatrix			FTransformRP;
	Fmatrix 		FTransform;
    Fmatrix			FITransformRP;
	Fmatrix 		FITransform;

    CCustomObject*	m_pOwnerObject;
	bool __stdcall  OnObjectNameAfterEdit	(PropValue* sender, shared_str& edit_val);
    void __stdcall 	OnTransformChange		(PropValue* value); 
	void __stdcall  OnMotionableChange		(PropValue* sender);
    void __stdcall 	OnMotionCommandsClick	(ButtonValue* value, bool& bModif, bool& bSafe);
    void __stdcall 	OnMotionFilesClick		(ButtonValue* value, bool& bModif, bool& bSafe);
    void __stdcall 	OnMotionControlClick	(ButtonValue* value, bool& bModif, bool& bSafe);
    void __stdcall 	OnMotionFrameChange		(PropValue* value); 
    void __stdcall 	OnMotionCurrentFrameChange(PropValue* value); 
    void __stdcall 	OnMotionCameraViewChange(PropValue* value); 
protected:
	LPCSTR			GetName			(){return *FName; }
	void			SetName			(LPCSTR N){string256 tmp; strcpy(tmp,N); strlwr(tmp); FName=tmp;}

    virtual Fvector& GetPosition	()	{ return FPosition; 	}
    virtual Fvector& GetRotation	()	{ return FRotation;		}
    virtual Fvector& GetScale		()	{ return FScale; 		}

    virtual void 	SetPosition		(const Fvector& pos)	{ FPosition.set(pos);	UpdateTransform();}
	virtual void 	SetRotation		(const Fvector& rot)	{ FRotation.set(rot);	UpdateTransform();}
    virtual void 	SetScale		(const Fvector& scale)	{ FScale.set(scale);	UpdateTransform();}

    void __stdcall 	OnNameChange		(PropValue* sender);
    void __stdcall 	OnNumChangePosition	(PropValue* sender);
    void __stdcall 	OnNumChangeRotation	(PropValue* sender);
    void __stdcall 	OnNumChangeScale	(PropValue* sender);

    virtual void	DeleteThis		(){m_RT_Flags.set(flRT_NeedSelfDelete,TRUE);}
public:
					CCustomObject	(LPVOID data, LPCSTR name);
					CCustomObject	(CCustomObject* source);
	virtual 		~CCustomObject	();
    
	IC BOOL 		Motionable		(){return m_CO_Flags.is(flMotion); 	}
	IC BOOL 		Visible			(){return m_CO_Flags.is(flVisible);	}
	IC BOOL 		Locked			(){return m_CO_Flags.is(flLocked); 	}
	IC BOOL 		Selected		(){return m_CO_Flags.is(flSelected);}
    IC BOOL			Valid			(){return m_RT_Flags.is(flRT_Valid);}
    IC BOOL			IsDeleted		(){return m_RT_Flags.is(flRT_NeedSelfDelete);}

	// editor integration
    virtual bool	Validate		(bool bMsg){return true;}
	virtual void	FillProp		(LPCSTR pref, PropItemVec& items);
	void			AnimationFillProp(LPCSTR pref, PropItemVec& items);
	virtual bool 	GetSummaryInfo	(SSceneSummary* inf);

	virtual void 	Select			(int  flag);
	virtual void 	Show			(BOOL flag);
	virtual void 	Lock			(BOOL flag);
    void			SetValid		(BOOL flag){m_RT_Flags.set(flRT_Valid,flag);}

	virtual bool 	IsRender		();
	virtual void 	Render			(int priority, bool strictB2F);
	virtual void 	OnFrame			();
    virtual void 	OnUpdateTransform();

	virtual bool 	RaySelect		(int flag, const Fvector& start, const Fvector& dir, bool bRayTest=false); // flag 1,0,-1 (-1 invert)
    virtual bool 	FrustumSelect	(int flag, const CFrustum& frustum);
	virtual bool 	RayPick			(float& dist, const Fvector& start, const Fvector& dir, SRayPickInfo* pinf=NULL){ return false; };
    virtual bool 	FrustumPick		(const CFrustum& frustum){ return false; };
    virtual bool 	SpherePick		(const Fvector& center, float radius){ return false; };

    void			ResetTransform	(){
    					FScale.set				(1,1,1);
    					FRotation.set			(0,0,0);
    					FPosition.set			(0,0,0);
						FTransform.identity		();
	    				FTransformRP.identity	();
                        FITransform.identity	();
	    				FITransformRP.identity	();
					}
    virtual void 	ResetAnimation	(bool upd_t=true){;}
    virtual void 	UpdateTransform	(bool bForced=false){m_RT_Flags.set(flRT_UpdateTransform,TRUE);if(bForced)OnUpdateTransform();}

    // animation methods
    
    // grouping methods
    void			OnDetach		();
    void            OnAttach		(CCustomObject* owner);
    CCustomObject* 	GetOwner		(){return m_pOwnerObject;}
    virtual bool	CanAttach		()=0;

    virtual bool	OnChooseQuery	(LPCSTR specific){return true;}
    
    // change position/orientation methods
    virtual void 	NumSetPosition	(const Fvector& pos)	{ SetPosition(pos); }
	virtual void 	NumSetRotation	(const Fvector& rot)	{ SetRotation(rot);	}
    virtual void 	NumSetScale		(const Fvector& scale)	{ SetScale(scale);	}
	virtual void 	MoveTo			(const Fvector& pos, const Fvector& up);
	virtual void 	Move			(Fvector& amount);
	virtual void 	RotateParent	(Fvector& axis, float angle );
	virtual void 	RotateLocal		(Fvector& axis, float angle );
	virtual void 	RotatePivot		(const Fmatrix& prev_inv, const Fmatrix& current);
	virtual void 	Scale			(Fvector& amount);
	virtual void 	ScalePivot		(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount);

	virtual bool 	Load			(IReader&);
	virtual void 	Save			(IWriter&);
    virtual bool	ExportGame		(SExportStreams* data){return true;}

	virtual bool 	GetBox			(Fbox& box){return false;}
	virtual bool 	GetUTBox		(Fbox& box){return false;}
	virtual void 	OnSceneUpdate	(){;}
    virtual void 	OnObjectRemove	(const CCustomObject* object){;}
    virtual bool 	OnSelectionRemove(){return true;} // возвращает можно ли его удалять вообще

	virtual void 	OnDeviceCreate	(){;}
	virtual void 	OnDeviceDestroy	(){;}

	virtual void 	OnSynchronize	();
    virtual void    OnShowHint      (AStringVec& dest);

    virtual LPCSTR	RefName			(){return 0;}

    IC const Fmatrix& _ITransform			(){return FITransform;}
    IC const Fmatrix& _Transform			(){return FTransform;}
    IC const Fvector& _Position				(){return FPosition;}
    IC const Fvector& _Rotation				(){return FRotation;}
    IC const Fvector& _Scale				(){return FScale;}

    PropertyGP(GetPosition,SetPosition)		Fvector PPosition;
    PropertyGP(GetRotation,SetRotation)		Fvector PRotation;
    PropertyGP(GetScale,SetScale)			Fvector PScale;

    PropertyGP(FParentTools,FParentTools)	ESceneCustomOTools* ParentTools;
    PropertyGP(FClassID,FClassID)			ObjClassID 	ClassID;
    PropertyGP(GetName,SetName) 			LPCSTR  	Name;
public:
	static void		SnapMove		(Fvector& pos, Fvector& rot, const Fmatrix& rotRP, const Fvector& amount);
	static void		NormalAlign		(Fvector& rot, const Fvector& up);
};

//----------------------------------------------------
#endif /* _INCDEF_CustomObject_H_ */
