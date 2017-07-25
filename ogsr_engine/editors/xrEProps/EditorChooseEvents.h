#ifndef EditorChooseEventsH
#define EditorChooseEventsH

#include "ChooseTypes.h"
#include "SkeletonAnimated.h"
//#include "LevelGameDef.h"

ref_sound* choose_snd;

namespace ChoseEvents{
void __stdcall  FillEntity(ChooseItemVec& items, void* param)
{
//.    AppendItem						(RPOINT_CHOOSE_NAME);
    CInifile::Root& data 			= pSettings->sections();
    for (CInifile::RootIt it=data.begin(); it!=data.end(); it++){
    	LPCSTR val;
    	if ((*it)->line_exist("$spawn",&val))
            items.push_back(SChooseItem(*(*it)->Name,""));
    }
}
//---------------------------------------------------------------------------
void __stdcall  SelectSoundSource(SChooseItem* item, PropItemVec& info_items)
{
	choose_snd->stop			();
	choose_snd->create			(item->name.c_str(),st_Effect,sg_Undefined);
    choose_snd->play			(0,sm_2D);
//    snd.pla
/*
//.
	ECustomThumbnail*& thm, ref_sound& snd, 
    thm 		= xr_new<ESoundThumbnail>(item->name.c_str());
*/
}
void __stdcall  CloseSoundSource()
{
	choose_snd->destroy			();
}
void __stdcall  FillSoundSource(ChooseItemVec& items, void* param)
{
    FS_FileSet lst;
    if (SndLib->GetSounds(lst)){
	    FS_FileSetIt  it			= lst.begin();
    	FS_FileSetIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->name.c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __stdcall  FillSoundEnv(ChooseItemVec& items, void* param)
{
    AStringVec lst;
    if (SndLib->GetSoundEnvs(lst)){
	    AStringIt  it				= lst.begin();
    	AStringIt	_E				= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __stdcall  FillObject(ChooseItemVec& items, void* param)
{
    FS_FileSet lst;
    if (Lib.GetObjects(lst)){
	    FS_FileSetIt	it			= lst.begin();
    	FS_FileSetIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->name.c_str(),""));
    }
}
void __stdcall  SelectObject(SChooseItem* item, PropItemVec& info_items)
{
	EObjectThumbnail* thm			= xr_new<EObjectThumbnail>(*item->name);
    if (thm->Valid()) thm->FillInfo	(info_items);
    xr_delete						(thm);
}
void __stdcall  DrawObjectTHM(LPCSTR name, HDC hdc, const Irect& r)
{
	EObjectThumbnail* thm			= xr_new<EObjectThumbnail>(name);
    if (thm->Valid()) thm->Draw		(hdc,r);
    xr_delete						(thm);
}
//---------------------------------------------------------------------------
void __stdcall  FillGroup(ChooseItemVec& items, void* param)
{
    FS_FileSet lst;
    if (FS.file_list(lst,_groups_,FS_ListFiles|FS_ClampExt,"*.group")){
	    FS_FileSetIt	it			= lst.begin();
    	FS_FileSetIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->name.c_str(),""));
    }
}
void __stdcall  SelectGroup(SChooseItem* item, PropItemVec& info_items)
{
	EGroupThumbnail* thm			= xr_new<EGroupThumbnail>(*item->name);
    if (thm->Valid()) thm->FillInfo	(info_items);
    xr_delete						(thm);
}
void __stdcall  DrawGroupTHM(LPCSTR name, HDC hdc, const Irect& r)
{
	EGroupThumbnail* thm			= xr_new<EGroupThumbnail>(name);
    if (thm->Valid()) thm->Draw		(hdc,r);
    xr_delete						(thm);
}
//---------------------------------------------------------------------------
void __stdcall  FillVisual(ChooseItemVec& items, void* param)
{
    FS_FileSet lst;
    if (FS.file_list(lst,_game_meshes_,FS_ListFiles|FS_ClampExt,"*.ogf")){
	    FS_FileSetIt	it			= lst.begin();
    	FS_FileSetIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->name.c_str(),""));
    }
}
void __stdcall  SelectVisual(SChooseItem* item, PropItemVec& info_items)
{
/*
//.
    AnsiString fn					= ChangeFileExt(item->name.c_str(),".ogf");
    IRender_Visual* visual			= ::Render->model_Create(fn.c_str());
    if (visual){ 
        PHelper().CreateCaption	(info_items,	"Source",	*visual->desc.source_file?*visual->desc.source_file:"unknown");
        PHelper().CreateCaption	(info_items, 	"Creator N",*visual->desc.create_name?*visual->desc.create_name:"unknown");
        PHelper().CreateCaption	(info_items,	"Creator T",Trim(AnsiString(ctime(&visual->desc.create_time))).c_str());
        PHelper().CreateCaption	(info_items,	"Modif N",	*visual->desc.modif_name ?*visual->desc.modif_name :"unknown");
        PHelper().CreateCaption	(info_items,	"Modif T",	Trim(AnsiString(ctime(&visual->desc.modif_time))).c_str());
        PHelper().CreateCaption	(info_items,	"Build N",	*visual->desc.build_name ?*visual->desc.build_name :"unknown");
        PHelper().CreateCaption	(info_items,	"Build T",	Trim(AnsiString(ctime(&visual->desc.build_time))).c_str());
    }
    ::Render->model_Delete(visual);
*/
}
//---------------------------------------------------------------------------
void __stdcall  FillGameObjectMots(ChooseItemVec& items, void* param)
{
    FS_FileSet lst;
    if (FS.file_list(lst,_game_meshes_,FS_ListFiles|FS_ClampExt,"*.omf")){
	    FS_FileSetIt	it			= lst.begin();
    	FS_FileSetIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->name.c_str(),""));
    }
}
void __stdcall  SelectGameObjectMots(SChooseItem* item, PropItemVec& info_items)
{
}
//---------------------------------------------------------------------------
void __stdcall  FillGameAnim(ChooseItemVec& items, void* param)
{
    FS_FileSet lst;
    if (FS.file_list(lst,"$game_anims$",FS_ListFiles,"*.anm,*.anms")){
	    FS_FileSetIt	it			= lst.begin();
    	FS_FileSetIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->name.c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __stdcall  FillLAnim(ChooseItemVec& items, void* param)
{
    LAItemVec& lst 					= LALib.Objects();
    LAItemIt it						= lst.begin();
    LAItemIt _E						= lst.end();
    for (; it!=_E; it++)			items.push_back(SChooseItem(*(*it)->cName,""));
}
void __stdcall  DrawLAnim(LPCSTR name, HDC hdc, const Irect& r)
{
    int frame;
	CLAItem* item 					= LALib.FindItem(name);
    HBRUSH hbr 						= CreateSolidBrush(item->CalculateBGR(Device.fTimeGlobal,frame));
    FillRect						(hdc,(RECT*)&r,hbr);
    DeleteObject 					(hbr);
}
//---------------------------------------------------------------------------
void __stdcall  FillEShader(ChooseItemVec& items, void* param)
{
    CResourceManager::map_Blender& blenders = Device.Resources->_GetBlenders();
	CResourceManager::map_BlenderIt _S = blenders.begin();
	CResourceManager::map_BlenderIt _E = blenders.end();
	for (; _S!=_E; _S++)			items.push_back(SChooseItem(_S->first,""));
}
//---------------------------------------------------------------------------
void __stdcall  FillCShader(ChooseItemVec& items, void* param)
{
    Shader_xrLCVec& shaders 		= Device.ShaderXRLC.Library();
	Shader_xrLCIt _F 				= shaders.begin();
	Shader_xrLCIt _E 				= shaders.end();
	for ( ;_F!=_E;_F++)				items.push_back(SChooseItem(_F->Name,""));
}
//---------------------------------------------------------------------------
void __stdcall  FillPE(ChooseItemVec& items, void* param)
{
    for (PS::PEDIt E=::Render->PSLibrary.FirstPED(); E!=::Render->PSLibrary.LastPED(); E++)items.push_back(SChooseItem(*(*E)->m_Name,""));
}
//---------------------------------------------------------------------------
void __stdcall  FillParticles(ChooseItemVec& items, void* param)
{
    for (PS::PEDIt E=::Render->PSLibrary.FirstPED(); E!=::Render->PSLibrary.LastPED(); E++)items.push_back(SChooseItem(*(*E)->m_Name,""));
    for (PS::PGDIt G=::Render->PSLibrary.FirstPGD(); G!=::Render->PSLibrary.LastPGD(); G++)items.push_back(SChooseItem(*(*G)->m_Name,""));
}
//---------------------------------------------------------------------------
void __stdcall  FillTexture(ChooseItemVec& items, void* param)
{
    FS_FileSet	lst;
    if (ImageLib.GetTextures(lst)){
	    FS_FileSetIt	it			= lst.begin();
    	FS_FileSetIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->name.c_str(),""));
    }
}

