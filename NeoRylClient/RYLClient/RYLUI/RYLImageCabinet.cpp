#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "Texture.h"
#include "BaseDataDefine.h"
#include "GMMemory.h"

CRYLImageCabinet::CRYLImageCabinet() : CGMObject()
{
	pszFileName = NULL ;
}

CRYLImageCabinet::~CRYLImageCabinet()
{
	Clear() ;
}

VOID CRYLImageCabinet::Clear()
{
	CRYLImage*	pImage ;

	INT count = m_ImageList.size() ;
	for ( INT i = 0 ;i < count ; i ++ )
	{
		pImage = m_ImageList[ i ] ;
        GM_RELEASE(pImage);
	}

	m_ImageList.clear() ;
}

VOID CRYLImageCabinet::GetRYLImage( INT Index, CRYLImage **ppOutImage)
{
	CRYLImage* pImage ;
	*ppOutImage = NULL ;

	if ( ( Index < 0 ) && ( Index > (int)m_ImageList.size() - 1 ) )
		return ;
	
	pImage = m_ImageList[ Index ] ;
	*ppOutImage = pImage ;
}

CRYLImage* CRYLImageCabinet::GetRYLImage( INT Index )
{
	CRYLImage* pImage ;

	if ( ( Index < 0 ) && ( Index > (int)m_ImageList.size() - 1 ) )
		return NULL ;
	pImage = m_ImageList[ Index ] ;

	return pImage ;
}

