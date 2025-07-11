
#include <Network/ClientNetwork/Session.h>
#include <Network/ClientSocket/Send/SendAuthServer.h>
#include <Network/ClientSocket/Send/SendLoginout.h>
#include <Network/ClientSocket/Parse/ParseAuthServer.h>
#include <Network/ClientSocket/Parse/ParseLoginout.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>

#include <Creature/Character/CharacterClass.h>
#include <Creature/CreatureStructure.h>

#include <Utility/Exception.h>

#include "RYLEdit.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"
#include "RYLTimer.h"
#include "RYLStringTable.h"

#include "RYLSceneManager.h"
#include "RYLLoginScene.h"
#include "RYLCharacterSelectScene.h"
// WORK_LIST 2.2 NationSelectScene 이름 수정
#include "RYLRaceSelectScene.h"
#include "RYLGameScene.h"

#include "RYLNetworkData.h"
#include "RYLGameData.h"
#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h"
#include "RYLClientMain.h"

#include "GameHandler.h"
#include "AuthHandler.h"

#include "RYLCreatureManager.h"

#include "RYLIntegServerSelCharDlg.h"
#include "RYLIntegServerSelVaultDlg.h"
#include "RYLPasswordReAuthDlg.h"
#include "RYLChinaUnifiedServerSelCharDlg.h"
#include "RYLCharacterDeleteDlg.h"
#include "GMMemory.h"

#define DISPATCH_FUNCTION(funcName) bool funcName(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase)


// 전방 참조
DISPATCH_FUNCTION(ParseAuthAccount);
DISPATCH_FUNCTION(ParseUserLogin);
DISPATCH_FUNCTION(ParseCharSelect);
DISPATCH_FUNCTION(ParseCharCreate);
DISPATCH_FUNCTION(ParseCharDelete);
// WORK_LIST 2.1 계정 국적 추가
DISPATCH_FUNCTION(ParseSelectAccountNation);
// WORK_LIST 2.3 계정 국적 변경 기능 구현
DISPATCH_FUNCTION(ParseNationChangeResult);
DISPATCH_FUNCTION(ParseAuthServerZone);

// 서버통합 관련해서 넣음
DISPATCH_FUNCTION(ParseUnifiedCharInfo);
DISPATCH_FUNCTION(ParseUnifiedCharSelectAck);


CAuthEventHandler::CAuthEventHandler()
{


}

CAuthEventHandler::~CAuthEventHandler()
{

}


int CAuthEventHandler::OnOpen(int iErrorCode)
{
	CRYLGameData* pGame = CRYLGameData::Instance();

	pGame->m_csStatus.Destroy();
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);
	g_MessageBoxManager.DestroyList();

	if ( iErrorCode != 0 )
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[1500]);
		return 0;
    }

    g_pClientSocket->SetHandler(this, ClientSocket::AuthEventHandler);

	if ( pGame->m_cReLogin==CRYLGameData::CHAR_SELECT)
	{
		g_pClientSocket->SetStatusFlag( NS_AUTHACCOUNT );
		CRYLNetworkData::Instance()->SendLoginID( CRYLNetworkData::Instance()->m_strLoginID );

		SendPacket::AuthAccount( *g_pClientSocket, 
								CRYLNetworkData::Instance()->m_strLoginID, 
								CRYLNetworkData::Instance()->m_strLoginPass, 
								g_ClientMain.m_dwClientVersion, 
								g_ClientMain.m_dwVersion, 
								g_ClientMain.m_wClientLoginFlag );
	}

	
	return 0;
}


int CAuthEventHandler::OnClose()
{    
	if (!IsCloseCalled())
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[1501]);
		PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
	}
	
    g_pClientSocket->ResetHandler(this, ClientSocket::AuthEventHandler);
    return 0;
}

int CAuthEventHandler::OnDispatch(PktBase* lpPktBase, INET_Addr& peerAddr)
{
	bool rc = false;
	
    //by Hades Kang
	int nCmd = lpPktBase->GetCmd();
	ClientSocket::SetNSFlagOff( nCmd );

	switch ( nCmd )
	//switch (lpPktBase->GetCmd())
	{
	case CmdAuthAccount:			rc = ParseAuthAccount(this, lpPktBase);		break;
	case CmdUserLogin:				rc = ParseUserLogin(this, lpPktBase);			break;
	case CmdCharSelect:				rc = ParseCharSelect(this, lpPktBase);		break;
	case CmdCharCreate:				rc = ParseCharCreate(this, lpPktBase);		break;
	case CmdCharDelete:				rc = ParseCharDelete(this, lpPktBase);        break;

        // WORK_LIST 2.1 계정 국적 추가
	case CmdSelectAccountNation:	rc = ParseSelectAccountNation(this, lpPktBase);   break;

        // WORK_LIST 2.3 계정 국적 변경 기능 구현
	case CmdNationChangeResult:		rc = ParseNationChangeResult(this, lpPktBase);    break;
	case CmdServerZone:				rc = ParseAuthServerZone(this, lpPktBase);        break;

        // 캐릭터 통합 선택 관련해서 넣음.
    case CmdUnifiedCharInfo:        rc = ParseUnifiedCharInfo(this, lpPktBase);       break;
    case CmdUnifiedCharSelect:      rc = ParseUnifiedCharSelectAck(this, lpPktBase);  break;

	default:		
		break;
	};

	if (CRYLNetworkData::Instance()->m_dwNetworkError == 1)	//1 = 서버 에러 
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[296]);
	}

	return 0;
