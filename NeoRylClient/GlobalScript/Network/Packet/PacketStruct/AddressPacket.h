#ifndef _ADDRESS_PACKET_H_
#define _ADDRESS_PACKET_H_

// AddressPacket.h
#include <Network/Packet/PacketBase.h>

#pragma pack(1)

// UDP �ּ� ���� ������Ʈ ( Update Address )
//	: Ŭ���̾�Ʈ�� �α��� �Ŀ� UDP�� ������ �ѹ� �����ش�. 
//	: ������ Ŭ���̾�Ʈ�� ��Ŷ���� �ܺ� UDP �ּҸ� ��´�.
typedef struct PktUA* LPPktUA;
struct PktUA : public PktBase
{
	SOCKADDR_IN		m_PrivateAddress;
    unsigned long	m_dwCharID;
};

// UDP �ּ� ���� ������Ʈ Ack
typedef struct PktUAAck* LPPktUAAck;
struct PktUAAck : public PktBase
{
	SOCKADDR_IN		m_PublicAddress;
	SOCKADDR_IN		m_PrivateAddress;
	unsigned long	m_dwCharID;	
};


// �ּ� ����ü
typedef struct AddressInfo* LPAddressInfo;
struct AddressInfo
{
	SOCKADDR_IN		m_PublicAddress;
	SOCKADDR_IN		m_PrivateAddress;
	unsigned long	m_dwCharID;

    AddressInfo() : m_dwCharID(0)
    {
        memset(&m_PublicAddress, 0, sizeof(m_PublicAddress));
        memset(&m_PrivateAddress, 0, sizeof(m_PrivateAddress));
    }

    AddressInfo(const SOCKADDR_IN& PublicAddress, const SOCKADDR_IN& PrivateAddress, const unsigned long dwCharID)
        :   m_PublicAddress(PublicAddress), m_PrivateAddress(PrivateAddress), m_dwCharID(dwCharID) { }
};

// ���ͳ� �ּ� ����ü
typedef struct INETAddressInfo* LPINETAddressInfo;
struct INETAddressInfo
{
    unsigned long   m_dwCharID;
    unsigned long   m_dwPublicAddr;
    unsigned long   m_dwPrivateAddr;
    unsigned short  m_usPublicPort;
    unsigned short  m_usPrivatePort;

    INETAddressInfo() { m_dwCharID = m_dwPublicAddr = m_dwPrivateAddr = m_usPublicPort = m_usPrivatePort = 0; }

    SOCKADDR_IN GetPublicAddress()
    {
        SOCKADDR_IN PublicAddress;
        memset(&PublicAddress, 0, sizeof(SOCKADDR_IN));

        PublicAddress.sin_family            = PF_INET;
        PublicAddress.sin_addr.S_un.S_addr  = m_dwPublicAddr;
        PublicAddress.sin_port              = m_usPublicPort;

        return PublicAddress;
    }

    SOCKADDR_IN GetPrivateAddress()
    {
        SOCKADDR_IN PrivateAddress;
        memset(&PrivateAddress, 0, sizeof(SOCKADDR_IN));

        PrivateAddress.sin_family            = PF_INET;
        PrivateAddress.sin_addr.S_un.S_addr  = m_dwPrivateAddr;
        PrivateAddress.sin_port              = m_usPrivatePort;

        return PrivateAddress;
    }
};


// UDP �ּ� ���� ��Ŷ		( Address Info )
//	- ��巹�� ����ü
typedef struct PktAI* LPPktAI;
struct PktAI : public PktBase
{
    AddressInfo     m_AddressInfo;
};				

#pragma pack()

#endif