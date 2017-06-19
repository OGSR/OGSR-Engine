#include "stdafx.h"
#pragma hdrstop

#include "ObjectList.h"
#include "GroupObject.h"
#include "ui_leveltools.h"
#include "Scene.h"
#include "../ECore/Editor/ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TfrmObjectList* TfrmObjectList::CreateForm(TWinControl* parent)
{
	TfrmObjectList* OL=xr_new<TfrmObjectList>(parent);
    if (parent) OL->Parent = parent;
	return OL;
}

void TfrmObjectList::DestroyForm(TfrmObjectList*& obj_list)
{
	VERIFY(obj_list);
    xr_delete(obj_list);
}

void __fastcall TfrmObjectList::ShowObjectList()
{
	Show();
}

void __fastcall TfrmObjectList::ShowObjectListModal()
{
	ShowModal();
}

void __fastcall TfrmObjectList::HideObjectList()
{
	Hide();
}

void __fastcall TfrmObjectList::UpdateObjectList()
{
	if (Visible&&!bLockUpdate) sbRefreshListClick(0);
}

//---------------------------------------------------------------------------
__fastcall TfrmObjectList::TfrmObjectList(TComponent* Owner)
    : TForm(Owner)
{
	bLockUpdate = false;
}

void __fastcall TfrmObjectList::sbCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::FormShow(TObject *Sender)
{
    obj_count = 0;
    cur_cls = OBJCLASS_DUMMY;
	tvItems->FilteredVisibility = ((rgSO->ItemIndex==1)||(rgSO->ItemIndex==2));
    InitListBox();
	// check window position
    UI->CheckWindowPos(this);
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmObjectList::FindFolderByType(int type)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Parent && (node->Data == (void*)type)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmObjectList::FindObjectByType(int type, void *obj)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Parent && (node->Data == (void*)type)){
            for (TElTreeItem* chield = node->GetFirstChild(); chield; chield = node->GetNextChild(chield))
                if (chield->Data==obj) return chield;
            break;
        }
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmObjectList::AddFolder(ObjClassID type)
{
    AnsiString name;
    name.sprintf("%ss",Scene->GetMTools(type)->ClassDesc());
    TElTreeItem* node = tvItems->Items->AddObject(0,AnsiString(name).LowerCase().c_str(),(void*)type);
    node->ParentStyle = false;
    node->Bold = true;
    return node;
}

TElTreeItem* TfrmObjectList::AddObject(TElTreeItem* node, LPCSTR name, void* obj)
{
    return tvItems->Items->AddChildObject(node, AnsiString(name).LowerCase().c_str(), obj);
}

void __fastcall TfrmObjectList::InitListBox()
{
    tvItems->IsUpdating = true;
    tvItems->Items->Clear();
    cur_cls = LTools->CurrentClassID();
    for(SceneToolsMapPairIt it=Scene->FirstTools(); it!=Scene->LastTools(); it++){
    	ESceneCustomOTools* ot = dynamic_cast<ESceneCustomOTools*>(it->second);
        if (ot&&((cur_cls==OBJCLASS_DUMMY)||(it->first==cur_cls))){
        	if (it->first==OBJCLASS_DUMMY) continue;
            TElTreeItem* node = FindFolderByType(it->first);
            if (!node) node = AddFolder(it->first);
            VERIFY(node);
            ObjectList& lst = ot->GetObjects();
            if (OBJCLASS_GROUP==it->first){
                for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
                    TElTreeItem* grp_node = AddObject(node,(*_F)->Name,(void*)(*_F));
                    ObjectList& grp_lst = ((CGroupObject*)(*_F))->GetObjects();
                    for (ObjectIt _G=grp_lst.begin(); _G!=grp_lst.end(); _G++)
                        AddObject(grp_node,(*_G)->Name,(void*)(*_F));
                }
            }else{
                for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                    AddObject(node,(*_F)->Name,(void*)(*_F));
            }
        }
    }
    tvItems->Sort			(true);
    tvItems->IsUpdating 	= false;

    UpdateState();
    tvItems->FullExpand		();      
    obj_count 	= Scene->ObjCount();
}

