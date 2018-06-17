// DiskLED.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "SystemTray.h"


#define	WM_ICON_NOTIFY			WM_APP+10


// CDiskLEDApp:
// See DiskLED.cpp for the implementation of this class
//

class CDiskLEDApp : public CWinAppEx
{
public:
	CDiskLEDApp();

	CString m_strWindowName;		// Window name for application
	HICON   m_hIcon;				// Icon

	
// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDiskLEDApp theApp;