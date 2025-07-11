
#include <Network/ClientSocket/Send/SendCastle.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendCommunity.h>

#include "WinInput.h"
#include "GMImageDib.h"
#include "StringFilter.h"

#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLListBox.h"
#include "RYLEdit.h"
#include "RYLStringTable.h"
#include "RYLMessageBoxManager.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLNetWorkData.h"
#include "RYLGameData.h"

#include "RYLGuildInfoPanel.h"
#include "RYLGuildFrameDlg.h"
#include "RYLChattingDlg.h"
#include "RYLNewTradeDlg.h"
#include "RYLCampShopDlg.h"
#include "GMMemory.h"

CRYLCampShopDlg* g_pCampShopDlg = NULL;

CRYLCampShopDlg::CRYLCampShopDlg()
:	m_pChangeTaxButton(NULL), m_pRepairButton(NULL), m_pRepairAllButton(NULL), 
	m_pCampShopLabel(NULL), m_pTaxLabel(NULL), m_pTaxValueLabel(NULL), 
	m_pShopOwnerLabel(NULL), m_pShopOwnerValueLabel(NULL), m_pCurrentVisitor(NULL), 
	m_dwSellResult(0), m_dwChangeTaxResult(0), m_dwChangeTaxValue(0), 
	m_dwBuyResult(0), m_dwBuyNum(0), m_pBuyItem(NULL), 
	m_dwRegisterResult(0), m_dwRegisterPrice(0), m_cRegisterIndex(0), 
	m_cTax(0), m_dwCampID(0), m_bAdminMode(false)
{
	for (unsigned char cYIndex = 0; cYIndex < CONTAINER_HEIGHT; ++cYIndex)
	{
		for (unsigned char cXIndex = 0; cXIndex < CONTAINER_WIDTH; ++cXIndex)
		{
			m_aryItemIndex[cYIndex][cXIndex] = ITEM_EMPTY;
		}
	}

	RECT rt;
	CGMImageDib*	pImageDIB = NULL;
	CRYLImage*		pImage	  = NULL;

	CRYLSpriteList::Instance()->CreateGMImage(380, 15, &pImageDIB);
	pImageDIB->ClearbyColor(0x00000000);
	SetRect(&rt, 0, 0, 256, 15);
	pImageDIB->DrawRect(&rt, 0, 0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_CAMP_SHOP01));
	SetRect(&rt, 0, 0, 124, 15);
	pImageDIB->DrawRect(&rt, 256, 0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_CAMP_SHOP02));
	SetRect(&rt, 0, 0, 124, 15);
	pImageDIB->DrawRect(&rt, 256, 0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_CAMP_SHOP03));
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage(pImageDIB);
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage(pImage);
	AttachCaptionTexture((CGMTexture*)pImage);

	GM_DELETE(pImageDIB);

	CRYLSpriteList::Instance()->CreateGMImage(380, 426, &pImageDIB);
	pImageDIB->ClearbyColor(0x00000000);
	SetRect(&rt, 0, 15, 256, 256);
	pImageDIB->DrawRect(&rt, 0, 0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_CAMP_SHOP01));
	SetRect(&rt, 0, 0, 256, 185);
	pImageDIB->DrawRect(&rt, 0, 241, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_CAMP_SHOP02));
	SetRect(&rt, 0, 15, 124, 256);
	pImageDIB->DrawRect(&rt, 256, 0, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_CAMP_SHOP03));
	SetRect(&rt, 124, 0, 248, 185);
	pImageDIB->DrawRect(&rt, 256, 241, CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_CAMP_SHOP03));
	
	pImageDIB->MakeFrameRect(8, 370, 364, 1, 0xffB99E6B, 0xc0000000,  1);		

	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage(pImageDIB);
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage(pImage);
	AttachWindowTexture((CGMTexture*)pImage);

	GM_DELETE(pImageDIB);
	
	SetWidth(380);
	SetHeight(413);
	SetLeft((CGMUIScreen::GetInstance()->GetWidth() - GetWidth()) / 2);                                
	SetTop((CGMUIScreen::GetInstance()->GetHeight() - GetHeight()) / 2);
	
	SetFormType(RYLDLG_TYPE_CLOSEBUTTON);

	InitCtrl();
	g_pCampShopDlg = this;    
}

CRYLCampShopDlg::~CRYLCampShopDlg()
{
	FinalCtrl();
}

