#pragma once
#include "CustomDetector.h"
#include "Level.h"

class CUIArtefactDetectorElite;

class CEliteDetector : public CCustomDetector
{
    typedef CCustomDetector inherited;

public:
    CEliteDetector();
    virtual ~CEliteDetector() = default;
    virtual void render_item_3d_ui() override;
    virtual bool render_item_3d_ui_query() override;
    virtual LPCSTR ui_xml_tag() const { return "elite"; }

protected:
    virtual void UpdateAf() override;
    virtual void CreateUI() override;
    CUIArtefactDetectorElite& ui();
};

class CScientificDetector : public CEliteDetector
{
    typedef CEliteDetector inherited;

public:
    CScientificDetector();
    virtual ~CScientificDetector();
    virtual void Load(LPCSTR section) override;
    virtual void OnH_B_Independent(bool just_before_destroy) override;
    virtual void shedule_Update(u32 dt) override;
    virtual LPCSTR ui_xml_tag() const override { return "scientific"; }

protected:
    virtual void UpfateWork() override;
    CZoneList m_zones;
};
