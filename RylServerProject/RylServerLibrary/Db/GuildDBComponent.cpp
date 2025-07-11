
#include "stdafx.h"
#include "DBComponent.h"
#include "GuildDBComponent.h"

#include <Utility/Math/Math.h>
#include <Utility/Debug/PerformanceCheck.h>
#include <Utility/Setup/ServerSetup.h>

#include <Log/ServerLog.h>


bool DBComponent::GuildDB::InsertGuildInfo(CDBComponent& DBComponent, char* szGuildName,
                                           unsigned char cNation, char* szRight, unsigned short wSize, 
                                           unsigned long *dwGID, unsigned short *wError)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	unsigned long dwResult[2];

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
        "dbo.USPGuildCreate '%s', '%d', '0', '0', 0x", szGuildName, cNation);

	char* lpDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());

	// Query 뒷부분을 완성해준다. (권한 부분)
	char* lpPos = szRight;
	for (int nIndex = 0; nIndex < wSize; ++nIndex, ++lpPos, lpDest += 2)
	{
        Math::Convert::Hex08ToStr(lpDest, *lpPos);
	}
    
    size_t  nLen = strlen(DBComponent.GetQueryBuffer());
    lpDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());
    
    _snprintf(lpDest, DBComponent.GetQueryBufferLen() - nLen, ", %d", 
        CServerSetup::GetInstance().GetAgentServerType());

	if (!DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), (void *)dwResult)) 
	{
        SERLOG2(g_Log, "길드 생성 실패 : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	*wError = static_cast<unsigned short>(dwResult[0]);
	*dwGID = dwResult[1];  

	return true;
}

bool DBComponent::GuildDB::InsertGuildMember(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwCID, unsigned char cTitle, unsigned char cRank)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	unsigned long dwResult = 0;

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "dbo.USPGuildMemberInsert '%d', '%d', '%d', '%d'", 
		dwGID, dwCID, cTitle, cRank);

	if (false == DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), (void *)&dwResult)) 
	{
        SERLOG2(g_Log, "길드 멤버 추가 실패 : %s : Query:%s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
		return false;
	}

	if (0 != dwResult)
	{
		// CharInfo에 업데이트를 실패한 경우 (존재하지 않는 CID를 보냈다던지...)
		return false;
	}

	return true;	
}

