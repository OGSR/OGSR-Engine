//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#ifdef _PARTICLE_EDITOR

#include "../xrEProps/ChoseForm.h"
#include "ParticleEffect.h"
#include "ParticleEffectActions.h"
#include "../xrEProps/PropertiesListHelper.h"
#include "ui_particletools.h"
#include "ui_main.h"
#include "../xrEProps/FolderLib.h"
             
BOOL PS::CPEDef::Equal(const CPEDef* pe)
{
    if (!m_Flags.equal(pe->m_Flags)) 						return FALSE;
    if (!m_ShaderName.equal(pe->m_ShaderName)) 				return FALSE;
    if (!m_TextureName.equal(pe->m_TextureName))			return FALSE;
    if (0!=memcmp(&m_Frame,&pe->m_Frame,sizeof(m_Frame))) 	return FALSE;
	if (!fsimilar(m_fTimeLimit,pe->m_fTimeLimit))			return FALSE;
    if (m_MaxParticles!=pe->m_MaxParticles)					return FALSE;
    if (m_Actions.size()!=pe->m_Actions.size())				return FALSE;
    if (!m_VelocityScale.similar(pe->m_VelocityScale))		return FALSE;
	if (!fsimilar(m_fCollideOneMinusFriction,pe->m_fCollideOneMinusFriction))	return FALSE;
    if (!fsimilar(m_fCollideResilience,pe->m_fCollideResilience))				return FALSE;
    if (!fsimilar(m_fCollideSqrCutoff,pe->m_fCollideSqrCutoff))					return FALSE;
    return TRUE;
}

void PS::CPEDef::Copy(const CPEDef& src)
{
    m_Name				= src.m_Name; 
    m_Flags				= src.m_Flags;
    m_ShaderName		= src.m_ShaderName;
    m_TextureName		= src.m_TextureName;
    m_Frame				= src.m_Frame;
	m_fTimeLimit		= src.m_fTimeLimit;
    m_MaxParticles		= src.m_MaxParticles;
	m_CachedShader		= src.m_CachedShader;
    m_VelocityScale.set			(src.m_VelocityScale);
	m_fCollideOneMinusFriction	= src.m_fCollideOneMinusFriction;
    m_fCollideResilience		= src.m_fCollideResilience;
    m_fCollideSqrCutoff			= src.m_fCollideSqrCutoff; 

    m_EActionList.resize(src.m_EActionList.size());
    for (u32 k=0; k<src.m_EActionList.size(); k++){
        PAPI::PActionEnum type 	= src.m_EActionList[k]->type;
        m_EActionList[k]		= pCreateEAction(type);
        *m_EActionList[k]		= *src.m_EActionList[k];
    }
	Compile				();
}

void __fastcall PS::CPEDef::OnControlClick(ButtonValue* B, bool& bDataModified, bool& bSafe)
{
    switch (B->btn_num){
    case 0: 			PTools->PlayCurrent();		break;
    case 1: 			PTools->StopCurrent(false);	break;
    case 2: 			PTools->StopCurrent(true);	break;
    }
    bDataModified		= false;
}

void __fastcall PS::CPEDef::FindActionByName(LPCSTR new_name, bool& res)
{
	res 				= false;
	for (EPAVecIt s_it=m_EActionList.begin(); s_it!=m_EActionList.end(); s_it++)
    	if (0==stricmp(new_name,*(*s_it)->actionName)){res=true; break;};
}

IC __fastcall void PS::CPEDef::FillActionList(ChooseItemVec& items, void* param)
{
    for(int i=0; actions_token[i].name; i++)
        items.push_back(SChooseItem(actions_token[i].name,actions_token[i].info));
}

void __fastcall PS::CPEDef::OnActionsClick(ButtonValue* B, bool& bDataModified, bool& bSafe)
{
    switch (B->btn_num){
    case 0:{
    	LPCSTR 		nm;
	    if (TfrmChoseItem::SelectItem(smCustom,nm,1,0,fastdelegate::bind<TOnChooseFillItems>(this,&PS::CPEDef::FillActionList))&&nm){
            for(int i=0; actions_token[i].name; i++){
                if (0==strcmp(actions_token[i].name,nm)){
                    EParticleAction* A = pCreateEAction((PAPI::PActionEnum)actions_token[i].id);
                    AnsiString pref	= AnsiString(*A->actionName).LowerCase();
                    A->actionName	= FHelper.GenerateName(pref.c_str(),2,fastdelegate::bind<TFindObjectByName>(this,&PS::CPEDef::FindActionByName),true,true).LowerCase().c_str();
                    m_EActionList.push_back(A);
                    ExecCommand		(COMMAND_UPDATE_PROPERTIES);
                    bDataModified	= true;
                    bSafe			= true;
                	return ;
                }
			}        	
        }
    }break;
    }
    bDataModified	= false;
}

