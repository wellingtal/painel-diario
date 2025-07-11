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
			// 시간 체크 //
			// 예전엔 로그아웃 시간으로 접속해있는 시간을 계산했다.
			// 하지만 새로 바뀐건 현재 서버에 로그인한 시간에서 얼마나 지났는지를
			// 나타낼때 사용한다.
			// edith 2008.12.4 로그인 타임으로 처리
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
				// 캐릭터가 다크 카나번에 있을 경우만 체크.
				if(SERVER_ID::STONE_WAR1 <= serverID.GetZone() && serverID.GetZone() <= SERVER_ID::STONE_WAR3)
				{			
					if(lpCharacterData->GetRealmWarFlag()==Creature::WAR_INSTANCE || lpCharacterData->GetRealmWarFlag()==Creature::WAR_ON)
					{
						CTime timeLogin(pTime->Year, pTime->Month, pTime->Day, pTime->Hour, pTime->Minute, pTime->Second, pTime->MSecond);

						CTimeSpan ts = timeCurrent - timeLogin;

						// 2008.09.23 시간이 넘어야 렐름포인트를 주는걸 해제한다.
						// 무조건 참가했으면 1개 줌.
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
			// 캐릭터가 다크 카나번에 있을 경우만 체크.
			if(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= serverID.GetZone() && serverID.GetZone() <= SERVER_ID::STONE_WAR3)
			{			
				// 전쟁종료중 유저가 튕겼을때 GetRealmCheckPoint() 에 값이 셋팅된다.
				// 값이 셋팅된후 전쟁이 끝나도 1포인트를 +해준다.
				if(!lpCharacterData->GetRealmCheckPoint())
				{
					// edith 2009.06.13 쟁 참여 보너스 중 별 부여 보너스 삭제
/*
					CTime timeLogin(pTime->Year, pTime->Month, pTime->Day, pTime->Hour, pTime->Minute, pTime->Second, pTime->MSecond);

					CTimeSpan ts = timeCurrent - timeLogin;

					// 2008.11.28 시간이 넘어야 렐름포인트를 주는걸 해제한다.
					// 무조건 참가했으면 1개 줌.
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

				// 로그아웃때 호출될때
				if(cRealmType)
					return;

				pktRealmPoint.m_cRealmPoint	= cRealmPoint;
				pktRealmPoint.m_cType		= PktRealmPoint::REALM_JOIN;
			}
			else
			{
				// 국가전쟁에 참여하지않은자. 혹은 1시간 이상 참여하지 않은자.
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
			// 게임 서버에 알려준다.
			if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktRealmPoint), sizeof(PktRealmPoint), CmdRealmPoint, 0, 0))
			{
				DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
					reinterpret_cast<char*>(&pktRealmPoint), sizeof(PktRealmPoint), CmdRealmPoint));
			}

			// 정상적으로 랠름포인트를 얻었으니 로그아웃 타임을 갱신해주면된다.
			// 전쟁종료후 현재존에서 24시간동안 대기할것을 대비해서
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
			// edith 2008.12.4 로그인 타임갱신기능 추가
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

		// edith 2009.07.21 게임타임 작업물
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

							// 달이 넘어가는 경우
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

							// 지난 달로 넘어가는 경우
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
	//	: 이전 시간 상태와 새로 갱신된 시간을 체크해서 현재 시간 상태를 얻어온다.
	//  ---------------------------------------------------------------------------------------  //

	unsigned char cOldGuildTime = GetCurrentGuildTime();
	unsigned char cOldRealmTime = GetCurrentRealmTime();
	unsigned char cOldSiegeTime = GetCurrentSiegeTime();
	unsigned char cOldEnterTime = GetCurrentEnterTime();
	unsigned char cOldMiningTime = GetCurrentMiningTime();

	// edith 2009.07.21 게임타임 작업물
	unsigned char cOldEventTime = GetCurrentEventTime();

	// 시간 갱신
	CheckGameTime();

	unsigned char cNewGuildTime = GetCurrentGuildTime();
	unsigned char cNewRealmTime = GetCurrentRealmTime();
	unsigned char cNewSiegeTime = GetCurrentSiegeTime();
	unsigned char cNewEnterTime = GetCurrentEnterTime();
	unsigned char cNewMiningTime = GetCurrentMiningTime();

	// edith 2009.07.21 게임타임 작업물
	unsigned char cNewEventTime = GetCurrentEventTime();
	//===========================================================================================//

	//===========================================================================================//
	// Step 2
	//	: 가동중인 채굴기들은 임시 보관소에 채굴한 광물을 저장한다.
	//  ---------------------------------------------------------------------------------------  //

	if (IsMiningTime(cOldMiningTime) && !IsMiningTime())
	{
		// 임소 보관소로 채굴한 광물 저장
		CSiegeObjectDBMgr::GetInstance().SendProcessMining(PktProcessMining::TEMPORARY_PROCESS);
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 3
	//	: 이벤트 시간일때 처리
	//  ---------------------------------------------------------------------------------------  //

	if (IsEventTime(cOldEventTime) && !IsEventTime())
	{
		;// 어떤일을 해야할까?
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 3
	//	: 길드전이 시작된 경우의 처리
	//  ---------------------------------------------------------------------------------------  //

	if (!IsGuildWarTime(cOldRealmTime) && IsGuildWarTime())
	{
		// 길드 가입 대기자를 전부 탈퇴 시킨다. (용병은 제외)
		Guild::CGuildDBMgr::GetInstance().KickAllWaitMember() ;
	}

	//===========================================================================================//


	//===========================================================================================//
	// Step 4
	//	: 길드전이 끝났을 경우의 처리
	//  ---------------------------------------------------------------------------------------  //

	if (IsGuildWarTime(cOldGuildTime) && !IsGuildWarTime())
	{
		// 채굴기의 경우 임시 광물을 누적 광물로 옮겨준다.
		CSiegeObjectDBMgr::GetInstance().SendProcessMining(PktProcessMining::ACCUMULATED_PROCESS);

		// 월드 웨폰 파괴후 공성 시간 지난 횟수(현재는 임의로 길드전 시간 지난 횟수) 증가
		CSiegeObjectDBMgr::GetInstance().DecreaseWeaponRemainSiegeTime();

		// 공성 시간 지난 횟수(현재는 임의로 길드전 시간 지난 횟수) 업데이트
		CSiegeObjectDBMgr::GetInstance().UpdateSiegeCount();

		// edith 2008.10.28 공성 종료후 명성치 보상.
		// 공성시간이 지난후 소지한 길드요새별로 명성치 보상.
		CSiegeObjectDBMgr::GetInstance().UpdateRewardFame();

		// 길드 전쟁시 경계 적대 / 카운터 적대 해지
		Guild::CGuildDBMgr::GetInstance().CancelAlertAndCounterHostility();
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 5
	//	: 공성전이 시작된 경우 처리
	//  ---------------------------------------------------------------------------------------  //

	if (!IsSiegeWarTime(cOldSiegeTime) && IsSiegeWarTime())
	{
		// 성들이 공성이 가능한지 각각 체크한다.
		Castle::CCastleDBMgr::GetInstance().CheckEnableSiegeForAllCastle(true);

		// 공성이 가능한 성들의 성문을 막는다.
		Castle::CCastleDBMgr::GetInstance().CloseCastleGate();
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 6
	//	: 공성전이 끝난 경우 처리
	//  ---------------------------------------------------------------------------------------  //

	if (IsSiegeWarTime(cOldSiegeTime) && !IsSiegeWarTime())
	{
		// 공성 시간이 끝나면 병기들을 스타트킷으로 돌려준다.
		CSiegeObjectDBMgr::GetInstance().DestoryAllSiegeArms();

		// 공성 시간이 끝나면 수성 병기들을 병기 관리 NPC로 돌려놓는다.
		Castle::CCastleDBMgr::GetInstance().DestroyAllCastleArms();

		// 성의 공성 횟수 업데이트
		Castle::CCastleDBMgr::GetInstance().UpdateSiegeCount();

		// 성들이 공성이 가능한지 각각 체크한다.
		Castle::CCastleDBMgr::GetInstance().CheckEnableSiegeForAllCastle(false);

		// 공성이 끝난 성들의 성문을 막는다.
		Castle::CCastleDBMgr::GetInstance().CloseCastleGate();
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 7
	//	: 국가전이 시작될 경우 처리
	//  ---------------------------------------------------------------------------------------  //

	if (!IsRealmWarTime(cOldRealmTime) && IsRealmWarTime())
	{
	//	if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//hayzohar statue war fix
	//	{
			// 시간 초기화.
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
	//	: 국가전이 끝날 경우 처리
	//  ---------------------------------------------------------------------------------------  //

	// edith 2010.01.10 시간별로 길드포인트 주는 로직이 제거됬다. 
	// 별이 이제는 길드원 수에 따라 주게 수정됨
	if (IsRealmWarTime(cOldRealmTime) && !IsRealmWarTime())
	{
		// 컨텐츠 : 다크 카나번 국가 전쟁
		if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))
		{
			// 1시간 이상 플레이 한 캐릭터에게 국가 공헌훈장 포인트를 준다.
			DBAgent::DataStorage::CSessionDataMgr::GetInstance().ProcessRealmPoint(RealmPoint::AddRealmPoint, 0);
		}
	}
	//===========================================================================================//
*/

	//===========================================================================================//
	// Step 9
	//	: 게임 시간 정보 전송
	//  ---------------------------------------------------------------------------------------  //

	bool bGuildUpdated = false;
	bool bRealmUpdated = false;
	bool bSiegeUpdated = false;
	bool bEnterUpdated = false;

	// edith 2009.07.21 게임타임 작업물
	bool bEventUpdated = false;

	if (cOldGuildTime != cNewGuildTime)	bGuildUpdated = true;
	if (cOldRealmTime != cNewRealmTime)	bRealmUpdated = true;
	if (cOldSiegeTime != cNewSiegeTime)	bSiegeUpdated = true;
	if (cOldEnterTime != cNewEnterTime)	bEnterUpdated = true;

	// edith 2009.07.21 게임타임 작업물
	if (cOldEventTime != cNewEventTime)	bEventUpdated = true;

	if (bGuildUpdated || bRealmUpdated || bSiegeUpdated || bEnterUpdated || bEventUpdated)
	{
		SendGameTimeInfo(GameTime::TYPE_CHANGE_GAME_TIME, bGuildUpdated, bRealmUpdated, bSiegeUpdated, bEnterUpdated, bEventUpdated);
	}
	//===========================================================================================//


	//===========================================================================================//
	// Step 10
	//	: 길드전, 국가전 종료시 Instance Flag 를 Off 로 바꿔준다.
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

						// By Minbobo(좋은 위치가 있으면 옮겨주세요. 현재 국가전쟁 플레이타임을 얻어오는 부분).
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

		// DB 갱신
		DBComponent::GameTimeDB::UpdateGuildTime(*m_lpDBComponent, m_cNowGuildTime);
	}

	if (cNewRealmTime != m_cNowRealmTime)
	{
		m_cNowRealmTime = cNewRealmTime;

		// DB 갱신
		DBComponent::GameTimeDB::UpdateRealmTime(*m_lpDBComponent, m_cNowRealmTime);
	}

	if (cNewSiegeTime != m_cNowSiegeTime)
	{
		m_cNowSiegeTime = cNewSiegeTime;

		// DB 갱신
		DBComponent::GameTimeDB::UpdateSiegeTime(*m_lpDBComponent, m_cNowSiegeTime);
	}

	if (cNewEnterTime != m_cNowEnterTime)
	{
		m_cNowEnterTime = cNewEnterTime;

		// DB 갱신
		DBComponent::GameTimeDB::UpdateEnterTime(*m_lpDBComponent, m_cNowEnterTime);
	}

	if (cNewMiningTime != m_cNowMiningTime)
	{
		m_cNowMiningTime = cNewMiningTime;

		// DB 갱신
		DBComponent::GameTimeDB::UpdateMiningTime(*m_lpDBComponent, m_cNowMiningTime);
	}

	if (cNewEventTime != m_cNowEventTime)
	{
		m_cNowEventTime = cNewEventTime;

		// DB 갱신
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

	ERRLOG1(g_Log, "현재 게임타입의 시간 상태를 얻는데 실패했습니다. Type : %d", cTimeType);
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
		ERRLOG0(g_Log, "GameTime DB 얻어오기 실패");
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
		ERRLOG0(g_Log, "게임 시간 스크립트 파일 로링에 실패했습니다.(인자로 받은 파일명이 NULL 입니다.)");			
		return false;
	}

	if (!m_GameTimeTable.empty())
	{
		m_GameTimeTable.clear();
	}

	CTokenlizedFile TokenlizedFile;	

	if (!TokenlizedFile.Open(szFileName))
	{
		ERRLOG0(g_Log, "게임 시간 스크립트 파일 로딩에 실패했습니다.");
		return false;
	}

	if (!TokenlizedFile.ReadColumn())
	{
		ERRLOG0(g_Log, "게임 시간 스크립트 컬럼을 읽어오는 데 실패했습니다.");
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
				ERRLOG2(g_Log, "데이터를 읽어올 수 없습니다. Line:%d, ColumnName:%s", TokenlizedFile.GetCurrentLine(), itr->m_szColumnName);				
				return false;
			}

			if (!itr->m_fnParseFunc(gameTime, szValue))
			{
				ERRLOG2(g_Log, "알맞은 데이터가 아닙니다. Line:%d, ColumnName:%s", TokenlizedFile.GetCurrentLine(), itr->m_szColumnName);
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
		ERRLOG0(g_Log, "게임 시간 타입 에러");
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
		ERRLOG0(g_Log, "게임 시간 주기 타입 에러");
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
			ERRLOG0(g_Log, "요일 설정 에러");
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
		ERRLOG1(g_Log, "게임 시간 설정 에러 : Year = %d", gameTime.m_wYear);
		return false;
	}

	return true;
}

bool ReadMonth(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_cMonth = static_cast<unsigned char>(atoi(szValue));		

	if (gameTime.m_cMonth > 12)
	{
		ERRLOG1(g_Log, "게임 시간 설정 에러 : Month = %d", gameTime.m_cMonth);
		return false;
	}

	return true;
}

bool ReadDay(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_cDay = static_cast<unsigned char>(atoi(szValue));	

	if (gameTime.m_cDay > 31)
	{
		ERRLOG1(g_Log, "게임 시간 설정 에러 : Day = %d", gameTime.m_cDay);
		return false;
	}

	return true;
}

bool ReadHour(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_cHour = static_cast<unsigned char>(atoi(szValue));		

	if (gameTime.m_cHour >= 24)
	{
		ERRLOG1(g_Log, "게임 시간 설정 에러 : Hour = %d", gameTime.m_cHour);
		return false;
	}

	return true;
}

bool ReadMinute(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_cMinute = static_cast<unsigned char>(atoi(szValue));	

	if (gameTime.m_cMinute >= 60)
	{
		ERRLOG1(g_Log, "게임 시간 설정 에러 : Minute = %d", gameTime.m_cMinute);
		return false;
	}	

	return true;
}

bool ReadDuration(CGameTimeDBMgr::sGameTime& gameTime, const char* szValue)
{
	gameTime.m_dwDuration = static_cast<unsigned long>(atoi(szValue));	

	if (0 == gameTime.m_dwDuration)
	{
		ERRLOG0(g_Log, "게임 플레이 시간이 없습니다.");
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

	// edith 2009.07.21 게임타임 작업물
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
					ERRLOG1(g_Log, "정의되지 않은 게임 시간 타입이 있습니다. (Type : %d)", lpGameTime->m_eTimeType);
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
			// 이미 게임시간을 지나버렸다면, 다음주의 시간을 구해서 리턴한다.
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
		// 이미 게임시간을 지나버렸다면, 다음날의 시간을 구해서 리턴한다.
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
		// 이미 게임시간을 지나버렸다면, 다음달의 시간을 구해서 리턴한다.
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

	// 끝날때까지의 시간을 구하는 것인지 체크
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