void TfrmObjectList::UpdateState()
{
    tvItems->IsUpdating 	= true;

	tvItems->OnItemSelectedChange = 0;

    TElTreeItem* sel_node 	= 0;
    bool need_sort			= false;
    
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext()){
        if (node&&(node->Level>0)){
        	CCustomObject* O = (CCustomObject*)node->Data;
            node->ParentStyle = false;
            node->StrikeOut = !O->Visible();
            if (rgSO->ItemIndex==1) 	node->Hidden 	= !O->Visible();
            else if (rgSO->ItemIndex==2)node->Hidden 	= O->Visible();
            if (O->Visible())			node->Selected	= O->Selected();
            if (O->Selected())			sel_node		= node;
            if (!node->Hidden){
            	if (AnsiString(node->Text)!=O->Name){
	            	node->Text 			= O->Name;
                    need_sort			= true;
                }
            }
        }
    }

    tvItems->IsUpdating = false;

    if (need_sort) tvItems->Sort		(true);
    
    if (sel_node) tvItems->EnsureVisible(sel_node);
}                                                

void TfrmObjectList::UpdateSelection()
{
	if (tvItems->Items->Count){
        bLockUpdate = true;

        Scene->SelectObjects( false, (ObjClassID)cur_cls );
        for (TElTreeItem* node = tvItems->GetNextSelected(0); node; node=tvItems->GetNextSelected(node))
            if (node->Parent) ((CCustomObject*)(node->Data))->Select(true);
        UI->RedrawScene();

        bLockUpdate = false;
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmObjectList::ebHideSelClick(TObject *Sender)
{
    for (TElTreeItem* node = tvItems->GetNextSelected(0); node; node=tvItems->GetNextSelected(node))
        if (node->Parent) ((CCustomObject*)(node->Data))->Show(false);
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::ebShowSelClick(TObject *Sender)
{
    for (TElTreeItem* node = tvItems->GetNextSelected(0); node; node=tvItems->GetNextSelected(node))
        if (node->Parent){
            ((CCustomObject*)(node->Data))->Show	(true);
            ((CCustomObject*)(node->Data))->Select	(true);
        }
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::ebShowPropertiesClick(TObject *Sender)
{
	ExecCommand	(COMMAND_SHOW_PROPERTIES);
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::sbRefreshListClick(TObject *Sender)
{
    if ((Scene->ObjCount()!=obj_count)||(cur_cls!=LTools->CurrentClassID()))
	    InitListBox();
    else
    	UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::tmRefreshListTimer(TObject *Sender)
{
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    tvItems->IsUpdating 	= true;
    tvItems->Items->Clear	();
    tvItems->IsUpdating 	= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) sbClose->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::rgSOClick(TObject *Sender)
{
	UpdateState();
	tvItems->FilteredVisibility = ((rgSO->ItemIndex==1)||(rgSO->ItemIndex==2));
}
//---------------------------------------------------------------------------

extern bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails);
//---------------------------------------------------------------------------
void __fastcall TfrmObjectList::tvItemsKeyPress(TObject *Sender,
      char &Key)
{
	if (Key==VK_RETURN){
		ExecCommand	(COMMAND_SHOW_PROPERTIES);
    }else{
		TElTreeItem* node = tvItems->Items->LookForItemEx(tvItems->Selected,-1,false,false,false,&Key,LookupFunc);
    	if (!node) node = tvItems->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);
	    FHelper.RestoreSelection(tvItems,node,false);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::tvItemsItemFocused(TObject *Sender)
{
    UpdateSelection();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::tvItemsAfterSelectionChange(
      TObject *Sender)
{
    UpdateSelection();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::tvItemsDblClick(TObject *Sender)
{
	ExecCommand	(COMMAND_SHOW_PROPERTIES);
}
//---------------------------------------------------------------------------

