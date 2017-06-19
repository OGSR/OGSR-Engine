#include "StdAfx.h"
#include "UIMapList.h"
#include "UIListBox.h"
#include "UILabel.h"
#include "UIFrameWindow.h"
#include "UI3tButton.h"
#include "UISpinText.h"
#include "UIXmlInit.h"
#include "UIMapInfo.h"
#include "UIComboBox.h"
#include "UIListBoxItem.h"
#include "../../xr_ioconsole.h"
#include "../string_table.h"
#include "CExtraContentFilter.h"

#include "../object_broker.h"

extern ENGINE_API string512  g_sLaunchOnExit_app;
extern ENGINE_API string512  g_sLaunchOnExit_params;

xr_token	game_types		[];

CUIMapList::CUIMapList(){
	m_item2del = -1;

	m_pList1		= xr_new<CUIListBox>();
	m_pList2		= xr_new<CUIListBox>();
	m_pFrame1		= xr_new<CUIFrameWindow>();
	m_pFrame2		= xr_new<CUIFrameWindow>();
	m_pLbl1			= xr_new<CUILabel>();
	m_pLbl2			= xr_new<CUILabel>();
	m_pBtnLeft		= xr_new<CUI3tButton>();
	m_pBtnRight		= xr_new<CUI3tButton>();
	m_pBtnUp		= xr_new<CUI3tButton>();
	m_pBtnDown		= xr_new<CUI3tButton>();

	m_pExtraContentFilter = xr_new<CExtraContentFilter>();

	m_pList1->SetAutoDelete(true);
	m_pList2->SetAutoDelete(true);
	m_pFrame1->SetAutoDelete(true);
	m_pFrame2->SetAutoDelete(true);
	m_pLbl1->SetAutoDelete(true);
	m_pLbl2->SetAutoDelete(true);
	m_pBtnLeft->SetAutoDelete(true);
	m_pBtnRight->SetAutoDelete(true);
	m_pBtnUp->SetAutoDelete(true);
	m_pBtnDown->SetAutoDelete(true);

	AttachChild(m_pLbl1);
	AttachChild(m_pLbl2);
	AttachChild(m_pFrame1);
	AttachChild(m_pFrame2);
	AttachChild(m_pList1);
	AttachChild(m_pList2);
	AttachChild(m_pBtnLeft);
	AttachChild(m_pBtnRight);
	AttachChild(m_pBtnUp);
	AttachChild(m_pBtnDown);
}

CUIMapList::~CUIMapList(){
	delete_data(m_pExtraContentFilter);
}

void CUIMapList::StartDedicatedServer(){
	strcpy					(g_sLaunchOnExit_app,"dedicated\\xr_3da.exe");

	strcpy					(g_sLaunchOnExit_params,"-i -nosound -");
	strcat					(g_sLaunchOnExit_params,GetCommandLine(""));
	Msg						("%s","-- Going to quit before starting dedicated server");
	Msg						("%s %s",g_sLaunchOnExit_app, g_sLaunchOnExit_params);
	Console->Execute		("quit");
}

void CUIMapList::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);	
}

void CUIMapList::SendMessage(CUIWindow* pWnd, s16 msg, void* pData ){
	if (BUTTON_CLICKED == msg)
	{
		if (m_pBtnLeft == pWnd)
			OnBtnLeftClick();
		else if (m_pBtnRight == pWnd)
			OnBtnRightClick();
		else if (m_pBtnUp == pWnd)
			OnBtnUpClick();
		else if (m_pBtnDown == pWnd)
			OnBtnDownClick();
		else if (m_pModeSelector == pWnd)
			OnModeChange();
	}
	else if (WINDOW_LBUTTON_DB_CLICK == msg)
	{
		if (m_pList1 ==pWnd)
			OnBtnRightClick();
		else if (m_pList2 ==pWnd)
			OnBtnLeftClick();
	}
	else if (LIST_ITEM_CLICKED == msg)
	{
		if (pWnd == m_pList1)
            OnListItemClicked();
	}

		
}

