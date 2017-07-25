//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_LevelMain.h"

#include "UI_LevelTools.h"
#include "EditLibrary.h"
#include "../ECore/Editor/ImageEditor.h"
#include "leftbar.h"
#include "topbar.h"
#include "scene.h"
#include "sceneobject.h"
#include "Cursor3D.h"
#include "bottombar.h"
#include "xr_trims.h"
#include "main.h"
#include "xr_input.h"
#include "../ECore/Editor/ui_main.h"
#include "d3dutils.h"
#include "EditLightAnim.h"
#include "builder.h"
#include "SoundManager_LE.h"
#include "../xrEProps/NumericVector.h"
#include "LevelPreferences.h"

#ifdef _LEVEL_EDITOR
//.    if (m_Cursor->GetVisible()) RedrawScene();
#endif

CLevelMain*&	LUI=(CLevelMain*)UI;

CLevelMain::CLevelMain()
{
    m_Cursor        = xr_new<C3DCursor>();
    EPrefs			= xr_new<CLevelPreferences>();
}

CLevelMain::~CLevelMain()
{
    xr_delete		(EPrefs);
    xr_delete		(m_Cursor);
}



//------------------------------------------------------------------------------
// Tools commands
//------------------------------------------------------------------------------
CCommandVar CLevelTools::CommandChangeTarget(CCommandVar p1, CCommandVar p2)
{
	if (Scene->GetMTools(p1)->IsEnabled()){
	    SetTarget	(p1,p2);
	    ExecCommand	(COMMAND_UPDATE_PROPERTIES);
        return 		TRUE;
    }else{
    	return 		FALSE;
    }
}
CCommandVar CLevelTools::CommandShowObjectList(CCommandVar p1, CCommandVar p2)
{
    if (LUI->GetEState()==esEditScene) ShowObjectList();
    return TRUE;
}

