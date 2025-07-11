#ifndef _RYL_BASE_LIBRARY_PULSE_H_
#define _RYL_BASE_LIBRARY_PULSE_H_

#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "winmm.lib")

// Single Thread¿¸øÎ¿”.
class CPulse
{
public:

	enum PulseConst
	{
		DEFAULT_TICKS_PER_PULSE = 100
	};

	static CPulse& GetInstance();

	unsigned long CheckSleep();

    void SetTicksPerPulse(long nTicksPerPulse);

    long GetTicksPerPulse() const { return m_nTicksPerPulse; }
    long GetTicksPerSec() const   { return m_nTicksPerSec; } 

    unsigned long GetRemainTime() const;

    BOOL GetTPPOverTwoTime() const { return m_bTPPOverTwoTime; }
	unsigned long GetCurrentPulse() const { return m_dwPulse; }
	unsigned long GetLastTick() const { return m_dwLastTick; }
	
    inline bool ProcessBySecond(unsigned long dwCycleSec) const { return (0 == (m_dwPulse % (dwCycleSec * m_nTicksPerSec))); }
    inline bool ProcessByMinute(unsigned long dwCycleMin) const { return ProcessBySecond(dwCycleMin * 60); }    
    inline bool ProcessByHour(unsigned long dwCycleHour) const { return ProcessBySecond(dwCycleHour * 3600); }

private:

    CPulse();
    ~CPulse();

	unsigned long m_dwPulse;
	unsigned long m_dwLastTick;		
	unsigned long m_dwHeavyTrafficCount;
	
    long m_nTicksPerPulse;
    long m_nTicksPerSec;

	BOOL m_bTPPOverTwoTime;
};

#endif