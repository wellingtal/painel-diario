#include <winsock2.h>
#include "RYLGeneralOptionPanel.h"
#include "RYLLabel.h"
#include "RYLButton.h"
#include "RYLCheckBox.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"
#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLGameData.h"
#include "RYLOptionDlg.h"

#include "RYLClientMain.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLOptionDlg.h"
#include "RYLNetworkData.h"
#include <Network/ClientSocket/Send/SendEtc.h>
#include "GMMemory.h"

#include "RYLJoystickConfigureDlg.h" 


CRYLGeneralOptionPanel*	g_pGeneralOption = NULL ;

CRYLGeneralOptionPanel::CRYLGeneralOptionPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pProposalRefusalLabel = NULL ;		// ��û �ź�
	m_pChattingLabel		= NULL ;		// ��ȭ
	m_pCharacternameLabel	= NULL ;		// ĳ���� �̸�

	m_pDuelLabel		= NULL ;			// 1 : 1 ��û
	m_pExchangeLabel	= NULL ;			// �ŷ���û�ź�
	m_pPartyLabel		= NULL ;			// ��Ƽ��û�ź�
	m_pFriendLabel		= NULL ;			// ģ����ϰź�

	m_pWhisperLabel		= NULL ;			// �ӼӸ� ����
	m_pShoutLabel		= NULL ;			// ��ġ�� ����
	m_pTipBroadLabel	= NULL ;			// ���� ����
	m_pChatLabel		= NULL ;			// �Ϲ� ��ȭ ����
	
	m_pCharName01Label	= NULL ;			// ��ü ����
	m_pCharName02Label	= NULL ;			// �Ʊ��� ����
	m_pCharName03Label	= NULL ;			// ��Ƽ���� ����
	m_pCharName04Label	= NULL ;			// �̸� ���� ����

	m_pDuelButton		= NULL ;			// 1 : 1 ��û
	m_pExchangeButton	= NULL ;			// �ŷ���û�ź�
	m_pPartyButton		= NULL ;			// ��Ƽ��û�ź�
	m_pFriendButton		= NULL ;			// ģ����ϰź�

	m_pWhisperButton	= NULL ;			// �ӼӸ� ����
	m_pShoutButton		= NULL ;			// ��ġ�� ����
	m_pTipBroadButton	= NULL ;			// ���� ����
	m_pChatButton		= NULL ;			// �Ϲ� ��ȭ ����
	
	m_pCharName01Button = NULL ;			// ��ü ����
	m_pCharName02Button = NULL ;			// �Ʊ��� ����
	m_pCharName03Button = NULL ;			// ��Ƽ���� ����
	m_pCharName04Button = NULL ;			// �̸� ���� ����

	m_pOkButton			= NULL ;			

	m_pJoyButton		= NULL ;
	
	m_pGMUIParentDlg = pParentDlg ;
	
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	RECT			rt ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 395, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 74 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( i - 1 ) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 24, 22, 195, 100, 0xff6e6857, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 24, 145, 195, 84, 0xff6e6857, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 24, 253, 195, 84, 0xff6e6857, 0xc0000000,  1 ) ;		

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 395 ) ;

	g_pGeneralOption = this ;
}

