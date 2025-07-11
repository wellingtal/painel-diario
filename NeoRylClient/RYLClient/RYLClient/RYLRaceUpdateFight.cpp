
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include <Skill/SkillMgr.h>
#include <Creature/Character/CharacterClass.h>

#include "RYLCreature.h"

#include "CEffScript.h"
#include "FrameTimer.h"

#include "RYLTimer.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLQuickSlotDlg.h"

#include "RYLRaceBase.h"
#include "GMMemory.h"

#include "RYLJoystickConfigurePanel.h"
#include "RYLJoystickConfigureDlg.h"

void CRYLRaceBase::UpdateGameStillCasting( CHAR* lpszFileName )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
	if ( !pSelfData )
		return;

	CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
	if ( !pSelfModel )
		return;

	CRYLGameScene*				pScene			= (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
	CRYLJoystickConfigureDlg*	pJoystickDlg	= (CRYLJoystickConfigureDlg*)pScene->GetJoystickConfigureDlg() ;
	CRYLJoystickConfigurePanel* pJoystickPanel	= (CRYLJoystickConfigurePanel*)pJoystickDlg->GetRYLJoystickPanel();

	// 스킬 캐스팅 처리
	if (TRUE == pSelfCreature->GetStillCasting())
	{
		unsigned short wSkill = pGame->m_csStatus.m_lpQuickSelected->GetSkillID();			// 시전중인 스킬ID
		unsigned long wLock = pGame->m_csStatus.m_lpQuickSelected->GetSkillLockCount();		// 스킬 락 카운트
		unsigned long wLevel = pGame->m_csStatus.m_lpQuickSelected->GetNowDurability();		// 스킬 레벨

		// 캐스팅 시작시
		if (NULL == m_pCastingEffect) 
		{	
			// edith
			pGame->m_csStatus.CheckUseSkillforMP(wLock, wLevel);
			if ( (wLock == 0) && (wLevel == 0 ) )
			{
				g_CharacterData.m_dwCastingSkillTick = 0;
				g_CharacterData.m_dwCastingSkillGrade = 0;
				return;
			}
			m_pCastingEffect = new CEffScript;

			if (NULL != pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
			{
				m_pCastingEffect->SetSkillInfo(pGame->m_csStatus.m_Info.Sex, 
					pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType());
			} 
			else
			{
				m_pCastingEffect->SetSkillInfo(pGame->m_csStatus.       m_Info.Sex, 0);
			}

			switch (wSkill) 
			{
				case 0x8803:	// 에임드 샷
				case 0x9506:	// 블래스트
					m_pCastingEffect->GetScriptBinData(lpszFileName, pGame->m_bSelfEffect);
					m_pCastingEffect->SetCast(true);
					break;

				default:
					m_pCastingEffect->GetScriptBinData("casting_acol.esf", pGame->m_bSelfEffect);
					break;
			}

			D3DXVECTOR3 effstartpos( pSelfCreature->GetPosition()->x, 
				pSelfCreature->GetPosition()->y, 
				pSelfCreature->GetPosition()->z);	

			float fChrDir = pSelfCreature->GetDirection();
			fChrDir = -fChrDir;
			fChrDir -= FLOAT_PHI / 2.0f;

			vector3 vecChrToward;
			vecChrToward.x = cosf(fChrDir);
			vecChrToward.z = sinf(fChrDir);	
			vecChrToward.y = 0.0f;
			vecChrToward.Normalize();
			m_pCastingEffect->SetChrFwd(-vecChrToward.x, -vecChrToward.y, -vecChrToward.z);

			if ( CClass::RaceType::AKHAN == pGame->m_dwClientRace )
			{
				// 오른손 위치
				CZ3DObject *snappoint = NULL;
				float body_scale = 0.0f;
				vector3 r_hand(0.0f,0.0f,0.0f);

				if ( pGame->m_csStatus.m_lpQuickSelected->GetSkillID() == 0x9506 )
				{		
					snappoint = pSelfModel->GetSkeletonPartObject("L_HAND");
					body_scale = pSelfModel->GetScale();

					if ( snappoint != NULL )
					{
						vector3 snaptm;
						snappoint->GetPosition(snaptm);

						r_hand.x = snaptm.x;
						r_hand.y = snaptm.y;
						r_hand.z = snaptm.z;

						r_hand *= body_scale;
					}
				}

				m_pCastingEffect->SetStartPos(effstartpos.x + r_hand.x,effstartpos.y + r_hand.y,effstartpos.z + r_hand.z);
			}
			else
			{
				m_pCastingEffect->SetStartPos(effstartpos.x, effstartpos.y, effstartpos.z);
			}

			m_pCastingEffect->SetEndPos(effstartpos.x, effstartpos.y, effstartpos.z);

			CSceneManager::AddEffectScript(m_pCastingEffect);

			// 캐스팅을 서버에 알린다.
			// 이건 다른 컴에서 현재 캐릭의 동작을 보여주기위한 패킷 캐스팅 시작.
			unsigned long aryDefenser[AtNode::MAX_DEFENDER_NUM] = { 0, };
			aryDefenser[0] = m_dwSpecialTargetChrID;

			CRYLNetworkData::SendCharAttack( *( pSelfCreature->GetPosition() ),
				pSelfCreature->GetDirection() - FLOAT_PHI / 2.0f,
				wSkill, true, wLock, wLevel, 0, 1, aryDefenser) ;
		}
		else 
		{
			D3DXVECTOR3 effstartpos( pSelfCreature->GetPosition()->x, 
				pSelfCreature->GetPosition()->y, 
				pSelfCreature->GetPosition()->z);	

			float fChrDir = pSelfCreature->GetDirection();
			fChrDir = -fChrDir;
			fChrDir -= FLOAT_PHI / 2.0f;

			vector3 vecChrToward;
			vecChrToward.x = cosf(fChrDir);
			vecChrToward.z = sinf(fChrDir);	
			vecChrToward.y = 0.0f;
			vecChrToward.Normalize();
			m_pCastingEffect->SetChrFwd(-vecChrToward.x, -vecChrToward.y, -vecChrToward.z);

			if ( CClass::RaceType::AKHAN == pGame->m_dwClientRace )
			{
				// 오른손 위치
				CZ3DObject *snappoint = NULL;
				float body_scale = 0.0f;
				vector3 r_hand(0.0f,0.0f,0.0f);

				if ( pGame->m_csStatus.m_lpQuickSelected->GetSkillID() == 0x9506 ) 
				{		
					snappoint = pSelfModel->GetSkeletonPartObject("L_HAND");
					body_scale = pSelfModel->GetScale();

					if ( snappoint != NULL )
					{
						vector3 snaptm;
						snappoint->GetPosition(snaptm);

						r_hand.x = snaptm.x;
						r_hand.y = snaptm.y;
						r_hand.z = snaptm.z;

						r_hand *= body_scale;
					}
				}

				m_pCastingEffect->SetStartPos(effstartpos.x + r_hand.x,effstartpos.y + r_hand.y,effstartpos.z + r_hand.z);
			}
			else
			{
				m_pCastingEffect->SetStartPos(effstartpos.x,effstartpos.y,effstartpos.z);
			}

			m_pCastingEffect->SetEndPos(effstartpos.x,effstartpos.y,effstartpos.z);
		}

		unsigned long dwLevelTick = 0;

		const Skill::ProtoType *lpSkillTypeTemp = g_SkillMgr.GetSkillProtoType(wSkill);
		const Skill::ProtoType *lpSkillType = NULL;
		const Skill::ProtoType *lpNextSkillType = NULL;

		if (NULL != lpSkillTypeTemp) 
		{
			lpSkillType = &lpSkillTypeTemp[g_CharacterData.m_dwCastingSkillGrade];
			if ( g_CharacterData.m_dwCastingSkillGrade < wLock )
				lpNextSkillType = &lpSkillTypeTemp[g_CharacterData.m_dwCastingSkillGrade+1 ];
		}

		if (NULL != lpSkillType)
		{
			pGame->m_csStatus.CheckUseSkillforStatus(wLock, wLevel);
			pGame->m_csStatus.CheckUseSkillforMP(wLock, wLevel);

			unsigned long level = 0;
			if (wLock == g_CharacterData.m_dwCastingSkillGrade)
			{
				level = wLevel;
			}
			else if (wLock > g_CharacterData.m_dwCastingSkillGrade)
			{
				level = 6;
			}

			dwLevelTick = lpSkillType->m_LevelTick ;

			if (pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Frozen ))
			{
				g_CharacterData.m_dwCastingSkillTick = (timeGetTime() - g_CharacterData.m_dwStartSkillTick) ; // was /2 hayzohar 
			}
			else
			{
				g_CharacterData.m_dwCastingSkillTick = timeGetTime() - g_CharacterData.m_dwStartSkillTick;//something here for cooldown hack?
			}

			// 현재 레벨 소요 시간 이상으로 캐스팅 중일때
			if (g_CharacterData.m_dwCastingSkillTick > level * dwLevelTick)
			{
				// 다음 스킬있다면 다음 스킬로 넘어 간다.
				// 아니면 멈춰 있음.
				if (g_CharacterData.m_dwCastingSkillGrade < wLock)
				{
					bool bNextGrade = false;
					if ( lpNextSkillType )
					{
						if ( g_CharacterData.m_dwCastingSkillTick > 
							( CSkillMgr::MAX_SKILL_LEVEL * dwLevelTick + lpNextSkillType->m_StartTick ) )
						{
							bNextGrade = true ;
						}
					}
					else
					{
						bNextGrade = true ;
					}

					if ( bNextGrade )
					{
						if (pJoystickPanel->GetForcefeedback())
						{
							// ForceFeedback : 락카운트 증가
							KeyControl::StartForceFeedback(2000, 2000, 100);
						}

						g_CharacterData.m_dwCastingSkillGrade++;
						g_CharacterData.m_dwStartSkillTick = timeGetTime();
						g_CharacterData.m_dwCastingSkillTick = 0;

						// 그레이드 증가시 마다 튀겨주는 effect
						CEffScript* test_script = new CEffScript;			

						// 시전하고 있는 스킬 ID
						switch ( wSkill ) 
						{	
							case 0x8803:	// 에임드 샷
								test_script->GetScriptBinData("Archer_up.esf", pGame->m_bSelfEffect);
								break;

							case 0x9506:	// 블래스트
							default:
								test_script->GetScriptBinData("casting_acol_up.esf", pGame->m_bSelfEffect);
								break;
						}

						D3DXVECTOR3 startpos( pSelfCreature->GetPosition()->x, 
							pSelfCreature->GetPosition()->y, 
							pSelfCreature->GetPosition()->z);	

						test_script->SetStartPos(startpos.x,startpos.y,startpos.z);
						test_script->SetEndPos(startpos.x,startpos.y,startpos.z);
						CSceneManager::AddEffectScript(test_script);
					}
				} 
				else
				{
					g_CharacterData.m_dwCastingSkillTick = level * dwLevelTick;
					g_CharacterData.m_dwStartSkillTick = timeGetTime() - g_CharacterData.m_dwCastingSkillTick ;
				}
			}
			else
			{
				if (pJoystickPanel->GetForcefeedback())
				{
					// ForceFeedback : 스킬 캐스팅 중
					KeyControl::StartForceFeedback(100, 100, 100);
				}
			}

			m_lpSkillGauge[0]->SetRate(0.0f);
			m_lpSkillGauge[1]->SetRate(0.0f);
			m_lpSkillGauge[2]->SetRate(0.0f);
			m_lpSkillGauge[3]->SetRate(0.0f);
			m_lpSkillGauge[4]->SetRate(0.0f);

			m_lpSkillGauge[g_CharacterData.m_dwCastingSkillGrade]->SetRate(
				(g_CharacterData.m_dwCastingSkillTick / (float)(dwLevelTick * CSkillMgr::MAX_SKILL_LEVEL)));
		}
	}
	else 
	{
		// 캐스팅 끝일때 effect 해제 
		if (NULL != m_pCastingEffect) 
		{ 
			CSceneManager::m_EffectManager.DeleteEndScript(m_pCastingEffect);
			m_pCastingEffect = NULL;
		}
	}
}

