
#include <winsock2.h>

#include <WinInput.h>

#include <Utility/math/Math.h>
#include <Utility/Exception.h>

#include <Network/ClientSocket/Send/SendEtc.h>
#include <Network/ClientSocket/Send/SendLoginout.h>
#include <Network/ClientSocket/Send/SendCastle.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>


#include "BaseDataDefine.h"
#include "GMUIObject.h"
#include "GMImageDib.h"

#include "RYLCheckBox.h"
#include "RYLButton.h"
#include "RYLTabButton.h"
#include "RYLListBox.h"
#include "RYLEdit.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLTimer.h"

#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLRaceBase.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"

#include "RYLGuildFrameDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLStreetStallDlg.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLLoginScene.h"
#include "RYLClientMain.h"

#include "RYLStringTable.h"
#include "..\\StringFilter.h"
#include "..\\EventHandler\\GameProcess\\MoveProcess.h"

#include <RenderOption.h>
#include "SoundMgr.h"
#include "RYLProfile.h"

#include "GameProcess/ItemProcess.h"

#include "RYLChattingDlg.h"

#include <Network/ClientSocket/Send/SendCommunity.h>

#include "RylGuildWarPenaltyDlg.h"

#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"
#include "RYLCastleManager.h"
#include <Creature/EnemyCheck.h>

#include <GameTime/GameTimeConstants.h>
#include "RYLGameTimeManager.h"

#include "RYLIntroScene.h"
#include "BgmManager.h"
#include "GMMemory.h"

#include "RYLUI/GMUITooltipBox.h"

#include "InputJoystick.h"

CRYLChattingDlg*	g_pChattingDlg = NULL ;
CRYLChattingDlg*	CRYLChattingDlg::s_pInstance = NULL ;

CRYLChattingDlg::CRYLChattingDlg( INT nLeft, INT nTop, INT nDlgType ) 
{
	//프라이머리 언어만 뽑는다. 
	m_usLocalLang = PRIMARYLANGID(GetSystemDefaultLangID());  

	// LANG_JAPANESE
	// LANG_ENGLISH
	// LANG_KOREAN

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	m_pChatType1Image		  = NULL ;
	m_pChatType2Image		  = NULL ;
	m_pChatType3Image		  = NULL ;
	m_pChatType4Image		  = NULL ;
	m_iCurrentSize			  = nDlgType ;
	m_bShout				  = FALSE ;
	m_bTrade				  = FALSE ;
	m_bGuild				  = FALSE ;
	m_bParty				  = FALSE ;
	m_bHideEnable			  = FALSE ;
	m_dwOldTabFocus			  = 0x00000000 ;

	m_pGeneralChatCheckBox = NULL ;
	m_pShoutChatCheckBox   = NULL ;
	m_pDealChatCheckBox    = NULL ;	
	m_pPartyChatCheckBox   = NULL ;
	m_pGuildChatCheckBox   = NULL ;

	m_pGeneralChatLabel		= NULL ;	
	m_pShoutChatLabel		= NULL ;	
	m_pDealChatLabel		= NULL ;	
	m_pPartyChatLabel		= NULL ;	
	m_pGuildChatLabel		= NULL ;	

#ifdef _RYL_LANGUAGE
	m_pAllChatCheckBox		= NULL;
	m_pAllChatLabel			= NULL ;
#endif 

	//====================================================================================================
	// 359 x 121
	CRYLSpriteList::Instance()->CreateGMImage( 359, 121, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 21, 121 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	SetRect( &rt, 21, 0, 143, 121 ) ;
	pImageDIB->DrawRect( &rt, 21, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 143, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	SetRect( &rt, 21, 0, 61, 121 ) ;
	pImageDIB->DrawRect( &rt, 265, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;

	SetRect( &rt, 143, 0, 197, 121 ) ;
	pImageDIB->DrawRect( &rt, 305, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;

	SetRect( &rt, 197, 0, 233, 18) ;
	pImageDIB->DrawRect( &rt, 1, 102, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	SetRect( &rt, 197, 60, 244, 78 ) ;
	pImageDIB->DrawRect( &rt, 37, 102, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;

	pImageDIB->DrawFrameRect( 2, 36, 355, 2, 0x80000000 ) ;
	pImageDIB->DrawFrameRect( 2, 15, 355, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 2, 35, 355, 1, 0xffa88a60 ) ;

	m_pChatType1Image = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChatType1Image->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChatType1Image ) ;
	GM_DELETE( pImageDIB ) ;

	// 비어 있는 이미지
	CRYLSpriteList::Instance()->CreateGMImage( 359, 121, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	pImageDIB->DrawFrameRect( 2, 38, 355, 64, 0x80000000 ) ;
	m_pChatType1EmptyImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChatType1EmptyImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChatType1EmptyImage ) ;
	GM_DELETE( pImageDIB ) ;

	//====================================================================================================
	// 563 x 121
	CRYLSpriteList::Instance()->CreateGMImage( 563, 121, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 21, 121 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	SetRect( &rt, 21, 0, 143, 121 ) ;
	pImageDIB->DrawRect( &rt, 21, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 143, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 265, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 387, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	SetRect( &rt, 143, 0, 197, 121 ) ;
	pImageDIB->DrawRect( &rt, 509, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	SetRect( &rt, 197, 0, 233, 18) ;
	pImageDIB->DrawRect( &rt, 1, 102, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	SetRect( &rt, 197, 60, 244, 78 ) ;
	pImageDIB->DrawRect( &rt, 37, 102, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;

	pImageDIB->DrawFrameRect( 2, 36, 559, 2, 0x80000000 ) ;
	pImageDIB->DrawFrameRect( 2, 15, 559, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 2, 35, 559, 1, 0xffa88a60 ) ;

	m_pChatType2Image = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChatType2Image->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChatType2Image ) ;
	GM_DELETE( pImageDIB ) ;


	// 비어 있는 이미지
	CRYLSpriteList::Instance()->CreateGMImage( 563, 121, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	pImageDIB->DrawFrameRect( 2, 38, 559, 64, 0x80000000 ) ;
	m_pChatType2EmptyImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChatType2EmptyImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChatType2EmptyImage ) ;
	GM_DELETE( pImageDIB ) ;
	//====================================================================================================
	// 359 x 246
	CRYLSpriteList::Instance()->CreateGMImage( 359, 246, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 21, 246 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;
	SetRect( &rt, 21, 0, 143, 246 ) ;
	pImageDIB->DrawRect( &rt, 21, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 143, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;
	SetRect( &rt, 21, 0, 61, 246 ) ;
	pImageDIB->DrawRect( &rt, 265, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;

	SetRect( &rt, 143, 0, 197, 246 ) ;
	pImageDIB->DrawRect( &rt, 305, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;

	SetRect( &rt, 197, 0, 233, 18) ;
	pImageDIB->DrawRect( &rt, 1, 227, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	SetRect( &rt, 197, 60, 244, 78 ) ;
	pImageDIB->DrawRect( &rt, 37, 227, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;

	pImageDIB->DrawFrameRect( 2, 36, 355, 2, 0x80000000 ) ;
	pImageDIB->DrawFrameRect( 2, 15, 355, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 2, 35, 355, 1, 0xffa88a60 ) ;

	m_pChatType3Image = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChatType3Image->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChatType3Image ) ;
	GM_DELETE( pImageDIB ) ;

	// 비어 있는 이미지
	CRYLSpriteList::Instance()->CreateGMImage( 359, 246, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	pImageDIB->DrawFrameRect( 2, 38, 355, 187, 0x80000000 ) ;
	m_pChatType3EmptyImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChatType3EmptyImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChatType3EmptyImage ) ;
	GM_DELETE( pImageDIB ) ;
	//====================================================================================================
	// 563 x 246
	CRYLSpriteList::Instance()->CreateGMImage( 563, 246, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 21, 246 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;
	SetRect( &rt, 21, 0, 143, 246 ) ;
	pImageDIB->DrawRect( &rt, 21, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 143, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 265, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 387, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;
	SetRect( &rt, 143, 0, 197, 246 ) ;
	pImageDIB->DrawRect( &rt, 509, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) ;
	SetRect( &rt, 197, 0, 233, 18) ;
	pImageDIB->DrawRect( &rt, 1, 227, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;
	SetRect( &rt, 197, 60, 244, 78 ) ;
	pImageDIB->DrawRect( &rt, 37, 227, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) ;

	pImageDIB->DrawFrameRect( 2, 36, 559, 2, 0x80000000 ) ;
	pImageDIB->DrawFrameRect( 2, 15, 559, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 2, 35, 559, 1, 0xffa88a60 ) ;

	m_pChatType4Image = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChatType4Image->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChatType4Image ) ;
	GM_DELETE( pImageDIB ) ;

	// 비어 있는 이미지
	CRYLSpriteList::Instance()->CreateGMImage( 563, 246, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	pImageDIB->DrawFrameRect( 2, 38, 559, 187, 0x80000000 ) ;
	m_pChatType4EmptyImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pChatType4EmptyImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pChatType4EmptyImage ) ;
	GM_DELETE( pImageDIB ) ;
	//=====================================================================================================

#ifdef _RYL_LANGUAGE
	m_lpTooltip = NULL;
#endif 
	m_bShowTooltip = FALSE;

	g_pChattingDlg = this ;
	s_pInstance    = this ;
	InitCtrl() ;

	SetLeft( nLeft ) ;                                
	SetTop( nTop ) ;

	// DlgType 에 맞게 셋팅
	m_iCurrentSize = nDlgType - 1 ;
	ProcessChatDlgSize() ;

	SetFormType( RYLDLG_FRAME_TYPE_NONE ) ;

	m_dwStartStall = 0 ;
	m_pChattingListBox->SetVisible( TRUE ) ;

	m_dwChatMode = RYLCHATMODE::RYL_CHATMODE_WHOLE ;
	m_dwEditMode = RYLCHATMODE::RYL_CHATMODE_WHOLE ;
}

CRYLChattingDlg::~CRYLChattingDlg() 
{
	FinalCtrl() ;

	deque<Message *>::iterator it;
	for(it = m_deqChatMessage.begin(); it != m_deqChatMessage.end(); it++)
	{
		delete (*it);
	}
	m_deqChatMessage.clear();

	for(it = m_deqStallMessage.begin(); it != m_deqStallMessage.end(); it++)
	{
		delete (*it);
	}
	m_deqStallMessage.clear();
}
void CRYLChattingDlg::namecheckfunction(int check)
{
	/*if(check == 3)//for loop check
	{

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if(!pSelfCreature )
	return;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
	if(!pSelfData )
	return;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	if(!pSelfData)
	return;

	RYLCreature* pSenderCreature = RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID);
	if(!pSenderCreature )
	return;

	CharDataNode* pSendeerData = RYLCharacterDataManager::Instance()->GetCharData( pSenderCreature->GetCharID() );
	if(!pSendeerData )
	return;

	//hz name check:
	//string a=pSelfData->m_strName;
	//string b= pSelfCreature->GetCharacterName();
	//string c=pGame->m_csStatus.m_Info.Name;
	string d=pSenderCreature->GetCharacterName();
	string e=pSendeerData->m_strName;
	//const char* first=&a[0];
	//const char* second=&b[0];
	//const char* third=&c[0];
	const char* four=&d[0];
	const char* five=&e[0];
	//char name[16];
	//char name2[16];
	//char name3[16];
	char name4[16];
	char name5[16];
	//strncpy(name, first, 16);
	//strncpy(name2, second, 16);
	//strncpy(name3, third, 16);
	strncpy(name4, four, 16);
	strncpy(name5, five, 16);
	//CRYLNetworkData::SendFameInfo(name,check);
	//CRYLNetworkData::SendFameInfo(name2,check);
	//CRYLNetworkData::SendFameInfo(name3,check);
	CRYLNetworkData::SendFameInfo(name4,1);
	CRYLNetworkData::SendFameInfo(name5,1);
	}
	else */if(check == 1)
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if(!pSelfCreature )
			return;

		CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
		if(!pSelfData )
			return;

		CRYLGameData* pGame = CRYLGameData::Instance() ;
		if(!pSelfData)
			return;

		RYLCreature* pSenderCreature = RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID);
		if(!pSenderCreature )
			return;

		CharDataNode* pSendeerData = RYLCharacterDataManager::Instance()->GetCharData( pSenderCreature->GetCharID() );
		if(!pSendeerData )
			return;

		//hz name check:
		string a=pSelfData->m_strName;
		string b= pSelfCreature->GetCharacterName();
		string c=pGame->m_csStatus.m_Info.Name;
		string d=pSenderCreature->GetCharacterName();
		string e=pSendeerData->m_strName;
		const char* first=&a[0];
		const char* second=&b[0];
		const char* third=&c[0];
		const char* four=&d[0];
		const char* five=&e[0];
		char name[16];
		char name2[16];
		char name3[16];
		char name4[16];
		char name5[16];
		strncpy(name, first, 16);
		strncpy(name2, second, 16);
		strncpy(name3, third, 16);
		strncpy(name4, four, 16);
		strncpy(name5, five, 16);
		CRYLNetworkData::SendFameInfo(name,check);
		CRYLNetworkData::SendFameInfo(name2,check);
		CRYLNetworkData::SendFameInfo(name3,check);
		CRYLNetworkData::SendFameInfo(name4,check);
		CRYLNetworkData::SendFameInfo(name5,check);
	}
	else if(check == 0)
		CRYLNetworkData::SendFameInfo("admin",check);
//hz name check end
}


VOID	CRYLChattingDlg::InitCtrl() 
{
#ifdef _RYL_LANGUAGE
	// edith 2009.09.30 툴팁추가
	m_lpTooltip = new CGMUITooltipBox ;
	( ( CGMUITooltipBox* )m_lpTooltip )->InitTooltip( "Chatting Info", 0, 0 ) ;

	INT	iTop	= GetTop() ;
	INT iBottom = iTop + 24 ;
	( ( CGMUITooltipBox* )m_lpTooltip )->SetRect( GetLeft(), iTop, GetLeft() + 24, iBottom ) ;
#endif


	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;

	m_pGeneralChatLabel = new CRYLLabel( this ) ;//size of the right top mp hall of the status bar or lvl hz guigui
	m_pGeneralChatLabel->SetLeft( 25 ) ;
	m_pGeneralChatLabel->SetTop( 17 ) ;
	m_pGeneralChatLabel->SetFontColor (0xffffffff);
	m_pGeneralChatLabel->SetAutoSize( TRUE ) ;
	m_pGeneralChatLabel->SetCaption( CRYLStringTable::m_strString[ 2954 ] ) ;
	AttachGMUIControl( m_pGeneralChatLabel ) ;

	m_pShoutChatLabel = new CRYLLabel( this ) ;//size of the hp hall in the status bar(hp/mp) hz guigui
	m_pShoutChatLabel->SetLeft( 85 ) ;
	m_pShoutChatLabel->SetTop( 17 ) ;
	m_pShoutChatLabel->SetFontColor (0xffffffff);
	m_pShoutChatLabel->SetAutoSize( TRUE ) ;
	m_pShoutChatLabel->SetCaption( CRYLStringTable::m_strString[ 2955 ] ) ;
	AttachGMUIControl( m_pShoutChatLabel ) ;

	m_pDealChatLabel = new CRYLLabel( this ) ;
	m_pDealChatLabel->SetLeft( 145 ) ;
	m_pDealChatLabel->SetTop( 17 ) ;
	m_pDealChatLabel->SetFontColor (0xffffffff);
	m_pDealChatLabel->SetAutoSize( TRUE ) ;
	m_pDealChatLabel->SetCaption( CRYLStringTable::m_strString[ 2956 ] ) ;
	AttachGMUIControl( m_pDealChatLabel ) ;

	m_pPartyChatLabel = new CRYLLabel( this ) ;//total size of the two holes+ the space in the middle
	m_pPartyChatLabel->SetLeft( 205 ) ;
	m_pPartyChatLabel->SetTop( 17 ) ;
	m_pPartyChatLabel->SetFontColor (0xffffffff);
	m_pPartyChatLabel->SetAutoSize( TRUE ) ;
	m_pPartyChatLabel->SetCaption( CRYLStringTable::m_strString[ 2957 ] ) ;
	AttachGMUIControl( m_pPartyChatLabel ) ;

	m_pGuildChatLabel = new CRYLLabel( this ) ;//??
	m_pGuildChatLabel->SetLeft( 265 ) ;
	m_pGuildChatLabel->SetTop( 17 ) ;
	m_pGuildChatLabel->SetFontColor (0xffffffff);
	m_pGuildChatLabel->SetAutoSize( TRUE ) ;
	m_pGuildChatLabel->SetCaption( CRYLStringTable::m_strString[ 2958 ] ) ;
	AttachGMUIControl( m_pGuildChatLabel ) ;

#ifdef _RYL_LANGUAGE
	m_pAllChatLabel = new CRYLLabel( this ) ;
	m_pAllChatLabel->SetLeft( 325 ) ;
	m_pAllChatLabel->SetTop( 17 ) ;
	m_pAllChatLabel->SetFontColor (0xffffffff);
	m_pAllChatLabel->SetAutoSize( TRUE ) ;
	m_pAllChatLabel->SetCaption( CRYLStringTable::m_strString[ 2953 ] ) ;
	AttachGMUIControl( m_pAllChatLabel ) ;
#endif 

	// 일반 채팅 
	m_pGeneralChatCheckBox = new CRYLCheckBox( this ) ;
	m_pGeneralChatCheckBox->SetLeft( 5 ) ;
	m_pGeneralChatCheckBox->SetTop( 15 ) ;
	m_pGeneralChatCheckBox->SetInstance( RYLCHATTINGDLG::CHAT_ENABLE_CHECKBOX ) ;
	m_pGeneralChatCheckBox->OnClickEvent = DlgClick ;
	m_pGeneralChatCheckBox->DefaultEvent( m_pGeneralChatCheckBox ) ;
	m_pGeneralChatCheckBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pGeneralChatCheckBox ) ;

	// 외치기 채팅 
	m_pShoutChatCheckBox = new CRYLCheckBox( this ) ;
	m_pShoutChatCheckBox->SetLeft( 65 ) ;
	m_pShoutChatCheckBox->SetTop( 15 ) ;
	m_pShoutChatCheckBox->SetInstance( RYLCHATTINGDLG::SHOUT_ENABLE_CHECKBOX ) ;
	m_pShoutChatCheckBox->OnClickEvent = DlgClick ;
	m_pShoutChatCheckBox->DefaultEvent( m_pShoutChatCheckBox ) ;
	m_pShoutChatCheckBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pShoutChatCheckBox ) ;

	// 거래 채팅 
	m_pDealChatCheckBox = new CRYLCheckBox( this ) ;
	m_pDealChatCheckBox->SetLeft( 125 ) ;
	m_pDealChatCheckBox->SetTop( 15 ) ;
	m_pDealChatCheckBox->SetInstance( RYLCHATTINGDLG::DEAL_ENABLE_CHECKBOX ) ;
	m_pDealChatCheckBox->OnClickEvent = DlgClick ;
	m_pDealChatCheckBox->DefaultEvent( m_pDealChatCheckBox ) ;
	m_pDealChatCheckBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pDealChatCheckBox ) ;

	// 파티 채팅 
	m_pPartyChatCheckBox = new CRYLCheckBox( this ) ;
	m_pPartyChatCheckBox->SetLeft( 185 ) ;
	m_pPartyChatCheckBox->SetTop( 15 ) ;
	m_pPartyChatCheckBox->SetInstance( RYLCHATTINGDLG::PARTY_ENABLE_CHECKBOX ) ;
	m_pPartyChatCheckBox->OnClickEvent = DlgClick ;
	m_pPartyChatCheckBox->DefaultEvent( m_pPartyChatCheckBox ) ;
	m_pPartyChatCheckBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pPartyChatCheckBox ) ;

	// 길드 채팅 
	m_pGuildChatCheckBox = new CRYLCheckBox( this ) ;
	m_pGuildChatCheckBox->SetLeft( 245 ) ;
	m_pGuildChatCheckBox->SetTop( 15 ) ;
	m_pGuildChatCheckBox->SetInstance( RYLCHATTINGDLG::GUILD_ENABLE_CHECKBOX ) ;
	m_pGuildChatCheckBox->OnClickEvent = DlgClick ;
	m_pGuildChatCheckBox->DefaultEvent( m_pGuildChatCheckBox ) ;
	m_pGuildChatCheckBox->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pGuildChatCheckBox ) ;

#ifdef _RYL_LANGUAGE
	// 멀티캐릭터 채팅 
	m_pAllChatCheckBox = new CRYLCheckBox( this ) ;
	m_pAllChatCheckBox->SetLeft( 305 ) ;
	m_pAllChatCheckBox->SetTop( 15 ) ;
	m_pAllChatCheckBox->SetInstance( RYLCHATTINGDLG::ALL_ENABLE_CHECKBOX ) ;
	m_pAllChatCheckBox->OnClickEvent = DlgClick ;
	m_pAllChatCheckBox->DefaultEvent( m_pAllChatCheckBox ) ;

	if(g_ClientMain.m_iMultiSupport == 0)
		m_pAllChatCheckBox->SetChecked( FALSE ) ;
	else
		m_pAllChatCheckBox->SetChecked( TRUE ) ;

	AttachGMUIControl( m_pAllChatCheckBox ) ;
#endif

	// Hide
	m_pHideButton = new CRYLButton( this ) ;
	m_pHideButton->SetLeft( 550 ) ;
	m_pHideButton->SetTop( 3 ) ;
	m_pHideButton->OnClickEvent = DlgClick ;
	m_pHideButton->SetInstance( RYLCHATTINGDLG::HIDE_BUTTON ) ;
	//SetRect( &rt, 202, 0, 213, 9 ) ;//hz guigui
	SetRect( &rt, 5, 347, 16, 356 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	//SetRect( &rt, 202, 18, 213, 27 ) ;
	SetRect( &rt, 5, 267, 16, 276 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	//SetRect( &rt, 202, 9, 213, 18 ) ;
	SetRect( &rt, 5, 257, 16, 266 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	//SetRect( &rt, 202, 9, 213, 18 ) ;
	SetRect( &rt, 5, 257, 16, 266 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pHideButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1868], 1868 ) ;
	AttachGMUIControl( m_pHideButton ) ;

	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	// Size
	m_pSizeButton = new CRYLButton( this ) ;
	m_pSizeButton->SetLeft( 538 ) ;
	m_pSizeButton->SetTop( 3 ) ;
	m_pSizeButton->OnClickEvent = DlgClick ;
	m_pSizeButton->SetInstance( RYLCHATTINGDLG::SIZE_BUTTON ) ;
	SetRect( &rt, 246, 215, 256, 224 ) ;
	m_pSizeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 246, 234, 256, 241 ) ;
	m_pSizeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 246, 224, 256, 234 ) ;
	m_pSizeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 246, 215, 256, 224 ) ;
	m_pSizeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pSizeButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1869], 1869 ) ;
	AttachGMUIControl( m_pSizeButton ) ;

	// Chatting Edit
	m_pChatEdit = new CRYLEdit( this ) ;
	m_pChatEdit->SetLeft( 84 ) ;
	m_pChatEdit->SetTop( 104 ) ;
	m_pChatEdit->SetWidth( 477 ) ;
	m_pChatEdit->SetHeight( 14 ) ;
	// 태국테스트용
	//	m_pChatEdit->SetHeight( 20 ) ;
	m_pChatEdit->SetColor( 0xff000000) ;
	m_pChatEdit->SetFontColor( 0xffffffff ) ;
	m_pChatEdit->SetInstance( RYLCHATTINGDLG::CHAT_EDIT ) ;
	m_pChatEdit->SetMaxLength( 86 ) ;
	m_pChatEdit->OnKeyDown = DlgKeyDown;
	AttachGMUIControl( m_pChatEdit ) ;

	// Chatting ListBox
	m_pChattingListBox = new CRYLListBox( this, TRUE ) ;
	m_pChattingListBox->SetLeft( 2 ) ;
	m_pChattingListBox->SetTop( 38 ) ;
	m_pChattingListBox->SetWidth( 559 ) ;
	m_pChattingListBox->SetHeight( 64 ) ;

	// 태국테스트용
	m_pChattingListBox->SetItemHeight(16);
	//	m_pChattingListBox->SetItemHeight(20);

	m_pChattingListBox->OnClickEvent = DlgClick ;
	m_pChattingListBox->SetInstance( RYLCHATTINGDLG::CHAT_LISTBOX ) ;

	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pChattingListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 186, 240, 200 ) ;
	m_pChattingListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 186, 226, 200 ) ;
	m_pChattingListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pChattingListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 235, 164, 243, 184 ) ;
	m_pChattingListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pChattingListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pChattingListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pChattingListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 198, 200, 212, 214 ) ;
	m_pChattingListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 200, 240, 214 ) ;					  
	m_pChattingListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 200, 226, 214 ) ;					  
	m_pChattingListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 200, 212, 214 ) ;					  
	m_pChattingListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	m_pChattingListBox->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pChattingListBox ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ;
	// 귓속말
	m_pWhisperCheckBox = new CRYLCheckBox( this ) ;
	m_pWhisperCheckBox->SetLeft( 4 ) ;
	m_pWhisperCheckBox->SetTop( 104 ) ;
	m_pWhisperCheckBox->SetInstance( RYLCHATTINGDLG::WHISPER_CHECKBOX ) ;
	m_pWhisperCheckBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 197, 18, 211, 32 ) ;
	m_pWhisperCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 197, 32, 211, 46 ) ;
	m_pWhisperCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 197, 32, 211, 46 ) ;
	m_pWhisperCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 197, 18, 211, 32 ) ;
	m_pWhisperCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pWhisperCheckBox->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1787 ], 1787 ) ;
	AttachGMUIControl( m_pWhisperCheckBox ) ;

	// 외치기
	m_pShoutCheckBox = new CRYLCheckBox( this ) ;
	m_pShoutCheckBox->SetLeft( 20 ) ;
	m_pShoutCheckBox->SetTop( 104 ) ;
	m_pShoutCheckBox->SetInstance( RYLCHATTINGDLG::SHOUT_CHECKBOX ) ;
	m_pShoutCheckBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 211, 18, 225, 32 ) ;
	m_pShoutCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 211, 32, 225, 46 ) ;
	m_pShoutCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 211, 32, 225, 46 ) ;
	m_pShoutCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 211, 18, 225, 32 ) ;
	m_pShoutCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pShoutCheckBox->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1788 ], 1788 ) ;
	AttachGMUIControl( m_pShoutCheckBox ) ;

	// 거래
	m_pTradeCheckBox = new CRYLCheckBox( this ) ;
	m_pTradeCheckBox->SetLeft( 36 ) ;
	m_pTradeCheckBox->SetTop( 104 ) ;
	m_pTradeCheckBox->SetInstance( RYLCHATTINGDLG::TRADE_CHECKBOX ) ;
	m_pTradeCheckBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 239, 18, 253, 32 ) ;
	m_pTradeCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 239, 32, 253, 46 ) ;
	m_pTradeCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 239, 32, 253, 46 ) ;
	m_pTradeCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 239, 18, 253, 32 ) ;
	m_pTradeCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pTradeCheckBox->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 2969 ], 0) ;
	AttachGMUIControl( m_pTradeCheckBox ) ;

	// 파티
	m_pPartyCheckBox = new CRYLCheckBox( this ) ;
	m_pPartyCheckBox->SetLeft( 52 ) ;
	m_pPartyCheckBox->SetTop( 104 ) ;
	m_pPartyCheckBox->SetInstance( RYLCHATTINGDLG::PARTY_CHECKBOX ) ;
	m_pPartyCheckBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 197, 78, 211, 92 ) ;
	m_pPartyCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 197, 92, 211, 106 ) ;
	m_pPartyCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 197, 92, 211, 106 ) ;
	m_pPartyCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 197, 78, 211, 92 ) ;
	m_pPartyCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pPartyCheckBox->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 2970 ], 0 ) ;
	AttachGMUIControl( m_pPartyCheckBox ) ;

	// 길드
	m_pGuildCheckBox = new CRYLCheckBox( this ) ;
	m_pGuildCheckBox->SetLeft( 68 ) ;
	m_pGuildCheckBox->SetTop( 104 ) ;
	m_pGuildCheckBox->SetInstance( RYLCHATTINGDLG::GUILD_CHECKBOX ) ;
	m_pGuildCheckBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 225, 18, 239, 32 ) ;
	m_pGuildCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 225, 32, 239, 46 ) ;
	m_pGuildCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 225, 32, 239, 46 ) ;
	m_pGuildCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 225, 18, 239, 32 ) ;
	m_pGuildCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pGuildCheckBox->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 2971 ], 0 ) ;
	AttachGMUIControl( m_pGuildCheckBox ) ;

	SetFocusChattingEdit();
}

