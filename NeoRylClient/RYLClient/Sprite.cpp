// Sprite.cpp: implementation of the CSprite class.
//
//////////////////////////////////////////////////////////////////////

#include <vertex.h>
#include "Sprite.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSprite::CSprite()
{
	m_bShow			= TRUE;
	m_bHorizon		= TRUE;
//	m_lpVB			= NULL;
	m_fRate			= 1.0f;
	m_lpTexture		= NULL ;
}

CSprite::~CSprite()
{
//	if(m_lpVB) { m_lpVB->Release(); m_lpVB = NULL; }
}

BOOL CSprite::Create(long nPutX, long nPutY, long nTexX1, long nTexY1, long nTexX2, long nTexY2, CTexture *lpTexture)
{
/*	if(m_lpVB) { m_lpVB->Release(); m_lpVB = NULL; }

	if(FAILED(lpD3DDevice->CreateVertexBuffer(4 * sizeof(TLVertex), 0, TLVERTEXFVF, D3DPOOL_DEFAULT, &m_lpVB)))
	{
		return FALSE;
	}

	TLVertex *pVertices;
	if(FAILED(lpD3DDevice->Lock(0, 4 * sizeof(TLVertex), 0, (unsigned char **)&pVertex, 0)))
	{
		return FALSE;
	}*/

	m_nPutX = nPutX;			m_nPutY = nPutY;
	m_rcRender.left = nTexX1;
	m_rcRender.top = nTexY1;
	m_rcRender.right = nTexX2;
	m_rcRender.bottom = nTexY2;
	m_lpTexture = lpTexture;

	return TRUE;
}

void CSprite::Render(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned char Alpha, long fx, long fy)
{
	if(!this) return;

	if(!m_lpTexture) return;

	if(!m_bShow) return;

	TLVertex pVertices[4];
	pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
	pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = Alpha;
	pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
	pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
	pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

	if(m_bHorizon)
	{
		pVertices[0].v.x = pVertices[1].v.x = m_nPutX + fx - 0.5f;
		pVertices[2].v.x = pVertices[3].v.x = m_nPutX + fx + ((m_rcRender.right - m_rcRender.left) * m_fRate) - 0.5f;
		pVertices[0].v.y = pVertices[2].v.y = m_nPutY + fy - 0.5f;
		pVertices[1].v.y = pVertices[3].v.y = m_nPutY + fy + (m_rcRender.bottom - m_rcRender.top) - 0.5f;

		pVertices[0].tu = pVertices[1].tu = (float)m_rcRender.left / m_lpTexture->m_dwWidth;
		pVertices[2].tu = pVertices[3].tu = (float)(m_rcRender.left + ((m_rcRender.right - m_rcRender.left) * m_fRate)) / m_lpTexture->m_dwWidth;

		pVertices[0].tv = pVertices[2].tv = (float)m_rcRender.top / m_lpTexture->m_dwHeight;
		pVertices[1].tv = pVertices[3].tv = (float)m_rcRender.bottom / m_lpTexture->m_dwHeight;
	} else
	{
		pVertices[0].v.x = pVertices[1].v.x = m_nPutX + fx - 0.5f;
		pVertices[2].v.x = pVertices[3].v.x = m_nPutX + fx + (m_rcRender.right - m_rcRender.left) - 0.5f;
		pVertices[0].v.y = pVertices[2].v.y = m_nPutY + fy + ((m_rcRender.bottom - m_rcRender.top) * (1.0f - m_fRate)) - 0.5f;
		pVertices[1].v.y = pVertices[3].v.y = m_nPutY + fy + (m_rcRender.bottom - m_rcRender.top) - 0.5f;

		pVertices[0].tu = pVertices[1].tu = (float)m_rcRender.left / m_lpTexture->m_dwWidth;
		pVertices[2].tu = pVertices[3].tu = (float)m_rcRender.right / m_lpTexture->m_dwWidth;

		pVertices[0].tv = pVertices[2].tv = (float)(m_rcRender.top + ((m_rcRender.bottom - m_rcRender.top) * (1.0f - m_fRate))) / m_lpTexture->m_dwHeight;
		pVertices[1].tv = pVertices[3].tv = (float)m_rcRender.bottom / m_lpTexture->m_dwHeight;
	}

	SetRenderState( lpD3DDevice ) ;
	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(0, m_lpTexture->GetTexture());
	lpD3DDevice->SetTexture(1, NULL);
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));


}

