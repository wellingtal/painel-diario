#ifndef _SEND_CHAR_ETC_H_
#define _SEND_CHAR_ETC_H_

#include <DB/DBdefine.h>


// 전방 참조
class   CSendStream;
class	CCharacter;

struct	TakeType;
struct	RejectOption;
struct	Position;
struct  PktAdmin;

namespace GameClientSendPacket
{
	// 캐릭터 바인드 포지션	
    bool SendCharBindPosition(CCharacter& character, unsigned long dwNPCID, 
		unsigned char cCmd, Position Pos, char cZone, unsigned short usError);

	// HP, MP 회복
	bool SendCharHPRegen(CSendStream& SendStream, unsigned long dwCID, 
        unsigned short nNowHP, unsigned short nNowMP);

	// 캐릭터 퀵 슬롯 이동
	bool SendCharQuickSlotMove(CSendStream& SendStream, TakeType& takeType, unsigned short wSkillID, unsigned short usError);

	// 캐릭터 옵션 조정
	bool SendCharControlOption(CSendStream& SendStream, unsigned long dwCharID, RejectOption& Reject);
	// 명성(공헌도) 정보 요청
	bool SendCharFameInfo(CSendStream& SendStream, CCharacter* lpRequestCharacter, 
		const char* szWinCharName, const char* szLoseCharName, unsigned char cOtherNation, unsigned char cOtherClass,
		unsigned char cCmd, unsigned short usError);

	// 인증 판넬
	bool SendCharAuthorizePanel(CSendStream& SendStream, CCharacter* lpCaster, unsigned char cCmd);

	// 설정 저장
	bool SendConfigInfoDB(CSendStream& SendStream, CCharacter* lpCharacter);

	// 엘리트 보너스
	bool SendCharEliteBonus(CSendStream& SendStream, char cEliteBonus);

	// 운영자 명령 패킷 전송
	bool SendCharAdmin(CSendStream& SendStream, PktAdmin* pPktAdmin, unsigned short wError);

	// 캐릭터 특수 이벤트 관련
	// edith 2009.09.05 신규 이벤트 제작부분
	bool SendExtraEvent(CSendStream& SendStream, unsigned long dwUserID, unsigned long dwCharID, unsigned long dwEventID, unsigned long dwValue1, unsigned long dwValue2, unsigned short usError);
};

#endif