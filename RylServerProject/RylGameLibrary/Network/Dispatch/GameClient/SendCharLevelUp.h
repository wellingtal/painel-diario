#ifndef _SEND_CHAR_LEVELUP_H_
#define _SEND_CHAR_LEVELUP_H_


// 전방 참조
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
	// 클래스 업그레이드
    bool SendCharClassUpgrade(CSendStream& SendStream, unsigned long dwCharID, 
		CharacterDBData& DBData, unsigned short usError);

	// IP 증가
    bool SendCharIncreasePoint(CSendStream& SendStream, unsigned long dwCharID, 
		ChState State, unsigned short usError);

	// 레벨업
	bool SendCharLevelUp(CSendStream& SendStream, unsigned long dwCharID, 
		CharacterDBData& DBData);

	// 어워드 
	bool SendCharAward(CSendStream& SendStream, unsigned long dwCharID, 
		unsigned long dwExp);

	// 스탯 재분배 (클래스 처음 상태로)
	bool SendCharStateRedistribution(CSendStream& SendStream, unsigned long dwCharID, 
		const ChState& state, const SKILL& skill, unsigned short usError);

	// 스테이터스 재훈련
	bool SendCharStatusRetrain(CSendStream& SendStream, unsigned long dwCharID, CharacterDBData& DBData,
		Item::ItemPos InvenPos, unsigned long dwGold, unsigned short usError);

};

#endif