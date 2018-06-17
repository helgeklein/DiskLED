
#include <pdh.h>
#include <PDHMsg.h>


#pragma comment(lib, "pdh.lib")

// LED type constants
#define	LEDTYPE_FLICKER				1
#define	LEDTYPE_FLICKER_TEXT		TEXT ("Flicker")
#define	LEDTYPE_COLOREDBAR			2
#define	LEDTYPE_COLOREDBAR_TEXT		TEXT ("ColoredBar")

// The name of the program
#define PROGRAM_NAME				TEXT ("DiskLED")

// Default value for first LED's type
#define DEFAULT_COUNTER_1_LEDTYPE	LEDTYPE_FLICKER

// Default value for the performance 1 counter path
#define DEFAULT_COUNTER_1_PATH		TEXT ("\\PhysicalDisk(_Total)\\% Disk Time")

// Default value for the performance 1 counter update interval
#define DEFAULT_COUNTER_1_MS		30

// Default value for the performance 1 counter 100% utilization
#define DEFAULT_COUNTER_1_100PERCENT	100

// How many different icons do we use for state?
#define NUM_ICONS					5

// Name of the INI (settings) file
#define INI_SETTINGS_FILE			TEXT ("DiskLED.ini")

// INI section name of first performance counter
#define INI_COUNTER_1_SECTION		TEXT ("Performance Counter 1")

// INI value name of first performance counter LED type
#define INI_COUNTER_1_VALUE_LEDTYPE				TEXT("LEDType")

// INI value name of first performance counter path
#define INI_COUNTER_1_VALUE_PATH				TEXT("Path")

// INI value name of first performance counter update frequency
#define INI_COUNTER_1_VALUE_UPDATE_INTERVAL		TEXT("UpdateInterval")

// INI value name of first performance counter maximum value
#define INI_COUNTER_1_VALUE_100PERCENT			TEXT("100% Utilization")


class CMainFrame : public CFrameWnd
{
// Operations
public:
	CMainFrame();

	DECLARE_DYNCREATE(CMainFrame)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	// Member functions
	DWORD	Initialize (CString sCounterPath = TEXT (""), DWORD nLEDType = 0, DWORD nTimerInterval = 0, double dCounter_100Percent = 0.0);
	void	GetLEDDefaults (CString& sCounterPath, int& nLEDType, DWORD& nTimerInterval, double& dCounter_100Percent);
	int		GetLEDType ();
	DWORD	GetUpdateFrequency ();
	double	Get100PercentUtilization ();
	void	GetCounterPath (CString& sCounterPath);
	void	GetINIFile (CString& sINIFile);
	void	LogError (CString sCaller, CString sFailedComponent, DWORD nError, CString sHelpMessage = TEXT(""));
	
private:

	// Member variables
	CSystemTray				m_TrayIcon;
	CMenu					m_Menu;					// popup menu
	UINT_PTR				m_TimerID;				// Timer ID
	UINT					m_nCurrentIcon;
	PDH_HQUERY				m_hQuery;
	PDH_HCOUNTER			m_hCounter;
	PDH_FMT_COUNTERVALUE	m_oCounterVal;
	CString					m_sInstallationDirectory;
	CString					m_sExecutablePath;
	CString					m_sINIFile;

	DWORD					m_nLED1Type;					// Which type of LED to display
	DWORD					m_nTimerIntervalMS;				// Timer interval in ms
	CString					m_sCounter1Path;				// Path of first performance counter
	double					m_dCounter1_100Percent;			// 100% mark for first performance counter

	// Member functions
	void					ShowConfigurationDialog ();
	void					ShowAboutDialog ();
	void					Shutdown ();
	DWORD					StartTimer ();
	DWORD					InitPDH ();
	DWORD					ClosePDH ();
	DWORD					ReadSettings ();



// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg LRESULT OnPowerBroadcast(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
