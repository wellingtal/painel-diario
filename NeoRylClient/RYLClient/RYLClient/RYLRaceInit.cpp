
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"
#include "RYLClientMain.h"

#include <Item/ItemMgr.h>
#include <Creature/Character/CharacterClass.h>
#include <Map/FieldMap/MineralVeinMgr.h>

#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include "FullSceneEffect.h"
#include "SceneManager.h"
#include "BgmManager.h"
#include "SoundMgr.h"

#include "HumanStoneStatueRespawn.h"

#include "RYLCharacterControl.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLLoadingScene.h"

#include "RYLCampManager.h"
#include "RYLCastleManager.h"
#include "RYLMessageBoxManager.h"
#include "RYLSkillScriptMgr.h"
#include "RYLStringTable.h"
#include "RYLEdit.h"

#include "RYLMIniMapDlg.h"
#include "RYLLargeMapFrameDlg.h"
#include "RYLHelpFrameDlg.h"
#include "RYLExchangeDlg.h"
#include "RYLHumanInventoryFrameDlg.h"
#include "RYLHumanInventoryPanel.h"
#include "RYLAkhanInventoryPanel.h"
#include "RYLOptionDlg.h"
#include "RYLChattingDlg.h"
#include "RYLStreetStallDlg.h"
#include "RYLTradeDlg.h"
#include "RYLNewTradeDlg.h"
#include "RYLQuestOpeningDlg.h"
#include "RYLBlacksmithDlg.h"
#include "RYLItemCompositionDlg.h"
#include "RYLStatusRetrainDlg.h"
#include "RYLJobChangeDlg.h"
#include "RYLVaultDlg.h"
#include "RYLVaultPasswordDlg.h"



#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h"
#include "GMMemory.h"
#include <WinInput.h>
#include <Utility/CheckSum/Crc32Static.h>

