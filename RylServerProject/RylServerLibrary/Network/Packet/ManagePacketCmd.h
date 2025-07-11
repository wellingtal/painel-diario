#ifndef _MANAGE_PACKET_COMMAND_H_
#define _MANAGE_PACKET_COMMAND_H_

#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketCommand.h>
#include <ctime>

namespace ServerManage
{
	namespace CMD
	{
		enum CMDType
		{			
			// ManageClient, ManageServer, ManageTool ���� ����ϴ� ��Ŷ Ŀ�ǵ�.						
			UpdateRunList   = 0x00, // ���� ���� �ش� (ManageServer -> ManageClient, ManageTool), Ack����.
			RunProcess      = 0x01,	// �����Ѵ�. (ManageTool -> ManageServer -> ManageClient), Ack�� �ݴ� ������ ����
			QuitProcess     = 0x02, // �����Ѵ�. (ManageTool -> ManageServer -> ManageClient), Ack�� �ݴ� ������ ����
            ForceTerminate  = 0x03, // ���� �����Ѵ�. (ManageTool -> ManageServer -> ManageClient), Ack�� �ݴ� ������ ����
			ProcessStatus   = 0x04, // ���μ��� ���� ���� (ManageClient -> ManageServer -> ManageTool), Ack����.

			RelayCommand    = 0x05, // ��Ŷ Ŀ�ǵ带 ������ �޽����� �Ἥ �߰��ؼ� �ѱ��.
			AutoPatch       = 0x06, // �ڱ� �ڽ��� ��ġ�Ѵ�
			ChangeSetup     = 0x07, // ������ �ٲٰ� �����Ѵ�.
			ReloadSetup     = 0x08, // ������ ��ε��Ѵ�.

            ExecuteCommand  = 0x09, // �ܼ� Ŀ�ǵ带 ���ڿ��� �����Ѵ�.


			// ManageServer <-> ManageTool
			AuthUser            = 0x10, // ���� ����
			AddUser             = 0x11, // ���� �߰�
			DelUser             = 0x12, // ���� ����
			ModUser             = 0x13, // ���� ���� ����
            RequestUserReaction = 0x14, // �������� � �Ǵܿ� ���ؼ� ���Ǹ� ����.(usSubCommand Ȱ��)
			RequestPromote      = 0x15, // ������ �����ڰ� �ǰڴٰ� ����
            ManagerInfo         = 0x16, // ���� ������ ������ ��

			// ManageServer ->  ManageTool
			UserList        = 0x20, // ���� ����Ʈ(���� ����)
			UserLogin       = 0x21, // �������� ���� ���� ������ �ش�. (������ �α�������)
			UserLogout      = 0x22, // �������� ���� ���� ������ �ش�. (������ �α׾ƿ�����)
			StatServerStatus= 0x23, // ��������, ��輭���� ���� ���¸� �ش�.
            
            // ManageClient <-> Each Server
            IPC_ManagePing  = 0x30,  // Each Server -> ManageClient (Alive���� �� Window Name�� �ش�)

            // ManageTool <-> ManageServer
            Ping            = CmdSysPing
		};

        enum RelaySubCmd
        {
            // UserInfo
            SubCmd_ServerUserNum    = 0x01

        };

        enum UserReactionSubCmd
        {
            // Promote
            REQUEST_TAKEBACK_PROMOTE    = 0x01,
            ACK_TAKEBACK_PROMOTE        = 0x02,
            NAK_TAKEBACK_PROMOTE        = 0x03
        };


		enum UserStatCMD
		{
			UPDATE_USER_STATUS		= 0x01
		};
    };

#pragma pack(1)

	// ��Ŷ �ڿ� ������ �̸�, �߰� �����Ͱ� �پ� �´�.
	// ��� ��Ŷ : ExecuteFile, RelayCommand, AutoPatch, 
	//			   ChangeSetup, ReloadSetup
	//
	struct ManageCommand : public PktBase
	{
		enum AutoPatchFlag
		{
			AUTOPATCH_OPENFILE	= 1,
			AUTOPATCH_DATA		= 2			
		};

		unsigned int		nMessage;			// �޽��� ��ȣ
		WPARAM				wParam;				// �Ķ����1
		LPARAM				lParam;				// �Ķ����2

