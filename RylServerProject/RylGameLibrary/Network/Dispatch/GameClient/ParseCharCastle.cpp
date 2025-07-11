#include "stdafx.h"

#include "ParseCharCastle.h"
#include "SendCharCastle.h"
#include "GameClientDispatch.h"

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include <Creature/Character/Character.h>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/Camp.h>
#include <Creature/Siege/MiningCamp.h>
#include <Creature/Siege/CampShop.h>
#include <Creature/Siege/CastleGate.h>
#include <Creature/Siege/SiegeArms.h>
#include <Creature/Siege/SiegeObjectMgr.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Item/Item.h>
#include <Item/ItemStructure.h>
#include <Item/ItemConstants.h>
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>

using namespace Castle;


// �� ���� ��� ��Ŷ ó��
bool GameClientParsePacket::ParseCharCastleCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCastleCmd), GameClientDispatch);

	PktCastleCmd*	lpPktCastleCmd	= reinterpret_cast<PktCastleCmd*>(lpPktBase);
	CCharacter*		lpCharacter		= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	if (false == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
	{
		return true;
	}

	unsigned long	dwCID				= lpPktCastleCmd->m_dwCID;
	unsigned long	dwCastleID			= lpPktCastleCmd->m_dwCastleID;
	unsigned long	dwCastleObjectID	= lpPktCastleCmd->m_dwCastleObjectID;
	unsigned long	dwValue1			= lpPktCastleCmd->m_dwValue1;
	unsigned long	dwValue2			= lpPktCastleCmd->m_dwValue2;
	unsigned char	cSubCmd				= lpPktCastleCmd->m_cSubCmd;

	unsigned short	wError				= PktCastleCmd::NO_SERVER_ERR;

	Guild::CGuild* lpGuild = NULL;
	CSiegeObject* lpEmblem = NULL;

// CASTLE_TODO : ���� ���� �����ϹǷ� üũ ��ƾ ����

//	if (0 == lpCharacter->GetGID())
//	{
//		ERRLOG2(g_Log, "��忡 ���ԵǾ� �ִ� ���� ĳ���Ͱ� �� ���� ����� ��û�Ͽ����ϴ�. SenderID:%10u, Cmd:%d", dwCID, cSubCmd);
//		wError = PktCastleCmd::FAIL_NOT_GUILD_MEMBER;
//	}

	CCastle* lpCastle = CCastleMgr::GetInstance().GetCastle(dwCastleID);
	if (NULL == lpCastle)
	{
		ERRLOG4(g_Log, "dwCastleID:%10u �������� �ʴ� ���� ����� ��û�Ͽ����ϴ�. SenderID:%10u, GID:%10u, Cmd:%d", 
			dwCastleID, dwCID, lpCharacter->GetGID(), cSubCmd);
		wError = PktCastleCmd::FAIL_NOT_EXIST_CASTLE;
	}
	else
	{
// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���Ƶд�.

//		if (lpCharacter->GetGID() != lpCastle->GetGID())
//		{
//			ERRLOG5(g_Log, "�� ���� ����� ��û�� ĳ������ GID �� ���� GID �� ��ġ���� �ʽ��ϴ�. SenderID:%10u, SenderGID:%10u, CastleID:%10u, CastleGID:%10u, Cmd:%d", 
//				dwCID, lpCharacter->GetGID(), dwCastleID, lpCastle->GetGID(), cSubCmd);
//			wError = PktCastleCmd::FAIL_DIFFERENT_GUILD;
//		}
//
//		lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( lpCharacter->GetGID() );
//		if (NULL == lpGuild)
//		{
//			ERRLOG3(g_Log, "�� ���� ����� ��û�� ĳ������ ��尡 �������� �ʽ��ϴ�. SenderID:%10u, SenderGID:%10u, Cmd:%d", 
//					dwCID, lpCharacter->GetGID(), cSubCmd);
//			wError = PktCastleCmd::FAIL_NOT_EXIST_GUILD;
//		}

		lpEmblem = lpCastle->GetCastleEmblem();
		if (NULL == lpEmblem)
		{
			ERRLOG3(g_Log, "�� ���� ����� ��û�� ���� �� ��¡���� �������� �ʽ��ϴ�. SenderID:%10u, CastleID:%10u, Cmd:%d", 
					dwCID, lpCastle->GetCastleID(), cSubCmd);
			wError = PktCastleCmd::SERVER_ERROR;
		}
	}


	if (wError == PktBase::NO_SERVER_ERR)
	{
		switch (cSubCmd)
		{
			// ���� ����
			// [In/Out] dwValue1 : ���� Ÿ��
			// [In/Out] dwValue2 : ����
			case PktCastleCmd::CASTLE_SET_TAX:
			{
// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���� ������ �� ��ü�� ����!!

//				if (dwCID != lpGuild->GetMaster().m_dwCID)
//				{
//					wError = PktCastleCmd::FAIL_NOT_GUILD_MASTER;
//				}
//				else
//				{
//					if (false == lpCastle->IsTaxChangable(static_cast<unsigned char>(dwValue1)))
//					{
//						wError = PktCastleCmd::FAIL_CHANGE_TAX;
//					}
//					else
//					{
//						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
//						if (NULL == lpDBAgentDispatch)
//						{
//							wError = PktCastleCmd::SERVER_ERROR;
//							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
//						}
//						else
//						{
//							return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
//																					dwValue1, dwValue2, PktCastleCmd::CASTLE_SET_TAX, PktBase::NO_SERVER_ERR);
//						}
//					}
//				}
			}
			break;
            
			// �� ��¡�� ���׷��̵�
			// [Out] dwValue1 : ���׷��̵� Ÿ��
			// [Out] dwValue2 : ���׷��̵� �ð� (��)
			case PktCastleCmd::CASTLE_UPGRADE_EMBLEM:
			{
// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���׷��̵带 �� ��ü�� ����.!!

//				if (false == lpCastle->CheckRight(CastleRight::CONTROL_CASTLE_EMBLEM, lpCharacter->GetCID(), lpCharacter->GetGID()))
//				{
//					wError = PktCastleCmd::FAIL_CASTLE_RIGHT;
//				}
//				else
//				{
//					if (Siege::MAX_EMBLEM_UPGRADE == lpEmblem->GetUpgradeStep())
//					{
//						wError = PktCastleCmd::FAIL_MAX_UPGRADE;
//					}
//					else
//					{
//						// ��¡�� ���׷��̵忡 �ʿ��� ���� �Ҹ�
//						int nUpgradeType = 0;
//
//						// ���� ���� üũ
//						for (int i=0; i<Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM; ++i)
//						{
//							if (lpCastle->GetUpgradeItemNum(i) != Castle::EMBLEM_UPGRADE_JEWEL_NUM_PER_POS)
//							{
//								wError = PktCastleCmd::FAIL_NOT_ENOUGH_JEWEL;
//								break;
//							}
//						}
//
//						// ���� ���� ���� üũ
//						if (PktCastleCmd::NO_SERVER_ERR == wError)
//						{
//							if (0 == lpCastle->GetUpgradeItemID())
//							{
//								// �������� ���� ����
//								wError = PktCastleCmd::FAIL_JEWEL_KIND;
//							}
//
//							// ���� Ÿ�� ���� ID ���س���
//							nUpgradeType = (lpCastle->GetUpgradeItemID() - Siege::JEWEL_PROTOTYPE_ID) - Siege::JEWEL_KIND * lpEmblem->GetUpgradeStep();
//
//							if (nUpgradeType > Siege::JEWEL_KIND || nUpgradeType < 0)
//							{
//								// �������� ���� ����
//								wError = PktCastleCmd::FAIL_JEWEL_KIND;
//							}
//							else if (0 != lpEmblem->GetUpgradeStep() && lpEmblem->GetUpgradeType() != nUpgradeType)
//							{
//								// �������� ���� ����
//								wError = PktCastleCmd::FAIL_JEWEL_KIND;
//							}
//						}
//
//						// ���׷��̵� ó��
//						if (PktCastleCmd::NO_SERVER_ERR == wError)
//						{
//							lpCastle->InitUpgradeItem();
//
//							GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
//							if (NULL == lpDBAgentDispatch)
//							{
//								wError = PktCastleCmd::SERVER_ERROR;
//								ERRLOG0(g_Log, "������Ʈ ��� ����.");	
//							}
//							else
//							{
//								unsigned char cUpgradeTime = lpEmblem->GetUpgradeSpeed( static_cast<unsigned char>(nUpgradeType) );
//								return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, lpEmblem->GetCID(), 
//																						nUpgradeType, cUpgradeTime, PktCastleCmd::CASTLE_UPGRADE_EMBLEM,
//																						PktBase::NO_SERVER_ERR);
//							}
//						}
//					}
//				}
			}
			break;

			// ���� ȸ��
			// [In/Out] dwValue1 : ���� Ÿ��
			// [In/Out] dwValue2 : ȸ���� ���� �ݾ�
			case PktCastleCmd::CASTLE_TAKE_TAXMONEY:
			{
/*
				// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���� ȸ���� �� ��ü�� ����.!!
				// edith ���� �κ� �߰� (�ּ�ó�� ����)
				if (dwCID != lpGuild->GetMaster().m_dwCID)
				{
					wError = PktCastleCmd::FAIL_NOT_GUILD_MASTER;
				}
				else
				{
					if (dwValue2 > lpCastle->GetTaxMoney(static_cast<unsigned char>(dwValue1)))
					{
						wError = PktCastleCmd::FAIL_REQUEST_TOO_MUCH_GOLD; 
					}
					else
					{
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
						if (NULL == lpDBAgentDispatch)
						{
							wError = PktCastleCmd::SERVER_ERROR;
							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
						}
						else
						{
							return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
								dwValue1, dwValue2, PktCastleCmd::CASTLE_TAKE_TAXMONEY, PktBase::NO_SERVER_ERR);
						}
					}
				}
*/
			}
			break;

			// ���� ���� ���� ���� ��û
			case PktCastleCmd::CASTLE_SHOW_MINERAL:
			{
/*
				// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���� ���� ������ ��û�� ��ü�� ����.!!
				// edith ���� �κ� �߰� (�ּ�ó�� ����)

				if (dwCID != lpGuild->GetMaster().m_dwCID)
				{
					wError = PktCastleCmd::FAIL_NOT_GUILD_MASTER;
				}
				else
				{
					CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
					if (NULL != lpDispatch)
					{
						return lpCastle->SendMineralInfo(lpDispatch->GetSendStream());
					}
				}
*/
			}
			break;

			// ���� ȹ��
			// [In/Out] dwValue1 : ����ID
			// [In/Out] dwValue2 : ȹ�淮
			case PktCastleCmd::CASTLE_GAIN_MINERAL:
			{
/*
				// edith ���� �κ� �߰� (�ּ�ó�� ����)
				// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���� ȹ���� �� ��ü�� ����.!!
				if (dwCID != lpGuild->GetMaster().m_dwCID)
				{
					wError = PktCastleCmd::FAIL_NOT_GUILD_MASTER;
				}
				else
				{
					if (lpCastle->GetMineralNum(Siege::ACCUMULATED_MINERAL, static_cast<unsigned short>(dwValue1)) < dwValue2)
					{
						wError = PktCastleCmd::FAIL_NOT_ENOUGH_MINERAL;
					}
					else
					{
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
						if (NULL == lpDBAgentDispatch)
						{
							wError = PktCampCmd::SERVER_ERROR;
							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
						}
						else
						{
							if (GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
								dwValue1, dwValue2, PktCastleCmd::CASTLE_GAIN_MINERAL, PktBase::NO_SERVER_ERR))
							{
								const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo(static_cast<unsigned short>(dwValue1));
								if (NULL != lpItemInfo)
								{
									unsigned long dwGold = lpItemInfo->m_DetailData.m_dwPrice * dwValue2;

									// ������ ���Ű��� ȯ���� Gold �� ȹ���Ѱ����� �����ؼ� �� ���� ȸ������ ���� ��Ų��.
									GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
										Castle::CAMP_MINERAL_TAX, dwGold, PktCastleCmd::CASTLE_TAKE_TAXMONEY, PktBase::NO_SERVER_ERR);
								}

								return true;
							}
						}
					}
				}
*/
			}
			break;

			// ���� ���� / �ݱ�
			case PktCastleCmd::CASTLE_GATE_OPEN:
			case PktCastleCmd::CASTLE_GATE_CLOSE:
			{
// CASTLE_TODO : ������ ���� �ݰ��� ��ü�� ����.

//				CSiegeObject* lpGate = lpCastle->GetCastleObject(dwCastleObjectID);
//				if (NULL == lpGate)
//				{
//					wError = PktCastleCmd::SERVER_ERROR;
//				}
//				else
//				{
//					if (false == lpCastle->CheckRight(CastleRight::USE_CASTLE_GATE, lpCharacter->GetCID(), lpCharacter->GetGID()))
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_RIGHT;
//					}
//
//					if (Siege::COMPLETE != lpGate->GetState())
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
//					}
//
//					if (PktCastleCmd::NO_SERVER_ERR == wError)
//					{
//						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
//						if (NULL == lpDBAgentDispatch)
//						{
//							wError = PktCastleCmd::SERVER_ERROR;
//							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
//						}
//						else
//						{
//							return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
//																					0, 0, cSubCmd, PktBase::NO_SERVER_ERR);
//						}
//					}
//				}
			}
			break;

			// ���� ����
			case PktCastleCmd::CASTLE_GATE_PROTECT:
			{
// CASTLE_TODO : ���� ���� ����� 1�� �������� ���Ƶд�.!!

//				CCastleGate* lpGate = reinterpret_cast<CCastleGate*>( lpCastle->GetCastleObject(dwCastleObjectID) );
//				if (NULL == lpGate)
//				{
//					wError = PktBase::SERVER_ERROR;
//				}
//				else
//				{
//					if (false == lpCastle->CheckRight(CastleRight::USE_CASTLE_GATE, lpCharacter->GetCID(), lpCharacter->GetGID()))
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_RIGHT;
//					}
//
//					if (Siege::COMPLETE != lpGate->GetState())
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
//					}
//
//					if (PktCastleCmd::NO_SERVER_ERR == wError)
//					{
//						lpGate->AddProtectGate(lpCharacter);
//
//						// Ŭ���̾�Ʈ���� ����
//						CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
//						if (NULL != lpDispatch)
//						{
//							return GameClientSendPacket::SendCharCastleCmd(lpDispatch->GetSendStream(), dwCastleID, dwCastleObjectID,
//																			0, 0, PktCastleCmd::CASTLE_GATE_PROTECT, wError);
//						}
//					}
//				}
			}
			break;

			// ���� ���� ���
			case PktCastleCmd::CASTLE_GATE_PROTECT_CANCEL:
			{
// CASTLE_TODO : ���� ���� ����� 1�� �������� ���Ƶд�.

//				CCastleGate* lpGate = reinterpret_cast<CCastleGate*>( lpCastle->GetCastleObject(dwCastleObjectID) );
//				if (NULL == lpGate)
//				{
//					wError = PktBase::SERVER_ERROR;
//				}
//				else
//				{
//					if (false == lpCastle->CheckRight(CastleRight::USE_CASTLE_GATE, lpCharacter->GetCID(), lpCharacter->GetGID()))
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_RIGHT;
//					}
//
//					if (PktCastleCmd::NO_SERVER_ERR == wError)
//					{
//						lpGate->DeleteProtectGate(lpCharacter);
//
//						// Ŭ���̾�Ʈ���� ����
//						CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
//						if (NULL != lpDispatch)
//						{
//							return GameClientSendPacket::SendCharCastleCmd(lpDispatch->GetSendStream(), dwCastleID, dwCastleObjectID,
//																			0, 0, PktCastleCmd::CASTLE_GATE_PROTECT_CANCEL, wError);
//						}
//					}
//				}
			}
			break;

			// ���� ���׷��̵�
			// [In/Out] dwValue1 : ���׷��̵� ���
			// [Out] dwValue2 : ���׷��̵� �ð�
			case PktCastleCmd::CASTLE_UPGRADE_GATE:
			{
// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ������ ���׷��̵��� ��ü�� ����.

//				CSiegeObject* lpGate = lpCastle->GetCastleObject(dwCastleObjectID);
//				if (NULL == lpGate)
//				{
//					wError = PktBase::SERVER_ERROR;
//				}
//				else
//				{
//					if (Siege::COMPLETE != lpGate->GetState())
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
//					}
//
//					if (false == lpCastle->CheckRight(CastleRight::CONTROL_CASTLE_GATE, lpCharacter->GetCID(), lpCharacter->GetGID()))
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_RIGHT;
//					}
//
//					unsigned long dwUpgradeGold = lpGate->GetUpgradeGold();
//					unsigned char cUpgradeTime = lpGate->GetUpgradeSpeed();
//
//					if (dwValue1 != dwUpgradeGold)
//					{
//						wError = PktCastleCmd::FAIL_DIFFERENT_GOLD;
//					}
//
//					if (lpGuild->GetGold() < dwUpgradeGold)
//					{
//						wError = PktCastleCmd::FAIL_NOT_ENOUGH_GOLD;
//					}
//
//					if (PktCastleCmd::NO_SERVER_ERR == wError)
//					{
//						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
//						if (NULL == lpDBAgentDispatch)
//						{
//							wError = PktCastleCmd::SERVER_ERROR;
//							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
//						}
//						else
//						{
//							return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
//																					dwUpgradeGold, cUpgradeTime, PktCastleCmd::CASTLE_UPGRADE_GATE, PktBase::NO_SERVER_ERR);
//						}
//					}
//				}
			}
			break;

			// ���� ����
			// [In/Out] dwValue1 : ���� ���
			// [Out] dwValue2 : ������ HP ��
			case PktCastleCmd::CASTLE_REPAIR_GATE:
			{
// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ������ ������ ��ü�� ����.

//				CSiegeObject* lpGate = lpCastle->GetCastleObject(dwCastleObjectID);
//				if (NULL == lpGate)
//				{
//					wError = PktBase::SERVER_ERROR;
//				}
//				else
//				{
//					if (Siege::COMPLETE != lpGate->GetState() || lpGate->GetNowHP() == lpGate->GetMaxHP())
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
//					}
//
//					if (false == lpCastle->CheckRight(CastleRight::CONTROL_CASTLE_GATE, lpCharacter->GetCID(), lpCharacter->GetGID()))
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_RIGHT;
//					}
//
//					if (dwValue1 != lpGate->GetRepairGold())
//					{
//						wError = PktCastleCmd::FAIL_DIFFERENT_GOLD;
//					}
//
//					if (lpGuild->GetGold() < lpGate->GetRepairGold())
//					{
//						wError = PktCastleCmd::FAIL_NOT_ENOUGH_GOLD;
//					}
//
//					if (PktCastleCmd::NO_SERVER_ERR == wError)
//					{
//						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
//						if (NULL == lpDBAgentDispatch)
//						{
//							wError = PktCastleCmd::SERVER_ERROR;
//							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
//						}
//						else
//						{
//							return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
//																					dwValue1, lpGate->GetRepairHP(), PktCastleCmd::CASTLE_REPAIR_GATE, PktBase::NO_SERVER_ERR);
//						}
//					}
//				}
			}
			break;

			// ���� ����
			// [In/Out] dwValue1 : ���� ���� ���
			// [Out] dwValue2 : ���� ���� �ð� (��)
			case PktCastleCmd::CASTLE_RESTORE_GATE:
			{
// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ������ ������ ��ü�� ����. �������� �˾Ƽ� ���������ֵ��� �Ѵ�.!!

//				CSiegeObject* lpGate = lpCastle->GetCastleObject(dwCastleObjectID);
//				if (NULL == lpGate)
//				{
//					wError = PktBase::SERVER_ERROR;
//				}
//				else
//				{
//					if (Siege::DESTROYED != lpGate->GetState())
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
//					}
//
//					if (false == lpCastle->CheckRight(CastleRight::CONTROL_CASTLE_GATE, lpCharacter->GetCID(), lpCharacter->GetGID()))
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_RIGHT;
//					}
//
//					unsigned long dwRestoreGold = lpGate->GetDevelopGold();
//					unsigned char cRestoreTime = lpGate->GetDevelopSpeed();
//
//					if (dwValue1 != dwRestoreGold)
//					{
//						wError = PktCastleCmd::FAIL_DIFFERENT_GOLD;
//					}
//
//					if (lpGuild->GetGold() < dwRestoreGold)
//					{
//						wError = PktCastleCmd::FAIL_NOT_ENOUGH_GOLD;
//					}
//
//					if (PktCastleCmd::NO_SERVER_ERR == wError)
//					{
//						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
//						if (NULL == lpDBAgentDispatch)
//						{
//							wError = PktCastleCmd::SERVER_ERROR;
//							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
//						}
//						else
//						{
//							return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
//																					dwRestoreGold, cRestoreTime, PktCastleCmd::CASTLE_RESTORE_GATE, PktBase::NO_SERVER_ERR);
//						}
//					}
//				}
			}
			break;

			// �޹� ���
			// [In] dwValue1 : In / Out Ÿ��
			case PktCastleCmd::CASTLE_BACKDOOR:
			{
				CSiegeObject* lpBackDoor = lpCastle->GetCastleObject(dwCastleObjectID);
				if (NULL == lpBackDoor)
				{
					wError = PktBase::SERVER_ERROR;
				}
				else
				{
					if (dwValue1 != Siege::INSIDE && dwValue1 != Siege::OUTSIDE)
					{
						wError = PktCastleCmd::SERVER_ERROR;
					}

					if (PktCastleCmd::NO_SERVER_ERR == wError)
					{
						lpCharacter->MovePos(lpCastle->GetBackDoorPos(dwValue1), 0, false);
					}
				}
			}
			break;

			// ���� ���� ����
			// [In/Out] dwValue1 : ���� ���
			// [In/Out] dwValue2 : ���� 16��Ʈ : ����ð� / ���� 16��Ʈ : ���� Ÿ��
			case PktCastleCmd::CASTLE_CREATE_ARMS:
			{
				CSiegeObject* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
				if (NULL == lpCastleArms)
				{
					wError = PktBase::SERVER_ERROR;
				}
				else
				{
					if (GameTime::ST_SIEGE_WAR != CGameTimeMgr::GetInstance().GetCurrentSiegeTime() )
					{
						wError = PktCastleCmd::FAIL_NOT_SIEGE_TIME;
					}
					else if (Siege::COMPLETE != lpCastleArms->GetState() || Siege::CASTLE_ARMS_NPC != lpCastleArms->GetObjectType())
					{
						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
					}
					else if (lpCastle->HasAnotherCastleArms(dwCID))
					{
						wError = PktCastleCmd::FAIL_HAS_ANOTHER_CASTLE_ARMS;
					}
					else if(CSiegeObjectMgr::GetInstance().HasSiegeArms(dwCID))
					{
						wError = PktCastleCmd::FAIL_HAS_ANOTHER_SIEGE_ARMS ;
					}					
					else if(lpCharacter->GetLevel()<Siege::CREATE_LEVEL_LIMIT)
					{						
						wError = PktCastleCmd::NOT_LIMIT_LEVEL;
					}

					unsigned long dwDevelopGold = lpCastleArms->GetDevelopGold( static_cast<unsigned short>(dwValue2) );
					unsigned char cDevelopTime = lpCastleArms->GetDevelopSpeed( static_cast<unsigned short>(dwValue2) );

					if (dwValue1 != dwDevelopGold)
					{
						wError = PktCastleCmd::FAIL_DIFFERENT_GOLD;
					}
					else if (dwValue2 != Siege::SHORT_RANGE_CASTLE_ARMS &&
							 dwValue2 != Siege::LONG_RANGE_CASTLE_ARMS &&
							 dwValue2 != Siege::GUARD)
					{
						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
					}
					else if (lpCharacter->GetGold() < dwDevelopGold)
					{
						wError = PktCastleCmd::FAIL_NOT_ENOUGH_GOLD;
					}

					if (PktCastleCmd::NO_SERVER_ERR == wError)
					{
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
						if (NULL == lpDBAgentDispatch)
						{
							wError = PktCastleCmd::SERVER_ERROR;
							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
						}
						else
						{
							dwValue2 = (cDevelopTime << 16) + dwValue2;	// ���� 16��Ʈ : ����ð�	���� 16��Ʈ : ���� Ÿ��
							return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
																					dwDevelopGold, dwValue2, PktCastleCmd::CASTLE_CREATE_ARMS, PktBase::NO_SERVER_ERR);
						}
					}
				}
			}
			break;

			// ���� ���� ž��
			// [Out] dwValue1 : ž���� CID
			case PktCastleCmd::CASTLE_RIDE_ARMS:
			{
				CSiegeObject* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
				if (NULL == lpCastleArms)
				{
					wError = PktBase::SERVER_ERROR;
				}
				else
				{
					if (!lpCastleArms->IsCastleArms() || !lpCastleArms->IsRidable())
					{
						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
					}

					if (lpCastleArms->GetOwnerID() != dwCID)
					{
						wError = PktCastleCmd::FAIL_NOT_MY_CASTLE_ARMS;
					}

					if (Siege::COMPLETE != lpCastleArms->GetState())
					{
						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
					}

					if (Siege::NOT_RIDER != lpCastleArms->IsRider(dwCID))
					{
						wError = PktCastleCmd::FAIL_ALREADY_RIDE;
					}

					if (true == lpCharacter->IsRideArms())
					{
						wError = PktCastleCmd::FAIL_ALREADY_RIDE;
					}

					if (PktCastleCmd::NO_SERVER_ERR == wError)
					{
						if (false == lpCastleArms->Ride(dwCID))
						{
							wError = PktCastleCmd::SERVER_ERROR;
						}						
					}
				}
			}
			break;

			// ���� ���⿡�� ����
			// [Out] dwValue1 : ���� ��� CID
			case PktCastleCmd::CASTLE_GETOFF_ARMS:
			{
				CSiegeObject* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
				if (NULL == lpCastleArms)
				{
					wError = PktBase::SERVER_ERROR;
				}
				else
				{
					if ( !lpCastleArms->IsCastleArms() || !lpCastleArms->IsRidable() )
					{
						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
					}

					if (lpCastleArms->GetOwnerID() != dwCID)
					{
						wError = PktCastleCmd::FAIL_NOT_MY_CASTLE_ARMS;
					}

					if (Siege::COMPLETE != lpCastleArms->GetState())
					{
						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
					}

					if (false == lpCastleArms->IsRider(dwCID))
					{
						wError = PktCastleCmd::FAIL_NOT_RIDE;
					}

					if (PktCastleCmd::NO_SERVER_ERR == wError)
					{
						if (false == lpCastleArms->GetOff(dwCID))
						{
							wError = PktCastleCmd::SERVER_ERROR;
						}						
					}
				}
			}
			break;

			// ���� ���� ���׷��̵�
			// [In/Out] dwValue1 : ���׷��̵� ���
			// [Out] dwValue2 : ���׷��̵� �ð� (��)
			case PktCastleCmd::CASTLE_UPGRADE_ARMS:
			{
// CASTLE_TODO : ���׷��̵�� 1�� �������� ���Ƶд�.!!

//				CSiegeObject* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
//				if (NULL == lpCastleArms)
//				{
//					wError = PktBase::SERVER_ERROR;
//				}
//				else
//				{
//					if (false == lpCastle->CheckRight(CastleRight::CONTROL_CASTLE_ARMS, lpCharacter->GetCID(), lpCharacter->GetGID()))
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_RIGHT;
//					}
//
//					if (Siege::COMPLETE != lpCastleArms->GetState())
//					{
//						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
//					}
//
//					if (Siege::NOT_RIDER != lpCastleArms->IsRider(dwCID))
//					{
//						wError = PktCastleCmd::FAIL_ALREADY_RIDE;
//					}
//
//					unsigned long dwUpgradeGold = lpCastleArms->GetUpgradeGold();
//					unsigned char cUpgradeTime = lpCastleArms->GetUpgradeSpeed();
//
//					if (dwValue1 != dwUpgradeGold)
//					{
//						wError = PktCastleCmd::FAIL_DIFFERENT_GOLD;
//					}
//
//					if (lpGuild->GetGold() < dwUpgradeGold)
//					{
//						wError = PktCastleCmd::FAIL_NOT_ENOUGH_GOLD;
//					}
//
//					if (PktCastleCmd::NO_SERVER_ERR == wError)
//					{
//						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
//						if (NULL == lpDBAgentDispatch)
//						{
//							wError = PktCastleCmd::SERVER_ERROR;
//							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
//						}
//						else
//						{
//							return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
//																					dwUpgradeGold, cUpgradeTime, PktCastleCmd::CASTLE_UPGRADE_ARMS, PktBase::NO_SERVER_ERR);
//						}
//					}
//				}
			}
			break;

			// ���� ���� ����
			// [In/Out] dwValue1 : ���� ���
			// [Out] dwValue2 : ������ HP ��
			case PktCastleCmd::CASTLE_REPAIR_ARMS:
			{
				CSiegeObject* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
				if (NULL == lpCastleArms)
				{
					wError = PktBase::SERVER_ERROR;
				}
				else
				{
					if (Siege::COMPLETE != lpCastleArms->GetState())
					{
						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
					}

					if (lpCastleArms->GetOwnerID() != dwCID)
					{
						wError = PktCastleCmd::FAIL_NOT_MY_CASTLE_ARMS;
					}

					if (Siege::NOT_RIDER != lpCastleArms->IsRider(dwCID))
					{
						wError = PktCastleCmd::FAIL_ALREADY_RIDE;
					}

					if (lpCharacter->GetGold() < Siege::CASTLE_ARMS_REPAIR_GOLD_PER_UNIT)
					{
						wError = PktCastleCmd::FAIL_NOT_ENOUGH_GOLD;
					}

					if (PktCastleCmd::NO_SERVER_ERR == wError)
					{
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
						if (NULL == lpDBAgentDispatch)
						{
							wError = PktCastleCmd::SERVER_ERROR;
							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
						}
						else
						{
							// ������ ������ ��ŭ�� �����Ѵ�.
							if (lpCharacter->GetGold() < dwValue1)
							{
								dwValue1 = (lpCharacter->GetGold() / Siege::CASTLE_ARMS_REPAIR_GOLD_PER_UNIT) * Siege::CASTLE_ARMS_REPAIR_GOLD_PER_UNIT;
							}

							unsigned long dwRepairHP = static_cast<unsigned long>( lpCastleArms->GetMaxHP() * ( dwValue1 / Siege::CASTLE_ARMS_REPAIR_GOLD_PER_UNIT * 10.0f / 100.0f ) );
							if (lpCastleArms->GetNowHP() > lpCastleArms->GetMaxHP() - dwRepairHP)
							{
								dwRepairHP = lpCastleArms->GetMaxHP() - lpCastleArms->GetNowHP();
							}

							return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
																					dwValue1, dwRepairHP, PktCastleCmd::CASTLE_REPAIR_ARMS, PktBase::NO_SERVER_ERR);
						}
					}
				}
			}
			break;

			// ���� ���⸦ ���� ���� NPC �� ����
			// [Out] dwValue1 : TO_NPC (NPC �� ����)
			case PktCastleCmd::CASTLE_DESTROY_ARMS:
			{
				CSiegeObject* lpCastleArms = lpCastle->GetCastleObject(dwCastleObjectID);
				if (NULL == lpCastleArms)
				{
					wError = PktBase::SERVER_ERROR;
				}
				else
				{
					if (lpCastleArms->GetOwnerID() != dwCID)
					{
						wError = PktCastleCmd::FAIL_NOT_MY_CASTLE_ARMS;
					}

					if (!lpCastleArms->IsCastleArms())
					{
						wError = PktCastleCmd::SERVER_ERROR;
					}

					if (Siege::COMPLETE != lpCastleArms->GetState() || Siege::CASTLE_ARMS_NPC == lpCastleArms->GetObjectType())
					{
						wError = PktCastleCmd::FAIL_CASTLE_OBJECT_STATE;
					}

					if (Siege::NOT_RIDER != lpCastleArms->IsRider(dwCID))
					{
						wError = PktCastleCmd::FAIL_ALREADY_RIDE;
					}

					if (PktCastleCmd::NO_SERVER_ERR == wError)
					{
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
						if (NULL == lpDBAgentDispatch)
						{
							wError = PktCastleCmd::SERVER_ERROR;
							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
						}
						else
						{
							return GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, dwCastleObjectID,
								PktCastleCmd::TO_NPC, 0, PktCastleCmd::CASTLE_DESTROY_ARMS,
								PktBase::NO_SERVER_ERR);
						}
					}
				}
			}
			break;

			default:
			{
				ERRLOG3(g_Log, "CID:%10u CastleID:%10u �߸��� �� ���� ����Դϴ�. Cmd:%d", dwCID, dwCastleID, cSubCmd);
				wError = PktBase::SERVER_ERROR;
				break;
			}
		}
	}


	if (PktBase::NO_SERVER_ERR != wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharCastleCmd(lpDispatch->GetSendStream(), dwCastleID, dwCastleObjectID,
				dwValue1, dwValue2, cSubCmd, wError);
		}
	}

	return true;
}

