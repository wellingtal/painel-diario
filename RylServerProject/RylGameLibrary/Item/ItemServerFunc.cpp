
#include "stdafx.h"

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/GameClient/SendCharQuest.h>
#include <Network/ClientSocket/ClientConstants.h>

#include <Creature/Character/Character.h>
#include <Creature/Character/CharRespawnMgr.h>
#include <Creature/Siege/SiegeConstants.h>

#include <Log/ItemLog.h>
#include <Log/LogStruct.h>

#include <Skill/SkillTable.h>
#include <Quest/QuestMgr.h>
#include <Community/Party/Party.h>
#include <Map/FieldMap/Cell.h>

#include <GameTime/GameTimeMgr.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include "Item.h"

using namespace Item;

// --------------------------------------------------------------------------------------
// Desc : InstallSocket - 소켓에 보석을 박는 순서.
//
//	1. 보석을 찾는다. 
//	2. 장비를 찾는다. 
//	3. 장비에 보석을 박는다. 
//	4. 보석을 없앤다. 
//	5. 캐릭터 능력치를 재계산한다.
// --------------------------------------------------------------------------------------
CEquipment::EQUIPMENT_ERROR CEquipment::InstallSocket(CItem& Gem)
{
    using namespace Item;

    // 아이템이 보석이 맞는가 확인
    unsigned short usGemTypeID = Gem.GetPrototypeID();

	// edith 2008.08.10 유료 소켓 보석 처리
	// 8번 소켓 오픈 보석이면..
	if(usGemTypeID == EtcItemID::GEM_MAXSOCKET_ID)
	{
		// 보석 개수 확인
		if (0 == Gem.GetNumOrDurability())
		{
			return E_GET_GEM_FAILED;
		}

		if(m_cUpgradeLevel != EquipmentInfo::MAX_UPGRADE_LEVEL)  // blackdog supermod
		//	if(m_cUpgradeLevel < 15 )
		{
			return E_SOCKET_NOT_MAX;
		}

		// 소켓 번호가 7번이 아니면.
		if(m_cMaxSocket != EquipmentInfo::MAX_SOCKET_NUM-1) // blackdog supermod
		//	if(m_cMaxSocket < 12 )
		{
			return E_SOCKET_NOT_MAX;
		}

		// 최대 소켓을 1개 증가시킨다.
		++m_cMaxSocket;
		if(m_cMaxSocket >= EquipmentInfo::MAX_SOCKET_NUM)
			m_cMaxSocket = EquipmentInfo::MAX_SOCKET_NUM;

		Gem.SetNumOrDurability(Gem.GetNumOrDurability() - 1);

		return S_SUCCESS;
	}

	// UnInstall Socket	보석이면.
	if(usGemTypeID == EtcItemID::GEM_UNSOCKET_ID)
	{
		// 보석 개수 확인
		if (0 == Gem.GetNumOrDurability())
		{
			return E_GET_GEM_FAILED;
		}	

		// 인스톨된 소켓이 없다.
		if(m_cSocketNum == 0)
		{
			return E_SOCKET_NOT_INST;
		}

		m_cSocketNum = 0;
		std::fill_n(m_cSocket, int(EquipmentInfo::MAX_SOCKET_NUM), 0);

		ApplyGemAttribute(REMOVE);  
		InitializeGemAttribute();   
		ApplyGemAttribute(APPLY);   

		Gem.SetNumOrDurability(Gem.GetNumOrDurability() - 1);

		// 그레이드/가격 재계산
		CalculateItemGrade();
		CalculateItemPrice();

		// 3차 밸런스 패치 (S그레이드 아이템 제한선 보정)
		RevisionLimit();

		return S_SUCCESS;
	}

	// 현재 아이템에 남아 있는 소켓 개수 검사. 소켓이 전부 차면 더 못 박도록 한다. 
    if (EquipmentInfo::MAX_SOCKET_NUM < m_cMaxSocket ||
        m_cMaxSocket <= m_cSocketNum)
    {
        return E_OVER_MAX_SOCKET;
    }

	// 퍼펙트 보석은 현재 소켓에 못박게 수정한다.
    if (usGemTypeID < EtcItemID::GEM_START_ID || EtcItemID::GEM_RUNE_END_ID < usGemTypeID)
    {   
        return E_NOT_GEM;
    }

	// 룬보석이면 박을수 잇는 장비를 검색한다.
	if(EtcItemID::GEM_RUNE_START_ID <= usGemTypeID)
	{
		 // 룬보석은 박을수 있는 장비가 정해져 있다.
		CEquipment* lpEquip = CEquipment::DowncastToEquipment(this);
		if (NULL == lpEquip)
			return E_NOT_GEM;

		// 룬은 헬멧과 아머만 한다.
		switch (Item::CItemType::GetEquipType(lpEquip->GetItemInfo().m_DetailData.m_dwFlags))
		{
			case Item::CItemType::HELM_TYPE:
			case Item::CItemType::ARMOUR_TYPE:
				// 이거일땐 룬보석을 받을수 있다.
				break;
			default:
				// 저2개이외에는 보석을 박을수 없다.
				return E_NOT_GEM;
		}
	}

    // 보석 개수 확인
    if (0 == Gem.GetNumOrDurability())
    {
        return E_GET_GEM_FAILED;
    }

    unsigned char cGemType = usGemTypeID - EtcItemID::GEM_START_ID;

    // 젬 속성 얻기
    ApplyGemAttribute(REMOVE);

    // cGemType이 0일 수 있기 때문에, +1을 해서 넣어준다.
	AddSocket(cGemType + 1);    

    InitializeGemAttribute();
    ApplyGemAttribute(APPLY);

	Gem.SetNumOrDurability(Gem.GetNumOrDurability() - 1);

	// 그레이드/가격 재계산
	CalculateItemGrade();
	CalculateItemPrice();

	// 3차 밸런스 패치 (S그레이드 아이템 제한선 보정)
	RevisionLimit();

	// edith 새로운 장비로 설정.
	if(0 == GetSeasonRecord())
		SetNewEquip();

	return S_SUCCESS;
}

