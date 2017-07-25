//----------------------------------------------------
// file: GroupObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "GroupObject.h"
#include "Scene.h"
#include "ESceneCustomMTools.h"
#include "d3dutils.h"
//----------------------------------------------------
static const float EMPTY_GROUP_SIZE = 0.5f;
//----------------------------------------------------
#define GROUPOBJ_CURRENT_VERSION		0x0011
//----------------------------------------------------
#define GROUPOBJ_CHUNK_VERSION		  	0x0000
#define GROUPOBJ_CHUNK_OBJECT_LIST     	0x0001
#define GROUPOBJ_CHUNK_FLAGS	     	0x0003
#define GROUPOBJ_CHUNK_REFERENCE	  	0x0004
#define GROUPOBJ_CHUNK_OPEN_OBJECT_LIST	0x0005
//----------------------------------------------------
//------------------------------------------------------------------------------
// !!! при разворачивании груп использовать prefix если нужно имя !!!
//------------------------------------------------------------------------------

CGroupObject::CGroupObject(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct	(data);
}

void CGroupObject::Construct(LPVOID data)
{
	ClassID		= OBJCLASS_GROUP;
    m_Flags.zero();
    m_PObjects	= 0;
}

CGroupObject::~CGroupObject	()
{
	Clear();
}

void CGroupObject::Clear()
{
	if (!IsOpened()){
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
            (*it)->m_pOwnerObject=0;	//
            xr_delete(*it);
        }
    }
    m_Objects.clear();
    xr_delete(m_PObjects);
}

bool CGroupObject::GetBox(Fbox& bb)
{
    bb.invalidate		();
    // update box
    for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        switch((*it)->ClassID){
        case OBJCLASS_SPAWNPOINT:
        case OBJCLASS_SCENEOBJECT:{
            Fbox 	box;
            if ((*it)->GetBox(box))
                bb.merge(box);
        }break;
        default:
            bb.modify((*it)->PPosition);
        }
    }
    if (!bb.is_valid()){
    	bb.set			(PPosition,PPosition);
        bb.grow			(EMPTY_GROUP_SIZE);
    }
    return bb.is_valid();
}

void CGroupObject::OnUpdateTransform()
{
	inherited::OnUpdateTransform();
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnUpdateTransform();
}

void CGroupObject::OnFrame()
{
	inherited::OnFrame();
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnFrame();
}

bool CGroupObject::LL_AppendObject(CCustomObject* object, bool append)
{
    if (!object->CanAttach()){
    	ELog.Msg(mtError,"Can't attach object: '%s'",object->Name);
	    return false;
    }
    if (object->GetOwner()){
        if (mrNo==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Object '%s' already in group '%s'. Change group?",object->Name,object->GetOwner()->Name))
        	return false;
	    object->OnDetach();
    }
	object->OnAttach		(this);
    if (append)				m_Objects.push_back(object);
    return true;
}
bool CGroupObject::AppendObjectCB(CCustomObject* object)
{
    object->m_pOwnerObject	= this;
    m_Objects.push_back		(object);
    return true;
}
void CGroupObject::UpdatePivot(LPCSTR nm, bool center)
{
    // first init
    VERIFY(m_Objects.size());
    if (false==center){
    	CCustomObject* object = 0;
    	if (nm&&nm[0]){
            for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
            	if (0==strcmp(nm,(*it)->Name)){
                	object = *it;
                	break;
                }
            }
        }else{
        	bool bValidPivot = false;
            for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
                if ((*it)->ClassID==OBJCLASS_SCENEOBJECT){
                	object		= (*it);
                    bValidPivot = true;
                    break;
                }
            }
            if (!bValidPivot)
            	object			= m_Objects.front();
        }
        if (object){
            PPosition = object->PPosition;
            PRotation = object->PRotation;
//.			PScale	  = object->PScale;
			UpdateTransform(true);
//..		if (object->GetUTBox(box)) m_BBox.merge(box);
        }
    }else{
        // center alignment
        ObjectIt it=m_Objects.begin();
        Fvector C; C.set((*it)->PPosition); it++;
        for (; it!=m_Objects.end(); it++)
            C.add((*it)->PPosition);
        FPosition.div(C,m_Objects.size());
        FRotation.set(0,0,0);
//.		FScale.set(1.f,1.f,1.f);
		UpdateTransform(true);
    }
}