void __stdcall  DrawTextureTHM(LPCSTR name, HDC hdc, const Irect& r)
{
	if (name&&name[0]){
        ETextureThumbnail* thm		= xr_new<ETextureThumbnail>(name);
        if (thm->Valid()) thm->Draw	(hdc,r);
        xr_delete					(thm);
    }
}

//---------------------------------------------------------------------------
void __stdcall  FillTextureRaw(ChooseItemVec& items, void* param)
{
    FS_FileSet	lst;
    if (ImageLib.GetTexturesRaw(lst)){
	    FS_FileSetIt	it			= lst.begin();
    	FS_FileSetIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->name.c_str(),""));
    }
}

void __stdcall  DrawTextureTHMRaw(LPCSTR name, HDC hdc, const Irect& r)
{
	if (name&&name[0]){
        ETextureThumbnail* thm		= xr_new<ETextureThumbnail>(name);
        if (thm->Valid()) thm->Draw	(hdc,r);
        xr_delete					(thm);
    }
}

void __stdcall  SelectTexture(SChooseItem* item, PropItemVec& info_items)
{
	if (item->name.size()){
        ETextureThumbnail* thm			= xr_new<ETextureThumbnail>(*item->name);
        if (thm->Valid()) thm->FillInfo	(info_items);
        xr_delete						(thm);
    }
}
void __stdcall  SelectTextureRaw(SChooseItem* item, PropItemVec& info_items)
{
	if (item->name.size()){
        ETextureThumbnail* thm			= xr_new<ETextureThumbnail>(*item->name);
        if (thm->Valid()) thm->FillInfo	(info_items);
        xr_delete						(thm);
    }
}
//---------------------------------------------------------------------------
void __stdcall  FillGameMaterial(ChooseItemVec& items, void* param)
{
	GameMtlIt _F 					= GMLib.FirstMaterial();
	GameMtlIt _E 					= GMLib.LastMaterial();
	for ( ;_F!=_E;_F++)				items.push_back(SChooseItem(*(*_F)->m_Name,""));
}
//---------------------------------------------------------------------------

