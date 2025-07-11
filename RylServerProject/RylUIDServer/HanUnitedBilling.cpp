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
//!  CHanUnitedDispatch의 Session Policy를 담고 있는 전역 클래스이다.
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
//!  로그아웃 정보 리스트이다. 
//!  주기적으로 체크해서, Ack가 오지 않으면 재전송한다.
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
        // 삽입 실패. 같은 시퀀스 ID가 있습니다!
        SERLOG3(g_Log, "ID:%s/IP:%15s/Logout failed : insert failed(seqID:%d)",
            pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), dwSeqID);
    }
    else
    {
        // 삽입 성공. 전송을 시도한다.
        GET_SINGLE_DISPATCH(lpHanUnitedBilling, CHanUnitedDispatch, 
            CHanUnitedDispatch::GetDispatchTable());
        
        if(0 == lpHanUnitedBilling)
        {
            // 연결 끊어져 있음.
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

                // 재전송해야 한다.
                if(RETRANSMISSION_TIMEOUT < dwCurrentTime - logoutData.m_dwSendStamp)
                {
                    logoutData.m_dwSendStamp = dwCurrentTime;
                    
                    // 로그아웃을 재전송한다.
                    lpHanUnitedBilling->SendLogout(logoutData.m_pktUUT, 
                        logoutData.m_dwSeqID, &logoutData.m_systemTime);
                }
            }
        }
    }
}



//! ---------------------------------------------------------------
//!  
//!  한게임 통합빌링 패킷을 처리하는 클래스이다.
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
            // 스트림 크기보다 데이터 길이가 크면, break;
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

        // 미리 컨버팅 함.
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

// 미디어웹 통합빌링 서버로 로그인을 보낸다.
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
        // 버퍼 열기
        HanUnitedBilling::GLTransStruct* lpTransStruct = 
            reinterpret_cast<HanUnitedBilling::GLTransStruct*>(lpBuffer->wr_ptr());

        memset(lpTransStruct, 0, sizeof(HanUnitedBilling::GLTransStruct));

        // 헤더 세팅

        lpTransStruct->HeaderMsg    = htons(HanUnitedBilling::AUTH);
        lpTransStruct->SeqID        = htonl(dwSeqID);

        int nLength = 0;

        // IP[17] 세팅
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_IP_LEN, 
            "%s", inet_ntoa(pktUUT.m_IPAddress));

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_IP_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_IP_LEN;

        // ID[25] 세팅
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_ID_LEN, 
            "%s", pktUUT.m_strAccount);

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_ID_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_ID_LEN;

        lpTransStruct->DataSize = htons(nLength);

        nLength += sizeof(lpTransStruct->DataSize);
        nLength += sizeof(lpTransStruct->HeaderMsg);
        nLength += sizeof(lpTransStruct->SeqID);

        // 버퍼 닫기.
        lpBuffer->wr_ptr(nLength);

        // 테이블에 데이터 삽입.
        std::pair<LoginRequestMap::iterator, bool> resultPair = 
            m_LoginRequestMap.insert(LoginRequestMap::value_type(dwSeqID, 
            LoginRequestData(pktUUT, checkBillingLogin, dwTimeoutSec, dwSeqID, 
            HanUnitedBilling::AUTH, cServerGroup)));

        if(resultPair.second)
        {
            if(!GetSession().SendPending(lpBuffer))
            {
                m_LoginRequestMap.erase(resultPair.first);                
                szErrorReason = "네트워크로 전송할 수 없습니다.";
            }            
        }
        else
        {            
            szErrorReason = "테이블에 인증 요청을 넣을 수 없습니다.";
        }
    }
    else
    {        
        szErrorReason = "메모리가 부족해서 버퍼를 할당할 수 없습니다.";
    }
    
    if(0 != szErrorReason)
    {
        // 실패 로그 남김
        SERLOG7(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/통합빌링으로 인증 요청 : %s", 
            pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
            pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
            pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), szErrorReason);

        return false;
    }

    // 성공 로그 남김.
    DETLOG7(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/SeqID:%u/"
        "통합빌링으로 인증 요청에 성공했습니다.", 
        pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
        pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
        pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), dwSeqID);

    return true;
}

