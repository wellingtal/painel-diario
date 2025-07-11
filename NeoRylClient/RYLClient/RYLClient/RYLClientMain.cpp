
#include <process.h>

// 아래 지정된 플랫폼보다 우선하는 플랫폼을 대상으로 하는 경우 다음 정의를 수정하십시오.
#ifndef _WIN32_WINNT			// Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINNT 0x0501		// Windows XP
#endif
#include "RYLClientMain.h"
#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h"

#include "FrameTimer.h"
#include "WinInput.h"

#include <windows.h>
#include <mmsystem.h>

#include "dxutil.h"
#include "RenderOption.h"
#include "CEffscript.h"
#include "SceneManager.h"
#include "RYLMessageBoxManager.h"

#include "../MyFirewall/MyFirewall.h"
#include "../GUITextEdit.h"
#include "../StringFilter.h"
#include "../HumanStoneStatueRespawn.h"
#include "../resource.h"

#include "NPC/NPCList.h"

#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Network/ClientSocket/Send/SendAttack.h>
#include <Network/ClientSocket/Send/SendAuthServer.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/ClientSocket/Send/SendEtc.h>
#include <Network/ClientSocket/Send/SendLoginout.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendMove.h>
#include <Network/ClientSocket/Send/SendQuest.h>
#include <Network/ClientSocket/Send/SendCastle.h>

#include <Utility/Exception.h>
#include <Utility/CheckSum/Crc32Static.h>

#include <Creature/EnemyCheck.h>
#include <Creature/Monster/MonsterMgr.h>

#include <Item/ItemMgr.h>
#include <Skill/SkillMgr.h>
#include <GameGuardLib/NPGameLib.h>

#define USE_ANTIHACK 0
#include "..\\AntiHack.h"
DECLARE_ANTIHACK();

#include "Z3D_GLOBALS.h"
#include "Z3DGeneralChrModel.h"
#include "Z3DChrEventGenerator.h"
#include "Z3DManagedObject.h"

#include "GMBase.h"
#include "GMFont.h"
#include "GMUIMain.h"

#include "RYLGameData.h"
#include "RYLNetWorkData.h"
#include "RYLCommunityData.h"

#include "RYLProfile.h"
#include "RYLEdit.h"
#include "RYLTimer.h"
#include "RYLStringTable.h"
#include "RYLIMEView.h"

#include "RYLSceneManager.h"
#include "RYLSceneObject.h"
#include "RYLIntroScene.h"
#include "RYLLoginScene.h"
// WORK_LIST 2.2 NationSelectScene 이름 수정
#include "RYLRaceSelectScene.h"
#include "RYLCharacterSelectScene.h"
#include "RYLLoadingScene.h"
#include "RYLGameScene.h"
#include "RYLCastleManager.h"
#include "RYLCampManager.h"
#include "RYLSkillScriptMgr.h"

#include "BattleGround Client/BattleGroundChangeMoneyDlg.h"

#include "RYLStreetStallDlg.h"
#include "RYLCampShopDlg.h"
#include "RYLVaultPasswordDlg.h"
#include "RYLChattingDlg.h"
#include "RYLNoticeManager.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"

#include "SoundMgrInit.h"
#include "SoundMgr.h"

#include "RYLGameTimeManager.h"

// 메인보드 인식을 위한 WMI API를 쓰기 위한 헤더
#include <Wbemidl.h>
#include <atlbase.h>

#include <Utility/SystemInfo.h>

#include <Utility/Exception.h>
#include "GMMemory.h"

#include "InputDevice.h"

using namespace std;

CClientMain g_ClientMain;

static unsigned long g_dwClientTotalTick;


void	LogDisplay( TCHAR* szBuffer );
void	LogSoundDrv( TCHAR* szBuffer );
void	LogCPU( TCHAR* szBuffer );
void	LogMainBoard( TCHAR* szBuffer );
void	LogMemory( TCHAR* szBuffer );
BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg);


unsigned long __stdcall WindowProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{	
	return g_ClientMain.ClientProc(hWnd, uMsg, wParam, lParam);
}


BOOL WINAPI AntiFunc(HINSTANCE hInstance)
{
//	CExceptionReport::GetInstance().Load();


	MSG msg;
 	msg.wParam = 0;

	LANGID li;	
	li = GetUserDefaultLangID();	

	HWND hLoginWnd = FindWindow(NULL, "Login");

#ifdef NDEBUG
	if (!hLoginWnd && g_ClientMain.m_wAdminMode != ADMIN_L3)
	{
		MessageBox(NULL, "Please execute login.exe", "Program Error", MB_OK);
		return 0;
	}
#endif
	
#ifdef JAPAN_CODE
	g_ClientMain.GetJapanServerInfo() ;
#else
	g_ClientMain.GetServerInfo( hLoginWnd ) ;
#endif

	g_ClientMain.QueryRegScreenValue() ;

	if (!g_ClientMain.InitCreateGameClass())
    {
        g_ClientMain.Destroy();
        return 0;
    }

    if (!g_ClientMain.Init(hInstance, g_ClientMain.m_iScreenWidth, g_ClientMain.m_iScreenHeight ))
	{
		g_ClientMain.Destroy();
		return 0;
	}
	if ( FAILED( g_ClientMain.CreateScene() ) )
	{
		g_ClientMain.Destroy();
		return 0;
	}
/*
	// dump 테스트
	int i = 0;
	int a = 0;
	a = 15;
	int k = 0;
	k = a/i;
*/
	while(1) //stuck in loop when trying to change a character.
	{
		CProfileManager::Increment_Frame_Counter();

		if (PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE))
		{
			if (0 == GetMessage(&msg, NULL, 0, 0 ))
			{
				return (int)msg.wParam;
			}

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
            CSimpleProfile::GetInstance().IncrementFrameCounter();

			PROFILE("Update");
			Sleep(1);
			g_ClientMain.Update();
			g_ClientMain.Render();
		}
	}

	g_ClientMain.Destroy();

//	CExceptionReport::GetInstance().Delete();

	return msg.wParam;
}
/*
int WINAPI ExceptionUserFunc(TCHAR* szBuffer, const int nBufferSize)
{
	CHAR szBuff[ MAX_PATH ] ;
	sprintf( szBuff, "%s\n%s", CRYLStringTable::m_strString[ 2240 ], CRYLStringTable::m_strString[ 2241 ] ) ;

	MessageBox(NULL, szBuff, "Error", MB_OK);

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

	CRYLNetworkData*		pData;
	TCHAR					buff[2048];
	CRYLGameData*			pGame;
	
	int	zone = -1;
	memset( buff, 0, 2048 * sizeof(TCHAR) );
	pData = CRYLNetworkData::Instance();
	pGame =	CRYLGameData::Instance();

	if ( pData )
		zone = pData->m_dwPresentZone;

	// 서버 타입
	char szServerInfo[MAX_PATH] = "";
	if ( CRYLNetworkData::Instance() )
	{
		switch (CRYLNetworkData::Instance()->m_eServerType)
		{
			case GameRYL::SERVER_TEST:			strcpy(szServerInfo, "Test Server Version");			break;
			case GameRYL::SERVER_REGULAR:		strcpy(szServerInfo, "Regular Server Version");			break;
			case GameRYL::SERVER_BATTLE_LOHAN:	strcpy(szServerInfo, "Battle Ground Server Version");	break;
		}
	}

	// OS 정보
	char szOSInfo[MAX_PATH] = "";
	OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (FALSE == GetVersionEx((OSVERSIONINFO *)&osvi))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&osvi);
	}

	switch (osvi.dwMajorVersion)
	{
		case 3:
		{
			if (51 == osvi.dwMinorVersion)
			{
				strcpy(szOSInfo, "Windows NT 3.51");
			}
			else
			{
				strcpy(szOSInfo, "Unknown OS");
			}
			break;
		}

		case 4:
		{
			switch (osvi.dwMinorVersion)
			{
				case 0:
					strcpy(szOSInfo, "Windows 95");
					break;
				case 10:
					strcpy(szOSInfo, "Windows 98");
					break;
				case 90:
					strcpy(szOSInfo, "Windows Me");
					break;
				default:
					strcpy(szOSInfo, "Unknown OS");
					break;
			}
			break;
		}

		case 5:
		{
			switch (osvi.dwMinorVersion)
			{
				case 0:
					strcpy(szOSInfo, "Windows 2000");
					break;
				case 1:
					strcpy(szOSInfo, "Windows XP");
					break;
				case 2:
					strcpy(szOSInfo, "Windows Server 2003 family");
					break;
				default:
					strcpy(szOSInfo, "Unknown OS");
					break;
			}
			break;
		}

		default:
			strcpy(szOSInfo, "Unknown OS");
	}


	szBuffer[0] = '\0';
	if ( pData && pGame )
	{
		sprintf( buff," account \t: [%s]\n id \t: [%s]\n", pData->m_strLoginID, pGame->m_csStatus.m_Info.Name );
		strcat( szBuffer, buff );
		sprintf( buff," zone \t: [%d]\n", pData->m_dwPresentZone );
		strcat( szBuffer, buff );

        if ( pSelfCreature )
		{
			sprintf( buff," pos \t: x = [%f], y= [%f], z = [%f]\n\n", 
				pSelfCreature->GetPosition()->x, pSelfCreature->GetPosition()->y, pSelfCreature->GetPosition()->z );
			strcat( szBuffer, buff );
		}
	}

	// 그래픽 카드 정보
	LogDisplay( szBuffer );

	// 사운드 카드 정보
	LogSoundDrv( szBuffer );

	// CPU 정보
	LogCPU( szBuffer );

	// MainBoard 정보
	if ( osvi.dwMajorVersion == 4 )
	{
		if ( osvi.dwMinorVersion == 90 ) // 윈Me
		{
			LogMainBoard( szBuffer );
			LogMemory( szBuffer );
		}
	}
	else if ( osvi.dwMajorVersion == 5 )
	{
		LogMainBoard( szBuffer );
		LogMemory( szBuffer );
	}
	

	int	res;
	sprintf( buff, " Server \t\t: [%s]\n Client Version \t: [%3.2f]\n Operating System \t: [%s] [%s] Build [%d]\n\n", 
			szServerInfo, g_ClientMain.m_dwClientVersion / 100.0f, szOSInfo, osvi.szCSDVersion, osvi.dwBuildNumber );
	strcat( szBuffer, buff );

	res = strlen( szBuffer );

	return res;
}
*/

LONG __stdcall UserUnhandledExceptionFilterEx(PEXCEPTION_POINTERS pExceptionInfo)   // blackdog hackshield
{
	if(Exception::ExceptionSaveDump)
	{
		// 예외를 호출하기 전에 미니 덤프파일을 생성한다.
		char dumpfile[512];

		if(strlen(Exception::DefaultDumpFileName) == 0)
		{
			SYSTEMTIME	st;
			GetLocalTime(&st);

			sprintf(dumpfile, ".\\Errors\\%04d%02d%02d-%02d%02d%02d.dmp",
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		}
		else 
			strcpy(dumpfile, Exception::DefaultDumpFileName);

		// 이 덤프 생성루틴을 Execption 에서 옵션으로 변경할수 있도록 변경.
		// 미니덤프 생성.
		MiniDump(dumpfile, pExceptionInfo);
	}

#ifdef ON_HACKSHIELD
	if(g_ClientMain.m_pRYLNetworkData)
	{
		g_ClientMain.m_pRYLNetworkData->StopHShield();
		g_ClientMain.m_pRYLNetworkData->UnInstHShield();
	}
#endif

	MessageBox(GetActiveWindow(), "Ryl TidesOfWar Corrupt File Detected.", "Client Error", MB_OK);//was ROW hayzohar

	return EXCEPTION_EXECUTE_HANDLER;
}  // blackdog hackshield

int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
//	InitializeCriticalSection(&g_ExceptionCS);

//	MessageBox(NULL, lpCmdLine, "aa", MB_OK);

	// 임시 
	//::CopyFile(_T(".\\ROWYouxi.mcf"), _T(".\\GameGuard\\Splash.jpg"), FALSE);

	// 자체 함수 Exception 사용
	//...............................//Exception::EnableUnhandledExceptioinFilterEx(UserUnhandledExceptionFilterEx, true); // blackdog hackshield

	// 풀 사이즈 덤프
	Exception::SetDumpType(Exception::NormalDump);
	// Dump 저장
	Exception::EnableSaveDump(true);

	USE_ANTIHACK1();
	USE_ANTIHACK2( AntiFunc );
	USE_ANTIHACK3( hInstance );

//	AntiFunc(hInstance);

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientMain::CClientMain()
{
#ifdef JAPAN_CODE
	m_ReqGameID = NULL;
#endif
	m_hnProtectLib = NULL;
	m_wClientLoginFlag = 0;

#ifdef NDEBUG
	m_wAdminMode = ADMIN_NONE;					// 일반인
	m_bUDPInfo = false;
#else
	m_wAdminMode = ADMIN_L3;					// 개발자
	m_bUDPInfo = true;
#endif

	m_dwServerTotal=0;//hz speed hack test check start
	m_dwServerLast=0;
	m_dwClientTotal=0;
	m_dwClinetLast=0;
	m_dwHzSpeedHack=0;//hz speed hack test check end
	m_lpD3DDevice = NULL ;
	
	// 조이스틱 사용 관련 초기화 (2006-05-15 by hackermz)
	m_dwUseJoystick = 0;
	loop=0;//hz name check hack
}

CClientMain::~CClientMain()
{

}

bool CClientMain::InitCreateGameClass()
{
    TCHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH - 1);
    szFileName[MAX_PATH - 1] = 0;

    TCHAR szDrive[MAX_PATH];
    TCHAR szDir[MAX_PATH];
    TCHAR szName[MAX_PATH];
    TCHAR szExt[MAX_PATH];

    _tsplitpath(szFileName, szDrive, szDir, szName, szExt);

//#ifdef _DEBUG
//    const TCHAR* szScriptFileName = _T("Script.dat");
//#else
    const TCHAR* szScriptFileName = _T("Script.mcf");
//#endif

	TCHAR szScriptPath[MAX_PATH];
    TCHAR szMacroPath[MAX_PATH];

#ifdef _RYL_LANGUAGE
	int iMaxLanguage = 1;
	TCHAR szLanguage[1][32] = { _T("ENG")};
		//, _T("CHS"), _T("JPN"), _T("KOR"), _T("PRT"), _T("SPN"), _T("POL") };

	if(m_iLanguage >= iMaxLanguage)
		m_iLanguage = 0;

	_sntprintf(szScriptPath, MAX_PATH, 
        "%s%sLanguage\\%s\\%s", szDrive, szDir, szLanguage[m_iLanguage], szScriptFileName);

	_sntprintf(szMacroPath, MAX_PATH, 
		"%s%s%s", szDrive, szDir, _T("KEYMSG.txt"));
#else
	_sntprintf(szScriptPath, MAX_PATH, 
        "%s%s%s", szDrive, szDir, szScriptFileName);

	_sntprintf(szMacroPath, MAX_PATH, 
		"%s%s%s", szDrive, szDir, _T("KEYMSG.txt"));
#endif

    szScriptPath[MAX_PATH - 1] = 0;

	if (!CRYLStringTable::Load(szScriptPath, szMacroPath))
	{
		MessageBox(NULL, "Can't load Language Script.", "Program Error", MB_OK);
	}
    else
    {
	    //========================================================================
	    // GameData와 NetWork Data 생성
	    m_pRYLGameData = new CRYLGameData ;

        m_pRYLNetworkData = new CRYLNetworkData(
            CRYLStringTable::m_eInternationalCode, 
            CRYLStringTable::m_eServerType, 
            CRYLStringTable::m_szGameGuardCode);

	    m_pRYLCommunityData		= new CRYLCommunityData ;
	    m_pCastleManager		= new CRYLCastleManager ; 
	    m_pCampManager			= new CRYLCampManager ;
		m_pSkillScriptManager	= new CRYLSkillScriptMgr ;

	    if (0 != m_pRYLGameData &&
            0 != m_pRYLNetworkData &&
            0 != m_pRYLCommunityData &&
	        0 != m_pCastleManager &&
            0 != m_pCampManager &&
			0 != m_pSkillScriptManager)
        {
            m_pRYLNetworkData->m_dwChannelID = 0;
            return true;
        }
    }

    return false;
}