//	return rc ? 0 : -1;
}


DISPATCH_FUNCTION(ParseAuthAccount)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

    CRYLGameData*		pGame		= CRYLGameData::Instance();
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance();

	unsigned long dwUserID			= 0;
    unsigned char cFirstLogin		= 0;

	pNetwork->m_dwNetworkError = ParsePacket::HandleAuthAccount(
        lpPktBase, dwUserID, pNetwork->m_cAgentServerType, cFirstLogin);	

	if (0 == pNetwork->m_dwNetworkError)
	{
		CRYLLoginScene* pScene = static_cast< CRYLLoginScene* >(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) );
		CRYLIntegServerSelVaultDlg* pSelVaultDlg = static_cast< CRYLIntegServerSelVaultDlg* >( pScene->GetIntegServerSelVaultDlg() );
		CRYLChinaUnifiedServerSelCharDlg* pChinaSelCharDlg = static_cast< CRYLChinaUnifiedServerSelCharDlg* >( pScene->GetChinaUnifiedServerSelCharDlg() );

		if ( ( cFirstLogin & UnifiedConst::SELECTED_PART2 ) &&
			pNetwork->m_eInternationalCode == GameRYL::KOREA )
		{
			pSelVaultDlg->SetVaultEnable( true );
		}

		pNetwork->m_dwUserID = dwUserID;
		pScene->SetAuthSuccess( TRUE );

        if (UnifiedConst::ROW == pNetwork->m_cAgentServerType)
        {
			g_pClientSocket->SetStatusFlag(NS_USERLOGIN);		
            SendPacket::UserLogin( lpHandler, pNetwork->m_dwUserID, pGame->m_cReLogin );
        }
        else
        {
			if ( pNetwork->m_eInternationalCode == GameRYL::KOREA )
			{ 
				pSelVaultDlg->SetUnifiedStatus(pNetwork->m_cAgentServerType, cFirstLogin);
			}
			else
			{
				pChinaSelCharDlg->SetUnifiedStatus(pNetwork->m_cAgentServerType, cFirstLogin);
			}
        }

		return true;
	} 
	else
	{
		g_MessageBoxManager.DestroyList();

		CRYLMessageBox *lpMessageBox;
		switch (pNetwork->m_dwNetworkError)
		{
			case 3: // 체크섬 실패
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[429]);
//				lpMessageBox->Create(CRYLStringTable::m_strString[429], MB_CONFIRM | MB_EXIT);
//				lpMessageBox->SetResult( &pGame->m_dwClientExitConfirm );
//				lpMessageBox->SetEnableEnter( FALSE );
//				pGame->m_dwClientExitConfirm = 0;
				break;

			case 4: // 잘못된 버젼 클라이언트
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[430]);
//				lpMessageBox->Create(CRYLStringTable::m_strString[430], MB_CONFIRM | MB_EXIT);
//				lpMessageBox->SetResult( &pGame->m_dwClientExitConfirm );
//				lpMessageBox->SetEnableEnter( FALSE );
//				pGame->m_dwClientExitConfirm = 0;
				break;

			case 5: // 잘못된 계정/패스
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[431]);
				break;

			case 6: // 인증 실패
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[437]);
				break;

			case 7: // 인증 인원 초과
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[438]);
				break;

			case 8: // 블롯된 계정
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[439]);
				break;
			case 43:	// 이관 동의에 대한 질의를 처음 하는자.
				{
					CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) );
					pScene->SetPCCheckAgreeMode(TRUE);
				}
				break;
			case 10: // 기타 인증 에러
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[437]);
				break;

			case 11: // 등록이 되어 있지 않거나 암호가 틀림
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[440]);
				break;

			case 12: // 릴사용 동의된 사용자가 아님
				if ( pGame )
				{
					CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) );
					pScene->SetAgreeMode( TRUE );
				}
				break;

			case 13: // 한게임 사용자가 아니거나 비정상적인 사용자
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[441]);
//				lpMessageBox->Create(CRYLStringTable::m_strString[441], MB_CONFIRM | MB_EXIT);
//				lpMessageBox->SetResult( &pGame->m_dwClientExitConfirm );
//				lpMessageBox->SetEnableEnter( FALSE );
//				pGame->m_dwClientExitConfirm = 0;
				break;

			case 14: // 릴사용 동의한 사용자이지만 게임을 할 수 없는 상태
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[442]);
//				lpMessageBox->Create(CRYLStringTable::m_strString[442], MB_CONFIRM | MB_EXIT);
//				lpMessageBox->SetResult( &pGame->m_dwClientExitConfirm );
//				lpMessageBox->SetEnableEnter( FALSE );
//				pGame->m_dwClientExitConfirm = 0;
				break;

			case 15: // 15세 이하 유저
				lpMessageBox = new CRYLMessageBox;
				if (GameRYL::SERVER_BATTLE_LOHAN == pNetwork->m_eServerType)
				{
					// 배틀 로한은 12세 이용가
					lpMessageBox->Create(CRYLStringTable::m_strString[2277]);
				}
				else
				{
					lpMessageBox->Create(CRYLStringTable::m_strString[443]);
				}
				break;

			case 16: // 인증 시도 3회 실패
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2942], MB_CONFIRM | MB_EXIT);
				lpMessageBox->SetResult( &pGame->m_dwExecuteExplorerConfirm );
				lpMessageBox->SetEnableEnter( FALSE );
				pGame->m_dwExecuteExplorerConfirm = 0;
				break;

			case 20: // 등록이 되어 있지 않거나 암호가 틀림
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[440]);
				break;

			case 21: // UID 값 이상
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[437]);
				break;

			case 22: // 미설정된 포인트 량
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[444]);
				break;

			case 23: // 로그인 인증 오류
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[437]);
				break;

			case 24: // 포인트 카드 사용자가 아님
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[445]);
				break;

			case 25: // 남은 포인트가 없거나 음수 데이터
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[444]);
				break;

			case 26: // 남은 포인트 부족
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[444]);
				break;

			case 27: // 유효 기간 오류
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[446]);
				break;

			case 28: // 포인트 결제 오류
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[447]);
				break;

			case 29: // 정액 사용기간이 시작되지 않았음
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[448]);
				break;

			case 30: // 정액 사용기간 종료
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[446]);
				break;

			case 31: // 정액 결제 오류
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[449]);
				break;
			case 32: // 유효기간 종료
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4067]);
				break;

			case 40: // 세션 오픈 에러
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[432]);
				break;

			case 41: // 세션 열려 있음
				lpMessageBox = new CRYLMessageBox;

				if(CRYLSceneManager::Instance()->GetCurrentScene()->GetSceneID()==GAME_SCENE)
				{
					lpMessageBox->Create(CRYLStringTable::m_strString[433], MB_CONFIRM);
					lpMessageBox->SetTimerDisable(TRUE);
				}
				else
				{
					lpMessageBox->Create(CRYLStringTable::m_strString[433], MB_YES | MB_NO | MB_EXIT);
				}

				lpMessageBox->SetResult( &pGame->m_dwClientLoginResult );
				lpMessageBox->SetEnableEnter( FALSE );
				pGame->m_dwClientLoginResult = 0;
				break;

			case 42: // 세션 종료 시킴
				Sleep(5000);
				g_ClientMain.m_wClientLoginFlag = 0;
				g_pClientSocket->SetStatusFlag(NS_AUTHACCOUNT);
				pNetwork->SendLoginID(pNetwork->m_strLoginID);
				SendPacket::AuthAccount(*g_pClientSocket, pNetwork->m_strLoginID, pNetwork->m_strLoginPass, 
					g_ClientMain.m_dwClientVersion, g_ClientMain.m_dwVersion, g_ClientMain.m_wClientLoginFlag);
				break;

				// 50~70 감마니아 오류
			case 49:	// 등록되지 않은 감마니아오류
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4506]);
				break;
			case 50:	// 블럭된 계정
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[439]);
				break;
			case 51:	// 미설정된 서비스코드
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4500]);
				break;
			case 52:	// PKI로 인해 서브스 계정 블록
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[439]);
				break;
			case 53:	// 연산 타임아웃 오류
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4501]);
				break;
			case 54:	// 아이디 패스워드 오류
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[431]);
				break;
			case 55:	// 아이디나 비번에 사용 불가능 캐릭터가 있음
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[442]);
				break;
			case 56:	// 비번암호시 에러 발생
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4502]);
				break;
			case 57:	// 현재 이용할수 없는 서비스 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4503]);
				break;
			case 58:	// 인증을 위해 대기중
			case 59:	// 인증을 위해 대기중
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4504]);
				break;
			case 60:	// 인증타입오류
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4505]);
				break;
			case 61:	// 예외오류
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4506]);
				break;
			case 62:	// 등록되지 않은 계정
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4507]);
				break;
			case 63:	// 패스워드 오류
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[431]);
				break;


			case 100:	// 인도네시아 인증 에러.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[3912]);
				break;
		}
	}

	return false;
}



