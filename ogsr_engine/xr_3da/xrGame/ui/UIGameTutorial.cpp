#include "pch_script.h"
#include "UIGameTutorial.h"
#include "UIWindow.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "../object_broker.h"
#include "../../xr_input.h"
#include "../xr_level_controller.h"
#include "../script_engine.h"
#include "../ai_space.h"

void CUISequenceItem::Load(CUIXml* xml, int idx)
{
	XML_NODE* _stored_root			= xml->GetLocalRoot();
	xml->SetLocalRoot				(xml->NavigateToNode("item",idx));
	int disabled_cnt				= xml->GetNodesNum	(xml->GetLocalRoot(), "disabled_key");
	for(int i=0; i<disabled_cnt;++i){
		LPCSTR str					= xml->Read			("disabled_key", i, NULL);
		m_disabled_actions.push_back( action_name_to_id(str) );
	};

	LPCSTR		str;
	bool		functor_exists;
	int			j;
	int			f_num				= xml->GetNodesNum(xml->GetLocalRoot(),"function_on_start");
	m_start_lua_functions.resize	(f_num);
	for(j=0; j<f_num; ++j){
		str							= xml->Read(xml->GetLocalRoot(), "function_on_start", j, NULL);
		functor_exists				= ai().script_engine().functor(str ,m_start_lua_functions[j]);
		THROW3						(functor_exists, "Cannot find script function described in tutorial item ", str);
	}
	
	f_num							= xml->GetNodesNum(xml->GetLocalRoot(),"function_on_stop");
	m_stop_lua_functions.resize	(f_num);
	for(j=0; j<f_num; ++j){
		str							= xml->Read(xml->GetLocalRoot(), "function_on_stop", j, NULL);
		functor_exists				= ai().script_engine().functor(str ,m_stop_lua_functions[j]);
		THROW3						(functor_exists, "Cannot find script function described in tutorial item ", str);
	}

	xml->SetLocalRoot				(_stored_root);
}

bool CUISequenceItem::AllowKey(int dik)
{
	xr_vector<int>::iterator it = std::find(m_disabled_actions.begin(),m_disabled_actions.end(),get_binded_action(dik));
	if(it==m_disabled_actions.end())
		return true;
	else
		return false;
}

void CallFunctions	(xr_vector<luabind::functor<void> >& v)
{
	xr_vector<luabind::functor<void> >::iterator it	= v.begin();
	for(;it!=v.end();++it){
		if( (*it).is_valid() ) (*it)();
	}
}

void CUISequenceItem::Start()
{
	CallFunctions(m_start_lua_functions);
}

bool CUISequenceItem::Stop(bool bForce)
{
	CallFunctions(m_stop_lua_functions);
	return true;
}

CUISequencer::CUISequencer()
{
	m_bActive					= false;
	m_bPlayEachItem				= false;
}

void CUISequencer::Start(LPCSTR tutor_name)
{
	VERIFY(m_items.size()==0);
	Device.seqFrame.Add			(this, REG_PRIORITY_LOW-10000);
	Device.seqRender.Add		(this, 3);
	
	m_UIWindow					= xr_new<CUIWindow>();

	CUIXml uiXml;
	uiXml.Init					(CONFIG_PATH, UI_PATH, "game_tutorials.xml");
	
	int items_count				= uiXml.GetNodesNum	(tutor_name,0,"item");	VERIFY(items_count>0);
	uiXml.SetLocalRoot			(uiXml.NavigateToNode(tutor_name,0));

	m_bPlayEachItem				= !!uiXml.ReadInt("play_each_item",0,0);

	CUIXmlInit xml_init;
	xml_init.InitWindow			(uiXml, "global_wnd", 0,	m_UIWindow);
//.	xml_init.InitAutoStaticGroup(uiXml, "global_wnd",		m_UIWindow);

	for(int i=0;i<items_count;++i){
		LPCSTR	_tp				= uiXml.ReadAttrib			("item",i,"type","");
		bool bVideo				= 0==_stricmp(_tp,"video");
		CUISequenceItem* pItem	= 0;
		if (bVideo)	pItem		= xr_new<CUISequenceVideoItem>(this);
		else		pItem		= xr_new<CUISequenceSimpleItem>(this);
		m_items.push_back		(pItem);
		pItem->Load				(&uiXml,i);
	}

	CUISequenceItem* pCurrItem	= m_items.front();
	pCurrItem->Start			();
	m_pStoredInputReceiver		= pInput->CurrentIR();
	IR_Capture					();
	m_bActive					= true;
}

