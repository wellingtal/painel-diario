
#include "..\\ItemInstance.h"
#include "..\\ijl.h"
#include "WinInput.h"
#include "FrameTimer.h"

#include <Item/ItemMgr.h>
#include <Skill/SkillMgr.h>
#include <GameTime/GameTimeConstants.h>

#include <Creature/EnemyCheck.h>
#include <Creature/Monster/MonsterMgr.h>
#include <Creature/Siege/SiegeConstants.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendEtc.h>
#include <Network/ClientSocket/Send/SendAuthServer.h>

#include "GMBASE.h"
#include "SoundMgr.h"
#include "Texture.h"

#include "RYLMessageBoxManager.h"
#include "RYLStringTable.h"
#include "RYLTimer.h"
#include "RYLEdit.h"
#include "RYLListBox.h"
#include "RYLTabButton.h"

#include "RYLCharacterDataManager.h"
#include "RYLCreatureManager.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLCharacterSelectScene.h"

#include "RYLCastle.h"
#include "RYLCastleManager.h"
#include "RYLCampManager.h"
#include "RYLCamp.h"

#include "RYLCharacterInfoDlg.h"
#include "RYLItemCompositionDlg.h"
#include "RYLHumanInventoryFrameDlg.h"
#include "RYLHumanInventoryPanel.h"
#include "RYLAkhanInventoryPanel.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"
#include "RYLGuildPropertyPanel.h"
#include "RYLGuildMemberPanel.h"
#include "RYLGuildTacticsPanel.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLHostilityDlg.h"
#include "RYLSystemDlg.h"
#include "RYLChattingDlg.h"
#include "RYLNoticeManager.h"
#include "RYLSkillPanel.h"
#include "RYLSoundOptionPanel.h"
#include "RYLRefusalPanel.h"
#include "RYLFriendPanel.h"
#include "RYLSocietyFrameDlg.h"
#include "RYLPartyPanel.h"
#include "RylGuildWarPenaltyDlg.h"

#include "RylSiegeTimeDlg.h"

#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLNetWorkData.h"
#include "RYLGameData.h"

#include "RYLGameTimeManager.h"
#include "GMMemory.h"

#include "RYLJoystickConfigurePanel.h"

#include "SoundMgr.h"
#include "BgmManager.h"
#include "AmbienceManager.h"


CRYLGameData*		CRYLGameData::s_pInstance = NULL ;
char				CRYLGameData::m_strSheet[ MAX_PATH ] ;
CCharacterControl	g_CharacterData ;

CRYLGameData::CRYLGameData() 
{
//	m_dwFireingTick = 0;
	m_dwRideTickCount = 0;

	m_bSkillFlag			= FALSE;
	
	m_bCreateChar     		= FALSE ;
	m_lQuickChatTimer 		= 0 ;
	m_lTipBroadCounter		= 0;
	m_lTipBroadCounterSeed	= 60000;	// 3��
	m_roOption.m_wReject	= 0;
	m_lSkillUseCounter		= 0;
	m_lPortalCounter		= 0 ;
	m_lKillCounter			= 0 ;
	m_lRespawnCounter			= 0 ;
	m_lExitCounter			= 0 ;
	m_lLogoutCounter		= 0 ;
	m_lLogoutTimeResult		= 0 ;
	m_lBillingTimer 		= 0;
	m_dwGuildWarPerTime		= 1800000;
	m_dwNationWarPerTime	= 1800000;
	m_bCursorToggle 		= FALSE;

	m_bStallRegFlag			= FALSE;
	m_bPullDownFlag			= FALSE;
	m_bUseCashFlag			= FALSE;

	m_lpPickItem			= NULL;
	m_ClassEffectPtr		= NULL;
	m_lpItemUpgrade			= NULL;
	m_lpItemComposition		= NULL;
	m_lpItemResultEft		= NULL;

	m_lpRenameWarrantItem	= NULL;

	m_ClassEffectAlpha		= 0;
	m_pPlayerSelectEff		= NULL;
	m_pPlayerSkillSelectEff = NULL;
	m_pEnemySelectEff		= NULL;
	m_pEnemySkillSelectEff	= NULL;
	m_QuestCompleteEffectPtr= NULL;
	m_bQuestComplete		= FALSE;
	m_bUsedDeposit			= FALSE;

	m_bRYLTradeMode			= FALSE;
	m_bRYLAutoTrade			= FALSE;
	m_lpTradeItem			= NULL;

	m_bMouseMode			= TRUE;
	m_bShowCursor			= TRUE;
	m_bAutoRunMode			= FALSE;
	m_bSimpleMode			= FALSE;
	m_bEnableMove			= TRUE;
	m_dwKindCursor			= CURSOR_NORMAL;
	m_bHardwareCursor		= TRUE;
	m_bSystemConsoleShow	= FALSE;
	m_bTargetMouseMode		= FALSE;
	m_bChangeWeapon			= FALSE;
	m_bInterfaceAlreadyLoad = FALSE;
	m_bStreetStallEnter		= FALSE;
	m_bAdminObserver		= FALSE;

	m_wSkillUseID			= 0;
	m_dwRespawnResult		= 0;
	m_dwClassCleanResult	= 0;

	m_dwDropPickResult		= 0;
	m_dwTradePickResult		= 0;

	m_dwTempInvenResult		= 0;
	m_cTempInvenItemNum		= 0;

	m_dwAuthorizeResult		= 0;
	m_dwAuthorizeCasterID	= 0;
	m_cAuthorizeCmd			= 0;

	m_cEliteBonus			= 0;

	m_dwClientLoginResult 		= 0;
	m_dwClientExitConfirm 		= 0;
	m_dwExecuteExplorerConfirm	= 0;

	m_dwRealmWarJoinResult	= 0;

	m_dwGuildWarJoinLeaveResult = 0;

	m_dwRequestResult = 0;
	m_dwCounterHostilityResult = 0;
	m_dwTargetGID = 0 ;
	m_dwAlreadyHostilityGID = 0 ;

	m_bUpdateMove		= TRUE;
	m_bGuildSafeResult	= FALSE;

	m_dwFunction		= 0;

	m_cChangeNameCount = 0;	

	m_lpSceneManager 	= NULL ;
	m_lpInterface	 	= NULL ;

	m_bSimpleMouse		= FALSE ;
	m_bSaveID			= FALSE ;
	m_bFPS				= FALSE ;

	m_bMovieVersion		= false ;
	m_dwViewNameType	= VIEW_ALL_NAME ;

//	m_bRYLVaultClick					= FALSE ;
//	m_bRYLBlacksmithClick				= FALSE ;
//	m_bRYLTradeDlgClick					= FALSE ;
	m_bRYLStreetStallDlgClick			= FALSE ;
	m_bRYLHumanInventoryFrameDlgClick	= FALSE ;
	m_bRYLExchangeDlgClick				= FALSE ;


	m_cTempGuildWarFlag					= 0;
	m_cTempRealmWarFlag					= 0;

	m_cReLogin							= 0;

	m_lpNumber							= NULL ;
	m_lpStackNumber						= NULL ;	
	m_lpCursor							= NULL ;
	m_lpStartKit						= NULL ;
	m_lpCampStartKit					= NULL ;
	m_lpWorldWeaponKit					= NULL ;
	s_pInstance = this ;

    RYLCreatureManager::Instance()->SetObjectControl( &g_CharacterData );

	m_iRespawnNum = 0 ;
	memset( m_RespawnArea, 0, sizeof( RespawnArea ) * 100 ) ;
	m_bLogoutEnable = false ;


	m_dwNewTime = 0;
	m_dwPlayTime = 0;
	m_StartTime = CTime::GetCurrentTime();
	m_lPlayTimer = 0;	

	// �����̾� ���� �ʱ�ȭ
	m_lPremiumTime = 0;
	m_iPremiumType = 0;

	m_lNewPremiumTime = 0;
	m_StartPremiumTime = CTime::GetCurrentTime();
	m_lPlayPremiumTimer = 0;	


	memset( m_scLoginChar, 0, sizeof( CHAR_VIEW ) * MAX_CHAR_SLOT ) ;
	memset( m_scLoginCharGuildData, 0, sizeof(sGuildData) * MAX_CHAR_SLOT ) ;
	// WORK_LIST 2.1 ���� ���� �߰�
	m_cAccountNation = 0 ;
}

CRYLGameData::~CRYLGameData() 
{
	// edith ����
	RYLCreatureManager::Release();

//	RYLCharacterDataManager::Release();

	GM_DELETE( m_lpInterface ) ; 
	GM_DELETE( m_lpSceneManager ) ;

	GM_DELETE( m_lpNumber ) ;
	GM_DELETE( m_lpStackNumber ) ;
}

VOID	CRYLGameData::LoadTargetCursor() 
{
//	_ASSERTE(_CrtCheckMemory());
	m_pPlayerSelectEff = new CEffScript;
	m_lpSceneManager->AddEffectScript(m_pPlayerSelectEff);
	m_pPlayerSelectEff->GetScriptBinData("mark.esf");
	m_pPlayerSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
	m_pPlayerSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);

//	_ASSERTE(_CrtCheckMemory());
	m_pPlayerSkillSelectEff = new CEffScript;
	m_lpSceneManager->AddEffectScript(m_pPlayerSkillSelectEff);
	m_pPlayerSkillSelectEff->GetScriptBinData("inter_mark_player.esf");
	m_pPlayerSkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
	m_pPlayerSkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);

//	_ASSERTE(_CrtCheckMemory());
	m_pEnemySelectEff = new CEffScript;
	m_lpSceneManager->AddEffectScript(m_pEnemySelectEff);
	m_pEnemySelectEff->GetScriptBinData("inter-enemyattack.esf");
	m_pEnemySelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
	m_pEnemySelectEff->SetEndPos(0.0f, 0.0f, 0.0f);

//	_ASSERTE(_CrtCheckMemory());
	m_pEnemySkillSelectEff = new CEffScript;
	m_lpSceneManager->AddEffectScript(m_pEnemySkillSelectEff);
	m_pEnemySkillSelectEff->GetScriptBinData("inter_mark_enemy.esf");
	m_pEnemySkillSelectEff->SetStartPos(0.0f, 0.0f, 0.0f);
	m_pEnemySkillSelectEff->SetEndPos(0.0f, 0.0f, 0.0f);
}

VOID	CRYLGameData::LinkCharacterData() 
{
	g_CharacterData.GetMotionSheet			= GetMotionSheet ;
	g_CharacterData.GetWeaponType			= GetWeaponType ;
	g_CharacterData.CheckTargetLength		= CheckTargetLength ;
	g_CharacterData.CheckTargetforSkill		= CheckTargetforSkill ;
	
	g_CharacterData.CheckSkillStart			= CheckSkillStart ;
	g_CharacterData.CheckStillCasting		= CheckStillCasting ;

	g_CharacterData.SetKillCounter			= SetKillCounter ;
	g_CharacterData.SetClientExitCounter	= SetClientExitCounter ;
	g_CharacterData.SetLogoutCounter		= SetLogoutCounter ;
	g_CharacterData.SetPortalCounter		= SetPortalCounter ;
	g_CharacterData.SetSkillUseCounter		= SetSkillUseCounter ;
	g_CharacterData.SetAutoRun				= SetAutoRun ;
	g_CharacterData.GetAutoRun				= GetAutoRun ;
	g_CharacterData.ChangeWeapon			= ChangeWeapon ;
	g_CharacterData.SetChangeWeapon			= SetChangeWeapon ;
	g_CharacterData.CheckAttackable			= CheckAttackable ; 

	g_CharacterData.GetMonsterMinBox		= GetMonsterMinBox ;
	g_CharacterData.GetMonsterMaxBox		= GetMonsterMaxBox ;

	g_CharacterData.GetClientEnemyType		= GetClientEnemyType ;
	g_CharacterData.GetClientIdentity		= GetClientIdentity ;
	g_CharacterData.GetEnchantLevel			= GetEnchantLevel ;
	g_CharacterData.GetEnchantTime			= GetEnchantTime ;
	g_CharacterData.GetSkillIDbyChantID		= GetSkillIDbyChantID ;
	g_CharacterData.UpdateCamp				= UpdateCamp ;

    RYLCreatureManager::Instance()->GetMotionSheet      = GetMotionSheet;
    RYLCreatureManager::Instance()->GetWeaponType       = GetWeaponType;
    RYLCreatureManager::Instance()->GetClientIdentity   = GetClientIdentity;
    RYLCreatureManager::Instance()->GetClientEnemyType  = GetClientEnemyType;
}

char*		CRYLGameData::GetMotionSheet( char *strWeaponName, char *strShieldName, unsigned short wClass )
{
	if ( strWeaponName == NULL || !strcmp( strWeaponName, "" ) )
	{
		strcpy( m_strSheet, "NO_WEAPON_B" ) ;
		return m_strSheet ;
	}

	unsigned long dwDualWeapon = 0 ;
	const Item::ItemInfo *lpItemBase  = g_ItemMgr.GetItemInfoFromEquipName( strWeaponName ) ;
	const Item::ItemInfo *lpItemBase2 = (NULL == strShieldName) ? NULL : g_ItemMgr.GetItemInfoFromEquipName( strShieldName ) ;
	
	if ( lpItemBase2 )
	{
		switch ( lpItemBase2->m_DetailData.m_cItemType )
		{
			case Item::ItemType::ONEHANDED_AXE :
			case Item::ItemType::ONEHANDED_SWORD :
			case Item::ItemType::ONEHANDED_BLUNT :
				if ( wClass == 6 ) 
					dwDualWeapon = 1 ;
				break ;

			case Item::ItemType::DAGGER :
				if ( wClass == 7 ) 
					dwDualWeapon = 2 ;
				break ;

			case Item::ItemType::SKILL_A_KNIFE :
				if ( wClass == CClass::ShadowOff ) 
					dwDualWeapon = 3 ;
				break ;

			case Item::ItemType::SKILL_A_ATTACK :
				if ( wClass == CClass::Attacker)
					dwDualWeapon = 4 ;
				break;

		}
	}

	if ( lpItemBase )
	{
		switch ( lpItemBase->m_DetailData.m_cItemType )
		{
			case Item::ItemType::ONEHANDED_AXE :
			case Item::ItemType::ONEHANDED_SWORD :
			case Item::ItemType::ONEHANDED_BLUNT :
				if ( dwDualWeapon == 1 )
					strcpy( m_strSheet, "WEAPON_2DO" ) ;
				else
					strcpy( m_strSheet, "WEAPON_ONE_HAND" ) ;
				break ;

			case Item::ItemType::COM_SWORD :
			case Item::ItemType::COM_BLUNT :
			case Item::ItemType::OPP_AXE :
			case Item::ItemType::OPP_HAMMER :
			case Item::ItemType::OPP_SLUSHER :
				if ( dwDualWeapon == 4 )
					strcpy( m_strSheet, "WEAPON_2DO" ) ;
				else
					strcpy( m_strSheet, "WEAPON_ONE_HAND" ) ;
				break ;

			case Item::ItemType::OPP_SYTHE :
				strcpy( m_strSheet, "WEAPON_SICKLE" ) ;
				break ;

			case Item::ItemType::OPP_TALON :
				if ( wClass == CClass::ShadowOff )
				{
					if ( dwDualWeapon == 3 )
						strcpy( m_strSheet, "WEAPON_2CLAW" ) ;
					else
						strcpy( m_strSheet, "WEAPON_SHADOW_OFF" ) ;
				}
				else
					strcpy( m_strSheet, "WEAPON_CLAW" ) ;
				break ;

			case Item::ItemType::STAFF :
				strcpy( m_strSheet, "WEAPON_STAFF" ) ;
				break ;

			case Item::ItemType::TWOHANDED_BLUNT :
			case Item::ItemType::TWOHANDED_AXE :
				strcpy( m_strSheet, "WEAPON_BLUNT" ) ;
				break ;

			case Item::ItemType::BOW :
				strcpy( m_strSheet, "WEAPON_BOW" ) ;
				break ;

			case Item::ItemType::CROSSBOW :
				strcpy( m_strSheet, "WEAPON_CROSSBOW" ) ;
				break ;

			case Item::ItemType::DAGGER :
				if ( wClass == 2 || wClass == 7 )	// Rogue, Archer, Assasin
				{
					if ( wClass == 7 && dwDualWeapon == 2 )
						strcpy( m_strSheet, "WEAPON_2DA" ) ;
					else
						strcpy( m_strSheet, "CLASS_ROGUE" ) ;
				}
				else
					strcpy( m_strSheet, "WEAPON_DAGGER" ) ;
				break ;

			case Item::ItemType::TWOHANDED_SWORD :
				strcpy( m_strSheet, "WEAPON_TWO_HAND" ) ;
				break ;

			default :
				strcpy( m_strSheet, "NO_WEAPON_B" ) ;
				break ;
		}
	} 
	else
	{
		strcpy( m_strSheet, "NO_WEAPON_B" ) ;
	}

	return m_strSheet ;
}



unsigned long	CRYLGameData::GetWeaponType(char *strWeaponName)
{
	if (strWeaponName == NULL || !strcmp(strWeaponName, "")) return 0;

	const Item::ItemInfo *lpItemBase = g_ItemMgr.GetItemInfoFromEquipName((char *)strWeaponName);

	if (lpItemBase)
	{
		switch (lpItemBase->m_DetailData.m_cItemType)
		{
			case Item::ItemType::ONEHANDED_AXE:
			case Item::ItemType::ONEHANDED_SWORD:
			case Item::ItemType::ONEHANDED_BLUNT:
			case Item::ItemType::STAFF:
			case Item::ItemType::TWOHANDED_BLUNT:
			case Item::ItemType::TWOHANDED_AXE:
			case Item::ItemType::BOW:
			case Item::ItemType::CROSSBOW:
			case Item::ItemType::DAGGER:
			case Item::ItemType::TWOHANDED_SWORD:
				return lpItemBase->m_DetailData.m_cItemType;
		}
	}

	return 0;
}