void __stdcall  FillSkeletonAnims(ChooseItemVec& items, void* param)
{
	IRender_Visual* V 				= ::Render->model_Create((LPCSTR)param);
    if (PKinematicsAnimated(V)){
		u32 cnt						= PKinematicsAnimated(V)->LL_MotionsSlotCount();
    	for (u32 k=0; k<cnt; k++){
            accel_map *ll_motions	= PKinematicsAnimated(V)->LL_Motions(k);
            accel_map::iterator 	_I, _E;
            _I						= ll_motions->begin();
            _E						= ll_motions->end();
            for (; _I!=_E; ++_I){ 
            	bool bFound			= false;
            	for (ChooseItemVecIt it=items.begin(); it!=items.end(); it++)
                	if (it->name==_I->first){bFound=true; break;}
                if (!bFound)		items.push_back(SChooseItem(*_I->first,""));
            }
        }
    }
	::Render->model_Delete			(V);
}
void __stdcall  FillSkeletonBones(ChooseItemVec& items, void* param)
{
	IRender_Visual* V 				= ::Render->model_Create((LPCSTR)param);
    if (PKinematics(V)){
        CKinematicsAnimated::accel  	*ll_bones	= PKinematics(V)->LL_Bones();
        CKinematicsAnimated::accel::iterator _I, _E;
        _I							= ll_bones->begin();
        _E							= ll_bones->end();
        for (; _I!=_E; ++_I) 		items.push_back(SChooseItem(*_I->first,""));
    }
	::Render->model_Delete			(V);
}
}
void FillChooseEvents()
{
	TfrmChoseItem::AppendEvents	(smSoundSource,		"Select Sound Source",		ChoseEvents::FillSoundSource,	ChoseEvents::SelectSoundSource,	    0,							ChoseEvents::CloseSoundSource,	0);
	TfrmChoseItem::AppendEvents	(smSoundEnv,		"Select Sound Environment",	ChoseEvents::FillSoundEnv,		0,								    0,							0,								0);
	TfrmChoseItem::AppendEvents	(smObject,			"Select Library Object",	ChoseEvents::FillObject,		ChoseEvents::SelectObject,		    ChoseEvents::DrawObjectTHM,	0,								0);
	TfrmChoseItem::AppendEvents	(smGroup,			"Select Group",				ChoseEvents::FillGroup,			ChoseEvents::SelectGroup,		    ChoseEvents::DrawGroupTHM,	0,								0);
	TfrmChoseItem::AppendEvents	(smEShader,			"Select Engine Shader",		ChoseEvents::FillEShader,		0,								    0,							0,								0);
	TfrmChoseItem::AppendEvents	(smCShader,			"Select Compiler Shader",	ChoseEvents::FillCShader,		0,					                0,							0,								0);
	TfrmChoseItem::AppendEvents	(smPE,				"Select Particle Effect",	ChoseEvents::FillPE,			0,					                0,							0,								0);
	TfrmChoseItem::AppendEvents	(smParticles,		"Select Particle System", 	ChoseEvents::FillParticles,		0,					                0,							0,								0);
	TfrmChoseItem::AppendEvents	(smTextureRaw,		"Select Source Texture",	ChoseEvents::FillTextureRaw,	ChoseEvents::SelectTextureRaw,	    ChoseEvents::DrawTextureTHMRaw,0,								0);
	TfrmChoseItem::AppendEvents	(smTexture,			"Select Texture",			ChoseEvents::FillTexture,		ChoseEvents::SelectTexture,		    ChoseEvents::DrawTextureTHM,0,								0);
	TfrmChoseItem::AppendEvents	(smEntityType,		"Select Entity",			ChoseEvents::FillEntity,		0,					                0,							0,								0);
	TfrmChoseItem::AppendEvents	(smLAnim,			"Select Light Animation",	ChoseEvents::FillLAnim,			0,							        ChoseEvents::DrawLAnim,		0,								SChooseEvents::flAnimated);
	TfrmChoseItem::AppendEvents	(smVisual,			"Select Visual",			ChoseEvents::FillVisual,		ChoseEvents::SelectVisual,		    0,							0,								0);
	TfrmChoseItem::AppendEvents	(smSkeletonAnims,	"Select Skeleton Animation",ChoseEvents::FillSkeletonAnims,	0,					                0,							0,								0);
	TfrmChoseItem::AppendEvents	(smSkeletonBones,	"Select Skeleton Bones",	ChoseEvents::FillSkeletonBones,	0,					                0,							0,								0);
	TfrmChoseItem::AppendEvents	(smGameMaterial,	"Select Game Material",		ChoseEvents::FillGameMaterial,	0,					                0,							0,								0);
	TfrmChoseItem::AppendEvents	(smGameAnim,		"Select Animation",			ChoseEvents::FillGameAnim,		0,					                0,							0,								0);
	TfrmChoseItem::AppendEvents	(smGameSMotions,	"Select Game Object Motions",ChoseEvents::FillGameObjectMots,ChoseEvents::SelectGameObjectMots,	0,							0,								0);
    choose_snd = xr_new<ref_sound>();
}
void ClearChooseEvents()
{
	TfrmChoseItem::ClearEvents	();
    xr_delete					(choose_snd);
}

//---------------------------------------------------------------------------
#endif
 