#include "stdafx.h"
#include "Statistic.h"

void				WeaponUsageStatistic::SaveFile		(char* FileName)
{
	if (!FileName) return;

	DbgOut("Writing \'%s\'\n", FileName);

	FILE* pFile = fopen(FileName,"wb");
	if (!pFile) return;

	Save(pFile);
	
	fclose(pFile);
};

void				WeaponUsageStatistic::Save			(FILE* pFile)
{
	if (!pFile) return;
	
	Write_Header				(pFile);
	Write_WeaponUsageFrequency	(pFile);
	Write_WeaponKills			(pFile);
	Write_WeaponEfficiency		(pFile);
	Write_WeaponAccuracy		(pFile);
	Write_EffectiveRange		(pFile);
	Write_BonesHits				(pFile);
	Write_BonesKills			(pFile);

	Write_AverageMoney			(pFile);
	Write_Basket				(pFile);
	Write_AverageLifeSpan		(pFile);
}

void				WeaponUsageStatistic::Write_Header				(FILE* pFile)
{
	if (!pFile) return;
	fprintf(pFile, "Использованные файлы :\n");
	for (u32 i=0; i<aUsedFiles.size(); i++)
	{
		Used_Name& UF = aUsedFiles[i];
		fprintf(pFile, "\t%s\n", UF.Name);
	}
	fprintf(pFile, "\n");
};

void				WeaponUsageStatistic::Write_WeaponUsageFrequency	(FILE* pFile)
{
	fprintf(pFile, "------------- Частота покупки оружия -------------\n\n");
	fprintf(pFile, "\tОбщая\t");
	//-----------------------------------------------------
	for (u32 i=0; i<aPlayersStatistic.size(); i++)
	{
		Player_Statistic& PS = aPlayersStatistic[i];
		fprintf(pFile, "%s\t", PS.PName);
	}
	fprintf(pFile, "\n");
	//-----------------------------------------------------
	for (i=0; i<aWeapons.size(); i++)
	{
		Used_Name& UN = aWeapons[i];
		u32 NumBoughtTotal = 0;
		string1024 tmpStr0 = "", tmpStr1 = "";
		strcpy(tmpStr0, UN.Name);
		for (u32 j=0; j<aPlayersStatistic.size(); j++)
		{
			u32 NumBoughtPlayer = 0;
			
			Player_Statistic& PS = aPlayersStatistic[j];
			WEAPON_STATS_it WS_it;
			if (PS.GetPlayersWeapon(UN.Name, WS_it))
			{
				Weapon_Statistic& WS = *WS_it;
				strcpy(tmpStr0, WS.InvName);
				NumBoughtPlayer = WS.NumBought;
			};

			NumBoughtTotal += NumBoughtPlayer;
			sprintf(tmpStr1, "%s%d\t", tmpStr1, NumBoughtPlayer);
		};
		fprintf(pFile, "%s\t%d\t%s\n", tmpStr0, NumBoughtTotal, tmpStr1);
	}
	//-----------------------------------------------------
	fprintf(pFile, "\n");
};

void				WeaponUsageStatistic::Write_WeaponKills		(FILE* pFile)
{
	fprintf(pFile, "------------- Количество убийств из оружия -------------\n\n");
	fprintf(pFile, "\tОбщее\t");
	//-----------------------------------------------------
	for (u32 i=0; i<aPlayersStatistic.size(); i++)
	{
		Player_Statistic& PS = aPlayersStatistic[i];
		fprintf(pFile, "%s\t", PS.PName);
	}
	fprintf(pFile, "\n");
	//-----------------------------------------------------
	for (i=0; i<aWeapons.size(); i++)
	{
		Used_Name& UN = aWeapons[i];
		if (!strstr(UN.Name, "wpn_")) continue;
		u32 NumKillsTotal = 0;
		string1024 tmpStr0 = "", tmpStr1 = "";
		strcpy(tmpStr0, UN.Name);
		for (u32 j=0; j<aPlayersStatistic.size(); j++)
		{
			u32 NumKillsPlayer = 0;

			Player_Statistic& PS = aPlayersStatistic[j];
			WEAPON_STATS_it WS_it;
			if (PS.GetPlayersWeapon(UN.Name, WS_it))
			{
				Weapon_Statistic& WS = *WS_it;
				strcpy(tmpStr0, WS.InvName);
				NumKillsPlayer = WS.KillsScored;
			};

			NumKillsTotal += NumKillsPlayer;
			sprintf(tmpStr1, "%s%d\t", tmpStr1, NumKillsPlayer);
		};
		fprintf(pFile, "%s\t%d\t%s\n", tmpStr0, NumKillsTotal, tmpStr1);
	}
	//-----------------------------------------------------
	fprintf(pFile, "\n");
}

