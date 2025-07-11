// CommonOption.cpp: implementation of the CCommonOption class.
//
//////////////////////////////////////////////////////////////////////

#include "RYLClientMain.h"
#include "CommonOption.h"
#include "WinInput.h"

#include <Network/ClientSocket/Send/SendEtc.h>
//====================================================================
#include "RYLGameData.h"
#include "RYLNetworkData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static RejectOption m_roOption;
static ClientOption m_coOption;

CCommonOption::CCommonOption()
{

}

CCommonOption::~CCommonOption()
{

}

void CCommonOption::InitValue()
{
	m_dwShowOption = 0;
	m_dwStartMusic = 0;
	m_dwStartSound = 0;

	ShowOptionWindow(0);
}

void CCommonOption::ShowOptionWindow(unsigned long dwKind)
{
	m_dwShowOption = dwKind;

	switch(m_dwShowOption)
	{
		case 0:
			m_ButtonManager.m_lstButton[0]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[1]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[2]->ShowButton(TRUE);

			m_ButtonManager.m_lstButton[3]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[4]->ShowButton(FALSE);

			m_ButtonManager.m_lstButton[5]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[6]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[7]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[8]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[9]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[10]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[11]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[12]->ShowButton(FALSE);

			m_ButtonManager.m_lstButton[13]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[14]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[15]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[16]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[17]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[18]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[19]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[20]->ShowButton(FALSE);
			break;

		case 1:
			m_ButtonManager.m_lstButton[0]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[1]->ShowButton(FALSE);
			m_ButtonManager.m_lstButton[2]->ShowButton(FALSE);

			m_ButtonManager.m_lstButton[3]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[4]->ShowButton(TRUE);

			m_ButtonManager.m_lstButton[5]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[6]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[7]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[8]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[9]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[10]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[11]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[12]->ShowButton(TRUE);

			m_ButtonManager.m_lstButton[13]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[14]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[15]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[16]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[17]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[18]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[19]->ShowButton(TRUE);
			m_ButtonManager.m_lstButton[20]->ShowButton(TRUE);
			break;
	}
}

