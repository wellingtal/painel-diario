
#ifndef _GUILD_MGR_H_
#define _GUILD_MGR_H_

#pragma once

#include <string>
#include <Community/Guild/GuildStructure.h>

// 전방 참조
struct GuildCheckSumNode;
struct GuildLargeInfoNode;

class CCharacter;

namespace Guild
{
	// 전방 참조
	class CGuild;

	class CGuildMgr
	{
	public:

        static CGuildMgr& GetInstance();

		CGuildMgr(void);
		~CGuildMgr(void);

		bool Initialize(void);
		void Destroy(void);

		bool SerializeIn(char* lpBuffer_In, unsigned short wBufferSize_In, 
						unsigned char cTotalMemberNum, unsigned char cRelationByMineNum, unsigned char cRelationByTargetNum);

		bool CreateGuild(unsigned long dwMasterID, unsigned long dwGuildID, unsigned char cInclination, char* szGuildName);
		bool DissolveGuild(unsigned long dwGID);
		CGuild* GetGuild(unsigned long dwGID);
		CGuild* GetGuild(char* szName);

		bool SetRelation(unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelationType);

		void ProcessGuild(void);
		bool SendGuildList(unsigned long dwCID, unsigned char cSortCmd, unsigned char cPage, 
			unsigned char cNum, GuildCheckSumNode* lpNode);
		bool SendGuildRelationInfo(unsigned long dwCID, unsigned long dwGID);

		bool GetMemberList(unsigned long dwGID, const MemberList* pMemberList);

	protected:

		bool GetSortingPageList(CCharacter* lpCharacter, unsigned char cSortCmd, unsigned char cPage, 
			GuildLargeInfoNode* aryCurrentInfoList);

		bool SerializeOut(CGuild* lpGuild_In, char* lpBuffer_Out, unsigned short& wBufferSize_Out, 
			unsigned char& cTotalMemberNum, unsigned char& cRelationByMineNum, unsigned char& cRelationByTargetNum);


		GuildMap		m_GuildMap;			// GID를 이용한 검색용
		GuildNameMap	m_GuildNameMap;		// 길드명 순으로 소트용


	private:

		static CGuildMgr ms_this;
	};
};

#endif