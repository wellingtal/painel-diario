#ifndef _RYL_DBAGENT_DB_DATA_MGR_H_
#define _RYL_DBAGENT_DB_DATA_MGR_H_


#include <map>
#include <vector>
#include <utility>
#include <mmsystem.h>
#include <Log/ServerLogDefine.h>
#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/object_pool.hpp>

#include "SessionData.h"


//forward decl.
class CDBComponent;

namespace DBAgent
{
namespace DataStorage
{
    // forward decl.
    class CSessionData;

    enum DBDataError
    {
        DATA_SUCCEEDED              = 0,    //! 성공
        DATA_SERVER_ERROR           = 1,    //! 메모리 할당 오류, 맵 삽입 오류 등
        DATA_ALREADY_LOGIN_ERROR    = 2,    //! 이미 로그인 해 있음
        DATA_ALREADY_LOGOUT_ERROR   = 3,    //! 이미 로그아웃 해 있음
        MAX_DATA_ERROR              = 4
    };

    extern const char* g_szDBDataErrorString[MAX_DATA_ERROR];

    template<typename Key, typename Data, typename Pred = std::less<Key> >
    class CDBDataMgr
    {
    public:

        typedef typename Key     KeyType;
        typedef typename Data    DataType;

        enum Const { MAX_MANAGER_NAME = 32 };

        // 첫번째 인자는 관리할 대상
        // 두번째 인자는 로그인   맵은 마지막 업데이트 시간
        //               로그아웃 맵은 삽입된 시간
        typedef std::pair<DataType*, unsigned long> DBDataPair;

        typedef std::map<KeyType, DBDataPair, Pred,
            boost::fast_pool_allocator< std::pair<KeyType, DBDataPair> > > DataMap;

        //! DB를 연결한다.
        void SetDBComponent(CDBComponent* lpDBComponent) { m_lpDBComponent = lpDBComponent; }
		CDBComponent* GetDBComponent() { return m_lpDBComponent; }

        //! 데이터가 LoginDB에 있는지, LogoutDB에 있는지 알아 온다.
        bool IsDataLoginDB(const KeyType& dataKey)  { return m_LoginMap.end() != m_LoginMap.find(dataKey); }
        bool IsDataLogoutDB(const KeyType& dataKey) { return m_LogoutMap.end() != m_LogoutMap.find(dataKey); }

        //! 세팅된 관리자 이름을 얻어온다.
        const char*     GetManagerName() const  { return m_szManagerName; }
        unsigned int    GetLoginNum() const     { return static_cast<unsigned int>(m_LoginMap.size()); }
        unsigned int    GetLogoutNum() const    { return static_cast<unsigned int>(m_LogoutMap.size()); }

        //! 데이터를 얻어오거나, 얻어온 데이터를 반환한다.
        virtual DBDataError LoadData(CSessionData& SessionData, KeyType dataKey, DataType*& lpLoadData_Out);
        virtual DBDataError UnloadData(const CSessionData& SessionData, DataType* lpUnloadData);
        
        DataType*   GetLogoutData(const KeyType& dataKey);      //! 로그아웃된 데이터를 얻어온다.
        DBDataError ReloadData(const KeyType& dataKey);         //! 데이터를 DB로부터 Reload한다.
        bool        RemoveLogoutData(const KeyType& dataKey);   //! 로그아웃된 데이터를 캐쉬로부터 삭제한다.

        //! 데이터 개수가 너무 많은 경우, 로그아웃한지 오래된 데이터를 하나씩 삭제한다.
        bool ManageUnloadedData(unsigned long dwMaxDataNum);

        //! 로그인 셋 / 로그아웃 셋에 있는 데이터를 업데이트한다.
        //! Arg1 : 업데이트할 최소개수, 
        //! Arg2 : 업데이트에 소모할 최대 시간이다.
        void UpdateDBData(unsigned long dwMinUpdateNum, unsigned long dwMaxUseTime);
        
