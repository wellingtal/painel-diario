
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLClientMain.h"

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Skill/SkillMgr.h>

#include <Creature/Monster/MonsterMgr.h>
#include <Creature/EnemyCheck.h>

#include "GUITextEdit.h"

#include "RYLProfile.h"
#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "RYLRaceBase.h"

#include "RYLChattingDlg.h"
#include "RYLGameTimeManager.h"
#include "GMMemory.h"

void CRYLRaceBase::RenderCrossHair(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned long dwMode)
{
	TLVertex pVertices[4];
	pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
	pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = 0xFF;
	pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
	pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
	pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

	INT iTempTargetX = ( g_ClientMain.m_iScreenWidth / 2 ) - 7;
	INT iTempTargetY = ( g_ClientMain.m_iScreenHeight / 2 ) - 57;

	pVertices[0].v.x = pVertices[1].v.x = iTempTargetX - 0.5f;
	pVertices[2].v.x = pVertices[3].v.x = ( iTempTargetX + 14 ) - 0.5f;
	pVertices[0].v.y = pVertices[2].v.y = iTempTargetY - 0.5f;
	pVertices[1].v.y = pVertices[3].v.y = ( iTempTargetY + 14 ) - 0.5f;

	pVertices[0].tu = pVertices[1].tu = (1 + dwMode * 15) / 128.0f - 0.0005f;
	pVertices[2].tu = pVertices[3].tu = (1 + dwMode * 15 + 14) / 128.0f - 0.0005f;
	pVertices[0].tv = pVertices[2].tv = 41 / 64.0f - 0.0005f;
	pVertices[1].tv = pVertices[3].tv = 55 / 64.0f - 0.0005f;

	lpD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) ;
	lpD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ;
	lpD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ;

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ) ;
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) ;

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE ) ;
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ) ;
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ) ;

	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(1, NULL);
	lpD3DDevice->SetTexture(0, m_lpCommonWindow->GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
}

void CRYLRaceBase::RenderCrossHair( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	PROFILE("Crosshair");

	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	if (!pGame->m_bMovieVersion)
	{
		if (pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos])
		{
			unsigned long dwWeaponType = 
				pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos]->GetItemType();

			if (dwWeaponType == Item::ItemType::BOW || dwWeaponType == Item::ItemType::CROSSBOW)
			{
				CItemInstance* lpAmmo = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1 + pGame->m_csStatus.m_cWeaponPos];

				// edith 2008.02.05 ¿‹≈∫∫Œ¡∑ «•Ω√¿ÃπÃ¡ˆ
				if(NULL == lpAmmo)
				{
					m_lpQuiverD->Render(lpD3DDevice);				// ¿‹≈∫¿Ã æ¯¿ª∂ß ≥Ω¥Ÿ.
				}
				else if ( (lpAmmo->GetItemType() == Item::ItemType::ARROW || lpAmmo->GetItemType() == Item::ItemType::BOLT) )
				{
//					if (CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE3 || CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::BATTLE_ZONE)
//						m_lpQuiverD->Render(lpD3DDevice, 0xFF, -63, 0);	// ¿‹≈∫¿Ã æ¯¿ª∂ß ≥Ω¥Ÿ.

					// edith 2008.02.05 ¿‹≈∫∫Œ¡∑ «•Ω√¿ÃπÃ¡ˆ
					if(lpAmmo->GetNowDurability() < 0)	// ¿‹≈∫ ∫Œ¡∑//hayzohar was <30
					{
						m_lpQuiver->Render(lpD3DDevice);				// ¿‹≈∫¿Ã ∫Œ¡∑«“∂ß ≥Ω¥Ÿ.
					}
				}
			}
		}
	}
}

