#include "stdafx.h"

#include <boost/pool/pool_alloc.hpp>

#include <Network/Session/Session.h>
#include <Network/XORCrypt/XORCrypt.h>

#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>

#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include <Creature/Character/Character.h>
#include <GameGuardLib/ggerror.h>

#include <mmsystem.h>

#include "PacketParse.h"
#include "GameClientDispatch.h"


// 함수 선언
bool LogFailDispatch(CGameClientDispatch& GameClientDispatch, 
                     const char* szDetailText, const unsigned char cCmd);


CGameClientDispatchTable& CGameClientDispatchTable::GetInstance()
{
    static CGameClientDispatchTable gameClientDispatchTable;
    return gameClientDispatchTable;
}

CGameClientDispatchTable::CGameClientDispatchTable()
:   CPacketDispatchTable<CGameClientDispatch::FnProcess>(UCHAR_MAX)
{
    using namespace GameClientParsePacket;

    // 기본적인 Process들이 들어갈 것. ( character관련... )
    
    // 캐릭터 아이템 관련 - ParseCharItem.cpp
    AddDispatch(CmdCharTakeItem,            ParseCharTakeItem);     			// 아이템 이동 및 스택
    AddDispatch(CmdCharSwapItem,         	ParseCharSwapItem);					// 아이템 스왑
    AddDispatch(CmdCharRepairItem,       	ParseCharRepairItem);				// 아이템 수리
    AddDispatch(CmdCharRepairAllItem,		ParseCharRepairAllItem);			// 아이템 수리 (장비하고 있는 아이템 모두)
    AddDispatch(CmdCharUseItem,          	ParseCharUseItem);					// 아이템 사용
    AddDispatch(CmdCharTradeItem,        	ParseCharTradeItem);				// 아이템 거래
    AddDispatch(CmdCharEquipShopInfo,       ParseCharEquipShopInfo);			// NPC 장비 상점 정보
    AddDispatch(CmdCharPickUp,           	ParseCharPickUp);					// 아이템 집기
    AddDispatch(CmdCharPullDown,         	ParseCharPullDown);					// 아이템 떨구기
    AddDispatch(CmdCharSplitItem,        	ParseCharSplitItem);				// 아이템 나누기
    AddDispatch(CmdCharTakeGold,         	ParseCharTakeGold);					// 돈 다루기
    AddDispatch(CmdDeposit,              	ParseCharDepositCmd);   			// 창고 관련 패킷들
    AddDispatch(CmdCharAutoRouting,			ParseCharAutoRouting);				// 오토 루팅
    AddDispatch(CmdCharUseCashItem,			ParseCharUseCashItem);				// 캐쉬아이템
	
    // 캐릭터 아이템 업그레이드 관련 - ParseCharUpgradeItem.cpp
    AddDispatch(CmdCharInstallSocket,    	ParseCharInstallSocket);			// 아이템 인스톨 소켓
    AddDispatch(CmdCharItemChemical,		ParseCharItemChemical);				// 아이템 합성
    AddDispatch(CmdCharUpgradeItem,      	ParseCharUpgradeItem);				// 아이템 업그레이드
    AddDispatch(CmdCharItemOptionGraft,     ParseCharItemOptionGraft);			// 아이템 옵션 이식
	AddDispatch(CmdCharItemCompensation,	ParseCharItemCompensation);			// 아이템 보상 판매 

    // 캐릭터 스킬 관련 - ParseCharSkill.cpp
    AddDispatch(CmdCharUseSkill,         	ParseCharUseSkill);					// 스킬 사용
//  AddDispatch(CmdCharSkillLock,       	ParseCharSkillLock);				// 스킬 락
//  AddDispatch(CmdCharSkillUnLock,     	ParseCharSkillUnLock);				// 스킬 락 해제
    AddDispatch(CmdCharSkillErase,       	ParseCharSkillErase);				// 스킬 지우기
    
    // 캐릭터 전투 관련 패킷 - ParseCharAttack.cpp
    AddDispatch(CmdCharAttack,          	ParseCharAttack);					// 공격 패킷
    AddDispatch(CmdCharSwitchEQ,      		ParseCharSwitchEQ);					// 장비 바꾸기
    AddDispatch(CmdCharRespawn,         	ParseCharRespawn);					// 캐릭터 리스폰
    AddDispatch(CmdCharRespawnWaitQueue, 	ParseCharRespawnWaitQueue);			// 리스폰 큐 대기인 정보 (배틀그라운드용)
    AddDispatch(CmdCharRespawnInfo,			ParseCharRespawnInfo);				// 리스폰 정보 (리스폰 위치들)
    AddDispatch(CmdCharRespawnAreaInfo,		ParseCharRespawnAreaInfo);			// 리스폰 지역 세부 정보
//  AddDispatch(CmdCharMoveUpdate,      	ParseCharMoveUpdate);				// 캐릭터 움직임 업데이트
    AddDispatch(CmdCharDuelCmd,         	ParseCharDuelCmd);					// 듀얼 패킷 명령	
    AddDispatch(CmdCharPeaceMode,			ParseCharPeaceMode);				// 반전 모드
    AddDispatch(CmdCharSummonCmd,			ParseCharSummonCmd);				// 소환수 명령

    // 원래는 UDP p2p패킷이지만 서버로 올림
    AddDispatch(CmdCharMove,                ParseCharMoveEx);

    // 캐릭터 레벨업 관련 패킷 - ParseCharLevelUp.cpp
    AddDispatch(CmdCharClassUpgrade,     	ParseCharClassUpgrade);				// 클래스 업그레이드
    AddDispatch(CmdCharIncreasePoint,    	ParseCharIncreasePoint);			// IP 증가 패킷
    AddDispatch(CmdCharStateRedistribution, ParseCharStateRedistribution);		// IP 증가 패킷
    AddDispatch(CmdCharStatusRetrain,		ParseCharStatusRetrain);			// 캐릭터 스테이터스 재훈련

    // 캐릭터 커뮤니티 관련 패킷 - ParseCharCommunity.cpp
    AddDispatch(CmdCharExchangeCmd,      	ParseCharExchangeCmd);				// 교환 패킷 명령	
    AddDispatch(CmdCharPartyCmd,         	ParseCharPartyCmd);					// 파티 패킷 명령
    AddDispatch(CmdCharPartyFind,        	ParseCharPartyFind);				// 파티 찾기

    AddDispatch(CmdCharStallOpen,			ParseCharStallOpen);				// 캐틱터 노점상 개설
    AddDispatch(CmdCharStallRegisterItem,	ParseCharStallRegisterItem);		// 캐틱터 노점상 아이템 등록
    AddDispatch(CmdCharStallEnter,			ParseCharStallEnter);				// 캐틱터 노점상 입장

    AddDispatch(CmdFriendAddRequest,        ParseCharFriendAdd);    			// 친구 추가
    AddDispatch(CmdFriendRemoveRequest,     ParseCharFriendRemove); 			// 친구 삭제
    AddDispatch(CmdFriendEtcRequest,        ParseCharFriendEtc);    			// 기타

    AddDispatch(CmdCreateGuild,				ParseCharCreateGuild);    			// 길드 생성
    AddDispatch(CmdGuildCmd,				ParseCharGuildCmd);    				// 길드 멤버 관련 명령
    AddDispatch(CmdGuildMark,				ParseCharGuildMark);    			// 길드 마크 변경
    AddDispatch(CmdGuildLevel,				ParseCharGuildLevel);    			// 길드 레벨 변경
    AddDispatch(CmdGuildRelation,			ParseCharGuildRelation);    		// 길드 관계 변경
    AddDispatch(CmdGuildInclination,		ParseCharGuildInclination);			// 길드 성향 변경
    AddDispatch(CmdGuildList,				ParseCharGuildList);    			// 길드 리스트
    AddDispatch(CmdGuildRight,				ParseCharGuildRight);    			// 길드 권한 설정
    AddDispatch(CmdGuildMemberList,			ParseCharGuildMemberList);			// 길드 멤버 리스트
    AddDispatch(CmdGuildSafe,				ParseCharGuildSafe);				// 길드 금고
	AddDispatch(CmdGuildHostilityList,		ParseCharGuildHostilityList);		// 적대 길드 현황 리스트
	AddDispatch(CmdGuildRelationInfo,		ParseCharGuildRelationInfo);		// 길드의 관계 리스트 요청

    // 어드민 관련 패킷 - ParseCharAdmin.cpp
    AddDispatch(CmdCharAdminCmd,			ParseCharAdminCmd);					// 어드민 커맨드
    AddDispatch(CmdCharNameChange,          ParseCharNameChange);               // 캐릭터 이름 변경

    // 캐릭터 퀘스트 관련 패킷 - ParseCharQuest.cpp
    AddDispatch(CmdCharStartQuest,			ParseCharStartQuest);				// 퀘스트 시작
    AddDispatch(CmdCharOperateTrigger,		ParseCharOperateTrigger);			// 트리거 발동
    AddDispatch(CmdCharCancelQuest,			ParseCharCancelQuest);				// 퀘스트 취소
    
    // 캐릭터 기타 패킷 - ParseCharEtc.cpp
    AddDispatch(CmdCharSuicide,          	ParseCharSuicide);					// 캐릭터 자살	
    AddDispatch(CmdCharBindPosition,     	ParseCharBindPosition);				// 캐릭터 바인드 포지션
    AddDispatch(CmdCharQuickSlotMove,    	ParseCharQuickSlotMove);			// 캐릭터 퀵 슬롯 이동
    AddDispatch(CmdCharControlOption,    	ParseCharControlOption);			// 캐릭터 옵션 조정
    AddDispatch(CmdCharAuthorizePanel,		ParseCharAuthorizePanel);
    AddDispatch(CmdCharFameInfo,         	ParseCharFameInfo);					// 캐릭터 명성(공헌도) 정보 요청
	AddDispatch(CmdNameCheck,			  	ParseCharNameCheck);				//hz added for new packet
    AddDispatch(CmdRankingInfo,			 	ParseCharRankingInfo);				// 랭킹 정보

    // 배틀 그라운드 서버군 패킷 - ParseCharBGServer.cpp
    AddDispatch(CmdBGServerMapList,			ParseCharBGServerMapList);			// 배틀 그라운드 서버군 맵(방) 정보 리스트
    AddDispatch(CmdBGServerResultList,		ParseCharBGServerResultList);		// 배틀 그라운드 서버군 맵(방) 결과 리스트
    AddDispatch(CmdBGServerMoveZone,		ParseCharBGServerMoveZone);			// 배틀 그라운드 서버군 존 이동 (맵(방) 이동)
    AddDispatch(CmdBGServerMileageChange,	ParseCharBGServerMileageChange);	// 배틀 그라운드 서버군 환전소 명령
    AddDispatch(CmdBGServerCharSlot,		ParseCharBGServerCharSlot);			// 배틀 그라운드 서버 정섭 캐릭터 정보 요청

    // 공성 패킷 - ParseCharCastle.cpp
    AddDispatch(CmdCastleCmd,				ParseCharCastleCmd);				// 성 관련 오브젝트 명령
    AddDispatch(CmdCampCmd,					ParseCharCampCmd);					// 길드 요새 명령
    AddDispatch(CmdSiegeArmsCmd,			ParseCharSiegeArmsCmd);				// 공성 병기 명령
    AddDispatch(CmdCastleRight,				ParseCharCastleRight);				// 성 권한 설정
    AddDispatch(CmdCampRight,				ParseCharCampRight);				// 길드 요새 권한 설정

    // 채팅 패킷 - ParseCharChat.cpp 
    AddDispatch(CmdCharWhisper,          	ParseCharWhisper);					// 속삭임
    AddDispatch(CmdCharChat,             	ParseCharChat);						// 캐틱터 채팅
    
    AddDispatch(CmdCharCastObjectInfo,      ParseCastObjectInfo);               // 오브젝트 던지기 패킷 정보

	AddDispatch(CmdCharInstallRuneSocket,	ParseCharInstallRuneSocket);		// 룬설치, 삭제 관련 정보.

	AddDispatch(CmdTakeMaterial,			ParseCharTakeMaterial);				// 길드 요새 자재 넣기/빼기
	AddDispatch(CmdTakeCastleJewel,			ParseCharTakeCastleJewel);			// 성 상징물 보석 아이템 넣기/빼기
	AddDispatch(CmdWarOnOff,				ParseWarOnOff);						// 길드전, 국가전 플래그 바꾸기

	AddDispatch(CmdKeyInfo,					ParseKeyInfo);						// 조이스틱 키 정보 관련.
}



