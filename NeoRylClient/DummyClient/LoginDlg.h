#pragma once
#include "afxcmn.h"


// CLoginDlg dialog

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoginDlg();

    void ClearAuthInfo() { m_szAccount = "", m_szPassword = ""; }

// Dialog Data
	enum { IDD = IDD_DLG_LOGIN };

protected:
	
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnLogin();

    DWORD GetCheckSum()      
    {        
        char* szEndPtr = 0; 
        return strtoul(m_szCheckSum.GetBuffer(0), &szEndPtr, 16);
    }

    DWORD GetClientVersion() { return m_dwClientVersion; }

    CString GetAccount()        { return m_szAccount; }
    CString GetPassword()       { return m_szPassword; }
    CString GetAuthServerIP()   { return m_AuthServerIP; }

private:

    void MoveToCenter();

    CString         m_szAccount;
    CString         m_szPassword;
    CString         m_AuthServerIP;
    CString         m_szCheckSum;
    DWORD           m_dwClientVersion;

    LONG            m_nLoopCount;
    
public:

    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
};
