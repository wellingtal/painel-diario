#include "stdafx.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "SendAgentPacket.h"
#include "HanUnitedBilling.h"
#include "HanUnitedBillingPacket.h"

#include <Log/ServerLog.h>

#include <Network/IOCP/IOCPNet.h>
#include <Network/Session/Session.h>
#include <Network/Session/CreatePolicy.h>

#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include <Utility/Setup/ServerSetup.h>
#include <DB/DBComponent.h>
#include <DB/BillingDBComponent.h>
#include <mmsystem.h>


CHanUnitedSequenceID& CHanUnitedSequenceID::GetInstance()
{
    static CHanUnitedSequenceID hanUnitedSequenceID;
    return hanUnitedSequenceID;
}

//! ---------------------------------------------------------------
//!  
//!  CHanUnitedDispatch�� Session Policy�� ��� �ִ� ���� Ŭ�����̴�.
//!  
//! ---------------------------------------------------------------
class CHanUnitedPolicy
{
public:

    CHanUnitedPolicy();   
    ~CHanUnitedPolicy();
    
    static CHanUnitedPolicy& GetInstance();
    CSessionPolicy* GetPolicy() { return m_lpSessionPolicy; }

private:

    CSessionPolicy* m_lpSessionPolicy;
};


CHanUnitedPolicy& CHanUnitedPolicy::GetInstance()
{
    static CHanUnitedPolicy hanUnitedPolicy;
    return hanUnitedPolicy;
}

CHanUnitedPolicy::CHanUnitedPolicy()
:   m_lpSessionPolicy(SessionPolicy::CreateTCPPolicy<CHanUnitedDispatch>()) 
{

}
    
CHanUnitedPolicy::~CHanUnitedPolicy()
{ 
    if(0 != m_lpSessionPolicy) 
    { 
        m_lpSessionPolicy->Release(); 
        m_lpSessionPolicy = 0;
    } 
}


//! ---------------------------------------------------------------
CHanUnitedDispatch::LoginRequestData::LoginRequestData(const PktUUT& pktUUT, 
                                                       const RE_USPCheckBilling_Login& checkBillingLogin,
                                                       unsigned long dwTimeoutSec, unsigned long dwSeqID, 
                                                       unsigned short usMsgType, unsigned char cServerGroup)
                                                       :    m_PktUUT(pktUUT), 
                                                            m_SendStamp(timeGetTime()),
                                                            m_dwTimeoutSec(dwTimeoutSec),
                                                            m_dwSeqID(dwSeqID),          
                                                            m_usMsgType(usMsgType),
                                                            m_cServerGroup(cServerGroup),                                                        m_CheckBillingLogin(checkBillingLogin)
{
    _time64(&m_SendTime);
}


//! ---------------------------------------------------------------
CHanUnitedLogout::LogoutData::LogoutData(const PktUUT& pktUUT, DWORD dwSeqID)
:   m_pktUUT(pktUUT), m_dwSendStamp(timeGetTime()), m_dwSeqID(dwSeqID)
{
    GetLocalTime(&m_systemTime);
}



//! ---------------------------------------------------------------
//!  
//!  �α׾ƿ� ���� ����Ʈ�̴�. 
//!  �ֱ������� üũ�ؼ�, Ack�� ���� ������ �������Ѵ�.
//!  
//! ---------------------------------------------------------------
CHanUnitedLogout& CHanUnitedLogout::GetInstance()
{
    static CHanUnitedLogout hanUnitedLogout;
    return hanUnitedLogout;
}

bool CHanUnitedLogout::AddLogout(const PktUUT& pktUUT)
{
    unsigned long dwSeqID = 
        CHanUnitedSequenceID::GetInstance().GetUniqueID();

    std::pair<LogoutMap::iterator, bool> result = 
        m_LogoutMap.insert(std::make_pair(dwSeqID, LogoutData(pktUUT, dwSeqID)));

    if(!result.second)
    {
        // ���� ����. ���� ������ ID�� �ֽ��ϴ�!
        SERLOG3(g_Log, "ID:%s/IP:%15s/Logout failed : insert failed(seqID:%d)",
            pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), dwSeqID);
    }
    else
    {
        // ���� ����. ������ �õ��Ѵ�.
        GET_SINGLE_DISPATCH(lpHanUnitedBilling, CHanUnitedDispatch, 
            CHanUnitedDispatch::GetDispatchTable());
        
        if(0 == lpHanUnitedBilling)
        {
            // ���� ������ ����.
            ERRLOG3(g_Log, "ID:%s/IP:%15s/Logout failed : disconnected HanUnitedBilling - Retransmission later",
                pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), dwSeqID);
        }
        else
        {
            LogoutData& logoutData = result.first->second;

            if(lpHanUnitedBilling->SendLogout(logoutData.m_pktUUT,
                logoutData.m_dwSeqID, &logoutData.m_systemTime))
            {
                return true;
            }
        }
    }

    return false;
}


bool CHanUnitedLogout::RemoveLogout(const char* szID, const char* szIP, DWORD dwSeqID)
{
    LogoutMap::iterator pos = m_LogoutMap.find(dwSeqID);
    LogoutMap::iterator end = m_LogoutMap.end();

    if(pos == end)
    {
        SERLOG3(g_Log, "ID:%s/IP:%15s/RemoveLogout failed : Unknown request(seqID:%d)", 
            szID, szIP, dwSeqID);      
    }
    else
    {
        LogoutData& logoutData = pos->second;

        if(0 != strncmp(logoutData.m_pktUUT.m_strAccount, szID, PktUUT::MaxAccountLen) 
            || logoutData.m_pktUUT.m_IPAddress.S_un.S_addr != inet_addr(szIP))
        {
            SERLOG5(g_Log, "ID:%s(%s)/IP:%15s(%15s)/RemoveLogout invalid(seqID:%d)", 
                szID, logoutData.m_pktUUT.m_strAccount, 
                szIP, inet_ntoa(logoutData.m_pktUUT.m_IPAddress), dwSeqID);      
        }
        else
        {
            DETLOG3(g_Log, "ID:%s/IP:%15s/Logout success : Remove logout success(seqID:%d)", 
                szID, szIP, dwSeqID);
        }

        m_LogoutMap.erase(pos);
        return true;
    }

    return false;
}


