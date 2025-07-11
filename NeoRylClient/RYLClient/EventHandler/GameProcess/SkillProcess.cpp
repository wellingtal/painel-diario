
#include "../GameHandler.h"
#include "../../GUITextEdit.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/ClientSocket/Parse/ParseSkill.h>
#include <Network/ClientSocket/Send/SendCommunity.h>

#include <Creature/Monster/MonsterMgr.h>
#include <Creature/EnemyCheck.h>

#include <Skill/SkillMgr.h>

#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"

#include "RYLNetWorkData.h"
#include "RYLGameData.h"
#include "RYLClientMain.h"
#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLHumanInventoryFrameDlg.h"
#include "RYLChattingDlg.h"
#include "RYLSkillPanel.h"
#include "RYLAbilityPanel.h"

#include "BroadcastProcess.h"
#include "SkillProcess.h"
#include "GMMemory.h"

#include "RYLJoystickConfigurePanel.h"
#include "RYLJoystickConfigureDlg.h"

GAME_EVENT_FUNC(ParseCharSkillLock);
GAME_EVENT_FUNC(ParseCharSkillUnLock);
GAME_EVENT_FUNC(ParseCharSkillErase);
GAME_EVENT_FUNC(ParseCharSkillCreate);
GAME_EVENT_FUNC(ParseCharSummonCmd);
GAME_EVENT_FUNC(ParseCharSummon);
GAME_EVENT_FUNC(ParseCharSpellInfo);

bool GameProcess::RegisterSkillHandler(CGameEventHandler& GameEventHandler)
{
    unsigned long dwErrorCount = 0;
    unsigned long dwIgnoreNonPlayPacket = !(BIT(CLIENT_GAME) | BIT(CLIENT_GAMELOADING));

    dwErrorCount += GameEventHandler.AddHandler(CmdCharSkillLock, ParseCharSkillLock) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharSkillUnLock, ParseCharSkillUnLock) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharSkillErase, ParseCharSkillErase) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharSkillCreate, ParseCharSkillCreate) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharSummonCmd, ParseCharSummonCmd) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharSummon, ParseCharSummon) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCharSpellInfo, ParseCharSpellInfo, dwIgnoreNonPlayPacket) ? 0 : 1;

    return 0 == dwErrorCount;
}

