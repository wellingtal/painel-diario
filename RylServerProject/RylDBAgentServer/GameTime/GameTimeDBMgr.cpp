#include "stdafx.h"
#include "GameTimeDBMgr.h"

#include <GameTime/GameTimeDBComponent.h>
#include <DataStorage/SessionDataMgr.h>

#include <Log/ServerLog.h>
#include <DB/DBComponent.h>
#include <Utility/TokenlizedFile.h>

#include <algorithm>
#include <atltime.h>

#include <Creature/Siege/SiegeObjectDBMgr.h>
#include <Castle/CastleDB.h>
#include <Castle/CastleDBMgr.h>

#include <Creature/StatueDBMgr.h>

#include <Community/Guild/GuildDBMgr.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/GameDispatch.h>

#include <Utility/Setup/ServerSetup.h>

#include <DataStorage/CharacterData.h>

void RealmPoint::AddRealmPoint(DBAgent::DataStorage::CSessionData* sessionData, unsigned char cRealmType)
{
	if(sessionData)
	{	
		DBAgent::DataStorage::CCharacterData* lpCharacterData = sessionData->GetCharacterData();

		unsigned long dwTime	= CGameTimeDBMgr::GetInstance().GetRealmDuration()/2;
		unsigned char cType		= PktRealmPoint::REALM_OUT;

		if(lpCharacterData)
		{				
			// �ð� üũ //
			// ������ �α׾ƿ� �ð����� �������ִ� �ð��� ����ߴ�.
			// ������ ���� �ٲ�� ���� ������ �α����� �ð����� �󸶳� ����������
			// ��Ÿ���� ����Ѵ�.
			// edith 2008.12.4 �α��� Ÿ������ ó��
			TIME* pTime = lpCharacterData->GetLoginTime();
//			TIME* pTime = lpCharacterData->GetLogoutTime();

			if(!pTime)
			{
				return;
			}

			SERVER_ID serverID;
			serverID.dwID = sessionData->GetServerID();

			PktRealmPoint pktRealmPoint;		
			pktRealmPoint.m_dwCID		= lpCharacterData->GetCID();					

			unsigned char cRealmPoint	= lpCharacterData->GetRealmPoint();
			unsigned char cRealmMinute	= lpCharacterData->GetRealmMinute();

			CTime timeCurrent = CTime::GetCurrentTime();
/*
			if(!lpCharacterData->GetRealmCheckPoint())
			{
				// ĳ���Ͱ� ��ũ ī������ ���� ��츸 üũ.
				if(SERVER_ID::STONE_WAR1 <= serverID.GetZone() && serverID.GetZone() <= SERVER_ID::STONE_WAR3)
				{			
					if(lpCharacterData->GetRealmWarFlag()==Creature::WAR_INSTANCE || lpCharacterData->GetRealmWarFlag()==Creature::WAR_ON)
					{
						CTime timeLogin(pTime->Year, pTime->Month, pTime->Day, pTime->Hour, pTime->Minute, pTime->Second, pTime->MSecond);

						CTimeSpan ts = timeCurrent - timeLogin;

						// 2008.09.23 �ð��� �Ѿ�� ��������Ʈ�� �ִ°� �����Ѵ�.
						// ������ ���������� 1�� ��.
//						if(ts.GetTotalMinutes()+cRealmMinute>=dwTime)
//						{			
							if(cRealmPoint<DBAgent::DataStorage::CSessionData::MAX_REALM_POINT)
							{
								cRealmPoint++;

								lpCharacterData->SetRealmPoint(cRealmPoint);
								lpCharacterData->SetRealmCheckPoint(0);
								lpCharacterData->SetRealmMinute(0);

								if(cRealmType)
								{
									return;
								}

								pktRealmPoint.m_cRealmPoint	= cRealmPoint;
								pktRealmPoint.m_cType		= PktRealmPoint::REALM_JOIN;
								goto lb_send;
							}

							cType	= PktRealmPoint::REALM_JOIN;
//						}						
					}
				}
			}
			else
			{
				lpCharacterData->SetRealmCheckPoint(0);
				lpCharacterData->SetRealmMinute(0);

				if(cRealmPoint<DBAgent::DataStorage::CSessionData::MAX_REALM_POINT)
				{
					cRealmPoint++;

					lpCharacterData->SetRealmPoint(cRealmPoint);

					pktRealmPoint.m_cRealmPoint	= cRealmPoint;
					pktRealmPoint.m_cType		= PktRealmPoint::REALM_JOIN;

					goto lb_send;
				}
			}
*/
			// ĳ���Ͱ� ��ũ ī������ ���� ��츸 üũ.
			if(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= serverID.GetZone() && serverID.GetZone() <= SERVER_ID::STONE_WAR3)
			{			
				// ���������� ������ ƨ������ GetRealmCheckPoint() �� ���� ���õȴ�.
				// ���� ���õ��� ������ ������ 1����Ʈ�� +���ش�.
				if(!lpCharacterData->GetRealmCheckPoint())
				{
					// edith 2009.06.13 �� ���� ���ʽ� �� �� �ο� ���ʽ� ����
/*
					CTime timeLogin(pTime->Year, pTime->Month, pTime->Day, pTime->Hour, pTime->Minute, pTime->Second, pTime->MSecond);

					CTimeSpan ts = timeCurrent - timeLogin;

					// 2008.11.28 �ð��� �Ѿ�� ��������Ʈ�� �ִ°� �����Ѵ�.
					// ������ ���������� 1�� ��.
					int iTotalMinute = (int)ts.GetTotalMinutes();
					if(iTotalMinute+cRealmMinute>=dwTime)
					{			
						if(cRealmPoint<DBAgent::DataStorage::CSessionData::MAX_REALM_POINT)
						{
							cRealmPoint++;

							lpCharacterData->SetRealmPoint(cRealmPoint);
							lpCharacterData->SetRealmCheckPoint(0);
							lpCharacterData->SetRealmMinute(0);
						}
					}
*/
				}
				else
				{
					if(cRealmPoint<DBAgent::DataStorage::CSessionData::MAX_REALM_POINT)
					{
						cRealmPoint++;

						lpCharacterData->SetRealmPoint(cRealmPoint);
					}
					lpCharacterData->SetRealmCheckPoint(0);
					lpCharacterData->SetRealmMinute(0);
				}

				// �α׾ƿ��� ȣ��ɶ�
				if(cRealmType)
					return;

				pktRealmPoint.m_cRealmPoint	= cRealmPoint;
				pktRealmPoint.m_cType		= PktRealmPoint::REALM_JOIN;
			}
			else
			{
				// �������￡ ��������������. Ȥ�� 1�ð� �̻� �������� ������.
				if(cRealmPoint==0 || cRealmType)
				{
					return;
				}

				if(cType!=PktRealmPoint::REALM_JOIN)
				{
					cRealmPoint--;			
				}

				lpCharacterData->SetRealmPoint(cRealmPoint);
				lpCharacterData->SetRealmCheckPoint(0);
				lpCharacterData->SetRealmMinute(0);

				pktRealmPoint.m_cRealmPoint	= cRealmPoint;
				pktRealmPoint.m_cType		= cType;
			}

//lb_send:
			// ���� ������ �˷��ش�.
			if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktRealmPoint), sizeof(PktRealmPoint), CmdRealmPoint, 0, 0))
			{
				DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
					reinterpret_cast<char*>(&pktRealmPoint), sizeof(PktRealmPoint), CmdRealmPoint));
			}

			// ���������� ��������Ʈ�� ������� �α׾ƿ� Ÿ���� �������ָ�ȴ�.
			// ���������� ���������� 24�ð����� ����Ұ��� ����ؼ�
			pTime->Year		= timeCurrent.GetYear();
			pTime->Month	= timeCurrent.GetMonth();
			pTime->Day		= timeCurrent.GetDay();
			pTime->Hour		= timeCurrent.GetHour();
			pTime->Minute	= timeCurrent.GetMinute();
			pTime->Second	= timeCurrent.GetSecond();
		}
	}
}

void RealmPoint::InitRealmTime(DBAgent::DataStorage::CSessionData* sessionData, TIME* pTime)
{
	if(sessionData)
	{	
		DBAgent::DataStorage::CCharacterData* lpCharacterData = sessionData->GetCharacterData();		

		if(lpCharacterData)
		{
			TIME* pLogoutTime = lpCharacterData->GetLogoutTime();
			TIME* pLoginTime = lpCharacterData->GetLoginTime();

			if(pLogoutTime)
			{
				pLogoutTime->Year	= pTime->Year;
				pLogoutTime->Month	= pTime->Month;
				pLogoutTime->Day	= pTime->Day;
				pLogoutTime->Hour	= pTime->Hour;
				pLogoutTime->Minute	= pTime->Minute;
				pLogoutTime->Second	= pTime->Second;
			}
			// edith 2008.12.4 �α��� Ÿ�Ӱ��ű�� �߰�
			if(pLoginTime)
			{
				pLoginTime->Year	= pTime->Year;
				pLoginTime->Month	= pTime->Month;
				pLoginTime->Day		= pTime->Day;
				pLoginTime->Hour	= pTime->Hour;
				pLoginTime->Minute	= pTime->Minute;
				pLoginTime->Second	= pTime->Second;
			}
		}
	}
}

