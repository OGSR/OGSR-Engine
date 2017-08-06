#pragma once

enum{
		AF_GODMODE			=(1<<0),
		AF_INVISIBLE		=(1<<1),
		AF_ALWAYSRUN		=(1<<2),
		AF_UNLIMITEDAMMO	=(1<<3),
		AF_RUN_BACKWARD		=(1<<4),
		AF_AUTOPICKUP		=(1<<5),
		AF_PSP				=(1<<6),
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

