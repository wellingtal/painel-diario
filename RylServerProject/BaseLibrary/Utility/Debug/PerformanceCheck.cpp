#include "stdafx.h"
#include <utility>
#include <algorithm>
#include <functional>

#include "DebugUtils.h"
#include "PerformanceCheck.h"


CPerformanceCheck& CPerformanceCheck::GetInstance()
{
	static CPerformanceCheck performanceCheck;
	return performanceCheck;
}


CPerformanceCheck::CPerformanceCheck()
:   m_fnPreFix(0), m_fnPostFix(0)
{
 	ClockPerSec(m_Frequency, Frequency);
	m_fFrequency = static_cast<double>(m_Frequency.QuadPart);
}

CPerformanceCheck::~CPerformanceCheck()
{
	PerformanceCheckLock::Syncronize sync(m_TableLock);
    m_InstrumentTable.clear();
}

void CPerformanceCheck::SetUserMessageFunc(FnPrintUserMessage fnPreFix, FnPrintUserMessage fnPostFix)
{
    m_fnPreFix  = fnPreFix;
    m_fnPostFix = fnPostFix;
}

inline unsigned long perf_sdbmHash(const unsigned char *str)
{
    unsigned long hash = 0;
    int c;

	while (c = *str++) { hash = c + (hash << 6) + (hash << 16) - hash; }
    return hash;
}

void CPerformanceCheck::AddTime(const char* szfunctionName, double fEstimateTime)
{
    unsigned long dwHashedKey = perf_sdbmHash(reinterpret_cast<const unsigned char*>(szfunctionName));

	PerformanceCheckLock::Syncronize sync(m_TableLock);

    fEstimateTime /= m_fFrequency;

    std::pair<PerformanceTable::iterator, PerformanceTable::iterator> itrPair = 
        m_InstrumentTable.equal_range(dwHashedKey);

	for(;itrPair.first != itrPair.second; ++itrPair.first)
	{
		Instrument& instrument = itrPair.first->second;
        if(0 == strcmp(szfunctionName, instrument.m_szFunctionName))
		{
            // 테이블에서 찾은 경우, 값을 더한다.
			instrument.m_fProcessTime += fEstimateTime;
			if(instrument.m_fMinTime > fEstimateTime) { instrument.m_fMinTime = fEstimateTime; }
			if(instrument.m_fMaxTime < fEstimateTime) { instrument.m_fMaxTime = fEstimateTime; }
			++instrument.m_dwCalled;
			return;
		}
	}

	// 테이블에 존재하지 않는 경우, 밀어 넣는다.
    m_InstrumentTable.insert(std::make_pair(dwHashedKey, Instrument(dwHashedKey, fEstimateTime, szfunctionName)));
}


bool CPerformanceCheck::PrintAllTime(const char* fileName, bool bReset)
{
	if(m_InstrumentTable.empty())
	{
		return false;
	}

	char strLogPath[MAX_PATH];
	char strLogFileName[MAX_PATH];
	 
	DbgUtils::SetProgramName(strLogPath, MAX_PATH, 0);

	// create the directory if it doesn't exist
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(strLogPath))
	{
		if (!CreateDirectory(strLogPath, 0))
		{
			return false;
		}
	}

	// create log file name in good order
	unsigned long dwSpinCount	= 0;
	unsigned long dwMaxFileSize	= 50 * 1024 * 1024;
	
	while(true) 
	{
		_sntprintf(strLogFileName, MAX_PATH, "%s/PerformanceLog-%s-%05d.log", 
			strLogPath, fileName, dwSpinCount);
	
		HANDLE hLogFile = CreateFile(strLogFileName, GENERIC_WRITE, 
			FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

		if(INVALID_HANDLE_VALUE == hLogFile)
		{
			break;
		}
		else
		{
			unsigned long dwHighValue = 0;
			unsigned long dwFileSize = GetFileSize(hLogFile, &dwHighValue);

			CloseHandle(hLogFile);

			if(dwHighValue == 0 && dwFileSize < dwMaxFileSize)
			{
				break;
			}
		}

		++dwSpinCount;
	}

	// create log file
	PerformanceCheckLock::Syncronize sync(m_TableLock);

	FILE* logFile = fopen(strLogFileName, "at");

    if(0 == logFile)
	{
		return false;
	}

    if(0 != m_fnPreFix)
    {
        m_fnPreFix(logFile);
    }

	fprintf(logFile, 
		"    Average :    Minimum :    Maximum :        Total :         # :   Profile Name\n"
		"------------------------------------------------------------------------------------------------\n" );

	double fTotalAvgTime = 0;

	PerformanceTable::iterator pos;
	PerformanceTable::iterator end;

    for(pos = m_InstrumentTable.begin(),
		end = m_InstrumentTable.end();
		pos != end; ++pos)
    {
        const Instrument& instrument = pos->second;

		if(0 == instrument.m_szFunctionName)
        {
			continue;
		}

		const double fAvgTime = (0.0f == instrument.m_fProcessTime) ? 
            0.0f : (instrument.m_fProcessTime / instrument.m_dwCalled); // Average

		fTotalAvgTime += fAvgTime;
		
		fprintf(logFile, "   %3.6f     %3.6f     %3.6f      %8.1f    %10d    %s\n",
			fAvgTime, instrument.m_fMinTime, instrument.m_fMaxTime,
			instrument.m_fProcessTime, instrument.m_dwCalled, instrument.m_szFunctionName);
	}

	fprintf(logFile, "\n\n Total AvgTime : %3.6f\n\n\n", fTotalAvgTime);

    if(0 != m_fnPostFix)
    {
        m_fnPostFix(logFile);
    }
    
    fclose(logFile);

	if(bReset)
	{
        for(pos = m_InstrumentTable.begin(),
			end = m_InstrumentTable.end();
            pos != end; ++pos)
        {
            pos->second.Initialize();
        }
	}

	return true;
}