bool GameClientParsePacket::ParseCharCastleRight(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	return true ;
// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� ���� ������ �� �� ����.

//	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCastleRight), GameClientDispatch);
//
//	PktCastleRight*	lpPktCastleRight	= reinterpret_cast<PktCastleRight*>(lpPktBase);
//	CCharacter*		lpCharacter			= GameClientDispatch.GetCharacter();
//
//	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());
//
//	if (false == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
//	{
//		return true;
//	}
//
//	unsigned long dwCID			= lpPktCastleRight->m_dwCID;
//	unsigned long dwCastleID	= lpPktCastleRight->m_dwCastleID;
//	CastleRight casteRight		= lpPktCastleRight->m_CastleRight;
//
//	unsigned short wError		= PktBase::NO_SERVER_ERR;
//
//	if (0 == lpCharacter->GetGID())
//	{
//		ERRLOG1(g_Log, "��忡 ���ԵǾ� �ִ� ���� ĳ���Ͱ� �� ���� ������ �����Ϸ� �Ͽ����ϴ�. SenderID:%10u", dwCID);
//		wError = PktCastleRight::FAIL_NOT_EXIST_GUILD;
//	}
//
//	CCastle* lpCastle = CCastleMgr::GetInstance().GetCastle(dwCastleID);
//	if (NULL == lpCastle)
//	{
//		ERRLOG3(g_Log, "dwCastleID:%10u �������� �ʴ� ���� ���� ������ �����Ϸ� �Ͽ����ϴ�. SenderID:%10u, GID:%10u", 
//			dwCastleID, dwCID, lpCharacter->GetGID());
//		wError = PktCastleRight::FAIL_NOT_EXIST_CASTLE;
//	}
//	else
//	{
//		if (lpCharacter->GetGID() != lpCastle->GetGID())
//		{
//			ERRLOG4(g_Log, "�� ���� ������ �����Ϸ��� ĳ������ GID �� ���� GID �� ��ġ���� �ʽ��ϴ�. SenderID:%10u, SenderGID:%10u, CastleID:%10u, CastleGID:%10u", 
//				dwCID, lpCharacter->GetGID(), dwCastleID, lpCastle->GetGID());
//			wError = PktCastleRight::FAIL_DIFFERENT_GUILD;
//		}
//
//		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( lpCharacter->GetGID() );
//		if (NULL == lpGuild)
//		{
//			ERRLOG2(g_Log, "�� ���� ���� ������ ��û�� ĳ������ ��尡 �������� �ʽ��ϴ�. SenderID:%10u, SenderGID:%10u", dwCID, lpCharacter->GetGID());
//			wError = PktCastleRight::FAIL_NOT_EXIST_GUILD;
//		}
//		else
//		{
//			CSiegeObject* lpEmblem = lpCastle->GetCastleEmblem();
//			if (lpEmblem && Siege::COMPLETE == lpEmblem->GetState())
//			{
//				Guild::MemberInfo Master = lpGuild->GetMaster();
//				if (Master.m_dwCID != dwCID)
//				{
//					ERRLOG2(g_Log, "��� �����Ͱ� �ƴ� ĳ���Ͱ� �� ���� ������ �����Ϸ� �մϴ�. SenderID:%10u, MasterCID:%10u", dwCID, Master.m_dwCID);
//					wError = PktCastleRight::FAIL_NOT_MASTER;
//				}
//				else
//				{
//					GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
//					if (NULL == lpDBAgentDispatch)
//					{
//						wError = PktBase::SERVER_ERROR;
//						ERRLOG0(g_Log, "������Ʈ ��� ����.");	
//					}
//					else
//					{
//						return GameClientSendPacket::SendCharCastleRight(lpDBAgentDispatch->GetSendStream(), dwCastleID, 
//																		casteRight, PktBase::NO_SERVER_ERR);
//					}
//				}
//			}
//		}
//	}
//
//	if (PktBase::NO_SERVER_ERR != wError)
//	{
//		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
//		if (NULL != lpDispatch)
//		{
//			return GameClientSendPacket::SendCharCastleRight(lpDispatch->GetSendStream(), dwCastleID, casteRight, wError);
//		}
//	}
//
//	return true;
}

