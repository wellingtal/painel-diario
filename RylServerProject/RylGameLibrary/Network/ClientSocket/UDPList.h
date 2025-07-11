///////////////////////////////////////////////////////////////////////////////////////////////
//
// UDPList
//
///////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GAMA_CLIENT_UDP_ADDRESS_LIST_H_
#define _GAMA_CLIENT_UDP_ADDRESS_LIST_H_

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <winsock.h>
#include <list>

enum PEERTYPE
{
	Not_Defined = 0,												// ���� �ȵ�
	REAL_IP,														// ���� IP
	NAT_Friendly,													// ���� IP (���� NAT)
	NAT_Different													// ���� IP (�ٸ� NAT)
};

typedef struct UDP_LIST* LPUDP_LIST;
struct UDP_LIST
{
	DWORD			CharID;											// ĳ���� ���̵�

	PEERTYPE		PeerType;										// ���� Ÿ��
	SOCKADDR_IN		PublicAddress;									// �ּ� Public
	SOCKADDR_IN		PrivateAddress;									// �ּ� Private

	DWORD			MoveTick;										// ������ �ð�
};

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Ŭ���� ����
//
///////////////////////////////////////////////////////////////////////////////////////////////
class UDPList
{
public:

	typedef std::list<UDP_LIST*>    STDLIST;

	UDPList();
	~UDPList();

	DWORD InsertIntoAddressList(DWORD CharID_In, const SOCKADDR_IN& PubAddress_In, 
        const SOCKADDR_IN& PriAddress_In, PEERTYPE PeerType_In);

	DWORD InsertIntoAddressList(UDP_LIST* lpUDP_List);

	bool DeleteFromAddressList(DWORD CharID_In);
	bool DeleteFromAddressList(const SOCKADDR_IN& Address_In);
	void DeleteAllFromList();

    bool   IsListEmpty() { return m_UDP_List.empty(); }
    size_t GetListCount() { return m_UDP_List.size(); }

	LPUDP_LIST SearchFromAddressList(DWORD CharID_In);
	LPUDP_LIST SearchFromAddressList(const SOCKADDR_IN& Address_In);

    void SetGarbageStackLength(size_t StackLength_In);
    void ClearGarbageStack(void);

	STDLIST::iterator begin() { return m_UDP_List.begin(); }
	STDLIST::iterator end() { return m_UDP_List.end(); }

protected:

    STDLIST m_UDP_List;
    STDLIST m_GarbageList;

    size_t  m_nMaxGarbageStackLength;
};


#endif