bool DBComponent::GuildDB::InsertGuildRelation(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelationType)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "INSERT INTO TblGuildRelation(nGuildID, nTargetGuildID, tnRelation) VALUES(%d, %d, %d)",
			  dwGID, dwTargetGID, cRelationType);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드 관계 추가 실패 : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::DeleteGuildInfo(CDBComponent& DBComponent, unsigned long dwGID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "dbo.USPGuildDelete %d", dwGID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드 삭제 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::DeleteGuildMember(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwCID, unsigned char cRank)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	unsigned long dwResult = 0;

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "dbo.USPGuildMemberOut %d, %d, %d", 
		dwGID, dwCID, cRank);

	if (false == DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), (void *)&dwResult)) 
	{
		SERLOG1(g_Log, "길드 멤버 삭제 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	if (0 != dwResult)
	{
		// CharInfo에 업데이트를 실패한 경우 (존재하지 않는 CID를 보냈다던지...)
		return false;
	}

	return true;	
}

bool DBComponent::GuildDB::DeleteGuildRelation(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwTargetGID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "DELETE TblGuildRelation WHERE nGuildID=%d AND nTargetGuildID=%d", dwGID, dwTargetGID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG1(g_Log, "길드 관계 삭제 실패 : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::DeleteGuildRelation(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelation)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "DELETE TblGuildRelation WHERE nGuildID=%d AND nTargetGuildID=%d AND tnRelation=%d", dwGID, dwTargetGID, cRelation);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG1(g_Log, "길드 관계 삭제 실패 : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::DeleteGuildRelation(CDBComponent& DBComponent, unsigned long dwGID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "DELETE TblGuildRelation WHERE nGuildID=%d OR nTargetGuildID=%d", dwGID, dwGID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG1(g_Log, "길드 관계 삭제 실패 : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateMemberTitle(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cTitle, SYSTEMTIME UpdateTime)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	char strTime[MAX_PATH] = "";
	if (0 == UpdateTime.wYear)
	{
		// 년도가 0인 경우는 체크하지 않겠다는 의미이므로 DB에는 먼 미래로 기록해둔다.
		sprintf(strTime, "cast('2010-12-31 00:00:00' as smalldatetime)");
	}
	else
	{
		sprintf(strTime, "cast('%d-%d-%d %d:%d:%d' as smalldatetime)", UpdateTime.wYear, UpdateTime.wMonth, UpdateTime.wDay, 
			UpdateTime.wHour, UpdateTime.wMinute, UpdateTime.wSecond, UpdateTime.wMilliseconds);
	}

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGuildMember SET nPosition=%d, tLeaveGuildTime=%s WHERE nCID=%d", 
		cTitle, strTime, dwCID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드원 직위 변경 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateMemberRank(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwCID, unsigned char cBeforeRank, unsigned char cAfterRank)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "dbo.USPGuildChangePos %d, %d, %d, %d", 
		dwGID, dwCID, cBeforeRank, cAfterRank);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드원 직위 변경 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateGuildRelation(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwTargetGID, unsigned char cRelationType)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGuildRelation SET tnRelation=%d WHERE nGuildID=%d AND nTargetGuildID=%d",
				cRelationType, dwGID, dwTargetGID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		SERLOG1(g_Log, "길드 관계 업데이트 실패 : %s", DBComponent.GetErrorString());
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateGuildTime(CDBComponent& DBComponent, unsigned long dwGID, unsigned char cType, SYSTEMTIME UpdateTime)
{
	// 상수들은 Community/Guild/GuildConstants.h 파일을 참고
	// TODO : 현재 GameLibrary 프로젝트와 ServerLibrary 프로젝트의 종속성이 애매해서... 추후 정리가 필요.

	char strTime[MAX_PATH] = "";
	if (0 == UpdateTime.wYear)
	{
		// 년도가 0인 경우는 체크하지 않겠다는 의미이므로 DB에는 먼 미래로 기록해둔다.
		sprintf(strTime, "cast('2010-12-31 00:00:00' as smalldatetime)");
	}
	else
	{
		sprintf(strTime, "cast('%d-%d-%d %d:%d:%d' as smalldatetime)", UpdateTime.wYear, UpdateTime.wMonth, UpdateTime.wDay, 
			UpdateTime.wHour, UpdateTime.wMinute, UpdateTime.wSecond, UpdateTime.wMilliseconds);
	}

	char strColumn[MAX_PATH] = "";
	switch (cType)
	{
		case 0:
			sprintf(strColumn, "tLastLogout");
			break;			

		case 1:
			sprintf(strColumn, "tCheckMember");
			break;			

		case 2:
			sprintf(strColumn, "tGMLastLogout");
			break;			
	}

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGuildInfo SET %s=%s WHERE nGuildID=%d", 
		strColumn, strTime, dwGID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG2(g_Log, "길드 시간 변경 실패: %s - Query : %s", DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateGuildLevel(CDBComponent& DBComponent, unsigned long dwGID, unsigned char cLevel, unsigned long dwGold)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGuildInfo SET nGuildGold=%d, tnGuildLevel=%d WHERE nGuildID=%d", 
		dwGold, cLevel, dwGID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드 레벨 변경 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateGuildInclination(CDBComponent& DBComponent, unsigned long dwGID, unsigned char cInclination)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGuildInfo SET tnNationType=%d, tChangeState=GetDate() WHERE nGuildID=%d", 
		cInclination, dwGID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드 성향 변경 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateGuildRight(CDBComponent& DBComponent, unsigned long dwGID, char* szRight, unsigned short wSize)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
        "UPDATE TblGuildInfo SET bRight=0x");

	char* lpDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());
	char* lpPos = szRight;
	for (int nIndex = 0; nIndex < wSize; ++nIndex, ++lpPos, lpDest += 2)
	{
        Math::Convert::Hex08ToStr(lpDest, *lpPos);
	}

    const int MAX_FOOTER = 512;
	char szFooter[MAX_FOOTER];
	sprintf(szFooter, " WHERE nGuildID=%d", dwGID);

	strcat(DBComponent.GetQueryBuffer(), szFooter);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드 권한 변경 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateGuildMark(CDBComponent& DBComponent, unsigned long dwGID, char* szMark, unsigned short wSize, unsigned long dwGold)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGuildInfo SET nGuildGold=%d, bGuildMark=0x", dwGold);

	char* lpDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());
	char* lpPos = szMark;
	for (int nIndex = 0; nIndex < wSize; ++nIndex, ++lpPos, lpDest += 2)
	{
		Math::Convert::Hex08ToStr(lpDest, *lpPos);
	}

    const int MAX_FOOTER = 512;
	char szFooter[MAX_FOOTER];
	sprintf(szFooter, " WHERE nGuildID=%d", dwGID);

	strcat(DBComponent.GetQueryBuffer(), szFooter);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드 마크 변경 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateGuildMemberTactics(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwCID, unsigned char cTactics)
{

	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), 
		"Update TblGuildMember Set Tactics = %d where nGuildID = %d And nCID = %d",cTactics, dwGID, dwCID);
	
	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{	
		SERLOG1(g_Log, "용병 플레그 세팅 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateGuildGold(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwGold)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGuildInfo SET nGuildGold=%d WHERE nGuildID=%d", 
		dwGold, dwGID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드 금고 업데이트 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::UpdateGuildFame(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwFame)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "UPDATE TblGuildInfo SET nGuildFame=%d WHERE nGuildID=%d", 
		dwFame, dwGID);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드 명성 업데이트 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

bool DBComponent::GuildDB::ReleaseGold(CDBComponent& DBComponent, unsigned long dwGID, unsigned long dwGold)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), DBComponent.GetQueryBufferLen(), "dbo.USPGuildGiveMemberGold %d, %d", 
		dwGID, dwGold);

	if (false == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
	{
		SERLOG1(g_Log, "길드 금고 방출 실패 : %s", DBComponent.GetErrorString());	
		return false;
	}

	return true;
}

