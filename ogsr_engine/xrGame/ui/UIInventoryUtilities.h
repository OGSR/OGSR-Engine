#pragma once

#include "../inventory_item.h"
#include "../character_info_defs.h"

class CUIStatic;

//размеры сетки в текстуре инвентаря
#define INV_GRID_WIDTH			50
#define INV_GRID_HEIGHT			50

//размеры сетки в текстуре иконок персонажей
#define ICON_GRID_WIDTH			64
#define ICON_GRID_HEIGHT		64
//размер иконки персонажа для инвенторя и торговли
#define CHAR_ICON_WIDTH			2
#define CHAR_ICON_HEIGHT		2	

//размер иконки персонажа в полный рост
#define CHAR_ICON_FULL_WIDTH	2
#define CHAR_ICON_FULL_HEIGHT	5

#define TRADE_ICONS_SCALE		(4.f/5.f)

namespace InventoryUtilities
{

//сравнивает элементы по пространству занимаемому ими в рюкзаке
//для сортировки
bool GreaterRoomInRuck	(PIItem item1, PIItem item2);
//для проверки свободного места
bool FreeRoom_inBelt	(TIItemContainer& item_list, PIItem item, int width, int height);


// get shader for BuyWeaponWnd
ref_shader&	GetBuyMenuShader();
//получить shader на иконки инвенторя
ref_shader& GetEquipmentIconsShader();
// shader на иконки персонажей в мультиплеере
ref_shader&	GetMPCharIconsShader();
//удаляем все шейдеры
void DestroyShaders();
void CreateShaders();

// Получить значение времени в текстовом виде

// Точность возвращаемого функцией GetGameDateTimeAsString значения: до часов, до минут, до секунд
enum ETimePrecision
{
	etpTimeToHours = 0,
	etpTimeToMinutes,
	etpTimeToSeconds,
	etpTimeToMilisecs,
	etpTimeToSecondsAndDay
};

// Точность возвращаемого функцией GetGameDateTimeAsString значения: до года, до месяца, до дня
enum EDatePrecision
{
	edpDateToDay,
	edpDateToMonth,
	edpDateToYear
};

const shared_str GetGameDateAsString(EDatePrecision datePrec, char dateSeparator = '/');
const shared_str GetGameTimeAsString(ETimePrecision timePrec, char timeSeparator = ':');
const shared_str GetDateAsString(ALife::_TIME_ID time, EDatePrecision datePrec, char dateSeparator = '/');
const shared_str GetTimeAsString(ALife::_TIME_ID time, ETimePrecision timePrec, char timeSeparator = ':');
LPCSTR GetTimePeriodAsString	(LPSTR _buff, u32 buff_sz, ALife::_TIME_ID _from, ALife::_TIME_ID _to);
// Отобразить вес, который несет актер
void UpdateWeight(CUIStatic &wnd, bool withPrefix = false);

// Функции получения строки-идентификатора ранга и отношения по их числовому идентификатору
LPCSTR	GetRankAsText				(CHARACTER_RANK_VALUE		rankID);
LPCSTR	GetReputationAsText			(CHARACTER_REPUTATION_VALUE rankID);
LPCSTR	GetGoodwillAsText			(CHARACTER_GOODWILL			goodwill);

void	ClearCharacterInfoStrings	();

void	SendInfoToActor				(LPCSTR info_id);
u32		GetGoodwillColor			(CHARACTER_GOODWILL gw);
u32		GetRelationColor			(ALife::ERelationType r);
u32		GetReputationColor			(CHARACTER_REPUTATION_VALUE rv);
};