void CSprite::Render(LPDIRECT3DDEVICE8 lpD3DDevice, float fDirection, unsigned char Alpha)
{
	if(!this) return;

	if(!m_lpTexture) return;

	if(!m_bShow) return;

	TLVertex pVertices[4];
	pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
	pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = Alpha;
	pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
	pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
	pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

	int CenterX = m_nPutX + (m_rcRender.right - m_rcRender.left) / 2.0f;
	int CenterY = m_nPutY + (m_rcRender.bottom - m_rcRender.top) / 2.0f;

	vector3 vecRot[4];
	vecRot[0].x = vecRot[1].x = m_nPutX - CenterX;
	vecRot[2].x = vecRot[3].x = m_nPutX - CenterX + (m_rcRender.right - m_rcRender.left);
	vecRot[0].y = vecRot[2].y = m_nPutY - CenterY;
	vecRot[1].y = vecRot[3].y = m_nPutY - CenterY + (m_rcRender.bottom - m_rcRender.top);

	pVertices[0].v.x = vecRot[0].x * cosf(fDirection) - vecRot[0].y * sinf(fDirection);
	pVertices[0].v.y = vecRot[0].x * sinf(fDirection) + vecRot[0].y * cosf(fDirection);

	pVertices[1].v.x = vecRot[1].x * cosf(fDirection) - vecRot[1].y * sinf(fDirection);
	pVertices[1].v.y = vecRot[1].x * sinf(fDirection) + vecRot[1].y * cosf(fDirection);

	pVertices[2].v.x = vecRot[2].x * cosf(fDirection) - vecRot[2].y * sinf(fDirection);
	pVertices[2].v.y = vecRot[2].x * sinf(fDirection) + vecRot[2].y * cosf(fDirection);

	pVertices[3].v.x = vecRot[3].x * cosf(fDirection) - vecRot[3].y * sinf(fDirection);
	pVertices[3].v.y = vecRot[3].x * sinf(fDirection) + vecRot[3].y * cosf(fDirection);

	pVertices[0].v.x += CenterX - 0.5f;
	pVertices[0].v.y += CenterY - 0.5f;

	pVertices[1].v.x += CenterX - 0.5f;
	pVertices[1].v.y += CenterY - 0.5f;

	pVertices[2].v.x += CenterX - 0.5f;
	pVertices[2].v.y += CenterY - 0.5f;

	pVertices[3].v.x += CenterX - 0.5f;
	pVertices[3].v.y += CenterY - 0.5f;

	pVertices[0].tu = pVertices[1].tu = (float)m_rcRender.left / m_lpTexture->m_dwWidth;
	pVertices[2].tu = pVertices[3].tu = (float)m_rcRender.right / m_lpTexture->m_dwWidth;

	pVertices[0].tv = pVertices[2].tv = (float)m_rcRender.top / m_lpTexture->m_dwHeight;
	pVertices[1].tv = pVertices[3].tv = (float)m_rcRender.bottom / m_lpTexture->m_dwHeight;

	SetRenderState( lpD3DDevice ) ;
	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(0, m_lpTexture->GetTexture());
	lpD3DDevice->SetTexture(1, NULL);
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
}

