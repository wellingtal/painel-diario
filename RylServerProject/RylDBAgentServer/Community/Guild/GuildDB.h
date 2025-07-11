
#ifndef _RYL_DBAGENT_GUILDDB_H_
#define _RYL_DBAGENT_GUILDDB_H_

#pragma once

#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Community/Guild/Guild.h>


// forward decl.
class CDBComponent;
class CSendStream;

namespace Guild
{
	class CGuildDB : public CGuild
	{
	private:

		// 길드 클래스는 길드 매니저에서만 생성 가능 (삭제는 유틸을 이용하는 관계로 public)
		CGuildDB(CDBComponent& DBComponent);
		CGuildDB(CDBComponent& DBComponent, unsigned long dwGID, unsigned char cNation, char* szName);
		CGuildDB(CDBComponent& DBComponent, GuildInfoDB& guildInfo);

		friend class CGuildDBMgr;

        CDBComponent& m_DBComponent;

	public:

		~CGuildDB(void);

		bool SetMark(char* szMark);
		bool SetLevel(unsigned char cLevel);
//		bool SetInclination(unsigned char cInclination);
		bool SetRight(GuildRight guildRight);
		bool SetTitle(unsigned long dwCID, unsigned char cTitle);
		bool SetRelation(unsigned long dwTargetGID, unsigned char cRelationType, unsigned char cActorType=Guild::BY_MINE, unsigned char cInsert = 1);
		void DeleteRelation();
		void CancelAlertAndCounterHostility();
		void CancelAlertHostility(unsigned long dwTargetGID);

		unsigned long ReleaseGold(unsigned long dwGold);
		unsigned long SetNewMaster(void);

		int	 GetLoginedTotalMemberNum() const;
		int  GetLoginedCurrentMemberNum() const;		

		void AddAllMemberFame(unsigned char cType, int nFame, Guild::CGuildDB* lpEnemyGuild = 0);						// 로그인 유무 상관없이 명성치 변화 ( 음수값 사용가능 )
		void AddLoginedMemberFame(unsigned char cType, int nFame, Guild::CGuildDB* lpEnemyGuild = 0);					// 로그인한 길드원 명성치 변화      ( 음수값 사용가능 )
		void AddMemberFame(unsigned char cType, unsigned long dwCID, int nFame);	// 로그인 유무 상관없이 명성치 변화 ( 음수값 사용가능 )
		void AddGoldToMaster(unsigned long dwGold);									// 마스터가 로그아웃한 경우 돈을 돌려주기 위한 함수

		bool JoinMember(MemberInfo& memberInfo);
		bool LeaveMember(unsigned long dwCID);
		bool UpdateMemberInfo(unsigned long dwCID, MemberListInfo memberListInfo, MemberDetailInfo memberDetailInfo);
		void KickAllWaitMember() ;

		void SendLoginOutMember(unsigned long dwCID, unsigned long dwServerID);
		void SendJoinMember(CSendStream* lpSendStream, unsigned long dwCID, unsigned char cTitle);

		unsigned char GetTacticsMember(unsigned long dwCID);

		void SetTacticsMember(unsigned long dwCID, unsigned char cTactics);

		void SendJoinTacticsMember(CSendStream* lpSendStream, unsigned long dwCID, unsigned char cTitle, unsigned char CType);		
	};
};

#endif