void CUIMapList::OnListItemClicked()
{
	xr_string map_name = "intro\\intro_map_pic_";
	
	CUIListBoxItem* itm				= m_pList1->GetSelectedItem();
	u32 _idx						= (u32)(__int64)(itm->GetData());
	const shared_str& _map_name		= GetMapNameInt(GetCurGameType(), _idx);

	map_name						+=	_map_name.c_str();
	xr_string full_name				= map_name + ".dds";

	if (FS.exist("$game_textures$",full_name.c_str()))
		m_pMapPic->InitTexture(map_name.c_str());
	else
		m_pMapPic->InitTexture("ui\\ui_noise");

	m_pMapInfo->InitMap(_map_name.c_str());
}

xr_token g_GameModes[];

void CUIMapList::OnModeChange()
{
	UpdateMapList(GetCurGameType());
}

const char* CUIMapList::GetCLGameModeName(){
	return get_token_name(game_types, GetCurGameType() );
}

EGameTypes CUIMapList::GetCurGameType()
{
	LPCSTR text = m_pModeSelector->GetTokenText();

	if (0 == xr_strcmp(text, get_token_name(g_GameModes,GAME_DEATHMATCH)) )
		return	GAME_DEATHMATCH;
	else if (0 == xr_strcmp(text, get_token_name(g_GameModes,GAME_TEAMDEATHMATCH)) )
		return	GAME_TEAMDEATHMATCH;
	else if (0 == xr_strcmp(text, get_token_name(g_GameModes,GAME_ARTEFACTHUNT)) )
		return	GAME_ARTEFACTHUNT;
	else
		NODEFAULT;

#ifdef DEBUG
	return GAME_ANY;
#endif
}

const char* CUIMapList::GetCommandLine(LPCSTR player_name){
	string16		buf;

	CUIListBoxItem* itm				= m_pList2->GetItemByIDX(0);
	if (!itm)	
		return						NULL;

	u32 _idx						= (u32)(__int64)(itm->GetData());
	const shared_str& _map_name		= GetMapNameInt	(GetCurGameType(), _idx);

	m_command.clear();
	m_command = "start server(";
	m_command += _map_name.c_str();
	m_command += "/";
	m_command += GetCLGameModeName();
	m_command += m_srv_params;
	m_command += "/estime=";
	
	u32 id		= m_pWeatherSelector->GetListWnd()->GetSelectedItem()->GetTAG();

	int estime  = m_mapWeather[id].weather_time;
	m_command	+= itoa(estime/60,buf,10);
	m_command	+= ":";
	m_command	+= itoa(estime%60,buf,10);
	m_command	+= ")";


	m_command +=" client(localhost/name=";
	if (player_name == NULL || 0 == xr_strlen(player_name))
		m_command += Core.UserName;
	else
		m_command +=player_name;
	m_command +=")";

    return m_command.c_str();
}
#include "../UIGameCustom.h"
void CUIMapList::LoadMapList()
{

	GAME_WEATHERS game_weathers = gMapListHelper.GetGameWeathers();
	GAME_WEATHERS_CIT it		= game_weathers.begin();
	GAME_WEATHERS_CIT it_e		= game_weathers.end();
	
	u32 cnt=0;
	for( ;it!=it_e; ++it, ++cnt)
	{
		AddWeather			( (*it).m_weather_name, (*it).m_start_time, cnt);
	}
	if( game_weathers.size() )
		m_pWeatherSelector->SetItem(0);
}

void	CUIMapList::SaveMapList()
{
	string_path					temp;
	FS.update_path				(temp,"$app_data_root$", MAP_ROTATION_LIST);

	if(m_pList2->GetSize()<=1){
		FS.file_delete(temp);
		return;
	}

	IWriter*	pW = FS.w_open	(temp);
	if (!pW){
		Msg("! Cant create map rotation file [%s]", temp);
		return;
	}
	
	string_path					map_name;
	for(u32 idx=0; idx<m_pList2->GetSize(); ++idx)
	{
		CUIListBoxItem* itm				= m_pList2->GetItemByIDX(idx);
		u32 _idx						= (u32)(__int64)(itm->GetData());
		const shared_str& _map_name		= GetMapNameInt(GetCurGameType(), _idx);

		sprintf_s							(map_name, "sv_addmap %s", _map_name.c_str() );

		pW->w_string					(map_name);
	}

	FS.w_close							(pW);
}

void CUIMapList::SetWeatherSelector(CUIComboBox* ws){
	m_pWeatherSelector = ws;
}

