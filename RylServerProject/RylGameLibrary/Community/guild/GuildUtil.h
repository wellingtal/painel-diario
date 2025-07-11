
#ifndef _GUILD_UTIL_H_
#define _GUILD_UTIL_H_

#include "GuildMgr.h"
#include "Guild.h"

namespace Guild
{
	struct FnDeleteSecond
	{   
		template<typename PairType>
		bool operator() (PairType& pair) 
		{ 
			if (NULL != pair.second) 
			{ 
				delete pair.second; 
			} 

			return true; 
		}
	};

	struct IsCurrentMember
	{
		bool operator () (const MemberInfo& first) const
		{
			unsigned char cTitle = first.m_MemberListInfo.m_cTitle;
			return (JOIN_WAIT != cTitle && 0 == first.m_cTactics);
		}
	};

	struct IsCurrentLoginMember
	{
		bool operator () (const MemberInfo& first) const
		{
			return (0 != first.m_dwServerID);
		}
	};

	struct IsTacticsMember
	{
		bool operator () (const MemberInfo& first) const
		{
			return (0 != first.m_cTactics);
		}
	};

	struct IsNotTacticsMember
	{
		bool operator () (const MemberInfo& first) const
		{
			return (0 == first.m_cTactics);
		}
	};

	struct SumMemberFame
	{
		unsigned long operator () (const unsigned long init, const MemberInfo& first) const
		{
			return ((true == IsCurrentMember()(first)) ? init + first.m_MemberDetailInfo.m_dwFame : init); 
		}
	};

	struct CompareGuildFame
	{
		bool operator () (const GuildMap::value_type& lhs, const GuildMap::value_type& rhs) const
		{
			return lhs.second->GetFame() > rhs.second->GetFame(); 
		}
	};

	struct CompareMemberName
	{
		bool operator () (const MemberInfo& lhs, const MemberInfo& rhs) const
		{
			return std::string(lhs.m_strName) < std::string(rhs.m_strName); 
		}
	};

	struct CompareMemberTitle
	{
		bool operator () (const MemberInfo& lhs, const MemberInfo& rhs) const
		{
			if (lhs.m_MemberListInfo.m_cTitle == rhs.m_MemberListInfo.m_cTitle)
			{
				return CompareMemberName()(lhs, rhs);
			}

			return lhs.m_MemberListInfo.m_cTitle < rhs.m_MemberListInfo.m_cTitle; 
		}
	};

	struct CompareMemberClass
	{
		bool operator () (const MemberInfo& lhs, const MemberInfo& rhs) const
		{
			if (lhs.m_MemberListInfo.m_cClass == rhs.m_MemberListInfo.m_cClass)
			{
				return CompareMemberName()(lhs, rhs);
			}

			return lhs.m_MemberListInfo.m_cClass < rhs.m_MemberListInfo.m_cClass; 
		}
	};

	struct CompareMemberLevel
	{
		bool operator () (const MemberInfo& lhs, const MemberInfo& rhs) const
		{
			if (lhs.m_MemberListInfo.m_cLevel == rhs.m_MemberListInfo.m_cLevel)
			{
				return CompareMemberName()(lhs, rhs);
			}

			return lhs.m_MemberListInfo.m_cLevel > rhs.m_MemberListInfo.m_cLevel; 
		}
	};

	struct CompareMemberFame
	{
		bool operator () (const MemberInfo& lhs, const MemberInfo& rhs) const
		{
			if (lhs.m_MemberDetailInfo.m_dwFame == rhs.m_MemberDetailInfo.m_dwFame)
			{
				return CompareMemberName()(lhs, rhs);
			}

			return lhs.m_MemberDetailInfo.m_dwFame > rhs.m_MemberDetailInfo.m_dwFame; 
		}
	};

	struct CompareMemberGold
	{
		bool operator () (const MemberInfo& lhs, const MemberInfo& rhs) const
		{
			if (lhs.m_MemberDetailInfo.m_dwGold == rhs.m_MemberDetailInfo.m_dwGold)
			{
				return CompareMemberName()(lhs, rhs);
			}

			return lhs.m_MemberDetailInfo.m_dwGold > rhs.m_MemberDetailInfo.m_dwGold; 
		}
	};

	struct CompareMemberPosition
	{
		bool operator () (const MemberInfo& lhs, const MemberInfo& rhs) const
		{
			if (lhs.m_dwServerID == rhs.m_dwServerID)
			{
				return CompareMemberName()(lhs, rhs);
			}

			return lhs.m_dwServerID < rhs.m_dwServerID; 
		}
	};

	struct CompareRelation
	{
		bool operator () (const GuildHostilityInfoNode& lhs, const GuildHostilityInfoNode& rhs) const
		{
			if (lhs.m_cRelationType == rhs.m_cRelationType)
			{
				return std::string(lhs.m_szName) < std::string(rhs.m_szName); 
			}

			return lhs.m_cRelationType > rhs.m_cRelationType;
		}
	};
}


#endif