CRYLGeneralOptionPanel::~CRYLGeneralOptionPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLGeneralOptionPanel::InitCtrl() 
{
	// Title
	// ��û�ź�
	m_pProposalRefusalLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pProposalRefusalLabel->SetLeft( 27 ) ; 
	m_pProposalRefusalLabel->SetTop( 7 ) ;
	m_pProposalRefusalLabel->SetFontColor (0xffffffff);
	m_pProposalRefusalLabel->SetAutoSize( TRUE ) ;
	m_pProposalRefusalLabel->SetCaption( CRYLStringTable::m_strString[ 2168 ] ) ;
	CGMUICustomPanel::AddChild( m_pProposalRefusalLabel ) ;

	// ��ȭ
	m_pChattingLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pChattingLabel->SetLeft( 28 ) ;
	m_pChattingLabel->SetTop( 130 ) ;
	m_pChattingLabel->SetFontColor (0xffffffff);
	m_pChattingLabel->SetAutoSize( TRUE ) ;
	m_pChattingLabel->SetCaption( CRYLStringTable::m_strString[ 2169 ] ) ;
	CGMUICustomPanel::AddChild( m_pChattingLabel ) ;

	// ĳ���� �̸�
	m_pCharacternameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCharacternameLabel->SetLeft( 27 ) ; 
	m_pCharacternameLabel->SetTop( 238 ) ;
	m_pCharacternameLabel->SetFontColor (0xffffffff);
	m_pCharacternameLabel->SetAutoSize( TRUE ) ;
	m_pCharacternameLabel->SetCaption( CRYLStringTable::m_strString[ 2170 ] ) ;
	CGMUICustomPanel::AddChild( m_pCharacternameLabel ) ;

	// SubMenu
	// 1 : 1 ��û
	m_pDuelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pDuelLabel->SetLeft( 79 ) ;
	m_pDuelLabel->SetTop( 28 ) ;
	m_pDuelLabel->SetFontColor (0xffffffff);
	m_pDuelLabel->SetAutoSize( TRUE ) ;
	m_pDuelLabel->SetCaption( CRYLStringTable::m_strString[ 2174 ] ) ;
	CGMUICustomPanel::AddChild( m_pDuelLabel ) ;

	// �ŷ���û�ź�
	m_pExchangeLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pExchangeLabel->SetLeft( 78 ) ;
	m_pExchangeLabel->SetTop( 47 ) ;
	m_pExchangeLabel->SetFontColor (0xffffffff);
	m_pExchangeLabel->SetAutoSize( TRUE ) ;
	m_pExchangeLabel->SetCaption( CRYLStringTable::m_strString[ 2175 ] ) ;
	CGMUICustomPanel::AddChild( m_pExchangeLabel ) ;

	// ��Ƽ��û�ź�
	m_pPartyLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPartyLabel->SetLeft( 78 ) ;
	m_pPartyLabel->SetTop( 66 ) ;
	m_pPartyLabel->SetFontColor (0xffffffff);
	m_pPartyLabel->SetAutoSize( TRUE ) ;
	m_pPartyLabel->SetCaption( CRYLStringTable::m_strString[ 2176 ] ) ;
	CGMUICustomPanel::AddChild( m_pPartyLabel ) ;

	// ģ����ϰź�
	m_pFriendLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pFriendLabel->SetLeft( 78 ) ;
	m_pFriendLabel->SetTop( 85 ) ;
	m_pFriendLabel->SetFontColor (0xffffffff);
	m_pFriendLabel->SetAutoSize( TRUE ) ;
	m_pFriendLabel->SetCaption( CRYLStringTable::m_strString[ 2177 ] ) ;
	CGMUICustomPanel::AddChild( m_pFriendLabel ) ;

	// ���� �ź�
	m_pRecallLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pRecallLabel->SetLeft( 78 ) ;
	m_pRecallLabel->SetTop( 104 ) ;
	m_pRecallLabel->SetFontColor (0xffffffff);
	m_pRecallLabel->SetAutoSize( TRUE ) ;
	m_pRecallLabel->SetCaption( CRYLStringTable::m_strString[ 2173 ] ) ;
	CGMUICustomPanel::AddChild( m_pRecallLabel ) ;
	//===================================================================================

	// �ӼӸ� ����
	m_pWhisperLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pWhisperLabel->SetLeft( 79 ) ;
	m_pWhisperLabel->SetTop( 154 ) ;
	m_pWhisperLabel->SetFontColor (0xffffffff);
	m_pWhisperLabel->SetAutoSize( TRUE ) ;
	m_pWhisperLabel->SetCaption( CRYLStringTable::m_strString[ 2178 ] ) ;
	CGMUICustomPanel::AddChild( m_pWhisperLabel ) ;

	// ��ġ�� ����
	m_pShoutLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pShoutLabel->SetLeft( 79 ) ;
	m_pShoutLabel->SetTop( 173 ) ;
	m_pShoutLabel->SetFontColor (0xffffffff);
	m_pShoutLabel->SetAutoSize( TRUE ) ;
	m_pShoutLabel->SetCaption( CRYLStringTable::m_strString[ 2179 ] ) ;
	CGMUICustomPanel::AddChild( m_pShoutLabel ) ;

	// ���� ����
	m_pTipBroadLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pTipBroadLabel->SetLeft( 79 ) ;
	m_pTipBroadLabel->SetTop( 192 ) ;
	m_pTipBroadLabel->SetFontColor (0xffffffff);
	m_pTipBroadLabel->SetAutoSize( TRUE ) ;
	m_pTipBroadLabel->SetCaption( CRYLStringTable::m_strString[ 2180 ] ) ;
	CGMUICustomPanel::AddChild( m_pTipBroadLabel ) ;

	// �Ϲ� ��ȭ ����
	m_pChatLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pChatLabel->SetLeft( 79 ) ;
	m_pChatLabel->SetTop( 211 ) ;
	m_pChatLabel->SetFontColor (0xffffffff);
	m_pChatLabel->SetAutoSize( TRUE ) ;
	m_pChatLabel->SetCaption( CRYLStringTable::m_strString[ 2181 ] ) ;
	CGMUICustomPanel::AddChild( m_pChatLabel ) ;

	// -------------------------------------------------------------------------

	// ��ü ����
	m_pCharName01Label = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCharName01Label->SetLeft( 79 ) ;
	m_pCharName01Label->SetTop( 262 ) ;
	m_pCharName01Label->SetFontColor (0xffffffff);
	m_pCharName01Label->SetAutoSize( TRUE ) ;
	m_pCharName01Label->SetCaption( CRYLStringTable::m_strString[ 3159 ] );			
	CGMUICustomPanel::AddChild( m_pCharName01Label ) ;

	// �Ʊ��� ����
	m_pCharName02Label = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCharName02Label->SetLeft( 79 ) ;
	m_pCharName02Label->SetTop( 281 ) ;
	m_pCharName02Label->SetFontColor (0xffffffff);
	m_pCharName02Label->SetAutoSize( TRUE ) ;
	m_pCharName02Label->SetCaption( CRYLStringTable::m_strString[ 3160 ] );			
	CGMUICustomPanel::AddChild( m_pCharName02Label ) ;

	// ��Ƽ���� ����
	m_pCharName03Label = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCharName03Label->SetLeft( 79 ) ;
	m_pCharName03Label->SetTop( 300 ) ;
	m_pCharName03Label->SetFontColor (0xffffffff);
	m_pCharName03Label->SetAutoSize( TRUE ) ;
	m_pCharName03Label->SetCaption( CRYLStringTable::m_strString[ 3161 ] );		
	CGMUICustomPanel::AddChild( m_pCharName03Label ) ;

	// �̸� ���� ����
	m_pCharName04Label = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pCharName04Label->SetLeft( 79 ) ;
	m_pCharName04Label->SetTop( 319 ) ;
	m_pCharName04Label->SetFontColor (0xffffffff);
	m_pCharName04Label->SetAutoSize( TRUE ) ;
	m_pCharName04Label->SetCaption( CRYLStringTable::m_strString[ 3162 ] );		
	CGMUICustomPanel::AddChild( m_pCharName04Label ) ;

	// -------------------------------------------------------------------------

	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	// 1 : 1 ��û
	m_pDuelButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pDuelButton->SetLeft( 42 ) ;
	m_pDuelButton->SetTop( 26 ) ;
	m_pDuelButton->SetInstance( RYLGENERALOPTIONPANEL::DUEL_BUTTON ) ;
	m_pDuelButton->SetSecondParent( this ) ;
	m_pDuelButton->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pDuelButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pDuelButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pDuelButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pDuelButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pDuelButton ) ;

	// �ŷ���û�ź�
	m_pExchangeButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pExchangeButton->SetLeft( 42) ;
	m_pExchangeButton->SetTop( 45 ) ;
	m_pExchangeButton->SetInstance( RYLGENERALOPTIONPANEL::EXCHANGE_BUTTON ) ;
	m_pExchangeButton->SetSecondParent( this ) ;
	m_pExchangeButton->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pExchangeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pExchangeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pExchangeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pExchangeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pExchangeButton ) ;

    // ��Ƽ��û�ź�
	m_pPartyButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pPartyButton->SetLeft( 42 ) ;
	m_pPartyButton->SetTop( 64 ) ;
	m_pPartyButton->SetInstance( RYLGENERALOPTIONPANEL::PARTY_BUTTON ) ;
	m_pPartyButton->SetSecondParent( this ) ;
	m_pPartyButton->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPartyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pPartyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pPartyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPartyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPartyButton ) ;

	// ģ����ϰź�
	m_pFriendButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pFriendButton->SetLeft( 42 ) ;
	m_pFriendButton->SetTop( 83 ) ;
	m_pFriendButton->SetInstance( RYLGENERALOPTIONPANEL::FRIEND_BUTTON ) ;
	m_pFriendButton->SetSecondParent( this ) ;
	m_pFriendButton->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pFriendButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pFriendButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pFriendButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pFriendButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pFriendButton ) ;

	// ���ݰź�
	m_pRecallButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pRecallButton->SetLeft( 42 ) ;
	m_pRecallButton->SetTop( 102 ) ;
	m_pRecallButton->SetInstance( RYLGENERALOPTIONPANEL::RECALL_BUTTON ) ;
	m_pRecallButton->SetSecondParent( this ) ;
	m_pRecallButton->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pRecallButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pRecallButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pRecallButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pRecallButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pRecallButton ) ;
	




	// �ӼӸ� ����
	m_pWhisperButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pWhisperButton->SetLeft( 42 ) ;
	m_pWhisperButton->SetTop( 151 ) ;
	m_pWhisperButton->SetInstance( RYLGENERALOPTIONPANEL::WHISPER_BUTTON ) ;
	m_pWhisperButton->SetSecondParent( this ) ;
	m_pWhisperButton->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pWhisperButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pWhisperButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pWhisperButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pWhisperButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pWhisperButton->SetChecked( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pWhisperButton ) ;

	// ��ġ�� ����
	m_pShoutButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pShoutButton->SetLeft( 42 ) ;
	m_pShoutButton->SetTop( 170 ) ;
	m_pShoutButton->SetInstance( RYLGENERALOPTIONPANEL::SHOUT_BUTTON ) ;
	m_pShoutButton->SetSecondParent( this ) ;
	m_pShoutButton->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pShoutButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pShoutButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pShoutButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pShoutButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pShoutButton->SetChecked( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pShoutButton ) ;

	// ���� ����
	m_pTipBroadButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pTipBroadButton->SetLeft( 42 ) ;
	m_pTipBroadButton->SetTop( 189 ) ;
	m_pTipBroadButton->SetInstance( RYLGENERALOPTIONPANEL::TIPBROAD_BUTTON ) ;
	m_pTipBroadButton->SetSecondParent( this ) ;
	m_pTipBroadButton->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pTipBroadButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pTipBroadButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pTipBroadButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pTipBroadButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pTipBroadButton->SetChecked( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pTipBroadButton ) ;

	// �Ϲ� ��ȭ ����
	m_pChatButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pChatButton->SetLeft( 42 ) ;
	m_pChatButton->SetTop( 208 ) ;
	m_pChatButton->SetInstance( RYLGENERALOPTIONPANEL::CHAT_BUTTON ) ;
	m_pChatButton->SetSecondParent( this ) ;
	m_pChatButton->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pChatButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pChatButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pChatButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pChatButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pChatButton->SetChecked( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pChatButton ) ;

	// ��ü ����
	m_pCharName01Button = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pCharName01Button->SetLeft( 42 ) ;
	m_pCharName01Button->SetTop( 259 ) ;
	m_pCharName01Button->SetInstance( RYLGENERALOPTIONPANEL::CHARNAME01_BUTTON ) ;
	//m_pCharName01Button->OnClickEvent = DlgClick ;
	m_pCharName01Button->SetSecondParent( this ) ;
	m_pCharName01Button->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCharName01Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCharName01Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCharName01Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCharName01Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCharName01Button->SetChecked( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pCharName01Button ) ;

	// �Ʊ��� ����
	m_pCharName02Button = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pCharName02Button->SetLeft( 42) ;
	m_pCharName02Button->SetTop( 278) ;
	m_pCharName02Button->SetInstance( RYLGENERALOPTIONPANEL::CHARNAME02_BUTTON ) ;
	//m_pCharName02Button->OnClickEvent = DlgClick ;
	m_pCharName02Button->SetSecondParent( this ) ;
	m_pCharName02Button->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCharName02Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCharName02Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCharName02Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCharName02Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pCharName02Button ) ;

	// ��Ƽ���� ����
	m_pCharName03Button = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pCharName03Button->SetLeft( 42 ) ;
	m_pCharName03Button->SetTop( 297 ) ;
	m_pCharName03Button->SetInstance( RYLGENERALOPTIONPANEL::CHARNAME03_BUTTON ) ;
	//m_pCharName03Button->OnClickEvent = DlgClick ;
	m_pCharName03Button->SetSecondParent( this ) ;
	m_pCharName03Button->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCharName03Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCharName03Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCharName03Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCharName03Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pCharName03Button ) ;

	// �̸� ���� ����
	m_pCharName04Button = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pCharName04Button->SetLeft( 42 ) ;
	m_pCharName04Button->SetTop( 316 ) ;
	m_pCharName04Button->SetInstance( RYLGENERALOPTIONPANEL::CHARNAME04_BUTTON ) ;
	//m_pCharName04Button->OnClickEvent = DlgClick ;
	m_pCharName04Button->SetSecondParent( this ) ;
	m_pCharName04Button->OnMouseUpEvent = GeneralOptionMouseUp ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCharName04Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCharName04Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pCharName04Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pCharName04Button->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pCharName04Button ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	// Ȯ��
	m_pOkButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pOkButton->SetLeft( 154 ) ;
	m_pOkButton->SetTop( 345 ) ;
	m_pOkButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOkButton->SetFontColor( 0xffffffff) ;
	m_pOkButton->SetColor( 0xffffffff) ;
	m_pOkButton->OnClickEvent = DlgClick ;
	m_pOkButton->SetInstance( RYLGENERALOPTIONPANEL::OK_BUTTON ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pOkButton ) ;

	// �����е� ����.

	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();
	m_pJoyButton = new CRYLButton( m_pGMUIParentDlg ) ;

	// �ѱ������� �ϴ� ������� �ʱ� ������ ���ƵӴϴ�. (2006-05-02 by hackermz)
	// �׽�Ʈ �������� ������Ʈ���� UseJoystick �׸��� �����մϴ�. (2006-05-15 by hackermz)
