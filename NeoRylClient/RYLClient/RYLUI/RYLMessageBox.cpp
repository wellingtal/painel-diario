// GUIMessageBox.cpp: implementation of the CRYLMessageBox class.
//
//////////////////////////////////////////////////////////////////////
#include <winsock2.h>
#include "RYLClientMain.h"

#include "../GUITextEdit.h"
#include "IME/GMIMEPACK.h"

#include "NPC/NPCList.h"

#include "GMImageDib.h"
#include "GMUIScreen.h"

#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"
#include "RYLEdit.h"
#include "RYLListBox.h"
#include "RYLButton.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLStringTable.h"

#include "RYLRaceBase.h"
#include "RYLGameData.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLChattingDlg.h"
#include "RYLStreetStallDlg.h"
#include "RYLCampShopDlg.h"
#include "RYLVaultPasswordDlg.h"

#include "RYLNetworkData.h"

#include <RenderOption.h>
#include <WinInput.h>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRYLMessageBox*		g_pMessageBox = NULL ;
CRYLMessageBox::CRYLMessageBox() : CRYLDialog()
{
	m_bShowEx = TRUE;
	m_lTick = 3000;
	m_lpResultValue = NULL;
	m_lpValue = NULL;
	m_lpstrValue = NULL;

	m_bEnableEnter = TRUE;
	m_bEnableBack = FALSE;

	m_bWordBreak = TRUE;
	m_lMsgPage = 0;

	m_szSupplementData[ 0 ][ 0 ] = '\0' ;
	m_szSupplementData[ 1 ][ 0 ] = '\0' ;
	m_szSupplementData[ 2 ][ 0 ] = '\0' ;
	m_szSupplementData[ 3 ][ 0 ] = '\0' ;
	
	m_pDataListBox	= NULL ;
	m_pExitButton	= NULL ; 
	m_pPrevButton	= NULL ;	 
	m_pNextButton	= NULL ; 
	m_pConfirmButton = NULL ;
	m_pYesButton	= NULL ;
	m_pCancelButton = NULL ;
	m_pNoButton		= NULL ;
	m_pNumberEdit	= NULL ; 
	m_pStringEdit	= NULL ; 
	m_bTimerDisable	= FALSE;

	m_bModal		= TRUE;
	
	m_iHeight		= 0 ;

	m_dwDelayTick = GetTickCount();

	CGMUIObject::SetGMUICtrlType( GMUICtrlMessageBox ) ;
	g_pMessageBox = this ;

	CGMUIScreen::GetInstance()->SetViewDlg( this ) ;

	// 메세지 박스 생성시 팝업 메뉴를 닫아준다.
	CRYLGameData* pGame = CRYLGameData::Instance();
	if ( pGame->m_lpInterface )
	{
		pGame->m_lpInterface->m_bPopupShow = FALSE;
		pGame->m_lpInterface->m_dwPopupSelect = 0xFFFFFFFF;	
	}
}

CRYLMessageBox::~CRYLMessageBox()
{
	m_lpResultValue = NULL ;
	m_lpValue		= NULL ;
	m_lpstrValue	= NULL ;
	Destroy();
}

VOID	CRYLMessageBox::Show( CGMUIObject* Sender ) 
{
	if (m_dwFlag & MB_NUMBER)
	{
		if ( m_pNumberEdit )
        {
			m_pNumberEdit->SetNumeralOnly( TRUE );
            m_pNumberEdit->SetIsMoneyNumber( TRUE );
        }
	}
}

VOID	CRYLMessageBox::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLMessageBox::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CRYLMessageBox::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pMessageBox )
		return ;

	switch( Sender->GetInstance() )
	{
	case GUIMESSAGEBOX::EXIT :
		{
			if ( CRYLGameData::Instance()->m_bMovieVersion )
			{
				CRYLGameData* pGame = CRYLGameData::Instance();
				pGame->m_bMovieVersion = false;
				CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				pScene->MovieMode( TRUE ) ;

				if ( pGame->m_bQuestComplete )
				{
					pGame->RenderQuestCompleteEffect() ;
				}
			}
		}
		break ;
	case GUIMESSAGEBOX::CONFIRM :
	case GUIMESSAGEBOX::YES	:
		break ;
	case GUIMESSAGEBOX::CANCEL :
	case GUIMESSAGEBOX::NO :
		break ;
	
	case GUIMESSAGEBOX::PREV :
		break ;
	case GUIMESSAGEBOX::NEXT :
		break ;
	}
}

VOID	CRYLMessageBox::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) 
{
	if ( !g_pMessageBox )
		return ;

	switch( Sender->GetInstance() )
	{
	case GUIMESSAGEBOX::NUMBER :
		break ;
	case GUIMESSAGEBOX::STRING :
		break ;
	}
}

void CRYLMessageBox::Create(const char *strMessage, unsigned long dwFlag, float posX, float posY)
{
	m_bModal		= TRUE;
	m_dwFlag = dwFlag;

	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	if (m_dwFlag & MB_EXIT)
	{
		m_iHeight = 24 ;		
	}

	if (m_dwFlag & MB_PREV)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_NEXT)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_CONFIRM)
	{
		m_iHeight = 24 ;
	} 
	else if (m_dwFlag & MB_YES)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_CANCEL)
	{
		m_iHeight = 24 ;
	} 
	else if (m_dwFlag & MB_NO)
	{
		m_iHeight = 24 ;
	}
	
	if (m_dwFlag & MB_NUMBER)
	{
		if ( m_pNumberEdit )
        {
			m_pNumberEdit->SetNumeralOnly( TRUE );
            m_pNumberEdit->SetIsMoneyNumber( TRUE );
        }
	}

	if ( g_MessageBoxManager.m_lstMessageBox.size() != 0 )
	{
		g_MessageBoxManager.DestroyList() ;
	}

	SetText(strMessage);
	CreateDialogImage() ;
	CreateControl( dwFlag ) ;

	g_MessageBoxManager.AddMessageBox(this);
}

