
#include "stdafx.h"

#include <Utility/Math/Math.h>
#include <Utility/Setup/ServerSetup.h>

#include <Item/Item.h>
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>

#include <GameEvent/GameEventMgr.h>

#include "Monster.h"
#include "AwardTable.h"


using namespace AwardTable;

CAward CAward::ms_this;

unsigned long CAward::GetAward(unsigned char cItemKind, CMonster* lpDeadMonster, CCharacter* lpAttackCharacter, 
							   Item::CItem** lpItem_Out)
{
	if (NULL == lpDeadMonster || NULL == lpAttackCharacter) 
	{ 
		ERRLOG2(g_Log, "CAward::GetAward 포인터 오류입니다. Monster:0x%p, Character:0x%p", lpDeadMonster, lpAttackCharacter);
		return 0; 
	}
	// edith 드랍 아이템의 속성별 확율

	// 이벤트 아이템이 드랍될 타이밍인지 체크한다.
	unsigned long dwResultItem = CGameEventMgr::GetInstance().PopDropEventItem();

	if (0 == dwResultItem)
	{
		const unsigned char cLevel = lpDeadMonster->GetStatus().m_nLevel;
		const unsigned int  iPattern = lpDeadMonster->GetPattern();
		
		using namespace Item::EquipType;

		OptionType	eOptionType		= MAX_OPTION_TYPE;
		Grade		eGrade			= MAX_GRADE;
		float fAddPer = 1.0f;

		switch (cItemKind)
		{
			case MonsterInfo::ORIGINAL1:		
			case MonsterInfo::ORIGINAL2:		
			case MonsterInfo::ORIGINAL3:		
			case MonsterInfo::ORIGINAL4:		
			case MonsterInfo::ORIGINAL5:		
				dwResultItem = lpDeadMonster->GetAwardItem(cItemKind);
				break;

			case MonsterInfo::COIN:
				// 골드에 스틸핸드가 미치는 영향
				if(lpAttackCharacter->GetEnchantInfo().GetFlag(Skill::SpellID::StealHand))
				{
					// 배수로 확정한다. 1이면 
					int iLevel = lpAttackCharacter->GetEnchantLevel(Skill::SpellID::StealHand);
					// 기본 1.0
					fAddPer += ((float)iLevel/10.0f);
				}

				if( lpAttackCharacter->GetAbilityValue(Skill::Type::AB_GETUP_GOLD) )
				{
					fAddPer += ((float)lpAttackCharacter->GetAbilityValue(Skill::Type::AB_GETUP_GOLD)/100.0f);
				}					

				dwResultItem = GetAwardCoin(cLevel, fAddPer);	
				break;

			case MonsterInfo::GEM:				dwResultItem = GetAwardGem(cLevel);					break;
			case MonsterInfo::METAL:			dwResultItem = GetAwardMetal(cLevel);				break;
			case MonsterInfo::POTION:			dwResultItem = GetAwardPotion(cLevel);				break;

			case MonsterInfo::SKILL:			
				dwResultItem = GetAwardSkill(cLevel, lpAttackCharacter->GetRace());
				break;

			case MonsterInfo::CASH_ITEM:		dwResultItem = GetAwardCashItem(lpAttackCharacter->GetRace(), cLevel, iPattern);					break;
			case MonsterInfo::LOTTERY:			dwResultItem = GetAwardLottery();					break;

			case MonsterInfo::F_STANDARD_EQUIP:	eOptionType = STANDARD_OPTION;	eGrade = F_GRADE;	break;
			case MonsterInfo::D_STANDARD_EQUIP:	eOptionType = STANDARD_OPTION;	eGrade = D_GRADE;	break;
			case MonsterInfo::C_STANDARD_EQUIP:	eOptionType = STANDARD_OPTION;	eGrade = C_GRADE;	break;
			case MonsterInfo::B_STANDARD_EQUIP:	eOptionType = STANDARD_OPTION;	eGrade = B_GRADE;	break;
			case MonsterInfo::A_STANDARD_EQUIP:	eOptionType = STANDARD_OPTION;	eGrade = A_GRADE;	break;

			case MonsterInfo::F_OVER_EQUIP:		eOptionType = OVER_OPTION;		eGrade = F_GRADE;	break;
			case MonsterInfo::D_OVER_EQUIP:		eOptionType = OVER_OPTION;		eGrade = D_GRADE;	break;
			case MonsterInfo::C_OVER_EQUIP:		eOptionType = OVER_OPTION;		eGrade = C_GRADE;	break;
			case MonsterInfo::B_OVER_EQUIP:		eOptionType = OVER_OPTION;		eGrade = B_GRADE;	break;
			case MonsterInfo::A_OVER_EQUIP:		eOptionType = OVER_OPTION;		eGrade = A_GRADE;	break;

			case MonsterInfo::NO_EQUIP:			eOptionType = NO_OPTION;							break;

			//--//	start..
			case MonsterInfo::F_RING:			dwResultItem = GetAwardRing(cLevel, F_GRADE);		break;
			case MonsterInfo::D_RING:			dwResultItem = GetAwardRing(cLevel, D_GRADE);		break;
			case MonsterInfo::C_RING:			dwResultItem = GetAwardRing(cLevel, C_GRADE);		break;
			case MonsterInfo::B_RING:			dwResultItem = GetAwardRing(cLevel, B_GRADE);		break;
			case MonsterInfo::A_RING:			dwResultItem = GetAwardRing(cLevel, A_GRADE);		break;
			
			case MonsterInfo::F_NECKLACE:		dwResultItem = GetAwardNecklace(cLevel, F_GRADE);	break;
			case MonsterInfo::D_NECKLACE:		dwResultItem = GetAwardNecklace(cLevel, D_GRADE);	break;
			case MonsterInfo::C_NECKLACE:		dwResultItem = GetAwardNecklace(cLevel, C_GRADE);	break;
			case MonsterInfo::B_NECKLACE:		dwResultItem = GetAwardNecklace(cLevel, B_GRADE);	break;
			case MonsterInfo::A_NECKLACE:		dwResultItem = GetAwardNecklace(cLevel, A_GRADE);	break;

			case MonsterInfo::F_RUNE:			dwResultItem = GetAwardRune(cLevel, F_GRADE);		break;
			case MonsterInfo::D_RUNE:			dwResultItem = GetAwardRune(cLevel, D_GRADE);		break;
			case MonsterInfo::C_RUNE:			dwResultItem = GetAwardRune(cLevel, C_GRADE);		break;
			case MonsterInfo::B_RUNE:			dwResultItem = GetAwardRune(cLevel, B_GRADE);		break;
			case MonsterInfo::A_RUNE:			dwResultItem = GetAwardRune(cLevel, A_GRADE);		break;

			case MonsterInfo::DESTRUCTION_RUNE:	dwResultItem = GetAwardDestructionRune();			break;
			//--//	end..

			default:							return 0;
		}

		Item::CItemFactory& ItemFactory = Item::CItemFactory::GetInstance();
		if (MAX_OPTION_TYPE != eOptionType)
		{
			// 장비의 경우
			dwResultItem = GetAwardEquipment(eOptionType, cLevel, lpAttackCharacter, false);

			*lpItem_Out = ItemFactory.CreateItem(static_cast<unsigned short>(dwResultItem));
			Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(*lpItem_Out);
	        
			if (NULL != lpEquip)
			{
				// edith 2008.01.14 장비에 옵션을 랜덤으로 건다.(설명)
				if(lpEquip->AddRandomOption(eGrade, lpDeadMonster->GetOptionBaseNum(eOptionType), lpAttackCharacter->GetMagicChancePoint()))
					lpEquip->SetNewEquip(2);
				else
					lpEquip->SetNewEquip();
			}
		}
		else
		{
			if (!(dwResultItem & CCell::MONEY_BIT)) 
			{
				// 돈이 아닌 기타 잡템
				*lpItem_Out = ItemFactory.CreateItem(static_cast<unsigned short>(dwResultItem));
				if (NULL != *lpItem_Out)
				{
					(*lpItem_Out)->SetNumOrDurability(1);
				}
			}		
		}
	}

	return dwResultItem;
}


