#include "stdafx.h"
#include "SendCharCastle.h"

#include <Utility/Setup/ServerSetup.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>

#include <Castle/CastleMgr.h>
#include <Creature/Siege/SiegeObjectMgr.h>
#include <GameTime/GameTimeMgr.h>

// �� ���� ���� (�� ���� ������Ʈ ����)
bool GameClientSendPacket::SendCharCastleInfo(CSendStream& SendStream)
{
	return Castle::CCastleMgr::GetInstance().SendCastleInfo(SendStream);
}

// ���� �ð� ���� ����
bool GameClientSendPacket::SendCharGameTimeInfo(CSendStream& SendStream)
{
	return CGameTimeMgr::GetInstance().SendGameTimeInfo(SendStream);
}

// ���ڽ� ��밡�� ���� ���� ����
bool GameClientSendPacket::SendStealthInfo(CSendStream& SendStream, bool bUseStealth)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktStealthInfo));
	if (lpBuffer)
	{
		PktStealthInfo* lpPktSI = reinterpret_cast<PktStealthInfo*>(lpBuffer);
		lpPktSI->m_bUseStealth = bUseStealth;

		return SendStream.WrapCrypt(sizeof(PktStealthInfo), CmdStealthInfo, 0, PktBase::NO_SERVER_ERR);
	}

	return false;
}

// ��� ��� ���� ����
bool GameClientSendPacket::SendCharCampInfo(CSendStream& SendStream)
{
	return CSiegeObjectMgr::GetInstance().SendCampInfo(SendStream);
}

// �� ���� ��� ��Ŷ ����
bool GameClientSendPacket::SendCharCastleCmd(CSendStream& SendStream, unsigned long dwCastleID, unsigned long dwCastleObjectID,
											 unsigned long dwValue1, unsigned long dwValue2, unsigned char cSubCmd, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktCastleCmd));
	if (NULL != lpBuffer)
	{
		PktCastleCmd* lpPktCC = reinterpret_cast<PktCastleCmd *>(lpBuffer);

		lpPktCC->m_dwCastleID		= dwCastleID;
		lpPktCC->m_dwCastleObjectID = dwCastleObjectID;
		lpPktCC->m_dwValue1			= dwValue1;
		lpPktCC->m_dwValue2			= dwValue2;
		lpPktCC->m_cSubCmd			= cSubCmd;

		return SendStream.WrapCrypt(sizeof(PktCastleCmd), CmdCastleCmd, 0, wError);
	}

	return false;
}

// �� ���� ���� ��Ŷ ����
bool GameClientSendPacket::SendCharCastleRight(CSendStream& SendStream, unsigned long dwCasltID,
											   const CastleRight& castleRight, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktCastleRight));
	if (NULL != lpBuffer)
	{
		PktCastleRight* lpPktCastleRight = reinterpret_cast<PktCastleRight *>(lpBuffer);

		lpPktCastleRight->m_dwCID		= 0;			// C -> GS : ĳ����CID
		lpPktCastleRight->m_dwCastleID	= dwCasltID;
		lpPktCastleRight->m_CastleRight = castleRight;

		return SendStream.WrapCrypt(sizeof(PktCastleRight), CmdCastleRight, 0, wError);
	}

	return false;
}

// ��� ��� ���� ���� ��Ŷ ����
bool GameClientSendPacket::SendCharCampRight(CSendStream& SendStream, unsigned long dwCampID, 
											 const CampRight& campRight, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktCampRight));
	if (NULL != lpBuffer)
	{
		PktCampRight* lpPktCampRight = reinterpret_cast<PktCampRight *>(lpBuffer);

		lpPktCampRight->m_dwCID		= 0;			// C -> GS : ĳ����CID
		lpPktCampRight->m_dwCampID	= dwCampID;
		lpPktCampRight->m_CampRight = campRight;

		return SendStream.WrapCrypt(sizeof(PktCampRight), CmdCampRight, 0, wError);
	}

	return false;
}

