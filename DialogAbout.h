#pragma once


// CDialogAbout dialog

class CDialogAbout : public CDialog
{
	DECLARE_DYNAMIC(CDialogAbout)

public:
	CDialogAbout(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogAbout();

// Dialog Data
	enum { IDD = IDD_DIALOG_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickSyslinkUrl1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkUrl2(NMHDR *pNMHDR, LRESULT *pResult);
};
