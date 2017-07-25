#include "stdafx.h"
#pragma hdrstop

#include "ESceneGroupTools.h"
#include "ESceneGroupControls.h"
#include "ui_leveltools.h"
#include "FrameGroup.h"
#include "Scene.h"
#include "GroupObject.h"
#include "../ECore/Editor/EThumbnail.h"

void ESceneGroupTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
    AddControl		(xr_new<TUI_ControlGroupAdd >(estDefault,etaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraGroup>((TComponent*)0,this);
}
//----------------------------------------------------
 
void ESceneGroupTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

void ESceneGroupTools::UngroupObjects(bool bUndo)
{
    ObjectList lst 	= m_Objects;
    int sel_cnt		= 0;
    if (!lst.empty()){
    	bool bModif	= false;
        for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
        	if ((*it)->Selected()){
            	sel_cnt++;
            	CGroupObject* obj 	= dynamic_cast<CGroupObject*>(*it); VERIFY(obj);
                if (obj->CanUngroup(true)){
                    Scene->RemoveObject	(obj,false);
                    obj->UngroupObjects	();
                    xr_delete			(obj);
                    bModif				= true;
                }else{
                	ELog.DlgMsg			(mtError,"Can't ungroup object: '%s'.",obj->Name);
                }
            }
        }
	    if (bUndo&&bModif) Scene->UndoSave();
    }
    if (0==sel_cnt)	ELog.Msg		(mtError,"Nothing selected.");
}
//----------------------------------------------------

void ESceneGroupTools::GroupObjects(bool bUndo)
{
	string256 namebuffer;
	Scene->GenObjectName(OBJCLASS_GROUP, namebuffer);
    CGroupObject* group = xr_new<CGroupObject>((LPVOID)0,namebuffer);

	// validate objects
    ObjectList lst;
    if (Scene->GetQueryObjects(lst,OBJCLASS_DUMMY,1,1,0)) group->GroupObjects(lst);
    if (group->ObjectCount()){
	    ELog.DlgMsg(mtInformation,"Group '%s' successfully created.\nContain %d object(s)",group->Name,group->ObjectCount());
        Scene->AppendObject(group,bUndo);
    }else{
	    ELog.DlgMsg(mtError,"Group can't created.");
        xr_delete(group);
    }
}
//----------------------------------------------------

void ESceneGroupTools::OpenGroups(bool bUndo)
{
    ObjectList lst 	= m_Objects;
    int sel_cnt		= 0;
    if (!lst.empty()){
    	bool bModif	= false;
        for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
        	if ((*it)->Selected()){
            	sel_cnt++;
            	CGroupObject* obj 	= dynamic_cast<CGroupObject*>(*it); VERIFY(obj);
                if (obj->CanUngroup(true)){
                    obj->OpenGroup	();
                    bModif			= true;
                }else{
                    ELog.DlgMsg		(mtError,"Can't open group: '%s'.",obj->Name);    
                }
            }
        }
	    if (bUndo&&bModif) Scene->UndoSave();
    }
    if (0==sel_cnt)	ELog.Msg		(mtError,"Nothing selected.");
}
//----------------------------------------------------

void ESceneGroupTools::CloseGroups(bool bUndo)
{
    ObjectList lst 	= m_Objects;
    int sel_cnt		= 0;
    if (!lst.empty()){
        for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
        	if ((*it)->Selected()){
            	sel_cnt++;
                ((CGroupObject*)(*it))->CloseGroup();
            }
        }
	    if (bUndo) Scene->UndoSave();
    }
    if (0==sel_cnt)	ELog.Msg		(mtError,"Nothing selected.");
}
//----------------------------------------------------

void ESceneGroupTools::CenterToGroup()
{
    ObjectList& lst 	= m_Objects;
    if (!lst.empty()){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
			((CGroupObject*)(*it))->UpdatePivot(0,true);
        Scene->UndoSave();
    }
}
//----------------------------------------------------

void __stdcall  FillGroupItems(ChooseItemVec& items, void* param)
{
	CGroupObject* group = (CGroupObject*)param;
    for (ObjectIt it=group->GetObjects().begin(); it!=group->GetObjects().end(); it++){
	    items.push_back	(SChooseItem((*it)->Name,""));
    }
}

