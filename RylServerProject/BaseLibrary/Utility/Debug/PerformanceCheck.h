#ifndef _FUNCTION_PERFORMANCE_CHECK_H_
#define _FUNCTION_PERFORMANCE_CHECK_H_

#include <winsock2.h>
#include <windows.h>

#include <map>
#include "../../Thread/Lock.h"
#include "../../Pattern/Singleton.h"

#define _USING_REALCLOCK

#if defined(_USING_REALCLOCK) && _M_IX86 >= 500

	#define RealClock(Large_Integer_In) \
		{ __asm rdtsc __asm mov Large_Integer_In.HighPart, edx __asm mov Large_Integer_In.LowPart, eax }

	#define ClockSnapShot(Large_Integer_Out, UniqueVariableName) \
		LARGE_INTEGER SnapShot##UniqueVariableName; \
		RealClock(SnapShot##UniqueVariableName); \
		Large_Integer_Out = SnapShot##UniqueVariableName

	#define ClockPerSec(Large_Integer_Frequency_Out, UniqueVariableName) \
		LARGE_INTEGER Start##UniqueVariableName, Stop##UniqueVariableName; \
		RealClock(Start##UniqueVariableName) \
		Sleep(1000); \
		RealClock(Stop##UniqueVariableName) \
		Large_Integer_Frequency_Out.QuadPart = \
			Stop##UniqueVariableName.QuadPart - Start##UniqueVariableName.QuadPart - 57UL
#else

	#define ClockSnapShot(Large_Integer_Out, UniqueVariableName) \
		QueryPerformanceCounter(&Large_Integer_Out)

	#define ClockPerSec(Large_Integer_Frequency_Out, UniqueVariableName) \
		QueryPerformanceFrequency(&Large_Integer_Frequency_Out)

#endif



// ------------------------------------------------------------------------------------------------
// 한 함수에서 여러 개 만들 수 있음. 
// identifiler는 변수명에 포함되므로, 변수명의 명명 규칙과 동일하게 할 것.
// blockName은 반드시 문자열 리터럴로 할 것.
#define DeclareBlockTimingCheck(blockName, identifiler) \
	CPerformanceInstrument blockInstrument##identifiler(blockName);

#define BlockTimingCheckStart(identifiler) \
	blockInstrument##identifiler.Start();

#define BlockTimingCheckStop(identifiler) \
	blockInstrument##identifiler.Stop();



// ------------------------------------------------------------------------------------------------
// 한 함수에서 한개만 쓸 수 있음.

#define FunctionTimingCheck \
	CPerformanceInstrument functionInstrument(__FUNCTION__); \
	CAutoInstrument autofunctionInstrument(functionInstrument);


// 결과를 파일로 출력. 출력한 후, 정보를 전부 초기화한다.
#define GetFunctionTimingResult(fileName) \
	CPerformanceCheck::GetInstance().PrintAllTime(fileName);


// 결과를 파일로 출력. 출력한 후, 정보를 초기화하지 않는다.
#define GetFunctionTimingResultNoReset(fileName) \
    CPerformanceCheck::GetInstance().PrintAllTime(fileName, false);



class CPerformanceCheck
{
public:

	static CPerformanceCheck& GetInstance();
    
    typedef void (*FnPrintUserMessage) (FILE* fDescriptor);

    void SetUserMessageFunc(FnPrintUserMessage fnPreFix = 0, FnPrintUserMessage fnPostFix = 0);

	void AddTime(const char* szfunctionName, double fEstimateTime);
	bool PrintAllTime(const char* fileName, bool bReset = true);
	
private:

    CPerformanceCheck();
    ~CPerformanceCheck();

    // 전방 참조
    struct Instrument 
    {	
	    double m_fProcessTime;
	    double m_fMinTime;
	    double m_fMaxTime;
	    unsigned long m_dwHashKey;
	    unsigned long m_dwCalled;
	    const char* m_szFunctionName;

	    Instrument(unsigned long dwHashKey, double fProcessTime, const char* szFunctionName) 
		    :	m_dwHashKey(dwHashKey), m_fProcessTime(fProcessTime), m_dwCalled(1),
			    m_fMinTime(fProcessTime), m_fMaxTime(fProcessTime),
			    m_szFunctionName(szFunctionName) { }

	    Instrument() 
		    :	m_fProcessTime(0.0f), m_fMinTime(0.0f), m_fMaxTime(0.0f),
                m_dwCalled(1), m_dwHashKey(0),                 
			    m_szFunctionName(NULL) { }

        void Initialize()
        {
	        m_fProcessTime = m_fMinTime = m_fMaxTime = 0.0f;
	        m_dwCalled = 0;            
        }
    };
	
	typedef CCSLock			                            PerformanceCheckLock;
    typedef std::multimap<unsigned long, Instrument>    PerformanceTable;

	PerformanceCheckLock	m_TableLock;
    CACHE_PAD(PerformanceCheckLockPad, sizeof(PerformanceCheckLock));

	LARGE_INTEGER		m_Frequency;
	double				m_fFrequency;
    PerformanceTable	m_InstrumentTable;

    FnPrintUserMessage  m_fnPreFix;
    FnPrintUserMessage  m_fnPostFix;
};


class CPerformanceInstrument 
{
public:

	CPerformanceInstrument(char* szfunctionName)
		:   m_szfunctionName(szfunctionName)
    {
        m_startTime.QuadPart = 0;
	    CPerformanceCheck::GetInstance().AddTime(m_szfunctionName, 0);
    }

	CPerformanceInstrument()
		:   m_szfunctionName(0)
	{
		m_startTime.QuadPart = 0;
	}

	void SetName(char* szfunctionName) { m_szfunctionName = szfunctionName; }

    ~CPerformanceInstrument() { }

	inline void Start();
	inline void Stop();

private:

	LARGE_INTEGER	m_startTime;
	LARGE_INTEGER	m_stopTime;
	char*			m_szfunctionName;
};

inline void CPerformanceInstrument::Start() 
{
	m_stopTime.QuadPart = 0;
	ClockSnapShot(m_startTime, startTime);
}

inline void CPerformanceInstrument::Stop() 
{
	ClockSnapShot(m_stopTime, stopTime);
	if(0 == m_stopTime.QuadPart) { ++m_stopTime.QuadPart; }

	CPerformanceCheck::GetInstance().AddTime(m_szfunctionName, 
        static_cast<double>(m_stopTime.QuadPart - m_startTime.QuadPart));
}


class CAutoInstrument
{
public:
    CAutoInstrument(CPerformanceInstrument& PerformanceInstrument)
        :   m_PerformanceInstrument(PerformanceInstrument) { m_PerformanceInstrument.Start(); }
    
    ~CAutoInstrument() { m_PerformanceInstrument.Stop(); }
private:
    CPerformanceInstrument& m_PerformanceInstrument;
};



#endif