void CRYLCampShopDlg::InitCtrl(void)
{
	RECT			rtRect;

	CGMImageDib*	pSprite			= NULL;
	CRYLSpriteList* pSpriteList		= CRYLSpriteList::Instance();

	pSprite	= pSpriteList->FindSprite(ERYLSPRITENAME::RYL_COMPONENT01_256x256);

	m_pCampShopLabel = new CRYLLabel(this);
	m_pCampShopLabel->SetLeft(10);	
	m_pCampShopLabel->SetTop(21);
	m_pCampShopLabel->SetFontColor (0xffffffff);
	m_pCampShopLabel->SetAutoSize(TRUE);
	m_pCampShopLabel->SetCaption(CRYLStringTable::m_strString[ 3431 ]);
	AttachGMUIControl(m_pCampShopLabel);

	m_pTaxLabel = new CRYLLabel(this);
	m_pTaxLabel->SetLeft(14);	
	m_pTaxLabel->SetTop(46);
	m_pTaxLabel->SetFontColor (0xffffffff);
	m_pTaxLabel->SetAutoSize(TRUE);
	m_pTaxLabel->SetCaption(CRYLStringTable::m_strString[ 3485 ]);
	AttachGMUIControl(m_pTaxLabel);

	m_pTaxValueLabel = new CRYLLabel(this);
	m_pTaxValueLabel->SetLeft(47);	
	m_pTaxValueLabel->SetTop(46);
	m_pTaxValueLabel->SetFontColor (0xffffffff);
	m_pTaxValueLabel->SetAutoSize(TRUE);
	m_pTaxValueLabel->SetCaption("0%");
	AttachGMUIControl(m_pTaxValueLabel);

	m_pShopOwnerLabel = new CRYLLabel(this);
	m_pShopOwnerLabel->SetLeft(193);	
	m_pShopOwnerLabel->SetTop(46);
	m_pShopOwnerLabel->SetFontColor (0xffffffff);
	m_pShopOwnerLabel->SetAutoSize(TRUE);
	m_pShopOwnerLabel->SetCaption(CRYLStringTable::m_strString[ 2211 ]);
	AttachGMUIControl(m_pShopOwnerLabel);

	m_pShopOwnerValueLabel = new CRYLLabel(this);
	m_pShopOwnerValueLabel->SetLeft(248);	
	m_pShopOwnerValueLabel->SetTop(46);
	m_pShopOwnerValueLabel->SetFontColor (0xffffffff);
	m_pShopOwnerValueLabel->SetAutoSize(TRUE);
	m_pShopOwnerValueLabel->SetCaption(CRYLStringTable::m_strString[ 2015 ]);
	AttachGMUIControl(m_pShopOwnerValueLabel);

	m_pCurrentVisitor = new CRYLLabel(this);
	m_pCurrentVisitor->SetLeft(26);	
	m_pCurrentVisitor->SetTop(74);
	m_pCurrentVisitor->SetFontColor (0xffffffff);
	m_pCurrentVisitor->SetAutoSize(TRUE);
	m_pCurrentVisitor->SetCaption(CRYLStringTable::m_strString[ 2212 ]);
	AttachGMUIControl(m_pCurrentVisitor);

	pSprite = pSpriteList->FindSprite(ERYLSPRITENAME::RYL_BUTTON_256x256);

	// 세율 변경 버튼
	m_pChangeTaxButton = new CRYLButton(this);
	m_pChangeTaxButton->SetLeft(157);
	m_pChangeTaxButton->SetTop(410);
	m_pChangeTaxButton->SetCaption(CRYLStringTable::m_strString[ 2804 ]);
	m_pChangeTaxButton->SetFontColor(0xffffffff);
	m_pChangeTaxButton->SetColor(0xffffffff);
	m_pChangeTaxButton->OnClickEvent = DlgClick;
	m_pChangeTaxButton->SetInstance(CHANGETAX_BUTTON);
	m_pChangeTaxButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonNormalRect(4), CRYLImageCabinet::EDCT_BUTTON_NORMAL);
	m_pChangeTaxButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonDownRect(4), CRYLImageCabinet::EDCT_BUTTON_DOWN);
	m_pChangeTaxButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonUpRect(4), CRYLImageCabinet::EDCT_BUTTON_UP);
	m_pChangeTaxButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonDisableRect(4), CRYLImageCabinet::EDCT_BUTTON_DISIBLE);
	AttachGMUIControl(m_pChangeTaxButton);

	// 수리하기 버튼
	m_pRepairButton = new CRYLButton(this);
	m_pRepairButton->SetLeft(231);
	m_pRepairButton->SetTop(410);
	m_pRepairButton->SetCaption(CRYLStringTable::m_strString[ 242 ]);
	m_pRepairButton->SetFontColor(0xffffffff);
	m_pRepairButton->SetColor(0xffffffff);
	m_pRepairButton->OnClickEvent = DlgClick;
	m_pRepairButton->SetInstance(REPAIR_BUTTON);
	m_pRepairButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonNormalRect(4), CRYLImageCabinet::EDCT_BUTTON_NORMAL);
	m_pRepairButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonDownRect(4), CRYLImageCabinet::EDCT_BUTTON_DOWN);
	m_pRepairButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonUpRect(4), CRYLImageCabinet::EDCT_BUTTON_UP);
	m_pRepairButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonDisableRect(4), CRYLImageCabinet::EDCT_BUTTON_DISIBLE);
	AttachGMUIControl(m_pRepairButton);

	// 모두 수리 버튼
	m_pRepairAllButton = new CRYLButton(this);
	m_pRepairAllButton->SetLeft(305);
	m_pRepairAllButton->SetTop(410);
	m_pRepairAllButton->SetCaption(CRYLStringTable::m_strString[ 243 ]);
	m_pRepairAllButton->SetFontColor(0xffffffff);
	m_pRepairAllButton->SetColor(0xffffffff);
	m_pRepairAllButton->OnClickEvent = DlgClick;
	m_pRepairAllButton->SetInstance(REPAIRALL_BUTTON);
	m_pRepairAllButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonNormalRect(4), CRYLImageCabinet::EDCT_BUTTON_NORMAL);
	m_pRepairAllButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonDownRect(4), CRYLImageCabinet::EDCT_BUTTON_DOWN);
	m_pRepairAllButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonUpRect(4), CRYLImageCabinet::EDCT_BUTTON_UP);
	m_pRepairAllButton->AttachButtonImage(pSprite, pSpriteList->GetDefaultButtonDisableRect(4), CRYLImageCabinet::EDCT_BUTTON_DISIBLE);
	AttachGMUIControl(m_pRepairAllButton);

	// 입장객 리스트 박스
	m_pVisitorListBox = new CRYLListBox(this, TRUE);
	m_pVisitorListBox->SetLeft(10);
	m_pVisitorListBox->SetTop(97);
	m_pVisitorListBox->SetWidth(94);
	m_pVisitorListBox->SetHeight(173);
	
	SetRect(&rtRect, 198, 186, 212, 200);
	m_pVisitorListBox->AttachTopButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal);
	SetRect(&rtRect, 226, 186, 240, 200);
	m_pVisitorListBox->AttachTopButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down);
	SetRect(&rtRect, 212, 186, 226, 200);
	m_pVisitorListBox->AttachTopButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up);
	SetRect(&rtRect, 198, 186, 212, 200);
	m_pVisitorListBox->AttachTopButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible);

	SetRect(&rtRect, 235, 164, 243, 184);
 	m_pVisitorListBox->AttachTrackButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal);
	SetRect(&rtRect, 235, 164, 243, 184);					 
	m_pVisitorListBox->AttachTrackButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down);
	SetRect(&rtRect, 235, 164, 243, 184);					 
	m_pVisitorListBox->AttachTrackButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up);
	SetRect(&rtRect, 235, 164, 243, 184);					 
	m_pVisitorListBox->AttachTrackButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible);

	SetRect(&rtRect, 198, 200, 212, 214);
	m_pVisitorListBox->AttachBottomButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal);
	SetRect(&rtRect, 226, 200, 240, 214);					  
	m_pVisitorListBox->AttachBottomButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down);
	SetRect(&rtRect, 212, 200, 226, 214);					  
	m_pVisitorListBox->AttachBottomButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up);
	SetRect(&rtRect, 198, 200, 212, 214);					  
	m_pVisitorListBox->AttachBottomButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible);
 
	m_pVisitorListBox->SetColor(0xffffffff);
	AttachGMUIControl(m_pVisitorListBox);

	// 채팅 리스트 박스
	m_pChattingListBox = new CRYLListBox(this, TRUE);
	m_pChattingListBox->SetLeft(13);
	m_pChattingListBox->SetTop(284);
	m_pChattingListBox->SetWidth(349);
	m_pChattingListBox->SetHeight(100);

	SetRect(&rtRect, 198, 186, 212, 200);
	m_pChattingListBox->AttachTopButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal);
	SetRect(&rtRect, 226, 186, 240, 200);
	m_pChattingListBox->AttachTopButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down);
	SetRect(&rtRect, 212, 186, 226, 200);
	m_pChattingListBox->AttachTopButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up);
	SetRect(&rtRect, 198, 186, 212, 200);
	m_pChattingListBox->AttachTopButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible);

	SetRect(&rtRect, 235, 164, 243, 184);
 	m_pChattingListBox->AttachTrackButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal);
	SetRect(&rtRect, 235, 164, 243, 184);					 
	m_pChattingListBox->AttachTrackButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down);
	SetRect(&rtRect, 235, 164, 243, 184);					 
	m_pChattingListBox->AttachTrackButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up);
	SetRect(&rtRect, 235, 164, 243, 184);					 
	m_pChattingListBox->AttachTrackButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible);

	SetRect(&rtRect, 198, 200, 212, 214);
	m_pChattingListBox->AttachBottomButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal);
	SetRect(&rtRect, 226, 200, 240, 214);					  
	m_pChattingListBox->AttachBottomButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down);
	SetRect(&rtRect, 212, 200, 226, 214);					  
	m_pChattingListBox->AttachBottomButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up);
	SetRect(&rtRect, 198, 200, 212, 214);					  
	m_pChattingListBox->AttachBottomButtonImage(pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible);
	
	m_pChattingListBox->SetColor(0xffffffff);
	AttachGMUIControl(m_pChattingListBox);

	m_pChattingEdit = new CRYLEdit(this);
	m_pChattingEdit->SetLeft(10);
	m_pChattingEdit->SetTop(387);
	m_pChattingEdit->SetWidth(360);
	m_pChattingEdit->SetHeight(15);
	m_pChattingEdit->SetColor(0xff000000);
	m_pChattingEdit->SetFontColor(0xffffffff);
	m_pChattingEdit->SetInstance(CHATTING_EDIT);
	m_pChattingEdit->SetMaxLength(48);
	m_pChattingEdit->OnKeyDown = DlgKeyDown; 
	AttachGMUIControl(m_pChattingEdit);
}

