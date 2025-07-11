// Sprite.cpp: implementation of the CRYLSprite class.
//
//////////////////////////////////////////////////////////////////////

#include <vertex.h>
#include "RYLSprite.h"
#include "BaseGraphicsLayer.h"
#include "GMMemory.h"

LPDIRECT3DDEVICE8   CRYLSprite::m_lpD3DDevice = NULL ;

CRYLSprite::CRYLSprite()
{
	m_bShow			= TRUE ;
	m_bHorizon		= TRUE ;
	m_fRate			= 1.0f ;
	m_lpTexture		= NULL ;

	m_lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
}

CRYLSprite::~CRYLSprite()
{

}

BOOL	CRYLSprite::Create( long nPutX, long nPutY, long nTexX1, long nTexY1, long nTexX2, long nTexY2, CTexture *lpTexture )
{
	m_nPutX				= nPutX ;		 	
	m_nPutY				= nPutY ;
	m_rcRender.left		= nTexX1 ;
	m_rcRender.top		= nTexY1 ;
	m_rcRender.right	= nTexX2 ;
	m_rcRender.bottom	= nTexY2 ;
	m_lpTexture			= lpTexture ;

	return TRUE	;
}

void	CRYLSprite::Render( LPDIRECT3DDEVICE8 m_lpD3DDevice, unsigned char Alpha, long fx, long fy )
{
	if ( !this )					return ;
	if ( !m_lpTexture )				return ;
	if ( !m_bShow )					return ;
	if ( !m_lpTexture->m_dwWidth )	return ;
	if ( !m_lpTexture->m_dwHeight )	return ;

	TLVertex pVertices[ 4 ] ;
	pVertices[ 0 ].Diffuse.c	= pVertices[ 1 ].Diffuse.c	= pVertices[ 2 ].Diffuse.c	= pVertices[ 3 ].Diffuse.c	= 0xFFFFFFFF ;
	pVertices[ 0 ].Diffuse.a	= pVertices[ 1 ].Diffuse.a	= pVertices[ 2 ].Diffuse.a	= pVertices[ 3 ].Diffuse.a	= Alpha ;
	pVertices[ 0 ].Specular.c	= pVertices[ 1 ].Specular.c = pVertices[ 2 ].Specular.c = pVertices[ 3 ].Specular.c = 0 ;
	pVertices[ 0 ].w			= pVertices[ 1 ].w			= pVertices[ 2 ].w			= pVertices[ 3 ].w			= 0.1f ;
	pVertices[ 0 ].v.z			= pVertices[ 1 ].v.z		= pVertices[ 2 ].v.z		= pVertices[ 3 ].v.z		= 0.1f ;

	if ( m_bHorizon )
	{
		pVertices[ 0 ].v.x = pVertices[ 1 ].v.x = m_nPutX + fx - 0.5f ;
		pVertices[ 2 ].v.x = pVertices[ 3 ].v.x = m_nPutX + fx + ( ( m_rcRender.right - m_rcRender.left ) * m_fRate ) - 0.5f ;
		pVertices[ 0 ].v.y = pVertices[ 2 ].v.y = m_nPutY + fy - 0.5f ;
		pVertices[ 1 ].v.y = pVertices[ 3 ].v.y = m_nPutY + fy + ( m_rcRender.bottom - m_rcRender.top ) - 0.5f ;

		pVertices[ 0 ].tu = pVertices[ 1 ].tu = ( float )m_rcRender.left / m_lpTexture->m_dwWidth ;
		pVertices[ 2 ].tu = pVertices[ 3 ].tu = ( float )( m_rcRender.left + ( ( m_rcRender.right - m_rcRender.left ) * m_fRate ) ) / m_lpTexture->m_dwWidth ;

		pVertices[ 0 ].tv = pVertices[ 2 ].tv = ( float )m_rcRender.top / m_lpTexture->m_dwHeight ;
		pVertices[ 1 ].tv = pVertices[ 3 ].tv = ( float )m_rcRender.bottom / m_lpTexture->m_dwHeight ;
	} 
	else
	{
		pVertices[ 0 ].v.x = pVertices[ 1 ].v.x = m_nPutX + fx - 0.5f ;
		pVertices[ 2 ].v.x = pVertices[ 3 ].v.x = m_nPutX + fx + ( m_rcRender.right - m_rcRender.left ) - 0.5f ;
		pVertices[ 0 ].v.y = pVertices[ 2 ].v.y = m_nPutY + fy + ( ( m_rcRender.bottom - m_rcRender.top ) * ( 1.0f - m_fRate ) ) - 0.5f ;
		pVertices[ 1 ].v.y = pVertices[ 3 ].v.y = m_nPutY + fy + ( m_rcRender.bottom - m_rcRender.top ) - 0.5f ;

		pVertices[ 0 ].tu = pVertices[ 1 ].tu = ( float )m_rcRender.left / m_lpTexture->m_dwWidth ;
		pVertices[ 2 ].tu = pVertices[ 3 ].tu = ( float )m_rcRender.right / m_lpTexture->m_dwWidth ;

		pVertices[ 0 ].tv = pVertices[ 2 ].tv = ( float )( m_rcRender.top + ( ( m_rcRender.bottom - m_rcRender.top ) * ( 1.0f - m_fRate ) ) ) / m_lpTexture->m_dwHeight ;
		pVertices[ 1 ].tv = pVertices[ 3 ].tv = ( float )m_rcRender.bottom / m_lpTexture->m_dwHeight ;
	}

	m_lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	m_lpD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_lpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	m_lpD3DDevice->SetVertexShader( NULL ) ;
	m_lpD3DDevice->SetVertexShader( TLVERTEXFVF ) ;
	m_lpD3DDevice->SetTexture( 0, m_lpTexture->GetTexture() ) ;
	m_lpD3DDevice->SetTexture( 1, NULL ) ;
	m_lpD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof( TLVertex ) ) ;
}