void CUIMapList::SetModeSelector(CUISpinText* ms){
	m_pModeSelector = ms;
}

void CUIMapList::SetMapPic(CUIStatic* map_pic){
	m_pMapPic = map_pic;
}

void CUIMapList::SetMapInfo(CUIMapInfo* map_info){
	m_pMapInfo = map_info;	
}

void CUIMapList::SetServerParams(LPCSTR params){
	m_srv_params = params;
}

#include "uilistboxitem.h"
void CUIMapList::AddWeather(const shared_str& WeatherType, const shared_str& WeatherTime, u32 _id)
{
	R_ASSERT2					(m_pWeatherSelector, "m_pWeatherSelector == NULL");
	m_pWeatherSelector->AddItem_	(*WeatherType, 0)->SetTAG(_id);

	int	w_time;
	int hour = 0, min = 0;

	sscanf(*WeatherTime, "%d:%d", &hour, &min);
	w_time = hour*60+min;
	
	m_mapWeather.resize(m_mapWeather.size()+1);
	m_mapWeather.back().weather_name = WeatherType;
	m_mapWeather.back().weather_time = w_time;
}

void CUIMapList::InitFromXml(CUIXml& xml_doc, const char* path){
	CUIXmlInit::InitWindow(xml_doc, path, 0, this);
	string256 buf;
	CUIXmlInit::InitLabel		(xml_doc, strconcat(sizeof(buf),buf, path, ":header_1"),	0, m_pLbl1);
	CUIXmlInit::InitLabel		(xml_doc, strconcat(sizeof(buf),buf, path, ":header_2"),	0, m_pLbl2);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(sizeof(buf),buf, path, ":frame_1"),		0, m_pFrame1);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(sizeof(buf),buf, path, ":frame_2"),		0, m_pFrame2);
	CUIXmlInit::InitListBox		(xml_doc, strconcat(sizeof(buf),buf, path, ":list_1"),		0, m_pList1);
	CUIXmlInit::InitListBox		(xml_doc, strconcat(sizeof(buf),buf, path, ":list_2"),		0, m_pList2);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(sizeof(buf),buf, path, ":btn_left"),	0, m_pBtnLeft);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(sizeof(buf),buf, path, ":btn_right"),	0, m_pBtnRight);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(sizeof(buf),buf, path, ":btn_up"),		0, m_pBtnUp);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(sizeof(buf),buf, path, ":btn_down"),	0, m_pBtnDown);
}

void CUIMapList::UpdateMapList(EGameTypes GameType)
{
	m_pList1->Clear				();
	m_pList2->Clear				();

	const SGameTypeMaps& M		= gMapListHelper.GetMapListFor(GameType);
	u32 cnt						= M.m_map_names.size();
	for (u32 i=0; i<cnt; ++i)
	{
		CUIListBoxItem* itm		= m_pList1->AddItem( CStringTable().translate(M.m_map_names[i]).c_str() );
		itm->SetData			( (void*)(__int64)i );
		itm->Enable(m_pExtraContentFilter->IsDataEnabled(M.m_map_names[i].c_str()));
	}
}

void CUIMapList::OnBtnLeftClick(){
	m_pList2->RemoveWindow(m_pList2->GetSelected());
}

void CUIMapList::Update(){
	CUIWindow::Update();
}

void CUIMapList::OnBtnRightClick()
{
	CUIListBoxItem* itm1			= m_pList1->GetSelectedItem();
	if (!itm1) return;
	CUIListBoxItem* itm2			= m_pList2->AddItem( itm1->GetText() );
	itm2->SetData					(itm1->GetData());
}

void CUIMapList::OnBtnUpClick(){
	m_pList2->MoveSelectedUp();
}

void CUIMapList::OnBtnDownClick(){
	m_pList2->MoveSelectedDown();
}

bool CUIMapList::IsEmpty(){
	return 0 == m_pList2->GetSize();
}

const shared_str& CUIMapList::GetMapNameInt(EGameTypes _type, u32 idx)
{
	const SGameTypeMaps& M		= gMapListHelper.GetMapListFor(_type);
	R_ASSERT					(M.m_map_names.size()>idx);
	return						M.m_map_names[idx];
}