void CRYLCampShopDlg::FinalCtrl(void)
{
	GM_DELETE(m_pChangeTaxButton);	
	GM_DELETE(m_pRepairButton);	
	GM_DELETE(m_pRepairAllButton); 

	GM_DELETE(m_pCampShopLabel);		
	GM_DELETE(m_pTaxLabel);		
	GM_DELETE(m_pTaxValueLabel);				
	GM_DELETE(m_pShopOwnerLabel);
	GM_DELETE(m_pShopOwnerValueLabel);
	GM_DELETE(m_pCurrentVisitor);		

	GM_DELETE(m_pChattingEdit);				
	GM_DELETE(m_pChattingListBox);
	GM_DELETE(m_pVisitorListBox);
}

VOID CRYLCampShopDlg::Show(CGMUIObject* Sender)
{
	m_pChattingEdit->SetText("");
}

VOID CRYLCampShopDlg::Hide(CGMUIObject* Sender)
{
	CRYLGameData* pGame = CRYLGameData::Instance();

	pGame->m_dwTradingNpcID = 0;
	pGame->m_dwKindCursor = CURSOR_NORMAL;

	m_bAdminMode = false;

	m_pChattingEdit->SetText("");
	
	m_pVisitorListBox->ClearItem();
	m_pChattingListBox->ClearItem();

	for (int nIndex = 0; nIndex < PktStIInfo::MAX_CUSTOMER_NUM; ++nIndex)
	{
		m_aryVisitorList[nIndex] = VisitorNode();
	}

	CRYLChattingDlg* pDlg = CRYLChattingDlg::Instance();
	if (NULL != pDlg)
	{
		pDlg->m_pChatEdit->SetFocus();
		pDlg->m_pChatEdit->SetFocusState(FALSE);
    }

	for (list<CItemInstance*>::iterator it = m_lstItem.begin(); it != m_lstItem.end(); )
	{
		CItemInstance* lpItem = *it;
		it = m_lstItem.erase(it);
		delete lpItem;
	}

	for (unsigned char cYIndex = 0; cYIndex < CONTAINER_HEIGHT; ++cYIndex)
	{
		for (unsigned char cXIndex = 0; cXIndex < CONTAINER_WIDTH; ++cXIndex)
		{
			m_aryItemIndex[cYIndex][cXIndex] = ITEM_EMPTY;
		}
	}

	CRYLNetworkData::SendStallLeave();
}

