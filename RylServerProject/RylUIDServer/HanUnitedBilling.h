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
//! ���� SequenceID Ŭ�����̴�. �� ��Ŷ�� ������ ID�� �ű��.
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
//!  �α׾ƿ� ���� ����Ʈ�̴�. 
//!  �ֱ������� üũ�ؼ�, Ack�� ���� ������ �������Ѵ�.
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
//!  �Ѱ��� ���պ��� ��Ŷ�� ó���ϴ� Ŭ�����̴�.
//!  
//! ---------------------------------------------------------------
class CHanUnitedDispatch : public CPacketDispatch
{
public:

    enum
    {
        DEFAULT_SEND_TIMEOUT_SEC = 10
    };

    // �̵���� ���պ��������� ������ �õ���.
    static bool ConnectToHanUnited(CIOCPNet& iocpNet);
    static CSingleDispatch& GetDispatchTable();

    CHanUnitedDispatch(CSession& Session);
    virtual ~CHanUnitedDispatch();

	virtual bool ParsePacket(char* const lpStream_In, unsigned long* dwStreamSize_InOut);
    virtual bool Dispatch();
  
    virtual void Connected();
    virtual void Disconnected();

    CBufferFactory& GetBufferFactory();

    //! �Ѱ��� ���պ������� �α����� �����Ѵ�.
    bool SendLogin(const PktUUT& pktUUT, const RE_USPCheckBilling_Login& checkBillingLogin,
        unsigned char cServerGroup, unsigned long dwTimeoutSec = DEFAULT_SEND_TIMEOUT_SEC);

    //! �Ѱ��� ���պ������� �α׾ƿ��� �����Ѵ�. �ð��� ���� ������ ����ð����� �����Ѵ�.
    bool SendLogout(const PktUUT& pktUUT, unsigned long dwSeqID,
        LPSYSTEMTIME lpSystemTime = 0, unsigned long dwTimeoutSec = DEFAULT_SEND_TIMEOUT_SEC);    

    //! �Ѱ��� ���պ������� �α����� �������� ���θ� �����Ѵ�.
    bool SendCanLogin(const PktUUT& pktUUT, const RE_USPCheckBilling_Login& checkBillingLogin,
        unsigned char cServerGroup, unsigned long dwTimeoutSec = DEFAULT_SEND_TIMEOUT_SEC);
    
    bool ProcessLoginResult(HanUnitedBilling::GLTransStruct& glTransStruct);      // �α��� ��� ó��
    bool ProcessCanLoginResult(HanUnitedBilling::GLTransStruct& glTransStruct);   // �α��� �� �� �ִ��� ���η� ��� ó��.
    bool ProcessWarnNotice(HanUnitedBilling::GLTransStruct& glTransStruct);       // ���� ��� �޽���
    bool ProcessCutIP(HanUnitedBilling::GLTransStruct& glTransStruct);            // Ư�� ip����
    bool ProcessPing(HanUnitedBilling::GLTransStruct& glTransStruct);             // �� ��Ŷ ó��

    bool ProcessLogoutResult(HanUnitedBilling::GLTransStruct& glTransStruct);     // �α׾ƿ� Ackó��.

    void ProcessTimeout();
    void ProcessDisconnected();

protected:

    typedef CCSLock BufferLock;


    //! �α��� ������ ��. timeout�� ����ϵ��� �ϱ� �����̴�.
    //! �α���, �α��� ���ɿ��� üũ�ÿ��� ���
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
