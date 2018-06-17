// DialogAbout.cpp : implementation file
//

#include "stdafx.h"
#include "DiskLED.h"
#include "DialogAbout.h"
#include "commctrl.h"


// CDialogAbout dialog

IMPLEMENT_DYNAMIC(CDialogAbout, CDialog)

CDialogAbout::CDialogAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAbout::IDD, pParent)
{

}

CDialogAbout::~CDialogAbout()
{
}

void CDialogAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogAbout, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_URL1, &CDialogAbout::OnNMClickSyslinkUrl1)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_URL2, &CDialogAbout::OnNMClickSyslinkUrl2)
END_MESSAGE_MAP()


// CDialogAbout message handlers

void CDialogAbout::OnNMClickSyslinkUrl1(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK) pNMHDR;

	ShellExecute (NULL, TEXT ("open"), pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);

	*pResult = 0;
}

void CDialogAbout::OnNMClickSyslinkUrl2(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK) pNMHDR;

	ShellExecute (NULL, TEXT ("open"), pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);

	*pResult = 0;
}
