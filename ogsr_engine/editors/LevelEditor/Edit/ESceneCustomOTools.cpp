#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomOTools.h"
#include "../ECore/Editor/ui_main.h"
#include "scene.h"
#include "CustomObject.h"

ESceneCustomOTools::ESceneCustomOTools(ObjClassID cls):ESceneCustomMTools(cls)
{
}
//----------------------------------------------------

ESceneCustomOTools::~ESceneCustomOTools()
{
}
//----------------------------------------------------

void ESceneCustomOTools::UpdateSnapList()
{
}
//----------------------------------------------------

ObjectList*	ESceneCustomOTools::GetSnapList()
{
	return 0;
}
//----------------------------------------------------

BOOL ESceneCustomOTools::_AppendObject(CCustomObject* object)
{
    m_Objects.push_back(object);
    object->ParentTools = this;
    return TRUE;
}
//----------------------------------------------------

BOOL ESceneCustomOTools::_RemoveObject(CCustomObject* object)
{
	m_Objects.remove(object);
    return FALSE;
}
//----------------------------------------------------

void ESceneCustomOTools::Clear(bool bInternal)
{
	inherited::Clear	();
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	xr_delete(*it);
    m_Objects.clear();
}
//----------------------------------------------------

void ESceneCustomOTools::OnFrame()
{
	ObjectList remove_objects;
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	(*it)->OnFrame			();
        if ((*it)->IsDeleted())	remove_objects.push_back(*it);
    }
    bool need_undo = remove_objects.size();
    while (!remove_objects.empty()){
    	CCustomObject* O	= remove_objects.back();
        Scene->RemoveObject	(O,false);
        xr_delete			(O);
        remove_objects.pop_back();
    }
    if (need_undo) Scene->UndoSave();
}
//----------------------------------------------------

void ESceneCustomOTools::OnRender(int priority, bool strictB2F)
{
//	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
//    	(*it)->Render(priority,strictB2F);
}
//----------------------------------------------------

void ESceneCustomOTools::OnSynchronize()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnSynchronize();
}
//----------------------------------------------------

void ESceneCustomOTools::OnSceneUpdate()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnSceneUpdate();
}
//----------------------------------------------------

void ESceneCustomOTools::OnDeviceCreate()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnDeviceCreate();
}
//----------------------------------------------------

void ESceneCustomOTools::OnDeviceDestroy()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnDeviceDestroy();
}
//----------------------------------------------------

bool ESceneCustomOTools::Validate(bool)
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	if (!(*it)->Validate(true)) return false;
    return true;
}

bool ESceneCustomOTools::Valid()
{
	return true;
}

bool ESceneCustomOTools::IsNeedSave()
{
	return !m_Objects.empty();
}

void ESceneCustomOTools::OnObjectRemove(CCustomObject* O)
{
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        (*_F)->OnObjectRemove(O);
}

void ESceneCustomOTools::SelectObjects(bool flag)
{
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()){
            (*_F)->Select( flag );
        }
    UI->RedrawScene		();
}

void ESceneCustomOTools::RemoveSelection()
{
    ObjectIt _F = m_Objects.begin();
    while(_F!=m_Objects.end()){
        if((*_F)->Selected()){
            if ((*_F)->OnSelectionRemove()){
                ObjectIt _D = _F; _F++;
                CCustomObject* obj 	= *_D; 
                Scene->RemoveObject	(obj,false);
                xr_delete			(obj);
            }else{
                _F++;
            }
        }else{
            _F++;
        }
    }
	UI->RedrawScene		();
}

void ESceneCustomOTools::InvertSelection()
{
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()){
            (*_F)->Select(-1);
        }
        
    UI->RedrawScene		();
}

int ESceneCustomOTools::SelectionCount(bool testflag)
{
	int count = 0;

    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()	&& ((*_F)->Selected() == testflag)) count++;
        
    return count;
}