// 미디어웹 통합빌링 서버로 로그아웃을 보낸다.
bool CHanUnitedDispatch::SendLogout(const PktUUT& pktUUT, unsigned long dwSeqID, 
                                    LPSYSTEMTIME lpSystemTime, unsigned long dwTimeoutSec)
{
    CBuffer* lpBuffer = CREATE_BUFFER(GetBufferFactory(), sizeof(HanUnitedBilling::GLTransStruct));

    const char* szErrorReason = 0;

    if(0 != lpBuffer)
    {
        // 버퍼 열기
        HanUnitedBilling::GLTransStruct* lpTransStruct = 
            reinterpret_cast<HanUnitedBilling::GLTransStruct*>(lpBuffer->wr_ptr());

        memset(lpTransStruct, 0, sizeof(HanUnitedBilling::GLTransStruct));

        // 헤더 세팅
        lpTransStruct->HeaderMsg    = htons(HanUnitedBilling::LOGOUT);
        lpTransStruct->SeqID        = htonl(dwSeqID);

        int nLength = 0;

        // IP[17] 세팅
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_IP_LEN, 
            "%s", inet_ntoa(pktUUT.m_IPAddress));

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_IP_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_IP_LEN;

        // ID[25] 세팅
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_ID_LEN, 
            "%s", pktUUT.m_strAccount);

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_ID_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_ID_LEN;

        // OutTime[18] 세팅

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

        // 버퍼 닫기.
        lpBuffer->wr_ptr(nLength);

        // 단순 전달이기 때문에 테이블에 넣을 필요는 없음.
        if(!GetSession().SendPending(lpBuffer))
        {                             
            szErrorReason = "네트워크로 전송하는 데 실패했습니다.";
        }            
    }
    else
    {        
        szErrorReason = "메모리가 부족해서 버퍼를 할당할 수 없습니다.";
    }
    
    if(0 != szErrorReason)
    {
        // 실패 로그 남김
        SERLOG7(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/통합빌링 로그아웃 전송 : %s", 
            pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
            pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
            pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), szErrorReason);

        return false;
    }

    // 성공 로그 남김.
    DETLOG6(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/"
        "통합빌링 로그아웃 전송에 성공했습니다.", 
        pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
        pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
        pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress));

    return true;
}

// 미디어웹 통합빌링 서버로 로그인 할 수 있는지 여부를 보낸다.
bool CHanUnitedDispatch::SendCanLogin(const PktUUT& pktUUT, const RE_USPCheckBilling_Login& checkBillingLogin,
                                      unsigned char cServerGroup, unsigned long dwTimeoutSec)
{
    CBuffer* lpBuffer = CREATE_BUFFER(GetBufferFactory(), sizeof(HanUnitedBilling::GLTransStruct));

    const char* szErrorReason = 0;
    unsigned long dwSeqID = CHanUnitedSequenceID::GetInstance().GetUniqueID();

    if(0 != lpBuffer)
    {
        // 버퍼 열기
        HanUnitedBilling::GLTransStruct* lpTransStruct = 
            reinterpret_cast<HanUnitedBilling::GLTransStruct*>(lpBuffer->wr_ptr());

        // 헤더 세팅
        lpTransStruct->HeaderMsg    = htons(HanUnitedBilling::BILL_AUTH);
        lpTransStruct->SeqID        = htonl(dwSeqID);

        int nLength = 0;

        // IP[17] 세팅
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_IP_LEN, 
            "%s", inet_ntoa(pktUUT.m_IPAddress));

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_IP_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_IP_LEN;

        // ID[25] 세팅
        _snprintf(lpTransStruct->Data + nLength, HanUnitedBilling::MAX_ID_LEN, 
            "%s", pktUUT.m_strAccount);

        lpTransStruct->Data[nLength + HanUnitedBilling::MAX_ID_LEN - 1] = 0;
        nLength += HanUnitedBilling::MAX_ID_LEN;

        lpTransStruct->DataSize = htons(nLength);

        nLength += sizeof(lpTransStruct->DataSize);
        nLength += sizeof(lpTransStruct->HeaderMsg);
        nLength += sizeof(lpTransStruct->SeqID);

        // 버퍼 닫기.
        lpBuffer->wr_ptr(nLength);

        // 테이블에 데이터 삽입.
        std::pair<LoginRequestMap::iterator, bool> resultPair = 
            m_LoginRequestMap.insert(LoginRequestMap::value_type(dwSeqID, 
            LoginRequestData(pktUUT, checkBillingLogin, dwTimeoutSec, 
            dwSeqID, HanUnitedBilling::BILL_AUTH, cServerGroup)));

        if(resultPair.second)
        {
            if(!GetSession().SendPending(lpBuffer))
            {
                m_LoginRequestMap.erase(resultPair.first);
                szErrorReason = "네트워크 전송에 실패했습니다.";
            }
        }
        else
        {            
            szErrorReason = "테이블에 요청을 넣을 수 없습니다.";
        }
    }
    else
    {        
        szErrorReason = "메모리가 부족해서 버퍼를 할당할 수 없습니다.";
    }
    
    if(0 != szErrorReason)
    {
        // 실패 로그 남김
        SERLOG7(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/통합빌링 과금가능 여부 요청 : %s", 
            pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
            pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
            pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), szErrorReason);

        return false;
    }

    // 성공 로그 남김.
    DETLOG7(g_Log, "UID:%u/CID:%u/SessionID:0x%p/ServerID:0x%p/ID:%s/IP:%s/SeqID:%u/"
        "통합빌링 과금가능 여부 요청 성공.", 
        pktUUT.m_dwUserID, pktUUT.m_dwCharID, 
        pktUUT.m_dwSessionID, pktUUT.m_dwServerID,
        pktUUT.m_strAccount, inet_ntoa(pktUUT.m_IPAddress), dwSeqID);

    return true;
}


