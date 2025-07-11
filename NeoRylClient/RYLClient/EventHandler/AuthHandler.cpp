
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
// WORK_LIST 2.2 NationSelectScene �̸� ����
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


// ���� ����
DISPATCH_FUNCTION(ParseAuthAccount);
DISPATCH_FUNCTION(ParseUserLogin);
DISPATCH_FUNCTION(ParseCharSelect);
DISPATCH_FUNCTION(ParseCharCreate);
DISPATCH_FUNCTION(ParseCharDelete);
// WORK_LIST 2.1 ���� ���� �߰�
DISPATCH_FUNCTION(ParseSelectAccountNation);
// WORK_LIST 2.3 ���� ���� ���� ��� ����
DISPATCH_FUNCTION(ParseNationChangeResult);
DISPATCH_FUNCTION(ParseAuthServerZone);

// �������� �����ؼ� ����
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

        // WORK_LIST 2.1 ���� ���� �߰�
	case CmdSelectAccountNation:	rc = ParseSelectAccountNation(this, lpPktBase);   break;

        // WORK_LIST 2.3 ���� ���� ���� ��� ����
	case CmdNationChangeResult:		rc = ParseNationChangeResult(this, lpPktBase);    break;
	case CmdServerZone:				rc = ParseAuthServerZone(this, lpPktBase);        break;

        // ĳ���� ���� ���� �����ؼ� ����.
    case CmdUnifiedCharInfo:        rc = ParseUnifiedCharInfo(this, lpPktBase);       break;
    case CmdUnifiedCharSelect:      rc = ParseUnifiedCharSelectAck(this, lpPktBase);  break;

	default:		
		break;
	};

	if (CRYLNetworkData::Instance()->m_dwNetworkError == 1)	//1 = ���� ���� 
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
			case 3: // üũ�� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[429]);
//				lpMessageBox->Create(CRYLStringTable::m_strString[429], MB_CONFIRM | MB_EXIT);
//				lpMessageBox->SetResult( &pGame->m_dwClientExitConfirm );
//				lpMessageBox->SetEnableEnter( FALSE );
//				pGame->m_dwClientExitConfirm = 0;
				break;

			case 4: // �߸��� ���� Ŭ���̾�Ʈ
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[430]);
//				lpMessageBox->Create(CRYLStringTable::m_strString[430], MB_CONFIRM | MB_EXIT);
//				lpMessageBox->SetResult( &pGame->m_dwClientExitConfirm );
//				lpMessageBox->SetEnableEnter( FALSE );
//				pGame->m_dwClientExitConfirm = 0;
				break;

			case 5: // �߸��� ����/�н�
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[431]);
				break;

			case 6: // ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[437]);
				break;

			case 7: // ���� �ο� �ʰ�
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[438]);
				break;

			case 8: // ��Ե� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[439]);
				break;
			case 43:	// �̰� ���ǿ� ���� ���Ǹ� ó�� �ϴ���.
				{
					CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) );
					pScene->SetPCCheckAgreeMode(TRUE);
				}
				break;
			case 10: // ��Ÿ ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[437]);
				break;

			case 11: // ����� �Ǿ� ���� �ʰų� ��ȣ�� Ʋ��
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[440]);
				break;

			case 12: // ����� ���ǵ� ����ڰ� �ƴ�
				if ( pGame )
				{
					CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) );
					pScene->SetAgreeMode( TRUE );
				}
				break;

			case 13: // �Ѱ��� ����ڰ� �ƴϰų� ���������� �����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[441]);
//				lpMessageBox->Create(CRYLStringTable::m_strString[441], MB_CONFIRM | MB_EXIT);
//				lpMessageBox->SetResult( &pGame->m_dwClientExitConfirm );
//				lpMessageBox->SetEnableEnter( FALSE );
//				pGame->m_dwClientExitConfirm = 0;
				break;

			case 14: // ����� ������ ����������� ������ �� �� ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[442]);
