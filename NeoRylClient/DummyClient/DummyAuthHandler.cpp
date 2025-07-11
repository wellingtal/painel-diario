#include "stdafx.h"
#include "resource.h"
#include "MainFrm.h"
#include "CharacterSelectDlg.h"
#include "LoginDlg.h"
#include "DummyAuthHandler.h"
#include "DummyGameHandler.h"
#include "CharacterData.h"
#include "UnifiedCharSelect.h"

#include <string>

#include <Network/ClientSocket/Send/SendAuthServer.h>
#include <Network/ClientSocket/Send/SendLoginOut.h>
#include <Network/ClientSocket/Parse/ParseAuthServer.h>
#include <Network/ClientSocket/Parse/ParseLoginOut.h>

#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketCommand.h>
#include "GMMemory.h"

bool ParseAuthAccount(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);
bool ParseUserLogin(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);
bool ParseCharSelect(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);
bool ParseCharCreate(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);
bool ParseCharDelete(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);
bool ParseAuthServerZone(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);

bool ParseUnifiedCharInfo(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);
bool ParseUnifiedCharSelectAck(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData);



CDummyAuthHandler::CDummyAuthHandler(CCharacterData& characterData)
:   m_CharacterData(characterData)
{


}

CDummyAuthHandler::~CDummyAuthHandler()
{

}

int CDummyAuthHandler::OnOpen(int iErrorCode)
{
    // ������ ���� ��û�� �Ѵ�.
    if(0 != iErrorCode || 
        !SendPacket::AuthAccount(m_CharacterData.GetClientSocket(), m_CharacterData.GetAccount(),
        m_CharacterData.GetPassword(), m_CharacterData.GetClientVersion(), m_CharacterData.GetCheckSum(), 0))
    {
        // ���� ���� ����
        CCharacterDataMgr& characterDataMgr = 
            static_cast<CMainFrame*>(AfxGetMainWnd())->GetCharacterDataMgr();

        characterDataMgr.RemoveByAccount(m_CharacterData.GetAccount());
    }
    else
    {
        m_CharacterData.GetClientSocket().SetHandler(this, ClientSocket::AuthEventHandler);
    }

    return 0;
}

int CDummyAuthHandler::OnClose()
{
    // �������� ������ �������� ó���Ѵ�.
    CCharacterDataMgr& characterDataMgr = 
        static_cast<CMainFrame*>(AfxGetMainWnd())->GetCharacterDataMgr();

    characterDataMgr.RemoveByAccount(m_CharacterData.GetAccount());
    m_CharacterData.GetClientSocket().ResetHandler(this, ClientSocket::AuthEventHandler);
    return 0;
}

int CDummyAuthHandler::OnDispatch(PktBase* lpPktBase, INET_Addr& peerAddr)
{
    bool rc = true;

    switch(lpPktBase->GetCmd())
    {
    case CmdAuthAccount:	rc = ParseAuthAccount(this, lpPktBase, m_CharacterData);		break;
    case CmdUserLogin:		rc = ParseUserLogin(this, lpPktBase, m_CharacterData);		    break;
    case CmdServerZone:		rc = ParseAuthServerZone(this, lpPktBase, m_CharacterData);	break;
    case CmdCharSelect:		rc = ParseCharSelect(this, lpPktBase, m_CharacterData);		break;

    // �����Ƽ� ó�� �� ��.
    case CmdCharCreate:		rc = ParseCharCreate(this, lpPktBase, m_CharacterData);		break;
    case CmdCharDelete:		rc = ParseCharDelete(this, lpPktBase, m_CharacterData);		break;


    // ĳ���� ���� ���� �����ؼ� ����.
    case CmdUnifiedCharInfo:    rc = ParseUnifiedCharInfo(this, lpPktBase, m_CharacterData);       break;
    case CmdUnifiedCharSelect:  rc = ParseUnifiedCharSelectAck(this, lpPktBase, m_CharacterData);	break;
    }

    if(!rc)
    {
        CCharacterDataMgr& characterDataMgr = 
            static_cast<CMainFrame*>(AfxGetMainWnd())->GetCharacterDataMgr();
        
        characterDataMgr.RemoveByAccount(m_CharacterData.GetAccount());
    }

    return 0;
}

