
#ifndef _GUILD_H_
#define _GUILD_H_

#pragma once

#include <map>
#include <vector>

#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Community/Guild/GuildConstants.h>


// 전방 참조
class CAggresiveCreature;
class CCharacter;
class CCell;
class CSession;

namespace Guild
{
    // 길드 이름에 부적절한 글자가 있는지 살핀다.
    bool CheckGuildName(const char* szGuildName);

	class CGuild
	{
	protected:

		// 길드 클래스는 길드 매니저에서만 생성 가능 (삭제는 유틸을 이용하는 관계로 public)
		CGuild(void);
		CGuild(unsigned long dwGID, unsigned char cNation, char* szName);
		CGuild(GuildInfoDB& guildInfo);

		friend class CGuildMgr;

	public:

		~CGuild(void);

	public:
		
		// 클라이언트에게 정보를 받아 체크하고 중계 서버로 보내준다.
		// (TODO : 각종 에러들은 필요시 넘버링해준다. 일단은 모두 SERVER_ERROR.)
		unsigned short SetMark(unsigned long dwSenderID, char *szMark);
		bool SetLevel(unsigned long dwMaster, unsigned char cLevel);
		bool SetRight(unsigned long dwMaster, GuildRight guildRight);
		bool SetTitle(unsigned long dwSuperior, unsigned long dwFollower, unsigned char cTitle, unsigned short &wError);

		bool JoinMember(unsigned long dwCID, unsigned char cTitle, unsigned short &wError);
		bool JoinMemberDB(GuildMemberDB& guildMemberDB);
		virtual bool JoinMember(MemberInfo& memberInfo);

		bool TacticsMember(unsigned long dwCID, unsigned char cTitle, unsigned char cType, unsigned short &wError);

		bool SetTacticsTitle(unsigned long dwSuperior, unsigned long dwFollower, unsigned short wCmd, unsigned char cTitle, unsigned char cType, unsigned short &wError);		

		bool SetTacticsTitle(unsigned long dwCID, unsigned char cTitle);

		bool IsMember(unsigned long dwCID);
		bool IsTacticsWaitMember(unsigned long dwCID);

		bool KickMember(unsigned long dwSuperior, unsigned long dwFollower, unsigned short &wError);
		
		bool KickTacticsMember(unsigned long dwSuperior, unsigned long dwFollower, unsigned short wCmd, unsigned short &wError);

		bool LeaveTacticsMember(unsigned long dwCID, unsigned long dwReferenceID, unsigned short wCmd);

		bool UpdateMemberInfo(unsigned long dwCID, unsigned long dwValue, unsigned char cCmd);

		// 중계 서버로부터 정보를 받아 처리하고 클라이언트에게 알려준다.
		unsigned short SetMark(unsigned long dwSenderID, char *szMark, unsigned long dwGold);
		bool SetLevel(unsigned char cLevel, unsigned long dwGold);
		bool SetRight(GuildRight guildRight);
		virtual bool SetTitle(unsigned long dwCID, unsigned char cTitle);
		bool SetRelation(unsigned long dwTargetGID, unsigned char cRelationType, unsigned char cActorType=Guild::BY_MINE);

		bool UpdateMemberInfo(unsigned long dwCID, MemberListInfo memberListInfo, MemberDetailInfo memberDetailInfo);

		// -------------------------------------------------------------------------------------------

		MemberInfo GetMaster(void);

		unsigned char GetTitle(unsigned long dwCID);

		const unsigned long GetGID(void)	{ return m_dwGID;			}
		const char* GetName(void)			{ return m_strName;			}
		const unsigned long GetFame(void)	{ return m_dwFame;			}
		const GuildRight& GetRight(void)	{ return m_GuildRight;		}
		const unsigned long GetGold(void)	{ return m_dwGold;			}

		const unsigned char GetCurrentAllMember(void) const;	// 모든 멤버.
		const unsigned char GetTotalMemberNum(void) const;		// 용병을 포함한 모든 멤버
		const unsigned char GetCurrentMemberNum(void) const;	// 가입대기자, 용병을 제외한 멤버
		const unsigned char GetMemberNum(void) const;			// 용병을 제외한 멤버수
		const unsigned char GetTacticsNum(void) const;			// 용병 수


		const float GetGuildBonus(unsigned char cRealmPoint, float fPercent = 5.0f);	// 길드 보너스 관련 함수

		const unsigned char GetLoginMemberNum(void) const;		// 게임중인 길드원수
		const unsigned char GetMaxMemberNum(void) const;		// 최대 길드원수

