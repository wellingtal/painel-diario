
#include "stdafx.h"
#include "ParseCharSkill.h"
#include "SendCharSkill.h"
#include "GameClientDispatch.h"

#include <Creature/Character/Character.h>
#include <Item/ItemStructure.h>

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>


// ��ų ���
bool GameClientParsePacket::ParseCharUseSkill(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	return true;
}


// ��ų �����
bool GameClientParsePacket::ParseCharSkillErase(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSk), GameClientDispatch);

	PktSk*      lpPktSk     = static_cast<PktSk*>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long   dwCharID	= lpPktSk->m_dwCharID;
	unsigned short  usSkill	    = lpPktSk->m_wSkill;
	unsigned char   cIndex	    = lpPktSk->m_cIndex;
	Item::ItemPos   ItemPos		= lpPktSk->m_ItemPos;         // ������ �� ��ġ

	if(0x1000 <= usSkill && usSkill < 0x2000)
	{
		if (false == lpCharacter->AbilityErase(cIndex, ItemPos)) 
		{
    		GameClientSendPacket::SendCharSkillCommand(GameClientDispatch.GetSendStream(), dwCharID,
				CmdCharSkillErase, cIndex, usSkill, PktSk::SERVER_ERROR);

			ERRLOG3(g_Log, "CID:0x%08x ��ų ����� ��Ŷ ó���� �����Ͽ����ϴ�. Skill: %d, Index: %d", dwCharID, usSkill, cIndex);		
		}
	}
	else
	{
		// edith 2008.02.14 ��ų �����κ� (����������� �־�� ����� �ְ� ����??)
		if (false == lpCharacter->SkillErase(cIndex, ItemPos)) 
		{
    		GameClientSendPacket::SendCharSkillCommand(GameClientDispatch.GetSendStream(), dwCharID,
				CmdCharSkillErase, cIndex, usSkill, PktSk::SERVER_ERROR);

			ERRLOG3(g_Log, "CID:0x%08x ��ų ����� ��Ŷ ó���� �����Ͽ����ϴ�. Skill: %d, Index: %d", dwCharID, usSkill, cIndex);		
		}
	}

	return true;
}