// 로그인 결과 처리
bool CHanUnitedDispatch::ProcessLoginResult(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    // 1.   메시지 길이를 체크합니다.
    
    // 2.   메시지 SeqID로 누가 요청을 했는지 살핍니다. 요청자의 정보를 얻고 로그를 찍습니다.
    //      요청자 정보를 얻을 수 없으면, 로그를 찍고 패킷을 버립니다.

    // 3.   로그인 결과를 확인합니다. 로그인이 성공했으면, 클라이언트로 성공을 보내고,
    //      로그인이 실패했으면, 개인정량 체크를 한 후에, 체크 결과를 클라이언트로 보냅니다.

    /*
        Data Format
        IP char[17]
        ID char[25]
        result short (1 = success, 0 = failed)
    */

    const int nExpectedDataSize = 
        HanUnitedBilling::MAX_ID_LEN + HanUnitedBilling::MAX_IP_LEN + sizeof(short);

    // 호출 전에 미리 호스트 오더로 컨버팅 했음
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
            SERLOG3(g_Log, "ID:%s/IP:%s/RequestKey:%d/통합빌링 로그인 실패 : 요청 키를 찾을 수 없습니다.", 
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
                // 통합빌링 로그인 실패. 개인정량 체크 후 결과 전송
                switch(requestData.m_CheckBillingLogin.Flag)
                {
                case 2:

                    // 통합빌링 로그인 실패
                    // 개인정량 로그인 실패

                    // 로그인 실패 패킷 보냄. (과금 내역 없음 패킷)
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 2, 0, 0, 'N', 0, 0);

                    SERLOG6(g_Log, "ID:%s/IP:%s/UID:%d/CID:%d/Account:%s/RequestKey:%d/통합빌링 로그인 실패 : 과금 정보가 없습니다.",
                        szID, szIP, requestData.m_PktUUT.m_dwUserID, 
                        requestData.m_PktUUT.m_dwCharID,
                        requestData.m_PktUUT.m_strAccount,
                        glTransStruct.SeqID);

                    break;

                case 3:

                    // 통합빌링 로그인 실패
                    // 개인정량 로그인 성공
                    // DB에 프로시저 호출 후 로그인 성공 보냄

                    // DB에 캐릭터 Insert. 만일 flag가 1이면 중복로그인이므로 실패 보냄.

                    // Login        : TableType(1). 
                    // CanLogin     : TableType(0)
                    if(!DBComponent::BillingDB::USPCheckBilling_InsertCurrentUser_Post(CDBSingleObject::GetInstance(), 
                        1, requestData.m_PktUUT.m_strAccount, requestData.m_PktUUT.m_dwUserID,
                        inet_ntoa(requestData.m_PktUUT.m_IPAddress), requestData.m_cServerGroup,
                        requestData.m_CheckBillingLogin.strBillingType[0], 0, &dwResult))
                    {
                        // DB 호출에서 실패한 경우.
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);

                        szErrorMessage = "통합빌링 로그인 실패 : DB쿼리(USPCheckBilling_InsertCurrentUser_Post) 실패";
                    }
                    else if(1 == dwResult)
                    {
                        // DB 호출에서 성공했지만, 중복로그인인 경우. 
                        // (통합빌링 거쳐 오는 사이 먼저 로그인 된 경우)
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 1, 0, 0, 'N', 0, 0);

                        szErrorMessage = "통합빌링 로그인 실패 : 중복로그인";
                    }
                    else
                    {
                        // 개인정량 체크 성공. 로그인시킨다.
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 0, 
                            requestData.m_CheckBillingLogin.PlayTime, 
                            requestData.m_CheckBillingLogin.intCRMIndex1, 
                            requestData.m_CheckBillingLogin.strBillingType[0], 0, 0);

                        szErrorMessage = "개인정량 로그인 성공.";
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
                // 통합빌링 로그인 성공.

                // DB에 캐릭터 Insert. 만일 Flag가 1이면 중복로그인이므로 로그인 실패
                // Login        : TableType(1). 
                // CanLogin     : TableType(0)
                if(!DBComponent::BillingDB::USPCheckBilling_InsertCurrentUser_Post(CDBSingleObject::GetInstance(), 
                    1, requestData.m_PktUUT.m_strAccount, requestData.m_PktUUT.m_dwUserID,
                    inet_ntoa(requestData.m_PktUUT.m_IPAddress), requestData.m_cServerGroup,
                    'N', -1, &dwResult))
                {
                    // DB 호출에서 실패한 경우.
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);

                    // 통합빌링 로그인은 성공했으므로 로그인 실패를 다시 보냄
                    if(!CHanUnitedLogout::GetInstance().AddLogout(requestData.m_PktUUT))
                    {
                        szErrorMessage = "통합빌링 로그인 실패 : DB쿼리(USPCheckBilling_InsertCurrentUser_Post) 실패"
                            " 및 통합빌링 로그인 취소(로그아웃) 전송 실패";
                    }
                    else
                    {
                        szErrorMessage = "통합빌링 로그인 실패 : DB쿼리(USPCheckBilling_InsertCurrentUser_Post) 실패";
                    }
                }
                else if(1 == dwResult)
                {
                    // DB 호출에서 성공했지만, 중복로그인인 경우. 
                    // (통합빌링 거쳐 오는 사이 먼저 로그인 된 경우)
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 1, 0, 0, 'N', 0, 0);

                    // 통합빌링 로그인은 성공했으므로 로그인 실패를 다시 보냄
                    if(!CHanUnitedLogout::GetInstance().AddLogout(requestData.m_PktUUT))
                    {
                        szErrorMessage = "통합빌링 로그인 실패 : 중복로그인 및 통합빌링 로그인 취소(로그아웃) 전송 실패";
                    }
                    else
                    {
                        szErrorMessage = "통합빌링 로그인 실패 : 중복로그인";
                    }                    
                }
                else
                {
                    // 로그인 성공. DB에 프로시저 호출 후 로그인 성공 보냄
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 0, 0, 1, 'S', 0, 0);

                    szErrorMessage = "통합빌링 로그인 성공";                
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


