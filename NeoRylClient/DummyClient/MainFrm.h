// MainFrm.h : CMainFrame Ŭ������ �������̽�
//


#pragma once

class CGuildManager;
class CCharacterDataMgr;

namespace ClientNet
{
    class CSessionMgr;
}

class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.

	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư��
private:

    unsigned long           m_dwLastPingTime;

    CGuildManager*	        m_lpGuildManager;
    CCharacterDataMgr*      m_lpCharacterDataMgr;

//�۾�
public:
    
    CGuildManager&          GetGuildManager()       { return *m_lpGuildManager;     }
    CCharacterDataMgr&      GetCharacterDataMgr()   { return *m_lpCharacterDataMgr; }

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ����
public:
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.

	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

    // �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnFileNew();
    afx_msg void OnTimer(UINT nIDEvent);
protected:
    virtual void PostNcDestroy();
public:
    afx_msg void OnCharNameChange();
};