#ifdef JAPAN_CODE
VOID	CClientMain::GetJapanServerInfo() 
{
	CoInitialize( NULL ) ;
	HRESULT hr = m_pRYLNetworkData->m_ReqGameID.CreateInstance( "IDAuth.ReqGameID" ) ;
	
	if ( FAILED( hr ) )
	{
		char strError[ MAX_PATH ] ;
		sprintf( strError, "0x%08x", hr ) ;
		MessageBox( NULL, "Failed Create [IDAuth]", strError, MB_ICONERROR ) ;
		return 0 ;
	}

	char strGameCode[ MAX_PATH ]; 
	char strArgText[ MAX_PATH ], strAccountArg[ MAX_PATH ] ;
	strcpy( strGameCode, "209551D1-DE8A-40CF-BF54-152D7C17A62F" ) ;//게임 코드

	char *pLast = strCommandLine ;
	sscanf( pLast, "%s %d %s", CRYLNetworkData::m_strIP, &CRYLNetworkData::m_dwServerID, &strArgText ) ;
	sprintf( strAccountArg, "%s%c%s", strArgText, 2, strGameCode ) ;

USES_CONVERSION;
	BSTR bstrInput	= T2OLE( strAccountArg ) ;
	BSTR bstrGameID = NULL ;  
	hr = m_pRYLNetworkData->m_ReqGameID->get_ReqGameID( bstrInput, &bstrGameID ) ;
	if ( hr != S_OK )
	{
		char strError[ MAX_PATH ] ;
		sprintf( strError, "0x%08x", hr ) ;
		MessageBox( NULL, "Failed receive GameID", strError, MB_ICONERROR ) ;
		return FALSE ;
	}

	strcpy( m_pRYLNetworkData->m_strLoginID, OLE2T( bstrGameID ) ) ;//결과 -- Game에 사용할 ID	
	m_pRYLNetworkData->m_dwUserID = atoi( m_pRYLNetworkData->m_strLoginID ) ;

	SysAllocString( bstrGameID ) ;
	SysAllocString( bstrInput ) ;

	SendMessage( hLoginWnd, WM_USER + 0x7777, NULL, NULL ) ;

	if ( m_wAdminMode == ADMIN_L3 )
	{
		MessageBox( NULL, strCommandLine, "GP", MB_OK ) ;//was "ROW" hayzohar
	}
}
#endif // GetJapanServerInfo() 

VOID	CClientMain::GetServerInfo( HWND hWnd ) 
{
	char *strCommandLine = GetCommandLine() ;	

	if ( hWnd )
	{
		char *pLast = strCommandLine ;
		sscanf( pLast, "%s %d", CRYLNetworkData::m_strIP, &CRYLNetworkData::m_dwServerID ) ;
		SendMessage( hWnd, WM_USER + 0x7777, NULL, NULL ) ;
	} 
	else	
	{
		char *pLast = strrchr( strCommandLine, '"' ) ;
		pLast += 2 ;
		sscanf( pLast,"%s %d", CRYLNetworkData::m_strIP, &CRYLNetworkData::m_dwServerID ) ;
	}

#ifdef DEBUG
	strcpy(CRYLNetworkData::m_strIP, "127.0.0.1");
#endif
/*
	if ( m_wAdminMode == ADMIN_L3 )
	{
		MessageBox( NULL, strCommandLine, "ROW", MB_OK ) ;
	}
*/
}

unsigned long CClientMain::ClientProc(HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( CGMUIScreen::IsInited() )
	{
		GUITooltipMgr.m_ttRenderTarget    = NULL ;
		if ( CGMUIScreen::GetInstance()->WinProc( hWnd, uMsg, wParam, lParam ) )
			return 0 ;
	}

	switch (uMsg)
	{
		case WM_CLOSE:
		{
			// 강제종료
			unsigned long dwCounter = 10000;
			if ( m_wAdminMode > SUPPORTER)
			{
				// 운영자는 빠르게 종료 가능
				dwCounter = 3000;
			}

			if ( CRYLSceneManager::Instance()->GetSceneID() == GAME_SCENE )
			{
				CRYLGameData::SetClientExitCounter( dwCounter ) ;
				return 0;
			}
			break;
		}

		case WM_SETCURSOR:
		{
			if (LOWORD(lParam) == HTCLIENT) // hit-test code
			{
				if ( m_pRYLGameData->m_bHardwareCursor)
				{
					if (m_pRYLGameData->m_dwKindCursor == CURSOR_NORMAL)
					{
						if (g_TooltipManager.m_ttRenderTarget)
							::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_TOOLTIP)));
						else if (g_DeviceInput.GetIsLeftMousePress())
							::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_CLICKCURSOR)));
						else
							::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_NORMALCURSOR)));

						if (m_pRYLGameData->m_lpInterface)
						{
							if (m_pRYLGameData->m_lpPickItem && g_MessageBoxManager.m_lstMessageBox.empty())
							{
								::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_PICKUP)));
							} 
							else if (m_pRYLGameData->m_lpInterface->m_bPopupShow)
							{
								::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_POPUP)));
							} 
							else if (m_pRYLGameData->m_lpInterface->m_dwNormalTargetChrID != 0xFFFFFFFF)
							{
								if (CRYLGameData::GetClientEnemyType(m_pRYLGameData->m_lpInterface->m_dwNormalTargetChrID) == EnemyCheck::EC_ENEMY)
									::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_ATTACK)));
								else
									::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_DIALOG)));
							}
						}
					} 
					else if (m_pRYLGameData->m_dwKindCursor == CURSOR_REPAIR)
					{
						::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_REPAIR)));
					}
				}

				return 0;
			}

			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_RETURN:
				{
                    CRYLGameScene*		            pScene          = ( CRYLGameScene* )( CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) );
                    CRYLStreetStallDlg*             pStreetStallDlg = ( CRYLStreetStallDlg* )( pScene->GetStreetStallDlg() );
					CRYLCampShopDlg*				pCampShopDlg	= ( CRYLCampShopDlg* )( pScene->GetCampShopDlg() );
                    CRYLVaultPasswordDlg*           pVaultPassDlg   = ( CRYLVaultPasswordDlg* )( pScene->GetVaultPasswordDlg() );
                    CRYLVaultCertificationdDlg*     pVaultCertiDlg  = ( CRYLVaultCertificationdDlg* )( pScene->GetVaultCertificationdDlg() );
                    CRYLBattleGroundChangeMoneyDlg* pChangeMoneyDlg = ( CRYLBattleGroundChangeMoneyDlg*)( pScene->GetBattleGroundChangeMoney() );

                    CRYLEdit*   pTempEdit = NULL;                        

                    if ( pStreetStallDlg && pStreetStallDlg->GetVisible() )
                    {
                        pTempEdit = pStreetStallDlg->GetChattingEdit();
                    }
					else if ( pCampShopDlg && pCampShopDlg->GetVisible() )
					{
						pTempEdit = pCampShopDlg->GetChattingEdit();
					}
                    else if ( pVaultPassDlg && pVaultPassDlg->GetVisible() )
                    {
                        pTempEdit = pVaultPassDlg->m_pCurrentPasswordEdit;
                    }
                    else if ( pVaultCertiDlg && pVaultCertiDlg->GetVisible() )
                    {
                        pTempEdit = pVaultCertiDlg->m_pPasswordEdit;
                    }
                    else if ( pChangeMoneyDlg && pChangeMoneyDlg->GetVisible() )
                    {
                        pTempEdit = pChangeMoneyDlg->m_pInputMedalValueEdit;
                    }

                    if ( pTempEdit )
                    {
						pTempEdit->SetFocus();
                        pTempEdit->SetFocusState( TRUE );

                        return 0;
                    }

					if ( CRYLSceneManager::GetCurrentScene() == pScene )
					{
						if ( CRYLChattingDlg::Instance() )
						{
							CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() ) ;
							if ( FALSE == CRYLChattingDlg::Instance()->GetHideEnable() ) 
							{
								CRYLChattingDlg::Instance()->ShowButton() ;
								CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( TRUE ) ;
							}
						}
					}
				}

				return 0;
			}
			break;
		}

		case WM_CHAR:
		{
			switch (wParam)
			{
				case VK_RETURN:
					return 0;
			}
			break;
		}

		case WM_MOUSEWHEEL:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		{
			g_DeviceInput.GetMouseState(uMsg, wParam, lParam);
			break;
		}

		case WM_TIMER:
		{
			if (wParam == 255)
			{	
				// nProtect 검사
				m_pRYLNetworkData->ChecknProtect( m_hWnd ) ;
			}
			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			Destroy();
			break;
		}
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

VOID	CClientMain::QueryRegScreenValue() 
{
	HKEY hMPClientReg;
	unsigned long dwDisp = 0;

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\ROW\\MP-Client", 0, NULL, //was Software\\ROW\\MP-Client hayzohar
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hMPClientReg, &dwDisp);

	unsigned long dwSize = sizeof(unsigned long);
	if (ERROR_SUCCESS != RegQueryValueEx(hMPClientReg, "Width", 0, NULL, (LPBYTE)&m_iScreenWidth, &dwSize))
	{
		m_iScreenWidth = 800;
	}
	if (ERROR_SUCCESS != RegQueryValueEx(hMPClientReg, "Height", 0, NULL, (LPBYTE)&m_iScreenHeight, &dwSize))
	{
		m_iScreenHeight = 600;
	}
	if (ERROR_SUCCESS != RegQueryValueEx(hMPClientReg, "Depth", 0, NULL, (LPBYTE)&m_iBpp, &dwSize))
	{
		m_iBpp = 16;
	}
	if (ERROR_SUCCESS != RegQueryValueEx(hMPClientReg, "Refresh", 0, NULL, (LPBYTE)&m_iRefresh, &dwSize)) // by Belldandy
	{
		m_iRefresh = 60;	// by Belldandy
	}

	if (ERROR_SUCCESS != RegQueryValueEx(hMPClientReg, "WindowMode", 0, NULL, (LPBYTE)&m_iWindowMode, &dwSize)) // by Belldandy
	{
		m_iWindowMode = 0 ;
	}

	if (ERROR_SUCCESS != RegQueryValueEx(hMPClientReg, "Language", 0, NULL, (LPBYTE)&m_iLanguage, &dwSize)) // by edith
	{
		m_iLanguage = 0 ;
	}

	if (ERROR_SUCCESS != RegQueryValueEx(hMPClientReg, "MLSupport", 0, NULL, (LPBYTE)&m_iMultiSupport, &dwSize)) // by edith
	{
		m_iMultiSupport = 1 ;
	}

	RegCloseKey(hMPClientReg);
}

unsigned long  CClientMain::CheckOCTREE()
{
	unsigned long dwCrc = 0, dwTemp = 0;
    HANDLE              hSrch;
    WIN32_FIND_DATA     wfd;
    BOOL                bResult = TRUE;

	char strScriptPath[MAX_PATH];
	sprintf(strScriptPath, "%s\\OCTREE\\*.OCT", m_strClientPath);

	hSrch = FindFirstFile(strScriptPath, &wfd);

	int iCount = 0;

    while (bResult)
    {
        if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
			;
        }
        else
        {
			sprintf(strScriptPath, "%s\\OCTREE\\%s", m_strClientPath, wfd.cFileName);

			dwTemp = 0;
			if(CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
			{
				;
			}
			dwCrc = dwCrc ^ dwTemp;
			iCount++;
        }

        bResult = FindNextFile(hSrch, &wfd);
    }
    FindClose(hSrch);

	return dwCrc;
}

BOOL CClientMain::Init(HINSTANCE hInstance, INT iWidth, INT iHeight )
{
	// edith 2009.12.26 핵쉴드 업데이트 기능 사용.
	if(!CRYLNetworkData::UpdateHShield())
	{
		MessageBox(NULL, "핵쉴드 업데이트에 실패하였습니다.", "ERROR", MB_OK);
		return FALSE;
	}


#ifdef NDEBUG
	if ( !m_iWindowMode )
		SetDisplay();
#endif

	m_hInstance		= hInstance ;
	m_iScreenWidth	= iWidth ;
	m_iScreenHeight = iHeight ;
	////hz added for unwindow mode
	//unsigned long horizontal = 0;
 //   unsigned long vertical = 0;
	//RECT desktop;
 //  const HWND hDesktop = GetDesktopWindow();
 //  GetWindowRect(hDesktop, &desktop);
 //  horizontal = desktop.right;
 //  vertical = desktop.bottom;
 //   
	//if((horizontal>m_iScreenWidth)||(vertical>m_iScreenHeight))
	//{
	//	MessageBox(GetActiveWindow(), "Client can only run in full screen", "Ring Of War Error", MB_OK);
	//	this->Destroy();
	//}
	//////hz end

	WNDCLASS wc;
	// Set up and register window class
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC) WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(DWORD);
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_NORMALCURSOR));
	wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Tides of War";//was Return Of Warrior hayzohar

	if (!RegisterClass(&wc))
	{
		return FALSE;
	}

