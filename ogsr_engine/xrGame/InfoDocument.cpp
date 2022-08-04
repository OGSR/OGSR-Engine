///////////////////////////////////////////////////////////////
// InfoDocument.cpp
// InfoDocument - документ, содержащий сюжетную информацию
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "InfoDocument.h"
#include "PhysicsShell.h"
#include "PDA.h"
#include "inventoryowner.h"
#include "xrserver_objects_alife_items.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "actor.h"

CInfoDocument::CInfoDocument(void) {}

CInfoDocument::~CInfoDocument(void) {}

BOOL CInfoDocument::net_Spawn(CSE_Abstract* DC)
{
    BOOL res = inherited::net_Spawn(DC);

    CSE_Abstract* l_tpAbstract = static_cast<CSE_Abstract*>(DC);
    CSE_ALifeItemDocument* l_tpALifeItemDocument = smart_cast<CSE_ALifeItemDocument*>(l_tpAbstract);
    R_ASSERT(l_tpALifeItemDocument);

    m_Info.clear();
    shared_str m_wDoc = l_tpALifeItemDocument->m_wDoc;
    if (m_wDoc.size())
        m_Info.push_back(m_wDoc.c_str());

    CInifile& ini = l_tpAbstract->spawn_ini();
    if (ini.section_exist("known_info"))
    {
        const auto& sect = ini.r_section("known_info");
        for (const auto& I : sect.Data)
        {
            m_Info.push_back(I.first.c_str());
        }
    }

    if (pSettings->line_exist(l_tpAbstract->name(), "known_info"))
    {
        LPCSTR S = pSettings->r_string(l_tpAbstract->name(), "known_info");
        if (S && S[0])
        {
            string128 info;
            int count = _GetItemCount(S);
            for (int it = 0; it < count; ++it)
            {
                _GetItem(S, it, info);
                m_Info.push_back(info);
            }
        }
    }

    return (res);
}

void CInfoDocument::Load(LPCSTR section) { inherited::Load(section); }

void CInfoDocument::net_Destroy() { inherited::net_Destroy(); }

void CInfoDocument::shedule_Update(u32 dt) { inherited::shedule_Update(dt); }

void CInfoDocument::UpdateCL() { inherited::UpdateCL(); }

void CInfoDocument::OnH_A_Chield()
{
    inherited::OnH_A_Chield();

    //передать информацию содержащуюся в документе
    //объекту, который поднял документ
    // CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(H_Parent());
    CActor* pInvOwner = smart_cast<CActor*>(H_Parent());
    if (!pInvOwner)
        return;

    //создать и отправить пакет о получении новой информации
    for (const auto& it : m_Info)
    {
        NET_Packet P;
        u_EventGen(P, GE_INFO_TRANSFER, H_Parent()->ID());
        P.w_u16(ID()); //отправитель
        P.w_stringZ(it.c_str()); //сообщение
        P.w_u8(1); //добавление сообщения
        u_EventSend(P);
    }
}

void CInfoDocument::OnH_B_Independent(bool just_before_destroy) { inherited::OnH_B_Independent(just_before_destroy); }

void CInfoDocument::renderable_Render() { inherited::renderable_Render(); }

using namespace luabind;
#pragma optimize("s", on)
void CInfoDocument::script_register(lua_State* L) { module(L)[class_<CInfoDocument, CGameObject>("CInfoDocument").def(constructor<>())]; }