VOID	CRYLChattingDlg::FinalCtrl() 
{
#ifdef _RYL_LANGUAGE
	GM_DELETE(m_lpTooltip);
#endif 

	GM_DELETE( m_pHideButton ) ;				// HIde
	GM_DELETE( m_pSizeButton ) ;				// Size
	GM_DELETE( m_pChatEdit ) ;					// Chatting Edit
	GM_DELETE( m_pChattingListBox ) ;
	GM_DELETE( m_pWhisperCheckBox ) ;
	GM_DELETE( m_pShoutCheckBox ) ;				
	GM_DELETE( m_pTradeCheckBox ) ;
	GM_DELETE( m_pPartyCheckBox ) ;
	GM_DELETE( m_pGuildCheckBox ) ;	

	GM_DELETE( m_pGeneralChatCheckBox ) ;	// General Chatting CheckBox
	GM_DELETE( m_pShoutChatCheckBox ) ;		// Shout Chatting CheckBox
	GM_DELETE( m_pDealChatCheckBox ) ;		// Deal Chatting CheckBox
	GM_DELETE( m_pPartyChatCheckBox ) ;		// Party Chatting CheckBox
	GM_DELETE( m_pGuildChatCheckBox ) ;		// Guild Chatting CheckBox

	GM_DELETE( m_pGeneralChatLabel ) ;
	GM_DELETE( m_pShoutChatLabel ) ;
	GM_DELETE( m_pDealChatLabel ) ;	
	GM_DELETE( m_pPartyChatLabel ) ;
	GM_DELETE( m_pGuildChatLabel ) ;

#ifdef _RYL_LANGUAGE
	GM_DELETE( m_pAllChatCheckBox ) ;		// AllChat Chatting CheckBox
	GM_DELETE( m_pAllChatLabel ) ;	
#endif
}

VOID	CRYLChattingDlg::Show( CGMUIObject* Sender ) 
{
	ShowButton() ;
}

VOID	CRYLChattingDlg::Hide( CGMUIObject* Sender ) 
{
	HideButton() ;
}

VOID	CRYLChattingDlg::ShowButton() 
{
	m_pHideButton->SetVisible( TRUE ) ;						
	m_pSizeButton->SetVisible( TRUE ) ;			
	m_pChatEdit->SetVisible( TRUE ) ;			
	m_pWhisperCheckBox->SetVisible( TRUE ) ;
	m_pShoutCheckBox->SetVisible( TRUE ) ;	
	m_pTradeCheckBox->SetVisible( TRUE ) ;	
	m_pPartyCheckBox->SetVisible( TRUE ) ;
	m_pGuildCheckBox->SetVisible( TRUE ) ;
	m_pChattingListBox->SetVisible( TRUE ) ;

	switch ( m_iCurrentSize )
	{
	case 0 : AttachWindowTexture( ( CGMTexture*)m_pChatType1Image ) ; break ;
	case 1 : AttachWindowTexture( ( CGMTexture*)m_pChatType2Image ) ; break ;
	case 2 : AttachWindowTexture( ( CGMTexture*)m_pChatType3Image ) ; break ;
	case 3 : AttachWindowTexture( ( CGMTexture*)m_pChatType4Image ) ; break ;
	}

	m_bHideEnable = TRUE ;
}

VOID	CRYLChattingDlg::HideButton() 
{
	CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( FALSE ) ;
	m_pHideButton->SetVisible( FALSE ) ;						
	m_pSizeButton->SetVisible( FALSE ) ;			
	m_pChatEdit->SetVisible( FALSE ) ;			
	m_pWhisperCheckBox->SetVisible( FALSE ) ;
	m_pShoutCheckBox->SetVisible( FALSE ) ;
	m_pTradeCheckBox->SetVisible( FALSE ) ;	
	m_pPartyCheckBox->SetVisible( FALSE ) ;
	m_pGuildCheckBox->SetVisible( FALSE ) ;
	//m_pChattingListBox->SetVisible( FALSE ) ;

	switch ( m_iCurrentSize )
	{
	case 0 : AttachWindowTexture( ( CGMTexture*)m_pChatType1EmptyImage ) ; break ;
	case 1 : AttachWindowTexture( ( CGMTexture*)m_pChatType2EmptyImage ) ; break ;
	case 2 : AttachWindowTexture( ( CGMTexture*)m_pChatType3EmptyImage ) ; break ;
	case 3 : AttachWindowTexture( ( CGMTexture*)m_pChatType4EmptyImage ) ; break ;
	}

	m_bHideEnable = FALSE ;
}

VOID	CRYLChattingDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;

#ifdef _RYL_LANGUAGE
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	if(m_bShowTooltip && m_lpTooltip)
	{
		m_lpTooltip->Render(lpD3DDevice);
	}
#endif
}

HRESULT		CRYLChattingDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLChattingDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{

	return S_OK ;
}