bool ParseAuthAccount(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    unsigned long dwUID = 0;    
    unsigned char cAgentServerType = 0;
    unsigned char cFirstLogin = 0;

    unsigned long dwError = ParsePacket::HandleAuthAccount(lpPktBase, dwUID, cAgentServerType, cFirstLogin);
        
    if(0 == dwError)
    {
        // ���� ����. ����ID�����ϰ� UserLogin����
        characterData.SetUID(dwUID);

        // cAgentServerType, cFirstLogin ���� ������ ���Ҵ�, 
        // ���߿� ĳ���� ������ ���� �� �� ��Ե��� �Ѵ�.
        characterData.SetUnifiedLoginInfo(cAgentServerType, cFirstLogin);

        if (UnifiedConst::Part2 == cAgentServerType)
        {
            SendPacket::UserLogin(lpHandler, dwUID, FALSE);
        }
    }
    else
    {
        // �����ڵ� ���
        const char* szErrorString = "�� �� ���� �����Դϴ�";

        switch (dwError)
        {
        case 3:     szErrorString = "üũ�� ����";              break;
        case 4:     szErrorString = "�߸��� ���� Ŭ���̾�Ʈ";   break;
        case 5:     szErrorString = "�߸��� ����/�н�";         break;
        case 6:     szErrorString = "���� ����";                break;
        case 7:     szErrorString = "���� �ο� �ʰ�";           break;
        case 8:     szErrorString = "��Ե� ����";              break;
        case 10:    szErrorString = "��Ÿ ���� ����";           break;

        case 11:    szErrorString = "����� �Ǿ� ���� �ʰų� ��ȣ�� Ʋ��";                  break;
        case 12:    szErrorString = "����� ���ǵ� ����ڰ� �ƴ�";                          break;            
        case 13:    szErrorString = "�� ����ڰ� �ƴϰų� ���������� �����";           break;
        case 14:    szErrorString = "����� ������ ����������� ������ �� �� ���� ����";    break;
        case 15:    szErrorString = "15�� ���� ���� / ��Ʋ������ 12�� ���� ������";         break;
        
        case 20:    szErrorString = "����� �Ǿ� ���� �ʰų� ��ȣ�� Ʋ��";  break;
        case 21:    szErrorString = "UID �� �̻�";                          break;
        case 22:    szErrorString = "�̼����� ����Ʈ ��";                   break;
        case 23:    szErrorString = "�α��� ���� ����";                     break;
        case 24:    szErrorString = "����Ʈ ī�� ����ڰ� �ƴ�";            break;
        case 25:    szErrorString = "���� ����Ʈ�� ���ų� ���� ������";     break;
        case 26:    szErrorString = "���� ����Ʈ ����";                     break;
        case 27:    szErrorString = "��ȿ �Ⱓ ����";                       break;
        case 28:    szErrorString = "����Ʈ ���� ����";                     break;
        case 29:    szErrorString = "���� ���Ⱓ�� ���۵��� �ʾ���";      break;
        case 30:    szErrorString = "���� ���Ⱓ ����";                   break;
        case 31:    szErrorString = "���� ���� ����";                       break;
        case 40:    szErrorString = "���� ���� ����";                       break;
        case 41:    szErrorString = "���� ���� ����";                       break;
        case 42:    szErrorString = "���� ���� ��Ŵ";                       break;        
        }

        // AuthAccount : �����޽��� ��� - ��� ���̾�α� ���� ���ÿ�!
        std::string szErrMessage = "AuthAccount : ";
        szErrMessage += szErrorString;

        return false;
    }

    return true;
}