/*
	WS_VISIBLE | 
	WS_OVERLAPPED     |
    WS_CAPTION        | 
    WS_SYSMENU        | 
    WS_MINIMIZEBOX,
**/
	unsigned long dwWinStyle = 0, dwTmpWidth = 0, dwTmpHeight = 0;
	/*if ( m_iWindowMode )//wasnt commented out hz windowmode.
	{
		dwWinStyle = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX ;
		dwTmpWidth = m_iScreenWidth + 6 ;
		dwTmpHeight = m_iScreenHeight + 32 ;
	}
	else
	{*/
		dwWinStyle = WS_VISIBLE | WS_POPUP ;
		dwTmpWidth = m_iScreenWidth ;
		dwTmpHeight = m_iScreenHeight ;
	//}

	m_hWnd = CreateWindow(	"Tides of War",					// class was Return of Warrior hayzohar
							"Tides of War",					// caption
							dwWinStyle,							// style 
							0, 0, 
							dwTmpWidth,
							dwTmpHeight,						// window size
							NULL,								// parent window
							NULL,								// menu 
							hInstance,							// instance
							NULL);								// parms

	g_DeviceInput.SetHwnd(m_hWnd);
	
	HKEY hMPClientReg;
	unsigned long dwReadType = 0, dwReadLens = 256;
	char strScriptPath[MAX_PATH];

	LONG lResult = 0;
	char strReg[MAX_PATH];

	sprintf(strReg, "Software\\ROW\\MP-Client");//was ROW hayzohar

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg, 0, KEY_READ, &hMPClientReg))
	{
		switch (m_pRYLNetworkData->m_eServerType)
		{
			case GameRYL::SERVER_TEST:
				lResult = RegQueryValueEx(hMPClientReg, "TestFolder", 0, &dwReadType, (LPBYTE)m_strClientPath, &dwReadLens);
				break;

			case GameRYL::SERVER_REGULAR:
				lResult = RegQueryValueEx(hMPClientReg, "Folder", 0, &dwReadType, (LPBYTE)m_strClientPath, &dwReadLens);
				break;

			case GameRYL::SERVER_BATTLE_LOHAN:
				lResult = RegQueryValueEx(hMPClientReg, "BGFolder", 0, &dwReadType, (LPBYTE)m_strClientPath, &dwReadLens);
				break;
		}

		if (ERROR_SUCCESS == lResult)
		{
			if (m_InitValue.Load(hMPClientReg) == false)
			{
				MessageBox(NULL, CRYLStringTable::m_strString[ 904 ], "Program Error", MB_OK);
				RegCloseKey(hMPClientReg);
				return FALSE;
			}

			if (CRenderOption::Load(hMPClientReg) == false)
			{
				MessageBox(NULL, CRYLStringTable::m_strString[ 904 ], "Program Error", MB_OK);
				RegCloseKey(hMPClientReg);
				return FALSE;
			}			

//			RegCloseKey(hMPClientReg);
		} 
		else
		{
			MessageBox(NULL, "Please re-install the game.", "Program Error", MB_OK);
			RegCloseKey(hMPClientReg);
			return FALSE;
		}

		// 조이스틱 관련
		if(ERROR_SUCCESS != RegQueryValueEx(hMPClientReg, "UseJoyStick", 0, NULL, (LPBYTE)&m_dwUseJoystick, &dwReadLens))
		{
			m_dwUseJoystick = 0;
		}

		RegCloseKey(hMPClientReg);
	}
	else
	{
		MessageBox(NULL, "Please re-install the game.", "Program Error", MB_OK);
		return FALSE;
	}

	if (m_pRYLNetworkData->m_eInternationalCode == GameRYL::KOREA)
	{	
		// myFirewall 초기화
		myfirewall::On();
	}



	// 시스템 정보 얻어오기	
	using namespace SystemInfo;

	CSystemInfo* pSystemInfo;

	pSystemInfo = new CSystemInfo;

	pSystemInfo->GetMemoryInfo();
	pSystemInfo->GetCPUInfo();		
	pSystemInfo->GetVideoCardInfo();		

	if(!pSystemInfo->IsMinimum())
	{
		CRYLGameData* pGame = CRYLGameData::Instance() ;

		bool bSave =  false;

		// 최소 사양으로 돌린다. //				
		if(CRenderOption::m_CharacterProjectShadowTerrain!=0)	bSave = true;
		else if(CRenderOption::m_BuildingLightmap!=0)			bSave = true;
		else if(CRenderOption::m_ObjectLOD!=1)					bSave = true;
		else if(CRenderOption::m_TextureMethod!=0)				bSave = true;
		else if(CRenderOption::m_GrassRendering!=0)				bSave = true;
		else if(CRenderOption::m_ScreenFilter!=0)				bSave = true;
		else if(CRenderOption::m_WaterBumpEnvRendering!=0)		bSave = true;
		else if(CRenderOption::m_bFullSceneEffect!=0)			bSave = true;
		else if(CRenderOption::m_bWeather!=0)					bSave = true;
				
		CRenderOption::m_CharacterProjectShadowTerrain	= 0;
		CRenderOption::m_BuildingLightmap				= 0;
		CRenderOption::m_ObjectLOD						= 1;
		CRenderOption::m_TextureMethod					= 0;
		CRenderOption::m_GrassRendering					= 0;
		CRenderOption::m_ScreenFilter					= 0;
		CRenderOption::m_WaterBumpEnvRendering			= 0;
		CRenderOption::m_bFullSceneEffect				= 0;
		CRenderOption::m_bWeather						= 0;

		if(bSave)
		{
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg, 0, KEY_ALL_ACCESS, &hMPClientReg)==ERROR_SUCCESS)
			{
				if(!CRenderOption::Save(hMPClientReg))
				{
					MessageBox(NULL, "Not Render Option Save", "Tides of War", MB_OK);//was Return of Warrior hayzohar
				}
				
				RegCloseKey(hMPClientReg);
			}

			MessageBox(NULL, CRYLStringTable::m_strString[3801], "Tides of War", MB_OK);
		}				
	}	

	delete pSystemInfo;
	pSystemInfo = NULL;

	// edith 2009.02.14 렌더옵션의 웨더 이팩트를 스킬웨더이팩트에 연결.
	// 추후 스킬이팩트만 따로빠질수도 있어서 새로 변수를 작성했음.
	CSceneManager::m_SkillWeatherEffect = static_cast<bool>(CRenderOption::m_bWeather);

	// ---------------------------------------------------------------------------
	if ( !m_pRYLNetworkData->InitNPGameLib( m_hWnd ) )
		return FALSE ;


	// ---------------------------------------------------------------------------
	if ( !m_pRYLNetworkData->InitHShield( m_hWnd ) )
	{
		m_pRYLNetworkData->UnInstHShield();
		return FALSE ;
	}

	if ( !m_pRYLNetworkData->StartHShield( m_hWnd ) )
	{
		m_pRYLNetworkData->StopHShield();
		m_pRYLNetworkData->UnInstHShield();
		return FALSE ;
	} // blackdog hackshield off

	// ---------------------------------------------------------------------------

	g_dwClientTotalTick = timeGetTime();

	FILE *fp;
	int ver;

	sprintf(strScriptPath, "%s\\VersionInfo.dat", m_strClientPath);
	fp = fopen(strScriptPath, "r");
	fscanf(fp, "%d", &ver);
	fclose(fp);
	m_dwClientVersion = ver;

	if (!m_pRYLGameData->m_bHardwareCursor)
	{
		g_DeviceInput.ShowCursor(FALSE);
	}

	unsigned long dwCrc[20], dwTemp = 0;
	ZeroMemory(dwCrc, sizeof(unsigned long)*20);

	sprintf(strScriptPath, "%s\\Client.exe", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[0]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}

	// 옥트리 체크섬 검사
	dwTemp = CheckOCTREE();
	dwCrc[0] = dwCrc[0] ^ dwTemp;

#ifdef _RYL_LANGUAGE
	int iMaxLanguage = 1;
	TCHAR szLanguage[1][32] = { _T("ENG")};
		//, _T("CHS"), _T("JPN"), _T("KOR"), _T("PRT"), _T("SPN"), _T("POL") };

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// MUI
	for(int i = 0; i < iMaxLanguage; ++i)
	{
		sprintf(strScriptPath, "%s\\Language\\%s\\NPCScript.mcf", m_strClientPath, szLanguage[i]);
		if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
		{
			MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
			return FALSE;
		}
		dwCrc[1] = dwCrc[1]^dwTemp;

		// 배틀 로한은 별개의 아이템 스크립트를 사용합니다.
		if (GameRYL::SERVER_BATTLE_LOHAN == m_pRYLNetworkData->m_eServerType)
		{
			sprintf(strScriptPath, "%s\\Language\\%s\\BGItemScript.gsf", m_strClientPath, szLanguage[i]);
		}
		else
		{
			sprintf(strScriptPath, "%s\\Language\\%s\\ItemScript.gsf", m_strClientPath, szLanguage[i]);
		}
		if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
		{
			MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
			return FALSE;
		}
		dwCrc[2] = dwCrc[2]^dwTemp;

		sprintf(strScriptPath, "%s\\Language\\%s\\SkillScript.gsf", m_strClientPath, szLanguage[i]);
		if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
		{
			MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
			return FALSE;
		}
		dwCrc[3] = dwCrc[3]^dwTemp;

		sprintf(strScriptPath, "%s\\Language\\%s\\MonsterProtoType.gsf", m_strClientPath, szLanguage[i]);
		if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
		{
			MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
			return FALSE;
		}
		dwCrc[4] = dwCrc[4]^dwTemp;

		sprintf(strScriptPath, "%s\\Language\\%s\\Quest.mcf", m_strClientPath, szLanguage[i]);
		if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
		{
			MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
			return FALSE;
		}
		dwCrc[5] = dwCrc[5]^dwTemp;

		////////////////////////////////////////////////////////////////////////////////////////////////////
	}
#else
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// MUI
	sprintf(strScriptPath, "%s\\NPCScript.mcf", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[1]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	// 배틀 로한은 별개의 아이템 스크립트를 사용합니다.
	if (GameRYL::SERVER_BATTLE_LOHAN == m_pRYLNetworkData->m_eServerType)
	{
		sprintf(strScriptPath, "%s\\BGItemScript.gsf", m_strClientPath);
	}
	else
	{
		sprintf(strScriptPath, "%s\\ItemScript.gsf", m_strClientPath);
	}
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[2]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\SkillScript.gsf", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[3]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\MonsterProtoType.gsf", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[4]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\Quest.mcf", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[5]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

	sprintf(strScriptPath, "%s\\Script1.gsf", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[6]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\Character\\Data\\ChrEvent.eds", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[7]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\Chemical.gsf", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[8]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\SpeacialCompensation.gsf", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[9]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}

	// 존체크
	sprintf(strScriptPath, "%s\\zone1.z3s", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10]^dwTemp;

	sprintf(strScriptPath, "%s\\zone2.z3s", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10]^dwTemp;

	sprintf(strScriptPath, "%s\\zone4.z3s", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10]^dwTemp;

	sprintf(strScriptPath, "%s\\zone5.z3s", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10]^dwTemp;

	sprintf(strScriptPath, "%s\\zone8.z3s", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10]^dwTemp;

	sprintf(strScriptPath, "%s\\zone12.z3s", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10]^dwTemp;

	sprintf(strScriptPath, "%s\\zone16.z3s", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10]^dwTemp;
/*
	sprintf(strScriptPath, "%s\\GameGuard.des", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[11]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
*/
	sprintf(strScriptPath, "%s\\MineralVein8.gsf", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[11]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
/*
	sprintf(strScriptPath, "%s\\MineralVein16.gsf", m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[12]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
*/
	m_dwVersion = dwCrc[0];
	m_dwVersion = m_dwVersion ^ dwCrc[1];
	m_dwVersion = m_dwVersion ^ dwCrc[2];
	m_dwVersion = m_dwVersion ^ dwCrc[3];
	m_dwVersion = m_dwVersion ^ dwCrc[4];
	m_dwVersion = m_dwVersion ^ dwCrc[5];
	m_dwVersion = m_dwVersion ^ dwCrc[6];
	m_dwVersion = m_dwVersion ^ dwCrc[7];
	m_dwVersion = m_dwVersion ^ dwCrc[8];
	m_dwVersion = m_dwVersion ^ dwCrc[9];
	m_dwVersion = m_dwVersion ^ dwCrc[10];
	m_dwVersion = m_dwVersion ^ dwCrc[11];
//	m_dwVersion = m_dwVersion ^ dwCrc[12];

#ifdef _RYL_TEST
	// 디버그용 정섭 체크섬.
	m_dwVersion = 0xfeef66b3;
#endif

//#ifndef NDEBUG
//	switch (m_pRYLNetworkData->m_eServerType)
//	{
//		case GameRYL::SERVER_TEST:			m_dwVersion = 0x2e07d9fa;	break;	// 테섭
//		case GameRYL::SERVER_REGULAR:		m_dwVersion = 0xb332c5fb;	break;	// 정섭
//		case GameRYL::SERVER_BATTLE_LOHAN:	m_dwVersion = 0x00000001;	break;	// 배섭 
//	}
//#endif	

	m_pRYLGameData->InitTipBroad( m_strClientPath ) ;
	
	char strFilePath[MAX_PATH] ;
	sprintf(strFilePath, "%s\\ClientOption.dat", m_strClientPath);
	m_pRYLGameData->LoadClientOption(strFilePath);
    
	g_pClientSocket = ClientSocket::CreateInstance();

/*
	// 리밸런싱 작업 중 테스트용으로 사용되던 부분
    char strLogFilePath[MAX_PATH] ;
    sprintf(strLogFilePath, "%s\\NetworkLog.txt", m_strClientPath);
    g_pSessionMgr->NetworkLog(strLogFilePath);
*/

	// 조이스틱 초기화 //
	// 한국에서는 일단 사용하지 않기 때문에 막아둡니다. (2006-05-02 by hackermz)
	// 테스트 목적으로 레지스트리의 UseJoystick 항목을 참고합니다. (2006-05-15 by hackermz)
//	if (GameRYL::KOREA != m_pRYLNetworkData->m_eInternationalCode)

	// edith 조이스틱 수정.
	CInputDevice::GetInstance()->Initialize(m_hWnd, false, false, true);
/*
	if (1 == GetUseJoystick())
	{
		CInputDevice::GetInstance()->Initialize(m_hWnd, false, false, true);
	}
	else
	{
		CInputDevice::GetInstance()->Initialize(m_hWnd, false, false, false);
	}
*/
	CEnumD3D::Enum();
	CEnumD3D::m_nAdapter = m_InitValue.m_nAdapter;
	CEnumD3D::m_nDevice = m_InitValue.m_nDevice;
	CEnumD3D::m_nMode = m_InitValue.m_nMode;

	// edith 2008.01.18 ShowState를 위해 false->true로 변경해줘야함.
	// showState가 true 이면 스탠실 버퍼를 생성하기 때문에 그래픽카드의 특성을 사서 초기화에
	// 실패할 수가 있다.
	m_BaseGraphicLayer.Create(m_hWnd, true, true, 0, 0, m_iScreenWidth, m_iScreenHeight );

	m_pRYLGameData->m_lpSceneManager = new CSceneManager();
	m_pRYLGameData->m_lpSceneManager->SetDevice(m_BaseGraphicLayer.GetDevice());
	m_pRYLGameData->m_lpSceneManager->SetCamera(&m_BaseGraphicLayer.m_ViewCamera);
	m_pRYLGameData->m_lpSceneManager->m_WeatherManager.SetClearColor(&m_BaseGraphicLayer.m_ClearColor);
	m_pRYLGameData->m_lpSceneManager->Create(m_InitValue, (char *)m_strClientPath);

	// scene manager에 화면해상도 설정
	m_pRYLGameData->m_lpSceneManager->m_ScreenWidth = m_iScreenWidth;
	m_pRYLGameData->m_lpSceneManager->m_ScreenHeight = m_iScreenHeight;

	g_TextEdit.Create(m_hWnd, m_BaseGraphicLayer.GetDevice());
	g_DeviceInput.SetEditHwnd(g_TextEdit.GetHwnd());
	g_TextEdit.InitMessage();
	g_MessageBoxManager.Create();

	m_lpD3DDevice = m_BaseGraphicLayer.GetDevice() ;

	sprintf(strReg, "Software\\ROW\\MP-Client");//was ROW hayzohar

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg, 0, KEY_READ, &hMPClientReg) == ERROR_SUCCESS)
	{
		char strUserID[24];
		dwReadLens = 23;
		if (RegQueryValueEx(hMPClientReg, "UserID", 0, &dwReadType, (LPBYTE)strUserID, &dwReadLens) == ERROR_SUCCESS)
		{
			m_pRYLGameData->m_bSaveID = TRUE;

			strcpy( m_pRYLNetworkData->m_strLoginID, strUserID);
			ZeroMemory(m_pRYLNetworkData->m_strLoginPass, sizeof(m_pRYLNetworkData->m_strLoginPass));
		} 
		else
		{
			m_pRYLGameData->m_bSaveID = FALSE;

			ZeroMemory(m_pRYLNetworkData->m_strLoginID, sizeof(m_pRYLNetworkData->m_strLoginID));
			ZeroMemory(m_pRYLNetworkData->m_strLoginPass, sizeof(m_pRYLNetworkData->m_strLoginPass));
		}

		RegCloseKey(hMPClientReg);
	}

	// ---------------------------------------------------------------------------
	// 사운드 처리

//	try
//	{
		SoundMgrInit sndInit; 
		sndInit.m_hWnd = m_hWnd; 
		CSoundMgr::_GetInstance()->Init( sndInit ); 
//	}
//	catch(exception & e)
//	{
//		MessageBox(NULL, e.what(), " ", MB_OK);
//	}

	// ---------------------------------------------------------------------------
	// CharacterData의 함수포인터 Set

	m_pRYLNetworkData->LinkCharacterData() ;
	m_pRYLGameData->LinkCharacterData() ;

	g_CharacterData.m_Camera.m_nCameraMode = 0;

	for(int i = 0; i < 3; i++)
	{
		strcpy(CRYLCommunityData::Instance()->m_strChatMessage[i], "");
	}

	m_pRYLGameData->m_bShowInfo = FALSE;

	// ---------------------------------------------------------------------------
	// 타이머 초기화

	GRYLTimer.InitTimer() ;

	g_CharacterData.Create();
	EnemyCheck::CCheckTable::GetInstance().Initialize();

//	try
	{
#ifdef _RYL_LANGUAGE
		int iMaxLanguage = 1;
		TCHAR szLanguage[1][32] = { _T("ENG")};
			//, _T("CHS"), _T("JPN"), _T("KOR"), _T("PRT"), _T("SPN"), _T("POL") };

		int iLanguage = g_ClientMain.m_iLanguage;
		if(iLanguage >= iMaxLanguage)
			iLanguage = 0;

		// 배틀 로한은 별개의 아이템 스크립트를 사용합니다.
		if (GameRYL::SERVER_BATTLE_LOHAN == m_pRYLNetworkData->m_eServerType)
		{
			sprintf(strScriptPath, "%s\\Language\\%s\\BGItemScript.gsf", m_strClientPath, szLanguage[iLanguage]);
		}
		else
		{
			sprintf(strScriptPath, "%s\\Language\\%s\\ItemScript.gsf", m_strClientPath, szLanguage[iLanguage]);
		}
		g_ItemMgr.LoadItemProtoTypeBinary(strScriptPath);

		sprintf(strScriptPath, "%s\\Language\\%s\\SkillScript.gsf", m_strClientPath, szLanguage[iLanguage]);
		g_SkillMgr.LoadSkillsFromBinary(strScriptPath);

		sprintf(strScriptPath, "%s\\Language\\%s\\MonsterProtoType.gsf", m_strClientPath, szLanguage[iLanguage]);
		g_MonsterMgr.LoadMonstersFromBinary(strScriptPath);

		char strQuestPath[MAX_PATH];
		sprintf(strQuestPath, "%s\\Language\\%s\\Quest.mcf", m_strClientPath, szLanguage[iLanguage]);
		g_QuestList.Load(strQuestPath);

#else
		// 배틀 로한은 별개의 아이템 스크립트를 사용합니다.
		if (GameRYL::SERVER_BATTLE_LOHAN == m_pRYLNetworkData->m_eServerType)
		{
			sprintf(strScriptPath, "%s\\BGItemScript.gsf", m_strClientPath);
		}
		else
		{
			sprintf(strScriptPath, "%s\\ItemScript.gsf", m_strClientPath);
		}

		g_ItemMgr.LoadItemProtoTypeBinary(strScriptPath);

		sprintf(strScriptPath, "%s\\SkillScript.gsf", m_strClientPath);
		g_SkillMgr.LoadSkillsFromBinary(strScriptPath);

		sprintf(strScriptPath, "%s\\MonsterProtoType.gsf", m_strClientPath);
		g_MonsterMgr.LoadMonstersFromBinary(strScriptPath);

		char strQuestPath[MAX_PATH];
		sprintf(strQuestPath, "%s\\Quest.mcf", m_strClientPath);
		g_QuestList.Load(strQuestPath);
#endif
		sprintf(strScriptPath, "%s\\SpeacialCompensation.gsf", m_strClientPath);
		g_ItemMgr.LoadItemSpeacialCompensationBinary( strScriptPath );

		sprintf(strScriptPath, "%s\\Chemical.gsf", m_strClientPath);
		g_ItemMgr.LoadItemChemicalBinary( strScriptPath );

		sprintf(strScriptPath, "%s\\Script1.gsf", m_strClientPath);
		g_StringFilter.Load(strScriptPath);
	}
/*	catch( ... )
	{
		char szBuff[ 256 ] ;
		sprintf( szBuff,  "%s Load Failed!", strScriptPath ) ;
		MessageBox( m_hWnd, szBuff, "Error", MB_OK ) ;

		CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
		PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
	}
*/
	strcpy(m_pRYLGameData->m_csStatus.m_Info.Name, "");

	m_pRYLGameData->LoadTargetCursor() ;

/*	
	if (!m_pRYLGameData->m_lpCommonInterface->InitLogo())
	{
		return FALSE;
	}
*/

	return TRUE;
}