VOID CRYLCampShopDlg::GMUIPaint(INT xx, INT yy)
{
	// UI 렌더링
	CRYLDialog::GMUIPaint(xx, yy);

	LPDIRECT3DDEVICE8 lpD3DDevice = BaseGraphicsLayer::GetDevice();

	// 진열중인 아이템 렌더링
	for (list<CItemInstance*>::iterator it = m_lstItem.begin(); it != m_lstItem.end(); ++it)
	{
		const Item::ItemInfo& itemInfo = (*it)->GetItemInfo();
		(*it)->m_wPutX = GetLeft() + 111 + 26 * (*it)->m_lpItemBase->GetPos().GetXIndex() + 
			(((itemInfo.m_DetailData.m_cXSize * 26 - 1) - (*it)->m_lpSprite->GetSizeX()) / 2);
		(*it)->m_wPutY = GetTop() + 67 + 26 * (*it)->m_lpItemBase->GetPos().GetYIndex() + 
			(((itemInfo.m_DetailData.m_cYSize * 26 - 1) - (*it)->m_lpSprite->GetSizeY()) / 2);

		(*it)->Render(lpD3DDevice);
	}

	CRYLGameData*	pGame			= CRYLGameData::Instance();

	// 들고 있는 아이템을 놓을 자리 렌더링
	if (pGame->m_lpPickItem && g_MessageBoxManager.m_lstMessageBox.empty() && true == m_bAdminMode)
	{
		if (g_DeviceInput.InRect(GetLeft() + 111, GetTop() + 67, GetLeft() + 372, GetTop() + 270))
		{
			Item::ItemPos pItemIndex;
			POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
			pItemIndex.SetPos((ptMousePos->x - (GetLeft() + 111)) / 26, (ptMousePos->y - (GetTop() + 67)) / 26, 0);

			if (true == CheckItemIndex(pGame->m_lpPickItem, pItemIndex.GetXIndex(), pItemIndex.GetYIndex()))
			{
				pGame->RenderRect(lpD3DDevice,
					GetLeft() + 111 + pItemIndex.GetXIndex() * 26,
					GetTop()  + 67  + pItemIndex.GetYIndex() * 26,
					GetLeft() + 111 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize * 26 + pItemIndex.GetXIndex() * 26,
					GetTop()  + 67  + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize * 26 + pItemIndex.GetYIndex() * 26,
					0xFF0000FF, 0x80);
			} 
			else
			{
				if (pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize + pItemIndex.GetXIndex() < CONTAINER_WIDTH &&
					pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize + pItemIndex.GetYIndex() < CONTAINER_HEIGHT)
				{
					pGame->RenderRect(lpD3DDevice,
						GetLeft() + 111 + pItemIndex.GetXIndex() * 26,
						GetTop()  + 67  + pItemIndex.GetYIndex() * 26,
						GetLeft() + 111 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize * 26 + pItemIndex.GetXIndex() * 26,
						GetTop()  + 67  + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize * 26 + pItemIndex.GetYIndex() * 26,
						0xFFFF0000, 0x80);
				}
			}
		}
	}
}

