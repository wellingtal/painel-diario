#include "stdafx.h"
#include "CheckPing.h"
#include <Utility/Setup/ServerSetup.h>

CCheckPing::CCheckPing()
:   m_dwLastPingRecvTime(0), m_dwPingCount(0), m_dwFirstCheckTime(0)
{

}
  

bool CCheckPing::CheckPing(unsigned long dwCurrentTime)
{
	// ������� ���� �� üũ�� ���� ���� �� �ִ�.
    if (false == CServerSetup::GetInstance().GetPingCheck())
    {
        return true;
    }
    
	// ���� �� �м�
    CheckPingLock::Syncronize sync(m_PingLock);

    if (0 == m_dwLastPingRecvTime && 0 == m_dwPingCount)
    {
        if (0 == m_dwFirstCheckTime)
        {
            m_dwFirstCheckTime = dwCurrentTime;

            if (0 == m_dwFirstCheckTime)
            {
                ++m_dwFirstCheckTime;
            }
        }
        else
        {
            // 10�� * 30. ������ 300��(5��) ���� �� ��Ŷ�� ���� ������ ����.

            LONG nFirstDifference = dwCurrentTime - m_dwFirstCheckTime;
            if (PING_INTERVAL * 30 < nFirstDifference)
            {
                return false;
            }
        }

        return true;
    }

    LONG nDifference = dwCurrentTime - m_dwLastPingRecvTime;
    return (nDifference < PING_INTERVAL * 60) ? true : false;
}

void CCheckPing::SetLastPingRecvTime(unsigned long dwCurrentTime)
{
    CheckPingLock::Syncronize sync(m_PingLock);

    m_dwLastPingRecvTime = dwCurrentTime;
    ++m_dwPingCount;

    if (0 == m_dwPingCount) 
    {
        ++m_dwPingCount;
    }
}

unsigned long CCheckPing::GetLastPingRecvTime()
{
    CheckPingLock::Syncronize sync(m_PingLock);
    return m_dwLastPingRecvTime;
}

unsigned long CCheckPing::GetPingCount()
{    
    CheckPingLock::Syncronize sync(m_PingLock);
    return m_dwPingCount;
}


void CCheckPing::GetPingData(unsigned long& dwPingCount, 
                             unsigned long& dwLastPingRecvTime, 
                             unsigned long& dwFirstCheckTime)
{
    CheckPingLock::Syncronize sync(m_PingLock);

    dwPingCount         = m_dwPingCount;
    dwLastPingRecvTime  = m_dwLastPingRecvTime;
    dwFirstCheckTime    = m_dwFirstCheckTime;    
}
