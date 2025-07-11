
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
// Desc : InstallSocket - ���Ͽ� ������ �ڴ� ����.
//
//	1. ������ ã�´�. 
//	2. ��� ã�´�. 
//	3. ��� ������ �ڴ´�. 
//	4. ������ ���ش�. 
//	5. ĳ���� �ɷ�ġ�� �����Ѵ�.
// --------------------------------------------------------------------------------------
CEquipment::EQUIPMENT_ERROR CEquipment::InstallSocket(CItem& Gem)
{
    using namespace Item;

    // �������� ������ �´°� Ȯ��
    unsigned short usGemTypeID = Gem.GetPrototypeID();

	// edith 2008.08.10 ���� ���� ���� ó��
	// 8�� ���� ���� �����̸�..
	if(usGemTypeID == EtcItemID::GEM_MAXSOCKET_ID)
	{
		// ���� ���� Ȯ��
		if (0 == Gem.GetNumOrDurability())
		{
			return E_GET_GEM_FAILED;
		}

		if(m_cUpgradeLevel != EquipmentInfo::MAX_UPGRADE_LEVEL)  // blackdog supermod
		//	if(m_cUpgradeLevel < 15 )
		{
			return E_SOCKET_NOT_MAX;
		}

		// ���� ��ȣ�� 7���� �ƴϸ�.
		if(m_cMaxSocket != EquipmentInfo::MAX_SOCKET_NUM-1) // blackdog supermod
		//	if(m_cMaxSocket < 12 )
		{
			return E_SOCKET_NOT_MAX;
		}

		// �ִ� ������ 1�� ������Ų��.
		++m_cMaxSocket;
		if(m_cMaxSocket >= EquipmentInfo::MAX_SOCKET_NUM)
			m_cMaxSocket = EquipmentInfo::MAX_SOCKET_NUM;

		Gem.SetNumOrDurability(Gem.GetNumOrDurability() - 1);

		return S_SUCCESS;
	}

	// UnInstall Socket	�����̸�.
	if(usGemTypeID == EtcItemID::GEM_UNSOCKET_ID)
	{
		// ���� ���� Ȯ��
		if (0 == Gem.GetNumOrDurability())
		{
			return E_GET_GEM_FAILED;
		}	

		// �ν���� ������ ����.
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

		// �׷��̵�/���� ����
		CalculateItemGrade();
		CalculateItemPrice();

		// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
		RevisionLimit();

		return S_SUCCESS;
	}

	// ���� �����ۿ� ���� �ִ� ���� ���� �˻�. ������ ���� ���� �� �� �ڵ��� �Ѵ�. 
    if (EquipmentInfo::MAX_SOCKET_NUM < m_cMaxSocket ||
        m_cMaxSocket <= m_cSocketNum)
    {
        return E_OVER_MAX_SOCKET;
    }

	// ����Ʈ ������ ���� ���Ͽ� ���ڰ� �����Ѵ�.
    if (usGemTypeID < EtcItemID::GEM_START_ID || EtcItemID::GEM_RUNE_END_ID < usGemTypeID)
    {   
        return E_NOT_GEM;
    }

	// �麸���̸� ������ �մ� ��� �˻��Ѵ�.
	if(EtcItemID::GEM_RUNE_START_ID <= usGemTypeID)
	{
		 // �麸���� ������ �ִ� ��� ������ �ִ�.
		CEquipment* lpEquip = CEquipment::DowncastToEquipment(this);
		if (NULL == lpEquip)
			return E_NOT_GEM;

		// ���� ���� �ƸӸ� �Ѵ�.
		switch (Item::CItemType::GetEquipType(lpEquip->GetItemInfo().m_DetailData.m_dwFlags))
		{
			case Item::CItemType::HELM_TYPE:
			case Item::CItemType::ARMOUR_TYPE:
				// �̰��϶� �麸���� ������ �ִ�.
				break;
			default:
				// ��2���̿ܿ��� ������ ������ ����.
				return E_NOT_GEM;
		}
	}

    // ���� ���� Ȯ��
    if (0 == Gem.GetNumOrDurability())
    {
        return E_GET_GEM_FAILED;
    }

    unsigned char cGemType = usGemTypeID - EtcItemID::GEM_START_ID;

    // �� �Ӽ� ���
    ApplyGemAttribute(REMOVE);

    // cGemType�� 0�� �� �ֱ� ������, +1�� �ؼ� �־��ش�.
	AddSocket(cGemType + 1);    

    InitializeGemAttribute();
    ApplyGemAttribute(APPLY);

	Gem.SetNumOrDurability(Gem.GetNumOrDurability() - 1);

	// �׷��̵�/���� ����
	CalculateItemGrade();
	CalculateItemPrice();

	// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
	RevisionLimit();

	// edith ���ο� ���� ����.
	if(0 == GetSeasonRecord())
		SetNewEquip();

	return S_SUCCESS;
}

