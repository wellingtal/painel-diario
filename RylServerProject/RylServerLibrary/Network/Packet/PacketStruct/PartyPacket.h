#ifndef _PARTY_INFO_PACKET_H_
#define _PARTY_INFO_PACKET_H_

#include "DataPacket.h"
#include <DB/DBdefine.h>

#pragma pack(1)

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ��Ƽ ����
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktPID* LPPktPID;
struct PktPID : public PktDD 
{
	unsigned long		m_dwCharID;	
	unsigned long		m_dwPartyID;

	unsigned long		m_dwServerID;
	unsigned long		m_dwGID;
	unsigned short		m_wClass;
	char				m_cLevel;
};

typedef struct PktPIDAck* LPPktPIDAck;
struct PktPIDAck : public PktDD 
{
	PARTY				m_Party;

	unsigned long		m_dwCharID;

	unsigned long		m_dwServerID;
	unsigned long		m_dwGID;
	unsigned short		m_wClass;
	char				m_cLevel;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ��Ƽ ����
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktCPD* LPPktCPD;
struct PktCPD : public PktDD 
{
	unsigned long				m_dwLeaderID;
	unsigned long				m_dwMemberID;

	unsigned long				m_dwLeaderGID;
	unsigned short				m_wLeaderClass;
	char						m_cLeaderLevel;

	unsigned long				m_dwMemberGID;
	unsigned short				m_wMemberClass;
	char						m_cMemberLevel;	
};

typedef struct PktCPDAck* LPPktCPDAck;
struct PktCPDAck : public PktDD
{
	PARTY						m_Party;

	unsigned long				m_dwLeaderID;
	unsigned long				m_dwMemberID;

	unsigned long				m_dwLeaderGID;
	unsigned short				m_wLeaderClass;
	char						m_cLeaderLevel;

	unsigned long				m_dwMemberGID;
	unsigned short				m_wMemberClass;
	char						m_cMemberLevel;

	unsigned char				m_cZone;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ��Ƽ ����
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktDPD* LPPktDPD;
struct PktDPD : public PktDD 
{
	unsigned long		m_dwPartyID;	
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ��Ƽ ��� ����
//
//							
//	��� �α���		:		CID/CID					0/SID
//	��� �α׾ƿ�	:		CID/CID					0/���� ���̵�(0 �ϰ�� ���� �絵 ����)
//	��� ����		:		CID/CID					CID/CID
//	��� ����		:		CID/CID					CID/���� ���̵�(0 �� ��� ���� �絵 ����)
//
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct PktPMD* LPPktPMD;
struct PktPMD : public PktDD 
{
	enum PktPMDErr
	{
		NO_SERVER_ERR		= 0,	
		SERVER_ERROR		= 1,
		FAIL_WRONG_PARTY	= 2
	};

	unsigned long		m_dwPartyID;	
	unsigned long		m_dwSenderID;
	
	unsigned long		m_dwGID;				// ��� ���̵�(���� Reference �� ���).
	unsigned long		m_dwServerID;			// ���� ���̵�.
	unsigned short		m_wClass;				// Ŭ����.
	char				m_cLevel;				// ����.

	char	            m_strSenderName[CHAR_INFOST::MAX_NAME_LEN];	
};


struct PartyMemberData
{
	// ��æƮ ������ ��� CreatureStructure.h�� EnchantInfo�� ��ȭ�� ����� ������ �ʿ��մϴ�.

    enum ChangedData
    {
        CHANGED_XPOS        	= ( 1 << 0 ),
        CHANGED_YPOS        	= ( 1 << 1 ),
        CHANGED_ZPOS        	= ( 1 << 2 ),
        CHANGED_MAX_HP      	= ( 1 << 3 ),
        CHANGED_MAX_MP      	= ( 1 << 4 ),
        CHANGED_CUR_HP      	= ( 1 << 5 ),
        CHANGED_CUR_MP      	= ( 1 << 6 ),
        CHANGED_LEVEL       	= ( 1 << 7 ),
        CHANGED_CLASS       	= ( 1 << 8 ),

		CHANGED_ENCHANT_INFO_0	= ( 1 << 9 ),
		CHANGED_ENCHANT_INFO_1	= ( 1 << 10 ),
		CHANGED_ENCHANT_INFO_2	= ( 1 << 11 ),
		CHANGED_ENCHANT_INFO_3	= ( 1 << 12 )
    };

	enum Const
	{
		MAX_ENCHANT_INFO_NUM	= 4
	};

    unsigned long   m_dwCID;            // ĳ���� CID

    float           m_fXPos;            // X��ġ
    float           m_fYPos;            // Y��ġ
    float           m_fZPos;            // Z��ġ

    unsigned short  m_usMaxHP;          // �ִ� HP
    unsigned short  m_usMaxMP;          // �ִ� MP
    unsigned short  m_usCurHP;          // ���� HP
    unsigned short  m_usCurMP;          // ���� MP

    unsigned char   m_cLevel;           // ĳ���� ����
    unsigned char   m_cClass;           // ĳ���� Ŭ����

	unsigned long	m_dwEnchantInfo[MAX_ENCHANT_INFO_NUM];	// ��æƮ ����

    unsigned short  m_usDataChanged;    // ������ ���� ����
};

#pragma pack()


#endif