// ��� ��� ���� ���� ��Ŷ ó��
bool GameClientParsePacket::ParseCharCampRight(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCampRight), GameClientDispatch);

	PktCampRight*	lpPktCampRight	= reinterpret_cast<PktCampRight*>(lpPktBase);
	CCharacter*		lpCharacter		= GameClientDispatch.GetCharacter();

	unsigned long dwCID		= lpPktCampRight->m_dwCID;
	unsigned long dwCampID	= lpPktCampRight->m_dwCampID;
	CampRight campRight		= lpPktCampRight->m_CampRight;

	unsigned short wError		= PktBase::NO_SERVER_ERR;

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	if (0 == lpCharacter->GetGID())
	{
		ERRLOG1(g_Log, "��忡 ���ԵǾ� �ִ� ���� ĳ���Ͱ� �� ���� ������ �����Ϸ� �Ͽ����ϴ�. SenderID:%10u", dwCID);
		wError = PktCampRight::FAIL_NOT_EXIST_GUILD;
	}

	CCamp* lpCamp = CSiegeObjectMgr::GetInstance().GetCamp(dwCampID);
	if (NULL == lpCamp)
	{
		ERRLOG3(g_Log, "dwCampID:%10u �������� �ʴ� ������ ���� ������ �����Ϸ� �Ͽ����ϴ�. SenderID:%10u, GID:%10u", 
			dwCampID, dwCID, lpCharacter->GetGID());
		wError = PktCampRight::FAIL_NOT_EXIST_CAMP;
	}
	else
	{
		if ( !lpCamp->IsCamp() )
		{
			ERRLOG2(g_Log, "������ �ƴ� ���� ������Ʈ�Դϴ�. SenderID:%10u, dwObjectID:%10u", dwCID, dwCampID);
			wError = PktCampRight::FAIL_NOT_CAMP;
		}

		if (lpCamp->GetState() != Siege::COMPLETE)
		{
			ERRLOG2(g_Log, "������ ���°� �Ϸ� ���°� �ƴմϴ�. SenderID:%10u, dwObjectID:%10u", dwCID, dwCampID);
			wError = PktCampRight::FAIL_CAMP_STATE;
		}

		if (lpCharacter->GetGID() != lpCamp->GetGID())
		{
			ERRLOG4(g_Log, "��� ��� ���� ������ �����Ϸ��� ĳ������ GID �� ������ GID �� ��ġ���� �ʽ��ϴ�. SenderID:%10u, SenderGID:%10u, CampID:%10u, CampGID:%10u", 
				dwCID, lpCharacter->GetGID(), dwCampID, lpCamp->GetGID());
			wError = PktCampRight::FAIL_DIFFERENT_GUILD;
		}

		Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( lpCharacter->GetGID() );
		if (NULL == lpGuild)
		{
			ERRLOG2(g_Log, "��� ��� ���� ���� ������ ��û�� ĳ������ ��尡 �������� �ʽ��ϴ�. SenderID:%10u, SenderGID:%10u", dwCID, lpCharacter->GetGID());
			wError = PktCampRight::FAIL_NOT_EXIST_GUILD;
		}
		else
		{
			Guild::MemberInfo Master = lpGuild->GetMaster();
			if (Master.m_dwCID != dwCID)
			{
				ERRLOG2(g_Log, "��� �����Ͱ� �ƴ� ĳ���Ͱ� ��� ��� ���� ������ �����Ϸ� �մϴ�. SenderID:%10u, MasterCID:%10u", dwCID, Master.m_dwCID);
				wError = PktCampRight::FAIL_NOT_MASTER;
			}
			else
			{
				GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
				if (NULL == lpDBAgentDispatch)
				{
					wError = PktBase::SERVER_ERROR;
					ERRLOG0(g_Log, "������Ʈ ��� ����.");	
				}
				else
				{
					return GameClientSendPacket::SendCharCampRight(lpDBAgentDispatch->GetSendStream(), dwCampID, 
																	campRight, PktBase::NO_SERVER_ERR);
				}
			}
		}
	}

	if (PktBase::NO_SERVER_ERR != wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharCampRight(lpDispatch->GetSendStream(), dwCampID, campRight, wError);
		}
	}

	return true;
}