DISPATCH_FUNCTION(ParseUserLogin)
{	
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	UserID = 0, dwSessionID = 0;
	unsigned short	wAdminLevel	= 0;
	unsigned char	cBillingType = 0, cBillingUser = 0;
	unsigned char   cLoginType_Out= 0;
	unsigned char	cAccountNation = 0;
	unsigned long	dwFlag = 0;
	CHAR_VIEW		scLoginChar[ MAX_CHAR_SLOT ];	// 로그인 캐릭터 정보(5명)
	sGuildData		scLoginCharGuildData[ MAX_CHAR_SLOT ];

	int		nPlayTime = 0;
	time_t	ttCurrentTime = 0;
	CRYLGameData* pGame = CRYLGameData::Instance();

	// WORK_LIST 2.1 계정 국적 추가
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleUserLogin(lpPktBase, &dwSessionID, &UserID, 
		scLoginChar, scLoginCharGuildData, &cAccountNation, &ttCurrentTime, &nPlayTime,
		&wAdminLevel, &cBillingType, &cBillingUser, &cLoginType_Out, &dwFlag);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		g_ClientMain.m_wAdminMode	= 0/*wAdminLevel*/;//hz adminlevel
		pGame->m_cAccountNation		= cAccountNation;
		if(dwFlag & PktDepositUpdateDB::USED_DEPOSIT)
			pGame->m_bUsedDeposit = TRUE;
		else
			pGame->m_bUsedDeposit = FALSE;
		memcpy(pGame->m_scLoginChar, scLoginChar, sizeof(CHAR_VIEW) * MAX_CHAR_SLOT);
		memcpy(pGame->m_scLoginCharGuildData, scLoginCharGuildData, sizeof(sGuildData) * MAX_CHAR_SLOT);
		
		CRYLNetworkData::Instance()->m_dwSessionID	= dwSessionID;
		pGame->m_nPlayTime	= nPlayTime;
		CRYLNetworkData::Instance()->m_cBillingType	= cBillingType;
		CRYLNetworkData::Instance()->m_cBillingUser	= cBillingUser;

		if (CRYLNetworkData::Instance()->m_cBillingType == 'D' || 
            CRYLNetworkData::Instance()->m_cBillingType == 'N')
		{
			if (nPlayTime > 0) 
			{
				nPlayTime--;
			}

            // TODO : Deplecated : GRYLTimer.m_ttCurrentTime.. 아무데서도 안 쓰는 거 같은데, 로컬변수로 치환..
			GRYLTimer.m_ttCurrentTime = (ttCurrentTime + nPlayTime * 24 * 60 * 60);

			if ( GRYLTimer.m_ttCurrentTime > 0)
			{
				tm *lpEndTime = localtime(&GRYLTimer.m_ttCurrentTime);
			
				if (lpEndTime)
				{
					GRYLTimer.m_tmEndTime = *lpEndTime;
				} 
				else
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[435]);
					PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
					pGame->m_dwClientMode = CLIENT_END;
					return true;
				}
			} 
			else
			{
				memset(&GRYLTimer.m_tmEndTime, 0, sizeof(GRYLTimer.m_tmEndTime));
			}
		}
        else if (CRYLNetworkData::Instance()->m_cBillingType == 'E' ||  // E는 D와 유사품.
            CRYLNetworkData::Instance()->m_cBillingType == 'F')         // F는 N과 유사품.
        {
            // nPlayTime은 20040501과 같은 식으로 온다. 만료일자는 여기서 1초를 뺀 값이다.
            tm BillingTime;
            memset(&BillingTime, 0, sizeof(BillingTime));

            BillingTime.tm_year  = int(nPlayTime / 10000) - 1900;
            BillingTime.tm_mon   = int(nPlayTime / 100) - int((BillingTime.tm_year + 1900) * 100) - 1;
            BillingTime.tm_mday  = nPlayTime - int(int(nPlayTime / 100) * 100);

            time_t tBillingTime = mktime(&BillingTime);
            --tBillingTime;

            tm* lpEndTime = localtime(&tBillingTime);
            if(lpEndTime)
            {
                GRYLTimer.m_tmEndTime = *lpEndTime;
            }
            else
            {
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[435]);
				PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
				pGame->m_dwClientMode = CLIENT_END;
				return true;
            }
        }		

		if ( pGame )
		{
			CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) );
			pScene->FadeOut();

			// WORK_LIST 2.3 계정 국적 변경 UI 추가
			CRYLCharacterSelectScene* pCharSelectScene = ( CRYLCharacterSelectScene* )(CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) );
			pCharSelectScene->SetCurrentNation( pGame->m_cAccountNation );
		}

		// ----------------------------------------------------------------------------------------------------------
		// 캐릭터 선택화면으로 옮길때...

		if ( cLoginType_Out==CRYLGameData::CHAR_SELECT || pGame)
		{
			CRYLGameScene*		pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
			
			pScene->SceneHide();
//			pScene->GameSceneDelete();

			pGame->m_bInterfaceAlreadyLoad = FALSE;
            RYLCreatureManager::Instance()->ClearCreatureManager();
			pGame->m_lpSceneManager->BaseGraphicsDataUnload();
			pGame->m_lpSceneManager->DeleteAllData();
			GM_DELETE( pGame->m_lpInterface );

			CRYLNetworkData::Instance()->m_dwMyChrID = 0L;
			pGame->m_lpPickItem = NULL;
			pGame->EnterClient();
		}

		return true;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					//2 = 데이터 얻기 실패 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[297]);
				break;

			case 3:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[298]);
				PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
				pGame->m_dwClientMode = CLIENT_END;
				break;

			case 4:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[386]);
				break;

			case 5:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[387]);
				break;
		}
	}

	return false;
}

