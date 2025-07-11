#include "GameHandler.h"

#include "RYLClientMain.h"
#include "RYLMessageBox.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/ClientNetwork/Session.h>
#include <Network/ClientNetwork/SessionMgr.h>
#include <Network/ClientSocket/ClientSocket.h>
#include <Network/ClientSocket/Parse/ParseLoginout.h>
#include <Network/ClientSocket/Send/SendMove.h>

#include "GameProcess/CharInfoProcess.h"
#include "GameProcess/CommunityProcess.h"
#include "GameProcess/EtcProcess.h"
#include "GameProcess/FightProcess.h"
#include "GameProcess/ItemProcess.h"
#include "GameProcess/LoginoutProcess.h"
#include "GameProcess/MoveProcess.h"
#include "GameProcess/QuestProcess.h"
#include "GameProcess/SkillProcess.h"
#include "GameProcess/BattleGroundClientProcess.h"
#include "GameProcess/CastleProcess.h"
#include "GameProcess/BroadcastProcess.h"
#include "gameprocess/ChatProcess.h"

#include <Network/ClientSocket/Send/SendLoginout.h>

#include "RYLStringTable.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLProfile.h"

/*
// Rodin : �׽�Ʈ �ڵ� #1
#include "../GUITextEdit.h"
#include "RYLChattingDlg.h"
#include "GMMemory.h"
*/


// ���Ӽ��� �̺�Ʈ �ڵ鷯
CGameEventHandler::CGameEventHandler() 
{

    
}

CGameEventHandler::~CGameEventHandler()
{


}

int CGameEventHandler::OnOpen(int iErrorCode)
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
    CRYLNetworkData* pNetworkData = CRYLNetworkData::Instance();

    // ��� �ִ� ������ ����.
    if (pGame->m_lpPickItem) { delete pGame->m_lpPickItem; pGame->m_lpPickItem = 0; }

    pGame->DeleteAllFieldItem();
	pGame->m_csStatus.Destroy();

    g_pClientSocket->SetHandler(this, ClientSocket::GameEventHandler);
	g_pClientSocket->SetStatusFlag(NS_CHARLOGIN);

	SendPacket::CharLogin(*g_pClientSocket, pNetworkData->m_dwUserID, 
							pGame->m_scLoginChar[pGame->m_dwSelectChr].CID, 
							pNetworkData->m_dwSessionID);
    return 0;
}

int CGameEventHandler::OnClose()
{
	if(!IsCloseCalled())
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[1540]);
		PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
		CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
	}
	
    g_pClientSocket->ResetHandler(this, ClientSocket::GameEventHandler);
    return 0;
}