void __fastcall PS::CPEDef::OnFlagChange(PropValue* sender)
{
    ExecCommand			(COMMAND_UPDATE_PROPERTIES);
}          

void __fastcall PS::CPEDef::OnShaderChange(PropValue* sender)
{
	m_CachedShader.destroy	();
	if (m_ShaderName.size()&&m_TextureName.size())
		m_CachedShader.create(m_ShaderName.c_str(),m_TextureName.c_str());
}          

void __fastcall PS::CPEDef::OnFrameResize(PropValue* sender)
{
	m_Frame.m_iFrameDimX	= iFloor(1.f/m_Frame.m_fTexSize.x);
}

void PS::CPEDef::CollisionFrictionOnBeforeEdit(PropValue* sender, float& edit_val)
{    edit_val = 1.f-edit_val;}
bool PS::CPEDef::CollisionFrictionOnAfterEdit(PropValue* sender, float& edit_val)
{    edit_val = 1.f-edit_val; return true;}
void PS::CPEDef::CollisionFrictionOnDraw(PropValue* sender, xr_string& draw_val)
{    
	FloatValue* V	= dynamic_cast<FloatValue*>(sender); VERIFY(V);
	draw_sprintf(draw_val,1.f-V->GetValue(),V->dec);
}
void PS::CPEDef::CollisionCutoffOnBeforeEdit(PropValue* sender, float& edit_val)
{    edit_val = _sqrt(edit_val);}
bool PS::CPEDef::CollisionCutoffOnAfterEdit(PropValue* sender, float& edit_val)
{    edit_val = (edit_val)*(edit_val); return true;}
void PS::CPEDef::CollisionCutoffOnDraw(PropValue* sender, xr_string& draw_val)
{    
	FloatValue* V	= dynamic_cast<FloatValue*>(sender); VERIFY(V);
	draw_sprintf(draw_val,_sqrt(V->GetValue()),V->dec);
}

void __fastcall PS::CPEDef::OnActionEditClick(ButtonValue* B, bool& bDataModified, bool& bSafe)
{
    bDataModified	= false;
    int idx			= B->tag;
    switch (B->btn_num){
    case 0:		    // up
    	if (idx>0){
        	EParticleAction* E	= m_EActionList[idx-1];
            m_EActionList[idx-1]= m_EActionList[idx];
            m_EActionList[idx]	= E;
            ExecCommand			(COMMAND_UPDATE_PROPERTIES);
	        bDataModified		= true;
        }
    break;
    case 1:		    // down
    	if (idx<(int(m_EActionList.size())-1)){
        	EParticleAction* E	= m_EActionList[idx+1];
            m_EActionList[idx+1]= m_EActionList[idx];
            m_EActionList[idx]	= E;
            ExecCommand			(COMMAND_UPDATE_PROPERTIES);
	        bDataModified		= true;
        }
        bDataModified	= true;
    break;
    case 2:        
        if (ELog.DlgMsg(mtConfirmation, TMsgDlgButtons() << mbYes << mbNo,"Remove action?") == mrYes){
            PTools->RemoveAction(idx);
            ExecCommand		(COMMAND_UPDATE_PROPERTIES);
            bDataModified	= true;
        }
    break;
    }
}

