#pragma once
#include "inventory_item_object.h"
#include "..\xr_3da\feel_touch.h"
#include "HudSound.h"
#include "CustomZone.h"
#include "Artifact.h"
#include "ai_sounds.h"

class CCustomZone;
class CInventoryOwner;

struct ITEM_TYPE
{
    Fvector2 freq; // min,max
    HUD_SOUND detect_snds;

    shared_str zone_map_location;
    shared_str nightvision_particle;
};

//описание зоны, обнаруженной детектором
struct ITEM_INFO
{
    ITEM_TYPE* curr_ref{};
    float snd_time{};
    //текущая частота работы датчика
    float cur_period{};
    // particle for night-vision mode
    CParticlesObject* pParticle{};

    ITEM_INFO() = default;
    ~ITEM_INFO();
};

template <typename K>
class CDetectList : public Feel::Touch
{
protected:
    string_unordered_map<shared_str, ITEM_TYPE> m_TypesMap;

public:
    xr_map<K*, ITEM_INFO> m_ItemInfos;

protected:
    virtual void feel_touch_new(CObject* O) override
    {
        auto pK = smart_cast<K*>(O);
        auto it = m_TypesMap.find(O->cNameSect());
        if (it == m_TypesMap.end())
            it = m_TypesMap.find("class_all");

        m_ItemInfos[pK].snd_time = 0.0f;
        m_ItemInfos[pK].curr_ref = &(it->second);
    }
    virtual void feel_touch_delete(CObject* O) override
    {
        K* pK = smart_cast<K*>(O);
        R_ASSERT(pK);
        m_ItemInfos.erase(pK);
    }

public:
    void destroy()
    {
        auto it = m_TypesMap.begin();
        for (; it != m_TypesMap.end(); ++it)
            HUD_SOUND::DestroySound(it->second.detect_snds);
    }
    void clear()
    {
        m_ItemInfos.clear();
        Feel::Touch::feel_touch.clear();
    }
    virtual void load(LPCSTR sect, LPCSTR prefix)
    {
        u32 i = 1;
        do
        {
            string16 temp;
            xr_sprintf(temp, "%s_class_%d", prefix, i);
            if (pSettings->line_exist(sect, temp))
            {
                shared_str item_sect = pSettings->r_string(sect, temp);

                ITEM_TYPE item_type{};

                xr_sprintf(temp, "%s_freq_%d", prefix, i);
                item_type.freq = pSettings->r_fvector2(sect, temp);

                xr_sprintf(temp, "%s_sound_%d_", prefix, i);
                HUD_SOUND::LoadSound(sect, temp, item_type.detect_snds, SOUND_TYPE_ITEM);

                m_TypesMap.emplace(std::move(item_sect), std::move(item_type));

                ++i;
            }
            else
            {
                xr_sprintf(temp, "%s_class_all", prefix);
                if (pSettings->line_exist(sect, temp))
                {
                    ITEM_TYPE item_type{};

                    xr_sprintf(temp, "%s_freq_all", prefix);
                    item_type.freq = pSettings->r_fvector2(sect, temp);

                    xr_sprintf(temp, "%s_sound_all_", prefix);
                    HUD_SOUND::LoadSound(sect, temp, item_type.detect_snds, SOUND_TYPE_ITEM);

                    m_TypesMap.emplace("class_all", std::move(item_type));
                }

                break;
            }

        } while (true);
    }
};

class CAfList : public CDetectList<CArtefact>
{
protected:
    virtual BOOL feel_touch_contact(CObject* O) override;

public:
    CAfList() : m_af_rank(0) {}
    int m_af_rank;
};

class CUIArtefactDetectorBase;

class CCustomDetector : public CHudItemObject
{
    typedef CHudItemObject inherited;

protected:
    CUIArtefactDetectorBase* m_ui{};
    bool m_bFastAnimMode{};
    bool m_bNeedActivation{};

public:
    CCustomDetector() = default;
    virtual ~CCustomDetector();

    virtual BOOL net_Spawn(CSE_Abstract* DC) override;
    virtual void Load(LPCSTR section) override;

    virtual void OnH_A_Chield() override;
    virtual void OnH_B_Independent(bool just_before_destroy) override;

    virtual void shedule_Update(u32 dt) override;
    virtual void UpdateCL() override;

    bool IsWorking() const;

    virtual void OnMoveToSlot() override;
    virtual void OnMoveToRuck(EItemPlace prevPlace) override;

    virtual void OnActiveItem() override;
    virtual void OnHiddenItem() override;
    virtual void OnStateSwitch(u32 S, u32 oldState) override;
    virtual void OnAnimationEnd(u32 state) override;
    virtual void UpdateXForm() override;

    void ToggleDetector(bool bFastMode);
    void HideDetector(bool bFastMode);
    void ShowDetector(bool bFastMode);
    float m_fAfDetectRadius;
    virtual bool CheckCompatibility(CHudItem* itm) override;

    virtual u32 ef_detector_type() const override { return 1; }

protected:
    bool CheckCompatibilityInt(CHudItem* itm, u16* slot_to_activate);
    void TurnDetectorInternal(bool b);
    // void UpdateNightVisionMode(bool b_off);
    void UpdateVisibility();
    virtual void UpfateWork();
    virtual void UpdateAf() {}
    virtual void CreateUI() {}

    bool m_bWorking;
    float m_fAfVisRadius;
    float m_fDecayRate; // Alundaio
    CAfList m_artefacts;

    HUD_SOUND sndShow, sndHide;

    virtual size_t GetWeaponTypeForCollision() const override { return Detector; }
    virtual Fvector GetPositionForCollision() override;
    virtual Fvector GetDirectionForCollision() override;
};

class CZoneList : public CDetectList<CCustomZone>
{
protected:
    virtual BOOL feel_touch_contact(CObject* O) override;

public:
    CZoneList() = default;
    virtual ~CZoneList();
};