		unsigned long		dwRunID;			// Ÿ�� RunID
        unsigned short      usFlags;            // �߰� ������ �÷���
		unsigned short		usDataLen;			// �߰� ������ ����		
		
	};

	enum ManageCommandError
	{
		INVALID_MANAGE_PACKET_ERROR = 0,
		CANNOT_FIND_CLIENT_DISPATCH = 1,
		SEND_MANAGE_COMMAND_ERROR   = 2,
        UNKNOWN_RUN_INFO            = 3,
        INVALID_MANAGE_RIGHTS       = 4
	};

    enum UpdateRunInfoFlags
    {
        SEND_RUNINFO_START  = 0,
        SEND_RUNINFO_NOW    = 1,
        SEND_RUNINFO_FINISH = 2
    };

    enum ProcessStatusFlags
    {
        PROCESS_RUNNING     = (1 <<  0)        
    };

    struct ProcessStatus
    {
        unsigned long m_dwRunID;
        unsigned long m_dwStatusFlags;
        unsigned long m_dwCustomStatus;
        unsigned long m_dwLastSetWindowHandleTime;
        unsigned long m_dwCurrentTime;

        FILETIME m_CreationTIme;
        FILETIME m_ExitTime;
        FILETIME m_KernelTime;
        FILETIME m_UserTime;
        
        PROCESS_MEMORY_COUNTERS m_ProcessMemoryCounters;
    };


	/*

		��� ��Ŷ : AuthUser, AddUser, DelUser, ModUser, PromoteUser
		AuthUser	:	�� -> ���� (szID, szPassword), 
						���� -> �� (szID, szPassword, szFullName, dwIP, usAdminLevel)
							�ٸ� ���鿡�� ���� �ڽ��� UserInfo ��Ŷ�� ������.
							�α��� �� ����� ������ ������ �޴´�.

		���		:	���� ������ �Ѵ�.
		���� �ڵ�	:	INVALID_PACKET_ERROR,
						AUTHORIZE_FAILED

		AddUser		:	�� -> ���� (szID, szPassword, szFullName, dwIP, usAdminLevel)
						���� -> �� (szID, szPassword, szFullName, dwIP, usAdminLevel)
		���		:	������ �߰��Ѵ�.
		���� �ڵ�	:	

		DelUser		:	�� -> ���� (szID, szPassword, szFullName, dwIP, usAdminLevel)
						���� -> �� (szID, szPassword, szFullName, dwIP, usAdminLevel)
		���		:	������ �����Ѵ�.
		���� �ڵ�	:	

		ModUser		:	�� -> ���� (szID, szPassword, szFullName, dwIP, usAdminLevel)
						���� -> �� (szID, szPassword, szFullName, dwIP, usAdminLevel)
		���		:	���� ������ �����Ѵ�.
		���� �ڵ�	:	


		UserInfo	:	�� -> ���� 
						���� �α��ε� ���� ������ ��û�Ѵ�. �����ʹ� ����.

						���� -> ��
						���� �α��ε� ���� ����ŭ UserInfo����ü�� �ڿ� �پ� �´�.

		���		:	����� ������ �����Ѵ�.
		���� �ڵ�	:	

        
        Promote�� ����

        0. ���� ������, ���� Promote�� ������ ID�� �ش�.

        1. Manager�� ���ų� Master�� Promote

            ������ Promote��Ŷ�� ���� -> ������ Master�ų� Manager�� ������ Ȯ��.
            -> Ȯ�� ������ �� id�� Promote ������ �ش�.
            -> �� �����鿡�� ���� Promote���������� ������ �ش�.

        2. General�� Promote

            ������ Promote��Ŷ�� ���� -> ������ General���� Ȯ��
            -> Ȯ�� ������, ���� Promote ������ �ִ� ������ �ִ��� ���ǰ� Ȯ�� ��Ŷ ����
            -> Ȯ�� ��Ŷ����, ok�� ����, ������ �ѱ��, �� �����鿡�� Promote�������� �ش�.
            -> Ȯ�� ��Ŷ����, no�� ����, Promote��û�ڿ��� ���� �޽����� �ش�.

        �ʿ� ��Ŷ

        1. ManageUserInfo       (ManageServer -> ManageTool) ManagerID�� ����
        2. Request Promote      (ManageTool -> ManageServer) ���� ����
        3. Request Promote Ack  (ManageServer -> ManageTool) ���� ����

        4. Request User Reaction     (ManageServer -> ManageTool) �䱸�� ����, �䱸 Ÿ��(usSubCommand)
        5. Request User Reaction Ack (ManageTool -> ManageServer) �䱸�� ����, �䱸 ���(usSubCommand)
*/