bool PS::CPEDef::OnAfterActionNameEdit(PropValue* sender, shared_str& edit_val)
{
	bool found				= false;
	edit_val				= AnsiString(edit_val.c_str()).LowerCase().c_str();
    FindActionByName		(edit_val.c_str(),found); 
    return 					!found;
}
void PS::CPEDef::FillProp(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner)
{
	PHelper().CreateCaption	(items,PrepareKey(pref,"Version\\Owner Name"),	*m_OwnerName);
	PHelper().CreateCaption	(items,PrepareKey(pref,"Version\\Modif Name"),	*m_ModifName);
	PHelper().CreateCaption	(items,PrepareKey(pref,"Version\\Creation Time"),	Trim(AnsiString(ctime(&m_CreateTime))).c_str());
	PHelper().CreateCaption	(items,PrepareKey(pref,"Version\\Modified Time"),	Trim(AnsiString(ctime(&m_ModifTime))).c_str());
	ButtonValue* B;
	B=PHelper().CreateButton(items,PrepareKey(pref,"Control"),"Play(F5),Stop(F6),Stop...(F7)",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent.bind	(this,&PS::CPEDef::OnControlClick);
	PHelper().CreateName	(items,PrepareKey(pref,"Name"),&m_Name,owner);

    PropValue* P = 0;
    // max particles
    PHelper().CreateS32		(items,PrepareKey				(pref,"Max Particles"),					&m_MaxParticles,  0, 100000);
//    P->OnChangeEvent		= OnFlagChange;
	// time limit
    P=PHelper().CreateFlag32(items,PrepareKey				(pref,"Time Limit"),		  			&m_Flags, dfTimeLimit);
    P->OnChangeEvent.bind	(this,&PS::CPEDef::OnFlagChange);
    if (m_Flags.is(dfTimeLimit))
	    PHelper().CreateFloat	(items,PrepareKey			(pref,"Time Limit\\Value (sec)"),		&m_fTimeLimit,  0, 10000.f);
	// sprite
    P=PHelper().CreateFlag32(items,PrepareKey				(pref,"Sprite"),		 	   			&m_Flags, dfSprite);
    P->OnChangeEvent.bind	(this,&PS::CPEDef::OnFlagChange);
    if (m_Flags.is(dfSprite)){
	    P=PHelper().CreateChoose(items,PrepareKey			(pref,"Sprite\\Texture"), 	   			&m_TextureName, smTexture, 0,0, 2);
        P->OnChangeEvent.bind	(this,&PS::CPEDef::OnShaderChange);
	    P=PHelper().CreateChoose(items,PrepareKey			(pref,"Sprite\\Shader"), 	   			&m_ShaderName,	smEShader);
        P->OnChangeEvent.bind	(this,&PS::CPEDef::OnShaderChange);
    	// frame
        P=PHelper().CreateFlag32(items,PrepareKey			(pref,"Sprite\\Culling"),			 	&m_Flags, dfCulling);
        P->OnChangeEvent.bind	(this,&PS::CPEDef::OnFlagChange);
        if (m_Flags.is(CPEDef::dfCulling))
        	PHelper().CreateFlag32(items,PrepareKey			(pref,"Sprite\\Culling\\CCW"),			&m_Flags, dfCullCCW);
        P=PHelper().CreateFlag32(items,PrepareKey			(pref,"Sprite\\Frame"),		 		 	&m_Flags, dfFramed);
        P->OnChangeEvent.bind	(this,&PS::CPEDef::OnFlagChange);
        if (m_Flags.is(dfFramed)){
            PHelper().CreateFlag32(items,PrepareKey		(pref,"Sprite\\Frame\\Random Init"), 	&m_Flags, dfRandomFrame);
            PHelper().CreateS32	(items,PrepareKey			(pref,"Sprite\\Frame\\Count"),			&m_Frame.m_iFrameCount, 1,256);
            P=PHelper().CreateFloat(items,PrepareKey		(pref,"Sprite\\Frame\\Size U (0..1)"),	&m_Frame.m_fTexSize.x, EPS_S,1.f,0.001f,8);
            P->OnChangeEvent.bind	(this,&PS::CPEDef::OnFrameResize);
            PHelper().CreateFloat	(items,PrepareKey	   	(pref,"Sprite\\Frame\\Size V (0..1)"),	&m_Frame.m_fTexSize.y, EPS_S,1.f,0.001f,8);
	        // animate
            P=PHelper().CreateFlag32(items,PrepareKey		(pref,"Sprite\\Animated"),				&m_Flags, dfAnimated);
            P->OnChangeEvent.bind	(this,&PS::CPEDef::OnFlagChange);
            if (m_Flags.is(dfAnimated)){
                PHelper().CreateFlag32(items,PrepareKey	(pref,"Sprite\\Animated\\Random Playback"),	&m_Flags, dfRandomPlayback);
		    	PHelper().CreateFloat(items,PrepareKey		(pref,"Sprite\\Animated\\Speed"),		&m_Frame.m_fSpeed, 0.f,1000.f);
            }
        }
    }
	// align to path
    P=PHelper().CreateFlag32(items,PrepareKey	(pref,"Movement\\Align To Path"), 					&m_Flags, dfAlignToPath);
    P->OnChangeEvent.bind	(this,&PS::CPEDef::OnFlagChange);
    if (m_Flags.is(dfAlignToPath)){
	    PHelper().CreateFlag32(items,PrepareKey	(pref,"Movement\\Align To Path\\Face Align"), 		&m_Flags, dfFaceAlign);
	    PHelper().CreateFlag32(items,PrepareKey	(pref,"Movement\\Align To Path\\Default World Align"), &m_Flags, dfWorldAlign);
    	PHelper().CreateAngle3(items,PrepareKey	(pref,"Movement\\Align To Path\\Default Rotate"),	&m_APDefaultRotation);
    }
	// velocity scale                                                           
    P=PHelper().CreateFlag32(items,PrepareKey	(pref,"Movement\\Velocity Scale"),					&m_Flags, dfVelocityScale);
    P->OnChangeEvent.bind	(this,&PS::CPEDef::OnFlagChange);
    if (m_Flags.is(dfVelocityScale))
    	PHelper().CreateVector(items,PrepareKey	(pref,"Movement\\Velocity Scale\\Value"),			&m_VelocityScale, -1000.f, 1000.f);
	// collision
    P=PHelper().CreateFlag32(items,PrepareKey	(pref,"Movement\\Collision"),						&m_Flags, dfCollision);
    P->OnChangeEvent.bind	(this,&PS::CPEDef::OnFlagChange);
    FloatValue*	V 			= 0;
    if (m_Flags.is(dfCollision)){
    	PHelper().CreateFlag32(items,PrepareKey(pref,"Movement\\Collision\\Collide With Dynamic"),	&m_Flags, dfCollisionDyn);
    	PHelper().CreateFlag32(items,PrepareKey(pref,"Movement\\Collision\\Destroy On Contact"),	&m_Flags, dfCollisionDel);
	    V=PHelper().CreateFloat	(items,PrepareKey	(pref,"Movement\\Collision\\Friction"),			&m_fCollideOneMinusFriction,0.f, 1.f);
        V->OnBeforeEditEvent.bind	(this,&PS::CPEDef::CollisionFrictionOnBeforeEdit);
        V->OnAfterEditEvent.bind	(this,&PS::CPEDef::CollisionFrictionOnAfterEdit);
        V->Owner()->OnDrawTextEvent.bind(this,&PS::CPEDef::CollisionFrictionOnDraw);
	    PHelper().CreateFloat		(items,PrepareKey	(pref,"Movement\\Collision\\Resilence"), 	&m_fCollideResilience, 		0.f, 1.f);
	    V=PHelper().CreateFloat		(items,PrepareKey	(pref,"Movement\\Collision\\Cutoff"),	 	&m_fCollideSqrCutoff, 		0.f, P_MAXFLOAT);
        V->OnBeforeEditEvent.bind	(this,&PS::CPEDef::CollisionCutoffOnBeforeEdit);
        V->OnAfterEditEvent.bind	(this,&PS::CPEDef::CollisionCutoffOnAfterEdit);
        V->Owner()->OnDrawTextEvent.bind(this,&PS::CPEDef::CollisionCutoffOnDraw);
    }
    // actions
	B=::PHelper().CreateButton(items,PrepareKey(pref,"Actions\\Edit"),"Append",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent.bind	(this,&PS::CPEDef::OnActionsClick);
	for (EPAVecIt s_it=m_EActionList.begin(); s_it!=m_EActionList.end(); s_it++){
    	u32 clr				= (*s_it)->flags.is(EParticleAction::flEnabled)?clBlack:clSilver;
    	shared_str a_pref		= PrepareKey(pref,"Actions",AnsiString().sprintf("%s (%s)",*(*s_it)->actionType,*(*s_it)->actionName).c_str());
        ButtonValue* B		= PHelper().CreateButton(items,a_pref,"Up,Down,Remove",ButtonValue::flFirstOnly); B->tag = (s_it-m_EActionList.begin());
        B->Owner()->prop_color	= clr;
        B->OnBtnClickEvent.bind	(this,&PS::CPEDef::OnActionEditClick);
        RTextValue* R;
		R=PHelper().CreateRText	(items,PrepareKey(a_pref.c_str(),"Name"),&(*s_it)->actionName);
        R->OnAfterEditEvent.bind(this,&PS::CPEDef::OnAfterActionNameEdit);
        R->Owner()->prop_color	= clr;
    	(*s_it)->FillProp	(items,a_pref.c_str(),clr);
    }
}
void PS::CPEDef::Compile()
{
	m_Actions.clear	();
    m_Actions.w_u32	(m_EActionList.size());
    int cnt			= 0;
    for (EPAVecIt it=m_EActionList.begin(); it!=m_EActionList.end(); it++){
		if ((*it)->flags.is(EParticleAction::flEnabled)){
    	    (*it)->Compile(m_Actions);
            cnt++;
        }
    }
    m_Actions.seek	(0);
    m_Actions.w_u32 (cnt);
}
bool PS::CPEDef::Validate(bool bMsg)
{
    bool have_kill_old  = false;
    for (EPAVecIt it=m_EActionList.begin(); it!=m_EActionList.end(); it++)
    	if ((*it)->type==PAPI::PAKillOldID){ have_kill_old=true; break; }
    if (bMsg&&(false==have_kill_old))
    	Msg			("!.'%s': dosn't contains 'Kill Old' action. Owner - Modif: '%s' - '%s'",*m_Name,*m_OwnerName,*m_ModifName);
    return have_kill_old;
}
#endif

