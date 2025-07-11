// GUIMessageBox.cpp: implementation of the CGUIMessageBox class.
//
//////////////////////////////////////////////////////////////////////
#include <winsock2.h>
#include "RYLClientMain.h"
#include "GUITextEdit.h"
#include "GUIMessageBox.h"
#include "GUIMessageBoxManager.h"
#include "Creature\NPC\NPCList.h"
#include "RYLChattingDlg.h"
#include "RYLEdit.h"
#include "RYLListBox.h"
#include "RYLButton.h"

#include "GMImageDib.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "GMUIScreen.h"
#include "RYLStringTable.h"

//-- 2004. 1. 29. Zergra From.--//
#include "IME/GMIMEPACK.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLStreetStallDlg.h"
#include "RYLVaultPasswordDlg.h"
//-- Zergra To. --//

#include <RenderOption.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUIMessageBox*		g_pMessageBox = NULL ;
CGUIMessageBox::CGUIMessageBox() : CRYLDialog()
{
	m_bShowEx = TRUE;
	m_lTick = 3000;
	m_lpResultValue = NULL;
	m_lpValue = NULL;
	m_lpstrValue = NULL;
	m_bEnableEnter = TRUE;
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
	
	m_iHeight		= 0 ;

	CGMUIObject::SetGMUICtrlType( GMUICtrlMessageBox ) ;
	g_pMessageBox = this ;

	CGMUIScreen::GetInstance()->SetViewDlg( this ) ;
}

CGUIMessageBox::~CGUIMessageBox()
{
	m_lpResultValue = NULL ;
	m_lpValue		= NULL ;
	m_lpstrValue	= NULL ;
	Destroy();
}

VOID	CGUIMessageBox::Show( CGMUIObject* Sender ) 
{
    //-- 2004. 1. 20. zergra --//
	if(m_dwFlag & MB_NUMBER)
	{
		if( m_pNumberEdit )
        {
			m_pNumberEdit->SetNumeralOnly( TRUE );
            //-- 2004. 1. 30. --//
            m_pNumberEdit->SetIsMoneyNumber( TRUE );
        }
	}
}

VOID	CGUIMessageBox::Hide( CGMUIObject* Sender ) 
{
}

VOID	CGUIMessageBox::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CGUIMessageBox::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pMessageBox )
		return ;

	switch( Sender->GetInstance() )
	{
	case GUIMESSAGEBOX::EXIT :
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

VOID	CGUIMessageBox::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) 
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

void CGUIMessageBox::Create(const char *strMessage, unsigned long dwFlag, float posX, float posY)
{
	m_dwFlag = dwFlag;

	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	if(m_dwFlag & MB_EXIT)
	{
		m_iHeight = 24 ;		
	}

	if(m_dwFlag & MB_PREV)
	{
		m_iHeight = 24 ;
	}

	if(m_dwFlag & MB_NEXT)
	{
		m_iHeight = 24 ;
	}

	if(m_dwFlag & MB_CONFIRM)
	{
		m_iHeight = 24 ;
	} 
	else if(m_dwFlag & MB_YES)
	{
		m_iHeight = 24 ;
	}

	if(m_dwFlag & MB_CANCEL)
	{
		m_iHeight = 24 ;
	} 
	else if(m_dwFlag & MB_NO)
	{
		m_iHeight = 24 ;
	}
	
	//-- 2004. 1. 20. zergra --//
	if(m_dwFlag & MB_NUMBER)
	{
		if( m_pNumberEdit )
        {
			m_pNumberEdit->SetNumeralOnly( TRUE );
            //-- 2004. 1. 30. --//
            m_pNumberEdit->SetIsMoneyNumber( TRUE );
        }
	}

	/*
	if(m_dwFlag & MB_STRING)
	{
	}
	*/

	if ( g_MessageBoxManager.m_lstMessageBox.size() != 0 )
	{
		//CGUIMessageBox* lpMessageBox = new CGUIMessageBox;
		//lpMessageBox->Create( "다른 메시지창이 열려 있습니다." ) ;
		g_MessageBoxManager.DestroyList() ;

//		return ;
	}

	SetText(strMessage);
	CreateDialogImage() ;
	CreateControl( dwFlag ) ;

	g_MessageBoxManager.AddMessageBox(this);
}

