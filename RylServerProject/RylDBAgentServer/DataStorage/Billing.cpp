#include "stdafx.h"
#include "Billing.h"
#include "SessionData.h"

#include <Network/Stream/SendStream.h>
#include <Network/SendPacket/SendServerInfo.h>

#include <Utility/Setup/ServerSetup.h>
#include <Log/ServerLog.h>


namespace DBAgent
{
namespace DataStorage
{


enum BillingConst
{
    SENDED_BILLING_TIMEOUT_NOTIFY   = (1 <<  0)
};


CBilling::CBilling()
:   m_nLoginTime(0), 
    m_nPlayTime(0), 
    m_dwCRMIndex1(0),
    m_cBillingType(0), 
    m_cFlags(0),
    m_eWarnMsgType(WARN_BEFORE_1SEC)
{

}

CBilling::~CBilling()
{


}


void CBilling::StartBilling(int nPlayTime, unsigned long dwCRMIndex1, unsigned char cBillingType)
{
    m_nLoginTime    = time(0);
    m_nPlayTime     = nPlayTime;
    m_dwCRMIndex1   = dwCRMIndex1;
    m_cBillingType  = cBillingType;
    m_cFlags        = 0;
}

void CBilling::ClearBilling()
{
    m_nLoginTime    = 0;
    m_nPlayTime     = 0;
    m_dwCRMIndex1   = 0;
    m_cBillingType  = 0;
    m_cFlags        = 0;
}

void CBilling::AddYouxiLandPoint(int nAdditionalPoint, unsigned char cBillingType)
{
    m_nPlayTime     += nAdditionalPoint;
    m_cBillingType  =  cBillingType;

    m_cFlags &= ~SENDED_BILLING_TIMEOUT_NOTIFY;
}

bool CBilling::CheckBilling(const CSessionData& SessionData, time_t nCurrentTime)
{
    // 로그인 정보가 세팅되었고, PC방이 아니고,
    // 현재 CHAR_ENABLED상태이고,
    // 한게임 통합빌링이 아닌 경우에만 빌링 체크.
    if (0 != m_nLoginTime && 0 == m_dwCRMIndex1 && 
        CSessionData::SE_CHAR_ENABLED == SessionData.GetSessionState() &&
        CServerSetup::GamaUnitedBilling != CServerSetup::GetInstance().GetBillingType())
    {
        switch(m_cBillingType)
        {
        case 'T':

            // 정량제. m_nPlayTime은 분단위
            {
                // 추가시간 20초 더 줌.
                int nRemainSecond = int(m_nPlayTime * 60 + m_nLoginTime - nCurrentTime) + 20;

                // 5분 전부터 공지 날림.
                if (0 < nRemainSecond && nRemainSecond <= 300 && !(m_cFlags & SENDED_BILLING_TIMEOUT_NOTIFY))
                {
                    // 플래그 세팅(두번 이 루틴으로 못 들어오게 하기 위해서..
                    m_cFlags |= SENDED_BILLING_TIMEOUT_NOTIFY;

                    // 클라이언트에 매 분마다 공지 날리라고 패킷 보내기.                    
                    SendPacket::BillingNotify(SessionData,
                        static_cast<unsigned char>((nRemainSecond + 30)/60), m_cBillingType);
                }
                else if (nRemainSecond <= 0)
                {
                    DETLOG2(g_Log, "UID:%10u/CID:%10u/정량제 사용 유저 접속 해제 : 시간 만료", 
                        SessionData.GetUID(), SessionData.GetCID());

                    return false;
                }
            }
            break;

        case 'D':

            // 정액제. m_nPlayTime은 만료까지 남은 일 수
            {   
                const struct tm CurrentTime = *localtime(&nCurrentTime);
                const struct tm LoginTime   = *localtime(&m_nLoginTime);

                // 하루 남았고, 현재 시간이 정각에서 5분 전이면, 매 분마다 공지를 날리라고 패킷을 보낸다.
                if(LoginTime.tm_yday + m_nPlayTime - 1 == CurrentTime.tm_yday && 
                    23 == CurrentTime.tm_hour && 55 <= CurrentTime.tm_min &&
                    !(m_cFlags & SENDED_BILLING_TIMEOUT_NOTIFY))
                {       
                    // 플래그 세팅(두번 이 루틴으로 못 들어오게 하기 위해서..
                    m_cFlags |= SENDED_BILLING_TIMEOUT_NOTIFY;

                    // 클라이언트에 매 분마다 공지 날리라고 패킷 보내기.
                    SendPacket::BillingNotify(SessionData,
                        static_cast<unsigned char>(60 - CurrentTime.tm_min), m_cBillingType);
                }
                else if(LoginTime.tm_yday + static_cast<long>(m_nPlayTime) <= CurrentTime.tm_yday)
                {
                    DETLOG2(g_Log, "UID:%10u/CID:%10u/정액제 사용 유저 접속 해제 : 시간 만료", 
                        SessionData.GetUID(), SessionData.GetCID());

                    return false;
                }
            }
            break;

        case 'Y':

            // 대만 포인트. m_nPlayTime은 분단위
            {
                // 추가시간 1분 더 줌.
                int nRemainSecond = int(m_nPlayTime * 60 + m_nLoginTime - nCurrentTime) + 60;

                // 포인트 종료 1분후 데이터 체킹
                if (0 < nRemainSecond && nRemainSecond <= 60 && !(m_cFlags & SENDED_BILLING_TIMEOUT_NOTIFY))
                {
                    // 플래그 세팅(두번 이 루틴으로 못 들어오게 하기 위해서..
                    m_cFlags |= SENDED_BILLING_TIMEOUT_NOTIFY;
                    
                    // 키퍼에 다시 알아보기
                    DETLOG5(g_Log, "UID:%10u/CID:%10u/대만 포인트 체크합니다. 남은 초:%d, 로그인:%d 현재:%d",
                        SessionData.GetUID(), SessionData.GetCID(), nRemainSecond, m_nLoginTime, nCurrentTime); // 임시

                    // 클라이언트에 매 분마다 공지 날리라고 패킷 보내기.
                    unsigned char cRemainMinute = static_cast<unsigned char>((nRemainSecond + 30) / 60);
                    
                    if(WARN_BEFORE_1SEC == m_eWarnMsgType)
                    {
                        // 중국 이외의 나라에만 접속끊기전 1분전에 경고메시지 보낸다.
                        SendPacket::BillingCheckNotify(
                            SessionData, cRemainMinute, m_cBillingType);
                    }
                        
                    // UID서버쪽으로 포인트 확인 패킷 날리기
                    SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableBillingCheck,
                        SessionData.GetAccountName(), NULL, SessionData.GetUID(), SessionData.GetCID(),
                        SessionData.GetSessionID(), SessionData.GetServerID(), SessionData.GetRemoteAddress());
                }
                else if (nRemainSecond <= 0)
                {
                    DETLOG6(g_Log, "UID:%10u/CID:%10u/대만 포인트 사용 유저 접속 해제 : 시간 만료 "
                        "남은 초:%d 타임:%d, 로그인:%d 현재시간:%d", 
                        SessionData.GetUID(), SessionData.GetCID(), nRemainSecond, m_nPlayTime, m_nLoginTime, nCurrentTime);

                    return false;
                }
            }
            break;
        }
    }

    return true;
}

}
}