HRESULT		CRYLChattingDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if ( m_bHideEnable )
	{
		if (g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
		{
			bEdge = TRUE;

			if (g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
				g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
				g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
			{
				bClick = TRUE;
			}
		}
	}

	if (!m_pChatEdit->GetFocusState() )
	{
		if ( m_bShout )
			m_pChatEdit->SetText( "/c " ) ;
		if ( m_bTrade )
			m_pChatEdit->SetText( "/e " ) ;
		if ( m_bParty )
			m_pChatEdit->SetText( "/p " ) ;
		if ( m_bGuild )
			m_pChatEdit->SetText( "/g " ) ;

		if ( CRYLCommunityData::Instance()->m_bInstanceWhisper )
		{
			if (!m_pWhisperCheckBox->GetChecked()) // && m_pShoutCheckBox->GetChecked() )	
			{
				CRYLCommunityData::Instance()->m_bInstanceWhisper = FALSE ;
			}
			else
			{
				m_pChatEdit->SetText( "/s " ) ;
			}
		}
	}

	// Page Up / Down
	if ( g_DeviceInput.KeyHold(VK_PRIOR) )
	{
		int  iIndex, iSize = 4 ;
		if ( m_iCurrentSize == 2 || m_iCurrentSize == 3 ) 
			iSize = 13;

		if ( m_pChattingListBox->GetViewIndex() > 0 )
		{
			iIndex = m_pChattingListBox->GetViewIndex() - iSize ;
			if ( iIndex < 0 ) iIndex = 0 ;

			m_pChattingListBox->SetViewIndex( iIndex ) ;
		}
	}

	if ( g_DeviceInput.KeyHold(VK_NEXT) )
	{
		int iIndex, iSize = 4;
		if ( m_iCurrentSize == 2 || m_iCurrentSize == 3 ) 
			iSize = 13;

		if ( m_pChattingListBox->GetViewIndex() < m_pChattingListBox->Count() - iSize )
		{
			iIndex = m_pChattingListBox->GetViewIndex() + iSize ;

			if ( iIndex > m_pChattingListBox->Count() - iSize ) 
				iIndex = m_pChattingListBox->Count() - iSize ;

			m_pChattingListBox->SetViewIndex( iIndex ) ;
		}
	}

	m_bShowTooltip = FALSE;
#ifdef _RYL_LANGUAGE
	// 마우스가 특정영역안에 있을때

	// m_pGeneralChatCheckBox
	INT		iLeft = GetLeft()+m_pGeneralChatCheckBox->GetLeft() ;
	INT		iTop  = GetTop()+m_pGeneralChatCheckBox->GetTop() ;
	INT		iWidth = 50;//m_pGeneralChatCheckBox->GetWidth();
	INT		iHeight = m_pGeneralChatCheckBox->GetHeight();

	if( g_DeviceInput.InRect( iLeft, iTop, iLeft + iWidth, iTop + iHeight ) )
	{
		char strTooltop[512];
		strcpy(strTooltop, CRYLStringTable::m_strString[ 4751 ]);

		( ( CGMUITooltipBox* )m_lpTooltip )->InitTooltip( strTooltop, 0, 0 ) ;
		m_bShowTooltip = TRUE;
	}

	// m_pShoutChatCheckBox
	iLeft = GetLeft()+m_pShoutChatCheckBox->GetLeft() ;
	iTop  = GetTop()+m_pShoutChatCheckBox->GetTop() ;
	iWidth = 50;//m_pShoutChatCheckBox->GetWidth();
	iHeight = m_pShoutChatCheckBox->GetHeight();

	if( g_DeviceInput.InRect( iLeft, iTop, iLeft + iWidth, iTop + iHeight ) )
	{
		char strTooltop[512];
		strcpy(strTooltop, CRYLStringTable::m_strString[ 4752 ]);

		( ( CGMUITooltipBox* )m_lpTooltip )->InitTooltip( strTooltop, 0, 0 ) ;
		m_bShowTooltip = TRUE;
	}

	// m_pDealChatCheckBox
	iLeft = GetLeft()+m_pDealChatCheckBox->GetLeft() ;
	iTop  = GetTop()+m_pDealChatCheckBox->GetTop() ;
	iWidth = 50;//m_pDealChatCheckBox->GetWidth();
	iHeight = m_pDealChatCheckBox->GetHeight();

	if( g_DeviceInput.InRect( iLeft, iTop, iLeft + iWidth, iTop + iHeight ) )
	{
		char strTooltop[512];
		strcpy(strTooltop, CRYLStringTable::m_strString[ 4753 ]);

		( ( CGMUITooltipBox* )m_lpTooltip )->InitTooltip( strTooltop, 0, 0 ) ;
		m_bShowTooltip = TRUE;
	}

	// m_pPartyChatCheckBox
	iLeft = GetLeft()+m_pPartyChatCheckBox->GetLeft() ;
	iTop  = GetTop()+m_pPartyChatCheckBox->GetTop() ;
	iWidth = 50;//m_pPartyChatCheckBox->GetWidth();
	iHeight = m_pPartyChatCheckBox->GetHeight();

	if( g_DeviceInput.InRect( iLeft, iTop, iLeft + iWidth, iTop + iHeight ) )
	{
		char strTooltop[512];
		strcpy(strTooltop, CRYLStringTable::m_strString[ 4754 ]);

		( ( CGMUITooltipBox* )m_lpTooltip )->InitTooltip( strTooltop, 0, 0 ) ;
		m_bShowTooltip = TRUE;
	}

	// m_pGuildChatCheckBox
	iLeft = GetLeft()+m_pGuildChatCheckBox->GetLeft() ;
	iTop  = GetTop()+m_pGuildChatCheckBox->GetTop() ;
	iWidth = 50;//m_pGuildChatCheckBox->GetWidth();
	iHeight = m_pGuildChatCheckBox->GetHeight();

	if( g_DeviceInput.InRect( iLeft, iTop, iLeft + iWidth, iTop + iHeight ) )
	{
		char strTooltop[512];
		strcpy(strTooltop, CRYLStringTable::m_strString[ 4755 ]);

		( ( CGMUITooltipBox* )m_lpTooltip )->InitTooltip( strTooltop, 0, 0 ) ;
		m_bShowTooltip = TRUE;
	}

	// m_pAllChatCheckBox
	iLeft = GetLeft()+m_pAllChatCheckBox->GetLeft() ;
	iTop  = GetTop()+m_pAllChatCheckBox->GetTop() ;
	iWidth = 50;//m_pAllChatCheckBox->GetWidth();
	iHeight = m_pAllChatCheckBox->GetHeight();

	if( g_DeviceInput.InRect( iLeft, iTop, iLeft + iWidth, iTop + iHeight ) )
	{
		char strTooltop[512];
		strcpy(strTooltop, CRYLStringTable::m_strString[ 4756 ]);

		( ( CGMUITooltipBox* )m_lpTooltip )->InitTooltip( strTooltop, 0, 0 ) ;
		m_bShowTooltip = TRUE;
	}
#endif

	return S_OK ;
}

VOID	CRYLChattingDlg::AddChatText( LPSTR pText, DWORD dwColor, DWORD dwChatKind )
{
	INT iSize  = 4 ;
	if ( m_iCurrentSize == 2 || m_iCurrentSize == 3 ) 
		iSize = 13 ;

	if ( m_pChattingListBox->Count () > 100 )
		m_pChattingListBox->DeleteItem( 0 ) ;

	m_pChattingListBox->AddItem( pText, dwColor, 0xFFFFFFFF, 0x00FFFFFF ) ;

	//			마지막 페이지가 아닌 상태에서는 ViewIndex 를 갱신하지 않기 처리지만, 문제가 있어서 주석처리 해둠
	if ( m_pChattingListBox->Count() >= iSize )
	{
		m_pChattingListBox->SetItemIndex( m_pChattingListBox->Count() ) ;
	}
}

VOID	CRYLChattingDlg::ProcessChatDlgSize() 
{
	++ m_iCurrentSize ;
	if ( m_iCurrentSize == 4 )
	{
		m_iCurrentSize = 0 ;
	}

	m_pChatEdit->SetText( "" ) ;
	m_pChatEdit->SetFocus() ;

	switch ( m_iCurrentSize )
	{
	case 0 :
		{
			AttachWindowTexture( ( CGMTexture*)m_pChatType1Image ) ;
			SetWidth( 359 ) ;
			SetHeight( 121 ) ;

			if ( GetLeft() + GetWidth() > g_ClientMain.m_iScreenWidth ) SetLeft( GetLeft() + GetWidth() - g_ClientMain.m_iScreenWidth ) ;
			if ( GetTop() + GetHeight() > g_ClientMain.m_iScreenHeight ) SetTop( GetTop() + GetHeight() - g_ClientMain.m_iScreenHeight ) ;

			m_pSizeButton->SetLeft( 334 ) ;
			m_pSizeButton->SetTop( 3 ) ;
			m_pHideButton->SetLeft( 346 ) ;
			m_pHideButton->SetTop( 3 ) ;

			m_pChatEdit->SetTop( 104 ) ;
			m_pChatEdit->SetWidth( 273 ) ;
			m_pChatEdit->SetMaxLength( 46 ) ;

			m_pChattingListBox->SetWidth( 355 ) ;
			m_pChattingListBox->SetHeight( 64 ) ;

			m_pWhisperCheckBox->SetTop( 104 ) ;
			m_pShoutCheckBox->SetTop( 104 ) ;
			m_pTradeCheckBox->SetTop( 104 ) ;
			m_pPartyCheckBox->SetTop( 104 ) ;
			m_pGuildCheckBox->SetTop( 104 ) ;
		}
		break ;
	case 1 :
		{
			AttachWindowTexture( ( CGMTexture*)m_pChatType2Image ) ;
			SetWidth( 563 ) ;
			SetHeight( 121 ) ;

			if ( GetLeft() + GetWidth() > g_ClientMain.m_iScreenWidth ) SetLeft( GetLeft() + GetWidth() - g_ClientMain.m_iScreenWidth ) ;
			if ( GetTop() + GetHeight() > g_ClientMain.m_iScreenHeight ) SetTop( GetTop() + GetHeight() - g_ClientMain.m_iScreenHeight ) ;

			m_pSizeButton->SetLeft( 538 ) ;
			m_pSizeButton->SetTop( 3 ) ;
			m_pHideButton->SetLeft( 550 ) ;
			m_pHideButton->SetTop( 3 ) ;

			m_pChatEdit->SetTop( 104 ) ;
			m_pChatEdit->SetWidth( 477 ) ;
			m_pChatEdit->SetMaxLength( 86 ) ;

			m_pChattingListBox->SetWidth( 559 ) ;
			m_pChattingListBox->SetHeight( 64 ) ;

			m_pWhisperCheckBox->SetTop( 104 ) ;
			m_pShoutCheckBox->SetTop( 104 ) ;
			m_pTradeCheckBox->SetTop( 104 ) ;
			m_pPartyCheckBox->SetTop( 104 ) ;
			m_pGuildCheckBox->SetTop( 104 ) ;
		}
		break ;
	case 2 :
		{
			AttachWindowTexture( ( CGMTexture*)m_pChatType3Image ) ;
			SetWidth( 359 ) ;
			SetHeight( 245 ) ;

			if ( GetLeft() + GetWidth() > g_ClientMain.m_iScreenWidth ) SetLeft( GetLeft() + GetWidth() - g_ClientMain.m_iScreenWidth ) ;
			if ( GetTop() + GetHeight() > g_ClientMain.m_iScreenHeight ) SetTop( GetTop() + GetHeight() - g_ClientMain.m_iScreenHeight ) ;

			m_pSizeButton->SetLeft( 334 ) ;
			m_pSizeButton->SetTop( 3 ) ;
			m_pHideButton->SetLeft( 346 ) ;
			m_pHideButton->SetTop( 3 ) ;

			m_pChatEdit->SetTop( 229 ) ;
			m_pChatEdit->SetWidth( 273 ) ;
			m_pChatEdit->SetMaxLength( 46 ) ;

			m_pChattingListBox->SetWidth( 355 ) ;
			m_pChattingListBox->SetHeight( 187 ) ;

			m_pWhisperCheckBox->SetTop( 229 ) ;
			m_pShoutCheckBox->SetTop( 229 ) ;
			m_pTradeCheckBox->SetTop( 229 ) ;
			m_pPartyCheckBox->SetTop( 229 ) ;
			m_pGuildCheckBox->SetTop( 229 ) ;
		}
		break ;
	case 3 : 
		{
			AttachWindowTexture( ( CGMTexture*)m_pChatType4Image ) ;
			SetWidth( 563 ) ;
			SetHeight( 245 ) ;

			if ( GetLeft() + GetWidth() > g_ClientMain.m_iScreenWidth ) SetLeft( GetLeft() + GetWidth() - g_ClientMain.m_iScreenWidth ) ;
			if ( GetTop() + GetHeight() > g_ClientMain.m_iScreenHeight ) SetTop( GetTop() + GetHeight() - g_ClientMain.m_iScreenHeight ) ;

			m_pSizeButton->SetLeft( 538 ) ;
			m_pSizeButton->SetTop( 3 ) ;
			m_pHideButton->SetLeft( 550 ) ;
			m_pHideButton->SetTop( 3 ) ;

			m_pChatEdit->SetTop( 229 ) ;
			m_pChatEdit->SetWidth( 477 ) ;
			m_pChatEdit->SetMaxLength( 86 ) ;

			m_pChattingListBox->SetWidth( 559 ) ;
			m_pChattingListBox->SetHeight( 187 ) ;

			m_pWhisperCheckBox->SetTop( 229 ) ;
			m_pShoutCheckBox->SetTop( 229 ) ;
			m_pTradeCheckBox->SetTop( 229 ) ;
			m_pPartyCheckBox->SetTop( 229 ) ;
			m_pGuildCheckBox->SetTop( 229 ) ;
		}
		break ;
	}

	INT iSize  = 4 ;
	if ( m_iCurrentSize == 2 || m_iCurrentSize == 3 ) 
		iSize = 13 ;

	m_pChattingListBox->ClearItem() ;

	deque<Message *>::iterator it;
	DWORD dwID = CRYLNetworkData::Instance()->m_dwMyChrID ;

	for(it = m_deqChatMessage.begin(); it != m_deqChatMessage.end(); it++)
		AddChatMessage( ( *it )->strMessage, dwID, ( *it )->dwCommand ) ;

	if ( m_pChattingListBox->Count() >= iSize )
		m_pChattingListBox->SetItemIndex( m_pChattingListBox->Count() ) ;
}

VOID	CRYLChattingDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if (!g_pChattingDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLCHATTINGDLG::HIDE_BUTTON :
		g_pChattingDlg->HideButton() ;
		break ;
	case RYLCHATTINGDLG::SIZE_BUTTON :
		g_pChattingDlg->ProcessChatDlgSize() ;
		break ;

	case RYLCHATTINGDLG::WHISPER_CHECKBOX :
		g_pChattingDlg->ChatWhisper() ;
		break ;
	case RYLCHATTINGDLG::SHOUT_CHECKBOX	:
		g_pChattingDlg->ChatShout() ;
		break ;

	case RYLCHATTINGDLG::TRADE_CHECKBOX :
		g_pChattingDlg->ChatTrade() ;
		break ;

	case RYLCHATTINGDLG::PARTY_CHECKBOX	:
		g_pChattingDlg->ChatParty() ;
		break ;

	case RYLCHATTINGDLG::GUILD_CHECKBOX	:
		g_pChattingDlg->ChatGuild() ;
		break ;

	case RYLCHATTINGDLG::CHAT_LISTBOX :
		g_pChattingDlg->SetWhisperCharacter( Sender->GetInstance() ) ;
		break ;
	}
}

VOID	CRYLChattingDlg::SetWhisperCharacter( int instance ) 
{
	if ( m_pChattingListBox->GetItemIndex() == -1 )
		return ;

	LPSTR lpText = m_pChattingListBox->GetItem( m_pChattingListBox->GetItemIndex() ) ;
	char szBuff[ 64 ] ;
	char szTmpName[ 64 ] ;
	memset( szBuff, 0, 64 ) ;
	bool	bWhisper = true ;

	for ( size_t i = 0 ; i < strlen( lpText ) ; i ++ )
	{
		if ( lpText[ i     ] == ' ' &&
			lpText[ i + 1 ] == ':' &&
			lpText[ i + 2 ] == ' ' )
		{
			memset( szTmpName, 0, 64 ) ;
			strncpy( szTmpName, lpText, i ) ;
			szTmpName[ i + 1 ] = '\0' ;
			sprintf( szBuff, "/s %s ", szTmpName ) ;
			bWhisper = false ;
			break ;
		}		
	}

	if ( bWhisper )
	{
		for (size_t i = 0 ; i < strlen( lpText ) ; i ++ )
		{
			if ( lpText[ i ] == ')' )
			{
				memset( szTmpName, 0, 64 ) ;
				if ( lpText[ 1 ] == 't' )
				{
					strncpy( szTmpName, &lpText[5], i - 5 ) ;
				}
				else if ( lpText[ 1 ] == 'f' )
				{
					strncpy( szTmpName, &lpText[7], i - 7 ) ;
				}
				szTmpName[ i + 1 ] = '\0' ;
				sprintf( szBuff, "/s %s ", szTmpName ) ;
				break ;
			}		
		}
	}

	strcpy( CRYLCommunityData::Instance()->m_strRecentChatChrID, szTmpName ) ;
	strcpy( CRYLCommunityData::Instance()->m_strWhisperChrID, CRYLCommunityData::Instance()->m_strRecentChatChrID ) ;

	CRYLCommunityData::Instance()->m_bInstanceWhisper = TRUE ;
	m_pChatEdit->SetText( szBuff ) ;

	m_pChatEdit->SetFocus() ;
	m_pChatEdit->SetFocusState( TRUE ) ;

	HIMC hImc;
	hImc = ImmGetContext(g_ClientMain.m_hWnd);
	ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
	ImmReleaseContext(g_ClientMain.m_hWnd, hImc);
}