void CHanUnitedLogout::Retransmission()
{
    if(!m_LogoutMap.empty())
    {
        GET_SINGLE_DISPATCH(lpHanUnitedBilling, CHanUnitedDispatch, 
            CHanUnitedDispatch::GetDispatchTable());

        if(0 != lpHanUnitedBilling)
        {
            LogoutMap::iterator pos = m_LogoutMap.begin();
            LogoutMap::iterator end = m_LogoutMap.end();

            const long RETRANSMISSION_TIMEOUT = 10 * 1000;

            unsigned long dwCurrentTime = timeGetTime();

            for(;pos != end; ++pos)
            {
                LogoutData& logoutData = pos->second;

                // �������ؾ� �Ѵ�.
                if(RETRANSMISSION_TIMEOUT < dwCurrentTime - logoutData.m_dwSendStamp)
                {
                    logoutData.m_dwSendStamp = dwCurrentTime;
                    
                    // �α׾ƿ��� �������Ѵ�.
                    lpHanUnitedBilling->SendLogout(logoutData.m_pktUUT, 
                        logoutData.m_dwSeqID, &logoutData.m_systemTime);
                }
            }
        }
    }
}



//! ---------------------------------------------------------------
//!  
//!  �Ѱ��� ���պ��� ��Ŷ�� ó���ϴ� Ŭ�����̴�.
//!  
//! ---------------------------------------------------------------
CHanUnitedDispatch::CHanUnitedDispatch(CSession& Session)
:   CPacketDispatch(Session)
{

}

CHanUnitedDispatch::~CHanUnitedDispatch()
{

}


bool CHanUnitedDispatch::ParsePacket(char* const lpStream_In, unsigned long* dwStreamSize_InOut)
{
    int nStreamSize = *dwStreamSize_InOut;
    CBufferFactory& bufferFactory = GetSession().GetPolicy().GetBufferFactory();

    CBufferQueue bufferQueue;

    char* szPosition = lpStream_In;
        
    while(sizeof(unsigned short) <= nStreamSize)
    {
        unsigned short usPacketSize = ntohs(*reinterpret_cast<unsigned short*>(szPosition)) + 
            sizeof(unsigned short) + sizeof(unsigned short) + sizeof(unsigned long);

        if(nStreamSize < usPacketSize)
        {
            // ��Ʈ�� ũ�⺸�� ������ ���̰� ũ��, break;
            break;
        }

        CBuffer* lpBuffer = CREATE_BUFFER(bufferFactory, 
            std::max(usPacketSize, unsigned short(sizeof(HanUnitedBilling::GLTransStruct))));

        if(0 == lpBuffer)
        {
            ERRLOG2(g_Log, "SS:0x%p/DP:0x%p/Buffer create failed", &GetSession(), this);
            return false;
        }
        else
        {
            memcpy(lpBuffer->wr_ptr(), szPosition, usPacketSize);
            lpBuffer->wr_ptr(usPacketSize);

            bufferQueue.enqueue(lpBuffer);

            szPosition   += usPacketSize;
            nStreamSize  -= usPacketSize;
        }
    }

    *dwStreamSize_InOut = (szPosition - lpStream_In);

    BufferLock::Syncronize sync(m_BufferLock);
    m_PacketBuffer.splice(bufferQueue);
    return true;
}

bool CHanUnitedDispatch::Dispatch()
{
    CBuffer* lpBuffer = 0;

    while(0 != (lpBuffer = m_PacketBuffer.dequeue()))
    {
        bool bResult = false;

        HanUnitedBilling::GLTransStruct* lpTransStruct = 
            reinterpret_cast<HanUnitedBilling::GLTransStruct*>(lpBuffer->rd_ptr());

        // �̸� ������ ��.
        lpTransStruct->HeaderMsg = ntohs(lpTransStruct->HeaderMsg);
        lpTransStruct->DataSize  = ntohs(lpTransStruct->DataSize);
        lpTransStruct->SeqID     = ntohl(lpTransStruct->SeqID);

        switch(lpTransStruct->HeaderMsg)
        {
        case HanUnitedBilling::AUTH_RESULT:
            bResult = ProcessLoginResult(*lpTransStruct);
            break;

        case HanUnitedBilling::BILL_AUTH_RESULT:
            bResult = ProcessCanLoginResult(*lpTransStruct);
            break;

        case HanUnitedBilling::WARNNOTICE:
            bResult = ProcessWarnNotice(*lpTransStruct);
            break;

        case HanUnitedBilling::CUT_IP:
            bResult = ProcessCutIP(*lpTransStruct);
            break;

        case HanUnitedBilling::PING_AUTH:
            bResult = ProcessPing(*lpTransStruct);
            break;

        case HanUnitedBilling::LOGOUT_RESULT:
            bResult = ProcessLogoutResult(*lpTransStruct);
            break;

        default:
            bResult = true;
            ERRLOG2(g_Log, "MSGID:%d/SeqID:%d/Unknown packet received", 
                lpTransStruct->HeaderMsg, lpTransStruct->SeqID);
            break;
        }

        if(!bResult)
        {
            ERRLOG2(g_Log, "MSGID:%d/SeqID:%d/Packet process failed", 
                lpTransStruct->HeaderMsg, lpTransStruct->SeqID);
        }

        SAFE_RELEASE_BUFFER(lpBuffer);
    }

    return true;
}

bool CHanUnitedDispatch::ConnectToHanUnited(CIOCPNet& iocpNet)
{
    INET_Addr& hanUnitedBillingAddr = 
        CServerSetup::GetInstance().GetHanUnitedBillingAddr();
    
    return iocpNet.Connect(CHanUnitedPolicy::GetInstance().GetPolicy(), 
        hanUnitedBillingAddr.get_addr_string(), 
        hanUnitedBillingAddr.get_port_in());
}