bool ParseUserLogin(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    unsigned long	UserID = 0, dwSessionID = 0, dwTabFlag = 0;
    unsigned short	wAdminLevel	= 0;
    unsigned char	cBillingType = 0, cBillingUser = 0, cLoginType = 0;

    int		        nPlayTime = 0;
    time_t	        ttCurrentTime = 0;

	// WORK_LIST 3.2 ���� ���� 5���� �ø���
	// WORK_LIST 3.1 RcaeBase ���� ��ġ�� �� ĳ���� ����ȭ�� ����
	// WORK_LIST 2.1 ���� ���� �߰�
	CHAR_VIEW       aryCharView[CCharacterData::MAX_CHAR_VIEW];
	sGuildData		aryCharGuildData[CCharacterData::MAX_CHAR_VIEW];
	unsigned char	cAccountNation = 0;

    unsigned long dwError = ParsePacket::HandleUserLogin(lpPktBase, &dwSessionID, 
        &UserID, aryCharView, aryCharGuildData, &cAccountNation, &ttCurrentTime, &nPlayTime, &wAdminLevel, 
        &cBillingType, &cBillingUser, &cLoginType, &dwTabFlag);

    if(0 == dwError)
    {
        // ���ݰ����̳�, ĳ���� ����ȭ������ �̵����� �ƴ��� ���� ������ ���� ����.
        // �˰� ������ ������ ĳ���� ������.

		characterData.SetCharView(aryCharView, CCharacterData::MAX_CHAR_VIEW);
		// WORK_LIST 3.1 RcaeBase ���� ��ġ�� �� ĳ���� ����ȭ�� ����
		characterData.SetCharGuildData(aryCharGuildData, CCharacterData::MAX_CHAR_VIEW);
        characterData.SetSessionID(dwSessionID);

        // ĳ���� ������ ��޸����� ����. 
        // (��� ��ﶧ�� PostMessage���� �������������� �ѷ��� ó���Ѵ�. 
        //  ���� LoginDlg�� ���� ContinueModal�� �������̵� �ؼ� theApp.OnIdle�� ȣ��ǰ� �ؾ� �Ѵ�.)

        // â�� ������ NCDestory���� Delete�ϹǷ� �����͸� ���� ���� �ʿ�� ����.
        // (�ٸ� Ŭ������ ��ȣ������, NCDestory�ʿ��� �����ϴ��� �����͸� NULL�� �����ϴ� �κ��� �ʿ��ϴ�).
        CCharacterSelectDlg* lpCharSelectDlg = 
            CCharacterSelectDlg::CreateSelectDlg(characterData);        

        if(0 != lpCharSelectDlg)
        {
            lpCharSelectDlg->MoveToCenter();

            for(int nCount = 0; nCount < CCharacterData::MAX_CHAR_VIEW; ++nCount)
            {
                CHAR_VIEW& charView = aryCharView[nCount];

                lpCharSelectDlg->InsertData(charView.Name, 
                    characterData.GetUID(), charView.CID, 
                    charView.Level, charView.Fame, charView.Class);
            }

            lpCharSelectDlg->ShowWindow(SW_SHOW);
        }
    }
    else
    {   
        const char* szErrorString = "�� �� ���� �����Դϴ�";

        switch(dwError)
        {
        case 1:     szErrorString = "���� �����Դϴ�";                                  break;
        case 2:     szErrorString = "�������� �����͸� ��µ� �����߽��ϴ�.";           break;
        case 3:     szErrorString = "�߸��� ������ Ŭ���̾�Ʈ �Դϴ�. Ȩ���������� ������ġ�� �޾� ��ġ�� �ֽʽÿ�.";   break;
        case 4:     szErrorString = "�̹� �α��ε� �ִ� �����Դϴ�.";                   break;
        case 5:     szErrorString = "�α׾ƿ� ���Դϴ�. ��� �� �ٽ� �������ּ���.";    break;
        }

        std::string szErrMessage = "UserLogin : ";
        szErrMessage += szErrorString;

        return false;
    }

    return true;
}