VOID	CRYLChattingDlg::ChatWhisper()
{
	m_bShout = FALSE ;
	m_bTrade = FALSE ;
	m_bParty = FALSE ;
	m_bGuild = FALSE ;
	m_pShoutCheckBox->SetChecked( FALSE ) ;			// 외치기
	m_pTradeCheckBox->SetChecked( FALSE ) ;			// 거래
	m_pPartyCheckBox->SetChecked( FALSE ) ;			// 파티
	m_pGuildCheckBox->SetChecked( FALSE ) ;			// 길드

	if ( CRYLCommunityData::Instance()->m_bInstanceWhisper )
	{
		CRYLCommunityData::Instance()->m_bInstanceWhisper = FALSE ;
		m_pWhisperCheckBox->SetChecked( FALSE ) ;

		m_pChatEdit->SetText( "" );
	}
	else
	{
		CRYLCommunityData::Instance()->m_bInstanceWhisper = TRUE ;
		m_pWhisperCheckBox->SetChecked( TRUE ) ;
	}
}

VOID	CRYLChattingDlg::ChatShout()
{
	CRYLCommunityData::Instance()->m_bInstanceWhisper = FALSE ;
	m_bTrade = FALSE ;
	m_bParty = FALSE ;
	m_bGuild = FALSE ;
	m_pWhisperCheckBox->SetChecked( FALSE ) ;		// 귓속말
	m_pTradeCheckBox->SetChecked( FALSE ) ;			// 거래
	m_pPartyCheckBox->SetChecked( FALSE ) ;			// 파티
	m_pGuildCheckBox->SetChecked( FALSE ) ;			// 길드

	if ( m_bShout )
	{
		m_bShout = FALSE ;
		m_pShoutCheckBox->SetChecked( FALSE ) ;

		m_pChatEdit->SetText( "" );
	}
	else
	{
		m_bShout = TRUE ;
		m_pShoutCheckBox->SetChecked( TRUE ) ;
	}
}

VOID	CRYLChattingDlg::ChatTrade()
{
	CRYLCommunityData::Instance()->m_bInstanceWhisper = FALSE ;
	m_bShout = FALSE ;
	m_bParty = FALSE ;
	m_bGuild = FALSE ;
	m_pWhisperCheckBox->SetChecked( FALSE ) ;		// 귓속말
	m_pShoutCheckBox->SetChecked( FALSE ) ;			// 외치기
	m_pPartyCheckBox->SetChecked( FALSE ) ;			// 파티
	m_pGuildCheckBox->SetChecked( FALSE ) ;			// 길드

	if ( m_bTrade )
	{
		m_bTrade = FALSE ;
		m_pTradeCheckBox->SetChecked( FALSE ) ;

		m_pChatEdit->SetText( "" );
	}
	else
	{
		m_bTrade = TRUE ;
		m_pTradeCheckBox->SetChecked( TRUE ) ;
	}
}

VOID	CRYLChattingDlg::ChatParty()
{
	CRYLCommunityData::Instance()->m_bInstanceWhisper = FALSE ;
	m_bShout = FALSE ;
	m_bTrade = FALSE ;
	m_bGuild = FALSE ;
	m_pWhisperCheckBox->SetChecked( FALSE ) ;		// 귓속말
	m_pShoutCheckBox->SetChecked( FALSE ) ;			// 외치기
	m_pTradeCheckBox->SetChecked( FALSE ) ;			// 거래
	m_pGuildCheckBox->SetChecked( FALSE ) ;			// 길드


	if ( m_bParty )
	{
		m_bParty = FALSE ;
		m_pPartyCheckBox->SetChecked( FALSE ) ;

		m_pChatEdit->SetText( "" );
	}
	else
	{
		m_bParty = TRUE ;
		m_pPartyCheckBox->SetChecked( TRUE ) ;
	}
}

VOID	CRYLChattingDlg::ChatGuild()
{
	CRYLCommunityData::Instance()->m_bInstanceWhisper = FALSE ;
	m_bShout = FALSE ;
	m_bTrade = FALSE ;
	m_bParty = FALSE ;
	m_pWhisperCheckBox->SetChecked( FALSE ) ;		// 귓속말
	m_pShoutCheckBox->SetChecked( FALSE ) ;			// 외치기
	m_pTradeCheckBox->SetChecked( FALSE ) ;			// 거래
	m_pPartyCheckBox->SetChecked( FALSE ) ;			// 파티

	if ( m_bGuild )
	{
		m_bGuild = FALSE ;
		m_pGuildCheckBox->SetChecked( FALSE ) ;

		m_pChatEdit->SetText( "" );
	}
	else
	{
		m_bGuild = TRUE ;
		m_pGuildCheckBox->SetChecked( TRUE ) ;
	}
}

VOID	CRYLChattingDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) 
{
	if (!g_pChattingDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLCHATTINGDLG::CHAT_EDIT :
		if ( key == 13 )
		{
			/*
			if ( g_pChattingDlg->m_dwChatMode == RYLCHATMODE::RYL_CHATMODE_SHOUT )
			{
			g_pChattingDlg->AttachShoutCmd() ;
			}
			*/
			g_pChattingDlg->AdminCommand() ;
		}
		break ;
	}
}

VOID	CRYLChattingDlg::AttachShoutCmd() 
{
	CHAR	strChat[ MAX_PATH ] ;

	if ( strcmp( m_pChatEdit->GetText(), "") )
	{
		strChat[ 0 ] = '/' ;
		strChat[ 1 ] = 'c' ;
		strChat[ 2 ] = ' ' ;

		strcpy( &strChat[ 3 ], m_pChatEdit->GetText() ) ;
		m_pChatEdit->SetText( strChat ) ;
	}
}

const unsigned short ALLOW_LETTER_NUM = 33;
const char AllowLetters[ALLOW_LETTER_NUM] = {
	' ', '`',  '~', '!',  '@', '#',  '$', '%', '^', '&', 
	'*', '(',  ')', '-',  '=', '_',  '+', '{', '}', '|',
	'[', ']', '\\', ';', '\'', ':', '\"', '<', '>', '?', 
	',', '.', '/'
};