unsigned long CAward::GetAwardCoin(unsigned char cDeadMonsterLevel, float cAddPer)
{
	// edith 2009.06.13 골드 드랍양 조절
//	const unsigned long dwMinMoney = static_cast<unsigned long>(cDeadMonsterLevel) * 2;
//	const unsigned long dwMaxMoney = static_cast<unsigned long>(cDeadMonsterLevel) * 4 + 10;

	const unsigned long dwMinMoney = static_cast<unsigned long>(cDeadMonsterLevel) * 3 + 6;
	const unsigned long dwMaxMoney = static_cast<unsigned long>(cDeadMonsterLevel) * 6 + 12;

	unsigned long dwMoneyAmount = 
		static_cast<unsigned long>( min(Math::Random::ComplexRandom(dwMaxMoney + 1, dwMinMoney) * (CServerSetup::GetInstance().GetDropDefault() / 100.0f) * cAddPer, 
		float(CCell::MAX_MONEY_AMOUNT)) );

	dwMoneyAmount = min(dwMoneyAmount, ULONG_MAX & ~CCell::TYPE_CHECK_BIT);

	return (CCell::MONEY_BIT + dwMoneyAmount);
}


unsigned long CAward::GetAwardGem(unsigned char cDeadMonsterLevel)
{
	cDeadMonsterLevel = std::min(cDeadMonsterLevel, unsigned char(MAX_MONSTER_LEVEL));

	const int nPoint = Math::Random::ComplexRandom(std::accumulate(&(aryGemDropTable[(cDeadMonsterLevel - 1) / 10][0]), 
		&(aryGemDropTable[(cDeadMonsterLevel - 1) / 10][GEM_DROP_TABLE_ROW]), 0));
	int nSpace = 0;

	for (int nAwardTableIndex = 0; nAwardTableIndex < GEM_DROP_TABLE_ROW; ++nAwardTableIndex)
	{
		nSpace += aryGemDropTable[(cDeadMonsterLevel - 1) / 10][nAwardTableIndex];
		if (nSpace > nPoint) 
		{
			return (Item::EtcItemID::GEM_START_ID + 
				Math::Random::ComplexRandom(Item::MAX_GEM_KIND) + (nAwardTableIndex * Item::MAX_GEM_KIND));
		}
	}

	return 0;
}