//------------------------------------------------------------------------------
// Main commands
//------------------------------------------------------------------------------
CCommandVar CommandLibraryEditor(CCommandVar p1, CCommandVar p2)
{
    if (Scene->ObjCount()||(LUI->GetEState()!=esEditScene)){
        if (LUI->GetEState()==esEditLibrary)	TfrmEditLibrary::ShowEditor();
        else									ELog.DlgMsg(mtError, "Scene must be empty before editing library!");
    }else{
        TfrmEditLibrary::ShowEditor();
    }
    return TRUE;
}
CCommandVar CommandLAnimEditor(CCommandVar p1, CCommandVar p2)
{
    TfrmEditLightAnim::ShowEditor();
    return TRUE;
}
CCommandVar CommandFileMenu(CCommandVar p1, CCommandVar p2)
{
    FHelper.ShowPPMenu(fraLeftBar->pmSceneFile,0);
    return TRUE;
}
CCommandVar CLevelTools::CommandEnableTarget(CCommandVar p1, CCommandVar p2)
{
	ESceneCustomMTools* M 	= Scene->GetMTools(p1); VERIFY(M);
    BOOL res				= FALSE; 
	if (p2){
    	res 				= ExecCommand(COMMAND_LOAD_LEVEL_PART,M->ClassID,TRUE);
    }else{
        if (!Scene->IfModified()){
		    M->m_EditFlags.set(ESceneCustomMTools::flEnable,TRUE);
            res				= FALSE;
        }else{
	    	res				= ExecCommand(COMMAND_UNLOAD_LEVEL_PART,M->ClassID,TRUE);
        }
		if (res)        	ExecCommand(COMMAND_CHANGE_TARGET,OBJCLASS_SCENEOBJECT);
    }
    ExecCommand				(COMMAND_REFRESH_UI_BAR);
    return res;
}
CCommandVar CLevelTools::CommandReadonlyTarget(CCommandVar p1, CCommandVar p2)
{
	ESceneCustomMTools* M 	= Scene->GetMTools(p1); VERIFY(M);
    BOOL res				= TRUE; 
	if (p2){
        if (!Scene->IfModified()){    
		    M->m_EditFlags.set(ESceneCustomMTools::flForceReadonly,FALSE);
            res				= FALSE;
        }else{
            xr_string pn	= Scene->LevelPartName(LTools->m_LastFileName.c_str(),M->ClassID);
         	EFS.UnlockFile	(pn.c_str(),false);
        }
    }else{
        xr_string pn		= Scene->LevelPartName(LTools->m_LastFileName.c_str(),M->ClassID);
    	if (!EFS.CheckLocking(pn.c_str(),false,false))
         	EFS.LockFile	(pn.c_str(),false);
    }
    if (res){
    	Reset				();
	    ExecCommand			(COMMAND_REFRESH_UI_BAR);
    }
    return res;
}
CCommandVar CLevelTools::CommandMultiRenameObjects(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to rename selected objects?")){
			int cnt			= Scene->MultiRenameObjects();
            if (cnt){	
			    ExecCommand	(COMMAND_UPDATE_PROPERTIES);
            	Scene->UndoSave();
            }
	        ELog.DlgMsg		( mtInformation, "%d - objects are renamed.", cnt );
        }
    }else{
        ELog.DlgMsg			( mtError, "Scene sharing violation" );
    }
    return 					FALSE;
}
CCommandVar CommandLoadLevelPart(CCommandVar p1, CCommandVar p2)
{
    xr_string temp_fn	= LTools->m_LastFileName.c_str();
    if (!temp_fn.empty())
        return			Scene->LoadLevelPart(temp_fn.c_str(),p1,p2);
    return				FALSE;
}
CCommandVar CommandUnloadLevelPart(CCommandVar p1, CCommandVar p2)
{
    xr_string temp_fn	= LTools->m_LastFileName.c_str();
    if (!temp_fn.empty())
        return			Scene->UnloadLevelPart(temp_fn.c_str(),p1,p2);
    return				FALSE;
}
CCommandVar CommandLoad(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
    	if (!p1.IsString()){
        	xr_string temp_fn	= LTools->m_LastFileName.c_str();
        	if (EFS.GetOpenName	( _maps_, temp_fn ))
            	return 			ExecCommand(COMMAND_LOAD,temp_fn);
        }else{
	        xr_string temp_fn	= p1; xr_strlwr(temp_fn);
                
            if (!Scene->IfModified())	return FALSE;
            
            UI->SetStatus			("Level loading...");
            ExecCommand				(COMMAND_CLEAR);

            if (Scene->Load	(temp_fn.c_str(), false)){
                LTools->m_LastFileName	= temp_fn.c_str();
                UI->ResetStatus		();
                Scene->UndoClear	();
                Scene->UndoSave		();
//.             Scene->m_RTFlags.set(EScene::flRT_Unsaved|EScene::flRT_Modified,FALSE);
				ExecCommand			(COMMAND_CLEAN_LIBRARY);
                ExecCommand			(COMMAND_UPDATE_CAPTION);
                ExecCommand			(COMMAND_CHANGE_ACTION,etaSelect);
                EPrefs->AppendRecentFile(temp_fn.c_str());
            }else{
                ELog.DlgMsg	( mtError, "Can't load map '%s'", temp_fn.c_str() );
            }
            // update props
            ExecCommand			(COMMAND_UPDATE_PROPERTIES);
            UI->RedrawScene		();             
        }
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        return FALSE;
    }
    return TRUE;
}
CCommandVar CommandSaveBackup(CCommandVar p1, CCommandVar p2)
{
    string_path 	fn;
    strconcat(sizeof(fn),fn,Core.CompName,"_",Core.UserName,"_backup.level");
    FS.update_path	(fn,_maps_,fn);
    return 			ExecCommand(COMMAND_SAVE,xr_string(fn));
}
CCommandVar CommandSave(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if (p2==1){
            xr_string temp_fn	= LTools->m_LastFileName.c_str();
            if (EFS.GetSaveName	( _maps_, temp_fn ))
                return 			ExecCommand(COMMAND_SAVE,temp_fn,0);
            else
                return          FALSE;
        }else{
            if (p1.IsInteger())
            	return 			ExecCommand(COMMAND_SAVE,xr_string(LTools->m_LastFileName.c_str()),0);
            xr_string temp_fn	= xr_string(p1);
            if (temp_fn.empty()){
                return 			ExecCommand(COMMAND_SAVE,temp_fn,1);
            }else{
                xr_strlwr		(temp_fn);
                
                UI->SetStatus	("Level saving...");
                Scene->Save		(temp_fn.c_str(), false);
                UI->ResetStatus	();
                // set new name
                if (0!=xr_strcmp(Tools->m_LastFileName.c_str(),temp_fn.c_str())){
	                Scene->UnlockLevel		(Tools->m_LastFileName.c_str());
    	            Tools->m_LastFileName 	= temp_fn.c_str();
        	        Scene->LockLevel		(Tools->m_LastFileName.c_str());
                }
                ExecCommand		(COMMAND_UPDATE_CAPTION);
                EPrefs->AppendRecentFile(temp_fn.c_str());
                return 			TRUE;
            }
        }
    } else {
        ELog.DlgMsg			( mtError, "Scene sharing violation" );
        return				FALSE;
    }
    ELog.DlgMsg				( mtError, "Can't save level. Unknown error." );
    return 					FALSE;
}
CCommandVar CommandClear(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if (!Scene->IfModified()) return TRUE;
        Scene->UnlockLevel		(Tools->m_LastFileName.c_str());
        Device.m_Camera.Reset	();
        Scene->Reset			();
        Scene->m_LevelOp.Reset	();
        Tools->m_LastFileName 		= "";
        LTools->m_LastSelectionName = "";
        Scene->UndoClear		();
        ExecCommand				(COMMAND_UPDATE_CAPTION);
        ExecCommand				(COMMAND_CHANGE_TARGET,OBJCLASS_SCENEOBJECT);
        ExecCommand				(COMMAND_CHANGE_ACTION,etaSelect,estDefault);
	    ExecCommand				(COMMAND_UPDATE_PROPERTIES);
        Scene->UndoSave			();
        return 					TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        return					FALSE;
    }
}
CCommandVar CommandLoadFirstRecent(CCommandVar p1, CCommandVar p2)
{
    if (EPrefs->FirstRecentFile())
        return 					ExecCommand(COMMAND_LOAD,xr_string(EPrefs->FirstRecentFile()));
    return 						FALSE;
}

