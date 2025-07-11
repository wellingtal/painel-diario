#ifndef _CHAR_CONFIG_PACKET_H_
#define _CHAR_CONFIG_PACKET_H_

// CharConfigPacket.h
#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketStruct/DataPacket.h>

#include "CharConfigPacketStruct.h"

#pragma pack(1)


typedef struct PktConfigInfo* LPPktConfigInfo;
struct PktConfigInfo : public PktDD
{
	enum { MAX_BUFFER_SIZE = 50 };

	unsigned long   m_dwUserID;
	unsigned long   m_dwCharID;

	unsigned short	m_wSize;
};

// �ɼ� ����
typedef struct PktCOp* LPPktCOp;
struct PktCOp : public PktBase 
{
	unsigned long	m_dwCharID;         // ĳ���� ���̵�
	RejectOption	m_RejectOption;     // �ź� �ɼ� ��Ʈ�V
};

// ���� ��ȭ
typedef struct PktChatInfoChange* LPPktChatInfoChange;  
struct PktChatInfoChange : public PktBase
{
	unsigned long   m_dwUID;
	unsigned long   m_dwCID;

	unsigned long   m_dwPID;
	unsigned long   m_dwGID;
	RejectOption    m_RejectOption;
	unsigned char   m_bPeaceMode;
};


#pragma pack()

#endif