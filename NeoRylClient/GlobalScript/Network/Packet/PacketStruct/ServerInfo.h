#ifndef _LOGIN_SERVER_INFO_
#define _LOGIN_SERVER_INFO_

#include <winsock2.h>
#include <windows.h>
#include <algorithm>

#pragma pack(1)


// -----------------------------------------------------------------------------
// ���� ���� ���� ����ü�� 
typedef union SERVER_ID* LPSERVER_ID; 
union SERVER_ID
{
	// ������
	enum Group	
	{ 
		GROUP_TEST			= 0,	// �׽�Ʈ
		GROUP_SOLPHISTIA	= 1,	// ���ǽ�Ƽ��
		GROUP_KARTERANT		= 2,	// ī���׶�Ʈ
		GROUP_IRANSA		= 3,	// �̶���
		GROUP_HIGHLAND		= 4,	// ���̷���
		GROUP_ALDIA			= 5,	// �˵��
		GROUP_UNITE			= 6,	// ������
		GROUP_BATTLE_SERVER	= 7,	// ��Ʋ ����
		GROUP_US_TOURNAMENT	= 8,	// ��ʸ�Ʈ (�̱���)

        // Rodin : MAX_GROUP_NUM�� 8�� �ϸ� �α� ������ ������� �׽��ϴ�. ��?!?!
		MAX_GROUP_NUM		= 10
	}; 

	enum Zone	
	{ 
		NONE			= 0,
        ZONE1			= 1,		// �׷��� �ڽ�Ʈ
		ZONE2			= 2,		// ���� ����
		ZONE3			= 3,		// X �� ��Ʋ�׶��� (���� ������)
		ZONE4			= 4,		// ���̳� ���Ϸ���
		ZONE5			= 5,		// ���Ϸ��� ���� ����
		BATTLE_ZONE		= 6,		// X ��Ʋ ��
		ADMIN_TOOL_ZONE = 7,		// X (Ŭ���̾�Ʈ �������� ������ ����) - ����� ����ϰڼ�.
		CAPITAL			= 8,		// ī����
		ZONE9			= 9,		// X �� NEWZONE5 ���ε� �۷ι����� ����ؾ��ϱ� ������ ZONE9�� ������ ����
        FRAG_ZONE		= 10,		// X ��Ʋ �׶��� ���� ������ ���� ��
		BATTLE_SERVER	= 11,		// X ��Ʋ �׶��� ���� ����
        ZONE12          = 12,       // ���� ����
		STATUE_ZONE		= 13,		// X ��Ʋ �׶��� ���� ������ ���� ��
		PRISON			= 14,		// ���� (���� �ý���)
		CHAR_SELECT		= 15,		// X ĳ���� ���� ȭ�� ��
		STONE_WAR1		= 16,		// �ο� ��������
		STONE_WAR2		= 17,		// �ο� ��������
		STONE_WAR3		= 18,		// �ο� ��������

		MAX_ZONE_NUM	= 19
	};

	enum Channel
	{
        // sphawk : ä�� �����ؼ� ���ƽ��ϴ�. �������� �����ϰ� ����� ������ �Ժη� �ǵ��� ������.
        // ��ģ ��Ŷ�� ������ �����ϴ�.
        // 1. ����.
        // 2. PktSCInfo     (�߰��ʿ����� ����ϴ� ��Ŷ�Դϴ�)
        
        // 3. PktCSAck
        // 4. PktSZMvAck

        // 5. PktCSDAck     (�߰��ʿ����� ����ϴ� ��Ŷ�Դϴ�)
        // 6. PktSZMvDAck   (�߰��ʿ����� ����ϴ� ��Ŷ�Դϴ�)

        // SERVER_GROUP���� ä���̶�� �� ������ �����δ� �޸�/��ĭ �� �ο����Դϴ�.(�߰� �ҽ��� ���� �дϴ�.)
        // ���� �����ñ� �ٶ��ϴ�.
		MAX_CHANNEL_NUM	= 5
	};

	typedef struct SID* LPSID;
    struct SID
	{
		char Type;
		char Group;
		char Channel;
		char ID;
	};

	SID				sID;
	unsigned long	dwID;

    inline char GetType(void)       { return sID.Type;    }
	inline char GetChannel(void)	{ return sID.Channel; }
	inline char GetZone(void)		{ return sID.ID;	  }
	inline char GetGroup(void)		{ return sID.Group;   }
};


typedef struct SERVER* LPSERVER_GROUP;
struct SERVER_GROUP
{
	enum { CHANNEL_NUM = 2, SERVER_NAME = 15 };
	
	char			m_Group;					// ���� �׷�
	char			m_Name[SERVER_NAME];		// ���� �̸�
 	long	        m_ClientNum[CHANNEL_NUM];	// ���� �� ��
	IN_ADDR         m_Address;					// ���� �ּ�
};

typedef struct SERVER* LPSERVER_LIST;
struct SERVER_LIST
{
	enum { SERVER_NUM = 10 };

	unsigned short		m_ServerNum;						// ���� ����
	SERVER_GROUP		m_ServerGroup[SERVER_NUM];			// ���� �׷�
};


#pragma pack()



typedef struct RylServerInfo* LPRylServerInfo;
struct RylServerInfo
{
    enum
    {
        GROUP_NAME_LENGTH       = 120,
        PATCH_ADDRESS_LENGTH    = 100
    };

	SERVER_ID		m_ServerUID;
    IN_ADDR         m_ServerAddress;
    unsigned long	m_dwClientVer;
    unsigned long   m_dwChecksum;
    
    size_t          m_nGroupNameLen;
    size_t          m_nPatchAddressLen;

    char            m_szGroupName[GROUP_NAME_LENGTH];        
    char            m_szPatchAddress[PATCH_ADDRESS_LENGTH];

    unsigned short	m_usChannelClientNum[SERVER_GROUP::CHANNEL_NUM];
    unsigned short	m_usChannelNum;

    RylServerInfo::RylServerInfo()
        :   m_dwClientVer(0), m_dwChecksum(0), 
            m_nPatchAddressLen(0), m_usChannelNum(0)
    {
        m_ServerUID.dwID = 0;
        m_ServerAddress.S_un.S_addr = 0;

        m_szPatchAddress[0] = '\0';

        strncpy(m_szGroupName, "UnKnown", GROUP_NAME_LENGTH);
        m_nGroupNameLen = strlen(m_szGroupName);

        std::fill_n(m_usChannelClientNum, int(SERVER_GROUP::CHANNEL_NUM), 0);
    }
};



#endif