CSingleDispatch& CHanUnitedDispatch::GetDispatchTable()
{
    static CSingleDispatch singleDispatch;
    return singleDispatch;
}


void CHanUnitedDispatch::Connected()
{
    DETLOG2(g_Log, "SS:0x%p/DP:0x%p/Connected HanUnitedBillingServer",
        &GetSession(), this);

    GetDispatchTable().SetDispatch(this);
}

void CHanUnitedDispatch::Disconnected()
{
    DETLOG2(g_Log, "SS:0x%p/DP:0x%p/Disconnected HanUnitedBillingServer",
        &GetSession(), this);

    ProcessDisconnected();

    GetDispatchTable().RemoveDispatch(this);
}

CBufferFactory& CHanUnitedDispatch::GetBufferFactory()
{
    return GetSession().GetPolicy().GetBufferFactory();
}

// �̵���� ���պ��� ������ �α����� ������.
bool CHanUnitedDispatch::SendLogin(const PktUUT& pktUUT, 
                                   const RE_USPCheckBilling_Login& checkBillingLogin, 
                                   unsigned char cServerGroup, 
                                   unsigned long dwTimeoutSec)
{
    CBuffer* lpBuffer = CREATE_BUFFER(GetBufferFactory(), sizeof(HanUnitedBilling::GLTransStruct));

    const char* szErrorReason = 0;

    unsigned long dwSeqID = CHanUnitedSequenceID::GetInstance().GetUniqueID();

    if(0 != lpBuffer)
    {
        // ���� ����
        HanUnitedBilling::GLTransStruct* lpTransStruct = 
            reinterpret_cast<HanUnitedBilling::GLTransStruct*>(lpBuffer->wr_ptr());

        memset(lpTransStruct, 0, sizeof(HanUnitedBilling::GLTransStruct));

        // ��� ����

        lpTransStruct->HeaderMsg    = htons(HanUnitedBilling::AUTH);
        lpTransStruct->SeqID        = htonl(dwSeqID);

        int nLength = 0;

        // IP[17] ����
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_IP_LEN, 
            "%s", inet_ntoa(pktUUT.m_IPAddress));

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_IP_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_IP_LEN;

        // ID[25] ����
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_ID_LEN, 
            "%s", pktUUT.m_strAccount);

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_ID_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_ID_LEN;

        lpTransStruct->DataSize = htons(nLength);

        nLength += sizeof(lpTransStruct->DataSize);
        nLength += sizeof(lpTransStruct->HeaderMsg);
        nLength += sizeof(lpTransStruct->SeqID);

        // ���� �ݱ�.
        lpBuffer->wr_ptr(nLength);

        // ���̺� ������ ����.
        std::pair<LoginRequestMap::iterator, bool> resultPair = 
            m_LoginRequestMap.insert(LoginRequestMap::value_type(dwSeqID, 
            LoginRequestData(pktUUT, checkBillingLogin, dwTimeoutSec, dwSeqID, 
            HanUnitedBilling::AUTH, cServerGroup)));

        if(resultPair.second)
        {
            if(!GetSession().SendPending(lpBuffer))
            {
                m_LoginRequestMap.erase(resultPair.first);                
                szErrorReason = "��Ʈ��ũ�� ������ �� �����ϴ�.";
            }            
        }
        else
        {            
            szErrorReason = "���̺� ���� ��û�� ���� �� �����ϴ�.";
        }
    }
    else
    {        
        szErrorReason = "�޸𸮰� �����ؼ� ���۸� �Ҵ��� �� �����ϴ�.";
    }
    
    if(0 != szErrorReason)
    {
        // ���� �α� ����
        SERLOG7(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/���պ������� ���� ��û : %s", 
            pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
            pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
            pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), szErrorReason);

        return false;
    }

    // ���� �α� ����.
    DETLOG7(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/SeqID:%u/"
        "���պ������� ���� ��û�� �����߽��ϴ�.", 
        pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
        pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
        pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), dwSeqID);

    return true;
}

// �̵���� ���պ��� ������ �α׾ƿ��� ������.
bool CHanUnitedDispatch::SendLogout(const PktUUT& pktUUT, unsigned long dwSeqID, 
                                    LPSYSTEMTIME lpSystemTime, unsigned long dwTimeoutSec)
{
    CBuffer* lpBuffer = CREATE_BUFFER(GetBufferFactory(), sizeof(HanUnitedBilling::GLTransStruct));

    const char* szErrorReason = 0;

    if(0 != lpBuffer)
    {
        // ���� ����
        HanUnitedBilling::GLTransStruct* lpTransStruct = 
            reinterpret_cast<HanUnitedBilling::GLTransStruct*>(lpBuffer->wr_ptr());

        memset(lpTransStruct, 0, sizeof(HanUnitedBilling::GLTransStruct));

        // ��� ����
        lpTransStruct->HeaderMsg    = htons(HanUnitedBilling::LOGOUT);
        lpTransStruct->SeqID        = htonl(dwSeqID);

        int nLength = 0;

        // IP[17] ����
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_IP_LEN, 
            "%s", inet_ntoa(pktUUT.m_IPAddress));

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_IP_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_IP_LEN;

        // ID[25] ����
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_ID_LEN, 
            "%s", pktUUT.m_strAccount);

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_ID_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_ID_LEN;

        // OutTime[18] ����

        SYSTEMTIME systemTime;

        if(0 == lpSystemTime)
        {
            GetLocalTime(&systemTime);
            lpSystemTime = &systemTime;
        }

        _snprintf(lpTransStruct->Data + nLength,             
            HanUnitedBilling::MAX_OUT_TIME_LEN, "%04d%02d%02d%02d%02d%02d", 
            lpSystemTime->wYear, lpSystemTime->wMonth, lpSystemTime->wDay, 
            lpSystemTime->wHour, lpSystemTime->wMinute, lpSystemTime->wSecond);

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_OUT_TIME_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_OUT_TIME_LEN;

        lpTransStruct->DataSize = htons(nLength);

        nLength += sizeof(lpTransStruct->DataSize);
        nLength += sizeof(lpTransStruct->HeaderMsg);
        nLength += sizeof(lpTransStruct->SeqID);

        // ���� �ݱ�.
        lpBuffer->wr_ptr(nLength);

        // �ܼ� �����̱� ������ ���̺� ���� �ʿ�� ����.
        if(!GetSession().SendPending(lpBuffer))
        {                             
            szErrorReason = "��Ʈ��ũ�� �����ϴ� �� �����߽��ϴ�.";
        }            
    }
    else
    {        
        szErrorReason = "�޸𸮰� �����ؼ� ���۸� �Ҵ��� �� �����ϴ�.";
    }
    
    if(0 != szErrorReason)
    {
        // ���� �α� ����
        SERLOG7(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/���պ��� �α׾ƿ� ���� : %s", 
            pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
            pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
            pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), szErrorReason);

        return false;
    }

    // ���� �α� ����.
    DETLOG6(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/"
        "���պ��� �α׾ƿ� ���ۿ� �����߽��ϴ�.", 
        pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
        pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
        pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress));

    return true;
}