void CSprite::Render(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned char Alpha, LPRECT pRect )
{
	if(!this) return;

	if(!m_lpTexture) return;

	if(!m_bShow) return;

	TLVertex pVertices[4];
	pVertices[0].Diffuse.c  = pVertices[1].Diffuse.c  = pVertices[2].Diffuse.c  = pVertices[3].Diffuse.c  = 0xFFFFFFFF ;
	pVertices[0].Diffuse.a  = pVertices[1].Diffuse.a  = pVertices[2].Diffuse.a  = pVertices[3].Diffuse.a  = Alpha ;
	pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0 ;
	pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
	pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

	if(m_bHorizon)
	{
		pVertices[0].v.x = pVertices[1].v.x = m_nPutX - 0.5f;
		pVertices[2].v.x = pVertices[3].v.x = m_nPutX + ((pRect->right - pRect->left) * m_fRate) - 0.5f;
		pVertices[0].v.y = pVertices[2].v.y = m_nPutY - 0.5f;
		pVertices[1].v.y = pVertices[3].v.y = m_nPutY + (pRect->bottom - pRect->top) - 0.5f;

		pVertices[0].tu = pVertices[1].tu = (float)m_rcRender.left / m_lpTexture->m_dwWidth;
		pVertices[2].tu = pVertices[3].tu = (float)( m_rcRender.left + pRect->right ) / m_lpTexture->m_dwWidth;

		pVertices[0].tv = pVertices[2].tv = (float)m_rcRender.top / m_lpTexture->m_dwHeight;
		pVertices[1].tv = pVertices[3].tv = (float)( m_rcRender.top + pRect->bottom ) / m_lpTexture->m_dwHeight;
	} else
	{
		pVertices[0].v.x = pVertices[1].v.x = m_nPutX - 0.5f;
		pVertices[2].v.x = pVertices[3].v.x = m_nPutX + (pRect->right - pRect->left) - 0.5f;
		pVertices[0].v.y = pVertices[2].v.y = m_nPutY + ((pRect->bottom - pRect->top) * (1.0f - m_fRate)) - 0.5f;
		pVertices[1].v.y = pVertices[3].v.y = m_nPutY + (pRect->bottom - pRect->top) - 0.5f;

		pVertices[0].tu = pVertices[1].tu = (float)m_rcRender.left / m_lpTexture->m_dwWidth;
		pVertices[2].tu = pVertices[3].tu = (float)( m_rcRender.left + pRect->right ) / m_lpTexture->m_dwWidth;

		pVertices[0].tv = pVertices[2].tv = (float)m_rcRender.top / m_lpTexture->m_dwHeight;
		pVertices[1].tv = pVertices[3].tv = (float)( m_rcRender.top + pRect->bottom ) / m_lpTexture->m_dwHeight;
	}

	SetRenderState( lpD3DDevice ) ;
	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(0, m_lpTexture->GetTexture());
	lpD3DDevice->SetTexture(1, NULL);
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
}

