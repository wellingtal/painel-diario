#ifndef _GUILD_STRUCTURE_H_
#define _GUILD_STRUCTURE_H_

namespace Guild
{
	struct MemberListInfo
	{
		unsigned char	m_cRank;		// ���� (�� ��峻���� ����ũ�� ����. ��帶���͸� 0���� �Ͽ� ���������� ����)
		unsigned char	m_cTitle;		// ���� (Guild::Title�� ����� ����)
		unsigned char	m_cLevel;		// ����
		unsigned char	m_cClass;		// Ŭ����

		MemberListInfo()
			:	m_cRank(0), m_cTitle(0), m_cLevel(0), m_cClass(0)
		{ }

		MemberListInfo(unsigned char cRank, unsigned char cTitle, unsigned char cLevel, unsigned char cClass)
			:	m_cRank(cRank), m_cTitle(cTitle), m_cLevel(cLevel), m_cClass(cClass)
		{ }
	};

	struct MemberDetailInfo
	{
		unsigned long	m_dwFame;
		unsigned long	m_dwGold;

		MemberDetailInfo()
			:	m_dwFame(0), m_dwGold(0)
		{ }

		MemberDetailInfo(unsigned long dwFame, unsigned long dwGold)
			:	m_dwFame(dwFame), m_dwGold(dwGold)
		{ }
	};

	struct MemberInfo
	{
		// ������ �ʴ� ����
		unsigned long		m_dwCID;
		char				m_strName[MAX_MEMBER_NAME_LEN];

		// ���� ó���Ǵ� ����
		unsigned long		m_dwServerID;
		TIME				m_LeaveGuildTime;
		unsigned char		m_cUpdateCount;

		// ���� ���ϴ� ���� (����� ������ ������Ʈ)
		MemberListInfo		m_MemberListInfo;

		// ���� ���ϴ� ���� (���� Ƚ�� �̻� ����Ǹ� ������Ʈ)
		MemberDetailInfo	m_MemberDetailInfo;

		MemberInfo()
			:	m_dwCID(0), m_dwServerID(0), m_cUpdateCount(0), m_MemberListInfo(), m_MemberDetailInfo()
		{
			std::fill_n(m_strName, int(MAX_MEMBER_NAME_LEN), 0);
		}
	};
};

#endif