//--//	start..
unsigned long CAward::GetAwardRing(unsigned char cLevel, unsigned char cGrade)
{
	return Item::CItemMgr::GetInstance().GetRingDropItem(cLevel, cGrade);
}

unsigned long CAward::GetAwardNecklace(unsigned char cLevel, unsigned char cGrade)
{
	return Item::CItemMgr::GetInstance().GetNecklaceDropItem(cLevel, cGrade);
}

unsigned long CAward::GetAwardRune(unsigned char cLevel, unsigned char cGrade)
{
	return Item::CItemMgr::GetInstance().GetRuneDropItem(cLevel, cGrade);
}

unsigned long CAward::GetAwardDestructionRune()
{
	return Item::EtcItemID::DESTRUCTION_RUNE;
}
//--//	end..

unsigned long CAward::GetAwardMetal(unsigned char cDeadMonsterLevel)
{
	cDeadMonsterLevel = std::min(cDeadMonsterLevel, unsigned char(MAX_MONSTER_LEVEL));

	const int nPoint = Math::Random::ComplexRandom(std::accumulate(&(aryMetalDropTable[(cDeadMonsterLevel - 1) / 10][0]), 
		&(aryMetalDropTable[(cDeadMonsterLevel - 1) / 10][METAL_DROP_TABLE_ROW]), 0));
	int nSpace = 0;

	for (int nAwardTableIndex = 0; nAwardTableIndex < METAL_DROP_TABLE_ROW; ++nAwardTableIndex)
	{
		nSpace += aryMetalDropTable[(cDeadMonsterLevel - 1) / 10][nAwardTableIndex];
		if (nSpace > nPoint) 
		{
			// 코어시스템
			if(nAwardTableIndex == 6)
			{
				// edith 2009.09.17 코어시스템
				// 현재로서는 각성의 코어만 있으므로 각성의 코어를 떨군다.
				return Item::EtcItemID::AWAKENCORE;
			}
			else
			{
				int iMetalID = (Item::EtcItemID::MINERAL_START_ID + nAwardTableIndex);

				if(iMetalID > Item::EtcItemID::MINERAL_END_ID)
					iMetalID  = Item::EtcItemID::MINERAL_END_ID;

				return iMetalID;
			}
		}
	}

	return 0;
}

