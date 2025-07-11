#ifndef _GAMA_CLIENT_PARSE_CASTLE_PACKET_H_
#define _GAMA_CLIENT_PARSE_CASTLE_PACKET_H_

#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Packet/PacketStruct/CharMovePacket.h>

// ���� ����
struct PktBase;

namespace ParsePacket
{
	// ���� ������Ʈ ���� ��Ŷ
	unsigned long HandleCharCreateCastle(PktBase* lpPktBase, unsigned long* dwCastleID, unsigned char* cNation, unsigned char* cZone,
										 unsigned char* cNameID, unsigned char* cUpgradeStep, unsigned char* cInvincibleCount,
										 unsigned short* wTotalSiegeCount, unsigned long* dwTotalTaxMoney, float* fPosX,
										 float* fPosY, float* fPosZ, unsigned short* wItemID, unsigned char* cItemNum,
										 CastleRight* castleRight, CastleTaxInfo* castleTaxInfo);

	unsigned long HandleCharCreateCamp(PktBase* lpPktBase, unsigned long* dwActorCID, unsigned long* dwCID, unsigned long* dwCampID,
									   unsigned long* dwGID, unsigned long* dwHP, unsigned short* wObjectType, unsigned char* cState,
									   unsigned char* cSubState, unsigned char* cUpgradeStep, unsigned char* cNation, CampRight* campRight,
									   float* fPointX, float* fPointY, float* fPointZ, unsigned char* cMaterial);

	unsigned long HandleCharCreateSiegeArms(PktBase* lpPktBase, unsigned long* dwCID, unsigned long* dwOwnerID, unsigned char* cNation,
											unsigned long* dwHP, unsigned short* wObjectType, unsigned char* cState, unsigned char* cUpgradeStep,
											float* fPointX, float* fPointY, float* fPointZ);


	// ���� ��� ��Ŷ
	unsigned long HandleCharCastleCmd(PktBase* lpPktBase, unsigned long* dwCastleID, unsigned long* dwCastleObjectID, unsigned long* dwValue1,
									  unsigned long* dwValue2, unsigned char* cState, unsigned char* cSubCmd);

	unsigned long HandleCharCampCmd(PktBase* lpPktBase, unsigned long* dwCID, unsigned long* dwCampID, unsigned char* cState,
									unsigned long* dwValue1, unsigned long* dwValue2, unsigned char* cSubCmd);

	unsigned long HandleCharSiegeArmsCmd(PktBase* lpPktBase, unsigned long* dwCID, unsigned long* dwArmsID,
										 unsigned long* dwValue, unsigned char* cState, unsigned char* cSubCmd);


	// ���� ���� ���� ��Ŷ
	unsigned long HandleCharCastleRight(PktBase* lpPktBase, unsigned long* dwCastleID, CastleRight* castleRight);

	unsigned long HandleCharCampRight(PktBase* lpPktBase, unsigned long* dwCID, unsigned long* dwCampID, CampRight* campRight);


	// ���� ��ε� ĳ��Ʈ ��Ŷ
	unsigned long HandleCharSiegeBroadCast(PktBase* lpPktBase, unsigned long* dwCID, unsigned long* dwOwnerID, unsigned long* dwGID,
										   unsigned short* wObjectType, unsigned long* dwNowHP, unsigned long* dwMaxHP,
										   unsigned char* cState, unsigned char* cSubState, unsigned char* cUpgradeStep,
										   unsigned char* cUpgradeType, float* fDefaultDir, unsigned long* ppdwRidersID, CNetworkPos* netPos);

	unsigned long HandleCharSiegeBroadCast2nd(PktBase* lpPktBase, unsigned long* dwObjectCount, PktSiegeBroadCast* siegeObjectArray);

	unsigned long HandleCharNewSiegeBroadCast(PktBase* lpPktBase, unsigned long* dwCID, unsigned char* cBroadCastType);

	// ���� �ð� ��Ŷ
	unsigned long HandleCharGameTimeInfo(PktBase* lpPktBase, GuildTimeInfo* lpGuildTimeInfo, RealmTimeInfo* lpRealmTimeInfo, SiegeTimeInfo* lpSiegeTimeInfo, EnterTimeInfo* lpEnterTimeInfo,
										EventTimeInfo* lpEventTimeInfo, unsigned char* cType, unsigned char* cCastleNum, CastleSiegeInfo* lpCastleSiegeInfo);

	// ���� ���ڽ� ��밡�� ���� ��Ŷ
	unsigned long HandleCharStealthInfo(PktBase* lpPktBase, bool* bUseStealth);

	// ������ ���� Ƚ�� ���� ������Ʈ
	unsigned long HandleCharCastleSiegeCount(PktBase* lpPktBase, unsigned long* dwCastleID, unsigned char* cSiegeCount,
											 unsigned char* cInvincibleCount, unsigned short* wTotalSiegeCount);

	// ���� �ð��� �ӽ� ���� -> ���� ���� �̵� ����
	unsigned long HandleCharCastleTaxMove(PktBase* lpPktBase, unsigned long* dwCastleID, unsigned long* dwMoveTempMoney, unsigned long* dwResultTaxMoney);


	// ��� ��� ���� �޼��� ��Ŷ
	unsigned long HandleCharCampMessage(PktBase* lpPktBase, char* szGuildName, char* szMasterName, char* szPCName, unsigned char* cMsgCmd,
										unsigned long* dwCampID, unsigned char* cZone, unsigned char* cRemainTime, unsigned char* cNation,
										PktCampMessage::sRight* Right, bool* bNotify, int* nValue1, int* nValue2, unsigned char* cTimeChk);

	// ä���� ���� ���� ����Ʈ ��û ��� ��Ŷ
	unsigned long HandleCharMiningCampMineralInfo(PktBase* lpPktBase, unsigned long* dwCampID, unsigned char* cState, unsigned long* dwRemainTime,
												   unsigned char* cMineralType, unsigned char* cNum, CampMineralInfo* lpMineralList);

	
	// �� ��¡�� ���� ������ �ֱ�/����
	unsigned long HandleCharTakeCastleJewel(PktBase* lpPktBase, unsigned long* dwCID, unsigned long* dwCastleID, unsigned char* cSubCmd,
										unsigned short* wItemID, unsigned char* cIndex, unsigned char* cNum, Item::ItemPos* pos);
	
	// ��� ��� ���� �ֱ�/����
	unsigned long HandleCharTakeMaterial(PktBase* lpPktBase, unsigned long* dwCID, unsigned long* dwCampID, unsigned char* cSubCmd,
										unsigned char* cNum, Item::ItemPos* pos);

	// �� ���� ���� ���� ����Ʈ ��û ��� ��Ŷ
	unsigned long HandleCharCastleMineralInfo(PktBase* lpPktBase, unsigned long* dwCastleID, unsigned char* cNum, CastleMineral* lpMineralList);

	// ���, ���� ���� ����, Ż��.
	unsigned long HandleCharWarOnOff(PktBase* lpPktBase, unsigned long* dwCID, unsigned char* cFlag, unsigned char* cType);

	// �������� �������� ����Ʈ.
	unsigned long HandleCharRealmPoint(PktBase* lpPktBase, unsigned long* dwCID, unsigned char* cRealmPoint, unsigned char* cType);

	// �������� ���� ǥ�� ����
	unsigned long HandleCharRealmStatueDisplayInfo(PktBase* lpPktBase, unsigned short* wSubCmd, unsigned long* dwRealmStatueNum,
												   StatueDisplayInfo* lpDisplayInfo);
};


#endif