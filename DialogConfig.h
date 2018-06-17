#pragma once
#include "afxwin.h"


// CDialogConfig dialog

class CDialogConfig : public CDialog
{
	DECLARE_DYNAMIC(CDialogConfig)

public:
	CDialogConfig(CMainFrame* pMainFrame);   // standard constructor
	virtual ~CDialogConfig();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFIG };

protected:
	virtual	void	DoDataExchange(CDataExchange* pDX);		// DDX/DDV support
	BOOL			PreTranslateMessage(MSG* pMsg);			// Tooltip support

	DECLARE_MESSAGE_MAP()

private:
	// Control object variables
	CButton		m_cLEDType1;
	CButton		m_cLEDType2;
	CStatic		m_cUpdateFrequencyText1;
	CStatic		m_cUpdateFrequencyText2;
	CEdit		m_cUpdateFrequencyEdit;
	CEdit		m_c100Percent;
	CEdit		m_cCounterPath;
	CButton		m_cUpdateINI;
	CButton		m_cDefaultValues;
	CComboBox	m_cSelectPerfObjects;
	CComboBox	m_cSelectPerfCounters;
	CComboBox	m_cSelectPerfInstances;
	CButton		m_cBuildPathFromSelection;
	CEdit		m_cSelectPerfDescription;

	// Control value variables
	int			m_nLEDType;
	DWORD		m_nUpdateFrequency;
	int			m_nUpdateINI;
	double		m_d100Percent;
	CString		m_sCounterPath;
	CString		m_sPerfObject;
	CString		m_sPerfCounter;
	CString		m_sPerfInstance;
	CString		m_sPerfDescription;

	// Tooltip support
	CToolTipCtrl	m_oToolTip;

	// Member variables
	CMainFrame*	m_pMainFrame;
	CString		m_sSelectedPerfPath;

	// Memory buffers for PdhEnumObjectItems
	TCHAR*		m_sBufferCounters;
	DWORD		m_nBufferCounters;
	TCHAR*		m_sBufferInstances;
	DWORD		m_nBufferInstances;

	// Memory buffer for PdhGetCounterInfo
	DWORD				m_nBufferInfo;
	PDH_COUNTER_INFO*	m_oBufferInfo;

	// Functions
	BOOL		OnInitDialog ();
	void		ApplySettings ();
	void		InitPerfBrowser ();
	DWORD		SplitPerfPath (CString sCounterPath, CString& sObject, CString& sCounter, CString& sInstance);
	DWORD		BuildPerfPath (CString& sCounterPath, CString sObject, CString sCounter, CString sInstance);
	DWORD		GetCounterExplainText (CString sCounterPath, CString& sExplainText);

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDefaultvalues();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedApply();
	//afx_msg void OnBnClickedCounterpathSelect();
	afx_msg void OnCbnSelchangePerfObjects();
	afx_msg void OnBnClickedUseSelectedDataSource();
	afx_msg void OnCbnSelchangePerfCounters();
	afx_msg void OnCbnSelchangePerfInstances();
};