// ��� ��� ���� ��� ��Ŷ ����
bool GameClientSendPacket::SendCharCampCmd(CSendStream& SendStream, unsigned long dwCID, unsigned long dwCampID,
										   unsigned char cSubCmd, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktCampCmd));
	if (NULL != lpBuffer)
	{
		PktCampCmd* lpPktCampCmd = reinterpret_cast<PktCampCmd *>(lpBuffer);

		lpPktCampCmd->m_dwCID		= dwCID;
		lpPktCampCmd->m_dwCampID	= dwCampID;
		lpPktCampCmd->m_cSubCmd		= cSubCmd;

		return SendStream.WrapCrypt(sizeof(PktCampCmd), CmdCampCmd, 0, wError);
	}

	return false;
}

// ���� ���� ���� ��� ��Ŷ ����
bool GameClientSendPacket::SendCharSiegeArmsCmd(CSendStream& SendStream, unsigned long dwCID, unsigned long dwArmsID,
												unsigned char cSubCmd, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktSiegeArmsCmd));
	if (NULL != lpBuffer)
	{
		PktSiegeArmsCmd* lpPktSACmd = reinterpret_cast<PktSiegeArmsCmd *>(lpBuffer);

		lpPktSACmd->m_dwCID		= dwCID;
		lpPktSACmd->m_dwArmsID	= dwArmsID;
		lpPktSACmd->m_cSubCmd	= cSubCmd;

		return SendStream.WrapCrypt(sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, wError);
	}

	return false;
}




// ��� ��� ���� ��Ŷ ���� ( GameServer  -->  DBAgentServer )
bool GameClientSendPacket::SendCharCreateCamp(CSendStream& AgentSendStream, unsigned long dwActorCID,
											  unsigned long dwGID, unsigned short wObjectType, const Position& pos)
{
	char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktCreateCamp));
	if (NULL != lpBuffer)
	{
		PktCreateCamp* lpPktCreateCamp = reinterpret_cast<PktCreateCamp*>(lpBuffer);

		lpPktCreateCamp->m_dwActorCID	= dwActorCID;
		lpPktCreateCamp->m_dwCID		= 0;
		lpPktCreateCamp->m_dwGID		= dwGID;
		lpPktCreateCamp->m_dwCampID		= 0;
		lpPktCreateCamp->m_dwHP			= 0;
		lpPktCreateCamp->m_wObjectType	= wObjectType;
		lpPktCreateCamp->m_cState		= 0;
		lpPktCreateCamp->m_cSubState	= 0;
		lpPktCreateCamp->m_cUpgradeStep	= 0;
		lpPktCreateCamp->m_cNation		= 0;
		lpPktCreateCamp->m_cZone		= CServerSetup::GetInstance().GetServerZone();
		lpPktCreateCamp->m_cChannel		= CServerSetup::GetInstance().GetServerChannel();
		lpPktCreateCamp->m_Pos.fPointX	= pos.m_fPointX;
		lpPktCreateCamp->m_Pos.fPointY	= pos.m_fPointY;
		lpPktCreateCamp->m_Pos.fPointZ	= pos.m_fPointZ;
		lpPktCreateCamp->m_cMaterial	= 0;

		return AgentSendStream.WrapHeader(sizeof(PktCreateCamp), CmdCreateCamp, 0, 0);
	}

	return false;
}