void ESceneCustomOTools::ShowObjects(bool flag, bool bAllowSelectionFlag, bool bSelFlag)
{
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++){
        if (bAllowSelectionFlag){
            if ((*_F)->Selected()==bSelFlag){
                (*_F)->Show( flag );
            }
        }else{
            (*_F)->Show( flag );
        }
    }
    UI->RedrawScene();
}

BOOL ESceneCustomOTools::RayPick(CCustomObject*& object, float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
	object = 0;
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()&&(*_F)->RayPick(distance,start,direction,pinf))
            object=*_F;
	return !!object;
}

BOOL ESceneCustomOTools::FrustumPick(ObjectList& lst, const CFrustum& frustum)
{
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()&&(*_F)->FrustumPick(frustum))
        	lst.push_back(*_F);
	return !lst.empty();
}

BOOL ESceneCustomOTools::SpherePick(ObjectList& lst, const Fvector& center, float radius)
{
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()&&(*_F)->SpherePick(center, radius))
        	lst.push_back(*_F);
	return !lst.empty();
}

int ESceneCustomOTools::RaySelect(int flag, float& distance, const Fvector& start, const Fvector& direction, BOOL bDistanceOnly)
{
    CCustomObject* nearest_object=0;
    if (RayPick(nearest_object,distance,start,direction,0)&&!bDistanceOnly) 
    	nearest_object->RaySelect(flag,start,direction,false);
//    	nearest_object->Select(flag);
    UI->RedrawScene();
    return !!nearest_object;
}

int ESceneCustomOTools::FrustumSelect(int flag, const CFrustum& frustum)
{
	ObjectList lst;

    FrustumPick(lst,frustum);
    for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
	    (*_F)->Select(flag);
    
	return 0;
}

int ESceneCustomOTools::GetQueryObjects(ObjectList& lst, int iSel, int iVis, int iLock)
{
	int count=0;
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++){
        if(	((iSel==-1)||((*_F)->Selected()==iSel))&&
            ((iVis==-1)||((*_F)->Visible()==iVis))&&
            ((iLock==-1)||((*_F)->Locked()==iLock))){
                lst.push_back(*_F);
                count++;
        	}
    }
    return count;
}

int ESceneCustomOTools::LockObjects(bool flag, bool bAllowSelectionFlag, bool bSelFlag)
{
	int count=0;
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if(bAllowSelectionFlag){
            if((*_F)->Selected()==bSelFlag){
                (*_F)->Lock( flag );
                count++;
            }
        }else{
            (*_F)->Lock( flag );
            count++;
        }
    return count;
}

CCustomObject* ESceneCustomOTools::FindObjectByName(LPCSTR name, CCustomObject* pass)
{
	ObjectIt _I = m_Objects.begin();
    ObjectIt _E = m_Objects.end();
	for(;_I!=_E;_I++) if((pass!=*_I)&&(0==strcmp((*_I)->Name,name))) return (*_I);
    return 0;
}

void ESceneCustomOTools::FillProp(LPCSTR pref, PropItemVec& items)
{
    for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)  
    	if ((*it)->Selected()) (*it)->FillProp	(PrepareKey(pref,"Items").c_str(),items);
}

bool ESceneCustomOTools::GetSummaryInfo(SSceneSummary* inf)
{
    for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->GetSummaryInfo(inf);
    return true;
}

void ESceneCustomOTools::GetBBox(Fbox& BB, bool bSelOnly)
{
	Fbox bb;
    ObjectList lst;
    if (GetQueryObjects(lst, bSelOnly, true, -1)){
        for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
            if ((*_F)->GetBox(bb)) BB.merge(bb);
    }
}

int ESceneCustomOTools::MultiRenameObjects()
{
	int cnt			= 0;
    for (ObjectIt o_it=m_Objects.begin(); o_it!=m_Objects.end(); o_it++){
    	CCustomObject* obj	= *o_it;
    	if (obj->Selected()){
            string256 			buf;
        	Scene->GenObjectName(obj->ClassID,buf,obj->RefName());
            if (obj->Name!=buf){
	            obj->Name		= buf;
                cnt++; 
            }
        }
    }
    return cnt;
}