CRYLImage* CRYLImageCabinet::GetRYLImage (int Width, int Height, EDefaultCtrlType CtrlType, DWORD dwClearColor)
{
	CGMImageDib*	pDib = NULL;
	CRYLImage*		pImage = NULL;

	INT count = m_ImageList.size();

	for (INT i = 0 ; i < count ; i ++ )
	{
		pImage = m_ImageList[ i ] ;
		if ( ( CRYLImageCabinet::EDefaultCtrlType)pImage->GetInstance() == CtrlType )
		{
			return pImage ;
		}
	}

	pImage = NULL ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;

	if(!pImage)
	{
		return NULL;
	}
	
	char buff[ MAX_PATH ] ;
	
	switch( CtrlType )
	{
		case EDCT_BUTTON_NORMAL:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_BUTTON_NORMAL.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_NORMAL ) ;
			break;
		case EDCT_BUTTON_DOWN:
			
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_BUTTON_DOWN.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_DOWN ) ;
			break;
		case EDCT_BUTTON_UP:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_BUTTON_OVER.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance(EDCT_BUTTON_UP ) ;
			break;
		case EDCT_BUTTON_DISIBLE:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_BUTTON_DISABLE.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_DISIBLE ) ;
			break;

		case EDCT_TAB_BUTTON_OPEN :
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_TAB_OPEN.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_NORMAL ) ;
			break ;
			
		case EDCT_TAB_BUTTON_CLOSE :
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_TAB_CLOSE.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_NORMAL ) ;
			break ;
			
		case EDCT_TAB_BUTTON_LEFT_OPEN :
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_tab_left_open.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_NORMAL ) ;
			break ;

		case EDCT_TAB_BUTTON_LEFT_CLOSE :
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_tab_left_closed.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_NORMAL ) ;
			break ;

		case EDCT_TAB_BUTTON_RIGHT_OPEN :
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_tab_right_open.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_NORMAL ) ;
			break ;
			
		case EDCT_TAB_BUTTON_RIGHT_CLOSE :
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_tab_right_closed.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_NORMAL ) ;
			break ;

		case EDCT_TAB_BUTTON_CENTER_OPEN :
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_tab_middle_open.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_NORMAL ) ;
			break ;

		case EDCT_TAB_BUTTON_CENTER_CLOSE :
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_tab_middle_closed.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_NORMAL ) ;
			break ;

		case EDCT_TAB_BUTTON_SPACER :
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_tab_spacer.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_BUTTON_NORMAL ) ;
			break ;

		case EDCT_CAPTION_CLOSE_BUTTON_UP:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_TITLEBAR_CLOSE_BUTTON_NORMAL.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance(EDCT_CAPTION_CLOSE_BUTTON_UP ) ;
			break;
		case EDCT_CAPTION_CLOSE_BUTTON_DOWN:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_TITLEBAR_CLOSE_BUTTON_DOWN.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_CAPTION_CLOSE_BUTTON_DOWN ) ;
			break;
		case EDCT_CAPTION_CLOSE_BUTTON_DISIBLE:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_TITLEBAR_CLOSE_BUTTON_NORMAL.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_CAPTION_CLOSE_BUTTON_DISIBLE ) ;
			break;
		case EDCT_CAPTION_CLOSE_BUTTON_ENTER:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\RYL\\RYL_TITLEBAR_CLOSE_BUTTON_NORMAL.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_CAPTION_CLOSE_BUTTON_ENTER ) ;
			break;

		case EDCT_CAPTION_MINI_BUTTON_UP:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Caption_Mini_Button_UP.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_CAPTION_MINI_BUTTON_UP ) ;
			break;
		case EDCT_CAPTION_MINI_BUTTON_DOWN:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Caption_Mini_Button_DOWN.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_CAPTION_MINI_BUTTON_DOWN ) ;
			break;
		case EDCT_CAPTION_MINI_BUTTON_DISIBLE:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Caption_Mini_Button_DISIBLE.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_CAPTION_MINI_BUTTON_DISIBLE ) ;
			break;
		case EDCT_CAPTION_MINI_BUTTON_ENTER:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Caption_Mini_Button_ENTER.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_CAPTION_MINI_BUTTON_ENTER ) ;
			break;

		case EDCT_SMALL_BUTTON_UP:
			break;
		case EDCT_SMALL_BUTTON_DOWN:
			break;
		case EDCT_SMALL_BUTTON_DISIBLE:
			break;
		case EDCT_SMALL_BUTTON_ENTER:
			break;

		// ComboBox Button
		case EDCT_COMBOBOX_BUTTON_UP:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Combo_Button_UP.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_COMBOBOX_BUTTON_UP ) ;
			break;
		case EDCT_COMBOBOX_BUTTON_DOWN:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Combo_Button_DOWN.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_COMBOBOX_BUTTON_DOWN ) ;
			break;
		case EDCT_COMBOBOX_BUTTON_DISIBLE:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Combo_Button_DISIBLE.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_COMBOBOX_BUTTON_DISIBLE ) ;
			break;
		case EDCT_COMBOBOX_BUTTON_ENTER:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Combo_Button_ENTER.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_COMBOBOX_BUTTON_ENTER ) ;
			break;

		// CheckBox Button
		case EDCT_CHECKBOX_BUTTON_UP:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\CheckBox_Check.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_CHECKBOX_BUTTON_UP ) ;
			break;
		case EDCT_CHECKBOX_BUTTON_DOWN:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\CheckBox_Checked.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_CHECKBOX_BUTTON_DOWN ) ;
			break;
		case EDCT_CHECKBOX_BUTTON_DISIBLE:
			break;
		case EDCT_CHECKBOX_BUTTON_ENTER:
			break;

		// ListBox Top Button
		case EDCT_LISTBOX_TOP_BUTTON_NORMAL:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_UP.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_TOP_BUTTON_NORMAL ) ;
			break;
		case EDCT_LISTBOX_TOP_BUTTON_DOWN:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_DOWN.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_TOP_BUTTON_DOWN ) ;
			break;
		case EDCT_LISTBOX_TOP_BUTTON_UP:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_DISIBLE.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_TOP_BUTTON_UP ) ;
			break;
		case EDCT_LISTBOX_TOP_BUTTON_DISIBLE:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_ENTER.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_TOP_BUTTON_DISIBLE ) ;
			break;

		// ListBox Bottom Button
		case EDCT_LISTBOX_BOTTOM_BUTTON_NORMAL:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_UP.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_BOTTOM_BUTTON_NORMAL ) ;
			break;
		case EDCT_LISTBOX_BOTTOM_BUTTON_DOWN:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_DOWN.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_BOTTOM_BUTTON_DOWN ) ;
			break;
		case EDCT_LISTBOX_BOTTOM_BUTTON_UP:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_DISIBLE.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_BOTTOM_BUTTON_UP ) ;
			break;
		case EDCT_LISTBOX_BOTTOM_BUTTON_DISIBLE:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_ENTER.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_BOTTOM_BUTTON_DISIBLE ) ;
			break;

		// ListBox Bottom Button
		case EDCT_LISTBOX_TRACK_BUTTON_NORMAL:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_UP.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_TRACK_BUTTON_NORMAL ) ;
			break;
		case EDCT_LISTBOX_TRACK_BUTTON_DOWN:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_DOWN.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_TRACK_BUTTON_DOWN ) ;
			break;
		case EDCT_LISTBOX_TRACK_BUTTON_UP:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_DISIBLE.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_TRACK_BUTTON_UP ) ; 
			break;
		case EDCT_LISTBOX_TRACK_BUTTON_DISIBLE:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_ENTER.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_LISTBOX_TRACK_BUTTON_DISIBLE ) ;
			break;

		// ime view Left Button
		case EDCT_IMEVIEW_LEFT_BUTTON_UP:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_UP.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_IMEVIEW_LEFT_BUTTON_UP ) ;
			break;
		case EDCT_IMEVIEW_LEFT_BUTTON_DOWN:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_DOWN.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_IMEVIEW_LEFT_BUTTON_DOWN ) ;
			break;
		case EDCT_IMEVIEW_LEFT_BUTTON_DISIBLE:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_DISIBLE.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_IMEVIEW_LEFT_BUTTON_DISIBLE ) ;
			break;
		case EDCT_IMEVIEW_LEFT_BUTTON_ENTER:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_ENTER.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_IMEVIEW_LEFT_BUTTON_ENTER ) ;
			break;

		// ime view Right Button
		case EDCT_IMEVIEW_RIGHT_BUTTON_UP:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_UP.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_IMEVIEW_RIGHT_BUTTON_UP ) ;
			break;
		case EDCT_IMEVIEW_RIGHT_BUTTON_DOWN:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_DOWN.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_IMEVIEW_RIGHT_BUTTON_DOWN ) ;
			break;
		case EDCT_IMEVIEW_RIGHT_BUTTON_DISIBLE:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_DISIBLE.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_IMEVIEW_RIGHT_BUTTON_DISIBLE ) ;
			break;
		case EDCT_IMEVIEW_RIGHT_BUTTON_ENTER:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\Small_Button_ENTER.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_IMEVIEW_RIGHT_BUTTON_ENTER ) ;
			break;

		case EDCT_EDIT_Line:
			strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
			strcat( buff, "\\Data\\UI\\Default\\EditLine.gmi" ) ;
			pImage->LoadFromFile( buff ) ;
			pImage->SetInstance( EDCT_IMEVIEW_RIGHT_BUTTON_ENTER ) ;
			break;

		case EDCT_EDIT_ENTER:
			break;

		case EDCT_FORM_CAPTION:
			pDib = new CGMImageDib ();
			pDib->SetSize( Width, Height, FALSE, EAIF_32Bit ) ;
			pDib->ClearbyColor( dwClearColor ) ;
			pImage->LoadFromImage( pDib ) ;
			GM_DELETE( pDib ) ;
			break;
		case EDCT_FORM_WINDOW:
			pDib = new CGMImageDib ();
			pDib->SetSize( Width, Height, FALSE, EAIF_32Bit ) ;
			pDib->ClearbyColor( dwClearColor ) ;
			pImage->LoadFromImage( pDib ) ;
			GM_DELETE( pDib ) ;
			break ;
	
	}
	AddRYLImage( pImage ) ;
	return pImage ;
}