void				WeaponUsageStatistic::Write_WeaponEfficiency		(FILE* pFile)
{
	fprintf(pFile, "------------- Эффективность оружия, %%  ( Убийства/Попадания )-------------\n\n");
	fprintf(pFile, "\tОбщая\t");
	//-----------------------------------------------------
	for (u32 i=0; i<aPlayersStatistic.size(); i++)
	{
		Player_Statistic& PS = aPlayersStatistic[i];
		fprintf(pFile, "%s\t", PS.PName);
	}
	fprintf(pFile, "\n");
	//-----------------------------------------------------
	for (i=0; i<aWeapons.size(); i++)
	{
		Used_Name& UN = aWeapons[i];
		if (!strstr(UN.Name, "wpn_")) continue;
		u32 NumKillsTotal = 0;
		u32 NumHitsTotal = 0;
		string1024 tmpStr0 = "", tmpStr1 = "";
		strcpy(tmpStr0, UN.Name);
		for (u32 j=0; j<aPlayersStatistic.size(); j++)
		{
			u32 NumKillsPlayer = 0;
			u32 NumHitsPlayer = 0;

			Player_Statistic& PS = aPlayersStatistic[j];
			WEAPON_STATS_it WS_it;
			if (PS.GetPlayersWeapon(UN.Name, WS_it))
			{
				Weapon_Statistic& WS = *WS_it;
				strcpy(tmpStr0, WS.InvName);
				NumKillsPlayer = WS.KillsScored;
				NumHitsPlayer = WS.HitsScored;
			};

			NumKillsTotal += NumKillsPlayer;
			NumHitsTotal += NumHitsPlayer;
//			sprintf(tmpStr1, "%s%d\t", tmpStr1, (NumHitsPlayer > 0) ? int(.5f + float(NumKillsPlayer)/NumHitsPlayer*100) : 0);
			sprintf(tmpStr1, "%s%.2f\t", tmpStr1, (NumHitsPlayer > 0) ? float(NumKillsPlayer)/NumHitsPlayer : 0);
		};
//		fprintf(pFile, "%s\t%d\t%s\n", tmpStr0, (NumHitsTotal>0) ?int(.5f + float(NumKillsTotal)/NumHitsTotal*100):0, tmpStr1);
		ConvOut(pFile, "%s\t%.2f\t%s\n", tmpStr0, (NumHitsTotal>0) ? float(NumKillsTotal)/NumHitsTotal : 0, tmpStr1);
	}
	//-----------------------------------------------------
	fprintf(pFile, "\n");
}