unsigned long  CRYLRaceBase::CheckOCTREE()
{
	unsigned long dwCrc = 0, dwTemp = 0;
    HANDLE              hSrch;
    WIN32_FIND_DATA     wfd;
    BOOL                bResult = TRUE;

	char strScriptPath[MAX_PATH];
	sprintf(strScriptPath, "%s\\OCTREE\\*.OCT", g_ClientMain.m_strClientPath);

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
			sprintf(strScriptPath, "%s\\OCTREE\\%s", g_ClientMain.m_strClientPath, wfd.cFileName);

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
//a method that checks the version of the client and loads all of the gsf and mcf files, and loads everything that the zone needs(music, loading screen and etc..).
BOOL CRYLRaceBase::InitZone(unsigned long dwZoneNumber)
{
	char strScriptPath[MAX_PATH];

	FILE *fp;
	int ver;

	sprintf(strScriptPath, "%s\\VersionInfo.dat", g_ClientMain.m_strClientPath);
	fp = fopen(strScriptPath, "r");
	fscanf(fp, "%d", &ver);
	fclose(fp);
	g_ClientMain.m_dwClientVersion = ver;

	if (!g_ClientMain.m_pRYLGameData->m_bHardwareCursor)
	{
		g_DeviceInput.ShowCursor(FALSE);
	}

	unsigned long dwCrc[20], dwTemp = 0;
	ZeroMemory(dwCrc, sizeof(unsigned long)*20);

	sprintf(strScriptPath, "%s\\Client.exe", g_ClientMain.m_strClientPath);
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
	TCHAR szLanguage[1][32] = { _T("ENG")}; //, _T("CHS"), _T("JPN"), _T("KOR"), _T("PRT"), _T("SPN"), _T("POL") };

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// MUI
	for(int i = 0; i < iMaxLanguage; ++i)
	{
		sprintf(strScriptPath, "%s\\Language\\%s\\NPCScript.mcf", g_ClientMain.m_strClientPath, szLanguage[i]);
		if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
		{
			MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
			return FALSE;
		}
		dwCrc[1] = dwCrc[1] ^ dwTemp;

		// 배틀 로한은 별개의 아이템 스크립트를 사용합니다.
		if (GameRYL::SERVER_BATTLE_LOHAN == g_ClientMain.m_pRYLNetworkData->m_eServerType)
		{
			sprintf(strScriptPath, "%s\\Language\\%s\\BGItemScript.gsf", g_ClientMain.m_strClientPath, szLanguage[i]);
		}
		else
		{
			sprintf(strScriptPath, "%s\\Language\\%s\\ItemScript.gsf", g_ClientMain.m_strClientPath, szLanguage[i]);
		}
		if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
		{
			MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
			return FALSE;
		}
		dwCrc[2] = dwCrc[2] ^ dwTemp;

		sprintf(strScriptPath, "%s\\Language\\%s\\SkillScript.gsf", g_ClientMain.m_strClientPath, szLanguage[i]);
		if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
		{
			MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
			return FALSE;
		}
		dwCrc[3] = dwCrc[3] ^ dwTemp;

		sprintf(strScriptPath, "%s\\Language\\%s\\MonsterProtoType.gsf", g_ClientMain.m_strClientPath, szLanguage[i]);
		if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
		{
			MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
			return FALSE;
		}
		dwCrc[4] = dwCrc[4] ^ dwTemp;

		sprintf(strScriptPath, "%s\\Language\\%s\\Quest.mcf", g_ClientMain.m_strClientPath, szLanguage[i]);
		if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
		{
			MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
			return FALSE;
		}
		dwCrc[5] = dwCrc[5] ^ dwTemp;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////

#else

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// MUI
	sprintf(strScriptPath, "%s\\NPCScript.mcf", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[1]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	// 배틀 로한은 별개의 아이템 스크립트를 사용합니다.
	if (GameRYL::SERVER_BATTLE_LOHAN == g_ClientMain.m_pRYLNetworkData->m_eServerType)
	{
		sprintf(strScriptPath, "%s\\BGItemScript.gsf", g_ClientMain.m_strClientPath);
	}
	else
	{
		sprintf(strScriptPath, "%s\\ItemScript.gsf", g_ClientMain.m_strClientPath);
	}
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[2]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\SkillScript.gsf", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[3]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\MonsterProtoType.gsf", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[4]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\Quest.mcf", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[5]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

	sprintf(strScriptPath, "%s\\Script1.gsf", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[6]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}

	sprintf(strScriptPath, "%s\\Character\\Data\\ChrEvent.eds", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[7]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\Chemical.gsf", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[8]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	sprintf(strScriptPath, "%s\\SpeacialCompensation.gsf", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[9]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}

	// 존
	sprintf(strScriptPath, "%s\\zone1.z3s", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10] ^ dwTemp;

	sprintf(strScriptPath, "%s\\zone2.z3s", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10] ^ dwTemp;

	sprintf(strScriptPath, "%s\\zone4.z3s", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10] ^ dwTemp;

	sprintf(strScriptPath, "%s\\zone5.z3s", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10] ^ dwTemp;

	sprintf(strScriptPath, "%s\\zone8.z3s", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10] ^ dwTemp;

	sprintf(strScriptPath, "%s\\zone12.z3s", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10] ^ dwTemp;

	sprintf(strScriptPath, "%s\\zone16.z3s", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwTemp) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
	dwCrc[10] = dwCrc[10] ^ dwTemp;
/*
	sprintf(strScriptPath, "%s\\GameGuard.des", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[11]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
*/
	sprintf(strScriptPath, "%s\\MineralVein8.gsf", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[11]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}

/*
	sprintf(strScriptPath, "%s\\MineralVein16.gsf", g_ClientMain.m_strClientPath);
	if (CCrc32Static::FileCrc32Assembly(strScriptPath, dwCrc[12]) != NO_ERROR)
	{
		MessageBox(NULL, CRYLStringTable::m_strString[22], CRYLStringTable::m_strString[21], MB_OK);
		return FALSE;
	}
*/

//#ifndef NDEBUG
//	switch (g_ClientMain.m_pRYLNetworkData->m_eServerType)
//	{
//	case GameRYL::SERVER_TEST:			g_ClientMain.m_dwVersion = 0x2e07d9fa;	break;	// 테섭
//	case GameRYL::SERVER_REGULAR:		g_ClientMain.m_dwVersion = 0xb332c5fb;	break;	// 정섭
//	case GameRYL::SERVER_BATTLE_LOHAN:	g_ClientMain.m_dwVersion = 0x00000001;	break;	// 배섭 
//	}
//#endif

#ifndef _RYL_TEST
	const unsigned long dwVersion = g_ClientMain.m_dwVersion;

	g_ClientMain.m_dwVersion = dwCrc[0];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[1];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[2];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[3];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[4];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[5];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[6];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[7];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[8];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[9];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[10];
	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[11];
//	g_ClientMain.m_dwVersion = g_ClientMain.m_dwVersion ^ dwCrc[12];

	if (dwVersion != g_ClientMain.m_dwVersion)
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[298]);
		CRYLGameData::SetClientExitCounter( 1000 ) ;
		CRYLGameData::SetLogoutCounter( 0 ) ;
		CRYLGameData::Instance()->m_lLogoutTimeResult = 0 ;
	}
#endif

	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	pGame->m_dwClientState = GAME_LOADING;

	pGame->m_lpSceneManager->GetCamera()->EndPlay();	

#ifdef __NEWNPCSCRIPT
	char strPath[MAX_PATH], strCompoPath[MAX_PATH], szAmbienceFile[MAX_PATH], szBGMFile[MAX_PATH], Temp[MAX_PATH];
#else
	char strPath[MAX_PATH], szAmbienceFile[MAX_PATH], szBGMFile[MAX_PATH], Temp[MAX_PATH];
#endif

	pGame->m_lpSceneManager->m_ViewerMode = 1;


#ifdef _RYL_LANGUAGE
	int iLanguage = g_ClientMain.m_iLanguage;
	if(iLanguage >= iMaxLanguage)
		iLanguage = 0;

	sprintf(strPath, "%s\\Language\\%s\\NPCScript.mcf", g_ClientMain.m_strClientPath, szLanguage[iLanguage]);

#else
	sprintf(strPath, "%s\\NPCScript.mcf", g_ClientMain.m_strClientPath);
#endif

#ifdef __NEWNPCSCRIPT
	sprintf(strCompoPath, "%s\\Component.mcf", g_ClientMain.m_strClientPath);

	g_NPCList.Load(strPath, strCompoPath, dwZoneNumber);
#else
	g_NPCList.Load(strPath, dwZoneNumber);
#endif

	float fDayTime = (dwZoneNumber == 100) ? 17.0f : 13.0f;

	pGame->m_lpSceneManager->SetWeatherTime(fDayTime);
	CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_MOTIONBLUR, false ) ;
	CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_MOTIONBLUR, false ) ;
	CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_SATURATION2, false ) ;
	CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_SATURATION2, false ) ;
	CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_NEGATIVE, false ) ;
	CSceneManager::m_pFullSceneEffect->SetFlag( FULLSCENE_NEGATIVE, false ) ;

	switch (dwZoneNumber)
	{
/*		case SERVER_ID::ZONE1:
			sprintf(szAmbienceFile, "%s\\Sound\\am\\Ambience.snm", g_ClientMain.m_strClientPath);
			sprintf( szBGMFile, "%s\\Sound\\am\\Human.trg", g_ClientMain.m_strClientPath );
			sprintf(Temp, "%s\\sound\\am\\hudas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone1.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone1");
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(47313.0f,0.0f,18492.0f),vector3(338671.0f,0.0f,306341.0f));
			pGame->ResetTerrain();
			break;*/

			case SERVER_ID::ZONE1:
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			sprintf(Temp, "%s\\sound\\am\\hsnjdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone1.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone1");			
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(32500.0f,0.0f,32500.0f),vector3(376940.0f,0.0f,377423.8f));
			pGame->ResetTerrain();
			break;

		case SERVER_ID::ZONE2:
			sprintf(szAmbienceFile, "%s\\Sound\\am\\Akhan.snm", g_ClientMain.m_strClientPath);
			sprintf( szBGMFile, "%s\\Sound\\am\\Akhan.trg", g_ClientMain.m_strClientPath );
			sprintf(Temp, "%s\\sound\\am\\akdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone2.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone2");			
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(31507.0f,0.0f,31507.0f),vector3(378084.0f,0.0f,378084.0f));
			pGame->ResetTerrain();
			break;

		case SERVER_ID::ZONE3:
			sprintf(szAmbienceFile, "%s\\Sound\\am\\bg.snm", g_ClientMain.m_strClientPath);
			sprintf( szBGMFile, "%s\\Sound\\am\\bg.trg", g_ClientMain.m_strClientPath );
			sprintf(Temp, "%s\\sound\\am\\bgdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone3.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone3");
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(59181.0f,0.0f,57580.0f),vector3(222636.0f,0.0f,224901.0f));
			pGame->ResetTerrain();
			break;

		case SERVER_ID::ZONE4:
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			sprintf(Temp, "%s\\sound\\am\\hsnjdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone4.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone4");
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(48430.0f,0.0f,45270.0f),vector3(270453.0f,0.0f,270536.0f));
			pGame->ResetTerrain();
			break;

		case SERVER_ID::ZONE5:
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			sprintf(Temp, "%s\\sound\\am\\aapjdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone5.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone5");
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(45000.0f,0.0f,45000.0f),vector3(332583.0f,0.0f,334642.0f));
			pGame->ResetTerrain();
			break;

		case SERVER_ID::BATTLE_ZONE:
			sprintf(szAmbienceFile, "%s\\Sound\\am\\bg.snm", g_ClientMain.m_strClientPath);
			sprintf( szBGMFile, "%s\\Sound\\am\\bg.trg", g_ClientMain.m_strClientPath );
			sprintf(Temp, "%s\\sound\\am\\bgdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone3.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone3");
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(59181.0f,0.0f,57580.0f),vector3(222636.0f,0.0f,224901.0f));
			pGame->ResetTerrain();
			break;

		case SERVER_ID::CAPITAL:
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			sprintf(Temp, "%s\\sound\\am\\hsnjdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone8.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone8");			
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(36986.6f,0.0f,40274.8f),vector3(374361.1f,0.0f,373129.9f));
			pGame->ResetTerrain();

			// 로드.
			sprintf(strPath, "%s\\MineralVein8.gsf", g_ClientMain.m_strClientPath);
			g_MinveralVeinMgr.LoadMineralVeinsFromBinary( strPath );
			break;

		case SERVER_ID::ZONE9:
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			sprintf(Temp, "%s\\sound\\am\\aapjdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone9.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone9");
			pGame->m_lpSceneManager->LoadMap(1, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(63000.0f,0.0f,63000.0f),vector3(378000.0f,0.0f,378000.0f));
			pGame->ResetTerrain();
			break;

		case SERVER_ID::FRAG_ZONE:
		{
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			strcpy(Temp, "");
			sprintf(strPath, "%s\\Zone10.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone10");
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(31711.0f,0.0f,31783.0f),vector3(188999.0f,0.0f,188999.0f));
			pGame->ResetTerrain();

			char    Temp[128] = { 0, };

			CStreamingSound* pPrevSound; 
			sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - Waiting room.ogg", g_ClientMain.m_strClientPath );
			CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, Temp ); 
			pPrevSound->Stop();

			CStreamingSound* pCurrentSound; 
			sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - Death match.ogg", g_ClientMain.m_strClientPath );
			CSoundMgr::_GetInstance()->GetStreamingSound( pCurrentSound, Temp ); 
			pCurrentSound->SetLooping( true );
			pCurrentSound->Play();
			break;
		}

		// Battle Ground Wait Zone
		case SERVER_ID::BATTLE_SERVER:
		{
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			strcpy(Temp, "");
			sprintf(strPath, "%s\\Zone11.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone11");
		//	pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(47313.0f,0.0f,18492.0f),vector3(338671.0f,0.0f,306341.0f));
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3( 157471.0f, 0.0f, 95403.0f ), vector3( 225030.0f, 0.0f, 188457.0f ) );
			pGame->ResetTerrain();

			char    Temp[128] = { 0, };

			CStreamingSound* pPrevSound; 
			sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - 5 minutes remain.ogg", g_ClientMain.m_strClientPath );
			CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, Temp ); 
			pPrevSound->Stop();
			sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - Conquer.ogg", g_ClientMain.m_strClientPath );
			CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, Temp ); 
			pPrevSound->Stop();
			sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - Death match.ogg", g_ClientMain.m_strClientPath );
			CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, Temp ); 
			pPrevSound->Stop();

			CStreamingSound* pCurrentSound; 
			sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - Waiting room.ogg", g_ClientMain.m_strClientPath );
			CSoundMgr::_GetInstance()->GetStreamingSound( pCurrentSound, Temp ); 
			pCurrentSound->SetLooping( true );
			pCurrentSound->Play();
			break;
		}

		case SERVER_ID::ZONE12:
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			sprintf(Temp, "%s\\sound\\am\\hsnjdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone12.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone12");			
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(32500.0f,0.0f,32500.0f),vector3(376940.0f,0.0f,377423.8f));
			pGame->ResetTerrain();
