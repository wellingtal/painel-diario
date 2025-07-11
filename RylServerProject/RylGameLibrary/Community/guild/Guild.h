
#ifndef _GUILD_H_
#define _GUILD_H_

#pragma once

#include <map>
#include <vector>

#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Community/Guild/GuildConstants.h>


// ���� ����
class CAggresiveCreature;
class CCharacter;
class CCell;
class CSession;

namespace Guild
{
    // ��� �̸��� �������� ���ڰ� �ִ��� ���ɴ�.
    bool CheckGuildName(const char* szGuildName);

	class CGuild
	{
	protected:

		// ��� Ŭ������ ��� �Ŵ��������� ���� ���� (������ ��ƿ�� �̿��ϴ� ����� public)
		CGuild(void);
		CGuild(unsigned long dwGID, unsigned char cNation, char* szName);
		CGuild(GuildInfoDB& guildInfo);

		friend class CGuildMgr;

	public:

		~CGuild(void);

	public:
		
		// Ŭ���̾�Ʈ���� ������ �޾� üũ�ϰ� �߰� ������ �����ش�.
		// (TODO : ���� �������� �ʿ�� �ѹ������ش�. �ϴ��� ��� SERVER_ERROR.)
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

		// �߰� �����κ��� ������ �޾� ó���ϰ� Ŭ���̾�Ʈ���� �˷��ش�.
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

		const unsigned char GetCurrentAllMember(void) const;	// ��� ���.
		const unsigned char GetTotalMemberNum(void) const;		// �뺴�� ������ ��� ���
		const unsigned char GetCurrentMemberNum(void) const;	// ���Դ����, �뺴�� ������ ���
		const unsigned char GetMemberNum(void) const;			// �뺴�� ������ �����
		const unsigned char GetTacticsNum(void) const;			// �뺴 ��


		const float GetGuildBonus(unsigned char cRealmPoint, float fPercent = 5.0f);	// ��� ���ʽ� ���� �Լ�

		const unsigned char GetLoginMemberNum(void) const;		// �������� ������
		const unsigned char GetMaxMemberNum(void) const;		// �ִ� ������

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
		// Vincent - GetRelationGID() �Լ��� ���� ������ 1�� �̻� �Ҽ� ������쿡�� ����Ǿ���Ѵ�.!!

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

		// ��� ���� �޼ҵ�
		void SetGuildSafe(char cGuildSafe) { m_cGuildSafe = cGuildSafe; }
		char GetGuildSafe() { return m_cGuildSafe; }

	protected:

		bool InsertRelation(unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelation);

		void CalculateGuildFame(void);

		// ------------------------------------------------------------------
		// ��� ����
		char			m_cGuildSafe;

		unsigned long	m_dwGID;					// ��� ���̵�

		char	m_strName[MAX_GUILD_NAME_LEN];		// ��� �̸�
		char	m_szMark[MAX_MARK_SIZE];			// ��� ��ũ

		unsigned char	m_cInclination;				// ��� ���� (Guild::Inclination ����)
		unsigned char	m_cLevel;					// ��� ����

		unsigned long	m_dwFame;					// ��� ��
		unsigned long	m_dwGold;					// ��� �ݰ�

		unsigned char	m_cGuildPointFlag;			// ��� ������ ����.

		TIME			m_tmLastLogout;				/* ���� ���� �α׾ƿ� �ð� 
													(���� �ð����� ������ �ƹ��� �α������� ������ ��尡 �����ȴ�.) */

		TIME			m_tmCheckMember;			// ���� ���� ���� �ο� �̸����� ������ ��尡 �����Ǳ������ �ð�

		TIME			m_tmGMLastLogout;			/* ��� �������� �α׾ƿ� �ð� 
													(���� �ð����� ��� �����Ͱ� �α������� ������ ���� ������ ������ ��� �����Ͱ� �ȴ�.) */

		TIME			m_tmChangeInclination;		// ��� ������ ������ �ð� (24�ð� �̳����� �ٽ� ������ �� ����.)

		TIME			m_tmGuildPoint;				// ��� ������ ���� �ð�.		

		GuildRight		m_GuildRight;				// ��� ����

		MemberList		m_MemberList;				// ����

		RelationMap		m_Relation[Guild::MAX_RELATION_ACTOR];		// ���� ���� ��� ���� / �ٸ� ��忡 ���� �ξ��� ��� ����

		unsigned long	m_dwGuildMiles;
	};
};

#endif