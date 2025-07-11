#ifndef _PARSE_LOG_H_
#define _PARSE_LOG_H_

#include <vector>

// ���� ����
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

    // ������ ���� �ε��� ����, ������ �м��ؼ� �ִ´�. ���н� ���� �α׸� ����Ѵ�.
    bool LoadFile(const char* szFileName);
    void Destroy();

    const LogPtrArray& GetLogPtrArray() const { return m_LogPtrArray; }

    // Process. �Լ��ڸ� ������ �޴� �Ϳ� �����Ѵ�.
    template<typename FnProcess>
        void Process(FnProcess fnProcess)
    {
        std::for_each(m_LogPtrArray.begin(), m_LogPtrArray.end(), fnProcess);
    }
    
    // �˻�(TotalSet���� Result����)
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

    // �˻�(Source���� �̾Ƴ��� Result�� ����)
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