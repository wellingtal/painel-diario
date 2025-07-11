#include "stdafx.h"
#include "ParseEtc.h"

#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Packet/PacketStruct/CharConfigPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>
#include "GMMemory.h"

// ºô¸µ °ü·Ã
unsigned long ParsePacket::HandleCharBillingTimeoutInfo(PktBase* lpPktBase, unsigned long *dwCharID, 
                                                        unsigned char* cRemainMinute, unsigned char* cBillingType)
{
    PktBTN* lpPktBTN = reinterpret_cast<PktBTN *>(lpPktBase);

    *dwCharID       = lpPktBTN->m_dwCharID;
    *cRemainMinute  = lpPktBTN->m_cRemainMinute;
    *cBillingType   = lpPktBTN->m_cBillingType;

    return lpPktBTN->GetError();
}

unsigned long ParsePacket::HandleCharBillingTimeCheckNotify(PktBase* lpPktBase, unsigned long *dwCharID, 
                                                            unsigned char* cRemainMinute, unsigned char* cBillingType)
{
    PktBTN* lpPktBTN = reinterpret_cast<PktBTN *>(lpPktBase);

    *dwCharID       = lpPktBTN->m_dwCharID;
    *cRemainMinute  = lpPktBTN->m_cRemainMinute;
    *cBillingType   = lpPktBTN->m_cBillingType;

    return lpPktBTN->GetError();
}

unsigned long ParsePacket::HandleCharBindPos(PktBase* lpPktBase, unsigned long *NPCID_Out, 
                                             unsigned char *Cmd_Out, POS* lpPos_Out, char *Zone_Out)
{
    PktBP* lpBPPt = reinterpret_cast<PktBP *>(lpPktBase);

    *lpPos_Out		= lpBPPt->m_Pos;
    *NPCID_Out		= lpBPPt->m_dwNPCID;
    *Cmd_Out		= lpBPPt->m_cCmd;
    *Zone_Out		= lpBPPt->m_cZone;

    return lpBPPt->GetError();
}

unsigned long ParsePacket::HandleControlOption(PktBase* lpPktBase, unsigned long *CharID_Out, RejectOption* Reject_Out)
{
    PktCOp* lpCOpPt = reinterpret_cast<PktCOp *>(lpPktBase);

    *CharID_Out		= lpCOpPt->m_dwCharID;
    *Reject_Out		= lpCOpPt->m_RejectOption;

    return lpCOpPt->GetError();
}


unsigned long ParsePacket::HandleCharStateRedistribution(PktBase* lpPktBase, ChState* lpChState_Out, SKILL* lpSkill_Out)
{
    PktSRAck* lpSRPtAck = reinterpret_cast<PktSRAck *>(lpPktBase);

    *lpChState_Out  = lpSRPtAck->m_State;
    *lpSkill_Out    = lpSRPtAck->m_Skill;

    return lpSRPtAck->GetError();
}

unsigned long ParsePacket::HandleCharStatusRetrain(PktBase* lpPktBase, ChState* lpChState_Out, SKILL* lpSkill_Out,
												   Item::ItemPos* lpIndex_Out, unsigned long *Gold_Out)
{
	PktSRTAck* lpSRTPtAck = reinterpret_cast<PktSRTAck *>(lpPktBase);

	*lpChState_Out  = lpSRTPtAck->m_State;
	*lpSkill_Out    = lpSRTPtAck->m_Skill;
	*lpIndex_Out	= lpSRTPtAck->m_ItemPos;
	*Gold_Out		= lpSRTPtAck->m_dwGold;

	return lpSRTPtAck->GetError();
}

unsigned long ParsePacket::HandleCharLotteryResult(PktBase* lpPktBase, unsigned long *CharID_Out, 
                                                   Item::ItemPos* lpIndex_Out, unsigned long *dwSize_Out, 
                                                   char **lppBuffer_Out)
{
    PktLotteryResult* lpPktLR = reinterpret_cast<PktLotteryResult *>(lpPktBase);

    *CharID_Out		= lpPktLR->m_dwCharID;
    *lpIndex_Out	= lpPktLR->m_itemPos;

    *dwSize_Out		= lpPktLR->m_dwSize;
    *lppBuffer_Out	= reinterpret_cast<char*>(lpPktLR + 1);

    return lpPktLR->GetError();
}

unsigned long ParsePacket::HandleCharAdmin(PktBase* lpPktBase)
{
	PktAdmin* lpPktAdmin = reinterpret_cast<PktAdmin *>(lpPktBase);

	return lpPktAdmin->GetError();
}

unsigned long ParsePacket::HandleCharNameChange(PktBase* lpPktBase, char*& szChangedName,  
                                                unsigned char* cNameChangeCount)
{
    PktCharNameChange* lpCharNameChange = 
        reinterpret_cast<PktCharNameChange*>(lpPktBase);

    *cNameChangeCount = lpCharNameChange->m_cNameChangeCount;
    szChangedName = lpCharNameChange->m_szCharName;

    return lpPktBase->GetError();
}
