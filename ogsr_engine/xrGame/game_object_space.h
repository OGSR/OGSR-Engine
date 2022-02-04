#pragma once

namespace GameObject {
	enum ECallbackType {
		eTradeStart = u32(0),
		eTradeStop,
		eTradeSellBuyItem,
		eTradePerformTradeOperation,

		eZoneEnter,
		eZoneExit,
		eExitLevelBorder,
		eEnterLevelBorder,
		eDeath,

		ePatrolPathInPoint,

//		eInventoryPda,
		eInventoryInfo,
		eArticleInfo,
		eTaskStateChange,
		eMapLocationAdded,

		eUseObject,

		eHit,

		eSound,

		eActionTypeMovement,
		eActionTypeWatch,
		eActionTypeAnimation,
		eActionTypeSound,
		eActionTypeParticle,
		eActionTypeObject,

//		eActorSleep,

		eHelicopterOnPoint,
		eHelicopterOnHit,

		eOnItemTake,
		eOnItemDrop,

		eScriptAnimation,
		
		eTraderGlobalAnimationRequest,
		eTraderHeadAnimationRequest,
		eTraderSoundEnd,

		eInvBoxItemTake,
		eInvBoxItemPlace,

		eOnKeyPress,
		eOnKeyRelease,
		eOnKeyHold,
		eOnMouseWheel,
		eOnMouseMove,
		eOnItemToBelt,
		eOnItemToRuck,
		eOnItemToSlot,
		eOnBeforeUseItem,
		entity_alive_before_hit,

//		eOnHudAnimationEnd,

		eOnUpdateAddonsVisibiility,
		eOnUpdateHUDAddonsVisibiility,
		eOnAddonInit,

		// These specifically say actor as I intend to add callbacks for NPCs firing their weapons.
		eOnActorWeaponStartFiring,
		eOnActorWeaponFire,
		eOnActorWeaponJammed,
		eOnActorWeaponEmpty,
		eOnActorWeaponReload,
		eOnActorWeaponSwitchGL,

		// NPC Weapon Callbacks.
		eOnNPCWeaponStartFiring,
		eOnNPCWeaponFire,
		eOnNPCWeaponJammed,
		eOnNPCWeaponEmpty,
		eOnNPCWeaponReload,
		eOnNPCWeaponSwitchGL,

		// Called when the player zooms their weapon in or out.
		eOnActorWeaponZoomIn,
		eOnActorWeaponZoomOut,

		eCellItemFocus,
		eCellItemFocusLost,
		eCellItemSelect,
		eOnCellItemMouse,

		eBeforeSave,
		ePostSave,

		eUIMapClick,
		eUIMapSpotClick,

		eUIPickUpItemShowing,
		eUIGroupItems,
		eOnWpnShellDrop,
		eOnThrowGrenade,
		eOnGoodwillChange,
		eUpdateArtefactsOnBelt,
		eLevelChangerAction,

		eAttachVehicle,
		eDetachVehicle,
		eUseVehicle,

		eOnInvBoxItemTake,
		eOnInvBoxItemDrop,
		eOnInvBoxOpen,
		
		eSelectPdaContact,

		eUserCommand,

		eDummy = u32(-1),
	};
};