CGameClientDispatch::CGameClientDispatch(CSession& Session,
										 CGameClientDispatchTable& GameClientDispatchTable)
:   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE),
    m_lpCharacter(0), 
	m_dwUID(0), 
	m_GameClientDispatchTable(GameClientDispatchTable),
    m_dwMoveServerID(0)
{    
	// edith 2009.08.11 게임가드 2.5 업그레이드
//	m_CSAuth.Init();
	m_CSAuth2.Init();

	DETLOG2(g_Log, "DP:0x%p/UID:%u/GameClientDispatch Created", this, m_dwUID);
}


CGameClientDispatch::~CGameClientDispatch()
{
    CGameClientDispatch::Disconnected();

    // 객체 소멸 로그
    DETLOG2(g_Log, "DP:0x%p/UID:%u/GameClientDispatch Destroy", this, m_dwUID);
}

void CGameClientDispatch::Connected()
{
}

void CGameClientDispatch::Disconnected()
{
    if (0 != m_lpCharacter)
    {
        m_lpCharacter->SetDispatcher(0);
    }    
}

void CGameClientDispatch::Disconnect()
{
    CloseSession();
}

bool CGameClientDispatch::DispatchPacket(PktBase* lpPktBase)
{
    unsigned char   cCmd        = lpPktBase->GetCmd();
    bool            bHackCheck  = true;

    // 마지막으로 받은 패킷 시간 업데이트.
    m_CheckPing.SetLastPingRecvTime(CPulse::GetInstance().GetLastTick());

    switch (cCmd)
    {
		case CmdSysPing:			
            bHackCheck = m_CheckSpeedHack.CheckTimeUpdate(static_cast<PktSyP*>(lpPktBase)->m_dwTickTime);
			break;//return true;
	    
		case CmdCharAttack:     
			bHackCheck = m_CheckSpeedHack.CheckAttackReplay(static_cast<PktAt*>(lpPktBase));
			break;
    }

    if (!bHackCheck)
    {
        ERRLOG3(g_Log, "DP:0x%p/IP:%s/UID:%10d/스피드핵 체크에 걸렸습니다. 접속을 끊습니다.", 
            this, GetRemoteAddr().get_addr_string(), m_dwUID);
    }
	else if (CXORCrypt::PAGE_VERSION != CXORCrypt::GetInstance().GetPageVer(lpPktBase->GetCodePage()))
	{
        // (클라이언트 -> 게임 서버)간 통신은 한 가지 코드 페이지만(현재는 1번)을 사용합니다.
        ERRLOG3(g_Log, "DP:0x%p/IP:%s/UID:%d 잘못된 코드 페이지의 패킷입니다. 접속을 끊습니다.", 
            this, GetRemoteAddr().get_addr_string(), m_dwUID);
	}
    else 
    {
        CGameClientDispatch::FnProcess fnProcess = 
            m_GameClientDispatchTable.GetDispatch(cCmd);

        const char* szErrorString = 0;

        if (0 == fnProcess)
        {
            szErrorString = "Unknown Gameserver Packet";
        }
	    else if (!fnProcess(*this, lpPktBase))
        {
            szErrorString = "GameServer packet process failed.";
        }

        if (0 != szErrorString)
        {
            LogFailDispatch(*this, szErrorString, cCmd);
        }

        return true;
    }

    return false;
}

