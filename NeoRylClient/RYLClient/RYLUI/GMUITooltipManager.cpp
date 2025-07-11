#include "resource.h"
#include "WinInput.h"
#include "GMUITooltipManager.h"
#include "GMBase.h"
#include "GMMemory.h"

CGMUITooltipManager GUITooltipMgr ;
CGMUITooltip *CGMUITooltipManager::m_ttRenderTarget = NULL ;

CGMUITooltipManager::CGMUITooltipManager()
{
	m_ttRenderTarget = NULL;
}

CGMUITooltipManager::~CGMUITooltipManager()
{
	DestroyAllTooltip();
}

BOOL	CGMUITooltipManager::AddTooltip( CGMUITooltip *pTooltip )
{
	m_lstTooltip.push_back( pTooltip ) ;

	return TRUE ;
}

CGMUITooltip*		CGMUITooltipManager::FindTooltip( DWORD dwKey ) 
{
	std::vector< CGMUITooltip * >::iterator	it = m_lstTooltip.begin() ;

	while ( it != m_lstTooltip.end() )
	{
		if ( ( *it )->m_dwKey == dwKey )
			return ( *it ) ;

		++ it ;
	}

	return NULL ;
}

VOID	CGMUITooltipManager::DestroyAllTooltip()
{
	std::vector< CGMUITooltip * >::iterator	it ;
	CGMUITooltip*		lpTooltip ;

	for ( it = m_lstTooltip.begin() ; it != m_lstTooltip.end() ; )
	{
		lpTooltip = ( *it ) ;
		it = m_lstTooltip.erase( it ) ;
		
		GM_DELETE( lpTooltip ) ;
	}
	m_lstTooltip.clear();
}

VOID	CGMUITooltipManager::Update( LONG lMoveX, LONG lMoveY )
{
	std::vector< CGMUITooltip * >::iterator	it ;
	CGMUITooltip*		pTooltip ;

	for ( it = m_lstTooltip.begin() ; it != m_lstTooltip.end() ; it ++ )
	{
		pTooltip = *it ;

		if ( pTooltip->m_bShow )
		{
			if ( pTooltip->m_dwCtrlType == TOOLTIP_DLG )
			{
				pTooltip->m_rcEdge.right  = pTooltip->m_rcEdge.right - pTooltip->m_rcEdge.left ;
				pTooltip->m_rcEdge.bottom = pTooltip->m_rcEdge.bottom - pTooltip->m_rcEdge.top ;
				pTooltip->m_rcEdge.left = 0 ;
				pTooltip->m_rcEdge.top = 0 ;
			}
			if ( pTooltip->m_dwCtrlType == TOOLTIP_TAB )
			{
				pTooltip->m_rcEdge.bottom = ( pTooltip->m_rcEdge.bottom - pTooltip->m_rcEdge.top ) + 15 ;
				pTooltip->m_rcEdge.top = 15 ;
			}
			
			if ( g_DeviceInput.InRect( (unsigned short)(pTooltip->m_rcEdge.left + lMoveX), (unsigned short)(pTooltip->m_rcEdge.top + lMoveY),
									   (unsigned short)(pTooltip->m_rcEdge.right + lMoveX - 1), (unsigned short)(pTooltip->m_rcEdge.bottom + lMoveY - 1) ) )
			{
				m_ttRenderTarget = pTooltip ;
				break ;
			}
		}
	}
}

VOID	CGMUITooltipManager::Render( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	if ( m_ttRenderTarget && m_ttRenderTarget->m_bShow )
	{
		m_ttRenderTarget->Render(lpD3DDevice);
	}
}
