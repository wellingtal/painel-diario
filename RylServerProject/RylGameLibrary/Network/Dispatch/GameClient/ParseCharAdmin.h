#ifndef _PARSE_CHAR_ADMIN_H_
#define _PARSE_CHAR_ADMIN_H_


// 전방 참조
struct  PktBase;
struct	PktAdmin;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharAdminCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 어드민 패킷

	bool ProcessAdminCmd(CCharacter* lpAdmin, CCharacter* lpTarget, PktAdmin* lpPktAdmin);
	
    bool AddSpell(CCharacter* lpCaster, CCharacter* lpVictim, 
		unsigned short wSpellID, unsigned short wSpellLevel, unsigned long dwDurationSec);

    bool ParseCharNameChange(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);          // 이름 바꾸기
};


#endif