unsigned char FindPrimaryValue(unsigned char cTimeType, std::vector<unsigned char>& vecTimeResult)
{
	unsigned char cResult = 0;

	switch (cTimeType)
	{
		case GameTime::GUILD:
		{
			std::vector<unsigned char>::iterator itr ;
			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::GT_GUILD_WAR);
			if (itr != vecTimeResult.end()) return GameTime::GT_GUILD_WAR;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::GT_PREPARE_END_30);
			if (itr != vecTimeResult.end()) return GameTime::GT_PREPARE_END_30;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::GT_PREPARE_START_5);
			if (itr != vecTimeResult.end()) return GameTime::GT_PREPARE_START_5;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::GT_PREPARE_START_30);
			if (itr != vecTimeResult.end()) return GameTime::GT_PREPARE_START_30;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::GT_NORMAL);
			if (itr != vecTimeResult.end()) return GameTime::GT_NORMAL;
		}
		break;

		case GameTime::REALM:
		{
			std::vector<unsigned char>::iterator itr ;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::RT_REALM_WAR);
			if (itr != vecTimeResult.end()) return GameTime::RT_REALM_WAR;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::RT_REALM_WAR_END_5);
			if (itr != vecTimeResult.end()) return GameTime::RT_REALM_WAR_END_5;			

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::RT_PREPARE_START_5);
			if (itr != vecTimeResult.end()) return GameTime::RT_PREPARE_START_5;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::RT_PREPARE_START_10);
			if (itr != vecTimeResult.end()) return GameTime::RT_PREPARE_START_10;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::RT_PREPARE_START_15);
			if (itr != vecTimeResult.end()) return GameTime::RT_PREPARE_START_15;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::RT_PREPARE_START_30);
			if (itr != vecTimeResult.end()) return GameTime::RT_PREPARE_START_30;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::RT_NORMAL);
			if (itr != vecTimeResult.end()) return GameTime::RT_NORMAL;
		}
		break;

		case GameTime::SIEGE:
		{
			std::vector<unsigned char>::iterator itr ;
			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::ST_SIEGE_WAR);
			if (itr != vecTimeResult.end()) return GameTime::ST_SIEGE_WAR;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::ST_PREPARE_END_5);
			if (itr != vecTimeResult.end()) return GameTime::ST_PREPARE_END_5;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::ST_PREPARE_START_5);
			if (itr != vecTimeResult.end()) return GameTime::ST_PREPARE_START_5;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::ST_NORMAL);
			if (itr != vecTimeResult.end()) return GameTime::ST_NORMAL;
		}
		break;

		case GameTime::NEWZONE:
		{
			std::vector<unsigned char>::iterator itr ;
			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::ET_ALLOWENTER);
			if (itr != vecTimeResult.end()) return GameTime::ET_ALLOWENTER;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::ET_PREPARE_END_5);
			if (itr != vecTimeResult.end()) return GameTime::ET_PREPARE_END_5;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::ET_PREPARE_START_5);
			if (itr != vecTimeResult.end()) return GameTime::ET_PREPARE_START_5;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::ET_LIMIT);
			if (itr != vecTimeResult.end()) return GameTime::ET_LIMIT;
		}
		break;

		case GameTime::MINE:
		{
			std::vector<unsigned char>::iterator itr ;
			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::MT_MINING);
			if (itr != vecTimeResult.end()) return GameTime::MT_MINING;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::MT_NORMAL);
			if (itr != vecTimeResult.end()) return GameTime::MT_NORMAL;
		}
		break;

		// edith 2009.07.21 ����Ÿ�� �۾���
		case GameTime::EVENT:
		{
			std::vector<unsigned char>::iterator itr ;
			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::EVENT_All20);
			if (itr != vecTimeResult.end()) return GameTime::EVENT_All20;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::EVENT_All50);
			if (itr != vecTimeResult.end()) return GameTime::EVENT_All50;

			itr = find(vecTimeResult.begin(), vecTimeResult.end(), GameTime::EVENT_NORMAL);
			if (itr != vecTimeResult.end()) return GameTime::EVENT_NORMAL;
		}
		break;
	}

	return cResult;
}

CGameTimeDBMgr::CGameTimeDBMgr()
: m_cNowGuildTime(0), m_cNowRealmTime(0), m_cNowSiegeTime(0), m_cNowMiningTime(0), m_dwRealmTime(2), m_cNowEventTime(0),
  m_lpDBComponent(NULL)
{
}

CGameTimeDBMgr::~CGameTimeDBMgr()
{
}

CGameTimeDBMgr& CGameTimeDBMgr::GetInstance()
{
	static CGameTimeDBMgr ms_this;
	return ms_this;
}

void CGameTimeDBMgr::CheckRealmCount(CTime& startTime, unsigned long* dwRealmCount_Out)
{
	if(startTime.GetTime()==0)
	{
		*dwRealmCount_Out = 0;
		return;
	}

	CTimeSpan perWeek(7, 0, 0, 0);
	CTimeSpan perDay(1, 0, 0, 0);	

	CTime nowTime	= CTime::GetCurrentTime();
	CTime siegeTime	= CTime::GetCurrentTime();
	
	GameTimeTable::iterator pos = m_GameTimeTable.begin();
	GameTimeTable::iterator end = m_GameTimeTable.end();

	unsigned long dwRealmCount		= 0;	

	for(; pos!=end; ++pos)
	{
		sGameTime* lpGameTime = &pos->second;

		if(lpGameTime)
		{
			if(lpGameTime->m_eTimeType==GameTime::REALM)
			{
				CTime gameTime = GetTimeByCycle(lpGameTime, nowTime);

				if (0 == gameTime.GetTime())
				{
					continue;
				}

				switch(lpGameTime->m_eTimeCycle)
				{
				case GameTime::WEEKLY:
					{						
						Week eWeek = static_cast<Week>(startTime.GetDayOfWeek());
						
						char cWeek = eWeek - lpGameTime->m_eWeek;
						
						unsigned short wYear = startTime.GetYear();						

						if (cWeek < 0)
						{					 					 
							unsigned char cMonth = startTime.GetMonth() + 1;

							if (startTime.GetMonth() == 12)
							{
								wYear	= startTime.GetYear() + 1;
								cMonth	= 1;
							}

							CTime t1(startTime.GetYear(), startTime.GetMonth(), 1, 0, 0, 0);
							CTime t2(wYear, cMonth, 1, 0, 0, 0);

							CTimeSpan ts = t2 - t1;

							LONG lDays = static_cast<LONG>(ts.GetDays());
							unsigned char cDay = startTime.GetDay() - cWeek;

							wYear	= startTime.GetYear();
							cMonth	= static_cast<unsigned char>(startTime.GetMonth());				

							// ���� �Ѿ�� ���
							if (startTime.GetDay() - cWeek > lDays)
							{
								cDay = static_cast<unsigned char>(startTime.GetDay() - cWeek - lDays);

								if (startTime.GetMonth() == 12)
								{
									wYear	= startTime.GetYear() + 1;
									cMonth	= 1;
								}
								else
								{								
									cMonth	= startTime.GetMonth() + 1;
								}							
							}					

							siegeTime = CTime(wYear, cMonth, cDay, gameTime.GetHour(), gameTime.GetMinute(), 0);
						}					
						else if (cWeek > 0)
						{
							unsigned short	wYear	= startTime.GetYear();
							unsigned char	cMonth	= startTime.GetMonth() - 1;

							if (startTime.GetMonth() == 1)
							{
								wYear	= startTime.GetYear() - 1;
								cMonth	= 12;
							}

							CTime t1(startTime.GetYear(), startTime.GetMonth(), 1, 0, 0, 0);
							CTime t2(wYear, cMonth, 1, 0, 0, 0);

							CTimeSpan ts = t1 - t2;

							LONG lDays = static_cast<LONG>(ts.GetDays());

							// ���� �޷� �Ѿ�� ���
							unsigned char cDay = startTime.GetDay() - cWeek;

							wYear	= startTime.GetYear();
							cMonth	= startTime.GetMonth();

							if (startTime.GetDay() - cWeek < 1)
							{
								cDay = static_cast<unsigned char>(lDays - cWeek + 1);

								if (nowTime.GetMonth() == 1)
								{
									wYear	= nowTime.GetYear() - 1;
									cMonth	= 12;
								}
								else
								{
									cMonth	= static_cast<unsigned char>(startTime.GetMonth() - 1);
								}							
							}

							siegeTime = CTime(wYear, cMonth, cDay, gameTime.GetHour(), gameTime.GetMinute(), 0);

							CTime siegeTimeNext = siegeTime + perWeek;
							siegeTime = siegeTimeNext;
						}					
						else
						{
							siegeTime = CTime(startTime.GetYear(), startTime.GetMonth(), startTime.GetDay(), gameTime.GetHour(), gameTime.GetMinute(), 0);

							if(siegeTime.GetHour()<startTime.GetHour())
							{						
								CTime siegeTimeNext = siegeTime +  perWeek;
								siegeTime = siegeTimeNext;
							}
							else if(siegeTime.GetHour()==startTime.GetHour())
							{
								if(siegeTime.GetMinute()<startTime.GetMinute())
								{						
									CTime siegeTimeNext = siegeTime +  perWeek;
									siegeTime = siegeTimeNext;
								}
							}
						}							

						CTimeSpan perTime = nowTime - siegeTime;

						int nMinutes = static_cast<int>(perTime.GetTotalMinutes());

						while(nMinutes>static_cast<int>(lpGameTime->m_dwDuration))
						{
							dwRealmCount++;

							CTime siegeTimeNext = siegeTime + perWeek;
							siegeTime = siegeTimeNext;

							perTime = nowTime - siegeTime;

							nMinutes = static_cast<int>(perTime.GetTotalMinutes());
						}
					}
					break;
				case GameTime::DAILY:
					{	
						siegeTime = CTime(startTime.GetYear(), startTime.GetMonth(), startTime.GetDay(), gameTime.GetHour(), gameTime.GetMinute(), 0);

						if(siegeTime.GetHour()<startTime.GetHour())
						{						
							CTime siegeTimeNext = siegeTime + perDay;
							siegeTime = siegeTimeNext;
						}
						else if(siegeTime.GetHour()==startTime.GetHour())
						{
							if(siegeTime.GetMinute()<startTime.GetMinute())
							{						
								CTime siegeTimeNext = siegeTime + perDay;
								siegeTime = siegeTimeNext;
							}
						}
						
						CTimeSpan perTime = nowTime - siegeTime;

						int nMinutes = static_cast<int>(perTime.GetTotalMinutes());

						while(nMinutes>static_cast<int>(lpGameTime->m_dwDuration))
						{							
							dwRealmCount++;
							
							CTime siegeTimeNext = siegeTime + perDay;
							siegeTime = siegeTimeNext;

							perTime = nowTime - siegeTime;

							nMinutes = static_cast<int>(perTime.GetTotalMinutes());
						}
					}
					break;
				case GameTime::MONTHLY:
					{
						siegeTime = CTime(startTime.GetYear(), startTime.GetMonth(), gameTime.GetDay(), gameTime.GetHour(), gameTime.GetMinute(), 0);

						if(siegeTime.GetDay()<startTime.GetDay())
						{
							GetNextMonth(&siegeTime);
						}
						else if(siegeTime.GetDay()==startTime.GetDay())
						{
							if(siegeTime.GetHour()<startTime.GetHour())
							{						
								GetNextMonth(&siegeTime);
							}
							else if(siegeTime.GetHour()==startTime.GetHour())
							{
								if(siegeTime.GetMinute()<startTime.GetMinute())
								{						
									GetNextMonth(&siegeTime);
								}
							}
						}

						CTimeSpan perTime = nowTime - siegeTime;

						int nMinutes = static_cast<int>(perTime.GetTotalMinutes());

						while(nMinutes>static_cast<int>(lpGameTime->m_dwDuration))
						{							
							dwRealmCount++;

							GetNextMonth(&siegeTime);

							perTime = nowTime - siegeTime;

							nMinutes = static_cast<int>(perTime.GetTotalMinutes());
						}
					}
					break;
				case GameTime::DATE:
					{
						siegeTime = gameTime;

						CTimeSpan perTime = siegeTime - startTime;

						int nMinutes = static_cast<int>(perTime.GetTotalMinutes());

						if(nMinutes>0)
						{
							CTimeSpan timeDuration(0, 0, lpGameTime->m_dwDuration, 0);	

							CTime siegeCheckTime = siegeTime + timeDuration;
							
							perTime = siegeCheckTime - nowTime;

							nMinutes = static_cast<int>(perTime.GetTotalMinutes());

							if(nMinutes<0)
							{
								dwRealmCount++;
							}							
						}
					}
					break;
				}				
			}	
		}
	}

	*dwRealmCount_Out = dwRealmCount;
}

