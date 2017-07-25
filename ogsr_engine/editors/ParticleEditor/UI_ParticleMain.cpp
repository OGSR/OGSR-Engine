//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop       

#include "UI_ParticleMain.h"
#include "UI_ParticleTools.h"
#include "topbar.h"
#include "leftbar.h"           
#include "EditorPreferences.h"
#include "D3DUtils.h"
#include "bottombar.h"                  
#include "xr_trims.h"
#include "main.h"
#include "xr_input.h"

//---------------------------------------------------------------------------
CParticleMain*&	PUI=(CParticleMain*)UI;
//---------------------------------------------------------------------------

CParticleMain::CParticleMain()  
{
    EPrefs			= xr_new<CCustomPreferences>();
}
//---------------------------------------------------------------------------

CParticleMain::~CParticleMain()
{
    xr_delete		(EPrefs);
}
//---------------------------------------------------------------------------

CCommandVar CParticleTools::CommandSelectPreviewObj(CCommandVar p1, CCommandVar p2)
{
    SelectPreviewObject(p1);
    return TRUE;
}
CCommandVar CParticleTools::CommandEditPreviewProps(CCommandVar p1, CCommandVar p2)
{
    EditPreviewPrefs();
    return TRUE;
}
CCommandVar CParticleTools::CommandSave(CCommandVar p1, CCommandVar p2)
{
    Save(0,0);
    ExecCommand(COMMAND_UPDATE_CAPTION);
    return TRUE;
}
CCommandVar CParticleTools::CommandSaveBackup(CCommandVar p1, CCommandVar p2)
{
    ExecCommand(COMMAND_SAVE);
    return TRUE;
}
CCommandVar CParticleTools::CommandReload(CCommandVar p1, CCommandVar p2)
{
    if (!IfModified()) 	return FALSE;
    Reload				();
    ExecCommand(COMMAND_UPDATE_CAPTION);
    return TRUE;
}
CCommandVar CParticleTools::CommandValidate(CCommandVar p1, CCommandVar p2)
{
	Validate(true);
    return TRUE;
}
CCommandVar CParticleTools::CommandClear(CCommandVar p1, CCommandVar p2)
{
    Device.m_Camera.Reset();
    ResetPreviewObject();
    ExecCommand(COMMAND_UPDATE_CAPTION);
    return TRUE;
}
CCommandVar CParticleTools::CommandPlayCurrent(CCommandVar p1, CCommandVar p2)
{
    PlayCurrent();
    return TRUE;
}
CCommandVar CParticleTools::CommandStopCurrent(CCommandVar p1, CCommandVar p2)
{
    StopCurrent(p1);
    return TRUE;
}
CCommandVar CommandRefreshUIBar(CCommandVar p1, CCommandVar p2)
{
    fraTopBar->RefreshBar	();
    fraLeftBar->RefreshBar	();
    fraBottomBar->RefreshBar();
    return TRUE;
}
CCommandVar CommandRestoreUIBar(CCommandVar p1, CCommandVar p2)
{
    fraTopBar->fsStorage->RestoreFormPlacement();
    fraLeftBar->fsStorage->RestoreFormPlacement();
    fraBottomBar->fsStorage->RestoreFormPlacement();
    return TRUE;
}
CCommandVar CommandSaveUIBar(CCommandVar p1, CCommandVar p2)
{
    fraTopBar->fsStorage->SaveFormPlacement();
    fraLeftBar->fsStorage->SaveFormPlacement();
    fraBottomBar->fsStorage->SaveFormPlacement();
    return TRUE;
}
CCommandVar CommandUpdateToolBar(CCommandVar p1, CCommandVar p2)
{
    fraLeftBar->UpdateBar();
    return TRUE;
}
CCommandVar CommandUpdateCaption(CCommandVar p1, CCommandVar p2)
{
    frmMain->UpdateCaption();
    return TRUE;
}

