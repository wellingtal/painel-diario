#ifndef _SEND_CHAR_ATTACK_H_
#define _SEND_CHAR_ATTACK_H_

// �ӽ� �Լ� ������ �� �������
#include <Network/Packet/PacketStruct/CharAttackPacket.h>

// ���� ����
class   CSendStream;
class	CAggresiveCreature;
class	CCharacter;
class	CMonster;
struct	AtType;
struct	Position;

namespace GameClientSendPacket
{
	// ��� ��Ŷ ���	
    bool SendCharDuelCmd(CSendStream& SendStream, unsigned long dwSenderID, unsigned long dwRecverID, 
		unsigned char cCmd, unsigned short usError);

	// ���� ����
    bool SendCharAttacked(CSendStream& SendStream, CAggresiveCreature* pAttackCreature, CAggresiveCreature* pDefendCharacter, 
		const AtType attackType, float fDir, unsigned short wDamage, unsigned char cDefenserJudge, unsigned short wMPHeal, unsigned short usError);

	// ��� ������
	bool SendCharEquipDurability(CSendStream& SendStream, unsigned long dwCharID, unsigned char cIndex, unsigned char cValue, unsigned char cValueMax,
		unsigned short usError);

	// ��ȭ ���
	bool SendCharPeaceMode(CSendStream& SendStream, unsigned long dwCharID, 
		unsigned char cLeftTime, bool bPeace, unsigned short usError);

	// ��ȯ�� ���
	bool SendCharSummonCmd(CCharacter* lpCharacter, CMonster* lpSummonee, 
        unsigned char cCmd, unsigned long dwTargetID);

	// ��ȯ
	bool SendCharSummon(unsigned long dwCharID, CAggresiveCreature* lpSummonee);

	// ������ Ack (��Ʋ�׶��� �����)	
	bool SendCharBattleGroundRespawn(CSendStream& SendStream, unsigned long dwCharID, unsigned short wTurn, unsigned short wWaitNum, 
		unsigned short wLeftTime, unsigned short wHumanNum, unsigned short wAkhanNum, unsigned short usError);

	// ī�޶� ��ũ��Ʈ
	bool SendCameraScript(unsigned long dwDeleteObject, unsigned long dwNewObject, Position NewPos, unsigned long dwTime);

    // ����
    bool SendCharDeadToParty(CCharacter* lpDeadCharacter, CAggresiveCreature* lpReference, PktDeadInfo::TypeCode eTypeCode);
};

#endif