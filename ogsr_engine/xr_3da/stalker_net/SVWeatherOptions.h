#pragma once
#include "afxwin.h"
#include "SubDlg.h"

// SVWeatherOptions dialog

class SVWeatherOptions : public CSubDlg
{
	DECLARE_DYNAMIC(SVWeatherOptions)

public:
	SVWeatherOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVWeatherOptions();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_WEATHEROPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_pStartWeather;
	CEdit m_pWeatherSpeedCoeff;

	void	AddWeather(const char* WeatherType, const char* WeatherTime);
};