unsigned long CAward::GetAwardPotion(unsigned char cDeadMonsterLevel)
{
	cDeadMonsterLevel = std::min(cDeadMonsterLevel, unsigned char(MAX_MONSTER_LEVEL));

	const int nPoint = Math::Random::ComplexRandom(std::accumulate(&(aryPotionDropTable[(cDeadMonsterLevel - 1) / 10][0]), 
		&(aryPotionDropTable[(cDeadMonsterLevel - 1) / 10][POTION_DROP_TABLE_ROW]), 0));

	int nSpace = 0;
	
	for (int nAwardTableIndex = 0; nAwardTableIndex < POTION_DROP_TABLE_ROW; ++nAwardTableIndex)
	{
		nSpace += aryPotionDropTable[(cDeadMonsterLevel - 1) / 10][nAwardTableIndex];
		if (nSpace > nPoint)
		{
			if(nAwardTableIndex <= 3)
			{
				const int iType  = Math::Random::ComplexRandom(2);
				// 빵들.
				if(iType == 0)
				{
					switch (nAwardTableIndex)
					{
						case 0:		return Item::EtcItemID::BREAD;
						case 1:		return Item::EtcItemID::RARE_STEAK;
						case 2:		return Item::EtcItemID::WELLDONE_STEAK;
						case 3:		return Item::EtcItemID::BACON;
					}
				}
				else if(iType == 1)
				{
					switch (nAwardTableIndex)
					{
						case 0:		return Item::EtcItemID::MANNA1;
						case 1:		return Item::EtcItemID::MANNA2;
						case 2:		return Item::EtcItemID::MANNA3;
						case 3:		return Item::EtcItemID::MANNA4;
					}
				}
				else if(iType == 2)
				{
					switch (nAwardTableIndex)
					{
						case 0:		return Item::EtcItemID::SOUP1;
						case 1:		return Item::EtcItemID::SOUP2;
						case 2:		return Item::EtcItemID::SOUP3;
						case 3:		return Item::EtcItemID::SOUP4;
					}
				}

			}
			else
			{
				// 4부터 포션 0랩 포션
				// 포션계열,
				int iLevel = nAwardTableIndex-4;
				const int iType  = Math::Random::ComplexRandom(14);
				switch(iType)
				{
				case 0:			return Item::EtcItemID::POTION1+iLevel;
				case 1:			return Item::EtcItemID::POTION2+iLevel;
				case 2:			return Item::EtcItemID::POTION3+iLevel;
				case 3:			return Item::EtcItemID::POTION4+iLevel;
				case 4:			return Item::EtcItemID::POTION5+iLevel;
				case 5:			return Item::EtcItemID::POTION6+iLevel;
				case 6:			return Item::EtcItemID::POTION7+iLevel;
				case 7:			return Item::EtcItemID::POTION8+iLevel;
				case 8:			return Item::EtcItemID::POTION9+iLevel;
				case 9:			return Item::EtcItemID::POTION10+iLevel;
				case 10:		return Item::EtcItemID::POTION11+iLevel;
				case 11:		return Item::EtcItemID::POTION12+iLevel;
				case 12:		return Item::EtcItemID::POTION13+iLevel;
				case 13:		return Item::EtcItemID::POTION14+iLevel;
//				case 14:		return Item::EtcItemID::POTION15+iLevel;
				}
			}
		}
	}

	return 0;
}


