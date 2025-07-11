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
	// 일전에 전송했던 buffer를 전부 Release한다.
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
	// 일전에 전송했던 buffer를 전부 Release한다.
    ReleaseAllBuffer(m_lpBroadcast2ndBuffer);

	if (m_lstCharacter.empty() && m_lstMonster.empty())
	{
		// 이 셀에는 몬스터도, 캐릭터도 없다. 처리하지 않는다.
		return;
	}

	// 캐릭터 데이터를 생성한다.
    const int MAX_BROADCAST_BUFFER_SIZE = 14 * 1024;
    CBufferFactory& bufferFactory = CCellManager::GetInstance().GetBufferFactory();

    // 캐릭터 데이터를 생성한다.
    CharacterList::iterator char_pos = m_lstCharacter.begin();
    CharacterList::iterator char_end = m_lstCharacter.end();

    CBuffer* lpBroadcastBuffer = CREATE_BUFFER(bufferFactory, MAX_BROADCAST_BUFFER_SIZE);

    for (; char_pos != char_end && 0 != lpBroadcastBuffer; )
    {
        CCharacter* lpCharacter = *char_pos;

        //if (0 != lpCharacter && !lpCharacter->IsRideArms() && 
		
		// 우선 병기에 타고 있어도 보낸다.
		if (0 != lpCharacter && 
            lpCharacter->IsOperationFlagSet(CCharacter::CHAR_INFO_LOADED))
        {
            Broadcast2nd::CSerializeCharacterData& charData = 
                lpCharacter->GetSerializeData();
            
            if (lpBroadcastBuffer->push(
                charData.GetDeltaBroadcastData(),
                charData.GetDeltaBroadcastDataLen()))
            {   
                // 데이터 저장 성공
                ++char_pos;
            }
            else
            {
				// 버퍼가 꽉 찬 경우, 버퍼를 비워 주고, 계속 진행한다.
                lpBroadcastBuffer->next(m_lpBroadcast2ndBuffer);
                m_lpBroadcast2ndBuffer = lpBroadcastBuffer;

                lpBroadcastBuffer = CREATE_BUFFER(bufferFactory, MAX_BROADCAST_BUFFER_SIZE);
            }
        }
		else
		{
            // 캐릭터가 복사 대상이 아닌 경우이다.
			++char_pos;
		}
    }

    // 몬스터 데이터를 생성한다.
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
                // 데이터 저장 성공
                ++mon_pos;
            }
            else
            {
				// 버퍼가 꽉 찬 경우, 버퍼를 비워 주고, 계속 진행한다.
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
    // 패킷 헤더 작성
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
        // 본 셀에 보낼 캐릭터가 있는 경우에만 전송

        // 현재 셀의 캐릭터에게, 주변 셀의 캐릭터 정보를 전부 보낸다.
        // 버퍼 데이터를 가능한 많이 모아서 압축한 다음 한번에 많이 보낸다.
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
                        // 버퍼가 아직 비어 있다.
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

                        // 버퍼가 가득 찼다. 일단 전송하고, 버퍼 위치를 처음으로 돌린다.
                        szBufferPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
                    }
			    }
		    }
	    }

		// 최종으로 보낸다. (보낼 데이터 길이가 없더라도 보내야 한다)
        SendBroadcastPacket(*this, g_BroadcastBuffer, 
            static_cast<unsigned short>(szBufferPos - g_BroadcastBuffer),
            Broadcast2nd::PktBroadcast::BROADCAST_END, 
            Broadcast2nd::PktBroadcast::CHAR_DATA, dwCurrentPulse);

    }
}


