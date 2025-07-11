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
        char    m_cBillingType[2];  // E : ��������, T : ��������, F : ��������
        int     m_nEndTime;         // ����, ���� : 20030225, ���� : RemainMin
        int     m_nCRMIndex;        // ������ 0, PC���� 0�̻�. ���� ���θ� ���´�.
    };

#pragma pack()

    const int MAX_DATA = CDBSingleObject::MaxRowNum;
    HanUnitedUserData hanUnitedUserData[MAX_DATA];

    if(!DBComponent::BillingDB::USPCheckTimeoutUser(CDBSingleObject::GetInstance()))
    {
        ERRLOG1(g_Log, "���� ���� ������ ������ �� �����߽��ϴ�. GetErrorString() :%s", 
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
            
            // ���̺� UID�� ��� �ִ��� ���ɴ�.
            //     1. ��� ���� �ʴ�.
            //        ���� �ð��� üũ�� ��, 0 < time <5 �̸� ���� ��� �޽����� ������ ���̺� ����.
            //     2. ��� �ִ�.
            //        ���� �ð��� üũ�� ��, time <= 0 �̸� UserKill�� ������ ���̺��� ����.

            // ������ �α׾ƿ��ϸ� ���̺��� �����Ѵ�.

            HanUnitedUserData* lpHanUnitedUserData      = hanUnitedUserData;
            HanUnitedUserData* lpHanUnitedUserDataEnd   = hanUnitedUserData + nRowNum;

            for(; lpHanUnitedUserData != lpHanUnitedUserDataEnd; ++lpHanUnitedUserData)
            {
                DisconnectInfo* lpDisconnectInfo = GetDisconnectInfo(lpHanUnitedUserData->m_nUID);

                if(0 == lpDisconnectInfo)
                {
                    // ���� �ð��� üũ�� ��, timeout�� ������.
                    AgentSendPacket::SendHanBTN(lpHanUnitedUserData->m_nServerID, 
                        lpHanUnitedUserData->m_nUID, lpHanUnitedUserData->m_nRemainMin, 
                        lpHanUnitedUserData->m_cBillingType[0]);

                    DETLOG5(g_Log, "UID:%u/ServerID:%u/cRemainMinute:%d/cBillingType:%c/EndTime:%d/�������� ���� ���� ��� ���½��ϴ�.",
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

                        DETLOG5(g_Log, "UID:%u/ServerID:%u/cRemainMinute:%d/cBillingType:%c/EndTime:%d/������ ����Ǿ� ������ ������ �����ϴ�.",
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