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
    // �α��� ������ ���õǾ���, PC���� �ƴϰ�,
    // ���� CHAR_ENABLED�����̰�,
    // �Ѱ��� ���պ����� �ƴ� ��쿡�� ���� üũ.
    if (0 != m_nLoginTime && 0 == m_dwCRMIndex1 && 
        CSessionData::SE_CHAR_ENABLED == SessionData.GetSessionState() &&
        CServerSetup::GamaUnitedBilling != CServerSetup::GetInstance().GetBillingType())
    {
        switch(m_cBillingType)
        {
        case 'T':

            // ������. m_nPlayTime�� �д���
            {
                // �߰��ð� 20�� �� ��.
                int nRemainSecond = int(m_nPlayTime * 60 + m_nLoginTime - nCurrentTime) + 20;

                // 5�� ������ ���� ����.
                if (0 < nRemainSecond && nRemainSecond <= 300 && !(m_cFlags & SENDED_BILLING_TIMEOUT_NOTIFY))
                {
                    // �÷��� ����(�ι� �� ��ƾ���� �� ������ �ϱ� ���ؼ�..
                    m_cFlags |= SENDED_BILLING_TIMEOUT_NOTIFY;

                    // Ŭ���̾�Ʈ�� �� �и��� ���� ������� ��Ŷ ������.                    
                    SendPacket::BillingNotify(SessionData,
                        static_cast<unsigned char>((nRemainSecond + 30)/60), m_cBillingType);
                }
                else if (nRemainSecond <= 0)
                {
                    DETLOG2(g_Log, "UID:%10u/CID:%10u/������ ��� ���� ���� ���� : �ð� ����", 
                        SessionData.GetUID(), SessionData.GetCID());

                    return false;
                }
            }
            break;

        case 'D':

            // ������. m_nPlayTime�� ������� ���� �� ��
            {   
                const struct tm CurrentTime = *localtime(&nCurrentTime);
                const struct tm LoginTime   = *localtime(&m_nLoginTime);

                // �Ϸ� ���Ұ�, ���� �ð��� �������� 5�� ���̸�, �� �и��� ������ ������� ��Ŷ�� ������.
                if(LoginTime.tm_yday + m_nPlayTime - 1 == CurrentTime.tm_yday && 
                    23 == CurrentTime.tm_hour && 55 <= CurrentTime.tm_min &&
                    !(m_cFlags & SENDED_BILLING_TIMEOUT_NOTIFY))
                {       
                    // �÷��� ����(�ι� �� ��ƾ���� �� ������ �ϱ� ���ؼ�..
                    m_cFlags |= SENDED_BILLING_TIMEOUT_NOTIFY;

                    // Ŭ���̾�Ʈ�� �� �и��� ���� ������� ��Ŷ ������.
                    SendPacket::BillingNotify(SessionData,
                        static_cast<unsigned char>(60 - CurrentTime.tm_min), m_cBillingType);
                }
                else if(LoginTime.tm_yday + static_cast<long>(m_nPlayTime) <= CurrentTime.tm_yday)
                {
                    DETLOG2(g_Log, "UID:%10u/CID:%10u/������ ��� ���� ���� ���� : �ð� ����", 
                        SessionData.GetUID(), SessionData.GetCID());

                    return false;
                }
            }
            break;

        case 'Y':

            // �븸 ����Ʈ. m_nPlayTime�� �д���
            {
                // �߰��ð� 1�� �� ��.
                int nRemainSecond = int(m_nPlayTime * 60 + m_nLoginTime - nCurrentTime) + 60;

                // ����Ʈ ���� 1���� ������ üŷ
                if (0 < nRemainSecond && nRemainSecond <= 60 && !(m_cFlags & SENDED_BILLING_TIMEOUT_NOTIFY))
                {
                    // �÷��� ����(�ι� �� ��ƾ���� �� ������ �ϱ� ���ؼ�..
                    m_cFlags |= SENDED_BILLING_TIMEOUT_NOTIFY;
                    
                    // Ű�ۿ� �ٽ� �˾ƺ���
                    DETLOG5(g_Log, "UID:%10u/CID:%10u/�븸 ����Ʈ üũ�մϴ�. ���� ��:%d, �α���:%d ����:%d",
                        SessionData.GetUID(), SessionData.GetCID(), nRemainSecond, m_nLoginTime, nCurrentTime); // �ӽ�

                    // Ŭ���̾�Ʈ�� �� �и��� ���� ������� ��Ŷ ������.
                    unsigned char cRemainMinute = static_cast<unsigned char>((nRemainSecond + 30) / 60);
                    
                    if(WARN_BEFORE_1SEC == m_eWarnMsgType)
                    {
                        // �߱� �̿��� ���󿡸� ���Ӳ����� 1������ ���޽��� ������.
                        SendPacket::BillingCheckNotify(
                            SessionData, cRemainMinute, m_cBillingType);
                    }
                        
                    // UID���������� ����Ʈ Ȯ�� ��Ŷ ������
                    SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableBillingCheck,
                        SessionData.GetAccountName(), NULL, SessionData.GetUID(), SessionData.GetCID(),
                        SessionData.GetSessionID(), SessionData.GetServerID(), SessionData.GetRemoteAddress());
                }
                else if (nRemainSecond <= 0)
                {
                    DETLOG6(g_Log, "UID:%10u/CID:%10u/�븸 ����Ʈ ��� ���� ���� ���� : �ð� ���� "
                        "���� ��:%d Ÿ��:%d, �α���:%d ����ð�:%d", 
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

