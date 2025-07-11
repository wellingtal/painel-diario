
#ifndef _GUILDDB_MGR_H_
#define _GUILDDB_MGR_H_

#pragma once

#include <Community/Guild/GuildMgr.h>


// forward decl.
class CSendStream;


namespace Guild
{
    class CGuildDB;

	class CGuildDBMgr : public CGuildMgr
	{
	public:

        static CGuildDBMgr& GetInstance();

		CGuildDBMgr(void);
		~CGuildDBMgr(void);

		bool Initialize(CDBComponent& DBComponent);

		CGuildDB* CreateGuild(unsigned long dwMasterID, unsigned char cNation, char* szGuildName, unsigned short* wError);
		GuildMap::iterator DissolveGuild(unsigned long dwGID, unsigned short usDissolveReason);

		bool SetRelation(unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelationType);
		void CancelAlertAndCounterHostility();					// �ڽ��� ��� ���� / ī���� ���븦 ���� ���� �� ���踦 ����
		void CancelAlertHostility(unsigned long dwTargetGID);	// dwTargetGID ���� ��� ���� ������ �� ����� ���踦 ����

		void ProcessGuild(void);
        void SendGuildDBList(CSendStream& SendStream);

		void KickAllWaitMember() ;

	private:

		bool ReadGuildInfoDB(void);
		bool ReadGuildMemberDB(void);
		bool ReadGuildRelationDB(void);

        CDBComponent*   m_lpDBComponent;
	};
};

#endif