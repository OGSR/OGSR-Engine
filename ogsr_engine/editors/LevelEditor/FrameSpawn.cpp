#include "stdafx.h"
#pragma hdrstop

#include "ui_leveltools.h"
#include "../ECore/Editor/ui_main.h"
#include "FrameSpawn.h"
#include "Scene.h"
#include "SpawnPoint.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraSpawn::TfraSpawn(TComponent* Owner)
        : TForm(Owner)
{
    DEFINE_INI(fsStorage);
    m_Current = 0;
}
//---------------------------------------------------------------------------
void TfraSpawn::OnItemFocused(ListItemsVec& items)
{
	VERIFY(items.size()<=1);
    m_Current 			= 0;
    for (ListItemsIt it=items.begin(); it!=items.end(); it++)
        m_Current 		= (LPCSTR)(*it)->m_Object;
    ExecCommand			(COMMAND_RENDER_FOCUS);
}
//------------------------------------------------------------------------------

void __fastcall TfraSpawn::PaneMinClick(TObject *Sender)
{
    PanelMinMaxClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::ExpandClick(TObject *Sender)
{
    PanelMaximizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::ebAttachObjectClick(TObject *Sender)
{
	if (ebAttachObject->Down) ExecCommand(COMMAND_CHANGE_ACTION, etaAdd);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::evDetachObjectClick(TObject *Sender)
{
    ObjectList lst;
    if (Scene->GetQueryObjects(lst,OBJCLASS_SPAWNPOINT,1,1,0)){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
	        CSpawnPoint* O = dynamic_cast<CSpawnPoint*>(*it); R_ASSERT(O);
        	O->DetachObject();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::FormHide(TObject *Sender)
{
    m_Items->SaveSelection	(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::FormShow(TObject *Sender)
{
    m_Items->LoadSelection	(fsStorage);
    ListItemsVec items;
    LHelper().CreateItem	(items,RPOINT_CHOOSE_NAME,0,0,RPOINT_CHOOSE_NAME);
    LHelper().CreateItem	(items,ENVMOD_CHOOSE_NAME,0,0,ENVMOD_CHOOSE_NAME);
    CInifile::Root& data 	= pSettings->sections();
    for (CInifile::RootIt it=data.begin(); it!=data.end(); it++){
    	LPCSTR val;
    	if ((*it)->line_exist("$spawn",&val)){
        	shared_str v	= pSettings->r_string_wb((*it)->Name,"$spawn");
            if (v.size())	LHelper().CreateItem(items,*v,0,0,(LPVOID)*(*it)->Name);
        }
    }
    m_Items->AssignItems	(items,false,true);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::FormCreate(TObject *Sender)
{
    m_Items 				= TItemList::CreateForm("Spawns",paItems, alClient, 0);
    m_Items->SetOnItemsFocusedEvent(TOnILItemsFocused(this,&TfraSpawn::OnItemFocused));
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::FormDestroy(TObject *Sender)
{
    TItemList::DestroyForm	(m_Items);
}
//---------------------------------------------------------------------------

void TfraSpawn::SelByRefObject( bool flag )
{
    ObjectList objlist;
	LPCSTR N=Current();
	if (N){
        ObjectIt _F = Scene->FirstObj(OBJCLASS_SPAWNPOINT);
        ObjectIt _E = Scene->LastObj(OBJCLASS_SPAWNPOINT);
        for(;_F!=_E;_F++){
            if((*_F)->Visible() ){
                CSpawnPoint *_O = (CSpawnPoint*)(*_F);
                if(_O->RefCompare(N)) _O->Select( flag );
            }
        }
    }
}
//---------------------------------------------------------------------------

void TfraSpawn::MultiSelByRefObject ( bool clear_prev )
{
    ObjectList 	objlist;
    LPU32Vec 	sellist;
    if (Scene->GetQueryObjects(objlist,OBJCLASS_SPAWNPOINT,1,1,-1)){
    	for (ObjectIt it=objlist.begin(); it!=objlist.end(); it++){
	        LPCSTR N = ((CSpawnPoint*)*it)->RefName();
            ObjectIt _F = Scene->FirstObj(OBJCLASS_SPAWNPOINT);
            ObjectIt _E = Scene->LastObj(OBJCLASS_SPAWNPOINT);
            for(;_F!=_E;_F++){
	            CSpawnPoint *_O = (CSpawnPoint *)(*_F);
                if((*_F)->Visible()&&_O->RefCompare(N)){
                	if (clear_prev){
                    	_O->Select( false );
	                    sellist.push_back((u32*)_O);
                    }else{
                    	if (!_O->Selected())
                        	sellist.push_back((u32*)_O);
                    }
                }
            }
        }
        std::sort			(sellist.begin(),sellist.end());
        sellist.erase		(std::unique(sellist.begin(),sellist.end()),sellist.end());
        std::random_shuffle	(sellist.begin(),sellist.end());
        int max_k		= iFloor(float(sellist.size())/100.f*float(seSelPercent->Value)+0.5f);
        int k			= 0;
        for (LPU32It o_it=sellist.begin(); k<max_k; o_it++,k++){
            CSpawnPoint *_O = (CSpawnPoint *)(*o_it);
            _O->Select( true );
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::ebSelectByRefsClick(TObject *Sender)
{
	SelByRefObject( true );
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::ebDeselectByRefsClick(TObject *Sender)
{
	SelByRefObject( false );
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::ebMultiSelectByRefMoveClick(TObject *Sender)
{
	MultiSelByRefObject(true);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::ebMultiSelectByRefAppendClick(TObject *Sender)
{
	MultiSelByRefObject(false);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawn::seSelPercentKeyPress(TObject *Sender, char &Key)
{
	if (Key==VK_RETURN) ExecCommand(COMMAND_RENDER_FOCUS);
}
//---------------------------------------------------------------------------