bool CGameClientDispatch::Dispatch()
{
    unsigned long dwCurrentTime = timeGetTime();

	// 핑 체크
	if (0 != m_lpCharacter &&                   // 캐릭터가 있어야 하고
        0 == m_lpCharacter->GetAdminLevel() &&  // 운영자가 아니고
        !m_CheckPing.CheckPing(dwCurrentTime))  // Ping체크에 실패하면
	{
	    const int MAX_BUFFER = 256;
	    char szBuffer[MAX_BUFFER];

	    unsigned long dwPingCount           = 0;
	    unsigned long dwLastPingRecvTime    = 0;
	    unsigned long dwFirstCheckTime      = 0;

        unsigned long   dwCID = m_lpCharacter->GetCID();
	    const char*     szCharacterName = m_lpCharacter->GetCharacterName();
        
        m_CheckPing.GetPingData(dwPingCount, dwLastPingRecvTime, dwFirstCheckTime);

        _snprintf(szBuffer, MAX_BUFFER - 1, 
			"UID:%u/CID:%u/Name:%s/CurrentTime:%u/LastPingTime:%u/PingCount:%u/FirstCheckTime:%u/"
			"PingCheck failed. disconnect now.",
			m_dwUID, dwCID, szCharacterName, dwCurrentTime, 
			dwLastPingRecvTime, dwPingCount, dwFirstCheckTime);

		LogErrorPacket(szBuffer, 0);
		return false;
	}

	return CRylServerDispatch::Dispatch();
}

