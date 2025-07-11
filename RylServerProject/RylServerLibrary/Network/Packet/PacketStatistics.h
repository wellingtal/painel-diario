#ifndef _GAMA_PACKET_STATISTICS_H_
#define _GAMA_PACKET_STATISTICS_H_

#include <climits>
#include <ctime>

class CPacketStatistics
{
public:

    struct PacketData
    {
        double  m_Bytes;
        double  m_Count;

        PacketData();
        void Clear();
    };

    struct CompressData
    {
        double  m_CompressCount;
        double  m_FailedCount;

        double  m_TotalBytes;
        double  m_TotalCompressed;

        CompressData();
        void Clear();
    };

    typedef void (*FnPrintUserMessage) (FILE* fDescriptor);

    static CPacketStatistics& GetInstance();

    void Recv(unsigned char cCmd, unsigned long dwPacketSize);  // 받은 패킷 통계   (압축된 패킷)
    void Send(unsigned char cCmd, unsigned long dwPacketSize);  // 전송한 패킷 통계 (압축된 패킷)

    // 압축관련 통계 작성
    void CheckCompression(unsigned char cCmd, unsigned long dwSrcSize, unsigned long dwDstSize);

    void Clear();
    bool Log();

    void SetUserMessageFunc(FnPrintUserMessage fnPreFix = 0, FnPrintUserMessage fnPostFix = 0);

private:

    CPacketStatistics();
    ~CPacketStatistics();

    time_t              m_StartTime;
    FnPrintUserMessage  m_fnPrefix;
    FnPrintUserMessage  m_fnPostfix;

    PacketData      m_RecvData[UCHAR_MAX];
    PacketData      m_SendData[UCHAR_MAX];

    CompressData    m_CompressionData[UCHAR_MAX];
    
};


#endif