//			sprintf(strPath, "%s\\MineralVein12.gsf", g_ClientMain.m_strClientPath);
//			g_MinveralVeinMgr.LoadMineralVeinsFromBinary( strPath );
			break;

		case SERVER_ID::STATUE_ZONE:
		{
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			strcpy(Temp, "");
			sprintf(strPath, "%s\\Zone13.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone13");
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(31711.0f,0.0f,31783.0f),vector3(188999.0f,0.0f,188999.0f));
			pGame->ResetTerrain();

			char    Temp[128] = { 0, };

			CStreamingSound* pPrevSound; 
			sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - Waiting room.ogg", g_ClientMain.m_strClientPath );
			CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, Temp ); 
			pPrevSound->Stop();

			CStreamingSound* pCurrentSound; 
			sprintf( Temp, "%s\\sound\\am\\BG Client\\BG Client - Conquer.ogg", g_ClientMain.m_strClientPath );
			CSoundMgr::_GetInstance()->GetStreamingSound( pCurrentSound, Temp ); 
			pCurrentSound->SetLooping( true );
			pCurrentSound->Play();
			break;
		}

		case SERVER_ID::PRISON:
        {
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			strcpy(Temp, "");
			sprintf(strPath, "%s\\Zone100.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone100");			
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(0,0,0), vector3(0,0,0));
			pGame->ResetTerrain();
        }
		break;

		// 아레나
		case 100:
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			strcpy(Temp, "");
			sprintf(strPath, "%s\\Zone100.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone100");
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(0,0,0), vector3(0,0,0));
			pGame->ResetTerrain();
			break;

		case SERVER_ID::CHAR_SELECT:
			sprintf(strPath, "%s\\Zone15.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone15");
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(47313.0f,0.0f,18492.0f),vector3(338671.0f,0.0f,306341.0f));
			pGame->ResetTerrain();
			break;

		case SERVER_ID::STONE_WAR1:
		case SERVER_ID::STONE_WAR2:
		case SERVER_ID::STONE_WAR3:
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			sprintf(Temp, "%s\\sound\\am\\hsnjdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone16.z3s", g_ClientMain.m_strClientPath);

			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone16");
			pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(88000.0f,0.0f,89000.0f),vector3(197000.0f,0.0f,198000.0f));
			pGame->ResetTerrain();

//			sprintf(strPath, "%s\\MineralVein16.gsf", g_ClientMain.m_strClientPath);
//			g_MinveralVeinMgr.LoadMineralVeinsFromBinary( strPath );

			sprintf(Temp, "%s\\sound\\Bgm\\bground.ogg", g_ClientMain.m_strClientPath);
			CBgmManager::_GetInstance()->PlayBGM(Temp, EK_BGM_LOOP_UNSAFE, TRUE);
/*
			CStreamingSound* pCurrentSound; 
			sprintf(Temp, "%s\\sound\\Bgm\\bground.ogg", g_ClientMain.m_strClientPath);
			CSoundMgr::_GetInstance()->GetStreamingSound( pCurrentSound, Temp ); 
			pCurrentSound->SetLooping( true );
			pCurrentSound->Play();
*/
			break;

		default:
			return FALSE;
	}

	RYLCreatureManager::Instance()->DeleteAllArmsCreature();

	CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLMIniMapDlg*		 pDlg	= ( CRYLMIniMapDlg* )pScene->GetMiniMapDlg();

	pDlg->MiniMapDelete();
	pDlg->ReloadTexture();

	pScene->GetStatusDlg()->SetVisible(TRUE);

	pGame->m_lpSceneManager->SetWeatherTimeAdder(0.000f);
	pGame->m_lpSceneManager->HeightPosInit();

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return false;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
	if ( !pSelfData )
		return false;

	if ( pGame->m_cReLogin )
	{
		vector3 vecSelf = *( pSelfCreature->GetPosition() );
		vecSelf.y = 0.0f;
		vector3 vecLength = vecSelf - g_CharacterData.m_vecPrePosition;
		float fLength = vecLength.GetLens();
		{
			g_CharacterData.m_vecPrePosition = *( pSelfCreature->GetPosition() );
			g_CharacterData.m_vecPrePosition.y = 0.0f;
			float fMinX = pSelfCreature->GetPosition()->x - 5000.0f;
			float fMaxX = pSelfCreature->GetPosition()->x + 5000.0f;
			float fMinZ = pSelfCreature->GetPosition()->z - 5000.0f;
			float fMaxZ = pSelfCreature->GetPosition()->z + 5000.0f;

			NPCNode *pNPCNode;
			vector3 vecPos;
			for (unsigned long i = 0; i < g_NPCList.GetNpcNum(); i++)
			{
				pNPCNode = g_NPCList.GetNPCNodeByOrder(i);
				vecPos.x = pNPCNode->m_fPosX * 100.0f;
				vecPos.y = pNPCNode->m_fPosY * 100.0f;
				vecPos.z = pNPCNode->m_fPosZ * 100.0f;
				if (fMinX <= vecPos.x && vecPos.x <= fMaxX && fMinZ <= vecPos.z && vecPos.z <= fMaxZ)
				{	
					// NPC 추가
					if (strcmp(pNPCNode->m_strNpcSkin, "None"))
					{
#ifdef __NEWNPCSCRIPT
						RYLCreatureManager::Instance()->AddCreature( pNPCNode->m_dwUID, pNPCNode->m_strNpcName, pNPCNode->m_strNpcSkin, vecPos, pNPCNode->m_fDirection, false, pNPCNode->m_unNationality);
#else
						RYLCreatureManager::Instance()->AddCreature( pNPCNode->m_dwUID, pNPCNode->m_strNpcName, pNPCNode->m_strNpcSkin, vecPos, pNPCNode->m_fDirection, false, pNPCNode->m_unNationality);
#endif
					}
				}
			}
		}
	}
	else
	{
		char strScriptPath[ MAX_PATH ] ;

#ifdef _RYL_LANGUAGE
		int iLanguage = g_ClientMain.m_iLanguage;
		if(iLanguage >= iMaxLanguage)
			iLanguage = 0;

		sprintf( strScriptPath, "%s\\Language\\%s\\SkillScriptEx.gsf", g_ClientMain.m_strClientPath, szLanguage[iLanguage]);
#else
		sprintf( strScriptPath, "%s\\SkillScriptEx.gsf", g_ClientMain.m_strClientPath);
#endif
		g_pSkillScriptMgr()->SkillScriptLoad( strScriptPath ) ;
	}

	return TRUE;
}

BOOL CRYLRaceBase::InitChrSelect(void)//hayzohar: char select screen?
{
	char			strPath[MAX_PATH] ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	pGame->m_lpSceneManager->m_ViewerMode = 2;

	// original code char select
	/*

	sprintf(strPath, "%s\\Zone15.z3s", g_ClientMain.m_strClientPath);
	strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone15");
	pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(47313.0f,0.0f,18492.0f),vector3(338671.0f,0.0f,306341.0f));

	pGame->m_lpSceneManager->GetCamera()->SetTotalAniTime(3000.0f);
	pGame->m_lpSceneManager->GetCamera()->Unload();

	pGame->m_lpSceneManager->m_pCameraScript->SetPlay( false ) ;
	pGame->m_lpSceneManager->GetCamera()->LookAt( vector3(138646.98f, 1200.16f, 144921.97f),
		vector3(138647.324161f, 1200.136657f, 144922.908621f), vector3(0, 1, 0));

	pGame->m_lpSceneManager->SetWeatherTime(18.0f);

	char Sex[MAX_PATH], Hair[MAX_PATH], Face[MAX_PATH];
	const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];

	pGame->m_vecCharSelectPos[0] = vector3(138934.0f, 994.692f, 145607.0f);
	pGame->m_vecCharSelectPos[1] = vector3(138700.0f, 1047.093f, 145721.0f);
	pGame->m_vecCharSelectPos[2] = vector3(139311.0f, 1005.772f, 145758.0f);
	pGame->m_vecCharSelectPos[3] = vector3(138522.0f, 1120.52f, 146005.0f);
	pGame->m_vecCharSelectPos[4] = vector3(139635.0f, 1008.494f, 145887.0f);

	pGame->m_fCharSelectDir[0] = 0.47f;
	pGame->m_fCharSelectDir[1] = 0.93f;
	pGame->m_fCharSelectDir[2] = 0.0f;
	pGame->m_fCharSelectDir[3] = 1.63f;
	pGame->m_fCharSelectDir[4] = -0.74f;
	*/

	sprintf(strPath, "%s\\Zone11.z3s", g_ClientMain.m_strClientPath);
	strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone11");
	pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(47313.0f,0.0f,18492.0f),vector3(338671.0f,0.0f,306341.0f));
//	pGame->m_lpSceneManager->LoadMap(0, strPath, CRenderOption::m_strBaseGraphicsDataPath, vector3(0.0f,0.0f,0.0f),vector3(0.0f,0.0f,0.0f));

	pGame->m_lpSceneManager->GetCamera()->SetTotalAniTime(3000.0f);
	pGame->m_lpSceneManager->GetCamera()->Unload();
	 // sus/jos character codul din mijloc
	pGame->m_lpSceneManager->m_pCameraScript->SetPlay( false ) ;

	//pGame->m_lpSceneManager->GetCamera()->LookAt( vector3(103092.98f, 10300.16f, 194397.97f), // 1032  1945
	  pGame->m_lpSceneManager->GetCamera()->LookAt( vector3(166700.98f, 113400.16f, 184050.97f), // 1032  1945

    //inainte/inapoi  sus/jos  stanga/dreapta
	//vector3(143593.32f, 10300.16f, 191597.90f), vector3(0, 1, 0)); // 1430   1900
	vector3(155092.812031f, 113400.137219f, 184097.456050f), vector3(0, 1, 0)); // 1430   1900

	pGame->m_lpSceneManager->SetWeatherTime(18.0f); // was 18.0f

	char Sex[MAX_PATH], Hair[MAX_PATH], Face[MAX_PATH];
	const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];