DISPATCH_FUNCTION(ParseCharSelect)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	const unsigned int MAX_CHANNEL = 5;

	unsigned short	aryChannel[MAX_CHANNEL] = { 0, };
	unsigned char	cZone = 0;
	CRYLGameData*	pGame = CRYLGameData::Instance();

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSelect(lpPktBase, &cZone, aryChannel);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		unsigned long dwChannelCount = 0;

		for (unsigned long i = 0; i < MAX_CHANNEL; i++)
		{
			if (aryChannel[i])
			{
				dwChannelCount++;
			}
		}

		CRYLNetworkData::Instance()->m_dwNextZone = cZone;

		if (0 == dwChannelCount)
		{
			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[482]);
		} 
		else if (1 == dwChannelCount)
		{
			for (unsigned long i = 0; i < MAX_CHANNEL; i++)
			{
				if (aryChannel[i])
				{
					pGame->m_dwClientMode = CLIENT_ZONEMOVE;			
					g_pClientSocket->SetStatusFlag(NS_ZONEMOVE);

					SendPacket::ServerZone(lpHandler, CRYLNetworkData::Instance()->m_dwNextZone, i);
					break;
				}
			}
		} 
		else
		{
			if (pGame->m_lpInterface)
			{
				if (pGame->m_dwClientMode == CLIENT_CHARACTERSELECT)
				{
					pGame->m_dwClientState = CHRSELECT_SELECTCHANNEL;
				}
			}
		}

		memset( CRYLNetworkData::Instance()->m_aryChannel, 0, sizeof( unsigned short ) * MAX_CHANNEL );
		CRYLNetworkData::Instance()->m_aryChannelNum = 0;

		memcpy( CRYLNetworkData::Instance()->m_aryChannel, aryChannel, sizeof( unsigned short ) * MAX_CHANNEL );
		CRYLNetworkData::Instance()->m_aryChannelNum = dwChannelCount;

		CRYLGameScene*		pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
		
		pScene->InitResourceByRace();

		return true;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// 2 = 서버 포화
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[365]);
				break;

			case 3:					// 3 = 캐릭터 로드 실패
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[370]);
				break;