//	if (GameRYL::KOREA != pNetwork->m_eInternationalCode)
	if (1 == g_ClientMain.GetUseJoystick())
	{
		m_pJoyButton->SetLeft( 54 ) ;
		m_pJoyButton->SetTop( 345 ) ;
		m_pJoyButton->SetCaption( CRYLStringTable::m_strString[ 4221 ] ) ;
		m_pJoyButton->SetFontColor( 0xffffffff) ;
		m_pJoyButton->SetColor( 0xffffffff) ;
		m_pJoyButton->OnClickEvent = DlgClick ;
		m_pJoyButton->SetInstance( RYLGENERALOPTIONPANEL::JOY_BUTTON ) ;
		m_pJoyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pJoyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 6), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pJoyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pJoyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		CGMUICustomPanel::AddChild( m_pJoyButton ) ;
	}	
}

VOID	CRYLGeneralOptionPanel::FinalCtrl() 
{
	GM_DELETE( m_pProposalRefusalLabel ) ;	// ��û �ź�
	GM_DELETE( m_pChattingLabel ) ;			// ��ȭ
	GM_DELETE( m_pCharacternameLabel ) ;	// ĳ���� �̸�

	GM_DELETE( m_pDuelLabel ) ;				// 1 : 1 ��û
	GM_DELETE( m_pExchangeLabel ) ;			// �ŷ���û�ź�
	GM_DELETE( m_pPartyLabel ) ;			// ��Ƽ��û�ź�
	GM_DELETE( m_pFriendLabel ) ;			// ģ����ϰź�
	GM_DELETE( m_pRecallLabel ) ;			// ���ݰź�

	GM_DELETE( m_pWhisperLabel ) ;			// �ӼӸ� ����
	GM_DELETE( m_pShoutLabel ) ;			// ��ġ�� ����
	GM_DELETE( m_pTipBroadLabel ) ;			// ���� ����
	GM_DELETE( m_pChatLabel ) ;				// �Ϲ� ��ȭ ����
	
	GM_DELETE( m_pCharName01Label ) ;		// ��ü ����
	GM_DELETE( m_pCharName02Label ) ;		// �Ʊ��� ����
	GM_DELETE( m_pCharName03Label ) ;		// ��Ƽ���� ����
	GM_DELETE( m_pCharName04Label ) ;		// �̸� ���� ����

	GM_DELETE( m_pDuelButton ) ;			// 1 : 1 ��û
	GM_DELETE( m_pExchangeButton ) ;		// �ŷ���û�ź�
	GM_DELETE( m_pPartyButton ) ;			// ��Ƽ��û�ź�
	GM_DELETE( m_pFriendButton ) ;			// ģ����ϰź�
	GM_DELETE( m_pRecallButton ) ;			// ���ݰź�
	

	GM_DELETE( m_pWhisperButton ) ;			// �ӼӸ� ����
	GM_DELETE( m_pShoutButton ) ;			// ��ġ�� ����
	GM_DELETE( m_pTipBroadButton ) ;		// ���� ����
	GM_DELETE( m_pChatButton ) ;			// �Ϲ� ��ȭ ����

	GM_DELETE( m_pCharName01Button ) ;		// ��ü ����
	GM_DELETE( m_pCharName02Button ) ;		// �Ʊ��� ����
	GM_DELETE( m_pCharName03Button ) ;		// ��Ƽ���� ����
	GM_DELETE( m_pCharName04Button ) ;		// �̸� ���� ����

	GM_DELETE( m_pOkButton ) ;				// Ȯ��

	GM_DELETE( m_pJoyButton ) ;				// �����е�.
}

