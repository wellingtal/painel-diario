#ifndef _PARSE_CHAR_SKILL_H_
#define _PARSE_CHAR_SKILL_H_


// ���� ����
struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharUseSkill(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ��ų ���
    bool ParseCharSkillErase(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ��ų �����
};


#endif