void CParticleMain::RegisterCommands()
{
	inherited::RegisterCommands();
    // tools       
	REGISTER_CMD_CE	(COMMAND_SELECT_PREVIEW_OBJ,"Select Preview Object",PTools,CParticleTools::CommandSelectPreviewObj, true);
	REGISTER_CMD_CE	(COMMAND_EDIT_PREVIEW_PROPS,"Select Preview Props",	PTools,CParticleTools::CommandEditPreviewProps, true);
	REGISTER_CMD_CE	(COMMAND_SAVE,            	"File\\Save",			PTools,CParticleTools::CommandSave, true);
	REGISTER_CMD_C	(COMMAND_SAVE_BACKUP,       PTools,CParticleTools::CommandSaveBackup);
	REGISTER_CMD_CE	(COMMAND_LOAD,            	"File\\Reload",			PTools,CParticleTools::CommandReload, true);
	REGISTER_CMD_C	(COMMAND_VALIDATE,          PTools,CParticleTools::CommandValidate);
	REGISTER_CMD_CE	(COMMAND_CLEAR,             "File\\Clear",			PTools,CParticleTools::CommandClear, true);
	REGISTER_CMD_CE	(COMMAND_PLAY_CURRENT,      "Particles\\Play",		PTools,CParticleTools::CommandPlayCurrent, true);
    REGISTER_SUB_CMD_CE (COMMAND_STOP_CURRENT,	"Particles",			PTools,CParticleTools::CommandStopCurrent, true);
    	APPEND_SUB_CMD	("Stop Immediate",		0,0);
    	APPEND_SUB_CMD	("Stop Deffered", 		1,0);
    REGISTER_SUB_CMD_END;
	REGISTER_CMD_S	(COMMAND_REFRESH_UI_BAR,    CommandRefreshUIBar);
	REGISTER_CMD_S	(COMMAND_RESTORE_UI_BAR,    CommandRestoreUIBar);
	REGISTER_CMD_S	(COMMAND_SAVE_UI_BAR,     	CommandSaveUIBar);
	REGISTER_CMD_S	(COMMAND_UPDATE_TOOLBAR,    CommandUpdateToolBar);
	REGISTER_CMD_S	(COMMAND_UPDATE_CAPTION,    CommandUpdateCaption);
}                                                                    

char* CParticleMain::GetCaption()
{
	return "particles";
}

bool __fastcall CParticleMain::ApplyShortCut(WORD Key, TShiftState Shift)
{
    return inherited::ApplyShortCut(Key,Shift);
}
//---------------------------------------------------------------------------

bool __fastcall CParticleMain::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
    return inherited::ApplyGlobalShortCut(Key,Shift);
}
//---------------------------------------------------------------------------

void CParticleMain::RealUpdateScene()
{
	inherited::RealUpdateScene	();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Common
//---------------------------------------------------------------------------
void CParticleMain::ResetStatus()
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=""){
	    fraBottomBar->paStatus->Caption=""; fraBottomBar->paStatus->Repaint();
    }
}
void CParticleMain::SetStatus(LPSTR s, bool bOutLog)
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=s){
	    fraBottomBar->paStatus->Caption=s; fraBottomBar->paStatus->Repaint();
    	if (bOutLog&&s&&s[0]) ELog.Msg(mtInformation,s);
    }
}
void CParticleMain::ProgressDraw()
{
	fraBottomBar->RedrawBar();
}
//---------------------------------------------------------------------------
void CParticleMain::OutCameraPos()
{
	VERIFY(m_bReady);
    AnsiString s;
	const Fvector& c 	= Device.m_Camera.GetPosition();
	s.sprintf("C: %3.1f, %3.1f, %3.1f",c.x,c.y,c.z);
//	const Fvector& hpb 	= Device.m_Camera.GetHPB();
//	s.sprintf(" Cam: %3.1f°, %3.1f°, %3.1f°",rad2deg(hpb.y),rad2deg(hpb.x),rad2deg(hpb.z));
    fraBottomBar->paCamera->Caption=s; fraBottomBar->paCamera->Repaint();
}
//---------------------------------------------------------------------------
void CParticleMain::OutUICursorPos()
{
	VERIFY(m_bReady);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void CParticleMain::OutGridSize()
{
	VERIFY(fraBottomBar);
    AnsiString s;
    s.sprintf("Grid: %1.1f",EPrefs->grid_cell_size);
    fraBottomBar->paGridSquareSize->Caption=s; fraBottomBar->paGridSquareSize->Repaint();
}
//---------------------------------------------------------------------------
void CParticleMain::OutInfo()
{
	fraBottomBar->paSel->Caption = Tools->GetInfo();
}
//---------------------------------------------------------------------------
void CParticleMain::RealQuit()
{
	frmMain->Close();
}
//---------------------------------------------------------------------------