void CGroupObject::MoveTo(const Fvector& pos, const Fvector& up)
{
	Fvector old_r=FRotation;
	inherited::MoveTo(pos,up);
    Fmatrix prev; prev.invert(FTransform);
    UpdateTransform(true);

    Fvector dr; dr.sub(FRotation,old_r);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fvector r=(*it)->PRotation; r.add(dr); (*it)->PRotation=r;
    	Fvector v=(*it)->PPosition;
        prev.transform_tiny(v);
        FTransform.transform_tiny(v);
    	(*it)->PPosition=v;
    }
}

void CGroupObject::NumSetPosition(const Fvector& pos)
{
	inherited::NumSetPosition(pos);
    Fmatrix prev; prev.invert(FTransform);
    UpdateTransform(true);

	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fvector v=(*it)->PPosition;
        prev.transform_tiny(v);
        FTransform.transform_tiny(v);
    	(*it)->PPosition=v;
    }
}
void CGroupObject::NumSetRotation(const Fvector& rot)
{
	Fvector old_r;
    FTransformR.getXYZ(old_r);
	inherited::NumSetRotation(rot);
    Fmatrix prev; prev.invert(FTransform);
    UpdateTransform(true);

    Fvector dr; dr.sub(FRotation,old_r);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fvector r=(*it)->PRotation; r.add(dr); (*it)->PRotation=r;
    	Fvector v=(*it)->PPosition;
        prev.transform_tiny(v);
        FTransform.transform_tiny(v);
    	(*it)->PPosition=v;
    }
}
void CGroupObject::NumSetScale(const Fvector& scale)
{
	Fvector old_s = PScale;
	inherited::NumSetScale(scale);
    Fmatrix prev; prev.invert(FTransform);
    UpdateTransform(true);

    Fvector ds; ds.sub(FScale,old_s);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fvector s=(*it)->PScale; s.add(ds); (*it)->PScale=s;
    	Fvector v=(*it)->PPosition;
        prev.transform_tiny(v);
        FTransform.transform_tiny(v);
    	(*it)->PPosition=v;
    }
}

void CGroupObject::Move(Fvector& amount)
{
	Fvector old_r=FRotation;
	inherited::Move(amount);
    Fmatrix prev; prev.invert(FTransform);
    UpdateTransform(true);

    Fvector dr; dr.sub(FRotation,old_r);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fvector r=(*it)->PRotation; r.add(dr); (*it)->PRotation=r;
    	Fvector v=(*it)->PPosition;
        prev.transform_tiny(v);
        FTransform.transform_tiny(v);
    	(*it)->PPosition=v;
    }
}
void CGroupObject::RotateParent(Fvector& axis, float angle )
{
	inherited::RotateParent(axis,angle);
    Fmatrix  Ginv;
    Ginv.set		(FITransformRP);
	UpdateTransform	(true);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fmatrix 	O,On;
        O.mul		(Ginv,(*it)->FTransformRP);
        On.mul		(FTransform,O);
        Fvector 	xyz;
        On.getXYZ	(xyz);
        (*it)->NumSetRotation(xyz);
        (*it)->NumSetPosition(On.c);
//		(*it)->PivotRotateParent(m_old,FTransform,axis,angle);
    }
}
void CGroupObject::RotateLocal(Fvector& axis, float angle )
{
	inherited::RotateLocal(axis,angle);
    Fmatrix  Ginv;
    Ginv.set		(FITransformRP);
	UpdateTransform	(true);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fmatrix 	O,On;
        O.mul		(Ginv,(*it)->FTransformRP);
        On.mul		(FTransform,O);
        Fvector 	xyz;
        On.getXYZ	(xyz);
        (*it)->NumSetRotation(xyz);
        (*it)->NumSetPosition(On.c);
//		(*it)->PivotRotateParent(m_old,FTransform,axis,angle);
    }
/*    
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
		(*it)->PivotRotateLocal(FTransformRP,FPosition,axis,angle);
*/
}
void CGroupObject::Scale(Fvector& amount )
{
	inherited::Scale(amount);
    Fmatrix  m_old;
    m_old.invert(FTransform);
	UpdateTransform(true);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
		(*it)->ScalePivot(m_old,FTransform,amount);
}
void CGroupObject::Render(int priority, bool strictB2F)
{
	inherited::Render(priority, strictB2F);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	if ((*it)->IsRender()){
	    	switch ((*it)->ClassID){
    	    case OBJCLASS_SCENEOBJECT: (*it)->Render(priority,strictB2F); break;
            default:
                Device.SetShader(strictB2F?Device.m_SelectionShader:Device.m_WireShader);
                RCache.set_xform_world(Fidentity);
                (*it)->Render(priority,strictB2F);
        	}
    	}
    }
	if ((1==priority)&&(false==strictB2F)){
    	Fbox bb;
    	if (Selected()&&GetBox(bb)){
            Device.SetShader(Device.m_WireShader);
            RCache.set_xform_world(Fidentity);
            u32 clr = Locked()?0xFFFF0000:(IsOpened()?0xFF7070FF:0xFFFFFFFF);
            DU.DrawSelectionBox(bb,&clr);
        }
    }
}

