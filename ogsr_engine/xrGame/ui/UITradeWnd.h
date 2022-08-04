#pragma once
#include "UIWindow.h"
#include "../inventory_space.h"

class CInventoryOwner;
class CEatableItem;
class CTrade;
class CUIPropertiesBox;

struct CUITradeInternal;

class CUIDragDropListEx;
class CUICellItem;

class CUITradeWnd : public CUIWindow
{
private:
    typedef CUIWindow inherited;

public:
    CUITradeWnd();
    virtual ~CUITradeWnd();

    virtual void Init();

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);

    void InitTrade(CInventoryOwner* pOur, CInventoryOwner* pOthers);

    virtual void Draw();
    virtual void Update();
    virtual void Show();
    virtual void Hide();

    void DisableAll();
    void EnableAll();
    virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
    virtual bool OnMouse(float x, float y, EUIMessages mouse_action);

    void ActivatePropertiesBox();

    void SwitchToTalk();
    void StartTrade();
    void StopTrade();

protected:
    CUITradeInternal* m_uidata;

    bool bStarted;
    bool ToOurTrade(CUICellItem* itm);
    bool ToOthersTrade(CUICellItem* itm);
    bool ToOurBag(CUICellItem* itm);
    bool ToOthersBag(CUICellItem* itm);

    u32 CalcItemsPrice(CUIDragDropListEx* pList, CTrade* pTrade, bool bBuying);
    float CalcItemsWeight(CUIDragDropListEx* pList);

    void TransferItems(CUIDragDropListEx* pSellList, CUIDragDropListEx* pBuyList, CTrade* pTrade, bool bBuying);

    void PerformTrade();
    void UpdatePrices();
    void ColorizeItem(CUICellItem* itm, bool canTrade, bool highlighted);
    bool MoveItem(CUICellItem* itm);
    void MoveItems(CUICellItem* itm);

    enum EListType
    {
        eNone,
        e1st,
        e2nd,
        eBoth
    };

    void UpdateLists(EListType);

    void FillList(TIItemContainer&, CUIDragDropListEx&, bool);

    bool m_bDealControlsVisible;

    bool CanMoveToOther(PIItem, bool);

    //указатели игрока и того с кем торгуем
    CInventory* m_pInv;
    CInventory* m_pOthersInv;
    CInventoryOwner* m_pInvOwner;
    CInventoryOwner* m_pOthersInvOwner;
    CTrade* m_pTrade;
    CTrade* m_pOthersTrade;
    CUIPropertiesBox* m_pUIPropertiesBox;

    u32 m_iOurTradePrice;
    u32 m_iOthersTradePrice;

public:
    CUICellItem* m_pCurrentCellItem;

protected:
    TIItemContainer ruck_list;

    void SetCurrentItem(CUICellItem* itm);
    CUICellItem* CurrentItem();
    PIItem CurrentIItem();

    bool OnItemDrop(CUICellItem* itm);
    bool OnItemStartDrag(CUICellItem* itm);
    bool OnItemDbClick(CUICellItem* itm);
    bool OnItemSelected(CUICellItem* itm);
    bool OnItemRButtonClick(CUICellItem* itm);

    void BindDragDropListEvents(CUIDragDropListEx* lst);
};
