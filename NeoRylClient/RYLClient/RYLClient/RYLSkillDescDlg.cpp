#include <winsock2.h>
#include <time.h>
#include "RYLSkillDescDlg.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"
#include "RYLImageManager.h"

#include "RYLLabel.h"
#include "RYLListBox.h"
#include "RYLStringTable.h"

#include "RYLSkillScriptMgr.h"

#include <Skill/SkillMgr.h>
#include "GMMemory.h"
#include "RYLClientMain.h"


CRYLSkillDescDlg*	g_pSkillDescDlg = NULL ;

bool			    CRYLSkillDescDlg::ms_bSameSkillTooltip = false ;
unsigned short		CRYLSkillDescDlg::ms_wSkillID = 0x0000 ;
unsigned char		CRYLSkillDescDlg::ms_cSkillStep = 0x00 ;
unsigned char		CRYLSkillDescDlg::ms_cSkillLevel = 0x00 ;
unsigned char		CRYLSkillDescDlg::ms_cWeaponType = 0x00 ;

CRYLSkillDescDlg::CRYLSkillDescDlg() : CRYLDialog()
{
	m_pSkillFrameImage		= NULL ;
	m_pSkillKindLabel		= NULL ;		// 스킬 종류
	m_pSkillDescentLabel1	= NULL ;	// 스킬 계열
	m_pSkillDescentLabel2	= NULL ;	
	m_pSkillDetailLabel		= NULL ;	

	m_pSkillDescentListBox	= NULL ;
	m_pSkillDescentDescListBox = NULL ;
	m_pSkillDetailListBox	= NULL ;
	m_pSkillSubDescListBox	= NULL ;
	
	SetWidth( 256 ) ;
	g_pSkillDescDlg = this ;	
	InitCtrl() ;

	m_uiDlgHeight = 0 ;
}

CRYLSkillDescDlg::~CRYLSkillDescDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLSkillDescDlg::InitCtrl() 
{
	m_pSkillKindLabel = new CRYLLabel( this ) ;
	m_pSkillKindLabel->SetFontColor( 0xffffffff ) ;
	m_pSkillKindLabel->SetAutoSize( TRUE ) ;
	m_pSkillKindLabel->SetLeft( ( GetWidth() - CRYLStringTable::GetStringWidth( "Passive" ) ) / 2 ) ;
	m_pSkillKindLabel->SetTop( 7 ) ;
	AttachGMUIControl( m_pSkillKindLabel ) ;
	
	m_pSkillDescentLabel1 = new CRYLLabel( this ) ;
	m_pSkillDescentLabel1->SetLeft( 116 ) ;
	m_pSkillDescentLabel1->SetTop( 30 ) ;
	m_pSkillDescentLabel1->SetFontColor( 0xffffffff ) ;
	m_pSkillDescentLabel1->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pSkillDescentLabel1 ) ;

	m_pSkillDescentLabel2 = new CRYLLabel( this ) ;
	m_pSkillDescentLabel2->SetLeft( 29 ) ;
	m_pSkillDescentLabel2->SetTop( 135 ) ;
	m_pSkillDescentLabel2->SetFontColor( 0xffffffff ) ;
	m_pSkillDescentLabel2->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pSkillDescentLabel2 ) ;

	m_pSkillDetailLabel = new CRYLLabel( this ) ;
	m_pSkillDetailLabel->SetLeft( 29 ) ;
	m_pSkillDetailLabel->SetTop( 209 ) ;
	m_pSkillDetailLabel->SetFontColor( 0xffffffff ) ;
	m_pSkillDetailLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pSkillDetailLabel ) ;

	m_pAltSkillDescLabel = new CRYLLabel( this ) ;
	m_pAltSkillDescLabel->SetLeft( 29 ) ;
	m_pAltSkillDescLabel->SetTop( 209 ) ;
	m_pAltSkillDescLabel->SetFontColor( 0xff000000 ) ;
	m_pAltSkillDescLabel->SetAutoSize( TRUE ) ;
	m_pAltSkillDescLabel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pAltSkillDescLabel ) ;


	m_pSkillDescentListBox = new CRYLListBox( this, FALSE ) ;
	m_pSkillDescentListBox->SetLeft( 112 ) ;
	m_pSkillDescentListBox->SetTop( 46 ) ;
	m_pSkillDescentListBox->SetWidth( 130 ) ;
	m_pSkillDescentListBox->SetHeight( 97 ) ;
	m_pSkillDescentListBox->SetItemMerginX( 5 ) ;
    m_pSkillDescentListBox->SetItemMerginY( 10 ) ;
	m_pSkillDescentListBox->SetItemHeight( 16 ) ;
	m_pSkillDescentListBox->SetInstance( RYLSKILLDESCDLG::SKILLDESCENT_LISTBOX ) ;
	AttachGMUIControl( m_pSkillDescentListBox ) ;

	
	
	m_pSkillDescentDescListBox = new CRYLListBox( this, FALSE ) ;
	m_pSkillDescentDescListBox->SetLeft( 16 ) ;
	m_pSkillDescentDescListBox->SetTop( 156 ) ;
	m_pSkillDescentDescListBox->SetWidth( 224 ) ;
	m_pSkillDescentDescListBox->SetHeight( 37 ) ;
	m_pSkillDescentDescListBox->SetInstance( RYLSKILLDESCDLG::SKILLDESCENTDESC_LISTBOX ) ;
	AttachGMUIControl( m_pSkillDescentDescListBox ) ;


	
	m_pSkillDetailListBox = new CRYLListBox( this, FALSE ) ;
	m_pSkillDetailListBox->SetLeft( 16 ) ;
	m_pSkillDetailListBox->SetTop( 233 ) ;
	m_pSkillDetailListBox->SetWidth( 224 ) ;
	m_pSkillDetailListBox->SetHeight( 50 ) ;
	m_pSkillDetailListBox->SetInstance( RYLSKILLDESCDLG::SKILLDETAIL_LISTBOX ) ;
	AttachGMUIControl( m_pSkillDetailListBox ) ;
	 
		
	m_pSkillSubDescListBox = new CRYLListBox( this, FALSE ) ;
	m_pSkillSubDescListBox->SetLeft( 11 ) ;
	m_pSkillSubDescListBox->SetTop( 294 ) ;
	m_pSkillSubDescListBox->SetWidth( 231 ) ;
	m_pSkillSubDescListBox->SetHeight( 64 ) ;
	m_pSkillSubDescListBox->SetInstance( RYLSKILLDESCDLG::SKILLSUBDESC_LISTBOX ) ;
	AttachGMUIControl( m_pSkillSubDescListBox ) ;
}