//				lpMessageBox->Create(CRYLStringTable::m_strString[442], MB_CONFIRM | MB_EXIT);
//				lpMessageBox->SetResult( &pGame->m_dwClientExitConfirm );
//				lpMessageBox->SetEnableEnter( FALSE );
//				pGame->m_dwClientExitConfirm = 0;
				break;

			case 15: // 15�� ���� ����
				lpMessageBox = new CRYLMessageBox;
				if (GameRYL::SERVER_BATTLE_LOHAN == pNetwork->m_eServerType)
				{
					// ��Ʋ ������ 12�� �̿밡
					lpMessageBox->Create(CRYLStringTable::m_strString[2277]);
				}
				else
				{
					lpMessageBox->Create(CRYLStringTable::m_strString[443]);
				}
				break;

			case 16: // ���� �õ� 3ȸ ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2942], MB_CONFIRM | MB_EXIT);
				lpMessageBox->SetResult( &pGame->m_dwExecuteExplorerConfirm );
				lpMessageBox->SetEnableEnter( FALSE );
				pGame->m_dwExecuteExplorerConfirm = 0;
				break;

			case 20: // ����� �Ǿ� ���� �ʰų� ��ȣ�� Ʋ��
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[440]);
				break;

			case 21: // UID �� �̻�
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[437]);
				break;

			case 22: // �̼����� ����Ʈ ��
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[444]);
				break;

			case 23: // �α��� ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[437]);
				break;

			case 24: // ����Ʈ ī�� ����ڰ� �ƴ�
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[445]);
				break;

			case 25: // ���� ����Ʈ�� ���ų� ���� ������
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[444]);
				break;

			case 26: // ���� ����Ʈ ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[444]);
				break;

			case 27: // ��ȿ �Ⱓ ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[446]);
				break;

			case 28: // ����Ʈ ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[447]);
				break;

			case 29: // ���� ���Ⱓ�� ���۵��� �ʾ���
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[448]);
				break;

			case 30: // ���� ���Ⱓ ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[446]);
				break;

			case 31: // ���� ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[449]);
				break;
			case 32: // ��ȿ�Ⱓ ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4067]);
				break;

			case 40: // ���� ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[432]);
				break;

			case 41: // ���� ���� ����
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

			case 42: // ���� ���� ��Ŵ
				Sleep(5000);
				g_ClientMain.m_wClientLoginFlag = 0;
				g_pClientSocket->SetStatusFlag(NS_AUTHACCOUNT);
				pNetwork->SendLoginID(pNetwork->m_strLoginID);
				SendPacket::AuthAccount(*g_pClientSocket, pNetwork->m_strLoginID, pNetwork->m_strLoginPass, 
					g_ClientMain.m_dwClientVersion, g_ClientMain.m_dwVersion, g_ClientMain.m_wClientLoginFlag);
				break;

				// 50~70 �����Ͼ� ����
			case 49:	// ��ϵ��� ���� �����Ͼƿ���
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4506]);
				break;
			case 50:	// ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[439]);
				break;
			case 51:	// �̼����� �����ڵ�
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4500]);
				break;
			case 52:	// PKI�� ���� ���꽺 ���� ���
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[439]);
				break;
			case 53:	// ���� Ÿ�Ӿƿ� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4501]);
				break;
			case 54:	// ���̵� �н����� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[431]);
				break;
			case 55:	// ���̵� ����� ��� �Ұ��� ĳ���Ͱ� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[442]);
				break;
			case 56:	// �����ȣ�� ���� �߻�
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4502]);
				break;
			case 57:	// ���� �̿��Ҽ� ���� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4503]);
				break;
			case 58:	// ������ ���� �����
			case 59:	// ������ ���� �����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4504]);
				break;
			case 60:	// ����Ÿ�Կ���
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4505]);
				break;
			case 61:	// ���ܿ���
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4506]);
				break;
			case 62:	// ��ϵ��� ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4507]);
				break;
			case 63:	// �н����� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[431]);
				break;


			case 100:	// �ε��׽þ� ���� ����.
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
	CHAR_VIEW		scLoginChar[ MAX_CHAR_SLOT ];	// �α��� ĳ���� ����(5��)
	sGuildData		scLoginCharGuildData[ MAX_CHAR_SLOT ];

	int		nPlayTime = 0;
	time_t	ttCurrentTime = 0;
	CRYLGameData* pGame = CRYLGameData::Instance();

	// WORK_LIST 2.1 ���� ���� �߰�
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

            // TODO : Deplecated : GRYLTimer.m_ttCurrentTime.. �ƹ������� �� ���� �� ������, ���ú����� ġȯ..
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
        else if (CRYLNetworkData::Instance()->m_cBillingType == 'E' ||  // E�� D�� ����ǰ.
            CRYLNetworkData::Instance()->m_cBillingType == 'F')         // F�� N�� ����ǰ.
        {
            // nPlayTime�� 20040501�� ���� ������ �´�. �������ڴ� ���⼭ 1�ʸ� �� ���̴�.
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

			// WORK_LIST 2.3 ���� ���� ���� UI �߰�
			CRYLCharacterSelectScene* pCharSelectScene = ( CRYLCharacterSelectScene* )(CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) );
			pCharSelectScene->SetCurrentNation( pGame->m_cAccountNation );
		}

		// ----------------------------------------------------------------------------------------------------------
		// ĳ���� ����ȭ������ �ű涧...

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
			case 2:					//2 = ������ ��� ���� 
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
			case 2:					// 2 = ���� ��ȭ
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[365]);
				break;

			case 3:					// 3 = ĳ���� �ε� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[370]);
				break;

