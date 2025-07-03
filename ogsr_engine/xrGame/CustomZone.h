#pragma once

#include "space_restrictor.h"

class CActor;
class CLAItem;
class CArtefact;
class CParticlesObject;
class CZoneEffector;

#define SMALL_OBJECT_RADIUS 0.6f

//информация о объекте, находящемся в зоне
struct SZoneObjectInfo
{
    SZoneObjectInfo() : object(NULL), zone_ignore(false), time_in_zone(0), hit_num(0), total_damage(0), small_object(false), nonalive_object(false), death_in_zone(false) {}
    CGameObject* object;
    bool small_object;
    bool nonalive_object;
    //игнорирование объекта в зоне
    bool zone_ignore;
    //присоединенные партиклы
    xr_vector<CParticlesObject*> particles_vector;
    //время прибывания в зоне
    u32 time_in_zone;
    //количество раз, сколько зона воздействовала на объект
    u32 hit_num;
    //количество повреждений нанесенных зоной
    float total_damage;
    //существо померло в зоне
    bool death_in_zone;

    bool operator==(const CGameObject* O) const { return object == O; }
};

class CCustomZone : public CSpaceRestrictor
{
    friend class CAnomalyZoneScript;

private:
    typedef CSpaceRestrictor inherited;

public:
    CZoneEffector* m_effector;

public:
    CCustomZone(void);
    virtual ~CCustomZone();

    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Export(CSE_Abstract* E);
    virtual void Load(LPCSTR section);
    virtual void net_Destroy();

    virtual void UpdateCL();
    virtual void UpdateWorkload(u32 dt); // related to fast-mode optimizations
    virtual void shedule_Update(u32 dt);
    virtual void enter_Zone(SZoneObjectInfo& io);
    virtual void exit_Zone(SZoneObjectInfo& io);
    virtual void feel_touch_new(CObject* O);
    virtual void feel_touch_delete(CObject* O);
    virtual BOOL feel_touch_contact(CObject* O);
    virtual BOOL feel_touch_on_contact(CObject* O);
    virtual float effective_radius();
    virtual float distance_to_center(CObject* O);
    virtual void Postprocess(float val) {}
    virtual void net_Relcase(CObject* O);
    virtual void OnEvent(NET_Packet& P, u16 type);
    void OnOwnershipTake(u16 id);

    float GetMaxPower() { return m_fMaxPower; }
    void SetMaxPower(float p) { m_fMaxPower = p; }

    //вычисление силы хита в зависимости от расстояния до центра зоны
    //относительный размер силы (от 0 до 1)
    virtual float RelativePower(float dist);
    //абсолютный размер
    float Power(float dist);

    virtual CCustomZone* cast_custom_zone() { return this; }

    //различные состояния в которых может находиться зона
    typedef enum
    {
        eZoneStateIdle = 0, //состояние зоны, когда внутри нее нет активных объектов
        eZoneStateAwaking, //пробуждение зоны (объект попал в зону)
        eZoneStateBlowout, //выброс
        eZoneStateAccumulate, //накапливание энергии, после выброса
        eZoneStateDisabled,
        eZoneStateMax
    } EZoneState;

protected:
    bool IgnoreAny{};
    bool IgnoreNonAlive{};
    bool IgnoreSmall{};
    bool IgnoreArtefact{};
    bool BirthOnNonAlive{};
    bool BirthOnAlive{};
    bool BirthOnDead{};
    bool VisibleDetector{};
    bool BlowoutWind{};
    bool SpawnBlowoutArtefacts{};
    bool UseOnOffTime{};

    u32 m_owner_id; // if created from artefact
    u32 m_ttl;
    //список объетков, находящихся в зоне
    CActor* m_pLocalActor;

    //максимальная сила заряда зоны
    float m_fMaxPower;

    //линейный коэффициент затухания в зависимости от расстояния
    float m_fAttenuation;
    //процент удара зоны, который пойдет на физический импульс
    float m_fHitImpulseScale;
    //размер радиуса в процентах от оригинального,
    //где действует зона
    float m_fEffectiveRadius;

    //тип наносимого хита
    ALife::EHitType m_eHitTypeBlowout;

    EZoneState m_eZoneState;
    bool m_keep_update;

    //текущее время пребывания зоны в определенном состоянии
    int m_iStateTime;
    int m_iPreviousStateTime;

    u32 m_TimeToDisable;
    u32 m_TimeToEnable;
    u32 m_TimeShift;
    u32 m_StartTime;

    //массив с временами, сколько каждое состояние должно
    //длиться (если 0, то мгновенно -1 - бесконечность,
    //-2 - вообще не должно вызываться)
    typedef svector<int, eZoneStateMax> StateTimeSVec;
    StateTimeSVec m_StateTime;