HRESULT		CClientMain::CreateScene() 
{
	// TextSize초기화
	CRYLStringTable::CreateTextSize() ;
	
	// GM Font 초기화
	CGMFont::CreateInstance() ;
	if ( FAILED( CGMFont::Instance()->InitDeviceObjects() ) )
	{
		GMASSERT( FALSE, "GMFont Failed!" ) ;
		return E_FAIL ;
	}
	
	// GM UI 초기화
	if ( FAILED( GMUIInitialize( m_hWnd, m_iScreenWidth, m_iScreenHeight ) ) )
	{
		return E_FAIL ;
	}

	// ---------------------------------------------------------------------------

	// Init Scene
	// Game Scene이외의 Scene들을 로드
	m_pRYLSceneManager = new CRYLSceneManager() ;

	// Intro Scene
	m_pRYLSceneManager->InsertScene( new CRYLIntroScene( INTRO_SCENE ) ) ;
	m_pRYLSceneManager->SetScene( INTRO_SCENE ) ;
	if ( FAILED( m_pRYLSceneManager->GetCurrentScene()->InitResourceObject() ) )
	{
		GMASSERT( FALSE, "IntroScene Resource Load Error!" ) ;
		return E_FAIL ;
	}

	// Loagin Scene
	m_pRYLSceneManager->InsertScene( new CRYLLoginScene( LOGIN_SCENE ) ) ;

	// WORK_LIST 2.2 NationSelectScene 이름 수정
	// RaceSelect Scene
	m_pRYLSceneManager->InsertScene( new CRYLRaceSelectScene( RACESELECT_SCENE ) ) ;
	m_pRYLSceneManager->SetScene( RACESELECT_SCENE ) ;
	if ( FAILED( m_pRYLSceneManager->GetCurrentScene()->InitResourceObject() ) )
	{
		return E_FAIL ;
	}
	
	// Character Scene
	m_pRYLSceneManager->InsertScene( new CRYLCharacterSelectScene( CHARACTERSELECT_SCENE ) ) ;

	// Loading Scene
	m_pLoadingScene = new CRYLLoadingScene ;

/*
	m_pRYLSceneManager->InsertScene( new CRYLLoadingScene( LOADING_SCENE ) ) ;
	m_pRYLSceneManager->SetScene( LOADING_SCENE ) ;
	if ( FAILED( m_pRYLSceneManager->GetCurrentScene()->InitResourceObject() ) )
	{
		return E_FAIL ;
	}
*/

	// Game Scene
	m_pRYLSceneManager->InsertScene( new CRYLGameScene( GAME_SCENE ) ) ;
	
	m_pRYLSceneManager->SetScene( INTRO_SCENE ) ;

	// 리소스 초기화
	//InitResourceObject() ;

	return S_OK ;
}

void CClientMain::Destroy()
{
	if (NULL != g_pClientSocket)
	{
		ClientSocket::DestroyInstance();
		//delete g_pClientSocket;
		g_pClientSocket = NULL ;
	}

	if (NULL != m_pRYLGameData)
	{
		m_pRYLGameData->DestroyTipBroad() ;
	}

	if (NULL != m_pRYLCommunityData)
	{
		m_pRYLCommunityData->DestroyRefuseList() ;
	}
    
    RYLCreatureManager::Instance()->ClearCreatureManager();	
    
#ifdef NDEBUG
	if ( !m_iWindowMode )
		RestoreDisplay();
#endif

#ifdef JAPAN_CODE
	if (m_ReqGameID != NULL)
	{
		m_ReqGameID.Release();
		m_ReqGameID = NULL;
	}
	CoUninitialize();
#endif
    
	if (m_pRYLNetworkData->m_eInternationalCode == GameRYL::KOREA)
	{
		myfirewall::Off();
	}

	g_MessageBoxManager.DestroyList() ;

	GM_DELETE( m_pLoadingScene ) ;
	GM_DELETE( m_pRYLSceneManager ) ;

	GM_DELETE( m_pRYLNetworkData ) ;
	GM_DELETE( m_pRYLCommunityData ) ;
	GM_DELETE( m_pRYLGameData ) ;
	GM_DELETE( m_pCastleManager ) ;
	GM_DELETE( m_pCampManager ) ;
	GM_DELETE( m_pSkillScriptManager ) ;
    
    GMUIFinalize() ;

	CGMFont::Instance()->Clear();
	CGMFont::DeleteInstance();

	CSoundMgr::_Destroy();

	_Z3DGlobalClose( BaseGraphicsLayer::GetDevice() );
}
//hz added for full size
void GetDesktopResolution(unsigned long& horizontal, unsigned long& vertical)//hz was without the CRYLLoginScene::
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}

//hz end