// ��� ��� ���� ��� ��Ŷ ó��
bool GameClientParsePacket::ParseCharCampCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCampCmd), GameClientDispatch);

	PktCampCmd*		lpPktCampCmd	= reinterpret_cast<PktCampCmd*>(lpPktBase);
	CCharacter*		lpCharacter		= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	if (false == CServerSetup::GetInstance().UseContents(GameRYL::CAMP))
	{
		return true;
	}

    unsigned long	dwCID		= lpPktCampCmd->m_dwCID;
	unsigned long	dwCampID	= lpPktCampCmd->m_dwCampID;
	unsigned long	dwValue1	= lpPktCampCmd->m_dwValue1;
	unsigned long	dwValue2	= lpPktCampCmd->m_dwValue2;
	unsigned char	cSubCmd		= lpPktCampCmd->m_cSubCmd;

	unsigned short	wError		= PktCampCmd::NO_SERVER_ERR;

	Guild::CGuild* lpGuild = NULL;

	if (0 == lpCharacter->GetGID())
	{
		ERRLOG2(g_Log, "��忡 ���ԵǾ� �ִ� ���� ĳ���Ͱ� ��� ��� ����� ��û�Ͽ����ϴ�. SenderID:%10u, Cmd:%d", dwCID, cSubCmd);
		wError = PktCampCmd::FAIL_NOT_EXIST_GUILD;
	}

	CCamp* lpCamp = CSiegeObjectMgr::GetInstance().GetCamp(dwCampID);
	if (NULL == lpCamp)
	{
		ERRLOG4(g_Log, "dwCampID:%10u �������� �ʴ� ������ ����� ��û�Ͽ����ϴ�. SenderID:%10u, GID:%10u, Cmd:%d", 
			dwCampID, dwCID, lpCharacter->GetGID(), cSubCmd);
		wError = PktCampCmd::FAIL_NOT_EXIST_CAMP;
	}
	else
	{
		if (lpCharacter->GetGID() != lpCamp->GetGID())
		{
			ERRLOG5(g_Log, "��� ��� ����� ��â�� ĳ������ GID �� ������ GID �� ��ġ���� �ʽ��ϴ�. SenderID:%10u, SenderGID:%10u, CampID:%10u, CampGID:%10u, Cmd:%d", 
				dwCID, lpCharacter->GetGID(), dwCampID, lpCamp->GetGID(), cSubCmd);
			wError = PktCampCmd::FAIL_DIFFERENT_GUILD;
		}

		lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(lpCamp->GetGID());
		if (!lpGuild)
		{
			ERRLOG4(g_Log, "��� ��� ����� ��â�� ĳ������ ��尡 �������� �ʽ��ϴ�. SenderID:%10u, SenderGID:%10u, CampID:%10u, Cmd:%d", 
				dwCID, lpCharacter->GetGID(), dwCampID, cSubCmd);
			wError = PktCampCmd::FAIL_NOT_EXIST_GUILD;
		}
	}

	if (wError == PktBase::NO_SERVER_ERR)
	{
		switch (cSubCmd)
		{
			case PktCampCmd::CAMP_CANCEL:	// ��� ��� ���� ����
			{
				if (Siege::DEVELOPING != lpCamp->GetState())
				{
					wError = PktCampCmd::FAIL_CAMP_STATE;
				}
				else
				{
					Guild::MemberInfo Master = lpGuild->GetMaster();
					if (dwCID != Master.m_dwCID)
					{
						wError = PktCampCmd::FAIL_NOT_MASTER;
					}
					else
					{
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
						if (NULL == lpDBAgentDispatch)
						{
							wError = PktCampCmd::SERVER_ERROR;
							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
						}
						else
						{
							return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
								0, 0, PktCampCmd::CAMP_CANCEL, PktBase::NO_SERVER_ERR);
						}
					}
				}
			}
			break;

			case PktCampCmd::CAMP_UPGRADE:	// ��� ��� ���׷��̵�
			{
				// ���� üũ
				if (false == lpCamp->CheckRight(CampRight::CAMP_CONTROL, dwCID, lpCharacter->GetGID()))
				{
					wError = PktCampCmd::FAIL_CAMP_RIGHT;
				}
				else
				{
					// ä����� ���� ������ ��쿡�� SubState üũ
					switch (lpCamp->GetObjectType())
					{
						case Siege::MINING_CAMP:
						{	
							if (Siege::MINING_OFF != lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_MINING;
							}
						}
						break;

						case Siege::KARTERANT_WEAPON:
						case Siege::MERKADIA_WEAPON:
						{
							if (Siege::WEAPON_CHARGE == lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_CHARGING;
							}
							else if (Siege::WEAPON_FIRE == lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_FIRING;
							}
						}
						break;
					}

					if (Siege::MAX_UPGRADE_NUM == lpCamp->GetUpgradeStep())
					{
						wError = PktCampCmd::FAIL_MAX_UPGRADE;
					}

					if (Siege::COMPLETE != lpCamp->GetState())
					{
						wError = PktCampCmd::FAIL_NOT_COMPLETE_STATE;
					}

					if ( PktCampCmd::NO_SERVER_ERR == wError )
					{
						// ��� ��� ���׷��̵忡 �ʿ��� ���� �Ҹ�
						int nRemain = lpCamp->GetMaterialNum() - Siege::GetUpgradeMaterialNum(lpCamp->GetObjectType());
						if (nRemain < 0)
						{
							wError = PktCampCmd::FAIL_NOT_ENOUGH_MATERIAL;
						}
						else
						{
							lpCamp->SetMaterialNum(nRemain);

							GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
							if (NULL == lpDBAgentDispatch)
							{
								wError = PktCampCmd::SERVER_ERROR;
								ERRLOG0(g_Log, "������Ʈ ��� ����.");	
							}
							else
							{
								return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
									lpCamp->GetUpgradeSpeed(), nRemain, PktCampCmd::CAMP_UPGRADE, PktBase::NO_SERVER_ERR);
							}
						}
					}
				}
			}
			break;

			case PktCampCmd::CAMP_REPAIR:	// ��� ��� ����
			{
				// ���� üũ
				if (false == lpCamp->CheckRight(CampRight::CAMP_CONTROL, dwCID, lpCharacter->GetGID()))
				{
					wError = PktCampCmd::FAIL_CAMP_RIGHT;
				}
				else
				{
					// ä����� ���� ������ ��쿡�� SubState üũ
					switch (lpCamp->GetObjectType())
					{
						case Siege::MINING_CAMP:
						{	
							if (Siege::MINING_OFF != lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_MINING;
							}
						}
						break;

						case Siege::KARTERANT_WEAPON:
						case Siege::MERKADIA_WEAPON:
						{
							if (Siege::WEAPON_CHARGE == lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_CHARGING;
							}
							else if (Siege::WEAPON_FIRE == lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_FIRING;
							}
						}
						break;
					}

					// ��� ��� ���� ����Ҹ�� �ٽ� ����
					if (Siege::COMPLETE != lpCamp->GetState() || lpCamp->GetNowHP() == lpCamp->GetMaxHP())
					{
						wError = PktCampCmd::FAIL_NOT_COMPLETE_STATE;
					}

					if (PktCampCmd::NO_SERVER_ERR == wError)
					{
						if (0 == lpCamp->GetMaterialNum())
						{
							wError = PktCampCmd::FAIL_NOT_ENOUGH_MATERIAL;
						}
						else
						{
							// ��� ��� ���׷��̵忡 �ʿ��� ���� �Ҹ�
							unsigned long dwRepairHP = lpCamp->GetRepairHP();	// �����ؾ��� HP
							int nNeedMaterialNum = dwRepairHP / Siege::CAMP_REPAIR_HP_PER_MATERIAL;
							if (0 != (dwRepairHP % Siege::CAMP_REPAIR_HP_PER_MATERIAL)) ++nNeedMaterialNum;

							if (lpCamp->GetMaterialNum() > nNeedMaterialNum)
							{
								lpCamp->SetMaterialNum(lpCamp->GetMaterialNum() - nNeedMaterialNum);
							}
							else
							{
								if (lpCamp->GetMaterialNum() < nNeedMaterialNum)
								{
									dwRepairHP = lpCamp->GetMaterialNum() * Siege::CAMP_REPAIR_HP_PER_MATERIAL;
								}

								lpCamp->SetMaterialNum(0);
							}

							GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
							if (NULL == lpDBAgentDispatch)
							{
								wError = PktCampCmd::SERVER_ERROR;
								ERRLOG0(g_Log, "������Ʈ ��� ����.");	
							}
							else
							{
								return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
									dwRepairHP, lpCamp->GetMaterialNum(), PktCampCmd::CAMP_REPAIR, PktBase::NO_SERVER_ERR);
							}
						}
					}
				}
			}
			break;

			case PktCampCmd::CAMP_CARE:	// �����ϱ�
			{
				GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
				if (NULL == lpDBAgentDispatch)
				{
					wError = PktCampCmd::SERVER_ERROR;
					ERRLOG0(g_Log, "������Ʈ ��� ����.");	
				}
				else
				{
					return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
																		  0, 0, PktCampCmd::CAMP_CARE, PktBase::NO_SERVER_ERR);
				}
			}
			break;

			case PktCampCmd::CAMP_TO_STARTKIT:
			{
				// ���� üũ
				if (false == lpCamp->CheckRight(CampRight::CAMP_CONTROL, dwCID, lpCharacter->GetGID()))
				{
					wError = PktCampCmd::FAIL_CAMP_RIGHT;
				}
				else
				{
					// ä����� ���� ������ ��쿡�� SubState üũ
					switch (lpCamp->GetObjectType())
					{
						case Siege::MINING_CAMP:
						{	
							if (Siege::MINING_OFF != lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_MINING;
							}
						}
						break;

						case Siege::KARTERANT_WEAPON:
						case Siege::MERKADIA_WEAPON:
						{
							if (Siege::WEAPON_CHARGE == lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_CHARGING;
							}
							else if (Siege::WEAPON_FIRE == lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_FIRING;
							}
						}
						break;
					}

					if (Siege::COMPLETE != lpCamp->GetState())
					{
						wError = PktCampCmd::FAIL_NOT_COMPLETE_STATE;
					}

					if (PktCampCmd::NO_SERVER_ERR == wError )
					{
						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
						if (NULL == lpDBAgentDispatch)
						{
							wError = PktCampCmd::SERVER_ERROR;
							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
						}
						else
						{
							return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
								0, 0, PktCampCmd::CAMP_TO_STARTKIT, PktBase::NO_SERVER_ERR);
						}
					}
				}
			}
			break;

			// dwValue1[In] : ������ Ÿ��
			case PktCampCmd::CAMP_CHANGE_TYPE:	// ��� Ÿ�� ����
			{
				// ���� üũ
				if (false == lpCamp->CheckRight(CampRight::CAMP_CONTROL, dwCID, lpCharacter->GetGID()))
				{
					wError = PktCampCmd::FAIL_CAMP_RIGHT;
				}
				else
				{
					// �ѹ��̶� ���׷��̵带 �ؾ� Ÿ�� ������ �Ҽ� �ִ�.
					if (lpCamp->GetUpgradeStep() == 0)
					{
						wError = PktCampCmd::FAIL_UPGRADE_STEP;
					}

					// ä����� ���� ������ ��쿡�� SubState üũ
					switch (lpCamp->GetObjectType())
					{
						case Siege::MINING_CAMP:
						{	
							if (Siege::MINING_OFF != lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_MINING;
							}
							else
							{
								CMiningCamp* lpMiningCamp = reinterpret_cast<CMiningCamp* >(lpCamp);
								if (lpMiningCamp && lpMiningCamp->GetMineralTypeNum(Siege::ACCUMULATED_MINERAL) > 0)
								{
									wError = PktCampCmd::FAIL_HAS_MINERAL;
								}
							}
						}
						break;

						case Siege::CAMP_SHOP:
						{
							CCampShop* lpCampShop = reinterpret_cast<CCampShop* >(lpCamp);
							if (NULL != lpCampShop)
							{
								if (false == lpCampShop->GetContainer().IsEmpty())
								{
									wError = PktCampCmd::FAIL_HAS_ITEM;
								}
								else
								{
									lpCampShop->GetContainer().Close();
								}
							}
						}
						break;

						case Siege::KARTERANT_WEAPON:
						case Siege::MERKADIA_WEAPON:
						{
							if (Siege::WEAPON_CHARGE == lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_CHARGING;
							}
							else if (Siege::WEAPON_FIRE == lpCamp->GetSubState())
							{
								wError = PktCampCmd::FAIL_NOW_FIRING;
							}
						}
						break;
					}

					if (Siege::COMPLETE != lpCamp->GetState())
					{
						wError = PktCampCmd::FAIL_NOT_COMPLETE_STATE;
					}

					if ( PktCampCmd::NO_SERVER_ERR == wError )
					{
                        // ��� ��� Ÿ�� ���濡 �ʿ��� ���� �� ���
						int nNeedNum = Siege::GetChangingTypeMaterialNum(lpCamp->GetObjectType(), static_cast<unsigned short>(dwValue1), lpCamp->GetUpgradeStep());
						if (nNeedNum > 0)
						{
							int nRemain = lpCamp->GetMaterialNum() - nNeedNum;
							if (nRemain < 0)
							{
								wError = PktCampCmd::FAIL_NOT_ENOUGH_MATERIAL;
							}
							else
							{
								lpCamp->SetMaterialNum(nRemain);
							}
						}

						if ( PktCampCmd::NO_SERVER_ERR == wError )
						{
							GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
							if (NULL == lpDBAgentDispatch)
							{
								wError = PktCampCmd::SERVER_ERROR;
								ERRLOG0(g_Log, "������Ʈ ��� ����.");	
							}
							else
							{
								return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
									dwValue1, lpCamp->GetMaterialNum(), PktCampCmd::CAMP_CHANGE_TYPE, PktBase::NO_SERVER_ERR);
							}
						}
					}
				}
			}
			break;

			// ä���� ���� ������ Ȯ��
			case PktCampCmd::MINING_CAMP_SHOW_MINERAL:
			{
				if ( lpGuild->GetMaster().m_dwCID != lpCharacter->GetCID() )
				{
					wError = PktCampCmd::FAIL_NOT_MASTER;
				}
				else
				{
					CMiningCamp* lpMiningCamp = reinterpret_cast<CMiningCamp*>(lpCamp);
					if ( lpMiningCamp )
					{
						CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
						if (NULL != lpDispatch)
						{
							return lpMiningCamp->SendMineralInfo(lpDispatch->GetSendStream());
						}
					}
					else if (Siege::MINING_CAMP != lpCamp->GetObjectType())
					{
						wError = PktCampCmd::FAIL_NOT_MINING_CAMP;
					}
					else
					{
						wError = PktCampCmd::SERVER_ERROR;
						ERRLOG2(g_Log, "CID : %10u, CampID : %10u ä����� ĳ���� ����", lpCamp->GetCID(), lpCamp->GetCampID());
					}
				}
			}
			break;

			// ä���� On / Off
			// dwValue1[In] : On / Off ����
			case PktCampCmd::MINING_CAMP_ON_OFF:
			{
				if ( lpGuild->GetMaster().m_dwCID != lpCharacter->GetCID() )
				{
					wError = PktCampCmd::FAIL_NOT_MASTER;
				}
				else if (Siege::MINING_CAMP != lpCamp->GetObjectType())
				{
					wError = PktCampCmd::FAIL_NOT_MINING_CAMP;
				}
				else if (Siege::COMPLETE != lpCamp->GetState())
				{
					wError = PktCampCmd::FAIL_NOT_COMPLETE_STATE;
				}
				else
				{
					CMiningCamp* lpMiningCamp = reinterpret_cast<CMiningCamp*>(lpCamp);
					if ( lpMiningCamp )
					{
						if ( dwValue1 != lpMiningCamp->GetSubState() )
						{
							GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
							if (NULL == lpDBAgentDispatch)
							{
								wError = PktCampCmd::SERVER_ERROR;
								ERRLOG0(g_Log, "������Ʈ ��� ����.");	
							}
							else
							{
								return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
									dwValue1, 0, PktCampCmd::MINING_CAMP_ON_OFF, PktBase::NO_SERVER_ERR);
							}
						}
					}
					else
					{
						wError = PktCampCmd::SERVER_ERROR;
						ERRLOG2(g_Log, "CID : %10u, CampID : %10u ä����� ĳ���� ����", lpCamp->GetCID(), lpCamp->GetCampID());
					}
				}
			}
			break;

			// ä���� ���� ���� ȹ��
			// dwValue1[In] : Mineral Item ID
			// dwValue2[In] : Amount
			case PktCampCmd::MINING_CAMP_GAIN_MINERAL:
			{
				if ( lpGuild->GetMaster().m_dwCID != lpCharacter->GetCID() )
				{
					wError = PktCampCmd::FAIL_NOT_MASTER;
				}
				else if (Siege::MINING_CAMP != lpCamp->GetObjectType())
				{
					wError = PktCampCmd::FAIL_NOT_MINING_CAMP;
				}
				else
				{
					CMiningCamp* lpMiningCamp = reinterpret_cast<CMiningCamp*>(lpCamp);
					if ( lpMiningCamp )
					{
						if ( lpMiningCamp->GetMineralNum(Siege::ACCUMULATED_MINERAL, static_cast<unsigned short>(dwValue1)) < dwValue2 )
						{
							wError = PktCampCmd::FAIL_NOT_ENOUGH_MINERAL;
						}
						else
						{
							GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
							if (NULL == lpDBAgentDispatch)
							{
								wError = PktCampCmd::SERVER_ERROR;
								ERRLOG0(g_Log, "������Ʈ ��� ����.");	
							}
							else
							{
								return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
									dwValue1, dwValue2, PktCampCmd::MINING_CAMP_GAIN_MINERAL, PktBase::NO_SERVER_ERR);
							}
						}
					}
					else
					{
						wError = PktCampCmd::SERVER_ERROR;
						ERRLOG2(g_Log, "CID : %10u, CampID : %10u ä����� ĳ���� ����", lpCamp->GetCID(), lpCamp->GetCampID());
					}
				}
			}
			break;

			// ���� ���� (��� ��� ����)
			// dwValue1[In] : ���� �� ����
			case PktCampCmd::CAMP_SHOP_CHANGE_TAX:
			{
				if (lpGuild->GetMaster().m_dwCID != lpCharacter->GetCID())
				{
					wError = PktCampCmd::FAIL_NOT_MASTER;
				}
				else if (Siege::CAMP_SHOP != lpCamp->GetObjectType())
				{
					wError = PktCampCmd::FAIL_NOT_CAMP_SHOP;
				}
				else if (Siege::MAX_TAX < dwValue1)
				{
					wError = PktCampCmd::FAIL_TOO_MANY_TAX;
				}
				else
				{
					CCampShop* lpCampShop = reinterpret_cast<CCampShop*>(lpCamp);
					if (NULL != lpCampShop)
					{
						if (1 != lpCampShop->GetContainer().GetCurrentCustomerNum())
						{
							// ������ �ٲٷ��� �����͸� ������ ���¿��� �Ѵ�.
							wError = PktCampCmd::FAIL_NOT_ZERO_CUSTOMER;
						}
						else
						{
							GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
							if (NULL == lpDBAgentDispatch)
							{
								wError = PktCampCmd::SERVER_ERROR;
								ERRLOG0(g_Log, "������Ʈ ��� ����.");	
							}
							else
							{
								return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
									dwValue1, 0, PktCampCmd::CAMP_SHOP_CHANGE_TAX, PktBase::NO_SERVER_ERR);
							}
						}
					}
				}
			}
			break;

			// ���� ���� ���� ����
			// dwValue1[Out] : ���� �ð� (���׷��̵� �ð��� �����ϴ�)
			case PktCampCmd::WORLDWEAPON_CHARGE:
			{
				if (lpGuild->GetMaster().m_dwCID != lpCharacter->GetCID())
				{
					wError = PktCampCmd::FAIL_NOT_MASTER;
				}
				else if (Siege::KARTERANT_WEAPON != lpCamp->GetObjectType() && Siege::MERKADIA_WEAPON != lpCamp->GetObjectType())
				{
					wError = PktCampCmd::FAIL_NOT_WORLDWEAPON;
				}
				else if (Siege::WEAPON_EMPTY != lpCamp->GetSubState())
				{
					wError = PktCampCmd::FAIL_NOT_EMPTY_WEAPON;
				}
				else if (Siege::COMPLETE != lpCamp->GetState())
				{
					wError = PktCampCmd::FAIL_NOT_COMPLETE_STATE;
				}
				else
				{
					// ���� ���� ������ �ʿ��� ���� �Ҹ�
					int nRemain = lpCamp->GetMaterialNum() - Siege::WEAPON_CHARGE_MATERIAL_NUM;
					if (nRemain < 0)
					{
						wError = PktCampCmd::FAIL_NOT_ENOUGH_MATERIAL;
					}
					else
					{
						lpCamp->SetMaterialNum(nRemain);

						GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
						if (NULL == lpDBAgentDispatch)
						{
							wError = PktCampCmd::SERVER_ERROR;
							ERRLOG0(g_Log, "������Ʈ ��� ����.");	
						}
						else
						{
							return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
								Siege::WEAPON_CHARGE_TIME, nRemain, PktCampCmd::WORLDWEAPON_CHARGE, PktBase::NO_SERVER_ERR);
						}
					}
				}
			}
			break;

			// ���� ���� ���� �߻�
			// dwValue1[In] : X ��ǥ
			// dwValue2[In] : Z ��ǥ
			case PktCampCmd::WORLDWEAPON_FIRE:
			{
				if (lpGuild->GetMaster().m_dwCID != lpCharacter->GetCID())
				{
					wError = PktCampCmd::FAIL_NOT_MASTER;
				}
				else if (Siege::KARTERANT_WEAPON != lpCamp->GetObjectType() && Siege::MERKADIA_WEAPON != lpCamp->GetObjectType())
				{
					wError = PktCampCmd::FAIL_NOT_WORLDWEAPON;
				}
				else if (Siege::WEAPON_READY != lpCamp->GetSubState())
				{
					wError = PktCampCmd::FAIL_NOT_READY_WEAPON;
				}
				else if (Siege::COMPLETE != lpCamp->GetState())
				{
					wError = PktCampCmd::FAIL_NOT_COMPLETE_STATE;
				}
				else
				{
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
					if (NULL == lpDBAgentDispatch)
					{
						wError = PktCampCmd::SERVER_ERROR;
						ERRLOG0(g_Log, "������Ʈ ��� ����.");	
					}
					else
					{
						return GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCampID, 
							dwValue1, dwValue2, PktCampCmd::WORLDWEAPON_FIRE, PktBase::NO_SERVER_ERR);
					}
				}
			}
			break;

			default:
			{
				ERRLOG3(g_Log, "CID:%10u CampID:%10u �߸��� ��� ��� ����Դϴ�. Cmd:%d", dwCID, dwCampID, cSubCmd);
				wError = PktCampCmd::SERVER_ERROR;
				break;
			}
		}
	}

	if (PktBase::NO_SERVER_ERR != wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharCampCmd(lpDispatch->GetSendStream(), dwCID, dwCampID, cSubCmd, wError);
		}
	}

	return true;
}