// �̵���� ���պ��� ������ �α��� �� �� �ִ��� ���θ� ������.
bool CHanUnitedDispatch::SendCanLogin(const PktUUT& pktUUT, const RE_USPCheckBilling_Login& checkBillingLogin,
                                      unsigned char cServerGroup, unsigned long dwTimeoutSec)
{
    CBuffer* lpBuffer = CREATE_BUFFER(GetBufferFactory(), sizeof(HanUnitedBilling::GLTransStruct));

    const char* szErrorReason = 0;
    unsigned long dwSeqID = CHanUnitedSequenceID::GetInstance().GetUniqueID();

    if(0 != lpBuffer)
    {
        // ���� ����
        HanUnitedBilling::GLTransStruct* lpTransStruct = 
            reinterpret_cast<HanUnitedBilling::GLTransStruct*>(lpBuffer->wr_ptr());

        // ��� ����
        lpTransStruct->HeaderMsg    = htons(HanUnitedBilling::BILL_AUTH);
        lpTransStruct->SeqID        = htonl(dwSeqID);

        int nLength = 0;

        // IP[17] ����
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_IP_LEN, 
            "%s", inet_ntoa(pktUUT.m_IPAddress));

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_IP_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_IP_LEN;

        // ID[25] ����
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_ID_LEN, 
            "%s", pktUUT.m_strAccount);

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_ID_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_ID_LEN;

        lpTransStruct->DataSize = htons(nLength);

        nLength += sizeof(lpTransStruct->DataSize);
        nLength += sizeof(lpTransStruct->HeaderMsg);
        nLength += sizeof(lpTransStruct->SeqID);

        // ���� �ݱ�.
        lpBuffer->wr_ptr(nLength);

        // ���̺� ������ ����.
        std::pair<LoginRequestMap::iterator, bool> resultPair = 
            m_LoginRequestMap.insert(LoginRequestMap::value_type(dwSeqID, 
            LoginRequestData(pktUUT, checkBillingLogin, dwTimeoutSec, 
            dwSeqID, HanUnitedBilling::BILL_AUTH, cServerGroup)));

        if(resultPair.second)
        {
            if(!GetSession().SendPending(lpBuffer))
            {
                m_LoginRequestMap.erase(resultPair.first);
                szErrorReason = "��Ʈ��ũ ���ۿ� �����߽��ϴ�.";
            }
        }
        else
        {            
            szErrorReason = "���̺� ��û�� ���� �� �����ϴ�.";
        }
    }
    else
    {        
        szErrorReason = "�޸𸮰� �����ؼ� ���۸� �Ҵ��� �� �����ϴ�.";
    }
    
    if(0 != szErrorReason)
    {
        // ���� �α� ����
        SERLOG7(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/���պ��� ���ݰ��� ���� ��û : %s", 
            pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
            pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
            pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), szErrorReason);

        return false;
    }

    // ���� �α� ����.
    DETLOG7(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/SeqID:%u/"
        "���պ��� ���ݰ��� ���� ��û ����.", 
        pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
        pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
        pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), dwSeqID);

    return true;
}


