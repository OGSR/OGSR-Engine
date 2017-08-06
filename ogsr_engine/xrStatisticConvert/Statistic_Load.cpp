#include "stdafx.h"
#include "Statistic.h"

u32	g_FileVersion = 0;
string1024	LevelName = "";
string1024	GameType = "";
string1024	FilePath = "";

u32		fread_u32	(FILE* pFile)
{
	u32 tmp = 0;
	fread(&tmp, 4, 1, pFile);
	return tmp;
}

void		_GetName(char* outName, char** inName)
{
	outName[0] = 0;
	if (!inName) return;

	int Len = int(strlen(*inName));
	for (int i=0; i<Len; i++)
	{
		char c = *(*inName)++;
		if (c == '(') continue;
		if (c == ')') break;
		*outName++ = c;
	}
};

void				WeaponUsageStatistic::LoadFile		(char* FileName)
{
	if (!FileName) return;

	DbgOut("Processing \'%s\'\n", FileName);

	FILE* pFile = fopen(FileName,"rb");
	if (!pFile) return;

	u32 IDENT = 0;
	fread(&IDENT, 4, 1, pFile);
	if (IDENT != WUS_IDENT)
	{
		DbgOut("Not a proper WUS file.\n");
		fclose(pFile);
		return;
	};
	fread(&g_FileVersion, 4, 1, pFile);
	if (g_FileVersion > WUS_VERSION)
	{
		DbgOut("Strange file version - Skipping!\n");
		fclose(pFile);
		return;
	}
	//-------------------------------------------
	string1024 FullPathName = "";
	char* pFileName = NULL, *ptmpFileName = NULL;
	GetFullPathName(FileName, 1024, FullPathName, &pFileName);
	ptmpFileName = pFileName;
	//-----------------------------------------------------
	aUsedFiles.push_back(Used_Name(pFileName));
	//-----------------------------------------------------
	if (GameType[0] == 0 && LevelName[0] == 0)
	{
		_GetName(LevelName, &ptmpFileName); ptmpFileName++;
		_GetName(GameType, &ptmpFileName);
		
		*pFileName = 0;
		strcpy(FilePath, FullPathName);
		/*
		if (!stricmp(GameType, "dm"))
			sprintf(GameType, "Deathmatch");
		else
			if (!stricmp(GameType, "tdm"))
				sprintf(GameType, "TeamDeathmatch");
			else
				if (!stricmp(GameType, "ah"))
					sprintf(GameType, "ArtefactHunt");
					*/		
	};
	
	//-------------------------------------------
	Load(pFile);

	fclose(pFile);
};

void				WeaponUsageStatistic::Load			(FILE* pFile)
{
	if (!pFile) return;
	//----------------------------------------------
	m_dwTotalPlayersAliveTime[0] += fread_u32(pFile);
	m_dwTotalPlayersAliveTime[1] += fread_u32(pFile);
	m_dwTotalPlayersAliveTime[2] += fread_u32(pFile);
	DbgOut("\tm_dwTotalPlayersAliveTime - %d, %d, %d\n", m_dwTotalPlayersAliveTime[0], m_dwTotalPlayersAliveTime[1], m_dwTotalPlayersAliveTime[2]);
	m_dwTotalPlayersMoneyRound[0] += fread_u32(pFile);
	m_dwTotalPlayersMoneyRound[1] += fread_u32(pFile);
	m_dwTotalPlayersMoneyRound[2] += fread_u32(pFile);
	DbgOut("\tm_dwTotalPlayersMoneyRound - %d, %d, %d\n", m_dwTotalPlayersMoneyRound[0], m_dwTotalPlayersMoneyRound[1], m_dwTotalPlayersMoneyRound[2]);
	m_dwTotalNumRespawns[0] += fread_u32(pFile);
	m_dwTotalNumRespawns[1] += fread_u32(pFile);
	m_dwTotalNumRespawns[2] += fread_u32(pFile);
	DbgOut("\tm_dwTotalNumRespawns - %d, %d, %d\n", m_dwTotalNumRespawns[0], m_dwTotalNumRespawns[1], m_dwTotalNumRespawns[2]);
	//----------------------------------------------
	u32 NumPlayers;
	fread(&NumPlayers, 4, 1, pFile);
	DbgOut("\tNumPlayers - %d\n", NumPlayers);
	//----------------------------------------------
	for (u32 i=0; i<NumPlayers; i++)
	{
		string1024 Name="";
		ReadName(Name, pFile);
		DbgOut("\t\tPlayer : %s\n", Name);

        Player_Statistic& PS = *(FindPlayer(Name));
		PS.Load(pFile);
		//-----------------------------------------------
		for (u32 w=0; w<PS.aWeaponStats.size(); w++)
		{
			Weapon_Statistic& WS = PS.aWeaponStats[w];
			USED_NAMES_it UW_it	= std::find(aWeapons.begin(), aWeapons.end(), WS.WName);
			if (UW_it == aWeapons.end() || !((*UW_it) == WS.WName))
			{
				aWeapons.push_back(Used_Name(WS.WName));
			}
		}
	}
}

