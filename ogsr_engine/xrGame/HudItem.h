//////////////////////////////////////////////////////////////////////
// HudItem.h: класс предок для всех предметов имеющих
//			  собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#pragma once

class CSE_Abstract;
class CPhysicItem;
class NET_Packet;
struct HUD_SOUND;
class CInventoryItem;
struct attachable_hud_item;
class motion_marks;
class CMotionDef;

#include "actor_defs.h"

class CHUDState
{
public:
    enum EHudStates
    {
        eIdle,
        eFire,
        eFire2,
        eReload,
        eShowing,
        eHiding,
        eHidden,
        eMisfire,
        eMagEmpty,
        eSwitch,
        eSprintStart,
        eSprintEnd,
        eBore,
        eDeviceSwitch,
        eThrowStart,
        eReady,
        eThrow,
        eThrowEnd,
    };

private:
    u32 m_hud_item_state;
    u32 m_nextState;
    u32 m_dw_curr_state_time;

protected:
    u32 m_dw_curr_substate_time;

public:
    CHUDState() { SetState(eHidden); }
    IC u32 GetNextState() const { return m_nextState; }
    IC u32 GetState() const { return m_hud_item_state; }
    IC void SetState(u32 v)
    {
        m_hud_item_state = v;
        m_dw_curr_state_time = Device.dwTimeGlobal;
        ResetSubStateTime();
    }
    IC void SetNextState(u32 v) { m_nextState = v; }
    IC u32 CurrStateTime() const { return Device.dwTimeGlobal - m_dw_curr_state_time; }
    IC void ResetSubStateTime() { m_dw_curr_substate_time = Device.dwTimeGlobal; }
    virtual void SwitchState(u32 S) = 0;
    virtual void OnStateSwitch(u32 S, u32 oldState) = 0;
};

class CHudItem : public CHUDState
{
protected: //чтоб нельзя было вызвать на прямую
    CHudItem();
    virtual ~CHudItem() = default;
    virtual DLL_Pure* _construct();

    Flags16 m_huditem_flags;
    enum
    {
        fl_pending = (1 << 0),
        fl_renderhud = (1 << 1),
        fl_inertion_enable = (1 << 2),
        fl_inertion_allow = (1 << 3),
        fl_bobbing_allow = (1 << 4),
    };

    // Motion data
    const CMotionDef* m_current_motion_def;
    shared_str m_current_motion;
    u32 m_dwMotionCurrTm;
    u32 m_dwMotionStartTm;
    u32 m_dwMotionEndTm;
    u32 m_startedMotionState;

    bool m_bStopAtEndAnimIsRunning;
    bool SprintType{};
    bool BobbingEnable{};
    u32 m_dwStateTime;

public:
    virtual void Load(LPCSTR section);
    virtual CHudItem* cast_hud_item() { return this; }

    virtual void PlaySound(HUD_SOUND& snd, const Fvector& position, bool overlap = false);

    ///////////////////////////////////////////////
    // общие функции HUD
    ///////////////////////////////////////////////
    virtual void StopHUDSounds(){};

    //для предачи команд владельцем
    virtual bool Action(s32 cmd, u32 flags);

    virtual void OnDrawUI(){};

    virtual bool IsHidden() const { return GetState() == eHidden; } // Does weapon is in hidden state
    virtual bool IsHiding() const { return GetState() == eHiding; }
    virtual bool IsShowing() const { return GetState() == eShowing; }
    //посылка сообщения на сервер о смене состояния оружия
    virtual void SwitchState(u32 S);
    //прием сообщения с сервера и его обработка
    virtual void OnStateSwitch(u32 S, u32 oldState);
    virtual void OnEvent(NET_Packet& P, u16 type);

    virtual void OnH_A_Chield();
    virtual void OnH_B_Chield();
    virtual void OnH_B_Independent(bool just_before_destroy);
    virtual void OnH_A_Independent();

    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();

    virtual bool Activate(bool = false);
    virtual void Deactivate(bool = false);

    virtual void OnActiveItem(){};
    virtual void OnHiddenItem(){};

