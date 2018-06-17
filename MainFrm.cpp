// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DiskLED.h"
#include "MainFrm.h"
#include "Helpers.h"
#include "DialogAbout.h"
#include "DialogConfig.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
//
// CMainFrame
//
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP

	ON_COMMAND(ID_POPUP_CONFIGURE,	ShowConfigurationDialog)
	ON_COMMAND(ID_POPUP_ABOUT,		ShowAboutDialog)
	ON_COMMAND(ID_POPUP_EXIT,		Shutdown)
	ON_MESSAGE(WM_ICON_NOTIFY,		OnTrayNotification)
	ON_MESSAGE(WM_POWERBROADCAST,	OnPowerBroadcast)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
//
// CMainFrame construction/destruction
//
////////////////////////////////////////////////////////////////////////////////

CMainFrame::CMainFrame()
{
	m_TimerID				= 0;
	m_hQuery				= NULL;
	m_hCounter				= NULL;
	m_nCurrentIcon			= 0;
	m_oCounterVal;


	// Variables affecting the display of performance counter data
	m_nTimerIntervalMS		= DEFAULT_COUNTER_1_MS;
	m_sCounter1Path			= DEFAULT_COUNTER_1_PATH;
	m_nLED1Type				= DEFAULT_COUNTER_1_LEDTYPE;
	m_dCounter1_100Percent	= DEFAULT_COUNTER_1_100PERCENT;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{	
    cs.style &= ~WS_VISIBLE;

	return CFrameWnd::PreCreateWindow(cs);
}

int CMainFrame::OnCreate (LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_TrayIcon.Create (this, WM_ICON_NOTIFY, PROGRAM_NAME, theApp.m_hIcon, IDR_POPUP_MENU))
	{
		return -1;
	}
	
	SetWindowText(theApp.m_strWindowName);

	// Read settings (from INI, if present)
	ReadSettings ();

	// Initialize counter and timer
	Initialize ();

	return 0;
}

DWORD CMainFrame::StartTimer ()
{
    // Check if it's already going
	if (m_TimerID) 
	{
		// Re-initialize the timer
        KillTimer (m_TimerID);
		m_TimerID	= 0;
	}

	m_TimerID	= SetTimer (ID_TIMER, m_nTimerIntervalMS, (TIMERPROC) NULL);

    if (! m_TimerID)
	{
		MessageBox (_T("Unable to set timer!"));
		return ERROR_INVALID_FUNCTION;
	}

	return ERROR_SUCCESS;
}



////////////////////////////////////////////////////////////////////////////////
//
//	Close all PDH handles and reset variables etc.
//
////////////////////////////////////////////////////////////////////////////////

