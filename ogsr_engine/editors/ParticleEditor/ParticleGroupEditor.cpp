//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "../xrEProps/PropertiesListHelper.h"
#include "ui_main.h"
#include "ui_particletools.h"

BOOL PS::CPGDef::SEffect::Equal(const SEffect& src)
{
	if (!m_Flags.equal(src.m_Flags))	return FALSE;
    if (!m_EffectName.equal(src.m_EffectName)) return FALSE;
	if (!fsimilar(m_Time0,src.m_Time0))	return FALSE;
	if (!fsimilar(m_Time1,src.m_Time1))	return FALSE;
	return TRUE;
}

BOOL PS::CPGDef::Equal(const CPGDef* pg)
{
	if (!m_Flags.equal(pg->m_Flags))				return FALSE;
	if (!fsimilar(m_fTimeLimit,pg->m_fTimeLimit))	return FALSE;
    if (m_Effects.size()!=pg->m_Effects.size())		return FALSE;
    EffectIt s_it=m_Effects.begin(); 
    for (EffectIt d_it=m_Effects.begin(); d_it!=m_Effects.end(); s_it++,d_it++)
    	if (!(*s_it)->Equal(**d_it)) return FALSE;
	return TRUE;
}

void  PS::CPGDef::OnEffectsEditClick(ButtonValue* B, bool& bDataModified, bool& bSafe)
{
    switch (B->btn_num){
    case 0:
        m_Effects.push_back(xr_new<SEffect>());
        m_Effects.back()->m_Flags.set(CPGDef::SEffect::flEnabled,FALSE);
        ExecCommand		(COMMAND_UPDATE_PROPERTIES);
        OnParamsChange	(B);
        bDataModified	= true;
    break;
    }
}

void  PS::CPGDef::OnEffectTypeChange(PropValue* sender)
{
    ExecCommand			(COMMAND_UPDATE_PROPERTIES);
    OnParamsChange		(sender);
}

void  PS::CPGDef::OnControlClick(ButtonValue* B, bool& bDataModified, bool& bSafe)
{
    switch (B->btn_num){
    case 0: 			PTools->PlayCurrent();		break;
    case 1: 			PTools->StopCurrent(false);	break;
    case 2: 			PTools->StopCurrent(true);	break;
    }
    bDataModified		= false;
}

void  PS::CPGDef::OnEffectEditClick(ButtonValue* B, bool& bDataModified, bool& bSafe)
{
    switch (B->btn_num){
    case 0:		    	
    	PTools->PlayCurrent	(B->tag);    
		bDataModified	= false;
    break;
    case 1:{
    	CPGDef::SEffect* eff = *(m_Effects.begin()+B->tag); VERIFY(eff);
		PTools->SelectEffect(*eff->m_EffectName);
		bDataModified	= false;
        bSafe			= true;
    }break;
    case 2:        
        if (ELog.DlgMsg(mtConfirmation, TMsgDlgButtons() << mbYes << mbNo,"Remove effect?") == mrYes){
        	SEffect* eff	= *(m_Effects.begin()+B->tag);
        	xr_delete		(eff);
            m_Effects.erase	(m_Effects.begin()+B->tag);
            ExecCommand		(COMMAND_UPDATE_PROPERTIES);
            OnParamsChange	(B);
            bDataModified	= true;
        }else{
			bDataModified	= false;
        }
    break;
    }
}

void  PS::CPGDef::OnParamsChange(PropValue* sender)
{
	PTools->SetCurrentPG	(0);
	PTools->SetCurrentPG	(this);
}

