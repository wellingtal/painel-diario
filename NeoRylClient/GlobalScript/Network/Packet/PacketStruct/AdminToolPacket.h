#ifndef _ADMIN_TOOL_PACKET_H_
#define _ADMIN_TOOL_PACKET_H_

#include <Network/Packet/PacketBase.h>
#include <DB/DBDefine.h>

#pragma pack(1)

namespace PktAdminTool
{
    enum DefaultConstants
    {
        MAX_NAME            = 16,
        MAX_ITEM_NAME       = 32,
        MAX_PASSWORD        = 16,
        MAX_IP_LEN          = 16,        
        MAX_DATE_LEN        = 11,
        MAX_DATE_TIME_LEN   = 20,
        ADMIN_NAME_LEN      = 7,
        BLOCK_TYPE_LEN      = 9,
        MAX_RIGHTS_NUM      = 20,
        MAX_DESCRIPTION     = 31
    };

    namespace CMD
    {
        enum Type
        {
            Auth                    = 1,    // struct Auth
            AuthAck                 = 1,    // struct AuthAck

            AddAdmin                = 2,    // struct AddAdmin
            AddAdminAck             = 2,    // struct SimpleAck

            SearchAdmin             = 3,    // struct SearchAdmin
            SearchAdminAck          = 3,    // struct SearchAdminAck

            ViewAdmin               = 4,    // struct PktBase
            ViewAdminAck            = 4,    // struct SearchAdminAck

            DeleteAdmin             = 5,    // struct RemoveAdmin
            DeleteAdminAck          = 5,    // struct SimpleAck

            UpdateAdmin             = 6,    // struct UpdateAdmin
            UpdateAdminAck          = 6,    // struct SimpleAck

            FindLoginUser           = 7,    // struct FindLoginUser
            FindLoginUserAck        = 7,    // struct FindLoginUserAck

            ForceLogoutUser         = 8,    // struct ForceLogoutUser
            ForceLogoutUserAck      = 8,    // struct SimpleAck

            ViewRestraintUser       = 9,    // struct ViewRestraintUser
            ViewRestraintUserAck    = 9,    // struct ViewRestraintUserAck

            AddRestraintUser        = 10,   // struct AddRestraintUser
            AddRestraintUserAck     = 10,   // struct SimpleAck

            CheckRestraintUser      = 11,   // struct CheckRestraintUser
            CheckRestraintUserAck   = 11,   // struct CheckRestraintUserAck

            DeleteRestraintUser     = 12,   // struct DeleteRestraintUser
            DeleteRestraintUserAck  = 12,   // struct SimpleAck

            GetCharacterNames       = 13,   // struct GetCharacterNames
            GetCharacterNamesAck    = 13,   // struct GetCharacterNamesAck

            GetCharData             = 14,   // struct GetCharData
            GetCharDataAck          = 14,   // struct GetCharDataAck

            CommitData              = 15,   // struct PktBase
            CommitDataAck           = 15,   // struct SimpleAck

            UpdateCharData          = 16,   // struct UpdateCharData
            UpdateCharDataAck       = 16,   // struct SimpleAck

            UpdateCharPos           = 17,   // struct UpdateCharPos
            UpdateCharPosAck        = 17,   // struct SimpleAck

            CreateItem              = 18,   // struct CreateItem
            CreateItemAck           = 18,   // struct CreateItemAck

            UpdateItem              = 19,   // struct UpdateItem
            UpdateItemAck           = 19,   // struct UpdateItemAck

            DeleteItem              = 20,   // struct DeleteItem
            DeleteItemAck           = 20,   // struct DeleteItem

            ViewLoginoutLog         = 21,   // struct ViewLoginoutLog
            ViewLoginoutLogAck      = 21,   // struct ViewLoginoutLogAck

            GetAdminDetailRight     = 22,   // struct GetAdminDetailRight
            GetAdminDetailRightAck  = 22,   // struct AdminDetailRightAck

            SetAdminDetailRight     = 23,   // struct SetAdminDetailRight
            SetAdminDetailRightAck  = 23    // struct AdminDetailRightAck
        };
    };

    struct AdminToolItemInfo
    {
        char                m_szItemName[64];       // ������ �̸�. �����Ұ�
        unsigned __int64    m_ItemUID;              // ������ UID.  �����Ұ�
        unsigned short      m_usItemProtoTypeID;    // ������ ����ID. ���� �Ұ�
        unsigned char       m_cItemType;            // ������ Ÿ��.   ���� �Ұ�
        unsigned char       m_cTakePos;             // ������ �����̳� ��ġ(TakeType). ���� �Ұ�

        unsigned char       m_cXPos;                // ������ X��ġ.    �����Ұ�
        unsigned char       m_cYPos;                // ������ Y��ġ.    �����Ұ�
        unsigned char       m_cZPos;                // ������ Z��ġ.    �����Ұ�
        unsigned char       m_cNumOrDurability;     // ������ ���� Ȥ�� ������. ��������

        unsigned char       m_cXSize;               // ������ Xũ��.    �����Ұ�
        unsigned char       m_cYSize;               // ������ Yũ��.    �����Ұ�
        bool                m_bEquip;               // ������ ��񿩺�.    �����Ұ�
        unsigned char       m_cPadding;             // �ǹ� ����.

        AdminToolItemInfo()
        {
            memset(m_szItemName, 0, 64);
            m_ItemUID = m_usItemProtoTypeID = m_cItemType = m_cTakePos = 0;
            m_cXPos = m_cYPos = m_cZPos = m_cNumOrDurability = m_cXSize = m_cYSize = m_cPadding = 0;
            m_bEquip = false;
        }
    };

    struct AdminToolEquipInfo : public AdminToolItemInfo
    {
        unsigned short      m_usRuneSocket;         // ������ �� ����
        unsigned char       m_cMaxDurability;       // ������ �ִ� ������
        unsigned char       m_cCurrentSocketNum;    // ������ ���� ���� ��

        unsigned char       m_cMaxSocketNum;        // ������ �ִ� ���� ��
        unsigned char       m_cMaterialType;        // ������ ���׷��̵� �ܰ�

        unsigned char       m_cSocket[8];           // ������ ���� ����
        short               m_usAttribute[35];      // ������ �Ӽ�

        AdminToolEquipInfo()
        {
            m_usRuneSocket = m_cMaxDurability = m_cCurrentSocketNum = m_cMaxSocketNum = m_cMaterialType = 0;
            memset(m_cSocket, 0, 8);
            memset(m_usAttribute, 0, sizeof(unsigned short) * 35);
        }
    };

    // �ܼ� Ack
    struct SimpleAck : public PktBase
    {

    };

    // ----------------------------------------------------------------------------

    // ����� ����
    struct Auth : public PktBase
    {
        char            m_szName[MAX_NAME];
        char            m_szPassword[MAX_PASSWORD];        
    };


    // ����� ���� Ack
    struct AuthAck : public PktBase
    {        
        char    m_cRights;                   // ��� ���� ( G, M, T... )
        char    m_szRights[MAX_RIGHTS_NUM];  // ���� ��� ����
    };

    // ----------------------------------------------------------------------------

    // ������ �߰�
    struct AddAdmin : public PktBase
    {
        char            m_szName[MAX_NAME];
        char            m_szPassword[MAX_PASSWORD];
        char            m_szIP[MAX_IP_LEN];             // ���Ӱ��� IP
        char            m_szAdminName[ADMIN_NAME_LEN];  // ���� ����� �̸�.
        unsigned char   m_cRights;                      // ��� ���� ( 0, 1, 2, ... )
    };

    // ----------------------------------------------------------------------------

    // ������ �˻�
    struct SearchAdmin : public PktBase
    {
        char            m_szName[MAX_NAME];
    };

    struct SearchAdminAck : public PktBase
    {
        char            m_szAdminID[MAX_NAME];
        char            m_szPassword[MAX_PASSWORD];        
        char            m_szIP[MAX_IP_LEN];             // ���Ӱ��� IP.
        char            m_szAdminName[ADMIN_NAME_LEN];  // ������ �̸�.
        char            m_cRights;                      // ��� ���� ( 0, 1, 2, ... )        
        char            m_szDate[MAX_DATE_LEN];         // ��¥ ����
    };

    // ----------------------------------------------------------------------------

    // ������ ����
    struct RemoveAdmin : public PktBase
    {
        char            m_szName[MAX_NAME];
    };

    // ----------------------------------------------------------------------------

    // ������ ���� ������Ʈ
    struct UpdateAdmin : public PktBase
    {
        char            m_szAdminID[MAX_NAME];
        char            m_szPassword[MAX_PASSWORD];        
        char            m_szIP[MAX_IP_LEN];             // ���Ӱ��� IP.
        char            m_szAdminName[MAX_NAME];        // ������ �̸�.
        char            m_cRights;                      // ��� ���� ( 0, 1, 2, ... )                
    };

    // ----------------------------------------------------------------------------

    // ���� �α��� ����� �˻�. ( Billing DB������ �˻���. ���߿� �߰�� Query�� �ʿ�.
    struct FindLoginUser : public PktBase
    {
        char            m_szSearchID[MAX_NAME];
    };

