#ifndef _BUG_PLAY_LOG_H_
#define _BUG_PLAY_LOG_H_

// ���� ����
class CCharacter;
class CMonster;

namespace GAMELOG
{
    bool LogDamagePumping(CCharacter& AttackCharacter_In, CMonster& DefendMonster_In, 
        const short nDamage_In, bool bWriteForce_In = false);
};


#endif