void				WeaponUsageStatistic::Write_WeaponAccuracy		(FILE* pFile)
{
	fprintf(pFile, "------------- Точность оружия, %% ( Попадания/Выстрелы )-------------\n\n");
	fprintf(pFile, "\tОбщая\t");
	//-----------------------------------------------------
	for (u32 i=0; i<aPlayersStatistic.size(); i++)
	{
		Player_Statistic& PS = aPlayersStatistic[i];
		fprintf(pFile, "%s\t", PS.PName);
	}
	fprintf(pFile, "\n");
	//-----------------------------------------------------
	for (i=0; i<aWeapons.size(); i++)
	{
		Used_Name& UN = aWeapons[i];
		if (!strstr(UN.Name, "wpn_")) continue;
		u32 NumBulletsFiredTotal = 0;
		u32 NumHitsScoredTotal = 0;
		string1024 tmpStr0 = "", tmpStr1 = "";
		strcpy(tmpStr0, UN.Name);
		for (u32 j=0; j<aPlayersStatistic.size(); j++)
		{
			u32 NumBulletsFired = 0;
			u32 NumHitsScored = 0;

			Player_Statistic& PS = aPlayersStatistic[j];
			WEAPON_STATS_it WS_it;
			if (PS.GetPlayersWeapon(UN.Name, WS_it))
			{
				Weapon_Statistic& WS = *WS_it;
				strcpy(tmpStr0, WS.InvName);
				NumBulletsFired = WS.BulletsFired;
				NumHitsScored = WS.HitsScored;
			};

			NumBulletsFiredTotal += NumBulletsFired;
			NumHitsScoredTotal += NumHitsScored;
//			sprintf(tmpStr1, "%s%d\t", tmpStr1, (NumBulletsFired > 0) ? int(.5f+float(NumHitsScored)/NumBulletsFired*100) : 0);
			sprintf(tmpStr1, "%s%.2f\t", tmpStr1, (NumBulletsFired > 0) ? (float(NumHitsScored)/NumBulletsFired) : 0);
		};
//		fprintf(pFile, "%s\t%d\t%s\n", tmpStr0, (NumBulletsFiredTotal > 0) ? int(.5f+float(NumHitsScoredTotal)/NumBulletsFiredTotal*100) : 0, tmpStr1);
		ConvOut(pFile, "%s\t%.2f\t%s\n", tmpStr0, (NumBulletsFiredTotal > 0) ? float(NumHitsScoredTotal)/NumBulletsFiredTotal : 0, tmpStr1);
	}
	//-----------------------------------------------------
	fprintf(pFile, "\n");
}

void				WeaponUsageStatistic::Write_EffectiveRange		(FILE* pFile)
{
	fprintf(pFile, "------------- Эффективность по дальности -------------\n\n");
	//-----------------------------------------------------
	float MaxRange = 0.0f;
	for (u32 i=0; i<aPlayersStatistic.size(); i++)
	{
		Player_Statistic& PS = aPlayersStatistic[i];
		for (u32 j=0; j<PS.aWeaponStats.size(); j++)
		{
			Weapon_Statistic& WS = PS.aWeaponStats[j];
			for (u32 k=0; k<WS.m_Hits.size(); k++)
			{
				HitData& HD = WS.m_Hits[k];
				float range = Magnitude(HD.Pos0, HD.Pos1);
				if (MaxRange < range)
					MaxRange = range;
			}
		}
	};

	MaxRange =  ceil(MaxRange);

	for (i=0; i<aWeapons.size(); i++)
	{
		Used_Name& UN = aWeapons[i];
		if (!strstr(UN.Name, "wpn_")) continue;

		u32 HitsOnRange[1024];
		ZeroMemory(HitsOnRange, sizeof(HitsOnRange));
		u32 HitsTotal = 0;

		string1024 tmpStr0 = "", tmpStr1 = "";
		strcpy(tmpStr0, UN.Name);
		for (u32 j=0; j<aPlayersStatistic.size(); j++)
		{
			Player_Statistic& PS = aPlayersStatistic[j];
			WEAPON_STATS_it WS_it;
			if (PS.GetPlayersWeapon(UN.Name, WS_it))
			{
				Weapon_Statistic& WS = *WS_it;
				strcpy(tmpStr0, WS.InvName);
				for (u32 k=0; k<WS.m_Hits.size(); k++)
				{
					HitData& HD = WS.m_Hits[k];
					float range = Magnitude(HD.Pos0, HD.Pos1);
					HitsOnRange[int(floor(range))]++;
					HitsTotal++;
				}
			};
		};
		fprintf(pFile, "%s\t", tmpStr0);
		for (int k=0; k<int(MaxRange); k++)
		{
			fprintf(pFile, "%d\t", HitsOnRange[k]);// (HitsTotal>0) ? int(.5f + float(HitsOnRange[k])/HitsTotal*100) : 0);
		}
		fprintf(pFile, "\n");
	}
	//-----------------------------------------------------
	fprintf(pFile, "\n");
}