// 로그인 할 수 있는지 여부로 결과 처리.
bool CHanUnitedDispatch::ProcessCanLoginResult(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    // 1.   메시지 길이를 체크합니다.
    
    // 2.   메시지 SeqID로 누가 요청을 했는지 살핍니다. 요청자의 정보를 얻고 로그를 찍습니다.
    //      요청자 정보를 얻을 수 없으면, 로그를 찍고 패킷을 버립니다.

    // 3.   로그인 결과를 확인합니다. 로그인이 성공했으면, 클라이언트로 성공을 보내고,
    //      로그인이 실패했으면, 개인정량 체크를 한 후에, 체크 결과를 클라이언트로 보냅니다.

    /*
        Data Format
        IP char[17]
        ID char[25]
        result short (1 = success, 0 = failed)
    */

    const int nExpectedDataSize = 
        HanUnitedBilling::MAX_ID_LEN + HanUnitedBilling::MAX_IP_LEN + sizeof(short);

    // 호출 전에 미리 호스트 오더로 컨버팅 했음
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
            SERLOG3(g_Log, "ID:%s/IP:%s/RequestKey:%d/통합빌링 로그인 실패 : 요청 키를 찾을 수 없습니다.", 
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
                // 통합빌링 로그인 실패. 개인정량 체크 후 결과 전송
                switch(requestData.m_CheckBillingLogin.Flag)
                {
                case 2:

                    // 통합빌링 로그인 실패
                    // 개인정량 로그인 실패

                    // 로그인 실패 패킷 보냄. (과금 내역 없음 패킷)
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 2, 0, 0, 'N', 0, 0);

                    SERLOG6(g_Log, "ID:%s/IP:%s/UID:%d/CID:%d/Account:%s/RequestKey:%d/통합빌링 로그인 실패 : 과금 정보가 없습니다.",
                        szID, szIP, requestData.m_PktUUT.m_dwUserID, 
                        requestData.m_PktUUT.m_dwCharID,
                        requestData.m_PktUUT.m_strAccount,
                        glTransStruct.SeqID);
                    break;

                case 3:

                    // 통합빌링 로그인 실패
                    // 개인정량 로그인 성공
                    // DB에 프로시저 호출 후 로그인 성공 보냄

                    // DB에 캐릭터 Insert. 만일 flag가 1이면 중복로그인이므로 실패 보냄.

                    // Login        : TableType(1). 
                    // CanLogin     : TableType(0)
                    if(!DBComponent::BillingDB::USPCheckBilling_InsertCurrentUser_Post(CDBSingleObject::GetInstance(), 
                        0, requestData.m_PktUUT.m_strAccount, requestData.m_PktUUT.m_dwUserID,
                        inet_ntoa(requestData.m_PktUUT.m_IPAddress), requestData.m_cServerGroup,
                        requestData.m_CheckBillingLogin.strBillingType[0], 0, &dwResult))
                    {
                        // DB 호출에서 실패한 경우.
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);

                        szErrorMessage = "통합빌링 과금 불가능  : DB쿼리(USPCheckBilling_InsertCurrentUser_Post) 실패";
                    }
                    else if(1 == dwResult)
                    {
                        // DB 호출에서 성공했지만, 중복로그인인 경우. 
                        // (통합빌링 거쳐 오는 사이 먼저 로그인 된 경우)
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 1, 0, 0, 'N', 0, 0);

                        szErrorMessage = "통합빌링 과금 불가능 : 중복로그인";
                    }
                    else
                    {
                        // 개인정량 체크 성공. 로그인시킨다.
                        AgentSendPacket::SendUpdateUIDTable(
                            requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                            requestData.m_PktUUT.m_dwUserID, 0, 
                            requestData.m_CheckBillingLogin.PlayTime, 
                            requestData.m_CheckBillingLogin.intCRMIndex1, 
                            requestData.m_CheckBillingLogin.strBillingType[0], 0, 0);

                        szErrorMessage = "개인정량 과금 가능.";
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
                // 통합빌링 로그인 성공.

                // DB에 캐릭터 Insert. 만일 Flag가 1이면 중복로그인이므로 로그인 실패
                // Login        : TableType(1). 
                // CanLogin     : TableType(0)
                if(!DBComponent::BillingDB::USPCheckBilling_InsertCurrentUser_Post(CDBSingleObject::GetInstance(), 
                    0, requestData.m_PktUUT.m_strAccount, requestData.m_PktUUT.m_dwUserID,
                    inet_ntoa(requestData.m_PktUUT.m_IPAddress), requestData.m_cServerGroup,
                    'N', -1, &dwResult))
                {
                    // DB 호출에서 실패한 경우.
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);

                    szErrorMessage = "통합빌링 과금 불가능 : DB쿼리(USPCheckBilling_InsertCurrentUser_Post) 실패";
                }
                else if(1 == dwResult)
                {
                    // DB 호출에서 성공했지만, 중복로그인인 경우. 
                    // (통합빌링 거쳐 오는 사이 먼저 로그인 된 경우)
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 1, 0, 0, 'N', 0, 0);

                    szErrorMessage = "통합빌링 과금 불가능 : 중복로그인";
                }
                else
                {
                    // 로그인 성공. DB에 프로시저 호출 후 로그인 성공 보냄
                    AgentSendPacket::SendUpdateUIDTable(
                        requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                        requestData.m_PktUUT.m_dwUserID, 0, 0, 1, 'S', 0, 0);

                    szErrorMessage = "통합빌링 과금 가능";                
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


// 과금 경고 메시지
bool CHanUnitedDispatch::ProcessWarnNotice(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    /*
        DB중계서버로 패킷을 그대로 날린다. DB중계에서 패킷을 받으면, 
        바로 채팅 서버로 relay하고, 채팅 서버에서 처리를 한다.

        Data Format
        IP  char[17]
        ID  char[25]
        msg char[70]
    */

    // UID -> DB중계 -> 채팅 -> 클라이언트
    
    // 한게임 통합빌링 경고 메시지

    const int nExpectedDataSize = 
        HanUnitedBilling::MAX_ID_LEN + HanUnitedBilling::MAX_IP_LEN + HanUnitedBilling::MAX_MSG_LEN;

    // 호출 전에 미리 호스트 오더로 컨버팅 했음
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
            DETLOG3(g_Log, "ID:%s/IP:%s/통합빌링 과금 경고 메시지 전송 성공 : (%s)", 
                szID, szIP, szMsg);
        }
        else
        {
            SERLOG3(g_Log, "ID:%s/IP:%s/통합빌링 과금 경고 메시지 전송 실패 : (%s)", 
                szID, szIP, szMsg);
        }
    }

    return true;
}