void CClientMain::Update(void)
{
	if (NULL != g_pClientSocket)
	{
		g_pClientSocket->HandleEvents(50);
	}

	CPacketProfileMgr* lpPacketProfileMgr = m_pRYLNetworkData->m_lpPacketProfileMgr;
	if (NULL != lpPacketProfileMgr)
	{
		lpPacketProfileMgr->UpdateFrame();
	}

	g_TooltipManager.m_ttRenderTarget = NULL;

	CFrameTimer::UpdateTime();
	g_DeviceInput.UpdateInfo();
	//make doesnt lose focus state of window unminimize able
	//HWND m_hWnd=g_DeviceInput.GetHwnd();
	//SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	//focus end
	if ((TRUE == g_DeviceInput.KeyHold(VK_CONTROL))&&(TRUE == g_DeviceInput.KeyHold(VK_MENU))&&(TRUE == g_DeviceInput.KeyHold(VK_DELETE)))
	{
		HWND m_hWnd=g_DeviceInput.GetHwnd();
		SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
	if ((TRUE == g_DeviceInput.KeyHold(VK_MENU))&&(TRUE == g_DeviceInput.KeyHold(VK_CONTROL))&&(TRUE == g_DeviceInput.KeyHold(VK_DELETE)))
	{
		HWND m_hWnd=g_DeviceInput.GetHwnd();
		SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
	if ((TRUE == g_DeviceInput.KeyHold(VK_CONTROL))&&(TRUE == g_DeviceInput.KeyHold(VK_MENU))&&(TRUE == g_DeviceInput.KeyHold(VK_DELETE)))
	{
		HWND m_hWnd=g_DeviceInput.GetHwnd();
		SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
	/*loop++;//hz name check hack
	if(loop > 5000)
	{
		CRYLChattingDlg* pChat = CRYLChattingDlg::Instance();
		pChat->namecheckfunction(3);
	}*/
	

	//hz end

	if (g_DeviceInput.KeyDown(VK_SCROLL) )/*[SCROLL LOCK key] for teather mode hayzohar */
	{
		if(CRYLNetworkData::Instance()->m_dwPresentZone != SERVER_ID::CHAR_SELECT)
		{
			CRYLGameScene*		            pScene          = ( CRYLGameScene* )( CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) );
			CRYLStreetStallDlg*             pStreetStallDlg = ( CRYLStreetStallDlg* )( pScene->GetStreetStallDlg() );

			if ( pStreetStallDlg && !pStreetStallDlg->GetVisible() )
			{
				if(m_pRYLGameData->m_bMovieVersion == false)
				{
					m_pRYLGameData->m_bMovieVersion = true;
					m_pRYLGameData->m_bSimpleMode = TRUE;

					pScene->MovieMode( FALSE ) ;
				}
				else
				{
					m_pRYLGameData->m_bMovieVersion = false;

					pScene->MovieMode( TRUE ) ;
				}
			}
		}
	}
	else if (g_DeviceInput.KeyDown(VK_SNAPSHOT) ) /*[PrintScreen key] for taking screenshots hayzohar*/
	{
		ProcessScreenshot();
	}


	float fSkillCheckSum = CFrameTimer::GetUpdateTimer(GRYLTimer.GetSkillCheckSumTimerID() ) ;
	if ( fSkillCheckSum >= 1.0f )
	{
		if ( !CSkillMgr::GetInstance().CheckData() )
		{
			MessageBox(g_ClientMain.m_hWnd, CRYLStringTable::m_strString[1306], CRYLStringTable::m_strString[21], MB_OK ) ;
			PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
			m_pRYLGameData->m_dwClientMode = CLIENT_END;
			return;
		}
	}

	float fUpdate = CFrameTimer::GetUpdateTimer(GRYLTimer.GetShowInterfaceTimerID());
	if (fUpdate >= 1.0f)
	{
		//avalansa speed hack
//		 if(((fUpdate > 8.7f) && (fUpdate < 15.0f))&& (fSkillCheckSum < 1.0f))//hz speed hack test check
//		 {
//			MessageBox(NULL, "Speed hack detected!.","GP-Client Error!", MB_OK);
//			exit(10);
//		 }
		m_pRYLCommunityData->UpdateRefuseList(fUpdate * 2);
		
		m_pRYLGameData->UpdateQuickChatTimer( fUpdate ) ;
		m_pRYLGameData->UpdateTipBroadCounter( fUpdate ) ;
		m_pRYLGameData->UpdatePartyAttackTimer( fUpdate ) ;
		
		// Shout, FameInfo.. etc	Counter 처리
        CRYLNetworkData::ProcessCounter( fUpdate ) ;
		
		m_pRYLGameData->UpdateSkillUseCounter( fUpdate ) ;
		m_pRYLGameData->UpdatePortalCounter( fUpdate ) ;
		m_pRYLGameData->UpdateKillCounter( fUpdate ) ;
		m_pRYLGameData->UpdateClientExitCounter( fUpdate ) ;
		m_pRYLGameData->UpdateBillingTimer( fUpdate ) ;		

//		m_pRYLGameData->UpdateRespawnCounter( fUpdate ) ;		

		m_pRYLGameData->UpdateGameTimer( fUpdate ) ;		
		m_pRYLGameData->UpdatePremiumTimer( fUpdate ) ;		
		
		m_pRYLGameData->UpdateLogoutCounter( fUpdate ) ;

		CRYLGameTimeManager::Instance()->UpdateGuildTime(fUpdate*2);
		CRYLGameTimeManager::Instance()->UpdateRealmTime(fUpdate*2);
	}

	// ------------------------------------------------------------------------------------

	m_pRYLGameData->UpdateFrameTimer() ;

	// ------------------------------------------------------------------------------------

	if (m_pRYLGameData->m_dwClassCleanResult & MB_YES)
	{
		unsigned char cBasicClass = 0;
		switch (m_pRYLGameData->m_csStatus.m_Info.Class)
		{
			case 5:
			case 6:
				cBasicClass = 1;
				break;

			case 7:
			case 8:
				cBasicClass = 2;
				break;

			case 9:
			case 10:
				cBasicClass = 3;
				break;

			case 11:
			case 12:
				cBasicClass = 4;
				break;

			case 19:
			case 20:
			case 21:
				cBasicClass = 17;
				break;

			case 22:
			case 23:
			case 24:
				cBasicClass = 18;
				break;
		}

		if (cBasicClass) SendPacket::CharClassUpgrade(g_GameSession, m_pRYLNetworkData->m_dwMyChrID, cBasicClass);

		m_pRYLGameData->m_dwClassCleanResult = 0;
		return;
	} 
	else if (m_pRYLGameData->m_dwClassCleanResult & MB_NO || m_pRYLGameData->m_dwClassCleanResult & MB_EXIT)
	{
		m_pRYLGameData->m_dwClassCleanResult = 0;
		return;
	}

	if (m_pRYLGameData->m_dwClientLoginResult & MB_YES)
	{
		m_wClientLoginFlag |= 0x2000;
		g_pClientSocket->SetStatusFlag(NS_AUTHACCOUNT);
		m_pRYLNetworkData->SendLoginID(m_pRYLNetworkData->m_strLoginID);
		SendPacket::AuthAccount(*g_pClientSocket, m_pRYLNetworkData->m_strLoginID, m_pRYLNetworkData->m_strLoginPass, m_dwClientVersion, m_dwVersion, m_wClientLoginFlag);
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[905]);
		lpMessageBox->SetTimer(100000);

		m_pRYLGameData->m_dwClientLoginResult = 0;

		return;
	} 
	else if ( m_pRYLGameData->m_dwClientLoginResult & MB_NO || m_pRYLGameData->m_dwClientLoginResult & MB_EXIT )
	{
		m_pRYLGameData->m_dwClientLoginResult = 0;
		return;
	}

	if ( m_pRYLGameData->m_dwClientExitConfirm & MB_CONFIRM || m_pRYLGameData->m_dwClientExitConfirm & MB_EXIT )
	{
		m_pRYLGameData->m_dwClientExitConfirm = 0 ;
		PostMessage(m_hWnd, WM_DESTROY, 0, 0);
		m_pRYLGameData->m_dwClientMode = CLIENT_END;
		return;
	}

	if ( m_pRYLGameData->m_dwExecuteExplorerConfirm & MB_CONFIRM || m_pRYLGameData->m_dwExecuteExplorerConfirm & MB_EXIT )
	{
		m_pRYLGameData->m_dwExecuteExplorerConfirm = 0 ;

		char szPath[1024] = { 0, };
		strcpy(szPath, "http://www.rylartofwar.com");//was returnofwarrior.com hayzohar

		ShellExecute(NULL, "open", szPath, NULL, NULL, SW_SHOW);

		PostMessage(m_hWnd, WM_DESTROY, 0, 0);
		m_pRYLGameData->m_dwClientMode = CLIENT_END;
		return;
	}

	if (m_pRYLGameData->m_dwRespawnResult & MB_YES)
	{
		g_pClientSocket->SetStatusFlag(NS_RESPAWN);
		POS psPos = { 0.0f, 0.0f, 0.0f };
		m_pRYLNetworkData->SetNetworkFlag( NF_RESPAWN ) ; 
		SendPacket::CharRespawn(g_GameSession, m_pRYLNetworkData->m_dwMyChrID, ClientConstants::RespawnCmd_Normal, psPos, 0);

		m_pRYLGameData->m_dwRespawnResult = 0;

		return;
	} 
	else if (m_pRYLGameData->m_dwRespawnResult & MB_NO || m_pRYLGameData->m_dwRespawnResult & MB_EXIT)
	{
		POS psPos;
		switch (m_pRYLNetworkData->m_dwPresentZone)
		{
			case SERVER_ID::ZONE4:
				if (m_pRYLGameData->m_dwClientRace == CClass::RaceType::HUMAN)
				{
					psPos.fPointX = 1870.78f;
					psPos.fPointY = 14.20f;
					psPos.fPointZ = 1601.52f;
				} 
				else if (m_pRYLGameData->m_dwClientRace == CClass::RaceType::AKHAN)
				{
					psPos.fPointX = 674.59f;
					psPos.fPointY = 9.45f;
					psPos.fPointZ = 1457.43f;
				}
				break;

			case SERVER_ID::ZONE5:
				if (m_pRYLGameData->m_dwClientRace == CClass::RaceType::HUMAN)
				{
					psPos.fPointX = 934.73f;
					psPos.fPointY = 44.79f;
					psPos.fPointZ = 2719.47f;
				} 
				else if (m_pRYLGameData->m_dwClientRace == CClass::RaceType::AKHAN)
				{
					psPos.fPointX = 3041.62f;
					psPos.fPointY = 103.18f;
					psPos.fPointZ = 878.35f;
				}
				break;
		}

		g_pClientSocket->SetStatusFlag(NS_RESPAWN);
		m_pRYLNetworkData->SetNetworkFlag( NF_RESPAWN ) ; 
		SendPacket::CharRespawn(g_GameSession, m_pRYLNetworkData->m_dwMyChrID, ClientConstants::RespawnCmd_Select, psPos, 0);

		m_pRYLGameData->m_dwRespawnResult = 0;

		return;
	}

/*
	if (CRYLCommunityData::Instance()->m_dwFriendResult & MB_YES)
	{
		SendPacket::CharFriendAddRequest(g_GameSession, m_pRYLCommunityData->m_strFriendName, PktFriendAddReq::ADD_FRIEND_REQ);
		CRYLCommunityData::Instance()->m_dwFriendResult = 0;

		return;
	} 
	else if (CRYLCommunityData::Instance()->m_dwFriendResult & MB_NO || CRYLCommunityData::Instance()->m_dwFriendResult & MB_EXIT)
	{
		CRYLCommunityData::Instance()->m_dwFriendResult = 0;

		return;
	}
*/

	if (CRYLCommunityData::Instance()->m_dwPeaceModeResult & MB_YES)
	{
		if (CRYLCommunityData::Instance()->m_dwPeaceMode)
		{
			SendPacket::CharPeaceMode(g_GameSession, m_pRYLNetworkData->m_dwMyChrID, true);
		} else
		{
			SendPacket::CharPeaceMode(g_GameSession, m_pRYLNetworkData->m_dwMyChrID, false);
		}

		CRYLCommunityData::Instance()->m_dwPeaceMode = 0;
		CRYLCommunityData::Instance()->m_dwPeaceModeResult = 0;

		return;
	} 
	else if (CRYLCommunityData::Instance()->m_dwPeaceModeResult & MB_NO || CRYLCommunityData::Instance()->m_dwPeaceModeResult & MB_EXIT)
	{
		CRYLCommunityData::Instance()->m_dwPeaceMode = 0;
		CRYLCommunityData::Instance()->m_dwPeaceModeResult = 0;

		return;
	}

	if ( m_pRYLGameData->m_dwTradePickResult & MB_YES )
	{
		Item::ItemPos	pPos ;
		CRYLNetworkData::SendTradeItem( PktTr::TRC_SELL, m_pRYLGameData->m_dwTradingNpcID, 0, 
			TakeType(m_pRYLGameData->m_lpTradeItem->m_lpItemBase->GetPos(), pPos, 1), Item::ItemPos(), NULL );

		m_pRYLGameData->m_dwTradePickResult = 0;
		return;
	}
	else if ( m_pRYLGameData->m_dwTradePickResult & MB_NO  )
	{
		m_pRYLGameData->m_lpTradeItem = NULL;
		m_pRYLGameData->m_dwTradePickResult = 0;
		return;
	}


	if ( m_pRYLGameData->m_dwDropPickResult & MB_YES )
	{
		g_pClientSocket->SetStatusFlag(NS_TAKEITEM);

		unsigned long dwNum = (m_pRYLGameData->m_lpPickItem->GetIsEnableStack()) ? m_pRYLGameData->m_lpPickItem->GetNowDurability() : 1;
		if(CRYLNetworkData::SendFieldObjectPullDown(dwNum, m_pRYLGameData->m_lpPickItem->m_lpItemBase->GetPos()))
			m_pRYLGameData->m_bPullDownFlag = TRUE;
		else
			m_pRYLGameData->m_bPullDownFlag = FALSE;

		m_pRYLGameData->m_dwDropPickResult = 0;
		return;
	}
	else if ( m_pRYLGameData->m_dwDropPickResult & MB_NO  )
	{
		m_pRYLGameData->m_dwDropPickResult = 0;
		return;
	}

	if ( m_pRYLGameData->m_dwTempInvenResult & MB_YES )
	{
		g_pClientSocket->SetStatusFlag(NS_TAKEITEM);

		Item::ItemPos pItemIndex;
		pItemIndex.m_cPos = TakeType::TS_TEMPINVEN;
		pItemIndex.m_cIndex = 0;

		if(CRYLNetworkData::SendFieldObjectPullDown( m_pRYLGameData->m_cTempInvenItemNum, pItemIndex ))
			m_pRYLGameData->m_bPullDownFlag = TRUE;
		else
			m_pRYLGameData->m_bPullDownFlag = FALSE;

		CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[124], CRYLNetworkData::Instance()->m_dwMyChrID, 129);

		m_pRYLGameData->m_dwTempInvenResult = 0;
		m_pRYLGameData->m_cTempInvenItemNum = 0;
		return;
	}
	else if ( m_pRYLGameData->m_dwTempInvenResult & MB_NO || m_pRYLGameData->m_dwTempInvenResult & MB_EXIT )
	{
		m_pRYLGameData->m_dwTempInvenResult = 0;
		m_pRYLGameData->m_cTempInvenItemNum = 0;
		return;
	}

	if ( m_pRYLGameData->m_dwAuthorizeResult & MB_YES)
	{
		SendPacket::CharAuthorizePanel( g_GameSession, 
										m_pRYLNetworkData->m_dwMyChrID, 
										m_pRYLGameData->m_dwAuthorizeCasterID, 
										m_pRYLGameData->m_cAuthorizeCmd ) ;

		m_pRYLGameData->m_dwAuthorizeResult = 0;
		m_pRYLGameData->m_dwAuthorizeCasterID = 0;
		m_pRYLGameData->m_cAuthorizeCmd = 0;

		return;
	} 
	else if (m_pRYLGameData->m_dwAuthorizeResult & MB_NO || m_pRYLGameData->m_dwAuthorizeResult & MB_EXIT)
	{
		m_pRYLGameData->m_dwAuthorizeResult = 0;
		m_pRYLGameData->m_dwAuthorizeCasterID = 0;
		m_pRYLGameData->m_cAuthorizeCmd = 0;

		return;
	}

	if ( m_pRYLGameData->m_dwRealmWarJoinResult & MB_YES )
	{
		SendPacket::WarOnOff(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, Creature::WAR_INSTANCE, GameTime::REALM);
		m_pRYLGameData->m_dwRealmWarJoinResult = 0;
		return;
	}
	else if ( m_pRYLGameData->m_dwRealmWarJoinResult & MB_NO || m_pRYLGameData->m_dwRealmWarJoinResult & MB_EXIT )
	{
		m_pRYLGameData->m_dwRealmWarJoinResult = 0;
		return;
	}

	// ---------------------------------------------------------------------------------
	// 길드 관련 메시지창 처리

	// 길드 멤버탭의 참여/불참 선언
	if ( m_pRYLGameData->m_dwGuildWarJoinLeaveResult & MB_YES )
	{
		SendPacket::WarOnOff( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, Creature::WAR_ON, GameTime::GUILD ) ;
		m_pRYLGameData->m_dwGuildWarJoinLeaveResult = 0;
		return;
	}
	else if ( m_pRYLGameData->m_dwGuildWarJoinLeaveResult & MB_NO || m_pRYLGameData->m_dwGuildWarJoinLeaveResult & MB_EXIT )
	{
		m_pRYLGameData->m_dwGuildWarJoinLeaveResult = 0;
		return;
	}
	
	if ( m_pRYLGameData->m_dwRequestResult & MB_YES )
	{
		SendPacket::CharGuildRelation( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_pRYLGameData->m_csStatus.m_Info.GID,
			m_pRYLGameData->m_dwTargetGID, m_pRYLGameData->m_dwAlreadyHostilityGID, PktGuildRelation::GR_REQUEST ) ;

		m_pRYLGameData->m_dwRequestResult = 0 ;
		m_pRYLGameData->m_dwTargetGID = 0 ;
		m_pRYLGameData->m_dwAlreadyHostilityGID = 0 ;
		return ;
	}
	else if ( m_pRYLGameData->m_dwRequestResult & MB_NO || m_pRYLGameData->m_dwRequestResult & MB_EXIT )
	{
		m_pRYLGameData->m_dwRequestResult = 0 ;
		m_pRYLGameData->m_dwTargetGID = 0 ;
		m_pRYLGameData->m_dwAlreadyHostilityGID = 0 ;
		return ;
	}

	if ( m_pRYLGameData->m_dwCounterHostilityResult & MB_YES )
	{
		SendPacket::CharGuildRelation( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_pRYLGameData->m_csStatus.m_Info.GID,
			m_pRYLGameData->m_dwTargetGID, 0, PktGuildRelation::GR_COUNTER ) ;

		m_pRYLGameData->m_dwCounterHostilityResult = 0 ;
		m_pRYLGameData->m_dwTargetGID = 0 ;
		return ;
	}
	else if ( m_pRYLGameData->m_dwCounterHostilityResult & MB_NO || m_pRYLGameData->m_dwCounterHostilityResult & MB_EXIT )
	{
		m_pRYLGameData->m_dwCounterHostilityResult = 0 ;
		m_pRYLGameData->m_dwTargetGID = 0 ;
		return ;
	}

	if ( CRYLCommunityData::Instance()->m_iGulidMsgBoxState == PktGuildCmd::GC_INVITE )
	{
        CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;

        RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( CRYLCommunityData::Instance()->m_dwRefenceCID ); 
        if ( !pTempCreature )
            return;

        CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
        if ( !pTempData )
            return;

		CRYLGameScene*	pScene = ( CRYLGameScene* )( CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ) ;	
		if( !pScene )
			return;

		if ( pScene->GetGuildFrameDlg() )
		{
			LPGUILDLARGEINFOEX pGuildInfo;
			pGuildInfo = ( (CRYLGuildInfoPanel*) ( ( CRYLGuildFrameDlg * )pScene->GetGuildFrameDlg() )->GetGuildInfoPanel() )->FindGuild( pTempData->m_ulGuildID ) ;
			if( !pGuildInfo )
				return;
			if( pGuildInfo->m_cInclination == m_pRYLGameData->m_cAccountNation )
			{
				CHAR szBuff[ 128 ] ;
				sprintf( szBuff, CRYLStringTable::m_strString[2977], pTempData->m_strName.c_str(), pGuildInfo->m_szName ) ;
				lpMessageBox->Create( szBuff, MB_YES | MB_NO | MB_EXIT ) ;
				lpMessageBox->SetResult( &CRYLCommunityData::Instance()->m_dwResInvite ) ;
				CRYLCommunityData::Instance()->m_iGulidMsgBoxState = -1 ;
			}
		}
	}

	if ( CRYLCommunityData::Instance()->m_dwResInvite & MB_YES )
	{
		// edith 2008.02.27 길드 가입 제한제외
		if (Creature::LEVEL_ABLE_WAR > CRYLGameData::Instance()->m_csStatus.m_Info.Level)
		{
			char szErrMsg[MAX_PATH];
			sprintf(szErrMsg, CRYLStringTable::m_strString[1095], Creature::LEVEL_ABLE_WAR);

			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(szErrMsg);
		}
		else
		{
			// 초청에 의한 가입 허가
			SendPacket::CharGuildCmd( g_GameSession, CRYLCommunityData::Instance()->m_dwRefenceGID, m_pRYLNetworkData->m_dwMyChrID, 
				CRYLCommunityData::Instance()->m_dwRefenceCID, PktGuildCmd::GC_JOIN ) ;
		}

		CRYLCommunityData::Instance()->m_dwResInvite = 0 ;
	}
	else if ( ( CRYLCommunityData::Instance()->m_dwResInvite & MB_NO ) || ( CRYLCommunityData::Instance()->m_dwResInvite & MB_EXIT ) )
	{
		// 초청에 의한 가입 거절
		SendPacket::CharGuildCmd( g_GameSession, CRYLCommunityData::Instance()->m_dwRefenceGID, m_pRYLNetworkData->m_dwMyChrID, 
			CRYLCommunityData::Instance()->m_dwRefenceCID, PktGuildCmd::GC_REFUSE ) ;
		CRYLCommunityData::Instance()->m_dwResInvite = 0 ;
	}

	CRYLGameScene*	pScene = ( CRYLGameScene* )( CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ) ;	
	
	if ( pScene )
	{	
		if ( pScene->GetGuildFrameDlg() )
		{
			( ( CRYLGuildFrameDlg * )pScene->GetGuildFrameDlg() )->UpdateGuildMessageBox() ;
		}
	}

	// ---------------------------------------------------------------------------------

	if (m_pRYLGameData->m_csStatus.m_lstQuest.size())
	{
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if ( pSelfCreature )
		{
			unsigned long dwTriggerCount;
			QuestInstance* lpQuest = NULL;
            TriggerNode* lpTrigger = m_pRYLGameData->m_csStatus.GetTriggertoGeton(lpQuest, &dwTriggerCount, *( pSelfCreature->GetPosition() ) );
			if (lpTrigger)
			{
				SendPacket::CharOperateTrigger(g_GameSession, m_pRYLNetworkData->m_dwMyChrID,
					(unsigned short)lpQuest->m_dwQuestID, (unsigned char)lpQuest->m_dwPresentPhase,
					(unsigned char)TRIGGER_GETON, (unsigned char)dwTriggerCount, (unsigned char)lpQuest->m_lstTriggerCount[dwTriggerCount]);
				lpQuest->m_lstTriggerCount[dwTriggerCount] = 0;
			}
		}
	}

	// 위치 퀘스트를 받을 것이 있는가 체크
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( pSelfCreature && g_pClientSocket->GetStatusFlag() != NS_REQUESTQUEST )
	{
		if( !ClientSocket::IsNSFlagChainChecked( CmdCharStartQuest ) )
		{
			QuestNode* lpQuest = m_pRYLGameData->m_csStatus.GetQuestToGeton( *( pSelfCreature->GetPosition() ) ) ;
			if (lpQuest)
			{
				if ( m_pRYLGameData->m_csStatus.CheckQuestInvenSpace(lpQuest) )
				{
					SendPacket::CharStartQuest(g_GameSession, m_pRYLNetworkData->m_dwMyChrID, 0, lpQuest->m_wQuestID ) ;
					g_pClientSocket->SetStatusFlag( NS_REQUESTQUEST );
				}
				else
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[ 3569 ]);
				}
			}
		}
	}

	if ( CRYLSceneManager::GetCurrentScene() )
		CRYLSceneManager::GetCurrentScene()->Update() ;

	// ---------------------------------------------------------------------------------
	// interface effect

	m_pRYLGameData->UpdateInterfaceEffect() ;

	// ---------------------------------------------------------------------------------

	if (CFrameTimer::GetUpdateTimer( GRYLTimer.GetMoveUpdateTimer() ) >= 1.0f)
	{
		if (m_pRYLGameData->m_dwClientMode == CLIENT_GAME)
		{
            RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
            if ( !pSelfCreature )
                return;

            if (!pSelfCreature->GetIsDead() && m_pRYLGameData->m_bUpdateMove)
			{
                float fDir = pSelfCreature->GetDirection() - (FLOAT_PHI / 2.0f);
				vector3 vecPos = vector3(
					pSelfCreature->GetPosition()->x / 100.0f, 
					pSelfCreature->GetPosition()->y / 100.0f, 
					pSelfCreature->GetPosition()->z / 100.0f);
				CRYLNetworkData::SendMoveUpdatePacket(fDir, vecPos);
			}
		}

		m_pRYLGameData->m_bUpdateMove = !m_pRYLGameData->m_bUpdateMove;
	}

	if ( m_pRYLNetworkData->m_bChinaBillingTime )
	{
		if ( CFrameTimer::GetUpdateTimer( GRYLTimer.GetChinaBillingTimeID() ) >= 1.0f )
		{
			CRYLNoticeManager::Instance()->AddNotice( CRYLStringTable::m_strString[ 3378 ], NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;
		}
	}
	
	if ( m_pRYLGameData->m_dwClientState == GAME_PLAYING )
	{
		CSceneManager::m_fLife = ( FLOAT )( m_pRYLGameData->m_csStatus.GetCurrentHP() ) / ( m_pRYLGameData->m_csStatus.GetMaxHP() ) ;

		// 강제로 이팩트를 수정.
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if(pSelfCreature)
		{
	        if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Drunk ) )
			{
				CSceneManager::m_fLife = 0.0f;
			}
		}
	}

