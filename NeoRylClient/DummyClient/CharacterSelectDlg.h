#pragma once
#include "afxcmn.h"


// CCharacterSelectDlg dialog

// forward decl.
class CCharacterData;

class CCharacterSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CCharacterSelectDlg)

public:

    static CCharacterSelectDlg* CreateSelectDlg(CCharacterData& characterData);

	void InsertData(LPCTSTR lpszName, 
        unsigned long dwUID, unsigned long dwCID,
        char cLevel, unsigned long dwFame, unsigned short wClass);

    void ClearData();
    void MoveToCenter();

// Dialog Data
	enum { IDD = IDD_DLG_CHARSELECT };

protected:

    CCharacterSelectDlg(CCharacterData& characterData, CWnd* pParent = NULL);   // standard constructor
    virtual ~CCharacterSelectDlg();

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:

	CListCtrl       m_ctrlCharacterSelect;
    CCharacterData& m_CharacterData;

    afx_msg void OnNcDestroy();
public:
    afx_msg void OnNMDblclkCtrlCharselect(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedOk();
};