    struct FindLoginUserAck : public PktBase
    {
        char            m_szUserID[MAX_NAME];                   // ���� ID
        unsigned long   m_dwUID;                                // UID
        unsigned long   m_dwCRMIndex1;                          // PC�� ID
        char            m_szIP[MAX_IP_LEN];                     // ���� IP
        unsigned long   m_dwConnectedServerID;                  // ���ӵ� ���� ID
        char            m_szLoginDateTime[MAX_DATE_TIME_LEN];   // ���� �ð�
        unsigned char   m_cBillingType;                         // ����Ÿ��
    };

    // ----------------------------------------------------------------------------

    // ���� �α׾ƿ� �ϱ�.
    struct ForceLogoutUser : public PktBase
    {        
        unsigned long   m_dwUID;                // UID
        unsigned long   m_dwConnectedServerID;  // ���ӵ� ���� ID
    };

    // ----------------------------------------------------------------------------

    // ���� ��ȸ�ϱ�
    struct ViewRestraintUser : public PktBase
    {
        unsigned char   m_cFlags;               // �÷���
        unsigned char   m_cConnectedServer;     // ���� ����
        unsigned long   m_dwUID;                // ������ UID
    };

    struct ViewRestraintUserAck : public PktBase
    {
        unsigned long   m_dwUID;
        char            m_szUserID[MAX_NAME];                       // UserID
        char            m_szCharacterName[MAX_NAME];                // ĳ���� �̸�
        char            m_szBlockType[BLOCK_TYPE_LEN];              // �� ����
        char            m_szBlockKind[BLOCK_TYPE_LEN];              // �� ����
        char            m_szHasBlock[BLOCK_TYPE_LEN];               // �� ����
        char            m_szBlockFinishDateTime[MAX_DATE_TIME_LEN]; // �� ��������
        char            m_szAdminID[MAX_NAME];                      // ���� �� ��� ID
        char            m_szDescription[MAX_DESCRIPTION];           // �� ����
    };

    // ----------------------------------------------------------------------------

    // ���� ����ϱ�
    struct AddRestraintUser : public PktBase
    {
        unsigned char   m_cConnectedServer;                         // ���� ����
        unsigned long   m_dwUID;                                    // UID
        char            m_szCharacterName[MAX_NAME];                // ĳ���� �̸�
        unsigned char   m_cBlockType;                               // �� Ÿ�� ( ĳ���� �Ǵ� ���� )
        unsigned char   m_cBlockKind;                               // �� ���� ( �ӽú�, ������ )
        char            m_szBlockFinishDateTime[MAX_DATE_TIME_LEN]; // �� ��������
        char            m_szDescription[MAX_DESCRIPTION];           // �� ����
    };

    // ----------------------------------------------------------------------------

    // ���� Ȯ���ϱ�
    struct CheckRestraintUser : public PktBase
    {
        unsigned char   m_cConnectedServer;
        unsigned long   m_dwUID;
    };

    struct CheckRestraintUserAck : public PktBase
    {
        unsigned char   m_cResult;                  // 0 : ���� ����, 1 : ���� ��, 2 : ĳ���� ��
        char            m_szCharName1[MAX_NAME];    // ���� ĳ���� �̸� 1
        char            m_szCharName2[MAX_NAME];    // ���� ĳ���� �̸� 2
        char            m_szCharName3[MAX_NAME];    // ���� ĳ���� �̸� 3
    };

    // ----------------------------------------------------------------------------

    // ���� �����ϱ�
    struct DeleteRestraintUser : public PktBase
    {
        unsigned char   m_cServerID;                // ����ID
        unsigned long   m_dwUID;                    // UID
        char            m_szCharName[MAX_NAME];     // ���� ĳ���� �̸�
    };

    // ----------------------------------------------------------------------------

    // UID�� ĳ���� �̸� ��� ����
    struct GetCharacterNames : public PktBase
    {
        unsigned long   m_dwUID;
        unsigned char   m_cConnectedServer;
    };

    struct GetCharacterNamesAck : public PktBase
    {
        char            m_szCharName[3][MAX_NAME];
    };

    // ----------------------------------------------------------------------------

    // ĳ���� ������ ������
    struct PktAdminToolGetData : public PktBase
    {
        enum GetDataType
        {
            BasicInfo   = 0,    // 0 : SendLogin�� ���� ����
            StoreInfo   = 1,    // 1 : â�� ����
            Store1Data  = 2,    // 2 : â�� 1������
            Store2Data  = 3,    // 3 : â�� 2������
        };

        char            m_szName[MAX_NAME];
        unsigned long   m_dwRequestKey;
        unsigned long   m_dwUID;
        unsigned long   m_dwCID;
        unsigned char   m_cType;
    };

