#pragma once

#include "inventory_item_object.h"

struct SBoneProtections;

class CCustomOutfit : public CInventoryItemObject
{
    friend class COutfitScript;

private:
    typedef CInventoryItemObject inherited;

public:
    CCustomOutfit(void);
    virtual ~CCustomOutfit(void);

    virtual void Load(LPCSTR section);

    //уменьшенная версия хита, для вызова, когда костюм надет на персонажа
    virtual void Hit(float P, ALife::EHitType hit_type);

    //коэффициенты на которые домножается хит
    //при соответствующем типе воздействия
    //если на персонаже надет костюм
    float GetHitTypeProtection(ALife::EHitType hit_type, s16 element);
    float GetDefHitTypeProtection(ALife::EHitType hit_type);

    float HitThruArmour(float hit_power, s16 element, float AP);
    //коэффициент на который домножается потеря силы
    //если на персонаже надет костюм
    float GetPowerLoss();

    virtual void OnMoveToSlot();
    virtual void OnMoveToRuck(EItemPlace prevPlace) override;
    void OnDrop() override;

private:
    void OnDropOrMoveToRuck();

    HitImmunity::HitTypeSVec m_HitTypeProtection;
    float m_fPowerLoss{};

    shared_str m_ActorVisual;
    shared_str m_full_icon_name;
    SBoneProtections* m_boneProtection;

    u32 m_ef_equipment_type{};

    u32 m_artefact_count{};

public:
    float m_additional_weight{};
    float m_additional_weight2{};
    shared_str m_NightVisionSect;
    virtual u32 ef_equipment_type() const;
    const shared_str& GetFullIconName() const { return m_full_icon_name; };

    virtual void net_Export(CSE_Abstract* E);

    float m_fBleedingRestoreSpeed;
    float m_fHealthRestoreSpeed;
    float m_fPowerRestoreSpeed;
    float m_fSatietyRestoreSpeed;
    float m_fThirstRestoreSpeed;

    u32 get_artefact_count() const { return m_artefact_count; }
};