void CUISequencer::Destroy()
{
	Device.seqFrame.Remove		(this);
	Device.seqRender.Remove		(this);
	delete_data					(m_items);
	delete_data					(m_UIWindow);
	IR_Release					();
	m_bActive					= false;
	m_pStoredInputReceiver		= NULL;
}

void CUISequencer::Stop()
{
	if(m_items.size()){ 
		if (m_bPlayEachItem){
			Next				();
			return;
		}else{
			CUISequenceItem* pCurrItem	= m_items.front();
			pCurrItem->Stop		(true);
		}
	}
	Destroy			();
}

void CUISequencer::OnFrame()
{  
	if(!m_bActive)				return;

	if(!m_items.size()){
		Stop					();
		return;
	}else{
		CUISequenceItem* pCurrItem	= m_items.front();
		if(!pCurrItem->IsPlaying())	Next();
	}
	
	if(!m_items.size()){
		Stop					();
		return;
	}

	m_items.front()->Update		();
	m_UIWindow->Update			();
}

void CUISequencer::OnRender	()
{
	if (m_UIWindow->IsShown())	m_UIWindow->Draw();
	VERIFY						(m_items.size());
	m_items.front()->OnRender	();
}

void CUISequencer::Next		()
{
	CUISequenceItem* pCurrItem	= m_items.front();
	bool can_stop				= pCurrItem->Stop();
	if	(!can_stop)				return;

	m_items.pop_front			();
	delete_data					(pCurrItem);

	if(m_items.size())
	{
		pCurrItem				= m_items.front();
		pCurrItem->Start		();
	}
}

bool CUISequencer::GrabInput()
{
	if(m_items.size())
		return m_items.front()->GrabInput();
	else
	return false;

}

void CUISequencer::IR_OnMousePress		(int btn)
{
	if(!GrabInput()&&m_pStoredInputReceiver)
		m_pStoredInputReceiver->IR_OnMousePress(btn);
}

void CUISequencer::IR_OnMouseRelease		(int btn)
{
	if(!GrabInput()&&m_pStoredInputReceiver)
		m_pStoredInputReceiver->IR_OnMouseRelease(btn);
}

void CUISequencer::IR_OnMouseHold		(int btn)
{
	if(!GrabInput()&&m_pStoredInputReceiver)
		m_pStoredInputReceiver->IR_OnMouseHold(btn);
}

void CUISequencer::IR_OnMouseMove		(int x, int y)
{
	if(!GrabInput()&&m_pStoredInputReceiver)
		m_pStoredInputReceiver->IR_OnMouseMove(x, y);
}

void CUISequencer::IR_OnMouseStop		(int x, int y)
{
	if(!GrabInput()&&m_pStoredInputReceiver)
		m_pStoredInputReceiver->IR_OnMouseStop(x, y);
}

void CUISequencer::IR_OnKeyboardRelease	(int dik)
{
	if(!GrabInput()&&m_pStoredInputReceiver)
		m_pStoredInputReceiver->IR_OnKeyboardRelease(dik);
}


void CUISequencer::IR_OnKeyboardHold		(int dik)
{
	if(!GrabInput()&&m_pStoredInputReceiver)
		m_pStoredInputReceiver->IR_OnKeyboardHold(dik);
}


void CUISequencer::IR_OnMouseWheel		(int direction)
{
	if(!GrabInput()&&m_pStoredInputReceiver)
		m_pStoredInputReceiver->IR_OnMouseWheel(direction);
}

void CUISequencer::IR_OnKeyboardPress	(int dik)
{
	if(m_items.size())	m_items.front()->OnKeyboardPress			(dik);
	
	bool b = true;
	if(m_items.size()) b &= m_items.front()->AllowKey(dik);

	if(b && is_binded(kQUIT, dik) )
	{
		Stop		();
		return;
	}

	if(b&&!GrabInput()&&m_pStoredInputReceiver)	
		m_pStoredInputReceiver->IR_OnKeyboardPress	(dik);
}

void CUISequencer::IR_OnActivate()
{
	if(!pInput) return;
	int i;
	for (i = 0; i < CInput::COUNT_KB_BUTTONS; i++ )
	{
		if(IR_GetKeyState(i))
		{
			EGameActions action		= get_binded_action(i);
			switch (action){
			case kFWD			:
			case kBACK			:
			case kL_STRAFE		:
			case kR_STRAFE		:
			case kLEFT			:
			case kRIGHT			:
			case kUP			:
			case kDOWN			:
			case kCROUCH		:
			case kACCEL			:
			case kL_LOOKOUT		:
			case kR_LOOKOUT		:	
			case kWPN_FIRE		:
				{
					IR_OnKeyboardPress	(i);
				}break;
			};
		};
	}
}
