#include "StdAfx.h"
#include "TeamInfo.h"
#include "../string_table.h"

u32			CTeamInfo::team1_color;
u32			CTeamInfo::team2_color;
shared_str	CTeamInfo::team1_name;
shared_str	CTeamInfo::team2_name;
shared_str	CTeamInfo::team1_color_tag;
shared_str	CTeamInfo::team2_color_tag;

Flags32		CTeamInfo::flags;

u32 CTeamInfo::GetTeam1_color(){
	if (flags.test(flTeam1_color))
		return team1_color;

	string256 _buff;

	LPCSTR tm_col = pSettings->r_string("team1","color");
	team1_color = color_argb(155,atoi(_GetItem(tm_col, 0, _buff)),atoi(_GetItem(tm_col, 1, _buff)),atoi(_GetItem(tm_col, 2, _buff)));
	flags.set(flTeam1_color, true);

	return team1_color;
}

u32 CTeamInfo::GetTeam2_color(){
	if (flags.test(flTeam2_color))
		return team2_color;

	string256 _buff;

	LPCSTR tm_col = pSettings->r_string("team2","color");
	team2_color = color_argb(155,atoi(_GetItem(tm_col, 0, _buff)),atoi(_GetItem(tm_col, 1, _buff)),atoi(_GetItem(tm_col, 2, _buff)));
	flags.set(flTeam2_color, true);

	return team2_color;
}

shared_str	CTeamInfo::GetTeam1_name(){
	if (flags.test(flTeam1_name))
		return team1_name;

	CStringTable st;

	team1_name = st.translate(pSettings->r_string_wb("team1","name"));
	flags.set(flTeam1_name,true);

    return team1_name;
}

shared_str	CTeamInfo::GetTeam2_name(){
	if (flags.test(flTeam2_name))
		return team2_name;

	CStringTable st;

	team2_name = st.translate(pSettings->r_string_wb("team2","name"));
	flags.set(flTeam2_name,true);

    return team2_name;
}

LPCSTR	CTeamInfo::GetTeam_name(int team){
	R_ASSERT(1 == team || 2 == team);
    if (1 == team)
		return *GetTeam1_name();
	else 
		return *GetTeam2_name();
}

LPCSTR	CTeamInfo::GetTeam_color_tag(int team){
	R_ASSERT(1 == team || 2 == team);
//	if (flags.test(flTeam1_col_t)) return *team1_color_tag;
//	if (flags.test(flTeam2_col_t)) return *team2_color_tag;

	string256 _buff;

	//"%c[255,64,255,64]", "%c[255,64,64,255]"

	LPCSTR tm_col;
	if (1 == team)
		tm_col = pSettings->r_string("team1","color");
	else
		tm_col = pSettings->r_string("team2","color");

	//team1_color = color_argb(155,atoi(_GetItem(tm_col, 0, _buff)),atoi(_GetItem(tm_col, 1, _buff)),atoi(_GetItem(tm_col, 2, _buff)));
	xr_string str;
	str = "%c[255,";
	str += _GetItem(tm_col, 0, _buff);
	str += ",";
	str += _GetItem(tm_col, 1, _buff);
	str += ",";
	str += _GetItem(tm_col, 2, _buff);
	str += "]";

	if (1 == team){
		flags.set(flTeam1_col_t,true);
        team1_color_tag = str.c_str();
        return *team1_color_tag;
	}
	else{
		flags.set(flTeam2_col_t,true);
		team2_color_tag = str.c_str();
        return *team2_color_tag;
	}
	

}