    virtual void OnAnimationEnd(u32 state);
    virtual void OnMotionMark(u32 state, const motion_marks&){};
    virtual void OnMovementChanged(ACTOR_DEFS::EMoveCommand cmd);

    virtual void PlayAnimIdle();
    bool TryPlayAnimIdle();
    virtual bool IsZoomed() const { return false; }
    // virtual void	PlayAnimBore		();
    virtual void PlayAnimIdleMoving();
    virtual void PlayAnimIdleMovingSlow();
    virtual void PlayAnimIdleSprint();
    virtual void PlayAnimSprintStart();
    virtual void PlayAnimSprintEnd();
    virtual void PlayAnimIdleMovingCrouch();
    virtual void PlayAnimIdleMovingCrouchSlow();
    virtual void PlayAnimDeviceSwitch(){};

    virtual bool NeedBlendAnm();

    virtual void UpdateCL();
    virtual void renderable_Render();

    virtual void Hide(bool = false) {}
    virtual void Show(bool = false) {}

    virtual void UpdateXForm() = 0;

    u32 PlayHUDMotion(std::initializer_list<const char*>, const bool bMixIn, const u32 state, const bool randomAnim = true, float speed = 1.f);
    u32 PlayHUDMotion(const char* M, const bool bMixIn, const u32 state, const bool randomAnim = true, float speed = 1.f);
    u32 PlayHUDMotion_noCB(const shared_str& M, const bool bMixIn, const bool randomAnim = true, float speed = 1.f);

    bool AnimationExist(const char* M) const;
    void StopCurrentAnimWithoutCallback();

    attachable_hud_item* HudItemData() const;
    bool used_cop_fire_point() const;
    virtual void on_a_hud_attach();
    virtual void on_b_hud_detach();
    const shared_str& HudSection() const { return hud_sect; }

    BOOL GetHUDmode();
    IC void SetPending(BOOL H) { m_huditem_flags.set(fl_pending, H); }
    IC BOOL IsPending() const { return !!m_huditem_flags.test(fl_pending); }
    IC void RenderHud(BOOL B) { m_huditem_flags.set(fl_renderhud, B); }
    IC BOOL RenderHud() { return m_huditem_flags.test(fl_renderhud); }

    virtual void render_hud_mode(){};
    virtual bool need_renderable() { return true; };
    virtual void render_item_3d_ui() {}
    virtual bool render_item_3d_ui_query() { return false; }
    virtual bool CheckCompatibility(CHudItem*) { return true; }

protected:
    BOOL hud_mode;
    shared_str hud_sect;

    //кадры момента пересчета XFORM и FirePos
    u32 dwFP_Frame;
    u32 dwXF_Frame;

protected:
    u32 m_animation_slot;

public:
    IC u32 animation_slot() { return m_animation_slot; }

private:
    CPhysicItem* m_object;
    CInventoryItem* m_item;

public:
    IC CPhysicItem& object() const
    {
        VERIFY(m_object);
        return (*m_object);
    }
    IC CInventoryItem& item() const
    {
        VERIFY(m_item);
        return (*m_item);
    }

    virtual void on_renderable_Render() = 0;

public:
    class CWeaponBobbing
    {
    public:
        CWeaponBobbing() = delete;
        CWeaponBobbing(CHudItem* parent);
        ~CWeaponBobbing() = default;

        void Update(Fmatrix& m, Fmatrix& m2);
        void CheckState();

    private:
        CHudItem* parent_hud_item;
        float fTime;
        Fvector vAngleAmplitude;
        float fYAmplitude;
        float fSpeed;

        u32 dwMState;
        float fReminderFactor;
        bool is_limping;
        bool m_bZoomMode;

        float m_fAmplitudeController;
        float m_fAmplitudeRun;
        float m_fAmplitudeWalk;
        float m_fAmplitudeLimp;

        float m_fSpeedRun;
        float m_fSpeedWalk;
        float m_fSpeedLimp;

        float m_fCrouchFactor;
        float m_fZoomFactor;
        float m_fScopeZoomFactor;
    };
    std::unique_ptr<CWeaponBobbing> m_bobbing;

