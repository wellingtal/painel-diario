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
			// ManageClient, ManageServer, ManageTool 전부 사용하는 패킷 커맨드.						
			UpdateRunList   = 0x00, // 실행 목룍을 준다 (ManageServer -> ManageClient, ManageTool), Ack없음.
			RunProcess      = 0x01,	// 실행한다. (ManageTool -> ManageServer -> ManageClient), Ack는 반대 순서로 전송
			QuitProcess     = 0x02, // 종료한다. (ManageTool -> ManageServer -> ManageClient), Ack는 반대 순서로 전송
            ForceTerminate  = 0x03, // 강제 종료한다. (ManageTool -> ManageServer -> ManageClient), Ack는 반대 순서로 전송
			ProcessStatus   = 0x04, // 프로세스 상태 전송 (ManageClient -> ManageServer -> ManageTool), Ack없음.

			RelayCommand    = 0x05, // 패킷 커맨드를 윈도우 메시지를 써서 중계해서 넘긴다.
			AutoPatch       = 0x06, // 자기 자신을 패치한다
			ChangeSetup     = 0x07, // 설정을 바꾸고 저장한다.
			ReloadSetup     = 0x08, // 설정을 재로드한다.

            ExecuteCommand  = 0x09, // 콘솔 커맨드를 문자열로 전달한다.


			// ManageServer <-> ManageTool
			AuthUser            = 0x10, // 유저 인증
			AddUser             = 0x11, // 유저 추가
			DelUser             = 0x12, // 유저 삭제
			ModUser             = 0x13, // 유저 정보 변경
            RequestUserReaction = 0x14, // 유저에게 어떤 판단에 대해서 동의를 얻음.(usSubCommand 활용)
			RequestPromote      = 0x15, // 스스로 관리자가 되겠다고 선언
            ManagerInfo         = 0x16, // 현재 관리자 정보를 줌

			// ManageServer ->  ManageTool
			UserList        = 0x20, // 유저 리스트(유저 정보)
			UserLogin       = 0x21, // 서버에서 툴로 유저 정보를 준다. (유저가 로그인했음)
			UserLogout      = 0x22, // 서버에서 툴로 유저 정보를 준다. (유저가 로그아웃했음)
			StatServerStatus= 0x23, // 관리서버, 통계서버간 연결 상태를 준다.
            
            // ManageClient <-> Each Server
            IPC_ManagePing  = 0x30,  // Each Server -> ManageClient (Alive여부 및 Window Name을 준다)

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

	// 패킷 뒤에 윈도우 이름, 추가 데이터가 붙어 온다.
	// 사용 패킷 : ExecuteFile, RelayCommand, AutoPatch, 
	//			   ChangeSetup, ReloadSetup
	//
	struct ManageCommand : public PktBase
	{
		enum AutoPatchFlag
		{
			AUTOPATCH_OPENFILE	= 1,
			AUTOPATCH_DATA		= 2			
		};

		unsigned int		nMessage;			// 메시지 번호
		WPARAM				wParam;				// 파라미터1
		LPARAM				lParam;				// 파라미터2

		unsigned long		dwRunID;			// 타겟 RunID
        unsigned short      usFlags;            // 추가 데이터 플래그
		unsigned short		usDataLen;			// 추가 데이터 길이		
		
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

		사용 패킷 : AuthUser, AddUser, DelUser, ModUser, PromoteUser
		AuthUser	:	툴 -> 서버 (szID, szPassword), 
						서버 -> 툴 (szID, szPassword, szFullName, dwIP, usAdminLevel)
							다른 툴들에게 전부 자신의 UserInfo 패킷을 보낸다.
							로그인 된 사용자 전부의 정보를 받는다.

		기능		:	유저 인증을 한다.
		에러 코드	:	INVALID_PACKET_ERROR,
						AUTHORIZE_FAILED

		AddUser		:	툴 -> 서버 (szID, szPassword, szFullName, dwIP, usAdminLevel)
						서버 -> 툴 (szID, szPassword, szFullName, dwIP, usAdminLevel)
		기능		:	유저를 추가한다.
		에러 코드	:	

		DelUser		:	툴 -> 서버 (szID, szPassword, szFullName, dwIP, usAdminLevel)
						서버 -> 툴 (szID, szPassword, szFullName, dwIP, usAdminLevel)
		기능		:	유저를 삭제한다.
		에러 코드	:	

		ModUser		:	툴 -> 서버 (szID, szPassword, szFullName, dwIP, usAdminLevel)
						서버 -> 툴 (szID, szPassword, szFullName, dwIP, usAdminLevel)
		기능		:	유저 정보를 변경한다.
		에러 코드	:	


		UserInfo	:	툴 -> 서버 
						현재 로그인된 유저 정보를 요청한다. 데이터는 없다.

						서버 -> 툴
						현재 로그인된 유저 수만큼 UserInfo구조체가 뒤에 붙어 온다.

		기능		:	사용자 정보를 전송한다.
		에러 코드	:	

        
        Promote의 과정

        0. 인증 성공시, 현재 Promote된 유저의 ID를 준다.

        1. Manager가 없거나 Master의 Promote

            서버로 Promote패킷을 보냄 -> 서버는 Master거나 Manager가 없음을 확인.
            -> 확인 성공시 이 id에 Promote 권한을 준다.
            -> 각 유저들에게 누가 Promote유저인지를 가르쳐 준다.

        2. General의 Promote

            서버로 Promote패킷을 보냄 -> 서버는 General인지 확인
            -> 확인 성공시, 현재 Promote 권한이 있는 유저가 있는지 살피고 확인 패킷 보냄
            -> 확인 패킷에서, ok가 오면, 권한을 넘기고, 각 유저들에게 Promote유저임을 준다.
            -> 확인 패킷에서, no가 오면, Promote요청자에게 실패 메시지를 준다.

        필요 패킷

        1. ManageUserInfo       (ManageServer -> ManageTool) ManagerID및 정보
        2. Request Promote      (ManageTool -> ManageServer) 정보 없음
        3. Request Promote Ack  (ManageServer -> ManageTool) 정보 없음

        4. Request User Reaction     (ManageServer -> ManageTool) 요구자 정보, 요구 타입(usSubCommand)
        5. Request User Reaction Ack (ManageTool -> ManageServer) 요구자 정보, 요구 결과(usSubCommand)
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
			// edith 2008.03.17 ID,PASS 길이조정
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
		// 뒤에 UserInfo구조체가 cUserInfoNum개 만큼 붙는다.
	};


	/*
		Relay Command로 오는 커맨드
		서버 정보
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
		unsigned int	m_nNation;		// 국가 코드(AdminToolServer.ini 참조)
		unsigned long   m_dwServerID;	// 정보를 전송해온 서버 ID
		unsigned int	m_nUserNum;		// 접속 유저 수
	};

	struct StatServerStatus : public PktBase
	{
		bool	m_bGlobalStatServerOK;	// 글로번 통계서버와 관리서버 연결 상태
		bool	m_bLocalStatServerOK;	// 로컬 통계서버와 관리서버 연결 상태
	};

#pragma pack()

};

#endif