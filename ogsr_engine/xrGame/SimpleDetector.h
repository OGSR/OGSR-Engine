#pragma once
#include "CustomDetector.h"

class CUIArtefactDetectorSimple;

class CSimpleDetector : public CCustomDetector
{
    typedef CCustomDetector inherited;

public:
    CSimpleDetector();
    virtual ~CSimpleDetector() = default;

protected:
    virtual void UpdateAf() override;
    virtual void CreateUI() override;
    CUIArtefactDetectorSimple& ui();
};