void CRYLRaceBase::UpdateGameCoolDownTimeCheck( void ) 
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	for (list<CItemInstance *>::iterator it = pGame->m_csStatus.m_lstInventory.begin(); 
		it != pGame->m_csStatus.m_lstInventory.end(); ++it)
	{
		if ((*it)->m_lCoolDownTimeCount)
		{
			(*it)->UpdateCoolDownTime();
		}
	}

	for (vector<CItemInstance *>::iterator itSkill = pGame->m_csStatus.m_lstClassSkill.begin(); 
		itSkill != pGame->m_csStatus.m_lstClassSkill.end(); ++itSkill)
	{
		if ((*itSkill)->m_lCoolDownTimeCount)
		{
			(*itSkill)->UpdateCoolDownTime();
		}
	}

	unsigned long i = 0;

	for (i = 0; i < SKILL::MAX_SLOT_NUM; i++)
	{
		if (pGame->m_csStatus.m_lstSkillSlot[i])
		{
			if (pGame->m_csStatus.m_lstSkillSlot[i]->m_lCoolDownTimeCount)
			{
				pGame->m_csStatus.m_lstSkillSlot[i]->UpdateCoolDownTime();
			}
		}
	}

	// 소셜모션
	if(!pGame->m_csStatus.m_lstSocialSlot.empty())
	{
		unsigned long iSize = (unsigned long)pGame->m_csStatus.m_lstSocialSlot.size();
		for(i = 0; i < iSize; ++i)
		{
			if (pGame->m_csStatus.m_lstSocialSlot[i]->m_lCoolDownTimeCount)
			{
				pGame->m_csStatus.m_lstSocialSlot[i]->UpdateCoolDownTime();
			}
		}
	}


	for (i = 0; i < QUICK::MAX_QUICK_NUM; i++)
	{
		if (pGame->m_csStatus.m_lstQuickSlot[i])
		{
			CRYLGameScene*		pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
			CRYLQuickSlotDlg*	pQuickDlg	= ( CRYLQuickSlotDlg* )pScene->GetQuickSlotDlg();

			if (pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount && pGame->m_csStatus.m_lstQuickSlot[i]->UpdateCoolDownTime())
			{
				if (pGame->m_dwStartQuickSlot <= i && i < 10 + pGame->m_dwStartQuickSlot)
				{	
					// 이펙트
					char light[256] = { 0, };
					int pos_num = 0;

					if ( i >= 10 )
						pos_num = i - 10;
					else
						pos_num = i;

					if ( ( pos_num >= 0 ) && ( pos_num <= 10 ) ) 
					{
						if ( pQuickDlg->GetVisible() )
						{
							sprintf(light, "inter_light%d.esf", pos_num + 1);
							CEffScript* test_script = new CEffScript;
							test_script->SetInterfaceSet(true);
							test_script->GetScriptBinData(light);	

							// 퀵슬롯 시작위치 
							if ( pQuickDlg->GetVerticalEnable() )
							{
								test_script->m_InterfacePos[0] = pQuickDlg->GetLeft() + 28;
								test_script->m_InterfacePos[1] = ( pQuickDlg->GetTop() + 75 + ( pos_num ) * 26 );
							}
							else
							{
								test_script->m_InterfacePos[0] = ( pQuickDlg->GetLeft() + 3 + (pos_num) * 26 ) + 17;
								test_script->m_InterfacePos[1] = pQuickDlg->GetTop() + 22;
							}

							CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
						}
					}
				}

				if (pGame->m_csStatus.m_lpQuickSelected != NULL)
				{ 
					// 활성화 된 스킬 
					if (pGame->m_csStatus.m_lpQuickSelected == pGame->m_csStatus.m_lstQuickSlot[i]) 
					{
						if ( pQuickDlg->GetVisible() )
						{
							CEffScript* test_script = new CEffScript;
							test_script->SetInterfaceSet(true);
							test_script->GetScriptBinData("inter_light6.esf");	

							if ( pQuickDlg->GetVerticalEnable() )
							{
								test_script->m_InterfacePos[0] = pQuickDlg->GetLeft() + 30;
								test_script->m_InterfacePos[1] = pQuickDlg->GetTop() +  25;
							}
							else
							{
								test_script->m_InterfacePos[0] = pQuickDlg->GetLeft() + 302;
								test_script->m_InterfacePos[1] = pQuickDlg->GetTop() +  27;
							}

							CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
						}
					}
				}
			}
		}
	}

	float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetShowInterfaceTimerID());

	for (vector<PotionCoolTime>::iterator itPotion = pGame->m_csStatus.m_lstPotionCoolTime.begin(); 
		itPotion != pGame->m_csStatus.m_lstPotionCoolTime.end(); ++itPotion)
	{
		(*itPotion).m_lPotionCoolDownTime -= fUpdate * 2.0f;
		if ((*itPotion).m_lPotionCoolDownTime <= 0)
		{
			(*itPotion).m_lPotionCoolDownTime = 0;
		}
	}

	if (pGame->m_lpPickItem)
	{
		if (pGame->m_lpPickItem->m_lCoolDownTimeCount)
		{
			pGame->m_lpPickItem->UpdateCoolDownTime();
		}
	}

	if (!m_bWeaponExchageAble)
	{
		m_fWeaponExchageAbleTimeCounter -= CFrameTimer::GetUpdateTimer( GRYLTimer.GetPartyInfoTimerID() );
		if (m_fWeaponExchageAbleTimeCounter <= 0.0f)
		{
			m_bWeaponExchageAble = TRUE;
			m_fWeaponExchageAbleTimeCounter = 0.0f;
		}
	}
}