CEquipment::EQUIPMENT_ERROR CEquipment::InstallRuneSocket(CItem& Rune, bool bEquip, unsigned char cLevel)
{
	// �������� �� �������� �´°� Ȯ��.
	unsigned short usRuneTypeID = Rune.GetPrototypeID();

	if(usRuneTypeID < EtcItemID::RUNE_START_ID || EtcItemID::RUNE_END_ID < usRuneTypeID)
	{   
		return E_NOT_RUNE;
	}

	// �� �������� ���� �������� üũ.

	unsigned char cRuneSlot = GetRuneEquipable();
	
	if(!cRuneSlot)
	{
		return E_NOT_RUNE_SLOT;
	}

	// �� �����ۿ� �������� üũ(��������).
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

	// �� ���� Ȯ��
	if(!Rune.GetNumOrDurability())
	{
		return E_GET_RUNE_FAILED;
	}

	unsigned short usRuneType = usRuneTypeID - EtcItemID::RUNE_START_ID;

	// ���� �켱����
//	AddAttribute(Item::Attribute::Type::RUNE, usRuneType);

	Rune.SetNumOrDurability(Rune.GetNumOrDurability() - 1);

	// �׷��̵�/���� ����
	CalculateItemGrade();
	CalculateItemPrice();

	// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
	RevisionLimit();

	// edith ���ο� ���� ����.
	if(0 == GetSeasonRecord())
		SetNewEquip();

	return R_SUCCESS;
}

CEquipment::EQUIPMENT_ERROR CEquipment::UnInstallRuneSocket(CItem& Rune, bool bEquip, unsigned char cLevel, unsigned short usSkillPoint)
{	
	// �������� �� �������� �´°� Ȯ��.
	unsigned short usRuneTypeID = Rune.GetPrototypeID();

	if(usRuneTypeID < EtcItemID::RUNE_START_ID || EtcItemID::RUNE_END_ID < usRuneTypeID)
	{   
		return E_NOT_RUNE;
	}
	
	// �Ҹ� ������ üũ.
	if(usRuneTypeID!=EtcItemID::DESTRUCTION_RUNE)
	{
		return E_NOT_RUNE;
	}

	// ���� �������� üũ (��ų����Ʈ).

	if(bEquip)
	{
		if(usSkillPoint<GetAllRuneSkillPointCheck())
		{
			return E_USED_RUNE_SKILL_POINT;
		}
	}

	// �� ���� Ȯ��
	if(!Rune.GetNumOrDurability())
	{
		return E_GET_RUNE_FAILED;
	}
	
	// ��� �Ӽ��� �����Ѵ�.
	if(!SubRuneAttribute())
	{
		return E_NOT_RUNE;
	}

	Rune.SetNumOrDurability(Rune.GetNumOrDurability() - 1);

	// �׷��̵�/���� ����
	CalculateItemGrade();
	CalculateItemPrice();

	// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
	RevisionLimit();

	return R_SUCCESS;
}