void		CRYLGameData::CheckTargetLength( unsigned long &dwTargetID) 
{
	const Skill::ProtoType* lpSkill = NULL;

	// �����ʱ�ȭ
	for(int i = 0; i < QUICK::MAX_QUICK_NUM; ++i)
		s_pInstance->m_csStatus.m_bQuickSlotLength[i] = TRUE;

	if(dwTargetID == 0xFFFFFFFF)
		return;

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

    RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( dwTargetID );
    if ( pTargetCreature )
    {
		// �Ÿ��� �������Ѱ�
		vector3 vecLen = *( pTargetCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
		vecLen.y = 0.0f;

		// edith ������ �߰��۾�- �������� �Ÿ��� �缭 �����ִ°Ͱ� ���°��� �����Ѵ�.
		const Skill::ProtoType* lpQuickSkill = NULL;
		for(int i = 0; i < QUICK::MAX_QUICK_NUM; ++i)
		{
			if(s_pInstance->m_csStatus.m_lstQuickSlot[i] == NULL)
				continue;

			lpQuickSkill = g_SkillMgr.GetSkillProtoType( s_pInstance->m_csStatus.m_lstQuickSlot[i]->GetSkillID() ) ;
			if (NULL == lpQuickSkill)
				continue;

			// edith 2008.01.31 ���� ��ų�̸�.. ������ TRUE �̴�.
			if(lpQuickSkill->m_usSkill_ID == 0x8901 ||
				lpQuickSkill->m_usSkill_ID == 0x8a07 ||
				lpQuickSkill->m_usSkill_ID == 0x9601)
				continue;

			if (false == s_pInstance->CanUseSkill(pTargetCreature, lpQuickSkill->m_eTargetType))
				continue;

			// ��ī��Ʈ�� ��ų�� �����´�.
			lpQuickSkill = &lpQuickSkill[ s_pInstance->m_csStatus.m_lstQuickSlot[i]->GetSkillLockCount() ];

			// �����Ÿ��� 2m �̸��̸� �����Ÿ� ��ų. 
			// �������� Ȱ��ȭ ��Ų��.
			if(lpQuickSkill->m_fMaxRange < 2.0f)
				continue;

			if (vecLen.GetLens() > lpQuickSkill->m_fMaxRange * 100.0f) 
				s_pInstance->m_csStatus.m_bQuickSlotLength[i] = FALSE;
		}
	}
}

void		CRYLGameData::CheckTargetforSkill(unsigned long &dwTargetID, BOOL bCheckLength)
{
	const Skill::ProtoType* lpSkill = NULL;

	if (NULL != s_pInstance->m_csStatus.m_lpQuickSelected)
	{
		lpSkill = g_SkillMgr.GetSkillProtoType( s_pInstance->m_csStatus.m_lpQuickSelected->GetSkillID() ) ;
		if (NULL == lpSkill)
		{
			dwTargetID = 0xFFFFFFFF;
			return;
		}

		lpSkill = &lpSkill[ s_pInstance->m_csStatus.m_lpQuickSelected->GetSkillLockCount() ];
		g_CharacterData.m_fSkillLength = lpSkill->m_fMaxRange * 100.0f;
	} 
	else
	{
		dwTargetID = 0xFFFFFFFF;
		return;
	}

	if (lpSkill->m_eTargetType == Skill::Target::PARTY || 
		lpSkill->m_eTargetType == Skill::Target::SUMMON || 
		(lpSkill->m_eTargetType == Skill::Target::ENEMY && lpSkill->m_fMaxRange <= 1.0f))
	{
		dwTargetID = CRYLNetworkData::Instance()->m_dwMyChrID;
		return;
	}

	if (lpSkill->m_eTargetType != Skill::Target::FRIEND && 
		dwTargetID == CRYLNetworkData::Instance()->m_dwMyChrID) 
	{
		dwTargetID = 0xFFFFFFFF;
		return;
	}

	if (lpSkill->m_eTargetType == Skill::Target::FRIEND)
	{
		// ������� ��Ʈ���� ������ ���� �ڱ��ڽ�����ĳ����
		if (g_DeviceInput.KeyHold(VK_CONTROL) || KeyControl::IsKeyState(KeyControl::KEY_COLUM_SELFTARGET) || lpSkill->m_fMaxRange <= 1.0f)
		{
			dwTargetID = CRYLNetworkData::Instance()->m_dwMyChrID;
			return;
		}
	}

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

//    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
//    if ( !pSelfCreature )
//        return;

    RYLCreature* pTargetCreature = RYLCreatureManager::Instance()->GetCreature( dwTargetID );
    if ( pTargetCreature )
    {
//        CharDataNode* pTargetData = RYLCharacterDataManager::Instance()->GetCharData( pTargetCreature->GetCharID() );
//        if ( !pTargetData )
//            return;

		if (false == s_pInstance->CanUseSkill(pTargetCreature, lpSkill->m_eTargetType))
		{
			dwTargetID = 0xFFFFFFFF;
			return;
		}

		if (TRUE == bCheckLength)
		{
			// �Ÿ��� �������Ѱ�
			vector3 vecLen = *( pTargetCreature->GetPosition() ) - *( pSelfCreature->GetPosition() );
			vecLen.y = 0.0f;

			if (vecLen.GetLens() <= lpSkill->m_fMaxRange * 100.0f) 
			{
				return;
			}
		} 
	}
	else
	{
		dwTargetID = 0xFFFFFFFF ;
		return ;
	}
}

BOOL		CRYLGameData::CheckSkillStart(BOOL bDown)
{
	BOOL bAble = FALSE;

	// edith 2008.06.25 �������� ���� ����
	// ���� Ÿ�������� ĳ������ ������ �ȵȴ�.
	CRYLGameData* pGame = CRYLGameData::Instance();
	if(pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::AVATA] != NULL)
	{
		unsigned char cType = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::AVATA]->GetItemInfo().m_DetailData.m_cItemType;
		if(cType != Item::ItemType::TRANSFORM && s_pInstance->m_csStatus.m_cRidePos == 1)
				return FALSE;
	}

	// ����ó���� ������ ������ �������� �ʴ´�.
	if(pGame->m_bSkillFlag)
		return FALSE;

	if ( s_pInstance->m_csStatus.m_lpQuickSelected )
	{
		// æƮ�ϰ� �нú�� ��ų�� ���۵��� �ʴ´�.
		if ( s_pInstance->m_csStatus.m_lpQuickSelected->GetSkillType() == Skill::Type::CHANT ||
			 s_pInstance->m_csStatus.m_lpQuickSelected->GetSkillType() == Skill::Type::PASSIVE)		
		{
			return FALSE;
		}
	} 
	else
	{
		return FALSE;
	}
	static bool		chatMessage = true ;

	if (bDown)
	{
		unsigned long dwFunction = 0;
#ifdef _NEW_KEY_
		if (g_DeviceInput.GetSkillHold() || KeyControl::IsKeyState(KeyControl::KEY_COLUM_CASTING))
#else
		if (g_DeviceInput.GetIsRightMousePress() || KeyControl::IsKeyState(KeyControl::KEY_COLUM_CASTING))
#endif
		{
			chatMessage = true;

			// ���̾ ����
			if ( s_pInstance->m_csStatus.m_lpQuickSelected->GetSkillID() == 0x9502)	
			{
				// ���̾ ����� ��Ŷ ����� �޾ƾ߸� ���簡 �����ϰ� �Ѵ�.
				unsigned short wAmmo = 4003;
				CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItemfromID(wAmmo);

                if (!lpItem || ClientSocket::IsNSFlagChainChecked(CmdCharUseItem))
					return FALSE;

				// ���̾ 5�ʿ� 1�� �߻��ϰ� ����.
//				if((GetTickCount()-pGame->m_dwFireingTick) < 1000)
//					return FALSE;

//				pGame->m_dwFireingTick = GetTickCount();

				// ���̾�� �տ� Ÿ���� ���� ��쿡�� bAble �� TRUE �� �ȴ�.
				if ( s_pInstance->m_lpInterface && 0xFFFFFFFF != s_pInstance->m_lpInterface->m_dwSpecialTargetChrID )
				{
                    RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( s_pInstance->m_lpInterface->m_dwSpecialTargetChrID );
                    if ( !pTempCreature )
                        return false;

                    vector3* vecChrPos = pTempCreature->GetPosition();
					if ( vecChrPos )
					{
						s_pInstance->m_dwFunction = dwFunction ;
						bAble = TRUE ;
					}
					else 
					{
						return FALSE ;
					}
				}
				else 
				{
					return FALSE ;
				}
			}
			else
			{
				s_pInstance->m_dwFunction = dwFunction;
				bAble = TRUE;
			}
		}
		else if (g_DeviceInput.KeyDown(VK_F1))		{ dwFunction = 1;	}
		else if (g_DeviceInput.KeyDown(VK_F2)) 		{ dwFunction = 2;	}
		else if (g_DeviceInput.KeyDown(VK_F3)) 		{ dwFunction = 3;	}
		else if (g_DeviceInput.KeyDown(VK_F4)) 		{ dwFunction = 4;	}
		else if (g_DeviceInput.KeyDown(VK_F5)) 		{ dwFunction = 5;	}
		else if (g_DeviceInput.KeyDown(VK_F6)) 		{ dwFunction = 6;	}
		else if (g_DeviceInput.KeyDown(VK_F7)) 		{ dwFunction = 7;	}
		else if (g_DeviceInput.KeyDown(VK_F8)) 		{ dwFunction = 8;	}
		else if (g_DeviceInput.KeyDown(VK_F9)) 		{ dwFunction = 9;	}
		else if (g_DeviceInput.KeyDown(VK_F10))		{ dwFunction = 10;	}
		//hayzohar
		else if (g_DeviceInput.KeyDown(VK_F11))		{ dwFunction = 11;	} // avalansa was by hayzohar
		else if (g_DeviceInput.KeyDown(VK_F12))		{ dwFunction = 12;	}

		if (dwFunction)
		{
			const Skill::ProtoType *lpSkillType = g_SkillMgr.GetSkillProtoType( s_pInstance->m_csStatus.m_lpQuickSelected->GetSkillID());
			if (lpSkillType)
			{
				int count = 1;
				for(int i = 0; i < s_pInstance->m_csStatus.m_PartyInfo.m_cMemberNum; i++)
				{
					if ( s_pInstance->m_csStatus.m_PartyInfo.ServerID[ i ] )
					{
						if ( s_pInstance->m_csStatus.m_PartyInfo.MemberCID[i] != CRYLNetworkData::Instance()->m_dwMyChrID )
						{
							// Ÿ��
							count++;
						}
					}

					if (dwFunction == 1)
					{
						unsigned long dwTarget = CRYLNetworkData::Instance()->m_dwMyChrID;
						CheckTargetforSkill(dwTarget, TRUE);

						if (dwTarget != 0xFFFFFFFF)
						{
							bAble = TRUE;
							s_pInstance->m_dwFunction = dwFunction;
						}
						break;
					} else if (count == dwFunction)
					{
						unsigned long dwTarget = s_pInstance->m_csStatus.m_PartyInfo.MemberCID[i];
						CheckTargetforSkill(dwTarget, TRUE);

						// �Ÿ�üũ
						CheckTargetLength(dwTarget);

						if (dwTarget != 0xFFFFFFFF)
						{
							// ��Ŀ������ F2 ��ư���ε� �ڽſ��� ĳ���� �Ҽ� �ֵ��� �ϱ� ���� �Ʒ��� �ҽ��� �ּ� ó���մϴ�.
							// by vincent 2005 / 2 / 21
							/*
							if (dwTarget == CRYLNetworkData::Instance()->m_dwMyChrID)
							{
								dwFunction = 1;
							}
							*/

							bAble = TRUE;
							s_pInstance->m_dwFunction = dwFunction;
						}
						break;
					}
				}
			}
		}
	} else
	{
		unsigned long dwFunction = 0;
		if (g_DeviceInput.GetIsRightMouseUp()) bAble = TRUE;
		else if (g_DeviceInput.KeyUp(VK_F1)) { bAble = TRUE; dwFunction = 1; }
		else if (g_DeviceInput.KeyUp(VK_F2)) { bAble = TRUE; dwFunction = 2; }
		else if (g_DeviceInput.KeyUp(VK_F3)) { bAble = TRUE; dwFunction = 3; }
		else if (g_DeviceInput.KeyUp(VK_F4)) { bAble = TRUE; dwFunction = 4; }
		else if (g_DeviceInput.KeyUp(VK_F5)) { bAble = TRUE; dwFunction = 5; }
		else if (g_DeviceInput.KeyUp(VK_F6)) { bAble = TRUE; dwFunction = 6; }
		else if (g_DeviceInput.KeyUp(VK_F7)) { bAble = TRUE; dwFunction = 7; }
		else if (g_DeviceInput.KeyUp(VK_F8)) { bAble = TRUE; dwFunction = 8; }
		else if (g_DeviceInput.KeyUp(VK_F9)) { bAble = TRUE; dwFunction = 9; }
		else if (g_DeviceInput.KeyUp(VK_F10)) { bAble = TRUE; dwFunction = 10; }
		//hayzohar
		else if (g_DeviceInput.KeyUp(VK_F11)) { bAble = TRUE; dwFunction = 11; }
		else if (g_DeviceInput.KeyUp(VK_F12)) { bAble = TRUE; dwFunction = 12; }  // avalansa was by hayzohar
		if (dwFunction)
		{
			const Skill::ProtoType *lpSkillType = g_SkillMgr.GetSkillProtoType( s_pInstance->m_csStatus.m_lpQuickSelected->GetSkillID());
			if (lpSkillType)
			{
				bAble = TRUE;
			}
		}
	}

	return bAble;
}

BOOL		CRYLGameData::CheckStillCasting(unsigned long &dwFunction)
{
	BOOL bAble = FALSE;

	switch ( s_pInstance->m_dwFunction)
	{
		case 0:
#ifdef _NEW_KEY_
			if (g_DeviceInput.GetSkillHold() || KeyControl::IsKeyState(KeyControl::KEY_COLUM_CASTING)) 
#else
			if (g_DeviceInput.GetIsRightMousePress() || KeyControl::IsKeyState(KeyControl::KEY_COLUM_CASTING)) 
#endif
				bAble = TRUE;
			break;

		case 1:
			if (g_DeviceInput.KeyHold(VK_F1)) bAble = TRUE;
			break;

		case 2:
			if (g_DeviceInput.KeyHold(VK_F2)) bAble = TRUE;
			break;

		case 3:
			if (g_DeviceInput.KeyHold(VK_F3)) bAble = TRUE;
			break;

		case 4:
			if (g_DeviceInput.KeyHold(VK_F4)) bAble = TRUE;
			break;

		case 5:
			if (g_DeviceInput.KeyHold(VK_F5)) bAble = TRUE;
			break;

		case 6:
			if (g_DeviceInput.KeyHold(VK_F6)) bAble = TRUE;
			break;

		case 7:
			if (g_DeviceInput.KeyHold(VK_F7)) bAble = TRUE;
			break;

		case 8:
			if (g_DeviceInput.KeyHold(VK_F8)) bAble = TRUE;
			break;

		case 9:
			if (g_DeviceInput.KeyHold(VK_F9)) bAble = TRUE;
			break;

		case 10:
			if (g_DeviceInput.KeyHold(VK_F10)) bAble = TRUE;
			break;

		case 11:
			if (g_DeviceInput.KeyHold(VK_F11)) bAble = TRUE;//hayzohar  // avalansa was by hayzohar
			break;

		case 12:
			if (g_DeviceInput.KeyHold(VK_F12)) bAble = TRUE;
			break;
	}

	if ( s_pInstance->m_dwFunction && s_pInstance->m_csStatus.m_lpQuickSelected )
	{
		const Skill::ProtoType *lpSkillType = g_SkillMgr.GetSkillProtoType( s_pInstance->m_csStatus.m_lpQuickSelected->GetSkillID());
		if (lpSkillType)
		{
			int count = 1, i = 0;
			for(; i < s_pInstance->m_csStatus.m_PartyInfo.m_cMemberNum; i++)
			{
				if ( s_pInstance->m_csStatus.m_PartyInfo.ServerID[i])
				{
					if ( s_pInstance->m_csStatus.m_PartyInfo.MemberCID[i] != CRYLNetworkData::Instance()->m_dwMyChrID)
					{
						// Ÿ��
						count++;
					}
				}

				if (s_pInstance->m_dwFunction == 1)
				{
					dwFunction = CRYLNetworkData::Instance()->m_dwMyChrID;
					break;
				} else if (count == s_pInstance->m_dwFunction)
				{
					dwFunction = s_pInstance->m_csStatus.m_PartyInfo.MemberCID[i];
					break;
				}
			}

			if (i >= s_pInstance->m_csStatus.m_PartyInfo.m_cMemberNum)
			{
				dwFunction = 0xFFFFFFFF;
				bAble = FALSE;
			}
		} else
		{
			dwFunction = 0xFFFFFFFF;
			bAble = FALSE;
		}
	} else
	{
		dwFunction = 0xFFFFFFFF;
	}

	return bAble;
}

void		CRYLGameData::SetKillCounter(long lKillCounter)
{
	s_pInstance->m_lKillCounter = lKillCounter;
}

void		CRYLGameData::SetRespawnCounter(long lRespawnCounter)
{
	s_pInstance->m_lRespawnCounter = lRespawnCounter;
}


void		CRYLGameData::SetClientExitCounter(long lExitCounter)
{
	s_pInstance->m_lExitCounter = lExitCounter;

	if (0 != lExitCounter)
	{
		CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		if ( pScene )
		{
			pScene->SaveMainWindowPosToReg() ;
		}
	}
}

void		CRYLGameData::SetPortalCounter(long lPortalCounter)
{
	if (lPortalCounter == 0)
	{
		s_pInstance->m_lpPortalScroll = NULL;
		s_pInstance->m_lPortalCounter = lPortalCounter;
	} else if (s_pInstance->m_lPortalCounter == 0)
	{
		s_pInstance->m_lPortalCounter = lPortalCounter;
	}
}
void		CRYLGameData::SetSkillUseCounter(long lSkillUseCounter)
{
	if (lSkillUseCounter == 0)
	{
		s_pInstance->m_wSkillUseID = 0;
		s_pInstance->m_lSkillUseCounter = lSkillUseCounter;
	} else if (s_pInstance->m_lPortalCounter == 0)
	{
		s_pInstance->m_lSkillUseCounter = lSkillUseCounter;
	}
}

void		CRYLGameData::SetAutoRun(BOOL bAutoRun)
{
	s_pInstance->m_bAutoRunMode = bAutoRun;
}

BOOL		CRYLGameData::GetAutoRun(void)
{
	return s_pInstance->m_bAutoRunMode;
}

void		CRYLGameData::ChangeRide(void)
{
	// Minotaurs ���̴� 'p'
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

    if ( !pSelfCreature )
        return;

    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
    if ( !pSelfData )
        return;

	if(pSelfCreature->GetIsDead())
		return;

	// �ƹ�Ÿ �������� ������ ����.
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	if (NULL == pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::AVATA])
		return;
