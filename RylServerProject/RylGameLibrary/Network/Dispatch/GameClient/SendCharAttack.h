#ifndef _SEND_CHAR_ATTACK_H_
#define _SEND_CHAR_ATTACK_H_

// 임시 함수 때문에 들어간 헤더파일
#include <Network/Packet/PacketStruct/CharAttackPacket.h>

// 전방 참조
class   CSendStream;
class	CAggresiveCreature;
class	CCharacter;
class	CMonster;
struct	AtType;
struct	Position;

namespace GameClientSendPacket
{
	// 듀얼 패킷 명령	
    bool SendCharDuelCmd(CSendStream& SendStream, unsigned long dwSenderID, unsigned long dwRecverID, 
		unsigned char cCmd, unsigned short usError);

	// 공격 당함
    bool SendCharAttacked(CSendStream& SendStream, CAggresiveCreature* pAttackCreature, CAggresiveCreature* pDefendCharacter, 
		const AtType attackType, float fDir, unsigned short wDamage, unsigned char cDefenserJudge, unsigned short wMPHeal, unsigned short usError);

	// 장비 내구도
	bool SendCharEquipDurability(CSendStream& SendStream, unsigned long dwCharID, unsigned char cIndex, unsigned char cValue, unsigned char cValueMax,
		unsigned short usError);

	// 평화 모드
	bool SendCharPeaceMode(CSendStream& SendStream, unsigned long dwCharID, 
		unsigned char cLeftTime, bool bPeace, unsigned short usError);

	// 소환수 명령
	bool SendCharSummonCmd(CCharacter* lpCharacter, CMonster* lpSummonee, 
        unsigned char cCmd, unsigned long dwTargetID);

	// 소환
	bool SendCharSummon(unsigned long dwCharID, CAggresiveCreature* lpSummonee);

	// 리스폰 Ack (배틀그라운드 대기중)	
	bool SendCharBattleGroundRespawn(CSendStream& SendStream, unsigned long dwCharID, unsigned short wTurn, unsigned short wWaitNum, 
		unsigned short wLeftTime, unsigned short wHumanNum, unsigned short wAkhanNum, unsigned short usError);

	// 카메라 스크립트
	bool SendCameraScript(unsigned long dwDeleteObject, unsigned long dwNewObject, Position NewPos, unsigned long dwTime);

    // 죽음
    bool SendCharDeadToParty(CCharacter* lpDeadCharacter, CAggresiveCreature* lpReference, PktDeadInfo::TypeCode eTypeCode);
};

#endif