void CGameClientDispatch::ProcessTooManyPacket(CBufferQueue& bufferQueue)
{
    if (0 != m_lpCharacter)
    {
        typedef std::map<unsigned char, unsigned long, std::greater<unsigned long>, 
            boost::fast_pool_allocator< std::pair<unsigned char, unsigned long> > > PacketStatistics;

        const int           MAX_BUFFER = 512;
        char                szBuffer[MAX_BUFFER];

        PacketStatistics    packetStatistics;

        const SOCKADDR_IN&  sockAddr = GetRemoteAddr().get_addr_in();

        int nLength = _snprintf(szBuffer, MAX_BUFFER, 
            "/UID:%d/CID:0x%08x/Name:%s/IP:%15s/ProcessPkt:%4d/RemainPkt:%4d/",
            m_lpCharacter->GetUID(), m_lpCharacter->GetCID(), m_lpCharacter->GetCharacterName(),        
            inet_ntoa(sockAddr.sin_addr), m_dwMaxProcessPacketPerPulse, bufferQueue.getBufferNum());
        
        CBufferQueue tempQueue;
        tempQueue.splice(bufferQueue);
    
        const int DROP_PACKET_LIMIT     = 20;
        const int DROP_PACKET_MAX_LIMIT = 40;

        bool bDropPacket = DROP_PACKET_MAX_LIMIT < tempQueue.getBufferNum();

        // 패킷 통계를 구한다.
        CBuffer* lpBuffer = tempQueue.getHead();
        for(; 0 != lpBuffer; lpBuffer = lpBuffer->next())
        {
            unsigned char cCmd = reinterpret_cast<PktBase*>(lpBuffer->rd_ptr())->GetCmd();               
            ++packetStatistics[cCmd];
        }

        const unsigned long dwMaxPrint  = 5;
        unsigned long       dwPrinted   = 0;

        PktBase::CMDType aryDropCmd[dwMaxPrint];
        PktBase::CMDType* lpDropCmdEnd = aryDropCmd;

        std::pair<unsigned char, unsigned long> resultPair;

        for(PacketStatistics::iterator itr = packetStatistics.begin();
            itr != packetStatistics.end() && dwPrinted < dwMaxPrint; ++itr, ++dwPrinted)
        {
            resultPair = *itr;

            nLength += _snprintf(szBuffer + nLength, MAX_BUFFER - nLength, "0x%02x:%5d/", resultPair.first, resultPair.second);

            // 패킷이 한번에 얼마 이상 온 경우는 Drop할 리스트에 포함시킨다.
            // 여기서는 Top 5에 속하는 패킷 중 DROP_PACKET_LIMIT 이상 되는 패킷만 버린다.
            if(DROP_PACKET_LIMIT < resultPair.second)
            {
                *lpDropCmdEnd = resultPair.first;
                ++lpDropCmdEnd;
            }
        }

        std::sort(aryDropCmd, lpDropCmdEnd);

        ERRLOG0(g_Log, szBuffer);

        for(;;)
        {
            lpBuffer = tempQueue.dequeue();
            if(0 == lpBuffer)
            {
                break;
            }
            else 
            {
                unsigned char cCmd = reinterpret_cast<PktBase*>(lpBuffer->rd_ptr())->GetCmd();               

                // 버려도 될 패킷이면 버린다.
                if(bDropPacket && std::binary_search(aryDropCmd, lpDropCmdEnd, cCmd))
                {                    
                    SAFE_RELEASE_BUFFER(lpBuffer);
                }
                else
                {
                    bufferQueue.enqueue(lpBuffer);
                }
            }
        }
    }
}

