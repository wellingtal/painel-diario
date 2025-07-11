#include "stdafx.h"
#include "Cell.h"
#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include <Utility/Compress/MiniLZO/miniLZOWrapper.h>

#include <Creature/Creature.h>
#include <Creature/AggresiveCreature.h>
#include <Creature/Character/Character.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/VirtualMonsterMgr.h>
#include <Creature/Siege/SiegeObject.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharBroadCastPacket.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>

#include <Network/Packet/PacketStruct/CastlePacket.h>

__declspec(thread) static char g_BroadcastBuffer[PktMaxLen];

void CCell::ReleaseAllBuffer(CBuffer*& lpBuffer)
{
	// ������ �����ߴ� buffer�� ���� Release�Ѵ�.
	CBuffer* lpPos = lpBuffer;
	CBuffer* lpDel = lpBuffer;
    
    while (0 != lpPos)
	{
		lpDel = lpPos;
		lpPos = lpPos->next();

		SAFE_RELEASE_BUFFER(lpDel);
	}

    lpBuffer = 0;
}


void CCell::PrepareBroadCast()
{	
	// ������ �����ߴ� buffer�� ���� Release�Ѵ�.
    ReleaseAllBuffer(m_lpBroadcast2ndBuffer);

	if (m_lstCharacter.empty() && m_lstMonster.empty())
	{
		// �� ������ ���͵�, ĳ���͵� ����. ó������ �ʴ´�.
		return;
	}

	// ĳ���� �����͸� �����Ѵ�.
    const int MAX_BROADCAST_BUFFER_SIZE = 14 * 1024;
    CBufferFactory& bufferFactory = CCellManager::GetInstance().GetBufferFactory();

    // ĳ���� �����͸� �����Ѵ�.
    CharacterList::iterator char_pos = m_lstCharacter.begin();
    CharacterList::iterator char_end = m_lstCharacter.end();

    CBuffer* lpBroadcastBuffer = CREATE_BUFFER(bufferFactory, MAX_BROADCAST_BUFFER_SIZE);

    for (; char_pos != char_end && 0 != lpBroadcastBuffer; )
    {
        CCharacter* lpCharacter = *char_pos;

        //if (0 != lpCharacter && !lpCharacter->IsRideArms() && 
		
		// �켱 ���⿡ Ÿ�� �־ ������.
		if (0 != lpCharacter && 
            lpCharacter->IsOperationFlagSet(CCharacter::CHAR_INFO_LOADED))
        {
            Broadcast2nd::CSerializeCharacterData& charData = 
                lpCharacter->GetSerializeData();
            
            if (lpBroadcastBuffer->push(
                charData.GetDeltaBroadcastData(),
                charData.GetDeltaBroadcastDataLen()))
            {   
                // ������ ���� ����
                ++char_pos;
            }
            else
            {
				// ���۰� �� �� ���, ���۸� ��� �ְ�, ��� �����Ѵ�.
                lpBroadcastBuffer->next(m_lpBroadcast2ndBuffer);
                m_lpBroadcast2ndBuffer = lpBroadcastBuffer;

                lpBroadcastBuffer = CREATE_BUFFER(bufferFactory, MAX_BROADCAST_BUFFER_SIZE);
            }
        }
		else
		{
            // ĳ���Ͱ� ���� ����� �ƴ� ����̴�.
			++char_pos;
		}
    }

    // ���� �����͸� �����Ѵ�.
    MonsterList::iterator mon_pos = m_lstMonster.begin();
    MonsterList::iterator mon_end = m_lstMonster.end();

    for (; mon_pos != mon_end && 0 != lpBroadcastBuffer; )
    {
        CMonster* lpMonster = *mon_pos;

        if (0 != lpMonster)
        {
            Broadcast2nd::CSerializeMonsterData& monsterData = lpMonster->GetSerializeData();

            if (lpBroadcastBuffer->push(
                monsterData.GetDeltaBroadcastData(),
                monsterData.GetDeltaBroadcastDataLen()))
            {   
                // ������ ���� ����
                ++mon_pos;
            }
            else
            {
				// ���۰� �� �� ���, ���۸� ��� �ְ�, ��� �����Ѵ�.
                lpBroadcastBuffer->next(m_lpBroadcast2ndBuffer);
                m_lpBroadcast2ndBuffer = lpBroadcastBuffer;

                lpBroadcastBuffer = CREATE_BUFFER(bufferFactory, MAX_BROADCAST_BUFFER_SIZE);
            }
        }
        else
        {
            ++mon_pos;
        }
    }

    if (0 != lpBroadcastBuffer)
    {
        if(0 < lpBroadcastBuffer->length())
        {
            lpBroadcastBuffer->next(m_lpBroadcast2ndBuffer);
            m_lpBroadcast2ndBuffer = lpBroadcastBuffer;

            lpBroadcastBuffer = 0;
        }
        else
        {
            SAFE_RELEASE_BUFFER(lpBroadcastBuffer);
        }
    }
}


