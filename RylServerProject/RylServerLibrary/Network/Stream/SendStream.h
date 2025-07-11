#ifndef _SEND_STREAM_H_
#define _SEND_STREAM_H_


// 전방 참조
class CBuffer;
class CSession;


// SingleThread 전용임.
class CSendStream
{
public:

    // 버퍼 크기는 최대 패킷 크기의 두배 이상이어야 함.
    CSendStream(CSession& Session);
    ~CSendStream();

    // Desc : 필요한 만큼 버퍼를 얻어 온다.
    char* GetBuffer(unsigned long dwReserve_In);
    bool  PutBuffer(const char* szBuffer, const unsigned long dwBufferSize,
        unsigned char cCMD_In);

    // Completion Buffer - 얻어온 버퍼를 닫는다.
    bool WrapHeader(unsigned short usUsed_In, unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);
    bool WrapCrypt(unsigned short usUsed_In, unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);
    bool WrapCompress(char* lpSourceData, unsigned short usSourceLength, 
        unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);
    
private:

    // 버퍼가 다 차지 않았어도 Session에 Enqueue한다
    bool ForceSend();

    CBuffer*        m_lpBuffer;
    CSession&       m_Session;    
};

#endif