//	pGame->m_vecCharSelectPos[0] = vector3(166090.80f, 113100.000f, 183600.41f);
//	pGame->m_vecCharSelectPos[1] = vector3(166090.64f, 113100.000f, 183800.26f);
//	pGame->m_vecCharSelectPos[2] = vector3(166090.00f, 113100.000f, 184000.22f);
//	pGame->m_vecCharSelectPos[3] = vector3(166090.00f, 113100.000f, 184200.12f);
//	pGame->m_vecCharSelectPos[4] = vector3(166090.00f, 113100.000f, 184400.01f);

	pGame->m_vecCharSelectPos[0] = vector3(166180.00f, 113200.000f, 183770.41f);
	pGame->m_vecCharSelectPos[1] = vector3(166180.00f, 113200.000f, 183910.26f);
	pGame->m_vecCharSelectPos[2] = vector3(166180.00f, 113200.000f, 184050.22f);
	pGame->m_vecCharSelectPos[3] = vector3(166180.00f, 113200.000f, 184190.12f);
	pGame->m_vecCharSelectPos[4] = vector3(166180.00f, 113200.000f, 184330.01f);
	// rotatie character 
	pGame->m_fCharSelectDir[0] = 4.30f; // was 0.47f;
	pGame->m_fCharSelectDir[1] = 4.50f; // was 0.93f;
	pGame->m_fCharSelectDir[2] = 4.70f; // was 0.0f;
	pGame->m_fCharSelectDir[3] = 4.90f; // was 1.63f;
	pGame->m_fCharSelectDir[4] = 5.10f; // was -0.74f;