BOOL CCommonOption::Init(unsigned short x, unsigned short y)
{
	CRYLSprite *lpSprite;
	lpSprite = new CRYLSprite;
	lpSprite->Create(x + 145, y + 55, 0, 0, 222, 217, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[19]);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;
	lpSprite->Create(x, y, 0, 0, 256, 256, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;
	lpSprite->Create(x + 256, y, 0, 0, 256, 256, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[40]);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;
	lpSprite->Create(x, y + 256, 0, 0, 256, 152, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	m_lstSprite.push_back(lpSprite);

	lpSprite = new CRYLSprite;
	lpSprite->Create(x + 256, y + 256, 0, 0, 256, 152, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[58]);
	m_lstSprite.push_back(lpSprite);

	// 5 = 캐릭터 이름 X
	lpSprite = new CRYLSprite;
	lpSprite->Create(0, 0, 0, 179, 18, 196, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	m_lstSprite.push_back(lpSprite);

	// 6 = 스킬 이펙트 X
	lpSprite = new CRYLSprite;
	lpSprite->Create(0, 0, 0, 179, 18, 196, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	m_lstSprite.push_back(lpSprite);

	// 7 = 챈트 이펙트 X
	lpSprite = new CRYLSprite;
	lpSprite->Create(0, 0, 0, 179, 18, 196, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	m_lstSprite.push_back(lpSprite);

	// 8 = 무기 이펙트 X
	lpSprite = new CRYLSprite;
	lpSprite->Create(0, 0, 0, 179, 18, 196, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	m_lstSprite.push_back(lpSprite);

	CGUIButton *lpButton;
	// 게임 끝내기
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_NORMAL;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(41, 102, 41, 102, 182, 118, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[19]);
	lpButton->m_lpOnMouseButton = new CRYLSprite;
	lpButton->m_lpOnMouseButton->Create(41, 102, 0, 32, 141, 48, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[20]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(41, 102, 0, 112, 141, 128, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[20]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x + 145, y + 55);
	m_ButtonManager.AddButton(lpButton);

	// 세부 옵션
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_NORMAL;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(41, 136, 41, 136, 182, 152, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[19]);
	lpButton->m_lpOnMouseButton = new CRYLSprite;
	lpButton->m_lpOnMouseButton->Create(41, 136, 0, 48, 141, 64, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[20]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(41, 136, 0, 128, 141, 144, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[20]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x + 145, y + 55);
	m_ButtonManager.AddButton(lpButton);

	// 이전 게임으로
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_NORMAL;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(41, 172, 41, 172, 182, 188, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[19]);
	lpButton->m_lpOnMouseButton = new CRYLSprite;
	lpButton->m_lpOnMouseButton->Create(41, 172, 0, 64, 141, 80, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[20]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(41, 172, 0, 144, 141, 160, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[20]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x + 145, y + 55);
	m_ButtonManager.AddButton(lpButton);

	// 확인
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLE;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(176, 363, 176, 107, 244, 133, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(176, 363, 0, 152, 68, 178, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	// 취소
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLE;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(265, 363, 9, 107, 77, 133, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[58]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(265, 363, 0, 197, 68, 223, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	// 5 = 1:1 신청거부 ON
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(106, 90, 106, 90, 133, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(106, 90, 68, 152, 95, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(133, 90, 133, 90, 160, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(133, 90, 95, 152, 122, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	// 7 = 거래신청거부 ON
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(106, 110, 106, 90, 133, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(106, 110, 68, 152, 95, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(133, 110, 133, 90, 160, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(133, 110, 95, 152, 122, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	// 9 = 파티신청거부 ON
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(106, 129, 106, 90, 133, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(106, 129, 68, 152, 95, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(133, 129, 133, 90, 160, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(133, 129, 95, 152, 122, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	// 11 = 친구신청거부 ON
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(106, 148, 106, 90, 133, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(106, 148, 68, 152, 95, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(133, 148, 133, 90, 160, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(133, 148, 95, 152, 122, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	// 13 = 귓속말거부 ON
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(267, 90, 106, 90, 133, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(267, 90, 68, 152, 95, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(294, 90, 133, 90, 160, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(294, 90, 95, 152, 122, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	// 15 = 외치기거부 ON
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(267, 110, 106, 90, 133, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(267, 110, 68, 152, 95, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(294, 110, 133, 90, 160, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(294, 110, 95, 152, 122, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	// 17 = 도움말거부 ON
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(267, 129, 106, 90, 133, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(267, 129, 68, 152, 95, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(294, 129, 133, 90, 160, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(294, 129, 95, 152, 122, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	// 19 = 일반대화거부 ON
	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(267, 148, 106, 90, 133, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(267, 148, 68, 152, 95, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

	lpButton = new CGUIButton;
	lpButton->m_nMethod = BUTTON_SIMPLECHECK;
	lpButton->m_lpNormalButton = new CRYLSprite;
	lpButton->m_lpNormalButton->Create(294, 148, 133, 90, 160, 103, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpButton->m_lpClickMouseButton = new CRYLSprite;
	lpButton->m_lpClickMouseButton->Create(294, 148, 95, 152, 122, 165, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[57]);
	lpButton->SetTooltip(&m_TooltipManager, "");
	lpButton->SetMove(x, y);
	m_ButtonManager.AddButton(lpButton);

/*	CGUIScroll *lpScroll;
	lpScroll = new CGUIScroll;
	lpScroll->m_lpBarButton = new CGUIButton;
	lpScroll->m_lpBarButton->m_nMethod = BUTTON_SIMPLE;
	lpScroll->m_lpBarButton->m_lpNormalButton = new CRYLSprite;
	lpScroll->m_lpBarButton->m_lpNormalButton->Create(83, 105, 195, 220, 211, 229, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpScroll->m_lpBarButton->m_lpClickMouseButton = new CRYLSprite;
	lpScroll->m_lpBarButton->m_lpClickMouseButton->Create(83, 105, 195, 220, 211, 229, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);

	lpScroll->m_lValue = (long *)&m_dwStartMusic;
	lpScroll->SetScrollKind(SCROLL_HSCROLL);
	lpScroll->SetMove(x, y);
	lpScroll->SetPosition(83, 105);
	lpScroll->SetEdge(0, 0, 102, 11);
	lpScroll->SetRange(0, 6);
	m_lstScroll.push_back(lpScroll);

	lpScroll = new CGUIScroll;
	lpScroll->m_lpBarButton = new CGUIButton;
	lpScroll->m_lpBarButton->m_nMethod = BUTTON_SIMPLE;
	lpScroll->m_lpBarButton->m_lpNormalButton = new CRYLSprite;
	lpScroll->m_lpBarButton->m_lpNormalButton->Create(83, 127, 195, 220, 211, 229, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);
	lpScroll->m_lpBarButton->m_lpClickMouseButton = new CRYLSprite;
	lpScroll->m_lpBarButton->m_lpClickMouseButton->Create(83, 127, 195, 220, 211, 229, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[21]);

	lpScroll->m_lValue = (long *)&m_dwStartMusic;
	lpScroll->SetScrollKind(SCROLL_HSCROLL);
	lpScroll->SetMove(x, y);
	lpScroll->SetPosition(83, 127);
	lpScroll->SetEdge(0, 0, 102, 11);
	lpScroll->SetRange(0, 6);
	m_lstScroll.push_back(lpScroll);*/

	m_bShow = FALSE;

	m_dwWindowKind = WINDOW_FIX;

	m_rcWindowSize.left = x;					m_rcWindowSize.top = y;
	m_rcWindowSize.right = x + 512;				m_rcWindowSize.bottom = y + 408;

	m_rcMoveEdge.left = x;						m_rcMoveEdge.top = y;
	m_rcMoveEdge.right = x + 512;				m_rcMoveEdge.bottom = y + 20;

	InitValue();

	return TRUE;
}

void CCommonOption::ShowWindow(BOOL bShow)
{
	m_bShow = bShow;

	InitValue();

	if(m_bShow)
	{
		m_roOption = CRYLGameData::Instance()->m_roOption;
		m_coOption = CRYLGameData::Instance()->m_coOption;
	}
}

BOOL CCommonOption::Update(BOOL &bClick, BOOL &bEdge)
{
	if(g_DeviceInput.InRect(m_rcWindowSize.left, m_rcWindowSize.top, m_rcWindowSize.right, m_rcWindowSize.bottom))
	{
		bEdge = TRUE;

		if(g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}

	m_ButtonManager.Update();
	m_TooltipManager.Update();

	switch(m_dwShowOption)
	{
		case 0:	// 메인 메뉴
			if(m_ButtonManager.m_lstButton[0]->m_nState == BUTTON_CLICK)
			{
				SendMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
				CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;

				return FALSE;
			} else if(m_ButtonManager.m_lstButton[1]->m_nState == BUTTON_CLICK)
			{
				ShowOptionWindow(1);
			} else if(m_ButtonManager.m_lstButton[2]->m_nState == BUTTON_CLICK)
			{
				ShowWindow(FALSE);
			}
			break;

		case 1:	// 세부 옵션
			if(m_ButtonManager.m_lstButton[3]->m_nState == BUTTON_CLICK)
			{
				char strFileName[MAX_PATH];
				sprintf(strFileName, "%s\\ClientOption.dat", g_ClientMain.m_strClientPath);
				CRYLGameData::Instance()->SaveClientOption(strFileName, &m_coOption);
				SendPacket::CharControlOption(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_roOption);

				ShowOptionWindow(0);
			} else if(m_ButtonManager.m_lstButton[4]->m_nState == BUTTON_CLICK)
			{
				ShowOptionWindow(0);
			}

			SelectOption();

/*			m_lstScroll[0]->Update();
			m_lstScroll[1]->Update();*/
			break;
	}

	return TRUE;
}

void CCommonOption::SelectOption(void)
{
	if(g_DeviceInput.GetIsLeftMouseDown())
	{
		if(g_DeviceInput.InRect(m_rcWindowSize.left + 106, m_rcWindowSize.top + 90, m_rcWindowSize.left + 133, m_rcWindowSize.top + 103))
		{
			m_roOption.Reject.m_Duel = 1;				// 1:1 신청 ON
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 133, m_rcWindowSize.top + 90, m_rcWindowSize.left + 160, m_rcWindowSize.top + 103))
		{
			m_roOption.Reject.m_Duel = 0;				// 1:1 신청 OFF
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 106, m_rcWindowSize.top + 110, m_rcWindowSize.left + 133, m_rcWindowSize.top + 123))
		{
			m_roOption.Reject.m_Exchange = 1;			// 교환 ON
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 133, m_rcWindowSize.top + 110, m_rcWindowSize.left + 160, m_rcWindowSize.top + 123))
		{
			m_roOption.Reject.m_Exchange = 0;			// 교환 OFF
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 106, m_rcWindowSize.top + 129, m_rcWindowSize.left + 133, m_rcWindowSize.top + 142))
		{
			m_roOption.Reject.m_Party = 1;				// 파티 ON
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 133, m_rcWindowSize.top + 129, m_rcWindowSize.left + 160, m_rcWindowSize.top + 142))
		{
			m_roOption.Reject.m_Party = 0;				// 파티 OFF
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 106, m_rcWindowSize.top + 148, m_rcWindowSize.left + 133, m_rcWindowSize.top + 161))
		{
			m_roOption.Reject.m_Friend = 1;				// 친구 ON
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 133, m_rcWindowSize.top + 148, m_rcWindowSize.left + 160, m_rcWindowSize.top + 161))
		{
			m_roOption.Reject.m_Friend = 0;				// 친구 OFF
			CRYLGameData::Instance()->PlayClickSound();
		}

		if(g_DeviceInput.InRect(m_rcWindowSize.left + 267, m_rcWindowSize.top + 90, m_rcWindowSize.left + 294, m_rcWindowSize.top + 103))
		{
			m_roOption.Reject.m_Whisper = 0;			// 귓속말 ON
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 294, m_rcWindowSize.top + 90, m_rcWindowSize.left + 321, m_rcWindowSize.top + 103))
		{
			m_roOption.Reject.m_Whisper = 1;			// 귓속말 OFF
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 267, m_rcWindowSize.top + 110, m_rcWindowSize.left + 294, m_rcWindowSize.top + 123))
		{
			m_roOption.Reject.m_Shout = 0;				// 외치기 ON
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 294, m_rcWindowSize.top + 110, m_rcWindowSize.left + 321, m_rcWindowSize.top + 123))
		{
			m_roOption.Reject.m_Shout = 1;				// 외치기 OFF
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 267, m_rcWindowSize.top + 129, m_rcWindowSize.left + 294, m_rcWindowSize.top + 142))
		{
			m_coOption.m_bTipBroad = TRUE;				// 도움말 ON
			CRYLGameData::Instance()->PlayClickSound();
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 294, m_rcWindowSize.top + 129, m_rcWindowSize.left + 321, m_rcWindowSize.top + 142))
		{
			m_coOption.m_bTipBroad = FALSE;				// 도움말 OFF
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 267, m_rcWindowSize.top + 148, m_rcWindowSize.left + 294, m_rcWindowSize.top + 161))
		{
			m_coOption.m_bChat = TRUE;					// 대화 ON
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 294, m_rcWindowSize.top + 148, m_rcWindowSize.left + 321, m_rcWindowSize.top + 161))
		{
			m_coOption.m_bChat = FALSE;					// 대화 OFF
			CRYLGameData::Instance()->PlayClickSound() ;
		}

		// 캐릭터 이름 옵션
		if(g_DeviceInput.InRect(m_rcWindowSize.left + 453, m_rcWindowSize.top + 89, m_rcWindowSize.left + 471, m_rcWindowSize.top + 105))
		{
			m_coOption.m_dwCharName = 0;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 453, m_rcWindowSize.top + 108, m_rcWindowSize.left + 471, m_rcWindowSize.top + 124))
		{
			m_coOption.m_dwCharName = 1;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 453, m_rcWindowSize.top + 127, m_rcWindowSize.left + 471, m_rcWindowSize.top + 143))
		{
			m_coOption.m_dwCharName = 2;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 453, m_rcWindowSize.top + 146, m_rcWindowSize.left + 471, m_rcWindowSize.top + 162))
		{
			m_coOption.m_dwCharName = 3;
			CRYLGameData::Instance()->PlayClickSound() ;
		}

		// 스킬 이펙트 옵션
		if(g_DeviceInput.InRect(m_rcWindowSize.left + 141, m_rcWindowSize.top + 240, m_rcWindowSize.left + 158, m_rcWindowSize.top + 256))
		{
			m_coOption.m_dwSkillEffect = 0;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 141, m_rcWindowSize.top + 260, m_rcWindowSize.left + 158, m_rcWindowSize.top + 276))
		{
			m_coOption.m_dwSkillEffect = 1;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 141, m_rcWindowSize.top + 280, m_rcWindowSize.left + 158, m_rcWindowSize.top + 296))
		{
			m_coOption.m_dwSkillEffect = 2;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 141, m_rcWindowSize.top + 300, m_rcWindowSize.left + 158, m_rcWindowSize.top + 316))
		{
			m_coOption.m_dwSkillEffect = 3;
			CRYLGameData::Instance()->PlayClickSound() ;
		}

		// 챈트 이펙트 옵션
		if(g_DeviceInput.InRect(m_rcWindowSize.left + 297, m_rcWindowSize.top + 240, m_rcWindowSize.left + 314, m_rcWindowSize.top + 256))
		{
			m_coOption.m_dwChantEffect = 0;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 297, m_rcWindowSize.top + 260, m_rcWindowSize.left + 314, m_rcWindowSize.top + 276))
		{
			m_coOption.m_dwChantEffect = 1;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 297, m_rcWindowSize.top + 280, m_rcWindowSize.left + 314, m_rcWindowSize.top + 296))
		{
			m_coOption.m_dwChantEffect = 2;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 297, m_rcWindowSize.top + 300, m_rcWindowSize.left + 314, m_rcWindowSize.top + 316))
		{
			m_coOption.m_dwChantEffect = 3;
			CRYLGameData::Instance()->PlayClickSound() ;
		}

		// 무기 이펙트 옵션
		if(g_DeviceInput.InRect(m_rcWindowSize.left + 453, m_rcWindowSize.top + 240, m_rcWindowSize.left + 470, m_rcWindowSize.top + 256))
		{
			m_coOption.m_dwWeaponEffect = 0;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 453, m_rcWindowSize.top + 260, m_rcWindowSize.left + 470, m_rcWindowSize.top + 276))
		{
			m_coOption.m_dwWeaponEffect = 1;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 453, m_rcWindowSize.top + 280, m_rcWindowSize.left + 470, m_rcWindowSize.top + 296))
		{
			m_coOption.m_dwWeaponEffect = 2;
			CRYLGameData::Instance()->PlayClickSound() ;
		} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 453, m_rcWindowSize.top + 300, m_rcWindowSize.left + 470, m_rcWindowSize.top + 316))
		{
			m_coOption.m_dwWeaponEffect = 3;
			CRYLGameData::Instance()->PlayClickSound() ;
		}

	}

/*	if(g_DeviceInput.InRect(m_rcWindowSize.left + 30, m_rcWindowSize.top + 169, m_rcWindowSize.left + 48, m_rcWindowSize.top + 186) && g_DeviceInput.GetIsLeftMouseDown())			// 파티 거부
	{
		RejectOption roOption;
		roOption.m_wReject = g_ClientMain.m_roOption.m_wReject;
		if(roOption.Reject.m_Party)
			roOption.Reject.m_Party = 0;
		else
			roOption.Reject.m_Party = 1;

		SendPacket::CharControlOption(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, roOption);
	} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 30, m_rcWindowSize.top + 193, m_rcWindowSize.left + 48, m_rcWindowSize.top + 210) && g_DeviceInput.GetIsLeftMouseDown())		// 교환 거부
	{
		RejectOption roOption;
		roOption.m_wReject = g_ClientMain.m_roOption.m_wReject;
		if(roOption.Reject.m_Exchange)
			roOption.Reject.m_Exchange = 0;
		else
			roOption.Reject.m_Exchange = 1;

		SendPacket::CharControlOption(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, roOption);
	} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 30, m_rcWindowSize.top + 217, m_rcWindowSize.left + 48, m_rcWindowSize.top + 234) && g_DeviceInput.GetIsLeftMouseDown())		// 귓속말 거부
	{
		RejectOption roOption;
		roOption.m_wReject = g_ClientMain.m_roOption.m_wReject;
		if(roOption.Reject.m_Whisper)
			roOption.Reject.m_Whisper = 0;
		else
			roOption.Reject.m_Whisper = 1;

		SendPacket::CharControlOption(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, roOption);
	} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 30, m_rcWindowSize.top + 241, m_rcWindowSize.left + 48, m_rcWindowSize.top + 258) && g_DeviceInput.GetIsLeftMouseDown())		// 결투 거부
	{
		RejectOption roOption;
		roOption.m_wReject = g_ClientMain.m_roOption.m_wReject;
		if(roOption.Reject.m_Duel)
			roOption.Reject.m_Duel = 0;
		else
			roOption.Reject.m_Duel = 1;

		SendPacket::CharControlOption(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, roOption);
	} else if(g_DeviceInput.InRect(m_rcWindowSize.left + 30, m_rcWindowSize.top + 265, m_rcWindowSize.left + 48, m_rcWindowSize.top + 282) && g_DeviceInput.GetIsLeftMouseDown())		// 친구 등록 거부
	{
		RejectOption roOption;
		roOption.m_wReject = g_ClientMain.m_roOption.m_wReject;
		if(roOption.Reject.m_Friend)
			roOption.Reject.m_Friend = 0;
		else
			roOption.Reject.m_Friend = 1;

		SendPacket::CharControlOption(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, roOption);
	}*/
}

void CCommonOption::Render(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	switch(m_dwShowOption)
	{
		case 0:
			m_lstSprite[0]->Render(lpD3DDevice, m_cAlpha);
			break;

		case 1:
			m_lstSprite[1]->Render(lpD3DDevice, m_cAlpha);
			m_lstSprite[2]->Render(lpD3DDevice, m_cAlpha);
			m_lstSprite[3]->Render(lpD3DDevice, m_cAlpha);
			m_lstSprite[4]->Render(lpD3DDevice, m_cAlpha);

			if(m_roOption.Reject.m_Duel)
			{
				m_ButtonManager.m_lstButton[5]->m_nState = BUTTON_ON;
				m_ButtonManager.m_lstButton[6]->m_nState = BUTTON_OFF;
			} else
			{
				m_ButtonManager.m_lstButton[5]->m_nState = BUTTON_OFF;
				m_ButtonManager.m_lstButton[6]->m_nState = BUTTON_ON;
			}

			if(m_roOption.Reject.m_Exchange)
			{
				m_ButtonManager.m_lstButton[7]->m_nState = BUTTON_ON;
				m_ButtonManager.m_lstButton[8]->m_nState = BUTTON_OFF;
			} else
			{
				m_ButtonManager.m_lstButton[7]->m_nState = BUTTON_OFF;
				m_ButtonManager.m_lstButton[8]->m_nState = BUTTON_ON;
			}

			if(m_roOption.Reject.m_Party)
			{
				m_ButtonManager.m_lstButton[9]->m_nState = BUTTON_ON;
				m_ButtonManager.m_lstButton[10]->m_nState = BUTTON_OFF;
			} else
			{
				m_ButtonManager.m_lstButton[9]->m_nState = BUTTON_OFF;
				m_ButtonManager.m_lstButton[10]->m_nState = BUTTON_ON;
			}

			if(m_roOption.Reject.m_Friend)
			{
				m_ButtonManager.m_lstButton[11]->m_nState = BUTTON_ON;
				m_ButtonManager.m_lstButton[12]->m_nState = BUTTON_OFF;
			} else
			{
				m_ButtonManager.m_lstButton[11]->m_nState = BUTTON_OFF;
				m_ButtonManager.m_lstButton[12]->m_nState = BUTTON_ON;
			}

			if(!m_roOption.Reject.m_Whisper)
			{
				m_ButtonManager.m_lstButton[13]->m_nState = BUTTON_ON;
				m_ButtonManager.m_lstButton[14]->m_nState = BUTTON_OFF;
			} else
			{
				m_ButtonManager.m_lstButton[13]->m_nState = BUTTON_OFF;
				m_ButtonManager.m_lstButton[14]->m_nState = BUTTON_ON;
			}

			if(!m_roOption.Reject.m_Shout)
			{
				m_ButtonManager.m_lstButton[15]->m_nState = BUTTON_ON;
				m_ButtonManager.m_lstButton[16]->m_nState = BUTTON_OFF;
			} else
			{
				m_ButtonManager.m_lstButton[15]->m_nState = BUTTON_OFF;
				m_ButtonManager.m_lstButton[16]->m_nState = BUTTON_ON;
			}

			if(m_coOption.m_bTipBroad)
			{
				m_ButtonManager.m_lstButton[17]->m_nState = BUTTON_ON;
				m_ButtonManager.m_lstButton[18]->m_nState = BUTTON_OFF;
			} else
			{
				m_ButtonManager.m_lstButton[17]->m_nState = BUTTON_OFF;
				m_ButtonManager.m_lstButton[18]->m_nState = BUTTON_ON;
			}

			if(m_coOption.m_bChat)
			{
				m_ButtonManager.m_lstButton[19]->m_nState = BUTTON_ON;
				m_ButtonManager.m_lstButton[20]->m_nState = BUTTON_OFF;
			} else
			{
				m_ButtonManager.m_lstButton[19]->m_nState = BUTTON_OFF;
				m_ButtonManager.m_lstButton[20]->m_nState = BUTTON_ON;
			}

			m_lstSprite[5]->m_nPutX = m_rcWindowSize.left + 453;
			m_lstSprite[5]->m_nPutY = m_rcWindowSize.top + 89 + m_coOption.m_dwCharName * 19;
			m_lstSprite[5]->Render(lpD3DDevice);

			m_lstSprite[6]->m_nPutX = m_rcWindowSize.left + 141;
			m_lstSprite[6]->m_nPutY = m_rcWindowSize.top + 240 + m_coOption.m_dwSkillEffect * 20;
			m_lstSprite[6]->Render(lpD3DDevice);

			m_lstSprite[7]->m_nPutX = m_rcWindowSize.left + 297;
			m_lstSprite[7]->m_nPutY = m_rcWindowSize.top + 240 + m_coOption.m_dwChantEffect * 20;
			m_lstSprite[7]->Render(lpD3DDevice);

			m_lstSprite[8]->m_nPutX = m_rcWindowSize.left + 453;
			m_lstSprite[8]->m_nPutY = m_rcWindowSize.top + 240 + m_coOption.m_dwWeaponEffect * 20;
			m_lstSprite[8]->Render(lpD3DDevice);
/*			m_lstScroll[0]->Render(lpD3DDevice, m_cAlpha);
			m_lstScroll[1]->Render(lpD3DDevice, m_cAlpha);*/
			break;
	}

	m_ButtonManager.Render(lpD3DDevice);
}
