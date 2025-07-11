
#include "stdafx.h"

#include <Item/Item.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Utility/Math/Math.h>
#include <Utility/Setup/ServerSetup.h>

#include "CharacterClass.h"
#include "CharacterCreate.h"


bool CharCreate::CheckCharCreateName(const char *Name_In, bool HanCheck_In)
{
	const unsigned short MIN_CHAR_NAME	= 4;
	const unsigned short MAX_CHAR_NAME	= 20;

	if(Name_In == NULL)
	{
		return false;
	}

	// ���� ����
	size_t Len = strlen(Name_In);
	if(Len < MIN_CHAR_NAME || Len > MAX_CHAR_NAME)
	{
		return false;
	}

	LPBYTE CheckName = (LPBYTE)Name_In;

	if(true == HanCheck_In)
	{
		// ���� ���� �˻�
		int ACount = 0;
		for(unsigned short LCount = 0; LCount < Len; LCount++)
		{
			if((CheckName[LCount] & 0x80) == 0x80)
			{
				// 2Byte ���� üũ
				if(CheckName[LCount + 1] == NULL)
				{
					return false;
				}
				
				// ��� ���� üũ (�ѱ�)
				if(CheckName[LCount] < 0xB0 || CheckName[LCount] > 0xC9)
				{
					return false;
				}
					
				if(CheckName[LCount + 1] < 0xA1 || CheckName[LCount + 1] > 0xFE)
				{
                    return false;
				}

				// �ѱ� �κ� �� ���
				for(ACount = 0; ACount < ALLOW_HAN_NUM; ACount++)
				{
					if(MAKEWORD(CheckName[LCount + 1], CheckName[LCount])  == AllowHans[ACount])
					{
						break;
					}
				}

				if(ACount != ALLOW_HAN_NUM)
				{
					return false;
				}

				LCount += 1;
				continue;
			}
			else
			{
				// ������ ���	
				if((CheckName[LCount] >= 'A' && CheckName[LCount] <= 'Z') ||
					(CheckName[LCount] >= 'a' && CheckName[LCount] <= 'z'))
				{
					continue;
				}

				// ���� ���
				if(CheckName[LCount] >= '0' && CheckName[LCount] <= '9')
				{
					continue;
				}

				// Ư�� ��ȣ �κ� ���
				for(ACount = 0; ACount < ALLOW_LETTER_NUM; ACount++)
				{
					if(CheckName[LCount] == AllowLetters[ACount])
					{
						break;
					}
				}

				if(ACount == ALLOW_LETTER_NUM)
				{
					return false;
				}
			}
		}
	}
	else
	{
		if(!strcmp(Name_In, ""))
		{
			return false;
		}

		/////////////////////////////////////////////////////////////
		// ����, ����, Ư����ȣ�� �Է°���.
		// ���� ���� �˻�
		int ACount = 0;
		for(unsigned short LCount = 0; LCount < Len; LCount++)
		{
			// ������ ���	
			if((CheckName[LCount] >= 'A' && CheckName[LCount] <= 'Z') ||
				(CheckName[LCount] >= 'a' && CheckName[LCount] <= 'z'))
			{
				continue;
			}

			// ���� ���
			if(CheckName[LCount] >= '0' && CheckName[LCount] <= '9')
			{
				continue;
			}

			// Ư�� ��ȣ �κ� ���
			for(ACount = 0; ACount < ALLOW_LETTER_NUM; ACount++)
			{
				if(CheckName[LCount] == AllowLetters[ACount])
				{
					break;
				}
			}

			if(ACount == ALLOW_LETTER_NUM)
			{
				return false;
			}
		}
		
		// Ư����ȣ ������
		for(int LCount = 0; LCount < DISALLOW_LETTER_NUM; LCount++)
		{
			if(_tcschr(Name_In, DisAllowLetters[LCount]) != NULL)
			{
				return false;
			}
		}
	}

	return true;
}


