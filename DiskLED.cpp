
// DiskLED.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DiskLED.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDiskLEDApp

BEGIN_MESSAGE_MAP(CDiskLEDApp, CWinAppEx)
	//ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDiskLEDApp construction

CDiskLEDApp::CDiskLEDApp()
{
	m_strWindowName		= TEXT ("DiskLED");		// Window/Application name
}


// The one and only CDiskLEDApp object

CDiskLEDApp theApp;


// CDiskLEDApp initialization

BOOL CDiskLEDApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	SetRegistryKey(_T("DiskLED"));

	m_hIcon = LoadIcon(IDI_ICON1);
	if (m_hIcon == NULL)
	{
		AfxMessageBox(_T("Could not load icon. Exiting!"));
		return FALSE;
	}

	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

    pMainFrame->ModifyStyle(WS_VISIBLE, 0);

	return TRUE;
}
