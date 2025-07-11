
#include "RYLClientMain.h"

#include "..\\GUITextEdit.h"

#include "WinInput.h"
#include "GMUITooltipBox.h"

#include "RYLGameData.h"
#include "RYLStringTable.h"
#include "RYLStringGrid.h"

#include "GMBase.h"

#include <string>
#include "GMMemory.h"


CGMUITooltipBox::CGMUITooltipBox()
{

}

CGMUITooltipBox::~CGMUITooltipBox()
{
	DestroyTooltip();
}

VOID	CGMUITooltipBox::InitTooltip( LPCTSTR strTooltip, DWORD dwKey, DWORD dwCtrlType )
{
	char   *strTempTooltip ;
	WCHAR	 seps[] = L"\\" ;
	WCHAR	*token;
	
	WCHAR	strWideString[1024];
	char	strBreak[1024];
	int		nBufferSize = 1024;

	DestroyTooltip() ;

	if ( !strcmp( strTooltip, "" ) )
	{ 
		m_bShow = FALSE ; 
		return ; 
	}

	MultiByteToWideChar(CP_ACP, 0, strTooltip, strlen(strTooltip) + 1, strWideString, nBufferSize);
	token = wcstok( strWideString, seps ) ;

	m_dwMaxLength = 0 ;
	ULONG	dwLength ;
	
	for( ; ; )
	{
		if ( !token ) 
			break ;

		int nLen = wcslen( token ) ;

		strTempTooltip = new CHAR[ nLen * 2 + 1] ;

		WideCharToMultiByte(CP_ACP, 0, token, nLen + 1, strBreak, nBufferSize, NULL, NULL);
		
		memcpy( strTempTooltip, strBreak, nLen * 2 + 1 ) ;
		m_lstTooltip.push_back( strTempTooltip ) ;

		dwLength = CRYLStringTable::GetStringWidth( strTempTooltip ) + 4 ;
		
		if ( dwLength >= m_dwMaxLength ) 
			m_dwMaxLength = dwLength ;

		token = wcstok( NULL, seps ) ;
	}

	m_dwCtrlType = dwCtrlType ;
	m_dwKey		 = dwKey ;
}

BOOL	CGMUITooltipBox::GetIsTooltip()
{
	if ( m_lstTooltip.empty() )
		return FALSE ;
	else
		return TRUE ;
}

VOID	CGMUITooltipBox::DestroyTooltip()
{
	vector< CHAR * >::iterator it ;
	CHAR *strDelete;

	for ( it = m_lstTooltip.begin() ; it != m_lstTooltip.end() ; it ++ )
	{
		if ( ( *it ) )
		{
			strDelete = ( *it ) ;
			GM_DELETE_ARRAY( strDelete ) ;
		}
	}
	m_lstTooltip.clear() ;
}

VOID	CGMUITooltipBox::Render( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	POINT *ptMousePos = g_DeviceInput.GetMouseLocal() ;

	USHORT	SizeX = ptMousePos->x + 7 ;
	USHORT	SizeY = ptMousePos->y + 11 ;
	SizeY += 16;
	
	if ( SizeX + m_dwMaxLength + 6 > ( ULONG )BaseGraphicsLayer::m_lScreenSx ) 
	{ 
		SizeX -= ( SizeX + m_dwMaxLength + 6 ) - BaseGraphicsLayer::m_lScreenSx ; 
	}

	if ( SizeY + m_lstTooltip.size() * 18 + 6 > ( ULONG )BaseGraphicsLayer::m_lScreenSy )
	{ 
		SizeY -= ( SizeY + m_lstTooltip.size() * 18 + 6 ) - BaseGraphicsLayer::m_lScreenSy ;
	}

	CRYLGameData::Instance()->RenderRect( lpD3DDevice, 
		SizeX, SizeY, SizeX + m_dwMaxLength + 6, SizeY + m_lstTooltip.size() * 18 + 6, 0x00000000, 0x99 ) ;

	CRYLStringGrid	tooltipList(NULL, FALSE);

	tooltipList.SetLeft(SizeX);
	tooltipList.SetTop(SizeY);
	tooltipList.SetWidth(m_dwMaxLength);
	tooltipList.SetHeight(m_lstTooltip.size() * 18);
	tooltipList.SetColCount(m_lstTooltip.size());

	tooltipList.SetItemHeight(18);

	for (vector<CHAR*>::iterator it = m_lstTooltip.begin(); it != m_lstTooltip.end(); ++it)
	{
		tooltipList.AddItem(0, *it, m_dwMaxLength, D3DCOLOR_RGBA( 255, 255, 255, 255 ));
	}

	tooltipList.DrawItems(SizeX + 5, SizeY + 5);
}
