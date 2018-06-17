// DialogConfig.cpp : implementation file
//

#include "stdafx.h"
#include "DiskLED.h"
#include "MainFrm.h"
#include "DialogConfig.h"
#include "Helpers.h"


// CDialogConfig dialog

IMPLEMENT_DYNAMIC(CDialogConfig, CDialog)

CDialogConfig::CDialogConfig(CMainFrame* pMainFrame) : CDialog(CDialogConfig::IDD, pMainFrame)
														, m_nLEDType(0), m_nUpdateFrequency(0)
														, m_nUpdateINI(0)
														, m_d100Percent(0)
														, m_sCounterPath(_T(""))
														, m_sPerfObject(_T(""))
														, m_sPerfCounter(_T(""))
														, m_sPerfInstance(_T(""))
														, m_sPerfDescription(_T(""))
{
	if (pMainFrame)
	{
		m_pMainFrame	= (CMainFrame*) pMainFrame;
	}
	else
	{
		m_pMainFrame	= NULL;
	}

	// Memory buffers for PdhEnumObjectItems
	m_sBufferCounters	= NULL;
	m_nBufferCounters	= 0;
	m_sBufferInstances	= NULL;
	m_nBufferInstances	= 0;

	// Memory buffer for PdhGetCounterInfo
	m_nBufferInfo		= 0;
	m_oBufferInfo		= NULL;
}

CDialogConfig::~CDialogConfig()
{
	if (m_sBufferCounters)
	{
		HeapFree (GetProcessHeap (), 0, m_sBufferCounters);
		m_sBufferCounters	= NULL;
	}
	if (m_sBufferInstances)
	{
		HeapFree (GetProcessHeap (), 0, m_sBufferInstances);
		m_sBufferInstances	= NULL;
	}
	if (m_oBufferInfo)
	{
		HeapFree (GetProcessHeap (), 0, m_oBufferInfo);
		m_oBufferInfo	= NULL;
	}
}

void CDialogConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RADIO_LEDTYPE1, m_cLEDType1);
	DDX_Control(pDX, IDC_RADIO_LEDTYPE2, m_cLEDType2);
	DDX_Control(pDX, IDC_UPDATEFREQUENCY_TEXT1, m_cUpdateFrequencyText1);
	DDX_Control(pDX, IDC_UPDATEFREQUENCY_TEXT2, m_cUpdateFrequencyText2);
	DDX_Control(pDX, IDC_UPDATEFREQUENCY_EDIT, m_cUpdateFrequencyEdit);
	DDX_Control(pDX, IDC_UPDATE_INI, m_cUpdateINI);
	DDX_Control(pDX, IDC_DEFAULTVALUES, m_cDefaultValues);
	DDX_Control(pDX, IDC_100PERCENT_EDIT, m_c100Percent);
	DDX_Control(pDX, IDC_COUNTERPATH_EDIT, m_cCounterPath);

	DDX_Radio(pDX, IDC_RADIO_LEDTYPE1, m_nLEDType);
	DDX_Text(pDX, IDC_UPDATEFREQUENCY_EDIT, m_nUpdateFrequency);
	DDX_Text(pDX, IDC_100PERCENT_EDIT, m_d100Percent);
	DDX_Check (pDX, IDC_UPDATE_INI, m_nUpdateINI);
	DDX_Text(pDX, IDC_COUNTERPATH_EDIT, m_sCounterPath);
	DDX_Control(pDX, IDC_PERF_OBJECTS, m_cSelectPerfObjects);
	DDX_CBString(pDX, IDC_PERF_OBJECTS, m_sPerfObject);
	DDX_Control(pDX, IDC_PERF_COUNTERS, m_cSelectPerfCounters);
	DDX_CBString(pDX, IDC_PERF_COUNTERS, m_sPerfCounter);
	DDX_CBString(pDX, IDC_PERF_INSTANCES, m_sPerfInstance);
	DDX_Control(pDX, IDC_PERF_INSTANCES, m_cSelectPerfInstances);
	DDX_Control(pDX, IDC_USE_SELECTED_DATA_SOURCE, m_cBuildPathFromSelection);
	DDX_Control(pDX, IDC_PERF_DESCRIPTION, m_cSelectPerfDescription);
	DDX_Text(pDX, IDC_PERF_DESCRIPTION, m_sPerfDescription);
}


