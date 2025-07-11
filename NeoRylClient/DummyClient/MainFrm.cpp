// MainFrm.cpp : CMainFrame Ŭ������ ����
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
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
:   m_lpGuildManager(0), 
    m_lpCharacterDataMgr(0),
    m_dwLastPingTime(timeGetTime())
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
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
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	
	// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
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
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return TRUE;
}


// CMainFrame ����

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


// CMainFrame �޽��� ó����


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
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

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
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    if(nIDEvent == g_nTimerID_Network)
    {
        m_lpCharacterDataMgr->HandleEvents(100);
    }    

    CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::PostNcDestroy()
{
    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
    KillTimer(g_nTimerID_Network);
    CFrameWnd::PostNcDestroy();
}

void CMainFrame::OnCharNameChange()
{
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
    CChangeNameDlg nameDlg;
    nameDlg.DoModal();
}