VOID	CRYLSkillDescDlg::FinalCtrl()
{
	GM_DELETE( m_pSkillKindLabel ) ;		// 스킬 종류
	GM_DELETE( m_pSkillDescentLabel1 ) ;	// 스킬 계열
	GM_DELETE( m_pSkillDescentLabel2 ) ;
	GM_DELETE( m_pSkillDetailLabel ) ;
	GM_DELETE( m_pAltSkillDescLabel ) ;
	
	GM_DELETE( m_pSkillDescentListBox ) ;
	GM_DELETE( m_pSkillDescentDescListBox ) ;
	GM_DELETE( m_pSkillDetailListBox ) ;
	GM_DELETE( m_pSkillSubDescListBox ) ;

	GM_RELEASE( m_pSkillFrameImage ) ;
}

VOID	CRYLSkillDescDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLSkillDescDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLSkillDescDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLSkillDescDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )	
{
	return S_OK ;
}

HRESULT		CRYLSkillDescDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

void		CRYLSkillDescDlg::RestoreSkillUI( unsigned short wSkillID, 
											  unsigned char cSkillStep, 
											  unsigned char cSkillLevel, 
											  unsigned char cWeaponType ) 
{
	ms_cSkillStep  = cSkillStep ;
	ms_cSkillLevel = cSkillLevel ;
	ms_cWeaponType = cWeaponType ;
	
	if ( RestoreTooltip( wSkillID ) )
	{
		RestorePaint() ;
	}
}

