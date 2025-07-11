
#include "WinInput.h"

#include "GMImageDib.h"

#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLSpriteList.h"
#include "RYLTabButton.h"
#include "RYLStringTable.h"

#include "RYLHelpControlDlg.h"
#include "RYLHelpFrameDlg.h"
#include "GMMemory.h"

CRYLHelpFrameDlg*	g_pHelpFrameDlg = NULL;

CRYLHelpFrameDlg::CRYLHelpFrameDlg() 
:	m_pTabButton(NULL), m_pHelpControlPanel(NULL)
{
	RECT rt;
	CGMImageDib*	pImageDIB = NULL;
	CRYLImage*		pImage	  = NULL;

	CRYLSpriteList::Instance()->CreateGMImage(512, 34, &pImageDIB);
	pImageDIB->ClearbyColor(0x00000000);

	SetRect(&rt, 0, 0, 50, 13);
	pImageDIB->DrawRect(&rt, 0, 0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_COMPONENT01_256x256));
	SetRect(&rt, 50, 0, 150, 13);
	pImageDIB->DrawRect(&rt, 50,  0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_COMPONENT01_256x256));			
	pImageDIB->DrawRect(&rt, 150, 0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_COMPONENT01_256x256));
	pImageDIB->DrawRect(&rt, 250, 0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_COMPONENT01_256x256));
	pImageDIB->DrawRect(&rt, 350, 0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_COMPONENT01_256x256));
	SetRect(&rt, 194, 0, 256, 13);
	pImageDIB->DrawRect(&rt, 450, 0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_COMPONENT01_256x256));	

	pImageDIB->DrawFrameRect(55, 13, 457, 21, 0xc0000000);
	pImageDIB->DrawFrameRect(0, 13, 1, 21, 0xff5b4c35);
	pImageDIB->DrawFrameRect(1, 13, 1, 21, 0xffe1c995);
	pImageDIB->DrawFrameRect(57, 33, 453, 1, 0xff9f835b);
	pImageDIB->DrawFrameRect(510, 13, 1, 21, 0xffe1c995);
	pImageDIB->DrawFrameRect(511, 13, 1, 21, 0xff5b4c35);

	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage(pImageDIB);
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage(pImage);
	AttachCaptionTexture((CGMTexture*)pImage);
	GM_DELETE(pImageDIB);

	SetWidth(512);
	SetHeight(546);
	SetLeft((CGMUIScreen::GetInstance()->GetWidth() - GetWidth()) / 2);
	SetTop((CGMUIScreen::GetInstance()->GetHeight() - GetHeight()) / 2);

	SetFormType(RYLDLG_FRAME_TYPE_CLOSEBUTTON);
	InitCtrl();

	g_pHelpFrameDlg = this;
}

CRYLHelpFrameDlg::~CRYLHelpFrameDlg() 
{
	FinalCtrl();
}

void	CRYLHelpFrameDlg::InitCtrl(void) 
{
	m_pTabButton = new CRYLTabButton(this);
	m_pTabButton->SetLeft(1);
	m_pTabButton->SetTop(13);
	m_pTabButton->SetWidth(511);
	m_pTabButton->SetHeight(23);
	m_pTabButton->SetInstance(CRYLHelpFrameDlg::TAB_BUTTON);
	m_pTabButton->OnClickEvent = DlgClick;
	m_pTabButton->AttachTabButton(CRYLStringTable::m_strString[ 172 ], 3);
	AttachGMUIControl(m_pTabButton);

	LPTABBUTTONINFO lpTabButton = NULL;
	lpTabButton = m_pTabButton->GetTabButtonInfo(0);
	lpTabButton->m_pTabButton->SetTooltip(&m_TooltipManager, CRYLStringTable::m_strString[ 173 ], 173, TOOLTIP_TAB);

	if (NULL == m_pHelpControlPanel)
	{
		m_pHelpControlPanel = new CRYLHelpControlDlg(this);
	}

	m_pHelpControlPanel->SetLeft(0);
	m_pHelpControlPanel->SetTop(34);
	m_pHelpControlPanel->SetWidth(512);
	m_pHelpControlPanel->SetHeight(512);
	m_pHelpControlPanel->SetVisible(TRUE);
	AttachGMUIControl(m_pHelpControlPanel);
}

void	CRYLHelpFrameDlg::FinalCtrl(void) 
{
	GM_DELETE(m_pTabButton);
	GM_DELETE(m_pHelpControlPanel);
}

HRESULT		CRYLHelpFrameDlg::Update(BOOL &bClick, BOOL &bEdge) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if (TRUE == g_DeviceInput.InRect(GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight()))
	{
		bEdge = TRUE;

		if (TRUE == g_DeviceInput.GetIsLeftMouseDown() || 
			TRUE == g_DeviceInput.GetIsMiddleMouseDown() || 
			TRUE == g_DeviceInput.GetIsRightMouseDown() ||
			TRUE == g_DeviceInput.GetIsLeftMousePress() || 
			TRUE == g_DeviceInput.GetIsMiddleMousePress() || 
			TRUE == g_DeviceInput.GetIsRightMousePress() ||
			TRUE == g_DeviceInput.GetIsLeftMouseUp() || 
			TRUE == g_DeviceInput.GetIsMiddleMouseUp() || 
			TRUE == g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;	
		}
	}
	
	return S_OK;
}

VOID	CRYLHelpFrameDlg::Show(CGMUIObject* Sender) 
{
}

VOID	CRYLHelpFrameDlg::Hide(CGMUIObject* Sender) 
{
}

VOID	CRYLHelpFrameDlg::GMUIPaint(INT xx, INT yy) 
{
	CRYLDialog::GMUIPaint(xx, yy);
}

VOID	CRYLHelpFrameDlg::DlgClick(CGMUIObject* Sender, INT X, INT Y)
{
	if (NULL == g_pHelpFrameDlg)
	{
		return;
	}

	switch (Sender->GetInstance())
	{
		case CRYLHelpFrameDlg::TAB_BUTTON:
		{
			g_pHelpFrameDlg->SetFocusPanel(g_pHelpFrameDlg->GetTabButton()->GetFocusTabButton());
			break;
		}

		default :
		{
			break;
		}
	}
}

void	CRYLHelpFrameDlg::SetFocusPanel(int iFocus)
{
	// 탭 추가시 처리가 필요한 부분
	switch (iFocus)
	{
		case 0 : 
			g_pHelpFrameDlg->m_pHelpControlPanel->SetVisible(TRUE);
			break;
	}
}

void	CRYLHelpFrameDlg::LoadHelpTexture(void) 
{
	if (NULL != m_pHelpControlPanel)
	{
		static_cast<CRYLHelpControlDlg*>(m_pHelpControlPanel)->LoadHelpControlTexture();
	}
}