#ifndef NO_GAMEGUARD
	// edith 2009.08.11 게임가드 2.5 업그레이드
//	if (m_pRYLNetworkData->m_GGAuthCode.CheckFlag(GGAuthCode::GG_AUTH_CODE_1))
//	{
//		SendPacket::CSAuthReturnCode( g_GameSession, m_pRYLNetworkData->m_dwMyChrID, 1,
//			m_pRYLNetworkData->m_GGAuthCode.m_dwAuthCode, &m_pRYLNetworkData->m_GGAuthCode.m_AuthCode2 );
//	}

	if (m_pRYLNetworkData->m_GGAuthCode.CheckFlag(GGAuthCode::GG_AUTH_CODE_2))
	{
		SendPacket::CSAuthReturnCode( g_GameSession, m_pRYLNetworkData->m_dwMyChrID, 2,
			m_pRYLNetworkData->m_GGAuthCode.m_dwAuthCode, &m_pRYLNetworkData->m_GGAuthCode.m_AuthCode2 );
	}
#endif
}

void CClientMain::Render(void)
{
	CSceneManager::m_cRenderPolygon = 0;
	CSceneManager::m_cRenderVertex = 0;

	CSceneManager::m_cRenderSectorPoly = 0;
	CSceneManager::m_cRenderSectorVertex = 0;

	{
		PROFILE("Render Ready");
		m_BaseGraphicLayer.PrefareRender();
		m_BaseGraphicLayer.GetDevice()->BeginScene();
	}

	{
		RenderMain();
	}

	{
		PROFILE("Status Check");
		////////////////////////// 스테이터스 체크
		// edith 2008.03.13 클라이언트에서 해킹을 방지하기 위해 스테이터스의 수치를 체크하는 부분
		if (m_wAdminMode != ADMIN_L3)
		{
			float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetCheckStatusTimerID() ) ;
			if (fUpdate >= 1.0f)
			{
				if (!m_pRYLGameData->m_csStatus.CheckStatus())
				{
					char strText[MAX_PATH];
					strcpy(strText, "[Hack Log] ");
					strcat(strText, CRYLStringTable::m_strString[901]);
					SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_ClientLog, strText, strlen(strText), 0);

					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[902]);
					
					PostQuitMessage(0);
				}
			}
		}
	}

	unsigned long dwKindCursor = CURSOR_NORMAL ;
	{
		PROFILE("Select Cursor");
		////////////////////////// 커서 선택
		if (m_pRYLGameData->m_dwKindCursor == CURSOR_NORMAL)
		{
			if (g_TooltipManager.m_ttRenderTarget)
			{
				dwKindCursor = CURSOR_TOOLTIP;
			}
			else if (g_DeviceInput.GetIsLeftMousePress())
			{
				dwKindCursor = CURSOR_CLICK;
			}
			else
			{
				dwKindCursor = CURSOR_NORMAL;
			}

			if (m_pRYLGameData->m_lpInterface)
			{
				if (m_pRYLGameData->m_lpPickItem && g_MessageBoxManager.m_lstMessageBox.empty())
				{
					dwKindCursor = CURSOR_PICKUP;
				} 
				else if (m_pRYLGameData->m_lpInterface->m_bPopupShow)
				{
					dwKindCursor = CURSOR_POPUP;
				} 
				else if (m_pRYLGameData->m_lpInterface->m_dwNormalTargetChrID != 0xFFFFFFFF)
				{
					if (CRYLGameData::GetClientEnemyType(m_pRYLGameData->m_lpInterface->m_dwNormalTargetChrID ) == EnemyCheck::EC_ENEMY)
					{
						dwKindCursor = CURSOR_ATTACK;
					}
					else
					{
						dwKindCursor = CURSOR_DIALOG;
					}
				}
			}
		} 
		else if (m_pRYLGameData->m_dwKindCursor == CURSOR_REPAIR)
		{
			dwKindCursor = CURSOR_REPAIR;
		}

		if (!m_pRYLGameData->m_bHardwareCursor && m_pRYLGameData->m_bShowCursor)
		{	// 소프트웨어 커서
			POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
			m_pRYLGameData->RenderCursor(m_BaseGraphicLayer.GetDevice(), ptMousePos->x, ptMousePos->y, m_pRYLGameData->m_dwKindCursor);
		}
	}

	{
		PROFILE("Flip");
		m_BaseGraphicLayer.GetDevice()->EndScene();
		m_BaseGraphicLayer.Flip();
	}

	{
		PROFILE("Hardware Cursor");
		if (m_pRYLGameData->m_bHardwareCursor)
		{	// 하드웨어 커서
			switch (dwKindCursor)
			{
				case CURSOR_NORMAL:
					::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_NORMALCURSOR)));
					break;

				case CURSOR_CLICK:
					::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_CLICKCURSOR)));
					break;

				case CURSOR_PICKUP:
					::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_PICKUP)));
					break;

				case CURSOR_ATTACK:
					::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_ATTACK)));
					break;

				case CURSOR_DIALOG:
					::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_DIALOG)));
					break;

				case CURSOR_REPAIR:
					::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_REPAIR)));
					break;

				case CURSOR_TOOLTIP:
					::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_TOOLTIP)));
					break;

				case CURSOR_POPUP:
					::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_POPUP)));
					break;
			}

			g_DeviceInput.ShowCursor(m_pRYLGameData->m_bShowCursor);
		}
	}
}

void CClientMain::RenderMain()
{
	if ( CRYLSceneManager::GetCurrentScene() )
	{
		CRYLSceneManager::GetCurrentScene()->Render( m_lpD3DDevice ) ;
		CGMUIScreen::GetInstance()->GMUIPaint() ;	
		CRYLSceneManager::GetCurrentScene()->FinalRender( m_lpD3DDevice ) ;
	}

	switch (m_pRYLGameData->m_dwClientMode)
	{
		case CLIENT_LOGO:
			break;

		case CLIENT_MAIN:
			break;
		
		case CLIENT_NATIONSELECT:			
			break;

		case CLIENT_CHARACTERSELECTLOADING:
			m_pRYLGameData->m_dwClientMode = CLIENT_CHARACTERSELECT;
			break;
		
		case CLIENT_CHARACTERSELECT:
			break;
		
		case CLIENT_GAMELOADING:
			m_pRYLGameData->m_dwClientMode = CLIENT_GAME;
			break;

		case CLIENT_GAME:						
			break;
		case CLIENT_ZONEMOVE:
			break;
	}
	
	{
		PROFILE("Interface Effect");
		/////////////// 인터페이스 이팩트
		DWORD effcullmode;
		DWORD effzmode;
		DWORD effalphamode;

		m_BaseGraphicLayer.GetDevice()->GetRenderState(D3DRS_CULLMODE,&effcullmode);
		m_BaseGraphicLayer.GetDevice()->GetRenderState(D3DRS_ZENABLE,&effzmode);
		m_BaseGraphicLayer.GetDevice()->GetRenderState(D3DRS_ALPHABLENDENABLE,&effalphamode);
		
		CSceneManager::m_EffectManager.RenderInterfaceScript();

		m_BaseGraphicLayer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_BaseGraphicLayer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		m_BaseGraphicLayer.GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_BaseGraphicLayer.GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_BaseGraphicLayer.GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_BaseGraphicLayer.GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_BaseGraphicLayer.GetDevice()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_BaseGraphicLayer.GetDevice()->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_BaseGraphicLayer.GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_BaseGraphicLayer.GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_BaseGraphicLayer.GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		m_BaseGraphicLayer.GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
		m_BaseGraphicLayer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

		m_BaseGraphicLayer.GetDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
		m_BaseGraphicLayer.GetDevice()->SetRenderState(D3DRS_CULLMODE,effcullmode);
		m_BaseGraphicLayer.GetDevice()->SetRenderState(D3DRS_ZENABLE,effzmode);
		m_BaseGraphicLayer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,effalphamode);

		//전직시 같이 나오는 state 출력부
		m_pRYLGameData->RenderClassEffect() ;
	}
	

	{
		PROFILE("Etc Render");
/*
		// edith 2008.01.18 ShowState 화면에 위치정보등의 출력
		m_BaseGraphicLayer.ShowState();
		
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if(pSelfCreature)
		{
			char strTemp[128];
			CZ3DGeneralChrModel* pTempModel = pSelfCreature->GetCharModel();
			if(pTempModel)
			{
				sprintf(strTemp, "X : %f, Y : %f, Z : %f, Dir : %f", pTempModel->GetPosition().x, pTempModel->GetPosition().y, pTempModel->GetPosition().z, pTempModel->GetDirection());
				m_BaseGraphicLayer.DrawText(500, 20, D3DCOLOR_ARGB(255,255,0,0), strTemp);
			}
		}
*/


		///////////////////////////////	
		if (m_pRYLGameData->m_bShowInfo) 
			m_BaseGraphicLayer.ShowState();

		if (!m_pRYLGameData->m_lpPickItem)
			g_TooltipManager.Render(m_BaseGraphicLayer.GetDevice());
	}

	if ( m_pRYLGameData->m_bFPS == TRUE )
	{
		ViewFPS();
	}
}

void CClientMain::SetDayTimer()
{
/*	// 할로윈 이벤트
	// 존별로 시간을 지정한다.
	if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE1 ||
		CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE2 ||
		CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE12 ||
		CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STONE_WAR1 )
	{			
		int nTime = 3;
		GRYLTimer.m_dwStartTime = timeGetTime();
		GRYLTimer.m_dwServerTime = nTime * 600000;
	}
*/	
	unsigned long	dwNowTime = timeGetTime();
	unsigned long	dwInter = dwNowTime - GRYLTimer.GetStartTime() ;

	dwInter = dwInter + GRYLTimer.GetServerTime() ;
	dwInter = dwInter % 14400000;

	float fDayTime = (dwInter / 14400000.0f) * 24.0f;

	m_pRYLGameData->m_lpSceneManager->SetWeatherTime(fDayTime);
}

void CClientMain::ProcessScreenshot()
{
	static unsigned long dwNumBMPScreenShot = 1;
	static unsigned long dwNumJPGScreenShot = 1;
	unsigned long dwNumScreenShot;

	LPDIRECT3DSURFACE8 frontbuf; //this is our pointer to the memory location containing our copy of the front buffer
	D3DDISPLAYMODE dmDisplay;

	CSceneManager::m_cRenderPolygon = 0;
	CSceneManager::m_cRenderVertex = 0;
	CSceneManager::m_cRenderSectorPoly = 0;
	CSceneManager::m_cRenderSectorVertex = 0;

	m_BaseGraphicLayer.PrefareRender();
	m_BaseGraphicLayer.GetDevice()->BeginScene();

	RenderMain();

	TCHAR strFilename[MAX_PATH], strTemp[MAX_PATH], strMessage[MAX_PATH], strExt[MAX_PATH];

	static CHAR strText[ MAX_PATH ] ;

	switch ( CRYLNetworkData::m_dwServerID )
	{
		case 0:		strcpy( strText, CRYLStringTable::m_strString[ 2228 ] ) ;	break;
		case 1:		strcpy( strText, CRYLStringTable::m_strString[ 2229 ] ) ;	break;
		case 2:		strcpy( strText, CRYLStringTable::m_strString[ 2230 ] ) ;	break;
		case 3:		strcpy( strText, CRYLStringTable::m_strString[ 2231 ] ) ;	break;
		default:	strcpy( strText, CRYLStringTable::m_strString[ 2232 ] ) ;	break;
	}

	if(g_DeviceInput.KeyHold(VK_SHIFT))
	{
		strcpy(strExt, "bmp");
		dwNumScreenShot = dwNumBMPScreenShot;
	} else
	{
		strcpy(strExt, "jpg");
		dwNumScreenShot = dwNumJPGScreenShot;
	}

	// GMT타임구하기
	time_t rawtime;
	tm * ptm;
	time ( &rawtime );
	ptm = gmtime ( &rawtime );

	if (strcmp(m_pRYLGameData->m_csStatus.m_Info.Name, ""))
	{
		sprintf(strTemp, "%s_%s_%04d_%02d_%02d_%02d_%02d(%d).%s", strText, m_pRYLGameData->m_csStatus.m_Info.Name,
			1900+ptm->tm_year, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour+1, ptm->tm_min+1,
			dwNumScreenShot, strExt);
	} 
	else
	{
		sprintf(strTemp, "%s_Ryl_%04d_%02d_%02d_%02d_%02d(%d).%s", strText, //was ROW_%04d_%02d_%02d_%02d_%02d(%d).%s"
			1900+ptm->tm_year, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour+1, ptm->tm_min+1,
			dwNumScreenShot, strExt);
	}

	RECT rcRect = { 3, 465, 800, 480 };

	if(m_pRYLGameData->m_bMovieVersion == true)
	{
		SetRect(&rcRect, 10, 10, 800, 30 );
	}

	sprintf(strMessage, CRYLStringTable::m_strString[29], strTemp);
	g_TextEdit.DrawText(strMessage, &rcRect, DT_LEFT | DT_VCENTER, D3DCOLOR_RGBA(255, 222, 0, 255));

	m_BaseGraphicLayer.GetDevice()->EndScene();
	m_BaseGraphicLayer.Flip();

	m_BaseGraphicLayer.GetDevice()->GetDisplayMode(&dmDisplay);
	//now we create the image that our screen shot will be copied into
	//NOTE: Surface format of the front buffer is D3DFMT_A8R8G8B8 when it is returned
	m_BaseGraphicLayer.GetDevice()->CreateImageSurface(dmDisplay.Width, dmDisplay.Height, D3DFMT_A8R8G8B8, &frontbuf);

	//now we copy the front buffer into our surface
	HRESULT hr = m_BaseGraphicLayer.GetDevice()->GetFrontBuffer(frontbuf);

	//error checking
	if (hr != D3D_OK)
	{
		//do error handling etc...
		frontbuf->Release(); //release the surface so there is no memory leak
		return;
	} 

	sprintf(strFilename, "%s\\ScreenShots\\%s", g_ClientMain.m_strClientPath, strTemp);

	HANDLE hFile = CreateFile(strFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		char strDirTemp[MAX_PATH];
		sprintf(strDirTemp, "%s\\ScreenShots", g_ClientMain.m_strClientPath);
		if (!CreateDirectory(strDirTemp, NULL))
		{
			frontbuf->Release();
			return;
		}
	}
	CloseHandle(hFile);

	if(g_DeviceInput.KeyHold(VK_SHIFT))
	{
		hr = D3DXSaveSurfaceToFile(strFilename, D3DXIFF_BMP, frontbuf, NULL, NULL);

		dwNumBMPScreenShot++;
	} else
	{
		if (!m_pRYLGameData->EncodeJPGFile(strFilename, frontbuf))
		{
			;
		}

		dwNumJPGScreenShot++;
	}

	//release the surface so there is no memory leak
	frontbuf->Release();
}