// ���� ���� ���� ��Ŷ ����	( GameServer  -->  DBAgentServer )
bool GameClientSendPacket::SendCharCreateSiegeArms(CSendStream& AgentSendStream, unsigned long dwOwnerID, unsigned char cNation,
												   unsigned short wType, unsigned char cUpgradeStep, const Position& pos)
{
	char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktCreateSiegeArms));
	if (NULL != lpBuffer)
	{
		PktCreateSiegeArms* lpPktCreateSiegeArms = reinterpret_cast<PktCreateSiegeArms*>(lpBuffer);

		lpPktCreateSiegeArms->m_dwCID			= 0;			// CID (���� ���� ID)
		lpPktCreateSiegeArms->m_dwOwnerID		= dwOwnerID;	// ���� ���� ���� CID
		lpPktCreateSiegeArms->m_wObjectType		= wType;		// ���� ���� Ÿ��
		lpPktCreateSiegeArms->m_cNation			= cNation;		// ���� ���� �������� ����
		lpPktCreateSiegeArms->m_dwHP			= 0;
		lpPktCreateSiegeArms->m_cState			= 0;
		lpPktCreateSiegeArms->m_cUpgradeStep	= cUpgradeStep;	// ���� ���� ���׷��̵� �ܰ�
		lpPktCreateSiegeArms->m_cZone			= CServerSetup::GetInstance().GetServerZone();
		lpPktCreateSiegeArms->m_cChannel		= CServerSetup::GetInstance().GetServerChannel();
		lpPktCreateSiegeArms->m_Pos.fPointX		= pos.m_fPointX;
		lpPktCreateSiegeArms->m_Pos.fPointY		= pos.m_fPointY;
		lpPktCreateSiegeArms->m_Pos.fPointZ		= pos.m_fPointZ;

		return AgentSendStream.WrapHeader(sizeof(PktCreateSiegeArms), CmdCreateSiegeArms, 0, 0);
	}

	return false;
}


// �� ���� ��� ��Ŷ�� DBAgent ������ ������
bool GameClientSendPacket::SendCharCastleCmdToDBAgent(CSendStream& AgentSendStream, unsigned long dwCID, unsigned long dwCastleID, unsigned long dwCastleObjectID,
													  unsigned long dwValue1, unsigned long dwValue2, unsigned char cSubCmd, unsigned short wError)
{
	char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktCastleCmd));
	if (NULL != lpBuffer)
	{
		PktCastleCmd* lpPktCastleCmd	= reinterpret_cast<PktCastleCmd*>(lpBuffer);

		lpPktCastleCmd->m_dwCID				= dwCID;
		lpPktCastleCmd->m_dwCastleID		= dwCastleID;
		lpPktCastleCmd->m_dwCastleObjectID	= dwCastleObjectID;
		lpPktCastleCmd->m_dwValue1			= dwValue1;
		lpPktCastleCmd->m_dwValue2			= dwValue2;
		lpPktCastleCmd->m_cSubCmd			= cSubCmd;

		return AgentSendStream.WrapHeader(sizeof(PktCastleCmd), CmdCastleCmd, 0, wError);
	}

	return false;
}

// ��� ��� ���� ��� ��Ŷ�� DBAgent ������ ������
bool GameClientSendPacket::SendCharCampCmdToDBAgent(CSendStream& AgentSendStream, unsigned long dwCID, unsigned long dwCampID,
													unsigned long dwValue1, unsigned long dwValue2,	unsigned char cSubCmd,
													unsigned short wError)
{
	char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktCampCmd));
	if (NULL != lpBuffer)
	{
		PktCampCmd* lpPktCampCmd = reinterpret_cast<PktCampCmd*>(lpBuffer);

		lpPktCampCmd->m_dwCID		= dwCID;
		lpPktCampCmd->m_dwCampID	= dwCampID;
		lpPktCampCmd->m_dwValue1	= dwValue1;
		lpPktCampCmd->m_dwValue2	= dwValue2;
		lpPktCampCmd->m_cSubCmd		= cSubCmd;

		return AgentSendStream.WrapHeader(sizeof(PktCampCmd), CmdCampCmd, 0, wError);
	}

	return false;
}