int CGameEventHandler::OnDispatch(PktBase* lpPktBase, INET_Addr& peerAddr)
{
    unsigned long dwCmd = lpPktBase->GetCmd();

/*
	// Rodin : �׽�Ʈ �ڵ� #1
	//	=> ���� �����κ��� ���� ��Ŷ�� Ŀ�ǵ带 ����ش�.
	if (0x2f != dwCmd && 
		0x3d != dwCmd && 
		0x49 != dwCmd)
	{
		char strTemp[MAX_PATH];
		sprintf(strTemp, "0x%02x", dwCmd);
		CRYLChattingDlg::Instance()->AddMessage(strTemp, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
	}
*/

    if (dwCmd == CmdCompressedPacket)
    {
        // ���鼭 ���� ó�����ش�.        
        char* szPacketPos = reinterpret_cast<char*>(lpPktBase + 1);
        char* szPacketEnd = reinterpret_cast<char*>(lpPktBase) + lpPktBase->GetLen();

        char szPacketData[PktMaxLen];
        PktBase* lpPktCompressed = reinterpret_cast<PktBase*>(szPacketData);

        unsigned short usPacketLen = 0;        
        unsigned short usDataLen = 0;
        unsigned char  cCMD = 0;

        const unsigned short usHeaderLen = sizeof(usPacketLen) + sizeof(cCMD);
    
        while( szPacketPos < szPacketEnd )
        {
            usPacketLen = *reinterpret_cast<unsigned short*>(szPacketPos);
            cCMD = *reinterpret_cast<unsigned char*>(szPacketPos + sizeof(unsigned short));
              
            usDataLen = usPacketLen - usHeaderLen;

            lpPktCompressed->InitPtHead(usDataLen + sizeof(PktBase), cCMD, 0);            
            memcpy(lpPktCompressed + 1, szPacketPos + usHeaderLen, usDataLen);

            szPacketPos += usPacketLen;

            if (cCMD != CmdCompressedPacket) 
            {
                OnDispatch(lpPktCompressed, peerAddr); 
            }
        }
    }
    else
    {
	    CPacketProfileMgr* lpPacketProfileMgr = CRYLNetworkData::Instance()->m_lpPacketProfileMgr;
	    if (NULL != lpPacketProfileMgr)
	    {
		    lpPacketProfileMgr->RecvOtherPacket(static_cast<unsigned char>(dwCmd));
	    }

        EventSet::iterator end  = m_EventSet.end();
        EventSet::iterator find = std::lower_bound(m_EventSet.begin(), end, dwCmd);

        if(find != end && find->m_dwCommand == dwCmd)
        {
            if(0 == (find->m_dwIgnoreStatus & BIT(CRYLGameData::Instance()->m_dwClientMode)))
            {
			    //by Hades Kang �������� Cmd�� �ִٸ� ���⼭ ó�� �ؾ� �ҵ�.
			    ClientSocket::SetNSFlagOff( dwCmd );
                find->m_fnEvent(this, lpPktBase);
            }	
        }
    }

    // rc�� ������ ��Ŷ ó�� ���� ���θ� �ľ�.
	if(CRYLNetworkData::Instance()->m_dwNetworkError == 1)	//1 = ���� ���� 
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[296]);
		// edith 2009.01.29 Server Error Modal ����
		// ������ �˼����� ���� �� Server Error�� �߰ԵǸ� �����ϼ� ��� �װԵǹǷ� �����ϼ� �ְ� ����.
		lpMessageBox->SetModal(FALSE);
	}

    return 0;
}


bool CGameEventHandler::AddHandler(unsigned char cCmd, EventFunc fnEvent, unsigned long dwIgnoreType)
{
    EventSet::iterator end  = m_EventSet.end();
    EventSet::iterator find = std::lower_bound(m_EventSet.begin(), end, cCmd);

    if(find == end || find->m_dwCommand != cCmd)
    {
        m_EventSet.insert(find, EventFunctions(cCmd, fnEvent, dwIgnoreType));
        return true;
    }

    return false;
}

bool CGameEventHandler::RemoveHandler(unsigned char cCmd)
{
    EventSet::iterator end  = m_EventSet.end();
    EventSet::iterator find = std::lower_bound(m_EventSet.begin(), end, cCmd);

    if(find != end && find->m_dwCommand == cCmd)
    {
        m_EventSet.erase(find);
        return true;
    }

    return false;
}

void CGameEventHandler::ClearHandler()
{
    m_EventSet.clear();
}


CGameEventHandler* CGameEventHandler::Create()
{
    CGameEventHandler* lpGameHandler = new CGameEventHandler();

    if(NULL != lpGameHandler)
    {
        using namespace GameProcess;

        unsigned long dwFailedCount = 0;

        dwFailedCount += RegisterCharInfoHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterCommunityHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterEtcHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterFightHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterItemHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterLoginoutHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterMoveHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterQuestHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterSkillHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterBattleGroundClientHandler(*lpGameHandler) ? 0 : 1;
		dwFailedCount += RegisterCastleHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterBroadcastHandler(*lpGameHandler) ? 0 : 1;
        dwFailedCount += RegisterChatHandler(*lpGameHandler) ? 0 : 1;

        if(0 != dwFailedCount)
        {
            delete lpGameHandler;
            lpGameHandler = NULL;

			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create("Cannot create Game Event Handler!");
        }
    }

    return lpGameHandler;
}