/*
			case 4:					// 4 = 서버 찾기 실패
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[371]);
				break;
*/

			case 5:					// 5 = 캐릭터 블록
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[480]);
				break;
		}
	}

	return false;
}


DISPATCH_FUNCTION(ParseCharCreate)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	ChrID = 0, Slot = 0;
	CHAR_VIEW		CharView;
	CRYLGameData*	pGame = CRYLGameData::Instance();

	memset(&CharView, 0, sizeof(CHAR_VIEW));

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCreate(lpPktBase, &ChrID, &Slot, &CharView);

	g_MessageBoxManager.DestroyList();

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		pGame->m_scLoginChar[Slot] = CharView;
		return true;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:		// 2 = 데이터 얻기 실패 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[299]);
				break;

			case 3:		// 3 = 존재하는 캐릭터 이름 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[300]);
				break;

			case 4:		// 4 = 비어있지 않은 슬롯 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[301]);
				break;

			case 5:		// 5 = 적당하지 않은 이름 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[302]);
				break;

			case 6:		// 6 = 잘못된 생성 데이터 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[303]);
				break;

			case 7:		// 7 = 금지된 단어 들어간 캐릭터 이름
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[304]);
				break;

			case 8:		// 8 = 세션이 열려 있지 않음
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[388]);
				break;

			case 9:		// 9 = 캐릭터 생성후 view 재로드 실패
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[296]);
				break;

			case 10:	// 10 = 캐릭터 생성할 수 없음 (서버에서 막아놓은 상태)
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2756]);
				break;
		}
	}

	return false;
}


