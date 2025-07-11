
#include "stdafx.h"
#include "SendCharAttack.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/Packet/PacketStruct/CharMovePacket.h>

#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>

#include <Creature/AggresiveCreature.h>
#include <Creature/Character/Character.h>
#include <Creature/Monster/Monster.h>

#include <Skill/SkillTable.h>
#include <Skill/Spell/SpellTable.h>
#include <Skill/Spell/SpellUtil.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Map/FieldMap/Cell.h>


bool GameClientSendPacket::SendCharDuelCmd(CSendStream& SendStream, unsigned long dwSenderID, unsigned long dwRecverID, 
										   unsigned char cCmd, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktDuC));
    if (NULL == lpBuffer) { return false; }

    PktDuC* lpPktDuCAck = reinterpret_cast<PktDuC*>(lpBuffer);

	lpPktDuCAck->m_dwSenderID	= dwSenderID;
	lpPktDuCAck->m_dwRecverID	= dwRecverID;

	lpPktDuCAck->m_cCmd			= cCmd;
            
    return SendStream.WrapCrypt(sizeof(PktDuC), CmdCharDuelCmd, 0, usError);
}


bool GameClientSendPacket::SendCharAttacked(CSendStream& SendStream, CAggresiveCreature* pAttackCreature, CAggresiveCreature* pDefendCharacter, 
											const AtType attackType, float fDir, unsigned short wDamage, unsigned char cDefenserJudge, 
											unsigned short wMPHeal, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktAted));
    if (NULL != lpBuffer) 
    {
        PktAted* lpPktAted = reinterpret_cast<PktAted*>(lpBuffer);

	    lpPktAted->m_dwCharID = pAttackCreature->GetCID();
	    lpPktAted->m_dwMyID = pDefendCharacter->GetCID();

	    lpPktAted->m_AtType = attackType;

	    lpPktAted->m_Postion.fPointX = pAttackCreature->GetCurrentPos().m_fPointX;
	    lpPktAted->m_Postion.fPointY = pAttackCreature->GetCurrentPos().m_fPointY;
	    lpPktAted->m_Postion.fPointZ = pAttackCreature->GetCurrentPos().m_fPointZ;
	    lpPktAted->m_fDir = fDir;

	    lpPktAted->m_wMyHP = pDefendCharacter->GetStatus().m_nNowHP;
	    lpPktAted->m_wMyMP = pDefendCharacter->GetStatus().m_nNowMP;
		lpPktAted->m_wMyMPHeal = wMPHeal;

	    lpPktAted->m_cJudge = cDefenserJudge;

	    SendStream.WrapCrypt(sizeof(PktAted), CmdCharAttacked, 0, usError);
    }

    // 맞는 자가 사람이면 내 파티원들에게 내가 공격당했다는 메시지를 뿌린다.
    if(Creature::CT_PC == Creature::GetCreatureType(pDefendCharacter->GetCID()))
    {
        static_cast<CCharacter*>(pDefendCharacter)->SendAttackedToParty(
            pAttackCreature->GetCID(), attackType, wDamage, cDefenserJudge, wMPHeal);
    }
    return true;
}


bool GameClientSendPacket::SendCharEquipDurability(CSendStream& SendStream, unsigned long dwCharID, 
												   unsigned char cIndex, unsigned char cValue, unsigned char cValueMax, unsigned short usError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktEquipDurability));
	if (NULL == lpBuffer) { return false; }

	PktEquipDurability* lpPktED = reinterpret_cast<PktEquipDurability*>(lpBuffer);

	lpPktED->m_dwCharID = dwCharID;

	lpPktED->m_cIndex = cIndex;
	lpPktED->m_cValue = cValue;
	lpPktED->m_cValueMax = cValueMax;	// 이값이 0이면 최대내구도변화가 없다. 최대내구도의 최소값은 1.

	return SendStream.WrapCrypt(sizeof(PktEquipDurability), CmdCharEquipDurability, 0, usError);
}


bool GameClientSendPacket::SendCharPeaceMode(CSendStream& SendStream, unsigned long dwCharID, 
											 unsigned char cLeftTime, bool bPeace, unsigned short usError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktPeace));
	if (NULL == lpBuffer) { return false; }

	PktPeace* lpPktPeaceAck = reinterpret_cast<PktPeace *>(lpBuffer);

	lpPktPeaceAck->m_dwCharID	= dwCharID;
	lpPktPeaceAck->m_cLeftTime	= cLeftTime;
	lpPktPeaceAck->m_bPeace		= bPeace;

	return SendStream.WrapCrypt(sizeof(PktPeace), CmdCharPeaceMode, 0, usError);
}

