//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLDamageEffect.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: single creature's damage management
//
// Date: 2004. 4. 10. ( Sat )
//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include "FrameTimer.h"
#include "../RYLClient/RYLClient/RYLTimer.h"
#include "Texture.h"
#include "SceneManager.h"

#include "RYLCreatureCommon.h"
#include "RYLDamageEffect.h"
#include "GMMemory.h"

//--------------------------------------------------------------------------------------------------------------------------
// functor
//--------------------------------------------------------------------------------------------------------------------------
static void ftDeleteDamageEffect( RYLDamageNode* pNode )
{
    delete pNode;
    pNode = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLDamageEffect
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------------------
// constructor, destructor
//--------------------------------------------------------------------------------------------------------------------------
RYLDamageEffect::RYLDamageEffect()
{
    m_pNumberTexture = NULL;
}

RYLDamageEffect::~RYLDamageEffect()
{
    ClearDamageEffect();
}

//--------------------------------------------------------------------------------------------------------------------------
// member functions
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// RYLDamageEffect::RenderDamage()
void RYLDamageEffect::RenderDamage( LPDIRECT3DDEVICE8 lpD3DDevice, bool bMovieVersion, vector3 vecPos, DAMAGE_TYPE type )
{
    if( !m_lstDamageEffect.empty() )
	{
        float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetShowInterfaceTimerID( ) );
		short wAlpha;

		list<RYLDamageNode*>::iterator iter = m_lstDamageEffect.begin();

        while( iter != m_lstDamageEffect.end() )
        {
			wAlpha = ( (*iter)->sDamageLifeTime / 1500.0f * 255.0f );

			if( wAlpha < 0 ) 
            {
                wAlpha = 0;
            }

            unsigned short  wMode = 0;
            float           fRate = 0.0f;

			if( !bMovieVersion )
			{
                fRate = (*iter)->fScale;

                switch( (*iter)->usDamageType )
				{
					case Type_Front:
					case Type_Side:
					case Type_Back:
					case Type_Poisoned:
					case Type_Fired:
                        {
                            switch( type )
                            {
							case FRIENDLY_DAMAGE:
                                {
                                    wMode = 1;
                                }
                                break;

                            case ENEMY_DAMAGE:
                                {
                                    wMode = 0;
                                }
                                break;
                            }
                        }
                        break;

					case Type_Heal:
                        {
                            switch( type )
                            {
                            case FRIENDLY_DAMAGE:
                                {
                                    wMode = 3;
                                }
                                break;

                            case ENEMY_DAMAGE:
                                {
                                    wMode = 3;
                                }
                                break;
                            }
                        }
                        break;

					case Type_ManaHeal:
                        {
                            switch( type )
                            {
                            case FRIENDLY_DAMAGE:
                                {
                                    wMode = 2;
                                }
                                break;

                            case ENEMY_DAMAGE:
                                {
                                    wMode = 2;
                                }
                                break;
                            }
                        }
                        break;

					case Type_Guard:
                        {
                            switch( type )
                            {
                            case FRIENDLY_DAMAGE:
                                {
                                    wMode = 7;
                                }
                                break;

                            case ENEMY_DAMAGE:
                                {
                                    wMode = 8;
                                }
                                break;
                            }
                        }
                        break;

					case Type_Critical:
                        {
                            switch( type )
                            {
                            case FRIENDLY_DAMAGE:
                                {
                                    wMode = 6;
                                    if( (*iter)->sDamageLifeTime >= 1200 )
						            {
                                        fRate = ( 5.0f - ( 1.0f - ( ( (*iter)->sDamageLifeTime - 1200.0f ) / 300.0f ) ) * 4.0f ) * (*iter)->fScale;
                                    }
                                    else
                                    {
                                        fRate = (*iter)->fScale;
                                    }
                                }
                                break;

                            case ENEMY_DAMAGE:
                                {
                                    wMode = 5;
                                    if( (*iter)->sDamageLifeTime >= 1200 )
						            {
                                        fRate = ( 5.0f - ( 1.0f - ( ( (*iter)->sDamageLifeTime - 1200.0f ) / 300.0f ) ) * 4.0f ) * (*iter)->fScale;
                                    }
                                    else
                                    {
                                        fRate = (*iter)->fScale;
                                    }
                                }
                                break;
                            }
                        }
                        break;

					case Type_Evade:
                        {
                            switch( type )
                            {
                            case FRIENDLY_DAMAGE:
                                {
                                    wMode = 9;
                                }
                                break;

                            case ENEMY_DAMAGE:
                                {
                                    wMode = 9;
                                }
                                break;
                            }
                        }
                        break;

					case Type_Resist:
                        {
                            switch( type )
                            {
                            case FRIENDLY_DAMAGE:
                                {
                                    wMode = 10;
                                }
                                break;

                            case ENEMY_DAMAGE:
                                {
                                    wMode = 10;
                                }
                                break;
                            }
                        }
                        break;

					case Type_Miss:
                        {
                            switch( type )
                            {
                            case FRIENDLY_DAMAGE:
                                {
                                    wMode = 11;
                                }
                                break;

                            case ENEMY_DAMAGE:
                                {
                                    wMode = 12;
                                }
                                break;
                            }
                        }
	                      break;
                }
			}
            
			RenderDamageNumber( lpD3DDevice,
							    vecPos.x, vecPos.y - ( 1.0f - (*iter)->sDamageLifeTime / 1500.0f ) * 90.0f,
								static_cast<unsigned char>( wAlpha ) * (*iter)->fScale, wMode, (*iter)->lDamageValue, fRate, (*iter)->dwAlign );


            (*iter)->sDamageLifeTime -= fUpdate * 2.0f;

			if( (*iter)->sDamageLifeTime <= 0 )
			{
				delete (*iter) ;
				(*iter) = NULL ;
                m_lstDamageEffect.erase( iter );
				return;
			} 
            else
			{
				++iter;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLDamageEffect::RenderDamageNumber()
void RYLDamageEffect::RenderDamageNumber( LPDIRECT3DDEVICE8 lpD3DDevice, short sPosX, short sPosY, unsigned char cAlpha, unsigned short wMode, long lValue, float fRate, unsigned long dwAlign )
{
    if( !m_pNumberTexture )
	{
		m_pNumberTexture = new CTexture;
		CTexture::SetPath( INTERFACETEXTUREPATH );
		m_pNumberTexture->Load( "num.dds" );
	}
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ;
	lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ;

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ) ;
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) ;
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ) ;
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ) ;
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ) ;

	char strNum[ 12 ];
	itoa( abs( lValue ), strNum, 10 );

	switch( wMode )
	{
		case 5:							// Critical
		case 6:
			if( strcmp( strNum, "" ) )
			{
				float fRealX = 23.0f * fRate;
				float fRealY = 29.0f * fRate;

				int len = strlen( strNum );
				float fPosX;
				switch(dwAlign)
				{
					case RYLDamageNode::DN_Center:
						fPosX = sPosX - ( (len * fRealX) / 2.0f);
						break;

					case RYLDamageNode::DN_Left:
						fPosX = sPosX - ( (len * fRealX) + (fRealX / 2.0f) );
						break;

					case RYLDamageNode::DN_Right:
						fPosX = sPosX + (fRealX / 2.0f);
						break;
				}
				float fPosY = sPosY - ( ( fRealY - 29.0f ) / 2.0f );

				TLVertex pVertices[ 4 ];
				pVertices[ 0 ].Diffuse.c = pVertices[ 1 ].Diffuse.c = pVertices[ 2 ].Diffuse.c = pVertices[ 3 ].Diffuse.c = 0xFFFFFFFF;
				pVertices[ 0 ].Diffuse.a = pVertices[ 1 ].Diffuse.a = pVertices[ 2 ].Diffuse.a = pVertices[ 3 ].Diffuse.a = cAlpha;
				pVertices[ 0 ].Specular.c = pVertices[ 1 ].Specular.c = pVertices[ 2 ].Specular.c = pVertices[ 3 ].Specular.c = 0;
				pVertices[ 0 ].w = pVertices[ 1 ].w = pVertices[ 2 ].w = pVertices[ 3 ].w = 0.1f;
				pVertices[ 0 ].v.z = pVertices[ 1 ].v.z = pVertices[ 2 ].v.z = pVertices[ 3 ].v.z = 0.1f;

				pVertices[ 0 ].v.x = pVertices[ 1 ].v.x = fPosX - 0.5f;
				pVertices[ 2 ].v.x = pVertices[ 3 ].v.x = fPosX + fRealX - 0.5f;
				pVertices[ 0 ].v.y = pVertices[ 2 ].v.y = fPosY - 0.5f;
				pVertices[ 1 ].v.y = pVertices[ 3 ].v.y = fPosY + fRealY - 0.5f;

				pVertices[ 0 ].tv = pVertices[ 2 ].tv = ( 92 + ( ( wMode - 5 ) * 29.0f ) ) / 256.0f + 0.0005f;
				pVertices[ 1 ].tv = pVertices[ 3 ].tv = ( 92 + ( ( wMode - 5 ) * 29.0f + 29.0f ) ) / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader( NULL );
				lpD3DDevice->SetVertexShader( TLVERTEXFVF );
				lpD3DDevice->SetTexture( 1, NULL );
				lpD3DDevice->SetTexture( 0, m_pNumberTexture->GetTexture() );

				for( int i = 0; i < len; i++ )
				{
					pVertices[ 0 ].tu = pVertices[ 1 ].tu = ( ( strNum[ i ] - '0' ) * 23.0f ) / 256.0f + 0.0005f;
					pVertices[ 2 ].tu = pVertices[ 3 ].tu = ( ( strNum[ i ] - '0' ) * 23.0f + 23.0f ) / 256.0f - 0.0005f;
					lpD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof( TLVertex ) );

					pVertices[ 0 ].v.x += fRealX;
					pVertices[ 1 ].v.x += fRealX;
					pVertices[ 2 ].v.x += fRealX;
					pVertices[ 3 ].v.x += fRealX;
				}
			}
			break;

		case 7:							// Block
		case 8:
			{
				float fRealX = 88.0f * fRate;
				float fRealY = 24.0f * fRate;

				float fPosX = sPosX - fRealX / 2.0f;
				float fPosY = sPosY - ( ( fRealY - 24.0f ) / 2.0f );

				TLVertex pVertices[ 4 ];
				pVertices[ 0 ].Diffuse.c = pVertices[ 1 ].Diffuse.c = pVertices[ 2 ].Diffuse.c = pVertices[ 3 ].Diffuse.c = 0xFFFFFFFF;
				pVertices[ 0 ].Diffuse.a = pVertices[ 1 ].Diffuse.a = pVertices[ 2 ].Diffuse.a = pVertices[ 3 ].Diffuse.a = cAlpha;
				pVertices[ 0 ].Specular.c = pVertices[ 1 ].Specular.c = pVertices[ 2 ].Specular.c = pVertices[ 3 ].Specular.c = 0;
				pVertices[ 0 ].w = pVertices[ 1 ].w = pVertices[ 2 ].w = pVertices[ 3 ].w = 0.1f;
				pVertices[ 0 ].v.z = pVertices[ 1 ].v.z = pVertices[ 2 ].v.z = pVertices[ 3 ].v.z = 0.1f;

				pVertices[ 0 ].v.x = pVertices[ 1 ].v.x = fPosX - 0.5f;
				pVertices[ 2 ].v.x = pVertices[ 3 ].v.x = fPosX + fRealX - 0.5f;
				pVertices[ 0 ].v.y = pVertices[ 2 ].v.y = fPosY - 0.5f;
				pVertices[ 1 ].v.y = pVertices[ 3 ].v.y = fPosY + fRealY - 0.5f;

				pVertices[ 0 ].tu = pVertices[ 1 ].tu = 0.0005f;
				pVertices[ 2 ].tu = pVertices[ 3 ].tu = 88 / 256.0f - 0.0005f;
				pVertices[ 0 ].tv = pVertices[ 2 ].tv = ( 150 + ( ( wMode - 7 ) * 24.0f ) ) / 256.0f - 0.0005f;
				pVertices[ 1 ].tv = pVertices[ 3 ].tv = ( 150 + ( ( wMode - 7 ) * 24.0f + 24.0f ) ) / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader( NULL );
				lpD3DDevice->SetVertexShader( TLVERTEXFVF );
				lpD3DDevice->SetTexture( 1, NULL );
				lpD3DDevice->SetTexture( 0, m_pNumberTexture->GetTexture() );
				lpD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof( TLVertex ) );
			}
			break;

		case 9:							// Evade
			{
				float fRealX = 110.0f * fRate;
				float fRealY = 25.0f * fRate;

				float fPosX = sPosX - fRealX / 2.0f;
				float fPosY = sPosY - ( ( fRealY - 25.0f ) / 2.0f );

				TLVertex pVertices[ 4 ];
				pVertices[ 0 ].Diffuse.c = pVertices[ 1 ].Diffuse.c = pVertices[ 2 ].Diffuse.c = pVertices[ 3 ].Diffuse.c = 0xFFFFFFFF;
				pVertices[ 0 ].Diffuse.a = pVertices[ 1 ].Diffuse.a = pVertices[ 2 ].Diffuse.a = pVertices[ 3 ].Diffuse.a = cAlpha;
				pVertices[ 0 ].Specular.c = pVertices[ 1 ].Specular.c = pVertices[ 2 ].Specular.c = pVertices[ 3 ].Specular.c = 0;
				pVertices[ 0 ].w = pVertices[ 1 ].w = pVertices[ 2 ].w = pVertices[ 3 ].w = 0.1f;
				pVertices[ 0 ].v.z = pVertices[ 1 ].v.z = pVertices[ 2 ].v.z = pVertices[ 3 ].v.z = 0.1f;

				pVertices[ 0 ].v.x = pVertices[ 1 ].v.x = fPosX - 0.5f;
				pVertices[ 2 ].v.x = pVertices[ 3 ].v.x = fPosX + fRealX - 0.5f;
				pVertices[ 0 ].v.y = pVertices[ 2 ].v.y = fPosY - 0.5f;
				pVertices[ 1 ].v.y = pVertices[ 3 ].v.y = fPosY + fRealY - 0.5f;

				pVertices[ 0 ].tu = pVertices[ 1 ].tu = 0.0005f;
				pVertices[ 2 ].tu = pVertices[ 3 ].tu = 110 / 256.0f - 0.0005f;
				pVertices[ 0 ].tv = pVertices[ 2 ].tv = 198 / 256.0f - 0.0005f;
				pVertices[ 1 ].tv = pVertices[ 3 ].tv = 223 / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader( NULL );
				lpD3DDevice->SetVertexShader( TLVERTEXFVF );
				lpD3DDevice->SetTexture( 1, NULL );
				lpD3DDevice->SetTexture( 0, m_pNumberTexture->GetTexture() );
				lpD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof( TLVertex ) );
			}
			break;

		case 10:						// Resist
			{
				float fRealX = 105.0f * fRate;
				float fRealY = 27.0f * fRate;

				float fPosX;
				switch(dwAlign)
				{
					case RYLDamageNode::DN_Center:
						fPosX = sPosX - fRealX / 2.0f;
						break;

					case RYLDamageNode::DN_Left:
						fPosX = sPosX - ( fRealX + (fRealX / 6.0f) );
						break;

					case RYLDamageNode::DN_Right:
						fPosX = sPosX + (fRealX / 6.0f);
						break;
				}
				float fPosY = sPosY - ( ( fRealY - 27.0f ) / 2.0f );

				TLVertex pVertices[ 4 ];
				pVertices[ 0 ].Diffuse.c = pVertices[ 1 ].Diffuse.c = pVertices[ 2 ].Diffuse.c = pVertices[ 3 ].Diffuse.c = 0xFFFFFFFF;
				pVertices[ 0 ].Diffuse.a = pVertices[ 1 ].Diffuse.a = pVertices[ 2 ].Diffuse.a = pVertices[ 3 ].Diffuse.a = cAlpha;
				pVertices[ 0 ].Specular.c = pVertices[ 1 ].Specular.c = pVertices[ 2 ].Specular.c = pVertices[ 3 ].Specular.c = 0;
				pVertices[ 0 ].w = pVertices[ 1 ].w = pVertices[ 2 ].w = pVertices[ 3 ].w = 0.1f;
				pVertices[ 0 ].v.z = pVertices[ 1 ].v.z = pVertices[ 2 ].v.z = pVertices[ 3 ].v.z = 0.1f;

				pVertices[ 0 ].v.x = pVertices[ 1 ].v.x = fPosX - 0.5f;
				pVertices[ 2 ].v.x = pVertices[ 3 ].v.x = fPosX + fRealX - 0.5f;
				pVertices[ 0 ].v.y = pVertices[ 2 ].v.y = fPosY - 0.5f;
				pVertices[ 1 ].v.y = pVertices[ 3 ].v.y = fPosY + fRealY - 0.5f;

				pVertices[ 0 ].tu = pVertices[ 1 ].tu = 0.0005f;
				pVertices[ 2 ].tu = pVertices[ 3 ].tu = 105 / 256.0f - 0.0005f;
				pVertices[ 0 ].tv = pVertices[ 2 ].tv = 223 / 256.0f - 0.0005f;
				pVertices[ 1 ].tv = pVertices[ 3 ].tv = 250 / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader( NULL );
				lpD3DDevice->SetVertexShader( TLVERTEXFVF );
				lpD3DDevice->SetTexture( 1, NULL );
				lpD3DDevice->SetTexture( 0, m_pNumberTexture->GetTexture() );
				lpD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof( TLVertex ) );
			}
			break;

		case 11:						// Miss
		case 12:
			{
				float fRealX = 88.0f * fRate;
				float fRealY = 24.0f * fRate;

				float fPosX = sPosX - fRealX / 2.0f;
				float fPosY = sPosY - ( ( fRealY - 24.0f ) / 2.0f );

				TLVertex pVertices[ 4 ];
				pVertices[ 0 ].Diffuse.c = pVertices[ 1 ].Diffuse.c = pVertices[ 2 ].Diffuse.c = pVertices[ 3 ].Diffuse.c = 0xFFFFFFFF;
				pVertices[ 0 ].Diffuse.a = pVertices[ 1 ].Diffuse.a = pVertices[ 2 ].Diffuse.a = pVertices[ 3 ].Diffuse.a = cAlpha;
				pVertices[ 0 ].Specular.c = pVertices[ 1 ].Specular.c = pVertices[ 2 ].Specular.c = pVertices[ 3 ].Specular.c = 0;
				pVertices[ 0 ].w = pVertices[ 1 ].w = pVertices[ 2 ].w = pVertices[ 3 ].w = 0.1f;
				pVertices[ 0 ].v.z = pVertices[ 1 ].v.z = pVertices[ 2 ].v.z = pVertices[ 3 ].v.z = 0.1f;

				pVertices[ 0 ].v.x = pVertices[ 1 ].v.x = fPosX - 0.5f;
				pVertices[ 2 ].v.x = pVertices[ 3 ].v.x = fPosX + fRealX - 0.5f;
				pVertices[ 0 ].v.y = pVertices[ 2 ].v.y = fPosY - 0.5f;
				pVertices[ 1 ].v.y = pVertices[ 3 ].v.y = fPosY + fRealY - 0.5f;

				pVertices[ 0 ].tu = pVertices[ 1 ].tu = 91 / 256.0f - 0.0005f;
				pVertices[ 2 ].tu = pVertices[ 3 ].tu = 163 / 256.0f - 0.0005f;
				pVertices[ 0 ].tv = pVertices[ 2 ].tv = ( 150 + ( ( wMode - 11 ) * 24.0f ) ) / 256.0f - 0.0005f;
				pVertices[ 1 ].tv = pVertices[ 3 ].tv = ( 150 + ( ( wMode - 11 ) * 24.0f + 24.0f ) ) / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader( NULL );
				lpD3DDevice->SetVertexShader( TLVERTEXFVF );
				lpD3DDevice->SetTexture( 1, NULL );
				lpD3DDevice->SetTexture( 0, m_pNumberTexture->GetTexture() );
				lpD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof( TLVertex ) );
			}
			break;

		default:						// Normal
			if( strcmp( strNum, "" ) )
			{
				float fRealX = 21.0f * fRate;
				float fRealY = 23.0f * fRate;

				int len = strlen( strNum );
				float fPosX;
				switch(dwAlign)
				{
					case RYLDamageNode::DN_Center:
						fPosX = sPosX - ( (len * fRealX) / 2.0f);
						break;

					case RYLDamageNode::DN_Left:
						fPosX = sPosX - ( (len * fRealX) + (fRealX / 2.0f) );
						break;

					case RYLDamageNode::DN_Right:
						fPosX = sPosX + (fRealX / 2.0f);
						break;
				}
				float fPosY = sPosY - ( ( fRealY - 23.0f ) / 2.0f );

				TLVertex pVertices[ 4 ];
				pVertices[ 0 ].Diffuse.c = pVertices[ 1 ].Diffuse.c = pVertices[ 2 ].Diffuse.c = pVertices[ 3 ].Diffuse.c = 0xFFFFFFFF;
				pVertices[ 0 ].Diffuse.a = pVertices[ 1 ].Diffuse.a = pVertices[ 2 ].Diffuse.a = pVertices[ 3 ].Diffuse.a = cAlpha;
				pVertices[ 0 ].Specular.c = pVertices[ 1 ].Specular.c = pVertices[ 2 ].Specular.c = pVertices[ 3 ].Specular.c = 0;
				pVertices[ 0 ].w = pVertices[ 1 ].w = pVertices[ 2 ].w = pVertices[ 3 ].w = 0.1f;
				pVertices[ 0 ].v.z = pVertices[ 1 ].v.z = pVertices[ 2 ].v.z = pVertices[ 3 ].v.z = 0.1f;

				pVertices[ 0 ].v.x = pVertices[ 1 ].v.x = fPosX - 0.5f;
				pVertices[ 2 ].v.x = pVertices[ 3 ].v.x = fPosX + fRealX - 0.5f;
				pVertices[ 0 ].v.y = pVertices[ 2 ].v.y = fPosY - 0.5f;
				pVertices[ 1 ].v.y = pVertices[ 3 ].v.y = fPosY + fRealY - 0.5f;

				pVertices[ 0 ].tv = pVertices[ 2 ].tv = ( wMode * 23.0f ) / 256.0f + 0.0005f;
				pVertices[ 1 ].tv = pVertices[ 3 ].tv = ( wMode * 23.0f + 23.0f ) / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader( NULL );
				lpD3DDevice->SetVertexShader( TLVERTEXFVF );
				lpD3DDevice->SetTexture( 1, NULL );
				lpD3DDevice->SetTexture( 0, m_pNumberTexture->GetTexture() );

				for( int i = 0; i < len; i++ )
				{
					pVertices[ 0 ].tu = pVertices[ 1 ].tu = ( ( strNum[ i ] - '0' ) * 21.0f ) / 256.0f + 0.0005f;
					pVertices[ 2 ].tu = pVertices[ 3 ].tu = ( ( strNum[ i ] - '0' ) * 21.0f + 21.0f ) / 256.0f - 0.0005f;
					lpD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof( TLVertex ) );

					pVertices[ 0 ].v.x += fRealX;
					pVertices[ 1 ].v.x += fRealX;
					pVertices[ 2 ].v.x += fRealX;
					pVertices[ 3 ].v.x += fRealX;
				}
			}
			break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLDamageEffect::ClearDamageEffect()
void RYLDamageEffect::ClearDamageEffect()
{               
    for_each( m_lstDamageEffect.begin(), m_lstDamageEffect.end(), ftDeleteDamageEffect );
    m_lstDamageEffect.clear();

    if( m_pNumberTexture )
    {
        delete m_pNumberTexture;
        m_pNumberTexture = NULL;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// RYLDamageEffect::