HRESULT CRYLCampShopDlg::Update(BOOL &bClick, BOOL &bEdge)			
{ 
	if (FALSE == GetVisible() || TRUE == g_MessageBoxManager.GetIsExistModal())
	{
		return E_FAIL;
	}

	if (g_DeviceInput.InRect(GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight()))
	{
		bEdge = TRUE;

		if (g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}

	if (true == ProcessMessageBoxResult())
	{
		return S_OK;
	}

	CRYLGameData*		pGame		= CRYLGameData::Instance();
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	RYLCreature*	pSelfCreature	= RYLCreatureManager::Instance()->GetSelfCreature();
	CharDataNode*	pSelfData		= RYLCharacterDataManager::Instance()->GetCharData(pSelfCreature->GetCharID());

	// 길드 요새 상점의 아이템 공간을 클릭한 경우
	if (g_DeviceInput.InRect(GetLeft() + 111, GetTop() + 67, GetLeft() + 372, GetTop() + 270) && g_DeviceInput.GetIsLeftMouseUp())
	{
		// 이미 아이템을 들고 있을 때
		if (NULL != pGame->m_lpPickItem)
		{
			if (false == pGame->m_lpPickItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bSell)
			{
				CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 135 ], pNetwork->m_dwMyChrID, 129 );
				return S_OK;
			}

			if (pGame->m_lpPickItem->GetItemPos() == TakeType::TS_QSLOT)
			{
				CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[ 257 ], pNetwork->m_dwMyChrID, 129);
				return S_OK;
			} 

			if (pGame->m_lpPickItem->m_bSkill)
			{
				CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[ 2755 ], pNetwork->m_dwMyChrID, 129);
				return S_OK;
			}

			// 입장객이라면 '판매'
			if (false == m_bAdminMode)
			{
				ProcessSell();
				return S_OK;
			}
			// 상점 주인(길드 마스터)이라면 '아이템 등록'
			else
			{
				ProcessRegister();
				return S_OK;
			}
		}
	}

	// 길드 요새 상점에 등록된 아이템을 클릭한 경우
	if (NULL == pGame->m_lpPickItem)
	{
		if (TRUE == g_DeviceInput.GetIsLeftMouseUp() || 
			TRUE == g_DeviceInput.GetIsRightMouseUp())
		{
			for (std::list<CItemInstance*>::iterator it = m_lstItem.begin(); it != m_lstItem.end(); ++it)
			{
				CItemInstance* lpItem = *it;
				if (TRUE == lpItem->IsOverMouse())
				{
					// 입장객이라면 '구매'
					if (false == m_bAdminMode)
					{
						ProcessBuy(lpItem);
						return S_OK;
					}
					// 상점 주인(길드 마스터)이라면 '아이템 등록 취소'
					else
					{
						ProcessCancel(lpItem);
						return S_OK;
					}
				}
			}
		}
	}

	return S_OK;	
}

void CRYLCampShopDlg::ChangeTaxMessageBox(void)
{
	CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
	lpMessageBox->Create(CRYLStringTable::m_strString[ 3059 ], MB_NUMBER | MB_YES | MB_NO | MB_EXIT);
	lpMessageBox->SetResult(&m_dwChangeTaxResult);
	lpMessageBox->SetValue(&m_dwChangeTaxValue);

	m_dwChangeTaxResult = 0;
	m_dwChangeTaxValue = 0;
}

void CRYLCampShopDlg::ChangeTax(unsigned char cTax)
{
	m_cTax = cTax;

	char szTax[MAX_PATH];
	sprintf(szTax, "%d%%", m_cTax);
	m_pTaxValueLabel->SetCaption(szTax);
}