bool CGroupObject::FrustumPick(const CFrustum& frustum)
{
    if (m_Objects.empty()){
        Fbox 		bb;
        GetBox		(bb);
        u32 mask	= u32(-1); 
        return (frustum.testAABB(bb.data(),mask));
    }else{
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
            if ((*it)->FrustumPick(frustum)) return true;
    }
    return false;
}

bool CGroupObject::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
	bool bPick = false;
    if (m_Objects.empty()){
/*		Fbox 		bb;
        GetBox		(bb);
        u32 mask	= u32(-1); 
        return (frustum.testAABB(bb.data(),mask));
*/
    }else{
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
            if ((*it)->RayPick(distance,start,direction,pinf)) bPick=true;
    }
    return bPick;
}

void CGroupObject::OnDeviceCreate()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnDeviceCreate();
}

void CGroupObject::OnDeviceDestroy()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnDeviceDestroy();
}
//------------------------------------------------------------------------------

bool CGroupObject::Load(IReader& F)
{
    u32 version = 0;
    char buf[1024];
    R_ASSERT(F.r_chunk(GROUPOBJ_CHUNK_VERSION,&version));
    if (version!=GROUPOBJ_CURRENT_VERSION){
        ELog.DlgMsg( mtError, "CGroupObject: unsupported file version. Object can't load.");
        return false;
    }
	CCustomObject::Load(F);

    F.r_chunk(GROUPOBJ_CHUNK_FLAGS,&m_Flags);

	// objects
    if (IsOpened()){           
    	m_PObjects	= xr_new<SStringVec>();
        R_ASSERT(F.find_chunk(GROUPOBJ_CHUNK_OPEN_OBJECT_LIST));
        u32 cnt 	= F.r_u32();
        xr_string 	tmp;
        for (u32 k=0; k<cnt; k++){
            F.r_stringZ				(tmp);
        	m_PObjects->push_back	(tmp);
        }
    }else{
	    Scene->ReadObjects(F,GROUPOBJ_CHUNK_OBJECT_LIST,AppendObjectCB,0);
    }
    VERIFY(m_Objects.size()||(0!=m_PObjects));

    if (F.find_chunk(GROUPOBJ_CHUNK_REFERENCE))	
    	F.r_stringZ	(m_ReferenceName);

    return 			true;
}