unsigned short Item::CEquipment::GetAllRuneSkillPointCheck()
{
	// ���� �����ۿ� �ִ� ��� �� �����ۿ� �Ӽ��� ��ų����Ʈ �������ش�.

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
	// �� �������� ���� �Ǿ� ������ ��ų ����Ʈ üũ.

	Item::CEquipment* lpDstEquipment = Item::CEquipment::DowncastToEquipment(lpCharacter->GetItem(itemPos));

	unsigned short usRuneSkillPoint = GetAllRuneSkillPointCheck();

	SKILL skill = lpCharacter->GetSkill();

	if(usRuneSkillPoint)
	{
		short sSkillPoint = lpCharacter->GetStatus().m_StatusInfo.m_wSkillPoint - skill.GetSkillNum();			

		if(sSkillPoint<0)
		{				
			ERRLOG1(g_Log, "CID:0x%08x ����� ��ų����Ʈ�� �ִ� ��ų����Ʈ���� Ŭ ���", lpCharacter->GetCID());
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
													unsigned char& cValue_Out)	// cValue_Out ������ �ٿ����� ���.
{
	if((m_ItemData.m_usProtoTypeID==62601)||(m_ItemData.m_usProtoTypeID==62601))
		
	//	||(m_ItemData.m_usProtoTypeID==601)||(m_ItemData.m_usProtoTypeID==5101)||(m_ItemData.m_usProtoTypeID==5101)
	//	||(m_ItemData.m_usProtoTypeID==5201)||(m_ItemData.m_usProtoTypeID==5401)||(m_ItemData.m_usProtoTypeID==5110)
	//	||(m_ItemData.m_usProtoTypeID==5210)||(m_ItemData.m_usProtoTypeID==5801)||(m_ItemData.m_usProtoTypeID==6107))
	//	return E_UPGRADE_LEVEL_IS_FULL; // E_GRAFE_FAILD
	    return E_UPGRADE_LEVEL_IS_FULL; // E_GRAFE_FAILD

    dwUsedGold_Out = 0;

	// edith 2009.09.17 �ھ�ý��� �߰�
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

		// ���翬 ���� ������
		static const unsigned short s_UpgradeCoreTable[EquipmentInfo::MAX_CORE_LEVEL][MAX_CORE_ROW] = 
		{
			//  ����Ȯ��		�ٿ�Ȯ��(0)		�ٿ�Ȯ��(-1)	�����ٿ�Ȯ��	���� ����
			{	50,				100,			0,				60,				1	},	
			{	50,				100,			0,				60,				1	},	
			{	35,				100,			0,				60,				1	},	
			{	19,				100,			0,				60,				1	},	
			{	3,				100,			0,				60,				1	}
		};

		// ������ �ھ 100�ۼ�Ʈ ����
		if(GetSeasonRecord() == 3 && m_cCoreLevel >= 5)	// ������ �����ڵ��̸�.
		{
			// �̹� ������ �������̸� �ٽ� �����Ҽ� ����.
			return E_NOT_UPGRADE_AWAKEN;
		}

		// ���� ����Ѱ�?
		unsigned long dwNeedGold = GetUpgradePrice();
		if (dwCurrentGold_In < dwNeedGold)
		{
			return E_NOT_ENOUGH_MONEY;
		}

		// �������� ����Ѱ�.
		unsigned char cMaxDur = GetMaxNumOrDurability();
		if(cMaxDur <= 50)
		//	if(cMaxDur <= 10)
		{
			return E_NOT_UPGRADE_MINENDURANCE;
		}

		// ������ ����Ѱ�?
		
		int nDiffMineralNum = static_cast<int>(Mineral_InOut.GetNumOrDurability() - 1);

		if (nDiffMineralNum < 0)
		{
			return E_NOT_ENOUGH_MINERAL;
		}

		// ������ ���� ���� Ȯ���� ����
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

		// ���� ����, �� ����
		Mineral_InOut.SetNumOrDurability(nDiffMineralNum);
		dwUsedGold_Out = dwNeedGold;

		// ���� ������ ����
		wSuccessPercent = (unsigned short)((float)wSuccessPercent*(CServerSetup::GetInstance().GetRefineDefault() / 100.0f));

		// ���� ����
		if (wSuccessPercent > Math::Random::ComplexRandom(100))
		{
			// ������ �����ڵ�� �����Ѵ�.
			if(3 != GetSeasonRecord())
			{
				SetNewEquip(3);
				m_cCoreLevel = 1;
			}
			else
				m_cCoreLevel++;
		}
		// ���� ����
		else
		{
			unsigned char cLevelDownNum = 0;

			unsigned char cPersent = static_cast<unsigned char>(Math::Random::ComplexRandom(100));
			if (cPersent < s_UpgradeCoreTable[m_cCoreLevel][CORE_PERSENT_FAIL_LEVEL_KEEP])
			{
				// ���� �ٿ� ����
				cLevelDownNum = 0;
			}
			else
			{
				cPersent -= s_UpgradeCoreTable[m_cCoreLevel][CORE_PERSENT_FAIL_LEVEL_KEEP];
				if (cPersent < s_UpgradeCoreTable[m_cCoreLevel][CORE_PERSENT_FAIL_LEVEL1_DOWN])
				{
					// 1���� �ٿ�
					//cLevelDownNum = 1;
					cLevelDownNum = 0;
				}
				else
				{
					// 2���� �ٿ�
					cLevelDownNum = 0;
					//cLevelDownNum = 2;
				}
			}

			// ������ �ٿ��Ų��.
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
			// ���� 30�̶����� ����ִ�. �� 30���η� �������� �����Ѵ�.
			if (cPersent2 < s_UpgradeCoreTable[m_cCoreLevel][CORE_PERSENT_FAIL_DUR_DOWN])
			{
				// edith 2008.07.03 ���� ���н� �ִ� ������ ����
				unsigned char cMaxDur = GetMaxNumOrDurability();
				unsigned char cDownDur = 0;

				// ������ 10�� ����.
				cDownDur = 150;

				if(cMaxDur-cDownDur <= 0)
					SetMaxNumOrDurability(1);
				else
					SetMaxNumOrDurability(cMaxDur-cDownDur);

				SetNumOrDurability(1);
			}
			cValue_Out = GetMaxNumOrDurability();
		}

		// �׷��̵�/���� ����
		CalculateItemGrade();
		CalculateItemPrice();

		// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
		RevisionLimit();

		return S_SUCCESS;
	}	
	else if(Mineral_InOut.GetPrototypeID() == EtcItemID::ENDURANCESTONE)
	{
		// ������ ȸ�� �������� 100�ۼ�Ʈ ����

		// ���� ����Ѱ�?
		unsigned long dwNeedGold = GetUpgradePrice();
		if (dwCurrentGold_In < dwNeedGold)
		{
			return E_NOT_ENOUGH_MONEY;
		}

		// ������ ����Ѱ�?
		int nDiffMineralNum = static_cast<int>(Mineral_InOut.GetNumOrDurability() - 1);

		if (nDiffMineralNum < 0)
		{
			return E_NOT_ENOUGH_MINERAL;
		}

		// �������� ����Ѱ�.
		unsigned char cMaxDur = GetMaxNumOrDurability();
		if(cMaxDur >= 150) // WAS 100
		{
			return E_NOT_UPGRADE_ENDURANCE;
		}

		// edith 2008.07.03 �ִ� ������ ���� ������ ��뼺��.
		unsigned char cCurDur = GetNumOrDurability();

		// ���� ����, �� ����
		Mineral_InOut.SetNumOrDurability(nDiffMineralNum);
		dwUsedGold_Out = dwNeedGold;

		//cMaxDur = cMaxDur+100;
		cMaxDur = cMaxDur+150; // was +10
		if(cMaxDur >= 150)
			cMaxDur = 150;

		SetMaxNumOrDurability(cMaxDur);
		SetNumOrDurability(cCurDur);

		cValue_Out = GetMaxNumOrDurability();

		// �׷��̵�/���� ����
		CalculateItemGrade();
		CalculateItemPrice();

		// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
		RevisionLimit();

		if(0 == GetSeasonRecord())
			SetNewEquip();

		return S_SUCCESS;
	}

	// edith ������ ���׷��̵�
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

	// edith 2010.01.23 ������ ���� ���н� ������ ���� �ٿ�׷��̵� Ȯ�� ����.
	// ���翬 ���� ������
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
		//	SUCCESS_SILVIN	SUCCESS_MITH	    SUCCESS_ITER)		SUCCESS_SOUL	    SUCCESS_ANTON	    SUCCESS_ANCIENT��)	FAIL_LEVEL_KEEP	FAIL_LEVEL_DOWN	FAIL_DUR_KEEP	MINERAL_NUM
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

	// ���׷��̵带 �Ѱ���� �ߴ��� Ȯ��
	if (EquipmentInfo::MAX_UPGRADE_LEVEL <= m_cUpgradeLevel)
	{
		return E_UPGRADE_LEVEL_IS_FULL;
	}

	// S�׷��̵��̸� ���׷��̵� �Ұ����ϰ� �Ѵ�.
	/*if(m_GradeInfo.m_eItemGrade == Item::EquipType::S_GRADE)
	{
		return E_UPGRADE_LEVEL_IS_FULL;
	}*///no S limit hz

    // ���� ����Ѱ�?
	unsigned long dwNeedGold = GetUpgradePrice();
    if (dwCurrentGold_In < dwNeedGold)
    {
        return E_NOT_ENOUGH_MONEY;
    }

	// �������� ����Ѱ�.
	unsigned char cMaxDur = GetMaxNumOrDurability();
	if(cMaxDur <= 10)
	{
		return E_NOT_UPGRADE_MINENDURANCE;
	}


    // ������ ���� ���� Ȯ���� ����
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
    
    // ������ ����Ѱ�?
    int nDiffMineralNum = static_cast<int>(Mineral_InOut.GetNumOrDurability()) - 
        static_cast<int>(s_UpgradeTable[m_cUpgradeLevel][MINERAL_NUM]);

    if (nDiffMineralNum < 0)
    {
        return E_NOT_ENOUGH_MINERAL;
    }

	// ����� ���� ���. 
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
		// ANTONIUM ���� (100% Ȯ���̱� ������)
		if(Mineral_InOut.GetPrototypeID()!=EtcItemID::ANTONIUM)
		{
			m_cUpgradeCnt = 0;
		}
	}

    // ���� ����, �� ����
    Mineral_InOut.SetNumOrDurability(nDiffMineralNum);
	dwUsedGold_Out = dwNeedGold;

	// ���� ������ ����
	wSuccessPercent = (unsigned short)((float)wSuccessPercent*(CServerSetup::GetInstance().GetRefineDefault() / 100.0f));
/*
#ifdef _DEBUG
	wSuccessPercent = 100;
#endif
*/
	// ���� ����
	if (wSuccessPercent > Math::Random::ComplexRandom(100))
	{
		++m_cUpgradeLevel;

		// �ɼ� ��ȭ
		if (m_cUpgradeLevel <= MAX_LEVEL_OPTION_UPGRADE)
		{                
			ApplyUpgradeAttribute(REMOVE); 
			InitializeUpgradeAttribute();  
			ApplyUpgradeAttribute(APPLY);  
		}
		// ���� ����
		else
		{        
			++m_cMaxSocket;
		}

	}
	// ���� ����
	else
	{
		unsigned char cLevelDownNum = 0;

		unsigned char cPersent = static_cast<unsigned char>(Math::Random::ComplexRandom(100));
		if (cPersent < s_UpgradeTable[m_cUpgradeLevel][PERSENT_FAIL_LEVEL_KEEP])
		{
			// ���� �ٿ� ����
			cLevelDownNum = 0;
		}
		else
		{
			cPersent -= s_UpgradeTable[m_cUpgradeLevel][PERSENT_FAIL_LEVEL_KEEP];
			if (cPersent < s_UpgradeTable[m_cUpgradeLevel][PERSENT_FAIL_LEVEL1_DOWN])
			{
				// 1���� �ٿ�
				//cLevelDownNum = 1;// no more level down hz.
				cLevelDownNum = 0;
			}
			else
			{
				// 2���� �ٿ�
				cLevelDownNum = 0;
				//cLevelDownNum = 2;//no more level down hz.
			}
		}

		// // s�׷��̵� ���� ���н� �����ٿ� ����
		// �Ӽ� �ּҰ����ش��ϸ� �����ٿ��� �õ����� �ʴ´�.
		for (unsigned char cIndex = 0; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
		{
			if (true == CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
			{
				cLevelDownNum = 0;
				break;
			}
		}

		unsigned char cPersent2 = static_cast<unsigned char>(Math::Random::ComplexRandom(100));
		// ���� 30�̶����� ����ִ�. �� 30���η� �������� �����Ѵ�.
		if (cPersent2 < s_UpgradeTable[m_cUpgradeLevel][PERSENT_FAIL_DUR_DOWN])
		{
			// edith 2008.07.03 ���� ���н� �ִ� ������ ����
			unsigned char cMaxDur = GetMaxNumOrDurability();
			unsigned char cDownDur = 0;

			// ������ 10�� ����.
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
			// ������ ���� ����.
			cValue_Out = 0;
		}

		for (unsigned char cIndex = 0; cIndex < cLevelDownNum; ++cIndex)
		{
			if (0 < m_cUpgradeLevel)
			{
				// �ɼ� �ٿ�
				if (m_cUpgradeLevel <= MAX_LEVEL_OPTION_UPGRADE)
				{                
					--m_cUpgradeLevel;

					ApplyUpgradeAttribute(REMOVE);  
					InitializeUpgradeAttribute();   
					ApplyUpgradeAttribute(APPLY);   
				}
				// ���� ����
				else
				{    
					// ���Ͽ� ��� ������ �����ִ� ��� ���ϰ� �Բ� ���� ȿ���� �����ش�.
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

	// edith 2009.08.31 ���ý� ������ �̻������� Ȯ���ؼ� ó���Ѵ�.
	if (m_cUpgradeLevel <= MAX_LEVEL_OPTION_UPGRADE)
	{
		// ���׷��̵� ������ 5 �����϶� ���� ������ 2�� �̻��� �ɼ� ����.
		if(m_cMaxSocket >= 3)
		{
			// �̰� �̻��� �������̴�. ������ 0���� ������.
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


	// �׷��̵�/���� ����
	CalculateItemGrade();
	CalculateItemPrice();

	// 3�� �뷱�� ��ġ (S�׷��̵� ������ ���Ѽ� ����)
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
		ERRLOG1(g_Log, "CID:0x%08x ���� ĳ���Ͱ� �������� ����Ϸ� �մϴ�.", lpSender->GetCID()); 
		return false; 
	}

    char* szErrorString = NULL;

	switch (m_ItemInfo.m_DetailData.m_cItemType) 
	{
		case CASH_ITEM:
		{
			if (false == lpSender->UseCashItem(lpSender->GetCID(), lpRecver->GetCID(), itemPos, (unsigned short)m_ItemInfo.m_UseItemInfo.m_dwAmount, wError))
			{
				szErrorString = "ĳ��������";
				wError = PktUI::USE_FAILED;
			}
			break;
		}

		default:
			szErrorString = "������";
			break;
	}

	if (NULL == szErrorString)
	{
		// ĳ���������� ����� ������ �������� ���δ�.
		if (m_ItemData.m_cNumOrDurability <= 0)
		{
			ERRLOG5(g_Log, "CID:0x%08x %s��(��) ����ϴµ� �����߽��ϴ�.(������:0) "
				"ItemProtoTypeID : %d, SkillID : 0x%04x, LockCount : %d", 
				lpSender->GetCID(), szErrorString, m_ItemData.m_usProtoTypeID, 
				m_ItemInfo.m_UseItemInfo.m_usSkill_ID, m_ItemInfo.m_UseItemInfo.m_usSkill_LockCount); 
			return false;
		}
	}
	else
	{
		ERRLOG5(g_Log, "CID:0x%08x %s��(��) ����ϴµ� �����߽��ϴ�. "
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
		ERRLOG1(g_Log, "CID:0x%08x ���� ĳ���Ͱ� �������� ����Ϸ� �մϴ�.", lpSender->GetCID()); 
		return false; 
	}

    char* szErrorString = NULL;

	switch (m_ItemInfo.m_DetailData.m_cItemType) 
	{
		case AMMO:   	    
		{
			if (false == lpSender->UseAmmo(this)) 
			{ 
				szErrorString = "źȯ"; 
				wError = PktUI::USE_FAILED;
			} 
			break;
		}

		case POTION:		
		{
			if (false == UsePotion(lpSender, lpRecver)) 
			{ 
				szErrorString = "���� ������"; 
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
				szErrorString = "�����̳� ����, �Ǵ� ��ų ������"; 
				wError = PktUI::USE_FAILED;
			} 
			break;
		}

		case EXP_BOOK:
		{
			if (false == lpSender->IncrementExp(m_ItemInfo.m_UseItemInfo.m_dwAmount))
			{
				szErrorString = "����ġ ����";
				wError = PktUI::USE_FAILED;
			} 
			break;
		}

		case CASH_BOOK:
		{
			unsigned long dwSrcGold = lpSender->GetGold();
			if (false == lpSender->AddGold(m_ItemInfo.m_UseItemInfo.m_dwAmount, true))
			{
				szErrorString = "ĳ����";
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
				szErrorString = "����";
				wError = PktUI::USE_FAILED;
			} 
			break;
		}

		case PORTAL:
		{
			if (false == CCharRespawnMgr::GetInstance().SendRespawnInfo(lpSender->GetCID()))
			{
				szErrorString = "��Ż(��ġ ������)";
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
					szErrorString = "��ų��"; 
					wError = PktUI::USE_FAILED;
				} 
			}
			else if(0x1000 <= SkillID && SkillID < 0x2000)
			{
				if (false == lpSender->AbilityCreate(this)) 
				{ 
					szErrorString = "�����Ƽ��"; 
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
				// ����� �ͼ�, ������ �ͼ� üũ
				if (CGameTimeMgr::GetInstance().IsGuildWarTime() &&
					(lpSender->GetGuildWarFlag() == Creature::WAR_ON || lpSender->GetGuildWarFlag() == Creature::WAR_INSTANCE))
				{
					szErrorString = "��������� ��Ż(��ġ ������)";
					wError = PktUI::USE_FAILED;
					break;
				}

				// ������ : ��ũ ī���� ���� ����
				if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3) //hayzohar statue war fix was(true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))
				{
					if ((CGameTimeMgr::GetInstance().IsRealmWarReadyTime() || CGameTimeMgr::GetInstance().IsRealmWarTime()) &&
						(lpSender->GetRealmWarFlag() == Creature::WAR_ON || lpSender->GetRealmWarFlag() == Creature::WAR_INSTANCE))
					{
						szErrorString = "���������� ��Ż(��ġ ������)";
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
				szErrorString = "��Ż(��ġ ������)";
				wError = PktUI::USE_FAILED;
			} 
			break;
		}

		case CAMP_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::CAMP, wError))
			{
				szErrorString = "��� ��� ��ŸƮŶ";
			}
			break;
		}

		case WORLDWEAPON_K_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::KARTERANT_WEAPON, wError))
			{
				szErrorString = "�ܶ�ƾ ���� ���� ��Ÿ��Ŷ";
			}
			break;
		}

		case WORLDWEAPON_M_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::MERKADIA_WEAPON, wError))
			{
				szErrorString = "���� ����� ��Ÿ��Ŷ";
			}
			break;
		}

		case MINERAL_KIT:
			break;

		case SHORT_RANGE_ARMS_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::SHORT_RANGE_SIEGE_ARMS, wError))
			{
				szErrorString = "�ٰŸ� �������� ��ŸƮŶ";
			}
			break;
		}

		case LONG_RANGE_ARMS_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::LONG_RANGE_SIEGE_ARMS, wError))
			{
				szErrorString = "���Ÿ� �������� ��ŸƮŶ";
			}
			break;
		}

		case AIRSHIP_KIT:
		{
			if (false == lpSender->UseStartKit(Siege::AIRSHIP, wError))
			{
				szErrorString = "����� ��ŸƮŶ";
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
							szErrorString = "����Ʈ ������";
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
						szErrorString = "����Ʈ ������";
					}
				}
			}
			else
			{
				wError = PktUI::NOT_EXIST_QUEST;
				szErrorString = "����Ʈ ������";
			}
			break;
		}

		default:
			szErrorString = "������";
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
		ERRLOG5(g_Log, "CID:0x%08x %s��(��) ����ϴµ� �����߽��ϴ�. "
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
		// ������ ����� m_wType �� �ش��ϴ� ��ų���� �ش� ��ų�� ������ �����ϸ� 
		// ���ǻ���� �����Ѵ�.
		CSpell* pSpell = lpSender->GetSpellMgr().GetAffectedInfo().GetSpell(usSpellID);
		if (NULL != pSpell)	// �� ������ �������߸� ��밡���ϴ�.
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
		// ��Ÿ�ӿ� ���õ� ������ �ɾ��ش�.
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