GAME_EVENT_FUNC(ParseCharSkillLock)
{
	unsigned long   dwChrID = 0;
	unsigned short  wSkill = 0;
	unsigned char   cIndex = 0;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSkillLock(lpPktBase, &dwChrID, &wSkill, &cIndex);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (CRYLNetworkData::Instance()->m_dwMyChrID == dwChrID)
		{
			if(wSkill >= 0x8000)
			{
				if (pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill == wSkill)
				{
					if(3 != pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount ||
						6 != pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel)
					{
						pGame->m_csStatus.m_Skill.wSkillNum++;
					}
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount++;
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel = 0;

					if (pGame->m_csStatus.m_lstSkillSlot[cIndex])
					{
						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						long 			lPrevCoolDownTime		= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTime ;
						long 			lPrevCoolDownTimeCount	= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTimeCount ;
						unsigned char	cPrevCountCooldown		= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_cCountCooldown ;

						if ( pGame->m_csStatus.m_lstSkillSlot[cIndex] )
						{
							delete pGame->m_csStatus.m_lstSkillSlot[cIndex] ;
							pGame->m_csStatus.m_lstSkillSlot[cIndex] = NULL ;
						}
						
						pGame->m_csStatus.m_lstSkillSlot[cIndex] = new CItemInstance;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->SetSkillInfo(wSkill,
							pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
							pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
							TakeType::TS_SSLOT, cIndex, 0);

						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTime		= lPrevCoolDownTime ;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTimeCount	= lPrevCoolDownTimeCount ;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_cCountCooldown		= cPrevCountCooldown ;

						char strText[MAX_PATH];
						sprintf(strText, CRYLStringTable::m_strString[287], pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetItemInfo().m_SpriteData.m_szName);
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

						CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLSkillPanel* pPanel		     = ( CRYLSkillPanel* )pDlg->GetSkillPanel() ;
						
						pPanel->SetBindingItem( pGame->m_csStatus.m_lstSkillSlot[cIndex], cIndex ) ;
						// ��ų ������Ʈ
						pPanel->SetSkillSlot( pGame->m_csStatus.m_lstSkillSlot[cIndex], EDIT_SKILL, cIndex ) ;

						//======================================================================================
						// ���ͳ�Ƽ�� ��ų�� �ִ°�? ������ ������Ʈ ���ش�.
						const Skill::ProtoType *lpSkillBase		= g_SkillMgr.GetSkillProtoType( pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetProtoTypeID() ) ;
						const Skill::ProtoType *lpChildSkillBase= g_SkillMgr.GetSkillProtoType( lpSkillBase->m_usChildSkill ) ;

						if ( lpChildSkillBase )
						{
							unsigned short nCount = pPanel->FindVirtualIndex( lpSkillBase->m_usChildSkill ) ;

							delete pPanel->m_lstDisplaySkillSlot[ nCount ]->lpItem ;
							pPanel->m_lstDisplaySkillSlot[ nCount ]->lpItem = new CItemInstance ;

							CItemInstance* pItem = pPanel->m_lstDisplaySkillSlot[ nCount ]->lpItem ;
							pItem->SetSkillInfo( lpSkillBase->m_usChildSkill,
												pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
												pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
												TakeType::TS_SSLOT, cIndex, ALTERNATIVESKILL ) ;

							pItem->m_lCoolDownTime		= lPrevCoolDownTime ;
							pItem->m_lCoolDownTimeCount	= lPrevCoolDownTimeCount ;
							pItem->m_cCountCooldown		= cPrevCountCooldown ;

							sprintf(strText, CRYLStringTable::m_strString[287], pItem->GetItemInfo().m_SpriteData.m_szName ) ;
							pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

							// ��ų ������Ʈ
							pPanel->SetSkillSlot( pItem, EDIT_SKILL, nCount, true ) ;
						}

						pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

					}

					// �� ���� ���� ��ȭ
					const Skill::ProtoType *lpSkillBase    = g_SkillMgr.GetSkillProtoType( wSkill ) ;
					unsigned short usChildID = 0 ;
					if ( lpSkillBase )
					{
						usChildID  = lpSkillBase->m_usChildSkill ;
					}

					long lCoolTime;
					for(int i = 0; i < QUICK::MAX_QUICK_NUM; i++)
					{
						if (pGame->m_csStatus.m_lstQuickSlot[i] &&
							pGame->m_csStatus.m_lstQuickSlot[i]->m_lpItemBase->GetPrototypeID() == wSkill)
						{
							lCoolTime = pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount;

							BOOL bSelected = false;
							if (pGame->m_csStatus.m_lpQuickSelected == pGame->m_csStatus.m_lstQuickSlot[i])
							{
								bSelected = true;
								CRYLNetworkData::SetChant(NULL);
								pGame->m_csStatus.SetActivateSkill(NULL);
							}

							// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
							long 			lPrevCoolDownTime		= pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTime ;
							long 			lPrevCoolDownTimeCount	= pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount ;
							unsigned char	cPrevCountCooldown		= pGame->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown ;

							delete pGame->m_csStatus.m_lstQuickSlot[i];
							pGame->m_csStatus.m_lstQuickSlot[i] = new CItemInstance;
							pGame->m_csStatus.m_lstQuickSlot[i]->SetSkillInfo(wSkill,
								pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
								pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
								TakeType::TS_QSLOT, i, lCoolTime);

							// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
							pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTime		= lPrevCoolDownTime ;
							pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount	= lPrevCoolDownTimeCount ;
							pGame->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown		= cPrevCountCooldown ;

							if (bSelected)
							{
								CRYLNetworkData::SetChant(pGame->m_csStatus.m_lstQuickSlot[i]);
								pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[i]);
							}
						}	
						else if ( pGame->m_csStatus.m_lstQuickSlot[i] &&  
								pGame->m_csStatus.m_lstQuickSlot[i]->m_lpItemBase->GetPrototypeID() == usChildID )
						{
							lCoolTime = pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount;

							BOOL bSelected = false;
							if (pGame->m_csStatus.m_lpQuickSelected == pGame->m_csStatus.m_lstQuickSlot[i])
							{
								bSelected = true;
								CRYLNetworkData::SetChant(NULL);
								pGame->m_csStatus.SetActivateSkill(NULL);
							}

							// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
							long 			lPrevCoolDownTime		= pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTime ;
							long 			lPrevCoolDownTimeCount	= pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount ;
							unsigned char	cPrevCountCooldown		= pGame->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown ;

							delete pGame->m_csStatus.m_lstQuickSlot[i];
							pGame->m_csStatus.m_lstQuickSlot[i] = new CItemInstance;
							pGame->m_csStatus.m_lstQuickSlot[i]->SetSkillInfo( usChildID,
																			pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
																			pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
																			TakeType::TS_QSLOT, i, lCoolTime);

							// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
							pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTime		= lPrevCoolDownTime ;
							pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount	= lPrevCoolDownTimeCount ;
							pGame->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown		= cPrevCountCooldown ;

							if (bSelected)
							{
								CRYLNetworkData::SetChant(pGame->m_csStatus.m_lstQuickSlot[i]);
								pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[i]);
							}
						}
					}
				}

				pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
			}
			// �����Ƽ���..
			else if(0x1000 <= wSkill && wSkill < 0x2000 )
			{
				if (pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill == wSkill)
				{
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount++;
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel = 1;

					if (pGame->m_csStatus.m_lstSkillSlot[cIndex])
					{
						CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLAbilityPanel* pPanel		 = ( CRYLAbilityPanel* )pDlg->GetAbilityPanel() ;

						// �켱 ���� ��ų�� ��ġ�� �˻��ؼ� �����Ѵ�.
						// �� ������ �Ʒ����� pGame->m_csStatus.m_lstSkillSlot �޸𸮸� �����ϰ�
						// ���� ����µ� FindVirtualIndex�� SkillSlot�� �޸𸮸� ������ �־ ��ȿ���� �ʴ� 
						// �޸� ������ �Ͼ��.
						int iIndex = pPanel->FindVirtualIndex(wSkill);
						pPanel->SetProcessAbilityIndex(iIndex);

						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						long 			lPrevCoolDownTime		= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTime ;
						long 			lPrevCoolDownTimeCount	= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTimeCount ;
						unsigned char	cPrevCountCooldown		= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_cCountCooldown ;

						if ( pGame->m_csStatus.m_lstSkillSlot[cIndex] )
						{
							delete pGame->m_csStatus.m_lstSkillSlot[cIndex] ;
							pGame->m_csStatus.m_lstSkillSlot[cIndex] = NULL ;
						}
						
						pGame->m_csStatus.m_lstSkillSlot[cIndex] = new CItemInstance;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->SetSkillInfo(wSkill,
							pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
							pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
							TakeType::TS_SSLOT, cIndex, 0);

						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTime		= lPrevCoolDownTime ;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTimeCount	= lPrevCoolDownTimeCount ;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_cCountCooldown		= cPrevCountCooldown ;

						char strText[MAX_PATH];
						sprintf(strText, CRYLStringTable::m_strString[287], pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetItemInfo().m_SpriteData.m_szName);
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

						pPanel->SetBindingItem( pGame->m_csStatus.m_lstSkillSlot[cIndex], cIndex ) ;
						// ��ų ������Ʈ
						pPanel->SetAbilitySlot( pGame->m_csStatus.m_lstSkillSlot[cIndex], EDIT_ABILITY, cIndex ) ;

						pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

						// ��ų���� ��ų�� �ٽ� �������ش�.
						pGame->m_csStatus.SetClassSkill();
						pGame->m_csStatus.CheckGratherQuickSlot();
					}
				}

				pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
			}

		}
		return true;
	} else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// 2 = �ش� �ε����� ��ų ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[329]);
				break;

			case 3:					// 3 = ��ų�� �� �� �ʰ� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[330]);
				break;

			case 4:					// 4 = �� �Ҽ� ���� ��ų ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[331]);
				break;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharSkillUnLock)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID;
	unsigned short	wSkill;
	unsigned char	cIndex;
	unsigned char	cSkillLevel;
	Item::ItemPos	pItemIndex;

	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSkillUnLock(lpPktBase, &dwChrID, &wSkill, &cIndex, &cSkillLevel, &pItemIndex);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (CRYLNetworkData::Instance()->m_dwMyChrID == dwChrID)
		{
			if (pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill == wSkill)
			{
				if(wSkill >= 0x8000)
				{
					if(4 != pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount ||
						0 != pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel)
					{
						pGame->m_csStatus.m_Skill.wSkillNum--;
					}
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount--;
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel = cSkillLevel;

					if (pGame->m_csStatus.m_lstSkillSlot[cIndex])
					{
						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						long			lPrevCoolDownTime		= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTime ;
						long 			lPrevCoolDownTimeCount	= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTimeCount ;
						unsigned char	cPrevCountCooldown		= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_cCountCooldown ;

						if ( pGame->m_csStatus.m_lstSkillSlot[cIndex] )
						{
							delete pGame->m_csStatus.m_lstSkillSlot[cIndex] ;
							pGame->m_csStatus.m_lstSkillSlot[cIndex] = NULL ;
						}

						pGame->m_csStatus.m_lstSkillSlot[cIndex] = new CItemInstance;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->SetSkillInfo(pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill,
							pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
							pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
							TakeType::TS_SSLOT, cIndex, 0);

						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTime		= lPrevCoolDownTime ;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTimeCount	= lPrevCoolDownTimeCount ;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_cCountCooldown		= cPrevCountCooldown ;

						char strText[MAX_PATH];
						sprintf(strText, CRYLStringTable::m_strString[385], pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetItemInfo().m_SpriteData.m_szName);
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

						CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLSkillPanel* pPanel		     = ( CRYLSkillPanel* )pDlg->GetSkillPanel() ;

						pPanel->SetBindingItem( pGame->m_csStatus.m_lstSkillSlot[cIndex], cIndex ) ;
						// ��ų ������Ʈ
						pPanel->SetSkillSlot( pGame->m_csStatus.m_lstSkillSlot[cIndex], DEL_SKILLSLOT, cIndex ) ;
						
						//======================================================================================
						// ���ͳ�Ƽ�� ��ų�� �ִ°�? ������ ������Ʈ ���ش�.
						const Skill::ProtoType *lpSkillBase		= g_SkillMgr.GetSkillProtoType( pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetProtoTypeID() ) ;

						if(lpSkillBase	)
						{
							const Skill::ProtoType *lpChildSkillBase= g_SkillMgr.GetSkillProtoType( lpSkillBase->m_usChildSkill ) ;

							if ( lpChildSkillBase )
							{
								unsigned short nCount = pPanel->FindVirtualIndex( lpSkillBase->m_usChildSkill ) ;

								delete pPanel->m_lstDisplaySkillSlot[ nCount ]->lpItem ;
								pPanel->m_lstDisplaySkillSlot[ nCount ]->lpItem = new CItemInstance ;

								CItemInstance* pItem = pPanel->m_lstDisplaySkillSlot[ nCount ]->lpItem ;
								pItem->SetSkillInfo( lpSkillBase->m_usChildSkill,
									pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
									pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
									TakeType::TS_SSLOT, cIndex, ALTERNATIVESKILL ) ;

								pItem->m_lCoolDownTime		= lPrevCoolDownTime ;
								pItem->m_lCoolDownTimeCount	= lPrevCoolDownTimeCount ;
								pItem->m_cCountCooldown		= cPrevCountCooldown ;

								sprintf(strText, CRYLStringTable::m_strString[385], pItem->GetItemInfo().m_SpriteData.m_szName ) ;
								pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

								// ��ų ������Ʈ
								pPanel->SetSkillSlot( pItem, DEL_SKILLSLOT, cIndex + 1, true ) ;
							}
						}
						
						pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
					}
				}
				else if(0x1000 <= wSkill && wSkill < 0x2000)
				{
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount--;
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel = 1;

					if (pGame->m_csStatus.m_lstSkillSlot[cIndex])
					{
						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						long			lPrevCoolDownTime		= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTime ;
						long 			lPrevCoolDownTimeCount	= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTimeCount ;
						unsigned char	cPrevCountCooldown		= pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_cCountCooldown ;

						if ( pGame->m_csStatus.m_lstSkillSlot[cIndex] )
						{
							delete pGame->m_csStatus.m_lstSkillSlot[cIndex] ;
							pGame->m_csStatus.m_lstSkillSlot[cIndex] = NULL ;
						}

						pGame->m_csStatus.m_lstSkillSlot[cIndex] = new CItemInstance;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->SetSkillInfo(pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill,
							pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
							pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
							TakeType::TS_SSLOT, cIndex, 0);

						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTime		= lPrevCoolDownTime ;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_lCoolDownTimeCount	= lPrevCoolDownTimeCount ;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->m_cCountCooldown		= cPrevCountCooldown ;

						char strText[MAX_PATH];
						sprintf(strText, CRYLStringTable::m_strString[385], pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetItemInfo().m_SpriteData.m_szName);
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

						CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLAbilityPanel* pPanel		 = ( CRYLAbilityPanel* )pDlg->GetAbilityPanel() ;

						pPanel->SetBindingItem( pGame->m_csStatus.m_lstSkillSlot[cIndex], cIndex ) ;
						// ��ų ������Ʈ
						pPanel->SetAbilitySlot( pGame->m_csStatus.m_lstSkillSlot[cIndex], DEL_ABILITYSLOT, cIndex ) ;
						
						pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

						// ��ų����� ��ų�� �ٽ� �������ش�.
						pGame->m_csStatus.SetClassSkill();
						pGame->m_csStatus.CheckGratherQuickSlot();
					}

				}
				// edith 2008.02.14 ��ų�� �����Ҷ� ������ ���� ��������Ƿ� �������� 1 ���δ�.
				// �̰� ��ų�� ���������
				// ������ ����
				if(pItemIndex.m_cPos == TakeType::TS_INVEN)
				{
					// ������ ����
					CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItem(pItemIndex.m_cIndex);
					if (lpItem)
					{
						unsigned long dwNum = lpItem->GetNowDurability() - 1;
						lpItem->SetNowDurability(dwNum);
						if (lpItem->GetNowDurability() <= 0)
						{
							pGame->m_csStatus.DeleteInventoryItem(lpItem);
							delete lpItem;
							lpItem = NULL;
						} 
						else
						{
							pGame->m_csStatus.SetQuickSlot(lpItem);
						}
					} 
				}

				// �� ���� ���� ��ȭ
				const Skill::ProtoType *lpSkillBase    = g_SkillMgr.GetSkillProtoType( wSkill ) ;

				unsigned short usChildID = 0 ;

				if ( lpSkillBase )
				{
					usChildID  = lpSkillBase->m_usChildSkill ;
				}

				long lCoolTime;

				for(int i = 0; i < QUICK::MAX_QUICK_NUM; i++)
				{
					if (pGame->m_csStatus.m_lstQuickSlot[i] && pGame->m_csStatus.m_lstQuickSlot[i]->m_lpItemBase->GetPrototypeID() == wSkill)
					{
						lCoolTime = pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount;

						BOOL bSelected = false;
						if (pGame->m_csStatus.m_lpQuickSelected == pGame->m_csStatus.m_lstQuickSlot[i])
						{
							bSelected = true;
							CRYLNetworkData::SetChant(NULL);
							pGame->m_csStatus.SetActivateSkill(NULL);
						}

						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						long 			lPrevCoolDownTime		= pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTime ;
						long 			lPrevCoolDownTimeCount	= pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount ;
						unsigned char	cPrevCountCooldown		= pGame->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown ;

						delete pGame->m_csStatus.m_lstQuickSlot[i];
						pGame->m_csStatus.m_lstQuickSlot[i] = new CItemInstance;
						pGame->m_csStatus.m_lstQuickSlot[i]->SetSkillInfo(pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill,
							pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
							pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
							TakeType::TS_QSLOT, i, lCoolTime);

						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTime		= lPrevCoolDownTime ;
						pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount	= lPrevCoolDownTimeCount ;
						pGame->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown		= cPrevCountCooldown ;

						if (bSelected)
						{
							CRYLNetworkData::SetChant(pGame->m_csStatus.m_lstQuickSlot[i]);
							pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[i]);
						}
					}
					else if ( pGame->m_csStatus.m_lstQuickSlot[i] &&  
							pGame->m_csStatus.m_lstQuickSlot[i]->m_lpItemBase->GetPrototypeID() == usChildID )
					{
						lCoolTime = pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount;

						BOOL bSelected = false;
						if (pGame->m_csStatus.m_lpQuickSelected == pGame->m_csStatus.m_lstQuickSlot[i])
						{
							bSelected = true;
							CRYLNetworkData::SetChant(NULL);
							pGame->m_csStatus.SetActivateSkill(NULL);
						}

						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						long 			lPrevCoolDownTime		= pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTime ;
						long 			lPrevCoolDownTimeCount	= pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount ;
						unsigned char	cPrevCountCooldown		= pGame->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown ;

						delete pGame->m_csStatus.m_lstQuickSlot[i];
						pGame->m_csStatus.m_lstQuickSlot[i] = new CItemInstance;
						pGame->m_csStatus.m_lstQuickSlot[i]->SetSkillInfo( usChildID,
																		pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
																		pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
																		TakeType::TS_QSLOT, i, lCoolTime);

						// ���� ��ų�� ������̶� ��ٿ� Ÿ�ӿ� ������ �ټ� �ֱ� ������ ���� ������ ����ؼ� �ٽ� �������ش�.
						pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTime		= lPrevCoolDownTime ;
						pGame->m_csStatus.m_lstQuickSlot[i]->m_lCoolDownTimeCount	= lPrevCoolDownTimeCount ;
						pGame->m_csStatus.m_lstQuickSlot[i]->m_cCountCooldown		= cPrevCountCooldown ;

						if (bSelected)
						{
							CRYLNetworkData::SetChant(pGame->m_csStatus.m_lstQuickSlot[i]);
							pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[i]);
						}
					}
				}
			}

			pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
		}
		return true;
	} 
	else
	{
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharSkillErase)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long	dwChrID;
	unsigned short	wSkill;
	unsigned char	cIndex;
	Item::ItemPos	pItemIndex;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSkillErase(lpPktBase, &dwChrID, &wSkill, &cIndex, &pItemIndex);

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		if ( CRYLNetworkData::Instance()->m_dwMyChrID == dwChrID )
		{
			if ( pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill == wSkill )
			{
				if(wSkill >= 0x8000)
				{
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel -- ;
					pGame->m_csStatus.m_Skill.wSkillNum -- ;

					if ( pGame->m_csStatus.m_lstSkillSlot[ cIndex ] )
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLSkillPanel* pPanel = ( CRYLSkillPanel* )pDlg->GetSkillPanel() ;

						unsigned long dwNum = pGame->m_csStatus.m_lstSkillSlot[ cIndex ]->GetNowDurability() - 1 ;
						pGame->m_csStatus.m_lstSkillSlot[ cIndex ]->SetNowDurability( dwNum ) ;
						
						const Skill::ProtoType *lpSkillBase = 
							g_SkillMgr.GetSkillProtoType( pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetProtoTypeID() ) ;

						if (0 != lpSkillBase)
						{
							const Skill::ProtoType *lpChildSkillBase = 
								g_SkillMgr.GetSkillProtoType( lpSkillBase->m_usChildSkill ) ;

							if (0 != lpChildSkillBase)
							{
								CItemInstance* lpAlterSkill = pPanel->FindVirtualSkill( lpChildSkillBase->m_usSkill_ID );
								if (0 != lpAlterSkill)
								{
									lpAlterSkill->SetNowDurability(lpAlterSkill->GetNowDurability() - 1);
								}
							}
						}

						// ��ų ������Ʈ
						pPanel->SetSkillSlot( pGame->m_csStatus.m_lstSkillSlot[ cIndex ], DEL_SKILLSLOT ) ;
					}
				}
				else if(0x1000 <= wSkill && wSkill < 0x2000)
				{
					// �����Ƽ ���� ����
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount = 0;
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel = 0;

					// �׳� ���ŷ�ƾ (�����Ƽ�� ��� �ȴ�.)
					if ( pGame->m_csStatus.m_lstSkillSlot[ cIndex ] )
					{
						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLAbilityPanel* pPanel = ( CRYLAbilityPanel* )pDlg->GetAbilityPanel() ;
		
						const Skill::ProtoType *lpSkillBase = 
							g_SkillMgr.GetSkillProtoType( pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetProtoTypeID() ) ;

						// ��ų ������Ʈ
						pPanel->SetAbilitySlot( pGame->m_csStatus.m_lstSkillSlot[ cIndex ], DEL_ABILITYSLOT ) ;
					}
				}

				// edith 2008.02.14 ��ų�� �����Ҷ� ������ ���� ��������Ƿ� �������� 1 ���δ�.
				// �̰� ��ų�����Ҷ�
				// ������ ����
				if(pItemIndex.m_cPos == TakeType::TS_INVEN)
				{
					// ������ ����
					CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItem(pItemIndex.m_cIndex);
					if (lpItem)
					{
						unsigned long dwNum = lpItem->GetNowDurability() - 1;
						lpItem->SetNowDurability(dwNum);
						if (lpItem->GetNowDurability() <= 0)
						{
							pGame->m_csStatus.DeleteInventoryItem(lpItem);
							delete lpItem;
							lpItem = NULL;
						} 
						else
						{
							pGame->m_csStatus.SetQuickSlot(lpItem);
						}
					} 
				}

				// �� ���� ���� ��ȭ
				const Skill::ProtoType *lpSkillBase    = g_SkillMgr.GetSkillProtoType( wSkill ) ;
				unsigned short usChildID = 0 ;
				if ( lpSkillBase )
				{
					usChildID  = lpSkillBase->m_usChildSkill ;
				}

				for ( int i = 0; i < QUICK::MAX_QUICK_NUM; i++ )
				{
					if ( pGame->m_csStatus.m_lstQuickSlot[ i ] && pGame->m_csStatus.m_lstQuickSlot[i]->m_lpItemBase->GetPrototypeID() == wSkill )
					{
						unsigned long dwNum = pGame->m_csStatus.m_lstQuickSlot[i]->GetNowDurability() - 1;
						pGame->m_csStatus.m_lstQuickSlot[i]->SetNowDurability(dwNum);
					}
					else if ( pGame->m_csStatus.m_lstQuickSlot[ i ] && 
							  pGame->m_csStatus.m_lstQuickSlot[ i ]->m_lpItemBase->GetPrototypeID() == usChildID )
					{
						unsigned long dwNum = pGame->m_csStatus.m_lstQuickSlot[i]->GetNowDurability() - 1;
						pGame->m_csStatus.m_lstQuickSlot[i]->SetNowDurability(dwNum);
					}
				}

				if ( !pGame->m_csStatus.m_Skill.SSlot[ cIndex ].SKILLINFO.cSkillLevel &&
				    !pGame->m_csStatus.m_Skill.SSlot[ cIndex ].SKILLINFO.cLockCount )
				{
					if ( pGame->m_csStatus.m_lpQuickSelected )
					{
						if ( pGame->m_csStatus.m_lpQuickSelected->m_lpItemBase->GetPrototypeID() == wSkill )
						{
							CRYLNetworkData::SetChant( NULL ) ;
							pGame->m_csStatus.SetActivateSkill( NULL ) ;
						}
					}
					for ( int i = 0 ; i < QUICK::MAX_QUICK_NUM ; i ++ )
					{
						if ( pGame->m_csStatus.m_lstQuickSlot[ i ] && pGame->m_csStatus.m_lstQuickSlot[ i ]->m_lpItemBase->GetPrototypeID() == wSkill )
						{
                            CRYLNetworkData::SendQuickSlotMove( pGame->m_csStatus.m_lstQuickSlot[ i ], TakeType::TS_NONE, 0 ) ;

							if(pGame->m_csStatus.m_lstQuickSlot[ i ] == pGame->m_csStatus.m_lpQuickSelected)
							{
								CRYLNetworkData::SetChant( NULL ) ;
								pGame->m_csStatus.SetActivateSkill( NULL ) ;
							}
						}
						else if ( pGame->m_csStatus.m_lstQuickSlot[ i ] && pGame->m_csStatus.m_lstQuickSlot[ i ]->m_lpItemBase->GetPrototypeID() == usChildID )
						{
							CRYLNetworkData::SendQuickSlotMove( pGame->m_csStatus.m_lstQuickSlot[ i ], TakeType::TS_NONE, 0 ) ;

							if(pGame->m_csStatus.m_lstQuickSlot[ i ] == pGame->m_csStatus.m_lpQuickSelected)
							{
								CRYLNetworkData::SetChant( NULL ) ;
								pGame->m_csStatus.SetActivateSkill( NULL ) ;
							}
						}
					}
					
					if(wSkill >= 0x8000)
					{
						// ��ų ����
						CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLSkillPanel* pPanel		     = ( CRYLSkillPanel* )pDlg->GetSkillPanel() ;
						pPanel->DeleteCurrentSkillSlot() ;
					}
					else if(0x1000 <= wSkill && wSkill < 0x2000)
					{
						// ��ų ����
						CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLAbilityPanel* pPanel		 = ( CRYLAbilityPanel* )pDlg->GetAbilityPanel() ;
						pPanel->DeleteCurrentAbilitySlot() ;
					}

					delete pGame->m_csStatus.m_lstSkillSlot[ cIndex ] ;
					pGame->m_csStatus.m_lstSkillSlot[ cIndex ] = NULL ;

					for ( int j = cIndex; j < SKILL::MAX_SLOT_NUM - 1; j++ )
					{
						pGame->m_csStatus.m_Skill.SSlot[ j ].dwSkillSlot = pGame->m_csStatus.m_Skill.SSlot[ j + 1 ].dwSkillSlot ;
						pGame->m_csStatus.m_lstSkillSlot[ j ] = pGame->m_csStatus.m_lstSkillSlot[ j + 1 ] ;
					}
					pGame->m_csStatus.m_Skill.SSlot[ SKILL::MAX_SLOT_NUM - 1 ].dwSkillSlot = 0 ;

					pGame->m_csStatus.m_Skill.wSlotNum -- ;

					pGame->m_csStatus.m_lstSkillSlot[ SKILL::MAX_SLOT_NUM - 1 ] = NULL ;
				}

				pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true) ;
				pGame->m_csStatus.SetClassSkill();
				pGame->m_csStatus.CheckGratherQuickSlot();
			}
		}
		return true ;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case 2:					// 2 = �ش� �ε����� ��ų ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[332]);
				break;

			case 3:					// 3 = ��ų ����� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[333]);
				break;

			case 4:					// 4 = 5�ܰ� ��ų�� ������ 4�ܰ�� ������ ����.
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[4235]);
				break;

			case 5:					// 5 = Ŭ���� ��ų ���� �Ұ�
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1411]);
				break;

			case 6:					// 6 = ��û�� ��ġ�� ������(������ ��)�� ���� (Ȥ�� ������ �̻��ϰų� �������� ������ ���ſ� ������ ���)
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1412]);
				break;

			case 7:					// 7 = ��û�� ��ġ�� �ִ� �������� �ùٸ� ������(������ ��)�� �ƴ� ���
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1413]);
				break;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharSkillCreate)//when you learn a skill hayzohar
{
	unsigned long	dwChrID;
	unsigned short	wSkill;
	unsigned char	cIndex;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSkillCreate(lpPktBase, &dwChrID, &wSkill, &cIndex);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (CRYLNetworkData::Instance()->m_dwMyChrID == dwChrID)
		{
			if( pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill )
			{
				// ��ų�� ����������
				if (pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill == wSkill)
				{
					pGame->m_csStatus.m_Skill.wSkillNum++;
					pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel++;

					if (pGame->m_csStatus.m_lstSkillSlot[cIndex])
					{
						CRYLGameScene* pScene            = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLHumanInventoryFrameDlg* pDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
						CRYLSkillPanel* pPanel		     = ( CRYLSkillPanel* )pDlg->GetSkillPanel() ;

						char strText[MAX_PATH];
						sprintf(strText, CRYLStringTable::m_strString[288], pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetItemInfo().m_SpriteData.m_szName);
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						
						unsigned long dwNum = pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetNowDurability() + 1;
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->SetNowDurability(dwNum);
						pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

						const Skill::ProtoType *lpSkillBase = 
							g_SkillMgr.GetSkillProtoType( pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetProtoTypeID() ) ;

						if (0 != lpSkillBase)
						{
							const Skill::ProtoType *lpChildSkillBase = 
								g_SkillMgr.GetSkillProtoType( lpSkillBase->m_usChildSkill ) ;

							if (0 != lpChildSkillBase)
							{
								sprintf(strText, CRYLStringTable::m_strString[288], lpChildSkillBase->m_SpriteInfo.m_szName );
								pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);

								CItemInstance* lpAlterSkill = pPanel->FindVirtualSkill( lpChildSkillBase->m_usSkill_ID );
								if (0 != lpAlterSkill)
								{
									lpAlterSkill->SetNowDurability(lpAlterSkill->GetNowDurability() + 1);
								}
							}
						}

						// ��ų ������Ʈ
						pPanel->SetSkillSlot( pGame->m_csStatus.m_lstSkillSlot[ cIndex ], EDIT_SKILL, cIndex ) ;
					}

					// ������ ��ų ���� ��ȭ
					const Skill::ProtoType *lpSkillBase = g_SkillMgr.GetSkillProtoType( wSkill ) ;
					unsigned short usChildID = 0 ;
					if ( lpSkillBase )
					{
						usChildID  = lpSkillBase->m_usChildSkill ;
					}

					for(int i = 0; i < QUICK::MAX_QUICK_NUM; i++)
					{
						if ( pGame->m_csStatus.m_lstQuickSlot[i] && pGame->m_csStatus.m_lstQuickSlot[i]->m_lpItemBase->GetPrototypeID() == wSkill)
						{
							unsigned long dwNum = pGame->m_csStatus.m_lstQuickSlot[i]->GetNowDurability() + 1;
							pGame->m_csStatus.m_lstQuickSlot[i]->SetNowDurability(dwNum);
						}
						else if ( pGame->m_csStatus.m_lstQuickSlot[i] && pGame->m_csStatus.m_lstQuickSlot[i]->m_lpItemBase->GetPrototypeID() == usChildID )
						{
							unsigned long dwNum = pGame->m_csStatus.m_lstQuickSlot[i]->GetNowDurability() + 1;
							pGame->m_csStatus.m_lstQuickSlot[i]->SetNowDurability(dwNum);
						}
					}
				}
			} 
			else
			{
				pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill = wSkill;
				pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount = 0;
				pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel = 1;
	
				if(pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill >= 0x8000)
					pGame->m_csStatus.m_Skill.wSkillNum++;

				pGame->m_csStatus.m_Skill.wSlotNum++;

				if ( pGame->m_csStatus.m_lstSkillSlot[cIndex] )
				{
					delete pGame->m_csStatus.m_lstSkillSlot[cIndex] ;
					pGame->m_csStatus.m_lstSkillSlot[cIndex] = NULL ;
				}

				pGame->m_csStatus.m_lstSkillSlot[cIndex] = new CItemInstance;
				if ( pGame->m_csStatus.m_lstSkillSlot[cIndex] )
				{
					pGame->m_csStatus.m_lstSkillSlot[cIndex]->SetSkillInfo(
						pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.wSkill,
						pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cLockCount,
						pGame->m_csStatus.m_Skill.SSlot[cIndex].SKILLINFO.cSkillLevel,
						TakeType::TS_SSLOT, cIndex, 0);

					char strText[MAX_PATH];
					sprintf(strText, CRYLStringTable::m_strString[289], 
						pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetItemInfo().m_SpriteData.m_szName);
					pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
					
					const Skill::ProtoType *lpSkillBase = 
						g_SkillMgr.GetSkillProtoType( pGame->m_csStatus.m_lstSkillSlot[cIndex]->GetProtoTypeID() ) ;

					const Skill::ProtoType *lpChildSkillBase =
						g_SkillMgr.GetSkillProtoType( lpSkillBase->m_usChildSkill ) ;

					if ( lpChildSkillBase )
					{
						sprintf(strText, CRYLStringTable::m_strString[289], lpChildSkillBase->m_SpriteInfo.m_szName );
						pChat->AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128);						
					}
					
					pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);
					pGame->m_csStatus.SetClassSkill();
					pGame->m_csStatus.CheckGratherQuickSlot();
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
			case 2:					// 2 = ���Դ� ��ų �� �ʰ� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[334]);
				break;

			case 3:					// 3 = �ٸ� ���� ��ų ������ ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[335]);
				break;

			case 4:					// 4 = ���� ��ų ���� ���� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[336]);

				{
					char strText[MAX_PATH], strTemp[MAX_PATH];
					strcpy(strText, "[SkillErr Log] ");
					sprintf(strTemp, "Skill Create Failed = Level : %d, Int : %d", pGame->m_csStatus.m_Info.Level, pGame->m_csStatus.GetINT() );
					strcat(strText, strTemp);
					SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_ClientLog, strText, strlen(strText), 0);
				}
				break;

			case 5:					// 5 = �ش� Ŭ���� ��ų �ƴ� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[337]);
				break;

			case 6:					// 6 = ��ų ���� �������ͽ� �ʰ� 
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[338]);
				break;
		}
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharSummonCmd)
{
	unsigned long	dwChrID, dwTargetID;
	unsigned char	cCmd;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSummonCmd(lpPktBase, &dwChrID, &cCmd, &dwTargetID);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		switch (cCmd)
		{
        case PktSummonCmd::SUC_ATTACK:
			g_CharacterData.m_bSummonCmd = false;
			{
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwTargetID );

                if ( pTempData )
                {
                    char pstrText[ MAX_PATH ];
                    sprintf( pstrText, CRYLStringTable::m_strString[ 489 ], pTempData->m_strName.c_str() );
                    pChat->AddMessage( pstrText, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
                }
			}
			break;

        case PktSummonCmd::SUC_RETURN:
			pChat->AddMessage(CRYLStringTable::m_strString[490], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
			break;

        case PktSummonCmd::SUC_DIE:
			{
                RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwTargetID );

                if ( pTempCreature )
                {
                    pTempCreature->Dead();
                }

				if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
				{
					g_CharacterData.m_bSummon = false;
					g_CharacterData.m_bSummonCmd = false;
					pGame->m_csStatus.m_bSummonGuard = false;
				}
			}
			break;

        case PktSummonCmd::SUC_GUARD_ON:
            {
			    pGame->m_csStatus.m_bSummonGuard = true;
			    pChat->AddMessage(CRYLStringTable::m_strString[491], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
            }
			break;

        case PktSummonCmd::SUC_GUARD_OFF:
            {
			    pGame->m_csStatus.m_bSummonGuard = false;
			    pChat->AddMessage(CRYLStringTable::m_strString[492], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
            }
			break;
		}

		return true;
	} else
	{
		g_CharacterData.m_bSummonCmd = FALSE;
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharSummon)
{
	unsigned long	dwChrID, dwTargetID;
	POS				psPos;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSummon(lpPktBase, &dwChrID, &dwTargetID, &psPos);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			g_CharacterData.m_bSummon = true;
			g_CharacterData.m_bSummonCmd = false;
			pGame->m_csStatus.m_dwSummonID = dwTargetID;
		}

		const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType(dwTargetID & 0x0000FFFF);
		if (lpMonster)
		{
 			psPos.fPointX *= 100.0f;
			psPos.fPointY *= 100.0f;
			psPos.fPointZ *= 100.0f;

            GameProcess::LogCharLoad(dwTargetID, LOG_RFL);

            RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
            if ( !pSelfCreature )
                return false;

            RYLCreatureManager::Instance()->AddCreature( dwTargetID, lpMonster->m_MonsterInfo.m_strName, (char*)lpMonster->m_MonsterInfo.m_strModelingFlag, 
				(vector3 &)psPos, pSelfCreature->GetDirection() + FLOAT_PHI, lpMonster->m_MonsterInfo.m_bCollision, lpMonster->m_MonsterInfo.m_cNation );

            RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwTargetID );

            if ( pTempCreature )
            {
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwTargetID );

                if ( pTempData )
                {
					pTempData->m_ulSummonMasterCharID = dwChrID;
                    pTempData->m_ulMaxHP = pTempData->m_lCurrHP = lpMonster->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
                    pTempData->m_ulMaxMP = pTempData->m_lCurrMP = lpMonster->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
                }

                pTempCreature->SetAction( CA_SUMMON, CA_SUMMON );
            }
		}

		CEffScript* test_script = new CEffScript;
		if (test_script)
		{
			unsigned short wMonsterID = dwTargetID & 0x0000FFFF;
			char strSummonEffect[100];

			if (1006 <= wMonsterID && wMonsterID <= 1012)		strcpy(strSummonEffect, "summon_main2.esf");
			else if (1013 <= wMonsterID && wMonsterID <= 1019)	strcpy(strSummonEffect, "summon_main3.esf");
			else if (1020 <= wMonsterID && wMonsterID <= 1026)	strcpy(strSummonEffect, "summon_main4.esf");
			else												strcpy(strSummonEffect, "summon_main1.esf");

			// by Hades Kang - option dialog < skill effect >
			bool bEffect;
			switch ( pGame->GetClientEnemyType( dwTargetID ) )
			{
				case EnemyCheck::EC_ENEMY:
					bEffect = pGame->m_bEnemyEffect;
					break;

				case EnemyCheck::EC_FRIEND:
					bEffect = pGame->m_bFriendEffect;
					break;

				case EnemyCheck::EC_NEUTRAL:
					bEffect = pGame->m_bNeutralEffect;
					break;
			}

			if ( test_script->GetScriptBinData(strSummonEffect, bEffect) )
			{
				// ��ų ���� Ÿ�� ����Ʈ ó��
				vector3 vecPos = vector3(psPos.fPointX,psPos.fPointY,psPos.fPointZ);

				float fTerrainHeight = CSceneManager::m_HeightField.GetHeight(vecPos);
				vecPos.y = fTerrainHeight;

				test_script->SetStartPos(vecPos.x,vecPos.y,vecPos.z);
				test_script->SetEndPos(vecPos.x,vecPos.y,vecPos.z);

                RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                if ( !pSelfCreature )
                    return false;

                float fChrDir = pSelfCreature->GetDirection();
				fChrDir = -fChrDir;
				fChrDir -= FLOAT_PHI / 2.0f;

				vector3 vecChrToward;
				vecChrToward.x = cosf(fChrDir);
				vecChrToward.z = sinf(fChrDir);	
				vecChrToward.y = 0.0f;
				vecChrToward.Normalize();
				test_script->SetChrFwd(-vecChrToward.x, -vecChrToward.y, -vecChrToward.z);
				test_script->SetMine(true);
				test_script->SetBSkill(true);

				if (!CSceneManager::m_EffectManager.AddEffScript(test_script))			// ����Ʈ ��ũ��Ʈ�� ������ ������ �ε���
				{
					delete test_script;
				}
			} 
			else
			{
				delete test_script;
			}
		}

		return true;
	}

	return false;
}