void				Player_Statistic::Load			(FILE* pFile)
{
	if (!pFile) return;
	//-----------------------------------------------------
	TotalShots += fread_u32(pFile);	DbgOut("\t\t\t TotalShots - %d\n", TotalShots);
	m_dwTotalAliveTime[0] += fread_u32(pFile);
	m_dwTotalAliveTime[1] += fread_u32(pFile);
	m_dwTotalAliveTime[2] += fread_u32(pFile);
	DbgOut("\t\t\t m_dwTotalAliveTime - %d, %d, %d\n", m_dwTotalAliveTime[0], m_dwTotalAliveTime[1], m_dwTotalAliveTime[2]);
	m_dwTotalMoneyRound[0] += fread_u32(pFile);
	m_dwTotalMoneyRound[1] += fread_u32(pFile);
	m_dwTotalMoneyRound[2] += fread_u32(pFile);
	DbgOut("\t\t\t m_dwTotalMoneyRound - %d, %d, %d\n", m_dwTotalMoneyRound[0], m_dwTotalMoneyRound[1], m_dwTotalMoneyRound[2]);
	m_dwNumRespawned[0] += fread_u32(pFile);
	m_dwNumRespawned[1] += fread_u32(pFile);
	m_dwNumRespawned[2] += fread_u32(pFile);	
	DbgOut("\t\t\t m_dwNumRespawned - %d, %d, %d\n", m_dwNumRespawned[0], m_dwNumRespawned[1], m_dwNumRespawned[2]);
	//----------------------------------------------
	u32 NumWeapons = 0;
	fread(&NumWeapons, 4, 1, pFile); DbgOut("\t\t\t NumWeapons - %d\n", NumWeapons);
	for (u32 i=0; i<NumWeapons; i++)
	{
		string1024 Name="", IName = "";
		ReadName(Name, pFile);
		ReadName(IName, pFile);
		DbgOut("\t\t\t\t Weapon : %s\n", Name);

		Weapon_Statistic& WS = *(FindPlayersWeapon(Name, IName));
		WS.Load(pFile);
	};
}

void				Weapon_Statistic::Load			(FILE* pFile)
{
	if (!pFile) return;
	string1024 tmpBuf;
	fpos_t pos, pos1;
	fgetpos(pFile, &pos);
	fread(tmpBuf, 1024, 1, pFile);
	fsetpos(pFile, &pos);
	//-----------------------------------------------------
	u32 tmp = 0;
	NumBought += fread_u32(pFile);
	RoundsFired += fread_u32(pFile);
	BulletsFired += fread_u32(pFile);
	HitsScored += fread_u32(pFile);
	KillsScored += fread_u32(pFile);
	for (u32 a=0; a<3*MAX_BASKET; a++)
	{
		((u32*) m_Basket)[a] += fread_u32(pFile);
	}
	//----------------------------------------------
	u32 dwNumHits = 0;
	fread(&dwNumHits, 4, 1, pFile);		DbgOut("\t\t\t\t\t dwNumHits : %d\n", dwNumHits);  fgetpos(pFile, &pos1);
	for (u32 i=0; i<dwNumHits; i++)
	{
		HitData NewHit;
		NewHit.Load(pFile);
		m_Hits.push_back(NewHit);
	}
}

void				HitData::Load			(FILE* pFile)
{
	if (!pFile) return;
	//-----------------------------------------------------
	fread(&Pos0[0], 4, 1, pFile);
	fread(&Pos0[1], 4, 1, pFile);
	fread(&Pos0[2], 4, 1, pFile);

	fread(&Pos1[0], 4, 1, pFile);
	fread(&Pos1[1], 4, 1, pFile);
	fread(&Pos1[2], 4, 1, pFile);

	fread(&BoneID, 2, 1, pFile);
	fread(&Deadly, 1, 1, pFile);

	ReadName(TargetName, pFile);
	BoneName[0] = 0;
	if (g_FileVersion >= 2)
	{
		ReadName(BoneName, pFile);
	}
};
