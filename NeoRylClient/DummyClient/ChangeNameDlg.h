#pragma once


// CChangeNameDlg ��ȭ �����Դϴ�.

class CChangeNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CChangeNameDlg)

public:
	CChangeNameDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CChangeNameDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CHANGE_NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
private:
    CString m_szOldName;
    CString m_szNewName;
protected:
    virtual void OnOK();
};