bool GameClientSendPacket::SendCharSummon(unsigned long dwCharID, CAggresiveCreature* lpSummonee)
{
	CCell* lpCell = lpSummonee->GetCellPos().m_lpCell;
	if (NULL == lpCell)
	{
		ERRLOG1(g_Log, "CID:0x%08x 셀 얻기 실패.", lpSummonee->GetCID());
		return false;
	}

	PktSummon pktSummon;
    memset(&pktSummon, 0, sizeof(PktSummon));

	pktSummon.m_dwCharID	= dwCharID;
	pktSummon.m_dwTargetID	= lpSummonee->GetCID();

	pktSummon.m_NetworkPos.Initialize(lpSummonee->GetCurrentPos().m_fPointX, lpSummonee->GetCurrentPos().m_fPointY, 
		lpSummonee->GetCurrentPos().m_fPointZ, 0, 0);

    lpCell->SendAllNearCellCharacter(&pktSummon, sizeof(PktSummon), CmdCharSummon);
	return true;
}


bool GameClientSendPacket::SendCharSummonCmd(CCharacter* lpCharacter, CMonster* lpSummonee, 
											 unsigned char cCmd, unsigned long dwTargetID)
{
	PktSummonCmd pktSummonCmd;
    memset(&pktSummonCmd, 0, sizeof(PktSummonCmd));

	pktSummonCmd.m_dwCharID		= lpCharacter->GetCID();
	pktSummonCmd.m_cCmd			= cCmd;
	pktSummonCmd.m_dwTargetID	= dwTargetID;

	if (PktSummonCmd::SUC_DIE == cCmd)
	{
		CCell* lpCell = lpSummonee->GetCellPos().m_lpCell;
		if (NULL == lpCell)
		{
			ERRLOG1(g_Log, "CID:0x%08x 셀 얻기 실패.", lpSummonee->GetCID());
			return false;
		}

		lpCell->SendAllNearCellCharacter(&pktSummonCmd, sizeof(PktSummonCmd), CmdCharSummonCmd);
	}
	else
	{
        char* szPacket = reinterpret_cast<char*>(&pktSummonCmd);
        CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();

	    if (0 != lpDispatch && 
            PacketWrap::WrapCrypt(szPacket, sizeof(PktSummonCmd), CmdCharSummonCmd, 0, 0))
	    {
			lpDispatch->GetSendStream().PutBuffer(szPacket, sizeof(PktSummonCmd), CmdCharSummonCmd);
		}		
	}

	return true;
}

bool GameClientSendPacket::SendCharBattleGroundRespawn(CSendStream& SendStream, unsigned long dwCharID, 
													   unsigned short wTurn, unsigned short wWaitNum, unsigned short wLeftTime, 
													   unsigned short wHumanNum, unsigned short wAkhanNum, unsigned short usError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktBGRsAck));
	if (NULL == lpBuffer) { return false; }

	PktBGRsAck* lpPktBGRsAck = reinterpret_cast<PktBGRsAck *>(lpBuffer);

	lpPktBGRsAck->m_dwCharID = dwCharID;

	lpPktBGRsAck->m_wTurn = wTurn;
	lpPktBGRsAck->m_wWaitNum = wWaitNum;	

	lpPktBGRsAck->m_wLeftTime = wLeftTime;

	lpPktBGRsAck->m_wHumanNum = wHumanNum;
	lpPktBGRsAck->m_wAkhanNum = wAkhanNum;

	return SendStream.WrapCrypt(sizeof(PktBGRsAck), CmdCharBattleGroundRespawn, 0, usError);	
}

bool GameClientSendPacket::SendCameraScript(unsigned long dwDeleteObject, unsigned long dwNewObject, Position NewPos, unsigned long dwTime)
{
	CCreatureManager& CreatureManager = CCreatureManager::GetInstance();

	CreatureManager.ProcessAllCharacter(Skill::CAddSpell<CInvincibleSpell>(CSpell::Spell_Info(
		Skill::CProcessTable::ProcessInfo::m_NullProtoType, NULL, 
		Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::Invincible, 1, dwTime)));

	PktCameraScript pktCS;
	pktCS.m_dwDeleteObject = dwDeleteObject;
	pktCS.m_dwNewObject = dwNewObject;

	pktCS.m_NewPos.fPointX = NewPos.m_fPointX;
	pktCS.m_NewPos.fPointY = NewPos.m_fPointY;
	pktCS.m_NewPos.fPointZ = NewPos.m_fPointZ;

	char* szPacket = reinterpret_cast<char*>(&pktCS);
	if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCameraScript), CmdCameraScript, 0, 0))
	{
		CreatureManager.SendAllCharacter(szPacket, sizeof(PktCameraScript), CmdCameraScript);
	}		

	return true;
}