// �α��� ��� ó��
bool CHanUnitedDispatch::ProcessLoginResult(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    // 1.   �޽��� ���̸� üũ�մϴ�.
    
    // 2.   �޽��� SeqID�� ���� ��û�� �ߴ��� ���̴ϴ�. ��û���� ������ ��� �α׸� ����ϴ�.
    //      ��û�� ������ ���� �� ������, �α׸� ��� ��Ŷ�� �����ϴ�.

    // 3.   �α��� ����� Ȯ���մϴ�. �α����� ����������, Ŭ���̾�Ʈ�� ������ ������,
    //      �α����� ����������, �������� üũ�� �� �Ŀ�, üũ ����� Ŭ���̾�Ʈ�� �����ϴ�.

    /*
        Data Format
        IP char[17]
        ID char[25]
        result short (1 = success, 0 = failed)
    */

    const int nExpectedDataSize = 
        HanUnitedBilling::MAX_ID_LEN + HanUnitedBilling::MAX_IP_LEN + sizeof(short);

    // ȣ�� ���� �̸� ȣ��Ʈ ������ ������ ����
    if(nExpectedDataSize != glTransStruct.DataSize)
    {
        ERRLOG2(g_Log, "Invalid auth result packet size : expected(%d), real(%d)",
            nExpectedDataSize, glTransStruct.DataSize);        
    }
    else
    {
        const char* szIP = glTransStruct.Data;
        const char* szID = glTransStruct.Data + HanUnitedBilling::MAX_IP_LEN;

        short usResult = 0;
        memcpy(&usResult, glTransStruct.Data + 
            HanUnitedBilling::MAX_IP_LEN + HanUnitedBilling::MAX_ID_LEN, sizeof(short));

        usResult = ntohs(usResult);

        LoginRequestMap::iterator pos = m_LoginRequestMap.find(glTransStruct.SeqID);
        LoginRequestMap::iterator end = m_LoginRequestMap.end();

        if(pos == end)
        {
            SERLOG3(g_Log, "ID:%s/IP:%s/RequestKey:%d/���պ��� �α��� ���� : ��û Ű�� ã�� �� �����ϴ�.", 
                szID, szIP, glTransStruct.SeqID);
        }
        else
        {
            LoginRequestData requestData = pos->second;
            m_LoginRequestMap.erase(pos);

            unsigned long   dwResult = 0;
            const char*     szErrorMessage = 0;

            if(0 == usResult)
            {
                // ���պ��� �α��� ����. �������� üũ �� ��� ����
                switch(requestData.m_CheckBillingLogin.Flag)
                {
                case 2:

                    // ���պ��� �α��� ����
                    // �������� �α��� ����

                    // �α��� ���� ��Ŷ ����. (���� ���� ���� ��Ŷ)
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 2, 0, 0, 'N', 0, 0);

                    SERLOG6(g_Log, "ID:%s/IP:%s/UID:%d/CID:%d/Account:%s/RequestKey:%d/���պ��� �α��� ���� : ���� ������ �����ϴ�.",
                        szID, szIP, requestData.m_PktUUT.m_dwUserID, 
                        requestData.m_PktUUT.m_dwCharID,
                        requestData.m_PktUUT.m_strAccount,
                        glTransStruct.SeqID);

                    break;

                case 3:

                    // ���պ��� �α��� ����
                    // �������� �α��� ����
                    // DB�� ���ν��� ȣ�� �� �α��� ���� ����

                    // DB�� ĳ���� Insert. ���� flag�� 1�̸� �ߺ��α����̹Ƿ� ���� ����.

                    // Login        : TableType(1). 
                    // CanLogin     : TableType(0)
                    if(!DBComponent::BillingDB::USPCheckBilling_InsertCurrentUser_Post(CDBSingleObject::GetInstance(), 
                        1, requestData.m_PktUUT.m_strAccount, requestData.m_PktUUT.m_dwUserID,
                        inet_ntoa(requestData.m_PktUUT.m_IPAddress), requestData.m_cServerGroup,
                        requestData.m_CheckBillingLogin.strBillingType[0], 0, &dwResult))
                    {
                        // DB ȣ�⿡�� ������ ���.
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);

                        szErrorMessage = "���պ��� �α��� ���� : DB����(USPCheckBilling_InsertCurrentUser_Post) ����";
                    }
                    else if(1 == dwResult)
                    {
                        // DB ȣ�⿡�� ����������, �ߺ��α����� ���. 
                        // (���պ��� ���� ���� ���� ���� �α��� �� ���)
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 1, 0, 0, 'N', 0, 0);

                        szErrorMessage = "���պ��� �α��� ���� : �ߺ��α���";
                    }
                    else
                    {
                        // �������� üũ ����. �α��ν�Ų��.
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 0, 
                            requestData.m_CheckBillingLogin.PlayTime, 
                            requestData.m_CheckBillingLogin.intCRMIndex1, 
                            requestData.m_CheckBillingLogin.strBillingType[0], 0, 0);

                        szErrorMessage = "�������� �α��� ����.";
                    }

                    SERLOG7(g_Log, "ID:%s/IP:%s/UID:%d/CID:%d/Account:%s/RequestKey:%d/%s", 
                        szID, szIP, 
                        requestData.m_PktUUT.m_dwUserID, 
                        requestData.m_PktUUT.m_dwCharID,
                        requestData.m_PktUUT.m_strAccount,                        
                        glTransStruct.SeqID,
                        szErrorMessage);
                    break;
                }                
            }
            else
            {
                // ���պ��� �α��� ����.

                // DB�� ĳ���� Insert. ���� Flag�� 1�̸� �ߺ��α����̹Ƿ� �α��� ����
                // Login        : TableType(1). 
                // CanLogin     : TableType(0)
                if(!DBComponent::BillingDB::USPCheckBilling_InsertCurrentUser_Post(CDBSingleObject::GetInstance(), 
                    1, requestData.m_PktUUT.m_strAccount, requestData.m_PktUUT.m_dwUserID,
                    inet_ntoa(requestData.m_PktUUT.m_IPAddress), requestData.m_cServerGroup,
                    'N', -1, &dwResult))
                {
                    // DB ȣ�⿡�� ������ ���.
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);

                    // ���պ��� �α����� ���������Ƿ� �α��� ���и� �ٽ� ����
                    if(!CHanUnitedLogout::GetInstance().AddLogout(requestData.m_PktUUT))
                    {
                        szErrorMessage = "���պ��� �α��� ���� : DB����(USPCheckBilling_InsertCurrentUser_Post) ����"
                            " �� ���պ��� �α��� ���(�α׾ƿ�) ���� ����";
                    }
                    else
                    {
                        szErrorMessage = "���պ��� �α��� ���� : DB����(USPCheckBilling_InsertCurrentUser_Post) ����";
                    }
                }
                else if(1 == dwResult)
                {
                    // DB ȣ�⿡�� ����������, �ߺ��α����� ���. 
                    // (���պ��� ���� ���� ���� ���� �α��� �� ���)
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 1, 0, 0, 'N', 0, 0);

                    // ���պ��� �α����� ���������Ƿ� �α��� ���и� �ٽ� ����
                    if(!CHanUnitedLogout::GetInstance().AddLogout(requestData.m_PktUUT))
                    {
                        szErrorMessage = "���պ��� �α��� ���� : �ߺ��α��� �� ���պ��� �α��� ���(�α׾ƿ�) ���� ����";
                    }
                    else
                    {
                        szErrorMessage = "���պ��� �α��� ���� : �ߺ��α���";
                    }                    
                }
                else
                {
                    // �α��� ����. DB�� ���ν��� ȣ�� �� �α��� ���� ����
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 0, 0, 1, 'S', 0, 0);

                    szErrorMessage = "���պ��� �α��� ����";                
                }

                SERLOG7(g_Log, "ID:%s/IP:%s/UID:%d/CID:%d/Account:%s/RequestKey:%d/%s",
                    szID, szIP, 
                    requestData.m_PktUUT.m_dwUserID, 
                    requestData.m_PktUUT.m_dwCharID,
                    requestData.m_PktUUT.m_strAccount,
                    glTransStruct.SeqID,
                    szErrorMessage);
            }
        }
    }
    
    return true;
}


