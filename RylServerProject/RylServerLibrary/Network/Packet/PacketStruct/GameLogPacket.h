#ifndef _GAME_LOG_PACKET_H_
#define _GAME_LOG_PACKET_H_

#include <Network/Packet/PacketBase.h>


namespace GAMELOG
{
    namespace CMD
    {
        enum Packet
        {
            DefaultInfo                 = 1,
            ItemInfo                    = 2,
            EquipInfo                   = 3,
        };

        enum LogType
        {
            CHAR_LOGIN                  = 1,    // �α���
            CHAR_LOGOUT                 = 2,    // �α׾ƿ�
            PICKUP                      = 3,    // ������ ����                      - �� ����
            DROP                        = 4,    // ������ ������                    - �� ����
            BUY_ITEM                    = 5,    // ������ ���                      - �� ����
            SELL_ITEM                   = 6,    // ������ �ȱ�                      - �� ����
            BEFORE_EXCHANGE             = 7,    // ������ ��ȯ ���� ��ȯâ ������   - �� ����
            AFTER_EXCHANGE              = 8,    // ������ ��ȯ ���� ��ȯâ ������   - �� ����
            USE_ITEM                    = 9,    // ������ ���                      - �� ����
            UPGRADE_ITEM                = 10,   // ������ ���׷��̵�                - �� ����

            INSTALL_SOCKET              = 11,   // ������ ���Ͽ� ���� �ڱ�          
            GET_QUEST_ITEM              = 12,   // ����Ʈ ������ ���
            GET_QUEST_REWARD            = 13,   // ����Ʈ ���� ���                 - �� ����
            REMOVE_QUEST_ITEM           = 14,   // ����Ʈ ������ ����
            REPAIR_ITEM                 = 15,   // ������ ����                      - �� ����
            ADMIN_DROP                  = 16,   // ��� ������� ������ ������    - �� ����

            CHAR_LOGIN_INVENTORY_GOLD   = 17,   // ĳ���� �α��ν� �κ��丮 ��      
            CHAR_LOGIN_DEPOSIT_GOLD     = 18,   // ĳ���� �α��ν� â�� ��          
            CHAR_LOGOUT_INVENTORY_GOLD  = 19,   // ĳ���� �α׾ƿ��� �κ��丮 ��    
            CHAR_LOGOUT_DEPOSIT_GOLD    = 20,   // ĳ���� �α׾ƿ��� â�� ��        
            DEPOSIT_RENT_GOLD           = 21,   // â�� �뿩��

			FAME_GET_BATTLE				= 22,	// ������ ���� ��
			FAME_LOSE_BATTLE			= 23,	// ������ ���� ��
			FAME_GET_CAMP				= 24,	// ����� ���� ��
			FAME_LOSE_CAMP				= 25,	// ����� ���� ��

			QUEST_GET_REWARD			= 26,	// ����Ʈ �������� ���� ����

			CHANGE_RIDE					= 27,	// ���̴� Ÿ�� �α�

            MAX_LOG_TYPE                = 28
        };
    };

    namespace PACKET
    {
        enum Flags
        {
            ITEM_INFO   = (1 << 0),
            EQUIP_INFO  = (1 << 1)
        };

        enum
        {
            MAX_SOCKET_NUM      = 8,
            MAX_ATTRIBUTE_NUM   = 35
        };
      
        #pragma pack(1)

        struct DefaultInfo : public PktBase
        {
            time_t              m_dwTime;
            unsigned long       m_dwFirstUID;       // �������� UID
            unsigned long       m_dwFirstCID;       // �������� CID
            unsigned long       m_dwGold;           // ������ ���ؼ� ������ �ް� �� �ݾ�
            unsigned long       m_dwSecondUID;      // ������� UID
            unsigned long       m_dwSecondCID;      // ������� CID
            
            unsigned short      m_usPosX;           // ����� �Ͼ ���(X)
            unsigned short      m_usPosY;           // ����� �Ͼ ���(Y)
            unsigned short      m_usPosZ;           // ����� �Ͼ ���(Z)
            unsigned char       m_cCommand;         // ��� ����
            unsigned char       m_cFlags;           // ��Ÿ ����(������ ��������.. ��� ������ ���� �ٴ��� ��..)
        };

        struct ItemInfo : public DefaultInfo
        {
            unsigned __int64    m_dwItemUID;
            unsigned short      m_usItemProtoTypeID;
            unsigned char       m_cTakeType;
            unsigned char       m_cItemAmount;

            unsigned char       m_cItemPosX;
            unsigned char       m_cItemPosY;
            unsigned char       m_cItemPosZ;
            unsigned char       m_cItemExtra;       // �߰� ����
        };

        struct EquipInfo : public ItemInfo
        {
            unsigned short      m_usRuneSocket;                     // �� ���� ����
            unsigned char       m_cUpgradeStep;                     // ���׷��̵� �ܰ�
            unsigned char       m_cMaxSocket;                       // ���� �շ� �ִ� ���� ��            
            unsigned char       m_cMaxDurability;                   // �ִ� ������

            unsigned char       m_cSocket[MAX_SOCKET_NUM];          // ���� ����
            short               m_usAttribute[MAX_ATTRIBUTE_NUM];   // �Ӽ� ����
        };

        #pragma pack()
    };
};



#endif