bool ParseAuthServerZone(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    unsigned long dwServerID = 0;
    ClientSocket& clientSocket = characterData.GetClientSocket();

    // �� �ȿ��� ClientSocket::MoveZoneAddr�� �ּҸ� �����Ѵ�.
    unsigned long dwError = ParsePacket::HandleServerZone(lpPktBase, clientSocket, &dwServerID);

    if (0 == dwError)
    {
        characterData.SetServerID(dwServerID);

        // �������� ������ �ݴ´�.		
        clientSocket.Disconnect(ClientSocket::AuthEventHandler);

        // ClientSocket::MoveZoneAddr���� ������ ���� �ּҰ� ��� �ִ�.
        ClientNet::CClientEventHandler* lpGameHandler = new CDummyGameHandler(characterData);

        clientSocket.Open(clientSocket.GetAddressString(ClientSocket::MoveZoneAddr),
            clientSocket.GetAddressPort(ClientSocket::AddressType::MoveZoneAddr), lpGameHandler);            
    } 
    else
    {
        const char* szErrorString = "�� �� ���� �����Դϴ�";

        switch (dwError)
        {
        case 2:	szErrorString = "ĳ���� ���� �߸� �Ǿ� �ֽ��ϴ�";
        case 4:	szErrorString = "�ο��� �ʰ�";
        case 5: szErrorString = "������ ���� ���� ����";
        }

        std::string szErrMessage = "AuthServerZone : ";
        szErrMessage += szErrorString;

        return false;
    }

    return true;
}


bool ParseCharSelect(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    // ack�޾Ƽ� serveZone����.
    
    const unsigned int MAX_CHANNEL = 5;

    unsigned short	aryChannel[MAX_CHANNEL] = { 0, };
    unsigned char	cZone = 0;
    
    unsigned long dwError = ParsePacket::HandleCharSelect(lpPktBase, &cZone, aryChannel);

    if (0 == dwError)
    {
        unsigned long dwChannelCount = 0;

        for (unsigned long i = 0; i < MAX_CHANNEL; i++)
        {
            if (aryChannel[i])
            {
                dwChannelCount++;
            }
        }

        if (0 == dwChannelCount)
        {
            // ä�� ���� �޽��� �Ѹ�
            
        } 
        else if (1 == dwChannelCount)
        {
            // ä���� �ϳ���, ���� �ϳ��� ��.
            for (char cChannel = 0; cChannel < MAX_CHANNEL; ++cChannel)
            {
                if (aryChannel[cChannel])
                {
                    SendPacket::ServerZone(lpHandler, 
                        static_cast<char>(cZone), cChannel);
                    break;
                }
            }
        } 
    } 
    else 
    {
        const char* szErrorString = "�� �� ���� �����Դϴ�";
        switch (dwError)
        {
        case 1:     szErrorString = "���� ����";        break;
        case 2:     szErrorString = "���� ��ȭ";        break;
        case 3:     szErrorString = "ĳ���� �ε� ����"; break;
        case 4:     szErrorString = "���� ã�� ����";   break;
        case 5:     szErrorString = "ĳ���� ���";      break;
        }

        std::string szErrMessage = "CharSelect : ";
        szErrMessage += szErrorString;

        return false;
    }    


    return true;
}


bool ParseCharCreate(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    // ack�޾Ƽ� view�� �߰���

    return true;
}

bool ParseCharDelete(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    // ack�޾Ƽ� view���� ������

    return true;
}