void CRYLRaceBase::RenderNormalTarget( void )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	if (!pGame->m_bMovieVersion && m_dwNormalTargetChrID != 0xFFFFFFFF)
	{
		vector3* vecChrPos = g_CharacterData.GetFindChrPos( m_dwNormalTargetChrID );

		if ( CRYLGameData::GetClientEnemyType(m_dwNormalTargetChrID) == EnemyCheck::EC_ENEMY )
		{
			pGame->m_pPlayerSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
			pGame->m_pPlayerSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
		} 
		else
		{
			if (vecChrPos)
			{
				// Minotaurs ∫˚¿Ã π›¬¶~ 
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(m_dwNormalTargetChrID);
				CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
				if ( pTempModel )
				{
					vector3 vecLightChrPos = *pTempCreature->GetPosition();
					CSceneManager::AddChrLight( pTempModel, vecLightChrPos + vector3( 0.0f, 200.0f, 0.0f ), D3DXCOLOR(0.f,1.f,0.f,1.f), 500.0f, 6 );
				}
				/*pGame->m_pPlayerSelectEff->SetStartPos(vecChrPos->x, vecChrPos->y + 30.0f, vecChrPos->z);
				pGame->m_pPlayerSelectEff->SetEndPos(vecChrPos->x, vecChrPos->y + 30.0f, vecChrPos->z);*/
			} 
			else
			{
				pGame->m_pPlayerSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
				pGame->m_pPlayerSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
			}
		}
	} 
	else
	{
		pGame->m_pPlayerSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
		pGame->m_pPlayerSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
	}
}

void CRYLRaceBase::RenderSkillTarget( void )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	if (true == pGame->m_bMovieVersion || 
		0xFFFFFFFF == m_dwSpecialTargetChrID || 
		NULL == pGame->m_csStatus.m_lpQuickSelected || 
		0 != pGame->m_csStatus.m_lpQuickSelected->m_lCoolDownTimeCount)
	{
		pGame->m_pEnemySkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
		pGame->m_pEnemySkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
		pGame->m_pPlayerSkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
		pGame->m_pPlayerSkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);

		return;
	}

	vector3* vecChrPos = g_CharacterData.GetFindChrPos(m_dwSpecialTargetChrID);
	if (vecChrPos)
	{
		if ( CRYLGameData::GetClientEnemyType(m_dwSpecialTargetChrID) == EnemyCheck::EC_ENEMY )
		{
			pGame->m_pEnemySkillSelectEff->SetStartPos(vecChrPos->x, vecChrPos->y, vecChrPos->z);
			pGame->m_pEnemySkillSelectEff->SetEndPos(vecChrPos->x, vecChrPos->y, vecChrPos->z);
			pGame->m_pPlayerSkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
			pGame->m_pPlayerSkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);

			if ( CRYLGameData::GetClientIdentity( m_dwSpecialTargetChrID ) == Creature::CT_MONSTER )
			{
				const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType(m_dwSpecialTargetChrID & 0x0000FFFF);
				if (NULL != pMonster)
				{
					pGame->m_pEnemySkillSelectEff->SetScale( pMonster->m_MonsterInfo.m_fSize * pMonster->m_MonsterInfo.m_fSkillEffectSize );
				}
			}
		} 
		else
		{
			// ¿⁄±‚ ¡ﬂΩ…¿« π¸¿ß ∞¯∞›
			const Skill::ProtoType *pSkillType = g_SkillMgr.GetSkillProtoType(pGame->m_csStatus.m_lpQuickSelected->GetSkillID());	

			// æ◊º« Ω∫≈≥¿Ã∏È πﬂπÿø° ¿Ã∆—∆Æ ≤Ù±‚.
			if(NULL != pSkillType && (Skill::Type::ACTION == pSkillType->m_eSkillType || Skill::Type::GATHER == pSkillType->m_eSkillType))
			{
				pGame->m_pEnemySkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
				pGame->m_pEnemySkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
				pGame->m_pPlayerSkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
				pGame->m_pPlayerSkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);

				return;
			}

			if (NULL != pSkillType && Skill::Target::ENEMY == pSkillType->m_eTargetType && 
				0 == pSkillType->m_fMinRange && 0 == pSkillType->m_fMaxRange)
			{
				pGame->m_pEnemySkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
				pGame->m_pEnemySkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
				pGame->m_pPlayerSkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
				pGame->m_pPlayerSkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
			}
			else
			{
				pGame->m_pEnemySkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
				pGame->m_pEnemySkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
				pGame->m_pPlayerSkillSelectEff->SetStartPos(vecChrPos->x, vecChrPos->y, vecChrPos->z);
				pGame->m_pPlayerSkillSelectEff->SetEndPos(vecChrPos->x, vecChrPos->y, vecChrPos->z);
			}
		}
	} 
	else
	{
		pGame->m_pEnemySkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
		pGame->m_pEnemySkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
		pGame->m_pPlayerSkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
		pGame->m_pPlayerSkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
	}
}

