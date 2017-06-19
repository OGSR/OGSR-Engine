// SVWeatherOptions.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "SVWeatherOptions.h"
#include "xrstring.h"


// SVWeatherOptions dialog

IMPLEMENT_DYNAMIC(SVWeatherOptions, CSubDlg)
SVWeatherOptions::SVWeatherOptions(CWnd* pParent /*=NULL*/)
	: CSubDlg(SVWeatherOptions::IDD, pParent)
{
}

SVWeatherOptions::~SVWeatherOptions()
{
}

void SVWeatherOptions::DoDataExchange(CDataExchange* pDX)
{
	CSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STARTWEATHER, m_pStartWeather);
	DDX_Control(pDX, IDC_WEATHERSPEED, m_pWeatherSpeedCoeff);
}


BEGIN_MESSAGE_MAP(SVWeatherOptions, CSubDlg)
END_MESSAGE_MAP()


// SVWeatherOptions message handlers
BOOL SVWeatherOptions::OnInitDialog()
{
	CSubDlg::OnInitDialog();

	//-----------------------------------
	m_pWeatherSpeedCoeff.SetWindowText("1.0");
	//-----------------------------------
	return TRUE;
};

void	SVWeatherOptions::AddWeather(const char* WeatherType, const char* WeatherTime)
{
	if (!WeatherType || !WeatherTime) return;
	if (!WeatherType[0] || !WeatherTime[0]) return;

	int ID = m_pStartWeather.GetCount();
	m_pStartWeather.AddString(WeatherType);
	int hour = 0, min = 0;
	sscanf(WeatherTime, "%d:%d", &hour, &min);
	DWORD Time = hour*60+min;
	m_pStartWeather.SetItemData(ID, (DWORD_PTR) Time);

	m_pStartWeather.SetCurSel(0);
}