BOOL	CRYLChattingDlg::CheckCharSet(CHAR* str, int Len)
{
	int ACount = 0;

	// 영어만 입력가능
	for(unsigned short LCount = 0; LCount < Len; LCount++)
	{
		// 2바이트 검출
		if((str[LCount] & 0x80))
			return TRUE;

		// 영문자 허용	
		if((str[LCount] >= 'A' && str[LCount] <= 'Z') ||
			(str[LCount] >= 'a' && str[LCount] <= 'z'))
		{
			continue;
		}

		// 숫자 허용
		if(str[LCount] >= '0' && str[LCount] <= '9')
		{
			continue;
		}

		// 특수 기호 부분 허용
		for(ACount = 0; ACount < ALLOW_LETTER_NUM; ACount++)
		{
			if(str[LCount] == AllowLetters[ACount])
			{
				break;
			}
		}

		if(ACount == ALLOW_LETTER_NUM)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL	CRYLChattingDlg::CheckAisaLang(unsigned short usLang)
{
	switch(usLang)
	{
	case LANG_JAPANESE:	// 일본어
	case LANG_KOREAN:	// 한국어
	case LANG_CHINESE:	// 중국어
	case LANG_THAI:		// 태국어
		return TRUE;
	}

	return FALSE;
}


VOID	CRYLChattingDlg::AdminCommand() 
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if(!pSelfCreature )
		return;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
	if(!pSelfData )
		return;
	bool bResult = true;
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	{
		if ( strcmp( m_pChatEdit->GetText(), "") )
		{
			namecheckfunction(1);
			CHAR		strChat[ MAX_PATH ] ;
			DWORD		dwTotalLength ;
			strcpy( strChat, m_pChatEdit->GetText() ) ;
			dwTotalLength = strlen( strChat ) ;

			if ( strChat[ 0 ] == '/' )
			{
				CHAR		seps[] = " ,\t\n" ;
				CHAR		*token, strCommand[ MAX_PATH ] ;
				bResult = false;
				token = strtok( strChat, seps ) ;
				strcpy( strCommand, token ) ;
				token = strlwr( strCommand ) ;

				// 소설모션 체크면..
				if(pGame->m_lpInterface->CheckSocialAction(token))
				{
					m_pChatEdit->SetText( "" ) ;
					return;
				}

				// 자살
				if (!strcmp( token,"/kill" ) ) 
				{
					namecheckfunction(1);
					if( 0 != pSelfCreature->GetRiderID(0) )
					{
						AddMessage( CRYLStringTable::m_strString[ 4119 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					}
					else if( !pSelfCreature->GetIsDead() )
					{
						CRYLGameData::SetKillCounter( 15000 ) ;
					}
					bResult = true;
				} 
				// 파티 채팅
				else if (!strcmp( token, "/p" ) || !strcmp( token, "/ㅔ" ) || !strcmp( token, "/파티" ) )
				{
					namecheckfunction(1);
					if ( pGame->m_csStatus.m_PartyInfo.m_dwPartyID )
					{
						ULONG len = strlen( token ) + 1 ;

						if ( dwTotalLength >= len && strlen(&strChat[ len ]))
						{
							if(1)//g_StringFilter.ConvertFilter(&strChat[ len ]))
							{
								CRYLNetworkData::SendChatMessage( NULL, &strChat[ len ], PktChat::PARTY) ;
							} 
							else
							{
								CRYLNetworkData::SendClientLog( 1, &strChat[ len ], g_StringFilter.m_strSearched ) ;
								AddMessage( CRYLStringTable::m_strString[ 570 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
							}
						}
					}
					bResult = true;
				} 
				// 길드 채팅
				else if (!strcmp( token, "/g" ) || !strcmp( token, "/ㅎ" ) || !strcmp( token, "/길드" ) )
				{
					namecheckfunction(1);
					if ( pGame->m_csStatus.m_Info.GID )
					{
						ULONG len = strlen( token ) + 1 ;

						if ( dwTotalLength >= len && strlen(&strChat[ len ]) )
						{
							if(1)//g_StringFilter.ConvertFilter(&strChat[ len ]))
							{
								CRYLNetworkData::SendChatMessage( NULL, &strChat[ len ], PktChat::GUILD) ;
							} 
							else
							{
								CRYLNetworkData::SendClientLog( 1, &strChat[ len ], g_StringFilter.m_strSearched ) ;
								AddMessage( CRYLStringTable::m_strString[ 570 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
							}
						}
					}
					bResult = true;
				} 
				// 귓속말
				else if (!strcmp( token, "/s" ) || !strcmp( token, "/ㄴ" ) )
				{
					namecheckfunction(1);
					INT		count = 0 ;
					count += strlen( token ) + 1 ;

					ULONG dwLen = strlen( &strChat[ count ] ) ;
					token = strtok( NULL, seps ) ;

					if ( token )
					{
						ULONG dwCount = strlen( token ) ;

						if ( dwLen > dwCount )
						{
							count += dwCount + 1 ;

							if ( token && ( strlen( token ) < 16 ) )
							{
								if(1)//g_StringFilter.ConvertFilter(&strChat[ count ]))
								{
									CRYLNetworkData::SendChatMessage( token, &strChat[ count ], PktChat::WHISPER) ;
								} 
								else
								{
									CRYLNetworkData::SendClientLog( 1, &strChat[ count ], g_StringFilter.m_strSearched ) ;
									AddMessage( CRYLStringTable::m_strString[ 570 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
								}
							}
						}
					}
					else
					{
						if (CRYLCommunityData::Instance()->m_bInstanceWhisper) 
						{
							return;	
						}
					}
					bResult = true;
				} 
				// 외치기
				else if (!strcmp( token, "/c" ) || !strcmp( token, "/ㅊ" ) || !strcmp( token, "/외침" ) )		
				{
					namecheckfunction(1);
					ULONG len = strlen( token ) + 1  ;

					if ( dwTotalLength >= len && strlen(&strChat[ len ]) )
					{ 
						if(1)//g_StringFilter.ConvertFilter(&strChat[ len ]))
						{
							if (strChat[ len ] == '\0')	
							{
								if (m_bShout) 
								{
									return;
								}
							}

							// edith 2009.06.13 저렙쟁에서 전챗을 하면 [L] 고렙쟁에서 전챗을 하면 [H]가 채팅 메시지 앞에 붙음
							CHAR	strMessage[ MAX_PATH ] ;
							sprintf(strMessage, "%s", &strChat[ len ]);

							if(CRYLNetworkData::Instance()->m_dwPresentZone == /*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL)
								sprintf(strMessage, " %s", &strChat[ len ]);//was with [AG] before %S
							else if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STONE_WAR2 ||
								CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STONE_WAR3 )
								sprintf(strMessage, " %s", &strChat[ len ]);//was with [HG] before %S

							if (!CRYLNetworkData::SendChatMessage( NULL, strMessage, PktChat::SHOUT, m_usLocalLang ) )
							{
								sprintf( strMessage, CRYLStringTable::m_strString[572], 
									static_cast<int>(CRYLNetworkData::m_lShoutCounter / 1000.0f + 0.5f) ) ;
								AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							}
						} 
						else
						{
							CRYLNetworkData::SendClientLog( 1, &strChat[len], g_StringFilter.m_strSearched ) ;
							AddMessage( CRYLStringTable::m_strString[570], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
						}
					}
					bResult = true;
				} 
				// 외치기(거래)
				else if (!strcmp( token, "/e" ) || !strcmp( token, "/ㄷ" ) || !strcmp( token, "/거래" ) )		
				{
					//hz start name check:
					namecheckfunction(1);
					//hz finish name check
					ULONG len = strlen( token ) + 1  ;

					if ( dwTotalLength >= len && strlen(&strChat[ len ]) )
					{ 
						if(1)//g_StringFilter.ConvertFilter(&strChat[ len ]))
						{
							if (strChat[ len ] == '\0')	
							{
								if (m_bShout) 
								{
									return;
								}
							}

							// edith 2009.06.13 저렙쟁에서 전챗을 하면 [L] 고렙쟁에서 전챗을 하면 [H]가 채팅 메시지 앞에 붙음
							CHAR	strMessage[ MAX_PATH ] ;
							sprintf(strMessage, "%s", &strChat[ len ]);

							if(CRYLNetworkData::Instance()->m_dwPresentZone == /*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL)
								sprintf(strMessage, " %s", &strChat[ len ]);//was with [AG] before %s
							else if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STONE_WAR2 ||
								CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STONE_WAR3 )
								sprintf(strMessage, " %s", &strChat[ len ]);//was with [HG] before %s

							if (!CRYLNetworkData::SendChatMessage( NULL, strMessage, PktChat::TRADE, m_usLocalLang ) )
							{
								sprintf( strMessage, CRYLStringTable::m_strString[572], 
									static_cast<int>(CRYLNetworkData::m_lShoutCounter / 1000.0f + 0.5f) ) ;
								AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							}
						} 
						else
						{
							CRYLNetworkData::SendClientLog( 1, &strChat[len], g_StringFilter.m_strSearched ) ;
							AddMessage( CRYLStringTable::m_strString[570], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
						}
					}
					bResult = true;
				}
				// 지엠 신고
				else if (!strcmp( token, "/gm" ) || !strcmp( token, "/흐" ) || !strcmp( token, "/지엠" ) )		
				{
					namecheckfunction(1);
					ULONG len = strlen( token ) + 1  ;

					if ( dwTotalLength >= len && strlen(&strChat[ len ]) )
					{ 
						if(1)//g_StringFilter.ConvertFilter(&strChat[ len ]))
						{
							CHAR	strMessage[ MAX_PATH ] ;
							sprintf(strMessage, "%s", &strChat[ len ]);

							if (!CRYLNetworkData::SendChatMessage( NULL, strMessage, PktChat::GMREPORT, m_usLocalLang ) )
							{
								sprintf( strMessage, CRYLStringTable::m_strString[572], 
									static_cast<int>(CRYLNetworkData::m_lGMReportCounter / 1000.0f + 0.5f) ) ;
								AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							}
							else
								AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						} 
						else
						{
							CRYLNetworkData::SendClientLog( 1, &strChat[len], g_StringFilter.m_strSearched ) ;
							AddMessage( CRYLStringTable::m_strString[570], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
						}
					}
					bResult = true;
				} 
				// 조이스틱 감도 설정 (2006-05-16 by hackermz)
				else if (!strcmp( token, "/sensitive" ) || !strcmp( token, "/ㄴ두냐샾ㄷ" ) || !strcmp( token, "/감도" ))
				{
					namecheckfunction(1);
					CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
					CHAR	strMessage[ MAX_PATH ] ;

					token = strtok(NULL, seps);
					if (NULL != token)
					{
						short sSensivity = static_cast<short>(atoi(token));

						if (10 <= sSensivity && 250 >= sSensivity)
						{
							CInputJoystick::GetInstance()->SetSensitivity(sSensivity);

							sprintf(strMessage, CRYLStringTable::m_strString[ 4225 ], sSensivity);
							pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
						}
						else
						{
							sprintf(strMessage, CRYLStringTable::m_strString[ 4226 ]);
							pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
						}
					}
					else
					{
						sprintf(strMessage, CRYLStringTable::m_strString[ 4224 ],
							CInputJoystick::GetInstance()->GetSensitivity());
						pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					}
					bResult = true;
				}
				// 커서 
				else if (!strcmp(token, "/cursor" ) )
				{
					namecheckfunction(1);
					pGame->m_bHardwareCursor = !pGame->m_bHardwareCursor;
					if (pGame->m_bHardwareCursor)
					{
						g_DeviceInput.ShowCursor(TRUE);
					} 
					else
					{
						g_DeviceInput.ShowCursor(FALSE);
					}
					bResult = true;
				} 
				// 명성&공헌메달 보기
				else if (!strcmp(token, "/fame") || !strcmp(token, "/mileage") || !strcmp(token, "/medal"))
				{
					namecheckfunction(1);
					namecheckfunction(0);
					bResult = true;
				} 
				// 노점상 열기
				else if (!strcmp(token, "/shop"))
				{
					namecheckfunction(1);
					if (!pGame->m_lpInterface->GetBusy())
					{
						pGame->m_lpInterface->InitStreetStall(CRYLNetworkData::Instance()->m_dwMyChrID, NULL);
					}
					bResult = true;
				} 
				// 스피드 알기
				else if (!strcmp(token, "/speed"))
				{
					namecheckfunction(1);
					CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
					char	strMessage[256];
					sprintf(strMessage, CRYLStringTable::m_strString[ 2356 ], pGame->m_csStatus.GetSpeed());
					pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					//pGame->m_csStatus.GetSpeedRate();
					bResult = true;
				}
				// 유저 무시
				else if (!strcmp(token, "/ignore"))
				{
					namecheckfunction(1);
					INT		count = 0 ;
					count += strlen( token ) + 1 ;

					ULONG dwLen = strlen( &strChat[ count ] ) ;
					token = strtok( NULL, seps ) ;	// ID

					if ( token && ( strlen( token ) < PktFriendAddReq::MAX_NAME ) )
					{
						// 유저 무시
						SendPacket::CharFriendAddRequest( g_GameSession, token, (PktFriendAddReq::CMD)PktFriendAddReq::BAN_FRIEND_REQ ) ;
					}
					bResult = true;
				}
				// edith 2009.08.28 NoFriend 추가
				// 상대 친구리스트에서 내리스트 빼기
				else if (!strcmp(token, "/nofriend"))
				{
					namecheckfunction(1);
					INT		count = 0 ;
					count += strlen( token ) + 1 ;

					ULONG dwLen = strlen( &strChat[ count ] ) ;
					token = strtok( NULL, seps ) ;	// ID

					if ( token && ( strlen( token ) < PktFriendRemoveReq::MAX_NAME ) )
					{
						// 유저 무시
						SendPacket::CharFriendRemoveRequest( g_GameSession, 0, (PktFriendRemoveReq::CMD)PktFriendRemoveReq::NOFRIEND_REQ, token) ;
					}
					bResult = true;
				}
				// 럭키찬스 알기
				else if (!strcmp(token, "/luck"))
				{
					namecheckfunction(1);
					CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
					char	strMessage[256];
					sprintf(strMessage, CRYLStringTable::m_strString[ 2357 ], pGame->m_csStatus.GetLuck());
					pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					//pGame->m_csStatus.GetSpeedRate();
					bResult = true;
				}
				// 주사위
				else if (!strcmp(token, "/dice") || !strcmp( token, "/주사위" ) )
				{
					namecheckfunction(1);
					ULONG len = strlen( token ) + 1 ;

					if ( dwTotalLength >= len && strlen(&strChat[ len ]) )
					{
						CRYLNetworkData::SendChatMessage( token, &strChat[ len ], PktChat::DICE ) ;
					}
					bResult = true;
				} 
				// 횃불 켜기/끄기
				else if (!strcmp(token, "/t") || !strcmp(token, "/torch") || !strcmp(token, "/불"))
				{
					namecheckfunction(1);
					CSceneManager::m_bCharacterLightOp = !CSceneManager::m_bCharacterLightOp;
					bResult = true;
				} 
				// Rodin : 임시 봉인 (친구/거부 리스트 추가 커맨드)
				/*
				else if (!strcmp(token, "/fr"))
				{
				token = strtok(NULL, seps);

				if(token)
				{
				char strName[16];
				char strMyname[16];

				strcpy( strMyname , pGame->m_csStatus.GetName());

				if(strlen(token) < 16)
				{
				strcpy(strName, token);
				if( strcmp ( strName, strMyname) == 0 )  // 자기 자신이면 리턴 ( 텍스트 추가 가능 )
				{
				m_pChatEdit->SetText( "" ) ;
				return;
				}

				// 친구 추가 패킷 
				SendPacket::CharFriendAddRequest(g_GameSession, strName, PktFriendAddReq::ADD_FRIEND_REQ);									
				}
				}
				}
				else if (!strcmp(token, "/ig"))
				{
				token = strtok(NULL, seps);

				if(token)
				{
				char strName[16];
				char strMyname[16];

				strcpy( strMyname , pGame->m_csStatus.GetName());

				if(strlen(token) < 16)
				{
				strcpy(strName, token);
				if( strcmp ( strName, strMyname) == 0 )  // 자기 자신이면 리턴 ( 텍스트 추가 가능 )
				{
				m_pChatEdit->SetText( "" ) ;
				return;
				}

				// 거부 추가 패킷
				SendPacket::CharFriendAddRequest(g_GameSession, strName, PktFriendAddReq::BAN_FRIEND_REQ);									
				}
				}
				}
				*/				
				// UDP 정보 켜기/끄기
				else if (!strcmp(token, "/udpinfo"))
				{
					namecheckfunction(1);
					g_ClientMain.m_bUDPInfo = !g_ClientMain.m_bUDPInfo;
					bResult = true;
				}
				// 에러 스트림(네트워크 로그) 플러쉬 (added by sparrowhawk)
				else if (!strcmp(token, "/flush"))
				{
					namecheckfunction(1);
					fflush(stderr);
					bResult = true;
				}
				// 채팅서버로 인벤정보 보내기 (added by sparrowhawk)
				else if (!strcmp(token, "/sendinven123"))
				{
					namecheckfunction(1);
					GameProcess::LogInvenData(0LL);
					bResult = true;
				}
				//else
				//{
				//

				//	switch (g_ClientMain.m_wAdminMode)
				//	{
				// 개발자
				//case ADMIN_DEV:		
				else if (!strcmp(token, "/polymorph"))
				{
					token = strtok(NULL, seps);
					POS psPos = { 0.0f, 0.0f, 0.0f };

					// GM 모델로 변경.

					const vector3	vPos	= *(pSelfCreature->GetPosition());
					const float		fDir	= pSelfCreature->GetDirection();

					pSelfCreature->SetModelFileName(token);

					CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(pSelfCreature, strcat(token,".GCMDS"));

					if(pCharModel)
					{
						pCharModel->SetPosition(vPos);
						pCharModel->SetDirection(fDir);

						const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];										
						std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

						pSelfCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true );

						bResult = true;
					}
					/*if (!strcmp(token, "pc"))
					{
					POS psPos = { 0.0f, 0.0f, 0.0f };

					RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;

					if(pSelfCreature)
					{									
					const vector3	vPos	= *(pSelfCreature->GetPosition());
					const float		fDir	= pSelfCreature->GetDirection();										

					char strSex[MAX_PATH];
					const char* strShape[ Item::EquipmentPos::MAX_EQUPMENT_POS ];

					for (int j = 0; j < Item::EquipmentPos::MAX_EQUPMENT_POS; j++)
					{
					CItemInstance* lpEquip = pGame->m_csStatus.m_lstEquipment[j];
					strShape[j] = (NULL == lpEquip) ? NULL : g_ItemMgr.GetEquipName(lpEquip->GetProtoTypeID()); 
					}

					switch (pGame->m_csStatus.m_Info.Sex)
					{
					case 1 :
					strcpy( strSex, "PC_MAN.GCMDS" ) ; 
					break;
					case 2:
					strcpy( strSex, "PC_WOMAN.GCMDS" ) ;
					break;
					case 3 :
					strcpy( strSex, "PC_Akhan_A.GCMDS" ) ; 
					break;
					case 4:
					strcpy( strSex, "PC_Akhan_B.GCMDS" ) ;
					break;
					}

					pSelfCreature->SetModelFileName(strSex);

					CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(pSelfCreature, strSex);

					if(pCharModel)
					{
					pCharModel->SetPosition(vPos);
					pCharModel->SetDirection(fDir);
					pSelfCreature->InitWeapon(0);

					pSelfCreature->SetShape(strShape, pGame->GetNetworkEquipGrade(), true);

					bResult = true;
					}
					}

					}
					else if (!strcmp(token, "NPC"))
					{

					POS psPos = { 0.0f, 0.0f, 0.0f };

					// GM 모델로 변경.

					const vector3	vPos	= *(pSelfCreature->GetPosition());
					const float		fDir	= pSelfCreature->GetDirection();

					pSelfCreature->SetModelFileName("NPC_NEW_6.GCMDS");

					CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(pSelfCreature, "NPC_NEW_6.GCMDS");

					if(pCharModel)
					{
					pCharModel->SetPosition(vPos);
					pCharModel->SetDirection(fDir);

					const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];										
					std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

					pSelfCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true );

					bResult = true;
					}
					}
					else if (!strcmp(token, "seaman"))
					{
					POS psPos = { 0.0f, 0.0f, 0.0f };

					// GM 모델로 변경.

					const vector3	vPos	= *(pSelfCreature->GetPosition());
					const float		fDir	= pSelfCreature->GetDirection();

					pSelfCreature->SetModelFileName("mon_Seaman.GCMDS");

					CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(pSelfCreature, "mon_Seaman.GCMDS");

					if(pCharModel)
					{
					pCharModel->SetPosition(vPos);
					pCharModel->SetDirection(fDir);

					const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];										
					std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

					pSelfCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true );

					bResult = true;
					}
					}
					else if (!strcmp(token, "rider2_m"))
					{
					POS psPos = { 0.0f, 0.0f, 0.0f };

					const vector3	vPos	= *(pSelfCreature->GetPosition());
					const float		fDir	= pSelfCreature->GetDirection();

					pSelfCreature->SetModelFileName("Rider2_M.GCMDS");

					CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(pSelfCreature, "Rider2_M.GCMDS");

					if(pCharModel)
					{
					pCharModel->SetPosition(vPos);
					pCharModel->SetDirection(fDir);

					const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];										
					std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

					pSelfCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true );

					bResult = true;
					}
					}
					else if (!strcmp(token, "rider_w"))
					{
					POS psPos = { 0.0f, 0.0f, 0.0f };

					const vector3	vPos	= *(pSelfCreature->GetPosition());
					const float		fDir	= pSelfCreature->GetDirection();

					pSelfCreature->SetModelFileName("Rider_W.GCMDS");

					CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(pSelfCreature, "Rider_W.GCMDS");

					if(pCharModel)
					{
					pCharModel->SetPosition(vPos);
					pCharModel->SetDirection(fDir);

					const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];										
					std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

					pSelfCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true );

					bResult = true;
					}
					}
					else if (!strcmp(token, "rider_m"))
					{
					POS psPos = { 0.0f, 0.0f, 0.0f };

					const vector3	vPos	= *(pSelfCreature->GetPosition());
					const float		fDir	= pSelfCreature->GetDirection();

					pSelfCreature->SetModelFileName("Rider_M.GCMDS");

					CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(pSelfCreature, "Rider_M.GCMDS");

					if(pCharModel)
					{
					pCharModel->SetPosition(vPos);
					pCharModel->SetDirection(fDir);

					const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];										
					std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

					pSelfCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true );

					bResult = true;
					}
					}*/
				}
				// 보이는 캐릭터 개체수 조정
				/*else if (!strcmp(token, "/#setsize!"))
				{
				token = strtok(NULL, seps);
				if (NULL != token)
				{
				MAXCHRLOAD = atoi(token);

				bResult = true;
				}
				} 

				// 콘솔창 켜기/끄기
				else if (!strcmp(token, "/#systeminfo!"))
				{
				pGame->m_bSystemConsoleShow = !pGame->m_bSystemConsoleShow;
				if (pGame->m_bSystemConsoleShow == TRUE)
				{
				AllocConsole();
				SetConsoleTitle("ROW 시스템 콘솔");
				} 
				else
				{
				FreeConsole();
				}

				bResult = true;
				} 
				// 팁 빈도 변경
				else if (!strcmp(token, "/changetipcounter"))
				{
				token = strtok(NULL, seps);
				if (NULL != token)
				{
				pGame->m_lTipBroadCounterSeed = atoi(token);
				bResult = true;
				}
				} 
				// 오류 확인
				else if (!strcmp( token, "/dump" ) )
				{
				int nZero = 0;
				int nError = 10 / nZero;
				*reinterpret_cast<int*>(nError) = 0xFFFFFFFF;
				}
				// 덤프 레벨
				else if (!strcmp( token, "/dumplevel" ) )
				{
				token = strtok( NULL, seps ) ;
				if ( token )
				{
				if (!strcmp( token, "normal" ) )
				{
				Exception::SetDumpType(Exception::NormalDump);
				}
				else if (!strcmp( token, "dataseg" ) )
				{
				Exception::SetDumpType(Exception::DataSegs);
				}
				else if (!strcmp( token, "full" ) )
				{
				Exception::SetDumpType(Exception::HeapDump);
				}

				bResult = true;
				}

				}
				// 공성 병기로 전환
				else if (!strcmp( token, "/cm" ) )
				{
				token = strtok( NULL, seps ) ;
				if ( token )
				{
				bResult = true;

				RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
				if(!pSelfCreature )
				return;

				CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
				if(!pSelfData )
				return;

				if (!strcmp( token, CRYLStringTable::m_strString[ 2962 ] ) )
				{
				g_CharacterData.ChangeSiegeModel( pSelfCreature->GetCharID(),
				pSelfData->m_strName.c_str(), *( pSelfCreature->GetPosition() ), 0.0f, "Character" ) ;
				} 
				else if (!strcmp( token, CRYLStringTable::m_strString[ 2963 ] ) )
				{
				g_CharacterData.ChangeSiegeModel( pSelfCreature->GetCharID(),
				pSelfData->m_strName.c_str(), *( pSelfCreature->GetPosition() ), 0.0f, "LongSeigeWeapon" ) ;

				}
				else if (!strcmp( token, CRYLStringTable::m_strString[ 2964 ] ) )
				{
				g_CharacterData.ChangeSiegeModel( pSelfCreature->GetCharID(), 
				pSelfData->m_strName.c_str(), *( pSelfCreature->GetPosition() ), 0.0f, "ShortSiegeWeapon" ) ;
				}
				else if (!strcmp( token, CRYLStringTable::m_strString[ 2965 ] ) )
				{
				g_CharacterData.SetWeaponControlKind( LONG_DEFENCEWEAPON_CONTROL ) ;
				}
				else if (!strcmp( token, CRYLStringTable::m_strString[ 2966 ] ) )
				{
				g_CharacterData.SetWeaponControlKind( SHORT_DEFENCEWEAPON_CONTROL ) ;
				}
				else if (!strcmp( token, CRYLStringTable::m_strString[ 2967 ] ) )
				{
				g_CharacterData.ChangeSiegeModel( pSelfCreature->GetCharID(),
				pSelfData->m_strName.c_str(), *( pSelfCreature->GetPosition() ), 0.0f, "DropShip" ) ;
				}									
				}
				}						
				// 클라이언트 비정상 종료
				//						else if (!strcmp(token, "/clientkill"))
				//						{
				//							int nZero = 0;
				//							int nError = 10 / nZero;
				//							*reinterpret_cast<int*>(nError) = 0xFFFFFFFF;
				//						}
				// 패킷 퍼포먼스 로그 출력
				else if (!strcmp(token, "/printpacketlog"))
				{
				CPacketProfileMgr* lpPacketProfileMgr = CRYLNetworkData::Instance()->m_lpPacketProfileMgr;
				if (NULL != lpPacketProfileMgr)
				{
				char strLogFilePath[MAX_PATH] ;
				sprintf(strLogFilePath, "%s\\PacketProfileLog.txt", g_ClientMain.m_strClientPath);
				lpPacketProfileMgr->Print(strLogFilePath);
				}
				bResult = true;
				}
				// 프로파일러 로그 작성
				else if (!strcmp(token, "/writeprof"))
				{
				CSimpleProfile::GetInstance().WriteProfile();
				AddMessage("Write Profile", CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				bResult = true;
				}
				// 프로파일러 정보 제거
				else if (!strcmp(token, "/clearprof"))
				{
				CSimpleProfile::GetInstance().ClearProfile();
				AddMessage("Clear Profile", CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				bResult = true;

				}
				// 프로파일러 로그 작성 & 정보 제거
				else if (!strcmp(token, "/writeclearprof"))
				{
				CSimpleProfile::GetInstance().WriteProfile();
				CSimpleProfile::GetInstance().ClearProfile();

				AddMessage("Write and Clear Profile", CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				bResult = true;
				}
				// 중계서버 비정상 종료
				else if (!strcmp(token, "/dbagentkill"))
				{
				POS psPos = { 0.0f, 0.0f, 0.0f };
				SendPacket::CharAdmin(g_GameSession, PktAdmin::DBAGENT_KILL, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);
				bResult = true;
				}
				// 운영자 레벨 3
				// 돈 주기
				*/
				else if (!strcmp(token, "/givegold"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nAmount = atoi(token);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::GetGold, strName, 0, 0, 0, psPos, (unsigned long)nAmount);
								bResult = true;
							} 
						} 
					} 
				} 
				// 클라이언트 정보 보기
				else if (!strcmp(token, "/showinfo"))
				{
					pGame->m_bShowInfo = !pGame->m_bShowInfo;

					bResult = true;
				} 
				// FPS on/off
				else if (!strcmp(token, "/fps"))
				{
					CRYLGameData* pGame = CRYLGameData::Instance();
					pGame->m_bFPS = !(pGame->m_bFPS);
					bResult = true;
				}
				// 레벨업 (원하는 레벨까지)
				else if (!strcmp(token, "/levelup"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nAmount = atoi(token);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::LevelUp, strName, 0, 0, 0, psPos, (unsigned long)nAmount);
								bResult = true;
							} 
						} 
					} 
				} 
				// 레벨 다운
				else if (!strcmp(token, "/leveldown"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nAmount = atoi(token);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::LevelDown, strName, 0, 0, 0, psPos, (unsigned long)nAmount);
								bResult = true;
							}
						}
					}
				} 
				// 경험치 주기
				else if (!strcmp(token, "/giveexp"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nAmount = atoi(token);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::GetExp, strName, 0, 0, 0, psPos, (unsigned long)nAmount);
								bResult = true;
							} 
						} 
					}
				} 
				// 킥 (해당 캐릭터를 죽인다.)
				else if (!strcmp(token, "/kick"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);

							POS psPos = { 0.0f, 0.0f, 0.0f };
							SendPacket::CharAdmin(g_GameSession, ClientConstants::KillChar, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						}
					} 
				} 
				// 몬스터 생성
				else if (!strcmp(token, "/createmob"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int nMob = atoi(token);
						token = strtok(NULL, seps);
						if (token)
						{
							double x = atof(token);
							token = strtok(NULL, seps);
							if (token)
							{
								double y = atof(token);
								token = strtok(NULL, seps);
								if (token)
								{
									double z = atof(token);

									POS psPos = { x, y, z };
									SendPacket::CharAdmin(g_GameSession, ClientConstants::CreateMon,
										const_cast<char*>( pSelfData->m_strName.c_str() ),
										0, 0, nMob, psPos, 0);

									bResult = true;

									// edith
									// 생성한 몹을 file에 남긴다. (파일명은 MobLog.txt 이다.)
									char strBuff[256];
									GetCurrentDirectory(256, strBuff);
									strcat(strBuff, "\\MobLog.txt");

									FILE* fp = fopen(strBuff, "a+t");
									fprintf(fp, "%d,%f,%f\n", nMob, psPos.fPointX, psPos.fPointZ);
									fclose(fp);
								} 
							} 
						} 
					}
				} 
				// 아이템 드랍
				else if (!strcmp(token, /*"/dropitem"*/"/drop"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nItemID = atoi(token);
								token = strtok(NULL, seps);
								if (token)
								{
									int nAmount = atoi(token);
									POS psPos = { 0.0f, 0.0f, 0.0f };

									SendPacket::CharAdmin(g_GameSession, ClientConstants::CreateItem, strName, 0, 0, (unsigned short)nItemID, psPos, nAmount);
									bResult = true;
								} 
							} 
						} 
					} 
				} 
				// 아이템(장비) 생성
				else if (!strcmp(token, /*"/createitem"*/"/create"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nMonsterLevel = atoi(token);
								token = strtok(NULL, seps);
								if (token)
								{
									int nGrade = atoi(token);
									int nBaseNum = 0;

									token = strtok(NULL, seps);
									if (token)
									{
										nBaseNum = atoi(token);
									}

									POS psPos = { 0.0f, 0.0f, 0.0f };

									SendPacket::CharAdmin(g_GameSession, ClientConstants::CreateEquip, strName, 
										0, 0, nGrade, psPos, nMonsterLevel, nBaseNum);

									bResult = true;
								}
							} 
						} 
					} 
				} 
				// 명성치 조정
				else if (!strcmp(token, /*"/setfame"*/"/famous"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nAmount = atoi(token);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::GetFame, strName, 0, 0, 0, psPos, (unsigned long)nAmount);
								bResult = true;
							} 
						} 
					} 
				} 
				// 공헌메달 조정
				else if (!strcmp(token, /*"/setmileage"*/"/medaliot"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nAmount = atoi(token);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::GetMileage, strName, 0, 0, 0, psPos, (unsigned long)nAmount);
								bResult = true;
							} 
						} 
					} 
				} 
				// 명예의 석상 초기화
				else if (!strcmp(token, "/resstatue") || !strcmp(token, "/statueinit"))
				{
					POS psPos = { 0.0f, 0.0f, 0.0f };

					SendPacket::CharAdmin(g_GameSession, PktAdmin::STATUE_INIT, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);
					bResult = true;
				}
				// 캐릭터 리스폰 (전체)
				else if (!strcmp(token, "/resmen") || !strcmp(token, "/respawnall"))
				{
					POS psPos = { 0.0f, 0.0f, 0.0f };

					SendPacket::CharAdmin(g_GameSession, PktAdmin::RESPAWN_ALL, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);
					bResult = true;
				}
				// 엘리트 보너스 조정 (인원에 의한 부분을 무시)
				else if (!strcmp(token, "/elitebonus"))
				{
					BOOL bExcept = FALSE;

					token = strtok(NULL, seps);
					if (token)
					{
						int nNation =  atoi(token);
						token = strtok(NULL, seps);
						if (token)
						{
							int nLevel = atoi(token);
							POS psPos = { 0.0f, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::EliteBonus, pGame->m_csStatus.m_Info.Name, (char)nNation, (char)nLevel, 0, psPos, 0);
							bResult = true;
						} 
					} 
				} 
				// 임시 인벤토리에 아이템 넣기
				else if (!strcmp(token, "/addtempinven"))
				{
					BOOL bExcept = FALSE;

					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nItemID = atoi(token);
								token = strtok(NULL, seps);
								if (token)
								{
									int nAmount = atoi(token);
									POS psPos = { 0.0f, 0.0f, 0.0f };

									SendPacket::CharAdmin(g_GameSession, PktAdmin::SET_ITEM_TEMP_INVEN, 
										strName, 0, 0, (unsigned short)nItemID, psPos, nAmount);
									bResult = true;
								} 
							} 
						} 
					} 
				} 
				else if (!strcmp(token, "/relogin"))
				{
					CRYLGameData::Instance()->ClearGameDlgData();					

					g_pClientSocket->Disconnect(ClientSocket::GameEventHandler);
					g_ClientMain.Update();	

					CRYLLoginScene* pScene = static_cast<CRYLLoginScene*>(CRYLSceneManager::Instance()->FindScene(LOGIN_SCENE));
					pScene->SetAuthSuccess(FALSE);

					CRYLSceneManager::Instance()->SetScene(INTRO_SCENE);
					CRYLGameData::Instance()->m_dwClientMode	= CLIENT_LOGO;
					CRYLGameData::Instance()->m_cReLogin		= CRYLGameData::LOGIN;

					// 리소스 초기화
					RYLCreatureManager::Instance()->ClearCreatureManager();
					pGame->m_lpSceneManager->BaseGraphicsDataUnload();					
					pGame->m_lpSceneManager->DeleteAllData();
					GM_DELETE( pGame->m_lpInterface );

					CRYLNetworkData::Instance()->m_dwMyChrID = 0L;
					pGame->m_lpPickItem = NULL;

					// 배경 사운드 정지
					CBgmManager::_GetInstance()->AllStop(); 

					BaseGraphicsLayer::m_ClearColor = color(0,0,0);

					// 인증 서버에 접속
					ClientNet::CClientEventHandler* lpAuthHandler = new CAuthEventHandler;
					g_pClientSocket->Open(CRYLNetworkData::m_strIP, ClientSocket::DEFAULT_AUTH_TCP_PORT, lpAuthHandler);

					// 인트로 화면 Fade In
					CRYLIntroScene* pIntroScene = static_cast<CRYLIntroScene*>(CRYLSceneManager::Instance()->FindScene(INTRO_SCENE));
					pIntroScene->InitResourceObject();												
				}
				/*else if (!strcmp(token, "/realmpoint"))
				{
				token = strtok(NULL, seps);

				if (token)
				{
				char strName[16];

				if (strlen(token) < 16)
				{
				strcpy(strName, token);

				token = strtok(NULL, seps);

				if (token)
				{
				int nLevel = atoi(token);

				if(nLevel>=0 && nLevel<=5)
				{
				POS psPos = { 0.0f, 0.0f, 0.0f };

				SendPacket::CharAdmin(g_GameSession, PktAdmin::REALM_POINT, strName, 0, 0, 0, psPos, nLevel);								

				bResult = true;
				}																																
				} 
				} 
				} 
				}*/
				// 주위 몬스터 몰살
				else if (!strcmp(token, "/killall"))
				{
					POS psPos = { 0.0f, 0.0f, 0.0f };
					SendPacket::CharAdmin(g_GameSession, ClientConstants::Apocalypse, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);

					bResult = true;
				} 
				// 전직
				else if (!strcmp(token, "/class"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nClassID = atoi(token);

								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::ChangeClass, strName, 0, 0, (unsigned short)nClassID, psPos, 0);
								bResult = true;
							}
						}
					}
				} 
				// 스킬 배우기
				else if (!strcmp(token, "/learnskill"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char szID[32] = "0x0000";
						strcat(szID, (token + 2));
						int nSkillID = Math::Convert::Atoi(szID);
						token = strtok(NULL, seps);
						if (token)
						{
							int nStep = atoi(token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nLevel = atoi(token);
								POS psPos = { (float)nStep, (float)nLevel, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::LearnSkill, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nSkillID, psPos, 0);
								bResult = true;
							}
						}
					}
				} 
				// 스킬 지우기
				else if (!strcmp(token, "/eraseskill"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char szID[32] = "0x0000";
						strcat(szID, (token + 2));
						int nSkillID = Math::Convert::Atoi(szID);
						token = strtok(NULL, seps);
						if (token)
						{
							int nStep = atoi(token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nLevel = atoi(token);
								POS psPos = { (float)nStep, (float)nLevel, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::EraseSkill, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nSkillID, psPos, 0);
								bResult = true;
							}
						}
					}
				} 
				// 어빌리티 배우기
				else if (!strcmp(token, "/learnability"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char szID[32] = "0x0000";
						strcat(szID, (token + 2));
						int nSkillID = Math::Convert::Atoi(szID);
						token = strtok(NULL, seps);
						if (token)
						{
							int nStep = atoi(token);
							POS psPos = { (float)nStep, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::LearnAbility, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nSkillID, psPos, 0);
							bResult = true;
						}
					}
				} 
				// 어빌리티 지우기
				else if (!strcmp(token, "/eraseability"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char szID[32] = "0x0000";
						strcat(szID, (token + 2));
						int nSkillID = Math::Convert::Atoi(szID);
						token = strtok(NULL, seps);
						if (token)
						{
							int nStep = atoi(token);
							POS psPos = { (float)nStep, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::EraseAbility, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nSkillID, psPos, 0);
							bResult = true;
						}
					}
				} 
				// 퀘스트 얻기
				else if (!strcmp(token, "/getquest"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								char szID[32] = "0x0000";
								strcat(szID, (token + 2));
								int nQuestID = Math::Convert::Atoi(szID);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::GetQuest, strName, 0, 0, (unsigned short)nQuestID, psPos, 0);
								bResult = true;
							}
						}
					}
				} 
				// 완료 퀘스트에 추가
				else if (!strcmp(token, "/addquest"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								char szID[32] = "0x0000";
								strcat(szID, (token + 2));
								int nQuestID = Math::Convert::Atoi(szID);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::AddQuest, strName, 0, 0, (unsigned short)nQuestID, psPos, 0);
								bResult = true;
							}
						}
					}
				} 
				// 퀘스트 삭제
				else if (!strcmp(token, "/deletequest"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								char szID[32] = "0x0000";
								strcat(szID, (token + 2));
								int nQuestID = Math::Convert::Atoi(szID);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::DeleteQuest, strName, 0, 0, (unsigned short)nQuestID, psPos, 0);
								bResult = true;
							}
						}
					}
				} 
				// 스펠관련 명령
				else if (!strcmp(token, "/addspell"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								unsigned long dwSpellID = static_cast<unsigned long>(atoi(token));
								token = strtok(NULL, seps);
								if (token)
								{
									unsigned long dwEnchantLevel = static_cast<unsigned long>(atoi(token));
									token = strtok(NULL, seps);
									if (token)
									{
										unsigned long dwDurationSec = static_cast<unsigned long>(_atoi64(token));
										POS psPos = { 0.0f, 0.0f, 0.0f };

										SendPacket::CharAdmin(g_GameSession, ClientConstants::AddSpell, strName, 0, 0, 0, psPos, dwSpellID, dwEnchantLevel, dwDurationSec);
										bResult = true;
									}
								}
							}
						}
					}
				}
				// 리스폰 (해당 캐릭터)
				else if (!strcmp(token, "/respawn") || !strcmp(token, "/리스폰"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);

							POS psPos = { 0.0f, 0.0f, 0.0f };
							SendPacket::CharAdmin(g_GameSession, ClientConstants::RespawnChar, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						}
					} 
				} 
				// 듀얼 해제
				else if (!strcmp(token, "/duelinit") || !strcmp(token, "/듀얼해제"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);

							POS psPos = { 0.0f, 0.0f, 0.0f };
							SendPacket::CharAdmin(g_GameSession, ClientConstants::DuelInit, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						}
					} 
				} 
				// BGServer : 해당 방의 경기 종료 (승패 결정)
				else if (!strcmp(token, "/finish"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int nChannelNum = atoi(token);
						POS psPos = { 0.0f, 0.0f, 0.0f };

						SendPacket::CharAdmin(g_GameSession, ClientConstants::FinishGame, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nChannelNum, psPos, 0);
						bResult = true;
					}
				} 
				// BGServer : 해당 방의 경기 종료 (승패 결정)
				else if (!strcmp(token, "/draw"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int nChannelNum = atoi(token);
						POS psPos = { 0.0f, 0.0f, 0.0f };

						SendPacket::CharAdmin(g_GameSession, ClientConstants::DrawGame, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nChannelNum, psPos, 0);
						bResult = true;
					}
				} 
				// BGServer : 해당 방의 경기 종료 (승패 결정)
				else if (!strcmp(token, "/ignoregame"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int nChannelNum = atoi(token);
						POS psPos = { 0.0f, 0.0f, 0.0f };

						SendPacket::CharAdmin(g_GameSession, ClientConstants::IgnoreGame, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nChannelNum, psPos, 0);
						bResult = true;
					}
				} 
				// BGServer : 해당 방의 최대 인원수 설정
				else if (!strcmp(token, "/max"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int nChannelNum = atoi(token);
						token = strtok(NULL, seps);
						if (token)
						{
							int nNum = atoi(token);
							POS psPos = { 0.0f, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::SetMax, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nChannelNum, psPos, (unsigned long)nNum);
							bResult = true;
						}
					}
				} 
				// BGServer : 해당 방의 목표 점수 설정
				else if (!strcmp(token, "/score"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int nChannelNum = atoi(token);
						token = strtok(NULL, seps);
						if (token)
						{
							int nScore = atoi(token);
							POS psPos = { 0.0f, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::SetScore, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nChannelNum, psPos, (unsigned long)nScore);
							bResult = true;
						}
					}
				} 
				// BGServer : 해당 방의 시간 제한 설정
				else if (!strcmp(token, "/time"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int nChannelNum = atoi(token);
						token = strtok(NULL, seps);
						if (token)
						{
							int nMin = atoi(token);
							POS psPos = { 0.0f, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::SetLimitTime, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nChannelNum, psPos, (unsigned long)nMin);
							bResult = true;
						}
					}
				}
				// BGServer : 해당 방의 쉬는 시간 설정
				else if (!strcmp(token, "/rest"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int nChannelNum = atoi(token);
						token = strtok(NULL, seps);
						if (token)
						{
							int nMin = atoi(token);
							POS psPos = { 0.0f, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::SetRestTime, pGame->m_csStatus.m_Info.Name, 0, 0, (unsigned short)nChannelNum, psPos, (unsigned long)nMin);
							bResult = true;
						}
					}
				}						


				// 운영자 레벨 2
				else if (!strcmp(token, "/deletespell"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int nSpellID = atoi(token);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, ClientConstants::DeleteSpell, strName, 0, 0, (unsigned short)nSpellID, psPos, 0);
								bResult = true;
							}
						}
					}
				}
				else if (!strcmp(token, "/stop"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							POS psPos = { 0.0f, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::Stop, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						}
					}
				}
				else if (!strcmp(token, "/stopoff"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							POS psPos = { 0.0f, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::StopOff, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						}
					}
				}
				// 무비 모드
				else if (!strcmp(token, "/#moviemode!") || !strcmp(token, "/무비모드"))
				{
					pGame->m_bMovieVersion = true;
					pGame->m_bSimpleMode = TRUE;

					CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					pScene->MovieMode( FALSE ) ;

					bResult = true;
				} 
				// 게임 모드 (무비 모드 종료)
				else if (!strcmp(token, "/#gamemode!") || !strcmp(token, "/게임모드"))
				{
					pGame->m_bMovieVersion = false;

					CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					pScene->MovieMode( TRUE ) ;

					bResult = true;
				} 
				// 존 이동 (간략)
				else if (!strcmp(token, "/zone"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int zone = atoi(token);
						token = strtok(NULL, seps);
						if (token)
						{
							double x = atof(token);
							token = strtok(NULL, seps);
							if (token)
							{
								double y = atof(token);

								POS psPos = { x, 100.0f, y };
								SendPacket::CharMoveZone(g_GameSession, zone, -1, psPos);
								bResult = true;
							} 
						} 
					} 
				} 
				// 존 이동 (개인)
				else if (!strcmp(token, "/move") || !strcmp(token, "/존이동"))
				{
					token = strtok(NULL, seps);
					char strName[16];
					if (token)
					{
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int zone = atoi(token);
								token = strtok(NULL, seps);
								if (token)
								{
									double x = atof(token);
									token = strtok(NULL, seps);
									if (token)
									{
										double y = atof(token);
										token = strtok(NULL, seps);
										if (token)
										{
											double z = atof(token);

											POS psPos = { x, y + 0.2f, z };
											if (zone == 100)
											{
												SendPacket::CharAdmin(g_GameSession, ClientConstants::MoveChar, strName, zone, -1, 0, psPos, 0);
											} 
											else
											{
												SendPacket::CharAdmin(g_GameSession, ClientConstants::MoveZoneChar, strName, zone, -1, 0, psPos, 0);
											}
											bResult = true;
										} 
									} 
								} 
							} 
						} 
					} 
				} 
				// 존 이동 (파티)
				else if (!strcmp(token, "/moveparty") || !strcmp(token, "/존이동파티"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								int zone = atoi(token);
								token = strtok(NULL, seps);
								if (token)
								{
									double x = atof(token);
									token = strtok(NULL, seps);
									if (token)
									{
										double y = atof(token);
										token = strtok(NULL, seps);
										if (token)
										{
											double z = atof(token);

											POS psPos = { x, y + 0.2f, z };
											if (zone == 100)
											{
												SendPacket::CharAdmin(g_GameSession, ClientConstants::MoveParty, strName, zone, -1, 0, psPos, 0);
											} 
											else
											{
												SendPacket::CharAdmin(g_GameSession, ClientConstants::MoveZoneParty, strName, zone, -1, 0, psPos, 0);
											}
											bResult = true;
										} 
									} 
								} 
							} 
						} 
					} 
				} 
				// 워프 (개인)
				else if (!strcmp(token, "/moveit") || !strcmp(token, "/워프"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								double x = atof(token);
								token = strtok(NULL, seps);
								if (token)
								{
									double y = atof(token);
									token = strtok(NULL, seps);
									if (token)
									{
										double z = atof(token);

										POS psPos = { x, y + 0.2f, z };
										SendPacket::CharAdmin(g_GameSession, ClientConstants::MoveChar, strName, 0, 0, 0, psPos, 0);
										bResult = true;
									} 
								} 
							} 
						} 
					} 
				} 
				// 워프 (파티)
				else if (!strcmp(token, "/moveitparty") || !strcmp(token, "/워프파티"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								double x = atof(token);
								token = strtok(NULL, seps);
								if (token)
								{
									double y = atof(token);
									token = strtok(NULL, seps);
									if (token)
									{
										double z = atof(token);

										POS psPos = { x, y + 0.2f, z };
										SendPacket::CharAdmin(g_GameSession, ClientConstants::MoveParty, strName, 0, 0, 0, psPos, 0);
										bResult = true;
									} 
								} 
							} 
						} 
					} 
				} 
				// 감금
				else if( !strcmp( token, "/prison" ) )
				{
					token = strtok(NULL, seps);
					char strName[16];
					if (token)
					{
						if( strlen(token) < 16 )
						{
							strcpy(strName, token);

							POS psPos = { 3127, 1136, 3212 };
							SendPacket::CharAdmin(g_GameSession, ClientConstants::MoveZoneChar, strName, 14, -1, 0, psPos, 0);
							bResult = true;
						} 
					} 
				}

				// 운영자 외치기
				else if (!strcmp(token, "/cz"))
				{
					unsigned long len = strlen(token) + 1;
					if (dwTotalLength >= len)
					{
						CRYLNetworkData::SendChatMessage(NULL, &strChat[len], PktChat::ADMIN_SHOUT);
					}

					bResult = true;
				}

				/*
				//by Hades Kang - Net Status 를 체크 하기 위한 임시 테스트용 코드
				else if (!strcmp(token, "/ns_on"))
				{
				token = strtok(NULL, seps);
				if (token)
				{
				if( !stricmp(token,"takeitem" ) )
				{
				ClientSocket::SetNSFlagOn( CmdCharTakeItem );
				}
				else if( !stricmp(token,"swapitem" ) )
				{
				ClientSocket::SetNSFlagOn( CmdCharSwapItem );
				}
				else if( !stricmp( token, "RepairItem" ) )
				{
				ClientSocket::SetNSFlagOn( CmdCharRepairItem );
				}
				else if( !stricmp( token, "UseItem" ) )
				{
				ClientSocket::SetNSFlagOn( CmdCharUseItem );
				}
				else
				{
				int nCmd = atoi( token );
				ClientSocket::SetNSFlagOn( nCmd );
				}
				bResult = true;
				}
				break;
				}
				else if (!strcmp(token, "/ns_off"))
				{
				token = strtok(NULL, seps);
				if (token)
				{
				if( !stricmp(token,"takeitem" ) )
				{
				ClientSocket::SetNSFlagOff( CmdCharTakeItem );
				}
				else if( !stricmp(token,"swapitem" ) )
				{
				ClientSocket::SetNSFlagOff( CmdCharSwapItem );
				}
				else if( !stricmp( token, "RepairItem" ) )
				{
				ClientSocket::SetNSFlagOff( CmdCharRepairItem );
				}
				else if( !stricmp( token, "UseItem" ) )
				{
				ClientSocket::SetNSFlagOff( CmdCharUseItem );
				}
				else
				{
				int nCmd = atoi( token );
				ClientSocket::SetNSFlagOff( nCmd );
				}
				bResult = true;
				}
				break;
				}
				else if (!strcmp(token, "/ns_check"))
				{
				int arrResult[ CmdFinalPacketNum ];
				int cnt = 0, i;
				char	buff[8];
				char	strMessage[256];
				strcpy( strMessage,"" );

				cnt = ClientSocket::CheckFlag( arrResult );
				if( cnt > 0 )
				{
				sprintf( strMessage, "%2x", arrResult[0] );
				for( i = 1 ; i < cnt; i++ )
				{
				sprintf(buff,", %2x", arrResult[i] );
				strcat( strMessage, buff );
				}
				AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
				bResult = true;
				break;
				}
				*/
				// 이동
				else if (!strcmp(token, "/goto") || !strcmp(token, "/이동"))
				{
					const char* tokenX = strtok(NULL, seps);
					const char* tokenY = strtok(NULL, seps);

					if(0 != tokenX && 0 != tokenY)
					{
						double x = atof(tokenX);
						double y = atof(tokenY);

						vector3 vecPos = vector3(x, 0.0f, y);
						vecPos.y = pGame->m_lpSceneManager->m_HeightField.GetHeight(vecPos) + 15.0f;
						POS psPos = { vecPos.x, vecPos.y / 100.0f, vecPos.z };

						// 수정된 방법
						SendPacket::CharAdmin(g_GameSession, ClientConstants::MoveChar, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);
						/*
						CRYLNetworkData::Instance()->SetNetworkFlag( NF_BINDPOS ) ; 
						SendPacket::CharBindPos(g_GameSession, 
						CRYLNetworkData::Instance()->m_dwMyChrID, 1, &psPos, 0);
						*/
						bResult = true;
					}
				} 

				// 어드민레벨1
				// 현재 좌표
				else if (!strcmp(token, "/pos") || !strcmp(token, "/좌표"))
				{
					char strText[MAX_PATH];
					float fDir = pSelfCreature->GetDirection() - FLOAT_PHI;
					if (fDir < 0.0f) fDir = FLOAT_PHI * 2 + fDir;

					sprintf( strText, "X = %f, Y = %f, Z = %f, Dir = %f", 
						pSelfCreature->GetPosition()->x / 100.0f,
						pSelfCreature->GetPosition()->y / 100.0f,
						pSelfCreature->GetPosition()->z / 100.0f,
						fDir );
					AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );

					bResult = true;
				} 
				// PC 에서 GM 으로.						
				/*else if (!strcmp(token, "/gmform"))
				{
				// PC 인지 체크.
				if(g_CharacterData.GetWeaponControlKind()==CHARACTER_CONTROL)
				{
				POS psPos = { 0.0f, 0.0f, 0.0f };

				// GM 모델로 변경.

				const vector3	vPos	= *(pSelfCreature->GetPosition());
				const float		fDir	= pSelfCreature->GetDirection();

				pSelfCreature->SetModelFileName("PC_GM.GCMDS");

				CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(pSelfCreature, "PC_GM.GCMDS");

				if(pCharModel)
				{
				pCharModel->SetPosition(vPos);
				pCharModel->SetDirection(fDir);
				pCharModel->SetMotionSheet("NO_WEAPON");

				const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];										
				std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

				pSelfCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true );

				// 레벨업 이펙트 출력.
				CEffScript *pLevelUpEff = new CEffScript;
				CSceneManager::AddEffectScript( pLevelUpEff );

				pLevelUpEff->GetScriptBinData( "respawn.esf", pGame->m_bSelfEffect );
				pLevelUpEff->SetStartPos( vPos.x, vPos.y, vPos.z );			

				pLevelUpEff->SetEndPos( (D3DXVECTOR3*)pSelfCreature->GetPosition(), 1 );

				bResult = true;

				pSelfCreature->SetGMModelFlag(1);
				}

				// 서버로 전송.
				SendPacket::CharAdmin(g_GameSession, PktAdmin::GM_FORM, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);
				}
				}
				// GM 에서 PC 로.
				else if (!strcmp(token, "/pcform"))
				{
				// GM 인지 체크.
				if(g_CharacterData.GetWeaponControlKind()==CHARACTER_CONTROL)
				{
				POS psPos = { 0.0f, 0.0f, 0.0f };

				// PC 모델로 변경.								

				RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;

				if(pSelfCreature)
				{									
				const vector3	vPos	= *(pSelfCreature->GetPosition());
				const float		fDir	= pSelfCreature->GetDirection();										

				char strSex[MAX_PATH];
				const char* strShape[ Item::EquipmentPos::MAX_EQUPMENT_POS ];

				for (int j = 0; j < Item::EquipmentPos::MAX_EQUPMENT_POS; j++)
				{
				CItemInstance* lpEquip = pGame->m_csStatus.m_lstEquipment[j];
				strShape[j] = (NULL == lpEquip) ? NULL : g_ItemMgr.GetEquipName(lpEquip->GetProtoTypeID()); 
				}

				switch (pGame->m_csStatus.m_Info.Sex)
				{
				case 1 :
				strcpy( strSex, "PC_MAN.GCMDS" ) ; 
				break;
				case 2:
				strcpy( strSex, "PC_WOMAN.GCMDS" ) ;
				break;
				case 3 :
				strcpy( strSex, "PC_Akhan_A.GCMDS" ) ; 
				break;
				case 4:
				strcpy( strSex, "PC_Akhan_B.GCMDS" ) ;
				break;

				}

				pSelfCreature->SetModelFileName(strSex);

				CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(pSelfCreature, strSex);

				if(pCharModel)
				{
				pCharModel->SetPosition(vPos);
				pCharModel->SetDirection(fDir);
				pSelfCreature->InitWeapon(0);

				pSelfCreature->SetShape(strShape, pGame->GetNetworkEquipGrade(), true);

				CEffScript *pLevelUpEff = new CEffScript;
				CSceneManager::AddEffectScript( pLevelUpEff );

				pLevelUpEff->GetScriptBinData( "respawn.esf", pGame->m_bSelfEffect );
				pLevelUpEff->SetStartPos( vPos.x, vPos.y, vPos.z );			

				pLevelUpEff->SetEndPos( (D3DXVECTOR3*)pSelfCreature->GetPosition(), 1 );

				bResult = true;

				pSelfCreature->SetGMModelFlag(0);
				}
				}

				// 서버로 전송.
				SendPacket::CharAdmin(g_GameSession, PktAdmin::PC_FORM, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);								
				}
				}*/			
				// 무적 모드
				else if (!strcmp(token, "/god"))
				{
					POS psPos = { 0.0f, 0.0f, 0.0f };
					SendPacket::CharAdmin(g_GameSession, ClientConstants::InvincibleChar, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);

					bResult = true;
				} 
				// 무적 모드 해제
				else if (!strcmp(token, "/human"))
				{
					POS psPos = { 0.0f, 0.0f, 0.0f };
					SendPacket::CharAdmin(g_GameSession, ClientConstants::NormalChar, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);

					bResult = true;
				} 
				// 강제 로그아웃
				else if (!strcmp(token, "/logout"))
				{
					token = strtok(NULL, seps);
					if(token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);

							POS psPos = { 0.0f, 0.0f, 0.0f };
							SendPacket::CharAdmin(g_GameSession, ClientConstants::LogoutChar, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						}
					}
				} 
				// 채팅 금지
				else if ( !strcmp( token, "/nochat" ) )
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							token = strtok(NULL, seps);
							if (token)
							{
								unsigned long dwAmount = atoi(token);
								POS psPos = { 0.0f, 0.0f, 0.0f };

								SendPacket::CharAdmin(g_GameSession, PktAdmin::CHAT_BAN, strName, 0, 0, 0, psPos, dwAmount);
								bResult = true;
							} 
						} 
					} 
				}
				// 채금 해제
				else if ( !strcmp( token, "/yeschat" ) )
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);

							POS psPos = { 0.0f, 0.0f, 0.0f };
							SendPacket::CharAdmin(g_GameSession, PktAdmin::CHAT_OK, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						}
					}
				}

				// 캐릭터 정보
				else if (!strcmp(token, "/hp"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);

							POS psPos = { 0.0f, 0.0f, 0.0f };
							SendPacket::CharAdmin(g_GameSession, ClientConstants::InfoChar, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						}
					} 
				}

				// 캐릭터 적아군 식별.
				/*else if(!strcmp(token, "/relation"))
				{							
				token = strtok(NULL, seps);
				if (token)
				{
				char strName[16];
				if (strlen(token) < 16)
				{
				strcpy(strName, token);

				CRYLGameData*	pGame	= CRYLGameData::Instance();									

				RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

				if ( !pSelfCreature )
				{
				AddMessage("Not Self Creature", CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				}

				CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );

				if ( !pSelfData )
				{
				AddMessage("Not Self Data", CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				}

				CharDataNode* pRivalData = RYLCharacterDataManager::Instance()->GetCharData( strName );

				if ( !pRivalData )
				{
				AddMessage("Not Rival Data", CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				}

				RYLCreature* pRivalCreature = RYLCreatureManager::Instance()->GetCreature(pRivalData->m_ulCharID);

				if ( !pRivalCreature )
				{
				AddMessage("Not Rival Creature", CRYLNetworkData::Instance()->m_dwMyChrID, 128);
				}				

				unsigned long dwResult	= CRYLGameData::Instance()->GetClientEnemyType(pRivalData->m_ulCharID);
				unsigned char cResult	= CRYLGameData::Instance()->GetEnemyTypeResult();

				POS psPos = { 0.0f, 0.0f, 0.0f };
				SendPacket::CharAdmin(g_GameSession, ClientConstants::Relation, 
				strName, cResult, 0, EnemyCheck::CCheckTable::GetInstance().GetEnemyCheckRule(), psPos, dwResult);
				bResult = true;
				}
				} 
				}*/
				// 추적
				else if (!strcmp(token, "/trace") || !strcmp(token, "/추적"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);

							POS psPos = { 0.0f, 0.0f, 0.0f };
							SendPacket::CharAdmin(g_GameSession, ClientConstants::TraceChar, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						}
					}
				} 
				// 숨기
				else if (!strcmp(token, "/hide"))
				{
					POS psPos = { 0.0f, 0.0f, 0.0f };
					SendPacket::CharAdmin(g_GameSession, ClientConstants::HideChar, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);

					bResult = true;
				} 
				// 나타나기
				else if (!strcmp(token, "/show"))
				{
					POS psPos = { 0.0f, 0.0f, 0.0f };
					SendPacket::CharAdmin(g_GameSession, ClientConstants::ShowChar, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);

					bResult = true;
				} 
				else if (!strcmp(token, "/fly")) // 하늘 날기 (네오 모드)
				{ 
					POS psPos = { 0.0f, 0.0f, 0.0f };
					SendPacket::CharAdmin(g_GameSession, ClientConstants::Neo, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);
					/*
					SendPacket::CharAdmin(g_GameSession, ClientConstants::HideChar, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);

					g_CharacterData.m_bFreelook		= TRUE;
					g_CharacterData.m_vecCameraPos  = *( pSelfCreature->GetPosition() );
					g_CharacterData.m_fCameraDir    = pSelfCreature->GetDirection();
					*/
					bResult = true;
				} 
				// 네오 모드 해제
				else if (!strcmp(token, "/land"))
				{
					POS psPos = { 0.0f, 0.0f, 0.0f };
					SendPacket::CharAdmin(g_GameSession, ClientConstants::Smith, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);

					/*
					//							SendPacket::CharAdmin(g_GameSession, ClientConstants::ShowChar, pGame->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);

					CRYLGameData::SetAutoRun( FALSE ) ;
					CRYLGameData::Instance()->m_bUpdateMove = FALSE ;
					g_CharacterData.m_bFreelook = FALSE;
					*/
					bResult = true;
				} 
				// 네오 모드에서의 움직임 속도 조절
				else if (!strcmp(token, "/camspeed"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int a = atoi(token);
						g_CharacterData.m_fCameraSpeed = (float)a;
						bResult = true;
					}
				}
				// 평화 모드
				else if (!strcmp(token, "/peacemode"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							POS psPos = { 0.0f, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::PeaceMode, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						} 
					} 
				} 
				// 전쟁 모드
				else if (!strcmp(token, "/battlemode"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						char strName[16];
						if (strlen(token) < 16)
						{
							strcpy(strName, token);
							POS psPos = { 0.0f, 0.0f, 0.0f };

							SendPacket::CharAdmin(g_GameSession, ClientConstants::WarMode, strName, 0, 0, 0, psPos, 0);
							bResult = true;
						} 
					} 
				} 
				// 운영자 레벨
				else if (!strcmp(token, "/adminlevel"))
				{
					char strText[MAX_PATH];
					switch (g_ClientMain.m_wAdminMode)
					{
						// 운영자 레벨 0
					case SUPPORTER:
						strcpy(strText, CRYLStringTable::m_strString[573]);
						break;

						// 운영자 레벨 1
					case ADMIN_L1:
						strcpy(strText, CRYLStringTable::m_strString[574]);
						break;

						// 운영자 레벨 2
					case ADMIN_L2:
						strcpy(strText, CRYLStringTable::m_strString[575]);
						break;

						// 개발자
					case ADMIN_L3:
					case ADMIN_DEV:
						strcpy(strText, CRYLStringTable::m_strString[576]);
						break;
					}

					AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

					bResult = true;
				} 
				// 날씨 변경
				else if (!strcmp(token, "/setweather"))
				{
					token = strtok(NULL, seps);
					if (token)
					{
						int nTime = atoi(token);

						GRYLTimer.m_dwStartTime = timeGetTime();
						GRYLTimer.m_dwServerTime = nTime * 600000;
					}

					bResult = true;
				}
			}

			if (false == bResult)
			{
				AddMessage(CRYLStringTable::m_strString[571], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
			}
			//}

			else
			{
				char strMessage[MAX_PATH];
				unsigned long Count = 0;
				strcpy(strMessage, m_pChatEdit->GetText());

				// 일반 채팅
				if(1)//g_StringFilter.ConvertFilter(strMessage))
				{
					switch ( m_dwChatMode )
					{
					case RYLCHATMODE::RYL_CHATMODE_WHOLE :
					case RYLCHATMODE::RYL_CHATMODE_CHAT:
						if (strChat[ 0 ] != '/'){
							for(int i = 0; i < 3; i++)
							{
								if (!strcmp(CRYLCommunityData::Instance()->m_strChatMessage[i], strMessage)) Count++;
							}
							if (Count < 2)
							{
								for(int i = 0; i < 2; i++)
								{
									strcpy(CRYLCommunityData::Instance()->m_strChatMessage[i], CRYLCommunityData::Instance()->m_strChatMessage[i + 1]);
								}
								strcpy(CRYLCommunityData::Instance()->m_strChatMessage[2], strMessage);
								CRYLNetworkData::SendChatMessage(NULL, strMessage, PktChat::NORMAL, m_usLocalLang);
							} else
							{
								AddMessage(CRYLStringTable::m_strString[104], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							}
						}
						break;

					case RYLCHATMODE::RYL_CHATMODE_SHOUT :
						CRYLNetworkData::SendChatMessage(NULL, strMessage, PktChat::SHOUT, m_usLocalLang);
						break ;

					case RYLCHATMODE::RYL_CHATMODE_DEAL :
						CRYLNetworkData::SendChatMessage(NULL, strMessage, PktChat::TRADE, m_usLocalLang);
						break ;					

					case RYLCHATMODE::RYL_CHATMODE_PARTY:
						CRYLNetworkData::SendChatMessage(NULL, strMessage, PktChat::PARTY);
						break;

					case RYLCHATMODE::RYL_CHATMODE_GUILD:
						{
							CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
							CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
							CRYLAuthoritySettingDlg* pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;

							unsigned char ucTitle = pASDlg->GetTitle() ;
							if ( ucTitle <= Guild::COMMON )
								CRYLNetworkData::SendChatMessage(NULL, strMessage, PktChat::GUILD);
						}
						break;
					}
				} 
				else
				{
					CRYLNetworkData::SendClientLog(1, m_pChatEdit->GetText(), g_StringFilter.m_strSearched);
					AddMessage(CRYLStringTable::m_strString[570], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				}
			}
		} 

		m_pChatEdit->SetText( "" ) ;
	}
}