//	m_ucEnableRide = 0;
	for (int i = 0; i < MAX_CHAR_SLOT; i++) // Minotaurs 캐릭 처음 불러올
	{
		if (pGame->m_scLoginChar[i].CID)
		{
			switch (pGame->m_scLoginChar[i].Race)
			{
				case CClass::RaceType::HUMAN :
				{
					if (pGame->m_scLoginChar[i].Sex == 1) 
						strcpy(Sex, "PC_MAN.GCMDS"); 
					else 
						strcpy(Sex, "PC_WOMAN.GCMDS");
				}
				break;

				case CClass::RaceType::AKHAN :
				{
					if (pGame->m_scLoginChar[i].Sex == 1) 
						strcpy(Sex, "PC_Akhan_A.GCMDS"); 
					else 
						strcpy(Sex, "PC_Akhan_B.GCMDS");
				}
				break;
			}

			sprintf(Hair, "HAIR%02d", pGame->m_scLoginChar[i].Hair);
			sprintf(Face, "FACE%02d", pGame->m_scLoginChar[i].Face);

			for (int j = 0; j < Item::EquipmentPos::MAX_EQUPMENT_POS; j++)
			{
				strShape[j] = g_ItemMgr.GetEquipName(pGame->m_scLoginChar[i].Equip[j]);//loads all of the characters equipment
			}

			
			// 장비 그레이드별 이펙트는 그레이드 정보가 없으므로 일단 무시합니다. 
			// (추후 서버에서 캐릭터를 로드해 이펙트값을 계산 후 클라이언트에게 보내주도록 수정할 필요가 있습니다.)
			RYLCreatureManager::Instance()->AddCreature( pGame->m_scLoginChar[i].CID, 
				pGame->m_scLoginChar[i].Name, 
				Sex, Face, Hair, strShape, Broadcast2nd::CNetworkEquipGrade(), 
				pGame->m_scLoginChar[i].Race, pGame->m_vecCharSelectPos[i], pGame->m_fCharSelectDir[i], false );//a method that adds a new character to the database

			RYLCreature* pCreature = RYLCreatureManager::Instance()->GetCreature(pGame->m_scLoginChar[i].CID);

			if (pCreature)//if the character exist:
			{
				CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData( pCreature->GetCharID() );
				if ( pData )
				{
					pData->m_cRace = pGame->m_scLoginChar[i].Race;

					// WORK_LIST 2.3 계정 국적 변경 기능 구현
					if ( pGame->m_scLoginChar[i].GID )
					{
						pData->m_cNation = pGame->m_scLoginCharGuildData[i].m_cNation;
					}
					else
					{
						pData->m_cNation = pGame->m_cAccountNation;
					}
				}

				pCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), TRUE );
				pCreature->InitWeapon(0);
			}
			
		} 
		else//if the character does not exist(creating a new character):
		{
			std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

			RYLCreatureManager::Instance()->AddCreature( 
				0xC0000000 + i,  "", "NPC_ChrSelect.GCMDS", "FACE01", "HAIR01", 
				strShape, Broadcast2nd::CNetworkEquipGrade(), 
				CClass::RaceType::HUMAN, pGame->m_vecCharSelectPos[i], pGame->m_fCharSelectDir[i], false );

			RYLCreature* pCreature = RYLCreatureManager::Instance()->GetCreature( 0xC0000000 + i );
			if (pCreature)
			{
				CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData( pCreature->GetCharID() );
				if ( !pData )
					return false;

				pData->m_cRace = pGame->m_scLoginChar[i].Race;
				switch (pData->m_cRace)
				{
					case CClass::RaceType::HUMAN:	pData->m_cNation = Creature::KARTERANT;	break;
					case CClass::RaceType::AKHAN:	pData->m_cNation = Creature::MERKADIA;	break;
					default:						pData->m_cNation = Creature::STATELESS;	break;
				}

				pCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), TRUE );
				pCreature->InitWeapon(0);
			}
		}
		
	}

	if ( pGame->m_cReLogin )
	{
		CRYLLoadingScene::Instance()->InitResourceObject( 0 ) ; 
	}
	else
	{
		CRYLLoadingScene::Instance()->InitResourceObject() ; 
	}

	pGame->m_dwClientMode = CLIENT_CHARACTERSELECTLOADING;
	pGame->m_dwClientState = CHRSELECT_INITSELECT;
	pGame->m_dwSelectChr = 0xFFFFFFFF;

	if(g_CharacterData.GetWeaponControlKind()!=CHARACTER_CONTROL)
	{
		g_CharacterData.m_Camera.m_fBattleInterCharacterCamera = 300.0f;
		g_CharacterData.m_Camera.m_fMouseClickInterCharacterCamera=1000.0f;
		g_CharacterData.m_Camera.m_nCameraMode=1;			
		g_CharacterData.SetWeaponControlKind( CHARACTER_CONTROL ) ;
	}	

	CTexture::SetPath(INTERFACETEXTUREPATH);
	return TRUE;
}
//a method that handle the character load to the game[load the look,information,last status(dead or alive,position) and etc]
BOOL CRYLRaceBase::InitGame(void) 
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ; // Minotaurs 게임 초기화 (game point of view?)
	DestroyAllObject();
	RYLCreatureManager::Instance()->ClearCreatureManager();

	if (!strcmp(pGame->m_csStatus.m_Info.Name, ""))//if the character does not have name, it creates a default character(does not work?).
	{
		pGame->m_vecStartPosition = vector3(268742.6270f, 2519.8332f, 53045.1599f);

		strcpy(pGame->m_csStatus.m_Info.Name, "Belldandy");
		pGame->m_csStatus.m_Info.Class = 1;
		pGame->m_csStatus.m_Info.Sex = 1;
		pGame->m_csStatus.m_Info.Hair = 2;
		pGame->m_csStatus.m_Info.Face = 1;
		pGame->m_csStatus.m_Info.Level = 1;
		pGame->m_csStatus.m_Info.Race = pGame->m_dwClientRace;
		pGame->m_csStatus.m_Info.STR = 25;
		pGame->m_csStatus.m_Info.DEX = 20;
		pGame->m_csStatus.m_Info.CON = 20;
		pGame->m_csStatus.m_Info.INT = 20;
		pGame->m_csStatus.m_Info.WIS = 20;
		pGame->m_csStatus.m_Info.Exp = 0;
		pGame->m_csStatus.m_llExpMax = 1100;
		pGame->m_csStatus.m_Info.IP = 1;
		pGame->m_csStatus.m_Info.HP = 100;
		pGame->m_csStatus.m_Info.MP = 100;

		pGame->m_csStatus.m_Info.Mileage = 0;
		pGame->m_csStatus.m_Info.Fame = 0;

		pGame->m_csStatus.m_Skill.wSlotNum = 10;
		pGame->m_csStatus.m_Skill.wSkillNum = 100;

		for (int i = 0; i < SKILL::MAX_SLOT_NUM; i++)
		{
			if ( pGame->m_csStatus.m_lstSkillSlot[i] )
			{
				delete pGame->m_csStatus.m_lstSkillSlot[i] ;
			}

			pGame->m_csStatus.m_lstSkillSlot[i] = NULL;
		}

		pGame->m_csStatus.m_PartyInfo.m_cMemberNum = 2;
		pGame->m_csStatus.m_PartyInfo.m_dwLeaderID = CRYLNetworkData::Instance()->m_dwMyChrID;

		pGame->m_csStatus.m_PartyInfo.MemberCID[0] = CRYLNetworkData::Instance()->m_dwMyChrID;
		strcpy(pGame->m_csStatus.m_PartyInfo.Name[0], pGame->m_csStatus.m_Info.Name);

		pGame->m_csStatus.m_PartyInfo.MemberCID[1] = 100;
		strcpy(pGame->m_csStatus.m_PartyInfo.Name[1], "바보");

		pGame->m_csStatus.m_Skill.SSlot[0].SKILLINFO.wSkill = 0x8102;//sword mastery
		pGame->m_csStatus.m_Skill.SSlot[0].SKILLINFO.cLockCount = 3;
		pGame->m_csStatus.m_Skill.SSlot[0].SKILLINFO.cSkillLevel = 1;

		pGame->m_csStatus.m_lstSkillSlot[0] = new CItemInstance;
		pGame->m_csStatus.m_lstSkillSlot[0]->SetSkillInfo(pGame->m_csStatus.m_Skill.SSlot[0].SKILLINFO.wSkill,
			pGame->m_csStatus.m_Skill.SSlot[0].SKILLINFO.cLockCount, pGame->m_csStatus.m_Skill.SSlot[0].SKILLINFO.cSkillLevel,
			TakeType::TS_SSLOT, 0, 0);

		pGame->m_csStatus.m_Skill.SSlot[1].SKILLINFO.wSkill = 0x8202;// improved critical
		pGame->m_csStatus.m_Skill.SSlot[1].SKILLINFO.cLockCount = 1;
		pGame->m_csStatus.m_Skill.SSlot[1].SKILLINFO.cSkillLevel = 6;

		pGame->m_csStatus.m_Skill.SSlot[2].SKILLINFO.wSkill = 0x8103;//axe mastery
		pGame->m_csStatus.m_Skill.SSlot[2].SKILLINFO.cLockCount = 1;
		pGame->m_csStatus.m_Skill.SSlot[2].SKILLINFO.cSkillLevel = 2;

		pGame->m_csStatus.m_Skill.SSlot[3].SKILLINFO.wSkill = 0x8104;//hard hit
		pGame->m_csStatus.m_Skill.SSlot[3].SKILLINFO.cLockCount = 0;
		pGame->m_csStatus.m_Skill.SSlot[3].SKILLINFO.cSkillLevel = 3;

		pGame->m_csStatus.m_Skill.SSlot[4].SKILLINFO.wSkill = 0x8105;//blunt mastery
		pGame->m_csStatus.m_Skill.SSlot[4].SKILLINFO.cLockCount = 1;
		pGame->m_csStatus.m_Skill.SSlot[4].SKILLINFO.cSkillLevel = 5;

		pGame->m_csStatus.m_Skill.SSlot[5].SKILLINFO.wSkill = 0x8302;//bloody mana
		pGame->m_csStatus.m_Skill.SSlot[5].SKILLINFO.cLockCount = 0;
		pGame->m_csStatus.m_Skill.SSlot[5].SKILLINFO.cSkillLevel = 1;

		pGame->m_csStatus.m_Skill.SSlot[6].SKILLINFO.wSkill = 0x8403;//not existed??
		pGame->m_csStatus.m_Skill.SSlot[6].SKILLINFO.cLockCount = 0;
		pGame->m_csStatus.m_Skill.SSlot[6].SKILLINFO.cSkillLevel = 5;

		pGame->m_csStatus.m_Skill.SSlot[7].SKILLINFO.wSkill = 0x8404;//encourage
		pGame->m_csStatus.m_Skill.SSlot[7].SKILLINFO.cLockCount = 0;
		pGame->m_csStatus.m_Skill.SSlot[7].SKILLINFO.cSkillLevel = 2;

		pGame->m_csStatus.m_Skill.SSlot[8].SKILLINFO.wSkill = 0x8406;//GuardianAngel
		pGame->m_csStatus.m_Skill.SSlot[8].SKILLINFO.cLockCount = 2;
		pGame->m_csStatus.m_Skill.SSlot[8].SKILLINFO.cSkillLevel = 6;

		pGame->m_csStatus.m_Skill.SSlot[9].SKILLINFO.wSkill = 0x8504;//fullswing
		pGame->m_csStatus.m_Skill.SSlot[9].SKILLINFO.cLockCount = 1;
		pGame->m_csStatus.m_Skill.SSlot[9].SKILLINFO.cSkillLevel = 6;

		pGame->m_csStatus.m_cWeaponPos = 0;
		pGame->m_csStatus.m_cRidePos = 0;

		pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
	}

	pGame->m_csStatus.m_cWeaponPos = 0;
	pGame->m_csStatus.m_cRidePos = 0;

	char strSex[MAX_PATH], strFace[MAX_PATH], strHair[MAX_PATH];
	const char* strShape[ Item::EquipmentPos::MAX_EQUPMENT_POS ];

	for (int j = 0; j < Item::EquipmentPos::MAX_EQUPMENT_POS; j++)//load the character equipment
	{
		CItemInstance* lpEquip = pGame->m_csStatus.m_lstEquipment[j];
		strShape[j] = (NULL == lpEquip) ? NULL : g_ItemMgr.GetEquipName(lpEquip->GetProtoTypeID()); 
	}
	
	CRYLGameScene*				pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	switch (pGame->m_csStatus.m_Info.Race)
	{
		case CClass::RaceType::HUMAN :
		{
			if ( pGame->m_csStatus.m_Info.Sex == 1 )
			{
				strcpy( strSex, "PC_MAN.GCMDS" ) ; 
			}
			else 
			{
				strcpy( strSex, "PC_WOMAN.GCMDS" ) ;
			}

			CRYLHumanInventoryFrameDlg*	pDlg		= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
			CRYLHumanInventoryPanel*	pPanel		= ( CRYLHumanInventoryPanel* )pDlg->GetHumanInvenPanel() ;
			pPanel->UpdateWeaponCheckButton(pGame->m_csStatus.m_cWeaponPos);
			//pPanel->UpdateRideFlag(pGame->m_csStatus.m_cRidePos);


			break;
		}

		case CClass::RaceType::AKHAN :
		{
			if ( pGame->m_csStatus.m_Info.Sex == 1 )
			{
				strcpy( strSex, "PC_Akhan_A.GCMDS" ) ; 
			}
			else 
			{
				strcpy( strSex, "PC_Akhan_B.GCMDS" ) ;
			}

			CRYLHumanInventoryFrameDlg*	pDlg		= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
			CRYLAkhanInventoryPanel*	pPanel		= ( CRYLAkhanInventoryPanel* )pDlg->GetAkhanInvenPanel() ;
			//pPanel->UpdateRideFlag(pGame->m_csStatus.m_cRidePos);
			break;
		}
	}

	sprintf(strFace, "FACE%02d", pGame->m_csStatus.m_Info.Face);
	sprintf(strHair, "HAIR%02d", pGame->m_csStatus.m_Info.Hair);


	

	//add the character by his details. 
	RYLCreatureManager::Instance()->AddCreature( CRYLNetworkData::Instance()->m_dwMyChrID,
		pGame->m_csStatus.m_Info.Name,
		strSex,
		strFace,
		strHair,
		strShape,
		pGame->GetNetworkEquipGrade(), 
		pGame->m_csStatus.m_Info.Race,
		pGame->m_vecStartPosition,
		0.0f,
		false );

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID );//loads the character.(model point of view-hold the character model information)
	if ( pSelfCreature )//if he exists we set his details
	{
		RYLCreatureManager::Instance()->SetSelfCreature( pSelfCreature );
		g_CharacterData.m_pRYLCharacterControl->SetCreature( pSelfCreature ) ;

		pSelfCreature->SetGuildWarFlag( pGame->m_cTempGuildWarFlag ) ;
		pSelfCreature->SetRealmWarFlag( pGame->m_cTempRealmWarFlag ) ;
		pSelfCreature->SetRealmPoint( pGame->m_cTempRealmPoint );
		pSelfCreature->SetTactics(pGame->m_cTempTacticsFlag);
	}
	else
	{
		return false;
	}

	pSelfCreature->SetIsUsed( true );
	pSelfCreature->SetShape( strShape, pGame->GetNetworkEquipGrade(), TRUE );
	pSelfCreature->InitWeapon(0);

	if(pGame->m_cTempAdminFlag)//if he used the gmform command.
	{		
		// GM 모델일 경우.
		POS psPos = { 0.0f, 0.0f, 0.0f };

		const vector3	vPos	= *(pSelfCreature->GetPosition());
		const float		fDir	= pSelfCreature->GetDirection();

		pSelfCreature->SetModelFileName("PC_GM.GCMDS");

		CZ3DGeneralChrModel* pCharModel = RYLCreatureManager::Instance()->ReplaceModel(pSelfCreature, "PC_GM.GCMDS");//replace model , can be possibly used in the item model changer? hayzohar

		if(pCharModel)
		{
			pCharModel->SetPosition(vPos);
			pCharModel->SetDirection(fDir);
			pCharModel->SetMotionSheet("NO_WEAPON");

			const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];										
			std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

			pSelfCreature->SetShape(strShape, Broadcast2nd::CNetworkEquipGrade(), true );

			pSelfCreature->SetGMModelFlag(1);
		}

		pGame->m_cTempAdminFlag = 0;
	}	

	pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
	//pGame holds the character information and its beign transfered to pSelfData. (pGame is a CRYLGameData pointer).
	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );//(character point of view-hold the character actual information)
	if ( !pSelfData )
		return false;

	CZ3DOBB* pTempOBB = new CZ3DOBB;
	pSelfCreature->SetOBB( pTempOBB );

	pSelfData->m_bPeaceMode = CRYLCommunityData::Instance()->m_bPeaceMode;
	pSelfData->m_ulPartyID  = pGame->m_csStatus.m_Info.PID;
	pSelfData->m_ulGuildID  = pGame->m_csStatus.m_Info.GID;
	pSelfData->m_usClass    = pGame->m_csStatus.m_Info.Class;
	pSelfData->m_cLevel     = pGame->m_csStatus.m_Info.Level;
	pSelfData->m_cRace		= pGame->m_csStatus.m_Info.Race;
	pSelfData->m_cSex       = pGame->m_csStatus.m_Info.Sex;
	pSelfData->m_ulFame     = pGame->m_csStatus.m_Info.Fame;
	pSelfData->m_ulMileage  = pGame->m_csStatus.m_Info.Mileage;
	pSelfData->m_ulMaxHP    = pGame->m_csStatus.m_MaxHP;
	pSelfData->m_ulMaxMP    = pGame->m_csStatus.m_MaxMP;
	pSelfData->m_lCurrHP    = pGame->m_csStatus.m_Info.HP;
	pSelfData->m_lCurrMP    = pGame->m_csStatus.m_Info.MP;
	pSelfData->m_cNation	= pGame->m_cAccountNation;
	pSelfData->m_dwPlayTime = pGame->m_dwPlayTime;

	RYLCharacterDataManager::Instance()->SetSelfData( pSelfData );

	InitZone(CRYLNetworkData::Instance()->m_dwPresentZone);

	// 성과 진지를 초기화
	CRYLCampManager::Instance()->ClearAllCamp() ;
	CRYLCastleManager::Instance()->ClearAllCastle() ;
	//set the character position and camera details.
	if (pGame->m_vecStartPosition.y<10.0f)
	{
		pGame->m_vecStartPosition.y=CSceneManager::m_HeightField.GetHeight(pGame->m_vecStartPosition);		
		pSelfCreature->SetPosition(pGame->m_vecStartPosition);
	}

	pGame->m_lpSceneManager->GetCamera()->LookAt(vector3(0.0f, 150.0f, -300.0f) + pGame->m_vecStartPosition,
		vector3(0.0f, 150.0f, 0.0f) + pGame->m_vecStartPosition,
		vector3(0.0f, 1.0f, 0.0f));
	pGame->m_lpSceneManager->GetCamera()->m_vecCameraPosition = vector3(0.0f, 150.0f, -300.0f);
	pGame->m_lpSceneManager->GetCamera()->m_vecCameraPosition.Normalize();
	g_CharacterData.m_Camera.m_fCameraRotY += (-50 / 300.0f);

	if (0 == static_cast<unsigned short>(pGame->m_csStatus.m_Info.HP))//if the character is dead:
	{
		// 죽은후 Login 했을때
		CSceneManager::m_pFullSceneEffect->SetLockFlag( FULLSCENE_MOTIONBLUR,false ) ;//dead effect 1.
		CSceneManager::m_pFullSceneEffect->SetLockFlag(FULLSCENE_SATURATION2,false);//dead effect 2.
		pGame->m_csStatus.m_Info.HP = 0;//showed hp is 0.
		pSelfData->m_lCurrHP = 0;//actual character hp is set to 0.
		pSelfCreature->Dead();//dead mode.
		pGame->m_csStatus.ChantDisable();//disable all of the enchants(buffs he had).
		CRYLGameData::SetAutoRun(FALSE);
		g_CharacterData.m_bAutoFollowMode = FALSE;
		g_CharacterData.m_dwAutoFollowChrID = 0xFFFFFFFF;
	}

	CRYLLoadingScene::Instance()->InitResourceObject() ;
	pGame->m_dwClientMode = CLIENT_GAMELOADING;

	pGame->SetTipBroadTimer( pGame->m_lTipBroadCounterSeed ) ;	

	CTexture::SetPath(INTERFACETEXTUREPATH);
	CTexture* lpTexture = NULL;