    virtual void SwitchZoneState(EZoneState new_state);
    virtual void OnStateSwitch(EZoneState new_state);
    virtual void CheckForAwaking();
    //обработка зоны в различных состояниях
    virtual bool IdleState();
    virtual bool AwakingState();
    virtual bool BlowoutState();
    virtual bool AccumulateState();

    bool Enable();
    bool Disable();
    void UpdateOnOffState();
    virtual void GoEnabledState();
    virtual void GoDisabledState();

public:
    bool IsEnabled() { return m_eZoneState != eZoneStateDisabled; };
    void ZoneEnable();
    void ZoneDisable(bool = false);
    EZoneState ZoneState() { return m_eZoneState; }

protected:
    //воздействие зоной на объект
    virtual void Affect(SZoneObjectInfo* O) {}

    //воздействовать на все объекты в зоне
    virtual void AffectObjects();

    u32 m_dwAffectFrameNum;

    u32 m_dwDeltaTime;
    u32 m_dwPeriod;
    //	bool					m_bZoneReady;
    //если в зоне есть не disabled объекты
    bool m_bZoneActive;

    //параметры для выброса, с какой задержкой
    //включать эффекты и логику
    u32 m_dwBlowoutParticlesTime;
    u32 m_dwBlowoutLightTime;
    u32 m_dwBlowoutSoundTime;
    u32 m_dwBlowoutExplosionTime;
    void UpdateBlowout();

    //ветер
    bool m_bBlowoutWindActive;
    u32 m_dwBlowoutWindTimeStart;
    u32 m_dwBlowoutWindTimePeak;
    u32 m_dwBlowoutWindTimeEnd;
    //сила ветра (увеличение текущего) (0,1) когда в аномалию попадает актер
    float m_fBlowoutWindPowerMax;
    float m_fStoreWindPower;

    void StartWind();
    void StopWind();
    void UpdateWind();

    //время, через которое, зона перестает реагировать
    //на объект мертвый объект (-1 если не указано)
    int m_iDisableHitTime;
    //тоже самое но для маленьких объектов
    int m_iDisableHitTimeSmall;
    int m_iDisableIdleTime;

    ////////////////////////////////
    // имена партиклов зоны

    //обычное состояние зоны
    shared_str m_sIdleParticles;
    //выброс зоны
    shared_str m_sBlowoutParticles;
    shared_str m_sAccumParticles;
    shared_str m_sAwakingParticles;

    //появление большого и мальнекого объекта в зоне
    shared_str m_sEntranceParticlesSmall;
    shared_str m_sEntranceParticlesBig;
    //поражение большого и мальнекого объекта в зоне
    shared_str m_sHitParticlesSmall;
    shared_str m_sHitParticlesBig;
    //нахождение большого и мальнекого объекта в зоне
    shared_str m_sIdleObjectParticlesSmall;
    shared_str m_sIdleObjectParticlesBig;
    BOOL m_bIdleObjectParticlesDontStop;

    ref_sound m_idle_sound;
    ref_sound m_awaking_sound;
    ref_sound m_accum_sound;
    ref_sound m_blowout_sound;
    ref_sound m_hit_sound;
    ref_sound m_entrance_sound;

    // Interactive grass Settings
    u32 m_fBlowoutTimeLeft{};

    s8 m_BendGrass_idle_anim{};
    float m_BendGrass_idle_radius{};
    float m_BendGrass_idle_speed{};
    float m_BendGrass_idle_str{};

    s8 m_BendGrass_whenactive_anim{};
    float m_BendGrass_whenactive_speed{};
    float m_BendGrass_whenactive_str{};

    bool m_BendGrass_Blowout{};
    u32 m_BendGrass_Blowout_time{};
    float m_BendGrass_Blowout_speed{};
    float m_BendGrass_Blowout_radius{};

    size_t grassbender_id{};
    u32 grassbender_frame{};

    void GrassZoneUpdate();

    //объект партиклов обычного состояния зоны
    CParticlesObject* m_pIdleParticles;
    CParticlesObject* m_pBlowoutParticles;
    CParticlesObject* m_pAccumParticles;
    CParticlesObject* m_pAwakingParticles;

    //////////////////////////////
    //подсветка аномалии

    //подсветка idle состояния
    ref_light m_pIdleLight;
    Fcolor m_IdleLightColor;
    float m_fIdleLightRange;
    float m_fIdleLightRangeDelta;
    float m_fIdleLightHeight;
    CLAItem* m_pIdleLAnim;

    // подсветка выброса
    ref_light m_pLight;
    Fcolor m_LightColor;
    float m_fLightRange;
    float m_fLightHeight;
    float m_fLightTime;
    float m_fLightTimeLeft;

    bool IdleLight{};
    bool IdleLightShadow{};
    bool IdleLightVolumetric{};
    float IdleLightVolumetricQuality{};
    float IdleLightVolumetricIntensity{};
    float IdleLightVolumetricDistance{};

