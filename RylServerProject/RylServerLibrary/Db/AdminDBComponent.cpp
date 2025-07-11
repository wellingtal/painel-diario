
#include <Log/ServerLog.h>
#include <Utility/Setup/ServerSetup.h>

#include "DBComponent.h"
#include "AdminDBComponent.h"

bool DBComponent::AdminDB::StartCharChatBan(CDBComponent& DBComponent, unsigned long dwAdminCID, 
											unsigned long dwTargetCID, unsigned long dwMinutes)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	SYSTEMTIME localTime;
    GetLocalTime(&localTime);

	char strStartTime[MAX_PATH] = "";
	char strEndTime[MAX_PATH] = "";

	sprintf(strStartTime, "cast('%d-%d-%d %d:%d:%d' as smalldatetime)", 
		localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, localTime.wMinute, localTime.wSecond);
	sprintf(strEndTime, "cast('2010-12-31 00:00:00' as smalldatetime)");

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
		"INSERT INTO TblChatBanList(CID, ServerGroupID, RemainTime, AdminCID, StartTime, EndTime) VALUES(%d, %d, %d, %d, %s, %s)", 
		dwTargetCID, CServerSetup::GetInstance().GetServerGroup(), dwMinutes, dwAdminCID, strStartTime, strEndTime);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "ä�� ���� ĳ���� �߰� ���� : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::AdminDB::GetCharChatBan(CDBComponent& DBComponent, unsigned long dwCID, unsigned long& dwMinutes)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
		"SELECT RemainTime FROM TblChatBanList WHERE CID=%d AND RemainTime>0", dwCID);

	if (!DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), (void*)&dwMinutes)) 
	{
		dwMinutes = 0;
		return true;
	}

	return true;
}

bool DBComponent::AdminDB::EndCharChatBan(CDBComponent& DBComponent, unsigned long dwCID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	SYSTEMTIME localTime;
    GetLocalTime(&localTime);

	char strEndTime[MAX_PATH] = "";

	sprintf(strEndTime, "cast('%d-%d-%d %d:%d:%d' as smalldatetime)", 
		localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, localTime.wMinute, localTime.wSecond);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
		"UPDATE TblChatBanList SET RemainTime=0, EndTime=%s WHERE CID=%d AND RemainTime>0", strEndTime, dwCID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "ĳ���� ä�� ���� ���� ���� : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::AdminDB::UpdateCharChatBanTime(CDBComponent& DBComponent, unsigned long dwCID, unsigned long dwMinutes)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
		"UPDATE TblChatBanList SET RemainTime=%d WHERE CID=%d AND RemainTime>0", dwMinutes, dwCID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "ä�� ���� �ð� ������Ʈ ���� : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}