bool CRYLCampShopDlg::Enter(unsigned long dwOwnerID)
{
	CRYLGameData* pGame = CRYLGameData::Instance();

	pGame->m_dwTradingNpcID = m_dwOwnerID = dwOwnerID;

	RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(m_dwOwnerID);
	if (NULL == pTempCreature)
	{
		return false;
	}

	m_dwCampID = pTempCreature->GetCampID();

	CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData(m_dwOwnerID);
	if (NULL == pData)
	{
		return false;
	}

	CRYLGameScene*		pScene	= (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
	CRYLGuildFrameDlg*	pDlg	= (CRYLGuildFrameDlg*)pScene->GetGuildFrameDlg();
	CRYLGuildInfoPanel* pPanel	= (CRYLGuildInfoPanel*)pDlg->GetGuildInfoPanel();

	GUILDLARGEINFOEX* lpGuildInfo = pPanel->FindGuild(pData->m_ulGuildID);

	if (NULL == lpGuildInfo)
	{
		return false;
	}
	
	m_pShopOwnerValueLabel->SetCaption(lpGuildInfo->m_szName);

	m_pChattingEdit->SetText("");
	m_pChattingEdit->SetFocus();

	// 마스터 권한을 이용할 수 있는지 체크해둔다.
	RYLCreature*	pSelfCreature	= RYLCreatureManager::Instance()->GetSelfCreature();
	CharDataNode*	pSelfData		= RYLCharacterDataManager::Instance()->GetCharData(pSelfCreature->GetCharID());

	if (true == m_bAdminMode)
	{
		m_pChangeTaxButton->SetEnable(TRUE);
		m_pRepairButton->SetEnable(FALSE);
		m_pRepairAllButton->SetEnable(FALSE);
	}
	else
	{
		m_pChangeTaxButton->SetEnable(FALSE);
		m_pRepairButton->SetEnable(TRUE);
		m_pRepairAllButton->SetEnable(TRUE);
	}

	return true;
}

CItemInstance* CRYLCampShopDlg::GetItem(Item::ItemPos& srcPos)
{
	for (list<CItemInstance *>::iterator it = m_lstItem.begin(); it != m_lstItem.end(); ++it)
	{
		if ((*it)->m_lpItemBase->GetPos() == srcPos) 
		{
			return (*it);
		}
	}

	return NULL;
}

bool CRYLCampShopDlg::RemoveItem(CItemInstance* lpItem)
{
	for (list<CItemInstance *>::iterator it = m_lstItem.begin(); it != m_lstItem.end(); ++it)
	{
		if ((*it) == lpItem)
		{
			unsigned char cX, cY, cTab;
			lpItem->m_lpItemBase->GetPos().GetPos(cX, cY, cTab);

			unsigned char cXSize = lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_cXSize;
			unsigned char cYSize = lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_cYSize;

			for (unsigned char cYIndex = cY; cYIndex < cY + cYSize; ++cYIndex)
			{
				for (unsigned char cXIndex = cX; cXIndex < cX + cXSize; ++cXIndex)
				{
					m_aryItemIndex[cYIndex][cXIndex] = ITEM_EMPTY;
				}
			}

			m_lstItem.erase(it);
			return true;
		}
	}

	return false;
}

void CRYLCampShopDlg::AddItemList(CItemInstance* lpItem, bool bSound)
{
	for (list<CItemInstance *>::iterator it = m_lstItem.begin(); it != m_lstItem.end(); ++it)
	{
		if ((*it)->m_lpItemBase->GetPos().m_cIndex == lpItem->m_lpItemBase->GetPos().m_cIndex)
		{
			return;
		}
	}

	m_lstItem.push_back(lpItem);

	unsigned char cX, cY, cTab;
	lpItem->m_lpItemBase->GetPos().GetPos(cX, cY, cTab);

	unsigned char cXSize = lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_cXSize;
	unsigned char cYSize = lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_cYSize;

	for (unsigned char cYIndex = cY; cYIndex < cY + cYSize; ++cYIndex)
	{
		for (unsigned char cXIndex = cX; cXIndex < cX + cXSize; ++cXIndex)
		{
			m_aryItemIndex[cYIndex][cXIndex] = lpItem->m_dwItemUID;
		}
	}

	CRYLStallDlg::AddItemList(lpItem, bSound);
}

bool CRYLCampShopDlg::SendChat(char* szChatMessage)
{
	char szVisitorName[CHAR_INFOST::MAX_NAME_LEN * PktStIInfo::MAX_CUSTOMER_NUM] = { 0, };

	int nIndex = 0;
	for (; nIndex < PktStIInfo::MAX_CUSTOMER_NUM; ++nIndex)
	{
		if (0 == m_aryVisitorList[nIndex].m_dwCharID)
		{
			break;
		}

		strncpy(szVisitorName + (nIndex * CHAR_INFOST::MAX_NAME_LEN), 
			m_aryVisitorList[nIndex].m_szCharName, CHAR_INFOST::MAX_NAME_LEN);
	}

	return SendPacket::CharTargetedChat(g_GameSession, PktChat::CAMP_SHOP, 
		szVisitorName, nIndex, szChatMessage, strlen(szChatMessage));
}

VOID CRYLCampShopDlg::DlgClick(CGMUIObject* Sender, INT X, INT Y) 
{
	if (NULL == g_pCampShopDlg)
		return;

	CRYLGameData* pGame = CRYLGameData::Instance();

	switch (Sender->GetInstance())
	{
		case CRYLCampShopDlg::CHANGETAX_BUTTON:
			g_pCampShopDlg->ChangeTaxMessageBox();
			break;

		case CRYLCampShopDlg::REPAIR_BUTTON:
		{
			if (pGame->m_dwKindCursor != CURSOR_REPAIR)
			{
				if(CRYLGameData::Instance()->m_bGuildSafeResult)
				{
					pGame->m_dwKindCursor = CURSOR_NORMAL;
					return;
				}

				pGame->m_dwKindCursor = CURSOR_REPAIR;
			}
			else
				pGame->m_dwKindCursor = CURSOR_NORMAL;
			break;
		}

		case CRYLCampShopDlg::REPAIRALL_BUTTON:
		{
			CRYLGameScene*	    pScene  = (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
			CRYLNewTradeDlg*    pDlg    = (CRYLNewTradeDlg*)pScene->GetNewTradeDlg();

			pDlg->AllRepair();
			break;
		}
	}
}

VOID CRYLCampShopDlg::DlgKeyDown(CGMUIObject* Sender, WORD key, INT Shift) 
{
	if (NULL == g_pCampShopDlg)
		return;

	switch (Sender->GetInstance())
	{
		case CRYLCampShopDlg::CHATTING_EDIT:
		{
			if (key == 13)
			{
				if (strcmp(g_pCampShopDlg->m_pChattingEdit->GetText(), ""))
				{
					CHAR		strChat[ MAX_PATH ];
					DWORD		dwTotalLength; 
					strcpy(strChat, g_pCampShopDlg->m_pChattingEdit->GetText());
					dwTotalLength = strlen(strChat);

					if (g_StringFilter.Filter(g_pCampShopDlg->m_pChattingEdit->GetText()))
					{
						CRYLNetworkData::SendChatMessage(NULL, g_pCampShopDlg->m_pChattingEdit->GetText(), PktChat::CAMP_SHOP);
					}
					else
					{
						CRYLNetworkData::SendClientLog(1, g_pCampShopDlg->m_pChattingEdit->GetText(), g_StringFilter.m_strSearched);
						g_pCampShopDlg->m_pChattingListBox->AddItem(CRYLStringTable::m_strString[ 570 ], 0xff64ff64, 0xff64ff64, 0xffffffff);
					}
				}

				g_pCampShopDlg->m_pChattingEdit->SetText("");
			}
			break;
		}
	}
}

bool CRYLCampShopDlg::ProcessMessageBoxResult(void)
{
	CRYLGameData*		pGame		= CRYLGameData::Instance();
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	if (m_dwSellResult & MB_YES)
	{
		if(CRYLGameData::Instance()->m_bGuildSafeResult)
		{
			m_dwSellResult = 0;
			return S_OK;
		}

		// 아이템 판매 (패킷 전송)
		if (NULL != pGame->m_lpPickItem)
		{
			const unsigned char cNum = (pGame->m_lpPickItem->GetIsEnableStack()) ? pGame->m_lpPickItem->GetNowDurability() : 1;
			pNetwork->SendTradeItem(PktTr::TRC_SELL, m_dwOwnerID, 0, 
				TakeType(pGame->m_lpPickItem->m_lpItemBase->GetPos(), Item::ItemPos(), cNum), Item::ItemPos(), NULL);
		}

		m_dwSellResult = 0;
		return true;
	} 
	else if (m_dwSellResult & MB_NO || m_dwSellResult & MB_EXIT)
	{
		m_dwSellResult = 0;
		return true;
	}

	if (m_dwChangeTaxResult & MB_YES)
	{
		// 세율 변경 (패킷 전송)
		if (0 <= m_dwChangeTaxValue && m_dwChangeTaxValue <= Siege::MAX_TAX)
		{
			SendPacket::CharCampCmd(g_GameSession, 
				pNetwork->m_dwMyChrID, m_dwCampID, m_dwChangeTaxValue, 0, PktCampCmd::CAMP_SHOP_CHANGE_TAX);
		}
		else
		{
			CRYLMessageBox* pMessageBox = new CRYLMessageBox;
			pMessageBox->Create(CRYLStringTable::m_strString[ 3060 ]);
		}

		m_dwChangeTaxResult = 0;
		return true;
	} 
	else if (m_dwChangeTaxResult & MB_NO || m_dwChangeTaxResult & MB_EXIT)
	{
		m_dwChangeTaxResult = 0;
		return true;
	}

	if (m_dwBuyResult & MB_YES)
	{
		if(CRYLGameData::Instance()->m_bGuildSafeResult)
		{
			m_dwBuyResult	= 0;
			m_dwBuyNum		= 0;
			m_pBuyItem		= NULL;
			return S_OK;
		}

		// 아이템 구매 (패킷 전송)
		if (TRUE == m_pBuyItem->GetIsEnableStack())
		{
			m_dwBuyNum = min(static_cast<unsigned char>(m_dwBuyNum), m_pBuyItem->GetNowDurability());		
		}
		else
		{
			m_dwBuyNum = 1;
		}

		if (0 != m_dwBuyNum)
		{
			const unsigned long dwBuyPrice = m_dwBuyNum * m_pBuyItem->m_lpItemBase->GetBuyPrice();
			if (pGame->m_csStatus.m_Info.Gold < dwBuyPrice)
			{
				// 금액 부족
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 252 ]);
			}
			else
			{
				Item::ItemPos itemPos;
				
				if ( pGame->m_csStatus.GetAbleInven(m_pBuyItem->GetProtoTypeID(), m_dwBuyNum, itemPos, FALSE) )
				{
					CRYLNetworkData::SendTradeItem(PktTr::TRC_BUY_WITH_GOLD, m_dwOwnerID, 
						m_pBuyItem->GetProtoTypeID(), 
                        TakeType(m_pBuyItem->m_lpItemBase->GetPos(), itemPos, m_dwBuyNum), Item::ItemPos(), NULL);
				}
			}
		}

		m_dwBuyResult	= 0;
		m_dwBuyNum		= 0;
		m_pBuyItem		= NULL;

		return true;
	} 
	else if (m_dwBuyResult & MB_NO || m_dwBuyResult & MB_EXIT)
	{
		m_dwBuyResult	= 0;
		m_dwBuyNum		= 0;
		m_pBuyItem		= NULL;

		return true;
	}

	if (m_dwRegisterResult & MB_YES)
	{
		if (false == pGame->m_lpPickItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bExchangeAndDrop)
		{
			m_dwRegisterResult	= 0;
			m_dwRegisterPrice	= 0;
			m_cRegisterIndex	= 0;

			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[ 136 ]);
			return true;
		}

		if (m_dwRegisterPrice < 1) 
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[ 3484 ]);
		}
		else
		{
			// 상점 아이템 올려놓기
			TakeType takeType;
			takeType.m_srcPos			= pGame->m_lpPickItem->m_lpItemBase->GetPos();
			takeType.m_dstPos.m_cPos	= TakeType::TS_CAMPSHOP;
			takeType.m_dstPos.m_cIndex	= m_cRegisterIndex;
			
			pGame->m_bStallRegFlag = TRUE;
//			g_pClientSocket->SetStatusFlag(NS_STALLREGISTERITEM);
			SendPacket::CharStallRegisterItem(g_GameSession, pNetwork->m_dwMyChrID, m_dwOwnerID, 
				takeType, m_dwRegisterPrice, PktStRI::SC_CAMP_REGISTER);
		}

		m_dwRegisterResult	= 0;
		m_dwRegisterPrice	= 0;
		m_cRegisterIndex	= 0;

		return true;
	} 
	else if (m_dwRegisterResult & MB_NO || m_dwRegisterResult & MB_EXIT)
	{
		m_dwRegisterResult	= 0;
		m_dwRegisterPrice	= 0;
		m_cRegisterIndex	= 0;

		return true;
	}

	CRYLGameScene*	    pScene  = (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
	CRYLNewTradeDlg*    pDlg    = (CRYLNewTradeDlg*)pScene->GetNewTradeDlg();

	// 모두 수리
	pDlg->UpdateAllRepair(pGame);

	return false;
}

