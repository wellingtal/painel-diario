// SkillSystem.cpp: implementation of the CSkillSystem class.
//
//////////////////////////////////////////////////////////////////////

#include <winsock2.h>

#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/Packet/PacketStruct/CharAttackPacketStruct.h>

#include <Skill/SkillMgr.h>
#include <Creature/Monster/MonsterMgr.h>

#include "RYLObjectControl.h"
#include "RYLCreatureManager.h"

#include "GUITextEdit.h"
#include "WinInput.h"

#include "RYLRaceBase.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLChattingDlg.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "SkillSystem.h"
#include "GMMemory.h"

#include "RYLJoystickConfigurePanel.h"
#include "RYLJoystickConfigureDlg.h"

CSkillSystem g_SkillSystem;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkillSystem::CSkillSystem()
{

}

CSkillSystem::~CSkillSystem()
{

}

void CSkillSystem::AddSkill(CSkillNode *lpSkillNode)
{
	m_lstSkillNode.push_back(lpSkillNode);
}

void CSkillSystem::Update()
{
	int snum = CSceneManager::m_EffectManager.GetScriptNum();
	int bActive;
	vector<CSkillNode *>::iterator it;
	CSkillNode *lpSkillNode;

	CRYLGameData* pGame = CRYLGameData::Instance();

	for (int scnt = 0; scnt < snum; scnt++)
	{
		bActive = CSceneManager::m_EffectManager.ProcessScript(scnt);
		
		if (bActive == 2)// S - M - E 구조의 Script 일때 target의 몸에 충돌하는 순간	
		{
		/*	char *esf_name = NULL;
			esf_name = CSceneManager::m_EffectManager.GetEsfName(scnt); 
			if (esf_name != NULL) { //화살 꽃히는 효과를 위한 것
				if (strstr(esf_name,"arrow_normal")!= NULL) {	
					for (int cChr = 0; cChr < g_CharacterData.m_lstCharData.num; cChr++) {
						if (!g_CharacterData.m_lstCharData[cChr]->m_lpChrModel) continue;
						if (g_CharacterData.m_lstCharData[cChr]->m_bCharDead) continue;

						if (g_CharacterData.m_lstCharData[cChr]->m_dwChrID == lpSkillNode->m_dwTargetChrID)
						{
							g_CharacterData.m_lstCharData[cChr]->AddArrow("CHEST",CSceneManager::m_EffectManager.m_lstScript[scnt]->GetExt4Quat());
							
						}
					}
				}	
			}*/
			for (it = m_lstSkillNode.begin(); it != m_lstSkillNode.end();)
			{
				if (!(*it)->m_lpEffectIndex) { it++; continue; }

				if (CSceneManager::m_EffectManager.CheckScript(scnt, (*it)->m_lpEffectIndex))
				{
					// 대거 파이어 관련 박히는 effect setting(추후 삭제한뒤 effect script 안에 들어가야 할 것임)
					char *esf_name = NULL;
					esf_name = CSceneManager::m_EffectManager.GetEsfName(scnt); 
					if (esf_name != NULL) 
                    { 
						// 화살 꽂히는 효과를 위한 것
						int arrow_value = -1;

						if		(strstr(esf_name,"A_Dfire1.esf") != NULL)	{ arrow_value = 1; }
						else if (strstr(esf_name,"A_Dfire2.esf") != NULL)	{ arrow_value = 2; }
						else if (strstr(esf_name,"A_Dfire3.esf") != NULL)	{ arrow_value = 3; }
						else if (strstr(esf_name,"A_Dfire4.esf") != NULL)	{ arrow_value = 4; }
						else if (strstr(esf_name,"A_Dfire5.esf") != NULL)	{ arrow_value = 5; }

                        if ( arrow_value > 0 )
                        {
                            vector<unsigned long> vecCharID;
                            RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

							std::vector<unsigned long>::iterator pos = vecCharID.begin();
							std::vector<unsigned long>::iterator end = vecCharID.end();

							for (; pos != end; ++pos)
							{
								RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
								if ( NULL == pTempCreature )
                                    continue;

                                CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
                                if ( NULL == pTempModel )
                                    continue;

                                if ( pTempCreature->GetIsDead() )
                                    continue;

                                if ( pTempCreature->GetCharID() == (*it)->m_dwTargetChrID )
                                {
                                    pTempCreature->GetEffect()->AddArrow( "CHEST", 
                                                                          CSceneManager::m_EffectManager.m_lstScript[ scnt ]->GetExt4Quat(),
                                                                          arrow_value );
                                }
                            }
                        }
					}

					lpSkillNode = (*it);

					const Skill::ProtoType *lpSkillProtoType = g_SkillMgr.GetSkillProtoType(lpSkillNode->m_wSkill);

					if (lpSkillNode->m_wNumAttackCount == 0)
					{
						// 활
						if (lpSkillNode->m_wSkill == AtType::BOW || lpSkillNode->m_wSkill == AtType::CROSSBOW) 
                        { 														
							vector<unsigned long> vecCharID;
							RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );

							std::vector<unsigned long>::iterator pos = vecCharID.begin();
							std::vector<unsigned long>::iterator end = vecCharID.end();

							for (; pos != end; ++pos)
							{
								RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
								if ( NULL == pTempCreature )
									continue;

								CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
								if ( NULL == pTempModel )
									continue;

								if ( pTempCreature->GetIsDead() )
									continue;

								if ( pTempCreature->GetCharID() == (*it)->m_dwTargetChrID )
								{
									pTempCreature->GetEffect()->AddArrow( "CHEST", 
										CSceneManager::m_EffectManager.m_lstScript[ scnt ]->GetExt4Quat() );
								}
							}
						}

						BOOL bGod = TRUE;		// 무적 상태
						BOOL bCanDead = FALSE;	// 죽은 사람
						if (lpSkillNode->m_wSkill & 0x8000)
						{
							if (lpSkillProtoType)
							{
								if (lpSkillProtoType->m_eTargetType == Skill::Target::FRIEND ||
									lpSkillProtoType->m_eTargetType == Skill::Target::FRIEND_EXCEPT_SELF ||
									lpSkillProtoType->m_eTargetType == Skill::Target::SUMMON)
								{
									bGod = FALSE;
								}

								if (lpSkillProtoType->m_eTargetType == Skill::Target::DEAD_ENEMY ||
									lpSkillProtoType->m_eTargetType == Skill::Target::DEAD_FRIEND)
								{
									bCanDead = TRUE;
								}
							}
						}

						if (lpSkillNode->m_wSkill == 0x9502 || lpSkillNode->m_wSkill == 0x9506)
						{
                            // 파이어링/블래스트 스킬 사용시 탄환 소비
							unsigned short wAmmo = 4003;
							CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItemfromID(wAmmo);

                            if (lpItem && !ClientSocket::IsNSFlagChainChecked(CmdCharUseItem) )
							{
                                // 아이템이 존재하고, 아이템 사용중이 아닐 경우 발사 가능
								SendPacket::CharUseItem(g_GameSession, 
                                    CRYLNetworkData::Instance()->m_dwMyChrID, 
                                    CRYLNetworkData::Instance()->m_dwMyChrID, 
                                    &lpItem->m_lpItemBase->GetPos());

								// 파이어링 사용시 스킬 사용되는 부분.
								g_CharacterData.MakeSkillAttackInfo(lpSkillNode->m_dwTargetChrID, 
                                    lpSkillNode->m_wSkill, lpSkillNode->m_wSkillGrade, 
                                    lpSkillNode->m_cLevel, lpSkillNode->m_wNumAttack, bGod, bCanDead);
							}
						}
						else 
						{
							if (NULL != lpSkillNode->m_lpUseItem)
							{
								// 스킬 아이템을 사용해서 스킬을 사용하는 경우
								g_pClientSocket->SetStatusFlag( NS_USEITEM ) ;
								SendPacket::CharUseItem( g_GameSession, 
									CRYLNetworkData::Instance()->m_dwMyChrID, lpSkillNode->m_dwTargetChrID, 
									&lpSkillNode->m_lpUseItem->m_lpItemBase->GetPos() ) ;
							}
							else
							{
								// edith 
								// 일반적인 스킬 사용시
								g_CharacterData.MakeSkillAttackInfo(lpSkillNode->m_dwTargetChrID, 
									lpSkillNode->m_wSkill, lpSkillNode->m_wSkillGrade, 
									lpSkillNode->m_cLevel, lpSkillNode->m_wNumAttack, bGod, bCanDead);

								if (lpSkillNode->m_wSkill == AtType::BOW || lpSkillNode->m_wSkill == AtType::CROSSBOW)
								{
									g_pClientSocket->SetStatusFlag(NS_NOWAITING);
								}
							}
						}
						// 활부분에 대한. 스킬 초기화. 
						pGame->m_bSkillFlag = FALSE;

						CRYLGameScene*				pScene			= (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
						CRYLJoystickConfigureDlg*	pJoystickDlg	= (CRYLJoystickConfigureDlg*)pScene->GetJoystickConfigureDlg() ;
						CRYLJoystickConfigurePanel* pJoystickPanel	= (CRYLJoystickConfigurePanel*)pJoystickDlg->GetRYLJoystickPanel();

						if (pJoystickPanel->GetForcefeedback() && lpSkillProtoType != NULL)
						{
							if (lpSkillNode->m_wSkill != 0x9502 &&
								lpSkillNode->m_wSkill != 0x9504 &&
								lpSkillNode->m_wSkill != 0x9804 &&
								lpSkillNode->m_wSkill != 0x8704 &&
								lpSkillNode->m_wSkill != 0x8805 &&
								Skill::Type::CHANT != lpSkillProtoType->m_eSkillType)
							{
								// ForceFeedback : 파이어링/스텔스/캐모플라쥐/챈트 제외 일반 스킬 사용
								KeyControl::StartForceFeedback(2000, 2000, 1000);
							}
						}
					}

					lpSkillNode->m_wNumAttackCount++;

					if (lpSkillNode->m_wNumAttack <= lpSkillNode->m_wNumAttackCount)
					{
						it = m_lstSkillNode.erase(it);
						delete lpSkillNode;
					} 
					else
					{
						it++;
					}

					break;
				} 
				else
				{
					it++;
				}
			}
		} 
		else if (bActive == 0)
		{
			for (it = m_lstSkillNode.begin(); it != m_lstSkillNode.end();)
			{
				if (!(*it)->m_lpEffectIndex) { it++; continue; }

				if (CSceneManager::m_EffectManager.CheckNullScript((*it)->m_lpEffectIndex))
				{
					lpSkillNode = (*it);
					it = m_lstSkillNode.erase(it);
					delete lpSkillNode;
				} 
				else
				{
					it++;
				}
			}

			vector<LPEffectNode>::iterator itEffect;
			LPEffectNode lpEffect;
			for (itEffect = m_lstEffect.begin(); itEffect != m_lstEffect.end();)
			{
				if (CSceneManager::m_EffectManager.CheckNullScript((*itEffect)->m_lpEffect))
				{
					lpEffect = (*itEffect);
					itEffect = m_lstEffect.erase(itEffect);
					delete lpEffect;
				} 
				else
				{
					itEffect++;
				}
			}
		}
	}

    RYLCreature *pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

    CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
    if ( !pSelfModel )
        return;

    int nResult = CSceneManager::GetCharacterEvent( pSelfModel, EVENT_SHOT );

	if ( nResult )
	{
		if ( pGame->m_lpInterface->m_dwRangedWeaponTargetChrID != 0xFFFFFFFF )
		{
			BOOL bAttackable = FALSE;

			if ( pGame->m_csStatus.m_lstEquipment[ Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos ] )
            {
				unsigned long dwType = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + pGame->m_csStatus.m_cWeaponPos]->GetItemType();

				if ( pGame->m_csStatus.m_lstEquipment[ Item::EquipmentPos::SHIELD_HAND1 + pGame->m_csStatus.m_cWeaponPos ] )
				{
					unsigned long dwArrowType = pGame->m_csStatus.m_lstEquipment[ Item::EquipmentPos::SHIELD_HAND1 + pGame->m_csStatus.m_cWeaponPos ]->GetItemType();

					switch(dwType)
					{
						case Item::ItemType::BOW:
                        {
							if ( dwArrowType == Item::ItemType::ARROW )
							{
								lpSkillNode = new CSkillNode;
								lpSkillNode->Create( pGame->m_lpInterface->m_dwRangedWeaponTargetChrID, AtType::BOW, 0, 0, NULL );
							}
                        }
					    break;

						case Item::ItemType::CROSSBOW:
                        {
							if (dwArrowType == Item::ItemType::BOLT)
							{
								lpSkillNode = new CSkillNode;
								lpSkillNode->Create( pGame->m_lpInterface->m_dwRangedWeaponTargetChrID, AtType::CROSSBOW, 0, 0, NULL );
							}
                        }
						break;
					}
				}
			}
		}
	}

    nResult = CSceneManager::GetCharacterEvent( pSelfModel, EVENT_JUST);

