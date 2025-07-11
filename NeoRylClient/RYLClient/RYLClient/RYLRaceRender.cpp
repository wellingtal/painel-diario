
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLClientMain.h"

#include <Creature/Character/CharacterClass.h>

#include "BaseGraphicsLayer.h"

#include "GUITextEdit.h"
#include "GMFontTexture.h"
#include "GMUITooltipManager.h"

#include "RYLLabel.h"
#include "RYLTimer.h"
#include "RYLStringTable.h"
#include "RYLMessageBoxManager.h"

#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h"
#include "GMMemory.h"

void CRYLRaceBase::RenderChrSelect(LPDIRECT3DDEVICE8 lpD3DDevice) 
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	lpD3DDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	lpD3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	lpD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	pGame->m_lpSceneManager->Render();

	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	lpD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	lpD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	lpD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (pGame->m_dwClientState == CHRSELECT_SELECT)
	{
		if (GameRYL::KOREA == CRYLNetworkData::Instance()->m_eInternationalCode && 
			GameRYL::SERVER_BATTLE_LOHAN != CRYLNetworkData::Instance()->m_eServerType)
		{
			char strText[MAX_PATH];
			switch (CRYLNetworkData::Instance()->m_cBillingUser)
			{
				// 개인
				case 0:
				{
					switch (CRYLNetworkData::Instance()->m_cBillingType)
					{
						case 'D':
						case 'E':
						{
							if ( GRYLTimer.m_tmEndTime.tm_year + 1900 > 2019)
							{
								strcpy(strText, CRYLStringTable::m_strString[967]);
							} 
							else
							{
								sprintf(strText, CRYLStringTable::m_strString[962], 
									GRYLTimer.m_tmEndTime.tm_year + 1900, GRYLTimer.m_tmEndTime.tm_mon + 1, GRYLTimer.m_tmEndTime.tm_mday);
							}
							break;
						}

						case 'T':
						{
							sprintf(strText, CRYLStringTable::m_strString[963], 
								(unsigned long)( pGame->m_nPlayTime / 60), (unsigned long)( pGame->m_nPlayTime % 60));
							break;
						}

						case 'N':
						case 'F':
						{
							sprintf(strText, CRYLStringTable::m_strString[964], 
								GRYLTimer.m_tmEndTime.tm_year + 1900, GRYLTimer.m_tmEndTime.tm_mon + 1, GRYLTimer.m_tmEndTime.tm_mday);
							break;
						}

						case 'B':
						{
							sprintf(strText, "");
							break;
						}
					}
					break;
				}

				// PC방
				case 1:	
				{
					switch (CRYLNetworkData::Instance()->m_cBillingType)
					{
						case 'D':
						case 'E':
						{
							sprintf(strText, CRYLStringTable::m_strString[965], 
								GRYLTimer.m_tmEndTime.tm_year + 1900, GRYLTimer.m_tmEndTime.tm_mon + 1, GRYLTimer.m_tmEndTime.tm_mday);
							break;
						}

						case 'T':
						{
							sprintf(strText, CRYLStringTable::m_strString[966], 
								(unsigned long)(pGame->m_nPlayTime / 60), (unsigned long)(pGame->m_nPlayTime % 60));
							break;
						}

						case 'N':
						case 'F':
						{
							sprintf(strText, CRYLStringTable::m_strString[964], 
								GRYLTimer.m_tmEndTime.tm_year + 1900, GRYLTimer.m_tmEndTime.tm_mon + 1, GRYLTimer.m_tmEndTime.tm_mday);
							break;
						}

						case 'S':
						{
							sprintf(strText, "색다른 PC방 - 쨈존에서 로그인 하셨습니다." ) ;
							break;
						}
					}
					break;
				}
			}

			pGame->RenderRect(lpD3DDevice, 1, 10, g_ClientMain.m_iScreenWidth - 1, 26, 0x00000000, 200);

			RECT rt;
			CRYLLabel noticeLabel;
			noticeLabel.SetCaption(strText);
			noticeLabel.SetFontColor(D3DCOLOR_RGBA(255, 0, 255, 255));
			SetRect(&rt, 1, 10, g_ClientMain.m_iScreenWidth - 1, 26);
			noticeLabel.SetAlignRect(rt);
			rt = noticeLabel.SetAlign(CENTER);
			noticeLabel.SetAutoSize(TRUE);
			noticeLabel.GetFontTexture()->SetFontDrawType(CGMFontTexture::FontDrawType_Border);
			noticeLabel.DoDraw(NULL, rt, 0);
		}
	}
}

void CRYLRaceBase::RenderGame(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	SetRenderState( lpD3DDevice ) ;

	if (pGame->m_dwClientState == GAME_PLAYING) // Minotaurs 게임 랜더
	{
		RenderScene( lpD3DDevice ) ;

		RenderHead( lpD3DDevice ) ;

		// 일반 타겟팅 표시
		RenderNormalTarget() ;

		// 스킬 타겟팅 표시
		RenderSkillTarget() ;

		// 휴먼만 사용
		if ( CClass::RaceType::HUMAN == pGame->m_dwClientRace )
		{
			RenderCrossHair(lpD3DDevice ) ;
		}

		// 배틀 그라운드에서의 석상 랜더링
		RenderStoneRespawn( lpD3DDevice ) ;

		// 인터페이스 랜더링
		RenderInterface( lpD3DDevice ) ;

		// 메시지창, 팝업창, 주운 아이템등.. 랜더링
		RenderMsgBox( lpD3DDevice ) ;
	}
}

void CRYLRaceBase::FinalRenderGame(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	if (pGame->m_dwClientState == GAME_PLAYING)
	{
		// 자잘한 인터페이스 랜더링
		RenderEtcInterface( lpD3DDevice ) ;

		// Pick한 아이템 랜더링
		if (pGame->m_lpPickItem)// && !g_MessageBoxManager.GetIsExistModal() )
		{
			pGame->m_lpPickItem->Render(lpD3DDevice);
		}

		if ( GUITooltipMgr.m_ttRenderTarget )
		{
			GUITooltipMgr.m_ttRenderTarget->Render( lpD3DDevice ) ;
		}
	}
}

void CRYLRaceBase::SetRenderState( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	lpD3DDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	lpD3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	lpD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CRYLRaceBase::RenderScene( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	pGame->m_lpSceneManager->RenderInCubemap();
	pGame->m_lpSceneManager->Render();

	g_CharacterData.Render(BaseGraphicsLayer::GetDevice());

	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	lpD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	lpD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	lpD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}