unsigned long CAward::GetAwardSkill(unsigned char cDeadMonsterLevel, char cAttackCharacterNation)
{
	cDeadMonsterLevel = std::min(cDeadMonsterLevel, unsigned char(MAX_MONSTER_LEVEL));
	const short nDeadMonsterLevelRange = (cDeadMonsterLevel - 1) / 10;

	const short nPoint = static_cast<short>(Math::Random::ComplexRandom(arySkillBookDropTable[nDeadMonsterLevelRange] * 3));

	short nSkillLevel = (101 <= cDeadMonsterLevel) ? 4 : nPoint % arySkillBookDropTable[nDeadMonsterLevelRange];

	// edith 2008.05.07 스킬북 드랍 설정
   	if (true == CServerSetup::GetInstance().UseContents(GameRYL::LEVEL_LIMIT_100)) // was 90
	{
		// 4단계까지 드랍 가능
		nSkillLevel = min(nSkillLevel, short(3));
	}
	else
	{
		// 3단계까지 드랍 가능
		nSkillLevel = min(nSkillLevel, short(2));
	}
/*
	else if (true == CServerSetup::GetInstance().UseContents(GameRYL::LEVEL_LIMIT_80))
	{
		// 4단계까지 드랍 가능
		nSkillLevel = min(nSkillLevel, short(3));
	}
	else
	{
		// 3단계까지 드랍 가능
		nSkillLevel = min(nSkillLevel, short(2));
	}
*/
	const CClass::JobLevel nSkillType = (nPoint < arySkillBookDropTable[nDeadMonsterLevelRange] || 4 <= nSkillLevel) ? 
		CClass::JOB_CHANGE_1ST : CClass::DEFAULT_CLASS;

	int  nSkillKind = (4 <= nSkillLevel) ? FIFTHSKILL_KIND : SKILL_KIND;
	unsigned short* arySkillShuffleList = new unsigned short[nSkillKind];

	for(int i = 0; i < nSkillKind; i++)
	{
		arySkillShuffleList[i] = 0;
	}

	if(4 <= nSkillLevel)
	{
		std::copy(aryDropableFifthSkillbookList, aryDropableFifthSkillbookList + nSkillKind, arySkillShuffleList);
	}
	else
	{
		std::copy(aryDropableSkillbookList, aryDropableSkillbookList + nSkillKind, arySkillShuffleList);
	}
	
	std::random_shuffle(arySkillShuffleList, arySkillShuffleList + nSkillKind);

	for (int nAwardTableIndex = 0; nAwardTableIndex < nSkillKind; nAwardTableIndex++)
	{
		const unsigned char cSkillBookClass = (arySkillShuffleList[nAwardTableIndex] & ~Skill::SKILL_MASK) >> 8;
		if ((cSkillBookClass >> 4) == cAttackCharacterNation && CClass::GetJobLevel(cSkillBookClass) == nSkillType) 
		{
			return Item::CItemMgr::GetInstance().GetItemIDFromSkillID(arySkillShuffleList[nAwardTableIndex], nSkillLevel);
		}
	}

	delete arySkillShuffleList;

	return 0;
}


