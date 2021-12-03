#pragma once
#include "CustomDetector.h"
class CUIArtefactDetectorAdv;

class CAdvancedDetector : public CCustomDetector
{
    typedef CCustomDetector inherited;

public:
    CAdvancedDetector();
    virtual ~CAdvancedDetector() = default;
    virtual void on_a_hud_attach() override;
    virtual void on_b_hud_detach() override;

protected:
    virtual void UpdateAf() override;
    virtual void CreateUI() override;
    CUIArtefactDetectorAdv& ui();
};
