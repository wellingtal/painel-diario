#include "stdafx.h"
#include <numeric>
#include "LogStruct.h"
#include "LogCommands.h"


int GAMELOG::sLogBase::GetSize()
{	
	using namespace CMD;

	int nSize = 0;	

	switch(m_cCmd)
	{
	case CHAR_LOGIN:
	case CHAR_LOGOUT:
	case CHAR_DBUPDATE:
        {
            sCharLoginOut* lpCharLoginOut = static_cast<sCharLoginOut*>(this);
            nSize = sizeof(sCharLoginOut) + 
                lpCharLoginOut->GetCharacterInfoSize() + lpCharLoginOut->m_usDepositData;                
        }
		break;			

	case CHAR_CREATE:
	case CHAR_DELETE:
		
		nSize = sizeof(sCharCreateDelete);
		break;

	case CHAR_LEVELUP:	nSize = sizeof(sCharLevelUpLog);	break;
	case CHAR_BIND_POS:	nSize = sizeof(sCharBindPosLog);	break;
	case CHAR_DEAD:		nSize = sizeof(sCharDeadLog);		break;
	case CHAR_RESPAWN:	nSize = sizeof(sCharRespawnLog);	break;

	case MOVE_ITEM:

		nSize = sizeof(sMoveItemLog);
		if(0 != m_cErr)
		{				
			nSize += sizeof(sItemDump) + reinterpret_cast<sItemDump*>((char*)this + 
				sizeof(sMoveItemLog))->GetItemDumpSize();
		}
		break;

	case SWAP_ITEM:

		nSize = sizeof(sSwapItemLog);
		if(0 != m_cErr)
		{
			nSize += sizeof(sItemDump) + reinterpret_cast<sItemDump*>((char*)this + 
				sizeof(sSwapItemLog))->GetItemDumpSize();
		}

		break;

	case USE_ITEM:
		nSize = sizeof(sUseItemLog);
		if(0 != m_cErr)
		{
			nSize += sizeof(sItemDump) + reinterpret_cast<sItemDump*>((char*)this + 
				sizeof(sUseItemLog))->GetItemDumpSize();
		}
		break;


	case USE_LOTTERY:
		nSize = sizeof(sUseLotteryLog);
		if(0 != m_cErr)
		{
			nSize += sizeof(sItemDump) + reinterpret_cast<sItemDump*>((char*)this + 
				sizeof(sUseItemLog))->GetItemDumpSize();
		}
		break;

	case SPLIT_ITEM:

		nSize = sizeof(sSplitItemLog);
		if(0 != m_cErr)
		{
			nSize += sizeof(sItemDump) + reinterpret_cast<sItemDump*>((char*)this + 
				sizeof(sSplitItemLog))->GetItemDumpSize();
		}

		break;

	case PICKUP_ITEM:

		nSize = sizeof(sPickupItemLog);			
		
		if(0 != m_cErr)
		{
			nSize += sizeof(sItemDump) + reinterpret_cast<sItemDump*>((char*)this + 
				nSize)->GetItemDumpSize();
		}
        else if(0 == static_cast<sPickupItemLog*>(this)->m_dwGold)
        {
            unsigned char cItemSize = 
                reinterpret_cast<Item::ItemData*>(reinterpret_cast<char*>(this) + nSize)->m_cItemSize;
            nSize += cItemSize;
        }

		break;

	case DROP_ITEM:

		nSize = sizeof(sDropItemLog);

		if(0 != m_cErr)
		{
			nSize += sizeof(sItemDump) + reinterpret_cast<sItemDump*>((char*)this + 
				nSize)->GetItemDumpSize();
		}
        else
        {
            nSize += reinterpret_cast<Item::ItemData*>(reinterpret_cast<char*>(this) + nSize)->m_cItemSize;
        }

		break;



	case BUY_ITEM:
    case MEDAL_BUY_ITEM:
	case SELL_ITEM:
    case TICKET_BUY_SKILLBOOK:

		nSize = sizeof(sTradeItemLog);

		if(0 != m_cErr)
		{
			nSize += sizeof(sItemDump) + reinterpret_cast<sItemDump*>((char*)this + 
				nSize)->GetItemDumpSize();
		}
        else
        {
            nSize += reinterpret_cast<Item::ItemData*>(reinterpret_cast<char*>(this) + nSize)->m_cItemSize;
        }

		break;

	case BEFORE_EXCHANGE_ITEM:
	case AFTER_EXCHANGE_ITEM:

        nSize = sizeof(sExchangeItemLog) + 
			static_cast<sExchangeItemLog*>(this)->m_usItemSize;
        
		break;


	case INSTALL_SOCKET_ITEM:
		{
			nSize = sizeof(sInstallSocketLog);
			
			if(0 != m_cErr)
			{
				nSize += sizeof(sItemDump) + 
					reinterpret_cast<sItemDump*>((char*)this + nSize)->GetItemDumpSize();
			}
            else
            {
                nSize += reinterpret_cast<sInstallSocketLog*>(this)->m_cGemSize + 
                    reinterpret_cast<sInstallSocketLog*>(this)->m_cEquipSize;                
            }

        } break;

    case REPAIR_ITEM:

        nSize = sizeof(sRepairItemLog);
        break;

    case CHANGE_WEAPON:

        nSize = sizeof(sChangeWeaponLog);
        break;

    case TAKE_GOLD:

        nSize = sizeof(sTakeGoldLog);
        break;

    case TAKE_GOLD_V2:

        nSize = sizeof(sTakeGoldLogV2);
        break;

    case UPGRADE_ITEM:
        {
            nSize = sizeof(sUpgradeItemLog);

            if(0 != m_cErr)
            {
                nSize += sizeof(sItemDump) + 
                    reinterpret_cast<sItemDump*>((char*)this + nSize)->GetItemDumpSize();
            }
            else
            {
                nSize += reinterpret_cast<sUpgradeItemLog*>(this)->m_cMineralSize + 
                    reinterpret_cast<sUpgradeItemLog*>(this)->m_cEquipSize;
            }
        }
        break;

    case STALL_OPEN_CLOSE:              nSize = sizeof(sStallOpenCloseLog);             break;
    case STALL_ENTER_LEAVE:             nSize = sizeof(sStallEnterLeaveLog);            break;
    case STALL_ITEM_REGISTER_REMOVE:    nSize = sizeof(sStallRegisterRemoveItemLog);    break;

    case GUILD_CREATE:          // 길드 생성
    case GUILD_JOIN:            // 길드 가입 신청
    case GUILD_LEAVE:           // 길드 탈퇴 (길드원 탈퇴 혹은, 다른 사람이 탈퇴시킴)
    case GUILD_MEMBER_LEVEL:    // 길드 멤버 레벨 조정
    case GUILD_RIGHTS_CHANGE:   // 길드 전체 권한 조정
    case GUILD_LEVEL_ADJUST:    // 길드 레벨 조정
    case GUILD_MARK_ADJUST:     // 길드 마크 등록/변경
    case GUILD_GOLD_CHANGE:     // 길드 창고 돈 입금/출금
    
    case GUILD_DISSOLVE:        // 길드 소멸

        nSize = sizeof(sGuildLog) + reinterpret_cast<sGuildLog*>(this)->m_usExtraDataSize;
        break;


    case ZONE_MOVE:
        nSize = sizeof(sMoveZoneLog);
        break;

    case ITEM_ATTACH_OPTION:    // 아이템 옵션 이식

        {
            sItemAttachOption* lpAttachOption = reinterpret_cast<sItemAttachOption*>(this);

            nSize = sizeof(sItemAttachOption) + lpAttachOption->m_cOldItemLen + 
                lpAttachOption->m_cUseItemLen + lpAttachOption->m_cNewItemLen;
        }
        break;

    case ITEM_COMPENSATION:     // 아이템 보상

        {
            sItemCompensation* lpCompensation = reinterpret_cast<sItemCompensation*>(this);

            nSize = sizeof(sItemCompensation) + lpCompensation->m_cOldItemLen + 
                lpCompensation->m_cNewItemLen;
        }
        break;

    case UPGRADE_ITEM_V2:       // 업그레이드 아이템 V2 (성공/실패여부 들어감)
        {
            nSize = sizeof(sUpgradeItemLogV2);

            if(0 != m_cErr)
            {
                nSize += sizeof(sItemDump) + 
                    reinterpret_cast<sItemDump*>((char*)this + nSize)->GetItemDumpSize();
            }
            else
            {
                nSize += reinterpret_cast<sUpgradeItemLogV2*>(this)->m_cMineralSize + 
                    reinterpret_cast<sUpgradeItemLogV2*>(this)->m_cEquipSize;
            }
        }
        break;

    case MONSTER_DEAD:

        nSize = sizeof(sMonsterDeadLog) + 
            reinterpret_cast<sMonsterDeadLog*>(this)->m_cDropItemNum * sizeof(unsigned long);
        break;

	case FAME_GET_BATTLE:
		nSize = sizeof(sFameGetBattleLog);
		break;

	case FAME_LOSE_BATTLE:
		nSize = sizeof(sFameLoseBattleLog);
		break;

	case FAME_GET_CAMP:
		nSize = sizeof(sFameGetCampLog);
		break;

	case FAME_LOSE_CAMP:
		nSize = sizeof(sFameLoseCampLog);
		break;

	case QUEST_GET_REWARD:
		nSize = sizeof(sQuestGetRewardLog);
		break;

	case CHANGE_RIDE:
		nSize = sizeof(sChangeRideLog);
		break;

	case ILLEGAL_ITEM:
		nSize = sizeof(sIllegalItemLog);
		break;

	case ILLEGAL_WARPPOS:
		nSize = sizeof(sIllegalWarpPosLog);
		break;

	case HACK_DOUBT:
		nSize = sizeof(sHockDoubtLog) + 
			reinterpret_cast<sHockDoubtLog*>(this)->m_wItemSize;
		break;
	};

	return nSize;
}