void CClientMain::WriteLogtoFile(const char *strLog, const char *strFileName, int nLine)
{
	FILE *fp;
	char strFilename[MAX_PATH], strRealLog[1000];
	SYSTEMTIME pSystemTime;
	GetLocalTime(&pSystemTime);

	sprintf(strFilename, "%s\\Log.txt", m_strClientPath);
	if (fp = fopen(strFilename, "ab"))
	{
		fseek(fp, 0, SEEK_END);
		sprintf(strRealLog, "[%04d:%02d:%02d:%02d:%02d] %s(%d) : %s\n", pSystemTime.wYear, pSystemTime.wMonth, pSystemTime.wDay,
			pSystemTime.wHour, pSystemTime.wMinute, strFileName, nLine, strLog);
		unsigned int nLength = strlen(strRealLog);
		fwrite((const void *)strRealLog, nLength, 1, fp);
		fclose(fp);
	}
}
/*
void CClientMain::PrintException(EXCEPTION_RECORD &SavedExceptRec, CONTEXT &SavedContext)
{
}
*/
BOOL CheckCRC(char * FilePath);

void CClientMain::SetDisplay(void)
{
	HDC hIC;
	hIC = CreateIC("DISPLAY", NULL, NULL, NULL);
	EnumDisplaySettings(NULL, 0, &m_DeviceMode);
	m_DeviceMode.dmPelsWidth=GetDeviceCaps(hIC, HORZRES);
	m_DeviceMode.dmPelsHeight=GetDeviceCaps(hIC, VERTRES);
	m_DeviceMode.dmBitsPerPel=GetDeviceCaps(hIC, BITSPIXEL);		
	m_DeviceMode.dmDisplayFrequency=GetDeviceCaps(hIC,VREFRESH);

	DEVMODE dm				= m_DeviceMode;
	dm.dmPelsWidth			= m_iScreenWidth;
	dm.dmPelsHeight			= m_iScreenHeight;
	dm.dmBitsPerPel			= m_iBpp;
	dm.dmDisplayFrequency	= m_iRefresh;			
	dm.dmFields				= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;	

	long lResult = ChangeDisplaySettings(&dm, 0);
	if (lResult != DISP_CHANGE_SUCCESSFUL)
	{
		// 지원하지 않는 모드입니다.
	}
}

void CClientMain::RestoreDisplay(void)
{
	m_DeviceMode.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
	long lResult = ChangeDisplaySettings(&m_DeviceMode,0);
	if (lResult != DISP_CHANGE_SUCCESSFUL)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[ 1301 ], CRYLStringTable::m_strString[21], MB_OK);
	}
}

void CClientMain::ViewFPS(void)
{
	static FLOAT fLastTime = 0.0f;
	static DWORD dwFrames  = 0L;

	static char strFrameStats[256];
	FLOAT fTime = DXUtil_Timer( TIMER_GETABSOLUTETIME );
	++dwFrames;

	// Update the scene stats once per second
	if ( fTime - fLastTime > 1.0f )
	{
		float fFPS	= dwFrames / (fTime - fLastTime);

		fLastTime 	= fTime;
		dwFrames  	= 0L;

		sprintf( strFrameStats, "%.02f fps", fFPS );		
	}

	m_pRYLGameData->RenderRect( m_BaseGraphicLayer.GetDevice(), 15, 15, 75, 35, 0xFF000000, 0x80 ) ;

	RECT rcRect ;
	SetRect( &rcRect, 21, 21, 77, 32 ) ;
	g_TextEdit.DrawText( strFrameStats, &rcRect, DT_LEFT | DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255 ) ) ;
	SetRect( &rcRect, 20, 20, 76, 31 ) ;
	g_TextEdit.DrawText( strFrameStats, &rcRect, DT_LEFT | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 0, 255 ) ) ;
}

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
#ifndef NO_GAMEGUARD
	switch (dwMsg)
	{
		// GameMon과의 통신 채널이 끊어졌을 경우입니다. 
		// 보통 GameMon이 비정상적으로 종료되었을 경우이므로 게임도 종료해줍니다
		case NPGAMEMON_COMM_ERROR :                  // Communication error
			MessageBox(g_ClientMain.m_hWnd, CRYLStringTable::m_strString[ 1302 ], CRYLStringTable::m_strString[21], MB_OK);
			g_ClientMain.Destroy();
			PostQuitMessage(WM_QUIT);
			return FALSE; // 반드시 종료

		// GameMon이 정상적으로 종료되어 보내는 메시지입니다. 
		// 게임도 종료해줍니다.
		case NPGAMEMON_COMM_CLOSE :                  // Communication closing
			MessageBox(g_ClientMain.m_hWnd, CRYLStringTable::m_strString[ 1303 ], CRYLStringTable::m_strString[21], MB_OK);
			g_ClientMain.Destroy();
			PostQuitMessage(WM_QUIT);
			return FALSE; // 반드시 종료

		// GameMon 실행 에러입니다. 
		// 에러코드인 dwArg 값과 함께 적절한 메시지를 사용자에게 출력해주고 게임을 종료해 줍니다.
		case NPGAMEMON_INIT_ERROR :			         // GameMon Init Error
			{
				char msg[1024];
				sprintf(msg, CRYLStringTable::m_strString[ 1304 ], dwArg);
				MessageBox(g_ClientMain.m_hWnd, msg, CRYLStringTable::m_strString[21], MB_OK);
				g_ClientMain.Destroy();
				PostQuitMessage(WM_QUIT);
			}
			return FALSE; // 반드시 종료

		// 스피드핵이 감지되었습니다. 
		// 적절한 메시지와 함께 게임을 종료해줍니다.
		case NPGAMEMON_SPEEDHACK :                   // SpeedHack detected
			MessageBox(g_ClientMain.m_hWnd, CRYLStringTable::m_strString[911], CRYLStringTable::m_strString[21], MB_OK);
			g_ClientMain.Destroy();
			PostQuitMessage(WM_QUIT);
			return FALSE; // 종료

		// 게임핵이 실행되었지만 성공적으로 강제 종료시켰습니다. 
		// 게임을 계속 진행해도 무방하지만 종료하기를 권장합니다.
		case NPGAMEMON_GAMEHACK_KILLED :             // GameHack killed
			MessageBox(g_ClientMain.m_hWnd, CRYLStringTable::m_strString[ 1305 ], CRYLStringTable::m_strString[21], MB_OK);
			return true; // 계속 진행

		// 게임핵이 발견되었습니다. 
		// 강제 종료가 불가능한 경우이므로 적절한 메시지 출력 후 게임을 종료해줍니다.
		case NPGAMEMON_GAMEHACK_DETECT :             // GameHack detected
			MessageBox(g_ClientMain.m_hWnd, CRYLStringTable::m_strString[ 1306 ], CRYLStringTable::m_strString[21], MB_OK);
			g_ClientMain.Destroy();
			PostQuitMessage(WM_QUIT);
			return FALSE; // 종료

		// 게임핵으로 의심되는 프로그램이 실행 중 입니다. 
		// 혹은 게임이나 게임가드가 변조되었습니다. 
		// 불필요한 프로그램을 종료하고 다시 게임을 해보라는 메시지를 보여주고 게임을 종료해줍니다.
		case NPGAMEMON_GAMEHACK_DOUBT :              // GameHack doubt
			MessageBox(g_ClientMain.m_hWnd, CRYLStringTable::m_strString[ 2847 ], CRYLStringTable::m_strString[21], MB_OK);
			g_ClientMain.Destroy();
			PostQuitMessage(WM_QUIT);
			return FALSE ;

		case NPGAMEMON_CHECK_CSAUTH :                // CSAuth
			// edith 2009.08.11 게임가드 2.5 업그레이드
//			CRYLNetworkData::Instance()->m_GGAuthCode.m_dwAuthCode = dwArg;
//			CRYLNetworkData::Instance()->m_GGAuthCode.SetFlag(GGAuthCode::GG_AUTH_CODE_1);
			return TRUE;

		case NPGAMEMON_CHECK_CSAUTH2 :               // CSAuth2
			// edith 2009.08.11 게임가드 2.5 업그레이드
			::memcpy(&CRYLNetworkData::Instance()->m_GGAuthCode.m_AuthCode2, (PVOID)dwArg, sizeof(GG_AUTH_DATA));
			CRYLNetworkData::Instance()->m_GGAuthCode.SetFlag(GGAuthCode::GG_AUTH_CODE_2);
/*
			{
				char msg[1024];
				sprintf(msg, "NPGameMonCallback : %x, %x, %x, %x", CRYLNetworkData::Instance()->m_GGAuthCode.m_AuthCode2.dwIndex,
					CRYLNetworkData::Instance()->m_GGAuthCode.m_AuthCode2.dwValue1, 
					CRYLNetworkData::Instance()->m_GGAuthCode.m_AuthCode2.dwValue2, 
					CRYLNetworkData::Instance()->m_GGAuthCode.m_AuthCode2.dwValue3	);
				MessageBox(NULL, msg, "GG", MB_OK);
			}
*/
			return TRUE;
	}
#endif
	return TRUE;
}

void	LogDisplay( TCHAR* szBuffer )
{
	char szVideoCardInfo[MAX_PATH] = "";
	HKEY hKey, hSubKey1, hSubKey2;
	unsigned long dwSubKeyNum = 0;
	unsigned long dwSubKeyNum2 = 0;
	TCHAR					buff[2048];
	
	char szParentKeyName[MAX_PATH] = "SYSTEM\\CurrentControlSet\\Control\\Class";
	char szSubKeyName1[MAX_PATH];
	char szSubKeyName2[MAX_PATH];
	char szTempName[MAX_PATH];
	char szValue[MAX_PATH];
	char szDisplayVersion[MAX_PATH] = "";

	unsigned long dwSize = MAX_PATH;
	bool	bIsEnd = false;
	unsigned long nIndex2 = 0;
	D3DAdapterInfo*			pInfo = NULL;
	if ( CEnumD3D::m_dwNumAdapters > 0 )
		pInfo		=  &(CEnumD3D::m_Adapters[0]);


	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szParentKeyName, 0, KEY_READ, &hKey))
	{
		if (ERROR_SUCCESS == RegQueryInfoKey(hKey, NULL, NULL, NULL, &dwSubKeyNum, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
		{
			for (unsigned long nIndex = 0; nIndex < dwSubKeyNum; ++nIndex)
			{
				if ( ERROR_SUCCESS == RegEnumKey(hKey, nIndex, szTempName, MAX_PATH) )
				{

					strcpy(szSubKeyName1, szParentKeyName);
					strcat(szSubKeyName1, "\\");
					strcat(szSubKeyName1, szTempName);
					if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKeyName1, 0, KEY_READ, &hSubKey1))
						continue;

					if (ERROR_SUCCESS != RegQueryValueEx(hSubKey1, "Class", 0, NULL, (LPBYTE)szValue, &dwSize))	
					{
						RegCloseKey( hSubKey1 );
						continue;
					}

					if (0 == stricmp(szValue, "Display"))														
					{
						
						if ( ERROR_SUCCESS != RegQueryInfoKey(hSubKey1, NULL, NULL, NULL, &dwSubKeyNum2, NULL, NULL, NULL, NULL, NULL, NULL, NULL) )
							continue;

						for( nIndex2 = 0; nIndex2 < dwSubKeyNum2 ; nIndex2++ )
						{
							if ( ERROR_SUCCESS == RegEnumKey(hSubKey1, nIndex2, szTempName, MAX_PATH) )
							{

								strcpy(szSubKeyName2, szSubKeyName1 );
								strcat(szSubKeyName2, "\\");
								strcat(szSubKeyName2, szTempName);

								if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKeyName2, 0, KEY_READ, &hSubKey2))
									break;
								if (ERROR_SUCCESS == RegQueryValueEx( hSubKey2, "DriverVersion", 0, NULL, (LPBYTE)szValue, &dwSize))	
								{
									bIsEnd = true;
									strcpy( szDisplayVersion, szValue );
									RegCloseKey( hSubKey2 );
									break;
								}
								RegCloseKey( hSubKey2 );
							}
						}

					}
					RegCloseKey( hSubKey1 );
				}
				if ( bIsEnd )
					break;
			}
		}
		RegCloseKey( hKey );
	}



	if ( pInfo )
	{
		sprintf( buff, " Graphic Card\t\t: [%s]\n Graphic Driver\t\t: [%s]\n", 
					pInfo->d3dAdapterIdentifier.Description, pInfo->d3dAdapterIdentifier.Driver );
		strcat( szBuffer, buff );
		if ( strcmp( szDisplayVersion,"" ) )
		{
			sprintf( buff," Graphic Driver Version\t: [%s]\n", szDisplayVersion );
			strcat( szBuffer, buff );
		}
	}

}