#define ADD_UI_TEXTURE(szTextureName) \
	lpTexture = new CTexture; \
	lpTexture->Load(szTextureName);	\
	m_lstTexture.push_back(lpTexture);
	
	ADD_UI_TEXTURE("stone.dds");
	ADD_UI_TEXTURE("main_shortcut.dds");
	ADD_UI_TEXTURE("mmap01.dds");
	ADD_UI_TEXTURE("bsmith01.dds");		//we doesnt work with this model.
	ADD_UI_TEXTURE("mmap02.dds");		//we doesnt work with this model.
	ADD_UI_TEXTURE("LifeExt.dds");

	CGUIWindow* lpWindow = new CHumanStoneStatue ;
	lpWindow->Init(737, 360);
	m_lstWindow.push_back(lpWindow);

	// 석상.
	for (unsigned char cIndex =0; cIndex < MAX_STATUE_SPRITE; cIndex++)
	{
		if (!m_lpStatue[cIndex])
			m_lpStatue[cIndex] = new CRYLSprite;
	}

	m_lpStatue[0]->Create(0, 0, 0, 0, 60, 88, m_lstTexture[0]);
	m_lpStatue[1]->Create(0, 0, 60, 0, 120, 88, m_lstTexture[0]);
	m_lpStatue[2]->Create(0, 0, 0, 84, 60, 168, m_lstTexture[0]);
	m_lpStatue[3]->Create(0, 0, 60, 84, 120, 168, m_lstTexture[0]);
	m_lpStatue[4]->Create(0, 0, 120, 84, 180, 168, m_lstTexture[0]);
	m_lpStatue[5]->Create(0, 0, 180, 84, 240, 168, m_lstTexture[0]);
	m_lpStatue[6]->Create(0, 0, 0, 168, 60, 252, m_lstTexture[0]);
	m_lpStatue[7]->Create(0, 0, 60, 168, 120, 252, m_lstTexture[0]);
	m_lpStatue[8]->Create(0, 0, 120, 168, 180, 252, m_lstTexture[0]);
	m_lpStatue[9]->Create(0, 0, 180, 168, 240, 252, m_lstTexture[0]);

	m_lpStatue[10]->Create(0, 0, 0, 0, 60, 88, m_lstTexture[5]);
	m_lpStatue[11]->Create(0, 0, 60, 0, 120, 88, m_lstTexture[5]);
	m_lpStatue[12]->Create(0, 0, 0, 84, 60, 168, m_lstTexture[5]);
	m_lpStatue[13]->Create(0, 0, 60, 84, 120, 168, m_lstTexture[5]);
	m_lpStatue[14]->Create(0, 0, 120, 84, 180, 168, m_lstTexture[5]);
	m_lpStatue[15]->Create(0, 0, 180, 84, 240, 168, m_lstTexture[5]);
	m_lpStatue[16]->Create(0, 0, 0, 168, 60, 252, m_lstTexture[5]);
	m_lpStatue[17]->Create(0, 0, 60, 168, 120, 252, m_lstTexture[5]);
	m_lpStatue[18]->Create(0, 0, 120, 168, 180, 252, m_lstTexture[5]);
	m_lpStatue[19]->Create(0, 0, 180, 168, 240, 252, m_lstTexture[5]);
	
	if (!m_lpSkillGaugeFrame)
		m_lpSkillGaugeFrame = new CRYLSprite;

	int W = (CGMUIScreen::GetInstance()->GetWidth()-216)/2;
	int H = (CGMUIScreen::GetInstance()->GetHeight()-33)/3;

	m_iGateW = W;
	m_iGateH = H;

	// edith 수정
	m_lpSkillGaugeFrame->Create(W, H, 40, 183, 256, 216, m_lstTexture[ 2 ]);

	if (!m_lpSkillGauge[0])
		m_lpSkillGauge[0] = new CRYLSprite;
	
	m_lpSkillGauge[0]->Create(W+3, H+2, 40, 216, 246, 224, m_lstTexture[ 2 ]);
	

	if (!m_lpSkillGauge[1])
		m_lpSkillGauge[1] = new CRYLSprite;
	
	m_lpSkillGauge[1]->Create(W+3, H+2, 40, 224, 246, 232, m_lstTexture[ 2 ]);
	

	if (!m_lpSkillGauge[2])
		m_lpSkillGauge[2] = new CRYLSprite;

	m_lpSkillGauge[2]->Create(W+3, H+2, 40, 232, 246, 240, m_lstTexture[ 2 ]);


	if (!m_lpSkillGauge[3])
		m_lpSkillGauge[3] = new CRYLSprite;

	m_lpSkillGauge[3]->Create(W+3, H+2, 40, 240, 246, 248, m_lstTexture[ 2 ]);
	

	if (!m_lpSkillGauge[4])
		m_lpSkillGauge[4] = new CRYLSprite;

	m_lpSkillGauge[4]->Create(W+3, H+2, 40, 248, 246, 256, m_lstTexture[ 2 ]);

			
	if (!m_lpTarget[0])
		m_lpTarget[0] = new CRYLSprite;

	m_lpTarget[0]->Create(0, 0, 0, 0, 64, 64, m_lstTexture[3]);

	
	if (!m_lpTarget[1])
		m_lpTarget[1] = new CRYLSprite;

	m_lpTarget[1]->Create(0, 0, 64, 0, 128, 64, m_lstTexture[3]);


	if (pGame->m_csStatus.m_Info.Race == CClass::RaceType::HUMAN)
	{
		int W = (CGMUIScreen::GetInstance()->GetWidth()-49)/2;
		int H = (CGMUIScreen::GetInstance()->GetHeight())/3;

		// 49, 58
		if(!m_lpQuiver)
		{
			m_lpQuiver = new CRYLSprite;
			m_lpQuiver->Create(W, H, 124, 90, 173, 148, g_MessageBoxManager.m_lpTexture);
		}		
		if(!m_lpQuiverD)
		{
			m_lpQuiverD = new CRYLSprite;
			m_lpQuiverD->Create(W, H, 174, 90, 223, 148, g_MessageBoxManager.m_lpTexture);
		}		
	}

	pGame->m_bInterfaceAlreadyLoad = TRUE;

	g_CharacterData.m_bClientUsed = true;
	pGame->m_dwClientState = GAME_PLAYING;
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[140], CRYLNetworkData::Instance()->m_dwMyChrID, 128);

	if ( CRYLCommunityData::Instance()->m_bCheckRelation )
	{
		CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[2986], CRYLNetworkData::Instance()->m_dwMyChrID, 202 ) ;
	}

	
	CRYLLargeMapFrameDlg*	pLargeMapDlg	= ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg() ;
	CRYLHelpFrameDlg*		pHelpDlg		= ( CRYLHelpFrameDlg* )pScene->GetHelpDlg() ;

	pLargeMapDlg->LoadLargeMapTexture() ;
	pHelpDlg->LoadHelpTexture() ;

	// 교환 아이템이 남아있으면 나타낸다.
	if (pGame->m_csStatus.GetExistExchangeItem())
	{
		CRYLHumanInventoryFrameDlg*		pInvenDlg	 = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
		CRYLExchangeDlg*				pExchangeDlg = ( CRYLExchangeDlg* )pScene->GetExchangeDlg() ;
		pInvenDlg->SetVisible( TRUE ) ;
		pExchangeDlg->SetVisible( TRUE ) ;
		pExchangeDlg->ExchangeQuit() ;
	}

	// 게임옵션을 다시 로드
	CRYLOptionDlg*  pOptionDlg = static_cast<CRYLOptionDlg *>( pScene->GetOptionDlg() ) ;
	pOptionDlg->ReLoadOption() ;

	pGame->m_lpSceneManager->m_pCameraScript->HALT();

	return TRUE ;
}