DISPATCH_FUNCTION(ParseCharDelete)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long UserID = 0, Slot = 0;
	CRYLGameData* pGame = CRYLGameData::Instance();
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharDelete(lpPktBase, &UserID, &Slot);

	g_MessageBoxManager.DestroyList();

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene * )CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE );
		CRYLCharacterDeleteDlg*	pCharacterDeleteDlg = ( CRYLCharacterDeleteDlg* )pScene->GetCharacterDeleteDlg();
		pCharacterDeleteDlg->SetVisible( FALSE );

		if (UserID == CRYLNetworkData::Instance()->m_dwUserID)
		{
			memset(&pGame->m_scLoginChar[Slot], 0, sizeof(CHAR_VIEW));
			memset(&pGame->m_scLoginCharGuildData[Slot], 0, sizeof(sGuildData));
		}
		return true;
	} 
	else
	{
		CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene * )CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE );
		CRYLCharacterDeleteDlg*	pCharacterDeleteDlg = ( CRYLCharacterDeleteDlg* )pScene->GetCharacterDeleteDlg();
		pCharacterDeleteDlg->m_pPasswordEdit->SetText("");

		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// 2 = 데이터 삭제 실패 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[305]);
				break;

			case 3:					// 3 = 이미 로그인 삭제
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[306]);
				break;

			case 4:					// 4 = 세션이 열려 있지 않음
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[388]);
				break;

			case 5:					// 5 = 캐릭터 삭제는 성공했으나, 계정 국적 초기화는 실패했음
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2757]);
				break;

			case 6:					// 6 = 통합서버 관련되어 캐릭터를 지울수 없을경우
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2944]);
				break;

			case 7:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[475]);
				pCharacterDeleteDlg->InitCaption(2);	// 오류 메세지
				break;
		}
	}

	return false;
}

// WORK_LIST 2.1 계정 국적 추가
DISPATCH_FUNCTION(ParseSelectAccountNation)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long UserID = 0;
	unsigned char cType = 0;
	unsigned char cAccountNation = 0;
	CRYLGameData* pGame = CRYLGameData::Instance();
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleSelectAccountNation(lpPktBase, &UserID, &cType, &cAccountNation);

	g_MessageBoxManager.DestroyList();

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (UserID == CRYLNetworkData::Instance()->m_dwUserID)
		{
			pGame->m_cAccountNation = cAccountNation;

			// WORK_LIST 2.3 계정 국적 변경 UI 추가
			CRYLCharacterSelectScene* pCharSelectScene = ( CRYLCharacterSelectScene* )(CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) );
			pCharSelectScene->SetCurrentNation( cAccountNation );

			// 캐릭터 데이터에 국적도 수정.
			for (int i = 0; i < MAX_CHAR_SLOT; i++)
			{
				if (pGame->m_scLoginChar[i].CID)
				{
					CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData(pGame->m_scLoginChar[i].CID);

					if(pData)
					{
						pData->m_cNation = cAccountNation;
					}
				}
			}
		}

		return true;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// 2 = 계정 국적 설정 실패 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2758 ]);
				break;
		}
	}

	return false;
}

// WORK_LIST 2.3 계정 국적 변경 기능 구현
DISPATCH_FUNCTION(ParseNationChangeResult)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long UserID = 0;
	unsigned long aryGID[ MAX_CHAR_SLOT ];
	unsigned long aryFame[ MAX_CHAR_SLOT ];
	CRYLGameData* pGame = CRYLGameData::Instance();
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleNationChangeResult(lpPktBase, &UserID, aryGID, aryFame);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (UserID == CRYLNetworkData::Instance()->m_dwUserID)
		{
			for (int i=0; i<MAX_CHAR_SLOT; ++i)
			{
				if (0 != pGame->m_scLoginChar[i].CID)
				{
					if (0 != pGame->m_scLoginChar[i].GID && pGame->m_scLoginChar[i].GID != aryGID[i])
					{
						pGame->m_scLoginChar[i].GID = aryGID[i];
						
						pGame->m_scLoginCharGuildData[i].m_cNation = 0;
						strcpy( pGame->m_scLoginCharGuildData[i].m_szName, "" );
					}

					// 명성은 길드 가입 여부에 상관없이 업데이트 된다.
					pGame->m_scLoginChar[i].Fame = aryFame[i];
				}
			}
		}
	
		return true;
	} 

	return false;
}

DISPATCH_FUNCTION(ParseAuthServerZone)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long dwServerID = 0;
	CRYLGameData* pGame = CRYLGameData::Instance();

	// 이 안에서 ClientSocket::MoveZoneAddr에 주소를 세팅한다.
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleServerZone(lpPktBase, *g_pClientSocket, &dwServerID);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		SERVER_ID siServer;
		siServer.dwID = dwServerID;

		CRYLNetworkData::Instance()->m_dwChannelID		= siServer.GetChannel();
		CRYLNetworkData::Instance()->m_dwPresentZone	= siServer.GetZone();

		pGame->FadeIn();
		pGame->m_bSelectChar = TRUE;

		g_pClientSocket->SetStatusFlag(NS_SERVERMOVE);
		
		// 채널 닫기
		{
			CRYLCharacterSelectScene* pScene = static_cast<CRYLCharacterSelectScene*>( CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) );
			pScene->GetChannelDlg()->SetVisible( FALSE );
		}

		// 세션을 닫는다.		
        g_pClientSocket->Disconnect(ClientSocket::AuthEventHandler);
		
		// TODO : GameServer 와의 컨넥션 만들기. 
		// ClientSocket::MoveZoneAddr에는 연결할 서버 주소가 들어 있다.
        ClientNet::CClientEventHandler *lpGameHandler = CGameEventHandler::Create();
		g_pClientSocket->Open(g_pClientSocket->GetAddressString(ClientSocket::MoveZoneAddr),
			g_pClientSocket->GetAddressPort(ClientSocket::AddressType::MoveZoneAddr), lpGameHandler);

		return true;
	} 
	else
	{
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// 2 = 캐릭터 존이 잘못 되어 있습니다.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[389]);
				break;

			case 4:					// 4 = 인원이 초과
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[390]);
				break;

			case 5:					// 5 = 세션이 열려 있지 않음
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[388]);
				break;
		}
	}

	return false;
}