CCommandVar CommandClearDebugDraw(CCommandVar p1, CCommandVar p2)
{
    Tools->ClearDebugDraw		();
    UI->RedrawScene				();
    return 						TRUE;
}

CCommandVar CommandImportCompilerError(CCommandVar p1, CCommandVar p2)
{
    xr_string fn;
    if(EFS.GetOpenName("$logs$", fn, false, NULL, 0)){
        Scene->LoadCompilerError(fn.c_str());
    }
    UI->RedrawScene		();
    return TRUE;
}
CCommandVar CommandExportCompilerError(CCommandVar p1, CCommandVar p2)
{
    xr_string fn;
    if(EFS.GetSaveName("$logs$", fn, NULL, 0)){
        Scene->SaveCompilerError(fn.c_str());
    }
    return TRUE;
}
CCommandVar CommandValidateScene(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->Validate	(true,true,true,true,true,true);
	    return 			TRUE;
    } else {
        ELog.DlgMsg		( mtError, "Scene sharing violation" );
	    return 			FALSE;
    }
}
CCommandVar CommandCleanLibrary(CCommandVar p1, CCommandVar p2)
{
    if ( !Scene->locked() ){
        Lib.CleanLibrary();
        return 			TRUE;
    }else{
        ELog.DlgMsg		(mtError, "Scene must be empty before refreshing library!");
        return 			FALSE;
    }
}

CCommandVar CommandReloadObjects(CCommandVar p1, CCommandVar p2)
{
    Lib.ReloadObjects	();
    return 				TRUE;
}

CCommandVar CommandCut(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->CutSelection(LTools->CurrentClassID());
        fraLeftBar->miPaste->Enabled = true;
        fraLeftBar->miPaste2->Enabled = true;
        Scene->UndoSave	();
        return 			TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        return 			FALSE;
    }
}
CCommandVar CommandCopy(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->CopySelection(LTools->CurrentClassID());
        fraLeftBar->miPaste->Enabled = true;
        fraLeftBar->miPaste2->Enabled = true;
        return 			TRUE;
    } else {
        ELog.DlgMsg		( mtError, "Scene sharing violation" );
        return 			FALSE;
    }
}

CCommandVar CommandPaste(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->PasteSelection();
        Scene->UndoSave	();
        return 			TRUE;
    } else {
        ELog.DlgMsg		( mtError, "Scene sharing violation" );
        return  		FALSE;
    }
}

CCommandVar CommandLoadSelection(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        xr_string fn			= LTools->m_LastSelectionName;
        if( EFS.GetOpenName( _maps_, fn ) ){
        	LPCSTR maps_path	= FS.get_path(_maps_)->m_Path;
        	if (fn.c_str()==strstr(fn.c_str(),maps_path))
		        LTools->m_LastSelectionName = fn.c_str()+xr_strlen(maps_path);
            UI->SetStatus		("Fragment loading...");
            Scene->LoadSelection(fn.c_str());
            UI->ResetStatus		();
            Scene->UndoSave		();
            ExecCommand			(COMMAND_CHANGE_ACTION,etaSelect);
            ExecCommand			(COMMAND_UPDATE_PROPERTIES);
            UI->RedrawScene		();
	        return 				TRUE;
        }               	
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 				FALSE;
}        
CCommandVar CommandSaveSelection(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        xr_string fn			= LTools->m_LastSelectionName;
        if( EFS.GetSaveName		( _maps_, fn ) ){
        	LPCSTR maps_path	= FS.get_path(_maps_)->m_Path;
        	if (fn.c_str()==strstr(fn.c_str(),maps_path))
		        LTools->m_LastSelectionName = fn.c_str()+xr_strlen(maps_path);
            UI->SetStatus		("Fragment saving...");
            Scene->SaveSelection(LTools->CurrentClassID(),fn.c_str());
            UI->ResetStatus		();
	        return 				TRUE;
        }
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}

CCommandVar CommandUndo(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if( !Scene->Undo() ) 	ELog.DlgMsg( mtInformation, "Undo buffer empty" );
        else{
            LTools->Reset		();
            ExecCommand			(COMMAND_CHANGE_ACTION, etaSelect);
	        return 				TRUE;
        }
    } else {
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}

CCommandVar CommandRedo(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if( !Scene->Redo() ) 	ELog.DlgMsg( mtInformation, "Redo buffer empty" );
        else{
            LTools->Reset		();
            ExecCommand			(COMMAND_CHANGE_ACTION, etaSelect);
		    return 				TRUE;
        }
    } else {
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}