    virtual u8 GetCurrentHudOffsetIdx() const { return 0; }
    virtual float GetHudFov(); // Получить HUD FOV от текущего оружия игрока
    void UpdateHudAdditional(Fmatrix&, const bool need_update_collision = false);
    bool HudBobbingAllowed() const { return m_huditem_flags.test(fl_bobbing_allow); }
    bool AnmIdleMovingAllowed() const;
    void AllowHudBobbing(BOOL B) { m_huditem_flags.set(fl_bobbing_allow, B); }
    void GetBoneOffsetPosDir(const shared_str& bone_name, Fvector& dest_pos, Fvector& dest_dir, const Fvector& offset);
    //Функция из ганслингера для приблизительной коррекции разности фовов худа и мира. Так себе на самом деле, но более годных способов я не нашел.
    void CorrectDirFromWorldToHud(Fvector& dir);
    float GetLastHudFov() const { return m_nearwall_last_hud_fov; }

protected:
    enum CollisionWeaponType : size_t
    {
        CWeaponBase,
        Pistol,
        SniperRifle,
        RPG,
        RG_6,
        Groza,
        FN2000,
        BM_16,
        Bolt,
        Detector,
        Knife_and_other,
        Binocular,
        _CollisionWeaponTypesCount_
    };
    virtual size_t GetWeaponTypeForCollision() const { return Knife_and_other; }
    virtual Fvector GetPositionForCollision() { return Device.vCameraPosition; }
    virtual Fvector GetDirectionForCollision() { return Device.vCameraDirection; }
    float m_fZoomRotationFactor{}; //от 0 до 1, показывает насколько процентов мы перемещаем HUD
    float m_fZoomRotateTime{}; //время приближения
    u32 skip_updated_frame{};
    bool HudInertionAllowed() const { return m_huditem_flags.test(fl_inertion_allow); }
    void AllowHudInertion(BOOL B) { m_huditem_flags.set(fl_inertion_allow, B); }
    void TimeLockAnimation();
    virtual void DeviceUpdate(){};

private:
    shared_str world_sect;
    float hud_recalc_koef{};
    void UpdateCollision(Fmatrix& trans);
    bool CollisionAllowed() const;
    bool m_nearwall_on{};
    float m_nearwall_target_hud_fov{}, m_nearwall_target_aim_hud_fov{};
    float m_nearwall_dist_max{}, m_nearwall_dist_min{};
    float m_nearwall_last_hud_fov{};
    float m_nearwall_speed_mod{}, m_nearwall_hud_offset_speed{};
    Fvector m_nearwall_target_hud_offset{}, m_nearwall_target_hud_rotate{};
    float saved_rq_range{};
    Fvector m_nearwall_last_pos{}, m_nearwall_last_rot{};
    u32 m_nearwall_last_call{};

    float m_fLR_MovingFactor{}, m_fLookout_MovingFactor{}, m_fAimLookout_MovingFactor{}, m_fJump_MovingFactor{}, m_fFall_MovingFactor{};
    Fvector m_strafe_offset[3][2]{}, m_lookout_offset[3][2]{}, m_jump_offset[3][2]{}, m_fall_offset[3][2]{};

    float m_base_fov{};

    bool allow_bobbing{true};

    struct inertion_params
    {
        float m_pitch_offset_r;
        float m_pitch_offset_n;
        float m_pitch_offset_d;
        float m_pitch_low_limit;
        // отклонение модели от "курса" из за инерции во время движения
        float m_origin_offset;
        // отклонение модели от "курса" из за инерции во время движения с прицеливанием
        float m_origin_offset_aim;
        // скорость возврата худ модели в нужное положение
        float m_tendto_speed;
        // скорость возврата худ модели в нужное положение во время прицеливания
        float m_tendto_speed_aim;
    } inertion_data{}; //--#SM+#--
    Fvector inert_st_last_dir{};
    void UpdateInertion(Fmatrix& trans);
    float GetInertionFactor() const { return 1.f; } //--#SM+#--
    float GetInertionPowerFactor() const { return 1.f; } //--#SM+#--
    bool HudInertionEnabled() const { return m_huditem_flags.test(fl_inertion_enable); }
    void EnableHudInertion(BOOL B) { m_huditem_flags.set(fl_inertion_enable, B); }
};