CEquipment::EQUIPMENT_ERROR CEquipment::InstallRuneSocket(CItem& Rune, bool bEquip, unsigned char cLevel)
{
	// 아이템이 룬 아이템이 맞는가 확인.
	unsigned short usRuneTypeID = Rune.GetPrototypeID();

	if(usRuneTypeID < EtcItemID::RUNE_START_ID || EtcItemID::RUNE_END_ID < usRuneTypeID)
	{   
		return E_NOT_RUNE;
	}

	// 룬 아이템을 장착 가능한지 체크.

	unsigned char cRuneSlot = GetRuneEquipable();
	
	if(!cRuneSlot)
	{
		return E_NOT_RUNE_SLOT;
	}

	// 룬 아이템에 장착제한 체크(레벨제한).
	if(bEquip)
	{
		const ItemInfo* itemInfo = CItemMgr::GetInstance().GetItemInfo(usRuneTypeID);

		if(itemInfo)
		{
			if(itemInfo->m_UseLimit.m_cLimitStatus==Item::StatusLimit::LEVEL)
			{
				if(cLevel<itemInfo->m_UseLimit.m_wLimitValue)
				{
					return E_NOT_RUNE_LEVEL_LIMIT;
				}
			}
		}
		else
		{
			return E_NOT_RUNE_SCRIPT_INFO;
		}
	}

	// 룬 개수 확인
	if(!Rune.GetNumOrDurability())
	{
		return E_GET_RUNE_FAILED;
	}

	unsigned short usRuneType = usRuneTypeID - EtcItemID::RUNE_START_ID;

	// 룬을 우선제외
//	AddAttribute(Item::Attribute::Type::RUNE, usRuneType);

	Rune.SetNumOrDurability(Rune.GetNumOrDurability() - 1);

	// 그레이드/가격 재계산
	CalculateItemGrade();
	CalculateItemPrice();

	// 3차 밸런스 패치 (S그레이드 아이템 제한선 보정)
	RevisionLimit();

	// edith 새로운 장비로 설정.
	if(0 == GetSeasonRecord())
		SetNewEquip();

	return R_SUCCESS;
}

CEquipment::EQUIPMENT_ERROR CEquipment::UnInstallRuneSocket(CItem& Rune, bool bEquip, unsigned char cLevel, unsigned short usSkillPoint)
{	
	// 아이템이 룬 아이템이 맞는가 확인.
	unsigned short usRuneTypeID = Rune.GetPrototypeID();

	if(usRuneTypeID < EtcItemID::RUNE_START_ID || EtcItemID::RUNE_END_ID < usRuneTypeID)
	{   
		return E_NOT_RUNE;
	}
	
	// 소멸 룬인지 체크.
	if(usRuneTypeID!=EtcItemID::DESTRUCTION_RUNE)
	{
		return E_NOT_RUNE;
	}

	// 삭제 가능한지 체크 (스킬포인트).

	if(bEquip)
	{
		if(usSkillPoint<GetAllRuneSkillPointCheck())
		{
			return E_USED_RUNE_SKILL_POINT;
		}
	}

	// 룬 개수 확인
	if(!Rune.GetNumOrDurability())
	{
		return E_GET_RUNE_FAILED;
	}
	
	// 룬과 속성을 삭제한다.
	if(!SubRuneAttribute())
	{
		return E_NOT_RUNE;
	}

	Rune.SetNumOrDurability(Rune.GetNumOrDurability() - 1);

	// 그레이드/가격 재계산
	CalculateItemGrade();
	CalculateItemPrice();

	// 3차 밸런스 패치 (S그레이드 아이템 제한선 보정)
	RevisionLimit();

	return R_SUCCESS;
}

unsigned short Item::CEquipment::GetAllRuneSkillPointCheck()
{
	// 장착 아이템에 있는 모든 룬 아이템에 속성중 스킬포인트 리턴해준다.

	unsigned short	usRune	= 0;

	for(unsigned char cIndex = 0; cIndex < 2; cIndex++)
	{
		const ItemInfo* itemInfo = CItemMgr::GetInstance().GetItemInfo(m_usRuneSocket[cIndex]);

		if(itemInfo)
		{
			usRune += itemInfo->m_EquipAttribute.m_usAttributeValue[Attribute::SKILL_POINT];
		}
	}
	return usRune;
}

bool Item::CEquipment::GetRuneCheckSkill(CCharacter* lpCharacter, Item::ItemPos itemPos, bool bChk)
{
	// 룬 아이템이 장착 되어 있으면 스킬 포인트 체크.

	Item::CEquipment* lpDstEquipment = Item::CEquipment::DowncastToEquipment(lpCharacter->GetItem(itemPos));

	unsigned short usRuneSkillPoint = GetAllRuneSkillPointCheck();

	SKILL skill = lpCharacter->GetSkill();

	if(usRuneSkillPoint)
	{
		short sSkillPoint = lpCharacter->GetStatus().m_StatusInfo.m_wSkillPoint - skill.GetSkillNum();			

		if(sSkillPoint<0)
		{				
			ERRLOG1(g_Log, "CID:0x%08x 사용한 스킬포인트가 최대 스킬포인트보다 클 경우", lpCharacter->GetCID());
			return false;
		}

		if(lpDstEquipment && bChk)
		{
			unsigned short usDstRuneSkillPoint = lpDstEquipment->GetAllRuneSkillPointCheck();

			if(usDstRuneSkillPoint)
			{
				usRuneSkillPoint = std::max(usRuneSkillPoint, usDstRuneSkillPoint) - std::min(usRuneSkillPoint, usDstRuneSkillPoint);
			}
		}

		if(usRuneSkillPoint>sSkillPoint)
		{
			return false;
		}
	}																									
	return true;
}

