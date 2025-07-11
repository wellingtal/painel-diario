#include "stdafx.h"
#include "MonsterShout.h"
#include "MonsterMgr.h"
#include "ScriptEngine/ScriptEngine.h"

#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Map/FieldMap/Cell.h>
#include <Utility/Math/Math.h>

#include <Creature/CreatureManager.h>
#include <Creature/Monster/Monster.h>

const char* CMonsterShout::ms_DefaultFileName = "./Script/Game/MonsterChat.gsf";

CMonsterShout& CMonsterShout::GetInstance()
{
    static CMonsterShout monsterShout;
    return monsterShout;
}


CMonsterShout::CMonsterShout()
{
   
    
}

CMonsterShout::~CMonsterShout()
{
    ShoutMap::iterator pos = m_ShoutMap.begin();
    ShoutMap::iterator end = m_ShoutMap.end();

    for(; pos != end; ++pos)
    {
        ShoutInfo& shoutInfo = pos->second;

        ChatNode* lpNode = shoutInfo.m_lpNextNode;
        ChatNode* lpDeleteNode = lpNode;

        while(0 != lpNode)
        {
            lpDeleteNode = lpNode;
            lpNode = lpNode->m_lpNextNode;

            delete lpDeleteNode;
        }
    }
}


void SetShoutText(int nKID, int nBehavior, int nSkill_ID, 
                  int nChatType, int nPercentage, const char* szMessage)
{
    CMonsterShout::ShoutInfo shoutInfo;
    CMonsterShout::ChatNode  chatNode;
    memset(&shoutInfo, 0, sizeof(CMonsterShout::ShoutInfo));
    memset(&chatNode,  0, sizeof(CMonsterShout::ChatNode));

    shoutInfo.m_nKID        = nKID;
    shoutInfo.m_nBehavior   = nBehavior;
    shoutInfo.m_nSkill_ID   = nSkill_ID;
    shoutInfo.m_nChatType   = nChatType;
    chatNode.m_nPercentage = nPercentage;

    const CMonsterMgr::MonsterProtoType* lpMonsterProtoType = 
        CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);

    if(0 != szMessage && 0 != lpMonsterProtoType)
    {
        if(shoutInfo.m_nBehavior != CMonsterShout::RESPAWN)
        {
            _snprintf(chatNode.m_szMonsterChat, MAX_PATH - 1, "%s : %s", 
                lpMonsterProtoType->m_MonsterInfo.m_strName, szMessage);
        }
        else
        {
            _snprintf(chatNode.m_szMonsterChat, MAX_PATH - 1, "%s", szMessage);
        }

        chatNode.m_szMonsterChat[MAX_PATH - 1] = 0;

        chatNode.m_usChatLength = 
            static_cast<unsigned short>(strlen(chatNode.m_szMonsterChat)) + 1;

        CMonsterShout::GetInstance().AddMonsterShout(shoutInfo, chatNode);
    }
}


bool CMonsterShout::LoadScript(const char* szFileName)
{
	SCRIPT Script = _SE_Create(szFileName);
    if (NULL == Script)
    {
        return false; 
    }

    _SE_RegisterFunction(Script, SetShoutText, T_VOID, 
        "MonsterChat", T_INT, T_INT, T_INT, T_INT, T_INT, T_STRING, 0);

	_SE_Execute(Script);
	_SE_Destroy(Script);

    return true;
}

