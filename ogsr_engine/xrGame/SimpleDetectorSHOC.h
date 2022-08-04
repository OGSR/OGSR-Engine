#pragma once
#include "inventory_item_object.h"
#include "..\xr_3da\feel_touch.h"
#include "hudsound.h"

class CCustomZone;
//описание типа зоны
struct ZONE_TYPE_SHOC
{
    //интервал частот отыгрывания звука
    float min_freq;
    float max_freq;
    //звук реакции детектора на конкретную зону
    //	ref_sound	detect_snd;
    HUD_SOUND detect_snds;

    shared_str zone_map_location;
    float m_fRadius;
};

//описание зоны, обнаруженной детектором
struct ZONE_INFO_SHOC
{
    u32 snd_time;
    //текущая частота работы датчика
    float cur_freq;
    // particle for night-vision mode
    CParticlesObject* pParticle;

    ZONE_INFO_SHOC();
    ~ZONE_INFO_SHOC();
};

class CInventoryOwner;

class CCustomDetectorSHOC : public CInventoryItemObject, public Feel::Touch
{
    typedef CInventoryItemObject inherited;

public:
    CCustomDetectorSHOC(void);
    virtual ~CCustomDetectorSHOC(void);

    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void Load(LPCSTR section);

    virtual void OnH_A_Chield();
    virtual void OnH_B_Independent(bool just_before_destroy);

    virtual void shedule_Update(u32 dt);
    virtual void UpdateCL();

    virtual void feel_touch_new(CObject* O);
    virtual void feel_touch_delete(CObject* O);
    virtual BOOL feel_touch_contact(CObject* O);

    void TurnOn();
    void TurnOff();
    bool IsWorking() { return m_bWorking; }

    virtual void OnMoveToSlot() override;
    virtual void OnMoveToRuck(EItemPlace prevPlace) override;
    virtual void OnMoveToBelt() override;

protected:
    void StopAllSounds();
    void UpdateMapLocations();
    void AddRemoveMapSpot(CCustomZone* pZone, bool bAdd);
    void UpdateNightVisionMode();

    bool m_bWorking;

    float m_fRadius;

    //если хозяин текущий актер
    CActor* m_pCurrentActor;
    CInventoryOwner* m_pCurrentInvOwner;

    //информация об онаруживаемых зонах
    DEFINE_MAP(CLASS_ID, ZONE_TYPE_SHOC, ZONE_TYPE_MAP, ZONE_TYPE_MAP_IT);
    ZONE_TYPE_MAP m_ZoneTypeMap;

    //список обнаруженных зон и информация о них
    DEFINE_MAP(CCustomZone*, ZONE_INFO_SHOC, ZONE_INFO_MAP, ZONE_INFO_MAP_IT);
    ZONE_INFO_MAP m_ZoneInfoMap;

    shared_str m_nightvision_particle;

    bool m_detect_actor_radiation;
    u32 radiation_snd_time;
    void update_actor_radiation();

protected:
    u32 m_ef_detector_type;

public:
    virtual u32 ef_detector_type() const;
};

class CSimpleDetectorSHOC : public CCustomDetectorSHOC
{
public:
    CSimpleDetectorSHOC();
    virtual ~CSimpleDetectorSHOC();
};