CEquipment::EQUIPMENT_ERROR CEquipment::UpgradeItem(CItem& Mineral_InOut, 
													unsigned long dwCurrentGold_In, 
													unsigned long& dwUsedGold_Out, 
													unsigned char& cValue_Out)	// cValue_Out 내구도 다운으로 사용.
{
	if((m_ItemData.m_usProtoTypeID==62601)||(m_ItemData.m_usProtoTypeID==62601))
		
	//	||(m_ItemData.m_usProtoTypeID==601)||(m_ItemData.m_usProtoTypeID==5101)||(m_ItemData.m_usProtoTypeID==5101)
	//	||(m_ItemData.m_usProtoTypeID==5201)||(m_ItemData.m_usProtoTypeID==5401)||(m_ItemData.m_usProtoTypeID==5110)
	//	||(m_ItemData.m_usProtoTypeID==5210)||(m_ItemData.m_usProtoTypeID==5801)||(m_ItemData.m_usProtoTypeID==6107))
	//	return E_UPGRADE_LEVEL_IS_FULL; // E_GRAFE_FAILD
	    return E_UPGRADE_LEVEL_IS_FULL; // E_GRAFE_FAILD

    dwUsedGold_Out = 0;

	// edith 2009.09.17 코어시스템 추가
	if(Mineral_InOut.GetPrototypeID() == EtcItemID::AWAKENCORE)
	{
		enum CoreArrayType
		{
			CORE_PERSENT_SUCCESS_AWAKEN			= 0,
			CORE_PERSENT_FAIL_LEVEL_KEEP		= 1,
			CORE_PERSENT_FAIL_LEVEL1_DOWN		= 2,
			CORE_PERSENT_FAIL_DUR_DOWN			= 3,
			CORE_MINERAL_NUM					= 4,

			MAX_CORE_ROW						= 5
		};

		// 김재연 제련 성공율
		static const unsigned short s_UpgradeCoreTable[EquipmentInfo::MAX_CORE_LEVEL][MAX_CORE_ROW] = 
		{
			//  성공확율		다운확률(0)		다운확률(-1)	내구다운확율	광물 개수
			{	50,				100,			0,				60,				1	},	
			{	50,				100,			0,				60,				1	},	
			{	35,				100,			0,				60,				1	},	
			{	19,				100,			0,				60,				1	},	
			{	3,				100,			0,				60,				1	}
		};

		// 각성의 코어도 100퍼센트 성공
		if(GetSeasonRecord() == 3 && m_cCoreLevel >= 5)	// 각성의 시즌레코드이면.
		{
			// 이미 각성된 아이템이면 다시 각성할수 없다.
			return E_NOT_UPGRADE_AWAKEN;
		}

		// 돈은 충분한가?
		unsigned long dwNeedGold = GetUpgradePrice();
		if (dwCurrentGold_In < dwNeedGold)
		{
			return E_NOT_ENOUGH_MONEY;
		}

		// 내구도는 충분한가.
		unsigned char cMaxDur = GetMaxNumOrDurability();
		if(cMaxDur <= 50)
		//	if(cMaxDur <= 10)
		{
			return E_NOT_UPGRADE_MINENDURANCE;
		}

		// 개수는 충분한가?
		
		int nDiffMineralNum = static_cast<int>(Mineral_InOut.GetNumOrDurability() - 1);

		if (nDiffMineralNum < 0)
		{
			return E_NOT_ENOUGH_MINERAL;
		}

		// 광물에 맞춰 성공 확률을 결정
		unsigned short wSuccessPercent = s_UpgradeCoreTable[m_cCoreLevel][CORE_PERSENT_SUCCESS_AWAKEN];;
		
		switch(m_GradeInfo.m_eItemGrade)
		{
		case Item::EquipType::S_GRADE:
		case Item::EquipType::AAA_GRADE:
		case Item::EquipType::AA_GRADE:
		case Item::EquipType::A_GRADE:	
			wSuccessPercent = (unsigned short)(wSuccessPercent*0.7f);		
			break;

		case Item::EquipType::B_GRADE:	wSuccessPercent = (unsigned short)(wSuccessPercent);			break;
		case Item::EquipType::C_GRADE:	wSuccessPercent = (unsigned short)(wSuccessPercent*1.2f);		break;
		case Item::EquipType::D_GRADE:	wSuccessPercent = (unsigned short)(wSuccessPercent*1.6f);		break;
		case Item::EquipType::F_GRADE:	wSuccessPercent = (unsigned short)(wSuccessPercent*2.2f);		break;
		default:				break;
		}

		// 광물 제거, 돈 제거
		Mineral_InOut.SetNumOrDurability(nDiffMineralNum);
		dwUsedGold_Out = dwNeedGold;

		// 제련 성공율 조정
		wSuccessPercent = (unsigned short)((float)wSuccessPercent*(CServerSetup::GetInstance().GetRefineDefault() / 100.0f));

		// 제련 성공
		if (wSuccessPercent > Math::Random::ComplexRandom(100))
		{
			// 각성의 시즌레코드로 수정한다.
			if(3 != GetSeasonRecord())
			{
				SetNewEquip(3);
				m_cCoreLevel = 1;
			}
			else
				m_cCoreLevel++;
		}
		// 제련 실패
		else
		{
			unsigned char cLevelDownNum = 0;

			unsigned char cPersent = static_cast<unsigned char>(Math::Random::ComplexRandom(100));
			if (cPersent < s_UpgradeCoreTable[m_cCoreLevel][CORE_PERSENT_FAIL_LEVEL_KEEP])
			{
				// 레벨 다운 없음
				cLevelDownNum = 0;
			}
			else
			{
				cPersent -= s_UpgradeCoreTable[m_cCoreLevel][CORE_PERSENT_FAIL_LEVEL_KEEP];
				if (cPersent < s_UpgradeCoreTable[m_cCoreLevel][CORE_PERSENT_FAIL_LEVEL1_DOWN])
				{
					// 1레벨 다운
					//cLevelDownNum = 1;
					cLevelDownNum = 0;
				}
				else
				{
					// 2레벨 다운
					cLevelDownNum = 0;
					//cLevelDownNum = 2;
				}
			}

			// 레벨을 다운시킨다.
			if(3 == GetSeasonRecord())
			{
				for(int i = 0; i < cLevelDownNum; ++i)
				{
					if(m_cCoreLevel > 0)
						--m_cCoreLevel;
				}

				if(m_cCoreLevel == 0)
					SetNewEquip();
			}

			unsigned char cPersent2 = static_cast<unsigned char>(Math::Random::ComplexRandom(100));
			// 현재 30이란값이 들어있다. 즉 30프로로 내구도가 감소한다.
			if (cPersent2 < s_UpgradeCoreTable[m_cCoreLevel][CORE_PERSENT_FAIL_DUR_DOWN])
			{
				// edith 2008.07.03 제련 실패시 최대 내구도 감소
				unsigned char cMaxDur = GetMaxNumOrDurability();
				unsigned char cDownDur = 0;

				// 무조건 10씩 감소.
				cDownDur = 150;

				if(cMaxDur-cDownDur <= 0)
					SetMaxNumOrDurability(1);
				else
					SetMaxNumOrDurability(cMaxDur-cDownDur);

				SetNumOrDurability(1);
			}
			cValue_Out = GetMaxNumOrDurability();
		}

		// 그레이드/가격 재계산
		CalculateItemGrade();
		CalculateItemPrice();

		// 3차 밸런스 패치 (S그레이드 아이템 제한선 보정)
		RevisionLimit();

		return S_SUCCESS;
	}	
	else if(Mineral_InOut.GetPrototypeID() == EtcItemID::ENDURANCESTONE)
	{
		// 내구도 회복 아이템은 100퍼센트 성공

		// 돈은 충분한가?
		unsigned long dwNeedGold = GetUpgradePrice();
		if (dwCurrentGold_In < dwNeedGold)
		{
			return E_NOT_ENOUGH_MONEY;
		}

		// 개수는 충분한가?
		int nDiffMineralNum = static_cast<int>(Mineral_InOut.GetNumOrDurability() - 1);

		if (nDiffMineralNum < 0)
		{
			return E_NOT_ENOUGH_MINERAL;
		}

		// 내구도는 충분한가.
		unsigned char cMaxDur = GetMaxNumOrDurability();
		if(cMaxDur >= 150) // WAS 100
		{
			return E_NOT_UPGRADE_ENDURANCE;
		}

		// edith 2008.07.03 최대 내구도 복구 아이템 사용성공.
		unsigned char cCurDur = GetNumOrDurability();

		// 광물 제거, 돈 제거
		Mineral_InOut.SetNumOrDurability(nDiffMineralNum);
		dwUsedGold_Out = dwNeedGold;

		//cMaxDur = cMaxDur+100;
		cMaxDur = cMaxDur+150; // was +10
		if(cMaxDur >= 150)
			cMaxDur = 150;

		SetMaxNumOrDurability(cMaxDur);
		SetNumOrDurability(cCurDur);

		cValue_Out = GetMaxNumOrDurability();

		// 그레이드/가격 재계산
		CalculateItemGrade();
		CalculateItemPrice();

		// 3차 밸런스 패치 (S그레이드 아이템 제한선 보정)
		RevisionLimit();

		if(0 == GetSeasonRecord())
			SetNewEquip();

		return S_SUCCESS;
	}

	// edith 아이템 업그레이드
	enum ArrayType
	{
		PERSENT_SUCCESS_SILVIN			= 0,
		PERSENT_SUCCESS_MITHRIL			= 1,
		PERSENT_SUCCESS_ITERNIUM		= 2,
		PERSENT_SUCCESS_SOULMETAL		= 3,
		PERSENT_SUCCESS_ANTONIUM		= 4,
		PERSENT_SUCCESS_ANCIENTMETAL	= 5,

		PERSENT_FAIL_LEVEL_KEEP			= 6,
		PERSENT_FAIL_LEVEL1_DOWN		= 7,
		PERSENT_FAIL_DUR_DOWN			= 8,

		MINERAL_NUM						= 9,

		MAX_ROW							= 10
	};

	// edith 2010.01.23 아이템 제련 실패시 아이템 레벨 다운그레이드 확율 지정.
	// 김재연 제련 성공율
/*	static const unsigned short s_UpgradeTable[EquipmentInfo::MAX_UPGRADE_LEVEL][MAX_ROW] = 
	{
		// 0-Silvin		 1-	Mithril			  2-Iternium          3-soul metal       4-anatonium     5-Ancient Metal    6-Fail level keep 7-Fail leveldown  8-Fail dur     9-Mineral	
		{	9,				12,					15,					0,					0,					100,					80,				20,				0,				1	},	//hz was all 1
		{	8,				11,					14,					0,					0,					100,					80,				20,				0,				1	},	
		{	7,				10,					13,					0,					0,					100,					80,				20,				0,				1	},	
		{	6,				9,					12,					0,					0,					100,					80,				20,				0,				1	},	
		{	5,				8,					11,					0,					0,					100,					80,				20,				0,				1	},	

		{	4,				7,					10,					0,					0,					100,					80,				20,				0,				1	},	
		{	3,				6,					9,					0,					0,					100,					80,				20,				0,				1	},	
		{	2,				5,					8,					0,					0,					100,					80,				20,				0,				1	},	
		{	1,				4,					7,					0,					0,					100,					80,				20,				0,				1	},	
		{	1,				3,					5,					0,					0,					100,					80,				20,				0,				1	}
	};*/

	// blackdog
	static const unsigned short s_UpgradeTable[EquipmentInfo::MAX_UPGRADE_LEVEL][MAX_ROW] = 
	{
		//	SUCCESS_SILVIN	SUCCESS_MITH	    SUCCESS_ITER)		SUCCESS_SOUL	    SUCCESS_ANTON	    SUCCESS_ANCIENT돌)	FAIL_LEVEL_KEEP	FAIL_LEVEL_DOWN	FAIL_DUR_KEEP	MINERAL_NUM
		{	55,				0,					0,					0,					0,					0,					0,				0,				100,			10	},	
		{	55,				0,					0,					0,					0,					0,					0,				0,				100,			15	},	
		{	55,				0,					0,					0,					0,					0,					0,				0,				100,			20	},	
		{	55,				0,					0,					0,					0,					0,					0,				0,				100,			25	},	
		{	55,				0,					0,					0,					0,					0,					0,				0,				100,			30	},	

		{	0,				50,					0,					0,					0,					0,					0,				0,				100,			10	},	
		{	0,				50,					0,					0,					0,					0,					0,				0,				100,			15	},	
		{	0,				50,					0,					0,					0,					0,					0,				0,				100,			20	},	
		{	0,				50,					0,					0,					0,					0,					0,				0,				100,			25	},
		{	0,				50,					0,					0,					0,					0,					0,				0,				100,			30	},
		{	0,				0,					50,					0,					0,					0,					0,				0,				100,			10	},
		{	0,				0,					47,					0,					0,					0,					0,				0,				100,			15	},
		{	0,				0,					45,					0,					0,					0,					0,				0,				100,			20	},
		{	0,				0,					40,					0,					0,					0,					0,				0,				100,			25	}
	}; 

	// 업그레이드를 한계까지 했는지 확인
	if (EquipmentInfo::MAX_UPGRADE_LEVEL <= m_cUpgradeLevel)
	{
		return E_UPGRADE_LEVEL_IS_FULL;
	}

	// S그레이드이면 업그레이드 불가능하게 한다.
	/*if(m_GradeInfo.m_eItemGrade == Item::EquipType::S_GRADE)
	{
		return E_UPGRADE_LEVEL_IS_FULL;
	}*///no S limit hz

    // 돈은 충분한가?
	unsigned long dwNeedGold = GetUpgradePrice();
    if (dwCurrentGold_In < dwNeedGold)
    {
        return E_NOT_ENOUGH_MONEY;
    }

	// 내구도는 충분한가.
	unsigned char cMaxDur = GetMaxNumOrDurability();
	if(cMaxDur <= 10)
	{
		return E_NOT_UPGRADE_MINENDURANCE;
	}


    // 광물에 맞춰 성공 확률을 결정
	unsigned short wSuccessPercent = 0;
	float fSuccessLimitGrade = 1.0f;

	unsigned char cType = 0;
	switch (Mineral_InOut.GetPrototypeID())
	{
		case EtcItemID::SILVIN:
			wSuccessPercent = s_UpgradeTable[m_cUpgradeLevel][PERSENT_SUCCESS_SILVIN];
			fSuccessLimitGrade = 0.6f;
			break;
		case EtcItemID::MITHRIL:
			wSuccessPercent = s_UpgradeTable[m_cUpgradeLevel][PERSENT_SUCCESS_MITHRIL];
			fSuccessLimitGrade = 0.6f;
			break;
		case EtcItemID::ITERNIUM:
			wSuccessPercent = s_UpgradeTable[m_cUpgradeLevel][PERSENT_SUCCESS_ITERNIUM];
			fSuccessLimitGrade = 0.6f;
			break;
		case EtcItemID::SOULMETAL:
			wSuccessPercent = s_UpgradeTable[m_cUpgradeLevel][PERSENT_SUCCESS_SOULMETAL];
			break;
		case EtcItemID::ANTONIUM:
			wSuccessPercent = s_UpgradeTable[m_cUpgradeLevel][PERSENT_SUCCESS_ANTONIUM];
			break;
		case EtcItemID::ANCIENTMETAL:
			wSuccessPercent = s_UpgradeTable[m_cUpgradeLevel][PERSENT_SUCCESS_ANCIENTMETAL];
			break;
	}

	switch(m_GradeInfo.m_eItemGrade)
	{
	case Item::EquipType::S_GRADE:	
		wSuccessPercent = (unsigned short)(wSuccessPercent*fSuccessLimitGrade);		
		break;
	case Item::EquipType::AAA_GRADE:
		wSuccessPercent = (unsigned short)(wSuccessPercent*fSuccessLimitGrade);		
		break;
	case Item::EquipType::AA_GRADE:	
		wSuccessPercent = (unsigned short)(wSuccessPercent*fSuccessLimitGrade);		
		break;
	case Item::EquipType::A_GRADE:	
		wSuccessPercent = (unsigned short)(wSuccessPercent*fSuccessLimitGrade*1.1f);		
		break;

	case Item::EquipType::B_GRADE:	wSuccessPercent = (unsigned short)(wSuccessPercent*1.1f);			break;
	case Item::EquipType::C_GRADE:	wSuccessPercent = (unsigned short)(wSuccessPercent*1.1f);		break;
	case Item::EquipType::D_GRADE:	wSuccessPercent = (unsigned short)(wSuccessPercent*1.1f);		break;
	case Item::EquipType::F_GRADE:	wSuccessPercent = (unsigned short)(wSuccessPercent*1.1f);		break;
	default:				break;
	}

	if (0 == wSuccessPercent)
    {
        return E_NOT_MATCH_MINERAL_TYPE;
    }
    
    // 개수는 충분한가?
    int nDiffMineralNum = static_cast<int>(Mineral_InOut.GetNumOrDurability()) - 
        static_cast<int>(s_UpgradeTable[m_cUpgradeLevel][MINERAL_NUM]);

    if (nDiffMineralNum < 0)
    {
        return E_NOT_ENOUGH_MINERAL;
    }

	// 고대의 돌에 경우. 
	if(Mineral_InOut.GetPrototypeID()==EtcItemID::ANCIENTMETAL)
	{		
		if(m_cUpgradeCnt==Item::MAX_UPGRADE_COUNT)
		{
			return E_NOT_UPGRADE_AS_OVER;
		}
		else
		{
			m_cUpgradeCnt++;
		}
	}
	else
	{
//////////
		if(	Mineral_InOut.GetPrototypeID()==EtcItemID::SILVIN	||
			Mineral_InOut.GetPrototypeID()==EtcItemID::ITERNIUM ||
			Mineral_InOut.GetPrototypeID()==EtcItemID::MITHRIL)
/////////
		// ANTONIUM 제외 (100% 확율이기 때문에)
		if(Mineral_InOut.GetPrototypeID()!=EtcItemID::ANTONIUM)
		{
			m_cUpgradeCnt = 0;
		}
	}

    // 광물 제거, 돈 제거
    Mineral_InOut.SetNumOrDurability(nDiffMineralNum);
	dwUsedGold_Out = dwNeedGold;

	// 제련 성공율 조정
	wSuccessPercent = (unsigned short)((float)wSuccessPercent*(CServerSetup::GetInstance().GetRefineDefault() / 100.0f));
/*
#ifdef _DEBUG
	wSuccessPercent = 100;
#endif
*/
	// 제련 성공
	if (wSuccessPercent > Math::Random::ComplexRandom(100))
	{
		++m_cUpgradeLevel;

		// 옵션 강화
		if (m_cUpgradeLevel <= MAX_LEVEL_OPTION_UPGRADE)
		{                
			ApplyUpgradeAttribute(REMOVE); 
			InitializeUpgradeAttribute();  
			ApplyUpgradeAttribute(APPLY);  
		}
		// 소켓 증가
		else
		{        
			++m_cMaxSocket;
		}

	}
	// 제련 실패
	else
	{
		unsigned char cLevelDownNum = 0;

		unsigned char cPersent = static_cast<unsigned char>(Math::Random::ComplexRandom(100));
		if (cPersent < s_UpgradeTable[m_cUpgradeLevel][PERSENT_FAIL_LEVEL_KEEP])
		{
			// 레벨 다운 없음
			cLevelDownNum = 0;
		}
		else
		{
			cPersent -= s_UpgradeTable[m_cUpgradeLevel][PERSENT_FAIL_LEVEL_KEEP];
			if (cPersent < s_UpgradeTable[m_cUpgradeLevel][PERSENT_FAIL_LEVEL1_DOWN])
			{
				// 1레벨 다운
				//cLevelDownNum = 1;// no more level down hz.
				cLevelDownNum = 0;
			}
			else
			{
				// 2레벨 다운
				cLevelDownNum = 0;
				//cLevelDownNum = 2;//no more level down hz.
			}
		}

		// // s그레이드 제련 실패시 레벨다운 없음
		// 속성 최소값에해당하면 레벨다운을 시도하지 않는다.
		for (unsigned char cIndex = 0; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
		{
			if (true == CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
			{
				cLevelDownNum = 0;
				break;
			}
		}

		unsigned char cPersent2 = static_cast<unsigned char>(Math::Random::ComplexRandom(100));
		// 현재 30이란값이 들어있다. 즉 30프로로 내구도가 감소한다.
		if (cPersent2 < s_UpgradeTable[m_cUpgradeLevel][PERSENT_FAIL_DUR_DOWN])
		{
			// edith 2008.07.03 제련 실패시 최대 내구도 감소
			unsigned char cMaxDur = GetMaxNumOrDurability();
			unsigned char cDownDur = 0;

			// 무조건 10씩 감소.
			cDownDur = 150;

			if(cMaxDur-cDownDur <= 0)
				SetMaxNumOrDurability(1);
			else
				SetMaxNumOrDurability(cMaxDur-cDownDur);

			cValue_Out = GetMaxNumOrDurability();

			SetNumOrDurability(1);
		}
		else
		{
			// 내구도 감소 없음.
			cValue_Out = 0;
		}

		for (unsigned char cIndex = 0; cIndex < cLevelDownNum; ++cIndex)
		{
			if (0 < m_cUpgradeLevel)
			{
				// 옵션 다운
				if (m_cUpgradeLevel <= MAX_LEVEL_OPTION_UPGRADE)
				{                
					--m_cUpgradeLevel;

					ApplyUpgradeAttribute(REMOVE);  
					InitializeUpgradeAttribute();   
					ApplyUpgradeAttribute(APPLY);   
				}
				// 소켓 감소
				else
				{    
					// 소켓에 모두 보석이 박혀있는 경우 소켓과 함께 보석 효과도 없애준다.
					if (m_cSocketNum == m_cMaxSocket)
					{
						--m_cSocketNum;
						m_cSocket[m_cSocketNum] = 0;
					}

					ApplyGemAttribute(REMOVE);  
					InitializeGemAttribute();   
					ApplyGemAttribute(APPLY);   

					--m_cMaxSocket;
					--m_cUpgradeLevel;
				}
			}
		}
	}

	// edith 2009.08.31 제련시 소켓의 이상유무를 확인해서 처리한다.
	if (m_cUpgradeLevel <= MAX_LEVEL_OPTION_UPGRADE)
	{
		// 업그레이드 레벨이 5 이하일때 절대 소켓이 2개 이상이 될수 없다.
		if(m_cMaxSocket >= 3)
		{
			// 이건 이상한 아이템이다. 소켓을 0으로 만들어라.
			m_cMaxSocket = 0;
			m_cSocketNum = 0;
			ZeroMemory(m_cSocket, sizeof(m_cSocket));
		}
	}
	else
	{
		if(m_cMaxSocket >= EquipmentInfo::MAX_SOCKET_NUM)
		{
			m_cMaxSocket = EquipmentInfo::MAX_SOCKET_NUM-1;

			if (m_cSocketNum >= m_cMaxSocket)
			{
				m_cSocketNum = m_cMaxSocket;
				m_cSocket[m_cSocketNum] = 0;
			}
		}
	}


	// 그레이드/가격 재계산
	CalculateItemGrade();
	CalculateItemPrice();

	// 3차 밸런스 패치 (S그레이드 아이템 제한선 보정)
	RevisionLimit();

	if(0 == GetSeasonRecord())
		SetNewEquip();

    return S_SUCCESS;
}


bool CUseItem::UseCashItem(CCharacter* lpSender, CCharacter* lpRecver, Item::ItemPos itemPos, unsigned short& wError)
{
    using namespace ItemType;

	if (0 == lpSender->GetStatus().m_nNowHP)
	{ 
		ERRLOG1(g_Log, "CID:0x%08x 죽은 캐릭터가 아이템을 사용하려 합니다.", lpSender->GetCID()); 
		return false; 
	}

    char* szErrorString = NULL;

	switch (m_ItemInfo.m_DetailData.m_cItemType) 
	{
		case CASH_ITEM:
		{
			if (false == lpSender->UseCashItem(lpSender->GetCID(), lpRecver->GetCID(), itemPos, (unsigned short)m_ItemInfo.m_UseItemInfo.m_dwAmount, wError))
			{
				szErrorString = "캐쉬아이템";
				wError = PktUI::USE_FAILED;
			}
			break;
		}

		default:
			szErrorString = "아이템";
			break;
	}

	if (NULL == szErrorString)
	{
		// 캐쉬아이템은 결과를 받은후 내구도를 줄인다.
		if (m_ItemData.m_cNumOrDurability <= 0)
		{
			ERRLOG5(g_Log, "CID:0x%08x %s을(를) 사용하는데 실패했습니다.(내구도:0) "
				"ItemProtoTypeID : %d, SkillID : 0x%04x, LockCount : %d", 
				lpSender->GetCID(), szErrorString, m_ItemData.m_usProtoTypeID, 
				m_ItemInfo.m_UseItemInfo.m_usSkill_ID, m_ItemInfo.m_UseItemInfo.m_usSkill_LockCount); 
			return false;
		}
	}
	else
	{
		ERRLOG5(g_Log, "CID:0x%08x %s을(를) 사용하는데 실패했습니다. "
			"ItemProtoTypeID : %d, SkillID : 0x%04x, LockCount : %d", 
			lpSender->GetCID(), szErrorString, m_ItemData.m_usProtoTypeID, 
			m_ItemInfo.m_UseItemInfo.m_usSkill_ID, m_ItemInfo.m_UseItemInfo.m_usSkill_LockCount); 
		return false;
	}

	return true;
}

bool CUseItem::Use(CCharacter* lpSender, CCharacter* lpRecver, Item::ItemPos itemPos, unsigned short& wError)
{
    using namespace ItemType;

	if (0 == lpSender->GetStatus().m_nNowHP)
	{ 
		ERRLOG1(g_Log, "CID:0x%08x 죽은 캐릭터가 아이템을 사용하려 합니다.", lpSender->GetCID()); 
		return false; 
	}

    char* szErrorString = NULL;

	switch (m_ItemInfo.m_DetailData.m_cItemType) 
	{
		case AMMO:   	    
		{
			if (false == lpSender->UseAmmo(this)) 
			{ 
				szErrorString = "탄환"; 
				wError = PktUI::USE_FAILED;
			} 
			break;
		}

		case POTION:		
		{
			if (false == UsePotion(lpSender, lpRecver)) 
			{ 
				szErrorString = "포션 아이템"; 
				wError = PktUI::USE_FAILED;
				return false;
			} 
			break;
		}
		case FIRE_CRACKER:
		case SKILL_ITEM:
		{
			if (false == UsePotion(lpSender, lpRecver)) 
			{ 
				szErrorString = "포션이나 폭죽, 또는 스킬 아이템"; 
				wError = PktUI::USE_FAILED;
			} 
			break;
		}

		case EXP_BOOK:
		{
			if (false == lpSender->IncrementExp(m_ItemInfo.m_UseItemInfo.m_dwAmount))
			{
				szErrorString = "경험치 쿠폰";
				wError = PktUI::USE_FAILED;
			} 
			break;
		}

		case CASH_BOOK:
		{
			unsigned long dwSrcGold = lpSender->GetGold();
			if (false == lpSender->AddGold(m_ItemInfo.m_UseItemInfo.m_dwAmount, true))
			{
				szErrorString = "캐쉬백";
				wError = PktUI::USE_FAILED;
			}
			else
			{
				GAMELOG::LogTakeGold(*lpSender, 
					dwSrcGold, lpSender->GetGold(), m_ItemInfo.m_UseItemInfo.m_dwAmount, 
					TakeType::TS_INVEN, TakeType::TS_INVEN, GAMELOG::sTakeGoldLogV2::USE_CASHBAG, 0);
			}
			break;
		}
//		case MEDALBOOK://hz medal
//		{
//			unsigned long dwMileage = lpSender->GetMileage() + m_ItemInfo.m_UseItemInfo.m_dwAmount;
//			lpSender->SetMileage(dwMileage);
//			break;
//		}
		case EVENT_LOTTERY:
		{
			if (false == lpSender->UseLottery(m_ItemData.m_usProtoTypeID))
			{
				szErrorString = "복권";
				wError = PktUI::USE_FAILED;
			} 
			break;
		}

		case PORTAL:
		{
			if (false == CCharRespawnMgr::GetInstance().SendRespawnInfo(lpSender->GetCID()))
			{
				szErrorString = "포탈(위치 선택형)";
				wError = PktUI::USE_FAILED;
			}
			break;
		}

		case SKILL_BOOK:     
		{
			unsigned short SkillID = GetItemInfo().m_UseItemInfo.m_usSkill_ID;
			if(SkillID >= 0x8000)
			{
				if (false == lpSender->SkillCreate(this)) 
				{ 
					szErrorString = "스킬북"; 
					wError = PktUI::USE_FAILED;
				} 
			}
			else if(0x1000 <= SkillID && SkillID < 0x2000)
			{
				if (false == lpSender->AbilityCreate(this)) 
				{ 
					szErrorString = "어빌리티북"; 
					wError = PktUI::USE_FAILED;
				} 
			}

			break;
		}

		case FIXED_PORTAL:		
		{
			SERVER_ID serverID;
			serverID.dwID = CServerSetup::GetInstance().GetServerID();

			if(m_ItemInfo.m_UseItemInfo.m_cZone!=serverID.GetZone())
			{
				// 길드전 귀속, 국가전 귀속 체크
				if (CGameTimeMgr::GetInstance().IsGuildWarTime() &&
					(lpSender->GetGuildWarFlag() == Creature::WAR_ON || lpSender->GetGuildWarFlag() == Creature::WAR_INSTANCE))
				{
					szErrorString = "길드전쟁중 포탈(위치 고정형)";
					wError = PktUI::USE_FAILED;
					break;
				}

				// 컨텐츠 : 다크 카나번 국가 전쟁
				if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3) //hayzohar statue war fix was(true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))
				{
					if ((CGameTimeMgr::GetInstance().IsRealmWarReadyTime() || CGameTimeMgr::GetInstance().IsRealmWarTime()) &&
						(lpSender->GetRealmWarFlag() == Creature::WAR_ON || lpSender->GetRealmWarFlag() == Creature::WAR_INSTANCE))
					{
						szErrorString = "석상전쟁중 포탈(위치 고정형)";
						wError = PktUI::USE_FAILED;
						break;
					}
				}

				POS Pos;
				Pos.fPointX	= m_ItemInfo.m_UseItemInfo.m_Pos.m_fPointX;
				Pos.fPointY	= m_ItemInfo.m_UseItemInfo.m_Pos.m_fPointY;
				Pos.fPointZ	= m_ItemInfo.m_UseItemInfo.m_Pos.m_fPointZ;

				lpSender->MoveZone(Pos, m_ItemInfo.m_UseItemInfo.m_cZone, -1);

				break;
			}

			if (false == lpSender->MovePos(m_ItemInfo.m_UseItemInfo.m_Pos, m_ItemInfo.m_UseItemInfo.m_cZone, false))
			{
				szErrorString = "포탈(위치 고정형)";
				wError = PktUI::USE_FAILED;
			} 
			break;
		}

		case CAMP_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::CAMP, wError))
			{
				szErrorString = "길드 요새 스타트킷";
			}
			break;
		}

		case WORLDWEAPON_K_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::KARTERANT_WEAPON, wError))
			{
				szErrorString = "콘라틴 왕의 권위 스타터킷";
			}
			break;
		}

		case WORLDWEAPON_M_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::MERKADIA_WEAPON, wError))
			{
				szErrorString = "생명 추출기 스타터킷";
			}
			break;
		}

		case MINERAL_KIT:
			break;

		case SHORT_RANGE_ARMS_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::SHORT_RANGE_SIEGE_ARMS, wError))
			{
				szErrorString = "근거리 공성병기 스타트킷";
			}
			break;
		}

		case LONG_RANGE_ARMS_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::LONG_RANGE_SIEGE_ARMS, wError))
			{
				szErrorString = "원거리 공성병기 스타트킷";
			}
			break;
		}

		case AIRSHIP_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::AIRSHIP, wError))
			{
				szErrorString = "비공정 스타트킷";
			}
			break;
		}

		case QUEST_ITEM:
		{
			Quest::QuestNode* lpQuestNode = NULL;
			lpQuestNode = CQuestMgr::GetInstance().GetQuestNode(m_ItemInfo.m_UseItemInfo.m_wQuestID);
			if (lpQuestNode)
			{
				if (lpQuestNode->IsPartyQuest() && NULL != lpSender->GetParty())
				{
					CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(lpSender->GetParty());
					if (lpParty)
					{
						lpParty->StartQuest(lpQuestNode, lpSender->GetCurrentPos(), 0);
					}
				}
				else
				{
					if (true == lpSender->CheckQuest(lpQuestNode, wError))
					{
						if (false == lpSender->GiveQuest(lpQuestNode))
						{
							wError = PktUI::FAIL_FULL_QUEST;
							szErrorString = "퀘스트 아이템";
						}
						else
						{
							CGameClientDispatch* lpDispatch = lpSender->GetDispatcher();
							if (NULL != lpDispatch)
							{
								GameClientSendPacket::SendCharStartQuest(lpDispatch->GetSendStream(),
									lpSender->GetCID(), 0, lpQuestNode->m_wQuestID,
									PktBase::NO_SERVER_ERR);

								lpSender->StartPhase(lpQuestNode->m_wQuestID, 1);
							}
						}
					}
					else
					{
						wError = PktUI::FAIL_QUEST_LEVEL;
						szErrorString = "퀘스트 아이템";
					}
				}
			}
			else
			{
				wError = PktUI::NOT_EXIST_QUEST;
				szErrorString = "퀘스트 아이템";
			}
			break;
		}

		default:
			szErrorString = "아이템";
			break;
	}

	if (NULL == szErrorString)
	{
		if (0 < m_ItemData.m_cNumOrDurability)//hz useammo
		{ 
			if( m_ItemInfo.m_DetailData.m_cItemType != AMMO)//hz wasnt here
				--m_ItemData.m_cNumOrDurability; 
		}
	}
	else
	{
		ERRLOG5(g_Log, "CID:0x%08x %s을(를) 사용하는데 실패했습니다. "
			"ItemProtoTypeID : %d, SkillID : 0x%04x, LockCount : %d", 
			lpSender->GetCID(), szErrorString, m_ItemData.m_usProtoTypeID, 
			m_ItemInfo.m_UseItemInfo.m_usSkill_ID, m_ItemInfo.m_UseItemInfo.m_usSkill_LockCount); 
		return false;
	}

	return true;
}