void CMonsterShout::Shout(unsigned long dwMonsterCID, unsigned long nKID, 
                          unsigned short usXPos, unsigned short usZPos,
                          Behavior eBehavior, const char* szName, unsigned short usSkill_ID)
{
    // KID로 검색한다.
    // 확률 및 요건을 검사한다.
    
    std::pair<ShoutMap::iterator, ShoutMap::iterator> result = m_ShoutMap.equal_range(nKID);
    std::vector<int> shoutedList;

    for(; result.first != result.second; ++result.first)
    {
        ShoutInfo& shoutInfo = result.first->second;
        bool bChatSend = false;

        if(shoutInfo.m_nBehavior == eBehavior)
        {
            if(SKILL_ATTACK == eBehavior || SKILL_ATTACKED == eBehavior)
            {
                // 스킬에 공격당한 경우는 스킬 타입도 확인해야 한다.
                if(shoutInfo.m_nSkill_ID == 0xFFFF || shoutInfo.m_nSkill_ID == usSkill_ID)
                {
                    bChatSend = true;
                }
            }
            else
            {
                bChatSend = true;
            }

            // 이미 외친 ChatType이면 외치지 않는다.
            if(shoutedList.end() != 
                std::find(shoutedList.begin(), shoutedList.end(), shoutInfo.m_nChatType))
            {
                bChatSend = false;
            }

            int nRand = Math::Random::ComplexRandom(100, 1);

            if(bChatSend && (nRand < shoutInfo.m_nTotalPercentage))
            {
                // 여러 메시지중에서 어느 메시지를 보낼지를 결정한다.
                int nPercentage = 0;

                ChatNode* lpNode = shoutInfo.m_lpNextNode;
                while(0 != lpNode)
                {
                    if(nPercentage <= nRand && nRand <= nPercentage + lpNode->m_nPercentage)
                    {   
                        // 원하는 구간 안에 랜덤값이 있으면 Break;
                        break;
                    }

                    nPercentage += lpNode->m_nPercentage;
                    lpNode = lpNode->m_lpNextNode;
                }

                if(0 != lpNode)
                {                    
                    shoutedList.push_back(shoutInfo.m_nChatType);

                    // 몬스터 채팅을 채팅서버로 보낸다. usState는 X좌표, usError는 Z좌표이다.
                    // 노멀 채팅일때만 좌표가 의미가 있다.

                    char szChatBuffer[PktChat::PktChatMaxSize * 2];
                    char szNameBuffer[PktChat::PktChatMaxSize];

                    szChatBuffer[0] = 0;
                    szNameBuffer[0] = 0;

                    if(0 != szName)
                    {
                        // 이름 복사하기..
                        _snprintf(szNameBuffer, PktChat::PktChatMaxSize - 1, "%s", szName);
                        szNameBuffer[PktChat::PktChatMaxSize - 1];
                    }

                    const char* szTarget    = 0;
                    char*       szTargetPos = 0;
                    size_t      nTargetLen  = 0;

                    char*       szMessageResult = 0;
                    size_t      nMessageLen = 0;

                    switch(shoutInfo.m_nBehavior)
                    {
                    case NORMAL_ATTACK:
                    case SKILL_ATTACK:
                    case CRITICAL_ATTACK:
                        
                        szTarget = "$DEFNAME$";
                        break;

                    case NORMAL_ATTACKED:                        
                    case SKILL_ATTACKED:                        
                    case CRITICAL_ATTACKED:

                        szTarget = "$ATTNAME$";
                        break;
                        
                    case DEAD:

                        szTarget = "$KILLERNAME$";
                        break;
                    }

                    if(0 != szTarget && (szTargetPos = strstr(lpNode->m_szMonsterChat, szTarget)))
                    {                        
                        // 치환해서 문자열 복사.
                        nTargetLen = strlen(szTarget);

                        const char* szLastMessage = szTargetPos + nTargetLen;
                        size_t nPreMessageLen     = szTargetPos - lpNode->m_szMonsterChat;
                        
                        memcpy(szChatBuffer, lpNode->m_szMonsterChat, nPreMessageLen);
                        nMessageLen = _snprintf(szChatBuffer + nPreMessageLen, 
                            PktChat::PktChatMaxSize * 2 - 1, "%s%s", szNameBuffer, szLastMessage);

                        szMessageResult = szChatBuffer;
                        nMessageLen += nPreMessageLen;                        
                    }
                    else
                    {
                        szMessageResult = lpNode->m_szMonsterChat;
                        nMessageLen     = lpNode->m_usChatLength;
                    }

                    PktChat::PktChatCmd eChatCmd = static_cast<PktChat::PktChatCmd>(shoutInfo.m_nChatType);

                    CChatPacket chatPacket(szMessageResult, dwMonsterCID, eChatCmd, 0);

                    if(chatPacket.IsValid())
                    {
                        // 챗 타입은 공지/일반/외치기 뿐이다.

                        switch(eChatCmd)
                        {                        
                        case PktChat::SHOUT:
                        case PktChat::NOTICE:

                            // 몬스터가 속한 존에만 전부 전송한다.
                            CCreatureManager::GetInstance().SendAllCharacter(
                                chatPacket.GetCompressedPacket(), 
                                chatPacket.GetCompressedSize(), CmdCharChat);
                            break;

                        case PktChat::ADMIN_SHOUT: // was NORMAL

                            {
                                // 몬스터 주변 8개 셀에 전부 메시지를 보낸다.
                                CMonster* lpMonster = 
                                    CCreatureManager::GetInstance().GetMonster(dwMonsterCID);

                                CCell* lpCell = 0;

                                if(0 != lpMonster && 0 != (lpCell = lpMonster->GetCellPos().m_lpCell))
                                {
                                    lpCell->SendAllNearCellCharacter(
                                        chatPacket.GetPacketData(),
                                        chatPacket.GetPacketSize(), CmdCharChat);
                                }
                            }
                            break;
                        }
                    }
                }

            }
        }
    };
}

void CMonsterShout::AddMonsterShout(ShoutInfo& shoutInfo, ChatNode& chatNode)
{
    std::pair<ShoutMap::iterator, ShoutMap::iterator> result = m_ShoutMap.equal_range(shoutInfo.m_nKID);

    for(; result.first != result.second; ++result.first)
    {
        ShoutInfo& foundShoutInfo = result.first->second;

        if(foundShoutInfo.m_nKID        == shoutInfo.m_nKID &&
            foundShoutInfo.m_nBehavior   == shoutInfo.m_nBehavior &&
            foundShoutInfo.m_nSkill_ID   == shoutInfo.m_nSkill_ID &&
            foundShoutInfo.m_nChatType   == shoutInfo.m_nChatType)
        {
            foundShoutInfo.m_nTotalPercentage += chatNode.m_nPercentage;

            ChatNode* lpNode = foundShoutInfo.m_lpNextNode;
            while(0 != lpNode && 0 != lpNode->m_lpNextNode)
            {
                lpNode = lpNode->m_lpNextNode;
            }

            if(0 != lpNode && 0 == lpNode->m_lpNextNode)
            {                    
                lpNode->m_lpNextNode = new ChatNode(chatNode);
            }

            break;
        }
    }

    if(result.first == result.second)
    {
        shoutInfo.m_lpNextNode = new ChatNode(chatNode);
        shoutInfo.m_nTotalPercentage = chatNode.m_nPercentage;
        m_ShoutMap.insert(std::make_pair(shoutInfo.m_nKID, shoutInfo));
    }
}

