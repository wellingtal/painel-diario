#ifndef _MONSTER_SHOUT_H_
#define _MONSTER_SHOUT_H_

#include <map>
#include <string>

class CMonsterShout
{
public:

    enum Behavior
    {
        NORMAL_ATTACK       = 0,
        NORMAL_ATTACKED     = 1,

        SKILL_ATTACK        = 2,
        SKILL_ATTACKED      = 3,

        CRITICAL_ATTACK     = 4,
        CRITICAL_ATTACKED   = 5,

        RESPAWN             = 6,
        DEAD                = 7
    };

    struct ChatNode
    {
        ChatNode*       m_lpNextNode;

        int             m_nPercentage;
        unsigned short  m_usChatLength;
        char            m_szMonsterChat[MAX_PATH];
    };

    struct ShoutInfo
    {
        int             m_nKID;
        int             m_nBehavior;
        int             m_nSkill_ID;
        int             m_nChatType;
        int             m_nTotalPercentage;

        ChatNode*       m_lpNextNode;
    };

    static CMonsterShout& GetInstance();

    bool LoadScript(const char* szFileName = ms_DefaultFileName);
    void AddMonsterShout(ShoutInfo& shoutInfo, ChatNode& chatNode);

    Behavior GetBehavior(unsigned short usAttackType, unsigned char cDefenseJudge);
    void Shout(unsigned long dwMonsterCID, unsigned long nKID, 
        unsigned short usXPos, unsigned short usZPos,
        Behavior eBehavior, const char* szName = 0, unsigned short usSkill_ID = 0);

private:

    CMonsterShout();
    ~CMonsterShout();

    static const char* ms_DefaultFileName;    

    // KID / ShoutInfo
    typedef std::multimap<int, ShoutInfo>  ShoutMap;
    
    ShoutMap m_ShoutMap;
};

#endif