CCommandVar CommandClearSceneSummary(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->ClearSummaryInfo	();
	    return 					TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandCollectSceneSummary(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->CollectSummaryInfo();
	    return 					TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandShowSceneSummary(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->ShowSummaryInfo();
	    return 					TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandExportSceneSummary(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->ExportSummaryInfo(xr_string(p1).c_str());
	    return 					TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}

CCommandVar CommandSceneHighlightTexture(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        LPCSTR new_val 		 	= 0;
		if (TfrmChoseItem::SelectItem(smTexture,new_val,1)){
	       	Scene->HighlightTexture(new_val,false,0,0,false);
		    return 				TRUE;
        }
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}

CCommandVar CommandOptions(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        ExecCommand				(COMMAND_SHOW_PROPERTIES);
	    return 					TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}

CCommandVar CommandBuild(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to build level?"))
            return				Builder.Compile( );
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}
CCommandVar CommandMakeAIMap(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export ai-map?"))
            return 				Builder.MakeAIMap( );
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}
CCommandVar CommandMakeGame(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export game?"))
            return				Builder.MakeGame( );
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}
CCommandVar CommandMakeDetails(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export details?"))
            return 				Builder.MakeDetails();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}
CCommandVar CommandMakeHOM(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export HOM?"))
            return				Builder.MakeHOM();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}
CCommandVar CommandMakeSOM(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export Sound Occlusion Model?"))
            return				Builder.MakeSOM();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}
CCommandVar CommandInvertSelectionAll(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->InvertSelection	(LTools->CurrentClassID());
	    return 					TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}

CCommandVar CommandSelectAll(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->SelectObjects	(true,LTools->CurrentClassID());
	    return 					TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
    }
    return 						FALSE;
}

CCommandVar CommandDeselectAll(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->SelectObjects	(false,LTools->CurrentClassID());
	    return 					TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}

CCommandVar CommandDeleteSelection(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->RemoveSelection	( LTools->CurrentClassID() );
        Scene->UndoSave			();
        return					TRUE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}

CCommandVar CommandHideUnsel(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->ShowObjects		( false, LTools->CurrentClassID(), true, false );
        Scene->UndoSave			();
        ExecCommand				(COMMAND_UPDATE_PROPERTIES);
	    return 					TRUE;
    } else {
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandHideSel(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->ShowObjects		( bool(p1), LTools->CurrentClassID(), true, true );
        Scene->UndoSave			();
        ExecCommand				(COMMAND_UPDATE_PROPERTIES);
	    return 					TRUE;
    } else {
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandHideAll(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->ShowObjects		( bool(p1), LTools->CurrentClassID(), false );
        Scene->UndoSave			();
        ExecCommand				(COMMAND_UPDATE_PROPERTIES);
	    return 					TRUE;
    }else{
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandLockAll(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->LockObjects		(bool(p1),LTools->CurrentClassID(),false);
        Scene->UndoSave			();
	    return 					TRUE;
    }else{
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandLockSel(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->LockObjects		(bool(p1),LTools->CurrentClassID(),true,true);
        Scene->UndoSave			();
	    return 					TRUE;
    }else{
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandLockUnsel(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->LockObjects		(bool(p1),LTools->CurrentClassID(),true,false);
        Scene->UndoSave			();
        return					TRUE;
   }else{
        ELog.DlgMsg				( mtError, "Scene sharing violation" );                   
	    return 					FALSE;
    }
}
CCommandVar CommandSetSnapObjects(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->SetSnapList		();
	    return 					TRUE;
    }else{
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandAddSelSnapObjects(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->AddSelToSnapList	();
	    return 					TRUE;
    }else{
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandDelSelSnapObjects(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->DelSelFromSnapList();
	    return 					TRUE;
    }else{
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandClearSnapObjects(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->ClearSnapList	(true);
	    return 					TRUE;
    }else{
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandSelectSnapObjects(CCommandVar p1, CCommandVar p2)
{
    if( !Scene->locked() ){
        Scene->SelectSnapList	();
	    return 					TRUE;
    }else{
        ELog.DlgMsg				( mtError, "Scene sharing violation" );
	    return 					FALSE;
    }
}
CCommandVar CommandRefreshSnapObjects(CCommandVar p1, CCommandVar p2)
{
    fraLeftBar->UpdateSnapList();
    return 						TRUE;
}
CCommandVar CommandRefreshSoundEnvs(CCommandVar p1, CCommandVar p2)
{
    ::Sound->refresh_env_library();
    return 						TRUE;
//		::Sound->_restart();
}
CCommandVar CommandRefreshSoundEnvGeometry(CCommandVar p1, CCommandVar p2)
{
    LSndLib->RefreshEnvGeometry();
    return 						TRUE;
}
CCommandVar CommandShowContextMenu(CCommandVar p1, CCommandVar p2)
{
    LUI->ShowContextMenu		(p1);
    return 						TRUE;
}
//------        
CCommandVar CommandRefreshUIBar(CCommandVar p1, CCommandVar p2)
{
    fraTopBar->RefreshBar		();
    fraLeftBar->RefreshBar		();
    fraBottomBar->RefreshBar	();
    return 						TRUE;
}
CCommandVar CommandRestoreUIBar(CCommandVar p1, CCommandVar p2)
{
    fraTopBar->fsStorage->RestoreFormPlacement();
    fraLeftBar->fsStorage->RestoreFormPlacement();
    fraBottomBar->fsStorage->RestoreFormPlacement();
    return 						TRUE;
}
CCommandVar CommandSaveUIBar(CCommandVar p1, CCommandVar p2)
{
    fraTopBar->fsStorage->SaveFormPlacement();
    fraLeftBar->fsStorage->SaveFormPlacement();
    fraBottomBar->fsStorage->SaveFormPlacement();
    return 						TRUE;
}
CCommandVar CommandUpdateToolBar(CCommandVar p1, CCommandVar p2)
{
    fraLeftBar->UpdateBar		();
    return 						TRUE;
}
CCommandVar CommandUpdateCaption(CCommandVar p1, CCommandVar p2)
{
    frmMain->UpdateCaption		();
    return 						TRUE;
}
//------
CCommandVar CommandCreateSoundLib(CCommandVar p1, CCommandVar p2)
{
    SndLib						= xr_new<CLevelSoundManager>();
    return 						TRUE;
}

void CLevelMain::RegisterCommands()
{
	inherited::RegisterCommands	();
    // tools             
	REGISTER_SUB_CMD_CE	(COMMAND_CHANGE_TARGET,             "Change Target", 		LTools,CLevelTools::CommandChangeTarget, true);
		APPEND_SUB_CMD	("Object", 							OBJCLASS_SCENEOBJECT,	0);
		APPEND_SUB_CMD	("Light", 							OBJCLASS_LIGHT, 		0);
		APPEND_SUB_CMD	("Sound Source",					OBJCLASS_SOUND_SRC, 	0);
		APPEND_SUB_CMD	("Sound Env", 		                OBJCLASS_SOUND_ENV, 	0);
		APPEND_SUB_CMD	("Glow", 			                OBJCLASS_GLOW, 			0);
		APPEND_SUB_CMD	("Shape", 			                OBJCLASS_SHAPE, 		0);
		APPEND_SUB_CMD	("Spawn Point", 	                OBJCLASS_SPAWNPOINT, 	0);
		APPEND_SUB_CMD	("Way", 			                OBJCLASS_WAY, 			0);
		APPEND_SUB_CMD	("Way Point", 		                OBJCLASS_WAY, 			1);
		APPEND_SUB_CMD	("Sector", 			                OBJCLASS_SECTOR, 		0);
		APPEND_SUB_CMD	("Portal", 			                OBJCLASS_PORTAL, 		0);
		APPEND_SUB_CMD	("Group", 			                OBJCLASS_GROUP, 		0);
		APPEND_SUB_CMD	("Particle System",                 OBJCLASS_PS, 			0);
		APPEND_SUB_CMD	("Detail Objects", 	                OBJCLASS_DO, 			0);
		APPEND_SUB_CMD	("AI Map", 			                OBJCLASS_AIMAP, 		0);
		APPEND_SUB_CMD	("Static Wallmark",                 OBJCLASS_WM, 			0);
    REGISTER_SUB_CMD_END;    
	REGISTER_CMD_C	    (COMMAND_ENABLE_TARGET,           	LTools,CLevelTools::CommandEnableTarget);
	REGISTER_CMD_C	    (COMMAND_READONLY_TARGET,          	LTools,CLevelTools::CommandReadonlyTarget);
	REGISTER_CMD_C	    (COMMAND_MULTI_RENAME_OBJECTS,     	LTools,CLevelTools::CommandMultiRenameObjects);

	REGISTER_CMD_CE	    (COMMAND_SHOW_OBJECTLIST,           "Scene\\Show Object List",		LTools,CLevelTools::CommandShowObjectList, false);
	// common
	REGISTER_CMD_S	    (COMMAND_LIBRARY_EDITOR,           	CommandLibraryEditor);
	REGISTER_CMD_S	    (COMMAND_LANIM_EDITOR,            	CommandLAnimEditor);
	REGISTER_CMD_SE	    (COMMAND_FILE_MENU,              	"File\\Menu",					CommandFileMenu, 		true);
    REGISTER_CMD_S		(COMMAND_LOAD_LEVEL_PART,			CommandLoadLevelPart);
    REGISTER_CMD_S		(COMMAND_UNLOAD_LEVEL_PART,			CommandUnloadLevelPart);
	REGISTER_CMD_SE	    (COMMAND_LOAD,              		"File\\Load Level", 			CommandLoad, 			true);
    REGISTER_SUB_CMD_SE (COMMAND_SAVE, 						"File",							CommandSave,			true);
    	APPEND_SUB_CMD	("Save",							0,								0);
    	APPEND_SUB_CMD	("Save As",							0,								1);
    REGISTER_SUB_CMD_END;
	REGISTER_CMD_S	    (COMMAND_SAVE_BACKUP,              	CommandSaveBackup);
	REGISTER_CMD_SE	    (COMMAND_CLEAR,              		"File\\Clear Scene", 			CommandClear,			true);
	REGISTER_CMD_SE	    (COMMAND_LOAD_FIRSTRECENT,          "File\\Load First Recent",		CommandLoadFirstRecent, true);
	REGISTER_CMD_S	    (COMMAND_CLEAR_DEBUG_DRAW, 		    CommandClearDebugDraw);
	REGISTER_CMD_S	    (COMMAND_IMPORT_COMPILER_ERROR,     CommandImportCompilerError);
	REGISTER_CMD_S	    (COMMAND_EXPORT_COMPILER_ERROR,     CommandExportCompilerError);
	REGISTER_CMD_S	    (COMMAND_VALIDATE_SCENE,            CommandValidateScene);
	REGISTER_CMD_S	    (COMMAND_CLEAN_LIBRARY,           	CommandCleanLibrary);
	REGISTER_CMD_S	    (COMMAND_RELOAD_OBJECTS,            CommandReloadObjects);
	REGISTER_CMD_SE	    (COMMAND_CUT,              			"Edit\\Cut",					CommandCut,false);
	REGISTER_CMD_SE	    (COMMAND_COPY,              		"Edit\\Copy",					CommandCopy,false);
	REGISTER_CMD_SE	    (COMMAND_PASTE,              		"Edit\\Paste",					CommandPaste,false);
	REGISTER_CMD_S	    (COMMAND_LOAD_SELECTION,            CommandLoadSelection);
	REGISTER_CMD_S	    (COMMAND_SAVE_SELECTION,            CommandSaveSelection);
	REGISTER_CMD_SE	    (COMMAND_UNDO,              		"Edit\\Undo",					CommandUndo,false);
	REGISTER_CMD_SE	    (COMMAND_REDO,              		"Edit\\Redo",					CommandRedo,false);
	REGISTER_CMD_S	    (COMMAND_CLEAR_SCENE_SUMMARY,	    CommandClearSceneSummary);
	REGISTER_CMD_S	    (COMMAND_COLLECT_SCENE_SUMMARY,     CommandCollectSceneSummary);
	REGISTER_CMD_S	    (COMMAND_SHOW_SCENE_SUMMARY,        CommandShowSceneSummary);
	REGISTER_CMD_S	    (COMMAND_EXPORT_SCENE_SUMMARY,      CommandExportSceneSummary);
	REGISTER_CMD_S	    (COMMAND_SCENE_HIGHLIGHT_TEXTURE,	CommandSceneHighlightTexture);
	REGISTER_CMD_SE	    (COMMAND_OPTIONS,              		"Scene\\Options",		        CommandOptions,false);
	REGISTER_CMD_SE	    (COMMAND_BUILD,              		"Compile\\Build",		        CommandBuild,false);
	REGISTER_CMD_SE	    (COMMAND_MAKE_GAME,              	"Compile\\Make Game",	        CommandMakeGame,false);
	REGISTER_CMD_SE	    (COMMAND_MAKE_AIMAP,              	"Compile\\Make AI Map",	        CommandMakeAIMap,false);
	REGISTER_CMD_SE	    (COMMAND_MAKE_DETAILS,              "Compile\\Make Details",        CommandMakeDetails,false);
	REGISTER_CMD_SE	    (COMMAND_MAKE_HOM,              	"Compile\\Make HOM",	        CommandMakeHOM,false);
	REGISTER_CMD_SE	    (COMMAND_MAKE_SOM,              	"Compile\\Make SOM",	        CommandMakeSOM,false);
	REGISTER_CMD_SE	    (COMMAND_INVERT_SELECTION_ALL,      "Selection\\Invert", 			CommandInvertSelectionAll,false);
	REGISTER_CMD_SE	    (COMMAND_SELECT_ALL,              	"Selection\\Select All", 		CommandSelectAll,false);
	REGISTER_CMD_SE	    (COMMAND_DESELECT_ALL,              "Selection\\Unselect All", 		CommandDeselectAll,false);
	REGISTER_CMD_SE	    (COMMAND_DELETE_SELECTION,          "Edit\\Delete", 				CommandDeleteSelection,false);
	REGISTER_CMD_SE	    (COMMAND_HIDE_UNSEL,              	"Visibility\\Hide Unselected",	CommandHideUnsel,false);
	REGISTER_CMD_SE	    (COMMAND_HIDE_SEL,              	"Visibility\\Hide Selected", 	CommandHideSel,false);
	REGISTER_CMD_SE	    (COMMAND_HIDE_ALL,              	"Visibility\\Hide All", 		CommandHideAll,false);
	REGISTER_CMD_S	    (COMMAND_LOCK_ALL,              	CommandLockAll);
	REGISTER_CMD_S	    (COMMAND_LOCK_SEL,             		CommandLockSel);
	REGISTER_CMD_S	    (COMMAND_LOCK_UNSEL,              	CommandLockUnsel);
	REGISTER_CMD_S		(COMMAND_SET_SNAP_OBJECTS,          CommandSetSnapObjects);
	REGISTER_CMD_S	    (COMMAND_ADD_SEL_SNAP_OBJECTS,      CommandAddSelSnapObjects);
	REGISTER_CMD_S	    (COMMAND_DEL_SEL_SNAP_OBJECTS,      CommandDelSelSnapObjects);
	REGISTER_CMD_S	    (COMMAND_CLEAR_SNAP_OBJECTS,        CommandClearSnapObjects);
	REGISTER_CMD_S	    (COMMAND_SELECT_SNAP_OBJECTS,       CommandSelectSnapObjects);
	REGISTER_CMD_S	    (COMMAND_REFRESH_SNAP_OBJECTS,      CommandRefreshSnapObjects);
	REGISTER_CMD_S	    (COMMAND_REFRESH_SOUND_ENVS,        CommandRefreshSoundEnvs);
	REGISTER_CMD_S	    (COMMAND_REFRESH_SOUND_ENV_GEOMETRY,CommandRefreshSoundEnvGeometry);
	REGISTER_CMD_S	    (COMMAND_SHOWCONTEXTMENU,           CommandShowContextMenu);
	REGISTER_CMD_S	    (COMMAND_REFRESH_UI_BAR,            CommandRefreshUIBar);
	REGISTER_CMD_S	    (COMMAND_RESTORE_UI_BAR,            CommandRestoreUIBar);
	REGISTER_CMD_S	    (COMMAND_SAVE_UI_BAR,              	CommandSaveUIBar);
	REGISTER_CMD_S	    (COMMAND_UPDATE_TOOLBAR,            CommandUpdateToolBar);
	REGISTER_CMD_S	    (COMMAND_UPDATE_CAPTION,            CommandUpdateCaption);
	REGISTER_CMD_S	    (COMMAND_CREATE_SOUND_LIB,          CommandCreateSoundLib);
}                                                                            

char* CLevelMain::GetCaption()
{
	return Tools->m_LastFileName.IsEmpty()?"noname":Tools->m_LastFileName.c_str();
}

bool __fastcall CLevelMain::ApplyShortCut(WORD Key, TShiftState Shift)
{
    return inherited::ApplyShortCut(Key,Shift);
}
//---------------------------------------------------------------------------

bool __fastcall CLevelMain::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
    return inherited::ApplyGlobalShortCut(Key,Shift);
}
//---------------------------------------------------------------------------

bool CLevelMain::PickGround(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap, Fvector* hitnormal){
	VERIFY(m_bReady);
    // pick object geometry
    if ((bSnap==-1)||(Tools->GetSettings(etfOSnap)&&(bSnap==1))){
        bool bPickObject;
        SRayPickInfo pinf;
	    EEditorState est = GetEState();
        switch(est){
        case esEditLibrary:{
        	bPickObject = !!TfrmEditLibrary::RayPick(start,direction,&pinf);
        }break;
        case esEditScene:{
        	bPickObject = !!Scene->RayPickObject(pinf.inf.range, start,direction,OBJCLASS_SCENEOBJECT,&pinf,Scene->GetSnapList(false)); break;
        }
        default: return false;
        }
        if (bPickObject){
		    if (Tools->GetSettings(etfVSnap)&&bSnap){
                Fvector pn;
                float u = pinf.inf.u;
                float v = pinf.inf.v;
                float w = 1-(u+v);
				Fvector verts[3];
                pinf.e_obj->GetFaceWorld(pinf.s_obj->_Transform(),pinf.e_mesh,pinf.inf.id,verts);
                if ((w>u) && (w>v)) pn.set(verts[0]);
                else if ((u>w) && (u>v)) pn.set(verts[1]);
                else pn.set(verts[2]);
                if (pn.distance_to(pinf.pt) < LTools->m_MoveSnap) hitpoint.set(pn);
                else hitpoint.set(pinf.pt);
            }else{
            	hitpoint.set(pinf.pt);
            }
            if (hitnormal){
	            Fvector verts[3];
    	        pinf.e_obj->GetFaceWorld(pinf.s_obj->_Transform(),pinf.e_mesh,pinf.inf.id,verts);
        	    hitnormal->mknormal(verts[0],verts[1],verts[2]);
            }
			return true;
        }
    }

    // pick grid
	Fvector normal;
	normal.set( 0, 1, 0 );
	float clcheck = direction.dotproduct( normal );
	if( fis_zero( clcheck ) ) return false;
	float alpha = - start.dotproduct(normal) / clcheck;
	if( alpha <= 0 ) return false;

	hitpoint.x = start.x + direction.x * alpha;
	hitpoint.y = start.y + direction.y * alpha;
	hitpoint.z = start.z + direction.z * alpha;

    if (Tools->GetSettings(etfGSnap) && bSnap){
        hitpoint.x = snapto( hitpoint.x, LTools->m_MoveSnap );
        hitpoint.z = snapto( hitpoint.z, LTools->m_MoveSnap );
        hitpoint.y = 0.f;
    }
	if (hitnormal) hitnormal->set(0,1,0);
	return true;
}
//----------------------------------------------------

bool CLevelMain::SelectionFrustum(CFrustum& frustum)
{
	VERIFY(m_bReady);
    Fvector st,d,p[4];
    Ivector2 pt[4];

    float depth = 0;

    float x1=m_StartCp.x, x2=m_CurrentCp.x;
    float y1=m_StartCp.y, y2=m_CurrentCp.y;

	if(!(x1!=x2&&y1!=y2)) return false;

	pt[0].set(_min(x1,x2),_min(y1,y2));
	pt[1].set(_max(x1,x2),_min(y1,y2));
	pt[2].set(_max(x1,x2),_max(y1,y2));
	pt[3].set(_min(x1,x2),_max(y1,y2));

    SRayPickInfo pinf;
    for (int i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        if (EPrefs->bp_lim_depth){
			pinf.inf.range = Device.m_Camera._Zfar(); // max pick range
            if (Scene->RayPickObject(pinf.inf.range, st, d, OBJCLASS_SCENEOBJECT, &pinf, 0))
	            if (pinf.inf.range > depth) depth = pinf.inf.range;
        }
    }
    if (depth<Device.m_Camera._Znear()) depth = Device.m_Camera._Zfar();
    else depth += EPrefs->bp_depth_tolerance;

    for (i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        p[i].mad(st,d,depth);
    }

    Fvector pos = Device.m_Camera.GetPosition();
    frustum.CreateFromPoints(p,4,pos);

    Fplane P; P.build(p[0],p[1],p[2]);
    if (P.classify(st)>0) P.build(p[2],p[1],p[0]);
	frustum._add(P);

	return true;
}
//----------------------------------------------------
void CLevelMain::RealUpdateScene()
{
	inherited::RealUpdateScene	();
	if (GetEState()==esEditScene){
	    Scene->OnObjectsUpdate	();
    	LTools->OnObjectsUpdate	(); // обновить все что как-то связано с объектами
	    RedrawScene				();
    }
}
//---------------------------------------------------------------------------


void CLevelMain::ShowContextMenu(int cls)
{
	VERIFY(m_bReady);
    POINT pt;
    GetCursorPos(&pt);
    fraLeftBar->miProperties->Enabled = false;
    if (Scene->SelectionCount( true, cls )) fraLeftBar->miProperties->Enabled = true;
    RedrawScene(true);
    fraLeftBar->pmObjectContext->TrackButton = tbRightButton;
    fraLeftBar->pmObjectContext->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Common
//---------------------------------------------------------------------------
void CLevelMain::ResetStatus()
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=""){
	    fraBottomBar->paStatus->Caption=""; fraBottomBar->paStatus->Repaint();
    }
}
void CLevelMain::SetStatus(LPSTR s, bool bOutLog)
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=s){
	    fraBottomBar->paStatus->Caption=s; fraBottomBar->paStatus->Repaint();
    	if (bOutLog&&s&&s[0]) ELog.Msg(mtInformation,s);
    }
}
void CLevelMain::ProgressDraw()
{
	fraBottomBar->RedrawBar();
}
//---------------------------------------------------------------------------
void CLevelMain::OutCameraPos()
{
	if (m_bReady){
        AnsiString s;
        const Fvector& c 	= Device.m_Camera.GetPosition();
        s.sprintf("C: %3.1f, %3.1f, %3.1f",c.x,c.y,c.z);
    //	const Fvector& hpb 	= Device.m_Camera.GetHPB();
    //	s.sprintf(" Cam: %3.1f°, %3.1f°, %3.1f°",rad2deg(hpb.y),rad2deg(hpb.x),rad2deg(hpb.z));
        fraBottomBar->paCamera->Caption=s; fraBottomBar->paCamera->Repaint();
    }
}
//---------------------------------------------------------------------------
void CLevelMain::OutUICursorPos()
{
	VERIFY(fraBottomBar);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void CLevelMain::OutGridSize()
{
	VERIFY(fraBottomBar);
    AnsiString s;
    s.sprintf("Grid: %1.1f",EPrefs->grid_cell_size);
    fraBottomBar->paGridSquareSize->Caption=s; fraBottomBar->paGridSquareSize->Repaint();
}
//---------------------------------------------------------------------------
void CLevelMain::OutInfo()
{
	fraBottomBar->paSel->Caption = Tools->GetInfo();
}
//---------------------------------------------------------------------------
void CLevelMain::RealQuit()
{
	frmMain->Close();
}
//---------------------------------------------------------------------------

void CLevelMain::SaveSettings(CInifile* I)
{
	inherited::SaveSettings(I);
    SSceneSummary::Save(I);
}
void CLevelMain::LoadSettings(CInifile* I)
{
	inherited::LoadSettings(I);
    SSceneSummary::Load(I);
}
//---------------------------------------------------------------------------