void SendBroadcastPacket(CCell& sendCell,
                         char* szBroadcastBuffer, 
                         unsigned short usPacketLength,
                         unsigned char cBroadcastType, 
                         unsigned char cDataType,
                         unsigned long dwCurrentPulse)
{
    // ��Ŷ ��� �ۼ�
    Broadcast2nd::PktBroadcast* lpBroadcast = 
        reinterpret_cast<Broadcast2nd::PktBroadcast*>(szBroadcastBuffer);

    lpBroadcast->m_dwCurrentPulse = dwCurrentPulse;
    lpBroadcast->m_cBroadcastType = cBroadcastType;
    lpBroadcast->m_cDataType      = cDataType;
    
    sendCell.SendAllCharacter(lpBroadcast, usPacketLength, CmdCellBroadCast2nd);    
}


void CCell::BroadCast(unsigned long dwCurrentPulse)
{	
    if (!m_lstCharacter.empty())
    {
        // �� ���� ���� ĳ���Ͱ� �ִ� ��쿡�� ����

        // ���� ���� ĳ���Ϳ���, �ֺ� ���� ĳ���� ������ ���� ������.
        // ���� �����͸� ������ ���� ��Ƽ� ������ ���� �ѹ��� ���� ������.
	    CCell** lppCellPos = m_lpConnectCell;
	    CCell** lppCellEnd = m_lpConnectCell + CONNECT_NUM;

        char* szBufferPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
        char* szBufferEnd = g_BroadcastBuffer + sizeof(g_BroadcastBuffer);

        CCell*   lpCell   = 0;
        CBuffer* lpBuffer = 0;

        unsigned long  dwCompressed = PktMaxLen;
        unsigned short usPacketLength = 0;

	    for (; lppCellPos != lppCellEnd; ++lppCellPos)
	    {
		    lpCell = *lppCellPos;

		    if (0 != lpCell)
		    {
			    lpBuffer = lpCell->m_lpBroadcast2ndBuffer;

                while(0 != lpBuffer)
			    {
                    size_t nBufferLen = lpBuffer->length();

                    if (szBufferPos + nBufferLen < szBufferEnd)
                    {
                        // ���۰� ���� ��� �ִ�.
                        memcpy(szBufferPos, lpBuffer->rd_ptr(), nBufferLen);
                        szBufferPos += nBufferLen;

                        lpBuffer = lpBuffer->next();
                    }
                    else
                    {
                        SendBroadcastPacket(*this, g_BroadcastBuffer, 
                            static_cast<unsigned short>(szBufferPos - g_BroadcastBuffer),
                            Broadcast2nd::PktBroadcast::BROADCAST,
                            Broadcast2nd::PktBroadcast::CHAR_DATA, dwCurrentPulse);

                        // ���۰� ���� á��. �ϴ� �����ϰ�, ���� ��ġ�� ó������ ������.
                        szBufferPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
                    }
			    }
		    }
	    }

		// �������� ������. (���� ������ ���̰� ������ ������ �Ѵ�)
        SendBroadcastPacket(*this, g_BroadcastBuffer, 
            static_cast<unsigned short>(szBufferPos - g_BroadcastBuffer),
            Broadcast2nd::PktBroadcast::BROADCAST_END, 
            Broadcast2nd::PktBroadcast::CHAR_DATA, dwCurrentPulse);

    }
}


