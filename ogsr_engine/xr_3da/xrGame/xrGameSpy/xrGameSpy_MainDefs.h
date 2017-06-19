#pragma once

#define QRCDKEY_INTEGRATION
#define SB_ICMP_SUPPORT

#undef DEMO_BUILD

//#define DEMO_BUILD

#ifdef DEMO_BUILD
	#define	GAMESPY_GAMENAME				"stalkerscd"
	#define GAMESPY_GAMEID					1576
	#define GAMESPY_PRODUCTID				10954
	#define GAME_VERSION					"1.0007(rc1)"
	#define REGISTRY_PATH					"Software\\GSC Game World\\STALKER-SoC\\"
#else
	#define	GAMESPY_GAMENAME				"stalkersc"
	#define GAMESPY_GAMEID					1067
	#define GAMESPY_PRODUCTID				10953
	#define GAME_VERSION					"1.0007(rc1)"
	#define REGISTRY_PATH					"Software\\GSC Game World\\STALKER-SHOC\\"
#endif

#define	GAMESPY_QR2_BASEPORT				5447
#define GAMESPY_BROWSER_MAX_UPDATES			20

#define START_PORT							0
#define END_PORT							65535
#define START_PORT_LAN						5445
#define END_PORT_LAN						START_PORT_LAN + 100//GameSpy only process 500 ports

#define REGISTRY_BASE						HKEY_LOCAL_MACHINE
#define REGISTRY_VALUE_GSCDKEY				"InstallCDKEY"
#define REGISTRY_VALUE_VERSION				"InstallVers"
#define REGISTRY_VALUE_SKU					"InstallSource"
#define REGISTRY_VALUE_INSTALL_PATCH_ID		"InstallPatchID"

#define GAMESPY_PATCHING_VERSIONUNIQUE_ID	"test_version_1"
#define GAMESPY_PATCHING_DISTRIBUTION_ID	0

 

#ifndef XRGAMESPY_API
	#ifdef XRGAMESPY_EXPORTS
		#define XRGAMESPY_API				__declspec(dllexport)
	#endif
#endif

#define EXPORT_FN_DECL(r, f, p)		XRGAMESPY_API r xrGS_##f p;

extern void FillSecretKey (char* SecretKey);