void CRYLCampShopDlg::ProcessSell(void)
{
	CRYLGameData* pGame = CRYLGameData::Instance();

	unsigned long dwSellPrice = pGame->m_lpPickItem->m_lpItemBase->GetSellPrice();
    if (TRUE == pGame->m_lpPickItem->GetIsEnableStack())
	{
		dwSellPrice *= pGame->m_lpPickItem->GetNowDurability();
	} 

	// 세율 적용
	dwSellPrice -= dwSellPrice * (m_cTax / 100.0f);

	CHAR strMessage[ MAX_PATH ];
	sprintf(strMessage, CRYLStringTable::m_strString[ 2998 ], 
		m_pShopOwnerValueLabel->GetCaption(), CRYLStringTable::MakePrintGold(dwSellPrice));

	CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
	lpMessageBox->Create(strMessage, MB_YES | MB_NO | MB_EXIT);
	lpMessageBox->SetResult(&m_dwSellResult);

	m_dwSellResult = 0;
}

bool CRYLCampShopDlg::ProcessBuy(CItemInstance* lpItem)
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	
	// 금액 부족
	if (pGame->m_csStatus.m_Info.Gold < lpItem->m_lpItemBase->GetBuyPrice())
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[ 252 ]);
		return false;
	}

	// 인벤에 공간 부족
	Item::ItemPos itemPos;
	
	if (!pGame->m_csStatus.GetAbleInven(lpItem->GetProtoTypeID(), 1, itemPos, FALSE))
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[ 13 ]);
		return false;
	}

	if (TRUE == g_DeviceInput.GetIsLeftMouseUp())
	{
		m_pBuyItem = lpItem;

		// 구매 확인 메세지
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;

		if (TRUE == m_pBuyItem->GetIsEnableStack())
		{	
			lpMessageBox->Create(CRYLStringTable::m_strString[ 254 ], MB_NUMBER | MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetValue(&m_dwBuyNum);
			lpMessageBox->SetEnableEnter(TRUE);
		} 
		else
		{
			char strMessage[MAX_PATH];
			sprintf(strMessage, CRYLStringTable::m_strString[ 255 ], 
				CRYLStringTable::MakePrintGold(m_pBuyItem->m_lpItemBase->GetBuyPrice()));
			lpMessageBox->Create(strMessage, MB_YES | MB_NO | MB_EXIT);
		}

		lpMessageBox->SetResult(&m_dwBuyResult);
	}
	else if (TRUE == g_DeviceInput.GetIsRightMouseUp())
	{
		// 바로 구매
		m_dwBuyResult = MB_YES;
		m_pBuyItem = lpItem;

		if (TRUE == m_pBuyItem->GetIsEnableStack())
		{
			m_dwBuyNum = m_pBuyItem->GetNowDurability();
		}
	}

	return true;
}