void CGroupObject::Save(IWriter& F)
{
	CCustomObject::Save(F);

	F.open_chunk	(GROUPOBJ_CHUNK_VERSION);
	F.w_u16			(GROUPOBJ_CURRENT_VERSION);
	F.close_chunk	();

    F.w_chunk		(GROUPOBJ_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

    // objects
    if (IsOpened()){
        F.open_chunk(GROUPOBJ_CHUNK_OPEN_OBJECT_LIST);
        F.w_u32		(m_Objects.size());
		for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
            F.w_stringZ	((*it)->Name);
		F.close_chunk	();
    }else{
	    Scene->SaveObjects(m_Objects,GROUPOBJ_CHUNK_OBJECT_LIST,F);
    }

    F.open_chunk	(GROUPOBJ_CHUNK_REFERENCE);
    F.w_stringZ		(m_ReferenceName);
	F.close_chunk	();
}
//----------------------------------------------------

bool CGroupObject::ExportGame(SExportStreams* data)
{
	bool bres=true;
	if (!IsOpened()){
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
            if (!(*it)->ExportGame(data)) bres = false;
    }
	return bres;
}
//----------------------------------------------------

void CGroupObject::ReferenceChange(PropValue* sender)
{
	UpdateReference		();
}
//----------------------------------------------------

bool CGroupObject::SetReference(LPCSTR ref_name)
{
	shared_str old_refs	= m_ReferenceName;
    m_ReferenceName		= ref_name;
    bool bres 			=  UpdateReference();
	if (false==bres)	m_ReferenceName	= old_refs;
    return bres;
}

bool CGroupObject::UpdateReference()
{
	if (!m_ReferenceName.size()){
        ELog.Msg		(mtError,"ERROR: '%s' - has empty reference.",Name);
     	return false;
    }
    
    xr_string fn		= m_ReferenceName.c_str();
    fn					= EFS.ChangeFileExt(fn,".group");
    IReader* R			= FS.r_open(_groups_,fn.c_str());
    bool bres			= false;
    if (R){
    	CloseGroup		();
		Clear			();
        xr_string nm	= Name;
		shared_str old_refs	= m_ReferenceName;
        UpdateTransform	(true);
        Fvector old_pos	= PPosition;
        Fvector old_rot	= PRotation;
        Fvector old_sc	= PScale;
        if (Load(*R)){
            Name 		= nm.c_str();
            bres		= true;
	        UpdateTransform	(true);
        }
	    m_ReferenceName	= old_refs;
        NumSetPosition	(old_pos);
        NumSetRotation	(old_rot);
        NumSetScale		(old_sc);
        UpdateTransform	(true);
        FS.r_close		(R);
    }else{
        ELog.Msg		(mtError,"ERROR: Can't open group file: '%s'.",fn.c_str());
    }
    return bres;
}
//----------------------------------------------------

void CGroupObject::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref, items);
    PropValue* V		= PHelper().CreateChoose	(items,PrepareKey(pref,"Reference"),&m_ReferenceName, smGroup); 
    V->OnChangeEvent.bind(this,&CGroupObject::ReferenceChange);
    PHelper().CreateCaption							(items,PrepareKey(pref,"State"), 	IsOpened()?"Opened":"Closed");
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
	    PHelper().CreateCaption						(items,PrepareKey(pref,AnsiString().sprintf("%s: objects",Name).c_str(),ParentTools->ClassDesc(),(*it)->Name), "");
}
//----------------------------------------------------

void CGroupObject::OnShowHint(AStringVec& dest)
{
	inherited::OnShowHint(dest);
    dest.push_back(AnsiString("Reference: ")+m_ReferenceName.c_str());
    dest.push_back(AnsiString("-------"));
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
	    dest.push_back((*it)->Name);
}
//----------------------------------------------------

void CGroupObject::OnObjectRemove(const CCustomObject* object)
{
	if (IsOpened()){
	    m_Objects.remove((CCustomObject*)object);
        if (m_Objects.empty()){
            ELog.Msg	(mtInformation,"Group '%s' has no objects and will be removed.",Name);
            DeleteThis	();
        }
    }
}
//----------------------------------------------------

void CGroupObject::OnSceneUpdate()
{
	inherited::OnSceneUpdate();
    if (IsOpened()&&(0!=m_PObjects)){
    	for (SStringVecIt it=m_PObjects->begin(); it!=m_PObjects->end(); it++){
        	CCustomObject* obj	= Scene->FindObjectByName(it->c_str(),(CCustomObject*)0); 
            if (0==obj){ 
            	ELog.Msg	(mtError,"Group '%s' has invalid reference to object '%s'.",Name,it->c_str());
            }else{
			    m_Objects.push_back	(obj);
            }
        }
        xr_delete	(m_PObjects);
        if (m_Objects.empty()){
            ELog.Msg	(mtInformation,"Group '%s' has no objects and will be removed.",Name);
            DeleteThis	();
        }
    }
}
//----------------------------------------------------
bool CGroupObject::CanUngroup(bool bMsg)
{
	bool res	= true;
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	if (!Scene->GetMTools((*it)->ClassID)->IsEditable()){ 
        	if (bMsg) Msg("!.Can't detach object '%s'. Target '%s' in readonly mode.",(*it)->Name,Scene->GetMTools((*it)->ClassID)->ClassDesc());
        	res = false;
        }
    return res;
}

void CGroupObject::GroupObjects(ObjectList& lst)
{
	R_ASSERT(lst.size());
	for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
    	LL_AppendObject(*it,true);
    UpdatePivot(0,false);
}
void CGroupObject::UngroupObjects()
{
	if (!IsOpened()){
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
            (*it)->OnDetach();
    }
    m_Objects.clear();
}
void CGroupObject::OpenGroup()
{
	if (!IsOpened()){
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
            (*it)->OnDetach();
        m_Flags.set(flStateOpened,TRUE);
    }
}
void CGroupObject::CloseGroup()
{
	if (IsOpened()){
        m_Flags.set(flStateOpened,FALSE);										// последовательность очень важна!!! иначе удалим себя из списка
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
        	LL_AppendObject(*it,false);
    }
}
//----------------------------------------------------

