#ifndef _RYL_SERVER_NETWORK_SERVER_LOG_H_
#define _RYL_SERVER_NETWORK_SERVER_LOG_H_

#include <Network/Packet/PacketBase.h>


#pragma pack(1)

struct PktServerLog : public PktBase
{
    enum LogType
    {
        ITEM_DUPLICATED_LOG     = 0x01            
    };

    unsigned char       m_cLogCmd;
};

// ������ ���� �α�
struct PktItemDuplicated : public PktServerLog
{
    enum { MAX_NAME = 16 };

    char                m_szName[MAX_NAME];
    unsigned __int64    m_dwItemSerial;
    unsigned long       m_dwUID;
    unsigned long       m_dwCID;
    unsigned long       m_dwQty;
};

#pragma pack()


#endif