void SendCharInfoToOthers(CCharacter& character, 
                          CCell** lppSendCell, const int nMaxSendCell)
{
	// �켱 ���⿡ Ÿ�� �־ ������.
	//if (!character.IsRideArms()) 
    {
        const unsigned short MAX_PKT_DATA = sizeof(Broadcast2nd::PktBroadcast) + 
            Broadcast2nd::CSerializeCharacterData::MAX_CHARACTER_DATA * 2;                

        char szPacketBuffer[MAX_PKT_DATA];
        
        Broadcast2nd::PktBroadcast* lpBroadcast = 
            reinterpret_cast<Broadcast2nd::PktBroadcast*>(szPacketBuffer);

        Broadcast2nd::CSerializeCharacterData& characterData = character.GetSerializeData();
        unsigned short usCharacterDataLen = characterData.GetBroadcastDataLen();

        // ��ǥ�� �ٲ���� ������, �����͸� �����ؼ� ������ (Delta�� �������� �ʴ´�)
        characterData.PrepareBroadcastData(character);

        lpBroadcast->m_dwCurrentPulse   = 0;
        lpBroadcast->m_cBroadcastType   = Broadcast2nd::PktBroadcast::LOGIN;
        lpBroadcast->m_cDataType        = Broadcast2nd::PktBroadcast::CHAR_DATA;

        memcpy(lpBroadcast + 1, characterData.GetBroadcastData(), usCharacterDataLen);

        CCell** lppSendPos = lppSendCell;
        CCell** lppSendEnd = lppSendCell + nMaxSendCell;

        for (; lppSendPos != lppSendEnd; ++lppSendPos)
        {
            CCell* lpSendPos = *lppSendPos;
            if (0 != lpSendPos)
            {   
                lpSendPos->SendAllCharacter(lpBroadcast, 
                    sizeof(Broadcast2nd::PktBroadcast) + usCharacterDataLen, 
                    CmdCellBroadCast2nd);
            }
        }
    }
}