void CGUIMessageBox::Create(unsigned long dwFlag, float posX, float posY)
{
	m_dwFlag = dwFlag;

	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	if(m_dwFlag & MB_EXIT)
	{
		m_iHeight = 24 ;
	}

	if(m_dwFlag & MB_PREV)
	{
		m_iHeight = 24 ;
	}

	if(m_dwFlag & MB_NEXT)
	{
		m_iHeight = 24 ;
	}

	if(m_dwFlag & MB_CONFIRM)
	{
		m_iHeight = 24 ;
	} 
	else if(m_dwFlag & MB_YES)
	{
		m_iHeight = 24 ;
	}
	if(m_dwFlag & MB_CANCEL)
	{
		m_iHeight = 24 ;
	} 
	else if(m_dwFlag & MB_NO)
	{
		m_iHeight = 24 ;
	}
	
	//-- 2004. 1. 20. zergra --//
	if(m_dwFlag & MB_NUMBER)
	{
		if( m_pNumberEdit )
        {
			m_pNumberEdit->SetNumeralOnly( TRUE );
            //-- 2004. 1. 30. --//
            m_pNumberEdit->SetIsMoneyNumber( TRUE );
        }
	}

	if ( g_MessageBoxManager.m_lstMessageBox.size() != 0 )
	{
		g_MessageBoxManager.DestroyList() ;
	}

	if(m_lstWord.size())
	{
		SetText(m_lstWord[m_lMsgPage]);
	}
	CreateDialogImage() ;
	CreateControl( dwFlag ) ;

	g_MessageBoxManager.AddMessageBox(this);
}

