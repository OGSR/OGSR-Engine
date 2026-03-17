#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "xrSheduler.h"
#include "xr_object_list.h"

#include "xr_object.h"
#include "NET_Server_Trash/net_utils.h"

#include "CustomHUD.h"

class fClassEQ
{
    CLASS_ID cls;

public:
    fClassEQ(CLASS_ID C) : cls(C) {};
    IC bool operator()(CObject* O) { return cls == O->CLS_ID; }
};

constexpr bool debug_destroy{};

CObjectList::~CObjectList()
{
    R_ASSERT(objects_active.empty());
    R_ASSERT(objects_sleeping.empty());
    R_ASSERT(destroy_queue.empty());
}

CObject* CObjectList::FindObjectByName(const shared_str& name)
{
    for (auto I = objects_active.begin(); I != objects_active.end(); I++)
        if ((*I)->cName().equal(name))
            return (*I);
    for (auto I = objects_sleeping.begin(); I != objects_sleeping.end(); I++)
        if ((*I)->cName().equal(name))
            return (*I);
    return nullptr;
}
CObject* CObjectList::FindObjectByName(LPCSTR name) { return FindObjectByName(shared_str(name)); }

CObject* CObjectList::FindObjectByCLS_ID(CLASS_ID cls)
{
    {
        auto O = std::find_if(objects_active.begin(), objects_active.end(), fClassEQ(cls));
        if (O != objects_active.end())
            return *O;
    }
    {
        auto O = std::find_if(objects_sleeping.begin(), objects_sleeping.end(), fClassEQ(cls));
        if (O != objects_sleeping.end())
            return *O;
    }

    return nullptr;
}

void CObjectList::o_remove(Objects& v, CObject* O)
{
    auto _i = std::find(v.begin(), v.end(), O);
    if (_i != v.end())
        v.erase(_i);
}

void CObjectList::o_activate(CObject* O)
{
    VERIFY(O && O->processing_enabled());
    o_remove(objects_sleeping, O);
    objects_active.push_back(O);
    O->MakeMeCrow();
}
void CObjectList::o_sleep(CObject* O)
{
    VERIFY(O && !O->processing_enabled());
    o_remove(objects_active, O);
    objects_sleeping.push_back(O);
    O->MakeMeCrow();
}

void CObjectList::SingleUpdate(CObject* O)
{
    if (O->getDestroy() && (Device.dwFrame != O->dwFrame_UpdateCL))
    {
        // Msg("- !!!processing_enabled ->destroy_queue.push_back %s[%d] frame [%d]", O->cName().c_str(), O->ID(), Device.dwFrame);
        //  if (std::find(destroy_queue.begin(), destroy_queue.end(), O) == destroy_queue.end())
        //      destroy_queue.push_back(O);
        return;
    }
    else if (O->H_Parent() && (O->H_Parent()->getDestroy() || O->H_Root()->getDestroy()))
    {
        // Msg("! ERROR: incorrect destroy sequence for object[%d:%s], section[%s], parent[%d:%s]", O->ID(), *O->cName(), *O->cNameSect(), O->H_Parent()->ID(),
        // *O->H_Parent()->cName());
        //  if (std::find(destroy_queue.begin(), destroy_queue.end(), O) == destroy_queue.end())
        //      destroy_queue.push_back(O);
        return;
    }

    TracyMessageL(O->cNameSect().c_str());

    O->IAmNotACrowAnyMore(); // важно !! надо делать всегда даже если не нужно вызывать  UpdateCL
    O->dwFrame_AsCrow = (u32)-1;

    if (Device.dwFrame == O->dwFrame_UpdateCL)
    {
#ifdef DEBUG
        if (O->getDestroy())
            Msg("- !!!processing_enabled ->destroy_queue.push_back %s[%d] frame [%d]", O->cName().c_str(), O->ID(), Device.dwFrame);
#endif // #ifdef DEBUG

        return;
    }

    if (!O->processing_enabled())
    {
#ifdef DEBUG
        if (O->getDestroy())
            Msg("- !!!processing_enabled ->destroy_queue.push_back %s[%d] frame [%d]", O->cName().c_str(), O->ID(), Device.dwFrame);
#endif // #ifdef DEBUG

        return;
    }

    if (O->H_Parent())
        SingleUpdate(O->H_Parent());

    Device.Statistic->UpdateClient_updated++;
    O->dwFrame_UpdateCL = Device.dwFrame;

    O->UpdateCL();

    VERIFY3(O->dbg_update_cl == Device.dwFrame, "Broken sequence of calls to 'UpdateCL'", *O->cName());

    if (O->getDestroy() && (Device.dwFrame != O->dwFrame_UpdateCL))
    {
        Msg("- !!!processing_enabled ->destroy_queue.push_back %s[%d] frame [%d]", O->cName().c_str(), O->ID(), Device.dwFrame);
        // if (std::find(destroy_queue.begin(), destroy_queue.end(), O) == destroy_queue.end())
        //     destroy_queue.push_back(O);
        return;
    }
    if (O->H_Parent() && (O->H_Parent()->getDestroy() || O->H_Root()->getDestroy()))
    {
        // Push to destroy-queue if it isn't here already
        Msg("! ERROR: incorrect destroy sequence for object[%d:%s], section[%s], parent[%d:%s]", O->ID(), *O->cName(), *O->cNameSect(), O->H_Parent()->ID(),
            *O->H_Parent()->cName());
    }

#ifdef DEBUG
    if (O->getDestroy())
        Msg("- !!!processing_enabled ->destroy_queue.push_back %s[%d] frame [%d]", O->cName().c_str(), O->ID(), Device.dwFrame);
#endif // #ifdef DEBUG
}