DWORD CMainFrame::ClosePDH ()
{
	// Check if initialized
	if (m_hQuery == NULL)
	{
		// Not initialized
		return ERROR_SUCCESS;
	}

	PdhCloseQuery (m_hQuery);
	m_hQuery	= NULL;

	return ERROR_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Log an error message
//
////////////////////////////////////////////////////////////////////////////////

void CMainFrame::LogError (CString sCaller, CString sFailedComponent, DWORD nError, CString sHelpMessage)
{
	CString	sError;

	if (nError)
	{
		// Report an API error
		sError.Format (TEXT ("%s: %s failed with: 0x%x\n"), sCaller, sFailedComponent, nError);
	}
	else
	{
		// Report an internal (logic) error
		sError.Format (TEXT ("%s: %s\n"), sCaller, sFailedComponent);
	}

	// Add an optional explanatory help message
	if (!sHelpMessage.IsEmpty ())
	{
		sError	+= sHelpMessage;
	}

	// Display the message
	MessageBox (sError);
}


////////////////////////////////////////////////////////////////////////////////
//
//	Initialize PDH (performance monitor API)
//
////////////////////////////////////////////////////////////////////////////////

DWORD CMainFrame::InitPDH ()
{
	PDH_STATUS	nError	= ERROR_INVALID_FUNCTION;

	// Check if already initialized
	if (m_hQuery)
	{
		// Re-initialize
		ClosePDH ();
		m_hQuery	= NULL;
	}
		
	// Open a query handle
	nError	= PdhOpenQuery (NULL, 0, &m_hQuery);
	if(nError != ERROR_SUCCESS)
	{
		ClosePDH ();
		LogError (TEXT ("InitPDH"), TEXT ("PdhOpenQuery"), nError);
		return nError;
	}

	// Add a counter to the query
	// NOTE: PdhAddEnglishCounter works from Vista onwards
	nError	= PdhAddCounter (m_hQuery, m_sCounter1Path, 0, &m_hCounter);
	if (nError != ERROR_SUCCESS)
	{
		CString	sExplanation;
		if (nError == PDH_CSTATUS_NO_OBJECT || nError == PDH_CSTATUS_NO_COUNTER)
		{
			sExplanation	= TEXT ("This probably means that the performance counter path is invalid.\nPlease take note that it must be localized. The default path used (if no config file is specified) is valid on English systems only.");
		}
		ClosePDH ();
		LogError (TEXT ("InitPDH"), TEXT ("PdhAddCounter"), nError, sExplanation);
		return nError;
	}

	// Get initial counter value as counter requires difference between two PdhCollectQueryData
	nError	= PdhCollectQueryData (m_hQuery);
	if (nError != ERROR_SUCCESS && nError != PDH_NO_DATA)
	{
		ClosePDH ();
		LogError (TEXT ("InitPDH"), TEXT ("PdhCollectQueryData"), nError);
		return nError;
	}

	return ERROR_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
//
// CMainFrame message handlers
//
////////////////////////////////////////////////////////////////////////////////

void CMainFrame::ShowConfigurationDialog ()
{
	// This is initialized to NULL
	static CDialog*	pdlgConfig;

	// Create the dialog object if necessary
	if (pdlgConfig == NULL)
	{
		pdlgConfig	= new CDialogConfig (this);
	}

	// Make sure the window is not already shown
	if (pdlgConfig->m_hWnd == NULL)
	{
		// Show the config dialog
		pdlgConfig->DoModal ();
		delete pdlgConfig;
		pdlgConfig	= NULL;
	}
	else
	{
		pdlgConfig->SetFocus ();
	}
}


void CMainFrame::ShowAboutDialog ()
{
	// This is initialized to NULL
	static CDialog*	pdlgAbout;

	// Create the dialog object if necessary
	if (pdlgAbout == NULL)
	{
		pdlgAbout	= new CDialogAbout ();
	}

	// Make sure the window is not already shown
	if (pdlgAbout->m_hWnd == NULL)
	{
		// Show the about dialog
		pdlgAbout->DoModal ();
		delete pdlgAbout;
		pdlgAbout	= NULL;
	}
	else
	{
		pdlgAbout->SetFocus ();
	}
}


void CMainFrame::Shutdown ()
{
	PostMessage (WM_CLOSE, 0, 0);
}


afx_msg LRESULT CMainFrame::OnPowerBroadcast(WPARAM, LPARAM)
{
	return 0L;
}

LRESULT CMainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	return m_TrayIcon.OnTrayNotification (wParam, lParam);
}

void CMainFrame::OnTimer (UINT_PTR nIDEvent) 
{
	PDH_STATUS	nError	= ERROR_INVALID_FUNCTION;

	if (m_hQuery == NULL)
	{
		// Not initialized
		return;
	}

	// Get current data
	nError	= PdhCollectQueryData (m_hQuery);
	if (nError != ERROR_SUCCESS)
	{
		ClosePDH ();

		CString	sExplanation;
		if (nError == PDH_NO_DATA)
		{
			sExplanation	= TEXT ("PDH_NO_DATA - No data is available for this performance counter.");
		}
		LogError (TEXT ("OnTimer"), TEXT ("PdhCollectQueryData"), nError, sExplanation);
		return;
	}

	//
	// Format the counter data
	//
	// Get the data as floating point number
	DWORD	nDataFormat	= PDH_FMT_DOUBLE;

	// Get the counter data
	nError	= PdhGetFormattedCounterValue (m_hCounter, nDataFormat, 0, &m_oCounterVal);
	if (nError != ERROR_SUCCESS)
	{
		// Let's have a look at the error message
		if (nError == PDH_CALC_NEGATIVE_VALUE || nError == PDH_CALC_NEGATIVE_DENOMINATOR || nError == PDH_CALC_NEGATIVE_TIMEBASE)
		{
			// This (PDH_CALC_NEGATIVE_VALUE) happens on Windows 7 RTM if a USB flash drive is inserted.
			// The algorithm probably has a bug in that it returns a negative value because of the additional disk which cannot be converted to LONG.
			// Ignore this and do nothing.
			return;
		}
		else
		{
			ClosePDH ();
			LogError (TEXT ("OnTimer"), TEXT ("PdhGetFormattedCounterValue"), nError);
			return;
		}
	}

	// Normalize the value read (make 100 the maximum)
	double	dNormalizedCounterValue	= m_oCounterVal.doubleValue * 100.0 / m_dCounter1_100Percent;

	//
	// Select the icon to display
	//
	UINT	nNextIcon	= 0;

	if (dNormalizedCounterValue == 0)
	{
		if (m_nLED1Type == LEDTYPE_FLICKER)
		{
			nNextIcon	= IDI_ICON1;
		}
		else if (m_nLED1Type == LEDTYPE_COLOREDBAR)
		{
			nNextIcon	= IDI_ICON7;
		}
	}
	else if (dNormalizedCounterValue < 25)
	{
		if (m_nLED1Type == LEDTYPE_FLICKER)
		{
			nNextIcon	= IDI_ICON2;
		}
		else if (m_nLED1Type == LEDTYPE_COLOREDBAR)
		{
			nNextIcon	= IDI_ICON8;
		}
	}
	else if (dNormalizedCounterValue < 50)
	{
		if (m_nLED1Type == LEDTYPE_FLICKER)
		{
			nNextIcon	= IDI_ICON3;
		}
		else if (m_nLED1Type == LEDTYPE_COLOREDBAR)
		{
			nNextIcon	= IDI_ICON9;
		}
	}
	else if (dNormalizedCounterValue < 75)
	{
		if (m_nLED1Type == LEDTYPE_FLICKER)
		{
			nNextIcon	= IDI_ICON4;
		}
		else if (m_nLED1Type == LEDTYPE_COLOREDBAR)
		{
			nNextIcon	= IDI_ICON10;
		}
	}
	else
	{
		if (m_nLED1Type == LEDTYPE_FLICKER)
		{
			nNextIcon	= IDI_ICON5;
		}
		else if (m_nLED1Type == LEDTYPE_COLOREDBAR)
		{
			nNextIcon	= IDI_ICON11;
		}
	}

	if (nNextIcon != m_nCurrentIcon)
	{
		m_TrayIcon.SetIcon (nNextIcon);
		m_nCurrentIcon	= nNextIcon;
	}

	// Call the parent class's function
	CFrameWnd::OnTimer (nIDEvent);
}


void CMainFrame::OnClose() 
{
	// Clean up the PDH variables
	if (m_hQuery)
	{
		PdhCloseQuery (m_hQuery);
		m_hQuery	= NULL;
	}

	m_TrayIcon.RemoveIcon();
	
	// Call the parent class's function
	CFrameWnd::OnClose();
}


////////////////////////////////////////////////////////////////////////////////
//
//	Read settings (from INI or potentially other sources)
//
////////////////////////////////////////////////////////////////////////////////

DWORD CMainFrame::ReadSettings ()
{
	DWORD	nError						= ERROR_INVALID_FUNCTION;
	DWORD	nRegData					= 0;
	double	dRegData					= 0;
	int		nErr						= 0;
	CString	sData;

	// Determine and store the path to our executable and the installation directory
	nError	=  GetModuleFileName(NULL, m_sExecutablePath.GetBuffer (MAX_PATH), MAX_PATH);
	m_sExecutablePath.ReleaseBuffer ();
	if (nError == 0)
	{
		return ERROR_INVALID_FUNCTION;
	}
	m_sInstallationDirectory	=	m_sExecutablePath.Left (m_sExecutablePath.ReverseFind('\\'));

	// Build the path to the INI file
	m_sINIFile	= m_sInstallationDirectory + TEXT ("\\") + INI_SETTINGS_FILE;

	//
	// Read settings
	//
	// LED type
	nError	= ReadINIValue (m_sINIFile, INI_COUNTER_1_SECTION, INI_COUNTER_1_VALUE_LEDTYPE, &sData);
	if (nError == ERROR_SUCCESS)
	{
		if (sData.CompareNoCase (LEDTYPE_FLICKER_TEXT) == 0)
		{
			m_nLED1Type	= LEDTYPE_FLICKER;
		}
		else if (sData.CompareNoCase (LEDTYPE_COLOREDBAR_TEXT) == 0)
		{
			m_nLED1Type	= LEDTYPE_COLOREDBAR;
		}
	}

	// Counter path
	nError	= ReadINIValue (m_sINIFile, INI_COUNTER_1_SECTION, INI_COUNTER_1_VALUE_PATH, &sData);
	if (nError == ERROR_SUCCESS)
	{
		m_sCounter1Path	= sData;
	}

	// Update interval
	nError	= ReadINIValue (m_sINIFile, INI_COUNTER_1_SECTION, INI_COUNTER_1_VALUE_UPDATE_INTERVAL, &sData);
	if (nError == ERROR_SUCCESS)
	{
		// Convert the string read to DWORD
		nRegData	= _ttoi (sData);
		if (_get_errno (&nErr) == 0 && nErr == 0)
		{
			// Store the value from the INI file
			m_nTimerIntervalMS	= nRegData;
		}
	}

	// 100% mark (optional for some counter types)
	nError	= ReadINIValue (m_sINIFile, INI_COUNTER_1_SECTION, INI_COUNTER_1_VALUE_100PERCENT, &sData);
	if (nError == ERROR_SUCCESS)
	{
		// Convert the string read to double
		dRegData	= _tstof (sData);
		if (_get_errno (&nErr) == 0 && nErr == 0)
		{
			// Store the value from the INI file
			m_dCounter1_100Percent	= dRegData;
		}
	}

	return ERROR_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Initialize the performance counter and timer
//
////////////////////////////////////////////////////////////////////////////////

DWORD CMainFrame::Initialize (CString sCounterPath, DWORD nLEDType, DWORD nTimerInterval, double dCounter_100Percent)
{
	DWORD	nError	= ERROR_INVALID_FUNCTION;

	//
	// All parameters have default values. If a parameter is empty, use either the previous or the default value
	//
	if (!sCounterPath.IsEmpty ())
	{
		m_sCounter1Path	= sCounterPath;
	}
	if (m_sCounter1Path.IsEmpty ())
	{
		m_sCounter1Path			= DEFAULT_COUNTER_1_PATH;
	}

	if (nLEDType)
	{
		m_nLED1Type	= nLEDType;
	}
	if (m_nLED1Type == 0)
	{
		m_nLED1Type				= DEFAULT_COUNTER_1_LEDTYPE;
	}

	if (nTimerInterval)
	{
		m_nTimerIntervalMS	= nTimerInterval;
	}
	if (m_nTimerIntervalMS == 0)
	{
		m_nTimerIntervalMS		= DEFAULT_COUNTER_1_MS;
	}

	if (dCounter_100Percent != 0.0)
	{
		m_dCounter1_100Percent	= dCounter_100Percent;
	}
	if (m_dCounter1_100Percent == 0.0)
	{
		m_dCounter1_100Percent	= DEFAULT_COUNTER_1_100PERCENT;
	}

	// Check parameters for validity
	if (m_nLED1Type != LEDTYPE_FLICKER && m_nLED1Type != LEDTYPE_COLOREDBAR)
	{
		LogError (TEXT ("Initialize"), TEXT ("Invalid LED type specified!"), 0);
		return ERROR_INVALID_PARAMETER;
	}

	// Init PDH
	nError	= InitPDH ();
	if (nError != ERROR_SUCCESS)
	{
		KillTimer (m_TimerID);

		// Set an error icon
		m_TrayIcon.SetIcon (IDI_ICON6);

		return nError;
	}

	// Start the timer
	nError	= StartTimer ();

	// Set a new info tip including the counter path
	CString	sToolTipText	=	PROGRAM_NAME;
	sToolTipText			+=	TEXT (" - ") + m_sCounter1Path;
	m_TrayIcon.SetTooltipText (sToolTipText);

	return nError;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Return the LED type
//
////////////////////////////////////////////////////////////////////////////////

int CMainFrame::GetLEDType ()
{
	return m_nLED1Type;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Return the update frequency
//
////////////////////////////////////////////////////////////////////////////////

DWORD CMainFrame::GetUpdateFrequency ()
{
	return m_nTimerIntervalMS;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Return the default LED settings
//
////////////////////////////////////////////////////////////////////////////////

void CMainFrame::GetLEDDefaults (CString& sCounterPath, int& nLEDType, DWORD& nTimerInterval, double& dCounter_100Percent)
{
	nTimerInterval		= DEFAULT_COUNTER_1_MS;
	sCounterPath		= DEFAULT_COUNTER_1_PATH;
	nLEDType			= DEFAULT_COUNTER_1_LEDTYPE;
	dCounter_100Percent	= DEFAULT_COUNTER_1_100PERCENT;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Get the path to the INI file
//
////////////////////////////////////////////////////////////////////////////////

void CMainFrame::GetINIFile (CString& sINIFile)
{
	sINIFile	= m_sINIFile;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Return the counter value that corresponds to 100% utilization
//
////////////////////////////////////////////////////////////////////////////////

double CMainFrame::Get100PercentUtilization ()
{
	return m_dCounter1_100Percent;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Return the counter path
//
////////////////////////////////////////////////////////////////////////////////

void CMainFrame::GetCounterPath (CString& sCounterPath)
{
	sCounterPath	= m_sCounter1Path;
}