// �α��� �� �� �ִ��� ���η� ��� ó��.
bool CHanUnitedDispatch::ProcessCanLoginResult(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    // 1.   �޽��� ���̸� üũ�մϴ�.
    
    // 2.   �޽��� SeqID�� ���� ��û�� �ߴ��� ���̴ϴ�. ��û���� ������ ��� �α׸� ����ϴ�.
    //      ��û�� ������ ���� �� ������, �α׸� ��� ��Ŷ�� �����ϴ�.

    // 3.   �α��� ����� Ȯ���մϴ�. �α����� ����������, Ŭ���̾�Ʈ�� ������ ������,
    //      �α����� ����������, �������� üũ�� �� �Ŀ�, üũ ����� Ŭ���̾�Ʈ�� �����ϴ�.

    /*
        Data Format
        IP char[17]
        ID char[25]
        result short (1 = success, 0 = failed)
    */

    const int nExpectedDataSize = 
        HanUnitedBilling::MAX_ID_LEN + HanUnitedBilling::MAX_IP_LEN + sizeof(short);

    // ȣ�� ���� �̸� ȣ��Ʈ ������ ������ ����
    if(nExpectedDataSize != glTransStruct.DataSize)
    {
        ERRLOG2(g_Log, "Invalid auth result packet size : expected(%d), real(%d)",
            nExpectedDataSize, glTransStruct.DataSize);        
    }
    else
    {
        const char* szIP = glTransStruct.Data;
        const char* szID = glTransStruct.Data + HanUnitedBilling::MAX_IP_LEN;

        short usResult = 0;
        memcpy(&usResult, glTransStruct.Data + 
            HanUnitedBilling::MAX_IP_LEN + HanUnitedBilling::MAX_ID_LEN, sizeof(short));

        usResult = ntohs(usResult);

        LoginRequestMap::iterator pos = m_LoginRequestMap.find(glTransStruct.SeqID);
        LoginRequestMap::iterator end = m_LoginRequestMap.end();

        if(pos == end)
        {
            SERLOG3(g_Log, "ID:%s/IP:%s/RequestKey:%d/���պ��� �α��� ���� : ��û Ű�� ã�� �� �����ϴ�.", 
                szID, szIP, glTransStruct.SeqID);
        }
        else
        {
            LoginRequestData requestData = pos->second;
            m_LoginRequestMap.erase(pos);

            unsigned long   dwResult = 0;
            const char*     szErrorMessage = 0;

            if(0 == usResult)
            {
                // ���պ��� �α��� ����. �������� üũ �� ��� ����
                switch(requestData.m_CheckBillingLogin.Flag)
                {
                case 2:

                    // ���պ��� �α��� ����
                    // �������� �α��� ����

                    // �α��� ���� ��Ŷ ����. (���� ���� ���� ��Ŷ)
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 2, 0, 0, 'N', 0, 0);

                    SERLOG6(g_Log, "ID:%s/IP:%s/UID:%d/CID:%d/Account:%s/RequestKey:%d/���պ��� �α��� ���� : ���� ������ �����ϴ�.",
                        szID, szIP, requestData.m_PktUUT.m_dwUserID, 
                        requestData.m_PktUUT.m_dwCharID,
                        requestData.m_PktUUT.m_strAccount,
                        glTransStruct.SeqID);
                    break;

                case 3:

                    // ���պ��� �α��� ����
                    // �������� �α��� ����
                    // DB�� ���ν��� ȣ�� �� �α��� ���� ����

                    // DB�� ĳ���� Insert. ���� flag�� 1�̸� �ߺ��α����̹Ƿ� ���� ����.

                    // Login        : TableType(1). 
                    // CanLogin     : TableType(0)
                    if(!DBComponent::BillingDB::USPCheckBilling_InsertCurrentUser_Post(CDBSingleObject::GetInstance(), 
                        0, requestData.m_PktUUT.m_strAccount, requestData.m_PktUUT.m_dwUserID,
                        inet_ntoa(requestData.m_PktUUT.m_IPAddress), requestData.m_cServerGroup,
                        requestData.m_CheckBillingLogin.strBillingType[0], 0, &dwResult))
                    {
                        // DB ȣ�⿡�� ������ ���.
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);

                        szErrorMessage = "���պ��� ���� �Ұ���  : DB����(USPCheckBilling_InsertCurrentUser_Post) ����";
                    }
                    else if(1 == dwResult)
                    {
                        // DB ȣ�⿡�� ����������, �ߺ��α����� ���. 
                        // (���պ��� ���� ���� ���� ���� �α��� �� ���)
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 1, 0, 0, 'N', 0, 0);

                        szErrorMessage = "���պ��� ���� �Ұ��� : �ߺ��α���";
                    }
                    else
                    {
                        // �������� üũ ����. �α��ν�Ų��.
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 0, 
                            requestData.m_CheckBillingLogin.PlayTime, 
                            requestData.m_CheckBillingLogin.intCRMIndex1, 
                            requestData.m_CheckBillingLogin.strBillingType[0], 0, 0);

                        szErrorMessage = "�������� ���� ����.";
                    }

                    SERLOG7(g_Log, "ID:%s/IP:%s/UID:%d/CID:%d/Account:%s/RequestKey:%d/%s", 
                        szID, szIP, 
                        requestData.m_PktUUT.m_dwUserID, 
                        requestData.m_PktUUT.m_dwCharID,
                        requestData.m_PktUUT.m_strAccount,                        
                        glTransStruct.SeqID,
                        szErrorMessage);

                    break;
                }                
            }
            else
            {
                // ���պ��� �α��� ����.

                // DB�� ĳ���� Insert. ���� Flag�� 1�̸� �ߺ��α����̹Ƿ� �α��� ����
                // Login        : TableType(1). 
                // CanLogin     : TableType(0)
                if(!DBComponent::BillingDB::USPCheckBilling_InsertCurrentUser_Post(CDBSingleObject::GetInstance(), 
                    0, requestData.m_PktUUT.m_strAccount, requestData.m_PktUUT.m_dwUserID,
                    inet_ntoa(requestData.m_PktUUT.m_IPAddress), requestData.m_cServerGroup,
                    'N', -1, &dwResult))
                {
                    // DB ȣ�⿡�� ������ ���.
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);

                    szErrorMessage = "���պ��� ���� �Ұ��� : DB����(USPCheckBilling_InsertCurrentUser_Post) ����";
                }
                else if(1 == dwResult)
                {
                    // DB ȣ�⿡�� ����������, �ߺ��α����� ���. 
                    // (���պ��� ���� ���� ���� ���� �α��� �� ���)
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 1, 0, 0, 'N', 0, 0);

                    szErrorMessage = "���պ��� ���� �Ұ��� : �ߺ��α���";
                }
                else
                {
                    // �α��� ����. DB�� ���ν��� ȣ�� �� �α��� ���� ����
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 0, 0, 1, 'S', 0, 0);

                    szErrorMessage = "���պ��� ���� ����";                
                }

                SERLOG7(g_Log, "ID:%s/IP:%s/UID:%d/CID:%d/Account:%s/RequestKey:%d/%s",
                    szID, szIP, 
                    requestData.m_PktUUT.m_dwUserID, 
                    requestData.m_PktUUT.m_dwCharID,
                    requestData.m_PktUUT.m_strAccount,
                    glTransStruct.SeqID,
                    szErrorMessage);
            }
        }
    }
    
    return true;
}


