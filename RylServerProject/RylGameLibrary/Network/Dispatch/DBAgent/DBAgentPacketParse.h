#ifndef _DBAGENT_PACKET_PARSE_H_
#define _DBAGENT_PACKET_PARSE_H_

// 전방 참조
struct PktBase;
class CDBAgentDispatch;
class CGameClientDispatch;

namespace DBAgentPacketParse
{
	bool SendUserKill(CDBAgentDispatch& DBAgentDispatch, unsigned long dwUserID, unsigned short usError);
	bool SendServerLogout(CDBAgentDispatch& DBAgentDispatch);
    bool SendAbnormalLogout(unsigned long dwUID, unsigned long dwCID, 
        unsigned long dwSessionID, unsigned long dwRequestKey, 
        CGameClientDispatch* lpGameClientDispatch);

	bool SendRankDelete(const char* szName,	unsigned short usError);							// 이름 변경시 랭킹순위에 있을 경우 삭제.
					

	bool ParseUserKill(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);

    // 로그인시 DB로부터 순서대로 업데이트됨. (DepositUpdate를 받으면 종료.)
    bool ParseUpdateDBData(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);          	// DB데이터 업데이트
    bool ParseFriendList(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);            	// 친구 리스트 업데이트
	bool ParseQuestDB(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 퀘스트 업데이트
	bool ParseDepositCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);            	// DB데이터 업데이트

	bool ParseConfigInfoDB(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 설정 업데이트

    bool ParseAgentParty(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 파티 명령
    bool ParseDepositUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);         	// 창고 업데이트
    bool ParseAgentZone(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);

   	bool ParseSysServerLogin(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// 서버 로그인
	bool ParseSysServerVerUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);		// 서버 버젼 업데이트
	bool ParseSysChannelUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// 서버 종족 비율 업데이트    
	bool ParseSysRankingUpdate(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// 서버 랭킹 정보 업데이트

    bool ParseBillingTimeoutNotify(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);  	// 빌링 타임아웃 공지
	bool ParseBillingTimeCheckNotify(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);	// 빌링 체크

	bool ParseCharUseCashItem(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// 캐쉬아이템 사용
	bool ParseExtraEvent(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 엑스트라 이벤트 발동


	bool ParseEventDropItem(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 아이템 떨구기 이벤트
	bool ParseCharAdminCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 어드민 명령 처리
    bool ParseGiveItemToTempInven(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);       // 임시인벤으로 아이템 지급
    bool ParseCharNameChangeAck(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);         // 이름 변경 요청에대한 Ack

	bool ParseSaveEnemy(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 듀얼시 연사 방지 관련.

	bool ParseKeyInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 조이스틱 키 정보 관련..
};

#endif