/*
	// 
	char **m_ppShape = const_cast<char **>(pSelfCreature->GetShapeString());
	if(m_ppShape[13] == NULL)
		return;
*/

	// ���� ������ ������ ���� ����
	if(pSelfCreature->GetCharModel() && pSelfCreature->GetCharModel()->GetFixHeight())
	{	
		CRYLGameScene*				pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		if (s_pInstance->m_csStatus.m_cRidePos == 1)
		{
		
			if(s_pInstance->m_csStatus.m_Info.Race == CClass::RaceType::HUMAN)
			{
				CRYLHumanInventoryFrameDlg*	pDlg		= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
				CRYLHumanInventoryPanel*	pPanel		= ( CRYLHumanInventoryPanel* )pDlg->GetHumanInvenPanel() ;

				s_pInstance->m_csStatus.m_cRidePos = 0;
				pPanel->UpdateRideFlag( 0 ) ;
			}
			else if(s_pInstance->m_csStatus.m_Info.Race == CClass::RaceType::AKHAN)
			{
				CRYLHumanInventoryFrameDlg*	pDlg		= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
				CRYLAkhanInventoryPanel*	pPanel		= ( CRYLAkhanInventoryPanel* )pDlg->GetAkhanInvenPanel() ;

				s_pInstance->m_csStatus.m_cRidePos = 0;
				pPanel->UpdateRideFlag( 0 ) ;
			}

			CRYLNetworkData::SendCharSwitchRide();
			CRYLNetworkData::SendCharShape();
			s_pInstance->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

			s_pInstance->m_lpInterface->m_bQuickRideExchange = FALSE;
		}
	}


    CGMUICustomEdit*    pEdit = NULL;
    CGMUIScreen::GetInstance()->GetCurFocusEdit( &pEdit );
    if ( pEdit->GetFocusState() )
        return;

    if ( g_MessageBoxManager.GetIsExistModal() )
        return;

	if (!s_pInstance->m_lpInterface) return;

	if ( ( (g_DeviceInput.KeyDown('P') && !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState()) ||
		s_pInstance->m_lpInterface->m_bQuickRideExchange ) && !pSelfCreature->GetCharModel()->GetFixHeight() )
	{
		// P�� �������� �ɷ��ִ� ������ Ȯ���Ѵ�.
		// ���ڽ� ���¸� �� ��Ÿ�� ���´�.
		if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stealth )) 
			return;	
		
		DWORD dwTickCount = ::GetTickCount();
		if(dwTickCount - pGame->m_dwRideTickCount > 2000)
		{
			pGame->m_dwRideTickCount = dwTickCount;

			CRYLGameScene*				pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;

			// �ڽ��� ���⿡ ž���� ��� Skip.		
			if(!EnemyCheck::IsSiegeArms(pSelfCreature->GetObjectType()) && !EnemyCheck::IsCastleArms(pSelfCreature->GetObjectType()))		
			{
				if (g_CharacterData.GetAttacking())
				{
					if (( g_DeviceInput.KeyDown('P') && !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState()))
					{
						s_pInstance->m_lpInterface->m_bQuickRideExchange = !s_pInstance->m_lpInterface->m_bQuickRideExchange;
						g_DeviceInput.InitKey('P');
					}
				} 
				else
				{
					if(s_pInstance->m_csStatus.m_Info.Race == CClass::RaceType::HUMAN)
					{
						CRYLHumanInventoryFrameDlg*	pDlg		= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLHumanInventoryPanel*	pPanel		= ( CRYLHumanInventoryPanel* )pDlg->GetHumanInvenPanel() ;

						if (s_pInstance->m_csStatus.m_cRidePos == 0)
						{
							s_pInstance->m_csStatus.m_cRidePos = 1;
							//pSelfCreature->SetRideEnable( true ) ;
							pPanel->UpdateRideFlag( 1 ) ;
						}
						else
						{
							s_pInstance->m_csStatus.m_cRidePos = 0;
							//pSelfCreature->SetRideEnable( false ) ;
							pPanel->UpdateRideFlag( 0 ) ;
						}
					}
					else if(s_pInstance->m_csStatus.m_Info.Race == CClass::RaceType::AKHAN)
					{
						CRYLHumanInventoryFrameDlg*	pDlg		= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLAkhanInventoryPanel*	pPanel		= ( CRYLAkhanInventoryPanel* )pDlg->GetAkhanInvenPanel() ;

						if (s_pInstance->m_csStatus.m_cRidePos == 0)
						{
							s_pInstance->m_csStatus.m_cRidePos = 1;
							//pSelfCreature->SetRideEnable( true ) ;
							pPanel->UpdateRideFlag( 1 ) ;
						}
						else
						{
							s_pInstance->m_csStatus.m_cRidePos = 0;
							//pSelfCreature->SetRideEnable( false ) ;
							pPanel->UpdateRideFlag( 0 ) ;
						}
					}

					// ���� ����Ʈ ���
//					if(s_pInstance->m_csStatus.m_cRidePos)
					{
						vector3* vecPos = pSelfCreature->GetPosition();

						CEffScript *pLevelUpEff = new CEffScript;
						CSceneManager::AddEffectScript( pLevelUpEff );
					

						pLevelUpEff->GetScriptBinData( "respawn.esf", pGame->m_bSelfEffect );
						pLevelUpEff->SetStartPos(vecPos->x,vecPos->y,vecPos->z);			

						pLevelUpEff->SetEndPos((D3DXVECTOR3*)vecPos, 1 );
					}

					CRYLNetworkData::SendCharSwitchRide();
					CRYLNetworkData::SendCharShape();
					s_pInstance->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

					s_pInstance->m_lpInterface->m_bQuickRideExchange = FALSE;
				}
			}
		}
	}		
	
	
}

void		CRYLGameData::ChangeWeapon(void)
{
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
    if ( !pSelfCreature )
        return;

    CGMUICustomEdit*    pEdit = NULL;
    CGMUIScreen::GetInstance()->GetCurFocusEdit( &pEdit );
    if ( pEdit && pEdit->GetFocusState() )
        return;

    if ( g_MessageBoxManager.GetIsExistModal() )
        return;

	if (!s_pInstance->m_lpInterface) return;
//	if (s_pInstance->m_csStatus.m_Info.Sex > 2) return;
	if (s_pInstance->m_csStatus.m_Info.Race != CClass::RaceType::HUMAN) return;

	if (( (g_DeviceInput.KeyDown('Q')  || KeyControl::IsKeyState(KeyControl::KEY_COLUM_WEAPONCHANGE)) &&/*[Q Key] for changing weapons hayzohar*/
		s_pInstance->m_lpInterface->m_bWeaponExchageAble && !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState()))
	{
		// �ڽ��� ���⿡ ž���� ��� Skip.		
		if(!EnemyCheck::IsSiegeArms(pSelfCreature->GetObjectType()) && !EnemyCheck::IsCastleArms(pSelfCreature->GetObjectType()))		
		{
			s_pInstance->m_bChangeWeapon = TRUE;
		}		
	}

	if (( (g_DeviceInput.KeyDown('Q')  || KeyControl::IsKeyState(KeyControl::KEY_COLUM_WEAPONCHANGE))/*[Q Key] for changing weapons hayzohar*/
		&& s_pInstance->m_lpInterface->m_bWeaponExchageAble && !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState()) ||
		s_pInstance->m_lpInterface->m_bQuickWeaponExchange)
	{
		CRYLGameScene*				pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLHumanInventoryFrameDlg*	pDlg		= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
		CRYLHumanInventoryPanel*	pPanel		= ( CRYLHumanInventoryPanel* )pDlg->GetHumanInvenPanel() ;

		// �ڽ��� ���⿡ ž���� ��� Skip.		
		if(!EnemyCheck::IsSiegeArms(pSelfCreature->GetObjectType()) && !EnemyCheck::IsCastleArms(pSelfCreature->GetObjectType()))		
		{
			s_pInstance->m_bChangeWeapon = TRUE;
		
			if (g_CharacterData.GetAttacking())
			{
				if (( (g_DeviceInput.KeyDown('Q') || KeyControl::IsKeyState(KeyControl::KEY_COLUM_WEAPONCHANGE)) &&/*[Q Key] for changing weapons hayzohar*/
					s_pInstance->m_lpInterface->m_bWeaponExchageAble && !CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState()))
				{
					s_pInstance->m_lpInterface->m_bQuickWeaponExchange = !s_pInstance->m_lpInterface->m_bQuickWeaponExchange;
					g_DeviceInput.InitKey('Q');/*[Q Key] for changing weapons hayzohar*/
				}
			} 
			else
			{
				if (s_pInstance->m_csStatus.m_cWeaponPos == 0)
				{
					s_pInstance->m_csStatus.m_cWeaponPos = 1;
					pPanel->UpdateWeaponCheckButton( 1 ) ;
				}
				else
				{
					s_pInstance->m_csStatus.m_cWeaponPos = 0;
					pPanel->UpdateWeaponCheckButton( 0 ) ;
				}

				CRYLNetworkData::SendCharSwitchHand();
				CRYLNetworkData::SendCharShape();
				s_pInstance->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

				pSelfCreature->m_ulAttackCombo = 0;
				s_pInstance->m_bChangeWeapon = TRUE;

				s_pInstance->m_lpInterface->m_fWeaponExchageAbleTimeCounter = 3.0f;
				s_pInstance->m_lpInterface->m_bWeaponExchageAble = FALSE;
				s_pInstance->m_lpInterface->m_bQuickWeaponExchange = FALSE;
			}
		}
	}
}

void		CRYLGameData::SetChangeWeapon(BOOL bChangeWeapon)
{
	s_pInstance->m_bChangeWeapon = bChangeWeapon;
}

BOOL		CRYLGameData::GetChangeWeapon(void)
{
	return s_pInstance->m_bChangeWeapon;
}


BOOL		CRYLGameData::CheckAttackable()
{
	BOOL bAttackable = TRUE;

	CItemInstance *lpWeapon = s_pInstance->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + s_pInstance->m_csStatus.m_cWeaponPos];
	CItemInstance *lpShield = s_pInstance->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1 + s_pInstance->m_csStatus.m_cWeaponPos];

	if (lpWeapon)
	{
		if (lpWeapon->GetNowDurability())
		{
			if (lpShield)
			{
				unsigned long dwType = lpShield->GetItemType();
				if (dwType == Item::ItemType::ONEHANDED_SWORD || dwType == Item::ItemType::ONEHANDED_BLUNT || dwType == Item::ItemType::DAGGER || dwType == Item::ItemType::ONEHANDED_AXE)
				{
					if (lpShield->GetNowDurability() == 0)
					{
						bAttackable = FALSE;
					}
				}
			}
		} else
		{
			bAttackable = FALSE;
		}
	}

	return bAttackable;
}

vector3		CRYLGameData::GetMonsterMinBox(unsigned long dwChrID)
{
	const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType(dwChrID & 0x0000FFFF);

	vector3 vecMinBox = vector3(-35.0f, 0.0f, -55.0f);

	if (lpMonster)
	{
		vecMinBox = vector3(lpMonster->m_MonsterInfo.m_fHitBox[1], 0.0f, lpMonster->m_MonsterInfo.m_fHitBox[3]);
	}

	return vecMinBox;
}

vector3		CRYLGameData::GetMonsterMaxBox(unsigned long dwChrID)
{
	const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType(dwChrID & 0x0000FFFF);

	vector3 vecMaxBox = vector3(35.0f, 0.0f, 55.0f);

	if (lpMonster)
	{
		vecMaxBox = vector3(lpMonster->m_MonsterInfo.m_fHitBox[0], 0.0f, lpMonster->m_MonsterInfo.m_fHitBox[2]);
	}

	return vecMaxBox;
}


unsigned long 
// PC���� NPC���� MONSTER���� ��ȯ�� ���� �ǹ� ����
CRYLGameData::GetClientIdentity( unsigned long dwChrID )
{
	if ( dwChrID & Creature::SIEGE_OBJECT_BIT )
	{
		return Creature::CT_SIEGE_OBJECT;
	}

	if (dwChrID & Creature::MONSTER_BIT)
	{
		if (Creature::SUMMON_MONSTER_BIT == (dwChrID & Creature::SUMMON_MONSTER_BIT))
		{
			return Creature::CT_SUMMON;
		}

		const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType(dwChrID & Creature::MONSTER_KIND_BIT);
		if (NULL != lpMonster)
		{
			// ���� �� ���
			if (lpMonster->m_MonsterInfo.m_cSkillPattern == MonsterInfo::MonsterPattern::PATTERN_STRUCTURE)		
			{
				return Creature::CT_STRUCT;
			}
		}

		return Creature::CT_MONSTER;
	}

	if (dwChrID & Creature::NPC_BIT)
	{
		return Creature::CT_NPC;
	}

	return Creature::CT_PC;
}

bool CRYLGameData::IsInSafetyZone( RYLCreature* pCreature )
{

	// ��������� ���ؼ�.. ����.
	// edith �ʺ����� ������ ������Ƽ������ �����մϴ�.
	if (SERVER_ID::ZONE12 == CRYLNetworkData::Instance()->m_dwPresentZone ) 
	{
		return true;
	}

	return pCreature->IsInSafetyZone();
}

bool CRYLGameData::CanUseSkill( RYLCreature* pCreature, Skill::Target::Type eTargetType )
{
	// ���翩�ο� ���� ������ ��ų�� �޶�����.
	if (Skill::Target::DEAD_ENEMY == eTargetType || Skill::Target::DEAD_FRIEND == eTargetType)
	{
        if ( !pCreature->GetIsDead() ) { return false; }
	}
	else
	{
		if ( pCreature->GetIsDead() ) { return false; }
	}

	//����� NPC���� �� �� �ִ� ��ų�� ����. by Hades Kang
    if ( Creature::CT_NPC == CRYLGameData::GetClientIdentity( pCreature->GetCharID() ) )
		return false;

	// �ڱ��ڽſ��� ������� ���ϴ� ��ų
	if (Skill::Target::FRIEND_EXCEPT_SELF == eTargetType && 
        pCreature->GetCharID() == CRYLNetworkData::Instance()->m_dwMyChrID )
	{
		return false;
	}

	switch (eTargetType)
	{
		case Skill::Target::MELEE :				
		case Skill::Target::ENEMY :				
		case Skill::Target::DEAD_ENEMY :		
		case Skill::Target::ENEMY_OBJECT:		
		case Skill::Target::LINE_ENEMY:
		{
			return (EnemyCheck::EC_ENEMY != CRYLGameData::GetClientEnemyType( pCreature->GetCharID() )) ? false : true;
		}

		case Skill::Target::FRIEND :			
		case Skill::Target::FRIEND_EXCEPT_SELF:	
		case Skill::Target::DEAD_FRIEND :		
		case Skill::Target::FRIEND_OBJECT :		
		{
			unsigned long dwEnemyResult = CRYLGameData::GetClientEnemyType( pCreature->GetCharID() ) ;
			if ( EnemyCheck::EC_FRIEND == dwEnemyResult )
			{
				return true ;
			}

			return false ;
		}
	}

	return true;
}

Skill::Type::SkillType CRYLGameData::GetAlterSkillType(void)
{
	// �ڽ� ��ų�� ������ �ڽ� ��ų��, ������ �θ� ��ų�� �����Ѵ�. 
	const Skill::ProtoType* lpSkillBase = 
		g_SkillMgr.GetSkillProtoType(m_csStatus.m_lpQuickSelected->GetSkillID());

	return lpSkillBase->m_eSkillType;
}

bool CRYLGameData::GetAlterSkillEnable(void)
{
	if (NULL != m_csStatus.m_lpQuickSelected)
	{
		const Skill::ProtoType* lpSkillBase = 
			g_SkillMgr.GetSkillProtoType(m_csStatus.m_lpQuickSelected->m_lpItemBase->GetPrototypeID());
		if (NULL != lpSkillBase)
		{
			if ( 0 != lpSkillBase->m_usChildSkill)
			{
				return true;
			}
		}
	}

	return false;
}

// �Ǿƽĺ�( friend/neutral/enemy )
unsigned long CRYLGameData::GetClientEnemyType( unsigned long dwChrID)
{
	CRYLGameData* pGame = CRYLGameData::Instance();

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )	
	{
		pGame->SetEnemyTypeResult(43);
		return EnemyCheck::EC_NEUTRAL;
	}

    CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
    if ( !pSelfData )
	{
		pGame->SetEnemyTypeResult(43);
		return EnemyCheck::EC_NEUTRAL;
	}
			
    RYLCreature* pRivalCreature = RYLCreatureManager::Instance()->GetCreature( dwChrID );
    if ( !pRivalCreature )
	{
		return EnemyCheck::EC_NEUTRAL;
	}

    CharDataNode* pRivalData = RYLCharacterDataManager::Instance()->GetCharData( pRivalCreature->GetCharID() );
    if ( !pRivalData )
	{
		return EnemyCheck::EC_NEUTRAL;
	}

    unsigned long ulMyType			= GetClientIdentity( pSelfCreature->GetCharID() );
    unsigned long ulRivalType		= GetClientIdentity( dwChrID );
	unsigned char cEnemyTypeResult	= 0;

    // ��ȯ���� ��ȯ���� �����ͷ� üũ��
    if ( ulRivalType == Creature::CT_SUMMON )
    {
        pRivalCreature = RYLCreatureManager::Instance()->GetCreature( pRivalData->m_ulSummonMasterCharID );
        if ( !pRivalCreature )
		{
			return EnemyCheck::EC_NEUTRAL;
		}

        pRivalData = RYLCharacterDataManager::Instance()->GetCharData( pRivalCreature->GetCharID() );
        if ( !pRivalData )
		{
			return EnemyCheck::EC_NEUTRAL;
		}
    }

	if ( pSelfCreature->GetCharID() == dwChrID ) // �ڽ��� ĳ���̶��
	{
		return EnemyCheck::EC_FRIEND;
	}

	return GetClientEnemyTypePC( pRivalCreature, pRivalData );
}

bool CRYLGameData::IsPeaceMode(const GUILDLARGEINFOEX* pGuildInfo, unsigned char& ucNation)
{
	// MichaelBless �Ͻ� �ǽ����� ����
	// ���� ������ ������ ��ȭ ���
	if (SERVER_ID::ZONE1 == CRYLNetworkData::Instance()->m_dwPresentZone)//(SERVER_ID::ZONE12 == CRYLNetworkData::Instance()->m_dwPresentZone) // was CAPITAL
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		CRYLGameData* pCharacter = CRYLGameData::Instance() ;
		if(pCharacter->m_csStatus.GetLevel() < 51)
		{
		if (pSelfCreature &&  pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::MichaelBless )) 
			return true;	
		}
	}

/*	// ���� ������ ������ ��ȭ ���
	if (SERVER_ID::ZONE12 == CRYLNetworkData::Instance()->m_dwPresentZone)
	{
		return true;
	}
*/
/*
	if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
	{
		// ī������ �ִ��� ����� �ð� (���� ���� �ð�)�� �ƴ϶�� ��ȭ ���
		if (SERVER_ID::CAPITAL == CRYLNetworkData::Instance()->m_dwPresentZone && 
			!CRYLGameTimeManager::Instance()->IsGuildWarTime())
		{
			return true;
		}
	}
*/
	return false;
}

// �Ǿƽĺ�( friend/neutral/enemy )
unsigned long CRYLGameData::GetClientEnemyTypePC(RYLCreature* pRivalCreature, CharDataNode* pRivalData)
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );

	BattleInclination::CharData     ownerInfo;
	BattleInclination::CharData     targetInfo;

	BattleInclination::SetCharData(pSelfCreature, pSelfData, ownerInfo);
	BattleInclination::SetCharData(pRivalCreature, pRivalData, targetInfo);

	BattleInclination::RelationData	relationInfo;
	BattleInclination::SetRelationData(pSelfCreature, pRivalCreature, relationInfo);

	unsigned char cResultFromStruct = 0;
	unsigned long dwResult = EnemyCheck::CCheckTable::GetInstance().IsEnemyFromStruct(
		ownerInfo, targetInfo, relationInfo, cResultFromStruct);

	s_pInstance->SetEnemyTypeResult(cResultFromStruct);
	return dwResult;
}

VOID	CRYLGameData::InitTipBroad( char* pszFilePath )
{
	char strFilePath[MAX_PATH], *strTipBroad ;
	sprintf(strFilePath, "%s\\Tip.dat", pszFilePath ) ;

	FILE* fp = fopen(strFilePath,"r");

	if (fp)
	{
		for(;;)
		{
			strTipBroad = new char[ MAX_PATH ] ;
			if (fgets(strTipBroad, MAX_PATH, fp))
			{
				m_lstTipBroad.push_back(strTipBroad);
			} else
			{
				delete[] strTipBroad;
				break;
			}
		}
		fclose(fp);
	}
}

VOID	CRYLGameData::DestroyTipBroad()
{
	if (m_lstTipBroad.size())
	{
		vector<char *>::iterator it = m_lstTipBroad.begin();

		while( it != m_lstTipBroad.end() ) 
		{
			GM_DELETE_ARRAY( (*it ) ) ;
			++ it ; 
		}

		m_lstTipBroad.clear() ;
	}
}

VOID	CRYLGameData::UpdateQuickChatTimer( FLOAT fUpdate )
{
	if ( m_lQuickChatTimer )
	{
		m_lQuickChatTimer -= fUpdate * 2 ;
		if ( m_lQuickChatTimer < 0 ) 
			m_lQuickChatTimer = 0 ;
	}
}

VOID	CRYLGameData::UpdateTipBroadCounter( FLOAT fUpdate )
{
	if ( m_coOption.m_bTipBroad && m_lTipBroadCounter )
	{
		m_lTipBroadCounter -= fUpdate * 2 ;
		if ( m_lTipBroadCounter <= 0 )
		{
			SetTipBroadTimer( m_lTipBroadCounterSeed ) ;
			char strMessage[MAX_PATH] ;
			sprintf( strMessage, "Tip : %s", m_lstTipBroad[ rand() % m_lstTipBroad.size() ] ) ;
			CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::TIP, NOTICETIME::NORMAL ) ;
		}
	}
}

