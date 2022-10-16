#pragma once

#define CMD_START (1 << 0)
#define CMD_STOP (1 << 1)
#define CMD_OPT (1 << 2)

enum : u32
{
    KNIFE_SLOT,
    FIRST_WEAPON_SLOT,
    SECOND_WEAPON_SLOT,
    GRENADE_SLOT,
    APPARATUS_SLOT,
    BOLT_SLOT,
    OUTFIT_SLOT,
    PDA_SLOT,
    DETECTOR_SLOT,
    TORCH_SLOT,
    HELMET_SLOT,
    NIGHT_VISION_SLOT,
    BIODETECTOR_SLOT,
    SLOTS_TOTAL,
    NO_ACTIVE_SLOT = 255
};

#define RUCK_HEIGHT 280
#define RUCK_WIDTH 7

class CInventoryItem;
class CInventory;

typedef CInventoryItem* PIItem;
typedef xr_vector<PIItem> TIItemContainer;

enum EItemPlace
{
    eItemPlaceUndefined,
    eItemPlaceSlot,
    eItemPlaceBelt,
    eItemPlaceRuck,
    eItemPlaceBeltActor,
};

extern u32 INV_STATE_LADDER;
extern u32 INV_STATE_CAR;
extern u32 INV_STATE_BLOCK_ALL;
extern u32 INV_STATE_INV_WND;
extern u32 INV_STATE_BUY_MENU;