void CObjectList::Update(bool bForce)
{
    // merge parallel crows
    if (!m_parallel_crows.empty())
    {
        // Parallel crows
        for (auto* O : m_parallel_crows)
        {
            m_crows.push_back(O);
        }
        m_parallel_crows.clear();
    }

    if ((!Device.Paused() && Device.fTimeDelta > EPS_S) || bForce)
    {
        ZoneScoped;

        xr_vector<CObject*> primary_crows;

        {
            primary_crows.insert(primary_crows.end(), m_crows.cbegin(), m_crows.cend());
            m_crows.clear();
        }

        Device.Statistic->UpdateClient.Begin();
        Device.Statistic->UpdateClient_updated = 0;
        Device.Statistic->UpdateClient_crows = primary_crows.size();
        Device.Statistic->UpdateClient_active = objects_active.size();
        Device.Statistic->UpdateClient_total = objects_active.size() + objects_sleeping.size();

        for (auto* obj_dup : bForce ? objects_active : primary_crows) // bForce только при удалении уровня
        {
            SingleUpdate(obj_dup);
        }

        Device.Statistic->UpdateClient.End();
    }

    // Destroy
    ProcessDestroyQueue();
}

void CObjectList::ProcessDestroyQueue()
{
    // Destroy
    if (!destroy_queue.empty())
    {
        // Info
        for (auto oit = objects_active.begin(); oit != objects_active.end(); oit++)
            for (int it = destroy_queue.size() - 1; it >= 0; it--)
            {
                (*oit)->net_Relcase(destroy_queue[it]);
            }
        for (auto oit = objects_sleeping.begin(); oit != objects_sleeping.end(); oit++)
            for (int it = destroy_queue.size() - 1; it >= 0; it--)
                (*oit)->net_Relcase(destroy_queue[it]);

        for (int it = destroy_queue.size() - 1; it >= 0; it--)
            Sound->object_relcase(destroy_queue[it]);

        auto It = m_relcase_callbacks.begin();
        auto Ite = m_relcase_callbacks.end();
        for (; It != Ite; ++It)
        {
            VERIFY(*(*It).m_ID == (It - m_relcase_callbacks.begin()));
            auto dIt = destroy_queue.begin();
            auto dIte = destroy_queue.end();
            for (; dIt != dIte; ++dIt)
            {
                (*It).m_Callback(*dIt);
                g_hud->net_Relcase(*dIt);
            }
        }

        // Destroy
        for (int it = destroy_queue.size() - 1; it >= 0; it--)
        {
            CObject* O = destroy_queue[it];

            O->setDestroy(TRUE); //https://github.com/OGSR/OGSR_Engine_private/commit/406a9d3766bbb726d39c9a73f4038634261d65a4

            if (debug_destroy)
                Msg("Destroying object[%x][%x] [%d][%s] frame[%d]", smart_cast<void*>(O), O, O->ID(), *O->cName(), Device.dwFrame);

            O->net_Destroy();
            Destroy(O);
        }
        destroy_queue.clear();
    }
}

void CObjectList::net_Register(CObject* O)
{
    R_ASSERT(O);
    R_ASSERT(O->ID() < 0xffff);

    map_NETID[O->ID()] = O;
}