// ���� ���� ���� ��� ��Ŷ ó��
bool GameClientParsePacket::ParseCharSiegeArmsCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSiegeArmsCmd), GameClientDispatch);

	PktSiegeArmsCmd*	lpPktSACmd	= reinterpret_cast<PktSiegeArmsCmd*>(lpPktBase);
	CCharacter*			lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	if (false == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
	{
		return true;
	}

	unsigned long	dwCID		= lpPktSACmd->m_dwCID;
	unsigned long	dwArmsID	= lpPktSACmd->m_dwArmsID;
	unsigned char	cSubCmd		= lpPktSACmd->m_cSubCmd;

	unsigned short	wError		= PktSiegeArmsCmd::NO_SERVER_ERR;
	
	CSiegeArms* lpSiegeArms = reinterpret_cast<CSiegeArms*>( CSiegeObjectMgr::GetInstance().GetSiegeObject(dwArmsID) );

	if (NULL == lpSiegeArms)
	{
		ERRLOG4(g_Log, "dwArmsID:%10u �������� �ʴ� ���� ������ ����� ��û�Ͽ����ϴ�. SenderID:%10u, GID:%10u, Cmd:%d", 
			dwArmsID, dwCID, lpCharacter->GetGID(), cSubCmd);
		wError = PktSiegeArmsCmd::FAIL_NOT_EXIST_SIEGE_ARMS;
	}
	else
	{
		unsigned short wObjectType = lpSiegeArms->GetObjectType();

		if (wObjectType != Siege::SHORT_RANGE_SIEGE_ARMS &&
			wObjectType != Siege::LONG_RANGE_SIEGE_ARMS &&
			wObjectType != Siege::AIRSHIP)
		{
			ERRLOG3(g_Log, "dwArmsID:%10u ���� ���Ⱑ �ƴ� ���� ������Ʈ�Դϴ�. SenderID:%10u, Cmd:%d",
				dwArmsID, dwCID, cSubCmd);
			wError = PktSiegeArmsCmd::FAIL_NOT_SIEGE_ARMS;
		}

		if (lpCharacter->GetCID() != lpSiegeArms->GetOwnerID())
		{
			ERRLOG4(g_Log, "���� ���� ����� ��â�� ĳ���Ͱ� ���� ������ �����ڰ� �ƴմϴ�. SenderID:%10u, ArmsID:%10u, ArmsOwnerID:%10u, Cmd:%d", 
				dwCID, dwArmsID, lpSiegeArms->GetOwnerID(), cSubCmd);
			wError = PktSiegeArmsCmd::FAIL_NOT_OWNER;
		}
	}

	if (wError == PktSiegeArmsCmd::NO_SERVER_ERR)
	{
		switch (cSubCmd)
		{
			// ���� ���� ���� ���
		case PktSiegeArmsCmd::SIEGE_CANCEL:
			{
				if (Siege::DEVELOPING != lpSiegeArms->GetState())
				{
					wError = PktSiegeArmsCmd::FAIL_SIEGE_ARMS_STATE;
				}
				else
				{
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
					if (NULL == lpDBAgentDispatch)
					{
						wError = PktBase::SERVER_ERROR;
						ERRLOG0(g_Log, "������Ʈ ��� ����.");	
					}
					else
					{
						return GameClientSendPacket::SendCharSiegeArmsCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwArmsID,
																				   0, PktSiegeArmsCmd::SIEGE_CANCEL, PktBase::NO_SERVER_ERR);
					}
				}
			}
			break;

			// ���� ���� ž��
		case PktSiegeArmsCmd::SIEGE_RIDE_ARMS:
			{
				if (Siege::COMPLETE != lpSiegeArms->GetState())
				{
					wError = PktSiegeArmsCmd::FAIL_SIEGE_ARMS_STATE;
				}

				if (Siege::NOT_RIDER != lpSiegeArms->IsRider(dwCID))
				{
					wError = PktSiegeArmsCmd::FAIL_ALREADY_RIDE;
				}

				if (true == lpCharacter->IsRideArms())
				{
					wError = PktSiegeArmsCmd::FAIL_ALREADY_RIDE;
				}

				if (PktSiegeArmsCmd::NO_SERVER_ERR == wError)
				{
					if (false == lpSiegeArms->Ride(dwCID))
					{
						wError = PktSiegeArmsCmd::FAIL_RIDE_SIEGE_ARMS;
					}
				}
			}
			break;

			// ���� ���� ����
		case PktSiegeArmsCmd::SIEGE_GETOFF_ARMS:
			{
				if (Siege::COMPLETE != lpSiegeArms->GetState())
				{
					wError = PktSiegeArmsCmd::FAIL_SIEGE_ARMS_STATE;
				}

				if (false == lpSiegeArms->IsRider(dwCID))
				{
					wError = PktSiegeArmsCmd::FAIL_NOT_RIDE;
				}

				if (PktSiegeArmsCmd::NO_SERVER_ERR == wError)
				{
					if (false == lpSiegeArms->GetOff(dwCID))
					{
						wError = PktSiegeArmsCmd::FAIL_GET_OFF_SIEGE_ARMS;
					}
				}
			}
			break;

			// ���� ���� ����
		case PktSiegeArmsCmd::SIEGE_REPAIR_ARMS:
			{
				// ���� ���� ������ �ʿ��� ���� �Ҹ�
				unsigned short wItemNum = lpCharacter->GetInventory().GetItemNum(Item::EtcItemID::SIEGE_MATERIAL_ID);
				if (0 == wItemNum || lpSiegeArms->IsMaxHP())
				{
					wError = PktSiegeArmsCmd::FAIL_NOT_ENOUGH_MATERIAL;
				}
				else
				{
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
					if (NULL == lpDBAgentDispatch)
					{
						wError = PktBase::SERVER_ERROR;
						ERRLOG0(g_Log, "������Ʈ ��� ����.");	
					}
					else
					{
						vector<Item::ItemGarbage> vecItemGarbage;
						// ���� �����ϴ� �ڸ�
						vecItemGarbage.push_back(Item::ItemGarbage(NULL, 0));

						unsigned short wRequireItemNum = lpSiegeArms->GetRepairMaterialNum();
						unsigned long dwRepairHP = 0;

						if (wItemNum >= wRequireItemNum)
						{
							// ���� ����
							dwRepairHP = lpSiegeArms->GetRepairHP();
							lpCharacter->GetInventory().DisappearItem(Item::EtcItemID::SIEGE_MATERIAL_ID, wRequireItemNum, vecItemGarbage);
						}
						else
						{
							// ���� ����ŭ�� ����
							dwRepairHP = lpSiegeArms->GetMaxHP() / Siege::SIEGE_ARMS_REPAIR_HP_PER_MATERIAL * wItemNum;
							lpCharacter->GetInventory().DisappearItem(Item::EtcItemID::SIEGE_MATERIAL_ID, wItemNum, vecItemGarbage);
						}

						if (false == vecItemGarbage.empty())
						{
							lpCharacter->ClearGarbage(vecItemGarbage, PktDisappearItem::DIC_DEFAULT);
						}

						return GameClientSendPacket::SendCharSiegeArmsCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwArmsID, 
																					dwRepairHP, PktSiegeArmsCmd::SIEGE_REPAIR_ARMS,
																					PktBase::NO_SERVER_ERR);
					}
				}
			}
			break;

			// ���� ���⸦ ��ŸƮŶ����
		case PktSiegeArmsCmd::SIEGE_TO_STARTKIT:
			{
				GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
				if (NULL == lpDBAgentDispatch)
				{
					wError = PktSiegeArmsCmd::SERVER_ERROR;
					ERRLOG0(g_Log, "������Ʈ ��� ����.");	
				}
				else
				{
					return GameClientSendPacket::SendCharSiegeArmsCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwArmsID, 
																				0, PktSiegeArmsCmd::SIEGE_TO_STARTKIT, PktBase::NO_SERVER_ERR);
				}
			}
			break;


		default:
			{
				ERRLOG3(g_Log, "CID:%10u ArmsID:%10u �߸��� ���� ���� ����Դϴ�. Cmd:%d", dwCID, dwArmsID, cSubCmd);
				wError = PktSiegeArmsCmd::SERVER_ERROR;
			}
			break;
		}
	}

	if (PktBase::NO_SERVER_ERR != wError)
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			return GameClientSendPacket::SendCharSiegeArmsCmd(lpDispatch->GetSendStream(), dwCID, dwArmsID, cSubCmd, wError);
		}
	}


	return true;
}