void		CRYLSkillDescDlg::RestorePaint() 
{
	/*
	262B71 -  파랑 캐스트(cast)
	000000 - 까망 패시브(passive)
	6F5D0F - 노랑 챈트(chant)
	7E1818 - 빨강 인스턴스(Instance)
	*/
	GM_RELEASE( m_pSkillFrameImage ) ;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;

    srand( (unsigned)time( NULL ) );

//	unsigned long dwHeight = 410 ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, m_uiDlgHeight, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	for ( int i = 0 ; i < (int)( m_uiDlgHeight / 5 ) - 2 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, m_uiDlgHeight - 10, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	// head 
	pImageDIB->DrawFrameRect( 0, 0, 256, 1, 0xff4a321d ) ;
	pImageDIB->DrawFrameRect( 1, 1, 254, 1, 0xffcbb985 ) ;
	
	if ( strcmp( m_pSkillKindLabel->GetCaption(), "Cast" ) == 0 )
		pImageDIB->DrawFrameRect( 2, 2, 252,  20, 0xff7E1818 ) ;
	else if ( strcmp( m_pSkillKindLabel->GetCaption(), "Passive" ) == 0 )
		pImageDIB->DrawFrameRect( 2, 2, 252,  20, 0xff000000 ) ;
	else if ( strcmp( m_pSkillKindLabel->GetCaption(), "Chant" ) == 0 )
		pImageDIB->DrawFrameRect( 2, 2, 252,  20, 0xff6F5D0F ) ;
	else if ( strcmp( m_pSkillKindLabel->GetCaption(), "Instance" ) == 0 )
		pImageDIB->DrawFrameRect( 2, 2, 252,  20, 0xff262B71 ) ;

	pImageDIB->DrawFrameRect( 106, 46, 124,  2, 0xff959595 ) ;

	INT iHeight = m_pSkillDescentDescListBox->GetHeight() + 21 ;
	pImageDIB->MakeFrameRect( 6, 139, 244, iHeight, 0xff959595, 0xc0000000,  1 ) ;

	INT iTop     = iHeight + m_pSkillDescentDescListBox->GetTop() + 10 ;
	    iHeight  = m_pSkillDetailListBox->GetHeight() + 25 ;
	pImageDIB->MakeFrameRect( 6, iTop, 244, iHeight, 0xff959595, 0xc0000000,  1 ) ;

	INT iWidth =  CRYLStringTable::GetStringWidth( m_pSkillDescentLabel2->GetCaption() ) + 10 ;
	pImageDIB->MakeFrameRect( 24, 130, iWidth,  21, 0xff959595, 0xc0000000,  1 ) ;
	iWidth =  CRYLStringTable::GetStringWidth( m_pSkillDetailLabel->GetCaption() ) + 10 ;
	pImageDIB->MakeFrameRect( 24, iTop - 10, iWidth,  21, 0xff959595, 0xc0000000,  1 ) ;

	m_pSkillFrameImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pSkillFrameImage->LoadFromImage( pImageDIB ) ;
	AttachWindowTexture( ( CGMTexture*)m_pSkillFrameImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( m_uiDlgHeight ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
}

bool		CRYLSkillDescDlg::RestoreTooltip( unsigned short wSkillID ) 
{
	LPRYLSKILLNODE  lpSkillNode = g_pSkillScriptMgr()->FindSkillNode( wSkillID ) ;

	if ( lpSkillNode )
	{
		m_pSkillKindLabel->SetCaption( const_cast<LPSTR>( lpSkillNode->s_strSkillType.c_str() ) ) ;
		m_pSkillDescentLabel1->SetCaption( const_cast<LPSTR>( lpSkillNode->s_strSkillGradeName.c_str() ) ) ;
		m_pSkillDescentLabel2->SetCaption( const_cast<LPSTR>( lpSkillNode->s_strSkillGradeName.c_str() ) ) ;
		
		
		m_pSkillDescentListBox->ClearItem() ;
		m_pSkillDescentDescListBox->ClearItem() ; 
		m_pSkillDetailListBox->ClearItem() ; 
		m_pSkillSubDescListBox->ClearItem() ; 
		
		std::vector<std::string>::iterator	itTooltip =  lpSkillNode->s_vecSkillTooltip.begin() ;	// 스킬 툴팁
		std::vector<std::string>::iterator  itGrade   =	 lpSkillNode->s_vecSkillGrade.begin() ;	// 스킬 계열
		std::vector<std::string>::iterator  itDesc	  =  lpSkillNode->s_vecSkillDesc.begin() ;	// 스킬 설명

		while( itTooltip != lpSkillNode->s_vecSkillTooltip.end() )
		{
			( *itTooltip ).clear() ;
			++ itTooltip ;
		}
		lpSkillNode->s_vecSkillTooltip.clear() ;

		//g_pSkillScriptMgr()->CallFunction( g_SkillScriptMgr.m_Script, lpSkillNode ) ;
		g_pSkillScriptMgr()->UpdateScriptFlag() ;
		g_pSkillScriptMgr()->CallSkillDesc(  wSkillID, ms_cSkillStep + 1, lpSkillNode ) ;

		itTooltip = lpSkillNode->s_vecSkillTooltip.begin() ;
		while( itTooltip != lpSkillNode->s_vecSkillTooltip.end() )
		{
			std::string str = ( *itTooltip ) ; 
			m_pSkillDetailListBox->AddItem( const_cast<LPSTR>( str.c_str() ), 0xffffffff, 0xff00ff00, 0xffffffff ) ;
			++ itTooltip ;
		}

		int iCount = 0 ;
		while( itGrade != lpSkillNode->s_vecSkillGrade.end() )
		{
			std::string str = ( *itGrade ) ;
			if ( iCount == ms_cSkillStep )
				m_pSkillDescentListBox->AddItem( const_cast<LPSTR>( str.c_str() ), 0xffffff00, 0xff00ff00, 0xffffffff ) ;
			else
				m_pSkillDescentListBox->AddItem( const_cast<LPSTR>( str.c_str() ), 0xffffffff, 0xff00ff00, 0xffffffff ) ;

			++ itGrade ;
			++ iCount ;
		}

		while( itDesc != lpSkillNode->s_vecSkillDesc.end() )
		{
			std::string str = ( *itDesc ) ;
			if( g_ClientMain.m_iLanguage == 2)
			{
				WCHAR*	strWideString = NULL;
				WCHAR	seps[] = L"\\" ;
				WCHAR*	token;
				char	strSkill[1024];				

				int nLen = MultiByteToWideChar( CP_ACP, 0, str.c_str(), str.size(), NULL, NULL);
				strWideString = SysAllocStringLen(NULL, nLen);
				MultiByteToWideChar( CP_ACP, 0, str.c_str(), str.size(), strWideString, nLen ) ;

				token = wcstok( strWideString, seps ) ;

				nLen = wcslen( token ) ;

				WideCharToMultiByte(CP_ACP, 0, token, nLen + 1, strSkill, 1024, NULL, NULL);

				m_pSkillDescentDescListBox->AddItem( const_cast<LPSTR>( strSkill ), 0xffffffff, 0xff00ff00, 0xffffffff ) ;
		
				SysFreeString(strWideString);
			}
			else
			{
				m_pSkillDescentDescListBox->AddItem( const_cast<LPSTR>( str.c_str() ), 0xffffffff, 0xff00ff00, 0xffffffff ) ;
			}
			++ itDesc ;
		}

		m_pSkillDescentDescListBox->SetHeight( m_pSkillDescentDescListBox->GetListSize() * 15 + 10) ;
		INT iTop = m_pSkillDescentDescListBox->GetTop() + m_pSkillDescentDescListBox->GetHeight() + 26 ;
		m_pSkillDetailLabel->SetTop( iTop ) ;
		m_pSkillDetailListBox->SetTop( iTop + 24 ) ;
		m_pSkillDetailListBox->SetHeight( m_pSkillDetailListBox->GetListSize() * 16 + 10 ) ;

		iTop = m_pSkillDetailListBox->GetTop() + m_pSkillDetailListBox->GetHeight() + 23 ;
		m_pSkillSubDescListBox->SetTop( iTop ) ;

		m_uiDlgHeight = iTop + m_pSkillSubDescListBox->GetHeight() + 10 ;
		m_uiDlgHeight = ( m_uiDlgHeight / 5 ) * 5 ;
		m_pSkillSubDescListBox->ClearItem() ;
		if ( strcmp( m_pSkillKindLabel->GetCaption(), "Cast" ) == 0 )
		{
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3134], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3135], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3136], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3137], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
		}
		else if ( strcmp( m_pSkillKindLabel->GetCaption(), "Passive" ) == 0 )
		{
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3138], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3139], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
		}
		else if ( strcmp( m_pSkillKindLabel->GetCaption(), "Chant" ) == 0 )
		{
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3140], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3141], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3142], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
		}
		else if ( strcmp( m_pSkillKindLabel->GetCaption(), "Instance" ) == 0 )
		{
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3143], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3144], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
			m_pSkillSubDescListBox->AddItem( CRYLStringTable::m_strString[3145], 0xffffffff, 0xff00ff00, 0xffffffff ) ;
		}

		return true ;
	}

	return false ;
}

HRESULT		CRYLSkillDescDlg::Update( BOOL &bClick, BOOL &bEdge )  
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

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

	return S_OK ;
}