#ifdef _NEW_KEY_
	if ( nResult && (g_DeviceInput.GetSkillHold() || KeyControl::IsKeyState(KeyControl::KEY_COLUM_CASTING)) )
#else
	if ( nResult && (g_DeviceInput.GetIsRightMousePress() || KeyControl::IsKeyState(KeyControl::KEY_COLUM_CASTING)) )
#endif
    {
        if ( pSelfCreature->m_bSkillCombo )
        {
            const Skill::ProtoType *lpSkill = g_SkillMgr.GetSkillProtoType(pGame->m_csStatus.m_lpQuickSelected->GetSkillID());

            if ( lpSkill && lpSkill->m_bGauge )
            {
                lpSkill = &lpSkill[ pSelfCreature->m_ulSkillComboCount ];

                unsigned long ulAction;
                pSelfCreature->SetActionID( &ulAction, lpSkill->m_szCastingFileName );

                if ( pSelfCreature->GetUpperActionID() + 1 == ulAction )
                {
                    // 쓴 스킬과 지금 선택된게 같은 스킬일때 클리어
                    g_CharacterData.m_bComboSuccess = TRUE;

                    if ( strcmp( lpSkill->m_szCastingFileName, "NULL" ) )
					{
						pSelfCreature->EndSkill( (char *)lpSkill->m_szCastingFileName );
					}

                    // 스킬을 집어넣는다.
					unsigned long ulGrade, ulLevel;
					ulGrade = pSelfCreature->m_ulSkillComboCount;

					if ( pGame->m_csStatus.m_lpQuickSelected->GetSkillLockCount() == pSelfCreature->m_ulSkillComboCount )
					{
						ulLevel = pGame->m_csStatus.m_lpQuickSelected->GetNowDurability();
						pSelfCreature->m_ulSkillComboCount = 0;
						pSelfCreature->m_bSkillCombo = FALSE;
					} 
                    else
					{
						ulLevel = 6;
						pSelfCreature->m_ulSkillComboCount++;
						pSelfCreature->m_bSkillCombo = TRUE;
					}

                    CSkillNode *lpSkillNode = new CSkillNode;
					lpSkillNode->Create( pGame->m_lpInterface->m_dwSpecialTargetChrID, 
						pGame->m_csStatus.m_lpQuickSelected->GetSkillID(), ulGrade, ulLevel, 
						pGame->m_csStatus.m_lpQuickSelected );

					pSelfCreature->SetStillCasting( false );
                }
                else
                {
                    pSelfCreature->m_ulSkillComboCount = 0;
					pSelfCreature->m_bSkillCombo = FALSE ;
                }
            }
            else
            {    
                pSelfCreature->m_ulSkillComboCount = 0 ;
				pSelfCreature->m_bSkillCombo = FALSE ;
            }
		}
    }

    nResult = CSceneManager::GetCharacterEvent( pSelfModel, EVENT_JUSTEND );

	if ( nResult )
	{
		pSelfCreature->m_ulSkillComboCount = 0;
		pSelfCreature->m_bSkillCombo = FALSE;
	}

    static bool bAttack;
	nResult = CSceneManager::GetCharacterEvent( pSelfModel, EVENT_HIT );

	bool isLeftAttack = false;
	if ( !nResult )
	{
		nResult = CSceneManager::GetCharacterEvent( pSelfModel, EVENT_HIT_LEFT );
		if ( nResult )
			isLeftAttack = true;
	}

	if ( nResult )
	{
		CSkillNode* lpNextSkillNode ;

		do
		{
			lpNextSkillNode = NULL ;	
			bAttack = FALSE;
			lpSkillNode = GetNormalSkillNode();
			if (!lpSkillNode)
			{
				// 일반 공격
				lpSkillNode = new CSkillNode;
				if ( isLeftAttack )
				{
					lpSkillNode->Create(0, AtType::LEFT_MELEE, 0, 0, NULL);
				}
				else
				{
					lpSkillNode->Create(0, AtType::RIGHT_MELEE, 0, 0, NULL);
				}
			}

			if (!lpSkillNode->m_bFirstAttack)
			{
				// 처음 공격 일때 패킷을 보낸다.
				float fAttackRotation[6];
				fAttackRotation[0] = -55.0f;
				fAttackRotation[1] = 30.0f;
				fAttackRotation[2] = -10.0f;
				fAttackRotation[3] = -10.0f;
				fAttackRotation[4] = -10.0f;
				fAttackRotation[5] = -10.0f;

				float fAttackRate[6];
				fAttackRate[0] = 0.0f;
				fAttackRate[1] = 0.0f;
				fAttackRate[2] = 0.0f;
				fAttackRate[3] = 20.0f;
				fAttackRate[4] = 20.0f;
				fAttackRate[5] = 20.0f;

				BOOL bGod = TRUE;
				if (lpSkillNode->m_wSkill & 0x8000)
				{
					const Skill::ProtoType *lpSkillProtoType = g_SkillMgr.GetSkillProtoType(lpSkillNode->m_wSkill);

					if (lpSkillProtoType)
					{
						if (lpSkillProtoType->m_eTargetType == Skill::Target::FRIEND ||
							lpSkillProtoType->m_eTargetType == Skill::Target::FRIEND_EXCEPT_SELF)
						{
							bGod = FALSE;
						}
					}
				}

				bAttack = g_CharacterData.MakeAttackInfo( lpSkillNode->m_wSkill, 
														lpSkillNode->m_wSkillGrade, 
														lpSkillNode->m_cLevel,
														lpSkillNode->m_wNumAttack, 
														fAttackRotation[ pSelfCreature->m_ulAttackCombo ],
														fAttackRate[ pSelfCreature->m_ulAttackCombo ], 
														lpSkillNode->m_bAngle, 
														bGod );
			}
			else
			{
				g_CharacterData.MakeVisualAttackInfo();			
			}

			lpSkillNode->m_wNumAttackCount += nResult;

			lpSkillNode->m_bFirstAttack = TRUE;

			if ( lpSkillNode->m_wNumAttack <= lpSkillNode->m_wNumAttackCount || lpSkillNode->m_wSkill == 0x9305 )
			{
				// 공격 수가 끝났을 때 스킬 노드를 없앤다.
				for ( it = m_lstSkillNode.begin(); it != m_lstSkillNode.end(); it++ )
				{
					if ( (*it) == lpSkillNode )
						break;
				}

				it = m_lstSkillNode.erase( it );
				delete lpSkillNode;
			}

			lpNextSkillNode = GetNormalSkillNode() ;
		} while ( m_lstSkillNode.size() > 0 && lpNextSkillNode && lpNextSkillNode->m_bMelee ) ;

		pGame->m_bSkillFlag = FALSE;
	}

    nResult = CSceneManager::GetCharacterEvent( pSelfCreature->GetCharModel(), EVENT_CANCEL );

	if ( nResult )
	{
		if ( pGame->GetChangeWeapon() )
		{
			pSelfCreature->m_ulAttackCombo = 0;
			pGame->m_bChangeWeapon = FALSE;
		} 
        else
		{
			if ( bAttack && ( g_DeviceInput.GetIsLeftMousePress() || g_CharacterData.m_bAutoTargetAttack || KeyControl::IsKeyState(KeyControl::KEY_COLUM_ATTACK)) )	// 타이밍이 맞았을때
			{
				// 계속 콤보가 누적된다.
				if ( pGame->m_csStatus.m_lstEquipment[ 11 + pGame->m_csStatus.m_cWeaponPos ] )
				{
					if ( ++pSelfCreature->m_ulAttackCombo >= g_CharacterData.m_dwMaxCombo )
					{
						pSelfCreature->m_ulAttackCombo = 0;
					} 
                    else
					{
						if ( pSelfCreature->GetLowerActionID() == CA_ATTACK ||
							pSelfCreature->GetLowerActionID() == CA_ATTACK2 ||
							pSelfCreature->GetLowerActionID() == CA_ATTACK3 ||
							pSelfCreature->GetLowerActionID() == CA_ATTACK4 ||
							pSelfCreature->GetLowerActionID() == CA_ATTACK5 ||
							pSelfCreature->GetLowerActionID() == CA_ATTACK6 )
						{                                                 
							pSelfCreature->SetAction( CA_WAIT, CA_WAIT );
							pSelfCreature->SetUpperAble( true );
							pSelfCreature->SetLowerAble( true );
							pSelfCreature->SetUpperActing( false );
							pSelfCreature->SetLowerActing( false );
						}
					}
				}
			}
			else
			{
				pSelfCreature->m_ulAttackCombo = 0;
			}
		}
	}

	vector<LPEffectNode>::iterator itEffect;
	for (itEffect = m_lstEffect.begin(); itEffect != m_lstEffect.end(); ++itEffect)
	{
        if ( pSelfCreature )
		{
			(*itEffect)->m_lpEffect->SetStartPos( pSelfCreature->GetPosition()->x, pSelfCreature->GetPosition()->y, pSelfCreature->GetPosition()->z);

			// 스킬 이펙트 사이즈 설정
			if ( CRYLGameData::GetClientIdentity( pSelfCreature->GetCharID() ) == Creature::CT_MONSTER )
			{
				const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( pSelfCreature->GetCharID() & 0x0000FFFF );
				if (NULL != lpMonster)
				{
					(*itEffect)->m_lpEffect->SetScale( pSelfCreature->GetCharModel()->GetEffectScale() * lpMonster->m_MonsterInfo.m_fSkillEffectSize );
				}
			}
		}

        RYLCreature* pTempCreature = NULL;

		if ( (*itEffect)->m_dwTargetID != 0xFFFFFFFF )
		{                
            pTempCreature = RYLCreatureManager::Instance()->GetCreature( (*itEffect)->m_dwTargetID );

			if ( pTempCreature )
			{
				(*itEffect)->m_lpEffect->SetEndPos( pTempCreature->GetPosition()->x, pTempCreature->GetPosition()->y, pTempCreature->GetPosition()->z );
			}
		}
	}
}

CSkillNode * CSkillSystem::GetNormalSkillNode()
{
	vector<CSkillNode *>::iterator it;
	for (it = m_lstSkillNode.begin(); it != m_lstSkillNode.end(); it++)
	{
		if ((*it)->m_bMelee || (*it)->m_wSkill == 0x9407) return (*it);
	}

	return NULL;
}