CRYLImage* CRYLImageCabinet::GetRYLImage( LPSTR pFileName )
{
	CRYLImage* pImage = NULL;

	INT count = m_ImageList.size();
	
	for (INT i = 0 ;i < count ; i ++ )
	{
		pImage = m_ImageList[ i ] ;
		if ( pImage->GetFileName() == pFileName )
		{
			return pImage ;
		}
	}

	CGMImageDib* pImageDib = NULL;

	pImageDib = new CGMImageDib() ;	
	pImageDib->LoadFromFile( pFileName ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;

	if(pImage)
	{
		pImage->LoadFromImage( pImageDib ) ;
		AddRYLImage( pImage ) ;
	}	

	GM_DELETE( pImageDib ) ;

	return pImage;
}

CRYLImage* CRYLImageCabinet::GetRYLImage( INT Width, INT Height, DWORD dwClearColor )
{
	CRYLImage*		pImage = NULL;
	CGMImageDib*	pImageDib = NULL;
	
	pImageDib = new CGMImageDib() ;
	pImageDib->SetSize( Width, Height, FALSE, EAIF_32Bit ) ;
	pImageDib->ClearbyColor( dwClearColor ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;

	if(pImage)
	{
		pImage->LoadFromImage( pImageDib ) ;
		AddRYLImage( pImage ) ;
	}
	
	GM_DELETE( pImageDib ) ;

	return pImage ;
}

VOID CRYLImageCabinet::SetRYLImage( INT Index, CRYLImage* pRYLImage)
{
	if ( (Index < 0) && (Index > (int)m_ImageList.size() - 1 ) )
		return;
}

VOID CRYLImageCabinet::AddRYLImage( CRYLImage* pRYLImage )
{
	int nCount = m_ImageList.size() ;
	
	m_ImageList.push_back( pRYLImage ) ;
}

VOID CRYLImageCabinet::DeleteRYLImage( INT Index )
{
	CRYLImage* pImage ;

	if ( ( Index < 0 ) && ( Index > (int)m_ImageList.size() - 1 ) )
		return;
	
	pImage = m_ImageList[ Index ] ;

	vector<CRYLImage*>::iterator lt ;
	lt = m_ImageList.begin();
	lt = lt + Index;
    GM_RELEASE(*lt);
	m_ImageList.erase( lt ) ;
}

VOID CRYLImageCabinet::DeleteRYLImage( CRYLImage* pRYLImage )
{
	CRYLImage* pImage ;
	vector<CRYLImage*>::iterator it ;

	it = m_ImageList.begin();
	INT count = m_ImageList.size();
	for ( INT i = 0 ; i < count ; i ++ )
	{
		pImage = *it;
		if (pImage == pRYLImage)
		{
			GM_RELEASE( pImage ) ;
			m_ImageList.erase( it ) ;
			return ;
		}
		it ++ ;
	}
}

INT CRYLImageCabinet::GetCount()
{
	return m_ImageList.size();
}

LPSTR CRYLImageCabinet::GetFileName()
{
	return pszFileName;
}

VOID CRYLImageCabinet::LoadFromFile( LPSTR pFileName)	
{
}

VOID CRYLImageCabinet::LoadFromStream( CGMFileStream* pStream ) 	
{
}