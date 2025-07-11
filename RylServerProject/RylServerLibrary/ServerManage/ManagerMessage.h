#ifndef _MANAGER_MESSAGE_H_
#define _MANAGER_MESSAGE_H_

// 예전 운영툴 매니저 메시지.

#include <winsock2.h>
#include <windows.h>

#define WM_RYLLOGIN_AUTOSTART				1201
#define WM_RYLUID_AUTOSTART					1202
#define WM_RYLAGENT_AUTOSTART				1203
#define WM_RYLAUTH_AUTOSTART				1204
#define WM_RYLGAME_AUTOSTART				1205

// 종료 메시지
#define WM_RYLLOGIN_QUIT					1211
#define WM_RYLUID_QUIT						1212
#define WM_RYLAGENT_QUIT					1213
#define WM_RYLAUTH_QUIT						1214
#define WM_RYLGAME_QUIT						1215
#define WM_MNGCLIENT_QUIT					1216

// 각서버에서 서버간 연결이 끊겼을 경우를 처리하기 위한 윈도우 메시지
#define WM_LOGIN_CONNECTPERMIT_STOP			1221
#define WM_LOGINAGENT_DISCONNECT			1222
#define WM_UIDAGENT_DISCONNECT				1223
#define WM_AGENTAUTH_DISCONNECT				1224
#define WM_AGENTGAME_DISCONNECT				1225

// 각 관리 클라이언트에서 서버간 연결을 처리하기 위한 윈도우 메시지
#define WM_LOGIN_CONNECTPERMIT_START		1231
#define WM_LOGINAGENT_CONNECT				1232
#define WM_UIDAGENT_CONNECT					1233
#define WM_AGENTGAME_CONNECT				1234
#define WM_AGENTAUTH_CONNECT				1235

// 서버의 세부 제어 관련 메시지들
#define WM_LOGIN_CONNECTPERMIT				1241
#define WM_AGENT_CONNECTTOLOGIN				1242
#define WM_AGENT_CONNECTTOUID				1243
#define WM_AUTH_CONNECTTOAGENT				1244
#define WM_GAME_CONNECTTOAGENT				1245

// 관리서버 및 클라이언트의 어플리케이션 이름
#define MANAGERSERVER_APPNAME				"RYLMANAGERSERVER"
#define MANAGERCLIENT_APPNAME				"RYLMANAGERCLIENT"



const int	MNGMSGTYPE_CONNECTORNUM		= 1;
const int	MNGMSGTYPE_NOTIFY			= 2;


struct MNGMSG_ConnectorNum 
{
	enum	{ MAX_CHANNEL_INDEX	= 2 };
	enum	{ MAX_ZONE_INDEX	= 3 };

	int		m_Connector[MAX_CHANNEL_INDEX][MAX_ZONE_INDEX];	// 각 중계서버의 채널 및 존서버의 배열
	int		m_TotalConnector;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// constructor
	MNGMSG_ConnectorNum() : m_TotalConnector(-1)
	{
		for (int ch=0; ch<MAX_CHANNEL_INDEX; ++ch)
        {
			for (int zn=0; zn<MAX_ZONE_INDEX; ++zn)
            {
				m_Connector[ch][zn]	= 0;	// 각 중계서버의 채널 및 존서버의 배열
            }
        }
	}

	// operator overloading
	MNGMSG_ConnectorNum	&operator=(MNGMSG_ConnectorNum ConnNum)
	{
		for (int ch=0; ch<MAX_CHANNEL_INDEX; ++ch)
        {
			for (int zn=0; zn<MAX_ZONE_INDEX; ++zn)
            {
				m_Connector[ch][zn] = ConnNum.m_Connector[ch][zn]; // 각 중계서버의 채널 및 존서버의 배열
            }
        }

		m_TotalConnector = ConnNum.m_TotalConnector;
		return *this;
	}
};

struct MNGMSG_Notify
{
	enum	{ MAX_NOTIFY_NUM	= 128 };
	enum	{ NOTIFY_ALL, NOTIFY_TOSPECIFICSERVER };

	unsigned long	m_dwType; // 0 인경우 모든 서버가 다 공지를 보내고, 서버아이디가 셋된경우는 해당되는 서버만 공지를 보냄
	char	m_Message[MAX_NOTIFY_NUM];

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// constructor
	MNGMSG_Notify() : m_dwType(NOTIFY_ALL)
	{
		m_Message[0] = '\0';
	}
};


#endif