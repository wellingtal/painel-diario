#include "stdafx.h"
#include "ChatProcess.h"

#include "RYLClientMain.h"
#include "RYLMessageBox.h"
#include "../GUITextEdit.h"

#include <Network/ClientNetwork/Session.h>
#include <Network/ClientNetwork/SessionMgr.h>
#include <Network/ClientSocket/ClientSocket.h>
#include <Network/ClientSocket/Parse/ParseCommunity.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/ClientSocket/Send/SendLoginout.h>
//==============================================================================
#include "RYLStringTable.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"
#include "RYLChattingDlg.h"
#include "RYLNoticeManager.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLSocietyFrameDlg.h"
#include "RYLFriendPanel.h"
#include "RYLRefusalPanel.h"
#include "GMMemory.h"

#include "StringFilter.h"

GAME_EVENT_FUNC(ParseCharChat);
GAME_EVENT_FUNC(ParseCharWhisper);
GAME_EVENT_FUNC(ParseCharChatBan);


void LogChatMessage(unsigned long dwCID, const char* szMessage, unsigned short wCommand)
{
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);

    fprintf(stderr, "[%4d-%2d-%2d %2d:%2d:%2d]\tCID:0x%08X\tChatMessage(%2d):%s\n", 
        sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
        sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
        dwCID, wCommand, szMessage);
}


bool GameProcess::RegisterChatHandler(CGameEventHandler& GameEventHandler)
{
	unsigned long dwErrorCount = 0;

	dwErrorCount += GameEventHandler.AddHandler(CmdCharChat,    ParseCharChat) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharWhisper, ParseCharWhisper) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdChatBan,		ParseCharChatBan) ? 0 : 1;
	
	return (0 == dwErrorCount);
}