void	LogSoundDrv( TCHAR* szBuffer )
{
	char szVideoCardInfo[MAX_PATH] = "";
	HKEY hKey, hSubKey1, hSubKey2;
	unsigned long dwSubKeyNum = 0;
	unsigned long dwSubKeyNum2 = 0;
	TCHAR					buff[2048];
	
	//char szParentKeyName[MAX_PATH] = "SYSTEM\\CurrentControlSet\\Enum\\PCI";
	char szParentKeyName[MAX_PATH] = "SYSTEM\\CurrentControlSet\\Control\\Class";
	char szSubKeyName1[MAX_PATH];
	char szSubKeyName2[MAX_PATH];
	char szTempName[MAX_PATH];
	char szValue[MAX_PATH];
	char szDisplayVersion[MAX_PATH] = "";

	unsigned long dwSize = MAX_PATH;
	bool	bIsEnd = false;
	unsigned long nIndex2 = 0;
	char szMediaVersion[MAX_PATH] = "";
	char szSoundCard[MAX_PATH] = "";


	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szParentKeyName, 0, KEY_READ, &hKey))
	{
		if (ERROR_SUCCESS == RegQueryInfoKey(hKey, NULL, NULL, NULL, &dwSubKeyNum, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
		{
			for (unsigned long nIndex = 0; nIndex < dwSubKeyNum; ++nIndex)
			{
				if ( ERROR_SUCCESS == RegEnumKey(hKey, nIndex, szTempName, MAX_PATH) )
				{

					strcpy(szSubKeyName1, szParentKeyName);
					strcat(szSubKeyName1, "\\");
					strcat(szSubKeyName1, szTempName);
					if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKeyName1, 0, KEY_READ, &hSubKey1))
						continue;

					if (ERROR_SUCCESS != RegQueryValueEx(hSubKey1, "Class", 0, NULL, (LPBYTE)szValue, &dwSize))	
					{
						RegCloseKey( hSubKey1 );
						continue;
					}

					if (0 == stricmp(szValue, "MEDIA"))														
					{
						
						if ( ERROR_SUCCESS != RegQueryInfoKey(hSubKey1, NULL, NULL, NULL, &dwSubKeyNum2, NULL, NULL, NULL, NULL, NULL, NULL, NULL) )
							continue;

						for( nIndex2 = 0; nIndex2 < dwSubKeyNum2 ; nIndex2++ )
						{
							if ( ERROR_SUCCESS == RegEnumKey(hSubKey1, nIndex2, szTempName, MAX_PATH) )
							{

								strcpy(szSubKeyName2, szSubKeyName1 );
								strcat(szSubKeyName2, "\\");
								strcat(szSubKeyName2, szTempName);

								if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKeyName2, 0, KEY_READ, &hSubKey2))
									break;
								if (ERROR_SUCCESS == RegQueryValueEx( hSubKey2, "Driver", 0, NULL, (LPBYTE)szValue, &dwSize))
								{
									if ( 0 == stricmp( szValue, CEnumD3D::m_strSoundCardDrv ) )
									{
										bIsEnd = true;
										if (ERROR_SUCCESS == RegQueryValueEx( hSubKey2, "DriverDesc", 0, NULL, (LPBYTE)szValue, &dwSize))	
										{
											strcpy( szSoundCard, szValue );
										}
										if (ERROR_SUCCESS == RegQueryValueEx( hSubKey2, "DriverVersion", 0, NULL, (LPBYTE)szValue, &dwSize))	
										{
											strcpy( szMediaVersion, szValue );
										}
										RegCloseKey( hSubKey2 );
										break;
									}
								}
								RegCloseKey( hSubKey2 );
							}
						}

					}
					RegCloseKey( hSubKey1 );
				}
				if ( bIsEnd )
					break;
			}
		}
		RegCloseKey( hKey );
	}

	if ( strcmp( CEnumD3D::m_strSoundCard,"" ) )
	{
		sprintf( buff, " Sound Card Drv \t: [%s]\n", CEnumD3D::m_strSoundCardDrv );
		strcat( szBuffer, buff );
		if ( strcmp( szSoundCard, "") )
		{
			sprintf( buff, " Sound Card\t\t: [%s]\n", szSoundCard );
			strcat( szBuffer, buff );
		}
		else
		{
			sprintf( buff, " Sound Card\t\t: [%s]\n", CEnumD3D::m_strSoundCard );
			strcat( szBuffer, buff );
		}
				
		if ( strcmp( szMediaVersion,"") )
		{
			sprintf( buff, " Sound Version\t\t: [%s]\n", szMediaVersion );
			strcat( szBuffer, buff );
		}
	}

}

void	LogCPU( TCHAR* szBuffer )
{
	HKEY hKey, hSubKey1;
	unsigned long dwSubKeyNum = 0;
	TCHAR					buff[2048];
	
	char szParentKeyName[MAX_PATH] = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor";
	char szSubKeyName1[MAX_PATH];
	char szTempName[MAX_PATH];
	char szValue[MAX_PATH];

	unsigned long dwSize = MAX_PATH;
	char	cpuInfo[4][MAX_PATH];


	int nCnt = 0;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szParentKeyName, 0, KEY_READ, &hKey))
	{
		if (ERROR_SUCCESS == RegQueryInfoKey(hKey, NULL, NULL, NULL, &dwSubKeyNum, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
		{
			for (unsigned long nIndex = 0; nIndex < dwSubKeyNum; ++nIndex)
			{
				if ( ERROR_SUCCESS == RegEnumKey(hKey, nIndex, szTempName, MAX_PATH) )
				{

					strcpy(szSubKeyName1, szParentKeyName);
					strcat(szSubKeyName1, "\\");
					strcat(szSubKeyName1, szTempName);
					if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKeyName1, 0, KEY_READ, &hSubKey1))
						continue;

					if (ERROR_SUCCESS == RegQueryValueEx(hSubKey1, "ProcessorNameString", 0, NULL, (LPBYTE)szValue, &dwSize))	
					{
						strcpy( cpuInfo[nCnt], szValue );
						nCnt++;
						RegCloseKey( hSubKey1 );
						continue;
					}

					RegCloseKey( hSubKey1 );
				}
			}
		}
		RegCloseKey( hKey );
	}

	for( int i = 0; i < nCnt; i++ )
	{
		sprintf( buff, " CPU [%d]\t\t: [%s]\n", i, cpuInfo[i] );
		strcat( szBuffer, buff );
	}
}

void	LogMainBoard( TCHAR* szBuffer )
{

	HRESULT res;
	HRESULT		hres;
	IWbemLocator*	pWmiLoc = NULL;
	IWbemServices*	pWmiServ = NULL;
	TCHAR		buff[256];
	TCHAR		strManufacturer[256];
	TCHAR		strProduct[256];
	TCHAR		strVersion[256];

	hres =  CoInitializeEx(0, COINIT_MULTITHREADED); // Initialize COM.
	if (FAILED(hres))
	{
      // Program has failed.
		return ;
	}
	memset( strManufacturer, 0, sizeof(TCHAR) * 256 );
	memset( strProduct, 0, sizeof(TCHAR) * 256 );
	memset( strVersion, 0, sizeof(TCHAR) * 256 );


    hres = CoCreateInstance	( CLSID_WbemLocator, 0, 
								CLSCTX_INPROC_SERVER, IID_IWbemLocator	,
								(LPVOID *)( &pWmiLoc ));
 
    if (FAILED(hres))	
        return ;

	CComBSTR bstrNamespace("\\root\\CIMV2");

	hres = (pWmiLoc)->ConnectServer(bstrNamespace, NULL, NULL, 0, NULL, 0, 0, &pWmiServ );

	if (FAILED(hres))
	{
		pWmiLoc->Release();
		return ;
	}

	hres = CoSetProxyBlanket( pWmiServ	, 
		RPC_C_AUTHN_WINNT	, 
		RPC_C_AUTHZ_NONE	, 
		NULL, 
		RPC_C_AUTHN_LEVEL_CALL		,
		RPC_C_IMP_LEVEL_IMPERSONATE	, 
		NULL,
		EOAC_NONE);

    if (FAILED(hres))
	{
		pWmiLoc->Release();
		pWmiServ->Release();
		return ;
	}

	IEnumWbemClassObject*	piEnumObj = NULL;
	{
		CComBSTR		bstrObjectName( "Win32_BaseBoard" );
		res = pWmiServ->CreateInstanceEnum(	bstrObjectName	,
			WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY ,
			NULL	,
			&piEnumObj);
		if (FAILED(res))    
			return;
	}

    res = WBEM_S_NO_ERROR;
	BOOL		bInit = FALSE;
	VARIANT		Val;
	CComBSTR	bstrPropName;
    //	final 'Next' will return WBEM_S_FALSE or WBEM_S_NO_MORE_DATA
    while ( WBEM_S_NO_ERROR == res )
    {
        ULONG            uReturned;
        IWbemClassObject*    apObj;

        res = piEnumObj->Next( WBEM_INFINITE, 1, &apObj, &uReturned );
        if ( SUCCEEDED( res ) && 1 == uReturned)
        {
			CComBSTR	bstrDriveObj;
			HRESULT		hr;
			hr = apObj->GetObjectText( 0, &(bstrDriveObj.m_str) );
			
			if (SUCCEEDED( hr ))
			{
				//	fill the properties list
				VariantInit(&Val);
				USES_CONVERSION;
				bstrPropName = "Manufacturer";
				hr = apObj->Get(bstrPropName, 0L, &Val, NULL, NULL);

				if (!FAILED(hr))
				{
					CW2CT szString2( Val.bstrVal );
					strcpy( strManufacturer, szString2 );
				}


				VariantInit(&Val);
				bstrPropName = "Product";
				hr = apObj->Get(bstrPropName, 0L, &Val, NULL, NULL);
				if (!FAILED(hr))
				{
					CW2CT szString3( Val.bstrVal );
					strcpy( strProduct, szString3 );
				}

				VariantInit(&Val);
				bstrPropName = "Version";
				hr = apObj->Get(bstrPropName, 0L, &Val, NULL, NULL);
				if (!FAILED(hr))
				{
					CW2CT szString4( Val.bstrVal );
					strcpy( strVersion, szString4 );
				}


			}
			apObj->Release();
        }    //	If Enum succeeded...
    }    //	While Enum is returning objects...



	if ( piEnumObj )
	{
		piEnumObj->Release();
		piEnumObj = NULL;
	}

	if (pWmiLoc)	
	{
		pWmiLoc->Release();
		pWmiLoc = NULL;
	}
	if (pWmiServ)	
	{
		pWmiServ->Release();
		pWmiServ = NULL;
	}
	
	if ( strlen(strManufacturer) )
	{
		sprintf( buff, " MainBoard Manufacturer\t: [%s]\n", strManufacturer );
		strcat( szBuffer, buff );
	}
	if ( strlen(strProduct) )
	{
		sprintf( buff, " MainBoard Product\t: [%s]\n", strProduct );
		strcat( szBuffer, buff );
	}
	if ( strlen(strVersion) )
	{
		sprintf( buff, " MainBoard Version\t: [%s]\n", strVersion );
		strcat( szBuffer, buff );
	}
	CoUninitialize();
}

void	LogMemory( TCHAR* szBuffer )
{

	HRESULT res;
	HRESULT		hres;
	IWbemLocator*	pWmiLoc = NULL;
	IWbemServices*	pWmiServ = NULL;

	TCHAR		strAvailableMemory[20];
	TCHAR		strMemory[2048];
	TCHAR		strVideo[2048];

	hres =  CoInitializeEx(0, COINIT_MULTITHREADED); // Initialize COM.
	if (FAILED(hres))
	{
      // Program has failed.
		return ;
	}

	memset( strMemory, 0, sizeof(TCHAR) * 2048 );
	memset( strVideo, 0, sizeof(TCHAR) * 2048 );
	memset( strAvailableMemory, 0, sizeof(TCHAR) * 20 );


    hres = CoCreateInstance	( CLSID_WbemLocator, 0, 
								CLSCTX_INPROC_SERVER, IID_IWbemLocator	,
								(LPVOID *)( &pWmiLoc ));
 
    if (FAILED(hres))	
        return ;

	CComBSTR bstrNamespace("\\root\\CIMV2");

	hres = (pWmiLoc)->ConnectServer(bstrNamespace, NULL, NULL, 0, NULL, 0, 0, &pWmiServ );

	if (FAILED(hres))
	{
		pWmiLoc->Release();
		return ;
	}

	hres = CoSetProxyBlanket( pWmiServ	, 
		RPC_C_AUTHN_WINNT	, 
		RPC_C_AUTHZ_NONE	, 
		NULL, 
		RPC_C_AUTHN_LEVEL_CALL		,
		RPC_C_IMP_LEVEL_IMPERSONATE	, 
		NULL,
		EOAC_NONE);

    if (FAILED(hres))
	{
		pWmiLoc->Release();
		pWmiServ->Release();
		return ;
	}

	IEnumWbemClassObject*	piEnumObj = NULL;
	{
		CComBSTR		bstrObjectName( "Win32_LogicalMemoryConfiguration" );
		res = pWmiServ->CreateInstanceEnum(	bstrObjectName	,
			WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY ,
			NULL	,
			&piEnumObj);
		if (FAILED(res))    
			return;
	}

    res = WBEM_S_NO_ERROR;
	BOOL		bInit = FALSE;
	VARIANT		Val;
	CComBSTR	bstrPropName;
    //	final 'Next' will return WBEM_S_FALSE or WBEM_S_NO_MORE_DATA

    {
        ULONG            uReturned;
        IWbemClassObject*    apObj;

        res = piEnumObj->Next( WBEM_INFINITE, 1, &apObj, &uReturned );
        if ( SUCCEEDED( res ) && 1 == uReturned)
        {
			CComBSTR	bstrDriveObj;
			HRESULT		hr;
			hr = apObj->GetObjectText( 0, &(bstrDriveObj.m_str) );
			{
				CW2CT szString( bstrDriveObj.m_str );
				strcpy( strMemory, szString );
				strcat( szBuffer, strMemory );
			}
			apObj->Release();
        }    //	If Enum succeeded...
    }    //	While Enum is returning objects...

	if ( piEnumObj )
	{
		piEnumObj->Release();
		piEnumObj = NULL;
	}



	{
		CComBSTR		bstrObjectName( "Win32_VideoController" );
		res = pWmiServ->CreateInstanceEnum(	bstrObjectName	,
			WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY ,
			NULL	,
			&piEnumObj);
		if (FAILED(res))    
			return;
	}

    res = WBEM_S_NO_ERROR;
	bInit = FALSE;
    //	final 'Next' will return WBEM_S_FALSE or WBEM_S_NO_MORE_DATA
    while( WBEM_S_NO_ERROR == res )
    {
        ULONG            uReturned;
        IWbemClassObject*    apObj;

        res = piEnumObj->Next( WBEM_INFINITE, 1, &apObj, &uReturned );
        if ( SUCCEEDED( res ) && 1 == uReturned)
        {
			CComBSTR	bstrDriveObj;
			HRESULT		hr;
			hr = apObj->GetObjectText( 0, &(bstrDriveObj.m_str) );
			
			if (SUCCEEDED( hr ))
			{
				CW2CT szString( bstrDriveObj.m_str );
				strcpy( strVideo, szString );
				strcat( szBuffer, strVideo );
			}
			apObj->Release();
        }    //	If Enum succeeded...
    }    //	While Enum is returning objects...

	if ( piEnumObj )
	{
		piEnumObj->Release();
		piEnumObj = NULL;
	}



	{
		CComBSTR		bstrObjectName( "Win32_PerfRawData_PerfOS_Memory " );
		res = pWmiServ->CreateInstanceEnum(	bstrObjectName	,
			WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY ,
			NULL	,
			&piEnumObj);
		if (FAILED(res))    
			return;
	}

    res = WBEM_S_NO_ERROR;
	bInit = FALSE;
    //	final 'Next' will return WBEM_S_FALSE or WBEM_S_NO_MORE_DATA
    {
        ULONG            uReturned;
        IWbemClassObject*    apObj;

        res = piEnumObj->Next( WBEM_INFINITE, 1, &apObj, &uReturned );
        if ( SUCCEEDED( res ) && 1 == uReturned)
        {
			CComBSTR	bstrDriveObj;
			HRESULT		hr;
			hr = apObj->GetObjectText( 0, &(bstrDriveObj.m_str) );
			
			if (SUCCEEDED( hr ))
			{
				VariantInit(&Val);
				USES_CONVERSION;
				bstrPropName = "AvailableKBytes";
				hr = apObj->Get(bstrPropName, 0L, &Val, NULL, NULL);

				if (!FAILED(hr))
				{

					CW2CT szString( Val.bstrVal );
					sprintf( strAvailableMemory, "\n Available Memory = [%s KB] \n", szString );
					strcat( szBuffer, strAvailableMemory );
				}

			}
			apObj->Release();
        }    //	If Enum succeeded...
    }    //	While Enum is returning objects...

	if ( piEnumObj )
	{
		piEnumObj->Release();
		piEnumObj = NULL;
	}

	if (pWmiLoc)	
	{
		pWmiLoc->Release();
		pWmiLoc = NULL;
	}
	if (pWmiServ)	
	{
		pWmiServ->Release();
		pWmiServ = NULL;
	}
	
	CoUninitialize();
}