#ifndef _SEND_CHAR_LEVELUP_H_
#define _SEND_CHAR_LEVELUP_H_


// ���� ����
class	CSendStream;

struct	ChState;
struct	CharacterDBData;
struct	SKILL;

namespace Item
{
	struct ItemPos;
}

namespace GameClientSendPacket
{
	// Ŭ���� ���׷��̵�
    bool SendCharClassUpgrade(CSendStream& SendStream, unsigned long dwCharID, 
		CharacterDBData& DBData, unsigned short usError);

	// IP ����
    bool SendCharIncreasePoint(CSendStream& SendStream, unsigned long dwCharID, 
		ChState State, unsigned short usError);

	// ������
	bool SendCharLevelUp(CSendStream& SendStream, unsigned long dwCharID, 
		CharacterDBData& DBData);

	// ����� 
	bool SendCharAward(CSendStream& SendStream, unsigned long dwCharID, 
		unsigned long dwExp);

	// ���� ��й� (Ŭ���� ó�� ���·�)
	bool SendCharStateRedistribution(CSendStream& SendStream, unsigned long dwCharID, 
		const ChState& state, const SKILL& skill, unsigned short usError);

	// �������ͽ� ���Ʒ�
	bool SendCharStatusRetrain(CSendStream& SendStream, unsigned long dwCharID, CharacterDBData& DBData,
		Item::ItemPos InvenPos, unsigned long dwGold, unsigned short usError);

};

#endif