void SendCharInfoToOthers(CCharacter& character, 
                          CCell** lppSendCell, const int nMaxSendCell)
{
	// 우선 병기에 타고 있어도 보낸다.
	//if (!character.IsRideArms()) 
    {
        const unsigned short MAX_PKT_DATA = sizeof(Broadcast2nd::PktBroadcast) + 
            Broadcast2nd::CSerializeCharacterData::MAX_CHARACTER_DATA * 2;                

        char szPacketBuffer[MAX_PKT_DATA];
        
        Broadcast2nd::PktBroadcast* lpBroadcast = 
            reinterpret_cast<Broadcast2nd::PktBroadcast*>(szPacketBuffer);

        Broadcast2nd::CSerializeCharacterData& characterData = character.GetSerializeData();
        unsigned short usCharacterDataLen = characterData.GetBroadcastDataLen();

        // 좌표가 바뀌었기 때문에, 데이터를 갱신해서 보낸다 (Delta는 갱신하지 않는다)
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
// 캐릭터 셀 로그인
//
// Parameter :
//	1st : 크리쳐 주소
//	2st : 마지막으로 로그아웃한 셀 (로그인, 리젠, 순간이동등의 경우는 0)
//
// Do :
//	셀안으로 캐릭터가 로그인했음을 주변 셀에 알려 준다.
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
void CCell::SendCellLogin(CAggresiveCreature* lpAggresiveCreature, CCell* lpLastLogoutCell)
{
    if (0 != lpAggresiveCreature)
    {
        unsigned long dwCID = lpAggresiveCreature->GetCID();

        // 현재 셀과, 마지막으로 로그아웃한 셀과의 차집합을 구한다.
        // lpSendCell에는 내가 내 캐릭터 데이터를 보내고,
        // 나에게 캐릭터 데이터를 보내야 할 녀석들이 들어 있다.
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

                // 내 정보를 다른 캐릭터들에게 보내는 루틴이다.                
                SendCharInfoToOthers(*lpCharacter, lpSendCell, CONNECT_NUM);
                
                CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
                if (0 != lpDispatch)
                {
                    // 주변의 캐릭터/몬스터 정보를 나에게 보낸다.
                    char* szDataPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
                    char* szDataEnd = g_BroadcastBuffer + PktMaxLen - 1024;

                    // 헤더 기록
                    Broadcast2nd::PktBroadcast* lpBroadcast = 
                        reinterpret_cast<Broadcast2nd::PktBroadcast*>(g_BroadcastBuffer);

                    lpBroadcast->m_dwCurrentPulse = 0;
                    lpBroadcast->m_cBroadcastType = Broadcast2nd::PktBroadcast::LOGIN;
                    lpBroadcast->m_cDataType = Broadcast2nd::PktBroadcast::CHAR_DATA;

                    // 루프를 돈다.
                    lppSendPos = lpSendCell;
                    lppSendEnd = lpSendCell + CONNECT_NUM;

                    for (; lppSendPos != lppSendEnd; ++lppSendPos)
                    {
                        lpSendPos = *lppSendPos;
                        if (0 != lpSendPos)
                        {                            
                            // 데이터를 모아서 보낸다.
                            CCell::CharacterList::iterator char_pos = lpSendPos->m_lstCharacter.begin();
                            CCell::CharacterList::iterator char_end = lpSendPos->m_lstCharacter.end();

                            for (; char_pos != char_end; )
                            {
                                CCharacter* lpOtherCharacter = *char_pos;
                                
                                Broadcast2nd::CSerializeCharacterData& characterData = 
                                    lpOtherCharacter->GetSerializeData();

                                unsigned short usCharacterDataLen = 
                                    characterData.GetBroadcastDataLen();

								// 병기에 타고 있어서 보내지 않으면 문제가 생김(By Minbobo)
                                /*if (lpOtherCharacter->IsRideArms())
								{
                                    // 병기에 타고 있으면 보내지 않음.
									++char_pos;
								}*/

                                if (szDataPos + usCharacterDataLen < szDataEnd)
                                {
                                    // 버퍼가 아직 남아 있으면, 버퍼링. 아니면 전송.                                   

                                    memcpy(szDataPos, characterData.GetBroadcastData(), usCharacterDataLen);
                                    szDataPos += usCharacterDataLen;

                                    ++char_pos;
                                }
                                else
                                {
                                    // 패킷을 보낸다.
                                    lpDispatch->GetSendStream().WrapCompress(g_BroadcastBuffer,
                                        static_cast<unsigned short>(szDataPos - g_BroadcastBuffer), 
                                        CmdCellBroadCast2nd, 0, 0);

                                    // 버퍼 위치 초기화
                                    szDataPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
                                }
                            }

                            // 데이터를 모아서 보낸다.
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
                                    // 버퍼가 아직 남아 있으면, 버퍼링. 아니면 전송.
                                    memcpy(szDataPos, monsterData.GetBroadcastData(), usMonsterData);
                                    szDataPos += usMonsterData;

                                    ++mon_pos;
                                }
                                else
                                {
                                    // 패킷을 보낸다.
                                    lpDispatch->GetSendStream().WrapCompress(g_BroadcastBuffer,
                                        static_cast<unsigned short>(szDataPos - g_BroadcastBuffer), 
                                        CmdCellBroadCast2nd, 0, 0);

                                    // 버퍼 위치 초기화
                                    szDataPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);                                    
                                }                                
                            }
                        }
                    }

                    // 데이터가 있으면
                    if (g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast) < szDataPos)
                    {
                        // 패킷을 보낸다.
                        lpDispatch->GetSendStream().WrapCompress(g_BroadcastBuffer,
                            static_cast<unsigned short>(szDataPos - g_BroadcastBuffer), 
                            CmdCellBroadCast2nd, 0, 0);
                    }

                    // 주변 셀의 아이템 데이터를 모아서 내 캐릭터에 보낸다.
                    // 이때, 버퍼 길이를 초과하지 않도록 한다. 셀당 최대치는 14k정도로 한다.
                    szDataPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
                    lpBroadcast->m_dwCurrentPulse = 0;
                    lpBroadcast->m_cBroadcastType = Broadcast2nd::PktBroadcast::LOGIN;
                    lpBroadcast->m_cDataType = Broadcast2nd::PktBroadcast::ITEM_DATA;

                    // 루프를 돈다.
                    lppSendPos = lpSendCell;
                    lppSendEnd = lpSendCell + CONNECT_NUM;

                    for (; lppSendPos != lppSendEnd; ++lppSendPos)
                    {
                        lpSendPos = *lppSendPos;
                        if (0 != lpSendPos)
                        {                            
                            // 데이터를 모아서 보낸다.
                            CCell::ItemList::iterator item_pos = lpSendPos->m_lstItem.begin();
                            CCell::ItemList::iterator item_end = lpSendPos->m_lstItem.end();

                            for (; item_pos != item_end; )
                            {
                                ItemInfo& itemInfo = (*item_pos);

                                // 버퍼가 아직 남아 있으면 버퍼링, 아니면 전송
                                if (szDataPos + sizeof(FieldObject) < szDataEnd)
                                {   
                                    itemInfo.MakeFieldObject(*reinterpret_cast<FieldObject*>(szDataPos));

                                    szDataPos += sizeof(FieldObject);
                                    ++item_pos;
                                }
                                else
                                {
                                    // 패킷을 보낸다.
                                    lpDispatch->GetSendStream().WrapCompress(g_BroadcastBuffer,
                                        static_cast<unsigned short>(szDataPos - g_BroadcastBuffer), 
                                        CmdCellBroadCast2nd, 0, 0);

                                    // 버퍼 위치 초기화
                                    szDataPos = g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast);
                                }
                            }
                        }
                    }

                    // 데이터가 있으면
                    if (szDataPos != g_BroadcastBuffer + sizeof(Broadcast2nd::PktBroadcast))
                    {
                        // 패킷을 보낸다.
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

                // 자신의 정보를 주변 캐릭터들에게 뿌린다.
                const unsigned short MAX_PKT_DATA = sizeof(Broadcast2nd::PktBroadcast) + 
                    Broadcast2nd::CSerializeMonsterData::MAX_MONSTER_DATA * 2;                

                char szPacketBuffer[MAX_PKT_DATA];
                
                Broadcast2nd::PktBroadcast* lpBroadcast = 
                    reinterpret_cast<Broadcast2nd::PktBroadcast*>(szPacketBuffer);

                Broadcast2nd::CSerializeMonsterData& monsterData = lpMonster->GetSerializeData();
                unsigned short usMonsterDataLen = monsterData.GetBroadcastDataLen();

                // 좌표가 바뀌었기 때문에, 데이터를 갱신해서 보낸다 (Delta는 갱신하지 않는다)
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
// 캐릭터 셀 로그 아웃
//
// Parameter :
//	1st : 크리쳐 주소
//	2st : 로그아웃한 후 로그인하려는 셀 (로그아웃 등으로 없을 경우에는 0을 넣는다.)
//
// Do :
//	주변 셀에 내가 로그아웃한다고 알린다.
//  보이지 않아야 될 캐릭터들을 전부 지우게 한다.
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
void CCell::SendCellLogout(CAggresiveCreature* lpAggresiveCreature, CCell* lpPrepareLoginCell)
{


}

