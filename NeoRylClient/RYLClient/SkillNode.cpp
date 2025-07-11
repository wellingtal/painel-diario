// SkillNode.cpp: implementation of the CSkillNode class.
//
//////////////////////////////////////////////////////////////////////

#include <winsock2.h>

#include <Skill/SkillMgr.h>

#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/Packet/PacketStruct/CharAttackPacketStruct.h>

#include "RYLCreatureManager.h"
#include "RYLObjectControl.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLClientMain.h"

#include "SkillSystem.h"
#include "SkillNode.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkillNode::CSkillNode()
:	m_lpEffectIndex(NULL), m_lpButtonDown(NULL), m_lpButtonSuccess(NULL), 
	m_lpUseItem(NULL), m_dwTargetChrID(0), m_cLevel(0),
	m_wSkill(0), m_wSkillGrade(0), m_wNumAttack(0), m_wNumAttackCount(0), 
	m_bMelee(FALSE), m_bAngle(FALSE), m_bFirstAttack(FALSE)
{
}

CSkillNode::~CSkillNode()
{

}

void CSkillNode::Create(unsigned long dwChrID, unsigned short wSkill, 
						unsigned short wSkillGrade, unsigned long dwCastingTick, 
						CItemInstance* lpUseItem)
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
/*
	if (0x4000 <= wSkill && wSkill < 0x5000)		// 스킬인지 아닌지 구분
	{
		const Skill::ProtoType *lpSkillTypeTemp = g_SkillMgr.GetSkillProtoType(wSkill);
		const Skill::ProtoType *lpSkillType = &lpSkillTypeTemp[wSkillGrade];
		const Skill::ProtoType *lpSkillTypeforStartMP = lpSkillType;

        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if ( !pSelfCreature )
            return;

		// edith 2009.08.06 캐릭터가 스킬을 쓸때 동작을 초기화한다.
		CRYLNetworkData::SendMovingPacket( CA_WAIT, CA_WAIT,
			pSelfCreature->GetDirection(), *(pSelfCreature->GetPosition()) );

		pGame->SetSkillCoolDown(wSkillGrade, lpSkillType->m_cEndCoolDown);
	}
	else */
	if (wSkill & 0x8000)		// 스킬인지 아닌지 구분
	{
		const Skill::ProtoType *lpSkillTypeTemp = g_SkillMgr.GetSkillProtoType(wSkill);
		const Skill::ProtoType *lpSkillType = &lpSkillTypeTemp[wSkillGrade];
		const Skill::ProtoType *lpSkillTypeforStartMP = lpSkillType;

		if (lpSkillType)
		{
			// 인스턴트 스킬은 해당 스킬락카운트(Grade) 그대로의 스킬을 사용한다.
			if (Skill::Type::INSTANCE == lpSkillType->m_eSkillType)
			{
				m_cLevel = (unsigned char)dwCastingTick;
				m_wSkillGrade = wSkillGrade;
			}
			else if (Skill::Type::GATHER == lpSkillType->m_eSkillType)
			{
				m_cLevel = (unsigned char)dwCastingTick;
				m_wSkillGrade = wSkillGrade;
			}
			else
			{
				unsigned long dwLevelTick = lpSkillType->m_LevelTick;
				if (dwLevelTick) m_cLevel = dwCastingTick / dwLevelTick; else m_cLevel = 0;
				if ( m_cLevel > CSkillMgr::MAX_SKILL_LEVEL )
					m_cLevel = CSkillMgr::MAX_SKILL_LEVEL; 
				
				// 캐스팅 스킬은 현재 MP가 쓸수 있는 한도내의 스킬이 나간다.
				long dwMaxMP1, dwMaxMP2, cMaxMP = lpSkillTypeforStartMP->m_StartMP;
				if (wSkillGrade)
				{
					unsigned long dwPrevLock = wSkillGrade - 1;
					dwMaxMP1 = lpSkillTypeTemp[dwPrevLock].m_LevelMP * 6;
					dwMaxMP2 = lpSkillType->m_LevelMP * m_cLevel;

					if (dwMaxMP1 < dwMaxMP2) 
						cMaxMP += dwMaxMP2; 
					else 
						cMaxMP += dwMaxMP1;
				} 
				else
				{
					cMaxMP += (lpSkillType->m_LevelMP * m_cLevel);
				}

				if (static_cast<unsigned short>(pGame->m_csStatus.m_Info.MP) < cMaxMP)
				{
					for (int i = wSkillGrade; i >= 0; i--)
					{
						if (static_cast<unsigned short>(pGame->m_csStatus.m_Info.MP) >= lpSkillTypeforStartMP->m_StartMP)
						{
							wSkillGrade = i;
							lpSkillType = &lpSkillTypeTemp[wSkillGrade];

							cMaxMP = lpSkillTypeforStartMP->m_StartMP;
							if (lpSkillType->m_LevelMP)
							{
								m_cLevel = (static_cast<unsigned short>(pGame->m_csStatus.m_Info.MP) - lpSkillTypeforStartMP->m_StartMP) / lpSkillType->m_LevelMP;
							} 
							else
							{
								m_cLevel = 0;
							}

							if (wSkillGrade)
							{
								unsigned long dwPrevLock = wSkillGrade - 1;
								dwMaxMP1 = lpSkillTypeTemp[dwPrevLock].m_LevelMP * 6;
								dwMaxMP2 = lpSkillType->m_LevelMP * m_cLevel;

								if (dwMaxMP1 < dwMaxMP2)
								{
									break;
								} 
								else
								{
									continue;
								}
							} 
							else
							{
								break;
							}
						}
					}
				}
			}

			if (m_cLevel > 6)
			{
				SKILLSLOT *lpSlot = pGame->m_csStatus.GetSkillSlot(lpSkillType->m_usSkill_ID);
				if (lpSlot)
				{
					if (lpSlot->SKILLINFO.cLockCount && wSkillGrade < lpSlot->SKILLINFO.cLockCount)
					{
						m_cLevel = 6;
					} 
					else
					{
						m_cLevel = lpSlot->SKILLINFO.cSkillLevel;
					}
				} 
				else
				{
					m_cLevel = 6;
				}
			}

			m_dwTargetChrID = dwChrID;
			if (lpSkillType->m_eTargetType == Skill::Target::MELEE) m_bMelee = TRUE; else m_bMelee = FALSE;

			CEffScript *test_script;
			////////// skil ///////////
			CEffScript *skill_name;
			
			char skill[256]={0};
			char *skill_ptr = NULL;
			bool skill_nameuse = true;

			if (strstr("A_Firing",lpSkillType->m_szEffectFileName) != NULL || 
				strstr("NULL",lpSkillType->m_szEffectFileName) != NULL ) //Firing 은 skill name 출력 X
				skill_nameuse = false;

			// 병기 스킬은 이름 출력 무시.						
			if(lpSkillType->m_usSkill_ID==0x9A01 || lpSkillType->m_usSkill_ID==0x9A03)
			{
				skill_nameuse = false;
			}

//			_ASSERTE(_CrtCheckMemory());
			test_script = new CEffScript;
			
			if (skill_nameuse == true) 
			{
//				_ASSERTE(_CrtCheckMemory());
				skill_name = new CEffScript;
				strcpy(skill,lpSkillType->m_szEffectFileName);
				skill_ptr = strrchr(skill,'.');
				(*skill_ptr) = '_';skill_ptr++;
				(*skill_ptr) = 'n';skill_ptr++;
				(*skill_ptr) = 'a';skill_ptr++;
				(*skill_ptr) = 'm';skill_ptr++;
				(*skill_ptr) = 'e';skill_ptr++;
				(*skill_ptr) = '.';skill_ptr++;
				(*skill_ptr) = 'e';skill_ptr++;
				(*skill_ptr) = 's';skill_ptr++;
				(*skill_ptr) = 'f';skill_ptr++;
				(*skill_ptr) = '\0';skill_ptr++;
			}

			// -------------------------------------------------------------------------

			if (pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
			{
				test_script->SetSkillInfo(pGame->m_csStatus.m_Info.Sex, pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType());
			} 
			else
			{
				test_script->SetSkillInfo(pGame->m_csStatus.m_Info.Sex, 0);
			}
			if (skill_nameuse == true) {
				skill_name->SetInterfaceSet(true);
				skill_name->GetScriptBinData(skill);
			}
			test_script->GetScriptBinData((char *)lpSkillType->m_szEffectFileName, pGame->m_bSelfEffect);

			// 스킬 사용시 타격 이팩트 처리
			if (wSkill == 0x8603) // chain action 
				g_CharacterData.m_SkillAttackValue = lpSkillType->m_cStrikeNum; //Hit Count Setting

            RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
            if ( !pSelfCreature )
                return;

            vector3 vecTempPos = *(pSelfCreature->GetPosition());
            test_script->SetStartPos( vecTempPos.x, vecTempPos.y, vecTempPos.z );

			if (m_dwTargetChrID != 0xFFFFFFFF)
			{
				vector3 *vecPos = g_CharacterData.GetFindChrPos(m_dwTargetChrID);
				if (vecPos) test_script->SetEndPos((D3DXVECTOR3 *)vecPos, 1);
				// 캐릭터 세팅
                RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                if ( !pSelfCreature )
                    return;

                RYLCreature* pTestCreature = RYLCreatureManager::Instance()->GetCreature( m_dwTargetChrID );
                if ( !pTestCreature )
                    return;
                
                test_script->SetChr( pSelfCreature->GetCharModel(), pTestCreature->GetCharModel() );
			} 
			else
			{
				if (m_bMelee)
				{
                    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                    if ( !pSelfCreature )
                        return;

                    float fChrDir = pSelfCreature->GetDirection();
					fChrDir = -fChrDir;
					fChrDir -= FLOAT_PHI / 2.0f;

					vector3 vecChrToward;
					vecChrToward.x = cosf(fChrDir);
					vecChrToward.z = sinf(fChrDir);	
					vecChrToward.y = 0.0f;
					vecChrToward.Normalize();
					vecChrToward *= 50.0f;

                    vector3 vecEnd = *(pSelfCreature->GetPosition()) + vecChrToward;
                    test_script->SetEndPos( (D3DXVECTOR3*)&vecEnd, 1 );
				}
			}

            float fChrDir = pSelfCreature->GetDirection();
			fChrDir = -fChrDir;
			fChrDir -= FLOAT_PHI / 2.0f;

			vector3 vecChrToward;
			vecChrToward.x = cosf(fChrDir);
			vecChrToward.z = sinf(fChrDir);	
			vecChrToward.y = 0.0f;
			vecChrToward.Normalize();
			test_script->SetChrFwd(-vecChrToward.x, -vecChrToward.y, -vecChrToward.z);
            test_script->SetPlayer( pSelfCreature, pSelfCreature->GetCharID() );
			m_lpEffectIndex = CSceneManager::m_EffectManager.AddEffScript(test_script);			// 이펙트 스크립트를 읽으면 나오는 인덱스
			
			if (skill_nameuse == true) 
			{	
				skill_name->m_InterfacePos[0] = (float)g_ClientMain.m_iScreenWidth / 2.0f;
				skill_name->m_InterfacePos[1] = (float)g_ClientMain.m_iScreenHeight / 8.0f;

				CSceneManager::m_EffectManager.AddInterfaceScript(skill_name);
			}

			m_bAngle = TRUE;
			m_wSkill = wSkill;
			m_wSkillGrade = wSkillGrade;
			m_wNumAttack = lpSkillType->m_cStrikeNum;
			m_wNumAttackCount = 0;

			vector3 *vecPos;
			if (m_dwTargetChrID != 0xFFFFFFFF)
			{
				vecPos = g_CharacterData.GetFindChrPos(m_dwTargetChrID);
			} 
			else
			{
                vecPos = pSelfCreature->GetPosition();
			}

			CastObject pCastObject;
			pCastObject.m_wTypeID = wSkill;

			if (pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
			{
				pCastObject.m_cObjectType = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType();
			} 
			else
			{
				pCastObject.m_cObjectType = 0;
			}

			pCastObject.m_cObjectLevel = wSkillGrade;
			pCastObject.m_DstPos.fPointX = vecPos->x;
			pCastObject.m_DstPos.fPointY = vecPos->y;
			pCastObject.m_DstPos.fPointZ = vecPos->z;


			// edith 2009.08.06 캐릭터가 스킬을 쓸때 동작을 초기화한다.
			CRYLNetworkData::SendMovingPacket( CA_ATTACK, CA_ATTACK,
				pSelfCreature->GetDirection(), *(pSelfCreature->GetPosition()) );

			SendPacket::CharCastObjectInfo(g_GameSession, 
                CRYLNetworkData::Instance()->m_dwMyChrID, m_dwTargetChrID, pCastObject);

			g_SkillSystem.AddSkill(this);

			pGame->SetSkillCoolDown(wSkillGrade, lpSkillType->m_cEndCoolDown);

			if (lpSkillType->m_cEndScript)
			{
				LPEffectNode lpEffect;
				lpEffect = new EffectNode;
				lpEffect->m_lpEffect = m_lpEffectIndex;
				lpEffect->m_dwTargetID = m_dwTargetChrID;
				g_SkillSystem.m_lstEffect.push_back(lpEffect);
			}
		}
	} 
	else if (wSkill == AtType::BOW || wSkill == AtType::CROSSBOW)
	{		
		// 활 / 석궁 공격
		m_cLevel = 0;
		m_dwTargetChrID = dwChrID;
		m_wSkill = wSkill;
		m_wSkillGrade = wSkillGrade;
		m_wNumAttack = 1;
		m_wNumAttackCount = 0;
		m_bAngle = TRUE;
		m_bMelee = FALSE;

		CEffScript *test_script;
//		_ASSERTE(_CrtCheckMemory());
		test_script = new CEffScript;
		
		if (pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
		{
			test_script->SetSkillInfo(pGame->m_csStatus.m_Info.Sex, pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType());
		} 
		else
		{
			test_script->SetSkillInfo(pGame->m_csStatus.m_Info.Sex, 0);
		}
		test_script->GetScriptBinData("arrow_normal.esf");
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if ( !pSelfCreature )
            return;

        test_script->SetStartPos( pSelfCreature->GetPosition()->x, pSelfCreature->GetPosition()->y, pSelfCreature->GetPosition()->z );
        
		if (m_dwTargetChrID != 0xFFFFFFFF)
		{
            CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();

            RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( m_dwTargetChrID );

            if ( pTargetCreature )
            {
                CZ3DGeneralChrModel* pTargetModel = pTargetCreature->GetCharModel();

                vector3* vecPos = pTargetCreature->GetPosition();
                test_script->SetEndPos( (D3DXVECTOR3*)vecPos, 1 );
                test_script->SetChr( pSelfModel, pTargetModel );
            }
            else
            {
                test_script->SetChr( pSelfModel, NULL );
            }
		}

        float fChrDir = pSelfCreature->GetDirection();
		fChrDir = -fChrDir;
		fChrDir -= FLOAT_PHI / 2.0f;

		vector3 vecChrToward;
		vecChrToward.x = cosf(fChrDir);
		vecChrToward.z = sinf(fChrDir);	
		vecChrToward.y = 0.0f;
		vecChrToward.Normalize();
		test_script->SetChrFwd(-vecChrToward.x, -vecChrToward.y, -vecChrToward.z);

		m_lpEffectIndex = CSceneManager::m_EffectManager.AddEffScript(test_script);			// 이펙트 스크립트를 읽으면 나오는 인덱스

		vector3 *vecPos;
		if (m_dwTargetChrID != 0xFFFFFFFF)
		{
			vecPos = g_CharacterData.GetFindChrPos(m_dwTargetChrID);
		} 
		else
		{
            vecPos = pSelfCreature->GetPosition();
		}

		CastObject pCastObject;
		pCastObject.m_wTypeID = wSkill;
		if (pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos])
		{
			pCastObject.m_cObjectType = pGame->m_csStatus.m_lstEquipment[11 + pGame->m_csStatus.m_cWeaponPos]->GetItemType();
		} 
		else
		{
			pCastObject.m_cObjectType = 0;
		}
		pCastObject.m_cObjectLevel = wSkillGrade;
		if (vecPos)
		{
			pCastObject.m_DstPos.fPointX = vecPos->x;
			pCastObject.m_DstPos.fPointY = vecPos->y;
			pCastObject.m_DstPos.fPointZ = vecPos->z;
		} 
		else
		{
            pCastObject.m_DstPos.fPointX = pSelfCreature->GetPosition()->x;
			pCastObject.m_DstPos.fPointY = pSelfCreature->GetPosition()->y;
			pCastObject.m_DstPos.fPointZ = pSelfCreature->GetPosition()->z;
		}

		SendPacket::CharCastObjectInfo(g_GameSession, 
            CRYLNetworkData::Instance()->m_dwMyChrID, m_dwTargetChrID, pCastObject);

		g_SkillSystem.AddSkill(this);
	} 
	else 
	{
		// 일반 공격
		m_cLevel = 0;
		m_dwTargetChrID = dwChrID;
		m_lpEffectIndex = NULL;
		m_wSkill = wSkill;
		m_wSkillGrade = wSkillGrade;

        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if ( !pSelfCreature )
            return;

        if ( pSelfCreature->GetLowerActionID() == CA_JUMPATTACK )
		{
            CZ3DGeneralChrModel* pSelfModel = pSelfCreature->GetCharModel();
			if ( pSelfModel )
			{
				const char *strMotionSheet = pSelfModel->GetCurrentMotionSheetName();

				if ( strMotionSheet && !strcmp( strMotionSheet, "CLASS_ROGUE" ) )
				{
					m_wNumAttack = 2;
				}
				else
				{
					m_wNumAttack = 1;
				}

				m_bAngle = FALSE;
			}
		} 
		else if ( pSelfCreature->GetLowerActionID() == CA_JUMPATTACK2 )
		{
			m_wNumAttack = 4;
			m_bAngle = FALSE;
		} 
		else if ( pSelfCreature->GetLowerActionID() == CA_JUMPATTACK3 )
		{
			m_wNumAttack = 3;
			m_bAngle = FALSE;
		} 
		else
		{
			m_wNumAttack = 1;
			m_bAngle = TRUE;
		}

		m_wNumAttackCount = 0;
		m_bMelee = TRUE;

		g_SkillSystem.AddSkill(this);
	}

	m_bFirstAttack = FALSE;

	// 스킬 아이템의 경우 서버로 패킷 전송시, 
	// 공격 패킷이 아닌 아이템 사용 패킷을 사용하기 위해 아이템 정보를 기록해둔다.
	if (NULL != lpUseItem)
	{
		if (Item::ItemType::SKILL_ITEM == lpUseItem->GetItemType())
		{
			m_lpUseItem = pGame->m_csStatus.GetInventoryItemfromID(lpUseItem->GetProtoTypeID());
		}
	}
}
