#include <winsock2.h>
#include "RYLQuestDetailDataDlg.h"
#include "BaseDataDefine.h"
#include "RYLButton.h"
#include "RYLListBox.h"
#include "RYLLabel.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLChattingDlg.h"

#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"

#include "RYLStringTable.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "..\\CharStatus.h"

#include <Creature/Monster/MonsterMgr.h>

#include <Network/ClientSocket/Send/SendQuest.h>
#include <RenderOption.h>
#include "GMMemory.h"

CRYLQuestDetailDataDlg*		g_pQuestDetailDataDlg = NULL ;

CRYLQuestDetailDataDlg::CRYLQuestDetailDataDlg() 
{
	m_pQuestTitle					= NULL ; // 퀘스트 제목 값
	m_pQuestLevel					= NULL ; // 퀘스트 레벨 값
	m_pQuestPurposeLabel			= NULL ; // 퀘스트 목적
	m_pExecutionReportLabel			= NULL ; // 퀘스트 수행과제
	m_pQuestCompensationLabel		= NULL ; // 퀘스트 보상
	m_pTargetObjectLabel			= NULL ; // 대상물
	m_pTargetObjectValueLabel		= NULL ; // 대상물값
	m_pProgressStateLabel			= NULL ; // 진행상황
	m_pProgressStateValueLabel		= NULL ; // 진행상황 값

	m_pQuestPurposeListBox			= NULL ; // 퀘스트목적 리스트
	m_pExecutionReportListBox		= NULL ; // 수행과제 리스트
	m_pQuestCompensationListBox		= NULL ; // 퀘스트 보상 리스트
	//----------------------------------------------------------------

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 397, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	for ( int i = 0 ; i < 74 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 387, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 0, 251, 22,  0xc0000000, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 2, 20, 251, 1,  0xffB99E6B, 0x80000000,  1 ) ;

	//pImageDIB->MakeFrameRect( 2, 15, 252, 381, 0xff5c4f35, 0x80000000,  1 ) ;
	
	pImageDIB->MakeFrameRect( 7, 49, 242, 76,  0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 7, 151, 242, 97,  0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 7, 272, 242, 67,  0xffB99E6B, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect( 7, 367, 170, 19,  0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 181, 367, 68, 19,  0xffB99E6B, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 422 ) ;

	SetLeft( 256 ) ;
	SetTop( 135 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	InitCtrl() ;
	g_pQuestDetailDataDlg = this ;
}

CRYLQuestDetailDataDlg::~CRYLQuestDetailDataDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLQuestDetailDataDlg::InitCtrl() 
{	
	// 퀘스트 제목 값 (Quest title value)
	m_pQuestTitle = new CRYLLabel( this ) ;		
	m_pQuestTitle->SetLeft( 8 ) ;
	m_pQuestTitle->SetTop( 18 ) ;
	m_pQuestTitle->SetFontColor ( 0xffffffff ) ;
	m_pQuestTitle->SetAutoSize( TRUE ) ;
	m_pQuestTitle->SetCaption( "" ) ;
	AttachGMUIControl( m_pQuestTitle ) ;

	// 퀘스트 레벨 값 (Quest level value)
	m_pQuestLevel = new CRYLLabel( this ) ;		
	m_pQuestLevel->SetLeft( 182 ) ;
	m_pQuestLevel->SetTop( 18 ) ;
	m_pQuestLevel->SetFontColor ( 0xffffffff ) ;
	m_pQuestLevel->SetAutoSize( TRUE ) ;
	m_pQuestLevel->SetCaption( "" ) ;
	AttachGMUIControl( m_pQuestLevel ) ;

	// 퀘스트 목적 (Quest objectives)
	m_pQuestPurposeLabel = new CRYLLabel( this ) ;		
	m_pQuestPurposeLabel->SetLeft( 8 ) ;
	m_pQuestPurposeLabel->SetTop( 49 ) ;
	m_pQuestPurposeLabel->SetFontColor (0xffffffff);
	m_pQuestPurposeLabel->SetAutoSize( TRUE ) ;
	m_pQuestPurposeLabel->SetCaption( CRYLStringTable::m_strString[ 1947 ] ) ;
	AttachGMUIControl( m_pQuestPurposeLabel ) ;

	// 퀘스트 단계 (Quest Phases)
	m_pQuestPhase = new CRYLLabel( this ) ;		
	m_pQuestPhase->SetLeft( 182 ) ;
	m_pQuestPhase->SetTop( 49 ) ;
	m_pQuestPhase->SetFontColor ( 0xffffffff ) ;
	m_pQuestPhase->SetAutoSize( TRUE ) ;
	m_pQuestPhase->SetCaption( "" ) ;
	AttachGMUIControl( m_pQuestPhase ) ;

	// 퀘스트 수행과제 (Quest Tasks)
	m_pExecutionReportLabel = new CRYLLabel( this ) ;		
	m_pExecutionReportLabel->SetLeft( 8 ) ;
	m_pExecutionReportLabel->SetTop( 151 ) ;
	m_pExecutionReportLabel->SetFontColor (0xffffffff);
	m_pExecutionReportLabel->SetAutoSize( TRUE ) ;
	m_pExecutionReportLabel->SetCaption( CRYLStringTable::m_strString[ 1948 ] ) ;
	AttachGMUIControl( m_pExecutionReportLabel ) ;

	// 퀘스트 보상 (Quest Reward)
	m_pQuestCompensationLabel = new CRYLLabel( this ) ;		
	m_pQuestCompensationLabel->SetLeft( 8 ) ;
	m_pQuestCompensationLabel->SetTop( 272 ) ;
	m_pQuestCompensationLabel->SetFontColor (0xffffffff);
	m_pQuestCompensationLabel->SetAutoSize( TRUE ) ;
	m_pQuestCompensationLabel->SetCaption( CRYLStringTable::m_strString[ 1954 ] ) ;
	AttachGMUIControl( m_pQuestCompensationLabel ) ;

	// 대상물 (Object)
	m_pTargetObjectLabel = new CRYLLabel( this ) ;		
	m_pTargetObjectLabel->SetLeft( 8 ) ;
	m_pTargetObjectLabel->SetTop( 364 ) ;
	m_pTargetObjectLabel->SetFontColor (0xffffffff);
	m_pTargetObjectLabel->SetAutoSize( TRUE ) ;
	m_pTargetObjectLabel->SetCaption( CRYLStringTable::m_strString[ 1949 ] ) ;
	AttachGMUIControl( m_pTargetObjectLabel ) ;

	// 대상물값 (Object Value)
	m_pTargetObjectValueLabel = new CRYLLabel( this ) ;		
	m_pTargetObjectValueLabel->SetLeft( 10 ) ;
	m_pTargetObjectValueLabel->SetTop( 386) ;
	m_pTargetObjectValueLabel->SetFontColor (0xffffffff);
	m_pTargetObjectValueLabel->SetAutoSize( TRUE ) ;
	m_pTargetObjectValueLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pTargetObjectValueLabel ) ;

	// 진행상황 (Progress)
	m_pProgressStateLabel = new CRYLLabel( this ) ;		
	m_pProgressStateLabel->SetLeft( 182 ) ;
	m_pProgressStateLabel->SetTop( 364 ) ;
	m_pProgressStateLabel->SetFontColor (0xffffffff);
	m_pProgressStateLabel->SetAutoSize( TRUE ) ;
	m_pProgressStateLabel->SetCaption( CRYLStringTable::m_strString[ 1950 ] ) ;
	AttachGMUIControl( m_pProgressStateLabel ) ;

	// 진행상황 값 (Progress value)
	m_pProgressStateValueLabel = new CRYLLabel( this ) ;		
	m_pProgressStateValueLabel->SetLeft( 189 ) ;
	m_pProgressStateValueLabel->SetTop( 386 ) ;
	m_pProgressStateValueLabel->SetFontColor (0xffffffff);
	m_pProgressStateValueLabel->SetAutoSize( TRUE ) ;
	m_pProgressStateValueLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pProgressStateValueLabel ) ;

	RECT rtRect ;
	CGMImageDib* pSprite	= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	// 퀘스트목적 리스트 (Quest objectives list)
	m_pQuestPurposeListBox = new CRYLListBox( this, TRUE ) ;
	m_pQuestPurposeListBox->SetLeft( 8 ) ;
	m_pQuestPurposeListBox->SetTop( 67 ) ;
	m_pQuestPurposeListBox->SetWidth( 237 ) ;
	m_pQuestPurposeListBox->SetHeight( 69 ) ;
	
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pQuestPurposeListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 186, 240, 200 ) ;
	m_pQuestPurposeListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 186, 226, 200 ) ;
	m_pQuestPurposeListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pQuestPurposeListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 235, 164, 243, 184 ) ;
 	m_pQuestPurposeListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestPurposeListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestPurposeListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestPurposeListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 198, 200, 212, 214 ) ;
	m_pQuestPurposeListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 200, 240, 214 ) ;					  
	m_pQuestPurposeListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 200, 226, 214 ) ;					  
	m_pQuestPurposeListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 200, 212, 214 ) ;					  
	m_pQuestPurposeListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;
 
	m_pQuestPurposeListBox->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pQuestPurposeListBox ) ;

	// 수행과제 리스트 (Projects List)
	m_pExecutionReportListBox = new CRYLListBox( this, TRUE ) ;
	m_pExecutionReportListBox->SetLeft( 8 ) ;
	m_pExecutionReportListBox->SetTop( 169 ) ;
	m_pExecutionReportListBox->SetWidth( 237 ) ;
	m_pExecutionReportListBox->SetHeight( 91 ) ;
	
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pExecutionReportListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 186, 240, 200 ) ;
	m_pExecutionReportListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 186, 226, 200 ) ;
	m_pExecutionReportListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pExecutionReportListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 235, 164, 243, 184 ) ;
 	m_pExecutionReportListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pExecutionReportListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pExecutionReportListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pExecutionReportListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 198, 200, 212, 214 ) ;
	m_pExecutionReportListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 200, 240, 214 ) ;					  
	m_pExecutionReportListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 200, 226, 214 ) ;					  
	m_pExecutionReportListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 200, 212, 214 ) ;					  
	m_pExecutionReportListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;
 
	m_pExecutionReportListBox->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pExecutionReportListBox ) ;

	// 퀘스트 보상 리스트 (Quest Reward List)
	m_pQuestCompensationListBox = new CRYLListBox( this, TRUE ) ;
	m_pQuestCompensationListBox->SetLeft( 8 ) ;
	m_pQuestCompensationListBox->SetTop( 291 ) ;
	m_pQuestCompensationListBox->SetWidth( 237 ) ;
	m_pQuestCompensationListBox->SetHeight( 60 ) ;

	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pQuestCompensationListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 186, 240, 200 ) ;
	m_pQuestCompensationListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 186, 226, 200 ) ;
	m_pQuestCompensationListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pQuestCompensationListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 235, 164, 243, 184 ) ;
	m_pQuestCompensationListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestCompensationListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestCompensationListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pQuestCompensationListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 198, 200, 212, 214 ) ;
	m_pQuestCompensationListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 200, 240, 214 ) ;					  
	m_pQuestCompensationListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 200, 226, 214 ) ;					  
	m_pQuestCompensationListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 200, 212, 214 ) ;					  
	m_pQuestCompensationListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	m_pQuestCompensationListBox->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pQuestCompensationListBox ) ;
}

HRESULT		CRYLQuestDetailDataDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if(g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if(g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}

	return S_OK ;
}

VOID	CRYLQuestDetailDataDlg::FinalCtrl() 
{
	GM_DELETE( m_pQuestTitle ) ;				// 퀘스트 제목 값
	GM_DELETE( m_pQuestLevel ) ;				// 퀘스트 레벨 값
	GM_DELETE( m_pQuestPurposeLabel ) ;			// 퀘스트 목적
	GM_DELETE( m_pQuestPhase ) ;				// 퀘스트 단계
	GM_DELETE( m_pExecutionReportLabel ) ;		// 퀘스트 수행과제
	GM_DELETE( m_pQuestCompensationLabel ) ;	// 퀘스트 보상
	GM_DELETE( m_pTargetObjectLabel ) ;			// 대상물
	GM_DELETE( m_pTargetObjectValueLabel ) ;	// 대상물값
	GM_DELETE( m_pProgressStateLabel ) ;		// 진행상황
	GM_DELETE( m_pProgressStateValueLabel ) ;	// 진행상황 값
	GM_DELETE( m_pQuestPurposeListBox ) ;		// 퀘스트목적 리스트
	GM_DELETE( m_pExecutionReportListBox ) ;	// 수행과제 리스트
	GM_DELETE( m_pQuestCompensationListBox ) ;	// 퀘스트 보상 리스트
}

VOID	CRYLQuestDetailDataDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLQuestDetailDataDlg::Hide( CGMUIObject* Sender ) 
{

}

VOID	CRYLQuestDetailDataDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CRYLQuestDetailDataDlg::SetQuestTitle( LPSTR strTitle )
{
	m_pQuestTitle->SetCaption( strTitle ) ;
}

LPSTR	CRYLQuestDetailDataDlg::GetQuestTitle()
{
	return m_pQuestTitle->GetCaption() ;
}

VOID	CRYLQuestDetailDataDlg::SetDetailDataDlg( QuestInstance* lpQuestInstance )
{
	m_pQuestPurposeListBox->ClearItem() ;
	m_pExecutionReportListBox->ClearItem() ;
	m_pQuestCompensationListBox->ClearItem() ;

	if ( !lpQuestInstance->m_lpQuestScript || !lpQuestInstance->m_lpPhaseScript )
	{
		 return ;
	}

	char strQuestDesc[2048] ;
	char *strMessageTemp, *token = NULL, seps[] = "\\";
	strcpy(strQuestDesc, lpQuestInstance->m_lpQuestScript->m_strQuestShortDesc);
	strMessageTemp = CRYLStringTable::MakeWordBreak(strQuestDesc, 220);

	token = strtok(strMessageTemp, seps);
	while (token != NULL)
	{
		m_pQuestPurposeListBox->AddItem( token, 0xffffffff, 0xffffffff, 0xffffffff ) ;
		token = strtok(NULL, seps);
	}

	strQuestDesc[ 0 ] = '\0' ;
	strcat(strQuestDesc, lpQuestInstance->m_lpPhaseScript->m_strPhaseDesc);

	
	strMessageTemp = CRYLStringTable::MakeWordBreak(strQuestDesc, 220);
	token = strtok(strMessageTemp, seps);

	while (token != NULL)
	{
		m_pExecutionReportListBox->AddItem( token, 0xffffffff, 0xffffffff, 0xffffffff ) ;
		token = strtok(NULL, seps);
	}

	strQuestDesc[ 0 ] = '\0' ;
	strcat(strQuestDesc, lpQuestInstance->m_lpQuestScript->m_strQuestAward);


	strMessageTemp = CRYLStringTable::MakeWordBreak(strQuestDesc, 220);
	token = strtok(strMessageTemp, seps);

	while (token != NULL)
	{
		m_pQuestCompensationListBox->AddItem( token, 0xffffffff, 0xffffffff, 0xffffffff ) ;
		token = strtok(NULL, seps);
	}

	CHAR strTemp[ MAX_PATH ] ;
	// edith 2008.03.21 퀘스트 진행 퍼센트 표시 변경
//	sprintf( strTemp, CRYLStringTable::m_strString[ 1925 ], (int)( (lpQuestInstance->m_dwPresentPhase - 1) * 100.0f / lpQuestInstance->m_lpQuestScript->m_wMaxPhase ) ) ;
	m_pQuestTitle->SetCaption( lpQuestInstance->m_lpQuestScript->m_strQuestTitle ) ;
	m_pQuestLevel->SetCaption( lpQuestInstance->m_lpQuestScript->m_strQuestLevel ) ;
	m_pQuestPhase->SetCaption( " " ) ;
//	m_pQuestPhase->SetCaption( strTemp ) ;

	TriggerNode* lpTriggerNode = lpQuestInstance->m_lpPhaseScript->m_lstTrigger[0];
	if ( NULL == lpTriggerNode )
	{
		return ;
	}

	// edith 2008.01.25 (퀘스트정보를 화면에 출력하지 못한다.)
	if ( lpTriggerNode->m_dwTriggerID & Creature::NPC_BIT )
	{
		NPCNode* pNPC = g_NPCList.GetNPCNode( lpTriggerNode->m_dwTriggerID ) ;
		if ( pNPC )
		{
			CRYLStringTable::GetFirstStringByToken( strTemp, pNPC->m_strNpcName, "\\" ) ;
			m_pTargetObjectValueLabel->SetCaption( strTemp ) ;
			m_pProgressStateValueLabel->SetCaption( "" ) ;
		}
		else
		{
			m_pTargetObjectValueLabel->SetCaption( "" ) ;
			m_pProgressStateValueLabel->SetCaption( "" ) ;
		}
	}
	else
	{
		sprintf(strTemp, " %2d / %2d", lpTriggerNode->m_dwMaxCount - lpQuestInstance->m_lstTriggerCount[0],
			lpTriggerNode->m_dwMaxCount ) ;

		// edith 2008.01.25 트리거 PICK 일때 아이템 설명을 표시해줘야한다.
		// lpTriggerNode->m_dwTriggerID 아이템 인덱스 번호가 된다.
		if(lpTriggerNode->m_dwTriggerKind == TRIGGER_PICK)
		{
			const Item::ItemInfo *lpItemInfo = g_ItemMgr.GetItemInfo(lpTriggerNode->m_dwTriggerID);
			if ( lpItemInfo )
			{
				m_pTargetObjectValueLabel->SetCaption( const_cast<LPSTR>(lpItemInfo->m_SpriteData.m_szName) ) ;
				m_pProgressStateValueLabel->SetCaption( strTemp ) ;
			}
			else
			{
				m_pTargetObjectValueLabel->SetCaption( "" ) ;
				m_pProgressStateValueLabel->SetCaption( "" ) ;
			}
		}
		else if(lpTriggerNode->m_dwTriggerKind == TRIGGER_FAME)
		{
			m_pTargetObjectValueLabel->SetCaption( "" ) ;
			m_pProgressStateValueLabel->SetCaption( "" ) ;
		}
		else
		{
			const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( lpTriggerNode->m_dwTriggerID ) ;
			if ( lpMonster )
			{
				m_pTargetObjectValueLabel->SetCaption( const_cast<LPSTR>(lpMonster->m_MonsterInfo.m_strName) ) ;
				m_pProgressStateValueLabel->SetCaption( strTemp ) ;
			}
			else
			{
				m_pTargetObjectValueLabel->SetCaption( "" ) ;
				m_pProgressStateValueLabel->SetCaption( "" ) ;
			}
		}
	}
}