void CRYLSprite::Render(LPDIRECT3DDEVICE8 m_lpD3DDevice, float fDirection, unsigned char Alpha)
{
	if ( !this )			return ;
	if ( !m_lpTexture )		return ;
	if ( !m_bShow )			return ;
	if ( !m_lpTexture->m_dwWidth )	return ;
	if ( !m_lpTexture->m_dwHeight )	return ;

	TLVertex pVertices[ 4 ] ;
	pVertices[ 0 ].Diffuse.c	= pVertices[ 1 ].Diffuse.c	= pVertices[ 2 ].Diffuse.c	= pVertices[ 3 ].Diffuse.c	= 0xFFFFFFFF ;
	pVertices[ 0 ].Diffuse.a	= pVertices[ 1 ].Diffuse.a	= pVertices[ 2 ].Diffuse.a	= pVertices[ 3 ].Diffuse.a	= Alpha ;
	pVertices[ 0 ].Specular.c	= pVertices[ 1 ].Specular.c = pVertices[ 2 ].Specular.c = pVertices[ 3 ].Specular.c = 0 ;
	pVertices[ 0 ].w			= pVertices[ 1 ].w			= pVertices[ 2 ].w			= pVertices[ 3 ].w			= 0.1f ;
	pVertices[ 0 ].v.z			= pVertices[ 1 ].v.z		= pVertices[ 2 ].v.z		= pVertices[ 3 ].v.z		= 0.1f ;

	int Width = m_rcRender.right - m_rcRender.left ;
	int Height = m_rcRender.bottom - m_rcRender.top ;

	vector3 vecRot[ 4 ] ;
	vecRot[ 0 ].x = vecRot[ 1 ].x = -( Width / 2.0f ) ;
	vecRot[ 2 ].x = vecRot[ 3 ].x =  ( Width / 2.0f ) ;
	vecRot[ 0 ].y = vecRot[ 2 ].y = -( Height / 2.0f ) ;
	vecRot[ 1 ].y = vecRot[ 3 ].y =  ( Height / 2.0f ) ;

	pVertices[ 0 ].v.x = vecRot[ 0 ].x * cosf( fDirection ) - vecRot[ 0 ].y * sinf( fDirection ) ;
	pVertices[ 0 ].v.y = vecRot[ 0 ].x * sinf( fDirection ) + vecRot[ 0 ].y * cosf( fDirection ) ;
	pVertices[ 1 ].v.x = vecRot[ 1 ].x * cosf( fDirection ) - vecRot[ 1 ].y * sinf( fDirection ) ;
	pVertices[ 1 ].v.y = vecRot[ 1 ].x * sinf( fDirection ) + vecRot[ 1 ].y * cosf( fDirection ) ;
	pVertices[ 2 ].v.x = vecRot[ 2 ].x * cosf( fDirection ) - vecRot[ 2 ].y * sinf( fDirection ) ;
	pVertices[ 2 ].v.y = vecRot[ 2 ].x * sinf( fDirection ) + vecRot[ 2 ].y * cosf( fDirection ) ;
	pVertices[ 3 ].v.x = vecRot[ 3 ].x * cosf( fDirection ) - vecRot[ 3 ].y * sinf( fDirection ) ;
	pVertices[ 3 ].v.y = vecRot[ 3 ].x * sinf( fDirection ) + vecRot[ 3 ].y * cosf( fDirection ) ;

	pVertices[ 0 ].v.x += m_nPutX - 0.5f ;
	pVertices[ 0 ].v.y += m_nPutY - 0.5f ;
	pVertices[ 1 ].v.x += m_nPutX - 0.5f ;
	pVertices[ 1 ].v.y += m_nPutY - 0.5f ;
	pVertices[ 2 ].v.x += m_nPutX - 0.5f ;
	pVertices[ 2 ].v.y += m_nPutY - 0.5f ;
	pVertices[ 3 ].v.x += m_nPutX - 0.5f ;
	pVertices[ 3 ].v.y += m_nPutY - 0.5f ;

	pVertices[ 0 ].tu = pVertices[ 1 ].tu = ( float )m_rcRender.left / m_lpTexture->m_dwWidth ;
	pVertices[ 2 ].tu = pVertices[ 3 ].tu = ( float )m_rcRender.right / m_lpTexture->m_dwWidth ;
	pVertices[ 0 ].tv = pVertices[ 2 ].tv = ( float )m_rcRender.top / m_lpTexture->m_dwHeight ;
	pVertices[ 1 ].tv = pVertices[ 3 ].tv = ( float )m_rcRender.bottom / m_lpTexture->m_dwHeight ;

	m_lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	m_lpD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_lpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	m_lpD3DDevice->SetVertexShader( NULL ) ;
	m_lpD3DDevice->SetVertexShader( TLVERTEXFVF ) ;
	m_lpD3DDevice->SetTexture( 0, m_lpTexture->GetTexture() ) ;
	m_lpD3DDevice->SetTexture( 1, NULL ) ;
	m_lpD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof( TLVertex ) ) ;
}

