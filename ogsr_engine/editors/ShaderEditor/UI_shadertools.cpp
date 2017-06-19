//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_ShaderTools.h"
#include "../xrEProps/ChoseForm.h"
#include "../ECore/Editor/ui_main.h"
#include "leftbar.h"
#include "../xrEProps/PropertiesList.h"
#include "blenders/Blender.h"
#include "../ECore/Engine/GameMtlLib.h"
#include "../xrEProps/ItemList.h"

//------------------------------------------------------------------------------
CShaderTools*&	STools=(CShaderTools*)Tools;
//------------------------------------------------------------------------------

CShaderTools::CShaderTools()
{
	m_Current			= 0;
    m_Items				= 0;
    m_ItemProps			= 0;
    m_PreviewProps		= 0;
    fFogness			= 0.9f;
    dwFogColor			= 0xffffffff;
    m_Flags.zero		();
}
//---------------------------------------------------------------------------

CShaderTools::~CShaderTools()
{
}
//---------------------------------------------------------------------------

void CShaderTools::OnChangeEditor(ISHTools* tools)
{
	if (m_Current) m_Current->OnDeactivate();
	m_Current = tools; R_ASSERT(m_Current);
	m_Current->OnActivate();
    ExecCommand(COMMAND_UPDATE_PROPERTIES);
	ExecCommand(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

bool CShaderTools::IfModified()
{	
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (!it->second->IfModified()) return false;
	return true;
}

bool CShaderTools::IsModified()
{
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (it->second->IsModified()) return true;
	return false;
}

void CShaderTools::Modified()
{
	Current()->Modified();
}
//---------------------------------------------------------------------------

bool CShaderTools::OnCreate()
{
	string_path 			fn;
	// shader test locking
    FS.update_path		(fn,_game_data_,"shaders.xr");
	if (EFS.CheckLocking(fn,false,true)) return false;
    EFS.LockFile		(fn);
	// shader test locking
    FS.update_path		(fn,_game_data_,"shaders_xrlc.xr");
	if (EFS.CheckLocking(fn,false,true)) return false;
    EFS.LockFile		(fn);
	// material test locking
    FS.update_path		(fn,_game_data_,GAMEMTL_FILENAME);
	if (EFS.CheckLocking(fn,false,true)) return false;
    EFS.LockFile		(fn);
	// sound_env test locking
    FS.update_path		(fn,_game_data_,SNDENV_FILENAME);
	if (EFS.CheckLocking(fn,false,true)) return false;
    EFS.LockFile		(fn);

    // create props
    m_Items				= TItemList::CreateForm		("Items",				fraLeftBar->paItemList,		alClient,TItemList::ilEditMenu|TItemList::ilDragAllowed|TItemList::ilFolderStore);
	m_Items->SetOnItemsFocusedEvent(fastdelegate::bind<TOnILItemsFocused>(this,&CShaderTools::OnItemFocused));
    m_ItemProps 		= TProperties::CreateForm	("Item Properties",		fraLeftBar->paShaderProps,	alClient);
    m_PreviewProps  	= TProperties::CreateForm	("Preview Properties",	fraLeftBar->paPreviewProps,	alClient);

    // create tools
    RegisterTools		();

	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (!it->second->OnCreate()) return false;

    return true;
}

void CShaderTools::OnDestroy()
{
	// unlock
	string_path			fn;
    FS.update_path		(fn,_game_data_,"shaders.xr");
    EFS.UnlockFile		(fn);
    FS.update_path		(fn,_game_data_,"shaders_xrlc.xr");
    EFS.UnlockFile		(fn);
    FS.update_path		(fn,_game_data_,GAMEMTL_FILENAME);
    EFS.UnlockFile		(fn);
    FS.update_path		(fn,_game_data_,SNDENV_FILENAME);
    EFS.UnlockFile		(fn);

	// destroy props
    TItemList::DestroyForm	(m_Items);
	TProperties::DestroyForm(m_ItemProps);
    TProperties::DestroyForm(m_PreviewProps);
	//
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	it->second->OnDestroy();

	UnregisterTools		();
}

#include "igame_persistent.h"

void CShaderTools::RenderEnvironment()
{
    if (psDeviceFlags.is(rsEnvironment)){
        g_pGamePersistent->Environment().RenderSky	();
        g_pGamePersistent->Environment().RenderClouds	();
    }
}

void CShaderTools::Render()
{
    PrepareLighting		();
	Current()->OnRender	();
    if (psDeviceFlags.is(rsEnvironment)) g_pGamePersistent->Environment().RenderLast	();
    inherited::Render	();
}

void CShaderTools::OnFrame()
{
	if (m_Flags.is(flRefreshList)) 
    	RealUpdateList();
	if (m_Flags.is(flRefreshProps)) 
    	RealUpdateProperties();
	Current()->OnFrame();
}

void CShaderTools::ZoomObject(BOOL bOnlySel)
{
	Current()->ZoomObject(bOnlySel);
}

void CShaderTools::PrepareLighting()
{
    // add directional light
    Flight L;
    ZeroMemory(&L,sizeof(Flight));
    L.type = D3DLIGHT_DIRECTIONAL;
    L.diffuse.set(1,1,1,1);
    L.direction.set(1,-1,1); L.direction.normalize();
	Device.SetLight(0,L);
	Device.LightEnable(0,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(-1,-1,-1); L.direction.normalize();
	Device.SetLight(1,L);
	Device.LightEnable(1,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(1,-1,-1); L.direction.normalize();
	Device.SetLight(2,L);
	Device.LightEnable(2,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(-1,-1,1); L.direction.normalize();
	Device.SetLight(3,L);
	Device.LightEnable(3,true);

	L.diffuse.set(1.0,0.8,0.7,1);
    L.direction.set(0,1,0); L.direction.normalize();
	Device.SetLight(4,L);
	Device.LightEnable(4,true);
}

void CShaderTools::OnDeviceCreate()
{
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (it->second) it->second->OnDeviceCreate();
}

void CShaderTools::OnDeviceDestroy()
{
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (it->second) it->second->OnDeviceDestroy();
}

void CShaderTools::OnShowHint(AStringVec& ss)
{
	Current()->OnShowHint(ss);
}

void CShaderTools::ApplyChanges()
{
	Current()->ApplyChanges();
}

void CShaderTools::ShowProperties()
{
	m_ItemProps->ShowProperties();
}

LPCSTR CShaderTools::CurrentToolsName()
{
	return Current()?Current()->ToolsName():"";
}

LPCSTR CShaderTools::GetInfo()
{
	return 0;
}

ISHTools* CShaderTools::FindTools(EToolsID id)
{
	ToolsPairIt it = m_Tools.find(id); R_ASSERT(it!=m_Tools.end());
    return it->second;
}

ISHTools* CShaderTools::FindTools(TElTabSheet* sheet)
{
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (it->second->Sheet()==sheet) return it->second;
    return 0;
}

bool CShaderTools::Load(LPCSTR name)
{
	return true;
}

bool CShaderTools::Save(LPCSTR name, bool bInternal)
{
	bool bRes = true;
    for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
        if (!it->second->Save()) bRes=false;
	return bRes;
}

void CShaderTools::Reload()
{
	if (!Current()->IfModified()) return;
    if (ELog.DlgMsg(mtConfirmation,"Reload current items?")==mrYes)
        Current()->Reload();
}

#include "SHEngineTools.h"
#include "SHGameMtlTools.h"
#include "SHGameMtlPairTools.h"
#include "SHCompilerTools.h"
#include "SHSoundEnvTools.h"

void CShaderTools::RegisterTools()
{
	for (int k=aeFirstTool; k<aeMaxTools; k++){	
    	ISHTools* tools = 0;
		switch(k){
		case aeEngine:		tools = xr_new<CSHEngineTools>		(ISHInit( EToolsID(k),	m_Items,	fraLeftBar->tsEngine,	m_ItemProps,	m_PreviewProps));   break;
    	case aeCompiler:	tools = xr_new<CSHCompilerTools>	(ISHInit( EToolsID(k),	m_Items,	fraLeftBar->tsCompiler, m_ItemProps,	m_PreviewProps));	break;
    	case aeMtl:			tools = xr_new<CSHGameMtlTools>		(ISHInit( EToolsID(k),	m_Items,	fraLeftBar->tsMaterial,	m_ItemProps,	m_PreviewProps));	break;
    	case aeMtlPair:		tools = xr_new<CSHGameMtlPairTools>	(ISHInit( EToolsID(k),	m_Items,	fraLeftBar->tsMaterialPair,m_ItemProps,	m_PreviewProps));	break;
    	case aeSoundEnv:	tools = xr_new<CSHSoundEnvTools>	(ISHInit( EToolsID(k),	m_Items,	fraLeftBar->tsSoundEnv,	m_ItemProps,	m_PreviewProps));	break;
        }
        R_ASSERT(tools);
		m_Tools.insert(mk_pair(k,tools));
    }
}

void CShaderTools::UnregisterTools()
{
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	xr_delete(it->second);
}

#include "../ECore/Editor/EditMesh.h"
bool CShaderTools::RayPick(const Fvector& start, const Fvector& dir, float& dist, Fvector* pt, Fvector* n)
{
/*
    if (m_EditObject)
    {
		SRayPickInfo pinf;
		if (m_EditObject->RayPick(dist,start,dir,Fidentity,&pinf)){
        	if (pt) pt->set(pinf.pt); 
            if (n){	
                const Fvector* PT[3];
                pinf.e_mesh->GetFacePT(pinf.inf.id, PT);
            	n->mknormal(*PT[0],*PT[1],*PT[2]);
            }
            return true;
        }else return false;
    }else
*/    
    {
    	Fvector np; np.mad(start,dir,dist);
    	if ((start.y>0)&&(np.y<0.f)){
            if (pt) pt->set(start); 
            if (n)	n->set(0.f,1.f,0.f);
            return true;
        }else return false;
    }
}

void CShaderTools::RealUpdateProperties()
{
    Current()->RealUpdateProperties();
	m_Flags.set(flRefreshProps,FALSE);
}

void CShaderTools::RealUpdateList()
{
    Current()->RealUpdateList();
	m_Flags.set(flRefreshList,FALSE);
}

void __fastcall CShaderTools::OnItemFocused(ListItemsVec& items)
{
	LPCSTR name				= 0;
    Current()->m_CurrentItem= 0;
    
	if (!items.empty()){
    	VERIFY(items.size()==1);
        Current()->m_CurrentItem	= *items.begin();
        name						= Current()->m_CurrentItem->Key();
    }
    Current()->SetCurrentItem(name,false);
    ExecCommand				(COMMAND_UPDATE_PROPERTIES);
}
//------------------------------------------------------------------------------