/*
			case 4:					// 4 = ���� ã�� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[371]);
				break;
*/

			case 5:					// 5 = ĳ���� ���
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
			case 2:		// 2 = ������ ��� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[299]);
				break;

			case 3:		// 3 = �����ϴ� ĳ���� �̸� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[300]);
				break;

			case 4:		// 4 = ������� ���� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[301]);
				break;

			case 5:		// 5 = �������� ���� �̸� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[302]);
				break;

			case 6:		// 6 = �߸��� ���� ������ 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[303]);
				break;

			case 7:		// 7 = ������ �ܾ� �� ĳ���� �̸�
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[304]);
				break;

			case 8:		// 8 = ������ ���� ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[388]);
				break;

			case 9:		// 9 = ĳ���� ������ view ��ε� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[296]);
				break;

			case 10:	// 10 = ĳ���� ������ �� ���� (�������� ���Ƴ��� ����)
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
			case 2:					// 2 = ������ ���� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[305]);
				break;

			case 3:					// 3 = �̹� �α��� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[306]);
				break;

			case 4:					// 4 = ������ ���� ���� ����
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[388]);
				break;

			case 5:					// 5 = ĳ���� ������ ����������, ���� ���� �ʱ�ȭ�� ��������
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2757]);
				break;

			case 6:					// 6 = ���ռ��� ���õǾ� ĳ���͸� ����� �������
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[2944]);
				break;

			case 7:
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[475]);
				pCharacterDeleteDlg->InitCaption(2);	// ���� �޼���
				break;
		}
	}

	return false;
}

// WORK_LIST 2.1 ���� ���� �߰�
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

			// WORK_LIST 2.3 ���� ���� ���� UI �߰�
			CRYLCharacterSelectScene* pCharSelectScene = ( CRYLCharacterSelectScene* )(CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) );
			pCharSelectScene->SetCurrentNation( cAccountNation );

			// ĳ���� �����Ϳ� ������ ����.
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
			case 2:					// 2 = ���� ���� ���� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2758 ]);
				break;
		}
	}

	return false;
}