// �� ��¡���� ���� ������ �ֱ�/����
bool GameClientParsePacket::ParseCharTakeCastleJewel(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	return true;
// CASTLE_TODO : �� ��¡�� ���׷��̵带 �� �� �����Ƿ�, ������ �ְ� ���� ����.

//	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktTakeCastleJewel), GameClientDispatch);
//
//	PktTakeCastleJewel*	lpPktTCJ	= reinterpret_cast<PktTakeCastleJewel*>(lpPktBase);
//	CCharacter*			lpCharacter	= GameClientDispatch.GetCharacter();
//
//	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());
//
//	if (false == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
//	{
//		return true;
//	}
//
//	unsigned long	dwCID		= lpPktTCJ->m_dwCID;
//	unsigned long	dwCastleID	= lpPktTCJ->m_dwCastleID;
//	unsigned char	cSubCmd		= lpPktTCJ->m_cSubCmd;
//	unsigned short	wItemID		= lpPktTCJ->m_wItemID;
//	unsigned char	cIndex		= lpPktTCJ->m_cIndex;
//	unsigned char	cNum		= lpPktTCJ->m_cNum;
//	Item::ItemPos	pos			= lpPktTCJ->m_Pos;
//
//	unsigned short	wError		= PktTakeCastleJewel::NO_SERVER_ERR;
//
//	switch (cSubCmd)
//	{
//		// �� ��¡���� ���� �������� ���� ��
//		case PktTakeCastleJewel::SC_TAKE_IN :
//		{
//			CCastle* lpCastle = CCastleMgr::GetInstance().GetCastle( dwCastleID );
//			if ( lpCastle )
//			{
//				CSiegeObject* lpEmblem = lpCastle->GetCastleEmblem();
//				if ( lpEmblem )
//				{
//					// ���׷��̵尡 �� ���
//					if ( lpEmblem->GetUpgradeStep() > 0 )
//					{
//						unsigned short wNeedItemID = Siege::JEWEL_PROTOTYPE_ID + Siege::JEWEL_KIND * lpEmblem->GetUpgradeStep() + lpEmblem->GetUpgradeType();
//						if ( wItemID != wNeedItemID )
//						{
//							wError = PktTakeCastleJewel::FAIL_WRONG_JEWEL;
//						}
//					}
//					else
//					{
//						if ( 0 != lpCastle->GetUpgradeItemID() && lpCastle->GetUpgradeItemID() != wItemID )
//						{
//							wError = PktTakeCastleJewel::FAIL_DIFFERENT_JEWEL;
//						}
//					}
//
//					// ���� Ÿ���� ��ġ�Ѵٸ�...
//					if ( PktTakeCastleJewel::NO_SERVER_ERR == wError )
//					{
//						const Item::ItemInfo* lpItemInfo = Item::CItemMgr::GetInstance().GetItemInfo( wItemID );
//						if (NULL != lpItemInfo)
//						{
//							if ( cNum + lpCastle->GetUpgradeItemNum(cIndex) <= lpItemInfo->m_DetailData.m_cMaxDurabilityOrStack )
//							{
//								Item::CItem* lpExistItem = lpCharacter->GetItem( pos );
//								if (NULL != lpExistItem)
//								{
//									if ( lpExistItem->GetNumOrDurability() >= cNum )
//									{
//										lpExistItem->SetNumOrDurability( lpExistItem->GetNumOrDurability() - cNum );
//
//										if (0 == lpExistItem->GetNumOrDurability())
//										{
//											lpCharacter->RemoveItem( pos );
//											DELETE_ITEM(lpExistItem);
//										}
//
//										lpCastle->AddUpgradeItem( wItemID, cIndex, cNum ) ;
//
//										// �߰� ������ ���� ���� ���� ���� (Type, TotalNum)
//										// [Out] dwValue1 : wItemID
//										// [Out] dwValue2 : cTotalItemNum
//										GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
//										if (NULL == lpDBAgentDispatch)
//										{
//											wError = PktCastleCmd::SERVER_ERROR;
//											ERRLOG0(g_Log, "������Ʈ ��� ����.");	
//										}
//										else
//										{
//											GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, lpEmblem->GetCID(),
//												lpCastle->GetUpgradeItemID(), lpCastle->GetUpgradeItemNum(), PktCastleCmd::CASTLE_UPGRADE_ITEM_INFO, PktBase::NO_SERVER_ERR);
//										}
//									}
//									else
//									{
//										wError = PktTakeMaterial::FAIL_ITEM_NUMBER;
//									}
//								}
//								else
//								{
//									wError = PktTakeCastleJewel::FAIL_NOT_EXIST_ITEM;
//								}
//							}
//							else
//							{
//								wError = PktTakeCastleJewel::FAIL_OVER_STACK;
//							}
//						}
//						else
//						{
//							wError = PktTakeCastleJewel::FAIL_NOT_EXIST_ITEM_PROTOTYPE;
//						}
//					}
//				}
//				else
//				{
//					wError = PktTakeCastleJewel::FAIL_NOT_EXIST_EMBLEM;
//				}
//			}
//			else
//			{
//				wError = PktTakeCastleJewel::FAIL_NOT_EXIST_CASTLE;
//			}
//		}
//		break;
//
//		// �� ��¡������ ���� �������� ���� ��
//		case PktTakeCastleJewel::SC_TAKE_OUT :
//		{
//			CCastle* lpCastle = CCastleMgr::GetInstance().GetCastle( dwCastleID );
//			if ( lpCastle )
//			{
//				CSiegeObject* lpEmblem = lpCastle->GetCastleEmblem();
//				if ( lpEmblem )
//				{
//					if ( lpCastle->GetUpgradeItemNum( cIndex ) > 0 )
//					{
//						if ( lpCastle->GetUpgradeItemNum( cIndex ) >= cNum )
//						{
//							Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem( lpCastle->GetUpgradeItemID() );
//							if ( lpItem )
//							{
//								lpItem->SetNumOrDurability( cNum );
//								lpCharacter->SetItem(pos, lpItem);
//
//								lpCastle->DeleteUpgradeItem(cIndex, cNum );
//
//								// �߰� ������ ���� ���� ���� ���� (Type, TotalNum)
//								// [Out] dwValue1 : wItemID
//								// [Out] dwValue2 : cTotalItemNum
//								GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
//								if (NULL == lpDBAgentDispatch)
//								{
//									wError = PktCastleCmd::SERVER_ERROR;
//									ERRLOG0(g_Log, "������Ʈ ��� ����.");	
//								}
//								else
//								{
//									GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCID, dwCastleID, lpEmblem->GetCID(),
//										lpCastle->GetUpgradeItemID(), lpCastle->GetUpgradeItemNum(), PktCastleCmd::CASTLE_UPGRADE_ITEM_INFO, PktBase::NO_SERVER_ERR);
//								}
//							}
//							else
//							{
//								wError = PktTakeCastleJewel::FAIL_CREATE_ITEM;
//							}
//						}
//						else
//						{
//							wError = PktTakeCastleJewel::FAIL_ITEM_NUMBER;
//						}
//					}
//					else
//					{
//						wError = PktTakeCastleJewel::FAIL_NOT_EXIST_ITEM;
//					}
//				}
//				else
//				{
//					wError = PktTakeCastleJewel::FAIL_NOT_EXIST_EMBLEM;
//				}
//			}
//			else
//			{
//				wError = PktTakeCastleJewel::FAIL_NOT_EXIST_CASTLE;
//			}
//		}
//		break;
//	}
//
//	// Ack ��Ŷ�� �����ش�.
//	if (wError == PktTakeCastleJewel::NO_SERVER_ERR)
//	{
//		// �������� ��쿡�� ���� �������� �����ش�.
//		CCastle* lpCastle = CCastleMgr::GetInstance().GetCastle( dwCastleID );
//		if ( lpCastle )
//		{
//			PktTakeCastleJewel pktTCJ;
//			memcpy(&pktTCJ, lpPktTCJ, sizeof(PktTakeCastleJewel));
//
//			char* szPacket = reinterpret_cast<char *>(&pktTCJ);
//			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktTakeCastleJewel), CmdTakeCastleJewel, 0, 0))
//			{
//				Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( lpCastle->GetGID() );
//				if ( lpGuild )
//				{
//					lpGuild->SendAllMember(szPacket, sizeof(PktTakeCastleJewel), CmdTakeCastleJewel);
//				}
//			}
//		}
//	}
//	else
//	{
//		// ���� �߻��ÿ��� �ش� ĳ���Ϳ��Ը� �����ش�.
//		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
//		if (NULL != lpDispatch)
//		{
//			char* lpBuffer = lpDispatch->GetSendStream().GetBuffer(sizeof(PktTakeCastleJewel));
//			if (NULL != lpBuffer)
//			{
//				PktTakeCastleJewel* lpPktTakeCastleJewel = reinterpret_cast<PktTakeCastleJewel *>(lpBuffer);
//				memcpy(lpPktTakeCastleJewel, lpPktTCJ, sizeof(PktTakeCastleJewel));
//
//				return lpDispatch->GetSendStream().WrapCrypt(sizeof(PktTakeCastleJewel), CmdTakeCastleJewel, 0, wError);
//			}
//		}
//	}
//
//	return true;
}


