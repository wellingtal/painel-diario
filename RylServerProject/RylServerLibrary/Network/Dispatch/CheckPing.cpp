#include "stdafx.h"
#include "CheckPing.h"
#include <Utility/Setup/ServerSetup.h>

CCheckPing::CCheckPing()
:   m_dwLastPingRecvTime(0), m_dwPingCount(0), m_dwFirstCheckTime(0)
{

}
  

bool CCheckPing::CheckPing(unsigned long dwCurrentTime)
{
	// 디버깅을 위해 핑 체크를 하지 않을 수 있다.
    if (false == CServerSetup::GetInstance().GetPingCheck())
    {
        return true;
    }
    
	// 받은 핑 분석
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
            // 10초 * 30. 연결후 300초(5분) 동안 핑 패킷이 오지 않으면 실패.

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
