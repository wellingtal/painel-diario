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
    // 서버로 인증 요청을 한다.
    if(0 != iErrorCode || 
        !SendPacket::AuthAccount(m_CharacterData.GetClientSocket(), m_CharacterData.GetAccount(),
        m_CharacterData.GetPassword(), m_CharacterData.GetClientVersion(), m_CharacterData.GetCheckSum(), 0))
    {
        // 인증 전송 실패
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
    // 서버에서 연결이 끊겼음을 처리한다.
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

    // 귀찮아서 처리 안 함.
    case CmdCharCreate:		rc = ParseCharCreate(this, lpPktBase, m_CharacterData);		break;
    case CmdCharDelete:		rc = ParseCharDelete(this, lpPktBase, m_CharacterData);		break;


    // 캐릭터 통합 선택 관련해서 넣음.
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
        // 인증 성공. 유저ID세팅하고 UserLogin전송
        characterData.SetUID(dwUID);

        // cAgentServerType, cFirstLogin 값을 저장해 놓았다, 
        // 나중에 캐릭터 데이터 왔을 때 잘 써먹도록 한다.
        characterData.SetUnifiedLoginInfo(cAgentServerType, cFirstLogin);

        if (UnifiedConst::Part2 == cAgentServerType)
        {
            SendPacket::UserLogin(lpHandler, dwUID, FALSE);
        }
    }
    else
    {
        // 에러코드 출력
        const char* szErrorString = "알 수 없는 에러입니다";

        switch (dwError)
        {
        case 3:     szErrorString = "체크섬 실패";              break;
        case 4:     szErrorString = "잘못된 버젼 클라이언트";   break;
        case 5:     szErrorString = "잘못된 계정/패스";         break;
        case 6:     szErrorString = "인증 실패";                break;
        case 7:     szErrorString = "인증 인원 초과";           break;
        case 8:     szErrorString = "블롯된 계정";              break;
        case 10:    szErrorString = "기타 인증 에러";           break;

        case 11:    szErrorString = "등록이 되어 있지 않거나 암호가 틀림";                  break;
        case 12:    szErrorString = "릴사용 동의된 사용자가 아님";                          break;            
        case 13:    szErrorString = "릴 사용자가 아니거나 비정상적인 사용자";           break;
        case 14:    szErrorString = "릴사용 동의한 사용자이지만 게임을 할 수 없는 상태";    break;
        case 15:    szErrorString = "15세 이하 유저 / 배틀로한은 12세 이하 유저임";         break;
        
        case 20:    szErrorString = "등록이 되어 있지 않거나 암호가 틀림";  break;
        case 21:    szErrorString = "UID 값 이상";                          break;
        case 22:    szErrorString = "미설정된 포인트 량";                   break;
        case 23:    szErrorString = "로그인 인증 오류";                     break;
        case 24:    szErrorString = "포인트 카드 사용자가 아님";            break;
        case 25:    szErrorString = "남은 포인트가 없거나 음수 데이터";     break;
        case 26:    szErrorString = "남은 포인트 부족";                     break;
        case 27:    szErrorString = "유효 기간 오류";                       break;
        case 28:    szErrorString = "포인트 결제 오류";                     break;
        case 29:    szErrorString = "정액 사용기간이 시작되지 않았음";      break;
        case 30:    szErrorString = "정액 사용기간 종료";                   break;
        case 31:    szErrorString = "정액 결제 오류";                       break;
        case 40:    szErrorString = "세션 오픈 에러";                       break;
        case 41:    szErrorString = "세션 열려 있음";                       break;
        case 42:    szErrorString = "세션 종료 시킴";                       break;        
        }

        // AuthAccount : 에러메시지 출력 - 모달 다이얼로그 쓰지 마시오!
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

	// WORK_LIST 3.2 정섭 슬롯 5개로 늘리기
	// WORK_LIST 3.1 RcaeBase 파일 합치기 및 캐릭터 선택화면 변경
	// WORK_LIST 2.1 계정 국적 추가
	CHAR_VIEW       aryCharView[CCharacterData::MAX_CHAR_VIEW];
	sGuildData		aryCharGuildData[CCharacterData::MAX_CHAR_VIEW];
	unsigned char	cAccountNation = 0;

    unsigned long dwError = ParsePacket::HandleUserLogin(lpPktBase, &dwSessionID, 
        &UserID, aryCharView, aryCharGuildData, &cAccountNation, &ttCurrentTime, &nPlayTime, &wAdminLevel, 
        &cBillingType, &cBillingUser, &cLoginType, &dwTabFlag);

    if(0 == dwError)
    {
        // 과금관련이나, 캐릭터 선택화면으로 이동인지 아닌지 여부 따위는 관계 없다.
        // 알고 싶은건 오로지 캐릭터 정보뿐.

		characterData.SetCharView(aryCharView, CCharacterData::MAX_CHAR_VIEW);
		// WORK_LIST 3.1 RcaeBase 파일 합치기 및 캐릭터 선택화면 변경
		characterData.SetCharGuildData(aryCharGuildData, CCharacterData::MAX_CHAR_VIEW);
        characterData.SetSessionID(dwSessionID);

        // 캐릭터 정보를 모달리스로 띄운다. 
        // (모달 띄울때는 PostMessage등을 메인프레임으로 뿌려서 처리한다. 
        //  물론 LoginDlg와 같이 ContinueModal을 오버라이딩 해서 theApp.OnIdle이 호출되게 해야 한다.)

        // 창을 닫으면 NCDestory에서 Delete하므로 포인터를 갖고 있을 필요는 없다.
        // (다른 클래스와 상호참조시, NCDestory쪽에서 참조하는쪽 포인터를 NULL로 세팅하는 부분이 필요하다).
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
        const char* szErrorString = "알 수 없는 에러입니다";

        switch(dwError)
        {
        case 1:     szErrorString = "서버 에러입니다";                                  break;
        case 2:     szErrorString = "서버에서 데이터를 얻는데 실패했습니다.";           break;
        case 3:     szErrorString = "잘못된 버젼의 클라이언트 입니다. 홈페이지에서 수동패치를 받아 설치해 주십시오.";   break;
        case 4:     szErrorString = "이미 로그인되 있는 계정입니다.";                   break;
        case 5:     szErrorString = "로그아웃 중입니다. 잠시 후 다시 접속해주세요.";    break;
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

    // 이 안에서 ClientSocket::MoveZoneAddr에 주소를 세팅한다.
    unsigned long dwError = ParsePacket::HandleServerZone(lpPktBase, clientSocket, &dwServerID);

    if (0 == dwError)
    {
        characterData.SetServerID(dwServerID);

        // 인증과의 세션을 닫는다.		
        clientSocket.Disconnect(ClientSocket::AuthEventHandler);

        // ClientSocket::MoveZoneAddr에는 연결할 서버 주소가 들어 있다.
        ClientNet::CClientEventHandler* lpGameHandler = new CDummyGameHandler(characterData);

        clientSocket.Open(clientSocket.GetAddressString(ClientSocket::MoveZoneAddr),
            clientSocket.GetAddressPort(ClientSocket::AddressType::MoveZoneAddr), lpGameHandler);            
    } 
    else
    {
        const char* szErrorString = "알 수 없는 에러입니다";

        switch (dwError)
        {
        case 2:	szErrorString = "캐릭터 존이 잘못 되어 있습니다";
        case 4:	szErrorString = "인원이 초과";
        case 5: szErrorString = "세션이 열려 있지 않음";
        }

        std::string szErrMessage = "AuthServerZone : ";
        szErrMessage += szErrorString;

        return false;
    }

    return true;
}


bool ParseCharSelect(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    // ack받아서 serveZone날림.
    
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
            // 채널 없음 메시지 뿌림
            
        } 
        else if (1 == dwChannelCount)
        {
            // 채널이 하나면, 그중 하나에 들어감.
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
        const char* szErrorString = "알 수 없는 에러입니다";
        switch (dwError)
        {
        case 1:     szErrorString = "서버 에러";        break;
        case 2:     szErrorString = "서버 포화";        break;
        case 3:     szErrorString = "캐릭터 로드 실패"; break;
        case 4:     szErrorString = "서버 찾기 실패";   break;
        case 5:     szErrorString = "캐릭터 블록";      break;
        }

        std::string szErrMessage = "CharSelect : ";
        szErrMessage += szErrorString;

        return false;
    }    


    return true;
}


bool ParseCharCreate(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    // ack받아서 view에 추가함

    return true;
}

bool ParseCharDelete(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    // ack받아서 view에서 지워줌

    return true;
}

bool ParseUnifiedCharInfo(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase, CCharacterData& characterData)
{
    // 캐릭터 정보 받아서 선택한 후에, 정보 보내도록 한다.
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

        // 고를만한 캐릭터가 없으면 그냥 로그인해버린다.
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
    // 받으면 에러 나면 에러코드를, 아니면 UserLogin을 보낸다.

    unsigned long dwError = ParsePacket::HandleUnifiedCharSelectAck(lpPktBase);
    
    if (0 == dwError)
    {        
        // 성공했으니 UserLogin을 보내 보자.
        SendPacket::UserLogin(lpHandler, characterData.GetUID(), FALSE);
    }
    else
    {
        const char* szErrorString = "알 수 없는 에러입니다";
        switch (dwError)
        {
        case PktUnifiedCharSelectAck::SERVER_ERROR:                 szErrorString = "서버 에러";    break;
        case PktUnifiedCharSelectAck::PACKET_ERROR:                 szErrorString = "패킷 에러";    break;
        case PktUnifiedCharSelectAck::PASSWORD_FAILED:              szErrorString = "재인증 실패";  break;

        case PktUnifiedCharSelectAck::WRONG_STORE_SELECTED:         szErrorString = "잘못된 창고 선택(내 소유 아닌것 선택)";                        break;
        case PktUnifiedCharSelectAck::WRONG_CHAR_SELECTED:          szErrorString = "잘못된 캐릭터 선택(내 소유 아닌것 선택. 이미 선택한 것 선택)"; break;

        case PktUnifiedCharSelectAck::UNIFIED_STORE_READ_ERROR:     szErrorString = "선택한 창고 읽기 실패";        break;
        case PktUnifiedCharSelectAck::UNIFIED_STORE_WRITE_ERROR:    szErrorString = "선택한 창고 기록 실패";        break;

        case PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR:     szErrorString = "캐릭터 선택 정보 기록 실패";   break;
        case PktUnifiedCharSelectAck::CHAR_VIEW_RELOAD_ERROR:       szErrorString = "캐릭터 뷰 리로드 실패";        break;
        }

        std::string szErrMessage = "UnifiedCharSelect : ";
        szErrMessage += szErrorString;
    }

    return true;
}

