#include "stdAfx.h"
#include "HanUnitedDisconnID.h"
#include "SendAgentPacket.h"

#include <DB/DBComponent.h>
#include <DB/BillingDBComponent.h>
#include <Log/ServerLog.h>


CHanUnitedDisconnectID& CHanUnitedDisconnectID::GetInstance()
{
    static CHanUnitedDisconnectID hanUnitedDisconnectID;
    return hanUnitedDisconnectID;
}

CHanUnitedDisconnectID::CHanUnitedDisconnectID()
{


}


CHanUnitedDisconnectID::~CHanUnitedDisconnectID()
{


}


void CHanUnitedDisconnectID::UpdateDisconnectInfo(const DisconnectInfo& disconnectInfo)
{
    DisconnectInfoMap::iterator pos = m_DisconnectInfoMap.find(disconnectInfo.m_nUID);

    if(pos == m_DisconnectInfoMap.end())
    {
        m_DisconnectInfoMap.insert(DisconnectInfoMap::value_type(
            disconnectInfo.m_nUID, disconnectInfo));
    }
    else
    {
        pos->second = disconnectInfo;
    }
}

bool CHanUnitedDisconnectID::RemoveDisconnectInfo(int nUID)
{
    DisconnectInfoMap::iterator pos = m_DisconnectInfoMap.find(nUID);

    if(pos != m_DisconnectInfoMap.end())
    {
        m_DisconnectInfoMap.erase(pos);
        return true;
    }

    return false;
}

CHanUnitedDisconnectID::DisconnectInfo* CHanUnitedDisconnectID::GetDisconnectInfo(int nUID)
{
    DisconnectInfoMap::iterator pos = m_DisconnectInfoMap.find(nUID);
        
    if(pos != m_DisconnectInfoMap.end())
    {
        return &pos->second;
    }

    return 0;
}


void CHanUnitedDisconnectID::CheckDisconnect()
{
        
#pragma pack(1)

    struct HanUnitedUserData
    {
        int     m_nUID;             // 
        int     m_nServerID;        // 
        int     m_nRemainMin;       //  
        char    m_cBillingType[2];  // E : 개인정량, T : 개인정액, F : 무료사용자
        int     m_nEndTime;         // 무료, 정액 : 20030225, 정량 : RemainMin
        int     m_nCRMIndex;        // 개인은 0, PC방은 0이상. 보통 개인만 들어온다.
    };

#pragma pack()

    const int MAX_DATA = CDBSingleObject::MaxRowNum;
    HanUnitedUserData hanUnitedUserData[MAX_DATA];

    if(!DBComponent::BillingDB::USPCheckTimeoutUser(CDBSingleObject::GetInstance()))
    {
        ERRLOG1(g_Log, "과금 만료 유저를 얻어오는 데 실패했습니다. GetErrorString() :%s", 
            CDBSingleObject::GetInstance().GetErrorString());
    }
    else
    {
        int nRowNum = 0;

        while(CDBSingleObject::GetInstance().GetData(
            (void**)hanUnitedUserData, sizeof(HanUnitedUserData), MAX_DATA, &nRowNum))
        {
            if(0 == nRowNum)
            {
                break;
            }
            
            // 테이블에 UID가 들어 있는지 살핀다.
            //     1. 들어 있지 않다.
            //        남은 시간을 체크한 후, 0 < time <5 이면 과금 경고 메시지를 보내고 테이블에 삽입.
            //     2. 들어 있다.
            //        남은 시간을 체크한 후, time <= 0 이면 UserKill을 보내고 테이블에서 제거.

            // 유저가 로그아웃하면 테이블에서 제거한다.

            HanUnitedUserData* lpHanUnitedUserData      = hanUnitedUserData;
            HanUnitedUserData* lpHanUnitedUserDataEnd   = hanUnitedUserData + nRowNum;

            for(; lpHanUnitedUserData != lpHanUnitedUserDataEnd; ++lpHanUnitedUserData)
            {
                DisconnectInfo* lpDisconnectInfo = GetDisconnectInfo(lpHanUnitedUserData->m_nUID);

                if(0 == lpDisconnectInfo)
                {
                    // 남은 시간을 체크한 후, timeout을 보낸다.
                    AgentSendPacket::SendHanBTN(lpHanUnitedUserData->m_nServerID, 
                        lpHanUnitedUserData->m_nUID, lpHanUnitedUserData->m_nRemainMin, 
                        lpHanUnitedUserData->m_cBillingType[0]);

                    DETLOG5(g_Log, "UID:%u/ServerID:%u/cRemainMinute:%d/cBillingType:%c/EndTime:%d/유저에게 과금 만료 경고를 보냈습니다.",
                        lpHanUnitedUserData->m_nUID, lpHanUnitedUserData->m_nServerID,
                        lpHanUnitedUserData->m_nRemainMin, lpHanUnitedUserData->m_cBillingType[0],
                        lpHanUnitedUserData->m_nEndTime);

                    DisconnectInfo disconnInfo;

                    disconnInfo.m_nUID          = lpHanUnitedUserData->m_nUID;
                    disconnInfo.m_nServerID     = lpHanUnitedUserData->m_nServerID;
                    disconnInfo.m_nRemainMin    = lpHanUnitedUserData->m_nRemainMin;
                    disconnInfo.m_nEndTime      = lpHanUnitedUserData->m_nEndTime;
                    disconnInfo.m_nCRMIndex     = lpHanUnitedUserData->m_nCRMIndex;
                    disconnInfo.m_cBillingType  = lpHanUnitedUserData->m_cBillingType[0];

                    UpdateDisconnectInfo(disconnInfo);
                }
                else
                {
                    lpDisconnectInfo->m_nServerID       = lpHanUnitedUserData->m_nServerID;
                    lpDisconnectInfo->m_nRemainMin      = lpHanUnitedUserData->m_nRemainMin;
                    lpDisconnectInfo->m_nEndTime        = lpHanUnitedUserData->m_nEndTime;
                    lpDisconnectInfo->m_nCRMIndex       = lpHanUnitedUserData->m_nCRMIndex;
                    lpDisconnectInfo->m_cBillingType    = lpHanUnitedUserData->m_cBillingType[0];

                    if(lpDisconnectInfo->m_nRemainMin <= 0)
                    {
                        AgentSendPacket::SendUserKill(lpDisconnectInfo->m_nServerID, lpDisconnectInfo->m_nUID);

                        DETLOG5(g_Log, "UID:%u/ServerID:%u/cRemainMinute:%d/cBillingType:%c/EndTime:%d/과금이 만료되어 유저의 접속을 끊습니다.",
                            lpHanUnitedUserData->m_nUID, lpHanUnitedUserData->m_nServerID,
                            lpHanUnitedUserData->m_nRemainMin, lpHanUnitedUserData->m_cBillingType[0],
                            lpHanUnitedUserData->m_nEndTime);

                        RemoveDisconnectInfo(lpDisconnectInfo->m_nUID);
                    }
                }
            }
        }
    }
}