VOID	CRYLGameData::UpdatePartyAttackTimer( FLOAT fUpdate )
{
	for ( int i = 0; i < PARTY::MAX_MEM; i++ )
	{
		if ( m_csStatus.m_aryPartyExtraInfo[ i ].m_lPartyAttackTimer )
		{
			m_csStatus.m_aryPartyExtraInfo[ i ].m_lPartyAttackTimer -= fUpdate * 2 ;
			if ( m_csStatus.m_aryPartyExtraInfo[ i ].m_lPartyAttackTimer < 0) 
				m_csStatus.m_aryPartyExtraInfo[ i ].m_lPartyAttackTimer = 0 ;
		}
	}
}

VOID	CRYLGameData::UpdateSkillUseCounter( FLOAT fUpdate )
{
	if ( m_lSkillUseCounter )
	{
		static long lSkillUsePrintCounter = 0 ;
		m_lSkillUseCounter -= fUpdate * 2 ;

		if ( m_lSkillUseCounter <= 0 )
		{
			// edith 2009.08.05 Ÿ�̸� �̻� ���� Ȯ�ο� ��·���
//			char strText[ MAX_PATH ] ;
//			sprintf( strText, CRYLStringTable::m_strString[ 907 ], 0 ) ;
//			CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;

			// ��ų���
			CRYLGameData::SetSkillUseCounter( 0 ) ;
		} 
		else
		{
			if ( lSkillUsePrintCounter != m_lSkillUseCounter / 1000 )
			{
				lSkillUsePrintCounter = m_lSkillUseCounter / 1000 ;

				if ( lSkillUsePrintCounter )
				{
					char strText[ MAX_PATH ] ;

					sprintf( strText, CRYLStringTable::m_strString[ 907 ], lSkillUsePrintCounter ) ;
					CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}
		}
	}
}

VOID	CRYLGameData::UpdatePortalCounter( FLOAT fUpdate )
{
	if ( m_lPortalCounter )
	{
		static long lPortalPrintCounter = 0 ;
		m_lPortalCounter -= fUpdate * 2 ;

		if ( m_lPortalCounter <= 0 )
		{
			if ( m_lpPortalScroll && m_csStatus.GetIsInventoryItem( m_lpPortalScroll ) )
			{
				// edith 2009.08.05 Ÿ�̸� �̻� ���� Ȯ�ο� ��·���
//				char strText[ MAX_PATH ] ;
//				sprintf( strText, CRYLStringTable::m_strString[ 903 ], 0 ) ;
//				CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;

				g_pClientSocket->SetStatusFlag( NS_USEITEM ) ;
				SendPacket::CharUseItem( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, CRYLNetworkData::Instance()->m_dwMyChrID, &m_lpPortalScroll->m_lpItemBase->GetPos() ) ;
			}
			
			CRYLGameData::SetPortalCounter( 0 ) ;
		} 
		else
		{
			if ( lPortalPrintCounter != m_lPortalCounter / 1000 )
			{
				lPortalPrintCounter = m_lPortalCounter / 1000 ;
				
				if ( lPortalPrintCounter )
				{
					char strText[ MAX_PATH ] ;

					sprintf( strText, CRYLStringTable::m_strString[ 903 ], lPortalPrintCounter ) ;
					CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}
		}
	}
}

VOID	CRYLGameData::UpdateKillCounter( FLOAT fUpdate )
{
	if ( m_lKillCounter )
	{
		static long lPrintCounter = 0 ;
		m_lKillCounter -= fUpdate * 2 ;

		if ( m_lKillCounter <= 0 )
		{
			// edith 2009.08.05 Ÿ�̸� �̻� ���� Ȯ�ο� ��·���
//			char strText[ MAX_PATH ] ;
//			sprintf( strText, CRYLStringTable::m_strString[ 900 ], 0 ) ;
//			CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;

			CRYLNetworkData::Instance()->SetNetworkFlag( NF_SUICIDE ) ; 
			SendPacket::CharSuicide( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID ) ;
			CRYLGameData::SetKillCounter( 0 ) ;
		} 
		else
		{
			if ( lPrintCounter != m_lKillCounter / 1000 )
			{
				lPrintCounter = m_lKillCounter / 1000 ;
				
				if ( lPrintCounter )
				{
					char strText[ MAX_PATH ] ;
					sprintf( strText, CRYLStringTable::m_strString[ 900 ], lPrintCounter ) ;
					CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}
		}
	}
}
/*
VOID	CRYLGameData::UpdateRespawnCounter( FLOAT fUpdate )
{
	if ( m_lRespawnCounter )
	{
		static long lPrintCounter = 0 ;
		m_lRespawnCounter -= fUpdate * 2 ;

		if ( m_lRespawnCounter <= 0 )
		{
			// edith 2009.08.05 Ÿ�̸� �̻� ���� Ȯ�ο� ��·���
//			char strText[ MAX_PATH ] ;
//			sprintf( strText, CRYLStringTable::m_strString[ 882 ], 0 ) ;
//			CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;

		// edith 2009.06.13 1Min Over Auto Respawn  
			g_CharacterData.CharRespawn() ;

			CRYLGameData::SetRespawnCounter( 0 ) ;
		} 
		else
		{
			if ( lPrintCounter != m_lRespawnCounter / 1000 )
			{
				lPrintCounter = m_lRespawnCounter / 1000 ;
				if(lPrintCounter % 10 == 0)
				{
					if ( lPrintCounter )
					
					{
						char strText[ MAX_PATH ] ;
						sprintf( strText, CRYLStringTable::m_strString[ 882 ], lPrintCounter ) ;
						CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					}
				}
			}
		}
	}
}
*/

VOID	CRYLGameData::UpdateClientExitCounter( FLOAT fUpdate )
{
	if ( m_lExitCounter )
	{
		static long lPrintCounter = 0 ;
		m_lExitCounter -= fUpdate * 2 ;

		if ( m_lExitCounter <= 0 )
		{
			// edith 2009.08.05 Ÿ�̸� �̻� ���� Ȯ�ο� ��·���
//			char strText[ MAX_PATH ] ;
//			sprintf( strText, CRYLStringTable::m_strString[ 2778 ], 0 ) ;
//			CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;

			PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
			m_dwClientMode = CLIENT_END;
			return;
		} 
		else
		{
			if ( lPrintCounter != m_lExitCounter / 1000 )
			{
				lPrintCounter = m_lExitCounter / 1000 ;

				if ( lPrintCounter )
				{
					char strText[ MAX_PATH ] ;
					sprintf( strText, CRYLStringTable::m_strString[2778], lPrintCounter ) ;
					CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}
		}
	}
}

void	CRYLGameData::SetLogoutCounter(long lLogoutCounter) 
{
	s_pInstance->m_lLogoutCounter = lLogoutCounter;
	s_pInstance->m_bLogoutEnable = ( 0 == lLogoutCounter ) ? false : true ;
}

VOID	CRYLGameData::UpdateLogoutCounter( FLOAT fUpdate ) //hayzohar: this function is called when you trying to go to the character selection screen 
{
	if ( m_lLogoutCounter )
	{
		m_lLogoutCounter -= fUpdate * 2;

		if ( m_lLogoutCounter <= 0 )
		{
			if ( m_bLogoutEnable )
			{
				// edith 2009.08.05 Ÿ�̸� �̻� ���� Ȯ�ο� ��·���
				//char strText[ MAX_PATH ] ;
				//sprintf( strText, CRYLStringTable::m_strString[ 2943 ], 0 ) ;
				//CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;

				ClearGameDlgData() ;

				// ĳ���� ����ȭ������ ���ư���
				MoveCharSelectScene() ;//hayzohar: something wrong here
			}

			CRYLGameData::SetLogoutCounter( 0 ) ;
		} 
		else
		{
			if ( m_lLogoutTimeResult != m_lLogoutCounter / 1000 )
			{
				m_lLogoutTimeResult = m_lLogoutCounter / 1000 ;
				
				if ( m_lLogoutTimeResult )
				{
					char strText[ MAX_PATH ] ;
					sprintf( strText, CRYLStringTable::m_strString[ 2943 ], m_lLogoutTimeResult ) ;
					CRYLChattingDlg::Instance()->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}
		}
	}
}

VOID	CRYLGameData::UpdatePremiumTimer( FLOAT fUpdate ) 
{
	if(m_iPremiumType == 0)
		return;
		
	if(m_lPlayPremiumTimer)
	{
		m_lPlayPremiumTimer -= fUpdate*2;

		if(m_lPlayPremiumTimer <= 0)
		{
			// 1�д� 1�� ����Ѵ�.
			m_lPlayPremiumTimer = 60000;
			m_lPremiumTime-- ;

			if (m_lPremiumTime <= 10 && m_lPremiumTime > 0)
			{
				char strText[ MAX_PATH ] ;
				sprintf( strText, CRYLStringTable::m_strString[ 930 ], m_lPremiumTime ) ;
				CRYLNoticeManager::Instance()->AddNotice( strText, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;
			} 
			else if(m_lPremiumTime <= 0)
			{
				CRYLNoticeManager::Instance()->AddNotice( CRYLStringTable::m_strString[ 931 ], NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;
				m_iPremiumType = 0;
				m_lPremiumTime = 0 ;
			}
		}
	}

}

VOID	CRYLGameData::UpdateGameTimer( FLOAT fUpdate )
{
	if ( CRYLNetworkData::Instance()->m_eInternationalCode != GameRYL::CHINA || m_dwPlayTime == 0 )
		return;

	if(m_lPlayTimer)
	{
		m_lPlayTimer -= fUpdate*2;

		if(m_lPlayTimer <= 0)
		{
			m_lPlayTimer = 60000;

			CTime		nowTime = CTime::GetCurrentTime();
			CTimeSpan	tm(m_StartTime.GetDay(), m_StartTime.GetHour(), m_StartTime.GetMinute(), m_StartTime.GetSecond());    
			CTime		tmPrev = nowTime-tm;

			int			iTime = (tmPrev.GetHour()*60)+tmPrev.GetMinute();

			iTime += m_dwPlayTime;

			if(iTime == 0)
				return;

			// 3�ð��϶�, 5�ð��϶�
			if( m_dwNewTime < 180 && 180 <= iTime) 
			{
				char strText[MAX_PATH];

				sprintf(strText, CRYLStringTable::m_strString[ 4254], iTime);
				CRYLNoticeManager::Instance()->AddNotice( strText, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;
				CRYLNoticeManager::Instance()->AddNotice( CRYLStringTable::m_strString[ 4255], NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;

				/////////////////////////////////////////////////////////////////////////////////////
				// ä�� �޽����� �޽��� �Ѹ���
//				sprintf( strText, CRYLStringTable::m_strString[ 4240 ], iTime ) ;
				CRYLChattingDlg* pChat = CRYLChattingDlg::Instance();
				if(pChat)
				{
					pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					pChat->AddMessage(CRYLStringTable::m_strString[ 4255 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				}
				/////////////////////////////////////////////////////////////////////////////////////
			}
			else if(m_dwNewTime < 300 && 300 <= iTime)
			{
				char strText[MAX_PATH];

				sprintf(strText, CRYLStringTable::m_strString[4256], iTime);
				CRYLNoticeManager::Instance()->AddNotice( strText, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;
				CRYLNoticeManager::Instance()->AddNotice( CRYLStringTable::m_strString[4257], NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;

				/////////////////////////////////////////////////////////////////////////////////////
				// ä�� �޽����� �޽��� �Ѹ���
//				sprintf( strText, CRYLStringTable::m_strString[ 4242 ], iTime ) ;
				CRYLChattingDlg* pChat = CRYLChattingDlg::Instance();
				if(pChat)
				{
					pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					pChat->AddMessage(CRYLStringTable::m_strString[ 4257 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				}
				/////////////////////////////////////////////////////////////////////////////////////
			}
			else if(m_dwNewTime <= 180)
			{
				if(m_dwNewTime/60 != iTime/60)	// �̰� 60�п� �ѹ���
				{
					char strText[MAX_PATH];

					sprintf(strText, CRYLStringTable::m_strString[4253], iTime);
					CRYLNoticeManager::Instance()->AddNotice( strText, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;

					/////////////////////////////////////////////////////////////////////////////////////
					// ä�� �޽����� �޽��� �Ѹ���
//					sprintf( strText, CRYLStringTable::m_strString[ 4253 ], iTime ) ;
					CRYLChattingDlg* pChat = CRYLChattingDlg::Instance();
					if(pChat)
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					/////////////////////////////////////////////////////////////////////////////////////
				}
			}
			else if(m_dwNewTime <= 300)
			{
				if(m_dwNewTime/30 != iTime/30)	// �̰� 30�п� �ѹ���
				{
					char strText[MAX_PATH];

					sprintf(strText, CRYLStringTable::m_strString[4253], iTime);
					CRYLNoticeManager::Instance()->AddNotice( strText, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;

					/////////////////////////////////////////////////////////////////////////////////////
					// ä�� �޽����� �޽��� �Ѹ���
//					sprintf( strText, CRYLStringTable::m_strString[ 4253 ], iTime ) ;
					CRYLChattingDlg* pChat = CRYLChattingDlg::Instance();
					if(pChat)
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					/////////////////////////////////////////////////////////////////////////////////////
				}
			}
			else if(m_dwNewTime > 300)
			{
				if(m_dwNewTime/15 != iTime/15)	// �̰� 15�п� �ѹ���
				{
					char strText[MAX_PATH];

					sprintf(strText, CRYLStringTable::m_strString[4253], iTime);
					CRYLNoticeManager::Instance()->AddNotice( strText, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;

					/////////////////////////////////////////////////////////////////////////////////////
					// ä�� �޽����� �޽��� �Ѹ���
//					sprintf( strText, CRYLStringTable::m_strString[ 4253 ], iTime ) ;
					CRYLChattingDlg* pChat = CRYLChattingDlg::Instance();
					if(pChat)
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					/////////////////////////////////////////////////////////////////////////////////////
				}
			}

			m_dwNewTime = iTime;
		}
	}
}

VOID	CRYLGameData::UpdateBillingTimer( FLOAT fUpdate )
{
	if ( m_lBillingTimer )
	{
		m_lBillingTimer -= fUpdate * 2 ;

		if ( m_lBillingTimer <= 0 )
		{
			m_lBillingTimer = 60000 ;
			m_nPlayTime-- ;

			if ( m_nPlayTime )
			{
				char strText[ MAX_PATH ] ;
				sprintf( strText, CRYLStringTable::m_strString[ 908 ], m_nPlayTime ) ;
				CRYLNoticeManager::Instance()->AddNotice( strText, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;
			} 
			else
			{
				CRYLNoticeManager::Instance()->AddNotice( CRYLStringTable::m_strString[ 909 ], NOTICECOLOR::NORMAL, NOTICETIME::NORMAL ) ;
				m_lBillingTimer = 0 ;
			}
		}
	}
}

void	CRYLGameData::SetTipBroadTimer(long lTimer)
{
	m_lTipBroadCounter = lTimer;
}

BOOL	CRYLGameData::LoadClientOption(const char *strFileName)
{
	m_coOption.m_bTipBroad = TRUE;
	m_coOption.m_bChat = TRUE;
	m_coOption.m_dwCharName = 15;
	m_coOption.m_dwSkillEffect = 15;
	m_coOption.m_dwChantEffect = 255;
	m_coOption.m_dwEquipEffect = 1;
	m_coOption.m_dwMasterSound = 0 ;
	m_coOption.m_dwEffectSound = 0 ;
	m_coOption.m_dwBGMSound = 0 ;
	m_coOption.m_fMasterVolume = 1.0f;
	m_coOption.m_fAmbVolume = 1.0f;
	m_coOption.m_fBgmVolume = 1.0f;

	FILE *fp = fopen(strFileName, "rb");
	if (fp)
	{
		if (fread(&m_coOption, sizeof(m_coOption), 1, fp) == 1)
		{
			fclose(fp);

			m_bSelfEffect = false;
			m_bPartyEffect = false;
			m_bEveryEffect = false;

			m_bFriendEffect = false ;
			m_bNeutralEffect = false ;
			m_bEnemyEffect = false ;

			if ( (m_coOption.m_dwSkillEffect & 4) && (m_coOption.m_dwSkillEffect & 8) )
			{
				m_bEveryEffect = true;
			}

			if ( m_coOption.m_dwSkillEffect & 1 )
			{
				m_bSelfEffect = true;
			}

			if ( m_coOption.m_dwSkillEffect & 2 )
			{
				m_bPartyEffect = true;
				m_bFriendEffect = true;
			}

			if ( m_coOption.m_dwSkillEffect & 4 )
			{
				m_bNeutralEffect = true;
			}

			if ( m_coOption.m_dwSkillEffect & 8 )
			{
				m_bEnemyEffect = true;
			}

			int		option;
			int		i;			
			option = 1;

			for( i = 0 ; i < 4; i++ )
			{
				if ( m_coOption.m_dwChantEffect & option )
					g_CharacterData.m_bGoodChantEffectShow[i] = true;
				else
					g_CharacterData.m_bGoodChantEffectShow[i] = false;
				option = 1<<(i+1);
			}

			for( i = 0 ; i < 4; i++ )
			{
				if ( m_coOption.m_dwChantEffect & option )
					g_CharacterData.m_bBadChantEffectShow[i] = true;
				else
					g_CharacterData.m_bBadChantEffectShow[i] = false;
				option = 1<<(i+4);
			}			 

			return TRUE;
		}

		fclose(fp);
	}

	CSoundMgr::_GetInstance()->SetVolume(m_coOption.m_fMasterVolume); 
	CBgmManager::_GetInstance()->SetVolume(m_coOption.m_fBgmVolume); 
	CAmbienceManager::_GetInstance()->SetVolume(m_coOption.m_fAmbVolume); 

	SaveClientOption(strFileName, &m_coOption);

	return FALSE;
}

BOOL	CRYLGameData::SaveClientOption(const char* strFileName, ClientOption* coOption)
{
	m_coOption.m_bTipBroad = coOption->m_bTipBroad;
	m_coOption.m_bChat = coOption->m_bChat;
	m_coOption.m_dwCharName = coOption->m_dwCharName;
	m_coOption.m_dwSkillEffect = coOption->m_dwSkillEffect;
	m_coOption.m_dwChantEffect = coOption->m_dwChantEffect;
	m_coOption.m_dwEquipEffect = coOption->m_dwEquipEffect;
	
	m_coOption.m_dwMasterSound = coOption->m_dwMasterSound ;
	m_coOption.m_dwEffectSound = coOption->m_dwEffectSound ;
	m_coOption.m_dwBGMSound = coOption->m_dwBGMSound ;
	m_coOption.m_fMasterVolume	= coOption->m_fMasterVolume;
	m_coOption.m_fAmbVolume	= coOption->m_fAmbVolume;
	m_coOption.m_fBgmVolume = coOption->m_fBgmVolume;

	m_bSelfEffect = false;
	m_bPartyEffect = false;
	m_bEveryEffect = false;

	m_bFriendEffect = false ;
	m_bNeutralEffect = false ;
	m_bEnemyEffect = false ;

	if ( (m_coOption.m_dwSkillEffect & 4) && (m_coOption.m_dwSkillEffect & 8) )
	{
		m_bEveryEffect = true;
	}

	if ( m_coOption.m_dwSkillEffect & 1 )
	{
		m_bSelfEffect = true;
	}

	if ( m_coOption.m_dwSkillEffect & 2 )
	{
		m_bPartyEffect = true;
		m_bFriendEffect = true;
	}

	if ( m_coOption.m_dwSkillEffect & 4 )
	{
		m_bNeutralEffect = true ;
	}

	if ( m_coOption.m_dwSkillEffect & 8 )
	{
		m_bEnemyEffect = true ;
	}

	int		option;
	int		i;			
	option = 1;

	for( i = 0 ; i < 4; i++ )
	{
		if ( m_coOption.m_dwChantEffect & option )
			g_CharacterData.m_bGoodChantEffectShow[i] = true;
		else
			g_CharacterData.m_bGoodChantEffectShow[i] = false;
		option = 1<<(i+1);
	}

	for( i = 0 ; i < 4; i++ )
	{
		if ( m_coOption.m_dwChantEffect & option )
			g_CharacterData.m_bBadChantEffectShow[i] = true;
		else
			g_CharacterData.m_bBadChantEffectShow[i] = false;
		option = 1<<(i+4);
	}
			
	FILE *fp = fopen(strFileName, "wb");
	if (fp)
	{
		if (fwrite(&m_coOption, sizeof(m_coOption), 1, fp) == 1)
		{
			fclose(fp);
			return TRUE;
		}

		fclose(fp);
	}

	return FALSE;
}

VOID		CRYLGameData::UpdateFrameTimer() 
{
	if ( CFrameTimer::GetUpdateTimer( GRYLTimer.GetCursorTimer() ) >= 1.0f )  
		m_bCursorToggle = !m_bCursorToggle ;

	if ( CFrameTimer::GetUpdateTimer( GRYLTimer.GetSpeedHackCheckTimerID() ) >= 1.0f )//was 1.0f
	{
		if ( GetSpeedHack() )//hz was not commented out hz speedhack
		{
			MessageBox( 0, "Hack Detected!","Hack Detected!"/*CRYLStringTable::m_strString[ 23 ], CRYLStringTable::m_strString[ 21 ]*/, MB_OK ) ;
			SendMessage( g_ClientMain.m_hWnd, WM_DESTROY, 0, 0 ) ;
			m_dwClientMode = CLIENT_END ;
			return ;
		}
	}
}

BOOL		CRYLGameData::GetSpeedHack()
{
	// edith 2008.05.15 ���ǵ��� �������
	if (FindWindow("#32770", "���ǵ��� Ver 1.0 "))				return TRUE ;
	if (FindWindow("#32770", "Speed Hack - PCGameHacks.com"))	return TRUE ;
	if (FindWindow("#32770", "                            "))	return TRUE ;
	if (FindWindow("#32770", "��������ʥ����"))					return TRUE ;
	if (FindWindow("#32770", "Brothers Speeder"))				return TRUE ;
	if (FindWindow("#32770", "cheatengine-x86_64.exe"))				return TRUE ;//hz added this for speed hack

	return FALSE;
}

void		CRYLGameData::SetSkillCoolDown(unsigned short wGrade, unsigned char cEndCooldown)
{
	unsigned short wSkill = m_csStatus.m_lpQuickSelected->GetSkillID();

	const Skill::ProtoType *lpSkillBase    = g_SkillMgr.GetSkillProtoType( wSkill ) ;
	unsigned short usChildID = 0, usParentID = 0 ;
	if ( lpSkillBase )
	{
		usChildID  = lpSkillBase->m_usChildSkill ;
		usParentID = lpSkillBase->m_usParentSkill ;
	}

	int i;
	if (m_csStatus.m_lpQuickSelected->m_bClassSkill)
	{
		vector<CItemInstance *>::iterator it;
		for(it = m_csStatus.m_lstClassSkill.begin(); it != m_csStatus.m_lstClassSkill.end(); it++)
		{
			if ((*it)->GetProtoTypeID() == wSkill)
			{
				(*it)->StartCoolDown(wGrade, cEndCooldown);
			}
		}
	} 
	else
	{
		for(i = 0; i < SKILL::MAX_SLOT_NUM; i++)
		{
			if (m_csStatus.m_lstSkillSlot[i])
			{
				if (m_csStatus.m_lstSkillSlot[i]->GetProtoTypeID() == wSkill)
				{
					m_csStatus.m_lstSkillSlot[i]->StartCoolDown(wGrade, cEndCooldown);
				}
				else if (m_csStatus.m_lstSkillSlot[i]->GetProtoTypeID() == usChildID ||
						 m_csStatus.m_lstSkillSlot[i]->GetProtoTypeID() == usParentID)
				{
					m_csStatus.m_lstSkillSlot[i]->StartCoolDown(wGrade, cEndCooldown, wSkill);
				}
			}
		}
	}

	// �Ҽȸ�Ǹ���ƮȮ��
	if(!m_csStatus.m_lstSocialSlot.empty())
	{
		int iSize = m_csStatus.m_lstSocialSlot.size();
		for(i = 0; i < iSize; ++i)
		{
			if(m_csStatus.m_lstSocialSlot[i]->GetProtoTypeID() == wSkill )
			{
				m_csStatus.m_lstSocialSlot[i]->StartCoolDown(wGrade, cEndCooldown);
			}
		}
	}

	for(i = 0; i < QUICK::MAX_QUICK_NUM; i++)
	{
		if (m_csStatus.m_lstQuickSlot[i])
		{
			if ( m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == wSkill )
			{
				m_csStatus.m_lstQuickSlot[i]->StartCoolDown(wGrade, cEndCooldown);
			}
			else if ( m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == usChildID ||
				      m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == usParentID )
			{
				m_csStatus.m_lstQuickSlot[i]->StartCoolDown(wGrade, cEndCooldown, wSkill);
			}
		}
	}

	if (m_csStatus.m_lpQuickSelected)
	{
		m_csStatus.m_lpQuickSelected->StartCoolDown(wGrade, cEndCooldown);
	}

	if (m_lpPickItem)
	{
		if (m_lpPickItem->m_bSkill && m_lpPickItem->GetProtoTypeID() == wSkill)
		{
			m_lpPickItem->StartCoolDown(wGrade, cEndCooldown);
		}
	}
}

void	CRYLGameData::SetSkillCoolDown( unsigned short wSkill, unsigned short wGrade, unsigned char cEndCooldown ) 
{
	const Skill::ProtoType *lpSkillBase    = g_SkillMgr.GetSkillProtoType( wSkill ) ;
	unsigned short usChildID = 0, usParentID = 0 ;
	if ( lpSkillBase )
	{
		usChildID  = lpSkillBase->m_usChildSkill ;
		usParentID = lpSkillBase->m_usParentSkill ;
	}

	int i;
	if (m_csStatus.m_lpQuickSelected && m_csStatus.m_lpQuickSelected->m_bClassSkill)
	{
		vector<CItemInstance *>::iterator it;
		for(it = m_csStatus.m_lstClassSkill.begin(); it != m_csStatus.m_lstClassSkill.end(); it++)
		{
			if ((*it)->GetProtoTypeID() == wSkill)
			{
				(*it)->StartCoolDown(wGrade, cEndCooldown);
			}
		}
	} 
	else
	{
		for(i = 0; i < SKILL::MAX_SLOT_NUM; i++)
		{
			if (m_csStatus.m_lstSkillSlot[i])
			{
				if (m_csStatus.m_lstSkillSlot[i]->GetProtoTypeID() == wSkill)
				{
					m_csStatus.m_lstSkillSlot[i]->StartCoolDown(wGrade, cEndCooldown);
				}
				else if (m_csStatus.m_lstSkillSlot[i]->GetProtoTypeID() == usChildID ||
						 m_csStatus.m_lstSkillSlot[i]->GetProtoTypeID() == usParentID)
				{
					m_csStatus.m_lstSkillSlot[i]->StartCoolDown(wGrade, cEndCooldown, wSkill);
				}
			}
		}
	}

	// �Ҽȸ�Ǹ���ƮȮ��
	if(!m_csStatus.m_lstSocialSlot.empty())
	{
		int iSize = m_csStatus.m_lstSocialSlot.size();
		for(i = 0; i < iSize; ++i)
		{
			if(m_csStatus.m_lstSocialSlot[i]->GetProtoTypeID() == wSkill )
			{
				m_csStatus.m_lstSocialSlot[i]->StartCoolDown(wGrade, cEndCooldown);
			}
		}
	}

	for(i = 0; i < QUICK::MAX_QUICK_NUM; i++)
	{
		if (m_csStatus.m_lstQuickSlot[i])
		{
			if ( m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == wSkill )
			{
				m_csStatus.m_lstQuickSlot[i]->StartCoolDown(wGrade, cEndCooldown);
			}
			else if ( m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == usChildID ||
				      m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == usParentID )
			{
				m_csStatus.m_lstQuickSlot[i]->StartCoolDown(wGrade, cEndCooldown, wSkill);
			}
		}
	}

	if (m_csStatus.m_lpQuickSelected && m_csStatus.m_lpQuickSelected->GetProtoTypeID() == wSkill)
	{
		m_csStatus.m_lpQuickSelected->StartCoolDown(wGrade, cEndCooldown);
	}

	if (m_lpPickItem)
	{
		if (m_lpPickItem->m_bSkill && m_lpPickItem->GetProtoTypeID() == wSkill)
		{
			m_lpPickItem->StartCoolDown(wGrade, cEndCooldown);
		}
	}
}

void	CRYLGameData::SetSkillCoolDown(unsigned short wSkill)
{
	int i;
	if (m_csStatus.m_lpQuickSelected)
	{
		if (m_csStatus.m_lpQuickSelected->m_bClassSkill)
		{
			vector<CItemInstance *>::iterator it;
			for(it = m_csStatus.m_lstClassSkill.begin(); it != m_csStatus.m_lstClassSkill.end(); it++)
			{
				if ((*it)->GetProtoTypeID() == wSkill)
				{
					if ((*it)->m_cCountCooldown)
					{
						(*it)->StartCoolDown((*it)->GetSkillGrade());
					}
					(*it)->ClearCoolDown();
				}
			}
		} 
		
	}
	
	for(i = 0; i < SKILL::MAX_SLOT_NUM; i++)
	{
		if (m_csStatus.m_lstSkillSlot[i])
		{
			if (m_csStatus.m_lstSkillSlot[i]->GetProtoTypeID() == wSkill)
			{
				if (m_csStatus.m_lstSkillSlot[i]->m_cCountCooldown)
				{
					m_csStatus.m_lstSkillSlot[i]->StartCoolDown(m_csStatus.m_lstSkillSlot[i]->GetSkillGrade());
				}
			}
		}
	}



	for(i = 0; i < QUICK::MAX_QUICK_NUM; i++)
	{
		if (m_csStatus.m_lstQuickSlot[i])
		{
			if (m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == wSkill)
			{
				if (m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown)
				{
					m_csStatus.m_lstQuickSlot[i]->StartCoolDown(m_csStatus.m_lstQuickSlot[i]->GetSkillGrade());
				}
			}
		}
	}

	if (m_csStatus.m_lpQuickSelected)
	{
		if (m_csStatus.m_lpQuickSelected->GetSkillID() == wSkill)
		{
			if (m_csStatus.m_lpQuickSelected->m_cCountCooldown)
			{
				m_csStatus.m_lpQuickSelected->StartCoolDown(m_csStatus.m_lpQuickSelected->GetSkillGrade());
			}
		}
	}

	if (m_lpPickItem)
	{
		if (m_lpPickItem->m_bSkill && m_lpPickItem->GetProtoTypeID() == wSkill)
		{
			if (m_lpPickItem->m_cCountCooldown)
			{
				m_lpPickItem->StartCoolDown(m_lpPickItem->GetSkillGrade());
			}
		}
	}
}

void		CRYLGameData::SetPotionCoolDown(unsigned short wPotionSkillID, unsigned int iCurCooldown)
{
	list<CItemInstance *>::iterator it;
	Item::CUseItem *lpPotion;

	for(it = m_csStatus.m_lstInventory.begin(); it != m_csStatus.m_lstInventory.end(); it++)
	{
		lpPotion = Item::CUseItem::DowncastToUseItem((*it)->m_lpItemBase);

		if (lpPotion && lpPotion->GetItemInfo().m_UseItemInfo.m_usSkill_ID == wPotionSkillID)
		{
			(*it)->StartCoolDown(0, 0, 0, iCurCooldown);
			BOOL bUpdate = FALSE;
			vector<PotionCoolTime>::iterator itPotion;
			for(itPotion = m_csStatus.m_lstPotionCoolTime.begin(); itPotion != m_csStatus.m_lstPotionCoolTime.end(); itPotion++)
			{
				if ((*itPotion).m_wPotionSkillID == wPotionSkillID)
				{
					if(iCurCooldown == 0 || (unsigned int)((*it)->m_lCoolDownTime) < iCurCooldown)
						(*itPotion).m_lPotionCoolDownTime = (*it)->m_lCoolDownTime;
					else 
						(*itPotion).m_lPotionCoolDownTime = iCurCooldown;

					bUpdate = TRUE;
					break;
				}
			}

			if (!bUpdate)
			{
				PotionCoolTime pPotion;
				pPotion.m_wPotionSkillID = wPotionSkillID;
				if(iCurCooldown == 0 || (unsigned int)((*it)->m_lCoolDownTime) < iCurCooldown)
					pPotion.m_lPotionCoolDownTime = (*it)->m_lCoolDownTime;
				else 
					pPotion.m_lPotionCoolDownTime = iCurCooldown;

				m_csStatus.m_lstPotionCoolTime.push_back(pPotion);
			}

//			if (m_csStatus.m_lPotionCoolDownTime <= 0)
//				m_csStatus.m_lPotionCoolDownTime = (*it)->m_lCoolDownTime;
		}
	}

	for(int i = 0; i < QUICK::MAX_QUICK_NUM; i++)
	{
		if (m_csStatus.m_lstQuickSlot[i])
		{
			lpPotion = Item::CUseItem::DowncastToUseItem(m_csStatus.m_lstQuickSlot[i]->m_lpItemBase);

			if (lpPotion && lpPotion->GetItemInfo().m_UseItemInfo.m_usSkill_ID == wPotionSkillID)
			{
				m_csStatus.m_lstQuickSlot[i]->StartCoolDown(0, 0, 0, iCurCooldown);
			}
		}
	}

	if (m_lpPickItem)
	{
		lpPotion = Item::CUseItem::DowncastToUseItem(m_lpPickItem->m_lpItemBase);

		if (lpPotion && lpPotion->GetItemInfo().m_UseItemInfo.m_usSkill_ID == wPotionSkillID)
		{
			m_lpPickItem->StartCoolDown(0, 0, 0, iCurCooldown);
		}
	}
}

unsigned long	CRYLGameData::GetScreenPosChr(POINT &Point, int nMode, float fLength)
{
	// edith 2008.06.25 �������� ���� ����
	// ���� Ÿ�� ������ ���ݸ��ϰ� 
	CRYLGameData* pGame = CRYLGameData::Instance();
	if(pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::AVATA] != NULL)
	{
		unsigned char cType = pGame->m_csStatus.m_lstEquipment[Item::EquipmentPos::AVATA]->GetItemInfo().m_DetailData.m_cItemType;
		if(cType != Item::ItemType::TRANSFORM && s_pInstance->m_csStatus.m_cRidePos == 1)
			return 0xFFFFFFFF;	
	}
	

	if ( nMode == Skill::Target::FRIEND )
	{
		if ( g_DeviceInput.KeyHold( VK_CONTROL )  || KeyControl::IsKeyState(KeyControl::KEY_COLUM_SELFTARGET) || fLength <= 100.0f )  
			return CRYLNetworkData::Instance()->m_dwMyChrID ;
	}

	if ( nMode == Skill::Target::ENEMY && fLength <= 100.0f )
	{
		return CRYLNetworkData::Instance()->m_dwMyChrID ;
	}

	if ( nMode == Skill::Target::PARTY )
	{
		return CRYLNetworkData::Instance()->m_dwMyChrID ;
	}

	if ( nMode == Skill::Target::SUMMON )
	{
		return CRYLNetworkData::Instance()->m_dwMyChrID ;
	}

	m_lpSceneManager->PickMakeRay( Point.x, Point.y, g_ClientMain.m_BaseGraphicLayer.m_lScreenSx, g_ClientMain.m_BaseGraphicLayer.m_lScreenSy ) ;

	vector3 vecChrPos ;
	
	float fRad				= 50.0f ;
	float fNearestInterval	= 100.0f ;

	vector3 vecMousePos ;
	vecMousePos.x = Point.x ;
	vecMousePos.y = Point.y ;
	vecMousePos.z = 0.0f ;

	vector3 vecLens, vecResult ;
	vector3 vecViewPos = *m_lpSceneManager->GetCamera()->GetPosition() ;
	float fW ;

    vector<unsigned long> vecCharID;
    RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );
    unsigned int unCount = 0;

	unsigned long dwCID = 0;
	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	for(; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
        if ( NULL == pTempCreature )
            continue;

		dwCID = pTempCreature->GetCharID();

		if ( VIEW_NO_NAME == m_dwViewNameType )
		{
			pTempCreature->SetViewName( false );
		}
		else
		{
			switch ( GetClientIdentity(*pos) )
			{
				case Creature::CT_PC :
				{
					if ( VIEW_PC_NAME == m_dwViewNameType || VIEW_ALL_NAME == m_dwViewNameType )
					{
						pTempCreature->SetViewName( true );
					}
					else
					{
						pTempCreature->SetViewName( false );
					}
				}
				break;

				case Creature::CT_MONSTER :
				case Creature::CT_NPC :
				case Creature::CT_SUMMON	:
				case Creature::CT_STRUCT	:
				case Creature::CT_SIEGE_OBJECT :
				{
					if ( VIEW_OTHER_NAME == m_dwViewNameType || VIEW_ALL_NAME == m_dwViewNameType )
					{
						pTempCreature->SetViewName( true );
					}
					else
					{
						pTempCreature->SetViewName( false );
					}
				}
				break;

				case Creature::CT_NONE_TYPE :
				{
					pTempCreature->SetViewName( false );
				}
				break;
			}
		}

		// Hide �� ĳ���� Ÿ���� ���� �ʱ�
        if ( pTempCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) )
            continue;   

		unsigned short wType = pTempCreature->GetObjectType();

		if(!EnemyCheck::IsCastleArms(wType) && !EnemyCheck::IsSiegeArms(wType))
		{
			if(pTempCreature->GetRideEnable())
			{
				continue;   
			}
		}

		// edith 2009.02.08 ũ�ν���� �˻��Ÿ� ����.
		// ���� 10000 ���� ��Ÿ� �ִ�Ÿ��� 3000���� ����.
		float fDist = 3000.f;

		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		
		if(pSelfCreature)
		{
			wType = pSelfCreature->GetObjectType();

			if(EnemyCheck::IsCastleArms(wType) || EnemyCheck::IsSiegeArms(wType))
			{
				fDist = 7000.0f ;
			}	
		}		
		
		vecChrPos = *( pTempCreature->GetPosition() );
		vecChrPos.y += 100.0f;

		matrix& rCameraMatrix = *((matrix*)(m_lpSceneManager->GetCamera()->GetMatView()));
		matrix mProjection;
		m_lpSceneManager->GetDevice()->GetTransform(D3DTS_PROJECTION, (D3DMATRIX*)(&mProjection));
		matrix mWVP = rCameraMatrix * mProjection;

		if ( g_ClientMain.m_BaseGraphicLayer.TransformVector( vecChrPos, mWVP, vecResult, fW ) )
		{
			if ( vecResult.z < 0.0f || vecResult.z > 1.0f ) { continue; }

			vecLens = vecViewPos - vecChrPos;
			float fInterCamera = vecLens.GetLens();
			float fZRate = -( fInterCamera / fDist ) + 1;
			vecResult.z = 0.0f;
			vecLens = vecMousePos - vecResult;

			if ( vecLens.GetLens() <= fRad * fZRate )
			{
				if (TARGET_SELECTCHAR == nMode)
				{
                    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                    if ( pSelfCreature == pTempCreature && !m_bMouseMode && !m_bSimpleMouse )
                        return 0xFFFFFFFF ;
					
					pTempCreature->SetViewName( true );
					return pTempCreature->GetCharID();
				}

				if (false == CanUseSkill( pTempCreature, static_cast<Skill::Target::Type>( nMode ) ) )
				{
					continue;
				}
            
                RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                if (NULL == pSelfCreature)
                    return 0xFFFFFFFF;

                if ( pSelfCreature == pTempCreature )
                    return 0xFFFFFFFF;

				vector3 vecPos = *( pTempCreature->GetPosition() );
				vecPos.y	= 0.0f;
				vector3 vecSelf = *( pSelfCreature->GetPosition() );
				vecSelf.y	= 0.0f;
				vecSelf	   -= vecPos;

				if ( vecSelf.GetLens() <= fLength ) 
				{
					D3DXVECTOR3 vecSrc, vecDest ;
					vecSrc.x = pSelfCreature->GetPosition()->x ;
					vecSrc.y = pSelfCreature->GetPosition()->y + 100.0f ;
					vecSrc.z = pSelfCreature->GetPosition()->z ;

					wType = pSelfCreature->GetObjectType();

					if(EnemyCheck::IsCastleArms(wType) || EnemyCheck::IsSiegeArms(wType))
					{
						vecSrc.x = g_CharacterData.m_Camera.m_vecCameraPos.x;
						vecSrc.y = g_CharacterData.m_Camera.m_vecCameraPos.y + 50.0f;
						vecSrc.z = g_CharacterData.m_Camera.m_vecCameraPos.z;
					}

					vecDest.x = pTempCreature->GetPosition()->x ;
					vecDest.y = pTempCreature->GetPosition()->y + 100.0f ;
					vecDest.z = pTempCreature->GetPosition()->z ;
	
					if ( CSceneManager::CheckRayCollision( vecSrc, vecDest  ) &&
						!( pTempCreature->GetObjectType() == Siege::CAMP ||
						pTempCreature->GetObjectType() == Siege::MINING_CAMP ||
						pTempCreature->GetObjectType() == Siege::CAMP_SHOP ||
						pTempCreature->GetObjectType() == Siege::KARTERANT_WEAPON ||
						pTempCreature->GetObjectType() == Siege::MERKADIA_WEAPON ||
						pTempCreature->GetObjectType() == Siege::GATE )
						)
						return 0xFFFFFFFF ;	
						
					return pTempCreature->GetCharID();
				}
			}
		}
	}

	return 0xFFFFFFFF;
}

void	CRYLGameData::ResetTerrain(void)
{
	m_lpSceneManager->m_HeightField.m_NowPositionX = -100;
	m_lpSceneManager->m_HeightField.m_NowPositionY = -100;
	m_lpSceneManager->m_HeightField.CheckingNextTerrain();
	m_lpSceneManager->m_HeightField.CheckingTessellate();
}

void	CRYLGameData::AddFieldObject(FieldObject* lpObject)
{
	std::list<LPFieldInstance>::iterator it;
	for(it = m_lstFieldItem.begin(); it != m_lstFieldItem.end(); it++)
	{
		if ((*it)->m_nOID == lpObject->m_nOID) return;
	}

	LPFieldInstance lpAdd = new FieldInstance;
	vector3 vecPos;
	lpAdd->m_nOID = lpObject->m_nOID;
	// �Ҽ��� ���ڸ��� ������ ������ �����Ƿ�, ���� ��ȯ�ÿ� * 10 �� ���ش�.
	vecPos.x = lpObject->m_usXPos * 10.0f;
	vecPos.z = lpObject->m_usZPos * 10.0f;
	if (0 != lpObject->m_usYPos)
	{
		vecPos.y = lpObject->m_usYPos * 10.0f + 15.0f;
	}
	else
	{
		vecPos.y = m_lpSceneManager->m_HeightField.GetHeight(vecPos) + 15.0f;
	}
	lpAdd->m_dwTypeID = lpObject->m_dwTypeID;
	lpAdd->m_cNum = lpObject->m_cNum;
	lpAdd->m_lLifeTick = 590000;
	lpAdd->m_vecPos.x = vecPos.x;
	lpAdd->m_vecPos.y = vecPos.y;
	lpAdd->m_vecPos.z = vecPos.z;

	if (lpObject->m_dwTypeID & FieldObject::Gold)	// ��
	{
		lpAdd->m_dwInstanceID = m_lpSceneManager->AddInstanceObject("0000.R3S", vecPos);
	} else									// ������
	{
		const Item::ItemInfo *lpProtoType = g_ItemMgr.GetItemInfo(lpObject->m_dwTypeID);
		if (lpProtoType)
		{
			lpAdd->m_dwInstanceID = m_lpSceneManager->AddInstanceObject((char *)lpProtoType->m_StringData.m_szFieldModelName, vecPos);
		} else
		{
			delete lpAdd;
			return;
		}
	}

	m_lstFieldItem.push_back(lpAdd);
}


void	CRYLGameData::DeleteFieldObject(unsigned __int64 nFieldInstanceID)
{
	std::list<LPFieldInstance>::iterator it;
	for(it = m_lstFieldItem.begin(); it != m_lstFieldItem.end(); it++)
	{
		if ((*it)->m_nOID == nFieldInstanceID)
		{
			LPFieldInstance lpInstance = (*it);
			m_lpSceneManager->DeleteInstanceObject(lpInstance->m_dwInstanceID);
			m_lstFieldItem.erase(it);
			delete lpInstance;
			break;
		}
	}
}

void	CRYLGameData::DeleteAllFieldItem()
{
	if ( m_lstFieldItem.empty() )
		return ;

	std::list<LPFieldInstance>::iterator it = m_lstFieldItem.begin() ;

	while( it != m_lstFieldItem.end() )
	{
		LPFieldInstance lpInstance = (*it);
		m_lpSceneManager->DeleteInstanceObject(lpInstance->m_dwInstanceID);
		GM_DELETE( lpInstance ) ;

		++ it ;
	}
	m_lstFieldItem.clear() ;
}


LPFieldInstance		CRYLGameData::GetFieldObject(unsigned long dwFieldInstanceID)
{
	std::list<LPFieldInstance>::iterator it;
	for(it = m_lstFieldItem.begin(); it != m_lstFieldItem.end(); it++)
	{
		if ((*it)->m_dwInstanceID == dwFieldInstanceID)
		{
			return (*it);
		}
	}

	return NULL;
}

VOID	CRYLGameData::UpdateInterfaceEffect() //effect load hayzohar
{
	int snum = CSceneManager::m_EffectManager.GetInterfaceScriptNum();
	bool effcheck = false;
	DWORD now_ceffecttime = 0;

	static DWORD before_ceffecttime = 0;
	static int effclick = 0;

	static int increase = 0;
	now_ceffecttime = timeGetTime();

	for(int scnt = 0;scnt<snum;scnt++) { 
		effcheck = CSceneManager::m_EffectManager.ProcessInterfaceScript(scnt);

		if (g_CharacterData.m_lpDuelReady)
		{
			if (CSceneManager::m_EffectManager.CheckNullInterfaceScript(g_CharacterData.m_lpDuelReady))
			{
				g_CharacterData.m_lpDuelReady = NULL;
				g_CharacterData.SetDuelTarget();
			}
		}

		if (m_ClassEffectPtr != NULL) {	// ���� ���� ����Ʈ�� Ȱ��ȭ �Ǿ� ������..	
			if (effcheck && CSceneManager::m_EffectManager.CheckInterfaceScript(scnt,m_ClassEffectPtr)) { 
				// ���� ����Ʈ�� hit ��ȣ�� �߻�������.
				before_ceffecttime = now_ceffecttime;
				effclick = 1;
				increase = increase > 0 ? 0:1;
			}
		}

		if (m_lpItemUpgrade)
		{
			if (effcheck && !(CSceneManager::m_EffectManager.CheckNullInterfaceScript(m_lpItemUpgrade)))
			{
				m_lpItemUpgrade = NULL;
				if (!SendPacket::CharUpgradeItem(g_GameSession, m_dwTradingNpcID))
				{
					g_pClientSocket->SetStatusFlag(NS_NOWAITING);
				}
			}
		}

		// ������ ���� �������Ʈ
		if (m_lpItemResultEft)
		{
			if (effcheck && !(CSceneManager::m_EffectManager.CheckNullInterfaceScript(m_lpItemResultEft)))
			{
				m_lpItemResultEft = NULL;
			}
		}
		

		if ( m_lpItemComposition)
		{
			if (effcheck && !(CSceneManager::m_EffectManager.CheckNullInterfaceScript(m_lpItemComposition)) )
			{
				CRYLGameScene*				pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLItemCompositionDlg*		pDlg	= ( CRYLItemCompositionDlg* )pScene->GetItemCompositionDlg();
				m_lpItemComposition = NULL;
				if (!SendPacket::CharItemOptionGraft( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, pDlg->GetOption() ))
				{
					g_pClientSocket->SetStatusFlag(NS_NOWAITING);
				}
			}

		}
			
	}	
	
	//
	
	//���� ����Ʈ�� ���̳����� �����̼��� ��ġ ������Ʈ ��.
	if (m_ClassEffectPtr != NULL) {
		if (!CSceneManager::m_EffectManager.CheckNullInterfaceScript(m_ClassEffectPtr)) {
			// ���� ����Ʈ�� ���� �������̸�.
			if (effclick) {  
				for(int timeskip = 0;timeskip <(int)(now_ceffecttime - before_ceffecttime)/40;timeskip++) {
					before_ceffecttime = now_ceffecttime;
						 
					if (increase==1) {
						m_ClassEffectAlpha += 13;
					}
					else if (increase == 0){	
						m_ClassEffectAlpha -= 14;
					}					
					
					if (m_ClassEffectAlpha >=255) { 
						m_ClassEffectAlpha = 255;
						effclick = 0;
					}
					else if (m_ClassEffectAlpha <0) 
						m_ClassEffectAlpha = 0;
					
				}
			}
		}
		else{
			m_ClassEffectAlpha = 0;
			increase = 0;
			before_ceffecttime = 0;
			m_ClassEffectPtr = NULL;
		}
	}
}


VOID		CRYLGameData::RenderClassEffect()
{
	if ( m_ClassEffectPtr != NULL) 
	{
		if ( !CSceneManager::m_EffectManager.CheckNullInterfaceScript( m_ClassEffectPtr ) )  
		{	
			int change  = 0 ;
			int csvalue = 0 ;
						
			for ( int classeffect = 0 ; classeffect < 7 ; classeffect ++ ) 
			{
				switch ( classeffect ) 
				{
				case 0 :
					change  = m_csStatus.m_Info.STR - m_csStatus.m_BeforeInfo.STR ;
					csvalue = m_csStatus.m_Info.STR ;
					break ;
				case 1 :
					change  = m_csStatus.m_Info.DEX - m_csStatus.m_BeforeInfo.DEX ;
					csvalue = m_csStatus.m_Info.DEX ;
					break ;
				case 2 :
					change  = m_csStatus.m_Info.INT - m_csStatus.m_BeforeInfo.INT ;
					csvalue = m_csStatus.m_Info.INT ;
					break ;
				case 3 :
					change  = m_csStatus.m_Info.WIS - m_csStatus.m_BeforeInfo.WIS ;
					csvalue = m_csStatus.m_Info.WIS ;
					break ;
				case 4 :
					change  = m_csStatus.m_Info.CON - m_csStatus.m_BeforeInfo.CON ;
					csvalue = m_csStatus.m_Info.CON ;
					break ;
				case 5 :
					change  = m_csStatus.m_Info.HP - m_csStatus.m_BeforeInfo.HP ;
					csvalue = m_csStatus.m_Info.HP ;
					break ;
				case 6 :
					change  = m_csStatus.m_Info.MP - m_csStatus.m_BeforeInfo.MP ;
					csvalue = m_csStatus.m_Info.MP ;
					break ; 
				
				}
				
				float fX = 730.0f / 800.0f;
				float fY1 = 264.0f / 600.0f;
				float fY2 = 292.0f / 600.0f;

				float fUnitY1 = 28.0f / 600.0f;
				float fUnitY2 = 24.0f / 600.0f;

				
				float fTargetX,fTargetY1,fTargetY2;
				float fTargetUnitY1,fTargetUnitY2;

				fTargetX  = fX * g_ClientMain.m_iScreenWidth;
				fTargetY1 = fY1 * g_ClientMain.m_iScreenHeight;
				fTargetY2 = fY2 * g_ClientMain.m_iScreenHeight;
				fTargetUnitY1 = fUnitY1 * g_ClientMain.m_iScreenHeight;
				fTargetUnitY2 = fUnitY2 * g_ClientMain.m_iScreenHeight;

				if ( classeffect < 5 ) 
				{
					if ( change > 0 )
						RenderNumber( g_ClientMain.m_BaseGraphicLayer.GetDevice(), fTargetX, fTargetY1 + ( classeffect * fTargetUnitY1 ), ( unsigned char )m_ClassEffectAlpha, 3, csvalue, 0.5f ) ;
					else 
						RenderNumber( g_ClientMain.m_BaseGraphicLayer.GetDevice(), fTargetX, fTargetY1 + ( classeffect * fTargetUnitY1 ), ( unsigned char )m_ClassEffectAlpha, 0, csvalue, 0.5f ) ;
				}
				else 
				{
					if ( change > 0 )
						RenderNumber( g_ClientMain.m_BaseGraphicLayer.GetDevice(), fTargetX, fTargetY2 + ( classeffect * fTargetUnitY2 ),( unsigned char )m_ClassEffectAlpha, 3, csvalue, 0.5f ) ;
					else 
						RenderNumber( g_ClientMain.m_BaseGraphicLayer.GetDevice(), fTargetX, fTargetY2 + ( classeffect * fTargetUnitY2 ),( unsigned char )m_ClassEffectAlpha, 0, csvalue, 0.5f ) ;
				}
			}
		}
	}
}


VOID	CRYLGameData::RenderQuestCompleteEffect()
{
	// ------------------------------------------------------------------------------------------
	// ����Ʈ �Ϸ� ����Ʈ ���

	if ( m_bQuestComplete )
	{
		m_bQuestComplete = FALSE ;

		if ( m_QuestCompleteEffectPtr )
		{
			CSceneManager::m_EffectManager.DeleteInterfaceScript( m_QuestCompleteEffectPtr );
			m_QuestCompleteEffectPtr = NULL;
		}

		m_QuestCompleteEffectPtr = new CEffScript;
		m_QuestCompleteEffectPtr->SetInterfaceSet( true );
		m_QuestCompleteEffectPtr->GetScriptBinData( "inter_complete.esf" );	

		float fX = m_QuestCompleteEffectPtr->m_InterfacePos[0] / 800.0f;
		float fY = m_QuestCompleteEffectPtr->m_InterfacePos[1] / 600.0f;

		m_QuestCompleteEffectPtr->m_InterfacePos[0] = fX * g_ClientMain.m_iScreenWidth;
		m_QuestCompleteEffectPtr->m_InterfacePos[1] = fY * g_ClientMain.m_iScreenHeight;				

		CSceneManager::m_EffectManager.AddInterfaceScript( m_QuestCompleteEffectPtr );
	}
	// ------------------------------------------------------------------------------------------
}


void	CRYLGameData::SetPickItem( CItemInstance *lpItem, BOOL bSound )
{
	m_lpPickItem = lpItem ;

	if ( bSound && lpItem )
	{
		char Temp[MAX_PATH]; 
		sprintf( Temp, "%s\\Sound\\Interface\\click.mnd", g_ClientMain.m_strClientPath ) ;
		CSound* pSound; 
		CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
		pSound->Play(); 
	}
}

void	CRYLGameData::PlayClickSound(void)
{
	char Temp[MAX_PATH]; 
	sprintf( Temp, "%s\\Sound\\Interface\\click.mnd", g_ClientMain.m_strClientPath ) ;
	CSound* pSound; 
	CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
	pSound->Play(); 
}

BOOL	CRYLGameData::CreateRaceInterface( unsigned long dwRace ,unsigned long dwSex )
{
//	WORK_LIST 1.1 ���� ���� ȭ�� �ٽ� �߰�
	m_dwClientRace = dwRace;
	m_dwClientSex = dwSex;
	// WORK_LIST 2.1 ���� ���� �߰�
	if ( Creature::STATELESS == m_cAccountNation )
	{
		unsigned char cAccountNation = 0;
		switch ( dwRace )
		{
			case CClass::RaceType::HUMAN:	cAccountNation = Creature::KARTERANT;		break;
			case CClass::RaceType::AKHAN:	cAccountNation = Creature::MERKADIA;		break;
		}

		// ���� ���� ���� ��Ŷ ����
		CRYLNetworkData::SendSelectAccountNation( 1, cAccountNation ) ;	// 1 = TYPE_SET
	}	

	m_lpInterface = new CRYLRaceBase ;	

	if ( !m_cReLogin )
	{
		CRYLGameScene*		pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;		

		pScene->InitResourceObject() ;
	}	

	if ( m_lpInterface )
	{
		g_CharacterData.PickItem = &CRYLRaceBase::PickItem ;
		return TRUE ;
	}

	return FALSE ;
}

VOID		CRYLGameData::ClearGameDlgData() 
{	
	CRYLGameScene*				pScene			= static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ) ;
	pScene->GUIVisible( FALSE ) ;

	// ���â �ʱ�ȭ
	CRYLGuildFrameDlg*			pDlg			= (CRYLGuildFrameDlg*)pScene->GetGuildFrameDlg() ;
	CRYLGuildInfoPanel*			pPanel			= (CRYLGuildInfoPanel*)pDlg->GetGuildInfoPanel() ;	
	CRYLGuildMemberPanel*		pMemberPanel	= (CRYLGuildMemberPanel*)pDlg->GetGuildMemberPanel() ;
	CRYLGuildTacticsPanel*		pTacticsPanel	= (CRYLGuildTacticsPanel*)pDlg->GetGuildTacticsPanel() ;
	CRYLGuildPropertyPanel*		pPropertyPanel	= (CRYLGuildPropertyPanel*)pDlg->GetGuildPropertyPanel() ;
	CRYLAuthoritySettingDlg*	pASDlg			= (CRYLAuthoritySettingDlg*)pDlg->GetAuthoritySettingDlg() ;
	CRYLHostilityDlg*			pHostilityDlg	= (CRYLHostilityDlg*)pDlg->GetHostilityDlg() ;
	
	pPanel->ClearList() ;
	pMemberPanel->ClearList() ;
	pTacticsPanel->ClearList() ;
	pPropertyPanel->Clear() ;
	pASDlg->SetVisible( FALSE ) ;
	pHostilityDlg->SetVisible( FALSE ) ;
	
	// ģ��, �ź� â �ʱ�ȭ
	CRYLSocietyFrameDlg*		pSocietyDlg		= (CRYLSocietyFrameDlg*)pScene->GetSocietyFrameDlg() ;
	CRYLFriendPanel*			pFriendDlg		= (CRYLFriendPanel*)pSocietyDlg->GetFriendPanel() ;
	CRYLRefusalPanel*			pRefusalPanel	= (CRYLRefusalPanel*)pSocietyDlg->GetRefusalPanel() ;

	pFriendDlg->ClearWhisperCharacter();
	pRefusalPanel->ClearWhisperCharacter();

	pDlg->GetTabButton()->SetFocusTabButton( 0 ) ;
	( ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() )->SetVisible( TRUE ) ;
	( ( CRYLGuildPropertyPanel* )pDlg->GetGuildPropertyPanel() )->SetVisible( FALSE ) ;	
	( ( CRYLGuildMemberPanel* )pDlg->GetGuildMemberPanel() )->SetVisible( FALSE ) ;
	( ( CRYLGuildTacticsPanel* )pDlg->GetGuildTacticsPanel() )->SetVisible( FALSE ) ;
	
	pPanel->m_iCurrentSort = 0;

	// ������ ī��Ʈ �ʱ�ȭ
	CRYLGameData::SetRespawnCounter( 0 ) ;

	// ä��â �ʱ�ȭ
	CRYLChattingDlg::Instance()->m_pChatEdit->SetText( "" ) ;

	CRYLChattingDlg::Instance()->m_pChattingListBox->ClearItem() ;
	CRYLChattingDlg::Instance()->m_pChattingListBox->SetTrackPos(0);
	CRYLChattingDlg::Instance()->m_pChattingListBox->SetItemIndex(0);

	// ���� ���� ������ �ʱ�ȭ (��Ÿ��Ŷ ����)
	if ( m_csStatus.m_lpSiegeArmsMaterial )
	{
		delete m_csStatus.m_lpSiegeArmsMaterial ;
		m_csStatus.m_lpSiegeArmsMaterial = NULL ;
	}

	// ��Ƽâ �ʱ�ȭ
	CRYLPartyPanel*		    	pPartyPanel     = (CRYLPartyPanel*)pSocietyDlg->GetPartyPanel() ;

	pPartyPanel->DeletePartyList();
	pPartyPanel->CaptionClear();

	m_csStatus.m_iCompositionResult = -1;

	// ���� ������â�� �������� ������ �ʱ�ȭ
	GM_DELETE( m_csStatus.m_lpCompositionResult );
	GM_DELETE( m_csStatus.m_lpCompositionTempResult );
	GM_DELETE( m_csStatus.m_lpCompositionOffering );
	GM_DELETE( m_csStatus.m_lpCompositionOrigin );
	GM_DELETE( m_csStatus.m_lpCompensation );
	GM_DELETE( m_csStatus.m_lpCompensationResult );

	// ĳ���� ����ȭ�� �ʱ�ȭ
	CRYLCharacterSelectScene* pSelectScene = ( CRYLCharacterSelectScene* )CRYLSceneManager::Instance()->FindScene( CHARACTERSELECT_SCENE );
	( ( CRYLCharacterInfoDlg* )pSelectScene->GetCharacterInfoDlg() )->EnableButton( FALSE );
	m_dwSelectChr = 0xFFFFFFFF;

	m_bShowCursor = TRUE ;
	m_bSimpleMouse = TRUE ;	

	// ���� �Ҹ� ����
	char SoundBuf[256] = { 0, };
	sprintf( SoundBuf, "%s%s", EFFECTSOUNDMPATH, "heart.wav");				
	CSound* pSound = NULL;
	CSoundMgr::_GetInstance()->GetSound( pSound, SoundBuf, 1 ); 
	pSound->Stop(); 
}

VOID		CRYLGameData::MoveCharSelectScene() 
{	
	g_pClientSocket->SetStatusFlag(NS_SERVERMOVE);	

	// Disconnect() �ϰ� �Ϸ�Ǹ� ����		
    g_pClientSocket->Disconnect(ClientSocket::GameEventHandler);//n

	if ( m_bLogoutEnable )
	{
		m_bLogoutEnable = false ;
		g_ClientMain.Update() ;	
	}
	Sleep( 3000 );	
	
    ClientNet::CClientEventHandler* lpAuthHandler = new CAuthEventHandler();
		
    g_pClientSocket->Open( CRYLNetworkData::m_strIP,
						   ClientSocket::DEFAULT_AUTH_TCP_PORT,
						   lpAuthHandler) ;
	
	memset( m_scLoginChar, 0, sizeof( CHAR_VIEW ) * MAX_CHAR_SLOT ) ;
	memset( m_scLoginCharGuildData, 0, sizeof( sGuildData ) * MAX_CHAR_SLOT ) ;
	// WORK_LIST 2.1 ���� ���� �߰�
	m_cAccountNation = 0 ;

	m_cReLogin = RELOGIN::CHAR_SELECT;
	g_ClientMain.Update() ;

	g_CharacterData.InitDuelTarget() ;	

	
	
}

void		CRYLGameData::EnterClient()
{
//	WORK_LIST 1.1 ���� ���� ȭ�� �ٽ� �߰�
/*	���� �� �ҽ�	*/

	for (int i = 0; i < MAX_CHAR_SLOT; ++i)
	{
		if ( m_scLoginChar[ i ].CID )
		{
			CreateRaceInterface( m_scLoginChar[ i ].Race,m_scLoginChar[ i ].Sex  ) ;
			m_lpInterface->InitChrSelect() ;

			CRYLSceneManager::Instance()->SetScene( CHARACTERSELECT_SCENE ) ;

			if ( !m_cReLogin )
			{
				( ( CRYLCharacterSelectScene* ) CRYLSceneManager::GetCurrentScene() )->CreateCharacterSelectDlg( m_scLoginChar[ i ].Race ) ;
				if ( FAILED( CRYLSceneManager::Instance()->GetCurrentSceneInitResource() ) )
				{
					GMASSERT( FALSE, "IntroScene Resource Load Error!" ) ;
					return ;
				}
			}
			g_ClientMain.Update() ;			

			return;
		}
	}

	// WORK_LIST 2.1 ���� ���� �߰�
	m_cAccountNation = Creature::STATELESS;
	m_dwClientMode = CLIENT_NATIONSELECT;
	//m_lpCommonInterface->InitNationSelect() ;

	// WORK_LIST 2.2 NationSelectScene �̸� ����
	CRYLSceneManager::Instance()->SetScene( RACESELECT_SCENE ) ;

/*	*/

/*	������ �ҽ�
	CreateRaceInterface() ;
	m_lpInterface->InitChrSelect() ;

	CRYLSceneManager::Instance()->SetScene( CHARACTERSELECT_SCENE ) ;

	if ( !m_bReLogin )
	{
		( ( CRYLCharacterSelectScene* ) CRYLSceneManager::GetCurrentScene() )->CreateCharacterSelectDlg() ;
		if ( FAILED( CRYLSceneManager::Instance()->GetCurrentSceneInitResource() ) )
		{
			GMASSERT( FALSE, "IntroScene Resource Load Error!" ) ;
			return ;
		}
	}

	g_ClientMain.Update() ;
*/	
}

void CRYLGameData::RenderNumber(LPDIRECT3DDEVICE8 lpD3DDevice, short sPosX, short sPosY, unsigned char cAlpha, unsigned short wMode, long lValue, float fRate)
{
	if (!m_lpNumber)
	{
		m_lpNumber = new CTexture;
		CTexture::SetPath(INTERFACETEXTUREPATH);
		m_lpNumber->Load("num.dds");
	}

	char strNum[12];
	itoa(abs(lValue), strNum, 10);

	switch (wMode)
	{
		case 5:							// Critical
		case 6:
			if (strcmp(strNum, ""))
			{
				float fRealX = 23.0f * fRate;
				float fRealY = 29.0f * fRate;

				int len = strlen(strNum);
				float fPosX = sPosX - len * fRealX / 2.0f;
				float fPosY = sPosY - ((fRealY - 29.0f) / 2.0f);

				TLVertex pVertices[4];
				pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
				pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = cAlpha;
				pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
				pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
				pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

				pVertices[0].v.x = pVertices[1].v.x = fPosX - 0.5f;
				pVertices[2].v.x = pVertices[3].v.x = fPosX + fRealX - 0.5f;
				pVertices[0].v.y = pVertices[2].v.y = fPosY - 0.5f;
				pVertices[1].v.y = pVertices[3].v.y = fPosY + fRealY - 0.5f;

				pVertices[0].tv = pVertices[2].tv = (92 + ((wMode - 5) * 29.0f)) / 256.0f + 0.0005f;
				pVertices[1].tv = pVertices[3].tv = (92 + ((wMode - 5) * 29.0f + 29.0f)) / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader(NULL);
				lpD3DDevice->SetVertexShader(TLVERTEXFVF);
				lpD3DDevice->SetTexture(1, NULL);
				lpD3DDevice->SetTexture(0, m_lpNumber->GetTexture());

				for(int i = 0; i < len; i++)
				{
					pVertices[0].tu = pVertices[1].tu = ((strNum[i] - '0') * 23.0f) / 256.0f + 0.0005f;
					pVertices[2].tu = pVertices[3].tu = ((strNum[i] - '0') * 23.0f + 23.0f) / 256.0f - 0.0005f;
					lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));

					pVertices[0].v.x += fRealX;
					pVertices[1].v.x += fRealX;
					pVertices[2].v.x += fRealX;
					pVertices[3].v.x += fRealX;
				}
			}
			break;

		case 7:							// Block
		case 8:
			{
				float fRealX = 88.0f * fRate;
				float fRealY = 24.0f * fRate;

				float fPosX = sPosX - fRealX / 2.0f;
				float fPosY = sPosY - ((fRealY - 24.0f) / 2.0f);

				TLVertex pVertices[4];
				pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
				pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = cAlpha;
				pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
				pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
				pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

				pVertices[0].v.x = pVertices[1].v.x = fPosX - 0.5f;
				pVertices[2].v.x = pVertices[3].v.x = fPosX + fRealX - 0.5f;
				pVertices[0].v.y = pVertices[2].v.y = fPosY - 0.5f;
				pVertices[1].v.y = pVertices[3].v.y = fPosY + fRealY - 0.5f;

				pVertices[0].tu = pVertices[1].tu = 0.0005f;
				pVertices[2].tu = pVertices[3].tu = 88 / 256.0f - 0.0005f;
				pVertices[0].tv = pVertices[2].tv = (150 + ((wMode - 7) * 24.0f)) / 256.0f - 0.0005f;
				pVertices[1].tv = pVertices[3].tv = (150 + ((wMode - 7) * 24.0f + 24.0f)) / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader(NULL);
				lpD3DDevice->SetVertexShader(TLVERTEXFVF);
				lpD3DDevice->SetTexture(1, NULL);
				lpD3DDevice->SetTexture(0, m_lpNumber->GetTexture());
				lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
			}
			break;

		case 9:							// Evade
			{
				float fRealX = 110.0f * fRate;
				float fRealY = 25.0f * fRate;

				float fPosX = sPosX - fRealX / 2.0f;
				float fPosY = sPosY - ((fRealY - 25.0f) / 2.0f);

				TLVertex pVertices[4];
				pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
				pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = cAlpha;
				pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
				pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
				pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

				pVertices[0].v.x = pVertices[1].v.x = fPosX - 0.5f;
				pVertices[2].v.x = pVertices[3].v.x = fPosX + fRealX - 0.5f;
				pVertices[0].v.y = pVertices[2].v.y = fPosY - 0.5f;
				pVertices[1].v.y = pVertices[3].v.y = fPosY + fRealY - 0.5f;

				pVertices[0].tu = pVertices[1].tu = 0.0005f;
				pVertices[2].tu = pVertices[3].tu = 110 / 256.0f - 0.0005f;
				pVertices[0].tv = pVertices[2].tv = 198 / 256.0f - 0.0005f;
				pVertices[1].tv = pVertices[3].tv = 223 / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader(NULL);
				lpD3DDevice->SetVertexShader(TLVERTEXFVF);
				lpD3DDevice->SetTexture(1, NULL);
				lpD3DDevice->SetTexture(0, m_lpNumber->GetTexture());
				lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
			}
			break;

		case 10:						// Resist
			{
				float fRealX = 105.0f * fRate;
				float fRealY = 27.0f * fRate;

				float fPosX = sPosX - fRealX / 2.0f;
				float fPosY = sPosY - ((fRealY - 27.0f) / 2.0f);

				TLVertex pVertices[4];
				pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
				pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = cAlpha;
				pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
				pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
				pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

				pVertices[0].v.x = pVertices[1].v.x = fPosX - 0.5f;
				pVertices[2].v.x = pVertices[3].v.x = fPosX + fRealX - 0.5f;
				pVertices[0].v.y = pVertices[2].v.y = fPosY - 0.5f;
				pVertices[1].v.y = pVertices[3].v.y = fPosY + fRealY - 0.5f;

				pVertices[0].tu = pVertices[1].tu = 0.0005f;
				pVertices[2].tu = pVertices[3].tu = 105 / 256.0f - 0.0005f;
				pVertices[0].tv = pVertices[2].tv = 223 / 256.0f - 0.0005f;
				pVertices[1].tv = pVertices[3].tv = 250 / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader(NULL);
				lpD3DDevice->SetVertexShader(TLVERTEXFVF);
				lpD3DDevice->SetTexture(1, NULL);
				lpD3DDevice->SetTexture(0, m_lpNumber->GetTexture());
				lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
			}
			break;
		case 11:					// �ڽ��� miss
			{
				float fRealX = 110.0f * fRate * 0.6;
				float fRealY = 25.0f * fRate * 0.6;

				float fPosX = sPosX - fRealX / 2.0f;
				float fPosY = sPosY - ((fRealY - 25.0f) / 2.0f);

				TLVertex pVertices[4];
				pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
				pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = cAlpha;
				pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
				pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
				pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

				pVertices[0].v.x = pVertices[1].v.x = fPosX - 0.5f;
				pVertices[2].v.x = pVertices[3].v.x = fPosX + fRealX - 0.5f;
				pVertices[0].v.y = pVertices[2].v.y = fPosY - 0.5f;
				pVertices[1].v.y = pVertices[3].v.y = fPosY + fRealY - 0.5f;

				pVertices[0].tu = pVertices[1].tu = 91 / 256.0f - 0.0005f;
				pVertices[2].tu = pVertices[3].tu = 162 / 256.0f - 0.0005f;
				pVertices[0].tv = pVertices[2].tv = 152 / 256.0f - 0.0005f;
				pVertices[1].tv = pVertices[3].tv = 174 / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader(NULL);
				lpD3DDevice->SetVertexShader(TLVERTEXFVF);
				lpD3DDevice->SetTexture(1, NULL);
				lpD3DDevice->SetTexture(0, m_lpNumber->GetTexture());
				lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
			}
			break;
		case 12:					// ��밡 miss
			{
				float fRealX = 110.0f * fRate * 0.6;
				float fRealY = 25.0f * fRate * 0.6;

				float fPosX = sPosX - fRealX / 2.0f;
				float fPosY = sPosY - ((fRealY - 25.0f) / 2.0f);

				TLVertex pVertices[4];
				pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
				pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = cAlpha;
				pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
				pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
				pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

				pVertices[0].v.x = pVertices[1].v.x = fPosX - 0.5f;
				pVertices[2].v.x = pVertices[3].v.x = fPosX + fRealX - 0.5f;
				pVertices[0].v.y = pVertices[2].v.y = fPosY - 0.5f;
				pVertices[1].v.y = pVertices[3].v.y = fPosY + fRealY - 0.5f;

				pVertices[0].tu = pVertices[1].tu = 91 / 256.0f - 0.0005f;
				pVertices[2].tu = pVertices[3].tu = 162 / 256.0f - 0.0005f;
				pVertices[0].tv = pVertices[2].tv = 176 / 256.0f - 0.0005f;
				pVertices[1].tv = pVertices[3].tv = 198 / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader(NULL);
				lpD3DDevice->SetVertexShader(TLVERTEXFVF);
				lpD3DDevice->SetTexture(1, NULL);
				lpD3DDevice->SetTexture(0, m_lpNumber->GetTexture());
				lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
			}
			break;

		default:						// Normal
			if (strcmp(strNum, ""))
			{
				float fRealX = 21.0f * fRate;
				float fRealY = 23.0f * fRate;

				int len = strlen(strNum);
				float fPosX = sPosX - len * fRealX / 2.0f;
				float fPosY = sPosY - ((fRealY - 23.0f) / 2.0f);

				TLVertex pVertices[4];
				pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
				pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = cAlpha;
				pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
				pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
				pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

				pVertices[0].v.x = pVertices[1].v.x = fPosX - 0.5f;
				pVertices[2].v.x = pVertices[3].v.x = fPosX + fRealX - 0.5f;
				pVertices[0].v.y = pVertices[2].v.y = fPosY - 0.5f;
				pVertices[1].v.y = pVertices[3].v.y = fPosY + fRealY - 0.5f;

				pVertices[0].tv = pVertices[2].tv = (wMode * 23.0f) / 256.0f + 0.0005f;
				pVertices[1].tv = pVertices[3].tv = (wMode * 23.0f + 23.0f) / 256.0f - 0.0005f;

				lpD3DDevice->SetVertexShader(NULL);
				lpD3DDevice->SetVertexShader(TLVERTEXFVF);
				lpD3DDevice->SetTexture(1, NULL);
				lpD3DDevice->SetTexture(0, m_lpNumber->GetTexture());

				for(int i = 0; i < len; i++)
				{
					pVertices[0].tu = pVertices[1].tu = ((strNum[i] - '0') * 21.0f) / 256.0f + 0.0005f;
					pVertices[2].tu = pVertices[3].tu = ((strNum[i] - '0') * 21.0f + 21.0f) / 256.0f - 0.0005f;
					lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));

					pVertices[0].v.x += fRealX;
					pVertices[1].v.x += fRealX;
					pVertices[2].v.x += fRealX;
					pVertices[3].v.x += fRealX;
				}
			}
			break;
	}
}

void	CRYLGameData::RenderRect(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned long dwColor, unsigned char cAlpha)
{
	TLVertex pVertices[4];
	pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = dwColor;
	pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = cAlpha;
	pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
	pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
	pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

	pVertices[0].v.x = pVertices[1].v.x = x1;
	pVertices[2].v.x = pVertices[3].v.x = x2;
	pVertices[0].v.y = pVertices[2].v.y = y1;
	pVertices[1].v.y = pVertices[3].v.y = y2;

	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(1, NULL);
	lpD3DDevice->SetTexture(0, NULL);
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
}

char *CRYLGameData::GetClassName(unsigned short wClass)
{
	if (1 <= wClass && wClass <= 24)
		return (char *)&CRYLStringTable::m_strString[450 + wClass];
	else
		return (char *)&CRYLStringTable::m_strString[450];
}


void CRYLGameData::RenderStackNumber(LPDIRECT3DDEVICE8 lpD3DDevice, short sPosX, short sPosY, long lValue)
{
	if (!m_lpStackNumber)
	{
		m_lpStackNumber = new CTexture;
		CTexture::SetPath(INTERFACETEXTUREPATH);
		m_lpStackNumber->Load("num_stack.dds");
	}

	char strNum[12];
	itoa(abs(lValue), strNum, 10);

	if (strcmp(strNum, ""))
	{
		int len = strlen(strNum);
		float fPosX = sPosX - (len * 9.0f / 2.0f);
		float fPosY = sPosY;

		TLVertex pVertices[4];
		
		pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
		pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;

		pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
		pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

		pVertices[0].v.x = pVertices[1].v.x = fPosX;
		pVertices[2].v.x = pVertices[3].v.x = fPosX + 9.0f;
		pVertices[0].v.y = pVertices[2].v.y = fPosY;
		pVertices[1].v.y = pVertices[3].v.y = fPosY + 11.0f;

		pVertices[0].tv = pVertices[2].tv = (0 + 0.5f) / 16.0f ;
		pVertices[1].tv = pVertices[3].tv = (11.0f + 0.5f)  / 16.0f;

		lpD3DDevice->SetVertexShader(NULL);
		lpD3DDevice->SetVertexShader(TLVERTEXFVF);
		lpD3DDevice->SetTexture(1, NULL);
		lpD3DDevice->SetTexture(0, m_lpStackNumber->GetTexture());

		for(int i = 0; i < len; i++)
		{
			pVertices[0].tu = pVertices[1].tu = ((strNum[i] - '0') * 9.0f + 0.5f ) / 128.0f;
			pVertices[2].tu = pVertices[3].tu = ((strNum[i] - '0') * 9.0f + 9.0f + 0.5f ) / 128.0f;

			lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));

			pVertices[0].v.x += 8.0f;
			pVertices[1].v.x += 8.0f;
			pVertices[2].v.x += 8.0f;
			pVertices[3].v.x += 8.0f;
		}
	}
}

void CRYLGameData::RenderCursor(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned short wPosX, unsigned short wPosY, unsigned long dwKindCursor, unsigned char cAlpha)
{
	if (!m_lpCursor)
	{
		CTexture::SetPath(INTERFACETEXTUREPATH);
		m_lpCursor = new CTexture;
		m_lpCursor->Load("cursor.dds");
	}

	TLVertex pVertices[4];
	pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
	pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = cAlpha;
	pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
	pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
	pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

	pVertices[0].v.x = pVertices[1].v.x = wPosX - 0.5f;
	pVertices[2].v.x = pVertices[3].v.x = wPosX + 32.0f - 0.5f;
	pVertices[0].v.y = pVertices[2].v.y = wPosY - 0.5f;
	pVertices[1].v.y = pVertices[3].v.y = wPosY + 32.0f - 0.5f;

	pVertices[0].tu = pVertices[1].tu = (dwKindCursor * 32.0f) / 256.0f + 0.0005f;
	pVertices[2].tu = pVertices[3].tu = (dwKindCursor * 32.0f + 32.0f) / 256.0f - 0.0005f;

	pVertices[0].tv = pVertices[2].tv = 0.0f + 0.0005f;
	pVertices[1].tv = pVertices[3].tv = 1.0f - 0.0005f;

	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(1, NULL);
	lpD3DDevice->SetTexture(0, m_lpCursor->GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
}

BOOL CRYLGameData::EncodeJPGFile(LPTSTR lpstrFilename, LPDIRECT3DSURFACE8 lpSurface)
{
	BOOL bres = TRUE;
	IJLERR jerr;
	// Allocate the IJL JPEG_CORE_PROPERTIES structure.
	JPEG_CORE_PROPERTIES jcprops;

	// Initialize the IntelR JPEG Library.
	jerr = ijlInit(&jcprops);
	if (IJL_OK != jerr)
	{
		bres = FALSE;
		ijlFree(&jcprops);
		return bres;
	}

	D3DLOCKED_RECT drRect;
	D3DSURFACE_DESC ddDesc;
	unsigned long *lpColors;
	lpSurface->GetDesc(&ddDesc);
	RECT rcRect = { 0, 0, ddDesc.Width, ddDesc.Height };
	lpSurface->LockRect(&drRect, &rcRect, D3DLOCK_READONLY);
	lpColors = (unsigned long *)drRect.pBits;

	unsigned long dwTemp;
	for(unsigned long j = 0; j < ddDesc.Height; j++)
	{
		for(unsigned long i = 0; i < ddDesc.Width; i++)
		{
			dwTemp = *lpColors;
			*lpColors = ((unsigned char)(dwTemp >> 24) << 24);
			*lpColors += (unsigned char)(dwTemp >> 16);
			*lpColors += ((unsigned char)(dwTemp >> 8) << 8);
			*lpColors += ((unsigned char)(dwTemp) << 16);
			lpColors++;
		}
	}

	// Set up information to write from the pixel buffer.
	jcprops.DIBWidth = ddDesc.Width;
	jcprops.DIBHeight = ddDesc.Height; // Implies a bottom-up DIB.
	jcprops.DIBBytes = (unsigned char *)drRect.pBits;
	jcprops.DIBPadBytes = 0;
	// Note: the following are default values and thus
	// do not need to be set.
	jcprops.DIBChannels = 4;
	jcprops.DIBColor = IJL_RGB;

	jcprops.JPGFile = const_cast<LPSTR>(lpstrFilename);
	// Specify JPEG file creation parameters.
	jcprops.JPGWidth = ddDesc.Width;
	jcprops.JPGHeight = ddDesc.Height;
	// Note: the following are default values and thus
	// do not need to be set.
	jcprops.JPGChannels = 3;
	jcprops.JPGColor = IJL_YCBCR;
	jcprops.JPGSubsampling = IJL_422; // 4:2:2 subsampling.
	jcprops.jquality = 75; // Select "good" image quality
	// Write the actual JPEG image from the pixel buffer.
	jerr = ijlWrite(&jcprops, IJL_JFILE_WRITEWHOLEIMAGE);

	lpSurface->UnlockRect();

	if (IJL_OK != jerr)
	{
		bres = FALSE;
	}

	ijlFree(&jcprops);
	return bres;
}

unsigned short 
CRYLGameData::GetEnchantLevel( unsigned long lID ) 
{
	if ( lID >=  Skill::SpellID::MAX_SPELL_TYPE )
		return 0;

	return s_pInstance->m_csStatus.m_aryEnchantLevel[ lID ] ;
}

unsigned long 
CRYLGameData::GetEnchantTime( unsigned long lID ) 
{
	if ( lID >=  Skill::SpellID::MAX_SPELL_TYPE )
		return 0;

	return s_pInstance->m_csStatus.m_aryEnchantTime[ lID ] ;
}

unsigned long
CRYLGameData::GetClientRace()
{
	if ( s_pInstance )
		return s_pInstance->m_dwClientRace ;

	return CClass::RaceType::HUMAN;

}
unsigned short 
CRYLGameData::GetSkillIDbyChantID(unsigned char cChant)
{
	int i;
	unsigned short wSkill;
	wSkill = s_pInstance->m_csStatus.GetSkillIDbyChantID( cChant );
	if (s_pInstance->m_csStatus.m_lpQuickSelected)
	{
		if (s_pInstance->m_csStatus.m_lpQuickSelected->m_bClassSkill)
		{
			vector<CItemInstance *>::iterator it;
			for(it = s_pInstance->m_csStatus.m_lstClassSkill.begin(); it != s_pInstance->m_csStatus.m_lstClassSkill.end(); it++)
			{
				if ((*it)->GetProtoTypeID() == wSkill)
				{
					if ((*it)->m_cCountCooldown)
					{
						(*it)->StartCoolDown((*it)->GetSkillGrade());
					}
					(*it)->ClearCoolDown();
				}
			}
		} else
		{
			for(i = 0; i < SKILL::MAX_SLOT_NUM; i++)
			{
				if (s_pInstance->m_csStatus.m_lstSkillSlot[i])
				{
					if (s_pInstance->m_csStatus.m_lstSkillSlot[i]->GetProtoTypeID() == wSkill)
					{
						if (s_pInstance->m_csStatus.m_lstSkillSlot[i]->m_cCountCooldown)
						{
							s_pInstance->m_csStatus.m_lstSkillSlot[i]->StartCoolDown(s_pInstance->m_csStatus.m_lstSkillSlot[i]->GetSkillGrade());
						}
					}
				}
			}
		}
	}

	for(i = 0; i < QUICK::MAX_QUICK_NUM; i++)
	{
		if (s_pInstance->m_csStatus.m_lstQuickSlot[i])
		{
			if (s_pInstance->m_csStatus.m_lstQuickSlot[i]->GetProtoTypeID() == wSkill)
			{
				if (s_pInstance->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown)
				{
					s_pInstance->m_csStatus.m_lstQuickSlot[i]->StartCoolDown(s_pInstance->m_csStatus.m_lstQuickSlot[i]->GetSkillGrade());
				}
			}
		}
	}

	if (s_pInstance->m_csStatus.m_lpQuickSelected)
	{
		if (s_pInstance->m_csStatus.m_lpQuickSelected->GetSkillID() == wSkill)
		{
			if (s_pInstance->m_csStatus.m_lpQuickSelected->m_cCountCooldown)
			{
				s_pInstance->m_csStatus.m_lpQuickSelected->StartCoolDown(s_pInstance->m_csStatus.m_lpQuickSelected->GetSkillGrade());
			}
		}
	}

	if (s_pInstance->m_lpPickItem)
	{
		if (s_pInstance->m_lpPickItem->m_bSkill && s_pInstance->m_lpPickItem->GetProtoTypeID() == wSkill)
		{
			if (s_pInstance->m_lpPickItem->m_cCountCooldown)
			{
				s_pInstance->m_lpPickItem->StartCoolDown(s_pInstance->m_lpPickItem->GetSkillGrade());
			}
		}
	}
	return s_pInstance->m_csStatus.GetSkillIDbyChantID( cChant );
}


VOID		CRYLGameData::UpdateEnchantTime()		// ��þƮ Ÿ���� ������Ʈ �Ѵ�.
{
	for(int iID = 0; iID< Skill::SpellID::MAX_SPELL_TYPE; ++iID)
	{
		if(m_csStatus.m_aryEnchantLevel[ iID ] == 0)
			continue;

		if(m_csStatus.m_aryEnchantTime[ iID ] > 0)
			--m_csStatus.m_aryEnchantTime[ iID ];

	}
}

void		CRYLGameData::SetEnableMove( BOOL bEnableMove )
{
	m_bEnableMove = bEnableMove ;
}

BOOL		CRYLGameData::GetEnableMove( void )
{
	return m_bEnableMove ;
}

void		CRYLGameData::UpdateCamp( RYLCreature* pCreature )
{
	if ( pCreature )
	{
		if ( pCreature->GetSubState() == Siege::MINING_ON )
			pCreature->Attack();
	}
}

void	CRYLGameData::ChangeViewNameMode()
{
	if ( VIEW_NO_NAME == m_dwViewNameType )
	{
		m_dwViewNameType = VIEW_ALL_NAME ;
	}
	else
	{
		++m_dwViewNameType ;
	}

	switch (m_dwViewNameType)
	{
		case VIEW_ALL_NAME :
			CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[ 3570 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
			break;

		case VIEW_PC_NAME :
			CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[ 3571 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
			break;

		case VIEW_OTHER_NAME :
			CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[ 3572 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
			break;

		case VIEW_NO_NAME :
			CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[ 3573 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
			break;
	}

	vector<unsigned long> vecCharID;
	RYLCreatureManager::Instance()->GetCharIDArray( vecCharID );
	unsigned int unCount = 0;

	std::vector<unsigned long>::iterator pos = vecCharID.begin();
	std::vector<unsigned long>::iterator end = vecCharID.end();

	for(; pos != end; ++pos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*pos); 
		if ( NULL == pTempCreature )
			continue;

		if ( VIEW_NO_NAME == m_dwViewNameType )
		{
			pTempCreature->SetViewName( false );
		}
		else
		{
			switch ( GetClientIdentity(*pos) )
			{
				case Creature::CT_PC :
				{
					if ( VIEW_PC_NAME == m_dwViewNameType || VIEW_ALL_NAME == m_dwViewNameType )
					{
						pTempCreature->SetViewName( true );
					}
					else
					{
						pTempCreature->SetViewName( false );
					}
				}
				break;
				
				case Creature::CT_NPC :
				case Creature::CT_MONSTER :
				case Creature::CT_SUMMON	:
				case Creature::CT_STRUCT	:
				case Creature::CT_SIEGE_OBJECT :
				{
					if ( VIEW_OTHER_NAME == m_dwViewNameType || VIEW_ALL_NAME == m_dwViewNameType )
					{
						pTempCreature->SetViewName( true );
					}
					else
					{
						pTempCreature->SetViewName( false );
					}
				}
				break;

				case Creature::CT_NONE_TYPE :
				{
					pTempCreature->SetViewName( false );
				}
				break;
			}
		}
	}
}

using namespace Broadcast2nd;
const CNetworkEquipGrade CRYLGameData::GetNetworkEquipGrade(void)
{
	Item::CItem* lpHead = (NULL != m_csStatus.m_lstEquipment[Item::EquipmentPos::HELM]) ? 
		m_csStatus.m_lstEquipment[Item::EquipmentPos::HELM]->m_lpItemBase : NULL;

	Item::CItem* lpBody = (NULL != m_csStatus.m_lstEquipment[Item::EquipmentPos::ARMOUR]) ? 
		m_csStatus.m_lstEquipment[Item::EquipmentPos::ARMOUR]->m_lpItemBase : NULL;

	Item::CItem* lpRightHand = (NULL != m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + m_csStatus.m_cWeaponPos]) ? 
		m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1 + m_csStatus.m_cWeaponPos]->m_lpItemBase : NULL;

	Item::CItem* lpLeftHand = (NULL != m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1 + m_csStatus.m_cWeaponPos]) ? 
		m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1 + m_csStatus.m_cWeaponPos]->m_lpItemBase : NULL;

	CNetworkEquipGrade equipGrade(
		CNetworkEquipGrade::TransferNetworkEquipGrade(lpHead), 
		CNetworkEquipGrade::TransferNetworkEquipGrade(lpBody), 
		CNetworkEquipGrade::TransferNetworkEquipGrade(lpRightHand), 
		CNetworkEquipGrade::TransferNetworkEquipGrade(lpLeftHand));

	return equipGrade;
}

void BattleInclination::SetCharData(RYLCreature* pCreature, CharDataNode* pDataNode, 
								  BattleInclination::CharData& charData, bool bSetRideArmsInfo)
{	
	charData.m_dwCID			= pCreature->GetCharID();		// ĳ���� CID
	charData.m_dwGID			= pDataNode->m_ulGuildID;		// ĳ���� GID    
	if (0 != (charData.m_dwGID & Guild::MEMBER_WAIT_BIT))
	{
		charData.m_dwGID		= 0;							// ��� ���� �����(��û��)�� GID�� 0�� ����.
	}
	charData.m_dwCastleID		= 0;							// ���� ������ ���, ���� ���� ID�� ����.
	charData.m_cNation			= pDataNode->m_cNation;			// ĳ���� ����
	charData.m_cSiegeState		= 0;							// ���� ������ ���, ���� ���� ���¸� ����.
	charData.m_cFlags			= 0;							// ��Ÿ ����
	charData.m_cAdminLevel		= 0;
	charData.m_dwPID			= 0;	
	charData.m_wObjectType		= 0;
	charData.m_wKind			= 0;
	charData.m_dwPID			= pDataNode->m_ulPartyID;	
	charData.m_cCreatureType	= Creature::GetCreatureType(pCreature->GetCharID());
	charData.m_cTacticsFlag		= pCreature->GetTactics();
	charData.m_cRealmWarFlag	= pCreature->GetRealmWarFlag() ;
	charData.m_cGuildWarFlag	= pCreature->GetGuildWarFlag() ;
		
	charData.SetPeaceMode(pDataNode->m_bPeaceMode);
	charData.SetSafetyZone(CRYLGameData::Instance()->IsInSafetyZone(pCreature));	

	bool bHasDevelopingCamp, bHasCompleteCamp;
	CRYLCampManager::Instance()->HasCampByGID(pDataNode->m_ulGuildID, bHasDevelopingCamp, bHasCompleteCamp);
	charData.SetHasDevelopingCamp(bHasDevelopingCamp);
	charData.SetHasCamp(bHasCompleteCamp);

	charData.SetGODMode(pCreature->GetEffect()->GetChantStatus(Skill::SpellID::Invincible));

	switch (charData.m_cCreatureType)
	{
		case Creature::CT_SIEGE_OBJECT:
		{						
			charData.m_cSiegeState	= pCreature->GetState();
			charData.m_wObjectType	= pCreature->GetObjectType();						
			charData.SetCastleObject(pCreature->GetIsCastleObject());
			charData.SetCastleNPC(pCreature->GetIsCastleNPC());			
			charData.m_dwCastleID	= pCreature->GetCastleID();

			// ��� ����� ������ ����� �������� �����̴�.
			charData.m_cGuildWarFlag = Creature::WAR_ON ;

			CRYLGameScene*		pScene	= (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLGuildFrameDlg*	pDlg	= (CRYLGuildFrameDlg*)pScene->GetGuildFrameDlg() ;
			CRYLGuildInfoPanel*	pPanel	= (CRYLGuildInfoPanel*)pDlg->GetGuildInfoPanel() ;

			LPGUILDLARGEINFOEX lpGuildInfo = pPanel->FindGuild( pDataNode->m_ulGuildID ) ;

			if (lpGuildInfo)
			{
				charData.m_cNation = lpGuildInfo->m_cInclination;
			}
			break;
		}

		case Creature::CT_PC:
		{			
			// Rodin : ����Ʋ�� �������ΰ��� ���� �÷��� �ʿ�

			if (bSetRideArmsInfo && pCreature->GetRideEnable())
			{
				charData.m_cSiegeState	= pCreature->GetState();
				charData.m_wObjectType	= pCreature->GetObjectType();
				charData.m_dwCastleID	= pCreature->GetCastleID();
				charData.SetCastleObject(pCreature->GetIsCastleObject());
				charData.SetCastleNPC(pCreature->GetIsCastleNPC());				
			}
			break;
		}

		case Creature::CT_MONSTER:
		{			
			const CMonsterMgr::MonsterProtoType* lpMonster = g_MonsterMgr.GetMonsterProtoType(charData.m_dwCID & 0x0000FFFF);

			if (lpMonster)
			{
				charData.m_wKind = lpMonster->m_MonsterInfo.m_dwKID;
			}			
			break;
		}
	}
}

void BattleInclination::SetRelationData(RYLCreature* pOwnerCreature, RYLCreature* pTargetCreature, 
										BattleInclination::RelationData& relationData)
{
	relationData.m_cVSFlag			= 0;
	relationData.m_bHostileGuild	= false;

	if (Creature::CT_PC == Creature::GetCreatureType(pOwnerCreature->GetCharID()) && 
		Creature::CT_PC == Creature::GetCreatureType(pTargetCreature->GetCharID()))
	{
		relationData.m_cVSFlag = g_CharacterData.GetIsDuelTarget(pTargetCreature->GetCharID());
	}


	CharDataNode* pOwnerData = RYLCharacterDataManager::Instance()->GetCharData(pOwnerCreature->GetCharID());
	CharDataNode* pTargetData = RYLCharacterDataManager::Instance()->GetCharData(pTargetCreature->GetCharID());

	// edith 2009.08.20 ���� ��尡 ���ų� ��� ������̸� ���뼱���� �ȵȴ�.
	if(pOwnerData && (pOwnerData->m_ulGuildID == 0 || pOwnerData->m_ulGuildID & Guild::MEMBER_WAIT_BIT))
		return;
	if(pTargetData && (pTargetData->m_ulGuildID == 0 || pTargetData->m_ulGuildID & Guild::MEMBER_WAIT_BIT))
		return;

    // pOwnerCreature �� �׻� SelfCreature ���߸� �Ѵ�.!!
	// �ڽ� �̿��� �ٸ� ũ������ ���� ������ ���ü� ���� �����̴�..
	if ( pOwnerData && 0 != pOwnerData->m_ulGuildID &&
			pTargetData && 0 != pTargetData->m_ulGuildID )
	{
		CRYLGameScene*		pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLGuildFrameDlg*	pDlg	= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
		CRYLGuildInfoPanel* pPanel	= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;

		if(pPanel->IsHostilityGuild( pTargetData->m_ulGuildID ))
			relationData.m_bHostileGuild = true;

		if(pPanel->IsHostilityGuild( pOwnerData->m_ulGuildID ))
			relationData.m_bHostileGuild = true;
	}
}