HRESULT		CRYLGeneralOptionPanel::Update()
{
	
	return S_OK ;
}

VOID		CRYLGeneralOptionPanel::ReLoadOption() 
{
	m_pDuelButton->SetChecked( CRYLOptionDlg::m_roOption.Reject.m_Duel ) ;
	m_pExchangeButton->SetChecked( CRYLOptionDlg::m_roOption.Reject.m_Exchange ) ;
	m_pPartyButton->SetChecked( CRYLOptionDlg::m_roOption.Reject.m_Party ) ;	
	m_pFriendButton->SetChecked( CRYLOptionDlg::m_roOption.Reject.m_Friend ) ;	
	m_pRecallButton->SetChecked( CRYLOptionDlg::m_roOption.Reject.m_Recall ) ;	
	m_pWhisperButton->SetChecked( !(CRYLOptionDlg::m_roOption.Reject.m_Whisper) ) ;
	m_pShoutButton->SetChecked( !(CRYLOptionDlg::m_roOption.Reject.m_Shout) ) ;
	m_pTipBroadButton->SetChecked( CRYLOptionDlg::m_coOption.m_bTipBroad ) ;
	m_pChatButton->SetChecked( CRYLOptionDlg::m_coOption.m_bChat ) ;	

	m_pCharName01Button->SetChecked( FALSE ) ;
	m_pCharName02Button->SetChecked( FALSE ) ;
	m_pCharName03Button->SetChecked( FALSE ) ;
	m_pCharName04Button->SetChecked( FALSE ) ;

	if( CRYLOptionDlg::m_coOption.m_dwCharName & 1 )
		m_pCharName01Button->SetChecked( TRUE );
	if( CRYLOptionDlg::m_coOption.m_dwCharName & 2 )
		m_pCharName02Button->SetChecked( TRUE );
	if( CRYLOptionDlg::m_coOption.m_dwCharName & 4 )
		m_pCharName03Button->SetChecked( TRUE );
	if( CRYLOptionDlg::m_coOption.m_dwCharName & 8 )
		m_pCharName04Button->SetChecked( TRUE );
}