// ���� ��� �޽���
bool CHanUnitedDispatch::ProcessWarnNotice(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    /*
        DB�߰輭���� ��Ŷ�� �״�� ������. DB�߰迡�� ��Ŷ�� ������, 
        �ٷ� ä�� ������ relay�ϰ�, ä�� �������� ó���� �Ѵ�.

        Data Format
        IP  char[17]
        ID  char[25]
        msg char[70]
    */

    // UID -> DB�߰� -> ä�� -> Ŭ���̾�Ʈ
    
    // �Ѱ��� ���պ��� ��� �޽���

    const int nExpectedDataSize = 
        HanUnitedBilling::MAX_ID_LEN + HanUnitedBilling::MAX_IP_LEN + HanUnitedBilling::MAX_MSG_LEN;

    // ȣ�� ���� �̸� ȣ��Ʈ ������ ������ ����
    if(nExpectedDataSize != glTransStruct.DataSize)
    {
        ERRLOG2(g_Log, "Invalid auth result packet size : expected(%d), real(%d)",
            nExpectedDataSize, glTransStruct.DataSize);        
    }
    else
    {
        const char* szIP = glTransStruct.Data;
        const char* szID = glTransStruct.Data + HanUnitedBilling::MAX_IP_LEN;
        const char* szMsg = szID + HanUnitedBilling::MAX_ID_LEN;

        if(AgentSendPacket::SendHanUnitedBTN(szID, szIP, szMsg))
        {
            DETLOG3(g_Log, "ID:%s/IP:%s/���պ��� ���� ��� �޽��� ���� ���� : (%s)", 
                szID, szIP, szMsg);
        }
        else
        {
            SERLOG3(g_Log, "ID:%s/IP:%s/���պ��� ���� ��� �޽��� ���� ���� : (%s)", 
                szID, szIP, szMsg);
        }
    }

    return true;
}


// Ư�� ip����
bool CHanUnitedDispatch::ProcessCutIP(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    /*
        DB�߰輭���� ��Ŷ�� �״�� ������. DB�߰迡�� ��Ŷ�� ������,
        ID, IP�� ����� ã�� ��, UserKill�� ������.

        Data Format
        IP char[17]
        ID char[25]
    */

    // UID -> DB�߰� -> ����(UserKill)

    const int nExpectedDataSize = 
        HanUnitedBilling::MAX_ID_LEN + HanUnitedBilling::MAX_IP_LEN;

    // ȣ�� ���� �̸� ȣ��Ʈ ������ ������ ����
    if(nExpectedDataSize != glTransStruct.DataSize)
    {
        ERRLOG2(g_Log, "Invalid auth result packet size : expected(%d), real(%d)",
            nExpectedDataSize, glTransStruct.DataSize);        
    }
    else
    {
        const char* szIP = glTransStruct.Data;
        const char* szID = glTransStruct.Data + HanUnitedBilling::MAX_IP_LEN;
        
        if(AgentSendPacket::SendHanUnitedUserKill(szID, szIP))
        {
            DETLOG2(g_Log, "ID:%s/IP:%s/���պ��� ���� ���� ���� ���� ���� ����", szID, szIP);
        }
        else
        {
            SERLOG2(g_Log, "ID:%s/IP:%s/���պ��� ���� ���� ���� ���� ���� ����", szID, szIP);
        }
    }

    return true;
}


