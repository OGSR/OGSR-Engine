#include "stdafx.h"
#include "Statistic.h"

WeaponUsageStatistic::WeaponUsageStatistic()
{
	Clear();	
};

void	WeaponUsageStatistic::Clear			()
{
	aPlayersStatistic.clear();
	aUsedFiles.clear();
	aWeapons.clear();
	aBones.clear();

	ZeroMemory(m_dwTotalPlayersAliveTime, sizeof(m_dwTotalPlayersAliveTime));
	ZeroMemory(m_dwTotalPlayersMoneyRound, sizeof(m_dwTotalPlayersMoneyRound));
	ZeroMemory(m_dwTotalNumRespawns, sizeof(m_dwTotalNumRespawns));
};

WeaponUsageStatistic::~WeaponUsageStatistic()
{
	aPlayersStatistic.clear();
	aUsedFiles.clear();
	aWeapons.clear();
	aBones.clear();
};


bool					WeaponUsageStatistic::GetPlayer			(LPCSTR PlayerName, PLAYERS_STATS_it& pPlayerI)
{
	pPlayerI	= std::find(aPlayersStatistic.begin(), aPlayersStatistic.end(), PlayerName);
	if (pPlayerI == aPlayersStatistic.end() || !((*pPlayerI) == PlayerName))
		return false;
	return true;
}
PLAYERS_STATS_it					WeaponUsageStatistic::FindPlayer			(LPCSTR PlayerName)
{
	PLAYERS_STATS_it pPlayerI;
	if (!GetPlayer(PlayerName, pPlayerI))
	{
		aPlayersStatistic.push_back(Player_Statistic(PlayerName));
		pPlayerI = aPlayersStatistic.end()-1;
	};
	return pPlayerI;
};

Player_Statistic::Player_Statistic(LPCSTR Name)
{
	strcpy(PName,Name);
	TotalShots = 0;

	ZeroMemory(m_dwTotalAliveTime, sizeof(m_dwTotalAliveTime));
	ZeroMemory(m_dwTotalMoneyRound, sizeof(m_dwTotalMoneyRound));
	ZeroMemory(m_dwNumRespawned, sizeof(m_dwNumRespawned));
};

Player_Statistic::~Player_Statistic()
{
	aWeaponStats.clear();
};

bool	Player_Statistic::GetPlayersWeapon(LPCSTR WeaponName, WEAPON_STATS_it& Weapon_it)
{
	Weapon_it = std::find(aWeaponStats.begin(), aWeaponStats.end(), WeaponName);
	if (Weapon_it == aWeaponStats.end() || !((*Weapon_it) == WeaponName))
		return false;
	return true;
};

WEAPON_STATS_it	Player_Statistic::FindPlayersWeapon	(LPCSTR WeaponName, LPCSTR WIName)
{
	WEAPON_STATS_it pWeaponI;// = std::find(aWeaponStats.begin(), aWeaponStats.end(), WeaponName);
	if (!GetPlayersWeapon(WeaponName, pWeaponI) && !GetPlayersWeapon(WIName, pWeaponI))//	pWeaponI == aWeaponStats.end() || !((*pWeaponI) == WeaponName))
	{
		aWeaponStats.push_back(Weapon_Statistic(WeaponName, WIName));		
		pWeaponI = aWeaponStats.end()-1;
//		pWeaponI->InvName = pSettings->r_string_wb(WeaponName, "inv_name");
	}
	return pWeaponI;
};

Weapon_Statistic::Weapon_Statistic(LPCSTR Name, LPCSTR IName)
{
	strcpy(WName,Name);
	strcpy(InvName,IName);
	
	NumBought = 0;
	RoundsFired = 0;
	BulletsFired = 0;
	HitsScored = 0;
	KillsScored = 0;

	m_Hits.clear();
	ZeroMemory(m_Basket, sizeof(m_Basket));
};
Weapon_Statistic::~Weapon_Statistic()
{
	m_Hits.clear();
	ZeroMemory(m_Basket, sizeof(m_Basket));
};