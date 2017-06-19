///////////////////////////////////
// class CUIOptionsManager
///////////////////////////////////

#include "StdAfx.h"
#include "UIOptionsManager.h"
#include "UIOptionsItem.h"
#include "../../xr_ioconsole.h"

CUIOptionsManager::CUIOptionsManager(){
	m_b_vid_restart = false;
	m_b_vid_restart = false;
}

void CUIOptionsManager::RegisterItem(CUIOptionsItem* item, const char* group){
	groups_it it = m_groups.find(group);

	if (m_groups.end() != it)
	{
		(*it).second.push_back(item);
	}
	else
	{
		group_name gr_name = group;
		items_list list;

		list.push_back(item);
		m_groups.insert(mk_pair(gr_name, list));
	}
}

void CUIOptionsManager::UnRegisterGroup(const char* group){
	groups_it it = m_groups.find(group);

	if (it != m_groups.end())
		m_groups.erase(it);
}
void CUIOptionsManager::UnRegisterItem(CUIOptionsItem* item){
	groups_it it;
	for (it = m_groups.begin(); it!= m_groups.end(); it++){
		for (u32 i = 0; i < (*it).second.size(); i++)
			if ((*it).second[i] == item){
				(*it).second.erase((*it).second.begin() + i); return;}
	}
}

void CUIOptionsManager::SendMessage2Group(const char* group, const char* message){
	groups_it it = m_groups.find(group);

	R_ASSERT2(m_groups.end() != it, "invalid group name");

	for (u32 i = 0; i < (*it).second.size(); i++)
		(*it).second[i]->OnMessage(message);
}

void CUIOptionsManager::SeveBackupValues(const char* group)
{
	groups_it it = m_groups.find(group);

	R_ASSERT3(m_groups.end() != it, "invalid group name",group);

	for (u32 i = 0; i < (*it).second.size(); i++){
		(*it).second[i]->SeveBackUpValue();
	}
}

void CUIOptionsManager::SetCurrentValues(const char* group){
	groups_it it = m_groups.find(group);

	R_ASSERT3(m_groups.end() != it, "invalid group name",group);

	for (u32 i = 0; i < (*it).second.size(); i++){
		(*it).second[i]->SetCurrentValue();
//.		(*it).second[i]->SeveBackUpValue();
	}
}

void CUIOptionsManager::SaveValues(const char* group){
	groups_it it = m_groups.find(group);

	R_ASSERT3(m_groups.end() != it, "invalid group name",group);

	for (u32 i = 0; i < (*it).second.size(); i++){
		if ((*it).second[i]->IsChanged())
            (*it).second[i]->SaveValue();
	}
}

bool CUIOptionsManager::IsGroupChanged(const char* group){
	groups_it it = m_groups.find(group);	
	R_ASSERT2(m_groups.end() != it, "invalid group name");

	for (u32 i = 0; i < (*it).second.size(); i++)
	{
		if ((*it).second[i]->IsChanged())
			return true;
	}

	return false;
}

void CUIOptionsManager::UndoGroup(const char* group){
	groups_it it = m_groups.find(group);	
	R_ASSERT2(m_groups.end() != it, "invalid group name");

	for (u32 i = 0; i < (*it).second.size(); i++){
		if ((*it).second[i]->IsChanged())
            (*it).second[i]->Undo();
	}
}

void CUIOptionsManager::OptionsPostAccept(){
	if (m_b_vid_restart)
		Console->Execute("vid_restart");
	if (m_b_snd_restart)
		Console->Execute("snd_restart");

	m_b_vid_restart = false;
	m_b_snd_restart = false;
}

void CUIOptionsManager::DoVidRestart(){
	m_b_vid_restart = true;
}

void CUIOptionsManager::DoSndRestart(){
    m_b_snd_restart = true;
}




