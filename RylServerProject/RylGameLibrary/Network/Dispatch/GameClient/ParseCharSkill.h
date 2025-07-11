#ifndef _PARSE_CHAR_SKILL_H_
#define _PARSE_CHAR_SKILL_H_


// 전방 참조
struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharUseSkill(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 스킬 사용
    bool ParseCharSkillErase(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 스킬 지우기
};


#endif