bool CharCreate::CheckCharCreateData(CHAR_CREATE &CharCreate_In)
{
	if (CharCreate_In.Race == CClass::HUMAN)
	{
		if(CharCreate_In.Equip[Item::EquipmentPos::SHIRT] < 201 ||
			CharCreate_In.Equip[Item::EquipmentPos::SHIRT] > 209)
		{
			return false;
		}

		if(601 != CharCreate_In.Equip[Item::EquipmentPos::BOOTS])
		{
			return false;
		}

		if(CClass::Fighter == CharCreate_In.Class)			
		{	// ����
			if(701 != CharCreate_In.Equip[Item::EquipmentPos::WEAPON_HAND1]) 
			{
				return false;
			}
		}
		else if(CClass::Rogue == CharCreate_In.Class)
		{	// �α�
			if(1601 != CharCreate_In.Equip[Item::EquipmentPos::WEAPON_HAND1])
			{
				return false;
			}
		}
		else if(CClass::Mage == CharCreate_In.Class)
		{	// ������
			if(1501 != CharCreate_In.Equip[Item::EquipmentPos::WEAPON_HAND1])
			{
				return false;
			}
		}
		else if(CClass::Acolyte == CharCreate_In.Class)
		{	// ���ݶ���Ʈ
			if(801 != CharCreate_In.Equip[Item::EquipmentPos::WEAPON_HAND1]) 
			{
				return false;
			}
		}

		if(CharCreate_In.STR < 20 || CharCreate_In.CON < 20 ||
			CharCreate_In.DEX < 20 || CharCreate_In.INT < 20 || CharCreate_In.WIS < 20) 
		{
			return false;
		}

		if(CharCreate_In.STR + CharCreate_In.CON + 
			CharCreate_In.DEX + CharCreate_In.INT + CharCreate_In.WIS > 105)
		{
			return false;
		}		
	}
	else if (CharCreate_In.Race == CClass::AKHAN)
	{
		if(CClass::Combatant == CharCreate_In.Class)			
		{	// ������Ʈ
			if(5401 != CharCreate_In.Equip[Item::EquipmentPos::WEAPON]) 
			{
				return false;
			}

			if(5101 != CharCreate_In.Equip[Item::EquipmentPos::BODY])
			{
				return false;
			}

			if(5201 != CharCreate_In.Equip[Item::EquipmentPos::PELVIS])
			{
				return false;
			}
		}
		else if(CClass::Officiator == CharCreate_In.Class)
		{	// ���Ǽ�����
			if(5801 != CharCreate_In.Equip[Item::EquipmentPos::WEAPON]) 
			{
				return false;
			}

			if(5110 != CharCreate_In.Equip[Item::EquipmentPos::BODY])
			{
				return false;
			}

			if(5210 != CharCreate_In.Equip[Item::EquipmentPos::PELVIS])
			{
				return false;
			}
		}
		
		if(CharCreate_In.STR < 20 || CharCreate_In.CON < 20 || 
			CharCreate_In.DEX < 20 || CharCreate_In.INT < 20 || CharCreate_In.WIS < 20) 
		{
			return false;
		}

		if(CharCreate_In.STR + CharCreate_In.CON + CharCreate_In.DEX + 
			CharCreate_In.INT + CharCreate_In.WIS > 105) 
		{
			return false;
		}		
	}
	else
	{
		return false;
	}

	return true;
}

unsigned long CharCreate::GetDefaultStartGold(void)
{
	return START_GOLD;
}

POS CharCreate::GetDefaultCharacterPos(unsigned long dwRace, unsigned long dwRacePlayerNum)                                              
{
	if (true == CServerSetup::GetInstance().IsBattleAuthServer() ||
		true == CServerSetup::GetInstance().IsBattleGameServer())
	{
		POS StartPos = BGServerStartPos[dwRace][Math::Random::ComplexRandom(MAX_LOBBY_RESPAWN_POS)];
		StartPos.fPointX += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
		StartPos.fPointZ += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
		return StartPos;
	}

	// �������� ���� ��ġ�� �ϳ��̴�.
	unsigned long dwIndex  = 0;

	if (CClass::MAX_RACE <= dwRace)
	{
		dwRace = 0;
	}

	for (; dwIndex < MAX_START_POS_NUM; ++dwIndex)
	{
		if (dwRacePlayerNum < StartPointVariation[dwIndex])
		{
			if (0 < dwIndex)
			{
				dwIndex = Math::Random::ComplexRandom(dwIndex + 1);
			}

			break;
		}
	}

    POS startPos = StartPosNum[dwRace][dwIndex];

    startPos.fPointX += static_cast<float>(Math::Random::ComplexRandom(14)) - 7.0f;
    startPos.fPointZ += static_cast<float>(Math::Random::ComplexRandom(14)) - 7.0f;

	return startPos;
}


inline bool operator == (const POS& lhs, const POS& rhs)
{ 
	return (lhs.fPointX == rhs.fPointX) && (lhs.fPointY == rhs.fPointY) && (lhs.fPointZ == rhs.fPointZ); 
}

inline bool operator != (const POS& lhs, const POS& rhs)
{ 
	return !(lhs == rhs); 
}