VOID	CRYLChattingDlg::AddChatMessage( LPSTR strMessage, DWORD dwSender, DWORD dwCommand ) 
{
	CHAR	szText[ MAX_PATH ] ;
	BOOL	bAdd = FALSE ;
	INT		i ;
	DWORD	dwColor = CRYLStringTable::GetChatColor( dwCommand ) ;

	INT		iMaxWidth = 540 ;		// 긴 문장 아래로 내리기 사이즈
	INT		iMaxAnsiWidth = 540 ;
	if ( 0 == m_iCurrentSize || 2 == m_iCurrentSize )
	{
		iMaxWidth = 298 ;
		iMaxAnsiWidth = 290 ;
	}

	if ( dwSender != CRYLNetworkData::Instance()->m_dwMyChrID )
	{
		RYLCreature* pSenderCreature = RYLCreatureManager::Instance()->GetCreature( dwSender );
		if( pSenderCreature )
		{
			CharDataNode* pSendeerData = RYLCharacterDataManager::Instance()->GetCharData( pSenderCreature->GetCharID() );
			if( pSendeerData )
			{
				pSendeerData->m_strChatMessage = strMessage;
			}

			pSenderCreature->m_fChatTimer = 8000.0f;
		}
	}

	int		slpos = 0;
	INT		len = strlen( strMessage ), count = 0, countx = 0, start = 0 ;

	for ( i = 0 ; i < len ; i ++ )
	{
		if ( strMessage[ i ] & 0x80 )
		{
			countx += CRYLStringTable::m_dwTextSize[ 128 ] ;
			count += 2 ;
			i ++ ;

			if ( countx >= iMaxWidth ) // 540 - 12 하나의 최고 폭
			{
				memcpy( &szText[slpos], &strMessage[ start ], count ) ;
				szText[ slpos+count ] = '\0' ;
				AddChatText( szText, dwColor, RYLCHATMODE::RYL_CHATMODE_CHAT ) ;
				countx = count = 0 ;
				start  = i + 1 ;

				slpos = 0;
				for(int x = 0; x < 10; ++x)
				{
					szText[ slpos ] = ' ';
					countx += CRYLStringTable::m_dwTextSize[ ' ' ] ;
					slpos++;
				}
			}
		} 
		else
		{
			countx += CRYLStringTable::m_dwTextSize[ strMessage[ i ] ] ;
			count ++ ;

			if ( countx >= iMaxAnsiWidth ) // 540 - 8 하나의 최고 폭
			{
				memcpy( &szText[slpos], &strMessage[ start ], count ) ;
				szText[ slpos+count ] = '\0' ;
				AddChatText( szText, dwColor, RYLCHATMODE::RYL_CHATMODE_CHAT ) ;
				countx = count = 0;
				start = i + 1;

				slpos = 0;
				for(int x = 0; x < 10; ++x)
				{
					szText[ slpos ] = ' ';
					countx += CRYLStringTable::m_dwTextSize[ ' ' ] ;
					slpos++;
				}
			}
		}
	}

	if ( start <= len )
	{
		memcpy( &szText[slpos], &strMessage[ start ], count ) ;
		szText[ slpos+count ] = '\0' ;
		AddChatText( szText, dwColor, RYLCHATMODE::RYL_CHATMODE_CHAT ) ;
	}
}