        //! 변경된 데이터를 전부 적용한 후 제거한다.
        void Destroy()        
        {
            Destroy(m_LoginMap.begin(), m_LoginMap.end());
            Destroy(m_LogoutMap.begin(), m_LogoutMap.end());

            m_LoginMap.clear(); 
            m_LogoutMap.clear();
        }

    protected:

        CDBDataMgr() : m_lpDBComponent(0) { memset(m_szManagerName, 0, sizeof(char) * MAX_MANAGER_NAME); }
        ~CDBDataMgr() { Destroy(); }

        void SetManagerName(const char* szManagerName)
        {
            strncpy(m_szManagerName, szManagerName, MAX_MANAGER_NAME - 1);
            m_szManagerName[MAX_MANAGER_NAME - 1] = 0;
        }

        // KeyType을 로그로 남길 방법을 알 수 없기 때문에 상속받아서 처리하도록 한다.
        virtual void Log(LOG_TYPE eLogType, 
            const char* pRtn, const char* pFileName, int nLine,
            const CSessionData* lpSessionData, 
            const KeyType& dataKey, const char* szErrorMsg) = 0;

        // Data를 생성 및 삭제하는 메서드이다.
        Data* CreateData() { return new Data; }
        void  DeleteData(Data* lpDBData) { delete lpDBData; }

        /*
        Data* CreateData() { return m_DataPool.construct(); }
        void  DeleteData(Data* lpDBData) { m_DataPool.destruct(lpDBData); }
        */

    private:

        void Destroy(typename DataMap::iterator first, typename DataMap::iterator last);

        CDBComponent*               m_lpDBComponent;        //! DB와의 연결

        DataMap                     m_LoginMap;             //! 로그인 된 캐릭터 리스트
        DataMap                     m_LogoutMap;            //! 로그아웃된 캐릭터 리스트

        boost::object_pool<Data>    m_DataPool;             //! 객체 풀

        char                        m_szManagerName[MAX_MANAGER_NAME];  //! 관리 매니져의 이름.
    };

    template<typename Key, typename Data, typename Pred>
    class CGreaterLogoutTime
    {
    public:

        typedef typename CDBDataMgr<Key, Data, Pred>::DataMap::value_type DataType;

        CGreaterLogoutTime(unsigned long dwCurrentTime) : m_dwCurrentTime(dwCurrentTime) { }

        bool operator () (const DataType& lhs, const DataType& rhs)
        {
            return m_dwCurrentTime - lhs.second.second > 
                m_dwCurrentTime - rhs.second.second;
        }

    private:

        unsigned long m_dwCurrentTime;
    };

    template<typename Key, typename Data, typename Pred>
    class CGreaterUpdateTime
    {
    public:

        typedef typename CDBDataMgr<Key, Data, Pred>::DataMap::value_type DataType;

        CGreaterUpdateTime(unsigned long dwCurrentTime) : m_dwCurrentTime(dwCurrentTime) { }

        bool operator () (const DataType& lhs, const DataType& rhs)
        {
            return m_dwCurrentTime - lhs.second.first->GetLastDBUpdateTime() > 
                m_dwCurrentTime - rhs.second.first->GetLastDBUpdateTime();
        }

    private:

        unsigned long m_dwCurrentTime;
    };
    
