#ifndef _DBAGENT_CASTLE_PACKET_PARSE_H_
#define _DBAGENT_CASTLE_PACKET_PARSE_H_

// 전방 참조
struct PktBase;
class CDBAgentDispatch;
class CCharacter;

namespace DBAgentPacketParse
{
	// 공성 업데이트
	bool ParseCastleInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 성 데이터 업데이트
	bool ParseCampInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 길드 요새 데이터 업데이트
	bool ParseFertilityInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 지력 데이터 업데이트
	bool ParseWorldWeaponInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// 월드 웨폰 업데이트

	bool ParseCreateCamp(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 새로 생성된 길드 요새
	bool ParseCreateSiegeArms(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// 새로 생성된 공성 병기

	bool ParseCastleCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 성 관련 명령 패킷
	bool ParseCampCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 길드 요새 명령 패킷
	bool ParseSiegeArmsCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 공성 병기 명령 패킷

	bool ParseCastleRight(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 성 관리 권한 변경
	bool ParseCampRight(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 길드 요새 관리 권한 변경

	bool ParseGameTimeInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 게임 시간 관련 패킷
	bool ParseCastleTaxMove(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 성 임시 세금 -> 누적 세금 이동 정보 업데이트
	bool ParseCastleSiegeCount(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// 공성 시간 후 성의 남은 공성 횟수 정보 업데이트

	bool ParseCampMessage(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 길드 요새 관련 메세지 처리
	bool ParseProcessMining(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 채굴기 프로세스 실행

	bool ParseWarOnOff(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 길드전, 국가전 플래그 처리

	bool ParseRealmPoint(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// 국가전쟁 공헌훈장 포인트.

	void RealmInchantAdd(CCharacter* lpCharacter);												// 국가전쟁 공헌훈장 포인트 인챈트 사용.
	void RealmInchantRemove(CCharacter* lpCharacter);											// 국가전쟁 공헌훈장 포인트 인챈트 삭제.

	bool ParseStatueCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// 국가전쟁 석상 커맨드 패킷 처리

	void SiegeMovePos(CCharacter* pCharacter);													// 공성전 캐릭터 리스폰 처리.	
};

#endif