// Rodin : 위의 UpdateGameCoolDownTimeCheck 함수와 합치도록 합시다.
void CRYLRaceBase::UpdateGameSiegeCoolDownTimeCheck( void ) 
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	for (list<CItemInstance *>::iterator it = pGame->m_csStatus.m_lstInventory.begin(); 
		it != pGame->m_csStatus.m_lstInventory.end(); ++it)
	{
		if ((*it)->m_lCoolDownTimeCount)
		{
			(*it)->UpdateCoolDownTime();
		}
	}

	for (vector<CItemInstance *>::iterator itSkill = pGame->m_csStatus.m_lstClassSkill.begin(); 
		itSkill != pGame->m_csStatus.m_lstClassSkill.end(); ++itSkill)
	{
		if ((*itSkill)->m_lCoolDownTimeCount)
		{
			(*itSkill)->UpdateCoolDownTime();
		}
	}

	if (pGame->m_csStatus.m_lpSiegeArmsSkill )
	{
		if (pGame->m_csStatus.m_lpSiegeArmsSkill->m_lCoolDownTimeCount)
		{
			pGame->m_csStatus.m_lpSiegeArmsSkill->UpdateCoolDownTime();
		}
	}

	unsigned long i = 0;

	if (pGame->m_csStatus.m_lpSiegeArmsSkill)
	{
		CRYLGameScene*		   pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLQuickSlotDlg*	pQuickDlg = ( CRYLQuickSlotDlg* )pScene->GetQuickSlotDlg() ;
		if (pGame->m_csStatus.m_lpSiegeArmsSkill->m_lCoolDownTimeCount && pGame->m_csStatus.m_lpSiegeArmsSkill->UpdateCoolDownTime() )
		{
			if (pGame->m_dwStartQuickSlot <= i && i < 10 + pGame->m_dwStartQuickSlot)
			{	
				// 이펙트
				char light[256] = { 0, };
				int pos_num = 0;

				if ( i >= 10 )
					pos_num = i - 10;
				else
					pos_num = i;

				if ( ( pos_num >= 0 ) && ( pos_num <= 10 ) ) 
				{
					if ( pQuickDlg->GetVisible() )
					{
						sprintf(light, "inter_light%d.esf", pos_num + 1);
						CEffScript* test_script = new CEffScript;
						test_script->SetInterfaceSet(true);
						test_script->GetScriptBinData(light);	

						// 퀵슬롯 시작위치 
						if ( pQuickDlg->GetVerticalEnable() )
						{
							test_script->m_InterfacePos[0] = pQuickDlg->GetLeft() + 28;
							test_script->m_InterfacePos[1] = ( pQuickDlg->GetTop() + 75 + ( pos_num ) * 26 );
						}
						else
						{
							test_script->m_InterfacePos[0] = ( pQuickDlg->GetLeft() + 3 + (pos_num) * 26 ) + 17;
							test_script->m_InterfacePos[1] = pQuickDlg->GetTop() + 22;
						}

						CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
					}
				}
			}
			if (pGame->m_csStatus.m_lpQuickSelected != NULL)
			{ 
				// 활성화 된 스킬 
				if ( pGame->m_csStatus.m_lpSiegeArmsSkill ) 
				{
					if ( pQuickDlg->GetVisible() )
					{
						CEffScript *test_script;				
						test_script = new CEffScript;
						test_script->SetInterfaceSet(true);

						test_script->GetScriptBinData("inter_light6.esf");	
						if ( pQuickDlg->GetVerticalEnable() )
						{
							test_script->m_InterfacePos[0] = pQuickDlg->GetLeft() + 30 ;
							test_script->m_InterfacePos[1] = pQuickDlg->GetTop() +  25 ;
						}
						else
						{
							test_script->m_InterfacePos[0] = pQuickDlg->GetLeft() + 302 ;
							test_script->m_InterfacePos[1] = pQuickDlg->GetTop() +  27 ;
						}
						CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
					}
				}
			}
		}
	}

	float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetShowInterfaceTimerID());

	vector<PotionCoolTime>::iterator itPotion;
	for (itPotion = pGame->m_csStatus.m_lstPotionCoolTime.begin(); itPotion != pGame->m_csStatus.m_lstPotionCoolTime.end(); itPotion++)
	{
		(*itPotion).m_lPotionCoolDownTime -= fUpdate * 2.0f;
		if ((*itPotion).m_lPotionCoolDownTime <= 0)
		{
			(*itPotion).m_lPotionCoolDownTime = 0;
		}
	}

	if (pGame->m_lpPickItem)
	{
		if (pGame->m_lpPickItem->m_lCoolDownTimeCount)
		{
			pGame->m_lpPickItem->UpdateCoolDownTime();
		}
	}

	if (!m_bWeaponExchageAble)
	{
		m_fWeaponExchageAbleTimeCounter -= CFrameTimer::GetUpdateTimer( GRYLTimer.GetPartyInfoTimerID() );
		if (m_fWeaponExchageAbleTimeCounter <= 0.0f)
		{
			m_bWeaponExchageAble = TRUE;
			m_fWeaponExchageAbleTimeCounter = 0.0f;
		}
	}
}


