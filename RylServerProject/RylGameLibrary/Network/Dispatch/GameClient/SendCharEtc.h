#ifndef _SEND_CHAR_ETC_H_
#define _SEND_CHAR_ETC_H_

#include <DB/DBdefine.h>


// ���� ����
class   CSendStream;
class	CCharacter;

struct	TakeType;
struct	RejectOption;
struct	Position;
struct  PktAdmin;

namespace GameClientSendPacket
{
	// ĳ���� ���ε� ������	
    bool SendCharBindPosition(CCharacter& character, unsigned long dwNPCID, 
		unsigned char cCmd, Position Pos, char cZone, unsigned short usError);

	// HP, MP ȸ��
	bool SendCharHPRegen(CSendStream& SendStream, unsigned long dwCID, 
        unsigned short nNowHP, unsigned short nNowMP);

	// ĳ���� �� ���� �̵�
	bool SendCharQuickSlotMove(CSendStream& SendStream, TakeType& takeType, unsigned short wSkillID, unsigned short usError);

	// ĳ���� �ɼ� ����
	bool SendCharControlOption(CSendStream& SendStream, unsigned long dwCharID, RejectOption& Reject);
	// ��(���嵵) ���� ��û
	bool SendCharFameInfo(CSendStream& SendStream, CCharacter* lpRequestCharacter, 
		const char* szWinCharName, const char* szLoseCharName, unsigned char cOtherNation, unsigned char cOtherClass,
		unsigned char cCmd, unsigned short usError);

	// ���� �ǳ�
	bool SendCharAuthorizePanel(CSendStream& SendStream, CCharacter* lpCaster, unsigned char cCmd);

	// ���� ����
	bool SendConfigInfoDB(CSendStream& SendStream, CCharacter* lpCharacter);

	// ����Ʈ ���ʽ�
	bool SendCharEliteBonus(CSendStream& SendStream, char cEliteBonus);

	// ��� ��� ��Ŷ ����
	bool SendCharAdmin(CSendStream& SendStream, PktAdmin* pPktAdmin, unsigned short wError);

	// ĳ���� Ư�� �̺�Ʈ ����
	// edith 2009.09.05 �ű� �̺�Ʈ ���ۺκ�
	bool SendExtraEvent(CSendStream& SendStream, unsigned long dwUserID, unsigned long dwCharID, unsigned long dwEventID, unsigned long dwValue1, unsigned long dwValue2, unsigned short usError);
};

#endif