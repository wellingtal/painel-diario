
#include <deque>

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLClientMain.h"

#include <Creature/EnemyCheck.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Skill/SkillMgr.h>

#include "WinInput.h"
#include "GUITextEdit.h"
#include "GMFontTexture.h"

#include "RYLMessageBoxManager.h"
#include "RYLNoticeManager.h"
#include "RYLLabel.h"

#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h"
#include "RYLStringTable.h"
#include "RYLChattingDlg.h"
#include "RYLGameTimeManager.h"
#include "GMMemory.h"

void CRYLRaceBase::RenderStoneRespawn( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	if (CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE3 || // WAS ZONE3
		CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::BATTLE_ZONE)
	{
		if (g_pClientSocket->GetStatusFlag() == NS_CHARLOGIN || g_pClientSocket->GetStatusFlag() == NS_UPDATEADDRESS)
			return;

		// 명예의 석상 리스폰 창
		m_lstWindow[WINDOW_STONESTATUE]->Render(lpD3DDevice);	
	}
}

void CRYLRaceBase::RenderInterface( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	lpD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void CRYLRaceBase::RenderMsgBox( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	// Popup 렌더링
	if (m_bPopupShow)
	{
		RenderPopUp(lpD3DDevice);
	}

	// 메시지 박스 렌더링
	g_MessageBoxManager.Render(lpD3DDevice);

	CRYLGameData*		pGame = CRYLGameData::Instance();

	StringList strNotices ;
	DWORD dwColor ;
	if ( !pGame->m_bMovieVersion && CRYLNoticeManager::Instance()->GetNowNotice( strNotices, dwColor ) )
	{
		RECT rcRect ;
		unsigned long count = 0 ;
		StringList::iterator it ;
		for ( it = strNotices.begin() ; it != strNotices.end() ; ++it )
		{
			UINT	uiLeft = (g_ClientMain.m_iScreenWidth-503)/2 ;


			SetRect( &rcRect, uiLeft, count * 16, uiLeft + 503, count * 16 + 16 ) ;
			pGame->RenderRect( lpD3DDevice, rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, 0x00000000, 200 ) ;
			rcRect.left += 10 ; rcRect.right -= 10 ;

			CRYLLabel noticeLabel;
			noticeLabel.SetCaption((LPSTR)((*it).c_str()));
			noticeLabel.SetFontColor(dwColor);
			noticeLabel.SetAutoSize(TRUE);
			noticeLabel.GetFontTexture()->SetFontDrawType(CGMFontTexture::FontDrawType_Border);
			noticeLabel.DoDraw(NULL, rcRect, 0);

			++count ;
		}
	}

	// 스킬 게이지 렌더링
	if (!pGame->m_bMovieVersion)
	{
		RenderSkillGauge(lpD3DDevice);		
	}
}

void CRYLRaceBase::RenderEtcInterface( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if ( pGame->GetAlterSkillEnable() )
	{
		if ( pGame->GetAlterSkillType() == Skill::Type::CHANT || pGame->GetAlterSkillType() == Skill::Type::PASSIVE )		
		{
			return ;
		}
	}

	// edith 2008.01.25 화면중앙에 크로스헤어 포인터
	if (FALSE == pGame->m_bMouseMode)
	{
		BOOL bTarget = FALSE;

		if (NULL != pGame->m_csStatus.m_lpQuickSelected)
		{
			unsigned short wItemID = pGame->m_csStatus.m_lpQuickSelected->GetProtoTypeID();
			
			RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
			
			if (NULL == pSelfCreature)
			{
				return;
			}

			CItemInstance* lpItem = pGame->m_csStatus.m_lpQuickSelected;

#ifndef _NEW_KEY_
			// 기본적으로 CorssHair 이 화면중앙에 녹색, 타겟이 되면 붉은색,파란색등으로 
			// 나오기때문에 이 검색루틴이 필요없다.
			if (lpItem->GetItemInfo().m_UseItemInfo.m_cLimitRealmPoint > pSelfCreature->GetRealmPoint())
			{				
				return;
			}

			if (true == lpItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && 
				!CRYLGameTimeManager::Instance()->IsRealmWarTime())
			{			
				return;
			}

			if(lpItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && CRYLGameTimeManager::Instance()->IsRealmWarTime() && 
				pSelfCreature->GetRealmWarFlag() == Creature::WAR_OFF)
			{
				return;
			}
#endif

			// edith 2008.01.25 크로스 헤어용
			const Skill::ProtoType *pSkillType = g_SkillMgr.GetSkillProtoType(pGame->m_csStatus.m_lpQuickSelected->GetSkillID());	
			if (NULL != pSkillType)
			{
				switch (pSkillType->m_eTargetType)
				{
					case Skill::Target::DEAD_ENEMY:
					case Skill::Target::FRIEND:
					case Skill::Target::ENEMY:
					case Skill::Target::DEAD_FRIEND:
					case Skill::Target::FRIEND_OBJECT:
					case Skill::Target::ENEMY_OBJECT:
					case Skill::Target::PARTY:
					case Skill::Target::FRIEND_EXCEPT_SELF:
					{
						if (pSkillType->m_fMaxRange != 0.0f) 
						{
							bTarget = TRUE;
						}
						break;
					}
				}
			}
		}

		if (TRUE == g_CharacterData.m_bSummonCmd || TRUE == bTarget || TRUE == pGame->m_csStatus.m_bRangedWeapon)
		{
			// RenderCrossHair의 두번째 인자
			// 0 - 녹색 (중립)
			// 1 - 빨간색 (적군)
			// 2 - 파란색 (아군)

			// 소환수
			if (TRUE == g_CharacterData.m_bSummonCmd)
			{
				POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
				unsigned long dwTargetID = pGame->GetScreenPosChr(*ptMousePos, Skill::Target::ENEMY, 2000.0f);

				if (dwTargetID != 0xFFFFFFFF)
				{
					if (EnemyCheck::EC_ENEMY == CRYLGameData::GetClientEnemyType(dwTargetID))
					{
						RenderCrossHair(lpD3DDevice, 1);

						if (g_DeviceInput.GetIsLeftMouseDown())
						{
							CRYLNetworkData::SendSummonAttack(dwTargetID);
						}
						return;
					} 
				}
			} 

			// 원거리 스킬 타겟
//			m_dwSpecialTargetChrIDTemp = m_dwSpecialTargetChrID;
			if (0xFFFFFFFF != m_dwNormalTargetChrID)
			{
				switch (CRYLGameData::GetClientEnemyType(m_dwNormalTargetChrID))
				{
					case EnemyCheck::EC_ENEMY:	RenderCrossHair(lpD3DDevice, 1);	return;
					case EnemyCheck::EC_FRIEND:	RenderCrossHair(lpD3DDevice, 2);	return;
				}
			} 

			// 원거리 무기 타겟
			if (TRUE == pGame->m_csStatus.m_bRangedWeapon && 0xFFFFFFFF != m_dwRangedWeaponTargetChrID)
			{
				if (EnemyCheck::EC_ENEMY == CRYLGameData::GetClientEnemyType(m_dwRangedWeaponTargetChrID))
				{
					RenderCrossHair(lpD3DDevice, 1);
					return;
				}
			}
#ifndef _NEW_KEY_
			RenderCrossHair(lpD3DDevice, 0);
#endif
		}
#ifdef _NEW_KEY_
		// edith 2008.01.25 키보드모드이면 기본적으로 녹색 크로스해어를 출력해준다.
		if (0xFFFFFFFF != m_dwNormalTargetChrID)
		{
			switch (CRYLGameData::GetClientEnemyType(m_dwNormalTargetChrID))
			{
				case EnemyCheck::EC_ENEMY:	RenderCrossHair(lpD3DDevice, 0);	return;
				case EnemyCheck::EC_FRIEND:	RenderCrossHair(lpD3DDevice, 2);	return;
			}
		} 
		else
			RenderCrossHair(lpD3DDevice, 0);
#endif
	}
}