bool ParseUnifiedCharInfo(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    // ĳ���� ���� �޾Ƽ� ������ �Ŀ�, ���� �������� �Ѵ�.
    USER_INFO           userInfo;
    UnifiedStoreInfo*   lpUnifiedStoreInfo  = 0;
    UnifiedCharData*    lpUnifiedCharData   = 0;
    unsigned long       dwStoreInfoNum      = 0;
    unsigned long       dwCharDataNum       = 0;
	unsigned char		cRestrictedPart1ToPart2Level = 0 ; 
	unsigned char		cRemainCharTransferCount = 0 ;
	
    unsigned long       dwErrorCode = ParsePacket::HandleUnifiedCharInfo( lpPktBase, 
																		  cRestrictedPart1ToPart2Level,
																		  cRemainCharTransferCount,
																		  userInfo,
																		  &lpUnifiedStoreInfo, 
																		  dwStoreInfoNum, 
																		  &lpUnifiedCharData, 
																		  dwCharDataNum ) ;
    if (0 == dwErrorCode)
    {
        unsigned long dwCharNum = 0;

        // ������ ĳ���Ͱ� ������ �׳� �α����ع�����.
        for(int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
        {
            if(0 != userInfo.CharID[nCount])
            {
                ++dwCharNum;
            }
        }

        if ((0 == dwStoreInfoNum || 
            (1 == dwStoreInfoNum && lpUnifiedStoreInfo->cOldServerGroupID == characterData.GetAgentServerType()))
            && dwCharNum == dwCharDataNum)
        {
            UnifiedCharData* lpPos = lpUnifiedCharData;
            UnifiedCharData* lpEnd = lpUnifiedCharData + dwCharDataNum;

            for(; lpPos != lpEnd; ++lpPos)
            {
                if(!userInfo.HasCharacter(lpPos->dwNewCID))
                {
                    break;
                }
            }

            if(lpPos == lpEnd)
            {
                SendPacket::UserLogin(lpHandler, characterData.GetUID(), FALSE);
                return true;
            }
        }

        CUnifiedCharSelect* lpUnifiedCharSelect = 
            CUnifiedCharSelect::CreateUnifiedCharSelect(characterData);

        if(0 != lpUnifiedCharSelect)
        {
            lpUnifiedCharSelect->SetData(userInfo, 
                lpUnifiedStoreInfo, dwStoreInfoNum, 
                lpUnifiedCharData, dwCharDataNum);

            lpUnifiedCharSelect->ShowWindow(SW_SHOW);
        }
    }

    return true;
}


bool ParseUnifiedCharSelectAck(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    // ������ ���� ���� �����ڵ带, �ƴϸ� UserLogin�� ������.

    unsigned long dwError = ParsePacket::HandleUnifiedCharSelectAck(lpPktBase);
    
    if (0 == dwError)
    {        
        // ���������� UserLogin�� ���� ����.
        SendPacket::UserLogin(lpHandler, characterData.GetUID(), FALSE);
    }
    else
    {
        const char* szErrorString = "�� �� ���� �����Դϴ�";
        switch (dwError)
        {
        case PktUnifiedCharSelectAck::SERVER_ERROR:                 szErrorString = "���� ����";    break;
        case PktUnifiedCharSelectAck::PACKET_ERROR:                 szErrorString = "��Ŷ ����";    break;
        case PktUnifiedCharSelectAck::PASSWORD_FAILED:              szErrorString = "������ ����";  break;

        case PktUnifiedCharSelectAck::WRONG_STORE_SELECTED:         szErrorString = "�߸��� â�� ����(�� ���� �ƴѰ� ����)";                        break;
        case PktUnifiedCharSelectAck::WRONG_CHAR_SELECTED:          szErrorString = "�߸��� ĳ���� ����(�� ���� �ƴѰ� ����. �̹� ������ �� ����)"; break;

        case PktUnifiedCharSelectAck::UNIFIED_STORE_READ_ERROR:     szErrorString = "������ â�� �б� ����";        break;
        case PktUnifiedCharSelectAck::UNIFIED_STORE_WRITE_ERROR:    szErrorString = "������ â�� ��� ����";        break;

        case PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR:     szErrorString = "ĳ���� ���� ���� ��� ����";   break;
        case PktUnifiedCharSelectAck::CHAR_VIEW_RELOAD_ERROR:       szErrorString = "ĳ���� �� ���ε� ����";        break;
        }

        std::string szErrMessage = "UnifiedCharSelect : ";
        szErrMessage += szErrorString;
    }

    return true;
}

