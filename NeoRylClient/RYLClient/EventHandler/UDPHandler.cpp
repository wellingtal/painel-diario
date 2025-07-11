#include "UDPHandler.h"

#include "RYLClientMain.h"
#include "RYLMessageBox.h"

#include <Network/ClientNetwork/Session.h>
#include <Network/ClientNetwork/SessionMgr.h>
#include <Network/ClientSocket/ClientSocket.h>

#include "RYLStringTable.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#define DISPATCH_UDP(funcName) bool funcName(ClientNet::CSession& ReceivedSession, PktBase* lpPktBase, INET_Addr& peerAddr)

// UDP이벤트 핸들러
CUDPEventHandler::CUDPEventHandler(ClientNet::CSessionMgr& SessionMgr)
:   CClientEventHandler(SessionMgr)
{
}

CUDPEventHandler::~CUDPEventHandler()
{
}


int CUDPEventHandler::OnOpen(ClientNet::CSession& OpenSession, int iErrorCode)
{
	return 0;
}

int CUDPEventHandler::OnClose(ClientNet::CSession& CloseSession)
{
	if (!CloseSession.IsShutdownCalled())
	{
		CRYLGameData* pGame = CRYLGameData::Instance() ;

		MessageBox(NULL, CRYLStringTable::m_strString[1560], CRYLStringTable::m_strString[21], MB_OK);
		PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
		pGame->m_dwClientMode = CLIENT_END;
	}

	return 0;
}

int CUDPEventHandler::OnDispatch(ClientNet::CSession& ReceivedSession, PktBase* lpPktBase, INET_Addr& peerAddr)
{
	bool rc = false;

	if (!rc)
	{
		// 에러 처리
	}

	if (CRYLNetworkData::Instance()->m_dwNetworkError == 1)	//1 = 서버 에러 
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[296]);
	}

	return 0;
}
