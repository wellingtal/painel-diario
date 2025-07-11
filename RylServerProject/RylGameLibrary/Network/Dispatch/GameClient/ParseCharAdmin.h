#ifndef _PARSE_CHAR_ADMIN_H_
#define _PARSE_CHAR_ADMIN_H_


// ���� ����
struct  PktBase;
struct	PktAdmin;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharAdminCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ���� ��Ŷ

	bool ProcessAdminCmd(CCharacter* lpAdmin, CCharacter* lpTarget, PktAdmin* lpPktAdmin);
	
    bool AddSpell(CCharacter* lpCaster, CCharacter* lpVictim, 
		unsigned short wSpellID, unsigned short wSpellLevel, unsigned long dwDurationSec);

    bool ParseCharNameChange(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);          // �̸� �ٲٱ�
};


#endif