VOID		CRYLGeneralOptionPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pGeneralOption )
		return ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	switch ( Sender->GetInstance() )
	{
		case RYLGENERALOPTIONPANEL::DUEL_BUTTON :
			if ( g_pGeneralOption->m_pDuelButton->GetChecked() )		
				CRYLOptionDlg::m_roOption.Reject.m_Duel = 1;				// 1:1 ��û ON
			else
				CRYLOptionDlg::m_roOption.Reject.m_Duel = 0;				// 1:1 ��û Off
			
			pGame->PlayClickSound() ;
			break ;

		case RYLGENERALOPTIONPANEL::EXCHANGE_BUTTON :
			if ( g_pGeneralOption->m_pExchangeButton->GetChecked() )
				CRYLOptionDlg::m_roOption.Reject.m_Exchange = 1;
			else
				CRYLOptionDlg::m_roOption.Reject.m_Exchange = 0;

			pGame->PlayClickSound() ;
			break ;

		case RYLGENERALOPTIONPANEL::PARTY_BUTTON	:	
			if ( g_pGeneralOption->m_pPartyButton->GetChecked() )
				CRYLOptionDlg::m_roOption.Reject.m_Party = 1 ;	
			else
				CRYLOptionDlg::m_roOption.Reject.m_Party = 0 ;	

			pGame->PlayClickSound() ;
			break ;

		case RYLGENERALOPTIONPANEL::FRIEND_BUTTON	:	
			if ( g_pGeneralOption->m_pFriendButton->GetChecked() )
				CRYLOptionDlg::m_roOption.Reject.m_Friend = 1;	
			else
				CRYLOptionDlg::m_roOption.Reject.m_Friend = 0;	

			pGame->PlayClickSound() ;
			break ;

		case RYLGENERALOPTIONPANEL::RECALL_BUTTON	:	
			if ( g_pGeneralOption->m_pRecallButton->GetChecked() )
				CRYLOptionDlg::m_roOption.Reject.m_Recall = 1;	
			else
				CRYLOptionDlg::m_roOption.Reject.m_Recall = 0;	

			pGame->PlayClickSound() ;
			break ;


		case RYLGENERALOPTIONPANEL::WHISPER_BUTTON :		
			if ( g_pGeneralOption->m_pWhisperButton->GetChecked() )
				CRYLOptionDlg::m_roOption.Reject.m_Whisper = 0;
			else
				CRYLOptionDlg::m_roOption.Reject.m_Whisper = 1;

			pGame->PlayClickSound() ;
			break ;

		case RYLGENERALOPTIONPANEL::SHOUT_BUTTON	 :	
			if ( g_pGeneralOption->m_pShoutButton->GetChecked() )
				CRYLOptionDlg::m_roOption.Reject.m_Shout = 0;
			else
				CRYLOptionDlg::m_roOption.Reject.m_Shout = 1;

			pGame->PlayClickSound() ;
			break ;

		case RYLGENERALOPTIONPANEL::TIPBROAD_BUTTON :		
			if ( g_pGeneralOption->m_pTipBroadButton->GetChecked() )
				CRYLOptionDlg::m_coOption.m_bTipBroad = TRUE ;
			else
				CRYLOptionDlg::m_coOption.m_bTipBroad = FALSE ;

			pGame->PlayClickSound() ;
			break ;

		case RYLGENERALOPTIONPANEL::CHAT_BUTTON	 :		
			if ( g_pGeneralOption->m_pChatButton->GetChecked() )
				CRYLOptionDlg::m_coOption.m_bChat = TRUE ;	
			else
				CRYLOptionDlg::m_coOption.m_bChat = FALSE ;	

			pGame->PlayClickSound() ;
			break ;
		
		case RYLGENERALOPTIONPANEL::OK_BUTTON	:
		{
			CRYLGameScene*	pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
			CRYLOptionDlg*  pOptionDlg	= static_cast<CRYLOptionDlg *>( pScene->GetOptionDlg() );

			pOptionDlg->OK();
			break;
		}
		case RYLGENERALOPTIONPANEL::JOY_BUTTON:
		{
			CRYLGameScene*				pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
			CRYLJoystickConfigureDlg*	pJoyDlg		= static_cast<CRYLJoystickConfigureDlg *>( pScene->GetJoystickConfigureDlg() );
			CRYLOptionDlg*				pOptionDlg	= static_cast<CRYLOptionDlg *>( pScene->GetOptionDlg() );

			pJoyDlg->SetVisible(TRUE);
			pOptionDlg->SetVisible(FALSE);

			break;
		}
	}
}