GAME_EVENT_FUNC(ParseCharChat)
{
    unsigned long	dwSender = 0;
	unsigned short	wCommand = 0;
	unsigned short  usLang = 0;

    char strMessage[MAX_PATH] = "";
    CRYLGameData* pGame = CRYLGameData::Instance() ;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharChat(lpPktBase, dwSender, &wCommand, &usLang, strMessage);

    if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
    {
		int len = 0;
		int size = (int)strlen(strMessage)-1;
		for(len = 0; len < size; ++len)
		{
			if(strMessage[len] == ':')
				break;
		}

		g_StringFilter.ConvertFilter(&strMessage[len]);

		// 채팅 데이터 로그로 남김
		LogChatMessage(dwSender, strMessage, wCommand);

        CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

#ifdef _RYL_LANGUAGE
		// 모든 채팅보기가 아닐경우 (MultiByte)
		if ( !pChat->GetAllChatCheckBox() )
		{
			if(pChat->CheckCharSet(strMessage, strlen(strMessage))	// 멀티바이트 문자이고	
				&& pChat->CheckAisaLang(usLang)	// 아시아권 언어이고
				&& usLang != pChat->m_usLocalLang)	// 내 컴퓨터의 언어와 동일하지 않으면 
				return true;	// 화면에 출력하지 않는다.
		}
#endif
/*
		if ( !pChat->GetAllChatCheckBox() )
		{
			if(pChat->CheckCharSet(strMessage, strlen(strMessage)))	// 멀티바이트 문자이고	
				return true;	// 화면에 출력하지 않는다.
		}
*/
        switch (wCommand)
		{
			case PktChat::NORMAL:
				if ( pGame->m_coOption.m_bChat)
				{ 
					if ( pChat->GetGeneralChatChecked() )
					{
						pChat->AddChatMessage( strMessage, dwSender, wCommand ) ;
						pChat->SaveText( strMessage, wCommand ) ;
					}				
				}
				break;

			case PktChat::DICE:
				if ( pGame->m_coOption.m_bChat)
				{ 
					if ( pChat->GetGeneralChatChecked() )
					{
						CHAR strUser[128];
						int iMax, iCur;

						sscanf(strMessage, "%s %d %d", strUser, &iMax, &iCur);
						sprintf(strMessage, CRYLStringTable::m_strString[479], strUser, iCur, iMax);

						// 메시지  User 100, 10 AddString("%s rolled %d-sided dice and got %d."); 	// 479 							
						pChat->AddChatMessage( strMessage, dwSender, wCommand ) ;
						pChat->SaveText( strMessage, wCommand ) ;
					}				
				}
				break;

			case PktChat::PARTY:
				if ( pChat->GetPartyChatCheckBox() )
				{
					pChat->AddChatMessage( strMessage, dwSender, wCommand ) ;
					pChat->SaveText( strMessage, wCommand ) ;
				}
				break;

			case PktChat::GUILD:
				if ( pChat->GetGuildChatCheckBox() )
				{
					pChat->AddChatMessage( strMessage, dwSender, wCommand ) ;
					pChat->SaveText( strMessage, wCommand ) ;
				}
				break;

			case PktChat::TRADE :
				if ( pChat->GetDealChatCheckBox() )
				{
					pChat->AddChatMessage( strMessage, dwSender, wCommand ) ;
					pChat->SaveText( strMessage, wCommand ) ;
				}
				break ;

			case PktChat::STALL:
			{
				CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLStallDlg*	pDlg = ( CRYLStallDlg* )pScene->GetStreetStallDlg() ;

				pChat->AddStallChatMessage( pDlg, strMessage, dwSender, wCommand ) ;
				break;
			}

			case PktChat::CAMP_SHOP:
			{
				CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLStallDlg*	pDlg = ( CRYLStallDlg* )pScene->GetCampShopDlg() ;

				pChat->AddStallChatMessage( pDlg, strMessage, dwSender, wCommand ) ;
				break;
			}

			case PktChat::SHOUT:
				if ( pChat->GetShoutChatChecked() )
				{
					pChat->AddChatMessage( strMessage, dwSender, wCommand ) ;
					pChat->SaveText( strMessage, wCommand ) ;
				}
				break;

			case PktChat::ADMIN_NORMAL_CHAT:
			case PktChat::ADMIN_SHOUT:
			case PktChat::NOTIFY_CHAR_INFO:
			case PktChat::ENEMY_CHECK:
				pChat->AddChatMessage( strMessage, dwSender, wCommand ) ;
				pChat->SaveText( strMessage, wCommand ) ;
				break;

			case PktChat::FIND_PARTY:
				if ( pChat->GetGeneralChatChecked() )
				{
					pChat->AddChatMessage( strMessage, dwSender, wCommand ) ;
					pChat->SaveText( strMessage, wCommand ) ;
				}
				break;

			case PktChat::NOTICE:	// 공지
				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;
				break;	
		}

        return true;
    }
    else
    {
	    CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
        {
			case PktChat::CHAT_BAN:
				pChat->AddMessage(CRYLStringTable::m_strString[3379], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;
        }
    }

    return false;
}


GAME_EVENT_FUNC(ParseCharWhisper)
{
    char strSendName[MAX_PATH] = ""; 
    char strRecvName[MAX_PATH] = "";
    char strMessage[MAX_PATH] = "";
    char strChat[MAX_PATH];

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharWhisper(lpPktBase, strSendName, strRecvName, strMessage);

    CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

    if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
    {
		g_StringFilter.ConvertFilter(strMessage);

        sprintf(strChat, "(from. %s) %s", strSendName, strMessage);

        LogChatMessage(0, strMessage, PktChat::WHISPER);

        strcpy( CRYLCommunityData::Instance()->m_strRecentChatChrID, strSendName);

		pChat->AddChatMessage( strChat, CRYLNetworkData::Instance()->m_dwMyChrID, 10 ) ;
		pChat->SaveText( strChat, 10 ) ;
        
        return true;
    } 
    else
    {
        switch (CRYLNetworkData::Instance()->m_dwNetworkError)
        {
			case PktWh::FAIL_CHAR_LOGOUT:

				if (strcmp(CRYLCommunityData::Instance()->m_strWhisperChrID, ""))
				{
					strcpy(CRYLCommunityData::Instance()->m_strWhisperChrID, "");
				}

				sprintf(strChat, CRYLStringTable::m_strString[351], strRecvName);
				pChat->AddMessage( strChat, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				return true;

			case PktWh::FAIL_REJECT:			
				pChat->AddMessage(CRYLStringTable::m_strString[358], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;

			case PktWh::FAIL_ENEMY:				
				pChat->AddMessage(CRYLStringTable::m_strString[404], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;

			case PktWh::FAIL_CHAT_BAN:
				pChat->AddMessage(CRYLStringTable::m_strString[3379], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				break;
        }
    }

    return false;
}


GAME_EVENT_FUNC(ParseCharChatBan)
{
    CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	bool bChatBan = false;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharChatBan(lpPktBase, bChatBan);
    if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
    {
		if (true == bChatBan)
		{
			pChat->AddMessage(CRYLStringTable::m_strString[3380], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
		}
		else
		{
			pChat->AddMessage(CRYLStringTable::m_strString[3381], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
		}
        
        return true;
    } 

	return false;
}