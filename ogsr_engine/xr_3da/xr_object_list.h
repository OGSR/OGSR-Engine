#pragma once

#include <concurrent_vector.h>

// refs
class ENGINE_API CObject;
class NET_Packet;

class ENGINE_API CObjectList
{
private:
    // data
    CObject* map_NETID[0xffff]{};

    using Objects = xr_vector<CObject*>;

private:
    Objects destroy_queue;
    Objects objects_active;
    Objects objects_sleeping;

    Objects m_crows;
    concurrency::concurrent_vector<CObject*> m_parallel_crows;

public:
    using RELCASE_CALLBACK = fastdelegate::FastDelegate<void(CObject*)>;

    struct SRelcasePair
    {
        int* m_ID;
        RELCASE_CALLBACK m_Callback;
        SRelcasePair(int* id, RELCASE_CALLBACK cb) : m_ID(id), m_Callback(cb) {}
        bool operator==(RELCASE_CALLBACK cb) { return m_Callback == cb; }
    };
    typedef xr_vector<SRelcasePair> RELCASE_CALLBACK_VEC;
    RELCASE_CALLBACK_VEC m_relcase_callbacks;

    void relcase_register(RELCASE_CALLBACK, int*);
    void relcase_unregister(int*);

public:
    // methods
    CObjectList() = default;
    ~CObjectList();

    CObject* FindObjectByName(const shared_str& name);
    CObject* FindObjectByName(LPCSTR name);
    CObject* FindObjectByCLS_ID(CLASS_ID cls);

    void Load();
    void Unload();

    CObject* Create(LPCSTR name);
    void Destroy(CObject* O);

    void SingleUpdate(CObject* O);
    void Update(bool bForce);
    void ProcessDestroyQueue();

    void net_Register(CObject* O);
    void net_Unregister(CObject* O);

    u32 net_Export(NET_Packet* P, u32 _start, u32 _count, std::vector<CObject*>& net_exported_objects); // return next start
    void net_Import(NET_Packet* P);

    ICF CObject* net_Find(u16 ID) const
    {
        if (ID == u16(-1))
            return nullptr;

        return map_NETID[ID];
    }

    void o_crow(CObject* O);
    void o_remove(Objects& v, CObject* O);
    void o_activate(CObject* O);
    void o_sleep(CObject* O);
    IC u32 o_count() { return objects_active.size() + objects_sleeping.size(); };
    IC CObject* o_get_by_iterator(u32 _it)
    {
        if (_it < objects_active.size())
            return objects_active[_it];
        else
            return objects_sleeping[_it - objects_active.size()];
    }
    bool dump_all_objects();

public:
    void register_object_to_destroy(CObject* object_to_destroy);
#ifdef DEBUG
    bool registered_object_to_destroy(const CObject* object_to_destroy) const;
#endif // #ifdef DEBUG

private:
    static void dump_list(Objects& v, LPCSTR reason);
};
