#include "stdafx.h"
#pragma hdrstop

#include "UI_LevelTools.h"
#include "ESceneControlsCustom.h"
#include "cursor3d.h"
#include "LeftBar.h"
#include "Scene.h"
#include "ui_levelmain.h"

#include "editlibrary.h"
#include "ObjectList.h"

#include "igame_persistent.h"
#include "Builder.h"

#define DETACH_FRAME(a) 	if (a){ (a)->Hide(); 	(a)->Parent = NULL; }
#define ATTACH_FRAME(a,b)	if (a){ (a)->Parent=(b);(a)->Show(); 		}

CLevelTools*&	LTools=(CLevelTools*)Tools;

TShiftState ssRBOnly;
//---------------------------------------------------------------------------
CLevelTools::CLevelTools()
{
    fFogness	= 0.9f;
    dwFogColor	= 0xffffffff;
    m_Flags.zero();
}
//---------------------------------------------------------------------------
CLevelTools::~CLevelTools()
{
}
//---------------------------------------------------------------------------

TForm*	CLevelTools::GetFrame()
{
	if (pCurTools) return pCurTools->pFrame;
    return 0;
}
//---------------------------------------------------------------------------
bool CLevelTools::OnCreate()
{
	inherited::OnCreate();
    target          = OBJCLASS_DUMMY;
    sub_target		= -1;
    pCurTools       = 0;
    ssRBOnly << ssRight;
    paParent 		= fraLeftBar->paFrames;   VERIFY(paParent);
    m_Flags.set		(flChangeAction,FALSE);
    m_Flags.set		(flChangeTarget,FALSE);
    // scene creating
    Scene->OnCreate	();
    // change target to Object
    ExecCommand		(COMMAND_CHANGE_TARGET, OBJCLASS_SCENEOBJECT);
	m_Props 		= TProperties::CreateForm("Object Inspector",0,alClient,TOnModifiedEvent(this,&CLevelTools::OnPropsModified),0,TOnCloseEvent(this,&CLevelTools::OnPropsClose),TProperties::plItemFolders|TProperties::plFolderStore|TProperties::plNoClearStore|TProperties::plFullExpand);
    pObjectListForm = TfrmObjectList::CreateForm();
    return true;
}
//---------------------------------------------------------------------------

void CLevelTools::OnDestroy()
{
	inherited::OnDestroy();
    TfrmObjectList::DestroyForm(pObjectListForm);
	TProperties::DestroyForm(m_Props);
    // scene destroing
    if (pCurTools) 		pCurTools->OnDeactivate();
	Scene->OnDestroy		();
}
//---------------------------------------------------------------------------
void CLevelTools::Reset()
{
	RealSetTarget(GetTarget(),estDefault,true);
}
//---------------------------------------------------------------------------

