#pragma once

enum{
		AF_GODMODE			=(1<<0),
		AF_KEYPRESS_ON_START=(1<<1),
		AF_ALWAYSRUN		=(1<<2),
		AF_UNLIMITEDAMMO	=(1<<3),
		AF_DOF_ZOOM		=(1<<4),
		AF_WPN_AIM_TOGGLE =(1<<5),
		AF_PSP				=(1<<6),
		AF_MUSIC_TRACKS		=(1<<7),
		AF_DOF_SCOPE		=(1<<8),
		AF_AMMO_ON_BELT		=(1<<9),
		AF_3D_SCOPES		=(1<<10),
		AF_ZONES_DBG		=(1<<11),
		AF_VERTEX_DBG		=(1<<12),
		AF_CROSSHAIR_DBG	=(1<<13),
		AF_CAM_COLLISION = 1 << 14,
};

enum {
	CF_KEY_PRESS = (1 << 0),
	CF_KEY_HOLD = (1 << 1),
	CF_KEY_RELEASE = (1 << 2),
	CF_MOUSE_MOVE = (1 << 3),
	CF_MOUSE_WHEEL_ROT = (1 << 4),
};

extern Flags32 psActorFlags;
extern Flags32 psCallbackFlags;

extern BOOL		GodMode	();	