// 특정 ip끊기
bool CHanUnitedDispatch::ProcessCutIP(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    /*
        DB중계서버로 패킷을 그대로 날린다. DB중계에서 패킷을 받으면,
        ID, IP로 사람을 찾은 후, UserKill을 날린다.

        Data Format
        IP char[17]
        ID char[25]
    */

    // UID -> DB중계 -> 게임(UserKill)

    const int nExpectedDataSize = 
        HanUnitedBilling::MAX_ID_LEN + HanUnitedBilling::MAX_IP_LEN;

    // 호출 전에 미리 호스트 오더로 컨버팅 했음
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
            DETLOG2(g_Log, "ID:%s/IP:%s/통합빌링 과금 만료 접속 끊기 전송 성공", szID, szIP);
        }
        else
        {
            SERLOG2(g_Log, "ID:%s/IP:%s/통합빌링 과금 만료 접속 끊기 전송 실패", szID, szIP);
        }
    }

    return true;
}


bool CHanUnitedDispatch::ProcessPing(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    CBuffer* lpBuffer = CREATE_BUFFER(GetBufferFactory(), sizeof(HanUnitedBilling::GLTransStruct));

    if(0 != lpBuffer)
    {
        // 버퍼 열기
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
            DETLOG0(g_Log, "핑 패킷을 되돌려 보내는 데 실패했습니다.");
        }
    }

    return false;
}


// 로그아웃 Ack처리.
bool CHanUnitedDispatch::ProcessLogoutResult(HanUnitedBilling::GLTransStruct& glTransStruct)
{
    const int nExpectedDataSize = 
        HanUnitedBilling::MAX_ID_LEN + HanUnitedBilling::MAX_IP_LEN + sizeof(short);

    // 호출 전에 미리 호스트 오더로 컨버팅 했음
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
            // 로그를 남기고 제거한다.
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
                // 로그인 불가 처리.
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

        // 로그를 남기고 제거한다.
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
            // 로그인 불가 처리.
            AgentSendPacket::SendUpdateUIDTable(
                requestData.m_cServerGroup, requestData.m_PktUUT.m_cCmd, 
                requestData.m_PktUUT.m_dwUserID, 4, 0, 0, 'N', 0, 0);
        }
    }

    m_LoginRequestMap.clear();
}