	enum UserCommandError
	{
		NO_USER_COMMAND_ERROR               = 0,
		INVALID_USER_PACKET_ERROR           = 1,
		AUTHORIZE_FAILED                    = 2,
		CANNOT_AUTHORIZE_NOT_USER           = 3,
		CANNOT_AUTHORIZE_INVALID_PASSWORD   = 4,
		CANNOT_AUTHORIZE_INVALID_IPADDRESS  = 5,
		ALREADY_LOGINED                     = 6,
		ADD_USER_FAILED                     = 7,
		DELETE_USER_FAILED                  = 8,
		MODIFY_USER_FAILED                  = 9,
		PROMOTE_USER_FAILED                 = 10,
        PROMOTE_TAKEBACK_FAILED             = 11,
        REJECTED_PROMOTE_USER               = 12,
		SEND_USER_LIST_FAILED               = 13,

        INVALID_USERMANAGE_RIGHTS           = 14
	};

	struct UserInfo
	{
		enum Const
		{
			// edith 2008.03.17 ID,PASS ��������
			ID_LEN		= 16,
			PASS_LEN	= 16,
			NAME_LEN	= 32
		};

		TCHAR szFullName[NAME_LEN];
		TCHAR szID[ID_LEN];
		TCHAR szPassword[PASS_LEN];
		
		unsigned long		dwIP;
		unsigned short		usAdminLevel;
		unsigned short		usSubCommand;
	};

	struct UserCommand : public PktBase
	{
		unsigned char		cUserInfoNum;		
		// �ڿ� UserInfo����ü�� cUserInfoNum�� ��ŭ �ٴ´�.
	};


	/*
		Relay Command�� ���� Ŀ�ǵ�
		���� ����
	*/

	struct RunInfo
	{
		enum Const
		{
			MAX_SERVER_NAME = 64,
			MAX_PATH_LEN	= 260,
			MAX_OPT_LEN		= 260
		};

		unsigned long	m_dwRunID;		// RunID(primary key)
		unsigned long	m_dwServerIP;	// (network order ip)
		unsigned long	m_dwPathID;		// PathID
		unsigned long	m_dwOptionID;	// OptionID		
		char			m_szServerName[MAX_SERVER_NAME];
		char			m_szPath[MAX_PATH_LEN];
		char			m_szOption[MAX_OPT_LEN];
	};

    struct PktManagePacket : public PktBase
    {
        unsigned long m_dwPID;
        unsigned long m_dwStatusFlag;
        unsigned long m_dwSubCommand;
        
        PktManagePacket() : m_dwPID(GetCurrentProcessId()), m_dwStatusFlag(0), m_dwSubCommand(0) { }
    };

    struct PktManagePing : public PktManagePacket
    {
        char m_szAppFullPathName[MAX_PATH];
        char m_szWindowName[MAX_PATH];
        char m_szCommandLine[MAX_PATH * 2];
    };

    struct UserNumPair
    {
        unsigned long   m_dwServerID;
        int             m_nUserNum;
    };
    
    struct PktManageUserNum : public PktManagePacket
    {
        unsigned long m_dwUserInfoNum;
    };

	struct PktUserStat : public PktBase
	{
		enum SIZE
		{
			MAX_DATE = 20
		};

		//time_t			m_nTime;
		char			m_szSendingTime[MAX_DATE];
		unsigned short	m_usUserStatDataNum;
	};

	struct UserStatData
	{
		unsigned int	m_nNation;		// ���� �ڵ�(AdminToolServer.ini ����)
		unsigned long   m_dwServerID;	// ������ �����ؿ� ���� ID
		unsigned int	m_nUserNum;		// ���� ���� ��
	};

	struct StatServerStatus : public PktBase
	{
		bool	m_bGlobalStatServerOK;	// �۷ι� ��輭���� �������� ���� ����
		bool	m_bLocalStatServerOK;	// ���� ��輭���� �������� ���� ����
	};

#pragma pack()

};

#endif