VOID	CRYLChattingDlg::AddStallChatMessage( CRYLStallDlg* pDlg, LPSTR strMessage, DWORD dwSender, DWORD dwCommand ) 
{
	BOOL	bAdd = FALSE ;
	INT		i ;
	DWORD	dwColor = CRYLStringTable::GetChatColor( dwCommand ) ;

	INT		len = strlen( strMessage ), count = 0, countx = 0, start = 0 ;

	for ( i = 0 ; i < len ; i ++ )
	{
		if ( strMessage[ i ] & 0x80 )
		{
			countx += CRYLStringTable::m_dwTextSize[ 128 ] ;
			count  += 2 ;  
			i ++ ;

			if ( countx >= 326 ) // 338 - 12 하나의 최고 폭
			{
				if ( m_deqStallMessage.size() >= RYLCHATMODE::RYL_MAX_CHATMESSAGE )
				{
					GM_DELETE( (*m_deqStallMessage.begin() ) ) ;
					m_deqStallMessage.pop_front() ;
				}

				Message* lpMessage = new Message;
				memcpy( lpMessage->strMessage, &strMessage[ start ], count ) ;
				lpMessage->strMessage[ count ] = '\0' ;
				lpMessage->dwCommand	= dwCommand ;
				lpMessage->fTimer		= 8000.0f ;
				lpMessage->dwColor		= dwColor ; 

				pDlg->AddChatMessage( lpMessage->strMessage, dwColor ) ;

				m_deqStallMessage.push_back( lpMessage ) ;

				if ( m_deqStallMessage.size() > RYLCHATMODE::RYL_MAX_DRAWSTALLCHAT )
				{
					m_dwStartStall = m_deqStallMessage.size() - RYLCHATMODE::RYL_MAX_DRAWSTALLCHAT ;
				}
				countx = count = 0 ;
				start  = i + 1 ;
			}
		} 
		else
		{
			countx += CRYLStringTable::m_dwTextSize[ strMessage[ i ] ] ;
			count ++ ;

			if ( countx >= 326 ) // 338 - 8 하나의 최고 폭
			{
				if ( m_deqStallMessage.size() >= RYLCHATMODE::RYL_MAX_CHATMESSAGE )
				{
					GM_DELETE( (*m_deqStallMessage.begin() ) ) ;
					m_deqStallMessage.pop_front() ;
				}

				Message *lpMessage;
				lpMessage = new Message;
				memcpy(lpMessage->strMessage, &strMessage[start], count);
				lpMessage->strMessage[count] = '\0';
				lpMessage->dwCommand	= dwCommand ;
				lpMessage->fTimer		= 8000.0f ;
				lpMessage->dwColor		= dwColor ;

				pDlg->AddChatMessage( lpMessage->strMessage, dwColor ) ;

				m_deqStallMessage.push_back( lpMessage ) ;

				if ( m_deqStallMessage.size() > RYLCHATMODE::RYL_MAX_DRAWSTALLCHAT )
				{
					m_dwStartStall = m_deqStallMessage.size() - RYLCHATMODE::RYL_MAX_DRAWSTALLCHAT ;
				}
				countx = count = 0 ;
				start  = i + 1 ;
			}
		}
	}

	if ( start <= len )
	{
		if ( m_deqStallMessage.size() >= RYLCHATMODE::RYL_MAX_CHATMESSAGE )
		{
			GM_DELETE( ( *m_deqStallMessage.begin() ) ) ;
			m_deqStallMessage.pop_front() ;
		}

		Message *lpMessage;
		lpMessage = new Message;
		memcpy( lpMessage->strMessage, &strMessage[ start ], count ) ;
		lpMessage->strMessage[ count ] = '\0';
		lpMessage->dwCommand	= dwCommand ;
		lpMessage->fTimer		= 8000.0f ;
		lpMessage->dwColor		= dwColor ;

		pDlg->AddChatMessage( lpMessage->strMessage, dwColor ) ;

		m_deqStallMessage.push_back( lpMessage ) ;

		if ( m_deqStallMessage.size() > RYLCHATMODE::RYL_MAX_DRAWSTALLCHAT )
		{
			m_dwStartStall = m_deqStallMessage.size() - RYLCHATMODE::RYL_MAX_DRAWSTALLCHAT ;
		}

	}
}