bool LogFailDispatch(CGameClientDispatch& GameClientDispatch, 
                    const char* szDetailText, const unsigned char cCmd)
{
    unsigned long dwUID = GameClientDispatch.GetUID();

    ERRLOG5(g_Log, "DP:0x%p/UID:%d/PacketCMD:0x%02x/IP:%15s/%s",
        &GameClientDispatch, dwUID, cCmd, 
        GameClientDispatch.GetRemoteAddr().get_addr_string(), szDetailText);

    return false;
}

unsigned long CGameClientDispatch::PopRequestKey() 
{
    unsigned long dwRequestKey = 0;
    if (!m_DBRequestQueue.empty())
    {
        dwRequestKey = m_DBRequestQueue.front();
        m_DBRequestQueue.pop_front();
    }

    unsigned long dwUID = 0;
    unsigned long dwCID = 0;
    
    if (0 != m_lpCharacter)
    {
        dwUID = m_lpCharacter->GetUID();
        dwCID = m_lpCharacter->GetCID();
    }

    DETLOG7(g_Log, "UID:%u/CID:0x%p(0x%p)/IP:%15s/DP:0x%p/DUID:%u/RequestKey:%d/ "
        "Pop RequestKey", dwUID, dwCID, m_lpCharacter, 
		GetRemoteAddr().get_addr_string(),        
        this, m_dwUID, dwRequestKey);

    return dwRequestKey;
}
    