void CRYLRaceBase::InitPos(CItemInstance *pItem)
{
	switch (pItem->GetItemPos())
	{
		case TakeType::TS_EXCHANGE:
		{
			CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
			CRYLExchangeDlg*	 pExchangeDlg = ( CRYLExchangeDlg* )pScene->GetExchangeDlg();
			pExchangeDlg->InitPos( pItem );
			break;
		}
	}
}

void CRYLRaceBase::InitStreetStall(unsigned long dwChrID, char *pstrStreetName) 
{
	CRYLGameScene*				pScene			= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLHumanInventoryFrameDlg* pInventoryDlg	= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg();
	CRYLStreetStallDlg*			pStreetStallDlg = ( CRYLStreetStallDlg* )pScene->GetStreetStallDlg();

	g_CharacterData.InitGlobalAction();

	if ( CRYLNetworkData::Instance()->m_dwMyChrID == dwChrID )
	{
		pStreetStallDlg->InitStreetStall( dwChrID, pstrStreetName );
	}
	else
	{
		pInventoryDlg->ShowInventory();
		pInventoryDlg->SetVisible( TRUE );
		pStreetStallDlg->InitStreetStall( dwChrID, pstrStreetName );
		pStreetStallDlg->SetVisible( TRUE );
	}
}
#ifdef __NEWNPCSCRIPT
void CRYLRaceBase::InitTrade( unsigned long ulNPCID, unsigned long dwContainerID )
{
    CRYLGameScene*		pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLNewTradeDlg*	pDlg	= ( CRYLNewTradeDlg* )pScene->GetNewTradeDlg() ;

	pDlg->SetVisible( TRUE ) ;
	pDlg->InitTrade( ulNPCID, dwContainerID );
}
#else
void CRYLRaceBase::InitTrade( unsigned long ulNPCID, unsigned char ucTradeDlgIndex, unsigned short usCapacity )
{
    CRYLGameScene*		pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLNewTradeDlg*	pDlg	= ( CRYLNewTradeDlg* )pScene->GetNewTradeDlg() ;

	pDlg->SetVisible( TRUE ) ;
	pDlg->InitTrade( ulNPCID, ucTradeDlgIndex, usCapacity );
}
#endif

void CRYLRaceBase::InitQuest(unsigned long dwChrID, unsigned long dwQuestID) 
{
	NPCNode*		pNode = g_NPCList.GetNPCNode(dwChrID);
	CRYLGameData*	pGame = CRYLGameData::Instance();

	if (NULL != pNode)
	{
		QuestNode* pQuest = pNode->GetQuestforID(dwQuestID);
		if (NULL != pQuest)
		{
			unsigned short aryComplete[1000], aryQuest[QuestInstance::MAX_EXECUTING_QUEST];
			unsigned long i;
			ZeroMemory(aryComplete, sizeof(aryComplete));
			ZeroMemory(aryQuest, sizeof(aryQuest));
			list<LPQuestInstance>::iterator it;
			for (it = pGame->m_csStatus.m_lstQuest.begin(), i = 0; it != pGame->m_csStatus.m_lstQuest.end(); it++, i++)
			{
				aryQuest[i] = (*it)->m_dwQuestID;
			}

			for (it = pGame->m_csStatus.m_lstCompleteQuest.begin(), i = 0; it != pGame->m_csStatus.m_lstCompleteQuest.end(); it++, i++)
			{
				aryComplete[i] = (*it)->m_dwQuestID;
			}

			unsigned long dwResult = pQuest->CheckQuest(pGame->m_csStatus.m_Info.Level, pGame->m_csStatus.m_Info.Fame, pGame->m_csStatus.m_Info.Class, 
														pGame->m_cAccountNation, aryComplete, (unsigned short)i, aryQuest);
			switch (dwResult)
			{
				// 성공
				case QuestNode::RES_SUCCESS :	
				{
					CRYLGameScene*		 pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
					CRYLQuestOpeningDlg* pQuestDlg	= ( CRYLQuestOpeningDlg* )pScene->GetQuestOpeningDlg();
					pQuestDlg->SetVisible( TRUE );
					pQuestDlg->SetText( dwChrID, dwQuestID, pQuest->m_strQuestTitle, pQuest->m_strQuestLevel,
						pQuest->m_strQuestDesc, pQuest->m_strQuestAward );	
					break;
				}

				// 수행할 수 없는 국적입니다.
				case QuestNode::RES_MISMATCH_NATION :
				{
					CRYLMessageBox* pMessageBox = new CRYLMessageBox;
					pMessageBox->Create(CRYLStringTable::m_strString[2951]);
					break;
				}
				
				// 클래스가 맞지 않습니다.
				case QuestNode::RES_MISMATCH_CLASS :	
				{
					CRYLMessageBox* pMessageBox = new CRYLMessageBox;
					pMessageBox->Create(CRYLStringTable::m_strString[957]);
					break;
				}

				// 해당 퀘스트는 이미 완료한 퀘스트 입니다.
				case QuestNode::RES_ALREADY_TAKEN :		
				{
					// 수행하고 있는 퀘스트입니다.
					CRYLMessageBox* pMessageBox = new CRYLMessageBox;
					pMessageBox->Create(CRYLStringTable::m_strString[958]);
					break;
				}

				// 이전에 수행해야 할 퀘스트가 완료되지 않았습니다.
				case QuestNode::RES_NOT_COMPLETE_PRIOR_QUEST :	
				{
					CRYLMessageBox* pMessageBox = new CRYLMessageBox;
					QuestNode* pCompletedQuest = g_QuestList.GetQuest(pQuest->m_dwCompletedQuest);
					if(pCompletedQuest)
					{
						char strMessage[MAX_PATH];
						sprintf(strMessage, CRYLStringTable::m_strString[63], pCompletedQuest->m_strQuestTitle);
						pMessageBox->Create(strMessage);
					} 
					else
					{
						pMessageBox->Create(CRYLStringTable::m_strString[960]);
					}
					break;
				}

				// 해당 퀘스트는 %d ~ %d 레벨 사이에 수행 가능한 퀘스트입니다.
				case QuestNode::RES_LOW_LEVEL :		
				{
					char strMessage[MAX_PATH];
					sprintf(strMessage, CRYLStringTable::m_strString[961], pQuest->m_wMinLevel, pQuest->m_wMaxLevel);
					CRYLMessageBox* pMessageBox = new CRYLMessageBox;
					pMessageBox->Create(strMessage);
					break;
				}
				// 해당 퀘스트는 %d ~ %d 레벨 사이에 수행 가능한 퀘스트입니다.
				case QuestNode::RES_LOW_FAME:		
				{
					char strMessage[MAX_PATH];
					sprintf(strMessage, CRYLStringTable::m_strString[973], pQuest->m_dwMinFame, pQuest->m_dwMaxFame);
					CRYLMessageBox* pMessageBox = new CRYLMessageBox;
					pMessageBox->Create(strMessage);
					break;
				}
			}
		}
	}
}