DISPATCH_FUNCTION(ParseUnifiedCharInfo)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

    USER_INFO			UserInfo;
    UnifiedStoreInfo*	StoreInfo = 0;
    UnifiedCharData*	CharData = 0;

	unsigned long		dwStoreInfoNum_Out			= 0;
    unsigned long		dwCharDataNum_Out			= 0;
	unsigned char		cRestrictedPart1ToPart2Level = 0; 
	unsigned char		cRemainCharTransferCount = 0;

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleUnifiedCharInfo( lpPktBase, 
																						cRestrictedPart1ToPart2Level,
																						cRemainCharTransferCount,
																						UserInfo, 
																						&StoreInfo, 
																						dwStoreInfoNum_Out, 
																						&CharData, 
																						dwCharDataNum_Out );
 
    if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
    {
        CRYLLoginScene* pScene = static_cast< CRYLLoginScene* >(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) );
		
		if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA )
		{
			if( FALSE == pScene->GetAgreeMode() )
			{		
                // 다시 로그인하는 거면 그냥 로그인한다.
				if ( CRYLGameData::Instance()->m_cReLogin )
				{
					g_pClientSocket->SetStatusFlag(NS_USERLOGIN);		
					SendPacket::UserLogin( lpHandler, CRYLNetworkData::Instance()->m_dwUserID,
										CRYLGameData::Instance()->m_cReLogin );
					return true;
				}

				CRYLIntegServerSelCharDlg* pSelCharDlg = 
                    static_cast< CRYLIntegServerSelCharDlg* >( pScene->GetIntegServerSelCharDlg() );

				CRYLIntegServerSelVaultDlg* pSelVaultDlg = 
                    static_cast< CRYLIntegServerSelVaultDlg* >( pScene->GetIntegServerSelVaultDlg() );
				
				bool bFirstLoginEnable = 
                    !(pSelVaultDlg->GetFirstLogin() & UnifiedConst::SELECTED_PART2);

                // 창고 선택 여부 세팅.
				pSelCharDlg->SetFirstLoginEnable( bFirstLoginEnable );
				pScene->GetLoginDlg()->SetVisible( FALSE );

				pSelCharDlg->SetCharInfo(&UserInfo, CharData, dwCharDataNum_Out, cRestrictedPart1ToPart2Level );
				pSelVaultDlg->SetVaultInfo( StoreInfo, dwStoreInfoNum_Out );

                // 종족 검색
                unsigned short sCnt = 0;
                CClass::RaceType eRace = CClass::MAX_RACE;

                for ( int i = 0; i < USER_INFO::MAX_CHAR_NUM; i ++ )
                {
                    if ( 0 != UserInfo.CharID[ i ] ) 
                    {
                        if ( eRace == CClass::MAX_RACE )
                        {
                            // 이미 창고 선택한 적 있음. 최초로그인이 아니다.
                            UnifiedCharData* lpCharDataPos = CharData;
                            UnifiedCharData* lpCharDataEnd = CharData + dwCharDataNum_Out;

                            for(; lpCharDataPos != lpCharDataEnd; ++lpCharDataPos)
                            {
                                if ( lpCharDataPos->dwNewCID == UserInfo.CharID[ i ] )
                                {
                                    eRace = CClass::GetRace( lpCharDataPos->sClass );
                                    break;
                                }
                            }
                        }

                        ++sCnt;
                    }
                }

				if ( !bFirstLoginEnable )
				{
                    // 창고 이미 선택한 경우임
                    unsigned short sNotSelectedCharNum = 0;

                    // 선택한 캐릭터와 같은 종족이고, UserInfo에 없는 캐릭터가 있는지 살핀다.
                    UnifiedCharData* lpCharDataPos = CharData;
                    UnifiedCharData* lpCharDataEnd = CharData + dwCharDataNum_Out;

                    for(; lpCharDataPos != lpCharDataEnd; ++lpCharDataPos)
                    {
                        if ( (eRace == CClass::MAX_RACE || eRace == CClass::GetRace( lpCharDataPos->sClass ) ) && 
                            !UserInfo.HasCharacter( lpCharDataPos->dwNewCID ) )
                        {
                            ++sNotSelectedCharNum;
                        }
                    }

                    // 캐릭터가 전부 찼거나, 
                    // 선택할 수 있는  캐릭터(같은 종족이면서 선택하지 않은 캐릭터)가 없는 경우
                    if ( sCnt == USER_INFO::MAX_CHAR_NUM || 0 == sNotSelectedCharNum)                    
                    {
					    CRYLMessageBox* lpMessageBox = NULL;
					    lpMessageBox = new CRYLMessageBox;
					    lpMessageBox->Create( CRYLStringTable::m_strString[ 3397 ] );

					    g_pClientSocket->SetStatusFlag(NS_USERLOGIN);		
					    SendPacket::UserLogin( lpHandler, CRYLNetworkData::Instance()->m_dwUserID,
										    CRYLGameData::Instance()->m_cReLogin );

                        return true;
                    }
				}

				pSelCharDlg->UpdateDlg();
				pSelVaultDlg->UpdateDlg();

				if ( bFirstLoginEnable )
				{
                    // 창고 선택해야 하는 경우.
					pScene->FirstLoginMsgBox();
				}
				else
				{
					// 캐릭터만 선택하는 경우.
					pScene->AddUnifiedCharMsg();
				}
				
				pSelCharDlg->SetRemainCharTransferCount( cRemainCharTransferCount );
				pSelCharDlg->UpdateRemainCharTransferCount();
			}
		}
		else 
		{
            // 한국 아닌 국가는 전부 중국식통합이다.			
			CRYLChinaUnifiedServerSelCharDlg* pSelCharDlg = 
                static_cast< CRYLChinaUnifiedServerSelCharDlg* >( pScene->GetChinaUnifiedServerSelCharDlg() );

            // 다시 로그인하는 경우는 그냥 로그인
			if ( CRYLGameData::Instance()->m_cReLogin )
			{
				pSelCharDlg->SendUnifiedCharSelect();
				return true;
			}

            pScene->GetLoginDlg()->SetVisible( FALSE );

            bool bNewUser = (dwStoreInfoNum_Out == 0) || 
                (dwStoreInfoNum_Out == 1 && StoreInfo[ 0 ].cOldServerGroupID == pSelCharDlg->m_cAgentServerType);

			// 창고가 없거나, 통합창고 하나만 있는 캐릭터는 신규 캐릭터이다.
			if ( bNewUser ) 
			{
                // 신규 로그인 / 재로그인
                g_pClientSocket->SetStatusFlag( NS_USERLOGIN );
				SendPacket::UserLogin( lpHandler, CRYLNetworkData::Instance()->m_dwUserID,
                    CRYLGameData::Instance()->m_cReLogin );

				return true;
			}
            
			// 서버군을 선택한다.
			pScene->AddUnifiedCharMsg();

            pSelCharDlg->SetVaultInfo( StoreInfo, dwStoreInfoNum_Out );
			pSelCharDlg->SetCharInfo(&UserInfo, CharData, dwCharDataNum_Out );
			
			pSelCharDlg->UpdateCharDlg();
			pSelCharDlg->UpdateVaultDlg();
		}

        return true;
    }
    else
    {
        // error
    }
    return false;
}

