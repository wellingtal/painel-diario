#ifndef _UNIFIED_CHAR_DATA_PACKET_H_
#define _UNIFIED_CHAR_DATA_PACKET_H_

#include <DB/DBdefine.h>
#include <Network/Packet/PacketBase.h>

namespace UnifiedConst
{
    enum AgentServerType
    {
        // OldServerGroupID�� �����ε� ����Ѵ�. 0 ~ 10 �� ���� ������ ID�̴�.

        Part1			= 20,
        Part1Unified	= 21,
        ROW				= 22,
        Part2Unified	= 23,
        Part2Selectable = 24,
    };

    enum StoreSelected
    {
        NONE_SELECTED   = 0,
        SELECTED_PART1  = (1 << 0),
        SELECTED_PART2  = (1 << 1)
    };
}


struct PktUnifiedCharInfo : public PktBase
{
    enum
    {
        MAX_STORE_INFO  = 10,
        MAX_CHAR_DATA   = 30
    };

    enum ErrorCode
    {
        STORE_READ_ERROR        = 1,
        CHARINFO_READ_ERROR     = 2        
    };

    unsigned long       dwUID;
    
    unsigned char       cStoreInfoNum;
    unsigned char       cCharDataNum;
    unsigned char       cRestrictedPart1ToPart2Level;   // Part1 ���� Part2 �� ���� ������ ĳ���� ����
    unsigned char       cRemainCharTransferCount;       // Part1 ���� Part2 �� ���� ������ Ƚ��

    USER_INFO           userInfo;
    UnifiedStoreInfo    unifiedStoreInfo[MAX_STORE_INFO];
    UnifiedCharData     unifiedCharData[MAX_CHAR_DATA];
};

struct PktUnifiedCharSelectReq : public PktBase
{
    enum
    {        
		// edith 2008.03.17 ID,PASS ��������
        MAX_PASSWORD_LEN = 36            
    };

    char            szPassword[MAX_PASSWORD_LEN];   // ���� �н�����

    unsigned char   cSelectedServerGroupID;         // ������ ������ ��ȣ
    unsigned char   cSelectedNation;                // ������ ����        
    unsigned long   dwRequestKey;                   // RequestKey

    unsigned long   dwUID;                          // UID
    unsigned long   dwCID[USER_INFO::MAX_CHAR_NUM]; // ������ ���� ��ȣ�� CID�� ä�� �ش�.    
};

struct PktUnifiedCharSelectAck : public PktBase
{
    enum ErrorCode
    {
        SERVER_ERROR                = 1,
        PACKET_ERROR                = 2,
        PASSWORD_FAILED             = 3,

        WRONG_STORE_SELECTED        = 4,        // �߸��� â�� ����(�� ���� �ƴѰ� ����)
        WRONG_CHAR_SELECTED         = 5,        // �߸��� ĳ���� ����(�� ���� �ƴѰ� ����. �̹� ������ �� ����)

        UNIFIED_STORE_READ_ERROR    = 6,        // ������ â�� �б� ����
        UNIFIED_STORE_WRITE_ERROR   = 7,        // ������ â�� ��� ����

        UNIFIED_CHAR_READ_ERROR     = 8,        // ĳ���� ���� ���� �б� ����
        UNIFIED_CHAR_WRITE_ERROR    = 9,        // ĳ���� ���� ���� ��� ����
        CHAR_VIEW_RELOAD_ERROR      = 10,       // ĳ���� �� ���ε� ����

        PART1_DB_LOGINED            = 11,       // Part1 DB�� �α��� �� ����    
        TRANSFER_COUNT_OVER         = 12        // TransferCount�� ���� ȸ�� �̻� �ʰ��Ǿ� �ִ�.
    };

    unsigned long   dwUID;
    unsigned long   dwRequestKey;
};

#endif