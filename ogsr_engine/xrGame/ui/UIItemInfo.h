#pragma once
#include "uiwindow.h"

class CInventoryItem;
class CUIStatic;
class CUIScrollView;
class CUIProgressBar;
class CUIWpnParams;
class CUIArtefactParams;
class CPhysicsShellHolder;

// extern const char * const 		fieldsCaptionColor;

class CUIItemInfo : public CUIWindow
{
private:
    typedef CUIWindow inherited;
    struct _desc_info
    {
        CGameFont* pDescFont;
        u32 uDescClr;
        bool bShowDescrText;
    } m_desc_info{};
    CInventoryItem* m_pInvItem;

public:
    CUIItemInfo();
    virtual ~CUIItemInfo();

    void Init(float x, float y, float width, float height, LPCSTR xml_name);
    void Init(LPCSTR xml_name);
    void InitItem(CInventoryItem* pInvItem);

    void TryAddWpnInfo(CPhysicsShellHolder& obj);
    void TryAddArtefactInfo(const shared_str& af_section);
    void TryAddCustomInfo(CPhysicsShellHolder& obj);

    virtual void Draw();
    bool m_b_force_drawing;
    CUIStatic* UIName;
    CUIStatic* UIWeight;
    CUIStatic* UICost;
    CUIStatic* UICondition;
    CUIScrollView* UIDesc;
    CUIProgressBar* UICondProgresBar;
    CUIWpnParams* UIWpnParams;
    CUIArtefactParams* UIArtefactParams;

    Fvector2 UIItemImageSize;
    CUIStatic* UIItemImage;
};