unsigned short CAward::GetAwardEquipment(Item::EquipType::OptionType eOptionType, 
										 unsigned char cDeadMonsterLevel, CCharacter* lpCharacter, bool bQuestAward)
{
	unsigned short wItemID = 0;

	// 솔로잉
	if (NULL == lpCharacter->GetParty() || true == bQuestAward)
	{
		// edith 2008.01.14 40프로 확율로 클래스에 알맞는 장비 드랍
		// 40%의 확률로 클래스에 알맞은 장비를 드랍한다. 
		if (Math::Random::ComplexRandom(100) < (unsigned long)(40+lpCharacter->GetAbilityValue(Skill::Type::AB_MYEQUIP_UP)))
		{
			unsigned char aryShuffleList[PRIMARY_EQUIP_TABLE_ROW] = { 0, };

			std::copy(aryPrimaryEquipList[lpCharacter->GetClass()], 
				aryPrimaryEquipList[lpCharacter->GetClass()] + PRIMARY_EQUIP_TABLE_ROW, 
				aryShuffleList);

			std::random_shuffle(aryShuffleList, aryShuffleList + PRIMARY_EQUIP_TABLE_ROW);

			Item::CItemType& itemType = Item::CItemType::GetInstance();
			for (unsigned char cIndex = 0; cIndex < PRIMARY_EQUIP_TABLE_ROW; ++cIndex)
			{
				Item::ItemType::Type eItemType = static_cast<Item::ItemType::Type>(aryShuffleList[cIndex]);

				// edith 2008.01.14 옵션타입 알아오기 (스탠다드옵션, 오버옵션, 논옵션)
				if (MAX_ITEM_TYPE != aryShuffleList[cIndex] && 
					true == itemType.IsCorrectOptionType(eOptionType, eItemType))
				{
					wItemID = Item::CItemMgr::GetInstance().GetDropItem(eItemType, cDeadMonsterLevel);
					if (0 == wItemID) { continue; }
					break;
				}
			}
		}
		else
		{
			if (CClass::HUMAN == lpCharacter->GetRace())//lpCharacter->GetRace()) // hz race to nation change
			{
				wItemID = SelectItemByRace(eOptionType, aryHumanEquipTypeList, HUMAN_EQUIP_TYPE_NUM, cDeadMonsterLevel);
			}
			else if (CClass::AKHAN == lpCharacter->GetRace())//lpCharacter->GetRace()) // hz race to nation change
			{
				wItemID = SelectItemByRace(eOptionType, aryAkhanEquipTypeList, AKHAN_EQUIP_TYPE_NUM, cDeadMonsterLevel);
			}
			else
			{
				ERRLOG2(g_Log, "CID:0x%08x 캐릭터의 종족이 이상합니다. 종족:%d", 
					lpCharacter->GetCID(), lpCharacter->GetRace());
				return 0;
			}
		}
	}
	// 파티 플레이
	else
	{
		CCharacterParty* lpParty = reinterpret_cast<CCharacterParty *>(lpCharacter->GetParty());

		CCharacter* aryNearCharacterList[PARTY::MAX_MEM];
		unsigned char cHighestLevel = 0;

		const unsigned char cNearPartyMemberNum = lpParty->GetNearMemberList(lpCharacter->GetCellPos().m_lpCell, 
			true, aryNearCharacterList, cHighestLevel);
		if (0 == cNearPartyMemberNum) { return 0; }

		unsigned char cHumanNum = 0;
		for (unsigned char cMemberIndex = 0; cMemberIndex < cNearPartyMemberNum; ++cMemberIndex)
		{
			if (CClass::HUMAN == aryNearCharacterList[cMemberIndex]->GetRace())//GetRace()) // hz race to nation change
			{
				++cHumanNum;
			}
		}

		// 파티원의 종족 비율에 의해 드랍될 장비 타입의 확률이 결정된다.
		if (static_cast<unsigned long>(cHumanNum * 100.0f / cNearPartyMemberNum) > Math::Random::ComplexRandom(100))
		{
			wItemID = SelectItemByRace(eOptionType, aryHumanEquipTypeList, HUMAN_EQUIP_TYPE_NUM, cDeadMonsterLevel);
		}
		else
		{
			wItemID = SelectItemByRace(eOptionType, aryAkhanEquipTypeList, AKHAN_EQUIP_TYPE_NUM, cDeadMonsterLevel);
		}
	}

	if (0 == wItemID)
	{
		ERRLOG0(g_Log, "드랍할 장비를 결정하지 못하였습니다.");
		return 0;
	}

	return wItemID;
}