void CRYLMessageBox::Create( char* pszYes, char* pszNo, const char *strMessage, unsigned long dwFlag, float posX, float posY ) 
{
	m_bModal		= TRUE;
	m_dwFlag = dwFlag;

	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	if (m_dwFlag & MB_EXIT)
	{
		m_iHeight = 24 ;		
	}

	if (m_dwFlag & MB_PREV)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_NEXT)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_CONFIRM)
	{
		m_iHeight = 24 ;
	} 
	else if (m_dwFlag & MB_YES)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_CANCEL)
	{
		m_iHeight = 24 ;
	} 
	else if (m_dwFlag & MB_NO)
	{
		m_iHeight = 24 ;
	}
	
	if (m_dwFlag & MB_NUMBER)
	{
		if ( m_pNumberEdit )
        {
			m_pNumberEdit->SetNumeralOnly( TRUE );
            m_pNumberEdit->SetIsMoneyNumber( TRUE );
        }
	}

	if ( g_MessageBoxManager.m_lstMessageBox.size() != 0 )
	{
		g_MessageBoxManager.DestroyList() ;
	}

	SetText(strMessage);
	CreateDialogImage() ;
	CreateControl( dwFlag, pszYes, pszNo ) ;

	g_MessageBoxManager.AddMessageBox(this);
}

void CRYLMessageBox::Create(unsigned long dwFlag, float posX, float posY)
{
	m_bModal		= TRUE;
	m_dwFlag = dwFlag;

	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	if (m_dwFlag & MB_EXIT)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_PREV)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_NEXT)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_CONFIRM)
	{
		m_iHeight = 24 ;
	} 
	else if (m_dwFlag & MB_YES)
	{
		m_iHeight = 24 ;
	}
	if (m_dwFlag & MB_CANCEL)
	{
		m_iHeight = 24 ;
	} 
	else if (m_dwFlag & MB_NO)
	{
		m_iHeight = 24 ;
	}
	
	if (m_dwFlag & MB_NUMBER)
	{
		if ( m_pNumberEdit )
        {
			m_pNumberEdit->SetNumeralOnly( TRUE );
            m_pNumberEdit->SetIsMoneyNumber( TRUE );
        }
	}

	if ( g_MessageBoxManager.m_lstMessageBox.size() != 0 )
	{
		g_MessageBoxManager.DestroyList() ;
	}

	if (m_lstWord.size())
	{
		SetText(m_lstWord[m_lMsgPage]);
	}
	CreateDialogImage() ;
	CreateControl( dwFlag ) ;

	g_MessageBoxManager.AddMessageBox(this);
}

void CRYLMessageBox::CreateEx( const char *strMessage, 
							   const char *szTitle1,
							   const char *szData1,
							   const char *szTitle2,
							   const char *szData2,
							   unsigned long dwFlag, 
							   float posX, float posY )
{
	m_bModal		= TRUE;
	m_dwFlag = dwFlag;

	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	if (m_dwFlag & MB_EXIT)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_PREV)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_NEXT)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_CONFIRM)
	{
		m_iHeight = 24 ;
	} 
	else if (m_dwFlag & MB_YES)
	{
		m_iHeight = 24 ;
	}

	if (m_dwFlag & MB_CANCEL)
	{
		m_iHeight = 24 ;
	} 
	else if (m_dwFlag & MB_NO)
	{
		m_iHeight = 24 ;
	}
	
	if (m_dwFlag & MB_NUMBER)
	{
		if ( m_pNumberEdit )
        {
			m_pNumberEdit->SetNumeralOnly( TRUE );
            m_pNumberEdit->SetIsMoneyNumber( TRUE );
        }
	}

	if ( g_MessageBoxManager.m_lstMessageBox.size() != 0 )
	{
		g_MessageBoxManager.DestroyList() ;
	}

	if ( szTitle1 )
	{
		strcpy( m_szSupplementData[ 0 ], szTitle1 ) ;
		strcpy( m_szSupplementData[ 1 ], szData1 ) ;
	}

	if ( szTitle2 )
	{
		strcpy( m_szSupplementData[ 2 ], szTitle2 ) ;
		strcpy( m_szSupplementData[ 3 ], szData2 ) ;
	}

	SetText(strMessage);
	CreateDialogImage() ;
	CreateControl( dwFlag ) ;

	g_MessageBoxManager.AddMessageBox(this);
}

BOOL CRYLMessageBox::ShowDialog(unsigned long dwChrID, unsigned long dwPage)
{
	NPCNode *lpNPCNode;
	lpNPCNode = g_NPCList.GetNPCNode(dwChrID);
	if (lpNPCNode)
	{
		LPDialogNode lpDialog = lpNPCNode->GetScript(dwPage);

		if (lpDialog)
		{
			Create( lpDialog->m_strWord, MB_CONFIRM | MB_EXIT ) ;

			return TRUE;
		}
	}

	return FALSE;
}