bool CHanUnitedDispatch::ProcessPing(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    CBuffer* lpBuffer = CREATE_BUFFER(GetBufferFactory(), sizeof(HanUnitedBilling::GLTransStruct));

    if(0 != lpBuffer)
    {
        // ���� ����
        HanUnitedBilling::GLTransStruct* lpTransStruct = 
            reinterpret_cast<HanUnitedBilling::GLTransStruct*>(lpBuffer->wr_ptr());

        *lpTransStruct = glTransStruct;

        lpTransStruct->HeaderMsg = htons(glTransStruct.HeaderMsg);
        lpTransStruct->DataSize  = htons(glTransStruct.DataSize);
        lpTransStruct->SeqID     = htonl(glTransStruct.SeqID);
        
        size_t nLength = lpTransStruct->DataSize;
        nLength += sizeof(lpTransStruct->DataSize);
        nLength += sizeof(lpTransStruct->HeaderMsg);
        nLength += sizeof(lpTransStruct->SeqID);

        lpBuffer->wr_ptr(nLength);

        if(GetSession().SendPending(lpBuffer))
        {
            return true;
        }
        else
        {
            DETLOG0(g_Log, "�� ��Ŷ�� �ǵ��� ������ �� �����߽��ϴ�.");
        }
    }

    return false;
}


// �α׾ƿ� Ackó��.
bool CHanUnitedDispatch::ProcessLogoutResult(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    const int nExpectedDataSize = 
        HanUnitedBilling::MAX_ID_LEN + HanUnitedBilling::MAX_IP_LEN + sizeof(short);

    // ȣ�� ���� �̸� ȣ��Ʈ ������ ������ ����
    if(nExpectedDataSize != glTransStruct.DataSize)
    {
        ERRLOG2(g_Log, "Invalid auth result packet size : expected(%d), real(%d)",
            nExpectedDataSize, glTransStruct.DataSize);        
    }
    else
    {
        const char* szIP = glTransStruct.Data;
        const char* szID = glTransStruct.Data + HanUnitedBilling::MAX_IP_LEN;

        short usResult = 0;
        memcpy(&usResult, glTransStruct.Data + 
            HanUnitedBilling::MAX_IP_LEN + HanUnitedBilling::MAX_ID_LEN, sizeof(short));

        usResult = ntohs(usResult);
        
        return CHanUnitedLogout::GetInstance().RemoveLogout(szID, szIP, glTransStruct.SeqID);
    }

    return false;
};


void CHanUnitedDispatch::ProcessTimeout()
{
    LoginRequestMap::iterator pos = m_LoginRequestMap.begin();
    LoginRequestMap::iterator end = m_LoginRequestMap.end();

    unsigned long dwCurrentTime= timeGetTime();

    for(;pos != end; )
    {
        unsigned long dwRequestKey = pos->first;
        LoginRequestData& requestData = pos->second;

        long nDiffTime = requestData.m_SendStamp + (requestData.m_dwTimeoutSec * 1000) - dwCurrentTime;

        if(0 < nDiffTime)
        {
            ++pos;
        }
        else
        {
            // �α׸� ����� �����Ѵ�.
            const int MAX_BUFFER = 1024;
            char szBuffer[MAX_BUFFER];

            _snprintf(szBuffer, MAX_BUFFER - 1, 
                "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/RequestKey:%u/Cmd:%d/CurrentTime:%u/"
                "SendStamp:%u/TimeoutSec:%u/Request timeout. Cancel login request.",

                requestData.m_PktUUT.m_dwUserID, requestData.m_PktUUT.m_dwCharID, 
                requestData.m_PktUUT.m_dwSessionID, requestData.m_PktUUT.m_dwServerID,
                requestData.m_PktUUT.m_strAccount, inet_ntoa(requestData.m_PktUUT.m_IPAddress), 
                dwRequestKey, requestData.m_usMsgType, dwCurrentTime,
                requestData.m_SendStamp, requestData.m_dwTimeoutSec * 1000);

            szBuffer[MAX_BUFFER - 1] = 0;

            SERLOG1(g_Log, "%s", szBuffer);

            if(HanUnitedBilling::AUTH == requestData.m_usMsgType 
                || HanUnitedBilling::BILL_AUTH == requestData.m_usMsgType)
            {
                // �α��� �Ұ� ó��.
                AgentSendPacket::SendUpdateUIDTable(
                    requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                    requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);
            }

            pos = m_LoginRequestMap.erase(pos);
        }
    }
}

void CHanUnitedDispatch::ProcessDisconnected()
{
    LoginRequestMap::iterator pos = m_LoginRequestMap.begin();
    LoginRequestMap::iterator end = m_LoginRequestMap.end();
    
    unsigned long dwCurrentTime= timeGetTime();

    for(;pos != end; ++pos)
    {
        unsigned long dwRequestKey = pos->first;
        LoginRequestData& requestData = pos->second;

        // �α׸� ����� �����Ѵ�.
        const int MAX_BUFFER = 1024;
        char szBuffer[MAX_BUFFER];

        _snprintf(szBuffer, MAX_BUFFER - 1, 
            "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/RequestKey:%u/Cmd:%d/CurrentTime:%u/"
            "SendStamp:%u/TimeoutSec:%u/Disconnected HanUnitedBilling. Cancel login request .",

            requestData.m_PktUUT.m_dwUserID, requestData.m_PktUUT.m_dwCharID, 
            requestData.m_PktUUT.m_dwSessionID, requestData.m_PktUUT.m_dwServerID,
            requestData.m_PktUUT.m_strAccount, inet_ntoa(requestData.m_PktUUT.m_IPAddress), 
            dwRequestKey, requestData.m_usMsgType, dwCurrentTime,
            requestData.m_SendStamp, requestData.m_dwTimeoutSec * 1000);

        szBuffer[MAX_BUFFER - 1] = 0;

        SERLOG1(g_Log, "%s", szBuffer);

        if(HanUnitedBilling::AUTH == requestData.m_usMsgType 
            || HanUnitedBilling::BILL_AUTH == requestData.m_usMsgType)
        {
            // �α��� �Ұ� ó��.
            AgentSendPacket::SendUpdateUIDTable(
                requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);
        }
    }

    m_LoginRequestMap.clear();
}
