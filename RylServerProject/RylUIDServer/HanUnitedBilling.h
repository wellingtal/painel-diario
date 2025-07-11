#ifndef _HAN_UNITED_BILLING_PROCESS_
#define _HAN_UNITED_BILLING_PROCESS_

#include <Thread/Lock.h>
#include <Network/Dispatch/Dispatch.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>

#include <Stream/Buffer/BufferQueue.h>
#include <boost/pool/pool_alloc.hpp>

#include <Network/Dispatch/SingleDispatchStorage.h>

#include <DB/DBDefine.h>

// forward decl.
class CIOCPNet;
class CBufferFactory;

namespace HanUnitedBilling
{
    struct GLTransStruct;
}


//! ---------------------------------------------------------------
//!  
//! 전역 SequenceID 클래스이다. 각 패킷에 시퀀스 ID를 매긴다.
//!  
//! ---------------------------------------------------------------
class CHanUnitedSequenceID
{
public:

    static CHanUnitedSequenceID& GetInstance();
    unsigned long GetUniqueID() { return m_dwSeqID++; }

private:

    CHanUnitedSequenceID() : m_dwSeqID(0) { }
    ~CHanUnitedSequenceID() { }

    unsigned long m_dwSeqID;
};


//! ---------------------------------------------------------------
//!  
//!  로그아웃 정보 리스트이다. 
//!  주기적으로 체크해서, Ack가 오지 않으면 재전송한다.
//!  
//! ---------------------------------------------------------------
class CHanUnitedLogout
{
public:

    static CHanUnitedLogout& GetInstance();

    bool AddLogout(const PktUUT& pktUUT);
    bool RemoveLogout(const char* szID, const char* szIP, DWORD dwSeqID);
    void Retransmission();

private:

    struct LogoutData
    {
        PktUUT      m_pktUUT;
        SYSTEMTIME  m_systemTime;
        DWORD       m_dwSendStamp;
        DWORD       m_dwSeqID;

        LogoutData(const PktUUT& pktUUT, DWORD dwSeqID);
    };

    typedef std::map<DWORD, LogoutData>   LogoutMap;

    LogoutMap   m_LogoutMap;
};


//! ---------------------------------------------------------------
//!  
//!  한게임 통합빌링 패킷을 처리하는 클래스이다.
//!  
//! ---------------------------------------------------------------
class CHanUnitedDispatch : public CPacketDispatch
{
public:

    enum
    {
        DEFAULT_SEND_TIMEOUT_SEC = 10
    };

    // 미디어웹 통합빌링서버로 연결을 시도함.
    static bool ConnectToHanUnited(CIOCPNet& iocpNet);
    static CSingleDispatch& GetDispatchTable();

    CHanUnitedDispatch(CSession& Session);
    virtual ~CHanUnitedDispatch();

	virtual bool ParsePacket(char* const lpStream_In, unsigned long* dwStreamSize_InOut);
    virtual bool Dispatch();
  
    virtual void Connected();
    virtual void Disconnected();

    CBufferFactory& GetBufferFactory();

    //! 한게임 통합빌링으로 로그인을 전송한다.
    bool SendLogin(const PktUUT& pktUUT, const RE_USPCheckBilling_Login& checkBillingLogin,
        unsigned char cServerGroup, unsigned long dwTimeoutSec = DEFAULT_SEND_TIMEOUT_SEC);

    //! 한게임 통합빌링으로 로그아웃을 전송한다. 시간을 넣지 않으면 현재시간으로 전송한다.
    bool SendLogout(const PktUUT& pktUUT, unsigned long dwSeqID,
        LPSYSTEMTIME lpSystemTime = 0, unsigned long dwTimeoutSec = DEFAULT_SEND_TIMEOUT_SEC);    

    //! 한게임 통합빌링으로 로그인이 가능한지 여부를 전송한다.
    bool SendCanLogin(const PktUUT& pktUUT, const RE_USPCheckBilling_Login& checkBillingLogin,
        unsigned char cServerGroup, unsigned long dwTimeoutSec = DEFAULT_SEND_TIMEOUT_SEC);
    
    bool ProcessLoginResult(HanUnitedBilling::GLTransStruct& glTransStruct);      // 로그인 결과 처리
    bool ProcessCanLoginResult(HanUnitedBilling::GLTransStruct& glTransStruct);   // 로그인 할 수 있는지 여부로 결과 처리.
    bool ProcessWarnNotice(HanUnitedBilling::GLTransStruct& glTransStruct);       // 과금 경고 메시지
    bool ProcessCutIP(HanUnitedBilling::GLTransStruct& glTransStruct);            // 특정 ip끊기
    bool ProcessPing(HanUnitedBilling::GLTransStruct& glTransStruct);             // 핑 패킷 처리

    bool ProcessLogoutResult(HanUnitedBilling::GLTransStruct& glTransStruct);     // 로그아웃 Ack처리.

    void ProcessTimeout();
    void ProcessDisconnected();

protected:

    typedef CCSLock BufferLock;


    //! 로그인 데이터 맵. timeout시 사용하도록 하기 위함이다.
    //! 로그인, 로그인 가능여부 체크시에만 사용
    struct LoginRequestData
    {
        enum
        {
            MAX_NAME_LEN = 16
        };

        __time64_t      m_SendTime;
        unsigned long   m_SendStamp;
        unsigned long   m_dwTimeoutSec;

        unsigned long   m_dwSeqID;
        unsigned short  m_usMsgType;
        unsigned char   m_cServerGroup;
        unsigned char   m_cPadding;
                
        PktUUT                      m_PktUUT;
        RE_USPCheckBilling_Login    m_CheckBillingLogin;

        LoginRequestData(const PktUUT& pktUUT, const RE_USPCheckBilling_Login& checkBillingLogin,
            unsigned long dwTimeoutSec, unsigned long dwSeqID, unsigned short usMsgType,
            unsigned char cServerGroup);
    };

    typedef std::map<unsigned long, LoginRequestData, std::less<unsigned long>, 
        boost::fast_pool_allocator<std::pair<unsigned long, LoginRequestData> > > LoginRequestMap;

    BufferLock      m_BufferLock;
    CACHE_PAD(BufferLockPad, sizeof(BufferLock));

    CBufferQueue    m_PacketBuffer;
    LoginRequestMap m_LoginRequestMap;
};

#endif