void CRYLMessageBox::Destroy()
{
	SetVisible( FALSE ) ;

	vector<LPWORDNode>::iterator it;
	vector<char *>::iterator itchar;
	LPWORDNode lpWord;
	char *lpMessage;
	for (it = m_lstMessage.begin(); it != m_lstMessage.end();)
	{
		lpWord = (*it);
		it = m_lstMessage.erase(it);
		for (itchar = lpWord->m_lstMessage.begin(); itchar != lpWord->m_lstMessage.end();)
		{
			lpMessage = (*itchar);
			itchar = lpWord->m_lstMessage.erase(itchar);

			delete[] lpMessage;
		}
		lpWord->m_lstMessage.clear();

		delete lpWord;
	}
	m_lstMessage.clear();

	for (itchar = m_lstWord.begin(); itchar != m_lstWord.end();)
	{
		lpMessage = (*itchar);
		itchar = m_lstWord.erase(itchar);
		delete[] lpMessage;
	}
	m_lstWord.clear();

	if (m_lpResultValue && *m_lpResultValue == 0) 
	{
		*m_lpResultValue = MB_EXIT;
	}

	if (m_lpValue && *m_lpValue == 0)
	{
		if (strcmp( m_pNumberEdit->GetText(), ""))
		{
			__int64 tmpValue = _atoi64( m_pNumberEdit->GetText() ) ;
			if ( tmpValue > ULONG_MAX )
			{
				*m_lpValue = 0;
			}
			else
			{
				*m_lpValue = static_cast<unsigned long>( tmpValue ) ;
			}
		} 
		else
		{
			*m_lpValue = 0;
		}
	}

	RestoreFocus();

	if ( m_pDataListBox )
	{
		m_pDataListBox->ClearItem() ;
	}

	CGMTexture* pTexture = DetachWindowTexture();
	GM_RELEASE( pTexture );

	GM_DELETE( m_pDataListBox ) ;
	GM_DELETE( m_pExitButton ) ;
	GM_DELETE( m_pPrevButton ) ;
	GM_DELETE( m_pNextButton ) ;
	GM_DELETE( m_pConfirmButton ) ;
	GM_DELETE( m_pYesButton ) ;
	GM_DELETE( m_pCancelButton ) ;
	GM_DELETE( m_pNoButton ) ;
	GM_DELETE( m_pNumberEdit ) ;
	GM_DELETE( m_pStringEdit ) ;

	g_MessageBoxManager.DeleteMessageBox(this);
}

void CRYLMessageBox::SetText(const char *strMessage)
{
	vector<LPWORDNode>::iterator it;
	vector<char *>::iterator itchar;
	LPWORDNode lpWord;
	char* lpMessage = NULL;

	for (it = m_lstMessage.begin(); it != m_lstMessage.end();)
	{
		lpWord = (*it);
		it = m_lstMessage.erase(it);
		for (itchar = lpWord->m_lstMessage.begin(); itchar != lpWord->m_lstMessage.end();)
		{
			lpMessage = (*itchar);
			itchar = lpWord->m_lstMessage.erase(itchar);

			delete[] lpMessage;
		}
		lpWord->m_lstMessage.clear();

		delete lpWord;
	}
	m_lstMessage.clear();

//	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::JAPAN )
	if( g_ClientMain.m_iLanguage == 2)
	{
		WCHAR*	strWideString = NULL;
		WCHAR	seps[] = L"\\" ;
		WCHAR*	token;
		char	strBreak[1024];
		char*	strMessageTemp = NULL ;

		if (m_bWordBreak)
		{
			char* strWBMessage = CRYLStringTable::MakeWordBreak(strMessage, 230);

			int nLen = MultiByteToWideChar( CP_ACP, 0, strWBMessage, lstrlen(strWBMessage), NULL, NULL);
			strWideString = SysAllocStringLen(NULL, nLen);
			MultiByteToWideChar( CP_ACP, 0, strWBMessage, strlen(strWBMessage), strWideString, nLen ) ;
		}
		else
		{
			int nLen = MultiByteToWideChar( CP_ACP, 0, strMessage, lstrlen(strMessage), NULL, NULL);
			strWideString = SysAllocStringLen(NULL, nLen);
			MultiByteToWideChar( CP_ACP, 0, strMessage, strlen(strMessage), strWideString, nLen ) ;
		}

		token = wcstok( strWideString, seps ) ;

		while( token != NULL )
		{
			int nLen = wcslen( token ) ;
			{
				lpWord					= new WORDNode;
				lpWord->m_dwNum			= 1;			
				lpWord->m_bFirstLink	= FALSE;		
				
				strMessageTemp			= new char[ nLen * 2 + 1 ] ;
				WideCharToMultiByte(CP_ACP, 0, token, nLen + 1, strBreak, 1024, NULL, NULL);
				
				memcpy( strMessageTemp, strBreak, nLen * 2 + 1 ) ;
								
				lpWord->m_lstMessage.push_back(strMessageTemp);
			}

			m_lstMessage.push_back(lpWord);

			token = wcstok(NULL, seps);
		}

		SysFreeString(strWideString);
	}
	else
	{
		char *strMessageTemp, *strMessageTemp2, *token, seps[] = "\\";
		strMessageTemp = new char[strlen(strMessage) + 100];

		if (m_bWordBreak)
		{
			strcpy(strMessageTemp, CRYLStringTable::MakeWordBreak(strMessage, 230));
		}
		else
		{
			strcpy(strMessageTemp, strMessage);
		}
		token = strtok(strMessageTemp, seps);

		while(token != NULL)
		{
			{
				lpWord = new WORDNode;
				lpWord->m_dwNum = 1;				
				lpWord->m_bFirstLink = FALSE;		
				strMessageTemp2 = new char[strlen(token) + 100];
				strcpy(strMessageTemp2, token);
				lpWord->m_lstMessage.push_back(strMessageTemp2);
			}

			m_lstMessage.push_back(lpWord);

			token = strtok(NULL, seps);
		}

		delete[] strMessageTemp;
	}

}