// WORK_LIST 2.3 ���� ���� ���� ��� ����
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

					// ���� ��� ���� ���ο� ������� ������Ʈ �ȴ�.
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

	// �� �ȿ��� ClientSocket::MoveZoneAddr�� �ּҸ� �����Ѵ�.
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
		
		// ä�� �ݱ�
		{
			CRYLCharacterSelectScene* pScene = static_cast<CRYLCharacterSelectScene*>( CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE ) );
			pScene->GetChannelDlg()->SetVisible( FALSE );
		}

		// ������ �ݴ´�.		
        g_pClientSocket->Disconnect(ClientSocket::AuthEventHandler);
		
		// TODO : GameServer ���� ���ؼ� �����. 
		// ClientSocket::MoveZoneAddr���� ������ ���� �ּҰ� ��� �ִ�.
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
			case 2:					// 2 = ĳ���� ���� �߸� �Ǿ� �ֽ��ϴ�.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[389]);
				break;

			case 4:					// 4 = �ο��� �ʰ�
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[390]);
				break;

			case 5:					// 5 = ������ ���� ���� ����
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
                // �ٽ� �α����ϴ� �Ÿ� �׳� �α����Ѵ�.
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

                // â�� ���� ���� ����.
				pSelCharDlg->SetFirstLoginEnable( bFirstLoginEnable );
				pScene->GetLoginDlg()->SetVisible( FALSE );

				pSelCharDlg->SetCharInfo(&UserInfo, CharData, dwCharDataNum_Out, cRestrictedPart1ToPart2Level );
				pSelVaultDlg->SetVaultInfo( StoreInfo, dwStoreInfoNum_Out );

                // ���� �˻�
                unsigned short sCnt = 0;
                CClass::RaceType eRace = CClass::MAX_RACE;

                for ( int i = 0; i < USER_INFO::MAX_CHAR_NUM; i ++ )
                {
                    if ( 0 != UserInfo.CharID[ i ] ) 
                    {
                        if ( eRace == CClass::MAX_RACE )
                        {
                            // �̹� â�� ������ �� ����. ���ʷα����� �ƴϴ�.
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
                    // â�� �̹� ������ �����
                    unsigned short sNotSelectedCharNum = 0;

                    // ������ ĳ���Ϳ� ���� �����̰�, UserInfo�� ���� ĳ���Ͱ� �ִ��� ���ɴ�.
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

                    // ĳ���Ͱ� ���� á�ų�, 
                    // ������ �� �ִ�  ĳ����(���� �����̸鼭 �������� ���� ĳ����)�� ���� ���
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
                    // â�� �����ؾ� �ϴ� ���.
					pScene->FirstLoginMsgBox();
				}
				else
				{
					// ĳ���͸� �����ϴ� ���.
					pScene->AddUnifiedCharMsg();
				}
				
				pSelCharDlg->SetRemainCharTransferCount( cRemainCharTransferCount );
				pSelCharDlg->UpdateRemainCharTransferCount();
			}
		}
		else 
		{
            // �ѱ� �ƴ� ������ ���� �߱��������̴�.			
			CRYLChinaUnifiedServerSelCharDlg* pSelCharDlg = 
                static_cast< CRYLChinaUnifiedServerSelCharDlg* >( pScene->GetChinaUnifiedServerSelCharDlg() );

            // �ٽ� �α����ϴ� ���� �׳� �α���
			if ( CRYLGameData::Instance()->m_cReLogin )
			{
				pSelCharDlg->SendUnifiedCharSelect();
				return true;
			}

            pScene->GetLoginDlg()->SetVisible( FALSE );

            bool bNewUser = (dwStoreInfoNum_Out == 0) || 
                (dwStoreInfoNum_Out == 1 && StoreInfo[ 0 ].cOldServerGroupID == pSelCharDlg->m_cAgentServerType);

			// â�� ���ų�, ����â�� �ϳ��� �ִ� ĳ���ʹ� �ű� ĳ�����̴�.
			if ( bNewUser ) 
			{
                // �ű� �α��� / ��α���
                g_pClientSocket->SetStatusFlag( NS_USERLOGIN );
				SendPacket::UserLogin( lpHandler, CRYLNetworkData::Instance()->m_dwUserID,
                    CRYLGameData::Instance()->m_cReLogin );

				return true;
			}
            
			// �������� �����Ѵ�.
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
        // �������ʹ� ���� ó���̴�. UserLogin�� ������ View�� �ٽ� ��� �´�. 
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
		// ���� ó���� �Ѵ�. �����ڵ�� PktUnifiedCharSelectAck ��Ŷ�� �����Ѵ�.
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