DISPATCH_FUNCTION(ParseUnifiedCharSelectAck)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

    CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleUnifiedCharSelectAck(lpPktBase);
    CRYLLoginScene* pScene = static_cast< CRYLLoginScene* >(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) );

    if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
    {
        // 이제부터는 정상 처리이다. UserLogin을 보내서 View를 다시 얻어 온다. 
		g_pClientSocket->SetStatusFlag(NS_USERLOGIN);		
        SendPacket::UserLogin( lpHandler, CRYLNetworkData::Instance()->m_dwUserID,
            CRYLGameData::Instance()->m_cReLogin );
 
		if ( !CRYLGameData::Instance()->m_cReLogin )
		{
			CRYLMessageBox* lpMessageBox = NULL;
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3397 ] );
		}
		else
		{
			CRYLMessageBox* lpMessageBox = NULL;
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3398 ] );
		}
    }
    else
    {
		// 에러 처리를 한다. 에러코드는 PktUnifiedCharSelectAck 패킷을 참고한다.
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
		case PktUnifiedCharSelectAck::SERVER_ERROR:                 
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3399 ] );
			break;
		case PktUnifiedCharSelectAck::PACKET_ERROR:                 
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3400 ] );
			break;

		case PktUnifiedCharSelectAck::PASSWORD_FAILED:              
			pScene->GetPasswordReAuthDlg()->SetVisible( TRUE );
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3401 ] );
			break;

		case PktUnifiedCharSelectAck::WRONG_STORE_SELECTED:         
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3402 ] );
			break;
		case PktUnifiedCharSelectAck::WRONG_CHAR_SELECTED:          
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3403 ] );
			break;

		case PktUnifiedCharSelectAck::UNIFIED_STORE_READ_ERROR:     
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3404 ] );
			break;
		case PktUnifiedCharSelectAck::UNIFIED_STORE_WRITE_ERROR:    
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3405 ] );
			break;

		case PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR:     
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3406 ] );
			break;

		case PktUnifiedCharSelectAck::CHAR_VIEW_RELOAD_ERROR:       
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3407 ] );
			break;
		default :
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3408 ] );
			break;
		}
    }

    return false;
}