void CRYLRaceBase::InitItemUpgrade(unsigned long dwChrID) 
{
	NPCNode* pNPCNode = g_NPCList.GetNPCNode( dwChrID ) ;
	if ( pNPCNode )
	{
		PopupNode* pPopupNode = pNPCNode->GetPopupNode( POPUPMENU_UPGRADE ) ;
		if ( pPopupNode )
		{
#ifdef __NEWNPCSCRIPT
			if ( pPopupNode->m_dwLimitLevel > CRYLGameData::Instance()->m_csStatus.m_Info.Level )
#else
			if ( pPopupNode->m_usLimitLevel > CRYLGameData::Instance()->m_csStatus.m_Info.Level )
#endif
			{
				CRYLMessageBox* pMessageBox = new CRYLMessageBox;
				pMessageBox->Create(CRYLStringTable::m_strString[2779]);
				return;
			}
		}
		else
		{
			CRYLMessageBox* pMessageBox = new CRYLMessageBox;
			pMessageBox->Create(CRYLStringTable::m_strString[3165]);
			return;
		}
	}
	else
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[3166]);
		return;
	}

	CRYLGameScene*		pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLBlacksmithDlg*	pDlg	= ( CRYLBlacksmithDlg* )pScene->GetBlacksmithDlg();
	pDlg->SetVisible( TRUE );

	CRYLGameData*	pGame = CRYLGameData::Instance();
	pGame->m_dwTradingNpcID = dwChrID;
}

void CRYLRaceBase::InitItemComposition(unsigned long dwChrID)
{
	NPCNode* pNPCNode = g_NPCList.GetNPCNode( dwChrID ) ;
	if ( pNPCNode )
	{
		PopupNode* pPopupNode = pNPCNode->GetPopupNode( POPUPMENU_OPTIONCOMPOSITION ) ;
		if ( pPopupNode )
		{
#ifdef __NEWNPCSCRIPT
			if ( pPopupNode->m_dwLimitLevel > CRYLGameData::Instance()->m_csStatus.m_Info.Level )
#else
			if ( pPopupNode->m_usLimitLevel > CRYLGameData::Instance()->m_csStatus.m_Info.Level )
#endif
			{
				CRYLMessageBox* pMessageBox = new CRYLMessageBox;
				pMessageBox->Create(CRYLStringTable::m_strString[2780]);
				return;
			}
		}
		else
		{
			CRYLMessageBox* pMessageBox = new CRYLMessageBox;
			pMessageBox->Create(CRYLStringTable::m_strString[3165]);
			return;
		}
	}
	else
	{
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[3166]);
		return;
	}

	CRYLGameScene*			pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLItemCompositionDlg*	pDlg	= ( CRYLItemCompositionDlg* )pScene->GetItemCompositionDlg();
	pDlg->SetVisible( TRUE ) ;

	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	pScene->GetHumanInventoryFrameDlg()->SetVisible( TRUE );
	pGame->m_dwTradingNpcID = dwChrID ;
}

void CRYLRaceBase::InitCompensation(void)
{
	CRYLGameScene*		pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	pScene->GetItemReparationDlg()->SetVisible( TRUE );
}

void CRYLRaceBase::InitDeposit(void) 
{
	CRYLGameScene*				pScene					= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLVaultDlg*				pVaultDlg				= ( CRYLVaultDlg* )pScene->GetVaultDlg();
	CRYLVaultPasswordDlg*		pVaultPasswordDlg		= ( CRYLVaultPasswordDlg* )pScene->GetVaultPasswordDlg();
	CRYLVaultCertificationdDlg*	pVaultCertificationDlg	= ( CRYLVaultCertificationdDlg* )pScene->GetVaultCertificationdDlg();

	if ( pVaultDlg->m_bLogined )
	{
		SendPacket::StoreLogout( g_GameSession );
	}

	pVaultDlg->InitVault();
	pVaultPasswordDlg->m_pChangePasswordEdit->SetText( "" );
	pVaultPasswordDlg->m_pCurrentPasswordEdit->SetText( "" );

	pVaultCertificationDlg->m_pPasswordEdit->SetText( "" );
}

void CRYLRaceBase::InitStatusRetrain(unsigned char cState) 
{
	CRYLGameData*		pGame	= CRYLGameData::Instance() ;

	// 10랩이사잉구 전직을 해야만 사용가능
	if ( //pGame->m_csStatus.m_Info.Level < 10 || 
		( pGame->m_csStatus.m_Info.Class == CClass::Fighter		|| 
		pGame->m_csStatus.m_Info.Class == CClass::Rogue			|| 
		pGame->m_csStatus.m_Info.Class == CClass::Mage			|| 
		pGame->m_csStatus.m_Info.Class == CClass::Acolyte		|| 
		pGame->m_csStatus.m_Info.Class == CClass::Combatant		|| 
		pGame->m_csStatus.m_Info.Class == CClass::Officiator ) )
	{
		// 사용못함.
		CRYLMessageBox* pMessageBox = new CRYLMessageBox;
		pMessageBox->Create(CRYLStringTable::m_strString[2915]);
		return;
	}


	CRYLGameScene*			pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLStatusRetrainDlg*	pDlg		= reinterpret_cast<CRYLStatusRetrainDlg *>(pScene->GetStatusRetrainDlg());

	pDlg->SetState( cState );
	pDlg->SetVisible( TRUE );
}

VOID CRYLRaceBase::InitClassChange(ULONG dwNPCID) 
{
	CRYLGameData*		pGame	= CRYLGameData::Instance() ;
	CRYLGameScene*		pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLJobChangeDlg*	pJobDlg	= ( CRYLJobChangeDlg* )pScene->GetJobChangeDlg();

	NPCNode* pNode = g_NPCList.GetNPCNode(dwNPCID);
	if (NULL != pNode)
	{
		DialogNode* pDialog = pNode->GetScript( DialogNode::KIND_CHANGE_CLASS );
		if (NULL != pDialog)
		{
			// edith 2008.03.26 전직 제환 10랩이상.
			if ( //pGame->m_csStatus.m_Info.Level >= 1 && 
				( pGame->m_csStatus.m_Info.Class == CClass::Fighter		|| 
				pGame->m_csStatus.m_Info.Class == CClass::Rogue			|| 
				pGame->m_csStatus.m_Info.Class == CClass::Mage			|| 
				pGame->m_csStatus.m_Info.Class == CClass::Acolyte		|| 
				pGame->m_csStatus.m_Info.Class == CClass::Combatant		|| 
				pGame->m_csStatus.m_Info.Class == CClass::Officiator ) )
			{
				pJobDlg->SetText(pDialog->m_strWord);
				pJobDlg->SetVisible( TRUE );
			}
			else
			{
				CRYLMessageBox* pMessageBox = new CRYLMessageBox;
				pMessageBox->Create(CRYLStringTable::m_strString[3800]);
			}		
		}
	}
}