// 죽음
bool GameClientSendPacket::SendCharDeadToParty(CCharacter* lpDeadCharacter, 
                                               CAggresiveCreature* lpReference, PktDeadInfo::TypeCode eTypeCode)
{
    if(0 != lpDeadCharacter && 0 != lpDeadCharacter->GetPID())
    {
        GET_SINGLE_DISPATCH(lpDispatch, CDBAgentDispatch, 
            CDBAgentDispatch::GetDispatchTable());
        
        if(0 != lpDispatch)
        {
            CSendStream& SendStream = lpDispatch->GetSendStream();
            char* lpBuffer = SendStream.GetBuffer(sizeof(PktDeadInfo));
            if(0 != lpBuffer)
            {
                PktDeadInfo* lpDeadInfo = reinterpret_cast<PktDeadInfo*>(lpBuffer);

                lpDeadInfo->m_dwRequestKey      = 0;
                lpDeadInfo->m_wCmd              = PktDD::SCmdDeadPartyMem;

                lpDeadInfo->m_cTypeCode         = eTypeCode;            // 타입코드
                lpDeadInfo->m_usDeadMemCurrHP   = lpDeadCharacter->GetStatus().m_nNowHP;      // 죽은넘 HP
                lpDeadInfo->m_usDeadMemCurrMP   = lpDeadCharacter->GetStatus().m_nNowMP;      // 죽은넘 MP

                lpDeadInfo->m_dwDeadCID         = lpDeadCharacter->GetCID();            // 죽은넘 CID
                lpDeadInfo->m_dwDeadPID         = lpDeadCharacter->GetPID();            // 죽은넘 PID                
                strncpy(lpDeadInfo->m_szDeadName, lpDeadCharacter->GetCharacterName(), CHAR_INFOST::MAX_NAME_LEN);
                lpDeadInfo->m_szDeadName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

				lpDeadInfo->m_cDeadNation		= lpDeadCharacter->GetNation();			// 죽은넘 국적
				lpDeadInfo->m_cDeadClass		= (unsigned char)lpDeadCharacter->GetClass();			// 죽은넘 클래스
				lpDeadInfo->m_cRefNation		= 0;
				lpDeadInfo->m_cRefClass			= 0;

                if(0 != lpReference)
                {
                    lpDeadInfo->m_dwReferenceID     = lpReference->GetCID();        // 죽이거나 살린넘 CID

                    if(Creature::CT_PC == Creature::GetCreatureType(lpReference->GetCID()))
                    {
                        CCharacter* lpReferenceCharacter = static_cast<CCharacter*>(lpReference);

                        strncpy(lpDeadInfo->m_szRefName, lpReferenceCharacter->GetCharacterName(), CHAR_INFOST::MAX_NAME_LEN);
                        lpDeadInfo->m_szRefName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

                        Guild::CGuild* lpGuild = 0;
                        if (0 != lpReferenceCharacter->GetGID() && 
                            0 != (lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(lpReferenceCharacter->GetGID())))
                        {
                            strncpy(lpDeadInfo->m_szRefGuildName, lpGuild->GetName(), Guild::MAX_GUILD_NAME_LEN);
                            lpDeadInfo->m_szRefGuildName[Guild::MAX_GUILD_NAME_LEN - 1] = 0;
                        }
                        else
                        {
                            memset(lpDeadInfo->m_szRefGuildName, 0, sizeof(char) * Guild::MAX_GUILD_NAME_LEN);
                        }

						lpDeadInfo->m_cRefNation = lpReferenceCharacter->GetNation();
						lpDeadInfo->m_cRefClass = (unsigned char)lpReferenceCharacter->GetClass();
                    }
                    else
                    {
                        memset(lpDeadInfo->m_szRefName, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
                        memset(lpDeadInfo->m_szRefGuildName, 0, sizeof(char) * Guild::MAX_GUILD_NAME_LEN);
                    }
                }
                else
                {
                    lpDeadInfo->m_dwReferenceID = 0;
                    memset(lpDeadInfo->m_szRefName, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
                    memset(lpDeadInfo->m_szRefGuildName, 0, sizeof(char) * Guild::MAX_GUILD_NAME_LEN);
                }

                return SendStream.WrapCrypt(sizeof(PktDeadInfo), CmdAgentParty, 0, 0);
            }
        }
    }
    
    return false;
}
