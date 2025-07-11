#ifndef _RYL_SERVER_DISPATCH_H_
#define _RYL_SERVER_DISPATCH_H_

#include <winsock2.h>
#include <windows.h>

#include <Thread/Lock.h>
#include <Network/Dispatch/Dispatch.h>
#include <Network/Stream/SendStream.h>
#include <Stream/Buffer/BufferQueue.h>

// forward delc.
struct	PktBase;
class	CBuffer;
class	CBufferFactory;
class   INET_Addr;

class CRylServerDispatch : public CPacketDispatch
{
public:

    enum CreationResult
    {
        S_CREATE_SUCCESS,
        E_INVALID_STARTBIT,
        E_ALLOCATE_BUFFER_FAILED,
        E_DECOMPRESS_FAILED
    };

    static CreationResult CreatePacket(
        CBufferFactory& bufferFactory,
        CBufferQueue& bufferQueue, char* const lpStream_In, 
        unsigned long* dwStreamSize_InOut);
    
    virtual bool ParsePacket(char *const lpStream_In, unsigned long* dwStreamSize_InOut);

    unsigned long GetMaxProcessPacketPerPulse() const { return m_dwMaxProcessPacketPerPulse; }
    void SetMaxProcessPacketPerPulse(unsigned long dwMaxProcessPacketPerPulse)
    { m_dwMaxProcessPacketPerPulse = dwMaxProcessPacketPerPulse; }

	void LogErrorPacket(const char* szDetailText, const unsigned char cCmd);
    void LogErrorPacketCreation(CreationResult eResult);

	CBufferFactory& GetBufferFactory();
    CSendStream& GetSendStream() { return m_SendStream; }
	
    INET_Addr& GetRemoteAddr();
    INET_Addr& GetLocalAddr();

    bool Shutdown();
    void CloseSession();

protected:
    
    CRylServerDispatch(CSession& Session, unsigned long dwMaxProcessPacketPerPulse);
    ~CRylServerDispatch();

	// 패킷 처리를 수행한다.
    virtual bool Dispatch();

    // 너무 많은 패킷이 들어왔을 경우, 패킷 내용에 대해서 로그를 남기거나, 패킷을 삭제한다.
    virtual void ProcessTooManyPacket(CBufferQueue& bufferQueue);

    // 패킷을 처리한다.
    virtual bool DispatchPacket(PktBase* lpPktBase) = 0;

    // Recv를 멈추거나, 멈춘 Recv를 다시 돌린다.
    void InternalCheckSuspendRecv(CBufferQueue& bufferQueue);

    //-------------------------------------------------------------------------------

    typedef CCSLock BufferQueueLock;

    BufferQueueLock m_BufferQueueLock;
    CACHE_PAD(m_BufferQueueLockPadding, sizeof(BufferQueueLock));

    CBufferQueue		m_ProcessQueue;
    CSendStream         m_SendStream;
	
    unsigned long		m_dwMaxProcessPacketPerPulse;
	unsigned long		m_dwFlags;
};


class CSendPacketAllServer
{
public:

    CSendPacketAllServer(const char* szData, unsigned long dwDataLen, unsigned char cPacketCmd);
    bool operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch);

private:

    const char*     m_szData;
    unsigned long   m_dwDataLen;
    unsigned char   m_cPacketCmd;
};

#endif