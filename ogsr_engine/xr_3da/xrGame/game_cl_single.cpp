#include "pch_script.h"
#include "game_cl_single.h"
#include "UIGameSP.h"
#include "clsid_game.h"
#include "actor.h"

using namespace luabind;

ESingleGameDifficulty g_SingleGameDifficulty = egdMaster;

xr_token	difficulty_type_token						[ ]={
	{ "gd_novice",						egdNovice									},
	{ "gd_stalker",						egdStalker									},
	{ "gd_veteran",						egdVeteran									},
	{ "gd_master",						egdMaster									},
	{ 0,							0											}
};

game_cl_Single::game_cl_Single()
{
}

CUIGameCustom* game_cl_Single::createGameUI()
{
	CLASS_ID clsid			= CLSID_GAME_UI_SINGLE;
	CUIGameSP*			pUIGame	= smart_cast<CUIGameSP*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(pUIGame);
	pUIGame->SetClGame(this);
	pUIGame->Init();
	return pUIGame;
}

char*	game_cl_Single::getTeamSection(int Team)
{
	return NULL;
};

void game_cl_Single::OnDifficultyChanged()
{
	Actor()->OnDifficultyChanged();
}

#pragma optimize("s",on)
void CScriptGameDifficulty::script_register(lua_State *L)
{
	module(L)
		[
			class_<enum_exporter<ESingleGameDifficulty> >("game_difficulty")
			.enum_("game_difficulty")
			[
				value("novice",				int(egdNovice			)),
				value("stalker",			int(egdStalker			)),
				value("veteran",			int(egdVeteran			)),
				value("master",				int(egdMaster			))
			]
		];
}