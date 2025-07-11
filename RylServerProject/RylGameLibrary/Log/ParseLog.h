#ifndef _PARSE_LOG_H_
#define _PARSE_LOG_H_

#include <vector>

// 전방 참조
namespace GAMELOG
{
    struct sLogBase;
}

class CParseLog
{
public:

    typedef std::vector<GAMELOG::sLogBase*> LogPtrArray;

    CParseLog();
    ~CParseLog();

    // 파일을 열고 로드한 다음, 내용을 분석해서 넣는다. 실패시 에러 로그를 출력한다.
    bool LoadFile(const char* szFileName);
    void Destroy();

    const LogPtrArray& GetLogPtrArray() const { return m_LogPtrArray; }

    // Process. 함수자를 값으로 받는 것에 주의한다.
    template<typename FnProcess>
        void Process(FnProcess fnProcess)
    {
        std::for_each(m_LogPtrArray.begin(), m_LogPtrArray.end(), fnProcess);
    }
    
    // 검색(TotalSet에서 Result리턴)
    template<typename FnFind>
        void Find(LogPtrArray& Result, FnFind fnFind)
    {
        Result.clear();
        Result.reserve(m_LogPtrArray.size());

        for(LogPtrArray::iterator itr = m_LogPtrArray.begin();
            itr != m_LogPtrArray.end(); ++itr)
        {
            if(fnFind(*itr)) { Result.push_back(*itr); }
        }
    }

    // 검색(Source에서 뽑아내서 Result에 리턴)
    template<typename FnFind>
        static void Find(const LogPtrArray& Source, LogPtrArray& Result, FnFind fnFind)
    {
        Result.clear();
        Result.reserve(Source.size());

        for(LogPtrArray::const_iterator itr = Source.begin();
            itr != Source.end(); ++itr)
        {
            if(fnFind(*itr)) { Result.push_back(*itr); }
        }
    }

private:

    bool Parse();

	char*	        m_lpRawLog;
	unsigned long	m_dwRawLogSize;
    LogPtrArray     m_LogPtrArray;
};


#endif