void PS::CPGDef::FillProp(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner)
{                                   
	PHelper().CreateCaption	(items,PrepareKey(pref,"Version\\Owner Name"),*m_OwnerName);
	PHelper().CreateCaption	(items,PrepareKey(pref,"Version\\Modif Name"),*m_ModifName);
	PHelper().CreateCaption	(items,PrepareKey(pref,"Version\\Creation Time"),Trim(AnsiString(ctime(&m_CreateTime))).c_str());
	PHelper().CreateCaption	(items,PrepareKey(pref,"Version\\Modified Time"),Trim(AnsiString(ctime(&m_ModifTime))).c_str());
    ButtonValue* B;
	B=PHelper().CreateButton	(items,PrepareKey(pref,"Control"),"Play,Stop,Stop...",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent.bind		(this,&PS::CPGDef::OnControlClick);
    B=PHelper().CreateButton	(items,PrepareKey(pref,"Edit"),"Append Effect",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent.bind		(this,&PS::CPGDef::OnEffectsEditClick);
    PropValue* V;
	PHelper().CreateName		(items,PrepareKey(pref,"Name"),&m_Name,owner);
    V=PHelper().CreateFloat		(items,PrepareKey(pref,"Time Limit (s)"),	&m_fTimeLimit,	-1.f,1000.f);
    V->OnChangeEvent.bind		(this,&PS::CPGDef::OnParamsChange);
    for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
    	u32 clr					= (*it)->m_Flags.is(CPGDef::SEffect::flEnabled)?clBlack:clSilver;
        AnsiString nm 			= AnsiString("Effect #")+(it-m_Effects.begin()+1);
        B=PHelper().CreateButton(items,PrepareKey(pref,nm.c_str()),"Preview,Select,Remove",ButtonValue::flFirstOnly); B->tag = it-m_Effects.begin();
        B->OnBtnClickEvent.bind	(this,&PS::CPGDef::OnEffectEditClick);
        B->Owner()->prop_color	= clr;
        V=PHelper().CreateChoose(items,PrepareKey(pref,nm.c_str(),"Name"),&(*it)->m_EffectName,smPE);
        V->OnChangeEvent.bind	(this,&PS::CPGDef::OnParamsChange);
        V->Owner()->prop_color	= clr;
        V=PHelper().CreateFloat	(items,PrepareKey(pref,nm.c_str(),"Start Time (s)"),&(*it)->m_Time0,		0.f,1000.f);
        V->OnChangeEvent.bind	(this,&PS::CPGDef::OnParamsChange);
        V->Owner()->prop_color	= clr;
        V=PHelper().CreateFloat	(items,PrepareKey(pref,nm.c_str(),"End Time (s)"),	&(*it)->m_Time1,		0.f,1000.f);
        V->OnChangeEvent.bind	(this,&PS::CPGDef::OnParamsChange);
        V->Owner()->prop_color	= clr;
        V=PHelper().CreateFlag32(items,PrepareKey(pref,nm.c_str(),"Deferred Stop"),&(*it)->m_Flags,	SEffect::flDefferedStop);
        V->OnChangeEvent.bind	(this,&PS::CPGDef::OnParamsChange);
        V->Owner()->prop_color	= clr;
        V=PHelper().CreateFlag32(items,PrepareKey(pref,nm.c_str(),"Enabled"),									&(*it)->m_Flags, 	SEffect::flEnabled);
        V->OnChangeEvent.bind	(this,&PS::CPGDef::OnParamsChange);
        V->Owner()->prop_color	= clr;
        V=PHelper().CreateFlag32(items,PrepareKey(pref,nm.c_str(),"Children\\On Birth"),						&(*it)->m_Flags,	SEffect::flOnBirthChild);
        V->OnChangeEvent.bind	(this,&PS::CPGDef::OnParamsChange);
        V->Owner()->prop_color	= clr;
        if ((*it)->m_Flags.is(SEffect::flOnBirthChild)){
	        V=PHelper().CreateChoose(items,PrepareKey(pref,nm.c_str(),"Children\\On Birth\\Effect Name"),			&(*it)->m_OnBirthChildName,smPE);
    	    V->OnChangeEvent.bind	(this,&PS::CPGDef::OnParamsChange);
	        V->Owner()->prop_color	= clr;
        }
        V=PHelper().CreateFlag32(items,PrepareKey(pref,nm.c_str(),"Children\\On Play"),						&(*it)->m_Flags,	SEffect::flOnPlayChild);
        V->OnChangeEvent.bind		(this,&PS::CPGDef::OnParamsChange);
        V->Owner()->prop_color		= clr;
        if ((*it)->m_Flags.is(SEffect::flOnPlayChild)){
	        V=PHelper().CreateChoose	(items,PrepareKey(pref,nm.c_str(),"Children\\On Play\\Effect Name"),			&(*it)->m_OnPlayChildName,smPE);
    	    V->OnChangeEvent.bind	(this,&PS::CPGDef::OnParamsChange);
	        V->Owner()->prop_color	= clr;
            V=PHelper().CreateFlag32(items,PrepareKey(pref,nm.c_str(),"Children\\On Play\\Play After Stop"),		&(*it)->m_Flags,	SEffect::flOnPlayChildRewind);
            V->OnChangeEvent.bind	(this,&PS::CPGDef::OnParamsChange);
            V->Owner()->prop_color	= clr;
        }
        V=PHelper().CreateFlag32(items,PrepareKey(pref,nm.c_str(),"Children\\On Dead"),						&(*it)->m_Flags,	SEffect::flOnDeadChild);
        V->OnChangeEvent.bind		(this,&PS::CPGDef::OnParamsChange);
        V->Owner()->prop_color	= clr;
        if ((*it)->m_Flags.is(SEffect::flOnDeadChild)){
	        V=PHelper().CreateChoose	(items,PrepareKey(pref,nm.c_str(),"Children\\On Dead\\Effect Name"),			&(*it)->m_OnDeadChildName,smPE);
    	    V->OnChangeEvent.bind	(this,&PS::CPGDef::OnParamsChange);    
	        V->Owner()->prop_color	= clr;
        }
    }
}

 