void				WeaponUsageStatistic::Write_BonesHits		(FILE* pFile)
{
	fprintf(pFile, "------------- \tПопадания по костям\t -------------\n\n");
	//-----------------------------------------------------
	for (u32 i=0; i<aPlayersStatistic.size(); i++)
	{
		Player_Statistic& PS = aPlayersStatistic[i];
		for (u32 j=0; j<PS.aWeaponStats.size(); j++)
		{
			Weapon_Statistic& WS = PS.aWeaponStats[j];
			for (u32 k=0; k<WS.m_Hits.size(); k++)
			{
				HitData& HD = WS.m_Hits[k];
				if (HD.BoneName[0] == 0) continue;
				USED_NAMES_it UB_it	= std::find(aBones.begin(), aBones.end(), HD.BoneName);
				if (UB_it == aBones.end() || !((*UB_it) == HD.BoneName))
				{
					aBones.push_back(Used_Name(HD.BoneName));
				}
			}
		}
	};

	if (aBones.empty()) return;

	fprintf(pFile, "\tTotal");
	for (i=0; i<aBones.size(); i++)
	{
		Used_Name& UB = aBones[i];
		
		int xLen = int(strlen("bip01_"));
		if (!strstr(UB.Name, "bip01_")) xLen = 0;

		fprintf(pFile, "\t%s", UB.Name+xLen);
	};
	fprintf(pFile, "\n");
	//-----------------------------------------------
	u32 HitsOnBone[1024];
	ZeroMemory(HitsOnBone, sizeof(HitsOnBone));
	u32 TotalHits = 0;
	for (i=0; i<aWeapons.size(); i++)
	{
		Used_Name& UN = aWeapons[i];
		if (!strstr(UN.Name, "wpn_")) continue;

		u32 HitsOnBoneW[1024];
		ZeroMemory(HitsOnBoneW, sizeof(HitsOnBoneW));
		u32 TotalHitsW = 0;

		string1024 tmpStr0 = "", tmpStr1 = "";
		strcpy(tmpStr0, UN.Name);
		for (u32 j=0; j<aPlayersStatistic.size(); j++)
		{
			Player_Statistic& PS = aPlayersStatistic[j];
			WEAPON_STATS_it WS_it;
			if (PS.GetPlayersWeapon(UN.Name, WS_it))
			{
				Weapon_Statistic& WS = *WS_it;
				strcpy(tmpStr0, WS.InvName);
				for (u32 k=0; k<WS.m_Hits.size(); k++)
				{
					HitData& HD = WS.m_Hits[k];
					//-------------------------------------------
					USED_NAMES_it UB_it	= std::find(aBones.begin(), aBones.end(), HD.BoneName);
					HitsOnBoneW[UB_it - aBones.begin()]++;
					HitsOnBone[UB_it - aBones.begin()]++;
					TotalHits++;
					TotalHitsW++;
					//-------------------------------------------
				}
			};
		};
		fprintf(pFile, "%s\t", tmpStr0);
		ConvOut(pFile, "%d\t", TotalHitsW);
		for (u32 k=0; k<aBones.size(); k++)
		{
//			fprintf(pFile, "%d\t", (TotalHits > 0) ? int(float(HitsOnBoneW[k])/TotalHits*100+0.5f) : 0);
//			ConvOut(pFile, "%.2f\t", (TotalHits > 0) ? float(HitsOnBoneW[k])/TotalHits : 0);
			ConvOut(pFile, "%d\t", HitsOnBoneW[k]);
		}
		fprintf(pFile, "\n");
	}
	//-----------------------------------------------------
	fprintf(pFile, "Total\t%d\t", TotalHits);
	for (u32 k=0; k<aBones.size(); k++)
	{
		ConvOut(pFile, "%d\t", HitsOnBone[k]);
	}
	fprintf(pFile, "\n");
	//-----------------------------------------------------
	fprintf(pFile, "\n");
}
void				WeaponUsageStatistic::Write_BonesKills			(FILE* pFile)
{
	fprintf(pFile, "------------- \tСмертельные попадания по костям\t -------------\n\n");
	//-----------------------------------------------------
	aBones.clear();	
	for (u32 i=0; i<aPlayersStatistic.size(); i++)
	{
		Player_Statistic& PS = aPlayersStatistic[i];
		for (u32 j=0; j<PS.aWeaponStats.size(); j++)
		{
			Weapon_Statistic& WS = PS.aWeaponStats[j];
			for (u32 k=0; k<WS.m_Hits.size(); k++)
			{
				HitData& HD = WS.m_Hits[k];
				if (HD.BoneName[0] == 0) continue;
//				if (!HD.Deadly) continue;
				USED_NAMES_it UB_it	= std::find(aBones.begin(), aBones.end(), HD.BoneName);
				if (UB_it == aBones.end() || !((*UB_it) == HD.BoneName))
				{
					aBones.push_back(Used_Name(HD.BoneName));
				}
			}
		}
	};

	if (aBones.empty()) return;

	fprintf(pFile, "\tTotal");
	for (i=0; i<aBones.size(); i++)
	{
		Used_Name& UB = aBones[i];
		
		int xLen = int(strlen("bip01_"));
		if (!strstr(UB.Name, "bip01_")) xLen = 0;

		fprintf(pFile, "\t%s", UB.Name+xLen);
	};
	fprintf(pFile, "\n");

	u32 HitsOnBone[1024];
	ZeroMemory(HitsOnBone, sizeof(HitsOnBone));
	u32 TotalHits = 0;
	for (i=0; i<aWeapons.size(); i++)
	{
		Used_Name& UN = aWeapons[i];
		if (!strstr(UN.Name, "wpn_")) continue;

		u32 HitsOnBoneW[1024];
		ZeroMemory(HitsOnBoneW, sizeof(HitsOnBoneW));
		u32 TotalHitsW = 0;

		string1024 tmpStr0 = "", tmpStr1 = "";
		strcpy(tmpStr0, UN.Name);
		for (u32 j=0; j<aPlayersStatistic.size(); j++)
		{
			Player_Statistic& PS = aPlayersStatistic[j];
			WEAPON_STATS_it WS_it;
			if (PS.GetPlayersWeapon(UN.Name, WS_it))
			{
				Weapon_Statistic& WS = *WS_it;
				strcpy(tmpStr0, WS.InvName);
				for (u32 k=0; k<WS.m_Hits.size(); k++)
				{
					HitData& HD = WS.m_Hits[k];
					if (!HD.Deadly) continue;
					//-------------------------------------------
					USED_NAMES_it UB_it	= std::find(aBones.begin(), aBones.end(), HD.BoneName);
					HitsOnBone[UB_it - aBones.begin()]++;
					TotalHits++;
					HitsOnBoneW[UB_it - aBones.begin()]++;
					TotalHitsW++;
					//-------------------------------------------
				}
			};
		};
		fprintf(pFile, "%s\t", tmpStr0);
		fprintf(pFile, "%d\t", TotalHitsW);
		for (u32 k=0; k<aBones.size(); k++)
		{
//			fprintf(pFile, "%d\t", (TotalHits > 0) ? int(float(HitsOnBone[k])/TotalHits*100+0.5f) : 0);
//			ConvOut(pFile, "%.2f\t", (TotalHits > 0) ? float(HitsOnBone[k])/TotalHits : 0);
			ConvOut(pFile, "%d\t", HitsOnBoneW[k]);
		}
		fprintf(pFile, "\n");
	}
	fprintf(pFile, "Total\t%d\t", TotalHits);
	for (u32 k=0; k<aBones.size(); k++)
	{
		ConvOut(pFile, "%d\t", HitsOnBone[k]);
	}
	fprintf(pFile, "\n");
	//-----------------------------------------------------
	fprintf(pFile, "\n");
}
void				WeaponUsageStatistic::Write_AverageMoney		(FILE* pFile)
{
	fprintf(pFile, "------------- Среднее количество заработанных денег, (Заработанные деньги/кол-во респавнов) -------------\n\n");
	//-----------------------------------------------------------------------
	fprintf(pFile, "\tОбщее\t");
	//-----------------------------------------------------
	for (u32 i=0; i<aPlayersStatistic.size(); i++)
	{
		Player_Statistic& PS = aPlayersStatistic[i];
		fprintf(pFile, "%s\t", PS.PName);
	}
	fprintf(pFile, "\n");

	for (int t=0; t<3; t++)
	{
		switch (t)
		{
		case 0: fprintf(pFile, "DM Team\t"); break;
		case 1: fprintf(pFile, "Green Team\t"); break;
		case 2: fprintf(pFile, "Blue Team\t"); break;
		};

		ConvOut(pFile, "%.2f\t", (m_dwTotalNumRespawns[t]>0) ? (float(m_dwTotalPlayersMoneyRound[t])/m_dwTotalNumRespawns[t]) : 0);

		for (u32 i=0; i<aPlayersStatistic.size(); i++)
		{
			Player_Statistic& PS = aPlayersStatistic[i];
			ConvOut(pFile, "%.2f\t", (PS.m_dwNumRespawned[t]>0) ? (float(PS.m_dwTotalMoneyRound[t])/PS.m_dwNumRespawned[t]) : 0);
		}

		fprintf(pFile, "\n");
	}
	fprintf(pFile, "\n");
}
void				WeaponUsageStatistic::Write_Basket		(FILE* pFile)
{
	fprintf(pFile, "------------- Покупаемость предметов -------------\n\n");
	//-----------------------------------------------------
	for (int t=0; t<3; t++)
	{
		switch (t)
		{
		case 0: fprintf(pFile, "DM Team\n"); break;
		case 1: fprintf(pFile, "Green Team\n"); break;
		case 2: fprintf(pFile, "Blue Team\n"); break;
		};

		int MaxBasketPos = 0;
		for (u32 i=0; i<aPlayersStatistic.size(); i++)
		{
			Player_Statistic& PS = aPlayersStatistic[i];
			for (u32 j=0; j<PS.aWeaponStats.size(); j++)
			{
				Weapon_Statistic& WS = PS.aWeaponStats[j];
				for (int k=0; k<MAX_BASKET; k++)
				{
					if (WS.m_Basket[t][k]>0 && MaxBasketPos<k) MaxBasketPos = k;
				}
			}
		};	

		for (int k=0; k<MaxBasketPos; k++)
		{
			fprintf(pFile, "\t%d", (k==0) ? 500 : k*1000);
		}
		fprintf(pFile, "\n");

		for (i=0; i<aWeapons.size(); i++)
		{
			Used_Name& UN = aWeapons[i];		

			u32 TotalBought = 0;

			string1024 tmpStr0 = "", tmpStr1 = "";
			strcpy(tmpStr0, UN.Name);

			for (int k=0; k<MaxBasketPos; k++)
			{
				u32 BoughtOnPos = 0;
				for (u32 j=0; j<aPlayersStatistic.size(); j++)
				{
					Player_Statistic& PS = aPlayersStatistic[j];
					WEAPON_STATS_it WS_it;
					if (PS.GetPlayersWeapon(UN.Name, WS_it))
					{
						Weapon_Statistic& WS = *WS_it;
						strcpy(tmpStr0, WS.InvName);
						TotalBought += WS.m_Basket[t][k];
						BoughtOnPos += WS.m_Basket[t][k];
					};
				};
				sprintf(tmpStr1, "%s%d\t", tmpStr1, BoughtOnPos);
			};
			if (TotalBought>0)
				fprintf(pFile, "%s\t%s\n", tmpStr0, tmpStr1);
		}
		//-----------------------------------------------------
		fprintf(pFile, "\n");
	}
}
void				WeaponUsageStatistic::Write_AverageLifeSpan		(FILE* pFile)
{
	fprintf(pFile, "------------- Среднее время жизни игрока, сек. -------------\n\n");
	//-----------------------------------------------------------------------
	fprintf(pFile, "\tОбщее\t");
	//-----------------------------------------------------
	for (u32 i=0; i<aPlayersStatistic.size(); i++)
	{
		Player_Statistic& PS = aPlayersStatistic[i];
		fprintf(pFile, "%s\t", PS.PName);
	}
	fprintf(pFile, "\n");
	
	for (int t=0; t<3; t++)
	{
		switch (t)
		{
		case 0: fprintf(pFile, "DM Team\t"); break;
		case 1: fprintf(pFile, "Green Team\t"); break;
		case 2: fprintf(pFile, "Blue Team\t"); break;
		};
		
		ConvOut(pFile, "%.2f\t", (m_dwTotalNumRespawns[t]>0) ? (float(m_dwTotalPlayersAliveTime[t])/m_dwTotalNumRespawns[t]/1000) : 0);

		for (u32 i=0; i<aPlayersStatistic.size(); i++)
		{
			Player_Statistic& PS = aPlayersStatistic[i];
			ConvOut(pFile, "%.2f\t", (PS.m_dwNumRespawned[t]>0) ? (float(PS.m_dwTotalAliveTime[t])/PS.m_dwNumRespawned[t]/1000) : 0);
		}

		fprintf(pFile, "\n");
	}
}