// ��� ��� ���� �ֱ�/����
bool GameClientParsePacket::ParseCharTakeMaterial(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktTakeMaterial), GameClientDispatch);

	PktTakeMaterial*	lpPktTM	= reinterpret_cast<PktTakeMaterial*>(lpPktBase);
	CCharacter*			lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	if (false == CServerSetup::GetInstance().UseContents(GameRYL::CAMP))
	{
		return true;
	}

	unsigned long	dwCID		= lpPktTM->m_dwCID;
	unsigned long	dwCampID	= lpPktTM->m_dwCampID;
	unsigned char	cSubCmd		= lpPktTM->m_cSubCmd;
	unsigned char	cNum		= lpPktTM->m_cNum;
	Item::ItemPos	pos			= lpPktTM->m_Pos;

	unsigned short	wError		= PktTakeMaterial::NO_SERVER_ERR;

	switch (cSubCmd)
	{
		// ��� ����� ���� ���縦 ���� ��
		case PktTakeMaterial::SC_TAKE_IN :
		{
			CCamp* lpCamp = CSiegeObjectMgr::GetInstance().GetCamp( dwCampID );
			if (NULL != lpCamp)
			{
				const Item::ItemInfo* lpMaterialInfo = Item::CItemMgr::GetInstance().GetItemInfo( Item::EtcItemID::SIEGE_MATERIAL_ID );
				if (NULL != lpMaterialInfo)
				{
					if (lpCamp->GetMaterialNum() + cNum <= lpMaterialInfo->m_DetailData.m_cMaxDurabilityOrStack)
					{
						Item::CItem* lpExistItem = lpCharacter->GetItem( pos );
						if (NULL != lpExistItem)
						{
							if (lpExistItem->GetNumOrDurability() >= cNum)
							{
								lpExistItem->SetNumOrDurability(lpExistItem->GetNumOrDurability() - cNum);

								if (0 == lpExistItem->GetNumOrDurability())
								{
									lpCharacter->RemoveItem( pos );
									DELETE_ITEM(lpExistItem);
								}

								lpCamp->SetMaterialNum( lpCamp->GetMaterialNum() + cNum );

								// �߰� ������ ���� ��� ���� ���縦 �־����� �޼��� ������ �����ش�.
								// �߰� �������� ������ �޾Ƽ� ����� ���� ������ ������Ʈ �Ѵ�.!!
								GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
								if (NULL == lpDBAgentDispatch)
								{
									wError = PktCampMessage::SERVER_ERROR;
									ERRLOG0(g_Log, "������Ʈ ��� ����.");
								}
								else
								{
									GameClientSendPacket::SendCharCampMessageToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCampID,
										PktCampMessage::MSGCMD_TAKE_IN_MATERIAL, lpCharacter->GetCharacterName(), cNum, lpCamp->GetMaterialNum(), 
										PktCampMessage::NO_SERVER_ERR);
								}
							}
							else
							{
								wError = PktTakeMaterial::FAIL_ITEM_NUMBER;
							}
						}
						else
						{
							wError = PktTakeMaterial::FAIL_NOT_EXIST_ITEM;
						}
					}
					else
					{
						wError = PktTakeMaterial::FAIL_OVER_STACK;
					}
				}
				else
				{
					wError = PktTakeMaterial::FAIL_NOT_EXIST_ITEMINFO;
				}
			}
			else
			{
				wError = PktTakeMaterial::FAIL_NOT_EXIST_CAMP;
			}
		}
		break;


		// ��� ������� ���� ���縦 ������ �� (���� üũ)
		case PktTakeMaterial::SC_TAKE_OUT :
		{
			// ���� üũ
			CCamp* lpCamp = CSiegeObjectMgr::GetInstance().GetCamp( dwCampID );
			if (NULL != lpCamp)
			{
				if (false == lpCamp->CheckRight(CampRight::CAMP_CONTROL, dwCID, lpCharacter->GetGID()))
				{
					wError = PktTakeMaterial::FAIL_CAMP_RIGHT;
				}
				else
				{
					if (lpCamp->GetMaterialNum() > 0)
					{
						Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem( Item::EtcItemID::SIEGE_MATERIAL_ID );
						if ( lpItem )
						{
							lpItem->SetNumOrDurability( lpCamp->GetMaterialNum() );
							lpCharacter->SetItem(pos, lpItem);

							lpCamp->SetMaterialNum( 0 );

							// �߰� ������ ���� ��� ���� ���縦 �־����� �޼��� ������ �����ش�.
							// �߰� �������� ������ �޾Ƽ� ����� ���� ������ ������Ʈ �Ѵ�.!!
							GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
							if (NULL == lpDBAgentDispatch)
							{
								wError = PktCampMessage::SERVER_ERROR;
								ERRLOG0(g_Log, "������Ʈ ��� ����.");
							}
							else
							{
								GameClientSendPacket::SendCharCampMessageToDBAgent(lpDBAgentDispatch->GetSendStream(), dwCampID,
									PktCampMessage::MSGCMD_TAKE_OUT_MATERIAL, lpCharacter->GetCharacterName(), cNum, 0, 
									PktCampMessage::NO_SERVER_ERR);
							}
						}
						else
						{
							wError = PktTakeMaterial::FAIL_CREATE_ITEM;
						}
					}
					else
					{
						wError = PktTakeMaterial::FAIL_NOT_EXIST_ITEM;
					}
				}
			}
			else
			{
				wError = PktTakeMaterial::FAIL_NOT_EXIST_CAMP;
			}
		}
		break;
	}


	// Ack ��Ŷ�� �����ش�.
	CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
	if (NULL != lpDispatch)
	{
		char* lpBuffer = lpDispatch->GetSendStream().GetBuffer(sizeof(PktTakeMaterial));
		if (NULL != lpBuffer)
		{
			PktTakeMaterial* lpPktTakeMaterial = reinterpret_cast<PktTakeMaterial *>(lpBuffer);
			memcpy(lpPktTakeMaterial, lpPktTM, sizeof(PktTakeMaterial));

			return lpDispatch->GetSendStream().WrapCrypt(sizeof(PktTakeMaterial), CmdTakeMaterial, 0, wError);
		}
	}

	return true;
}