		const unsigned char GetNation(void) const { return m_cInclination;	}
		const bool IsPeaceMode(void);

		void SetGuildMiles(unsigned long dwGuildMiles);
		const unsigned long GetGuildMiles() const { return m_dwGuildMiles; }


		bool AddGold(unsigned long dwGold);
		bool DeductGold(unsigned long dwGold);
		void ReleaseGold(unsigned long dwGold);

		bool AutoRouting(CAggresiveCreature* lpPickkingCreature, Item::CItem* lpItem, unsigned long& dwGold);
		int  GetNearMemberList(CCell *pCell, CCharacter **aryNearCharacterList);

		void UpgradeMemberRespawnSpeedByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep);
		void DegradeMemberRespawnSpeedByEmblem();

		unsigned char	GetRelation(unsigned long dwGID, unsigned char cActorType=Guild::BY_MINE);
		int				GetNumOfRelation(unsigned char cRelationType, unsigned char cActorType=Guild::BY_MINE);
        unsigned long	GetRelationGID(unsigned char cRelationType, unsigned char cActorType=Guild::BY_MINE);
		// Vincent - GetRelationGID() 함수는 적대 선언을 1개 이상 할수 있을경우에는 변경되어야한다.!!

		bool IsEnemyGuild(unsigned long dwGID);

		bool InviteMember(unsigned long dwMember, unsigned long dwGuest, unsigned short &wError);
		bool LeaveMember(unsigned long dwCID);
		bool LogInOutMember(unsigned long dwCID, unsigned long dwServerID);

		const GuildLargeInfoNode GetLargeInfo(unsigned char cIndexOfPage, unsigned short wRank, CGuild* lpRelationGuild);

		bool SendGuildSafe(unsigned long dwCID, char* szCharName, unsigned char cCmd);
		bool SendMemberList(CCharacter* lpCharacter, unsigned char cMemberType, unsigned char cSortCmd, unsigned char cPage);
		void SendAllMember(const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In);
		void SendCurrentMember(const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In);
		bool SendHostilityList(CCharacter* lpCharacter, unsigned char cCurrentPage, unsigned char cPageState);

        void ChangeMemberName(unsigned long dwCID, const char* szChangeName);

		TIME* GetGuildPointTime(void) { return &m_tmGuildPoint;	}
		unsigned char GetGuildPointFlag(void) { return m_cGuildPointFlag; }
		void SetGuildPointFlag(unsigned char cFlag) { m_cGuildPointFlag = cFlag; }

		// 길드 관련 메소드
		void SetGuildSafe(char cGuildSafe) { m_cGuildSafe = cGuildSafe; }
		char GetGuildSafe() { return m_cGuildSafe; }

	protected:

		bool InsertRelation(unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelation);

		void CalculateGuildFame(void);

		// ------------------------------------------------------------------
		// 멤버 변수
		char			m_cGuildSafe;

		unsigned long	m_dwGID;					// 길드 아이디

		char	m_strName[MAX_GUILD_NAME_LEN];		// 길드 이름
		char	m_szMark[MAX_MARK_SIZE];			// 길드 마크

		unsigned char	m_cInclination;				// 길드 성향 (Guild::Inclination 참고)
		unsigned char	m_cLevel;					// 길드 레벨

		unsigned long	m_dwFame;					// 길드 명성
		unsigned long	m_dwGold;					// 길드 금고

		unsigned char	m_cGuildPointFlag;			// 길드 재정비 관련.

		TIME			m_tmLastLogout;				/* 최종 길드원 로그아웃 시간 
													(일정 시간동안 길드원이 아무도 로그인하지 않으면 길드가 삭제된다.) */

		TIME			m_tmCheckMember;			// 길드원 수가 일정 인원 미만으로 떨어져 길드가 삭제되기까지의 시간

		TIME			m_tmGMLastLogout;			/* 길드 마스터의 로그아웃 시간 
													(일정 시간동안 길드 마스터가 로그인하지 않으면 다음 서열의 길드원이 길드 마스터가 된다.) */

		TIME			m_tmChangeInclination;		// 길드 성향을 변경한 시간 (24시간 이내에는 다시 변경할 수 없다.)

		TIME			m_tmGuildPoint;				// 길드 재정비 관련 시간.		

		GuildRight		m_GuildRight;				// 길드 권한

		MemberList		m_MemberList;				// 길드원

		RelationMap		m_Relation[Guild::MAX_RELATION_ACTOR];		// 내가 맺은 길드 관계 / 다른 길드에 의해 맺어진 길드 관계

		unsigned long	m_dwGuildMiles;
	};
};

#endif