void CObjectList::net_Unregister(CObject* O)
{
    if (O->ID() < 0xffff) // demo_spectator can have 0xffff
        map_NETID[O->ID()] = nullptr;
}

constexpr bool g_Dump_Export_Obj{};

// Simp: net_exported_objects добавлено в OGSR, при адаптации ЗП учесть это!
u32 CObjectList::net_Export(NET_Packet* _Packet, u32 start, u32 max_object_size, std::vector<CObject*>& net_exported_objects)
{
    if (g_Dump_Export_Obj)
        Msg("---- net_export --- ");

    NET_Packet& Packet = *_Packet;
    u32 position;
    for (; start < objects_active.size() + objects_sleeping.size(); start++)
    {
        CObject* P = (start < objects_active.size()) ? objects_active[start] : objects_sleeping[start - objects_active.size()];
        if (P->net_Relevant() && !P->getDestroy())
        {
            Packet.w_u16(u16(P->ID()));
            Packet.w_chunk_open8(position);
            // Msg						("cl_export: %d '%s'",P->ID(),*P->cName());
            P->net_Export(Packet);

            net_exported_objects.push_back(P);

#ifdef DEBUG
            u32 size = u32(Packet.w_tell() - position) - sizeof(u8);
            if (size >= 256)
            {
                Debug.fatal(DEBUG_INFO, "Object [%s][%d] exceed network-data limit\n size=%d, Pend=%d, Pstart=%d", *P->cName(), P->ID(), size, Packet.w_tell(), position);
            }
#endif
            if (g_Dump_Export_Obj)
            {
                u32 size = u32(Packet.w_tell() - position) - sizeof(u8);
                Msg("* %s : %d", *(P->cNameSect()), size);
            }
            Packet.w_chunk_close8(position);
            //			if (0==(--count))
            //				break;
            if (max_object_size >= (NET_PacketSizeLimit - Packet.w_tell()))
                break;
        }
    }
    if (g_Dump_Export_Obj)
        Msg("------------------- ");
    return start + 1;
}

constexpr bool g_Dump_Import_Obj{};

void CObjectList::net_Import(NET_Packet* Packet)
{
    if (g_Dump_Import_Obj)
        Msg("---- net_import --- ");

    while (!Packet->r_eof())
    {
        u16 ID;
        Packet->r_u16(ID);
        u8 size;
        Packet->r_u8(size);
        CObject* P = net_Find(ID);
        if (P)
        {
            u32 rsize = Packet->r_tell();

            P->net_Import(*Packet);

            if (g_Dump_Import_Obj)
                Msg("* %s : %d - %d", *(P->cNameSect()), size, Packet->r_tell() - rsize);
        }
        else
            Packet->r_advance(size);
    }

    if (g_Dump_Import_Obj)
        Msg("------------------- ");
}

void CObjectList::o_crow(CObject* O)
{
    if (O->dwFrame_AsCrow == Device.dwFrame)
    {
        // already processed
        return;
    }

    O->dwFrame_AsCrow = Device.dwFrame;

    if (Device.OnMainThread())
    {
        VERIFY(std::find(m_crows.begin(), m_crows.end(), O) == m_crows.end());
        m_crows.push_back(O);
    }
    else
    {
        m_parallel_crows.push_back(O);
    }
}

void CObjectList::Load() { R_ASSERT(objects_active.empty() && destroy_queue.empty() && objects_sleeping.empty()); }

void CObjectList::Unload()
{
    if (!objects_sleeping.empty() || !objects_active.empty())
        Msg("! objects-leaked: %d", objects_sleeping.size() + objects_active.size());

    // Destroy objects
    while (!objects_sleeping.empty())
    {
        CObject* O = objects_sleeping.back();
        Msg("! [%x] s[%4d]-[%s]-[%s]", O, O->ID(), *O->cNameSect(), *O->cName());
        O->setDestroy(TRUE);

        if (debug_destroy)
            Msg("Destroying object [%d][%s]", O->ID(), *O->cName());

        O->net_Destroy();
        Destroy(O);
    }
    while (!objects_active.empty())
    {
        CObject* O = objects_active.back();
        Msg("! [%x] a[%4d]-[%s]-[%s]", O, O->ID(), *O->cNameSect(), *O->cName());
        O->setDestroy(TRUE);

        if (debug_destroy)
            Msg("Destroying object [%d][%s]", O->ID(), *O->cName());

        O->net_Destroy();
        Destroy(O);
    }
}