VOID	CRYLQuestDetailDataDlg::UpdateQuestMsg( QuestInstance* lpQuestInstance )
{
	if ( !lpQuestInstance->m_lpQuestScript || !lpQuestInstance->m_lpPhaseScript )
	{
		 return ;
	}

	CHAR strTemp[ MAX_PATH ] ;

	TriggerNode* lpTriggerNode = lpQuestInstance->m_lpPhaseScript->m_lstTrigger[0];
	if ( NULL == lpTriggerNode )
	{
		return ;
	}

	// edith 2008.01.25 (퀘스트정보를 화면에 출력하지 못한다.)
	if ( lpTriggerNode->m_dwTriggerID & Creature::NPC_BIT )
	{
		return;
	}
	else
	{
		// 먹은 아이템 개수가 0개이면 리턴이다.
		if(lpTriggerNode->m_dwMaxCount - lpQuestInstance->m_lstTriggerCount[0] == 0)
			return;

		// edith 2008.01.25 트리거 PICK 일때 아이템 설명을 표시해줘야한다.
		// lpTriggerNode->m_dwTriggerID 아이템 인덱스 번호가 된다.
		if(lpTriggerNode->m_dwTriggerKind == TRIGGER_PICK)
		{
			const Item::ItemInfo *lpItemInfo = g_ItemMgr.GetItemInfo(lpTriggerNode->m_dwTriggerID);
			if ( lpItemInfo )
			{
				sprintf(strTemp, CRYLStringTable::m_strString[4264], const_cast<LPSTR>(lpItemInfo->m_SpriteData.m_szName), lpTriggerNode->m_dwMaxCount - lpQuestInstance->m_lstTriggerCount[0],	lpTriggerNode->m_dwMaxCount);
				CRYLChattingDlg::Instance()->AddMessage( strTemp, CRYLNetworkData::Instance()->m_dwMyChrID, 140);
			}
		}
		else if(lpTriggerNode->m_dwTriggerKind == TRIGGER_KILL)
		{
			const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( lpTriggerNode->m_dwTriggerID ) ;
			if ( lpMonster )
			{
				sprintf(strTemp, CRYLStringTable::m_strString[4265], const_cast<LPSTR>(lpMonster->m_MonsterInfo.m_strName), lpTriggerNode->m_dwMaxCount - lpQuestInstance->m_lstTriggerCount[0],	lpTriggerNode->m_dwMaxCount);
				CRYLChattingDlg::Instance()->AddMessage( strTemp, CRYLNetworkData::Instance()->m_dwMyChrID, 140 );
			}
		}
	}
}