    bool BlowoutLight{};
    bool BlowoutLightShadow{};
    bool BlowLightVolumetric{};
    float BlowLightVolumetricQuality{};
    float BlowLightVolumetricIntensity{};
    float BlowLightVolumetricDistance{};

    void StartIdleLight();
    void StopIdleLight();
    void UpdateIdleLight();

    void StartBlowoutLight();
    void StopBlowoutLight();
    void UpdateBlowoutLight();

    //список партиклов для объетов внутри зоны
    //	DEFINE_MAP (CObject*, SZoneObjectInfo, OBJECT_INFO_MAP, OBJECT_INFO_MAP_IT);
    DEFINE_VECTOR(SZoneObjectInfo, OBJECT_INFO_VEC, OBJECT_INFO_VEC_IT);
    OBJECT_INFO_VEC m_ObjectInfoMap;

    void CreateHit(u16 id_to, u16 id_from, const Fvector& hit_dir, float hit_power, s16 bone_id, const Fvector& pos_in_bone, float hit_impulse, ALife::EHitType hit_type);

    virtual void Hit(SHit* pHDS);

    //для визуализации зоны
    void PlayIdleParticles();
    void StopIdleParticles();
    void PlayAccumParticles();
    void StopAccumParticles();
    void PlayAwakingParticles();
    void StopAwakingParticles();
    void PlayBlowoutParticles();
    void StopBlowoutParticles();
    void PlayEntranceParticles(CGameObject* pObject);
    void PlayBulletParticles(Fvector& pos);

    void PlayHitParticles(CGameObject* pObject);

    void PlayObjectIdleParticles(CGameObject* pObject);
    void StopObjectIdleParticles(CGameObject* pObject);

    virtual bool IsVisibleForZones() { return false; }

    //обновление, если зона передвигается
    virtual void OnMove();
    Fvector m_vPrevPos;
    u32 m_dwLastTimeMoved;

    //видимость зоны детектором
public:
    bool VisibleByDetector() { return VisibleDetector; }

    //////////////////////////////////////////////////////////////////////////
    // список артефактов
protected:
    virtual void SpawnArtefact();

    //рождение артефакта в зоне, во время ее срабатывания
    //и присоединение его к зоне
    void BornArtefact(bool forced);
    //выброс артефактов из зоны
    void ThrowOutArtefact(CArtefact* pArtefact);

    // void	PrefetchArtefacts			();
    virtual BOOL AlwaysTheCrow();

protected:
    DEFINE_VECTOR(CArtefact*, ARTEFACT_VECTOR, ARTEFACT_VECTOR_IT);
    ARTEFACT_VECTOR m_SpawnedArtefacts;

    //есть ли вообще функция выбрасывания артефактов во время срабатывания
    //	bool					m_bSpawnBlowoutArtefacts;
    //вероятность того, что артефакт засповниться при единичном
    //срабатывании аномалии
    float m_fArtefactSpawnProbability;
    // bak вероятность спавна при смерти в зоне
    float m_fArtefactSpawnOnDeathProbability;

    //величина импульса выкидывания артефакта из зоны
    float m_fThrowOutPower;
    //высота над центром зоны, где будет появляться артефакт
    float m_fArtefactSpawnHeight;

    //имя партиклов, которые проигрываются во время и на месте рождения артефакта
    shared_str m_sArtefactSpawnParticles;
    //звук рождения артефакта
    ref_sound m_ArtefactBornSound;

    struct ARTEFACT_SPAWN
    {
        shared_str section;
        float probability;
    };

    DEFINE_VECTOR(ARTEFACT_SPAWN, ARTEFACT_SPAWN_VECTOR, ARTEFACT_SPAWN_IT);
    ARTEFACT_SPAWN_VECTOR m_ArtefactSpawn;

    //расстояние от зоны до текущего актера
    float m_fDistanceToCurEntity;

    // bak / флаг для рождения артефакта
    bool m_bBornOnBlowoutFlag;

protected:
    u32 m_ef_anomaly_type;
    u32 m_ef_weapon_type;
    bool m_b_always_fastmode;

    bool DestroyAfterBlowout{}; //Для самоудаления мин после взрыва
    u32 LastBlowoutTime{};

public:
    virtual u32 ef_anomaly_type() const;
    virtual u32 ef_weapon_type() const;
    virtual bool register_schedule() const { return true; }

    // optimization FAST/SLOW mode
public:
    bool o_fastmode;
    IC void o_switch_2_fast()
    {
        if (o_fastmode)
            return;
        o_fastmode = true;
        processing_activate();
    }
    IC void o_switch_2_slow()
    {
        if (!o_fastmode)
            return;
        o_fastmode = false;
        processing_deactivate();
    }
};
