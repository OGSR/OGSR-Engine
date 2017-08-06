#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <typeinfo.h>
#include <Windows.h>

// stl
#pragma warning (push)
#pragma warning (disable:4702)
#include <algorithm>
#include <limits>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <string>
#pragma warning (pop)
#pragma warning (disable : 4100 )		// unreferenced formal parameter

//#include "vector.h"
//#include "_stl_extensions.h"
#include "Definitions.h"

struct HitData
{
	Fvector Pos0;
	Fvector Pos1;

	s16			BoneID;
	string1024	TargetName;
	string1024	BoneName;
	u32			BulletID;
	bool		Deadly;

//	bool	Completed;

//	void			net_save			(NET_Packet* P);
//	void			net_load			(NET_Packet* P);

	bool			operator	==		(u32 ID)	{return ID == BulletID;};
	bool			operator	!=		(u32 ID)	{return ID != BulletID;};
	//-----------------------------------------------------------
//	void				Write						(FILE* pFile);
	void				Load						(FILE* pFile);
};

DEF_VECTOR	(HITS_VEC, HitData);

#define MAX_BASKET		34
struct Weapon_Statistic
{
	string1024		WName;
	string1024		InvName;
	u32				NumBought;
	u32				RoundsFired;
	u32				BulletsFired;
	u32				HitsScored;
	u32				KillsScored;
	u32				m_Basket[3][MAX_BASKET];

//	u32				m_dwNumCompleted;
	HITS_VEC		m_Hits;


	Weapon_Statistic(LPCSTR Name, LPCSTR IName);
	~Weapon_Statistic();

//	void			net_save			(NET_Packet* P);
//	void			net_load			(NET_Packet* P);

//	bool			FindHit				(u32 BulletID, HITS_VEC_it& Hit_it);
	bool			operator	==		(LPCSTR name)
	{
		int res = strcmp(WName, name);
		if (res == 0) return true;
		res = strcmp(InvName, name);
		return	res	 == 0;
	}
	//-----------------------------------------------------------
//	void				Write						(FILE* pFile);
	void				Load						(FILE* pFile);
};

DEF_VECTOR		(WEAPON_STATS, Weapon_Statistic);

struct Player_Statistic
{
	string1024		PName;

	u32				TotalShots;
	u32				m_dwTotalAliveTime[3];
	s32				m_dwTotalMoneyRound[3];
	u32				m_dwNumRespawned[3];

	WEAPON_STATS	aWeaponStats;	
	//-----------------------------------------------

	Player_Statistic(LPCSTR Name);
	~Player_Statistic();

	WEAPON_STATS_it	FindPlayersWeapon	(LPCSTR WeaponName, LPCSTR WIName);
	bool	GetPlayersWeapon(LPCSTR WeaponName, WEAPON_STATS_it& Weapon_it);

//	void			net_save			(NET_Packet* P);
//	void			net_load			(NET_Packet* P);

	bool			operator	==		(LPCSTR name){int res = stricmp(PName, name);return	res	 == 0;}
	//-----------------------------------------------------------
//	void				Write						(FILE* pFile);
	void				Load			(FILE* pFile);
};

DEF_VECTOR	(PLAYERS_STATS, Player_Statistic);

struct Used_Name
{
	string1024 Name;
	bool			operator	==		(LPCSTR name){int res = strcmp(Name, name);return	res	 == 0;}
	Used_Name (char* pName) { strcpy(Name, pName);};
};
DEF_VECTOR	(USED_NAMES, Used_Name);

struct WeaponUsageStatistic 
{
	//-----------------------------------------------
	PLAYERS_STATS	aPlayersStatistic;
	USED_NAMES		aWeapons;
	USED_NAMES		aBones;
	//-----------------------------------------------
	u32				m_dwTotalPlayersAliveTime[3];
	s32				m_dwTotalPlayersMoneyRound[3];
	u32				m_dwTotalNumRespawns[3];
	//-----------------------------------------------
	WeaponUsageStatistic();
	~WeaponUsageStatistic();

	void				Clear			();
	//-----------------------------------------------
	PLAYERS_STATS_it					FindPlayer			(LPCSTR PlayerName);
	bool								GetPlayer			(LPCSTR PlayerName, PLAYERS_STATS_it& pPlayerI);	
	//-----------------------------------------------
	USED_NAMES			aUsedFiles;
	//-----------------------------------------------
	void				LoadFile		(char* FileName);
	void				Load			(FILE* pFile);

	void				SaveFile		(char* FileName);
	void				Save			(FILE* pFile);
	//-----------------------------------------------
	void				Write_Header				(FILE* pFile);
	void				Write_WeaponUsageFrequency	(FILE* pFile);
	void				Write_WeaponKills			(FILE* pFile);
	void				Write_WeaponEfficiency		(FILE* pFile);
	void				Write_WeaponAccuracy		(FILE* pFile);
	void				Write_EffectiveRange		(FILE* pFile);
	void				Write_BonesHits				(FILE* pFile);
	void				Write_BonesKills			(FILE* pFile);
	
	void				Write_AverageMoney			(FILE* pFile);
	void				Write_Basket				(FILE* pFile);
	void				Write_AverageLifeSpan		(FILE* pFile);
};

extern	string1024 LevelName;
extern	string1024 GameType;
extern	string1024 FilePath;