GAME_EVENT_FUNC(ParseCharSpellInfo)//when you active\deactive chant skill(include a skill with effect like stun) hayzohar
{
	unsigned long	dwChrID;
	unsigned char	cSpellType;
	unsigned short	wEnchantLevel;
	unsigned long	dwEnchantTime;
	bool			bOnOff;

	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = 
        ParsePacket::HandleCharSpellInfo(lpPktBase, &dwChrID, &cSpellType, &wEnchantLevel, &dwEnchantTime, &bOnOff);

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		RYLCreature* lpRYLCreature = RYLCreatureManager::Instance()->GetCreature(dwChrID);
		if (lpRYLCreature)
		{
            int	index = 0;
			if (lpRYLCreature->GetCharID() != CRYLNetworkData::Instance()->m_dwMyChrID)
			{
                switch (g_CharacterData.GetClientEnemyType(lpRYLCreature->GetCharID()))
				{
					case EnemyCheck::EC_FRIEND:		index = 1;		break;
					case EnemyCheck::EC_NEUTRAL:	index = 2;		break;
					case EnemyCheck::EC_ENEMY:		index = 3;		break;
				}
            }

			lpRYLCreature->GetEffect()->SetChantEffect(cSpellType, bOnOff, (0 == index),
				g_CharacterData.m_bGoodChantEffectShow[index], g_CharacterData.m_bBadChantEffectShow[index] );

            if (bOnOff)
            {
			    if (0 != index && cSpellType == Skill::SpellID::LuckyOrb &&
                    pGame->m_csStatus.CheckPartyMember(dwChrID))
			    {
                    // �ֺ� ��Ƽ���� ����� ���긦 ����� ���, �޽����� ��� �ش�.
				    char strMessage[MAX_PATH] = "";
				    CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData(dwChrID);
				    CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
				    sprintf(strMessage, CRYLStringTable::m_strString[2950], pTempData->m_strName.c_str());
				    pChat->AddChatMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 128);
			    }

                // �ڵ� Ÿ���� ���ڽ��� ����� ���, ���ڽ��� �����Ѵ�.
    		    if (lpRYLCreature->GetEffect()->GetChantStatus(Skill::SpellID::Stealth) &&
                    lpRYLCreature->GetCharID() == g_CharacterData.m_dwAutoTargetID)
			    {
				    g_CharacterData.m_bAutoTargetAttack = false ;
				    g_CharacterData.m_dwAutoTargetID = 0xFFFFFFFF ;
			    }
            }
        }

        if (dwChrID == CRYLNetworkData::Instance()->m_dwMyChrID)
        {

            if (bOnOff)
            {                
				pGame->m_csStatus.m_aryEnchantLevel[cSpellType] = wEnchantLevel;
				pGame->m_csStatus.m_aryEnchantTime[cSpellType] = dwEnchantTime;

				// ���⿡�� 
				unsigned short usSkill = pGame->m_csStatus.GetSkillIDbyChantID(cSpellType);
				switch(usSkill)
				{
				case 0x8D12:
				case 0x8D14:
				case 0x8D16:
				case 0x99A1:
				case 0x99A2:
				case 0x99A3:
					pGame->SetPotionCoolDown(usSkill, (unsigned int)(dwEnchantTime*1000));
					break;
				}
				
				CRYLGameScene*				pScene			= (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
				CRYLJoystickConfigureDlg*	pJoystickDlg	= (CRYLJoystickConfigureDlg*)pScene->GetJoystickConfigureDlg() ;
				CRYLJoystickConfigurePanel* pJoystickPanel	= (CRYLJoystickConfigurePanel*)pJoystickDlg->GetRYLJoystickPanel();

				if (pJoystickPanel->GetForcefeedback() && Skill::SpellID::BattleSong == cSpellType)
				{
					// ForceFeedback : ��Ʋ�� ���
					KeyControl::StartForceFeedback(2000, 2000, 1000);
				}
            }
            else
            {
				unsigned short usSkill = pGame->m_csStatus.GetSkillIDbyChantID(cSpellType);

				pGame->m_csStatus.m_aryEnchantLevel[cSpellType] = 0;				
				pGame->m_csStatus.m_aryEnchantTime[cSpellType] = 0;

				if (0 != usSkill) { pGame->SetSkillCoolDown(usSkill); }				
            }

            pGame->m_csStatus.StatusUpdate(STATUS_UPDATE_BASE, true, true);

            RYLCreature* lpSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
            RYLCreatureEffect* lpSelfEffect = 0;
            if (0 != lpSelfCreature && 0 != (lpSelfEffect = lpSelfCreature->GetEffect()))
            {
                // Hold�� Stun�� ���� ���
                if (lpSelfEffect->GetChantStatus(Skill::SpellID::Hold) || 
                    lpSelfEffect->GetChantStatus(Skill::SpellID::Stun))
                {
                    g_CharacterData.InitGlobalAction();
    				
                    lpSelfCreature->SetUpperAble(true);
                    lpSelfCreature->SetLowerAble(true);
                    lpSelfCreature->SetUpperActing(false);
                    lpSelfCreature->SetLowerActing(false);

                    if (lpSelfEffect->GetChantStatus(Skill::SpellID::Hold))
                    {
					    lpSelfCreature->SetAction(CA_WAIT, CA_WAIT);
                    }
				    else if (lpSelfEffect->GetChantStatus(Skill::SpellID::Stun))
                    {
					    lpSelfCreature->SetAction(CA_STUN, CA_STUN);
                    }
                }
			    else if (lpSelfCreature->GetLowerActionID() == CA_STUN)
			    {
                    g_CharacterData.InitGlobalAction();

				    lpSelfCreature->SetUpperAble(true);
				    lpSelfCreature->SetLowerAble(true);
				    lpSelfCreature->SetUpperActing(false);
				    lpSelfCreature->SetLowerActing(false);

				    lpSelfCreature->SetAction( CA_WAIT, CA_WAIT );
			    }
            }
		}

		return true;
	}

	return false;
}