//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� �� �α���
//
// Parameter :
//	1st : ũ���� �ּ�
//	2st : ���������� �α׾ƿ��� �� (�α���, ����, �����̵����� ���� 0)
//
// Do :
//	�������� ĳ���Ͱ� �α��������� �ֺ� ���� �˷� �ش�.
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
void CCell::SendCellLogin(CAggresiveCreature* lpAggresiveCreature, CCell* lpLastLogoutCell)
{
    if (0 != lpAggresiveCreature)
    {
        unsigned long dwCID = lpAggresiveCreature->GetCID();

        // ���� ����, ���������� �α׾ƿ��� ������ �������� ���Ѵ�.
        // lpSendCell���� ���� �� ĳ���� �����͸� ������,
        // ������ ĳ���� �����͸� ������ �� �༮���� ��� �ִ�.
        CCell* lpSendCell[CONNECT_NUM];

        CCell** lppSendPos  = 0;
        CCell** lppSendEnd  = 0;
        CCell*  lpSendPos   = 0;

        if (0 != lpLastLogoutCell)
        {
            std::fill_n(lpSendCell, int(CONNECT_NUM), reinterpret_cast<CCell*>(0));

            std::set_difference(
                m_lpSortedConnectedCell, 
                m_lpSortedConnectedCell + CONNECT_NUM,
                lpLastLogoutCell->m_lpSortedConnectedCell, 
                lpLastLogoutCell->m_lpSortedConnectedCell + CONNECT_NUM, 
                lpSendCell);
        }
        else
        {
            std::copy(m_lpSortedConnectedCell, m_lpSortedConnectedCell + CONNECT_NUM, lpSendCell);
        }

        switch(Creature::GetCreatureType(dwCID))
        {
        case Creature::CT_PC:
            {
                CCharacter* lpCharacter = static_cast<CCharacter*>(lpAggresiveCreature);

                // �� ������ �ٸ� ĳ���͵鿡�� ������ ��ƾ�̴�.                
                SendCharInfoToOthers(*lpCharacter, lpSendCell, CONNECT_NUM);
                
                CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
                if (0 != lpDispatch)
                {
                    // �ֺ��� ĳ����/���� ������ ������ ������.
                    char* szDataPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
                    char* szDataEnd = g_BroadcastBuffer + PktMaxLen - 1024;

                    // ��� ���
                    Broadcast2nd::PktBroadcast* lpBroadcast = 
                        reinterpret_cast<Broadcast2nd::PktBroadcast*>(g_BroadcastBuffer);

                    lpBroadcast->m_dwCurrentPulse = 0;
                    lpBroadcast->m_cBroadcastType = Broadcast2nd::PktBroadcast::LOGIN;
                    lpBroadcast->m_cDataType = Broadcast2nd::PktBroadcast::CHAR_DATA;

                    // ������ ����.
                    lppSendPos = lpSendCell;
                    lppSendEnd = lpSendCell + CONNECT_NUM;

                    for (; lppSendPos != lppSendEnd; ++lppSendPos)
                    {
                        lpSendPos = *lppSendPos;
                        if (0 != lpSendPos)
                        {                            
                            // �����͸� ��Ƽ� ������.
                            CCell::CharacterList::iterator char_pos = lpSendPos->m_lstCharacter.begin();
                            CCell::CharacterList::iterator char_end = lpSendPos->m_lstCharacter.end();

                            for (; char_pos != char_end; )
                            {
                                CCharacter* lpOtherCharacter = *char_pos;
                                
                                Broadcast2nd::CSerializeCharacterData& characterData = 
                                    lpOtherCharacter->GetSerializeData();

                                unsigned short usCharacterDataLen = 
                                    characterData.GetBroadcastDataLen();

								// ���⿡ Ÿ�� �־ ������ ������ ������ ����(By Minbobo)
                                /*if (lpOtherCharacter->IsRideArms())
								{
                                    // ���⿡ Ÿ�� ������ ������ ����.
									++char_pos;
								}*/

                                if (szDataPos + usCharacterDataLen < szDataEnd)
                                {
                                    // ���۰� ���� ���� ������, ���۸�. �ƴϸ� ����.                                   

                                    memcpy(szDataPos, characterData.GetBroadcastData(), usCharacterDataLen);
                                    szDataPos += usCharacterDataLen;

                                    ++char_pos;
                                }
                                else
                                {
                                    // ��Ŷ�� ������.
                                    lpDispatch->GetSendStream().WrapCompress(g_BroadcastBuffer,
                                        static_cast<unsigned short>(szDataPos - g_BroadcastBuffer), 
                                        CmdCellBroadCast2nd, 0, 0);

                                    // ���� ��ġ �ʱ�ȭ
                                    szDataPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
                                }
                            }

                            // �����͸� ��Ƽ� ������.
                            CCell::MonsterList::iterator mon_pos = lpSendPos->m_lstMonster.begin();
                            CCell::MonsterList::iterator mon_end = lpSendPos->m_lstMonster.end();

                            for (; mon_pos != mon_end; )
                            {
                                CMonster* lpMonster = *mon_pos;
                                                                
                                Broadcast2nd::CSerializeMonsterData& monsterData = 
                                    lpMonster->GetSerializeData();

                                unsigned short usMonsterData = monsterData.GetBroadcastDataLen();

                                if (szDataPos + usMonsterData < szDataEnd)
                                {
                                    // ���۰� ���� ���� ������, ���۸�. �ƴϸ� ����.
                                    memcpy(szDataPos, monsterData.GetBroadcastData(), usMonsterData);
                                    szDataPos += usMonsterData;

                                    ++mon_pos;
                                }
                                else
                                {
                                    // ��Ŷ�� ������.
                                    lpDispatch->GetSendStream().WrapCompress(g_BroadcastBuffer,
                                        static_cast<unsigned short>(szDataPos - g_BroadcastBuffer), 
                                        CmdCellBroadCast2nd, 0, 0);

                                    // ���� ��ġ �ʱ�ȭ
                                    szDataPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);                                    
                                }                                
                            }
                        }
                    }

                    // �����Ͱ� ������
                    if (g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast) < szDataPos)
                    {
                        // ��Ŷ�� ������.
                        lpDispatch->GetSendStream().WrapCompress(g_BroadcastBuffer,
                            static_cast<unsigned short>(szDataPos - g_BroadcastBuffer), 
                            CmdCellBroadCast2nd, 0, 0);
                    }

                    // �ֺ� ���� ������ �����͸� ��Ƽ� �� ĳ���Ϳ� ������.
                    // �̶�, ���� ���̸� �ʰ����� �ʵ��� �Ѵ�. ���� �ִ�ġ�� 14k������ �Ѵ�.
                    szDataPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
                    lpBroadcast->m_dwCurrentPulse = 0;
                    lpBroadcast->m_cBroadcastType = Broadcast2nd::PktBroadcast::LOGIN;
                    lpBroadcast->m_cDataType = Broadcast2nd::PktBroadcast::ITEM_DATA;

                    // ������ ����.
                    lppSendPos = lpSendCell;
                    lppSendEnd = lpSendCell + CONNECT_NUM;

                    for (; lppSendPos != lppSendEnd; ++lppSendPos)
                    {
                        lpSendPos = *lppSendPos;
                        if (0 != lpSendPos)
                        {                            
                            // �����͸� ��Ƽ� ������.
                            CCell::ItemList::iterator item_pos = lpSendPos->m_lstItem.begin();
                            CCell::ItemList::iterator item_end = lpSendPos->m_lstItem.end();

                            for (; item_pos != item_end; )
                            {
                                ItemInfo& itemInfo = (*item_pos);

                                // ���۰� ���� ���� ������ ���۸�, �ƴϸ� ����
                                if (szDataPos + sizeof(FieldObject) < szDataEnd)
                                {   
                                    itemInfo.MakeFieldObject(*reinterpret_cast<FieldObject*>(szDataPos));

                                    szDataPos += sizeof(FieldObject);
                                    ++item_pos;
                                }
                                else
                                {
                                    // ��Ŷ�� ������.
                                    lpDispatch->GetSendStream().WrapCompress(g_BroadcastBuffer,
                                        static_cast<unsigned short>(szDataPos - g_BroadcastBuffer), 
                                        CmdCellBroadCast2nd, 0, 0);

                                    // ���� ��ġ �ʱ�ȭ
                                    szDataPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
                                }
                            }
                        }
                    }

                    // �����Ͱ� ������
                    if (szDataPos != g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast))
                    {
                        // ��Ŷ�� ������.
                        lpDispatch->GetSendStream().WrapCompress(g_BroadcastBuffer,
                            static_cast<unsigned short>(szDataPos - g_BroadcastBuffer), 
                            CmdCellBroadCast2nd, 0, 0);
                    }
                }
            }

            break;

        case Creature::CT_MONSTER:		
        case Creature::CT_SUMMON:
        case Creature::CT_STRUCT:
            {
                CMonster* lpMonster = static_cast<CMonster*>(lpAggresiveCreature);

                // �ڽ��� ������ �ֺ� ĳ���͵鿡�� �Ѹ���.
                const unsigned short MAX_PKT_DATA = sizeof(Broadcast2nd::PktBroadcast) + 
                    Broadcast2nd::CSerializeMonsterData::MAX_MONSTER_DATA * 2;                

                char szPacketBuffer[MAX_PKT_DATA];
                
                Broadcast2nd::PktBroadcast* lpBroadcast = 
                    reinterpret_cast<Broadcast2nd::PktBroadcast*>(szPacketBuffer);

                Broadcast2nd::CSerializeMonsterData& monsterData = lpMonster->GetSerializeData();
                unsigned short usMonsterDataLen = monsterData.GetBroadcastDataLen();

                // ��ǥ�� �ٲ���� ������, �����͸� �����ؼ� ������ (Delta�� �������� �ʴ´�)
                monsterData.PrepareBroadcastData(*lpMonster);

                lpBroadcast->m_dwCurrentPulse   = 0;
                lpBroadcast->m_cBroadcastType   = Broadcast2nd::PktBroadcast::LOGIN;
                lpBroadcast->m_cDataType        = Broadcast2nd::PktBroadcast::CHAR_DATA;

                memcpy(lpBroadcast + 1, monsterData.GetBroadcastData(), usMonsterDataLen);

                lppSendPos = lpSendCell;
                lppSendEnd = lpSendCell + CONNECT_NUM;

                for (; lppSendPos != lppSendEnd; ++lppSendPos)
                {
                    CCell* lpSendPos = *lppSendPos;
                    if (0 != lpSendPos)
                    {   
                        lpSendPos->SendAllCharacter(lpBroadcast, 
                            sizeof(Broadcast2nd::PktBroadcast) + usMonsterDataLen, 
                            CmdCellBroadCast2nd);
                    }
                }
            }
            break;
        }
    }
}



//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� �� �α� �ƿ�
//
// Parameter :
//	1st : ũ���� �ּ�
//	2st : �α׾ƿ��� �� �α����Ϸ��� �� (�α׾ƿ� ������ ���� ��쿡�� 0�� �ִ´�.)
//
// Do :
//	�ֺ� ���� ���� �α׾ƿ��Ѵٰ� �˸���.
//  ������ �ʾƾ� �� ĳ���͵��� ���� ����� �Ѵ�.
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
void CCell::SendCellLogout(CAggresiveCreature* lpAggresiveCreature, CCell* lpPrepareLoginCell)
{


}