// ���� �÷��� On/Off
bool GameClientParsePacket::ParseWarOnOff(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktWarOnOff), GameClientDispatch);

	PktWarOnOff* lpWarOnOff	= static_cast<PktWarOnOff*>(lpPktBase);
	CCharacter*	lpCharacter = GameClientDispatch.GetCharacter();
	unsigned char cType = lpWarOnOff->m_cType;
	unsigned char cFlag = lpWarOnOff->m_cFlag;
	unsigned short wError = PktBase::NO_SERVER_ERR;

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned char cGuildWarFlag = lpCharacter->GetGuildWarFlag();
	unsigned char cRealmWarFlag = lpCharacter->GetRealmWarFlag();

	if (GameTime::GUILD == cType)
	{
		// ��� ����
		switch (cFlag)
		{
			case Creature::WAR_OFF:
			{
				if (Creature::WAR_ON == cGuildWarFlag)
				{
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
					if (NULL == lpDBAgentDispatch)
					{
						wError = PktWarOnOff::SERVER_ERROR;
						ERRLOG0(g_Log, "������Ʈ ��� ����.");
					}
					else
					{
						GameClientSendPacket::SendWarOnOff(lpDBAgentDispatch->GetSendStream(), lpCharacter->GetCID(), cType, cFlag, 0);
					}
				}
				else
				{
					wError = PktWarOnOff::E_WAR_OFF;
				}
			}
			break;

			case Creature::WAR_ON:
			{
				if (Creature::WAR_OFF == cGuildWarFlag)
				{
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
					if (NULL == lpDBAgentDispatch)
					{
						wError = PktWarOnOff::SERVER_ERROR;
						ERRLOG0(g_Log, "������Ʈ ��� ����.");
					}
					else
					{
						GameClientSendPacket::SendWarOnOff(lpDBAgentDispatch->GetSendStream(), lpCharacter->GetCID(), cType, cFlag, 0);
					}
				}
				else
				{
					wError = PktWarOnOff::E_WAR_ON;
				}
			}
			break;

			case Creature::WAR_INSTANCE:
			{
				if (Creature::WAR_OFF == cGuildWarFlag)
				{
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
					if (NULL == lpDBAgentDispatch)
					{
						wError = PktWarOnOff::SERVER_ERROR;
						ERRLOG0(g_Log, "������Ʈ ��� ����.");
					}
					else
					{
						GameClientSendPacket::SendWarOnOff(lpDBAgentDispatch->GetSendStream(), lpCharacter->GetCID(), cType, cFlag, 0);
					}
				}
				else
				{
					wError = PktWarOnOff::E_WAR_INSTANCE;
				}
			}
			break;
		}
	}
	else if (GameTime::REALM == cType)
	{
		// ���� ����
		switch (cFlag)
		{
			case Creature::WAR_OFF:
			{
				wError = PktWarOnOff::E_WAR_OFF;
			}
			break;

			case Creature::WAR_ON:
			{
				wError = PktWarOnOff::E_WAR_ON;
			}
			break;

			case Creature::WAR_INSTANCE:
			{
				if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
				{
					wError = PktWarOnOff::E_WAR_INSTANCE;
				}
				//if (Creature::WAR_OFF == cRealmWarFlag)
				else
				{
					GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
					if (NULL == lpDBAgentDispatch)
					{
						wError = PktWarOnOff::SERVER_ERROR;
						ERRLOG0(g_Log, "������Ʈ ��� ����.");
					}
					else
					{
						GameClientSendPacket::SendWarOnOff(lpDBAgentDispatch->GetSendStream(), lpCharacter->GetCID(), cType, cFlag, 0);
					}
				}
/*				else
				{
					wError = PktWarOnOff::E_WAR_INSTANCE;
				}
*/
			}
			break;
		}
	}
	else
	{
		wError = PktWarOnOff::E_INVALID_TYPE;	
	}

	if (PktBase::NO_SERVER_ERR != wError)
	{
		return GameClientSendPacket::SendWarOnOff(GameClientDispatch.GetSendStream(), lpCharacter->GetCID(), cType, cFlag, wError);
	}

	return true;
}