    struct PktAdminToolGetDataAck : public PktBase
    {
        unsigned long   m_dwRequestKey;
        unsigned long   m_dwUID;
        unsigned long   m_dwCID;
        unsigned char   m_cType;
    };

    // ���� ������ ������ ������Ʈ�ϱ�. (�ڿ� �����͸� ����)
    struct PktAdminToolSetData : public PktBase
    {
        enum SetDataType
        {
            BasicInfo   = 0,    // 0 : SendLogin�� ���� ����
            StoreInfo   = 1,    // 1 : â�� ����
            Store1Data  = 2,    // 2 : â�� 1������
            Store2Data  = 3,    // 3 : â�� 2������
        };

        unsigned __int64    m_dwSerial;
        unsigned long       m_dwRequestKey;
        unsigned long       m_dwUID;
        unsigned long       m_dwCID;
        unsigned char       m_cType;
    };

    struct PktAdminToolSetDataAck : public PktBase
    {
        unsigned long       m_dwRequestKey;
        unsigned char       m_cType;
    };

    // ----------------------------------------------------------------------------
    
    // ���������� ������ ��� ����
    struct GetCharData : public PktBase
    {
        char                m_szName[MAX_NAME];
        unsigned long       m_dwUID;
        unsigned long       m_dwCID;
        unsigned char       m_cServerGroup;
    };

    struct GetCharDataAck : public PktBase
    {
        unsigned long       m_dwUID;
        unsigned long       m_dwCID;
        unsigned char       m_cType;    // DataType
    };

    // ----------------------------------------------------------------------------

    // ĳ���� ������ ������Ʈ
    struct UpdateCharData : public PktBase
    {
        CHAR_INFOST         m_CharInfo;
    };

    // ĳ���� ��ġ ������Ʈ
    struct UpdateCharPos : public PktBase
    {
        POS                 m_LastPos;
    };

    // ----------------------------------------------------------------------------

    // ������ ����
    struct CreateItem : public PktBase
    {
        char                m_szItemName[MAX_ITEM_NAME];
        unsigned char       m_cTakePos;
        unsigned char       m_cXPos;
        unsigned char       m_cYPos;
        unsigned char       m_cZPos;
    };

    // ������ ���� Ack (�ڿ� ������ ������ ���� ����)
    struct CreateItemAck : public PktBase
    {
        AdminToolEquipInfo  m_EquipInfo;    // ��� �ƴϸ� ���� �����ʹ� 0���� ä�� ��
    };

    // ----------------------------------------------------------------------------

    // ������ ������Ʈ
    struct UpdateItem : public PktBase
    {
        AdminToolEquipInfo  m_EquipInfo;
    };

    // ������ ������Ʈ Ack
    struct UpdateItemAck : public PktBase
    {
        AdminToolEquipInfo  m_EquipInfo;
    };

    // ----------------------------------------------------------------------------

    // ������ ����
    struct DeleteItem : public PktBase
    {
        unsigned __int64    m_dwItemUID;
        unsigned short      m_usItemProtoTypeID;
        unsigned char       m_cTakePos;
        unsigned char       m_cXPos;
        unsigned char       m_cYPos;
        unsigned char       m_cZPos;
    };

    // ----------------------------------------------------------------------------

    // �α��ξƿ� ����
    struct ViewLoginoutLog : public PktBase
    {
        char            m_szClientID[MAX_NAME];
    };

    // �α��� �ƿ� ����
    struct ViewLoginoutLogAck : public PktBase 
    {
        char            m_szUserID[MAX_NAME];                   // ���� ID
        char            m_szIP[MAX_IP_LEN];                     // ���� IP
        char            m_szLoginDateTime[MAX_DATE_TIME_LEN];   // ���� �ð�
        char            m_szLogoutDateTime[MAX_DATE_TIME_LEN];  // ���� ���� �ð�

        unsigned long   m_dwUID;                                // UID
        unsigned long   m_dwCRMIndex1;                          // PC�� ID
        unsigned long   m_dwConnectedServerID;                  // ���ӵ� ���� ID
        
        unsigned char   m_cBillingType;                         // ����Ÿ��
    };

    // ----------------------------------------------------------------------------

    struct GetAdminDetailRight : public PktBase
    {
        char m_cRights;
    };

    struct SetAdminDetailRight : public PktBase
    {
        char m_cRights;
        char m_szRights[MAX_RIGHTS_NUM];
    };

    struct AdminDetailRightAck : public PktBase
    {        
        char m_cRights;
        char m_szRights[MAX_RIGHTS_NUM];
    };

    // ----------------------------------------------------------------------------
};

#pragma pack()

#endif