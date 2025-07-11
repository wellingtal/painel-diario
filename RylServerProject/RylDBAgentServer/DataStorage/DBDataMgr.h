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
        DATA_SUCCEEDED              = 0,    //! ����
        DATA_SERVER_ERROR           = 1,    //! �޸� �Ҵ� ����, �� ���� ���� ��
        DATA_ALREADY_LOGIN_ERROR    = 2,    //! �̹� �α��� �� ����
        DATA_ALREADY_LOGOUT_ERROR   = 3,    //! �̹� �α׾ƿ� �� ����
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

        // ù��° ���ڴ� ������ ���
        // �ι�° ���ڴ� �α���   ���� ������ ������Ʈ �ð�
        //               �α׾ƿ� ���� ���Ե� �ð�
        typedef std::pair<DataType*, unsigned long> DBDataPair;

        typedef std::map<KeyType, DBDataPair, Pred,
            boost::fast_pool_allocator< std::pair<KeyType, DBDataPair> > > DataMap;

        //! DB�� �����Ѵ�.
        void SetDBComponent(CDBComponent* lpDBComponent) { m_lpDBComponent = lpDBComponent; }
		CDBComponent* GetDBComponent() { return m_lpDBComponent; }

        //! �����Ͱ� LoginDB�� �ִ���, LogoutDB�� �ִ��� �˾� �´�.
        bool IsDataLoginDB(const KeyType& dataKey)  { return m_LoginMap.end() != m_LoginMap.find(dataKey); }
        bool IsDataLogoutDB(const KeyType& dataKey) { return m_LogoutMap.end() != m_LogoutMap.find(dataKey); }

        //! ���õ� ������ �̸��� ���´�.
        const char*     GetManagerName() const  { return m_szManagerName; }
        unsigned int    GetLoginNum() const     { return static_cast<unsigned int>(m_LoginMap.size()); }
        unsigned int    GetLogoutNum() const    { return static_cast<unsigned int>(m_LogoutMap.size()); }

        //! �����͸� �����ų�, ���� �����͸� ��ȯ�Ѵ�.
        virtual DBDataError LoadData(CSessionData& SessionData, KeyType dataKey, DataType*& lpLoadData_Out);
        virtual DBDataError UnloadData(const CSessionData& SessionData, DataType* lpUnloadData);
        
        DataType*   GetLogoutData(const KeyType& dataKey);      //! �α׾ƿ��� �����͸� ���´�.
        DBDataError ReloadData(const KeyType& dataKey);         //! �����͸� DB�κ��� Reload�Ѵ�.
        bool        RemoveLogoutData(const KeyType& dataKey);   //! �α׾ƿ��� �����͸� ĳ���κ��� �����Ѵ�.

        //! ������ ������ �ʹ� ���� ���, �α׾ƿ����� ������ �����͸� �ϳ��� �����Ѵ�.
        bool ManageUnloadedData(unsigned long dwMaxDataNum);

        //! �α��� �� / �α׾ƿ� �¿� �ִ� �����͸� ������Ʈ�Ѵ�.
        //! Arg1 : ������Ʈ�� �ּҰ���, 
        //! Arg2 : ������Ʈ�� �Ҹ��� �ִ� �ð��̴�.
        void UpdateDBData(unsigned long dwMinUpdateNum, unsigned long dwMaxUseTime);
        
        //! ����� �����͸� ���� ������ �� �����Ѵ�.
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

        // KeyType�� �α׷� ���� ����� �� �� ���� ������ ��ӹ޾Ƽ� ó���ϵ��� �Ѵ�.
        virtual void Log(LOG_TYPE eLogType, 
            const char* pRtn, const char* pFileName, int nLine,
            const CSessionData* lpSessionData, 
            const KeyType& dataKey, const char* szErrorMsg) = 0;

        // Data�� ���� �� �����ϴ� �޼����̴�.
        Data* CreateData() { return new Data; }
        void  DeleteData(Data* lpDBData) { delete lpDBData; }

        /*
        Data* CreateData() { return m_DataPool.construct(); }
        void  DeleteData(Data* lpDBData) { m_DataPool.destruct(lpDBData); }
        */

    private:

        void Destroy(typename DataMap::iterator first, typename DataMap::iterator last);

        CDBComponent*               m_lpDBComponent;        //! DB���� ����

        DataMap                     m_LoginMap;             //! �α��� �� ĳ���� ����Ʈ
        DataMap                     m_LogoutMap;            //! �α׾ƿ��� ĳ���� ����Ʈ

        boost::object_pool<Data>    m_DataPool;             //! ��ü Ǯ

        char                        m_szManagerName[MAX_MANAGER_NAME];  //! ���� �Ŵ����� �̸�.
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
    
    //! �����͸� �ε��� �� Session�� �����Ѵ�.
    template<typename Key, typename Data, typename Pred>
    DBDataError CDBDataMgr<Key, Data, Pred>::LoadData(CSessionData& SessionData, KeyType dataKey, Data*& lpLoadData_Out)
    {
        lpLoadData_Out = 0;
        Data* lpDBData = 0;

        DataMap::iterator pos;
        DataMap::iterator end;

        // �α��� �ʿ� �����Ͱ� �ִ��� ���ɴ�. ������ �α��� ����.
        pos = m_LoginMap.find(dataKey);
        end = m_LoginMap.end();

        if(pos != end)
        {
            Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, (pos->second.first->GetKey() != dataKey) 
                ? "������ �ε� ���� : Ű ���� �ٸ��ϴ�" : "������ �ε� ���� : �̹� ĳ���Ͱ� �α��� �� �ֽ��ϴ�");

            return DATA_ALREADY_LOGIN_ERROR;
        }
        else
        {
            // �α׾ƿ� �ʿ��� �˻��մϴ�.
            pos = m_LogoutMap.find(dataKey);
            end = m_LogoutMap.end();

            if(pos != end)
            {
                // �����Ͱ� �ִ�. �α׾ƿ� �ʿ��� �����ϰ�, ���� �غ� �Ѵ�.
                lpDBData = pos->second.first;
                m_LogoutMap.erase(pos);
            }
            else
            {
                // �����Ͱ� ����. ������ ���� �� �ε带 �Ѵ�.
                if(0 != m_lpDBComponent)
                {
                    lpDBData = CreateData();

                    if(0 != lpDBData)
                    {
                        if(!lpDBData->GetFromDB(*m_lpDBComponent, dataKey))
                        {
                            // DB���� ������ ������ ����. GetFromDB���ο��� �α� ����.
                            DeleteData(lpDBData);
                            lpDBData = 0;
                        }
                    }
                    else
                    {
                        Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "������ �ε� ���� : �޸� �Ҵ翡 �����߽��ϴ�");
                    }                
                }
                else
                {
                    Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "������ �ε� ���� : DB������ NULL�Դϴ�");
                }
            }
        }

        if(0 != lpDBData)
        {
            DBDataPair dataPair(lpDBData, timeGetTime());

            if(!m_LoginMap.insert(std::make_pair(dataKey, dataPair)).second)
            {
                Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, 
                    "������ �ε� ���� : LoginMap�� ������ ���� ����(���� �� ����)");

                DeleteData(lpDBData);
                lpDBData = 0;
            }
            else
            {
                Log(LOG_INFO, LOG_FFL, &SessionData, dataKey, "������ �ε� ����");

                lpLoadData_Out = lpDBData;
                return DATA_SUCCEEDED;
            }
        }

        return DATA_SERVER_ERROR;

    }

    //! �����͸� Session���� �и��� �� ��ε��Ѵ�.
    template<typename Key, typename Data, typename Pred>
    DBDataError CDBDataMgr<Key, Data, Pred>::UnloadData(const CSessionData& SessionData, Data* lpUnloadData)
    {
        if(0 != lpUnloadData)
        {
            KeyType dataKey = lpUnloadData->GetKey();

            // �α��� �ʿ��� ã�Ƽ� ����
            if (0 == m_LoginMap.erase(dataKey))
            {
                Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "������ ��ε� �̻� : �α��� �ʿ� ���� �����͸� ��ε��մϴ�");
            }
            else if(0 == m_lpDBComponent || !lpUnloadData->UpdateDBData(*m_lpDBComponent))
            {
                // ������ ������Ʈ ����.
                Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "������ ��ε� �̻� : ������Ʈ�� �����Ͽ����ϴ�");
            }
            else if (m_LogoutMap.insert(
                std::make_pair(dataKey, std::make_pair(lpUnloadData, timeGetTime()))).second)
            {
                // ������ ������Ʈ ����. �ʿ� ���� �ְ� ��ȣ������ �����Ѵ�.
                Log(LOG_INFO, LOG_FFL, &SessionData, dataKey, "������ ��ε� ����");

                // ����. ���⼭ Ż��
                return DATA_SUCCEEDED;
            }
            else
            {
                // ���� �ȵȴ�. �����Ͱ� ����� ����� �ִ�.
                Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "������ ��ε� ���� : Logout�ʿ� �̹� �����Ͱ� �ֽ��ϴ�");
            }

            // �ϴ� ������ ����� �����ϴ� ���� ����.
            if(!lpUnloadData->WriteDataToDumpFile(0))
            {
                Log(LOG_ERROR, LOG_FFL, &SessionData, dataKey, "������ ��ε� ���� : ��ε� ���� ��, �����͸� ���Ϸ� ����� �� ����");
            }

            // �޸� ���� ���ܵ� ��¿ �� ����. �߸� Delete�ϸ� ������;;
            // DeleteData(lpUnloadData);
        }
        else
        {
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / lpUnloadData:0x%p / %s ������ ��ε� ���� : ���ڰ� �̻��մϴ�",
                SessionData.GetUID(), SessionData.GetCID(), lpUnloadData, m_szManagerName);            
        }

        return DATA_SERVER_ERROR;
    }

    //! �����͸� DB�κ��� Reload�Ѵ�.
    template<typename Key, typename Data, typename Pred>
    DBDataError CDBDataMgr<Key, Data, Pred>::ReloadData(const KeyType& dataKey)
    {
        if(0 == m_lpDBComponent)
        {
            return DATA_SERVER_ERROR;
        }

        // �α��� �ʿ� �̹� ������, �ҷ��ͼ��� �ȵȴ�.
        DataMap::iterator pos = m_LoginMap.find(dataKey);
        DataMap::iterator end = m_LoginMap.end();

        if(pos != end)
        {
            return DATA_ALREADY_LOGIN_ERROR;
        }

        // �α׾ƿ� �ʿ� ������, DB�κ��� �ҷ� ����, ������ �׳� �����Ѵ�.
        pos == m_LogoutMap.find(dataKey);
        end == m_LogoutMap.end();

        return (pos != end && !pos->second.first->GetFromDB(*m_lpDBComponent, dataKey)) 
            ? DATA_SERVER_ERROR : DATA_SUCCEEDED;
    }

    template<typename Key, typename Data, typename Pred>
    bool CDBDataMgr<Key, Data, Pred>::RemoveLogoutData(const KeyType& dataKey)
    {
        // �α׾ƿ� ĳ���� �����Ͱ� ������, ����������.
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

    //! ������ ������ �ʹ� ���� ���, �α׾ƿ����� ������ �����͸� �ϳ��� �����Ѵ�.
    template<typename Key, typename Data, typename Pred>
    bool CDBDataMgr<Key, Data, Pred>::ManageUnloadedData(unsigned long dwMaxDataNum)
    {
        size_t nLogoutDataNum = m_LogoutMap.size();

        if(dwMaxDataNum < nLogoutDataNum)
        {
            // �ð��� ����, ���� ������ ĳ���� ������.        
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

    //! �α��� �¿� �ִ� �����͸�, �ð� ������ �����ؼ� ���� ������ �����ͺ���
    //! ������Ʈ�Ѵ�. ���ڴ� ������Ʈ�� �Ҹ��� �ִ� �ð��̴�.
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

            // ������ ���Ұų�, �ּ� ���� �̻� ������Ʈ�ϰ�, �ð��� �����Ǿ����� �׸� �Ѵ�.
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

            // ĳ�� DB����. ��κ� ������Ʈ �Ǿ� �ֱ� ������, 
            // �ֱ� �ź��� ������Ʈ �� �� �ʿ�� ����. �׳� ���� �ѹ��� �����ش�.
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
                // TODO : ���� ������ ������Ʈ ����. ĳ���� ����� file������ �����.

                Log(LOG_ERROR, LOG_FFL, 0, lpData->GetKey(), lpData->WriteDataToDumpFile(0) 
                    ?   "������ ���� ���� : ������Ʈ�� ���� �� ���Ϸ� ����ϴ�"
                    :   "������ ���� ���� : ������Ʈ, ���Ϸ� ����� �� �����߽��ϴ�");
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
            // �α׾ƿ� �ʿ� �����Ͱ� ���� ���
            DataMap::iterator loginPos = m_LoginMap.find(dataKey);

            if(loginPos == m_LoginMap.end())
            {
                // �α��� �ʿ� ������� ���� ��츸 �����͸� �ε�
                lpDBData = CreateData();
                if(0 != lpDBData)
                {
                    if (lpDBData->GetFromDB(*m_lpDBComponent, dataKey) &&
                        m_LogoutMap.insert(
                        std::make_pair(dataKey, std::make_pair(lpDBData, timeGetTime()))).second)
                    {
                        Log(LOG_INFO, LOG_FFL, 0, dataKey, "�α׾ƿ� ������ �ε� ����");
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
            // �α׾ƿ� �ʿ� �����Ͱ� �ִ� ���
            lpDBData = logoutPos->second.first;
        }

        return lpDBData;
    }




}
}

#endif