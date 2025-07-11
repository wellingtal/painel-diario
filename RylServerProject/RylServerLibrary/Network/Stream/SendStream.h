#ifndef _SEND_STREAM_H_
#define _SEND_STREAM_H_


// ���� ����
class CBuffer;
class CSession;


// SingleThread ������.
class CSendStream
{
public:

    // ���� ũ��� �ִ� ��Ŷ ũ���� �ι� �̻��̾�� ��.
    CSendStream(CSession& Session);
    ~CSendStream();

    // Desc : �ʿ��� ��ŭ ���۸� ��� �´�.
    char* GetBuffer(unsigned long dwReserve_In);
    bool  PutBuffer(const char* szBuffer, const unsigned long dwBufferSize,
        unsigned char cCMD_In);

    // Completion Buffer - ���� ���۸� �ݴ´�.
    bool WrapHeader(unsigned short usUsed_In, unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);
    bool WrapCrypt(unsigned short usUsed_In, unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);
    bool WrapCompress(char* lpSourceData, unsigned short usSourceLength, 
        unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);
    
private:

    // ���۰� �� ���� �ʾҾ Session�� Enqueue�Ѵ�
    bool ForceSend();

    CBuffer*        m_lpBuffer;
    CSession&       m_Session;    
};

#endif