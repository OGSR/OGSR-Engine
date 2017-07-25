//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditLibrary.h"
#include "../ECore/Editor/Library.h"
#include "ui_leveltools.h"
#include "../ECore/Editor/EditObject.h"
#include "Main.h"
#include "Scene.h"
#include "Builder.h"
#include "../ECore/Engine/Texture.h"
#include "BottomBar.h"
#include "../ECore/Editor/ImageManager.h"
#include "../ECore/Editor/EThumbnail.h"
#include "ESceneDOTools.h"
#include "xr_trims.h"
#include "SceneObject.h"
#include "../ECore/Engine/Image.h"
#include "../ECore/Editor/ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "ElHeader"
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "mxPlacemnt"
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "mxPlacemnt"
#pragma link "MxMenus"
#pragma link "ElTreeAdvEdit"
#pragma link "MXCtrls"
#pragma resource "*.dfm"

TfrmEditLibrary* TfrmEditLibrary::form=0;
FS_FileSet TfrmEditLibrary::modif_map;
bool TfrmEditLibrary::bFinalExit = false;
bool TfrmEditLibrary::bExitResult= true;

//---------------------------------------------------------------------------
__fastcall TfrmEditLibrary::TfrmEditLibrary(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
	m_pEditObject 	= xr_new<CSceneObject>((LPVOID)0,(LPSTR)0);
    m_Props 		= TfrmPropertiesEObject::CreateProperties(0,alNone,TOnModifiedEvent(this,&TfrmEditLibrary::OnModified));
    m_Items			= TItemList::CreateForm("Objects",paItems,alClient,TItemList::ilEditMenu|TItemList::ilDragAllowed|TItemList::ilFolderStore);
    m_Items->SetOnItemFocusedEvent	(fastdelegate::bind<TOnILItemFocused>(this,&TfrmEditLibrary::OnItemFocused));
    m_Items->SetOnItemRemoveEvent	(fastdelegate::bind<TOnItemRemove>(&Lib,&ELibrary::RemoveObject));
    m_Items->SetOnItemRenameEvent	(fastdelegate::bind<TOnItemRename>(&Lib,&ELibrary::RenameObject));
    bReadOnly		= false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ShowEditor()
{
	if (!form){
    	form = xr_new<TfrmEditLibrary>((TComponent*)0);
		Scene->lock();
    }
    form->Show();
}
//---------------------------------------------------------------------------
CSceneObject* __fastcall TfrmEditLibrary::RayPick(const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
	if (!form) return 0;
	if (form->cbPreview->Checked){
    	float dist=UI->ZFar();
        if (form->m_pEditObject->RayPick(dist,start,direction,pinf)){
        	R_ASSERT(pinf&&pinf->e_mesh&&pinf->e_obj);
        	form->m_Props->OnPick(*pinf);
			pinf->s_obj=form->m_pEditObject;
	        return form->m_pEditObject;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::OnRender()
{
	if (!form) return;
	if (form->cbPreview->Checked){
    	CSceneObject*		S = form->m_pEditObject;
    	CEditableObject* 	O = form->m_pEditObject->GetReference();
        if (O){
	        if (!S->PPosition.similar(O->t_vPosition)) 	S->PPosition 	= O->t_vPosition;
    	    if (!S->PRotation.similar(O->t_vRotate))	S->PRotation 	= O->t_vRotate;
        	if (!S->PScale.similar(O->t_vScale))		S->PScale		= O->t_vScale;
	        form->m_pEditObject->OnFrame();
    	    form->m_pEditObject->RenderSingle();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ZoomObject()
{
	if (!form) return;
	if (form->cbPreview->Checked){
        // get bbox&transform
		Fbox bb;
        if (form->m_pEditObject->GetBox(bb))
    	    Device.m_Camera.ZoomExtents(bb);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormShow(TObject *Sender)
{
	UI->BeginEState(esEditLibrary);
	modif_map.clear();

    InitObjects();
    ebSave->Enabled = false;
    // add directional light
    Flight L;
    ZeroMemory(&L,sizeof(Flight));
    L.type = D3DLIGHT_DIRECTIONAL;
    L.diffuse.set(1,1,1,1);
    L.direction.set(1,-1,1); L.direction.normalize();
	Device.SetLight(0,L);
	Device.LightEnable(0,true);
    L.diffuse.set(0.5,0.5,0.5,1);
    L.direction.set(1,-1,-1); L.direction.normalize();
	Device.SetLight(1,L);
	Device.LightEnable(1,true);

	// check window position
	UI->CheckWindowPos(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;

    if (!bFinalExit&&ebSave->Enabled){
	    bFinalExit = false;
        UI->SetStatus("Objects reloading...");
        FS_FileSetIt it=modif_map.begin();
		FS_FileSetIt _E=modif_map.end();
        for (;it!=_E;it++)
        	Lib.ReloadObject(it->name.c_str());
        UI->ResetStatus();
    }
	Scene->unlock();

    UI->EndEState(esEditLibrary);

    // remove directional light                             
	Device.LightEnable(0,false);
	Device.LightEnable(1,false);

    xr_delete(form->m_pEditObject);
	xr_delete(m_Thm);

    
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormDestroy(TObject *Sender)
{
	TItemList::DestroyForm(m_Items);
    TfrmPropertiesEObject::DestroyProperties(m_Props);

	form = 0;

    ExecCommand(COMMAND_CLEAR);
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = true;
    if (ebSave->Enabled){
    	int res = ELog.DlgMsg(mtConfirmation, "Library was change. Do you want save?");
		if (res==mrCancel) CanClose = false;
		if (res==mrYes) ebSaveClick(0);
    }
    bExitResult = CanClose;
}
//---------------------------------------------------------------------------
bool TfrmEditLibrary::FinalClose()
{
	if (!form) return true;
    bFinalExit = true;
    form->Close();
    return bExitResult;
}
//---------------------------------------------------------------------------
void TfrmEditLibrary::OnModified()
{
	if (!form) return;
    form->ebSave->Enabled = true;
    CEditableObject* E=form->m_pEditObject->GetReference();
    if (E){
    	modif_map.insert(FS_File(E->GetName()));
    	E->Modified();
		form->m_pEditObject->UpdateTransform();
    }
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::OnItemFocused(TElTreeItem* item)
{
	xr_delete(m_Thm);
    bool mt=false;
    if (item&&FHelper.IsObject(item)&&UI->ContainEState(esEditLibrary)){
        // change thm
        ListItem* prop 	= (ListItem*)item->Tag; VERIFY(prop);
        AnsiString nm	= prop->Key();
        string_path 	obj_fn,thm_fn;

        FS.update_path			(obj_fn,_objects_,ChangeFileExt(nm,".object").c_str());
        bReadOnly				= !FS.can_modify_file(obj_fn);
        if (bReadOnly)
        	ELog.Msg			(mtError,"You don't have permisions to modify object: '%s'",nm.c_str());
        ebRenameObject->Enabled = !bReadOnly;
        ebRemoveObject->Enabled = !bReadOnly;
//		ebExportLWO->Enabled 	= !bReadOnly;

        FS.update_path			(thm_fn,_objects_,ChangeFileExt(nm,".thm").c_str());
        if (FS.exist(thm_fn))
        {
        	// если версии совпадают
            int obj_age 		= FS.get_file_age(obj_fn);
            int thm_age 		= FS.get_file_age(thm_fn);
            m_Thm 				= xr_new<EObjectThumbnail>(nm.c_str());
            if (obj_age&&(obj_age==thm_age)){
            }else{
            	ELog.Msg(mtError,"Update object thumbnail. Stale data.");
            }
        }

        if (cbPreview->Checked||m_Props->Visible){
//.        	if (m_Props->IsModified()&&m_pEditObject->GetReference())
//.            	modif_map.insert(mk_pair(m_pEditObject->GetRefName(),FS_QueryItem(0,0,0)));
            ChangeReference(nm.c_str());
		    if (cbPreview->Checked) mt = true;
        }
    }else{
		ChangeReference(0);
    }
    if (m_Thm&&m_Thm->Valid()){
        lbFaces->Caption 	= m_Thm->_FaceCount()?AnsiString(m_Thm->_FaceCount()):AnsiString("?");
        lbVertices->Caption = m_Thm->_VertexCount()?AnsiString(m_Thm->_VertexCount()):AnsiString("?");
    }else{
        lbFaces->Caption 	= "?";
        lbVertices->Caption = "?";
    }
    paImage->Repaint	();
    UpdateObjectProperties();
    UI->RedrawScene();
    ebMakeThm->Enabled	= mt;
    ebMakeLOD_high->Enabled	= cbPreview->Checked;
    ebMakeLOD_low->Enabled	= cbPreview->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::cbPreviewClick(TObject *Sender)
{
	RefreshSelected();
}
//---------------------------------------------------------------------------

void TfrmEditLibrary::InitObjects()
{
	ListItemsVec items;
    FS_FileSet lst;
    Lib.GetObjects(lst);
    FS_FileSetIt it=lst.begin();
    FS_FileSetIt _E=lst.end();
    for(; it!=_E; it++)
    	LHelper().CreateItem(items,it->name.c_str(),0,0,0);
    m_Items->AssignItems(items,false,true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Shift.Contains(ssCtrl)){
    	if (Key==VK_CANCEL)		ExecCommand(COMMAND_BREAK_LAST_OPERATION);
    }else{
        if (Key==VK_ESCAPE){
            if (bFormLocked)	ExecCommand(COMMAND_BREAK_LAST_OPERATION);
            else				ebCancel->Click();
            Key = 0; // :-) нужно для того чтобы AccessVoilation не вылазил по ESCAPE
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditLibrary::ebPropertiesClick(TObject *Sender)
{
	TElTreeItem* node = m_Items->GetSelected();
    if (node&&FHelper.IsObject(node)){
	    AnsiString name;
    	FHelper.MakeName(node,0,name,false);
        ChangeReference	(name.c_str());
        UpdateObjectProperties();
        m_Props->ShowProperties();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebSaveClick(TObject *Sender)
{
	ebSave->Enabled 	= false;
    ChangeReference		(0);
    Lib.Save			(&modif_map);
    modif_map.clear 	();
    RefreshSelected		();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebCancelClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::tvItemsDblClick(TObject *Sender)
{
	ebPropertiesClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebMakeThmClick(TObject *Sender)
{
	U32Vec pixels;
    TElTreeItem* node = m_Items->GetSelected();
	if (node&&FHelper.IsObject(node)){
    	AnsiString name; FHelper.MakeName(node,0,name,false);
   	    CEditableObject* obj = Lib.CreateEditObject(name.c_str());
    	if (obj&&cbPreview->Checked){
            AnsiString tex_name;
            tex_name = ChangeFileExt(obj->GetName(),".thm");
            if (ImageLib.CreateOBJThumbnail(tex_name.c_str(),obj,obj->Version())){
	            ELog.Msg(mtInformation,"Thumbnail successfully created.");
                AnsiString full_name;
                FHelper.MakeFullName(node,0,full_name);
                m_Items->SelectItem(full_name.c_str(),true,false,true);
            }
	    }else{
            ELog.DlgMsg(mtError,"Can't create thumbnail. Set preview mode.");
        }
		Lib.RemoveEditObject(obj);
    }
}
//---------------------------------------------------------------------------

bool TfrmEditLibrary::GenerateLOD(ListItem* prop, bool bHighQuality)
{
	VERIFY				(prop);	
    xr_string nm		= prop->Key();
    ChangeReference		(nm.c_str());
    CEditableObject* O 	= m_pEditObject->GetReference();
    if (O&&O->IsMUStatic()){
        BOOL bLod 	= O->m_Flags.is(CEditableObject::eoUsingLOD);
        O->m_Flags.set(CEditableObject::eoUsingLOD,FALSE);
        xr_string tex_name;
        tex_name 	= EFS.ChangeFileExt(nm,"");
        string512 tmp; strcpy(tmp,tex_name.c_str()); _ChangeSymbol(tmp,'\\','_');
        tex_name 	= xr_string("lod_")+tmp;
        tex_name 	= ImageLib.UpdateFileName(tex_name);
        ImageLib.CreateLODTexture(O, tex_name.c_str(),LOD_IMAGE_SIZE,LOD_IMAGE_SIZE,LOD_SAMPLE_COUNT,O->Version(),bHighQuality?7:1);
        O->OnDeviceDestroy();
        O->m_Flags.set(CEditableObject::eoUsingLOD,bLod);
        ELog.Msg(mtInformation,"LOD for object '%s' successfully created.",O->GetName());
        return true;
    }else{
        ELog.Msg(mtError,"Can't create LOD texture from non 'Multiple Usage' object.");
    }
    return false;
}
//---------------------------------------------------------------------------

void TfrmEditLibrary::MakeLOD(bool bHighQuality)
{
	if (ebSave->Enabled){
    	ELog.DlgMsg				(mtError,"Save library changes before generating LOD.");
        return;
    }
	TElTreeItem* node 			= m_Items->GetSelected();
	if (node&&cbPreview->Checked){
        LockForm();
    	int res 				= ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo << mbCancel,"Do you want to select multiple objects?");
        if (res!=mrCancel){
            if (res==mrYes){
                LPCSTR new_val		= 0;
                if (TfrmChoseItem::SelectItem(smObject,new_val,256)){
                    int cnt			= _GetItemCount(new_val);
                    int iLODcnt 	= 0;
                    SPBItem* pb 	= UI->ProgressStart(cnt,"Making LOD");
                    for (int k=0; k<cnt; k++){
                        xr_string 	tmp;
                        _GetItem	(new_val,k,tmp);
                        pb->Inc		(tmp.c_str());
                        ListItem* I	= m_Items->FindItem(tmp.c_str()); VERIFY(I);
                        if (GenerateLOD(I,bHighQuality)) iLODcnt++;
                        if (UI->NeedAbort()) break;
                    }
                    ELog.DlgMsg 	(mtInformation,"'%d' LOD's succesfully created.",iLODcnt);
                    UI->ProgressEnd	(pb);
                }
            }else{
                AnsiString m_LastSelection;
                FHelper.MakeFullName(node,0,m_LastSelection);
                if (FHelper.IsObject(node)){
                    ListItem* prop 	= (ListItem*)node->Tag; VERIFY(prop);
                    GenerateLOD		(prop,bHighQuality);
                } if (FHelper.IsFolder(node)){
                    if (mrYes==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to generate LOD for all object in this folder?")){
                        int iLODcnt = 0;
                        SPBItem* pb = UI->ProgressStart(node->ChildrenCount,"Making LOD");
                        for (TElTreeItem* item=node->GetFirstChild(); item; item=node->GetNextChild(item)){
                            ListItem* prop 	= (ListItem*)node->Tag; VERIFY(prop);
                            pb->Inc	(prop->Key());
                            if (GenerateLOD(prop,bHighQuality)) iLODcnt++;
                            if (UI->NeedAbort()) break;
                        }
                        UI->ProgressEnd(pb);
                        ELog.DlgMsg 	(mtInformation,"'%d' LOD's succesfully created.",iLODcnt);
                    }
                }
            }
        }
        UnlockForm();
//        m_Items->SelectItem	(m_LastSelection.c_str(),true,false,true);
    }
}

void __fastcall TfrmEditLibrary::ebMakeLOD_highClick(TObject *Sender)
{
	MakeLOD(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebMakeLOD_lowClick(TObject *Sender)
{
	MakeLOD(false);
}
//---------------------------------------------------------------------------

void TfrmEditLibrary::ChangeReference(LPCSTR new_name)
{
	// save new position
	CEditableObject* E=m_pEditObject->GetReference();
    if (E&&new_name&&(stricmp(E->GetName(),new_name))==0) return;
    if (E){
		E->t_vPosition.set	(m_pEditObject->PPosition);
		E->t_vScale.set		(m_pEditObject->PScale);
		E->t_vRotate.set	(m_pEditObject->PRotation);
    }
    m_pEditObject->SetReference(new_name);
    // get old position
	E=m_pEditObject->GetReference();
    if (E){
		m_pEditObject->PPosition 	= E->t_vPosition;
		m_pEditObject->PScale 		= E->t_vScale;
		m_pEditObject->PRotation	= E->t_vRotate;
    }
    // update transformation
    m_pEditObject->UpdateTransform();
    ExecCommand(COMMAND_EVICT_OBJECTS);
    ExecCommand(COMMAND_EVICT_TEXTURES);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ResetSelected()
{
	if (form){
    	form->m_pEditObject->SetReference(0);
//	    TfrmPropertiesObject::HideProperties();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::RefreshSelected()
{
	if (form){
//    	form->m_pEditObject->SetReference(0);
//		Lib.RemoveEditObject(form->m_SelectedObject);
//.		form->tvItemsItemFocused(0);
        TElTreeItem *node = m_Items->GetSelected();
        bool mt=false;
        if (cbPreview->Checked&&node&&FHelper.IsObject(node)){
            AnsiString name;
            FHelper.MakeName(node,0,name,false);
            ChangeReference(name.c_str());
            mt = true;
        }
        ebMakeThm->Enabled 		= !bReadOnly&&mt;
        ebMakeLOD_high->Enabled = !bReadOnly&&cbPreview->Checked;
        ebMakeLOD_low->Enabled 	= !bReadOnly&&cbPreview->Checked;
        UI->RedrawScene();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::paImagePaint(TObject *Sender)
{
    if (m_Thm) m_Thm->Draw(paImage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebExportLWOClick(TObject *Sender)
{
    TElTreeItem* node = m_Items->GetSelected();
    if (node&&FHelper.IsObject(node)){
    	AnsiString name; FHelper.MakeName(node,0,name,false);
        xr_string save_nm;
        if (EFS.GetSaveName(_import_,save_nm,0,1)){
            CEditableObject* obj = Lib.CreateEditObject(name.c_str());
            if (obj){
                if (!obj->ExportLWO(save_nm.c_str())){
                    ELog.DlgMsg(mtInformation, "Can't export object '%s'.", obj->GetName());
                }else{
					ELog.DlgMsg(mtInformation, "Export complete.");
                }
            }else{
            	ELog.DlgMsg(mtError,"Can't load object.");
        	}
			Lib.RemoveEditObject(obj);
	    }
    }else{
        ELog.DlgMsg(mtInformation, "Select object to export.");
    }
}

//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebImportClick(TObject *Sender)
{
    xr_string open_nm, save_nm, nm;
    if (EFS.GetOpenName(_import_,open_nm,true)){
    	// remove selected object
        ResetSelected();
		// load
    	AStringVec lst;
        _SequenceToList(lst,open_nm.c_str());
		bool bNeedUpdate=false;
        // folder name
        AnsiString folder;
        TElTreeItem* item = m_Items->GetSelected(); 
        if (item) FHelper.MakeName(item,0,folder,true);
        //
//		AnsiString path; // нужен при multi-open для сохранения последнего пути
		xr_string m_LastSelection;
        for (AStringIt it=lst.begin(); it!=lst.end(); it++){
        	nm = ChangeFileExt(ExtractFileName(*it),"").c_str();
            CEditableObject* O = xr_new<CEditableObject>(nm.c_str());
            if (O->Load(it->c_str())){
                save_nm = xr_string(FS.get_path(_objects_)->m_Path)+folder.c_str()+EFS.ChangeFileExt(nm,".object");

                if (FS.exist(save_nm.c_str()))
			        if (mrNo==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Object '%s' already exist. Owerwrite it?",nm.c_str())){
                    	xr_delete(O);
                    	break;
                    }
                
                O->SaveObject	(save_nm.c_str());
                EFS.MarkFile	(it->c_str(),true);
                bNeedUpdate		= true;
            }else{
            	ELog.DlgMsg(mtError,"Can't load file '%s'.",it->c_str());
            }
            xr_delete(O);

            LPCSTR p = FS.get_path(_objects_)->m_Path;
            if (folder.Pos(p)>0){ 
            	m_LastSelection = xr_string(folder.c_str()+strlen(p))+nm;
                xr_strlwr		(m_LastSelection);
            }else{
            	m_LastSelection = xr_string(folder.c_str())+nm;
            }
        }
        if (bNeedUpdate){
			Lib.CleanLibrary	();
			InitObjects			();
            m_Items->SelectItem	(m_LastSelection.c_str(),true,false,true);
        }
        // refresh selected
//		RefreshSelected();
    }
}
//---------------------------------------------------------------------------

void TfrmEditLibrary::UpdateObjectProperties()
{
	m_Props->UpdateProperties(m_pEditObject,bReadOnly);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::FormActivate(TObject *Sender)
{
	m_Items->SetILFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::OnObjectRename(LPCSTR p0, LPCSTR p1, EItemType type)
{
	Lib.RenameObject(p0,p1,type);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::fsStorageRestorePlacement(TObject *Sender)
{
	m_Items->LoadSelection(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::fsStorageSavePlacement(TObject *Sender)
{
	m_Items->SaveSelection(fsStorage);
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditLibrary::ebRenameObjectClick(TObject *Sender)
{
	m_Items->RenameSelItem();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebRemoveObjectClick(TObject *Sender)
{
	m_Items->RemoveSelItems();	
}
//---------------------------------------------------------------------------