// ���� ���� ���� ��� ��Ŷ DBAgent ������ ������
bool GameClientSendPacket::SendCharSiegeArmsCmdToDBAgent(CSendStream& AgentSendStream, unsigned long dwCID, unsigned long dwArmsID,
														 unsigned long dwValue, unsigned char cSubCmd, unsigned short wError)
{
	char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktSiegeArmsCmd));
	if (NULL != lpBuffer)
	{
		PktSiegeArmsCmd* lpPktSACmd = reinterpret_cast<PktSiegeArmsCmd*>(lpBuffer);

		lpPktSACmd->m_dwCID		= dwCID;
		lpPktSACmd->m_dwArmsID	= dwArmsID;
		lpPktSACmd->m_dwValue	= dwValue;
		lpPktSACmd->m_cSubCmd	= cSubCmd;

		return AgentSendStream.WrapHeader(sizeof(PktSiegeArmsCmd), CmdSiegeArmsCmd, 0, wError);
	}

	return false;
}

// ��� ��� �޼��� ��Ŷ ����		( GameServer  -->  DBAgentServer )
bool GameClientSendPacket::SendCharCampMessageToDBAgent(CSendStream& AgentSendStream, unsigned long dwCampID, unsigned char cMsgCmd, unsigned short wError)
{
	char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktCampMessage));
	if (NULL != lpBuffer)
	{
		PktCampMessage* lpPktCM = reinterpret_cast<PktCampMessage*>(lpBuffer);
		memset(lpPktCM, 0, sizeof(PktCampMessage));

		lpPktCM->m_cMsgCmd	= cMsgCmd;
		lpPktCM->m_dwCampID	= dwCampID;
		lpPktCM->m_bNotify	= false;
		
		return AgentSendStream.WrapHeader(sizeof(PktCampMessage), CmdCampMessage, 0, wError);
	}

	return false;
}

bool GameClientSendPacket::SendCharCampMessageToDBAgent(CSendStream& AgentSendStream, unsigned long dwCampID, unsigned char cMsgCmd, 
														const char* szName, int nNum, int nTotal, unsigned short wError)
{
	char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktCampMessage));
	if (NULL != lpBuffer)
	{
		PktCampMessage* lpPktCM = reinterpret_cast<PktCampMessage*>(lpBuffer);
		memset(lpPktCM, 0, sizeof(PktCampMessage));

		lpPktCM->m_cMsgCmd	= cMsgCmd;
		lpPktCM->m_dwCampID	= dwCampID;
		strcpy(lpPktCM->m_szPCName, szName);
		lpPktCM->m_nValue1	= nNum;
		lpPktCM->m_nValue2	= nTotal;
		lpPktCM->m_bNotify	= false;

		return AgentSendStream.WrapHeader(sizeof(PktCampMessage), CmdCampMessage, 0, wError);
	}

	return false;
}

bool GameClientSendPacket::SendWarOnOff(CSendStream& SendStream, unsigned long dwCID, unsigned char cType,
										unsigned char cFlag, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktWarOnOff));
	if (NULL == lpBuffer) { return false; }

	PktWarOnOff* lpPktWarOnOff = reinterpret_cast<PktWarOnOff*>(lpBuffer);
	lpPktWarOnOff->m_dwCID = dwCID;	
	lpPktWarOnOff->m_cType = cType;
	lpPktWarOnOff->m_cFlag = cFlag;

	return SendStream.WrapCrypt(sizeof(PktWarOnOff), CmdWarOnOff, 0, wError);
}

bool GameClientSendPacket::SendRealmPoint(CSendStream& SendStream, unsigned long dwCID, unsigned char cRealmPoint, unsigned char cType, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktRealmPoint));

	if(!lpBuffer)
	{ 
		return false;
	}

	PktRealmPoint* lpPktRealmPoint = reinterpret_cast<PktRealmPoint*>(lpBuffer);

	lpPktRealmPoint->m_dwCID		= dwCID;	
	lpPktRealmPoint->m_cRealmPoint	= cRealmPoint;
	lpPktRealmPoint->m_cType		= cType;
	
	return SendStream.WrapCrypt(sizeof(PktRealmPoint), CmdRealmPoint, 0, wError);
}