void CGameClientDispatch::PushRequestKey(unsigned long dwRequestKey) 
{
    unsigned long dwUID = 0;
    unsigned long dwCID = 0;
    
    if (0 != m_lpCharacter)
    {
        dwUID = m_lpCharacter->GetUID();
        dwCID = m_lpCharacter->GetCID();
    }

    DETLOG7(g_Log, "UID:%u/CID:0x%p(0x%p)/IP:%15s/DP:0x%p/DUID:%u/RequestKey:%d/ "
        "Push RequestKey", dwUID, dwCID, m_lpCharacter, 
        GetRemoteAddr().get_addr_string(),
        this, m_dwUID, dwRequestKey);

    m_DBRequestQueue.push_back(dwRequestKey);
}

bool CGameClientDispatch::GetAuthQuery(GG_AUTH_DATA** lppAuthData_Out)
{
	unsigned long dwGGErrCode = m_CSAuth2.GetAuthQuery();
	if (dwGGErrCode != ERROR_SUCCESS)
	{
		PrintGameGuardError(dwGGErrCode);
		ERRLOG1(g_Log, "CID:0x%08x 인증 코드(2) GetAuthQuery 오류", m_lpCharacter->GetCID());
		return false;
	}

	*lppAuthData_Out = &m_CSAuth2.m_AuthQuery;
	return true;
}

bool CGameClientDispatch::CheckAuthAnswer(void)
{
	unsigned long dwGGErrCode = m_CSAuth2.CheckAuthAnswer();
	if (dwGGErrCode != ERROR_SUCCESS)
	{
		PrintGameGuardError(dwGGErrCode);
		ERRLOG1(g_Log, "CID:0x%08x 인증 코드(2) CheckAuthAnswer 오류", m_lpCharacter->GetCID());
		return false;
	}

	return true;
}