VOID		CSprite::SetRenderState( LPDIRECT3DDEVICE8 lpD3DDevice )
{

	/*	lpD3DDevice->SetRenderState( D3DRS_ZENABLE                   ,1 ) ; //=   7,
    lpD3DDevice->SetRenderState( D3DRS_FILLMODE                  ,3 ) ; //=   8,
    lpD3DDevice->SetRenderState( D3DRS_SHADEMODE                 ,2 ) ; //=   9,
    lpD3DDevice->SetRenderState( D3DRS_LINEPATTERN               ,0 ) ; //=  10,
    lpD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE              ,1 ) ; //=  14,
    lpD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE           ,0 ) ; //=  15,
    lpD3DDevice->SetRenderState( D3DRS_LASTPIXEL                 ,1 ) ; //=  16,
    lpD3DDevice->SetRenderState( D3DRS_SRCBLEND                  ,5 ) ; //=  19,
    lpD3DDevice->SetRenderState( D3DRS_DESTBLEND                 ,6 ) ; //=  20,
    lpD3DDevice->SetRenderState( D3DRS_CULLMODE                  ,1 ) ; //=  22,
    lpD3DDevice->SetRenderState( D3DRS_ZFUNC                     ,4 ) ; //=  23,
    lpD3DDevice->SetRenderState( D3DRS_ALPHAREF                  ,0 ) ; //=  24,
    lpD3DDevice->SetRenderState( D3DRS_ALPHAFUNC                 ,8 ) ; //=  25,
    lpD3DDevice->SetRenderState( D3DRS_DITHERENABLE              ,0 ) ; //=  26,
    lpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE          ,0 ) ; //=  27,
    lpD3DDevice->SetRenderState( D3DRS_FOGENABLE                 ,1 ) ; //=  28,
    lpD3DDevice->SetRenderState( D3DRS_SPECULARENABLE            ,0 ) ; //=  29,
    lpD3DDevice->SetRenderState( D3DRS_ZVISIBLE                  ,0 ) ; //=  30,
    lpD3DDevice->SetRenderState( D3DRS_FOGCOLOR                  ,-138606 ) ; //=  34,
    lpD3DDevice->SetRenderState( D3DRS_FOGTABLEMODE              ,3 ) ; //=  35,
    lpD3DDevice->SetRenderState( D3DRS_FOGSTART                  ,1184923452 ) ; //=  36,
    lpD3DDevice->SetRenderState( D3DRS_FOGEND                    ,1206607871 ) ; //=  37,
    lpD3DDevice->SetRenderState( D3DRS_FOGDENSITY                ,1065353216 ) ; //=  38,
    lpD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS             ,0 ) ; //=  40,
    lpD3DDevice->SetRenderState( D3DRS_ZBIAS                     ,0 ) ; //=  47,
    lpD3DDevice->SetRenderState( D3DRS_RANGEFOGENABLE            ,1 ) ; //=  48,
    lpD3DDevice->SetRenderState( D3DRS_STENCILENABLE             ,0 ) ; //=  52,
    lpD3DDevice->SetRenderState( D3DRS_STENCILFAIL               ,1 ) ; //=  53,
    lpD3DDevice->SetRenderState( D3DRS_STENCILZFAIL              ,1 ) ; //=  54,
    lpD3DDevice->SetRenderState( D3DRS_STENCILPASS               ,1 ) ; //=  55,
    lpD3DDevice->SetRenderState( D3DRS_STENCILFUNC               ,8 ) ; //=  56,
    lpD3DDevice->SetRenderState( D3DRS_STENCILREF                ,0 ) ; //=  57,
    lpD3DDevice->SetRenderState( D3DRS_STENCILMASK               ,-1 ) ; //=  58,
    lpD3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK          ,-1 ) ; //=  59,
    lpD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR             ,-1 ) ; //=  60,
    lpD3DDevice->SetRenderState( D3DRS_WRAP0                     ,0 ) ; //= 128,
    lpD3DDevice->SetRenderState( D3DRS_WRAP1                     ,0 ) ; //= 129,
    lpD3DDevice->SetRenderState( D3DRS_WRAP2                     ,0 ) ; //= 130,
    lpD3DDevice->SetRenderState( D3DRS_WRAP3                     ,0 ) ; //= 131,
    lpD3DDevice->SetRenderState( D3DRS_WRAP4                     ,0 ) ; //= 132,
    lpD3DDevice->SetRenderState( D3DRS_WRAP5                     ,0 ) ; //= 133,
    lpD3DDevice->SetRenderState( D3DRS_WRAP6                     ,0 ) ; //= 134,
    lpD3DDevice->SetRenderState( D3DRS_WRAP7                     ,0 ) ; //= 135,
    lpD3DDevice->SetRenderState( D3DRS_CLIPPING                  ,1 ) ; //= 136,
    lpD3DDevice->SetRenderState( D3DRS_LIGHTING                  ,0 ) ; //= 137,
    lpD3DDevice->SetRenderState( D3DRS_AMBIENT                   ,-9868951 ) ; //= 139,
    lpD3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE             ,3 ) ; //= 140,
    lpD3DDevice->SetRenderState( D3DRS_COLORVERTEX               ,1 ) ; //= 141,
    lpD3DDevice->SetRenderState( D3DRS_LOCALVIEWER               ,0 ) ; //= 142,
    lpD3DDevice->SetRenderState( D3DRS_NORMALIZENORMALS          ,0 ) ; //= 143,
    lpD3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE     ,1 ) ; //= 145,
    lpD3DDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE    ,2 ) ; //= 146,
    lpD3DDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE     ,0 ) ; //= 147,
    lpD3DDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE    ,0 ) ; //= 148,
    lpD3DDevice->SetRenderState( D3DRS_VERTEXBLEND               ,0 ) ; //= 151,
    lpD3DDevice->SetRenderState( D3DRS_CLIPPLANEENABLE           ,0 ) ; //= 152,
    lpD3DDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING  ,0 ) ; //= 153,
    lpD3DDevice->SetRenderState( D3DRS_POINTSIZE                 ,1065353216 ) ; //= 154,
    lpD3DDevice->SetRenderState( D3DRS_POINTSIZE_MIN             ,0 ) ; //= 155,
    lpD3DDevice->SetRenderState( D3DRS_POINTSPRITEENABLE         ,0 ) ; //= 156,
    lpD3DDevice->SetRenderState( D3DRS_POINTSCALEENABLE          ,0 ) ; //= 157,
    lpD3DDevice->SetRenderState( D3DRS_POINTSCALE_A              ,1065353216 ) ; //= 158,
    lpD3DDevice->SetRenderState( D3DRS_POINTSCALE_B              ,0 ) ; //= 159,
    lpD3DDevice->SetRenderState( D3DRS_POINTSCALE_C              ,0 ) ; //= 160,
    lpD3DDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS      ,1 ) ; //= 161,
    lpD3DDevice->SetRenderState( D3DRS_MULTISAMPLEMASK           ,-1 ) ; //= 162,
    lpD3DDevice->SetRenderState( D3DRS_PATCHEDGESTYLE            ,0 ) ; //= 163,
    lpD3DDevice->SetRenderState( D3DRS_PATCHSEGMENTS             ,1065353216 ) ; //= 164,
    lpD3DDevice->SetRenderState( D3DRS_DEBUGMONITORTOKEN         ,-1163015426 ) ; //= 165,
    lpD3DDevice->SetRenderState( D3DRS_POINTSIZE_MAX             ,1132462080 ) ; //= 166,
    lpD3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE  ,0 ) ; //= 167,
    lpD3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE          ,15 ) ; //= 168,
    lpD3DDevice->SetRenderState( D3DRS_TWEENFACTOR               ,0 ) ; //= 170,
    lpD3DDevice->SetRenderState( D3DRS_BLENDOP                   ,1 ) ; //= 171,    
    lpD3DDevice->SetRenderState( D3DRS_POSITIONORDER             ,3 ) ; //= 172,
    lpD3DDevice->SetRenderState( D3DRS_NORMALORDER               ,1 ) ; //= 173,


	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP		       , 2 ) ;
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1		       , 2 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2			   , 2 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP			   , 3 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1             , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2             , 2 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT00          , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT01          , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT10          , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT11          , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX         , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU              , 3 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV              , 3 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_BORDERCOLOR           , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER             , 2 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER             , 2 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER             , 2 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_MIPMAPLODBIAS         , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_MAXMIPLEVEL           , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_MAXANISOTROPY         , 1 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVLSCALE         , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVLOFFSET        , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS , 0 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSW              , 3 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG0             , 1 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG0             , 1 ) ;
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_RESULTARG             , 1 ) ;

	lpD3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP		       , 1 ) ;
	lpD3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1		       , 2 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2			   , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP			   , 1 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1             , 2 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2             , 1 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT00          , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT01          , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT10          , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT11          , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX         , 1 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU              , 1 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV              , 1 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_BORDERCOLOR           , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER             , 2 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_MINFILTER             , 2 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER             , 2 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_MIPMAPLODBIAS         , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_MAXMIPLEVEL           , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_MAXANISOTROPY         , 1 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_BUMPENVLSCALE         , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_BUMPENVLOFFSET        , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS , 0 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_ADDRESSW              , 1 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG0             , 1 ) ;
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG0             , 1 ) ;
	lpD3DDevice->SetTextureStageState( 1, D3DTSS_RESULTARG             , 1 ) ;
	//*/
}
