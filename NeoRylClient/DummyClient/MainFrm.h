// MainFrm.h : CMainFrame 클래스의 인터페이스
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
	
protected: // serialization에서만 만들어집니다.

	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성
private:

    unsigned long           m_dwLastPingTime;

    CGuildManager*	        m_lpGuildManager;
    CCharacterDataMgr*      m_lpCharacterDataMgr;

//작업
public:
    
    CGuildManager&          GetGuildManager()       { return *m_lpGuildManager;     }
    CCharacterDataMgr&      GetCharacterDataMgr()   { return *m_lpCharacterDataMgr; }

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현
public:
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.

	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

    // 메시지 맵 함수를 생성했습니다.
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


