// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "physicsshellholder.h"

class CPHStaticGeomShell;
struct dContact;
struct SGameMtl;

class CBreakableObject : public CPhysicsShellHolder
{
    typedef CPhysicsShellHolder inherited;

private:
    bool b_resived_damage;
    float m_max_frame_damage;
    static float m_damage_threshold;
    static float m_health_threshhold;
    static float m_immunity_factor;
    Fvector m_contact_damage_pos;
    Fvector m_contact_damage_dir;

    float fHealth;
    CPHStaticGeomShell* m_pUnbrokenObject;
    CPhysicsShell* m_Shell;
    static u32 m_remove_time;
    u32 m_break_time;
    bool bRemoved;
    //		Fbox				m_saved_box;
public:
    CBreakableObject();
    virtual ~CBreakableObject();

    virtual void Load(LPCSTR section);
    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();
    virtual void shedule_Update(u32 dt); // Called by sheduler
    virtual void UpdateCL();

    virtual void Hit(SHit* pHDS);

    virtual void net_Export(CSE_Abstract* E);
    virtual BOOL UsedAI_Locations();

private:
    void Init();
    void CreateUnbroken();
    void CreateBroken();
    void DestroyUnbroken();
    void ActivateBroken();
    void Split();
    void Break();
    void ApplyExplosion(const Fvector& dir, float impulse);
    void CheckHitBreak(float power, ALife::EHitType hit_type);
    void ProcessDamage();
    void SendDestroy();
    void enable_notificate();
    static void ObjectContactCallback(bool& /**do_colide/**/, bool bo1, dContact& c, SGameMtl* /*material_1*/, SGameMtl* /*material_2*/);
};
