#ifndef _RYL_DBAGENT_SESSION_DATA_MGR_H_
#define _RYL_DBAGENT_SESSION_DATA_MGR_H_

#include "SessionData.h"

#include <map>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool_alloc.hpp>

//! forward decl.
class CDBComponent;

namespace DBAgent
{
namespace DataStorage
{
    class CSessionDataMgr
    {
    public:

        static CSessionDataMgr& GetInstance();

        // 세션 처음으로 열기.
        CSessionData* SessionOpen(unsigned long dwUID);

        // 열려있는 세션 찾기
        CSessionData* GetOpenSession(unsigned long dwUID);
        CSessionData* GetOpenSession(const char* szAccountName, IN_ADDR peerAddress, bool bCheckAddress);
        CSessionData* GetCharLoadedSession(unsigned long dwCID);
        
        // 세션 닫기
        void SessionClose(unsigned long dwUID, unsigned long dwSessionID);
        void SessionCloseWithLogout(unsigned long dwUID, unsigned long dwServerID);

        void SessionCloseCurrentServer(unsigned long dwServerID);

        // CharEnable / CharDisable시에 연결 / 해제되는 CID맵 생성
        bool AddCharLoadedSession(unsigned long dwCID, CSessionData& SessionData);
        bool RemoveCharLoadedSession(unsigned long dwCID);

		bool IsLoadedChar(const char* szCharName);

        unsigned int GetSessionNum()            { return static_cast<unsigned int>(m_uidMap.size()); }
        unsigned int GetCharEnabledSessionNum() { return static_cast<unsigned int>(m_cidMap.size()); }

		// Load 된 캐릭터의 인스턴스 전쟁 플래그 값들을 OFF 로 만들어준다.
		void ClearGuildWarInstanceFlag();
		void ClearRealmWarInstanceFlag();

        template<typename FnProcess>
            void Process(FnProcess fnProcess)
        {
            SessionMap::iterator pos = m_uidMap.begin();
            SessionMap::iterator end = m_uidMap.end();

            for(;pos != end; ++pos)
            {
                fnProcess(*pos->second);
            }
        }

		template<typename FnProcess>
			void ProcessRealmPoint(FnProcess fnProcess, unsigned char cType)
		{
			SessionMap::iterator pos = m_uidMap.begin();
			SessionMap::iterator end = m_uidMap.end();

			for(;pos != end; ++pos)
			{
				fnProcess(pos->second, cType);
			}
		}
		
		template<typename FnProcess>
			void ProcessRealmTime(FnProcess fnProcess, TIME* pTime)
		{
			SessionMap::iterator pos = m_uidMap.begin();
			SessionMap::iterator end = m_uidMap.end();

			for(;pos != end; ++pos)
			{
				fnProcess(pos->second, pTime);
			}
		}

    private:

		typedef std::map<CSessionData*, unsigned long> SessionPoolCheck;

        typedef boost::object_pool<CSessionData> SessionPool;

        typedef std::map<unsigned long, CSessionData*, std::less<unsigned long>,
            boost::fast_pool_allocator<std::pair<unsigned long, CSessionData*> > > SessionMap;

        void SessionCloseWithLogout(SessionMap::iterator logoutPos);

        SessionMap  m_uidMap;
        SessionMap  m_cidMap;

        //SessionPool m_SessionPool;		
    };
}
}

#endif