void	CRYLSprite::Render( LPDIRECT3DDEVICE8 m_lpD3DDevice, unsigned char Alpha, LPRECT pRect )
{
	if ( !this )			return ;
	if ( !m_lpTexture )		return ;
	if ( !m_bShow )			return ;
	if ( !m_lpTexture->m_dwWidth )	return ;
	if ( !m_lpTexture->m_dwHeight )	return ;

	TLVertex pVertices[ 4 ] ;
	pVertices[ 0 ].Diffuse.c  = pVertices[ 1 ].Diffuse.c  = pVertices[ 2 ].Diffuse.c  = pVertices[ 3 ].Diffuse.c  = 0xFFFFFFFF ;
	pVertices[ 0 ].Diffuse.a  = pVertices[ 1 ].Diffuse.a  = pVertices[ 2 ].Diffuse.a  = pVertices[ 3 ].Diffuse.a  = Alpha ;
	pVertices[ 0 ].Specular.c = pVertices[ 1 ].Specular.c = pVertices[ 2 ].Specular.c = pVertices[ 3 ].Specular.c = 0 ;
	pVertices[ 0 ].w		  = pVertices[ 1 ].w		  = pVertices[ 2 ].w		  = pVertices[ 3 ].w		  = 0.1f ;
	pVertices[ 0 ].v.z		  = pVertices[ 1 ].v.z		  = pVertices[ 2 ].v.z		  = pVertices[ 3 ].v.z		  = 0.1f ;

	if ( m_bHorizon )
	{
		pVertices[ 0 ].v.x = pVertices[ 1 ].v.x = m_nPutX - 0.5f ;
		pVertices[ 2 ].v.x = pVertices[ 3 ].v.x = m_nPutX + ( ( pRect->right - pRect->left ) * m_fRate ) - 0.5f ;
		pVertices[ 0 ].v.y = pVertices[ 2 ].v.y = m_nPutY - 0.5f ;
		pVertices[ 1 ].v.y = pVertices[ 3 ].v.y = m_nPutY + ( pRect->bottom - pRect->top ) - 0.5f ;

		pVertices[ 0 ].tu = pVertices[ 1 ].tu = ( float )( m_rcRender.left + pRect->left ) / m_lpTexture->m_dwWidth ;
		pVertices[ 2 ].tu = pVertices[ 3 ].tu = ( float )( m_rcRender.left + pRect->right ) / m_lpTexture->m_dwWidth ;
		pVertices[ 0 ].tv = pVertices[ 2 ].tv = ( float )( m_rcRender.top + pRect->top ) / m_lpTexture->m_dwHeight ;
		pVertices[ 1 ].tv = pVertices[ 3 ].tv = ( float )( m_rcRender.top + pRect->bottom ) / m_lpTexture->m_dwHeight ;
	} else
	{
		pVertices[ 0 ].v.x = pVertices[ 1 ].v.x = m_nPutX - 0.5f ;
		pVertices[ 2 ].v.x = pVertices[ 3 ].v.x = m_nPutX + ( pRect->right - pRect->left ) - 0.5f;
		pVertices[ 0 ].v.y = pVertices[ 2 ].v.y = m_nPutY + ( ( pRect->bottom - pRect->top ) * ( 1.0f - m_fRate ) ) - 0.5f ;
		pVertices[ 1 ].v.y = pVertices[ 3 ].v.y = m_nPutY + ( pRect->bottom - pRect->top ) - 0.5f ;

		pVertices[ 0 ].tu = pVertices[ 1 ].tu = ( float )( m_rcRender.left + pRect->left ) / m_lpTexture->m_dwWidth ;
		pVertices[ 2 ].tu = pVertices[ 3 ].tu = ( float )( m_rcRender.left + pRect->right ) / m_lpTexture->m_dwWidth ;
		pVertices[ 0 ].tv = pVertices[ 2 ].tv = ( float )( m_rcRender.top + pRect->top ) / m_lpTexture->m_dwHeight ;
		pVertices[ 1 ].tv = pVertices[ 3 ].tv = ( float )( m_rcRender.top + pRect->bottom ) / m_lpTexture->m_dwHeight ;
	}

	m_lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	m_lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	m_lpD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_lpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	m_lpD3DDevice->SetVertexShader( NULL ) ;
	m_lpD3DDevice->SetVertexShader( TLVERTEXFVF ) ;
	m_lpD3DDevice->SetTexture( 0, m_lpTexture->GetTexture() ) ;
	m_lpD3DDevice->SetTexture( 1, NULL ) ;
	m_lpD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof( TLVertex ) ) ;
}
