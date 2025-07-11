// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "DummyClient.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#include "LoginDlg.h"
#include "CharacterSelectDlg.h"
#include "GuildManager.h"
#include "CharacterData.h"
#include "ChangeNameDlg.h"

#include <mmsystem.h>
#include <Network/ClientNetwork/SessionMgr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

const int g_nTimerID_Network = 1;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SHOWWINDOW()
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
    ON_WM_TIMER()
    ON_COMMAND(ID_CHAR_NAME_CHANGE, OnCharNameChange)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
:   m_lpGuildManager(0), 
    m_lpCharacterDataMgr(0),
    m_dwLastPingTime(timeGetTime())
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
	if ( 0 != m_lpGuildManager )
	{
		delete m_lpGuildManager ;
		m_lpGuildManager = 0 ;
	}

    if ( 0 != m_lpCharacterDataMgr )
    {
        delete m_lpCharacterDataMgr ;
        m_lpCharacterDataMgr = 0 ;
    }
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	
	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	*/

	m_lpGuildManager = new CGuildManager ;
    if (0 == m_lpGuildManager)
    {
        return -1;
    }

    m_lpCharacterDataMgr = new CCharacterDataMgr ;
    if (0 == m_lpCharacterDataMgr)
    {
        return -1;
    }

    SetTimer(g_nTimerID_Network, 100, 0);
    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기


void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	//CFrameWnd::OnSetFocus(pOldWnd);

    // TODO: Add your message handler code here
	
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFrameWnd::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
}

void CMainFrame::OnFileNew()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.

    CCharacterData* lpCharacterData = new CCharacterData;

    if(0 != lpCharacterData)
    {
        if(!lpCharacterData->Login(this) || 
            !m_lpCharacterDataMgr->AddByAccount(lpCharacterData))
        {            
            delete lpCharacterData;
        }
    }
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if(nIDEvent == g_nTimerID_Network)
    {
        m_lpCharacterDataMgr->HandleEvents(100);
    }    

    CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::PostNcDestroy()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    KillTimer(g_nTimerID_Network);
    CFrameWnd::PostNcDestroy();
}

void CMainFrame::OnCharNameChange()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    CChangeNameDlg nameDlg;
    nameDlg.DoModal();
}
