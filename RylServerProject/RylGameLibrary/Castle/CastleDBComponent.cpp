#include "stdafx.h"
#include "CastleDBComponent.h"

#include <Log/ServerLog.h>
#include <Castle/CastleConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

#include <DB/DBDefine.h>
#include <DB/DBComponent.h>

#include <Utility/Math/Math.h>
#include <Utility/Debug/PerformanceCheck.h>

#include <Creature/CreatureStructure.h>
#include <Creature/Siege/SiegeConstants.h>


// -------------------------------------------------------------------------------------------------------------------------- //
// �� ���� DB ó��

bool DBComponent::CastleDB::InsertCastleMineralInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cMineralType,
													unsigned short wMineralID, unsigned short wAmount)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"INSERT INTO TblCastleMineralInfo(nCastleID, snMineralID, snAmount, tnFlag) VALUES(%d, %d, %d, %d)",
		dwCastleID, wMineralID, wAmount, cMineralType);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		switch (cMineralType)
		{
			case Siege::ACCUMULATED_MINERAL:
				SERLOG2(g_Log, "�� ���� ���� ���� �߰� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
			break;

			case Siege::TEMPORARY_MINERAL:
				SERLOG2(g_Log, "�� �ӽ� ���� ���� �߰� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
			break;
		}

		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cNation, unsigned char cSiegeCount,
											 unsigned char cInvincibleCount, unsigned short wTotalSiegeCount, unsigned long dwTotalTaxMoney,
											 unsigned short wItemID, unsigned char cItemNum)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "dbo.USPCastleUpdate '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d'",
			  dwCastleID, cNation, cSiegeCount, cInvincibleCount, wTotalSiegeCount, dwTotalTaxMoney, wItemID, cItemNum);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "�� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleUpgradeItemInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned short wItemID, unsigned char cItemNum)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
		"UPDATE TblCastleInfo SET snItemID=%d, tnItemNum=%d WHERE nCastleID=%d", wItemID, cItemNum, dwCastleID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "�� ��¡�� ���� ������ ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleSiegeCount(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cSiegeCount,
												   unsigned char cInvincibleCount, unsigned short wTotalSiegeCount)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
		"UPDATE TblCastleInfo SET tnSiegeCount=%d, tnInvincibleCount=%d, snTotalSiegeCount=%d WHERE nCastleID=%d",
		cSiegeCount, cInvincibleCount, wTotalSiegeCount, dwCastleID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "�� ���� Ƚ�� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleTotalTaxMoney(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned long dwTotalTaxMoney)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
		"UPDATE TblCastleInfo SET nTotalTaxMoney=%d WHERE nCastleID=%d", dwTotalTaxMoney, dwCastleID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "���� ���� ���� ȸ���� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleRight(CDBComponent& DBComponent, unsigned long dwCastleID, char* szRight, unsigned short wSize)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCastleInfo SET bRight=0x");

	char* lpDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());
	char* lpPos = szRight;
	for (int nIndex = 0; nIndex < wSize; ++nIndex, ++lpPos, lpDest += 2)
	{
		Math::Convert::Hex08ToStr(lpDest, *lpPos);
	}

    const int MAX_FOOTER = 512;
	char szFooter[MAX_FOOTER];
	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	sprintf(szFooter, " WHERE nCastleID=%d", dwCastleID);

	strcat(DBComponent.GetQueryBuffer(), szFooter);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "�� ���� ���� ���� ���� : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}