void	CRYLChattingDlg::SaveText( LPSTR strMessage, DWORD dwCommand ) 
{
	DWORD	dwColor = CRYLStringTable::GetChatColor( dwCommand ) ;
	INT		len = strlen( strMessage ) ;
	Message *lpMessage;
	lpMessage = new Message;
	memcpy( lpMessage->strMessage, strMessage, len ) ;
	lpMessage->strMessage[ len ] = '\0' ;
	lpMessage->dwCommand	= dwCommand ;
	lpMessage->fTimer		= 8000.0f ;
	lpMessage->dwColor		= dwColor ;

	if ( m_deqChatMessage.size() >= RYLCHATMODE::RYL_MAX_CHATMESSAGE )
	{
		GM_DELETE( (*m_deqChatMessage.begin() ) ) ;
		m_deqChatMessage.pop_front() ;
	}
	m_deqChatMessage.push_back( lpMessage ) ;
}

VOID	CRYLChattingDlg::AddMessage( LPSTR strMessage, DWORD dwSender, DWORD dwCommand ) 
{
	AddChatMessage( strMessage, dwSender,dwCommand ) ;
	SaveText( strMessage, dwCommand ) ;
}


VOID	CRYLChattingDlg::DeleteStallMessage()
{
	deque< Message * >::iterator it ;
	for ( it = m_deqStallMessage.begin(); it != m_deqStallMessage.end(); it++ )
	{
		GM_DELETE( (*it) ) ;
	}
	m_deqStallMessage.clear() ;
	m_dwStartStall = 0 ;
}

VOID	CRYLChattingDlg::SetWhisperMode() 
{
	m_pShoutCheckBox->SetChecked( FALSE ) ;
	m_pTradeCheckBox->SetChecked( FALSE ) ;
	m_pPartyCheckBox->SetChecked( FALSE ) ;
	m_pGuildCheckBox->SetChecked( FALSE ) ;
	m_bShout				  = FALSE ;
	m_bTrade				  = FALSE ;
	m_bGuild				  = FALSE ;
	m_bParty				  = FALSE ;

	m_pWhisperCheckBox->SetChecked( TRUE ) ;
	m_pChatEdit->SetFocus() ;
}

//-----------------------------------------------------------------------------------------------------
// CRYLChattingDlg::SetFocusChattingEdit()
VOID CRYLChattingDlg::SetFocusChattingEdit()
{
	//-- 2004. . . Zergra From.--//
	if ( m_pChatEdit )
	{
		m_pChatEdit->SetFocus();
		m_pChatEdit->SetFocusState( FALSE );
	}
	//-- Zergra To. --//
}