VOID	CRYLGeneralOptionPanel::GeneralOptionMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	if ( !g_pGeneralOption )
		return ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	switch ( Sender->GetInstance() )
	{
	case RYLGENERALOPTIONPANEL::DUEL_BUTTON :
		if ( g_pGeneralOption->m_pDuelButton->GetChecked() )		
			CRYLOptionDlg::m_roOption.Reject.m_Duel = 1;				// 1:1 ��û ON
		else
			CRYLOptionDlg::m_roOption.Reject.m_Duel = 0;				// 1:1 ��û Off
		
		pGame->PlayClickSound() ;
		break ;
	case RYLGENERALOPTIONPANEL::EXCHANGE_BUTTON :
		if ( g_pGeneralOption->m_pExchangeButton->GetChecked() )
			CRYLOptionDlg::m_roOption.Reject.m_Exchange = 1;
		else
			CRYLOptionDlg::m_roOption.Reject.m_Exchange = 0;

		pGame->PlayClickSound() ;
		break ;
	case RYLGENERALOPTIONPANEL::PARTY_BUTTON	:	
		if ( g_pGeneralOption->m_pPartyButton->GetChecked() )
			CRYLOptionDlg::m_roOption.Reject.m_Party = 1 ;	
		else
			CRYLOptionDlg::m_roOption.Reject.m_Party = 0 ;	

		pGame->PlayClickSound() ;
		break ;
	case RYLGENERALOPTIONPANEL::FRIEND_BUTTON	:	
		if ( g_pGeneralOption->m_pFriendButton->GetChecked() )
			CRYLOptionDlg::m_roOption.Reject.m_Friend = 1;	
		else
			CRYLOptionDlg::m_roOption.Reject.m_Friend = 0;	

		pGame->PlayClickSound() ;
		break ;

	case RYLGENERALOPTIONPANEL::RECALL_BUTTON	:	
		if ( g_pGeneralOption->m_pRecallButton->GetChecked() )
			CRYLOptionDlg::m_roOption.Reject.m_Recall = 1;	
		else
			CRYLOptionDlg::m_roOption.Reject.m_Recall = 0;	

		pGame->PlayClickSound() ;
		break ;


	case RYLGENERALOPTIONPANEL::WHISPER_BUTTON :		
		if ( g_pGeneralOption->m_pWhisperButton->GetChecked() )
			CRYLOptionDlg::m_roOption.Reject.m_Whisper = 0;
		else
			CRYLOptionDlg::m_roOption.Reject.m_Whisper = 1;

		pGame->PlayClickSound() ;
		break ;
	case RYLGENERALOPTIONPANEL::SHOUT_BUTTON	 :	
		if ( g_pGeneralOption->m_pShoutButton->GetChecked() )
			CRYLOptionDlg::m_roOption.Reject.m_Shout = 0 ;
		else
			CRYLOptionDlg::m_roOption.Reject.m_Shout = 1;

		pGame->PlayClickSound() ;
		break ;
	case RYLGENERALOPTIONPANEL::TIPBROAD_BUTTON :		
		if ( g_pGeneralOption->m_pTipBroadButton->GetChecked() )
			CRYLOptionDlg::m_coOption.m_bTipBroad = TRUE ;
		else
			CRYLOptionDlg::m_coOption.m_bTipBroad = FALSE ;

		pGame->PlayClickSound() ;
		break ;
	case RYLGENERALOPTIONPANEL::CHAT_BUTTON	 :		
		if ( g_pGeneralOption->m_pChatButton->GetChecked() )
			CRYLOptionDlg::m_coOption.m_bChat = TRUE ;	
		else
			CRYLOptionDlg::m_coOption.m_bChat = FALSE ;	

		pGame->PlayClickSound() ;
		break ;
	case RYLGENERALOPTIONPANEL::CHARNAME01_BUTTON :
		if ( g_pGeneralOption->m_pCharName01Button->GetChecked() )
			CRYLOptionDlg::m_coOption.m_dwCharName |= 1;
		else
			CRYLOptionDlg::m_coOption.m_dwCharName ^= 1;	

		pGame->PlayClickSound() ;
		break;
	case RYLGENERALOPTIONPANEL::CHARNAME02_BUTTON :
		if ( g_pGeneralOption->m_pCharName02Button->GetChecked() )
			CRYLOptionDlg::m_coOption.m_dwCharName |= 2;
		else
			CRYLOptionDlg::m_coOption.m_dwCharName ^= 2;	
		pGame->PlayClickSound() ;
		break;
	case RYLGENERALOPTIONPANEL::CHARNAME03_BUTTON :
		if ( g_pGeneralOption->m_pCharName03Button->GetChecked() )
			CRYLOptionDlg::m_coOption.m_dwCharName |= 4;
		else
			CRYLOptionDlg::m_coOption.m_dwCharName ^= 4;	

		pGame->PlayClickSound() ;
		break;
	case RYLGENERALOPTIONPANEL::CHARNAME04_BUTTON :
		if ( g_pGeneralOption->m_pCharName04Button->GetChecked() )
			CRYLOptionDlg::m_coOption.m_dwCharName |= 8;
		else
			CRYLOptionDlg::m_coOption.m_dwCharName ^= 8;	

		pGame->PlayClickSound() ;
		break;
	}
}