void CGUIMessageBox::CreateEx( const char *strMessage, 
							   const char *szTitle1,
							   const char *szData1,
							   const char *szTitle2,
							   const char *szData2,
							   unsigned long dwFlag, 
							   float posX, float posY )
{
	m_dwFlag = dwFlag;

	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	if(m_dwFlag & MB_EXIT)
	{
		m_iHeight = 24 ;
	}

	if(m_dwFlag & MB_PREV)
	{
		m_iHeight = 24 ;
	}

	if(m_dwFlag & MB_NEXT)
	{
		m_iHeight = 24 ;
	}

	if(m_dwFlag & MB_CONFIRM)
	{
		m_iHeight = 24 ;
	} 
	else if(m_dwFlag & MB_YES)
	{
		m_iHeight = 24 ;
	}

	if(m_dwFlag & MB_CANCEL)
	{
		m_iHeight = 24 ;
	} 
	else if(m_dwFlag & MB_NO)
	{
		m_iHeight = 24 ;
	}
	
	//-- 2004. 1. 20. zergra --//
	if(m_dwFlag & MB_NUMBER)
	{
		if( m_pNumberEdit )
        {
			m_pNumberEdit->SetNumeralOnly( TRUE );
            //-- 2004. 1. 30. --//
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

BOOL CGUIMessageBox::ShowDialog(unsigned long dwChrID, unsigned long dwPage)
{
	NPCNode *lpNPCNode;
	lpNPCNode = g_NPCList.GetNPCNode(dwChrID);
	if(lpNPCNode)
	{
		LPDialogNode lpDialog = lpNPCNode->GetScript(dwPage);

		if(lpDialog)
		{
			Create( lpDialog->m_strWord, MB_CONFIRM | MB_EXIT ) ;

			return TRUE;
		}
	}

	return FALSE;
}

void CGUIMessageBox::Destroy()
{
	SetVisible( FALSE ) ;

	vector<LPWORDNode>::iterator it;
	vector<char *>::iterator itchar;
	LPWORDNode lpWord;
	char *lpMessage;
	for(it = m_lstMessage.begin(); it != m_lstMessage.end();)
	{
		lpWord = (*it);
		it = m_lstMessage.erase(it);
		for(itchar = lpWord->m_lstMessage.begin(); itchar != lpWord->m_lstMessage.end();)
		{
			lpMessage = (*itchar);
			itchar = lpWord->m_lstMessage.erase(itchar);

			delete[] lpMessage;
		}
		lpWord->m_lstMessage.clear();

		delete lpWord;
	}
	m_lstMessage.clear();

	for(itchar = m_lstWord.begin(); itchar != m_lstWord.end();)
	{
		lpMessage = (*itchar);
		itchar = m_lstWord.erase(itchar);
		delete[] lpMessage;
	}
	m_lstWord.clear();

	if(m_lpResultValue && *m_lpResultValue == 0) *m_lpResultValue = MB_EXIT;
	if(m_lpValue && *m_lpValue == 0)
	{
		if(strcmp( m_pNumberEdit->GetText(), ""))
		{
			*m_lpValue = atoi( m_pNumberEdit->GetText() );
		} else
		{
			*m_lpValue = 0;
		}
	}

	if ( m_pNumberEdit )
	{
		if ( m_pNumberEdit->GetFocusState() )
		{
			CGMUIScreen::GetInstance()->DeleteGMUIEdit( m_pNumberEdit ) ;
			CGMUIScreen::GetInstance()->SetCurrentEditFocus( NULL ) ;

			//-- 2004. 1. 29. Zergra From.--//
            CRYLChattingDlg::Instance()->SetFocusChattingEdit();
            //-- Zergra To. --//
		}
	}


	if ( m_pStringEdit )
	{
		{
			CGMUIScreen::GetInstance()->DeleteGMUIEdit( m_pStringEdit ) ;
			CGMUIScreen::GetInstance()->SetCurrentEditFocus( NULL ) ;

            //-- 2004. 1. 28. Zergra From.--//
            CRYLGameScene*		        pScene          = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
            CRYLStreetStallDlg*         pStreetStallDlg = ( CRYLStreetStallDlg* )pScene->GetStreetStallDlg();
            CRYLVaultPasswordDlg*       pVaultPassDlg   = ( CRYLVaultPasswordDlg* )( pScene->GetVaultPasswordDlg() );
            CRYLVaultCertificationdDlg* pVaultCertiDlg  = ( CRYLVaultCertificationdDlg* )( pScene->GetVaultCertificationdDlg() );

            CRYLEdit*   pTempEdit = NULL;

            if( pStreetStallDlg && pStreetStallDlg->GetVisible() )
            {
                pTempEdit = pStreetStallDlg->m_pChatEdit;
            }
            else if( pVaultPassDlg && pVaultPassDlg->GetVisible() )
            {
                pTempEdit = pVaultPassDlg->m_pCurrentPasswordEdit;
            }
            else if( pVaultCertiDlg && pVaultCertiDlg->GetVisible() )
            {
                pTempEdit = pVaultCertiDlg->m_pPasswordEdit;
            }

            if( pTempEdit )
            {
                pTempEdit->SetFocus();
                pTempEdit->SetFocusState( TRUE );
            }
            else
            {
                CRYLChattingDlg::Instance()->SetFocusChattingEdit();
            }
            //-- Zergra To. --//
        }
	}

	if ( m_pDataListBox )
		m_pDataListBox->ClearItem() ;

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

void CGUIMessageBox::SetText(const char *strMessage)
{
	vector<LPWORDNode>::iterator it;
	vector<char *>::iterator itchar;
	LPWORDNode lpWord;
	char *lpMessage;
	for(it = m_lstMessage.begin(); it != m_lstMessage.end();)
	{
		lpWord = (*it);
		it = m_lstMessage.erase(it);
		for(itchar = lpWord->m_lstMessage.begin(); itchar != lpWord->m_lstMessage.end();)
		{
			lpMessage = (*itchar);
			itchar = lpWord->m_lstMessage.erase(itchar);

			delete[] lpMessage;
		}
		lpWord->m_lstMessage.clear();

		delete lpWord;
	}
	m_lstMessage.clear();

	char *strMessageTemp, *strMessageTemp2, *token, seps[] = "\\";
	strMessageTemp = new char[strlen(strMessage) + 100];

	if(m_bWordBreak)
	{
		strcpy(strMessageTemp, CRYLStringTable::MakeWordBreak(strMessage, 230));
	} else
	{
		strcpy(strMessageTemp, strMessage);
	}
	token = strtok(strMessageTemp, seps);

	while(token != NULL)
	{
		{
			lpWord = new WORDNode;
			lpWord->m_dwNum = 1;				// 임시
			lpWord->m_bFirstLink = FALSE;		// 임시
			strMessageTemp2 = new char[strlen(token) + 100];
			strcpy(strMessageTemp2, token);
			lpWord->m_lstMessage.push_back(strMessageTemp2);
		}

		m_lstMessage.push_back(lpWord);

		token = strtok(NULL, seps);
	}

	delete[] strMessageTemp;
}

void CGUIMessageBox::SetPosition(float fPosX, float fPosY)					//x, y는 메시지 박스의 left top이 아닌 정가운데점.
{
	
}

BOOL CGUIMessageBox::UpdateEx(float fTick, BOOL &bClick, BOOL bAble)
{
	m_bAble = bAble ;
	
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
	else
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
				if ( m_pNoButton->GetIsClick() )
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
				if ( m_pCancelButton->GetIsClick() )
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
				if ( ( m_bEnableEnter && GetAsyncKeyState(VK_RETURN)) ||( m_pConfirmButton->GetIsClick() ) )
				{
					if(m_lpResultValue) { *m_lpResultValue = MB_YES | MB_CONFIRM; m_lpResultValue = NULL; }
					if(m_lpValue)
					{
						if ( strcmp( m_pNumberEdit->GetText(), "" ) )
							*m_lpValue = atoi( m_pNumberEdit->GetText() ) ;
						else
							*m_lpValue = 0;

						m_lpValue = NULL;
					}
					if(m_lpstrValue)
					{
						if ( strcmp( m_pStringEdit->GetText(), ""))
							strcpy(m_lpstrValue, m_pStringEdit->GetText() ) ;
						else
							strcpy(m_lpstrValue, "" ) ;
						m_lpstrValue = NULL ;
					}

					bClick = TRUE ;
					if ( m_dwFlag & MB_NUMBER )  
						m_pNumberEdit->SetFocus() ;

					if (m_dwFlag & MB_STRING ) 
						m_pStringEdit->SetFocus() ;
					return FALSE;
				}
			}
			else if ( m_pYesButton )
			{
				if ( ( m_bEnableEnter && GetAsyncKeyState(VK_RETURN)) || m_pYesButton->GetIsClick() )
				{
					if ( m_lpResultValue ) 
					{ 
						*m_lpResultValue = MB_YES | MB_CONFIRM ;
						m_lpResultValue = NULL ;
					}

					if ( m_lpValue )
					{
						if ( strcmp( m_pNumberEdit->GetText(), "" ) )
							*m_lpValue = atoi( m_pNumberEdit->GetText() ) ;
						else
							*m_lpValue = 0;

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

HRESULT CGUIMessageBox::Render(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	return S_OK ;
}

VOID CGUIMessageBox::GuildMsgBoxRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	if ( m_dwFlag & MB_SUPPLEMENTDATA )
	{
		if  ( ( strcmp( m_szSupplementData[ 2 ], "" ) ) && (strcmp( m_szSupplementData[ 0 ], "" ) ) )
		{
			RECT rcRect ;
/*
			SetRect(&rcRect, m_spNumber.m_nPutX + 10, m_spNumber.m_nPutY - 30, m_spNumber.m_nPutX + 110, m_spNumber.m_nPutY - 14 ) ;
			g_TextEdit.DrawText( m_szSupplementData[0], &rcRect, DT_VCENTER | DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));			

			SetRect(&rcRect, m_spNumber.m_nPutX + 110, m_spNumber.m_nPutY - 30, m_spNumber.m_nPutX + 267, m_spNumber.m_nPutY - 14 ) ;
			g_TextEdit.DrawText( m_szSupplementData[1], &rcRect, DT_RIGHT | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255));

			SetRect(&rcRect, m_spNumber.m_nPutX +10, m_spNumber.m_nPutY - 14, m_spNumber.m_nPutX + 110, m_spNumber.m_nPutY + 2 ) ;
			g_TextEdit.DrawText( m_szSupplementData[2], &rcRect, DT_VCENTER | DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));			

			SetRect(&rcRect, m_spNumber.m_nPutX + 110, m_spNumber.m_nPutY - 14, m_spNumber.m_nPutX + 267, m_spNumber.m_nPutY + 2 ) ;
			g_TextEdit.DrawText( m_szSupplementData[3], &rcRect, DT_RIGHT | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255));
*/
			return ;
		}

		if ( strcmp( m_szSupplementData[ 0 ], "" ) )
		{
			RECT rcRect ;
/*
			SetRect(&rcRect, m_spNumber.m_nPutX + 10, m_spNumber.m_nPutY - 14, m_spNumber.m_nPutX + 110, m_spNumber.m_nPutY + 2 ) ;
			g_TextEdit.DrawText( m_szSupplementData[0], &rcRect, DT_VCENTER | DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));			

			SetRect(&rcRect, m_spNumber.m_nPutX + 110, m_spNumber.m_nPutY - 14, m_spNumber.m_nPutX + 267, m_spNumber.m_nPutY + 2 ) ;
			g_TextEdit.DrawText( m_szSupplementData[1], &rcRect, DT_RIGHT | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255));
*/
		}
	}
}

void CGUIMessageBox::AddWord(char *strWord)
{
	char *strMessage = new char[strlen(strWord) + 10];
	strcpy(strMessage, strWord);
	m_lstWord.push_back(strMessage);
}

VOID		CGUIMessageBox::CreateDialogImage()
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	INT		iHeight = m_lstMessage.size() * 15;
	INT		iCount  = 0 ;	
	if ( iHeight < 256 )
	{
		if ( m_dwFlag == MB_NORMAL )
		{
			iCount = ( m_lstMessage.size() * 3 ) ;
		}
		else
		{
			iCount = ( m_lstMessage.size() * 3 ) + 8 ;
		}
	}
	else
	{
		iCount = 58 ;
	}

	CRYLSpriteList::Instance()->CreateGMImage( 256, ( iCount * 5 ) + 30 , &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	for ( int i = 0 ; i < iCount ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 15 + (i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 15 + (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	if ( ( m_dwFlag & MB_STRING ) || ( m_dwFlag & MB_NUMBER ) )
	{
		pImageDIB->MakeFrameRect( 26, ( iCount * 5 ) - 8, 83, 17,  0xff959595, 0xc0000000,  1 ) ;
	}

	pImage = new CRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
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

VOID		CGUIMessageBox::CreateControl( DWORD dwFlag ) 
{
	m_dwFlag = dwFlag;
	INT iHeight = m_lstMessage.size() * 15;

	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

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
		for(itchar = ( *it )->m_lstMessage.begin(); itchar != ( *it )->m_lstMessage.end(); ++ itchar )
		{
			m_pDataListBox->AddItem( (*itchar), 0xffffffff, 0xffffffff, 0xffffffff ) ;
		}
	}
	AttachGMUIControl( m_pDataListBox ) ;


	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	if(m_dwFlag & MB_EXIT)
	{
		RECT rt ;
		m_pExitButton = new CRYLButton( this ) ;
		m_pExitButton->SetLeft( 243 ) ;
		m_pExitButton->SetTop( 3 ) ;
		m_pExitButton->OnClickEvent = DlgClick ;
		m_pExitButton->SetInstance( GUIMESSAGEBOX::EXIT ) ;
		SetRect( &rt, 202, 0, 213, 9 ) ;
		m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 202, 18, 213, 27 ) ;
		m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 202, 9, 213, 18 ) ;
		m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 202, 9, 213, 18 ) ;
		m_pExitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pExitButton ) ;
	}

	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	if(m_dwFlag & MB_PREV)
	{
		m_pPrevButton = new CRYLButton( this ) ;
		m_pPrevButton->SetCaption( "이전" ) ;
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

	if(m_dwFlag & MB_NEXT)
	{
		m_pNextButton = new CRYLButton( this ) ;
		m_pNextButton->SetCaption( "다음" ) ;
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

	if(m_dwFlag & MB_CONFIRM)
	{
		m_pConfirmButton = new CRYLButton( this ) ;
		m_pConfirmButton->SetCaption( "확인" ) ;
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
	else if(m_dwFlag & MB_YES)
	{
		m_pYesButton = new CRYLButton( this ) ;
		m_pYesButton->SetCaption( "예" ) ;
		m_pYesButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
		m_pYesButton->SetFontColor( 0xffffffff) ;
		m_pYesButton->SetColor( 0xffffffff) ;
		m_pYesButton->OnClickEvent = DlgClick ;
		m_pYesButton->SetInstance( GUIMESSAGEBOX::YES ) ;
		m_pYesButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pYesButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pYesButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pYesButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pYesButton ) ;
	}

	if(m_dwFlag & MB_CANCEL)
	{
		m_pCancelButton = new CRYLButton( this ) ;
		m_pCancelButton->SetLeft( 194 ) ;
		m_pCancelButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
		m_pCancelButton->SetCaption( "취소" ) ;
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
	else if(m_dwFlag & MB_NO)
	{
		m_pNoButton = new CRYLButton( this ) ;
		m_pNoButton->SetLeft( 194  ) ;
		m_pNoButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
		m_pNoButton->SetCaption( "아니오" ) ;
		m_pNoButton->SetFontColor( 0xffffffff) ;
		m_pNoButton->SetColor( 0xffffffff) ;
		m_pNoButton->OnClickEvent = DlgClick ;
		m_pNoButton->SetInstance( GUIMESSAGEBOX::NO ) ;
		m_pNoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pNoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pNoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pNoButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 3 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pNoButton ) ;
	}

	if(m_dwFlag & MB_NUMBER)
	{
		m_pNumberEdit = new CRYLEdit( this ) ;
		m_pNumberEdit->SetWidth( 81 ) ;
		m_pNumberEdit->SetHeight( 15 ) ;
		m_pNumberEdit->SetColor( 0xff000000 ) ;
		m_pNumberEdit->SetFontColor( 0xffffffff ) ;
		m_pNumberEdit->SetInstance( GUIMESSAGEBOX::NUMBER ) ;
		m_pNumberEdit->SetMaxLength( 9 ) ;
		m_pNumberEdit->OnKeyDown = DlgKeyDown; 
		m_pNumberEdit->SetNumeralOnly( TRUE );
        m_pNumberEdit->SetIsMoneyNumber( TRUE );
		m_pNumberEdit->SetFocus();
		AttachGMUIControl( m_pNumberEdit ) ;
	}

	if(m_dwFlag & MB_STRING)
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

	if((m_dwFlag & MB_CONFIRM) || (m_dwFlag & MB_YES) || (m_dwFlag & MB_ARRAYCONFIRM))
	{
		if((m_dwFlag & MB_CANCEL) || (m_dwFlag & MB_NO))
		{
			if ( m_pConfirmButton )
			{
				m_pConfirmButton->SetLeft( 132 ) ;
				m_pConfirmButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
			}
			
			if ( m_pYesButton )
			{
				m_pYesButton->SetLeft( 132 ) ;
				m_pYesButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
			}
		} 
		else
		{
			if ( m_pConfirmButton )
			{
				m_pConfirmButton->SetLeft( 194 ) ;
				m_pConfirmButton->SetTop( m_pDataListBox->GetHeight()+ 23 ) ;
			}
			
			if ( m_pYesButton )
			{
				m_pYesButton->SetLeft( 194 ) ;
				m_pYesButton->SetTop( m_pDataListBox->GetHeight() + 23) ;
			}

		}
	}
	if((m_dwFlag & MB_NUMBER) || (m_dwFlag & MB_STRING)||( m_dwFlag & MB_SUPPLEMENTDATA ))
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