//hz added this for cooldown

bool CheckLastUseTime2(CCharacter* lpCharacter, unsigned short SkillID, unsigned long cooldownTime)
{
	if(cooldownTime > 2000)
		cooldownTime -= 1000;

	if((SkillID >= 0x8D01) && (SkillID <= 0x8D16))
	{
		int index=lpCharacter->GetPositionOfSkillInArr(SkillID);
		if(index == 12)
			return false;

		if(lpCharacter->LastUsePotionTime[index] == 0)
		{
			lpCharacter->LastUsePotionTime[index]=timeGetTime();
			return false;
		}

		long different=timeGetTime() - lpCharacter->LastUsePotionTime[index];

		if(different < cooldownTime)
		{
			return true;
		}
	}
	return false;
}
//hz end

bool CUseItem::UsePotion(CCharacter* lpSender, CCharacter* lpRecver)
{
	AtType attackType;
	attackType.m_wType = m_ItemInfo.m_UseItemInfo.m_usSkill_ID;
	attackType.m_cSkillLockCount = m_ItemInfo.m_UseItemInfo.m_usSkill_LockCount;
	attackType.m_cSkillLevel = 0;

	unsigned short usSpellID = 0;
	switch(attackType.m_wType)
	{
	//case 0x8D12:	usSpellID = Skill::SpellID::CD_8D12;	break;
	//case 0x8D14:	usSpellID = Skill::SpellID::CD_8D14;	break;//wasnt commented out
	//case 0x8D16:	usSpellID = Skill::SpellID::CD_8D16;	break;
	//case 0x99A1:	usSpellID = Skill::SpellID::CD_99A1;	break;
	//case 0x99A2:	usSpellID = Skill::SpellID::CD_99A2;	break;
	//case 0x99A3:	usSpellID = Skill::SpellID::CD_99A3;	break;
	//case 0x9922:    usSpellID = Skill::SpellID::DeCoolDownPotion;	break;
	}


	if(usSpellID != 0)
	{
		// 위에서 검출된 m_wType 에 해당하는 스킬에서 해당 스킬의 버프가 존재하면 
		// 포션사용이 실패한다.
		CSpell* pSpell = lpSender->GetSpellMgr().GetAffectedInfo().GetSpell(usSpellID);
		if (NULL != pSpell)	// 이 버프가 없어져야만 사용가능하다.
			return false;
	}
			bool doSkill=false;
	/*if(attackType.m_wType > 100)//skill
	{
		const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (0 == pThisSkill) 
		{ 
			return false;
		}

		int index=lpSender->GetPositionOfSkillInArr(attackType.m_wType);
		if(index != 12)
		{
			if(CheckLastUseTime2(lpSender,pThisSkill->m_usSkill_ID,pThisSkill->m_dwCoolDownTime))
			{
				CGameClientDispatch* lpDispatch = lpSender->GetDispatcher();
				if (NULL != lpDispatch)
				{
					lpDispatch->Disconnect();
					lpDispatch->Disconnected();
				}
				return false;
			}
		}
			
	}*/


	unsigned char cOffencerJudge = ClientConstants::Judge_SkillItem;
	unsigned char cDefenserJudge = 0;
	unsigned short wOffencerMPHeal = 0;
	unsigned short wDefenserMPHeal = 0;

	unsigned short wError = PktBase::NO_SERVER_ERR;

	unsigned short wDamage = lpRecver->ApplyDamage(attackType, lpSender, 
		cOffencerJudge, cDefenserJudge, wOffencerMPHeal, wDefenserMPHeal, wError);

	CCell* lpCell = lpRecver->GetCellPos().m_lpCell;
	if (NULL != lpCell)
	{
		DefenserNode node;
		
		node.m_wDamage = wDamage;
		node.m_dwCharID = lpRecver->GetCID();
		node.m_sCurrHP = lpRecver->GetStatus().m_nNowHP;
		node.m_sCurrMP = lpRecver->GetStatus().m_nNowMP;
		node.m_wMaxHP = lpRecver->GetStatus().m_StatusInfo.m_nMaxHP;
		node.m_wMaxMP = lpRecver->GetStatus().m_StatusInfo.m_nMaxMP;
		node.m_wMPHeal = wDefenserMPHeal;
		node.m_cJudge = cDefenserJudge;

		lpCell->SendAttackInfo(lpRecver->GetCID(), attackType, 1, &node);
	}


	if(usSpellID != 0)
	{
		// 쿨타임에 관련된 버프를 걸어준다.
		const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (NULL != lpProtoType)
		{
			unsigned short wResult = Skill::CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpSender,
				Skill::SpellType::PAYBUFF_SPELL, usSpellID, attackType.m_cSkillLockCount, lpProtoType->m_dwCoolDownTime/1000))(lpRecver);
		}
	}
	
	CGameClientDispatch* lpDispatch = lpRecver->GetDispatcher();
	if (NULL != lpDispatch)
	{
		GameClientSendPacket::SendCharAttacked(lpDispatch->GetSendStream(), lpSender, lpRecver, 
			attackType, 0, wDamage, cDefenserJudge, wDefenserMPHeal, wError);
	}

	return true;
}