void CGameClientDispatch::PrintGameGuardError(void)
{
/*	switch (m_CSAuth.PPGetLastError())
	{
		case 1:
            ERRLOG1(g_Log, "CID:0x%08x 키가 일치하지 않습니다. "
				"암호화되지 않은 패킷이거나, UserKey가 다른 클라이언트로부터 온 패킷일 수 있습니다.", m_lpCharacter->GetCID());
			break;

		case 2:
			ERRLOG1(g_Log, "CID:0x%08x 시퀀스넘버가 최근의 패킷과 같습니다. "
				"패킷 리플라이 공격이 원인일 수 있습니다.", m_lpCharacter->GetCID());
			break;

		case 3:
			ERRLOG1(g_Log, "CID:0x%08x 시퀀스넘버가 비정상입니다. 서버와 클라이언트의 약속된 시퀀스넘버가 일치하지 않습니다. "
				"네트웍 문제로 패킷 몇 개가 유실 되었을 수 있습니다. TCP 프로토콜에서는 거의 발생하지 않는 에러입니다.", m_lpCharacter->GetCID());
			break;

		case 4:
			ERRLOG1(g_Log, "CID:0x%08x 패킷 CRC 체크에 실패했습니다. "
				"패킷 에디터 등으로 고의로 패킷을 조작했을 수 있습니다.", m_lpCharacter->GetCID());
			break;
	}
*/
	// edith 2009.08.11 게임가드 2.5 업그레이드
    ERRLOG1(g_Log, "CID:0x%08x 제거된 게임가드 버젼을 사용하였습니다.", m_lpCharacter->GetCID());
}

void CGameClientDispatch::PrintGameGuardError(unsigned long dwGGErrCode)
{
    const char* szErrorString = "알 수 없는 에러입니다";

	switch (dwGGErrCode)
	{
		case ERROR_GGAUTH_FAIL_MEM_ALLOC:	
			szErrorString = "메모리 할당 실패";
			break;

		case ERROR_GGAUTH_FAIL_LOAD_DLL:	
			szErrorString = "ggauth.dll 로드 실패";
			break;

		case ERROR_GGAUTH_FAIL_GET_PROC:	
			szErrorString = "ggauth.dll의 Export 함수 가져오기 실패";
			break;

		case ERROR_GGAUTH_FAIL_BEFORE_INIT:	
			szErrorString = "ggauth.dll이 초기화되기 전에 Export 함수 호출하였음";
			break;

		case ERROR_GGAUTH_INVALID_PARAM:
			szErrorString = "함수 호출 시 invalid parameter 전달";
			break;

		case ERROR_GGAUTH_NO_REPLY:
			szErrorString = "인증 Query에 대한 클라이언트의 응답 없음";
			break;

		case ERROR_GGAUTH_INVALID_PROTOCOL_VERSION:
			szErrorString = "클라이언트의 인증 프로토콜 버전 틀림";
			break;

		case ERROR_GGAUTH_INVALID_REPLY:
			szErrorString = "인증 Query에 대한 클라이언트의 응답값이 틀림";
			break;
	}

    DWORD dwCID = (0 != m_lpCharacter) ? m_lpCharacter->GetCID() : 0;

    const int MAX_LOG = 256;
    char szLog[MAX_LOG];

    if (0 < _snprintf(szLog, MAX_LOG - 1, "CID:0x%08x %s : %d / "
        "AQ.Index:0x%08x, AQ.V1:0x%08x, AQ.V2:0x%08x, AQ.V3:0x%08x, "
        "AA.Index:0x%08x, AA.V1:0x%08x, AA.V2:0x%08x, AA.V3:0x%08x",

        dwCID, szErrorString, dwGGErrCode,

        m_CSAuth2.m_AuthQuery.dwIndex, 
        m_CSAuth2.m_AuthQuery.dwValue1, 
        m_CSAuth2.m_AuthQuery.dwValue2, 
        m_CSAuth2.m_AuthQuery.dwValue3, 

        m_CSAuth2.m_AuthAnswer.dwIndex, 
        m_CSAuth2.m_AuthAnswer.dwValue1, 
        m_CSAuth2.m_AuthAnswer.dwValue2, 
        m_CSAuth2.m_AuthAnswer.dwValue3))
    {
        ERRLOG0(g_Log, szLog);
    }
}

void CGameClientDispatch::SetCharacter(CCharacter* lpCharacter)
{
    m_lpCharacter = lpCharacter;
    m_CheckSpeedHack.SetCharacter(lpCharacter);
}
