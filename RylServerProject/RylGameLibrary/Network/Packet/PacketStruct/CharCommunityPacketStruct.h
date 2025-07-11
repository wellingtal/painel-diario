#ifndef _CHAR_COMMUNITY_PACKET_STRUCT_H_
#define _CHAR_COMMUNITY_PACKET_STRUCT_H_

#pragma pack(1)

#include <DB/DBDefine.h>

// ��Ƽ ã�� ����ü
typedef struct MemberFindParty* LPMemberFindParty;
struct MemberFindParty 
{
	char m_strName[CHAR_INFOST::MAX_NAME_LEN];       // �̸�
	unsigned char m_cLevel;         // ����    
	unsigned char m_cClass;         // Ŭ����
};

// ��Ƽ�� ã�� ����ü
typedef struct PartyFindMember* LPPartyFindMember;
struct PartyFindMember 
{
	char m_strLeaderName[CHAR_INFOST::MAX_NAME_LEN];     // ���� �̸�

	unsigned char m_cAverageLevel;      // ��� ����
	unsigned char m_cMemberNum;         // ��� ��
};

#pragma pack()

#endif