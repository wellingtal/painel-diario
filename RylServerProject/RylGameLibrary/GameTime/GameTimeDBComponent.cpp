#include "stdafx.h"
#include "GameTimeDBComponent.h"

#include <Log/ServerLog.h>
#include <GameTime/GameTimeConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

#include <DB/DBDefine.h>
#include <DB/DBComponent.h>

#include <Utility/Math/Math.h>
#include <Utility/Debug/PerformanceCheck.h>

bool DBComponent::GameTimeDB::UpdateGameTimeInfo(CDBComponent& DBComponent, unsigned char cGuildTime, unsigned char cRealmTime,
												 unsigned char cSiegeTime, unsigned char cMiningTime)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
		"UPDATE TblGameTime SET GuildTime=%d, RealmTime=%d, SiegeTime=%d, MiningTime=%d", cGuildTime, cRealmTime, cSiegeTime, cMiningTime);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "���� �ð� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::GameTimeDB::UpdateGuildTime(CDBComponent& DBComponent, unsigned char cGuildTime)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGameTime SET GuildTime=%d", cGuildTime);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "����� �ð� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::GameTimeDB::UpdateRealmTime(CDBComponent& DBComponent, unsigned char cRealmTime)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGameTime SET RealmTime=%d", cRealmTime);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "������ �ð� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::GameTimeDB::UpdateSiegeTime(CDBComponent& DBComponent, unsigned char cSiegeTime)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGameTime SET SiegeTime=%d", cSiegeTime);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "������ �ð� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::GameTimeDB::UpdateEnterTime(CDBComponent& DBComponent, unsigned char cEnterTime)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGameTime SET EnterTime=%d", cEnterTime);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "�ű��� �������� �ð� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}

bool DBComponent::GameTimeDB::UpdateMiningTime(CDBComponent& DBComponent, unsigned char cMiningTime)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGameTime SET MiningTime=%d", cMiningTime);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "ä���� �ӽ� ä�� �ð� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}


bool DBComponent::GameTimeDB::UpdateEventTime(CDBComponent& DBComponent, unsigned char cEventTime)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGameTime SET EventTime=%d", cEventTime);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG2(g_Log, "�����̺�Ʈ �ð� ���� ������Ʈ ���� : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	return true;
}