unsigned short CAward::GetQuestEquipmentClass(Item::EquipType::OptionType eOptionType, unsigned char cClass,
										 unsigned char cDeadMonsterLevel, CCharacter* lpCharacter)
{
	unsigned short wItemID = 0;

	unsigned char aryShuffleList[PRIMARY_EQUIP_TABLE_ROW] = { 0, };

	std::copy(aryPrimaryEquipList[cClass], 
		aryPrimaryEquipList[cClass] + PRIMARY_EQUIP_TABLE_ROW, 
		aryShuffleList);

	std::random_shuffle(aryShuffleList, aryShuffleList + PRIMARY_EQUIP_TABLE_ROW);

	Item::CItemType& itemType = Item::CItemType::GetInstance();
	for (unsigned char cIndex = 0; cIndex < PRIMARY_EQUIP_TABLE_ROW; ++cIndex)
	{
		Item::ItemType::Type eItemType = static_cast<Item::ItemType::Type>(aryShuffleList[cIndex]);

		// edith 2008.01.14 옵션타입 알아오기 (스탠다드옵션, 오버옵션, 논옵션)
		if (MAX_ITEM_TYPE != aryShuffleList[cIndex] && 
			true == itemType.IsCorrectOptionType(eOptionType, eItemType))
		{
			wItemID = Item::CItemMgr::GetInstance().GetDropItem(eItemType, cDeadMonsterLevel);
			if (0 == wItemID) { continue; }
			break;
		}
	}

	if (0 == wItemID)
	{
		ERRLOG0(g_Log, "드랍할 장비를 결정하지 못하였습니다.");
		return 0;
	}

	return wItemID;
}