void CRYLRaceBase::RenderSkillGauge(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
	if ( !pSelfData )
		return;

	if ( pSelfCreature->GetStillCasting() )
	{
		const Skill::ProtoType* pSkillProtoType = 
			g_SkillMgr.GetSkillProtoType(CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetSkillID());
		if (NULL != pSkillProtoType) 
		{
			const Skill::ProtoType* pSkillType = &pSkillProtoType[g_CharacterData.m_dwCastingSkillGrade];
			if (NULL != pSkillType)
			{
				unsigned long dwLevelTick = pSkillType->m_LevelTick;
				int nLevel = 0;
				if (0 != dwLevelTick)
				{
					nLevel = g_CharacterData.m_dwCastingSkillTick / dwLevelTick;
					if ( nLevel > CSkillMgr::MAX_SKILL_LEVEL )
						nLevel = CSkillMgr::MAX_SKILL_LEVEL;
				} 
				else
				{
					bool bChk = false;

					CRYLGameData* pGame = CRYLGameData::Instance();

					if(pGame->m_csStatus.m_lpQuickSelected)
					{
						unsigned short wItemID = pGame->m_csStatus.m_lpQuickSelected->GetProtoTypeID();

						RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

						if (pSelfCreature)
						{
							CItemInstance* lpItem = pGame->m_csStatus.m_lpQuickSelected;

							if (lpItem->GetItemInfo().m_UseItemInfo.m_cLimitRealmPoint > pSelfCreature->GetRealmPoint())
							{							
								bChk = true;
							}

							if (true == lpItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && 
								!CRYLGameTimeManager::Instance()->IsRealmWarTime())
							{								
								bChk = true;
							}

							if(lpItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && CRYLGameTimeManager::Instance()->IsRealmWarTime() && 
								pSelfCreature->GetRealmWarFlag() == Creature::WAR_OFF)
							{
								bChk = true;
							}
						}						
					}

					g_CharacterData.m_bSkillExecuted = FALSE;

					pSelfCreature->CancelSkill();
					pSelfCreature->SetStillCasting( false );
					pSelfCreature->m_ulSkillComboCount = 0;
					pSelfCreature->m_bSkillCombo = false;

					if(bChk)
					{
						return;
					}
                    
					if ( Skill::Type::PASSIVE != pSkillType->m_eSkillType &&
						 Skill::Type::ACTION != pSkillType->m_eSkillType &&
						 Skill::Type::GATHER != pSkillType->m_eSkillType)
						 
					{
						CRYLMessageBox* pMessageBox = new CRYLMessageBox;
						pMessageBox->Create(CRYLStringTable::m_strString[19]);
					}
					
					return;
				}

				// edith Ω∫≈≥ ƒ≥Ω∫∆√ πŸ.
				m_lpSkillGauge[g_CharacterData.m_dwCastingSkillGrade]->Render(lpD3DDevice);

				m_lpSkillGaugeFrame->Render(lpD3DDevice);

				RECT rcRect;
				char strTemp[10];
				SetRect(&rcRect, m_iGateW+172, m_iGateH+14, m_iGateW+181, m_iGateH+25);
				sprintf(strTemp, "%d", nLevel);
				// ¿”Ω√
//				char strTemp[128];
//				SetRect(&rcRect, m_iGateW+172, m_iGateH+34, m_iGateW+381, m_iGateH+55);
//				sprintf(strTemp, "Grade %d, Tick : %d", g_CharacterData.m_dwCastingSkillGrade, g_CharacterData.m_dwCastingSkillTick);
				g_TextEdit.DrawText(strTemp, &rcRect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255));
			}
		}
	}
}