// x, y는 메시지 박스의 left top이 아닌 정가운데점
void CRYLMessageBox::SetPosition(float fPosX, float fPosY)					
{
}

void CRYLMessageBox::SetTimerDisable(BOOL bChk)
{
	m_bTimerDisable = bChk;
}

BOOL CRYLMessageBox::UpdateEx(float fTick, BOOL &bClick, BOOL bAble)
{
	m_bAble = bAble ;
/*		
	// edith 2009.01.09 추가 : 아이템을 필드에 버릴때 확인메시지에서 바로 OK,CANCEL눌리는것 방지하기 위해서 추가. 
						삭제 : 메시지 박스가 뜨고 1초내에 Enter을 입력하면 메시지 박스가 처리되는게 아니라
							   채팅창으로 포커스가 가게됨. (이게 문제로 제기되서 삭제됨)
	if(abs((int)GetTickCount()-(int)m_dwDelayTick) < 1000)
	{
		return TRUE;
	}
*/
	if ( ( m_dwFlag & MB_EXIT ) )
	{
		if ( bAble )
		{
			if ( m_pExitButton->GetIsClick() )
			{
				bClick = TRUE ;

				if ( m_lpResultValue )
					*m_lpResultValue = MB_EXIT ;

				return FALSE ;
			}
		}
	} 
	else if(!m_bTimerDisable)
	{
		m_lTick -= fTick ;

		if ( m_lpResultValue )
			*m_lpResultValue = MB_EXIT ;
		
		if ( m_lTick <= 0 )  
			return FALSE ;
	}

	if ( bAble )
	{
		if ( ( m_dwFlag & MB_NO ) || ( m_dwFlag & MB_CANCEL ) )
		{
			if ( m_pNoButton )
			{
				if ( ( m_bEnableBack /*&& GetAsyncKeyState(VK_BACK)*/ ) || m_pNoButton->GetIsClick() )//hayzohar
//				if ( m_pNoButton->GetIsClick() )
				{
					if ( m_lpResultValue) 
					{ 
						*m_lpResultValue = MB_NO | MB_CANCEL ; 
						m_lpResultValue = NULL ; 
					}

					if ( m_lpValue ) 
					{ 
						*m_lpValue = 0 ; 
						m_lpValue = NULL ; 
					}

					if ( m_lpstrValue ) 
					{ 
						strcpy( m_lpstrValue, "" ) ; 
						m_lpstrValue = NULL ; 
					}

					bClick = TRUE ;
					
					if ( m_dwFlag & MB_NUMBER )
					{
						m_pNumberEdit->SetFocus() ;
					}
					return FALSE ;
				}
			}

			if ( m_pCancelButton )
			{
				if ( ( m_bEnableBack /*&& GetAsyncKeyState(VK_BACK)*/) || m_pCancelButton->GetIsClick() )//hayzohar
//				if ( m_pCancelButton->GetIsClick() )
				{
					if ( m_lpResultValue ) 
					{ 
						*m_lpResultValue = MB_NO | MB_CANCEL ; 
						m_lpResultValue = NULL ;  
					}

					if ( m_lpValue )
					{ 
						*m_lpValue = 0 ; 
						m_lpValue = NULL ; 
					}

					if ( m_lpstrValue ) 
					{ 
						strcpy( m_lpstrValue, "" ) ;
						m_lpstrValue = NULL ; 
					}

					bClick = TRUE ;
					
					if ( m_dwFlag & MB_NUMBER )
					{
						m_pNumberEdit->SetFocus() ;
					}
					return FALSE ;
				}
			}
		}

		if ( ( m_dwFlag & MB_YES ) || ( m_dwFlag & MB_CONFIRM ) || ( m_dwFlag & MB_ARRAYCONFIRM ) )
		{
			if ( m_pConfirmButton )
			{
				if ( ( m_bEnableEnter && GetAsyncKeyState(VK_RETURN)) || ( m_pConfirmButton->GetIsClick() && g_DeviceInput.GetIsLeftMouseUp() ) )// hayzohar was || in the first &&
				{
					if (m_lpResultValue) { *m_lpResultValue = MB_YES | MB_CONFIRM; m_lpResultValue = NULL; }

					if (m_lpValue)
					{
						if ( strcmp( m_pNumberEdit->GetText(), "" ) )
						{
							__int64 tmpValue = _atoi64( m_pNumberEdit->GetText() ) ;
							if ( tmpValue > ULONG_MAX )
							{
								*m_lpValue = 0;
							}
							else
							{
								*m_lpValue = static_cast<unsigned long>( tmpValue ) ;
							}
						}
						else
						{
							*m_lpValue = 0;
						}

						m_lpValue = NULL;
					}
					if (m_lpstrValue)
					{
						if ( strcmp( m_pStringEdit->GetText(), ""))
							strcpy(m_lpstrValue, m_pStringEdit->GetText() ) ;
						else
							strcpy(m_lpstrValue, "" ) ;
						m_lpstrValue = NULL ;
					}

					bClick = TRUE ;
					if ( m_dwFlag & MB_NUMBER )  
					{
						m_pNumberEdit->SetFocus() ;
					}
					if (m_dwFlag & MB_STRING ) 
					{
						m_pStringEdit->SetFocus() ;
					}
					if ( m_lstWord.size() > static_cast<size_t>(m_lMsgPage + 1) )
					{
						SetText(m_lstWord[++m_lMsgPage]);
						{
							RestoreFocus();

							if ( m_pDataListBox )
							{
								m_pDataListBox->ClearItem() ;
								DetachGMUIControl( m_pDataListBox );
								GM_DELETE( m_pDataListBox ) ;
							}
							if ( m_pExitButton )
							{
								DetachGMUIControl( m_pExitButton );
								GM_DELETE( m_pExitButton ) ;
							}
							if ( m_pPrevButton )
							{
								DetachGMUIControl( m_pPrevButton );
								GM_DELETE( m_pPrevButton ) ;
							}
							if ( m_pNextButton )
							{
								DetachGMUIControl( m_pNextButton );
								GM_DELETE( m_pNextButton ) ;
							}
							if ( m_pConfirmButton )
							{
								DetachGMUIControl( m_pConfirmButton );
								GM_DELETE( m_pConfirmButton ) ;
							}
							if ( m_pYesButton )
							{
								DetachGMUIControl( m_pYesButton );
								GM_DELETE( m_pYesButton ) ;
							}
							if (m_pCancelButton )
							{
								DetachGMUIControl( m_pCancelButton );
								GM_DELETE( m_pCancelButton ) ;
							}

							if ( m_pNoButton )
							{
								DetachGMUIControl( m_pNoButton );
								GM_DELETE( m_pNoButton ) ;
							}

							if ( m_pNumberEdit )
							{
								DetachGMUIControl( m_pNumberEdit );
								GM_DELETE( m_pNumberEdit ) ;
							}

							if ( m_pStringEdit )
							{
								DetachGMUIControl( m_pStringEdit );
								GM_DELETE( m_pStringEdit ) ;
							}
							CGMTexture* pTexture = DetachWindowTexture();
							GM_RELEASE( pTexture );
						}

						CreateDialogImage() ;
						CreateControl( m_dwFlag ) ;

						return TRUE;
					}
					else if ( !m_lstWord.empty() && ( m_dwFlag & MB_THEATER ) )
					{
						CRYLGameData* pGame = CRYLGameData::Instance() ;
						pGame->m_bMovieVersion = false;
						CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						pScene->MovieMode( TRUE ) ;

						if ( pGame->m_bQuestComplete )
						{
							pGame->RenderQuestCompleteEffect() ;
						}
					}
					return FALSE;
				}
			}
			else if ( m_pYesButton )
			{
				if ( ( m_bEnableEnter && GetAsyncKeyState(VK_RETURN)) || m_pYesButton->GetIsClick() )//was || hayzohar
				{
					if ( m_lpResultValue ) 
					{ 
						*m_lpResultValue = MB_YES | MB_CONFIRM ;
						m_lpResultValue = NULL ;
					}

					if ( m_lpValue )
					{
						if ( strcmp( m_pNumberEdit->GetText(), "" ) )
						{
							__int64 tmpValue = _atoi64( m_pNumberEdit->GetText() ) ;
							if ( tmpValue > ULONG_MAX )
							{
								*m_lpValue = 0;
							}
							else
							{
								*m_lpValue = static_cast<unsigned long>( tmpValue ) ;
							}
						}
						else
						{
							*m_lpValue = 0;
						}

						m_lpValue = NULL ;
					}

					if ( m_lpstrValue )
					{
						if ( strcmp( m_pStringEdit->GetText(), "" ) )
							strcpy(m_lpstrValue, m_pStringEdit->GetText() ) ;
						else
							strcpy(m_lpstrValue, "" ) ;
						m_lpstrValue = NULL ;
					}

					bClick = TRUE;
					
					return FALSE ;
				}
			}
		}
	}

	return TRUE;
}