unsigned long CAward::GetAwardCashItem(char cAttackCharacterNation, unsigned char cDeadMonsterLevel, unsigned int cDeadMonsterPattern)
{
	cDeadMonsterLevel = std::min(cDeadMonsterLevel, unsigned char(MAX_MONSTER_LEVEL));

	int iMonLevel = (cDeadMonsterLevel - 1) / 10;

	if(cDeadMonsterPattern == MonsterInfo::PATTERN_CHIEF)	// 치프면 11
		iMonLevel = 11;
	else if(cDeadMonsterPattern == MonsterInfo::PATTERN_NAMED)	// 네임드면 12 테이블
		iMonLevel = 12;

	const int nPoint = Math::Random::ComplexRandom(std::accumulate(&(aryCashDropTable[iMonLevel][0]), 
		&(aryCashDropTable[iMonLevel][CASH_DROP_TABLE_ROW]), 0));

	// 캐쉬템의 드랍아이템 리스트는 여기에

	int nSpace = 0;
	for (int nAwardTableIndex = 0; nAwardTableIndex < CASH_DROP_TABLE_ROW; ++nAwardTableIndex)
	{
		nSpace += aryCashDropTable[(cDeadMonsterLevel - 1) / 10][nAwardTableIndex];
		if (nSpace > nPoint)
		{
			if(nAwardTableIndex == 0)		// 망각의돌
				return Item::EtcItemID::OBLIVION_STONE;
			else if(nAwardTableIndex == 4)		// 내구도석
				return Item::EtcItemID::ENDURANCESTONE;
			else if(nAwardTableIndex == 5)		// 부활의돌
				return Item::EtcItemID::REBIRTH_STONE;
			else if(nAwardTableIndex == 6)		// 탈것
			{
				const int iType  = Math::Random::ComplexRandom(2);
				if(cAttackCharacterNation == 0)	//휴먼
				{
					if(iType == 0)
						return Item::EtcItemID::RIDEH1;
					else
						return Item::EtcItemID::RIDEH2;
				}
				else	// 아칸
				{
					if(iType == 0)
						return Item::EtcItemID::RIDEA1;
					else
						return Item::EtcItemID::RIDEA2;
				}
			}
			else if(nAwardTableIndex == 3)		// 오브
			{
				const int iType  = Math::Random::ComplexRandom(2);
				if(iType == 0)
					return Item::EtcItemID::ORB_EXP;
				else
					return Item::EtcItemID::ORB_LUCK;
			}
			else if(nAwardTableIndex == 1 || nAwardTableIndex == 2)		// 유료 포션
			{
				int iLevel = nAwardTableIndex-1;

				int iType = 0;
				const int iRage  = Math::Random::ComplexRandom(100);

				if(iRage < 60)
				{
					iType  = Math::Random::ComplexRandom(2);
				}
				else
				{
					iType  = 2+Math::Random::ComplexRandom(12);
				}

				switch(iType)
				{
				case 0:			return Item::EtcItemID::CASHPOTION1+iLevel;
				case 1:			return Item::EtcItemID::CASHPOTION2+iLevel;
				case 2:			return Item::EtcItemID::CASHPOTION3+iLevel;
				case 3:			return Item::EtcItemID::CASHPOTION4+iLevel;
				case 4:			return Item::EtcItemID::CASHPOTION5+iLevel;
				case 5:			return Item::EtcItemID::CASHPOTION6+iLevel;
				case 6:			return Item::EtcItemID::CASHPOTION7+iLevel;
				case 7:			return Item::EtcItemID::CASHPOTION8+iLevel;
				case 8:			return Item::EtcItemID::CASHPOTION9+iLevel;
				case 9:			return Item::EtcItemID::CASHPOTION10+iLevel;
				case 10:		return Item::EtcItemID::CASHPOTION11+iLevel;
				case 11:		return Item::EtcItemID::CASHPOTION12+iLevel;
				case 12:		return Item::EtcItemID::CASHPOTION13+iLevel;
				case 13:		return Item::EtcItemID::CASHPOTION14+iLevel;
//				case 14:		return Item::EtcItemID::CASHPOTION15+iLevel;
				}
			}
		}
	}

	ERRLOG0(g_Log, "드랍할 캐쉬아이템을 결정하지 못하였습니다.");
	return 0;//Item::EtcItemID::OBLIVION_STONE;
}


unsigned long CAward::GetAwardLottery(void)
{
	if (false == CServerSetup::GetInstance().GetLotteryEvent())
	{
		ERRLOG0(g_Log, "복권 이벤트를 하고 있지 않은데, 복권 드랍을 요청하였습니다.");
		return 0;
	}

	return CGameEventMgr::GetInstance().GetLotteryEvent().GetLottery();
}


unsigned short CAward::SelectItemByRace(Item::EquipType::OptionType eOptionType, 
										const unsigned char* aryItemType, unsigned char cMaxItemType, unsigned char cLevel)
{
	unsigned char aryShuffleList[UCHAR_MAX] = { 0, }; 

	std::copy(aryItemType, aryItemType + cMaxItemType, aryShuffleList); 
	std::random_shuffle(aryShuffleList, aryShuffleList + cMaxItemType);

	unsigned short wItemID = 0;

	for (unsigned char cIndex = 0; cIndex < cMaxItemType; ++cIndex) 
	{ 
		Item::ItemType::Type eItemType = static_cast<Item::ItemType::Type>(aryShuffleList[cIndex]);
		if (true == Item::CItemType::GetInstance().IsCorrectOptionType(eOptionType, eItemType))
		{
			wItemID = Item::CItemMgr::GetInstance().GetDropItem(eItemType, cLevel); 
			if (0 != wItemID) 
			{
				break;
			}
		}
	}

	return wItemID;
}