bool __fastcall CLevelTools::MouseStart(TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl){
    	if ((pCurTools->pCurControl->Action()!=etaSelect)&&
	    	(!pCurTools->IsEditable()||(pCurTools->ClassID==OBJCLASS_DUMMY))) return false;
        return pCurTools->pCurControl->Start(Shift);
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall CLevelTools::MouseMove(TShiftState Shift) 
{
    if(pCurTools&&pCurTools->pCurControl){
	    if (HiddenMode())	ExecCommand(COMMAND_UPDATE_PROPERTIES);
     	pCurTools->pCurControl->Move(Shift);
    }
}
//---------------------------------------------------------------------------
bool __fastcall CLevelTools::MouseEnd(TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl){	
	    if (HiddenMode())	ExecCommand(COMMAND_UPDATE_PROPERTIES);
    	return pCurTools->pCurControl->End(Shift);
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall CLevelTools::OnObjectsUpdate()
{
	UpdateProperties(false);
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->OnObjectsUpdate();
}
//---------------------------------------------------------------------------
bool __fastcall CLevelTools::HiddenMode()
{
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->pCurControl->HiddenMode();
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall CLevelTools::KeyDown   (WORD Key, TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->pCurControl->KeyDown(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall CLevelTools::KeyUp     (WORD Key, TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->pCurControl->KeyUp(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall CLevelTools::KeyPress  (WORD Key, TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->pCurControl->KeyPress(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------

void CLevelTools::RealSetAction   (ETAction act)
{
	inherited::SetAction(act);
    if (pCurTools) pCurTools->SetAction(act);
    ExecCommand(COMMAND_UPDATE_TOOLBAR);
    m_Flags.set	(flChangeAction,FALSE);
}

void __fastcall CLevelTools::SetAction(ETAction act)
{
	// если мышь захвачена - изменим action после того как она освободится
	if (UI->IsMouseCaptured()||UI->IsMouseInUse()||!false){
	    m_Flags.set	(flChangeAction,TRUE);
        iNeedAction=act;
    }else
    	RealSetAction	(act);
}
//---------------------------------------------------------------------------

void __fastcall CLevelTools::RealSetTarget   (ObjClassID tgt,int sub_tgt,bool bForced)
{
    if(bForced||(target!=tgt)||(sub_target!=sub_tgt)){
        target 					= tgt;
        sub_target 				= sub_tgt;
        if (pCurTools){
            DETACH_FRAME(pCurTools->pFrame);
            pCurTools->OnDeactivate();
        }
        pCurTools				= Scene->GetMTools(tgt); VERIFY(pCurTools);
        pCurTools->SetSubTarget	(sub_target);

        pCurTools->OnActivate	();
        
        pCurTools->SetAction	(GetAction());
        if (pCurTools->IsEditable()) ATTACH_FRAME(pCurTools->pFrame, paParent); 
    }
    UI->RedrawScene();
    fraLeftBar->ChangeTarget(tgt);
    fraLeftBar->UpdateSnapList();
    ExecCommand(COMMAND_UPDATE_TOOLBAR);
    m_Flags.set(flChangeTarget,FALSE);
}
//---------------------------------------------------------------------------
void __fastcall CLevelTools::ResetSubTarget()
{
	VERIFY(pCurTools);
	pCurTools->ResetSubTarget();
}
//---------------------------------------------------------------------------
void __fastcall CLevelTools::SetTarget(ObjClassID tgt, int sub_tgt)
{
	// если мышь захвачена - изменим target после того как она освободится
	if (UI->IsMouseCaptured()||UI->IsMouseInUse()||!false){
	    m_Flags.set(flChangeTarget,TRUE);
        iNeedTarget		= tgt;
        iNeedSubTarget  = sub_tgt;
    }else
    	RealSetTarget(tgt,sub_tgt,false);
}
//---------------------------------------------------------------------------

ObjClassID CLevelTools::CurrentClassID()
{
	return GetTarget();
}
//---------------------------------------------------------------------------

void CLevelTools::OnShowHint(AStringVec& ss)
{
	Scene->OnShowHint(ss);
}
//---------------------------------------------------------------------------

bool CLevelTools::Pick(TShiftState Shift)
{
    if( Scene->locked() && (esEditLibrary==UI->GetEState())){
        UI->IR_GetMousePosReal(Device.m_hRenderWnd, UI->m_CurrentCp);
        UI->m_StartCp = UI->m_CurrentCp;
        Device.m_Camera.MouseRayFromPoint(UI->m_CurrentRStart, UI->m_CurrentRNorm, UI->m_CurrentCp );
        SRayPickInfo pinf;
        TfrmEditLibrary::RayPick(UI->m_CurrentRStart,UI->m_CurrentRNorm,&pinf);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

void CLevelTools::RefreshProperties()
{
	m_Props->RefreshForm();
}

void CLevelTools::ShowProperties()
{
    m_Props->ShowProperties	();
    UpdateProperties		(false);
    UI->RedrawScene			();
}
//---------------------------------------------------------------------------

void CLevelTools::RealUpdateProperties()
{
	if (m_Props->Visible){
		if (m_Props->IsModified()) Scene->UndoSave();
        ObjectList lst;
        PropItemVec items;
        // scene common props
        Scene->FillProp				("",items,CurrentClassID());
		m_Props->AssignItems		(items);
    }
	m_Flags.set(flUpdateProperties,FALSE);
}
//---------------------------------------------------------------------------

void CLevelTools::OnPropsClose()
{
	if (m_Props->IsModified()) Scene->UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall CLevelTools::OnPropsModified()
{
	Scene->Modified();
//	Scene->UndoSave();
	UI->RedrawScene();
}
//---------------------------------------------------------------------------

#include "EditLightAnim.h"

bool CLevelTools::IfModified()
{
    EEditorState est 		= UI->GetEState();
    switch(est){
    case esEditLightAnim: 	return TfrmEditLightAnim::FinalClose();
    case esEditLibrary: 	return TfrmEditLibrary::FinalClose();
    case esEditScene:		return Scene->IfModified();
    default: THROW;
    }
    return false;
}
//---------------------------------------------------------------------------

void CLevelTools::ZoomObject(BOOL bSelectedOnly)
{
    if( !Scene->locked() ){
        Scene->ZoomExtents(CurrentClassID(),bSelectedOnly);
    } else {
        if (UI->GetEState()==esEditLibrary){
            TfrmEditLibrary::ZoomObject();
        }
    }
}
//---------------------------------------------------------------------------

void CLevelTools::GetCurrentFog(u32& fog_color, float& s_fog, float& e_fog)
{
	if (psDeviceFlags.is(rsEnvironment)&&psDeviceFlags.is(rsFog)){
        s_fog				= g_pGamePersistent->Environment().CurrentEnv.fog_near;
        e_fog				= g_pGamePersistent->Environment().CurrentEnv.fog_far;
        Fvector& f_clr		= g_pGamePersistent->Environment().CurrentEnv.fog_color;
        fog_color 			= color_rgba_f(f_clr.x,f_clr.y,f_clr.z,1.f);
    }else{
        s_fog				= psDeviceFlags.is(rsFog)?(1.0f - fFogness)* 0.85f * UI->ZFar():0.99f*UI->ZFar();
        e_fog				= psDeviceFlags.is(rsFog)?0.91f * UI->ZFar():UI->ZFar();
        fog_color 			= dwFogColor;
    }
/*
//.
    f_near	= g_pGamePersistent->Environment.Current.fog_near;
    f_far	= 1/(g_pGamePersistent->Environment.Current.fog_far - f_near);
*/
}
//---------------------------------------------------------------------------

LPCSTR CLevelTools::GetInfo()
{
	static AnsiString sel;
	int cnt = Scene->SelectionCount(true,CurrentClassID());
	return sel.sprintf(" Sel: %d",cnt).c_str();
}
//---------------------------------------------------------------------------

void __fastcall CLevelTools::OnFrame()
{
	Scene->OnFrame		(Device.fTimeDelta);
    EEditorState est 	= UI->GetEState();
    if ((est==esEditScene)||(est==esEditLibrary)||(est==esEditLightAnim)){
        if (!UI->IsMouseCaptured()){
            // если нужно изменить target выполняем после того как мышь освободится
            if(m_Flags.is(flChangeTarget)) 		RealSetTarget(iNeedTarget,iNeedSubTarget,false);
            // если нужно изменить action выполняем после того как мышь освободится
            if(m_Flags.is(flChangeAction)) 		RealSetAction(ETAction(iNeedAction));
        }
        if (m_Flags.is(flUpdateProperties)) 	RealUpdateProperties();
        if (m_Flags.is(flUpdateObjectList)) 	RealUpdateObjectList();
        if (est==esEditLightAnim) TfrmEditLightAnim::OnIdle();
    }
}
//---------------------------------------------------------------------------
#include "d3dutils.h"
void __fastcall CLevelTools::RenderEnvironment()
{
    // draw sky
    EEditorState est 		= UI->GetEState();
    switch(est){
    case esEditLightAnim:
    case esEditScene:		
    	if (psDeviceFlags.is(rsEnvironment)){ 
    		g_pGamePersistent->Environment().RenderSky	();
    		g_pGamePersistent->Environment().RenderClouds	();
        }
    }
}

void __fastcall CLevelTools::Render()
{
	// Render update
    ::Render->Calculate		();
    ::Render->Render		();

    EEditorState est 		= UI->GetEState();
    // draw scene
    switch(est){
    case esEditLibrary: 	TfrmEditLibrary::OnRender(); 	break;
    case esEditLightAnim:
    case esEditScene:
    	Scene->Render(Device.m_Camera.GetTransform()); 
	    if (psDeviceFlags.is(rsEnvironment)) g_pGamePersistent->Environment().RenderLast	();
    break;
    case esBuildLevel:  	Builder.OnRender();				break;
    }
    // draw cursor
    LUI->m_Cursor->Render();

    inherited::Render		();
}
//---------------------------------------------------------------------------

void CLevelTools::ShowObjectList()
{
	if (pObjectListForm) pObjectListForm->ShowObjectList();
}
//---------------------------------------------------------------------------

void CLevelTools::RealUpdateObjectList()
{
	if (pObjectListForm) pObjectListForm->UpdateObjectList();
	m_Flags.set(flUpdateObjectList,FALSE);
}
//---------------------------------------------------------------------------

bool CLevelTools::IsModified()
{
	return Scene->IsUnsaved();
}
//---------------------------------------------------------------------------

#include "../ECore/Editor/EditMesh.h"
bool CLevelTools::RayPick(const Fvector& start, const Fvector& dir, float& dist, Fvector* pt, Fvector* n)
{
    if (Scene->ObjCount()&&(UI->GetEState()==esEditScene)){
        SRayPickInfo pinf;
        pinf.inf.range	= dist;
        if (Scene->RayPickObject(dist, start,dir,OBJCLASS_SCENEOBJECT,&pinf,0)){ 
        	dist		= pinf.inf.range;
        	if (pt) 	pt->set(pinf.pt); 
            if (n){	
                const Fvector* PT[3];
                pinf.e_mesh->GetFacePT(pinf.inf.id, PT);
            	n->mknormal(*PT[0],*PT[1],*PT[2]);
            }
            return true;
        }
    }
    Fvector N={0.f,-1.f,0.f};
    Fvector P={0.f,0.f,0.f};
    Fplane PL; PL.build(P,N);
    float d;
    if (PL.intersectRayDist(start,dir,d)&&(d<=dist)){
        dist = d;
        if (pt) pt->mad(start,dir,dist); 
        if (n)	n->set(N);
        return true;
    }else return false;
}