void CRYLCampShopDlg::ProcessRegister(void)
{
	CRYLGameData* pGame = CRYLGameData::Instance();

	if (TakeType::TS_INVEN == pGame->m_lpPickItem->GetItemPos())
	{
		if (TRUE == pGame->m_lpPickItem->GetIsEnableStack())
		{
			CItemInstance* lpPreItem = pGame->m_csStatus.GetItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());
			if (NULL != lpPreItem && lpPreItem->GetProtoTypeID() == pGame->m_lpPickItem->GetProtoTypeID())
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 139 ]);
				return;
			}
		}

		POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
		unsigned long dwInvenX = (ptMousePos->x - (GetLeft() + 111)) / 26;
		unsigned long dwInvenY = (ptMousePos->y - (GetTop() + 67)) / 26;

		if (true == CheckItemIndex(pGame->m_lpPickItem, dwInvenX, dwInvenY))
		{
			Item::ItemPos itemPos;
			itemPos.SetPos(dwInvenX, dwInvenY, 0);
			m_cRegisterIndex = itemPos.m_cIndex;

			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[590], MB_NUMBER | MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetResult(&m_dwRegisterResult);
			lpMessageBox->SetValue(&m_dwRegisterPrice);
			lpMessageBox->SetEnableEnter(TRUE);

			m_dwRegisterResult = 0;
			m_dwRegisterPrice  = 0;
		}
	}
}

void CRYLCampShopDlg::ProcessCancel(CItemInstance* lpItem)
{
	Item::CItem* lpItemBase		= lpItem->m_lpItemBase;

	TakeType takeType;
	takeType.m_srcPos			= lpItemBase->GetPos();
	takeType.m_dstPos.m_cPos	= TakeType::TS_TEMP;
	takeType.m_dstPos.m_cIndex	= Item::ExtraSpacePos::HOLDITEM_POS;
	takeType.m_cNum				= (lpItemBase->IsSet(Item::DetailData::STACKABLE)) ? lpItemBase->GetNumOrDurability() : 1;

//	g_pClientSocket->SetStatusFlag(NS_STALLREGISTERITEM);
	CRYLGameData*		pGame		= CRYLGameData::Instance();
	pGame->m_bStallRegFlag = TRUE;
	SendPacket::CharStallRegisterItem(g_GameSession, 
		CRYLNetworkData::Instance()->m_dwMyChrID, m_dwOwnerID, takeType, 0, PktStRI::SC_CAMP_CANCEL);
}

bool CRYLCampShopDlg::CheckItemIndex(CItemInstance *lpItem, unsigned char cX, unsigned char cY)
{
	const Item::ItemInfo& itemInfo = lpItem->GetItemInfo();

	if (cX + itemInfo.m_DetailData.m_cXSize > CONTAINER_WIDTH || 
		cY + itemInfo.m_DetailData.m_cYSize > CONTAINER_HEIGHT) 
	{
		return false;
	}

	for (unsigned char cYIndex = cY; cYIndex < cY + itemInfo.m_DetailData.m_cYSize; ++cYIndex)
	{
		for (unsigned char cXIndex = cX; cXIndex < cX + itemInfo.m_DetailData.m_cXSize; ++cXIndex)
		{
			if (m_aryItemIndex[cYIndex][cXIndex] != ITEM_EMPTY)
			{
				return false;
			}
		}
	}

	return true;
}
