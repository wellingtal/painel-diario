#ifndef _CHECK_SPEEDHACK_H_
#define _CHECK_SPEEDHACK_H_

#include <vector>
#include <deque>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>

// 전방 참조
struct  PktMU;
struct  PktAt;
class   CCharacter;

namespace Skill
{
    // 전방 참조
    struct ProtoType;
};

// 스피드핵
class CSpeedHackCheck 
{
public:

	CSpeedHackCheck();
    
	bool CheckTimeUpdate(DWORD dwClientTime);
    bool CheckAttackReplay(PktAt* lpPktAt);

	inline void SetCharacter(CCharacter* lpCharacter) { m_lpCharacter = lpCharacter; }

private:

    struct CoolDownInfo
    {        
        const Skill::ProtoType* m_lpSkillProtoType;
        AtType                  m_attackType;
        unsigned long           m_dwServerLastCastingTime;
        unsigned long           m_dwClientLastCastingTime;
        unsigned long           m_dwSkillUsedCount;

        CoolDownInfo(const Skill::ProtoType* lpSkillProtoType, AtType attackType,
            unsigned long dwServerLastCastingTime, unsigned long dwClientLastCastingTime)
            :   m_lpSkillProtoType(lpSkillProtoType), m_attackType(attackType),
                m_dwServerLastCastingTime(dwServerLastCastingTime),
                m_dwClientLastCastingTime(dwClientLastCastingTime),
                m_dwSkillUsedCount(1)
        {

        }

        friend inline bool operator < (const CoolDownInfo& lhs, unsigned short usSkill_ID) { return lhs.m_attackType.m_wType < usSkill_ID; }
        friend inline bool operator < (unsigned short usSkill_ID, const CoolDownInfo& rhs) { return usSkill_ID < rhs.m_attackType.m_wType; }
        friend inline bool operator < (const CoolDownInfo& lhs, const CoolDownInfo& rhs) { return lhs.m_attackType.m_wType < rhs.m_attackType.m_wType; }
    };

    struct SkillHistory
    {
        AtType  m_attackType;
        long    m_nServerInterval;
        long    m_nClientInterval;
        long    m_nMinCoolDownTime;

        SkillHistory(AtType attackType, long nMinCoolDownTime, 
            long nServerInterval, long nClientInterval)
            :   m_attackType(attackType), m_nMinCoolDownTime(nMinCoolDownTime), 
                m_nServerInterval(nServerInterval), m_nClientInterval(nClientInterval)
        {
            
        }
    };

    struct AttackTime
    {
        long    m_nDiffServerInterval;
        long    m_nDiffClientInterval;

        AttackTime() : m_nDiffServerInterval(0), m_nDiffClientInterval(0) { }

        AttackTime(long nDiffServerInterval, long nDiffClientInterval)
            :   m_nDiffServerInterval(nDiffServerInterval), m_nDiffClientInterval(nDiffClientInterval)
        {

        }
    };

    struct SumAttackTime
    {
        long   m_nSumServerAttackTime;
        long   m_nSumClientAttackTime;

        SumAttackTime() : m_nSumServerAttackTime(0), m_nSumClientAttackTime(0) { }
        void clear() { m_nSumServerAttackTime = m_nSumClientAttackTime = 0; }

        void add(long nSumServerAttackTime, long nSumClientAttackTime) { m_nSumServerAttackTime += nSumServerAttackTime; m_nSumClientAttackTime += nSumClientAttackTime; }
        void add(AttackTime& atTime) { m_nSumServerAttackTime += atTime.m_nDiffServerInterval; m_nSumClientAttackTime += atTime.m_nDiffClientInterval; }

        void minus(long nSumServerAttackTime, long nSumClientAttackTime) { m_nSumServerAttackTime -= nSumServerAttackTime; m_nSumClientAttackTime -= nSumClientAttackTime; }
        void minus(AttackTime& atTime) { m_nSumServerAttackTime -= atTime.m_nDiffServerInterval; m_nSumClientAttackTime -= atTime.m_nDiffClientInterval; }
    };

    struct ClearAttackHackCheck;

	bool CheckSpeedHack(unsigned long CurrentServer, unsigned long CurrentClient);
    bool CheckAttackHack(PktAt* lpPktAt, unsigned long dwCurrentServerTime);
	
    void LogAttackHack(const char* szDetailText);
    
    void LogSkillCoolDownTime(const CoolDownInfo& coolDownInfo, long nMinCoolDownTime,
        long nServerCoolDownTimeInterval, long nClientCoolDownTimeInterval);

	unsigned long   m_dwServerTotal;
	unsigned long   m_dwServerLast;
	unsigned long   m_dwClientTotal;
	unsigned long   m_dwClinetLast;
	unsigned long   m_dwHzSpeedHack;//hz was not here speed hack check

    unsigned long   m_dwLastServerAttackTime;       // 서버가 가장 마지막으로 받은 공격 패킷 시간
    unsigned long   m_dwLastClientAttackTime;       // 클라이언트가 가장 마지막으로 보낸 공격 패킷 시간
    
    unsigned long   m_dwHackFoundCount;             // 핵 발견 회수

    typedef std::vector<CoolDownInfo>   CoolDownInfoList;    
    typedef std::deque<SkillHistory>    SkillHistoryList;
    typedef std::deque<AttackTime>      AttackTimeList;
    
    SumAttackTime       m_SumAttackTime;        // 공격 시간 합.
    AttackTimeList      m_AttackTimeList;       // 서버 공격 딜레이
    CoolDownInfoList    m_CoolDownInfo;         // 쿨다운 정보.          
    SkillHistoryList    m_SkillHistoryList;     // 스킬 사용 히스토리 리스트
	CCharacter*         m_lpCharacter;
};

#endif