void ESceneGroupTools::AlignToObject()
{
    ObjectList& lst 	= m_Objects;
    int sel_cnt		= 0;
    if (!lst.empty()){
        LPCSTR nm;
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
        	if ((*it)->Selected()){
			    sel_cnt++;
                if (TfrmChoseItem::SelectItem(smCustom,nm,1,nm,FillGroupItems,*it)){
                    ((CGroupObject*)(*it))->UpdatePivot(nm,false);
                }else break;
            }
        }
        Scene->UndoSave();
    }
    if (0==sel_cnt)	ELog.Msg		(mtError,"Nothing selected.");
}
//----------------------------------------------------

CCustomObject* ESceneGroupTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= xr_new<CGroupObject>(data,name);
    O->ParentTools		= this;
    return O;
}
//----------------------------------------------------

void ESceneGroupTools::ReloadRefsSelectedObject()
{
    ObjectList lst 	= m_Objects;
    int sel_cnt		= 0;
    if (!lst.empty()){
    	bool bModif	= false;
        for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
        	if ((*it)->Selected()){
			    sel_cnt++;
            	CGroupObject* obj 	= dynamic_cast<CGroupObject*>(*it); VERIFY(obj);
                if (obj->UpdateReference()){
                    bModif			= true;
                }else{
                    ELog.Msg		(mtError,"Can't reload group: '%s'.",obj->Name);    
                }
            }
        }
	    if (bModif) Scene->UndoSave();
    }
    if (0==sel_cnt)	ELog.Msg		(mtError,"Nothing selected.");
}
//----------------------------------------------------

void ESceneGroupTools::SaveSelectedObject()
{
	CGroupObject* obj	= 0;
	// find single selected object
    for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	if ((*it)->Selected()){ 
        	if (obj!=0){
            	ELog.DlgMsg(mtError,"Select single object and try again.");
                return;
            }
        	obj 		= dynamic_cast<CGroupObject*>(*it);
        }
    }
    if (obj){
    	if (obj->IsOpened()){	
	        ELog.DlgMsg(mtError,"Can't save opened group. Close group and retry.");
        }else{
            xr_string fn;
            if (EFS.GetSaveName(_groups_,fn)){
                IWriter* W 		= FS.w_open(fn.c_str());
                if (W){
	                obj->Save	(*W);
    	            FS.w_close	(W);
                }else{
			        ELog.DlgMsg	(mtError,"Access denied.");
                }
            }                                 	
        }
    }else{
        ELog.DlgMsg(mtError,"Nothing selected.");
    }
}
//----------------------------------------------------

void ESceneGroupTools::SetCurrentObject(LPCSTR nm)
{
	m_CurrentObject				= nm;
	TfraGroup* frame			=(TfraGroup*)pFrame;
    frame->lbCurrent->Caption 	= m_CurrentObject.c_str();
}
//----------------------------------------------------

void ESceneGroupTools::OnActivate()
{
	inherited::OnActivate		();
	TfraGroup* frame			= (TfraGroup*)pFrame;
    frame->lbCurrent->Caption 	= m_CurrentObject.c_str();
}
//----------------------------------------------------

void ESceneGroupTools::MakeThumbnail()
{
	if (SelectionCount(true)==1){
	    CGroupObject* object		= 0;
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        	if ((*it)->Selected()){
	            object				= dynamic_cast<CGroupObject*>(*it); 
                break;
            }
        }
        VERIFY						(object);
        object->Select				(false);
        // save render params
        Flags32 old_flag			= 	psDeviceFlags;
        // set render params
        psDeviceFlags.set			(rsStatistic|rsDrawGrid,FALSE);

        U32Vec pixels;
        u32 w=512,h=512;
        if (Device.MakeScreenshot	(pixels,w,h)){
            AnsiString tex_name		= ChangeFileExt(object->Name,".thm");
            SStringVec lst;
            for (ObjectIt it=object->GetObjects().begin(); it!=object->GetObjects().end(); it++)
                lst.push_back		((*it)->Name);
            EGroupThumbnail 		tex	(tex_name.c_str(),false);
            tex.CreateFromData		(pixels.begin(),w,h,lst);
            string_path fn;
            FS.update_path			(fn,_groups_,object->RefName());
            strcat					(fn,".group");
            tex.Save				(FS.get_file_age(fn));
        }else{
            ELog.DlgMsg				(mtError,"Can't make screenshot.");
        }
        object->Select				(true);

        // restore render params
        psDeviceFlags 				= old_flag;
    }else{
    	ELog.DlgMsg		(mtError,"Select 1 GroupObject.");
    }
}
//----------------------------------------------------

