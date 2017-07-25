//----------------------------------------------------
// file: GroupObject.h
//----------------------------------------------------
#ifndef GroupObjectH
#define GroupObjectH

#include "CustomObject.h"
//----------------------------------------------------
class CGroupObject: public CCustomObject{
	ObjectList      m_Objects;
    typedef CCustomObject inherited;
    bool			LL_AppendObject	(CCustomObject* object, bool append);
    bool			AppendObjectCB	(CCustomObject* object);
    enum{
    	flStateOpened	= (1<<0),
    };
    Flags32			m_Flags;
    SStringVec*		m_PObjects;
    shared_str		m_ReferenceName;
	void 			ReferenceChange	(PropValue* sender);
    void			Clear			();
public:
	bool 			UpdateReference	();
public:
					CGroupObject	(LPVOID data, LPCSTR name);
	void 			Construct		(LPVOID data);
	virtual 		~CGroupObject	();
    bool			CanUngroup		(bool bMsg);
    void			GroupObjects	(ObjectList& lst);
    void			UngroupObjects	();
    void			OpenGroup		();
    void			CloseGroup		();
    IC ObjectList&  GetObjects		(){return m_Objects;}
    IC int			ObjectCount		(){return m_Objects.size();}

    bool 			IsOpened		(){return m_Flags.is(flStateOpened);}
    bool			SetReference	(LPCSTR nm);
	virtual LPCSTR  RefName			(){return m_ReferenceName.size()?m_ReferenceName.c_str():"group";}
	bool    		RefCompare		(LPCSTR nm){return m_ReferenceName.equal(nm);}
    
	void			UpdatePivot		(LPCSTR nm, bool center);
	virtual bool 	GetBox			(Fbox& box);
    virtual bool	CanAttach		() {return false;}

    virtual void 	NumSetPosition	(const Fvector& pos);
	virtual void 	NumSetRotation	(const Fvector& rot);
    virtual void 	NumSetScale		(const Fvector& scale);
	virtual void 	MoveTo			(const Fvector& pos, const Fvector& up);
	virtual void 	Move			(Fvector& amount);
	virtual void 	RotateParent	(Fvector& axis, float angle );
	virtual void 	RotateLocal		(Fvector& axis, float angle );
	virtual void 	Scale			(Fvector& amount );

	virtual void 	Render			(int priority, bool strictB2F);

	virtual bool 	RayPick			(float& dist, const Fvector& start,const Fvector& dir, SRayPickInfo* pinf=NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum);

    virtual void 	OnUpdateTransform();
	virtual void 	OnFrame			();

	virtual bool 	Load			(IReader& F);
	virtual void 	Save			(IWriter& F);
    virtual bool	ExportGame		(SExportStreams* data);

    // device dependent routine
	virtual void 	OnDeviceCreate 	();
	virtual void 	OnDeviceDestroy	();

    virtual void    OnShowHint      (AStringVec& dest);
    virtual void 	OnObjectRemove	(const CCustomObject* object);
	virtual void 	OnSceneUpdate	();

	virtual void	FillProp		(LPCSTR pref, PropItemVec& items);
};
//----------------------------------------------------
#endif /* _INCDEF_GroupObject_H_ */
