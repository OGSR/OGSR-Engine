#pragma once
#include "afxwin.h"


// CPortsDlg dialog

class CPortsDlg : public CDialog
{
	DECLARE_DYNAMIC(CPortsDlg)

public:
	CPortsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPortsDlg();

// Dialog Data
	enum { IDD = IDD_PORTSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	void	FillPortsList();
	int*	m_pRes;
	int		m_iNumPorts;
	int*	m_pUsedPorts;
public:
	virtual INT_PTR DoModal( int NumPorts, int* pUsedPorts, int* pRes );
	CListBox m_pPortsList;
	afx_msg void OnLbnDblclkPorts();
	afx_msg void OnLbnSelchangePorts();
};