void CGameTimeDBMgr::GetNextMonth(CTime* pTime)
{
	if(!pTime)
		return;

	unsigned short	wYear	= pTime->GetYear();
	unsigned char	cMonth	= pTime->GetMonth();

	if(cMonth==12)
	{
		wYear	+=1;
		cMonth	= 1;
	}
	else
	{
		cMonth++;
	}

	CTime time(wYear, cMonth, pTime->GetDay(), pTime->GetHour(), pTime->GetMinute(), 0);	
	*pTime = time;
}

void	CGameTimeDBMgr::Process()
{
	//===========================================================================================//
	// Step 1
	//	: ���� �ð� ���¿� ���� ���ŵ� �ð��� üũ�ؼ� ���� �ð� ���¸� ���´�.
	//  ---------------------------------------------------------------------------------------  //

	unsigned char cOldGuildTime = GetCurrentGuildTime();
	unsigned char cOldRealmTime = GetCurrentRealmTime();
	unsigned char cOldSiegeTime = GetCurrentSiegeTime();
	unsigned char cOldEnterTime = GetCurrentEnterTime();
	unsigned char cOldMiningTime = GetCurrentMiningTime();

	// edith 2009.07.21 ����Ÿ�� �۾���
	unsigned char cOldEventTime = GetCurrentEventTime();

	// �ð� ����
	CheckGameTime();

	unsigned char cNewGuildTime = GetCurrentGuildTime();
	unsigned char cNewRealmTime = GetCurrentRealmTime();
	unsigned char cNewSiegeTime = GetCurrentSiegeTime();
	unsigned char cNewEnterTime = GetCurrentEnterTime();
	unsigned char cNewMiningTime = GetCurrentMiningTime();

	// edith 2009.07.21 ����Ÿ�� �۾���
	unsigned char cNewEventTime = GetCurrentEventTime();
	//===========================================================================================//

	//===========================================================================================//
	// Step 2
	//	: �������� ä������� �ӽ� �����ҿ� ä���� ������ �����Ѵ�.
	//  ---------------------------------------------------------------------------------------  //

	if (IsMiningTime(cOldMiningTime) && !IsMiningTime())
	{
		// �Ӽ� �����ҷ� ä���� ���� ����
		CSiegeObjectDBMgr::GetInstance().SendProcessMining(PktProcessMining::TEMPORARY_PROCESS);
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 3
	//	: �̺�Ʈ �ð��϶� ó��
	//  ---------------------------------------------------------------------------------------  //

	if (IsEventTime(cOldEventTime) && !IsEventTime())
	{
		;// ����� �ؾ��ұ�?
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 3
	//	: ������� ���۵� ����� ó��
	//  ---------------------------------------------------------------------------------------  //

	if (!IsGuildWarTime(cOldRealmTime) && IsGuildWarTime())
	{
		// ��� ���� ����ڸ� ���� Ż�� ��Ų��. (�뺴�� ����)
		Guild::CGuildDBMgr::GetInstance().KickAllWaitMember() ;
	}

	//===========================================================================================//


	//===========================================================================================//
	// Step 4
	//	: ������� ������ ����� ó��
	//  ---------------------------------------------------------------------------------------  //

	if (IsGuildWarTime(cOldGuildTime) && !IsGuildWarTime())
	{
		// ä������ ��� �ӽ� ������ ���� ������ �Ű��ش�.
		CSiegeObjectDBMgr::GetInstance().SendProcessMining(PktProcessMining::ACCUMULATED_PROCESS);

		// ���� ���� �ı��� ���� �ð� ���� Ƚ��(����� ���Ƿ� ����� �ð� ���� Ƚ��) ����
		CSiegeObjectDBMgr::GetInstance().DecreaseWeaponRemainSiegeTime();

		// ���� �ð� ���� Ƚ��(����� ���Ƿ� ����� �ð� ���� Ƚ��) ������Ʈ
		CSiegeObjectDBMgr::GetInstance().UpdateSiegeCount();

		// edith 2008.10.28 ���� ������ ��ġ ����.
		// �����ð��� ������ ������ ��������� ��ġ ����.
		CSiegeObjectDBMgr::GetInstance().UpdateRewardFame();

		// ��� ����� ��� ���� / ī���� ���� ����
		Guild::CGuildDBMgr::GetInstance().CancelAlertAndCounterHostility();
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 5
	//	: �������� ���۵� ��� ó��
	//  ---------------------------------------------------------------------------------------  //

	if (!IsSiegeWarTime(cOldSiegeTime) && IsSiegeWarTime())
	{
		// ������ ������ �������� ���� üũ�Ѵ�.
		Castle::CCastleDBMgr::GetInstance().CheckEnableSiegeForAllCastle(true);

		// ������ ������ ������ ������ ���´�.
		Castle::CCastleDBMgr::GetInstance().CloseCastleGate();
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 6
	//	: �������� ���� ��� ó��
	//  ---------------------------------------------------------------------------------------  //

	if (IsSiegeWarTime(cOldSiegeTime) && !IsSiegeWarTime())
	{
		// ���� �ð��� ������ ������� ��ŸƮŶ���� �����ش�.
		CSiegeObjectDBMgr::GetInstance().DestoryAllSiegeArms();

		// ���� �ð��� ������ ���� ������� ���� ���� NPC�� �������´�.
		Castle::CCastleDBMgr::GetInstance().DestroyAllCastleArms();

		// ���� ���� Ƚ�� ������Ʈ
		Castle::CCastleDBMgr::GetInstance().UpdateSiegeCount();

		// ������ ������ �������� ���� üũ�Ѵ�.
		Castle::CCastleDBMgr::GetInstance().CheckEnableSiegeForAllCastle(false);

		// ������ ���� ������ ������ ���´�.
		Castle::CCastleDBMgr::GetInstance().CloseCastleGate();
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 7
	//	: �������� ���۵� ��� ó��
	//  ---------------------------------------------------------------------------------------  //

	if (!IsRealmWarTime(cOldRealmTime) && IsRealmWarTime())
	{
	//	if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//hayzohar statue war fix
	//	{
			// �ð� �ʱ�ȭ.
			CTime timeLogin = CTime::GetCurrentTime();

			TIME LoggoutTime;

			LoggoutTime.Year	= timeLogin.GetYear();
			LoggoutTime.Month	= timeLogin.GetMonth();
			LoggoutTime.Day		= timeLogin.GetDay();			
			LoggoutTime.Hour	= timeLogin.GetHour();
			LoggoutTime.Minute	= timeLogin.GetMinute();
			LoggoutTime.Second	= timeLogin.GetSecond();

			DBAgent::DataStorage::CSessionDataMgr::GetInstance().ProcessRealmTime(RealmPoint::InitRealmTime, &LoggoutTime);		
		//}
	}
	//===========================================================================================//


/*
	//===========================================================================================//
	// Step 8
	//	: �������� ���� ��� ó��
	//  ---------------------------------------------------------------------------------------  //

	// edith 2010.01.10 �ð����� �������Ʈ �ִ� ������ ���ŉ��. 
	// ���� ������ ���� ���� ���� �ְ� ������
	if (IsRealmWarTime(cOldRealmTime) && !IsRealmWarTime())
	{
		// ������ : ��ũ ī���� ���� ����
		if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))
		{
			// 1�ð� �̻� �÷��� �� ĳ���Ϳ��� ���� �������� ����Ʈ�� �ش�.
			DBAgent::DataStorage::CSessionDataMgr::GetInstance().ProcessRealmPoint(RealmPoint::AddRealmPoint, 0);
		}
	}
	//===========================================================================================//
*/

	//===========================================================================================//
	// Step 9
	//	: ���� �ð� ���� ����
	//  ---------------------------------------------------------------------------------------  //

	bool bGuildUpdated = false;
	bool bRealmUpdated = false;
	bool bSiegeUpdated = false;
	bool bEnterUpdated = false;

	// edith 2009.07.21 ����Ÿ�� �۾���
	bool bEventUpdated = false;

	if (cOldGuildTime != cNewGuildTime)	bGuildUpdated = true;
	if (cOldRealmTime != cNewRealmTime)	bRealmUpdated = true;
	if (cOldSiegeTime != cNewSiegeTime)	bSiegeUpdated = true;
	if (cOldEnterTime != cNewEnterTime)	bEnterUpdated = true;

	// edith 2009.07.21 ����Ÿ�� �۾���
	if (cOldEventTime != cNewEventTime)	bEventUpdated = true;

	if (bGuildUpdated || bRealmUpdated || bSiegeUpdated || bEnterUpdated || bEventUpdated)
	{
		SendGameTimeInfo(GameTime::TYPE_CHANGE_GAME_TIME, bGuildUpdated, bRealmUpdated, bSiegeUpdated, bEnterUpdated, bEventUpdated);
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 10
	//	: �����, ������ ����� Instance Flag �� Off �� �ٲ��ش�.
	//  ---------------------------------------------------------------------------------------  //

	if (IsGuildWarTime(cOldGuildTime) && !IsGuildWarTime())
	{
		DBAgent::DataStorage::CSessionDataMgr::GetInstance().ClearGuildWarInstanceFlag();
	}

	if (IsRealmWarTime(cOldRealmTime) && !IsRealmWarTime())
	{
		DBAgent::DataStorage::CSessionDataMgr::GetInstance().ClearRealmWarInstanceFlag();
	}

	//===========================================================================================//
}

void	CGameTimeDBMgr::SendGameTimeInfo(unsigned char cType, bool bUpdateGuildTime, bool bUpdateRealmTime, bool bUpdateSiegeTime, bool bUpdateEnterTime, bool bUpdateEventTime)
{
	const unsigned short MAX_GAME_TIME_SEND_BUFFER = sizeof(PktGameTimeInfo) + sizeof(CastleSiegeInfo) * Castle::MAX_CASTLE_NUM;
	char szBuffer[MAX_GAME_TIME_SEND_BUFFER];

	unsigned char cNum = 0;
	unsigned short wSize = sizeof(PktGameTimeInfo);
	PktGameTimeInfo* lpPktGTI = reinterpret_cast<PktGameTimeInfo* >(szBuffer);
	CastleSiegeInfo* lpCastleSiegeInfo = reinterpret_cast<CastleSiegeInfo* >(lpPktGTI + 1);
	ZeroMemory(lpPktGTI, sizeof(PktGameTimeInfo));

    Castle::CCastleDBMgr::GetInstance().GetCastleSiegeInfo(lpCastleSiegeInfo, cNum, wSize);

	lpPktGTI->m_cType = cType;
	lpPktGTI->m_wSize = wSize;
	lpPktGTI->m_cCastleNum = cNum;
	lpPktGTI->m_GuildTimeInfo.m_bUpdated = bUpdateGuildTime;
	lpPktGTI->m_RealmTimeInfo.m_bUpdated = bUpdateRealmTime;
	lpPktGTI->m_SiegeTimeInfo.m_bUpdated = bUpdateSiegeTime;
	lpPktGTI->m_EnterTimeInfo.m_bUpdated = bUpdateEnterTime;
	lpPktGTI->m_EventTimeInfo.m_bUpdated = bUpdateEventTime;

	if (bUpdateGuildTime)
	{
		lpPktGTI->m_GuildTimeInfo.m_cNowGuildTime = m_cNowGuildTime;
		GetTimeInfo(GameTime::GUILD, lpPktGTI->m_GuildTimeInfo.m_TimeInfo);
	}

	if (bUpdateRealmTime)
	{
		lpPktGTI->m_RealmTimeInfo.m_cNowRealmTime = m_cNowRealmTime;
		GetTimeInfo(GameTime::REALM, lpPktGTI->m_RealmTimeInfo.m_TimeInfo);
	}

	if (bUpdateSiegeTime)
	{
		lpPktGTI->m_SiegeTimeInfo.m_cNowSiegeTime = m_cNowSiegeTime;
		GetTimeInfo(GameTime::SIEGE, lpPktGTI->m_SiegeTimeInfo.m_TimeInfo);
	}

	if (bUpdateEnterTime)
	{
		lpPktGTI->m_EnterTimeInfo.m_cNowEnterTime = m_cNowEnterTime;
		GetTimeInfo(GameTime::NEWZONE, lpPktGTI->m_EnterTimeInfo.m_TimeInfo);
	}

	if (bUpdateEventTime)
	{
		lpPktGTI->m_EventTimeInfo.m_cNowEventTime = m_cNowEventTime;
		GetTimeInfo(GameTime::EVENT, lpPktGTI->m_EventTimeInfo.m_TimeInfo);
	}

	if (PacketWrap::WrapHeader(reinterpret_cast<char*>(szBuffer), wSize, CmdGameTimeInfo, 0, 0))
	{
		DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
			reinterpret_cast<char*>(&szBuffer), wSize, CmdGameTimeInfo));
	}
}

void	CGameTimeDBMgr::SendGameTimeInfo(unsigned long dwCastleID)
{
	Castle::CCastleDB* lpCastle = Castle::CCastleDBMgr::GetInstance().GetCastle(dwCastleID);
	if (NULL != lpCastle)
	{
		SERVER_ID serverID;
		serverID.sID.Type       = CServerSetup::GameServer;
		serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
		serverID.sID.Channel    = lpCastle->GetChannel();
		serverID.sID.ID         = lpCastle->GetZone();

		GET_MULTI_DISPATCH(lpGameDispatch, serverID.dwID, 
			DBAgent::CGameDispatch, DBAgent::CGameDispatch::GetDispatchTable());

		if (NULL != lpGameDispatch)
		{
			const unsigned short BUFFER_SIZE = sizeof(PktGameTimeInfo) + sizeof(CastleSiegeInfo);
			CSendStream& SendStream = lpGameDispatch->GetSendStream();

			char* lpBuffer = SendStream.GetBuffer(BUFFER_SIZE);
			if (NULL != lpBuffer)
			{
				PktGameTimeInfo* lpPktGTI = reinterpret_cast<PktGameTimeInfo* >(lpBuffer);
				CastleSiegeInfo* lpCastleSiegeInfo = reinterpret_cast<CastleSiegeInfo* >(lpPktGTI + 1);
				ZeroMemory(lpPktGTI, sizeof(PktGameTimeInfo));

				lpCastleSiegeInfo->m_dwCastleID = dwCastleID;
				lpCastleSiegeInfo->m_bEnableSiege = lpCastle->GetEnableSiege();

				lpPktGTI->m_cType = GameTime::TYPE_ENABLE_CASTLE_SIEGE;
				lpPktGTI->m_wSize = BUFFER_SIZE;
				lpPktGTI->m_cCastleNum = 1;

				lpPktGTI->m_GuildTimeInfo.m_bUpdated = false;
				lpPktGTI->m_GuildTimeInfo.m_cNowGuildTime = m_cNowGuildTime;
				lpPktGTI->m_RealmTimeInfo.m_bUpdated = false;
				lpPktGTI->m_RealmTimeInfo.m_cNowRealmTime = m_cNowRealmTime;
				lpPktGTI->m_SiegeTimeInfo.m_bUpdated = false;
				lpPktGTI->m_SiegeTimeInfo.m_cNowSiegeTime = m_cNowSiegeTime;
				lpPktGTI->m_EnterTimeInfo.m_bUpdated = false;
				lpPktGTI->m_EnterTimeInfo.m_cNowEnterTime = m_cNowEnterTime;
				lpPktGTI->m_EventTimeInfo.m_bUpdated = false;
				lpPktGTI->m_EventTimeInfo.m_cNowEventTime = m_cNowEventTime;

				SendStream.WrapHeader(BUFFER_SIZE, CmdGameTimeInfo, 0, 0);
			}
		}
	}
}

void	CGameTimeDBMgr::CheckGameTime()
{
	CTime nowTime = CTime::GetCurrentTime();

	typedef std::vector<unsigned char>	TimeResultVector;
	TimeResultVector	vecGuildTimeResult;
	TimeResultVector	vecRealmTimeResult;
	TimeResultVector	vecSiegeTimeResult;
	TimeResultVector	vecEnterTimeResult;
	TimeResultVector	vecMiningTimeResult;
	TimeResultVector	vecEventTimeResult;

	GameTimeTable::iterator pos = m_GameTimeTable.begin();
	GameTimeTable::iterator end = m_GameTimeTable.end();

	for (; pos!=end; ++pos)
	{
		sGameTime* lpGameTime = &pos->second;

		if (NULL != lpGameTime)
		{
			CTime gameTime = GetTimeByCycle(lpGameTime, nowTime);
			if (0 == gameTime.GetTime())
			{
				continue;
			}

			CTimeSpan perTime(nowTime - gameTime);
			int nMinutes = static_cast<int>(perTime.GetTotalMinutes());
			if (0 != (perTime.GetTimeSpan() % 60) && perTime.GetTimeSpan() < 0)
			{
				--nMinutes;
			}

			switch (lpGameTime->m_eTimeType)
			{
				case GameTime::GUILD:
				{
					if ( nMinutes >= -5 && nMinutes < 0 )
					{
						vecGuildTimeResult.push_back(GameTime::GT_PREPARE_START_5);
					}
					else if ( nMinutes >= -30 && nMinutes < 0 )
					{
						vecGuildTimeResult.push_back(GameTime::GT_PREPARE_START_30);
					}
					else if ( nMinutes >= static_cast<int>(lpGameTime->m_dwDuration - 30) && nMinutes < static_cast<int>(lpGameTime->m_dwDuration) )
					{
						vecGuildTimeResult.push_back(GameTime::GT_PREPARE_END_30);
					}
					else if ( nMinutes >= 0 && nMinutes < static_cast<int>(lpGameTime->m_dwDuration) )
					{
						vecGuildTimeResult.push_back(GameTime::GT_GUILD_WAR);
					}
					else
					{
						vecGuildTimeResult.push_back(GameTime::GT_NORMAL);
					}
				}
				break;

				case GameTime::REALM:
				{
					if ( nMinutes >= -5 && nMinutes < 0 )
					{
						vecRealmTimeResult.push_back(GameTime::RT_PREPARE_START_5);
					}
					else if ( nMinutes >= -10 && nMinutes < 0 )
					{
						vecRealmTimeResult.push_back(GameTime::RT_PREPARE_START_10);
					}
					else if ( nMinutes >= -15 && nMinutes < 0 )
					{
						vecRealmTimeResult.push_back(GameTime::RT_PREPARE_START_15);
					}
					else if ( nMinutes >= -30 && nMinutes < 0 )
					{
						vecRealmTimeResult.push_back(GameTime::RT_PREPARE_START_30);
					}
					else if ( nMinutes >= static_cast<int>(lpGameTime->m_dwDuration - 5) && nMinutes < static_cast<int>(lpGameTime->m_dwDuration) )
					{
						vecRealmTimeResult.push_back(GameTime::RT_REALM_WAR_END_5);
					}
					else if ( nMinutes >= 0 && nMinutes < static_cast<int>(lpGameTime->m_dwDuration) )
					{
						vecRealmTimeResult.push_back(GameTime::RT_REALM_WAR);

						// By Minbobo(���� ��ġ�� ������ �Ű��ּ���. ���� �������� �÷���Ÿ���� ������ �κ�).
						m_dwRealmTime = lpGameTime->m_dwDuration;
					}
					else
					{
						vecRealmTimeResult.push_back(GameTime::RT_NORMAL);
					}
				}
				break;

				case GameTime::SIEGE:
				{
					if ( nMinutes >= -5 && nMinutes < 0 )
					{
						vecSiegeTimeResult.push_back(GameTime::ST_PREPARE_START_5);
					}
					else if ( nMinutes >= static_cast<int>(lpGameTime->m_dwDuration - 5) && nMinutes < static_cast<int>(lpGameTime->m_dwDuration) )
					{
						vecSiegeTimeResult.push_back(GameTime::ST_PREPARE_END_5);
					}
					else if ( nMinutes >= 0 && nMinutes < static_cast<int>(lpGameTime->m_dwDuration) )
					{
						vecSiegeTimeResult.push_back(GameTime::ST_SIEGE_WAR);
					}
					else
					{
						vecSiegeTimeResult.push_back(GameTime::ST_NORMAL);
					}
				}
				break;

				case GameTime::NEWZONE:
				{
					if ( nMinutes >= -5 && nMinutes < 0 )
					{
						vecEnterTimeResult.push_back(GameTime::ET_PREPARE_START_5);
					}
					else if ( nMinutes >= static_cast<int>(lpGameTime->m_dwDuration - 5) && nMinutes < static_cast<int>(lpGameTime->m_dwDuration) )
					{
						vecEnterTimeResult.push_back(GameTime::ET_PREPARE_END_5);
					}
					else if ( nMinutes >= 0 && nMinutes < static_cast<int>(lpGameTime->m_dwDuration) )
					{
						vecEnterTimeResult.push_back(GameTime::ET_ALLOWENTER);
					}
					else
					{
						vecEnterTimeResult.push_back(GameTime::ET_LIMIT);
					}
				}
				break;

				case GameTime::MINE:
				{
					if ( nMinutes >= 0 && nMinutes < static_cast<int>(lpGameTime->m_dwDuration) )
					{
						vecMiningTimeResult.push_back(GameTime::MT_MINING);
					}
					else
					{
						vecMiningTimeResult.push_back(GameTime::MT_NORMAL);
					}
				}
				break;


				case GameTime::EVENT:
				{
					if ( nMinutes >= 0 && nMinutes < static_cast<int>(lpGameTime->m_dwDuration) )
					{
						vecEventTimeResult.push_back(static_cast<unsigned char>(lpGameTime->m_dwValue1));
					}
					else
					{
						vecEventTimeResult.push_back(GameTime::EVENT_NORMAL);
					}
				}
				break;
			}
		}
	}

	unsigned char cNewGuildTime = FindPrimaryValue( GameTime::GUILD, vecGuildTimeResult );
	unsigned char cNewRealmTime = FindPrimaryValue( GameTime::REALM, vecRealmTimeResult );
	unsigned char cNewSiegeTime = FindPrimaryValue( GameTime::SIEGE, vecSiegeTimeResult );
	unsigned char cNewMiningTime = FindPrimaryValue( GameTime::MINE, vecMiningTimeResult );
	unsigned char cNewEnterTime = FindPrimaryValue( GameTime::NEWZONE, vecEnterTimeResult );

	unsigned char cNewEventTime = FindPrimaryValue( GameTime::EVENT, vecEventTimeResult );

	if (cNewGuildTime != m_cNowGuildTime)
	{
		m_cNowGuildTime = cNewGuildTime;

		// DB ����
		DBComponent::GameTimeDB::UpdateGuildTime(*m_lpDBComponent, m_cNowGuildTime);
	}

	if (cNewRealmTime != m_cNowRealmTime)
	{
		m_cNowRealmTime = cNewRealmTime;

		// DB ����
		DBComponent::GameTimeDB::UpdateRealmTime(*m_lpDBComponent, m_cNowRealmTime);
	}

	if (cNewSiegeTime != m_cNowSiegeTime)
	{
		m_cNowSiegeTime = cNewSiegeTime;

		// DB ����
		DBComponent::GameTimeDB::UpdateSiegeTime(*m_lpDBComponent, m_cNowSiegeTime);
	}

	if (cNewEnterTime != m_cNowEnterTime)
	{
		m_cNowEnterTime = cNewEnterTime;

		// DB ����
		DBComponent::GameTimeDB::UpdateEnterTime(*m_lpDBComponent, m_cNowEnterTime);
	}

	if (cNewMiningTime != m_cNowMiningTime)
	{
		m_cNowMiningTime = cNewMiningTime;

		// DB ����
		DBComponent::GameTimeDB::UpdateMiningTime(*m_lpDBComponent, m_cNowMiningTime);
	}

	if (cNewEventTime != m_cNowEventTime)
	{
		m_cNowEventTime = cNewEventTime;

		// DB ����
		DBComponent::GameTimeDB::UpdateEventTime(*m_lpDBComponent, m_cNowEventTime);
	}
}

unsigned char	CGameTimeDBMgr::GetCurrentGameTime(unsigned char cTimeType) const
{
	switch (cTimeType)
	{
		case GameTime::GUILD:		return m_cNowGuildTime;		break;
		case GameTime::REALM:		return m_cNowRealmTime;		break;
		case GameTime::SIEGE:		return m_cNowSiegeTime;		break;
		case GameTime::NEWZONE:		return m_cNowEnterTime;		break;
		case GameTime::MINE:		return m_cNowMiningTime;	break;
		case GameTime::EVENT:		return m_cNowEventTime;	break;
	}

	ERRLOG1(g_Log, "���� ����Ÿ���� �ð� ���¸� ��µ� �����߽��ϴ�. Type : %d", cTimeType);
	return 0;
}

unsigned char	CGameTimeDBMgr::GetCurrentGuildTime() const
{
	return m_cNowGuildTime;
}

unsigned char	CGameTimeDBMgr::GetCurrentRealmTime() const
{
	return m_cNowRealmTime;
}

unsigned char	CGameTimeDBMgr::GetCurrentSiegeTime() const
{
	return m_cNowSiegeTime;
}

unsigned char	CGameTimeDBMgr::GetCurrentEnterTime() const
{
	return m_cNowEnterTime;
}

unsigned char	CGameTimeDBMgr::GetCurrentMiningTime() const
{
	return m_cNowMiningTime;
}

unsigned char	CGameTimeDBMgr::GetCurrentEventTime() const
{
	return m_cNowEventTime;
}

bool	CGameTimeDBMgr::IsGuildWarTime() const
{
	switch (m_cNowGuildTime)
	{
		case GameTime::GT_GUILD_WAR:
		case GameTime::GT_PREPARE_END_30:
			return true;

		default:
			return false;
	}

	return false;
}

bool	CGameTimeDBMgr::IsRealmWarTime() const
{
	if (GameTime::RT_REALM_WAR == m_cNowRealmTime || GameTime::RT_REALM_WAR_END_5 == m_cNowRealmTime)
		return true;

	return false;
}

bool	CGameTimeDBMgr::IsSiegeWarTime() const
{
	if (GameTime::ST_SIEGE_WAR == m_cNowSiegeTime || GameTime::ST_PREPARE_END_5 == m_cNowSiegeTime)
	{
		return true;
	}

	return false;
}

bool	CGameTimeDBMgr::IsEnterTime() const
{
	if (GameTime::ET_ALLOWENTER == m_cNowEnterTime || GameTime::ET_PREPARE_END_5 == m_cNowEnterTime)
	{
		return true;
	}

	return false;
}

bool	CGameTimeDBMgr::IsMiningTime() const
{
	if (GameTime::MT_MINING == m_cNowMiningTime)
		return true;

	return false;
}

bool	CGameTimeDBMgr::IsEventTime() const
{
	if (GameTime::EVENT_NORMAL != m_cNowEventTime)
		return true;

	return false;
}

bool	CGameTimeDBMgr::IsGuildWarTime(unsigned char cTime) const
{
	switch (cTime)
	{
		case GameTime::GT_GUILD_WAR:
		case GameTime::GT_PREPARE_END_30:
			return true;

		default:
			return false;
	}

	return false;
}

bool	CGameTimeDBMgr::IsRealmWarTime(unsigned char cTime) const
{
	if (GameTime::RT_REALM_WAR == cTime || GameTime::RT_REALM_WAR_END_5 == cTime)
		return true;

	return false;
}

bool	CGameTimeDBMgr::IsSiegeWarTime(unsigned char cTime) const
{
	if (GameTime::ST_SIEGE_WAR == cTime || GameTime::ST_PREPARE_END_5 == cTime)
	{
		return true;
	}

	return false;
}

bool	CGameTimeDBMgr::IsEnterTime(unsigned char cTime) const
{
	if (GameTime::ET_ALLOWENTER == cTime || GameTime::ET_PREPARE_END_5 == cTime)
	{
		return true;
	}

	return false;
}

bool	CGameTimeDBMgr::IsMiningTime(unsigned char cTime) const
{
	if (GameTime::MT_MINING == cTime)
		return true;

	return false;
}

bool	CGameTimeDBMgr::IsEventTime(unsigned char cTime) const
{
	if (GameTime::EVENT_NORMAL != cTime)
		return true;

	return false;
}

bool	CGameTimeDBMgr::IsRealmWarReadyTime() const
{
	switch (m_cNowRealmTime)
	{
		case GameTime::RT_PREPARE_START_10:
		case GameTime::RT_PREPARE_START_5:
			return true;

		default:
			return false;
	}

	return false;
}

bool	CGameTimeDBMgr::IsRealmWarReadyTime(unsigned char cTime) const
{
	switch (cTime)
	{
		case GameTime::RT_PREPARE_START_10:
		case GameTime::RT_PREPARE_START_5:
			return true;

		default:
			return false;
	}

	return false;
}

bool CGameTimeDBMgr::Initialize(CDBComponent& DBComponent)
{
	m_lpDBComponent = &DBComponent;

	const int MAX_ROWS = 256;
	int nGetRows = 0;

	char szBuffer[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(szBuffer, CDBComponent::QUERY_BUFFER_LEN - 1, "SELECT * FROM TblGameTime");

	szBuffer[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
	if (0 == m_lpDBComponent || !m_lpDBComponent->ExecuteQuery(szBuffer))
	{
		ERRLOG0(g_Log, "GameTime DB ������ ����");
		return false;
	}

	GameTimeDB* gameTimeInfo = new GameTimeDB[MAX_ROWS];
	memset(gameTimeInfo, 0, sizeof(GameTimeDB) * MAX_ROWS);

	while (m_lpDBComponent->GetData((void**)gameTimeInfo, sizeof(GameTimeDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) { break; }

		for (GameTimeDB* lpGameTimeInfo = gameTimeInfo; nGetRows > 0; --nGetRows, ++lpGameTimeInfo)
		{
			InitGameTimeValue(lpGameTimeInfo->m_cGuildTime, lpGameTimeInfo->m_cRealmTime,
							lpGameTimeInfo->m_cSiegeTime, lpGameTimeInfo->m_cMiningTime, lpGameTimeInfo->m_cEnterTime, lpGameTimeInfo->m_cEventTime);
		}

		memset(gameTimeInfo, 0, sizeof(GameTimeDB) * MAX_ROWS);
	}

	delete [] gameTimeInfo;
	return true;
}

void CGameTimeDBMgr::InitGameTimeValue(unsigned char cGuildTime, unsigned char cRealmTime,
									   unsigned char cSiegeTime, unsigned char cMiningTime, 
									   unsigned char cEnterTime, unsigned char cEventTime)
{
	m_cNowGuildTime = cGuildTime;
	m_cNowRealmTime = cRealmTime;
	m_cNowSiegeTime = cSiegeTime;
	m_cNowEnterTime = cEnterTime;
	m_cNowMiningTime = cMiningTime;
	m_cNowEventTime = cEventTime;
}

bool CGameTimeDBMgr::LoadGameTimeScript(const char* szFileName)
{	
	if (NULL == szFileName)
	{
		ERRLOG0(g_Log, "���� �ð� ��ũ��Ʈ ���� �θ��� �����߽��ϴ�.(���ڷ� ���� ���ϸ��� NULL �Դϴ�.)");			
		return false;
	}

	if (!m_GameTimeTable.empty())
	{
		m_GameTimeTable.clear();
	}

	CTokenlizedFile TokenlizedFile;	

	if (!TokenlizedFile.Open(szFileName))
	{
		ERRLOG0(g_Log, "���� �ð� ��ũ��Ʈ ���� �ε��� �����߽��ϴ�.");
		return false;
	}

	if (!TokenlizedFile.ReadColumn())
	{
		ERRLOG0(g_Log, "���� �ð� ��ũ��Ʈ �÷��� �о���� �� �����߽��ϴ�.");
		return false;
	}

	ParseDataArray  GameTimeParseData;
	SetGameTimeParseData(GameTimeParseData);

	// Parsing
	while (TokenlizedFile.ReadLine())
	{
		sGameTime gameTime;

		for (ParseDataArray::iterator itr = GameTimeParseData.begin(); itr != GameTimeParseData.end(); ++itr)				
		{
			const char* szValue = TokenlizedFile.GetStringValue(itr->m_szColumnName);

			if (NULL == szValue)
			{
				ERRLOG2(g_Log, "�����͸� �о�� �� �����ϴ�. Line:%d, ColumnName:%s", TokenlizedFile.GetCurrentLine(), itr->m_szColumnName);				
				return false;
			}

			if (!itr->m_fnParseFunc(gameTime, szValue))
			{
				ERRLOG2(g_Log, "�˸��� �����Ͱ� �ƴմϴ�. Line:%d, ColumnName:%s", TokenlizedFile.GetCurrentLine(), itr->m_szColumnName);
				return false;
			}
		}

		if (!m_GameTimeTable.insert(std::make_pair(gameTime.m_dwID, gameTime)).second)
		{
			ERRLOG0(g_Log, "m_GameTimeTable.insert() failed");
			return false;
		}		
	}

	return IntegrityCheck();
}

// Parsing Function
bool ReadID(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_dwID = static_cast<unsigned long>(atoi(szValue));
	return true;
}

bool ReadTimeType(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	char szType[0xff] = {0, };
	memcpy(szType, szValue, strlen(szValue));	
	_strupr(szType);

	if (0 == strcmp(szType, "GUILD"))
	{
		gameTime.m_eTimeType = GameTime::GUILD;
	}
	else if (0 == strcmp(szType, "REALM"))
	{
		gameTime.m_eTimeType = GameTime::REALM;
	}
	else if (0 == strcmp(szType, "SIEGE"))
	{
		gameTime.m_eTimeType = GameTime::SIEGE;
	}
	else if (0 == strcmp(szType, "NEWZONE"))
	{
		gameTime.m_eTimeType = GameTime::NEWZONE;
	}
	else if (0 == strcmp(szType, "MINE"))
	{
		gameTime.m_eTimeType = GameTime::MINE;
	}
	else if (0 == strcmp(szType, "EVENT"))
	{
		gameTime.m_eTimeType = GameTime::EVENT;
	}
	else
	{
		ERRLOG0(g_Log, "���� �ð� Ÿ�� ����");
		return false;
	}

	return true;
}

bool ReadCycle(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	char szType[0xff] = {0, };
	memcpy(szType, szValue, strlen(szValue));	
	_strupr(szType);

	if (0 == strcmp(szType, "WEEKLY"))
	{
		gameTime.m_eTimeCycle = GameTime::WEEKLY;
	}
	else if (0 == strcmp(szType, "DAILY"))
	{
		gameTime.m_eTimeCycle = GameTime::DAILY;
	}
	else if (0 == strcmp(szType, "MONTHLY"))
	{
		gameTime.m_eTimeCycle = GameTime::MONTHLY;
	}
	else if (0 == strcmp(szType, "DATE"))
	{
		gameTime.m_eTimeCycle = GameTime::DATE;
	}
	else
	{
		ERRLOG0(g_Log, "���� �ð� �ֱ� Ÿ�� ����");
		return false;
	}

	return true;
}

bool ReadWeek(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	if (GameTime::WEEKLY == gameTime.m_eTimeCycle)
	{
		char szWeek[0xff] = {0, };
		memcpy(szWeek, szValue, strlen(szValue));	
		_strupr(szWeek);

		if (0 == strcmp(szWeek, "SUN"))
		{
			gameTime.m_eWeek = GameTime::SUNDAY;
		}
		else if (0 == strcmp(szWeek, "MON"))
		{
			gameTime.m_eWeek = GameTime::MONDAY;
		}
		else if (0 == strcmp(szWeek, "TUE"))
		{
			gameTime.m_eWeek = GameTime::TUESDAY;
		}
		else if (0 == strcmp(szWeek, "WED"))
		{
			gameTime.m_eWeek = GameTime::WEDNESDAY;
		}
		else if (0 == strcmp(szWeek, "THU"))
		{
			gameTime.m_eWeek = GameTime::THURSDAY;
		}
		else if (0 == strcmp(szWeek, "FRI"))
		{
			gameTime.m_eWeek = GameTime::FRIDAY;
		}
		else if (0 == strcmp(szWeek, "SAT"))
		{
			gameTime.m_eWeek = GameTime::SATURDAY;
		}
		else
		{
			ERRLOG0(g_Log, "���� ���� ����");
			return false;
		}
	}

	return true;
}

bool ReadYear(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_wYear = static_cast<unsigned short>(atoi(szValue));

	if (GameTime::DATE == gameTime.m_eTimeCycle && gameTime.m_wYear < 2005)
	{
		ERRLOG1(g_Log, "���� �ð� ���� ���� : Year = %d", gameTime.m_wYear);
		return false;
	}

	return true;
}

bool ReadMonth(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_cMonth = static_cast<unsigned char>(atoi(szValue));		

	if (gameTime.m_cMonth > 12)
	{
		ERRLOG1(g_Log, "���� �ð� ���� ���� : Month = %d", gameTime.m_cMonth);
		return false;
	}

	return true;
}

bool ReadDay(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_cDay = static_cast<unsigned char>(atoi(szValue));	

	if (gameTime.m_cDay > 31)
	{
		ERRLOG1(g_Log, "���� �ð� ���� ���� : Day = %d", gameTime.m_cDay);
		return false;
	}

	return true;
}

bool ReadHour(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_cHour = static_cast<unsigned char>(atoi(szValue));		

	if (gameTime.m_cHour >= 24)
	{
		ERRLOG1(g_Log, "���� �ð� ���� ���� : Hour = %d", gameTime.m_cHour);
		return false;
	}

	return true;
}

bool ReadMinute(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_cMinute = static_cast<unsigned char>(atoi(szValue));	

	if (gameTime.m_cMinute >= 60)
	{
		ERRLOG1(g_Log, "���� �ð� ���� ���� : Minute = %d", gameTime.m_cMinute);
		return false;
	}	

	return true;
}

bool ReadDuration(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_dwDuration = static_cast<unsigned long>(atoi(szValue));	

	if (0 == gameTime.m_dwDuration)
	{
		ERRLOG0(g_Log, "���� �÷��� �ð��� �����ϴ�.");
		return false;
	}	

	return true;
}

bool ReadValue1(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_dwValue1 = static_cast<unsigned long>(atoi(szValue));	

	return true;
}

bool ReadValue2(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_dwValue2 = static_cast<unsigned long>(atoi(szValue));	

	return true;
}

bool CGameTimeDBMgr::SetGameTimeParseData(CGameTimeDBMgr::ParseDataArray& parseDataArray)
{
	parseDataArray.reserve(10);

	parseDataArray.push_back(CGameTimeDBMgr::ParseData("ID",		ReadID));
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Type",		ReadTimeType));
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Cycle",		ReadCycle));
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Week",		ReadWeek));
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Year",		ReadYear));
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Month",		ReadMonth));
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Day",		ReadDay));
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Hour",		ReadHour));
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Minute",	ReadMinute));
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Duration",	ReadDuration));

	// edith 2009.07.21 ����Ÿ�� �۾���
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Value1",	ReadValue1));
	parseDataArray.push_back(CGameTimeDBMgr::ParseData("Value2",	ReadValue2));
	return true;
}

bool	CGameTimeDBMgr::IntegrityCheck()
{
	std::vector<sGameTime*> vecGuildTime;
	std::vector<sGameTime*> vecRealmTime;
	std::vector<sGameTime*> vecSiegeTime;
	std::vector<sGameTime*> vecMiningTime;
	std::vector<sGameTime*> vecEnterTime;
	std::vector<sGameTime*> vecEventTime;

	GameTimeTable::iterator pos = m_GameTimeTable.begin();
	GameTimeTable::iterator end = m_GameTimeTable.end();

	for (; pos!=end; ++pos)
	{
		sGameTime* lpGameTime = &pos->second;
		if (NULL != lpGameTime)
		{
			switch (lpGameTime->m_eTimeType)
			{
				case GameTime::GUILD:	vecGuildTime.push_back(lpGameTime);		break;
				case GameTime::REALM:	vecRealmTime.push_back(lpGameTime);		break;
				case GameTime::SIEGE:	vecSiegeTime.push_back(lpGameTime);		break;
				case GameTime::MINE:	vecMiningTime.push_back(lpGameTime);	break;
				case GameTime::NEWZONE:	vecEnterTime.push_back(lpGameTime);		break;
				case GameTime::EVENT:	vecEventTime.push_back(lpGameTime);		break;
					

				default:
				{
					ERRLOG1(g_Log, "���ǵ��� ���� ���� �ð� Ÿ���� �ֽ��ϴ�. (Type : %d)", lpGameTime->m_eTimeType);
					return false;
				}
				break;
			}
		}
	}

	if (vecGuildTime.size() > 1)
	{
		CTime nowTime = CTime::GetCurrentTime();
		CTime standardTime = CTime(nowTime.GetYear(), nowTime.GetMonth(), nowTime.GetDay(), 0, 0, 0);
		CTime t1, t2;
		CTimeSpan tA, tB, tC, tD;

		for (size_t idx1=0; idx1<vecGuildTime.size(); ++idx1)
		{
			t1 = GetTimeByCycle( vecGuildTime[idx1], nowTime );
			tA = (t1 - standardTime) - 30 * 60;
			tB = tA + vecGuildTime[idx1]->m_dwDuration * 60;

			for (size_t idx2=idx1+1; idx2<vecGuildTime.size(); ++idx2)
			{
				t2 = GetTimeByCycle( vecGuildTime[idx2], nowTime );
				tC = (t2 - standardTime) - 30 * 60;
				tD = tC + vecGuildTime[idx2]->m_dwDuration * 60;

				if (tA <= tC && tC < tB) return false;
				if (tC <= tA && tA < tD) return false;
			}
		}
	}

	if (vecRealmTime.size() > 1)
	{
		CTime nowTime = CTime::GetCurrentTime();
		CTime standardTime = CTime(nowTime.GetYear(), nowTime.GetMonth(), nowTime.GetDay(), 0, 0, 0);
		CTime t1, t2;
		CTimeSpan tA, tB, tC, tD;

		for (size_t idx1=0; idx1<vecRealmTime.size(); ++idx1)
		{
			t1 = GetTimeByCycle( vecRealmTime[idx1], nowTime );
			tA = (t1 - standardTime) - 30 * 60;
			tB = tA + vecRealmTime[idx1]->m_dwDuration * 60;

			for (size_t idx2=idx1+1; idx2<vecRealmTime.size(); ++idx2)
			{
				t2 = GetTimeByCycle( vecRealmTime[idx2], nowTime );
				tC = (t2 - standardTime) - 30 * 60;
				tD = tC + vecRealmTime[idx2]->m_dwDuration * 60;

				if (tA <= tC && tC < tB) return false;
				if (tC <= tA && tA < tD) return false;
			}
		}
	}

	if (vecSiegeTime.size() > 1)
	{
		CTime nowTime = CTime::GetCurrentTime();
		CTime standardTime = CTime(nowTime.GetYear(), nowTime.GetMonth(), nowTime.GetDay(), 0, 0, 0);
		CTime t1, t2;
		CTimeSpan tA, tB, tC, tD;

		for (size_t idx1=0; idx1<vecSiegeTime.size(); ++idx1)
		{
			t1 = GetTimeByCycle( vecSiegeTime[idx1], nowTime );
			tA = (t1 - standardTime) - 30 * 60;
			tB = tA + vecSiegeTime[idx1]->m_dwDuration * 60;

			for (size_t idx2=idx1+1; idx2<vecSiegeTime.size(); ++idx2)
			{
				t2 = GetTimeByCycle( vecSiegeTime[idx2], nowTime );
				tC = (t2 - standardTime) - 30 * 60;
				tD = tC + vecSiegeTime[idx2]->m_dwDuration * 60;

				if (tA <= tC && tC < tB) return false;
				if (tC <= tA && tA < tD) return false;
			}
		}
	}

	if (vecEnterTime.size() > 1)
	{
		CTime nowTime = CTime::GetCurrentTime();
		CTime standardTime = CTime(nowTime.GetYear(), nowTime.GetMonth(), nowTime.GetDay(), 0, 0, 0);
		CTime t1, t2;
		CTimeSpan tA, tB, tC, tD;

		for (size_t idx1=0; idx1<vecEnterTime.size(); ++idx1)
		{
			t1 = GetTimeByCycle( vecEnterTime[idx1], nowTime );
			tA = (t1 - standardTime) - 30 * 60;
			tB = tA + vecEnterTime[idx1]->m_dwDuration * 60;

			for (size_t idx2=idx1+1; idx2<vecEnterTime.size(); ++idx2)
			{
				t2 = GetTimeByCycle( vecEnterTime[idx2], nowTime );
				tC = (t2 - standardTime) - 30 * 60;
				tD = tC + vecEnterTime[idx2]->m_dwDuration * 60;

				if (tA <= tC && tC < tB) return false;
				if (tC <= tA && tA < tD) return false;
			}
		}
	}

	if (vecMiningTime.size() > 1)
	{
		CTime nowTime = CTime::GetCurrentTime();
		CTime standardTime = CTime(nowTime.GetYear(), nowTime.GetMonth(), nowTime.GetDay(), 0, 0, 0);
		CTime t1, t2;
		CTimeSpan tA, tB, tC, tD;

		for (size_t idx1=0; idx1<vecMiningTime.size(); ++idx1)
		{
			t1 = GetTimeByCycle( vecMiningTime[idx1], nowTime );
			tA = (t1 - standardTime) - 30 * 60;
			tB = tA + vecMiningTime[idx1]->m_dwDuration * 60;

			for (size_t idx2=idx1+1; idx2<vecMiningTime.size(); ++idx2)
			{
				t2 = GetTimeByCycle( vecMiningTime[idx2], nowTime );
				tC = (t2 - standardTime) - 30 * 60;
				tD = tC + vecMiningTime[idx2]->m_dwDuration * 60;

				if (tA <= tC && tC < tB) return false;
				if (tC <= tA && tA < tD) return false;
			}
		}
	}

	if (vecEventTime.size() > 1)
	{
		CTime nowTime = CTime::GetCurrentTime();
		CTime standardTime = CTime(nowTime.GetYear(), nowTime.GetMonth(), nowTime.GetDay(), 0, 0, 0);
		CTime t1, t2;
		CTimeSpan tA, tB, tC, tD;

		for (size_t idx1=0; idx1<vecEventTime.size(); ++idx1)
		{
			t1 = GetTimeByCycle( vecEventTime[idx1], nowTime );
			tA = (t1 - standardTime) - 30 * 60;
			tB = tA + vecEventTime[idx1]->m_dwDuration * 60;

			for (size_t idx2=idx1+1; idx2<vecEventTime.size(); ++idx2)
			{
				t2 = GetTimeByCycle( vecEventTime[idx2], nowTime );
				tC = (t2 - standardTime) - 30 * 60;
				tD = tC + vecEventTime[idx2]->m_dwDuration * 60;

				if (tA <= tC && tC < tB) return false;
				if (tC <= tA && tA < tD) return false;
			}
		}
	}
	

	return true;
}

CTime	CGameTimeDBMgr::GetTimeByCycle(sGameTime* lpGameTime, CTime nowTime)
{
	switch (lpGameTime->m_eTimeCycle)
	{
		case GameTime::WEEKLY:		return GetWeeklyTime(lpGameTime, nowTime);		break;
		case GameTime::DAILY:		return GetDailyTime(lpGameTime, nowTime);		break;
		case GameTime::MONTHLY:		return GetMonthlyTime(lpGameTime, nowTime);		break;
		case GameTime::DATE:		return GetDateTime(lpGameTime, nowTime);		break;
	}

	return CTime(0, 0, 0, 0, 0, 0);
}

CTime	CGameTimeDBMgr::GetWeeklyTime(sGameTime* lpGameTime, CTime nowTime)
{
	if (0 == nowTime.GetTime())
	{
		nowTime = CTime::GetCurrentTime();
	}

	GameTime::Week eWeek = static_cast<GameTime::Week>(nowTime.GetDayOfWeek());
	char cWeek = eWeek - lpGameTime->m_eWeek;
	unsigned short wYear = nowTime.GetYear();

	CTime t1(nowTime.GetYear(), nowTime.GetMonth(), 1, 0, 0, 0);

	if (cWeek != 0)
	{					 					 
		unsigned char cMonth = nowTime.GetMonth() + 1;

		if (nowTime.GetMonth() == 12)
		{
			wYear	= nowTime.GetYear() + 1;
			cMonth	= 1;
		}

		CTime t2(wYear, cMonth, 1, 0, 0, 0);
		CTimeSpan ts = t2 - t1;

		LONG lDays = static_cast<LONG>(ts.GetDays());
		unsigned char cDay = 0;
		
		wYear	= nowTime.GetYear();
		cMonth	= static_cast<unsigned char>(nowTime.GetMonth());				

		if(cWeek < 0)
		{
			cDay = nowTime.GetDay() - cWeek;
			
			if (nowTime.GetDay() - cWeek > lDays)
			{
				cDay -= static_cast<unsigned char>(lDays);

				if (nowTime.GetMonth() == 12)
				{
					wYear	= nowTime.GetYear() + 1;
					cMonth	= 1;
				}
				else
				{								
					cMonth	= nowTime.GetMonth() + 1;
				}
			}
		}
		else if (cWeek > 0)
		{			
			cDay = nowTime.GetDay() + ( 7 - cWeek ) ;

			if (cDay > lDays)
			{
				cDay -= static_cast<unsigned char>(lDays);

				if (nowTime.GetMonth() == 12)
				{
					wYear	= nowTime.GetYear() + 1;
					cMonth	= 1;
				}
				else
				{								
					cMonth	= nowTime.GetMonth() + 1;
				}
			}
		}

		return CTime(wYear, cMonth, cDay, lpGameTime->m_cHour, lpGameTime->m_cMinute, 0);
	}
	else
	{
		CTime gameTime = CTime(nowTime.GetYear(), nowTime.GetMonth(), nowTime.GetDay(), lpGameTime->m_cHour, lpGameTime->m_cMinute, 0);

		CTime CurrentNowTime = CTime::GetCurrentTime();

		CTimeSpan timeSpan(CurrentNowTime - gameTime);

		timeSpan -= (lpGameTime->m_dwDuration * 60);

		if (timeSpan.GetTimeSpan() >= 0)
		{
			// �̹� ���ӽð��� �������ȴٸ�, �������� �ð��� ���ؼ� �����Ѵ�.
			CTimeSpan timeForDay(7 * 24 * 60 * 60);
			gameTime += timeForDay;
		}

		return gameTime;
	}

	return CTime(0, 0, 0, 0, 0, 0);
}

CTime	CGameTimeDBMgr::GetDailyTime(sGameTime* lpGameTime, CTime nowTime)
{
	if (0 == nowTime.GetTime())
	{
		nowTime = CTime::GetCurrentTime();
	}

	CTime gameTime = CTime(nowTime.GetYear(), nowTime.GetMonth(), nowTime.GetDay(), lpGameTime->m_cHour, lpGameTime->m_cMinute, 0);
	CTime CurrentNowTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan(CurrentNowTime - gameTime);
	timeSpan -= (lpGameTime->m_dwDuration * 60);
	if (timeSpan.GetTimeSpan() >= 0)
	{
		// �̹� ���ӽð��� �������ȴٸ�, �������� �ð��� ���ؼ� �����Ѵ�.
		CTimeSpan timeForDay(24 * 60 * 60);
		gameTime += timeForDay;
	}

	return gameTime;
}

CTime	CGameTimeDBMgr::GetMonthlyTime(sGameTime* lpGameTime, CTime nowTime)
{
	if (0 == nowTime.GetTime())
	{
		nowTime = CTime::GetCurrentTime();
	}

	CTime gameTime = CTime(nowTime.GetYear(), nowTime.GetMonth(), lpGameTime->m_cDay, lpGameTime->m_cHour, lpGameTime->m_cMinute, 0);
	CTime CurrentNowTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan(CurrentNowTime - gameTime);
	timeSpan -= (lpGameTime->m_dwDuration * 60);
	if (timeSpan.GetTimeSpan() >= 0)
	{
		// �̹� ���ӽð��� �������ȴٸ�, �������� �ð��� ���ؼ� �����Ѵ�.
		if (nowTime.GetMonth() == 12)
		{
			gameTime = CTime(nowTime.GetYear() + 1, 1, lpGameTime->m_cDay, lpGameTime->m_cHour, lpGameTime->m_cMinute, 0);
		}
		else
		{
			gameTime = CTime(nowTime.GetYear(), nowTime.GetMonth() + 1, lpGameTime->m_cDay, lpGameTime->m_cHour, lpGameTime->m_cMinute, 0);
		}
	}

	return gameTime;
}

CTime	CGameTimeDBMgr::GetDateTime(sGameTime* lpGameTime, CTime nowTime)
{
	if (0 == nowTime.GetTime())
	{
		nowTime = CTime::GetCurrentTime();
	}

	return CTime(lpGameTime->m_wYear, lpGameTime->m_cMonth, lpGameTime->m_cDay, lpGameTime->m_cHour, lpGameTime->m_cMinute, 0);
}

void	CGameTimeDBMgr::GetTimeInfo(unsigned char cType, TimeInfo& timeInfo)
{
	ZeroMemory(&timeInfo, sizeof(TimeInfo));
	std::vector<sGameTime*> vecTime;

	GameTimeTable::iterator pos = m_GameTimeTable.begin();
	GameTimeTable::iterator end = m_GameTimeTable.end();

	for (; pos!=end; ++pos)
	{
		sGameTime* lpGameTime = &pos->second;
		if (NULL != lpGameTime)
		{
			if (lpGameTime->m_eTimeType == static_cast<GameTime::TimeType>(cType))
			{
				vecTime.push_back(lpGameTime);
			}
		}
	}

	// ������������ �ð��� ���ϴ� ������ üũ
	bool bRequestEndTime = false;
	switch (cType)
	{
		case GameTime::GUILD:
		{
			if (IsGuildWarTime())
			{
				bRequestEndTime = true;
			}
		}
		break;

		case GameTime::REALM:
		{
			if (IsRealmWarTime())
			{
				bRequestEndTime = true;
			}
		}
		break;

		case GameTime::SIEGE:
		{
			if (IsSiegeWarTime())
			{
				bRequestEndTime = true;
			}
		}
		break;

		case GameTime::NEWZONE:
		{
			if (IsEnterTime())
			{
				bRequestEndTime = true;
			}
		}
		break;

		case GameTime::EVENT:
		{
			if (IsEventTime())
			{
				bRequestEndTime = true;
			}
		}
		break;
	}

	__time64_t minTimeSpan = ULONG_MAX;
	unsigned long dwDuration = 0;
	CTime resTime;

	CTime nowTime = CTime::GetCurrentTime();
	std::vector<sGameTime*>::iterator vecItr = vecTime.begin();
	std::vector<sGameTime*>::iterator vecEnd = vecTime.end();

	for (; vecItr!=vecEnd; ++vecItr)
	{
		sGameTime* lpGameTime = *vecItr;
		if (NULL != lpGameTime)
		{
			CTime tempTime = GetTimeByCycle(lpGameTime, nowTime);
			CTimeSpan tempSpan(tempTime - nowTime);

			if (bRequestEndTime)
			{
				tempSpan += lpGameTime->m_dwDuration * 60;
			}
			
			if (tempSpan.GetTimeSpan() >= 0 && tempSpan.GetTimeSpan() < minTimeSpan)
			{
				minTimeSpan = tempSpan.GetTimeSpan();
				resTime = tempTime;
				dwDuration = lpGameTime->m_dwDuration;
			}
		}
	}

	if (ULONG_MAX != minTimeSpan)
	{
		timeInfo.m_cMonth	= resTime.GetMonth();
		timeInfo.m_cDay		= resTime.GetDay();
		timeInfo.m_cHour	= resTime.GetHour();
		timeInfo.m_cMinute	= resTime.GetMinute();
		timeInfo.m_dwTime	= dwDuration;
	}
}