BOOL CDialogConfig::PreTranslateMessage(MSG* pMsg)
{
	if (m_oToolTip)
	{
		m_oToolTip.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CDialogConfig, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDC_DEFAULTVALUES, &CDialogConfig::OnBnClickedDefaultvalues)
	ON_BN_CLICKED(IDCANCEL, &CDialogConfig::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_APPLY, &CDialogConfig::OnBnClickedApply)
	ON_BN_CLICKED(IDC_USE_SELECTED_DATA_SOURCE, &CDialogConfig::OnBnClickedUseSelectedDataSource)
	ON_CBN_SELCHANGE(IDC_PERF_OBJECTS, &CDialogConfig::OnCbnSelchangePerfObjects)
	ON_CBN_SELCHANGE(IDC_PERF_COUNTERS, &CDialogConfig::OnCbnSelchangePerfCounters)
	ON_CBN_SELCHANGE(IDC_PERF_INSTANCES, &CDialogConfig::OnCbnSelchangePerfInstances)
END_MESSAGE_MAP()


////////////////////////////////////////////////////////////////////////////////
//
//	Initialize the dialog
//
////////////////////////////////////////////////////////////////////////////////

BOOL CDialogConfig::OnInitDialog ()
{
	// Call the base class's handler
	CDialog::OnInitDialog ();

	// Set up tooltip support
	if (m_oToolTip.Create (this))
	{
		m_oToolTip.AddTool (&m_cLEDType1, IDS_TOOLTIP_LEDTYPE1);
		m_oToolTip.AddTool (&m_cLEDType2, IDS_TOOLTIP_LEDTYPE2);
		m_oToolTip.AddTool (&m_cUpdateFrequencyEdit, IDS_TOOLTIP_UPDATE_FREQUENCY);
		m_oToolTip.AddTool (&m_cUpdateINI, IDS_TOOLTIP_UPDATE_INI);
		m_oToolTip.AddTool (&m_cDefaultValues, IDS_TOOLTIP_DEFAULT_VALUES);
		m_oToolTip.AddTool (&m_c100Percent, IDS_TOOLTIP_100PERCENT);
		m_oToolTip.AddTool (&m_cBuildPathFromSelection, IDS_TOOLTIP_COUNTERPATH);
		m_oToolTip.AddTool (&m_cSelectPerfDescription, IDS_TOOLTIP_DESCRIPTION);
		m_oToolTip.AddTool (&m_cSelectPerfObjects, IDS_TOOLTIP_OBJECTS);
		m_oToolTip.AddTool (&m_cSelectPerfCounters, IDS_TOOLTIP_COUNTERS);
		m_oToolTip.AddTool (&m_cSelectPerfInstances, IDS_TOOLTIP_INSTANCES);

		// TODO: For some reason, the tooltip is not displayed on the text controls, although a dialog in VS shows it _can_ work somehow
		m_oToolTip.AddTool (&m_cUpdateFrequencyText1, IDS_TOOLTIP_UPDATE_FREQUENCY);
		m_oToolTip.AddTool (&m_cUpdateFrequencyText2, IDS_TOOLTIP_UPDATE_FREQUENCY);
	}

	// Set icons
	HICON	hIcon	= LoadIcon (GetModuleHandle (NULL), MAKEINTRESOURCE (IDI_ICON5));
	if (hIcon)
	{
		m_cLEDType1.SendMessage (BM_SETIMAGE, IMAGE_ICON, (LPARAM) hIcon);
	}
	hIcon	= LoadIcon (GetModuleHandle (NULL), MAKEINTRESOURCE (IDI_ICON9));
	if (hIcon)
	{
		m_cLEDType2.SendMessage (BM_SETIMAGE, IMAGE_ICON, (LPARAM) hIcon);
	}

	// Control value initialization
	if (m_pMainFrame)
	{
		// State of the LED type radio buttons
		m_nLEDType			= m_pMainFrame->GetLEDType () - 1;

		// Update frequency edit control
		m_nUpdateFrequency	= m_pMainFrame->GetUpdateFrequency ();

		// 100% value
		m_d100Percent		= m_pMainFrame->Get100PercentUtilization ();

		// Counter path
		m_pMainFrame->GetCounterPath (m_sCounterPath);

		// Update the dialog with the data written to the member variables
		UpdateData (FALSE);
	}

	// Set the initial focus to the icon type
	m_cLEDType1.SetFocus ();

	// Initialize the performance browser controls
	InitPerfBrowser ();

	// Success
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
//	The OK button was clicked
//
////////////////////////////////////////////////////////////////////////////////

void CDialogConfig::OnBnClickedOk()
{
	ApplySettings ();

	OnOK();
}


////////////////////////////////////////////////////////////////////////////////
//
//	The button "Default values" was clicked
//
////////////////////////////////////////////////////////////////////////////////

void CDialogConfig::OnBnClickedDefaultvalues()
{
	// Get defaults
	if (m_pMainFrame)
	{
		m_pMainFrame->GetLEDDefaults (m_sCounterPath, m_nLEDType, m_nUpdateFrequency, m_d100Percent);
	}

	// Translate values where necessary
	m_nLEDType--;

	// Update the dialog with the new data
	UpdateData (FALSE);
}


////////////////////////////////////////////////////////////////////////////////
//
//	The cancel button was clicked
//
////////////////////////////////////////////////////////////////////////////////

void CDialogConfig::OnBnClickedCancel()
{
	OnCancel();
}


////////////////////////////////////////////////////////////////////////////////
//
//	Apply the settings from the dialog
//
////////////////////////////////////////////////////////////////////////////////

void CDialogConfig::ApplySettings ()
{
	// Get data into our member variables
	UpdateData (TRUE);

	// Transmit the settings to the LED
	if (m_pMainFrame)
	{
		m_pMainFrame->Initialize (m_sCounterPath, m_nLEDType + 1, m_nUpdateFrequency, m_d100Percent);
	}

	// Store the new settings in the INI file?
	if (m_nUpdateINI && m_pMainFrame)
	{
		CString	sINIFile;
		m_pMainFrame->GetINIFile (sINIFile);

		// LED type
		CString	sLEDType;
		switch (m_nLEDType + 1)
		{
		case LEDTYPE_COLOREDBAR:
			sLEDType	= LEDTYPE_COLOREDBAR_TEXT;
			break;
		default:
		case LEDTYPE_FLICKER:
			sLEDType	= LEDTYPE_FLICKER_TEXT;
			break;
		}
		DWORD nError	= WriteINIValue (sINIFile, INI_COUNTER_1_SECTION, INI_COUNTER_1_VALUE_LEDTYPE, sLEDType);
		if (nError != ERROR_SUCCESS)
		{
			CString	sExplanation;
			if (nError == ERROR_ACCESS_DENIED)
			{
				sExplanation	= TEXT ("Access denied - could not write to the INI file: <") + sINIFile + TEXT (">");
			}
			else
			{
				sExplanation	= TEXT ("Could not write to the INI file: <") + sINIFile + TEXT (">");
			}
			m_pMainFrame->LogError (TEXT ("ApplySettings"), TEXT ("WriteINIValue"), nError, sExplanation);
			return;
		}
		
		// Update frequency
		nError	= WriteINIValue (sINIFile, INI_COUNTER_1_SECTION, INI_COUNTER_1_VALUE_UPDATE_INTERVAL, (int) m_nUpdateFrequency);
		if (nError != ERROR_SUCCESS)
		{
			m_pMainFrame->LogError (TEXT ("ApplySettings"), TEXT ("WriteINIValue"), nError);
			return;
		}
		
		// 100% counter value
		nError	= WriteINIValue (sINIFile, INI_COUNTER_1_SECTION, INI_COUNTER_1_VALUE_100PERCENT, (double) m_d100Percent);
		if (nError != ERROR_SUCCESS)
		{
			m_pMainFrame->LogError (TEXT ("ApplySettings"), TEXT ("WriteINIValue"), nError);
			return;
		}
		
		// Counter path
		nError	= WriteINIValue (sINIFile, INI_COUNTER_1_SECTION, INI_COUNTER_1_VALUE_PATH, m_sCounterPath);
		if (nError != ERROR_SUCCESS)
		{
			m_pMainFrame->LogError (TEXT ("ApplySettings"), TEXT ("WriteINIValue"), nError);
			return;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//	The apply button was clicked
//
////////////////////////////////////////////////////////////////////////////////

void CDialogConfig::OnBnClickedApply()
{
	ApplySettings ();
}


////////////////////////////////////////////////////////////////////////////////
//
//	The button to select a counter path was clicked
//
////////////////////////////////////////////////////////////////////////////////

/*void CDialogConfig::OnBnClickedCounterpathSelect()
{
	PDH_STATUS				pdhStatus;
	PDH_BROWSE_DLG_CONFIG	oPDHBrowseDialogCfg;
	TCHAR					sBuffer[PDH_MAX_COUNTER_PATH + 1];

	// Zero memory structures
	SecureZeroMemory (&oPDHBrowseDialogCfg, sizeof (PDH_BROWSE_DLG_CONFIG));

	// Initialize the path buffer
	SecureZeroMemory (&sBuffer, sizeof (sBuffer));
	//_tcscpy_s (sBuffer, PDH_MAX_COUNTER_PATH + 1, m_sCounterPath);

	// Initialize the browser dialog window settings
	oPDHBrowseDialogCfg.bIncludeInstanceIndex		= FALSE;   
	oPDHBrowseDialogCfg.bSingleCounterPerAdd		= TRUE;
	oPDHBrowseDialogCfg.bSingleCounterPerDialog		= TRUE;
	oPDHBrowseDialogCfg.bLocalCountersOnly			= FALSE;
	oPDHBrowseDialogCfg.bWildCardInstances			= TRUE;
	oPDHBrowseDialogCfg.bHideDetailBox				= TRUE;
	oPDHBrowseDialogCfg.bInitializePath				= FALSE;
	oPDHBrowseDialogCfg.bDisableMachineSelection	= FALSE;
	oPDHBrowseDialogCfg.bIncludeCostlyObjects		= FALSE;
	oPDHBrowseDialogCfg.bShowObjectBrowser			= FALSE;
	oPDHBrowseDialogCfg.hWndOwner					= m_hWnd;
	oPDHBrowseDialogCfg.szReturnPathBuffer			= sBuffer;
	oPDHBrowseDialogCfg.cchReturnPathLength			= sizeof (sBuffer) / sizeof (TCHAR);
	oPDHBrowseDialogCfg.pCallBack					= NULL;
	oPDHBrowseDialogCfg.dwCallBackArg				= 0;
	oPDHBrowseDialogCfg.CallBackStatus				= ERROR_SUCCESS;
	oPDHBrowseDialogCfg.dwDefaultDetailLevel		= PERF_DETAIL_WIZARD;
	oPDHBrowseDialogCfg.szDialogBoxCaption			= TEXT ("Select a counter for DiskLED");

	// Display the counter browser window. The dialog is configured
	// to return a single selection from the counter list.
	pdhStatus	= PdhBrowseCounters (&oPDHBrowseDialogCfg);
	if (pdhStatus != ERROR_SUCCESS)
	{
		if (pdhStatus != PDH_DIALOG_CANCELLED)
		{
			m_pMainFrame->LogError (TEXT ("OnBnClickedCounterpathSelect"), TEXT ("PdhBrowseCounters"), pdhStatus);
		}
	}
	else
	{
		m_sCounterPath	= sBuffer;

		// Update the dialog with the new data
		UpdateData (FALSE);
	}
}
*/

////////////////////////////////////////////////////////////////////////////////
//
//	Enumerate all performance objects and add them to the appropriate list boxes
//
////////////////////////////////////////////////////////////////////////////////

void CDialogConfig::InitPerfBrowser ()
{
	PDH_STATUS		nError			= ERROR_INVALID_FUNCTION;
	TCHAR*			sBuffer			= NULL;
	DWORD			nBuffer			= 0;
	CString			sObject;
	CString			sInstance;
	CString			sCounter;

	// Get the size of the buffer required
	nError	= PdhEnumObjects (NULL, NULL, sBuffer, &nBuffer, PERF_DETAIL_WIZARD, TRUE);
	if (nError != PDH_MORE_DATA)
	{
		m_pMainFrame->LogError (TEXT ("InitPerfBrowser"), TEXT ("PdhEnumObjects (1)"), nError);
		goto CleanUp;
	}

	// Allocate memory
	nBuffer++;			// For 2000/XP
	sBuffer	= (TCHAR*) HeapAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof (TCHAR) * nBuffer);
	if (nBuffer == NULL)
	{
		m_pMainFrame->LogError (TEXT ("InitPerfBrowser"), TEXT ("HeapAlloc"), ERROR_OUTOFMEMORY);
		goto CleanUp;
	}

	// Now get the list of perf objects
	nError	= PdhEnumObjects (NULL, NULL, sBuffer, &nBuffer, PERF_DETAIL_WIZARD, FALSE);
	if (nError != ERROR_SUCCESS || sBuffer == NULL)
	{
		m_pMainFrame->LogError (TEXT ("InitPerfBrowser"), TEXT ("PdhEnumObjects (2)"), nError);
		goto CleanUp;
	}

	// Add the object names we got to the UI
	for (TCHAR* sName = sBuffer; *sName != 0; sName += _tcscnlen (sName, nBuffer) + 1)
	{
		m_cSelectPerfObjects.AddString (sName);
	}

	// Did we get a counter path to select in the UI?
	if (m_sCounterPath.GetLength () == 0)
	{
		goto CleanUp;
	}

	if (SplitPerfPath (m_sCounterPath, sObject, sCounter, sInstance) == ERROR_SUCCESS)
	{
		// Select the object in the list
		m_cSelectPerfObjects.SelectString (-1, sObject);
		OnCbnSelchangePerfObjects ();
	}

CleanUp:
	if (sBuffer)
	{
		HeapFree(GetProcessHeap (), 0, sBuffer);
		sBuffer	= NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//	When a new perf object is selected, enumerate object items
//
////////////////////////////////////////////////////////////////////////////////

void CDialogConfig::OnCbnSelchangePerfObjects ()
{
	PDH_STATUS		nError				= ERROR_INVALID_FUNCTION;
	CString			sObject;
	CString			sCounter;
	CString			sInstance;
	DWORD			nBufferCounters		= 0;
	DWORD			nBufferInstances	= 0;
	CMap<CString, LPCTSTR, DWORD, DWORD>	oInstances;

	// Remove all items from the combo boxes
	m_cSelectPerfCounters.ResetContent ();
	m_cSelectPerfInstances.ResetContent ();

	// Get data from the dialog box into our member variables
	UpdateData (TRUE);

	// Zero the buffer memory allocated earlier
	if (m_sBufferCounters)
	{
		SecureZeroMemory (m_sBufferCounters, m_nBufferCounters);
	}
	if (m_sBufferInstances)
	{
		SecureZeroMemory (m_sBufferInstances, m_nBufferInstances);
	}

	// Is an item selected?
	if (m_sPerfObject.GetLength () == 0)
	{
		return;
	}

	// Get the sizes of the buffers required
	nError	= PdhEnumObjectItems (NULL, NULL, m_sPerfObject, NULL, &nBufferCounters, NULL, &nBufferInstances, PERF_DETAIL_WIZARD, 0);
	if (nError != PDH_MORE_DATA)
	{
		m_pMainFrame->LogError (TEXT ("OnCbnSelchangePerfObjects"), TEXT ("PdhEnumObjectItems (1)"), nError);
		goto CleanUp;
	}

	//
	// Allocate memory if necessary
	//
	if (nBufferCounters > m_nBufferCounters)
	{
		// Free memory of previously allocated
		if (m_sBufferCounters)
		{
			HeapFree (GetProcessHeap (), 0, m_sBufferCounters);
			m_sBufferCounters	= NULL;
		}

		// Allocate more than required
		m_nBufferCounters	= nBufferCounters * 2;
		nBufferCounters		= m_nBufferCounters;

		m_sBufferCounters		= (TCHAR*) HeapAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof (TCHAR) * m_nBufferCounters);
		if (m_sBufferCounters == NULL)
		{
			m_pMainFrame->LogError (TEXT ("OnCbnSelchangePerfObjects"), TEXT ("HeapAlloc (1) failed!"), 0);
			goto CleanUp;
		}
	}

	if (nBufferInstances > m_nBufferInstances)
	{
		// Free memory of previously allocated
		if (m_sBufferInstances)
		{
			HeapFree (GetProcessHeap (), 0, m_sBufferInstances);
			m_sBufferInstances	= NULL;
		}

		// Allocate more than required
		m_nBufferInstances	= nBufferInstances * 2;
		nBufferInstances	= m_nBufferInstances;

		m_sBufferInstances		= (TCHAR*) HeapAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof (TCHAR) * m_nBufferInstances);
		if (m_sBufferInstances == NULL)
		{
			m_pMainFrame->LogError (TEXT ("OnCbnSelchangePerfObjects"), TEXT ("HeapAlloc (2) failed!"), 0);
			goto CleanUp;
		}
	}

	// Now get the list of counters and instances
	nError	= PdhEnumObjectItems (NULL, NULL, m_sPerfObject, m_sBufferCounters, &nBufferCounters, m_sBufferInstances, &nBufferInstances, PERF_DETAIL_WIZARD, 0);
	if (nError != ERROR_SUCCESS || m_sBufferCounters == NULL || m_sBufferInstances == NULL)
	{
		m_pMainFrame->LogError (TEXT ("OnCbnSelchangePerfObjects"), TEXT ("PdhEnumObjectItems (2)"), nError);
		goto CleanUp;
	}

	// Add the counter names we got to the UI
	for (TCHAR* sName = m_sBufferCounters; *sName != 0; sName += _tcscnlen (sName, nBufferCounters) + 1)
	{
		m_cSelectPerfCounters.AddString (sName);
	}

	// Add the instance names we got to the UI
	// Some objects (e.g. processes or threads) can have multiple instances. In that case, we get multiple duplicate instance names. Add a zero-based index.
	// The first instance does not get an index, the second gets #1 appended, the third #2 and so on.
	TCHAR sBuffer[10];
	for (TCHAR* sName = m_sBufferInstances; *sName != 0; sName += _tcscnlen (sName, nBufferCounters) + 1)
	{
		DWORD	nValue	= 0;
		oInstances.Lookup (sName, nValue);
		nValue++;
		oInstances[sName]	= nValue;
		sInstance	= sName;
		if (nValue > 1)
		{
			sInstance	+= TEXT ("#");
			_itot_s (nValue - 1, sBuffer, 10, 10);
			sInstance	+= sBuffer;
		}
		m_cSelectPerfInstances.AddString (sInstance);
	}
	sInstance.Empty ();

	// Make sure initially nothing is selected in the counter and instance combo boxes
	// (This does not trigger the selection change event handler)
	m_cSelectPerfCounters.SetCurSel (-1);
	m_cSelectPerfInstances.SetCurSel (-1);

	// Do we have a counter path to select in the UI?
	if (m_sCounterPath.GetLength () == 0)
	{
		goto CleanUp;
	}

	// Select the items in the lists
	if (SplitPerfPath (m_sCounterPath, sObject, sCounter, sInstance) == ERROR_SUCCESS)
	{
		// Is the object selected the one displayed in the edit box?
		if (sObject.CompareNoCase (m_sPerfObject) == 0)
		{
			// Select the counter
			m_cSelectPerfCounters.SelectString (-1, sCounter);

			// Select the instance
			m_cSelectPerfInstances.SelectString (-1, sInstance);
		}
	}

	// Deal with the change of the counter and instance list boxes - the event handler are for some reason not called directly (sad, but documented)
	// Do it manually (sigh)
	CDialogConfig::OnCbnSelchangePerfCounters ();
	CDialogConfig::OnCbnSelchangePerfInstances ();

CleanUp:
	return;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Split a perf path into its components
//
////////////////////////////////////////////////////////////////////////////////

DWORD CDialogConfig::SplitPerfPath (CString sCounterPath, CString& sObject, CString& sCounter, CString& sInstance)
{
	CStringArray	asPathElements;
	CString			sObjectAndInstance;

	// Split the counter path string in its components, e.g.: \PhysicalDisk(_Total)\% Disk Time
	if (! Split (TEXT ("\\"), sCounterPath, &asPathElements))
	{
		m_pMainFrame->LogError (TEXT ("SplitPerfPath"), TEXT ("Split failed!"), 0);
		return ERROR_INVALID_FUNCTION;
	}
	if (asPathElements.GetSize () == 3)
	{
		sObjectAndInstance	= asPathElements.GetAt (1);
		sCounter			= asPathElements.GetAt (2);
	}
	else if (asPathElements.GetSize () == 5)
	{
		sObjectAndInstance	= asPathElements.GetAt (3);
		sCounter			= asPathElements.GetAt (4);
	}
	else
	{
		m_pMainFrame->LogError (TEXT ("SplitPerfPath"), TEXT ("Split returned an incorrect number of elements!"), 0);
		return ERROR_INVALID_FUNCTION;
	}

	// Separate object from instance, e.g.: PhysicalDisk(_Total)
	int	nIndex	= sObjectAndInstance.Find ('(');
	if (nIndex != -1)
	{
		sObject		= sObjectAndInstance.Left (nIndex);
		sInstance	= sObjectAndInstance.Mid (nIndex + 1, sObjectAndInstance.GetLength () - nIndex - 2);
	}
	else
	{
		// No instance
		sObject	= sObjectAndInstance;
	}

	return ERROR_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Build a perf path out of path elements
//
////////////////////////////////////////////////////////////////////////////////

DWORD CDialogConfig::BuildPerfPath (CString& sCounterPath, CString sObject, CString sCounter, CString sInstance)
{
	PDH_STATUS			nError			= ERROR_INVALID_FUNCTION;
	PDH_HQUERY			hQuery			= NULL;
	PDH_HCOUNTER		hCounter		= NULL;

	if (sInstance.IsEmpty ())
	{
		sCounterPath	= TEXT ("\\") + sObject + TEXT ("\\") + sCounter;
	}
	else
	{
		sCounterPath	= TEXT ("\\") + sObject + TEXT ("(") + sInstance + TEXT (")") + TEXT ("\\") + sCounter;
	}

	// Check the path we built for validity
	// Open a query handle
	nError	= PdhOpenQuery (NULL, 0, &hQuery);
	if(nError != ERROR_SUCCESS)
	{
		m_pMainFrame->LogError (TEXT ("BuildPerfPath"), TEXT ("PdhOpenQuery"), nError);
		goto CleanUp;
	}

	// Add a counter to the query in order to get a handle to the counter
	nError	= PdhAddCounter (hQuery, sCounterPath, 0, &hCounter);
	if (nError != ERROR_SUCCESS)
	{
		if (nError == PDH_CSTATUS_BAD_COUNTERNAME)
		{
			// If instance is empty, try again with instance set to an asterisk
			if (sInstance.IsEmpty ())
			{
				return BuildPerfPath (sCounterPath, sObject, sCounter, TEXT ("*"));
			}
		}
	}

CleanUp:
	if (hQuery != NULL)
	{
		PdhCloseQuery (hQuery);
		hQuery	= NULL;
	}

	return nError;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Use a selected data source: store the path in the edit box
//
////////////////////////////////////////////////////////////////////////////////

void CDialogConfig::OnBnClickedUseSelectedDataSource()
{
	// Get data from the dialog box into our member variables
	UpdateData (TRUE);

	// Did the user select a valid data source?
	if (m_sPerfObject.IsEmpty ())
	{
		MessageBox (TEXT ("Please select an object first."), TEXT ("DiskLED: Select an object"), MB_OK | MB_ICONEXCLAMATION);
		m_cSelectPerfObjects.SetFocus ();
		return;
	}
	if (m_sPerfCounter.IsEmpty ())
	{
		MessageBox (TEXT ("Please select a counter first."), TEXT ("DiskLED: Select a counter"), MB_OK | MB_ICONEXCLAMATION);
		m_cSelectPerfCounters.SetFocus ();
		return;
	}
	if (m_sPerfInstance.IsEmpty () && m_cSelectPerfInstances.GetCount () > 0)
	{
		MessageBox (TEXT ("Please select an instance first."), TEXT ("DiskLED: Select an instance"), MB_OK | MB_ICONEXCLAMATION);
		m_cSelectPerfInstances.SetFocus ();
		return;
	}

	// Build a path
	// BuildPerfPath (m_sCounterPath, m_sPerfObject, m_sPerfCounter, m_sPerfInstance);

	// Use the path already built (and tested!)
	m_sCounterPath	= m_sSelectedPerfPath;

	// Write the new path to the edit box
	UpdateData (FALSE);
}


////////////////////////////////////////////////////////////////////////////////
//
//	Get the explanatory text / description of a counter
//
////////////////////////////////////////////////////////////////////////////////

DWORD CDialogConfig::GetCounterExplainText (CString sCounterPath, CString& sExplainText)
{
	PDH_STATUS			nError			= ERROR_INVALID_FUNCTION;
	PDH_HQUERY			hQuery			= NULL;
	PDH_HCOUNTER		hCounter		= NULL;
	DWORD				nBufferInfo		= 0;

	// Zero earlier allocated buffers
	if (m_oBufferInfo)
	{
		SecureZeroMemory (m_oBufferInfo, m_nBufferInfo);
	}

	// Open a query handle
	nError	= PdhOpenQuery (NULL, 0, &hQuery);
	if(nError != ERROR_SUCCESS)
	{
		m_pMainFrame->LogError (TEXT ("GetCounterExplainText"), TEXT ("PdhOpenQuery"), nError);
		goto CleanUp;
	}

	// Add a counter to the query in order to get a handle to the counter
	nError	= PdhAddCounter (hQuery, sCounterPath, 0, &hCounter);
	if (nError != ERROR_SUCCESS)
	{
		CString	sExplanation;
		if (nError == PDH_CSTATUS_NO_OBJECT || nError == PDH_CSTATUS_NO_COUNTER)
		{
			sExplanation	= TEXT ("This probably means that the performance counter path is invalid.\nPlease take note that it must be localized. The default path used (if no config file is specified) is valid on English systems only.");
		}
		else if (nError == PDH_CSTATUS_BAD_COUNTERNAME)
		{
			sExplanation	= TEXT ("This probably means that the counter path built by DiskLED is invalid: <") + sCounterPath + TEXT (">");
		}
		m_pMainFrame->LogError (TEXT ("GetCounterExplainText"), TEXT ("PdhAddCounter"), nError, sExplanation);
		goto CleanUp;
	}

	// Get required buffer size
	nError	= PdhGetCounterInfo (hCounter, TRUE, &nBufferInfo, m_oBufferInfo);
	if (nError != PDH_MORE_DATA)
	{
		m_pMainFrame->LogError (TEXT ("GetCounterExplainText"), TEXT ("PdhGetCounterInfo (1)"), nError);
		goto CleanUp;
	}

	//
	// Allocate memory if necessary
	//
	if (nBufferInfo > m_nBufferInfo)
	{
		// Free memory of previously allocated
		if (m_oBufferInfo)
		{
			HeapFree (GetProcessHeap (), 0, m_oBufferInfo);
			m_oBufferInfo	= NULL;
		}

		// Allocate more than required
		m_nBufferInfo	= nBufferInfo * 2;
		nBufferInfo		= m_nBufferInfo;

		m_oBufferInfo	= (PDH_COUNTER_INFO*) HeapAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, m_nBufferInfo);
		if (m_oBufferInfo == NULL)
		{
			m_pMainFrame->LogError (TEXT ("GetCounterExplainText"), TEXT ("Memory allocation failed!"), 0);
			goto CleanUp;
		}
	}

	// Get information on the counter
	nError	= PdhGetCounterInfo (hCounter, TRUE, &nBufferInfo, m_oBufferInfo);
	if (nError != ERROR_SUCCESS)
	{
		m_pMainFrame->LogError (TEXT ("GetCounterExplainText"), TEXT ("PdhGetCounterInfo (2)"), nError);
		goto CleanUp;
	}

	// Store the out value
	sExplainText	= m_oBufferInfo->szExplainText;

	// For V2 providers (HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Perflib\_V2Providers)
	// the explain text is not returned. With these, nError is ERROR_SUCCESS, but szExplainText is empty.
	if (sExplainText.IsEmpty ())
	{
		sExplainText	= TEXT ("<Could not retrieve the description.>");
	}

CleanUp:
	if (hQuery != NULL)
	{
		PdhCloseQuery (hQuery);
		hQuery	= NULL;
	}

	return nError;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Gets called whenever a new counter is selected.
//	Updates the description
//
////////////////////////////////////////////////////////////////////////////////

void CDialogConfig::OnCbnSelchangePerfCounters()
{
	PDH_STATUS	nError			= ERROR_INVALID_FUNCTION;
	CString		sCounterPath;
	CString		sExplainText;
	CString		sInstance;

	// Get data from the dialog box into our member variables
	UpdateData (TRUE);

	// By default, use the instance from the combo box
	sInstance	= m_sPerfInstance;

	// Is this combo box empty? If so, empty the description.
	if (m_sPerfCounter.IsEmpty ())
	{
		nError	= ERROR_INVALID_DATA;
		goto Finished;
	}

	if (sInstance.IsEmpty () && m_cSelectPerfInstances.GetCount () > 0)
	{
		// This counter has instances, but none is selected. Use any instance to build a valid counter path
		m_cSelectPerfInstances.GetLBText (0, sInstance);
	}

	//
	// Get the description of the counter
	//

	// Build a perf path
	nError	= BuildPerfPath (sCounterPath, m_sPerfObject, m_sPerfCounter, sInstance);
	if (nError != ERROR_SUCCESS)
	{
		goto Finished;
	}

	// Set the description
	nError	= GetCounterExplainText (sCounterPath, sExplainText);

Finished:
	if (nError == ERROR_SUCCESS)
	{
		m_sPerfDescription	= sExplainText;
		m_sSelectedPerfPath	= sCounterPath;
	}
	else
	{
		m_sPerfDescription	= TEXT ("");
		m_sSelectedPerfPath	= TEXT ("");
	}

	// Update the UI with data from the member variables
	UpdateData (FALSE);
}


////////////////////////////////////////////////////////////////////////////////
//
//	Gets called whenever a new instance is selected.
//	Updates the description
//
////////////////////////////////////////////////////////////////////////////////

void CDialogConfig::OnCbnSelchangePerfInstances ()
{
	PDH_STATUS				nError	= ERROR_INVALID_FUNCTION;
	CString	sCounterPath;
	CString sExplainText;

	// Get data into member variables
	UpdateData (TRUE);

	// Is this combo box empty? If so, empty the description.
	if (m_sPerfInstance.IsEmpty () && m_cSelectPerfInstances.GetCount () > 0)
	{
		nError	= ERROR_INVALID_DATA;
		goto Finished;
	}

	if (m_sPerfCounter.IsEmpty ())
	{
		// If no counter is selected, we cannot build a valid counter path
		return;
	}

	//
	// Get the description of the counter
	//

	// Build a perf path
	nError	= BuildPerfPath (sCounterPath, m_sPerfObject, m_sPerfCounter, m_sPerfInstance);
	if (nError != ERROR_SUCCESS)
	{
		goto Finished;
	}

	// Set the description
	nError	= GetCounterExplainText (sCounterPath, sExplainText);

Finished:
	if (nError == ERROR_SUCCESS)
	{
		m_sPerfDescription	= sExplainText;
		m_sSelectedPerfPath	= sCounterPath;
	}
	else
	{
		m_sPerfDescription	= TEXT ("");
		m_sSelectedPerfPath	= TEXT ("");
	}

	// Update the UI with data from the member variables
	UpdateData (FALSE);
}