bool DBComponent::CastleDB::UpdateCastleTax(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cType, unsigned char cTax)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblCastleTaxInfo SET tnTax=%d WHERE nCastleID=%d AND tnType=%d", cTax, dwCastleID, cType);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG2(g_Log, "�� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleTaxChangable(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cType, unsigned char cTaxChangable)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblCastleTaxInfo SET tnTaxChangable=%d WHERE nCastleID=%d AND tnType=%d", cTaxChangable, dwCastleID, cType);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG2(g_Log, "�� ���� ���� ���� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleTaxMoney(CDBComponent& DBComponent, unsigned long dwCastleID,
												 unsigned char cType, unsigned long dwTempTaxMoney, unsigned long dwTaxMoney)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblCastleTaxInfo SET nTempMoney=%d, nTaxMoney=%d WHERE nCastleID=%d AND tnType=%d",
		dwTempTaxMoney, dwTaxMoney, dwCastleID, cType);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG2(g_Log, "�� ���� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleMineralInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cMineralType,
													unsigned short wMineralID, unsigned short wAmount)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblCastleMineralInfo SET snAmount=%d WHERE nCastleID=%d AND tnFlag=%d AND snMineralID=%d",
		wAmount, dwCastleID, cMineralType, wMineralID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "�� ���� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::DeleteCastleTaxInfo(CDBComponent& DBComponent, unsigned long dwCastleID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblCastleTaxInfo SET tnTax=0, nTempMoney=0, nTaxMoney=0, tnTaxChangable=%d WHERE nCastleID=%d", Castle::TAX_ENABLE, dwCastleID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "�� ���� �� ���� �ʱ�ȭ : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::DeleteCastleMineralInfo(CDBComponent& DBComponent, unsigned long dwCastleID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"DELETE FROM TblCastleMineralInfo WHERE nCastleID=%d", dwCastleID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "�� ���� ���� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::DeleteCastleMineralInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cMineralType)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"DELETE FROM TblCastleMineralInfo WHERE nCastleID=%d AND tnFlag=%d", dwCastleID, cMineralType);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "�� ���� ���� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::DeleteCastleMineralInfo(CDBComponent& DBComponent, unsigned long dwCastleID,
													unsigned char cMineralType, unsigned short wMineralID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCastleID = (dwCastleID & ~Castle::CASTLE_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"DELETE FROM TblCastleMineralInfo WHERE nCastleID=%d AND tnFlag=%d AND snMineralID=%d", dwCastleID, cMineralType, wMineralID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "�� ���� ���� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

// -------------------------------------------------------------------------------------------------------------------------- //
// �� ������Ʈ DB ó��

bool DBComponent::CastleDB::UpdateCastleObjectHP(CDBComponent& DBComponent, unsigned long dwCID, unsigned long dwHP)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCID = (dwCID & ~Creature::SIEGE_OBJECT_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCastleCreatureInfo SET nHP=%d WHERE nCreatureID=%d", dwHP, dwCID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG2(g_Log, "�� ������Ʈ HP ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleObjectType(CDBComponent& DBComponent, unsigned long dwCID, unsigned short wObjectType, unsigned long dwOwnerID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCID = (dwCID & ~Creature::SIEGE_OBJECT_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCastleCreatureInfo SET snObjectType=%d, nOwnerID=%d WHERE nCreatureID=%d", wObjectType, dwOwnerID, dwCID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG2(g_Log, "�� ������Ʈ Ÿ�� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleObjectState(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cState, unsigned char cSubState)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCID = (dwCID & ~Creature::SIEGE_OBJECT_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCastleCreatureInfo SET tnState=%d, tnSubState=%d WHERE nCreatureID=%d",
			  cState, cSubState, dwCID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG2(g_Log, "�� ������Ʈ ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleObjectUpgrade(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cUpgradeStep, unsigned char cUpgradeType)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCID = (dwCID & ~Creature::SIEGE_OBJECT_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCastleCreatureInfo SET tnUpgradeStep=%d, tnUpgradeType=%d WHERE nCreatureID=%d",
			  cUpgradeStep, cUpgradeType, dwCID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG2(g_Log, "�� ������Ʈ ���׷��̵� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleObjectTime(CDBComponent& DBComponent, unsigned long dwCID, SYSTEMTIME RemainTime)
{
	char strTime[MAX_PATH] = "";
	if (0 == RemainTime.wYear)
	{
		sprintf(strTime, "NULL");
	}
	else
	{
		sprintf(strTime, "cast('%d-%d-%d %d:%d:%d' as smalldatetime)", RemainTime.wYear, RemainTime.wMonth, RemainTime.wDay, 
				RemainTime.wHour, RemainTime.wMinute, RemainTime.wSecond, RemainTime.wMilliseconds);
	}

	dwCID = (dwCID & ~Creature::SIEGE_OBJECT_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCastleCreatureInfo SET RemainTime=%s WHERE nCreatureID=%d", strTime, dwCID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "�� ������Ʈ ���� �ð� ���� ����: %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCastleObjectLastUseTime(CDBComponent& DBComponent, unsigned long dwCID, SYSTEMTIME LastUseTime)
{
	char strTime[MAX_PATH] = "";
	if (0 == LastUseTime.wYear)
	{
		sprintf(strTime, "NULL");
	}
	else
	{
		sprintf(strTime, "cast('%d-%d-%d %d:%d:%d' as smalldatetime)", LastUseTime.wYear, LastUseTime.wMonth, LastUseTime.wDay, 
				LastUseTime.wHour, LastUseTime.wMinute, LastUseTime.wSecond, LastUseTime.wMilliseconds);
	}

	dwCID = (dwCID & ~Creature::SIEGE_OBJECT_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCastleCreatureInfo SET LastUseTime=%s WHERE nCreatureID=%d", strTime, dwCID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "�� ������Ʈ ������ ��� �ð� ���� ����: %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}
// -------------------------------------------------------------------------------------------------------------------------- //


// -------------------------------------------------------------------------------------------------------------------------- //
// ��� ��� ���� DB ó��

bool DBComponent::CastleDB::InsertCampInfo(CDBComponent& DBComponent, unsigned long dwGID, unsigned short wObjectType,
										   unsigned char cZone, unsigned char cChannel, const POS& Pos, 
                                           unsigned long* dwCampID, unsigned short* wError)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	unsigned long dwResult[2];

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "dbo.USPCampCreate '%d', '%d', '%d', '%d', '%f', '%f', '%f'",
			  dwGID, wObjectType, cZone, cChannel, Pos.fPointX, Pos.fPointY, Pos.fPointZ);

	if (false == DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), (void *)dwResult)) 
	{
		SERLOG2(g_Log, "��� ��� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	*dwCampID = dwResult[0];
	*wError = static_cast<unsigned short>(dwResult[0]);		// 0 �̸� ����, 0 ���� ũ�� ����

	return true;
}

bool DBComponent::CastleDB::InsertMiningCampMineralInfo(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cMineralType,
														unsigned short wMineralID, unsigned short wAmount)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
			"INSERT INTO TblCampMineralInfo(nCampID, snMineralID, snAmount, tnFlag) VALUES(%d, %d, %d, %d)",
			dwCampID, wMineralID, wAmount, cMineralType);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		switch (cMineralType)
		{
		case Siege::ACCUMULATED_MINERAL:
			SERLOG2(g_Log, "ä���� ���� ���� �߰� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
			break;

		case Siege::TEMPORARY_MINERAL:
			SERLOG2(g_Log, "ä���� �ӽ� ���� �߰� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
			break;
		}

		return false;
	}

	return true;
}

bool DBComponent::CastleDB::InsertCampShopInfo(CDBComponent& DBComponent, unsigned long dwCampID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"INSERT INTO TblCampShopInfo(CampID, Item, TempMoney, Tax) VALUES(%d, NULL, 0, 0)", dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "��� ��� ���� ���� �߰� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::InsertWorldWeaponInfo(CDBComponent& DBComponent, unsigned char cZone, unsigned char cChannel, unsigned char cNation)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	// �̹� ������ ������Ʈ��, ���ٸ� �μ�Ʈ��
	const int MAX_ROWS = 20;
	int nGetRows = 0;

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, 
		"SELECT TblWorldWeaponInfo.tnZone, "
		"TblWorldWeaponInfo.tnChannel, "
		"TblWorldWeaponInfo.tnKarRemainSiegeTime, "
		"TblWorldWeaponInfo.tnMerRemainSiegeTime "
		"FROM TblWorldWeaponInfo ");

	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;

	if (!DBComponent.ExecuteQuery(szBuffer))
	{
		ERRLOG0(g_Log, "WorldWeaponInfo DB ������ ����");
		return false;
	}

	WorldWeaponInfoDB* weaponInfo = new WorldWeaponInfoDB[MAX_ROWS];
	memset(weaponInfo, 0, sizeof(WorldWeaponInfoDB) * MAX_ROWS);

	if (DBComponent.GetData((void**)weaponInfo, sizeof(WorldWeaponInfoDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows)
		{
			// ���� �߰�
			switch (cNation)
			{
				case Creature::KARTERANT:
				{
					_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
						"INSERT INTO TblWorldWeaponInfo(tnZone, tnChannel, tnKarRemainSiegeTime, tnMerRemainSiegeTime) VALUES(%d, %d, %d, 0)",
						cZone, cChannel, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
				}
				break;

				case Creature::MERKADIA:
				{
					_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
						"INSERT INTO TblWorldWeaponInfo(tnZone, tnChannel, tnKarRemainSiegeTime, tnMerRemainSiegeTime) VALUES(%d, %d, 0, %d)",
						cZone, cChannel, Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT);
				}
				break;
			}

			if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
			{
				SERLOG2(g_Log, "���� ���� �ı� ���� �߰� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
				return false;
			}
		}
		else
		{
			// ������Ʈ
			switch (cNation)
			{
				case Creature::KARTERANT:
				{
					_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
						"UPDATE TblWorldWeaponInfo SET tnKarRemainSiegeTime=%d WHERE tnZone=%d AND tnChannel=%d",
						Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT, cZone, cChannel);
				}
				break;

				case Creature::MERKADIA:
				{
					_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
						"UPDATE TblWorldWeaponInfo SET tnMerRemainSiegeTime=%d WHERE tnZone=%d AND tnChannel=%d",
						Siege::WEAPON_REBUILD_SIEGE_TIME_COUNT, cZone, cChannel);
				}
				break;
			}

			if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
			{
				SERLOG2(g_Log, "���� ���� �ı� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
				return false;
			}
		}
	}

	return true;
}

bool DBComponent::CastleDB::DeleteCampInfo(CDBComponent& DBComponent, unsigned long dwCampID, unsigned short wCampType)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);
	
	dwCampID = (dwCampID & ~Castle::CAMP_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "dbo.USPCampDelete %d", dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG2(g_Log, "��� ��� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	if ( Siege::MINING_CAMP == wCampType )
	{
		DeleteMiningCampMineralInfo(DBComponent, dwCampID, Siege::ACCUMULATED_MINERAL);
		DeleteMiningCampMineralInfo(DBComponent, dwCampID, Siege::TEMPORARY_MINERAL);
	}

	return true;
}

bool DBComponent::CastleDB::DeleteMiningCampMineralInfo(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cMineralType)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    dwCampID = (dwCampID & ~Castle::CAMP_BIT);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"DELETE FROM TblCampMineralInfo WHERE nCampID=%d AND tnFlag=%d", dwCampID, cMineralType);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		switch (cMineralType)
		{
			case Siege::ACCUMULATED_MINERAL:
				SERLOG2(g_Log, "ä���� ���� ���� ���� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
				break;

			case Siege::TEMPORARY_MINERAL:
				SERLOG2(g_Log, "ä���� �ӽ� ���� ���� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
				break;
		}

		return false;
	}

	return true;
}

bool DBComponent::CastleDB::DeleteMiningCampMineralInfo(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cMineralType, unsigned short wMineralID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"DELETE FROM TblCampMineralInfo WHERE nCampID=%d AND tnFlag=%d AND snMineralID=%d", dwCampID, cMineralType, wMineralID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		switch (cMineralType)
		{
			case Siege::ACCUMULATED_MINERAL:
				SERLOG2(g_Log, "ä���� ���� ���� ���� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
				break;

			case Siege::TEMPORARY_MINERAL:
				SERLOG2(g_Log, "ä���� �ӽ� ���� ���� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
				break;
		}

		return false;
	}

	return true;
}

bool DBComponent::CastleDB::DeleteCampShopInfo(CDBComponent& DBComponent, unsigned long dwCampID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"DELETE FROM TblCampShopInfo WHERE CampID=%d", dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "��� ��� ���� ���� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::DeleteWorldWeaponInfo(CDBComponent& DBComponent, unsigned char cZone, unsigned char cChannel)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"DELETE FROM TblWorldWeaponInfo WHERE tnZone=%d AND tnChannel=%d", cZone, cChannel);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "���� ���� �ı� ���� ���� ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCampRight(CDBComponent& DBComponent, unsigned long dwCampID, char* szRight, unsigned short wSize)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCampInfo SET bRight=0x");

	char* lpDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());
	char* lpPos = szRight;
	for (int nIndex = 0; nIndex < wSize; ++nIndex, ++lpPos, lpDest += 2)
	{
		Math::Convert::Hex08ToStr(lpDest, *lpPos);
	}

    const int MAX_FOOTER = 512;
    char szFooter[MAX_FOOTER];

    dwCampID = (dwCampID & ~Castle::CAMP_BIT);
	sprintf(szFooter, " WHERE nCampID=%d", dwCampID);

	strcat(DBComponent.GetQueryBuffer(), szFooter);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "��� ��� ���� ���� ���� ���� : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;	
}

bool DBComponent::CastleDB::UpdateCampHP(CDBComponent& DBComponent, unsigned long dwCampID, unsigned long dwHP)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCampInfo SET nHP=%d WHERE nCampID=%d", dwHP, dwCampID);
	
	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG1(g_Log, "��� ��� HP ������Ʈ ���� : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCampType(CDBComponent& DBComponent, unsigned long dwCampID, unsigned short wType)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCampInfo SET snObjectType=%d WHERE nCampID=%d", wType, dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG1(g_Log, "��� ��� Ÿ�� ������Ʈ ���� : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCampState(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cState)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCampInfo SET tnState=%d WHERE nCampID=%d", cState, dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG1(g_Log, "��� ��� ���� ������Ʈ ���� : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCampSubState(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cSubState)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCampInfo SET tnSubState=%d WHERE nCampID=%d", cSubState, dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG1(g_Log, "��� ��� ���� ���� ������Ʈ ���� : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCampUpgrade(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cUpgradeStep)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCampInfo SET tnUpgradeStep=%d WHERE nCampID=%d", cUpgradeStep, dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG1(g_Log, "��� ��� ���׷��̵� ������Ʈ ���� : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCampTime(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cType, SYSTEMTIME TimeValue)
{
	char strTime[MAX_PATH] = "";
	if (0 == TimeValue.wYear)
	{
		sprintf(strTime, "NULL");
	}
	else
	{
		sprintf(strTime, "cast('%d-%d-%d %d:%d:%d' as smalldatetime)", TimeValue.wYear, TimeValue.wMonth, TimeValue.wDay, 
				TimeValue.wHour, TimeValue.wMinute, TimeValue.wSecond, TimeValue.wMilliseconds);
	}

	char strColumn[MAX_PATH] = "";
	switch (cType)
	{
	case Siege::TYPE_REMAIN_TIME:
		sprintf(strColumn, "RemainTime");
		break;			

	case Siege::TYPE_LAST_USE_TIME:
		sprintf(strColumn, "LastUseTime");
		break;			
	}

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);
	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblCampInfo SET %s=%s WHERE nCampID=%d", strColumn, strTime, dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "��� ��� �ð� ���� ����: %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;	
}

bool DBComponent::CastleDB::UpdateMiningCampMineralInfo(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cMineralType,
														  unsigned short wMineralID, unsigned short wAmount)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblCampMineralInfo SET snAmount=%d WHERE nCampID=%d AND tnFlag=%d AND snMineralID=%d", wAmount, dwCampID, cMineralType, wMineralID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		switch (cMineralType)
		{
			case Siege::ACCUMULATED_MINERAL:
				SERLOG2(g_Log, "ä���� ���� ���� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
				break;

			case Siege::TEMPORARY_MINERAL:
				SERLOG2(g_Log, "ä���� �ӽ� ���� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
				break;
		}

		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCampShopInfo(CDBComponent& DBComponent, unsigned long dwCampID, 
											   const char* lpItemBuffer, unsigned long dwItemBufferSize, unsigned char cItemNum, 
											   unsigned long dwTempSafe, unsigned char cTax)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);

	_snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
		"SELECT Item FROM TblCampShopInfo WHERE CampID=%d", dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update)) 
	{
		SERLOG2(g_Log, "��� ��� ���� ���� ������Ʈ ���� : %s : Query:%s", 
			DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	const int MAX_BUFFER_SIZE = sizeof(unsigned long) + sizeof(unsigned char) + CampShopInfoDB::MAX_CONTAINER_SIZE;
	char szBuffer[MAX_BUFFER_SIZE] = { 0, };

	unsigned long* lpTotalSize = reinterpret_cast<unsigned long*>(szBuffer);
	unsigned char* lpItemNum = reinterpret_cast<unsigned char*>(lpTotalSize + 1);
	char* lpData = reinterpret_cast<char*>(lpItemNum + 1);

	*lpTotalSize = sizeof(unsigned long) + sizeof(unsigned char) + dwItemBufferSize + sizeof(unsigned long) * cItemNum;
	*lpItemNum = cItemNum;
	memcpy(lpData, lpItemBuffer, dwItemBufferSize + sizeof(unsigned long) * cItemNum);

	if (false == DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)szBuffer)) 
	{
		SERLOG2(g_Log, "��� ��� ���� ���� ������Ʈ ���� : %s : Query:%s", 
			DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblCampShopInfo SET TempMoney=%d, Tax=%d WHERE CampID=%d", dwTempSafe, cTax, dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "��� ��� ���� ���� ������Ʈ ���� : %s : Query:%s", 
			DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateFertilityInfo(CDBComponent& DBComponent, unsigned char cZone, unsigned char cChannel,
												unsigned long dwVeinColor, unsigned long dwFertility)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblFertilityInfo SET nFertility=%d WHERE nVeinColor=%d AND tnZone=%d AND tnChannel=%d", 
		dwFertility, dwVeinColor, cZone, cChannel);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "���� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateWorldWeaponInfo(CDBComponent& DBComponent, unsigned char cZone, unsigned char cChannel,
												  unsigned char cKarRemainSiegeTime, unsigned char cMerRemainSiegeTime)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblWorldWeaponInfo SET tnKarRemainSiegeTime=%d, tnMerRemainSiegeTime=%d WHERE tnZone=%d AND tnChannel=%d", 
		cKarRemainSiegeTime, cMerRemainSiegeTime, cZone, cChannel);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "���� ���� �ı� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCampMaterial(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cMaterial)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblCampInfo SET tnMaterial=%d WHERE nCampID=%d", cMaterial, dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "��� ��� ���� �� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::CastleDB::UpdateCampSiegeCount(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cSiegeCount)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	dwCampID = (dwCampID & ~Castle::CAMP_BIT);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(),
		"UPDATE TblCampInfo SET tnSiegeCount=%d WHERE nCampID=%d", cSiegeCount, dwCampID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "��� ��� ���� ���� Ƚ�� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}


// -------------------------------------------------------------------------------------------------------------------------- //