    //! 데이터를 로드한 후 Session에 연결한다.
    template<typename Key, typename Data, typename Pred>
    DBDataError CDBDataMgr<Key, Data, Pred>::LoadData(CSessionData& SessionData, KeyType dataKey, Data*& lpLoadData_Out)
    {
        lpLoadData_Out = 0;
        Data* lpDBData = 0;

        DataMap::iterator pos;
        DataMap::iterator end;

        // 로그인 맵에 데이터가 있는지 살핀다. 있으면 로그인 실패.
        pos = m_LoginMap.find(dataKey);
        end = m_LoginMap.end();

        if(pos != end)
        {
            Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, (pos->second.first->GetKey() != dataKey) 
                ? "데이터 로드 실패 : 키 값이 다릅니다" : "데이터 로드 실패 : 이미 캐릭터가 로그인 해 있습니다");

            return DATA_ALREADY_LOGIN_ERROR;
        }
        else
        {
            // 로그아웃 맵에서 검색합니다.
            pos = m_LogoutMap.find(dataKey);
            end = m_LogoutMap.end();

            if(pos != end)
            {
                // 데이터가 있다. 로그아웃 맵에서 삭제하고, 세팅 준비를 한다.
                lpDBData = pos->second.first;
                m_LogoutMap.erase(pos);
            }
            else
            {
                // 데이터가 없다. 데이터 생성 및 로드를 한다.
                if(0 != m_lpDBComponent)
                {
                    lpDBData = CreateData();

                    if(0 != lpDBData)
                    {
                        if(!lpDBData->GetFromDB(*m_lpDBComponent, dataKey))
                        {
                            // DB에서 데이터 얻어오기 실패. GetFromDB내부에서 로그 찍음.
                            DeleteData(lpDBData);
                            lpDBData = 0;
                        }
                    }
                    else
                    {
                        Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "데이터 로드 실패 : 메모리 할당에 실패했습니다");
                    }                
                }
                else
                {
                    Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "데이터 로드 실패 : DB연결이 NULL입니다");
                }
            }
        }

        if(0 != lpDBData)
        {
            DBDataPair dataPair(lpDBData, timeGetTime());

            if(!m_LoginMap.insert(std::make_pair(dataKey, dataPair)).second)
            {
                Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, 
                    "데이터 로드 실패 : LoginMap에 데이터 삽입 실패(있을 수 없음)");

                DeleteData(lpDBData);
                lpDBData = 0;
            }
            else
            {
                Log(LOG_INFO, LOG_FFL, &SessionData, dataKey, "데이터 로드 성공");

                lpLoadData_Out = lpDBData;
                return DATA_SUCCEEDED;
            }
        }

        return DATA_SERVER_ERROR;

    }

    //! 데이터를 Session에서 분리한 후 언로드한다.
    template<typename Key, typename Data, typename Pred>
    DBDataError CDBDataMgr<Key, Data, Pred>::UnloadData(const CSessionData& SessionData, Data* lpUnloadData)
    {
        if(0 != lpUnloadData)
        {
            KeyType dataKey = lpUnloadData->GetKey();

            // 로그인 맵에서 찾아서 제거
            if (0 == m_LoginMap.erase(dataKey))
            {
                Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "데이터 언로드 이상 : 로그인 맵에 없는 데이터를 언로드합니다");
            }
            else if(0 == m_lpDBComponent || !lpUnloadData->UpdateDBData(*m_lpDBComponent))
            {
                // 데이터 업데이트 실패.
                Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "데이터 언로드 이상 : 업데이트에 실패하였습니다");
            }
            else if (m_LogoutMap.insert(
                std::make_pair(dataKey, std::make_pair(lpUnloadData, timeGetTime()))).second)
            {
                // 데이터 업데이트 성공. 맵에 쑤셔 넣고 상호참조를 해제한다.
                Log(LOG_INFO, LOG_FFL, &SessionData, dataKey, "데이터 언로드 성공");

                // 성공. 여기서 탈출
                return DATA_SUCCEEDED;
            }
            else
            {
                // 말도 안된다. 데이터가 복사될 우려가 있다.
                Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "데이터 언로드 실패 : Logout맵에 이미 데이터가 있습니다");
            }

            // 일단 덤프를 남기고 삭제하는 편이 좋다.
            if(!lpUnloadData->WriteDataToDumpFile(0))
            {
                Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "데이터 언로드 실패 : 언로드 실패 후, 데이터를 파일로 남기는 데 실패");
            }

            // 메모리 릭이 생겨도 어쩔 수 없다. 잘못 Delete하면 뻑난다;;
            // DeleteData(lpUnloadData);
        }
        else
        {
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / lpUnloadData:0x%p / %s 데이터 언로드 실패 : 인자가 이상합니다",
                SessionData.GetUID(), SessionData.GetCID(), lpUnloadData, m_szManagerName);            
        }

        return DATA_SERVER_ERROR;
    }

    //! 데이터를 DB로부터 Reload한다.
    template<typename Key, typename Data, typename Pred>
    DBDataError CDBDataMgr<Key, Data, Pred>::ReloadData(const KeyType& dataKey)
    {
        if(0 == m_lpDBComponent)
        {
            return DATA_SERVER_ERROR;
        }

        // 로그인 맵에 이미 있으면, 불러와서는 안된다.
        DataMap::iterator pos = m_LoginMap.find(dataKey);
        DataMap::iterator end = m_LoginMap.end();

        if(pos != end)
        {
            return DATA_ALREADY_LOGIN_ERROR;
        }

        // 로그아웃 맵에 있으면, DB로부터 불러 오고, 없으면 그냥 리턴한다.
        pos == m_LogoutMap.find(dataKey);
        end == m_LogoutMap.end();

        return (pos != end && !pos->second.first->GetFromDB(*m_lpDBComponent, dataKey)) 
            ? DATA_SERVER_ERROR : DATA_SUCCEEDED;
    }

    template<typename Key, typename Data, typename Pred>
    bool CDBDataMgr<Key, Data, Pred>::RemoveLogoutData(const KeyType& dataKey)
    {
        // 로그아웃 캐쉬에 데이터가 있으면, 날려버린다.
        DataMap::iterator pos = m_LogoutMap.find(dataKey);
        DataMap::iterator end = m_LogoutMap.end();

        if(pos != end)
        {
            DeleteData(pos->second.first);
            m_LogoutMap.erase(pos);

            return true;
        }

        return false;
    }

    //! 데이터 개수가 너무 많은 경우, 로그아웃한지 오래된 데이터를 하나씩 삭제한다.
    template<typename Key, typename Data, typename Pred>
    bool CDBDataMgr<Key, Data, Pred>::ManageUnloadedData(unsigned long dwMaxDataNum)
    {
        size_t nLogoutDataNum = m_LogoutMap.size();

        if(dwMaxDataNum < nLogoutDataNum)
        {
            // 시간을 보고, 제일 오래된 캐릭을 내린다.        
            typedef std::vector<std::pair<KeyType, DBDataPair>, 
                boost::pool_allocator<std::pair<KeyType, DBDataPair> > > RemoveMap;

            RemoveMap removeMap(nLogoutDataNum - dwMaxDataNum);

            std::partial_sort_copy(m_LogoutMap.begin(), m_LogoutMap.end(),
                removeMap.begin(), removeMap.end(), CGreaterLogoutTime<Key,Data,Pred>(timeGetTime()));

            RemoveMap::iterator pos = removeMap.begin();
            RemoveMap::iterator end = removeMap.end();

            for(; pos != end; ++pos)
            {
                m_LogoutMap.erase(pos->first);
                DeleteData(pos->second.first);
            }
        }

        return true;
    }

    //! 로그인 셋에 있는 데이터를, 시간 순으로 정렬해서 가장 오래된 데이터부터
    //! 업데이트한다. 인자는 업데이트에 소모할 최대 시간이다.
    template<typename Key, typename Data, typename Pred>
    void CDBDataMgr<Key, Data, Pred>::UpdateDBData(unsigned long dwMinUpdateNum, unsigned long dwMaxUseTime)
    {
        if(0 != m_lpDBComponent)
        {
            typedef std::vector<std::pair<KeyType, DBDataPair>, 
                boost::pool_allocator<std::pair<KeyType, DBDataPair> > > UpdateList;

            UpdateList updateList(m_LoginMap.size());

            unsigned long dwStartTime = timeGetTime();

            std::partial_sort_copy(m_LoginMap.begin(), m_LoginMap.end(), 
                updateList.begin(), updateList.end(), CGreaterUpdateTime<Key,Data,Pred>(dwStartTime));

            UpdateList::iterator loginPos = updateList.begin();
            UpdateList::iterator loginEnd = updateList.end();

            unsigned long dwCurrentTime = timeGetTime();

            // 끝까지 돌았거나, 최소 개수 이상 업데이트하고, 시간이 오버되었으면 그만 한다.
            for(unsigned long dwUpdateCount = 0; loginPos != loginEnd; ++loginPos)
            {
                if (dwMinUpdateNum < dwUpdateCount && 
                    dwCurrentTime < dwStartTime + dwMaxUseTime)
                {                
                    break;
                }

                Data* lpDBData = loginPos->second.first;

                if(lpDBData->IsDataChanged())
                {
                    lpDBData->UpdateDBData(*m_lpDBComponent);
                    ++dwUpdateCount;

                    dwCurrentTime = timeGetTime();
                }
            }

            // 캐쉬 DB쪽은. 대부분 업데이트 되어 있기 때문에, 
            // 최근 거부터 업데이트 해 줄 필요는 없다. 그냥 전부 한바퀴 돌아준다.
            DataMap::iterator logoutPos = m_LogoutMap.begin();
            DataMap::iterator logoutEnd = m_LogoutMap.end();

            dwCurrentTime = timeGetTime();

            for(unsigned long dwUpdateCount = 0; logoutPos != logoutEnd; ++logoutPos)
            {
                if (dwMinUpdateNum < dwUpdateCount && 
                    dwCurrentTime < dwStartTime + dwMaxUseTime)
                {                
                    break;
                }

                Data* lpDBData = logoutPos->second.first;

                if(lpDBData->IsDataChanged())
                {
                    lpDBData->UpdateDBData(*m_lpDBComponent);
                    ++dwUpdateCount;

                    dwCurrentTime = timeGetTime();
                }
            }
        }
    }

    template<typename Key, typename Data, typename Pred>
    void CDBDataMgr<Key, Data, Pred>::Destroy(typename DataMap::iterator first, typename DataMap::iterator last)
    {
        for(; first != last; ++first)
        {
            Data* lpData = first->second.first;

            if(0 == m_lpDBComponent || !lpData->UpdateDBData(*m_lpDBComponent))
            {
                // TODO : 최종 데이터 업데이트 실패. 캐릭터 기록을 file등으로 남긴다.

                Log(LOG_ERROR, LOG_FFL, 0, lpData->GetKey(), lpData->WriteDataToDumpFile(0) 
                    ?   "데이터 저장 실패 : 업데이트에 실패 후 파일로 남깁니다"
                    :   "데이터 저장 실패 : 업데이트, 파일로 남기는 데 실패했습니다");
            }

            DeleteData(lpData);
        }
    }

    template<typename Key, typename Data, typename Pred>
    Data* CDBDataMgr<Key, Data, Pred>::GetLogoutData(const KeyType& dataKey)
    {    
        DataMap::iterator logoutPos = m_LogoutMap.find(dataKey);

        Data* lpDBData = 0;

        if(logoutPos == m_LogoutMap.end())
        {
            // 로그아웃 맵에 데이터가 없는 경우
            DataMap::iterator loginPos = m_LoginMap.find(dataKey);

            if(loginPos == m_LoginMap.end())
            {
                // 로그인 맵에 들어있지 않은 경우만 데이터를 로드
                lpDBData = CreateData();
                if(0 != lpDBData)
                {
                    if (lpDBData->GetFromDB(*m_lpDBComponent, dataKey) &&
                        m_LogoutMap.insert(
                        std::make_pair(dataKey, std::make_pair(lpDBData, timeGetTime()))).second)
                    {
                        Log(LOG_INFO, LOG_FFL, 0, dataKey, "로그아웃 데이터 로드 성공");
                    }
                    else
                    {
                        DeleteData(lpDBData);
                        lpDBData = 0;
                    }
                }
            }
        }
        else
        {
            // 로그아웃 맵에 데이터가 있는 경우
            lpDBData = logoutPos->second.first;
        }

        return lpDBData;
    }




}
}

#endif