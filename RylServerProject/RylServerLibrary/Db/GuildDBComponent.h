#ifndef _RYL_GUILD_DB_COMPONENT_H_
#define _RYL_GUILD_DB_COMPONENT_H_

// forward decl.
class CDBComponent;

namespace DBComponent
{
    namespace GuildDB
    {
        // --------------------------------------------------------------------------------------------
        // 길드 관련 - GuildDBComponent.cpp --------------------------------------------------------------

        bool InsertGuildInfo(CDBComponent& DBComponent, char* szGuildName, unsigned char cNation, char* szRight, unsigned short wSize, unsigned long *dwGID, unsigned short *wError);
        bool InsertGuildMember(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwCID, unsigned char cTitle, unsigned char cRank);
        bool InsertGuildRelation(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelationType);
        bool DeleteGuildInfo(CDBComponent& DBComponent, unsigned long dwGID);
        bool DeleteGuildMember(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwCID, unsigned char cRank);
        bool DeleteGuildRelation(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwTargetGID);		// 두 길드 간의 관계를 삭제
        bool DeleteGuildRelation(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelation);		// 두 길드 간의 관계를 삭제
        bool DeleteGuildRelation(CDBComponent& DBComponent, unsigned long dwGID);									// 해당 길드와 관계가 있는 모든 길드 관계를 삭제
        bool UpdateMemberTitle(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cTitle, SYSTEMTIME UpdateTime);
        bool UpdateMemberRank(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwCID, unsigned char cBeforeRank, unsigned char cAfterRank);
        bool UpdateGuildRelation(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelationType);
        bool UpdateGuildTime(CDBComponent& DBComponent, unsigned long dwGID, unsigned char cType, SYSTEMTIME UpdateTime);
        bool UpdateGuildLevel(CDBComponent& DBComponent, unsigned long dwGID, unsigned char cLevel, unsigned long dwGold);
        bool UpdateGuildInclination(CDBComponent& DBComponent, unsigned long dwGID, unsigned char cInclination);
        bool UpdateGuildRight(CDBComponent& DBComponent, unsigned long dwGID, char* szRight, unsigned short wSize);
        bool UpdateGuildMark(CDBComponent& DBComponent, unsigned long dwGID, char* szMark, unsigned short wSize, unsigned long dwGold);
        bool UpdateGuildGold(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwGold);
        bool UpdateGuildFame(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwFame);
        bool ReleaseGold(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwGold);
		bool UpdateGuildMemberTactics(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwCID, unsigned char cTactics);
    }
}


#endif