void CRYLMessageBox::AddWord(char *strWord)
{
	char *strMessage = new char[strlen(strWord) + 10];
	strcpy(strMessage, strWord);
	m_lstWord.push_back(strMessage);
}

VOID		CRYLMessageBox::CreateDialogImage()
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	INT		iHeight = m_lstMessage.size() * 15;
	INT		iCount  = 0 ;	
	if ( iHeight < 256 )
	{
		if ( m_dwFlag == MB_NORMAL )
			iCount = ( m_lstMessage.size() * 3 ) ;
		else if ( m_dwFlag & MB_STRINGEX )
			iCount = ( m_lstMessage.size() * 3 ) + 13 ;
		else
			iCount = ( m_lstMessage.size() * 3 ) + 8 ;
	}
	else
	{
		iCount = 58 ;
	}

	CRYLSpriteList::Instance()->CreateGMImage( 256, ( iCount * 5 ) + 30 , &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ; // was RYL_COMPONENT01_256x256
	int i = 0;
	for (  ; i < iCount ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 15 + (i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;  // was RYL_COMPONENT01_256x256
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 15 + (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;  // was RYL_COMPONENT01_256x256
	
	if ( m_dwFlag & MB_STRINGEX )
	{
		pImageDIB->MakeFrameRect( 16, ( ( iCount - 5 ) * 5 ) - 8, 224, 17,  0xff959595, 0xc0000000,  1 ) ;
	}
	else if ( ( m_dwFlag & MB_STRING ) || ( m_dwFlag & MB_NUMBER ) )
	{
		pImageDIB->MakeFrameRect( 26, ( iCount * 5 ) - 8, 83, 17,  0xff959595, 0xc0000000,  1 ) ;
	}

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( iCount * 5 + 10 + m_iHeight + 15) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_NONE ) ;

	SetVisible( TRUE ) ;
	SetShowHint( FALSE ) ;
}

VOID		CRYLMessageBox::CreateControl( DWORD dwFlag, char* pszYesName, char* pszNoName ) 
{
	m_bModal		= TRUE;
	m_dwFlag = dwFlag;
	INT iHeight = m_lstMessage.size() * 15;

	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;  // was RYL_COMPONENT01_256x256

	if ( iHeight > 256 )
	{
		RECT rt ;
		
		m_pDataListBox = new CRYLListBox( this, TRUE ) ;
		m_pDataListBox->SetLeft( 7 ) ;
		m_pDataListBox->SetTop( 17 ) ;
		m_pDataListBox->SetWidth( 245 ) ;
		m_pDataListBox->SetHeight( 256 ) ;


		SetRect( &rt, 198, 186, 212, 200 ) ;
		m_pDataListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
		SetRect( &rt, 226, 186, 240, 200 ) ;
		m_pDataListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
		SetRect( &rt, 212, 186, 226, 200 ) ;
		m_pDataListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
		SetRect( &rt, 198, 186, 212, 200 ) ;
		m_pDataListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

		SetRect( &rt, 235, 164, 243, 184 ) ;
 		m_pDataListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
		SetRect( &rt, 235, 164, 243, 184 ) ;					 
		m_pDataListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
		SetRect( &rt, 235, 164, 243, 184 ) ;					 
		m_pDataListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
		SetRect( &rt, 235, 164, 243, 184 ) ;					 
		m_pDataListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

		SetRect( &rt, 198, 200, 212, 214 ) ;
		m_pDataListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
		SetRect( &rt, 226, 200, 240, 214 ) ;					  
		m_pDataListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
		SetRect( &rt, 212, 200, 226, 214 ) ;					  
		m_pDataListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
		SetRect( &rt, 198, 200, 212, 214 ) ;					  
		m_pDataListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;
	}
	else
	{
		m_pDataListBox = new CRYLListBox( this, FALSE ) ;
		m_pDataListBox->SetLeft( 7 ) ;
		m_pDataListBox->SetTop( 17 ) ;
		m_pDataListBox->SetWidth( 245 ) ;
		m_pDataListBox->SetHeight( m_lstMessage.size() * 15 + 5) ;
	}
	m_pDataListBox->SetItemHeight( 15 ) ;
	m_pDataListBox->SetColor( 0xffffffff ) ;
	
	vector<LPWORDNode>::iterator		it;
	vector<char *>::iterator			itchar;

	for ( it = m_lstMessage.begin(); it != m_lstMessage.end(); ++ it)
	{
		for (itchar = ( *it )->m_lstMessage.begin(); itchar != ( *it )->m_lstMessage.end(); ++ itchar )
		{
			m_pDataListBox->AddItem( (*itchar), 0xffffffff, 0xffffffff, 0xffffffff ) ;
		}
	}
	AttachGMUIControl( m_pDataListBox ) ;


	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	if (m_dwFlag & MB_EXIT)
	{
		RECT rt ;
		m_pExitButton = new CRYLButton( this ) ;
		m_pExitButton->SetLeft( 243 ) ;
		m_pExitButton->SetTop( 3 ) ;
		m_pExitButton->OnClickEvent = DlgClick ;
		m_pExitButton->SetInstance( GUIMESSAGEBOX::EXIT ) ;
		//SetRect( &rt, 202, 0, 213, 9 ) ;//hz guigui
		SetRect( &rt, 4, 347, 15, 356 ) ;
		m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		//SetRect( &rt, 202, 18, 213, 27 ) ;
		SetRect( &rt, 4, 267, 15, 276 ) ;
		m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		//SetRect( &rt, 202, 9, 213, 18 ) ;
		SetRect( &rt, 4, 257, 15, 266 ) ;
		m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		//SetRect( &rt, 202, 9, 213, 18 ) ;
		SetRect( &rt, 4, 257, 15, 266 ) ;
		m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pExitButton ) ;
	}

	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	if (m_dwFlag & MB_PREV)
	{
		m_pPrevButton = new CRYLButton( this ) ;
		m_pPrevButton->SetCaption( "<-" ) ;
		m_pPrevButton->SetFontColor( 0xffffffff) ;
		m_pPrevButton->SetColor( 0xffffffff) ;
		m_pPrevButton->OnClickEvent = DlgClick ;
		m_pPrevButton->SetInstance( GUIMESSAGEBOX::PREV ) ;
		m_pPrevButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect(3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pPrevButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pPrevButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pPrevButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pPrevButton ) ;
	}

	if (m_dwFlag & MB_NEXT)
	{
		m_pNextButton = new CRYLButton( this ) ;
		m_pNextButton->SetCaption( "->" ) ;
		m_pNextButton->SetFontColor( 0xffffffff) ;
		m_pNextButton->SetColor( 0xffffffff) ;
		m_pNextButton->OnClickEvent = DlgClick ;
		m_pNextButton->SetInstance( GUIMESSAGEBOX::NEXT ) ;
		m_pNextButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pNextButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pNextButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pNextButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pNextButton ) ;
	}

	if (m_dwFlag & MB_CONFIRM)
	{
		m_pConfirmButton = new CRYLButton( this ) ;
		m_pConfirmButton->SetCaption( CRYLStringTable::m_strString[ 125 ] ) ;
		m_pConfirmButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
		m_pConfirmButton->SetFontColor( 0xffffffff) ;
		m_pConfirmButton->SetColor( 0xffffffff) ;
		m_pConfirmButton->OnClickEvent = DlgClick ;
		m_pConfirmButton->SetInstance( GUIMESSAGEBOX::CONFIRM ) ;
		m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect(3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pConfirmButton ) ;
	} 
	else if (m_dwFlag & MB_YES)
	{
		m_pYesButton = new CRYLButton( this ) ;
		int iBtnSize = 3 ;

		if ( pszYesName )
		{
			m_pYesButton->SetCaption( pszYesName ) ;
			iBtnSize = 4 ;
		}
		else
			m_pYesButton->SetCaption( CRYLStringTable::m_strString[ 249 ] ) ;

		m_pYesButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
		m_pYesButton->SetFontColor( 0xffffffff) ;
		m_pYesButton->SetColor( 0xffffffff) ;
		m_pYesButton->OnClickEvent = DlgClick ;
		m_pYesButton->SetInstance( GUIMESSAGEBOX::YES ) ;
		m_pYesButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( iBtnSize ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pYesButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( iBtnSize ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pYesButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( iBtnSize ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pYesButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( iBtnSize ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pYesButton ) ;
	}

	if (m_dwFlag & MB_CANCEL)
	{
		m_pCancelButton = new CRYLButton( this ) ;
		m_pCancelButton->SetLeft( 194 ) ;
		m_pCancelButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
		m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 126 ] ) ;
		m_pCancelButton->SetFontColor( 0xffffffff) ;
		m_pCancelButton->SetColor( 0xffffffff) ;
		m_pCancelButton->OnClickEvent = DlgClick ;
		m_pCancelButton->SetInstance( GUIMESSAGEBOX::CANCEL ) ;
		m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pCancelButton ) ;
	} 
	else if (m_dwFlag & MB_NO)
	{
		m_pNoButton = new CRYLButton( this ) ;
		m_pNoButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
		
		int iBtnSize = 3 ;
		if ( pszNoName )
		{
			m_pNoButton->SetLeft( 172  ) ;
			m_pNoButton->SetCaption( pszNoName ) ;
			iBtnSize = 4 ;
		}
		else 
		{
			m_pNoButton->SetLeft( 194  ) ;
			m_pNoButton->SetCaption( CRYLStringTable::m_strString[ 250 ] ) ;
		}

		m_pNoButton->SetFontColor( 0xffffffff) ;
		m_pNoButton->SetColor( 0xffffffff) ;
		m_pNoButton->OnClickEvent = DlgClick ;
		m_pNoButton->SetInstance( GUIMESSAGEBOX::NO ) ;
		m_pNoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( iBtnSize ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pNoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( iBtnSize ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pNoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( iBtnSize ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pNoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( iBtnSize ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pNoButton ) ;
	}

	if (m_dwFlag & MB_NUMBER)
	{
		m_pNumberEdit = new CRYLEdit( this ) ;
		m_pNumberEdit->SetWidth( 81 ) ;
		m_pNumberEdit->SetHeight( 15 ) ;
		m_pNumberEdit->SetColor( 0xff000000 ) ;
		m_pNumberEdit->SetFontColor( 0xffffffff ) ;
		m_pNumberEdit->SetInstance( GUIMESSAGEBOX::NUMBER ) ;
		m_pNumberEdit->SetMaxLength( 10 ) ;
		m_pNumberEdit->OnKeyDown = DlgKeyDown; 
		m_pNumberEdit->SetNumeralOnly( TRUE );
        m_pNumberEdit->SetIsMoneyNumber( TRUE );
		m_pNumberEdit->SetFocus();
		AttachGMUIControl( m_pNumberEdit ) ;
	}

	if ( ( m_dwFlag & MB_STRING ) || ( m_dwFlag & MB_STRINGEX ) )
	{
		m_pStringEdit = new CRYLEdit( this ) ;
		m_pStringEdit->SetWidth( 81 ) ;
		m_pStringEdit->SetHeight( 15 ) ;
		m_pStringEdit->SetColor( 0xff000000) ;
		m_pStringEdit->SetFontColor( 0xffffffff ) ;
		m_pStringEdit->SetInstance( GUIMESSAGEBOX::STRING ) ;
		m_pStringEdit->SetMaxLength( 10 ) ;
		m_pStringEdit->OnKeyDown = DlgKeyDown; 
		AttachGMUIControl( m_pStringEdit ) ;
	}

	if ((m_dwFlag & MB_CONFIRM) || (m_dwFlag & MB_YES) || (m_dwFlag & MB_ARRAYCONFIRM))
	{
		if ((m_dwFlag & MB_CANCEL) || (m_dwFlag & MB_NO))
		{
			if ( m_pConfirmButton )
			{
				m_pConfirmButton->SetLeft( 132 ) ;

				if ( m_dwFlag & MB_STRINGEX )
					m_pConfirmButton->SetTop( m_pDataListBox->GetHeight() + 54) ;
				else
					m_pConfirmButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
			}
			
			if ( m_pYesButton )
			{
				if ( pszYesName )
					m_pYesButton->SetLeft( 91 ) ;
				else
					m_pYesButton->SetLeft( 132 ) ;

				if ( m_dwFlag & MB_STRINGEX )
					m_pYesButton->SetTop( m_pDataListBox->GetHeight() + 54) ;
				else
					m_pYesButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
			}

			if ( m_pCancelButton )
			{
				m_pCancelButton->SetLeft( 194 ) ;

				if ( m_dwFlag & MB_STRINGEX )
					m_pCancelButton->SetTop( m_pDataListBox->GetHeight() + 54) ;
				else
					m_pCancelButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
			}
		} 
		else
		{
			if ( m_pConfirmButton )
			{
				m_pConfirmButton->SetLeft( 194 ) ;

				if ( m_dwFlag & MB_STRINGEX )
					m_pConfirmButton->SetTop( m_pDataListBox->GetHeight() + 54) ;
				else
					m_pConfirmButton->SetTop( m_pDataListBox->GetHeight()+ 23 ) ;
			}
			
			if ( m_pYesButton )
			{
				if ( pszYesName )
					m_pYesButton->SetLeft( 172 ) ;
				else
					m_pYesButton->SetLeft( 194 ) ;

				if ( m_dwFlag & MB_STRINGEX )
					m_pYesButton->SetTop( m_pDataListBox->GetHeight() + 54) ;
				else
					m_pYesButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
			}

		}
	}

	if ( m_dwFlag & MB_STRINGEX )
	{
		if ( m_pStringEdit )
		{
			m_pStringEdit->SetLeft( 18 ) ;
			m_pStringEdit->SetTop( m_pDataListBox->GetHeight() + 28 ) ;
			m_pStringEdit->SetFocus() ;
		}
	}
	else if ( ( m_dwFlag & MB_NUMBER ) || ( m_dwFlag & MB_STRING ) || ( m_dwFlag & MB_SUPPLEMENTDATA ) )
	{
		if ( m_pNumberEdit )
		{
			m_pNumberEdit->SetLeft( 27 ) ;
			m_pNumberEdit->SetTop( m_pDataListBox->GetHeight() + 28 ) ;
			m_pNumberEdit->SetNumeralOnly( TRUE );
		}

		if ( m_pStringEdit )
		{
			m_pStringEdit->SetLeft( 27 ) ;
			m_pStringEdit->SetTop( m_pDataListBox->GetHeight() + 28 ) ;
			m_pStringEdit->SetFocus() ;
		}
	}
}

void CRYLMessageBox::RestoreFocus(void)
{
	if ( m_pNumberEdit )
	{
		if ( m_pNumberEdit->GetFocusState() )
		{
			CGMUIScreen::GetInstance()->DeleteGMUIEdit( m_pNumberEdit ) ;
			CGMUIScreen::GetInstance()->SetCurrentEditFocus( NULL ) ;
		}
	}

	if ( m_pStringEdit )
	{
		CGMUIScreen::GetInstance()->DeleteGMUIEdit( m_pStringEdit ) ;
		CGMUIScreen::GetInstance()->SetCurrentEditFocus( NULL ) ;
	}

	CRYLGameScene*		        pScene          = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLStreetStallDlg*         pStreetStallDlg = ( CRYLStreetStallDlg* )pScene->GetStreetStallDlg();
	CRYLCampShopDlg*			pCampShopDlg	= ( CRYLCampShopDlg* )pScene->GetCampShopDlg();
	CRYLVaultPasswordDlg*       pVaultPassDlg   = ( CRYLVaultPasswordDlg* )( pScene->GetVaultPasswordDlg() );
	CRYLVaultCertificationdDlg* pVaultCertiDlg  = ( CRYLVaultCertificationdDlg* )( pScene->GetVaultCertificationdDlg() );

	CRYLEdit*   pTempEdit = NULL;

	if ( pStreetStallDlg && pStreetStallDlg->GetVisible() )
	{
		pTempEdit = pStreetStallDlg->GetChattingEdit();
	}
	else if ( pCampShopDlg && pCampShopDlg->GetVisible() )
	{
		pTempEdit = pCampShopDlg->GetChattingEdit();
	}
	else if ( pVaultPassDlg && pVaultPassDlg->GetVisible() )
	{
		pTempEdit = pVaultPassDlg->m_pCurrentPasswordEdit;
	}
	else if ( pVaultCertiDlg && pVaultCertiDlg->GetVisible() )
	{
		pTempEdit = pVaultCertiDlg->m_pPasswordEdit;
	}

	if ( pTempEdit )
	{
		pTempEdit->SetFocus();
		pTempEdit->SetFocusState( TRUE );
	}
	else
	{
		CRYLChattingDlg* pDlg = CRYLChattingDlg::Instance();
		if (NULL != pDlg)
		{
			pDlg->SetFocusChattingEdit();
		}
	}
}

