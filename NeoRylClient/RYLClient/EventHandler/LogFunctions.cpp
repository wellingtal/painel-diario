#include "LogFunctions.h"
#include "RYLClientMain.h"
#include <Network/ClientSocket/ClientSocket.h>

#include "RYLNetworkData.h"
#include "RYLChattingDlg.h"
#include "GMMemory.h"

void GameClientLog::LogAddress(unsigned long dwID, unsigned long dwCharID, const sockaddr_in& PublicAddr, const sockaddr_in& PrivateAddr)
{
	if (false == g_ClientMain.m_bUDPInfo)
	{
		return;
	}

    /*
    PEERTYPE peerType = g_pClientSocket->GetPeerType(PublicAddr, PrivateAddr);

    const char* szPeerType = "NONE";
    switch (peerType)
    {
		case REAL_IP:       szPeerType = "REAL_IP";         break;
		case NAT_Friendly:  szPeerType = "NAT_Friendly";    break;
		case NAT_Different: szPeerType = "NAT_Different";   break;
    }
    

	char szText[MAX_PATH];
	_snprintf(szText, MAX_PATH - 1, "%d. 0x%08x = Pub : %15s:%5d,   Pri : %15s:%5d  %s", 
        dwID, dwCharID, 
        inet_ntoa(PublicAddr.sin_addr), ntohs(PublicAddr.sin_port), 
        inet_ntoa(PrivateAddr.sin_addr), ntohs(PrivateAddr.sin_port), 
        szPeerType);

    szText[MAX_PATH - 1] = 0;
	CRYLChattingDlg::Instance()->AddMessage(szText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

    */
}