CObject* CObjectList::Create(LPCSTR name)
{
    CObject* O = g_pGamePersistent->ObjectPool.create(name);
    //	Msg("CObjectList::Create [%x]%s", O, name);
    objects_sleeping.push_back(O);
    return O;
}

void CObjectList::Destroy(CObject* O)
{
    if (0 == O)
        return;
    net_Unregister(O);

    {
        std::erase(m_crows, O);
    }

    // active/inactive
    auto _i = std::find(objects_active.begin(), objects_active.end(), O);
    if (_i != objects_active.end())
    {
        objects_active.erase(_i);
        VERIFY(std::find(objects_active.begin(), objects_active.end(), O) == objects_active.end());
        VERIFY(std::find(objects_sleeping.begin(), objects_sleeping.end(), O) == objects_sleeping.end());
    }
    else
    {
        auto _ii = std::find(objects_sleeping.begin(), objects_sleeping.end(), O);
        if (_ii != objects_sleeping.end())
        {
            objects_sleeping.erase(_ii);
            VERIFY(std::find(objects_sleeping.begin(), objects_sleeping.end(), O) == objects_sleeping.end());
        }
        else
            FATAL("! Unregistered object being destroyed");
    }

    g_pGamePersistent->ObjectPool.destroy(O);
}

void CObjectList::relcase_register(RELCASE_CALLBACK cb, int* ID)
{
#ifdef DEBUG
    auto It = std::find(m_relcase_callbacks.begin(), m_relcase_callbacks.end(), cb);
    VERIFY(It == m_relcase_callbacks.end());
#endif
    *ID = m_relcase_callbacks.size();
    m_relcase_callbacks.emplace_back(ID, cb);
}

void CObjectList::relcase_unregister(int* ID)
{
    VERIFY(m_relcase_callbacks[*ID].m_ID == ID);
    m_relcase_callbacks[*ID] = m_relcase_callbacks.back();
    *m_relcase_callbacks.back().m_ID = *ID;
    m_relcase_callbacks.pop_back();
}

void CObjectList::dump_list(Objects& v, LPCSTR reason)
{
    auto it = v.begin();
    auto it_e = v.end();
    Msg("----------------dump_list [%s]", reason);
    for (; it != it_e; ++it)
        Msg("%x - name [%s] ID[%d] parent[%s] getDestroy()=[%s]", (*it), (*it)->cName().c_str(), (*it)->ID(), ((*it)->H_Parent()) ? (*it)->H_Parent()->cName().c_str() : "",
            ((*it)->getDestroy()) ? "yes" : "no");
}

bool CObjectList::dump_all_objects()
{
    dump_list(destroy_queue, "destroy_queue");
    dump_list(objects_active, "objects_active");
    dump_list(objects_sleeping, "objects_sleeping");
    dump_list(m_crows, "main_crows");
    return false;
}

void CObjectList::register_object_to_destroy(CObject* object_to_destroy)
{
    VERIFY(!registered_object_to_destroy(object_to_destroy));
    //	Msg("CObjectList::register_object_to_destroy [%x]", object_to_destroy);
    destroy_queue.push_back(object_to_destroy);

    auto it = objects_active.begin();
    auto it_e = objects_active.end();
    for (; it != it_e; ++it)
    {
        CObject* O = *it;
        if (!O->getDestroy() && O->H_Parent() == object_to_destroy)
        {
            Msg("setDestroy called, but not-destroyed child found parent[%d] child[%d]", object_to_destroy->ID(), O->ID(), Device.dwFrame);
            O->setDestroy(TRUE);
        }
    }

    it = objects_sleeping.begin();
    it_e = objects_sleeping.end();
    for (; it != it_e; ++it)
    {
        CObject* O = *it;
        if (!O->getDestroy() && O->H_Parent() == object_to_destroy)
        {
            Msg("setDestroy called, but not-destroyed child found parent[%d] child[%d]", object_to_destroy->ID(), O->ID(), Device.dwFrame);
            O->setDestroy(TRUE);
        }
    }
}

#ifdef DEBUG
bool CObjectList::registered_object_to_destroy(const CObject* object_to_destroy) const
{
    return (std::find(destroy_queue.begin(), destroy_queue.end(), object_to_destroy) != destroy_queue.end());
}
#endif // DEBUG
