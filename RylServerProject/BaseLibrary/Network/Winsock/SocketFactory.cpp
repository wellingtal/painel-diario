#include "stdafx.h"

#include <winsock2.h>
#include <windows.h>
#include <wsipx.h>
#include <cassert>

#include "SocketFactory.h"
//#include "../../Utility/Math/Math.h"


CSocketFactory::CSocketFactory(int nSocketFamily, int nSocketType, int nSocketProtocol, int nAddressLen)
:   m_nSockFamily(nSocketFamily), m_nSockType(nSocketType), m_nSockProtocol(nSocketProtocol), 
    m_nAddressLen(nAddressLen)
{

}


CINETFamilyFactory::CINETFamilyFactory(int nSocketType, int nSocketProtocol)
:   CSocketFactory(AF_INET, nSocketType, nSocketProtocol, sizeof(SOCKADDR_IN))
{

}


CTCPFactory::CTCPFactory()
:   CINETFamilyFactory(SOCK_STREAM, IPPROTO_TCP)
{

}


CUDPFactory::CUDPFactory()
:   CINETFamilyFactory(SOCK_DGRAM, IPPROTO_UDP)
{

}


/*
CIPXFamilyFactory::CIPXFamilyFactory(int nSocketType, int nSocketProtocol)
:   CSocketFactory(AF_IPX, nSocketType, nSocketProtocol, sizeof(SOCKADDR_IPX))
{

}


CSPXFactory::CSPXFactory()
:   CIPXFamilyFactory(SOCK_STREAM, NSPROTO_SPX)
{

}


CIPXFactory::CIPXFactory()
:   CIPXFamilyFactory(SOCK_DGRAM, NSPROTO_IPX)
{

}
*/


SOCKET CSocketFactory::CreateSocket()
{
	SOCKET hSocket = WSASocket(m_nSockFamily, m_nSockType, m_nSockProtocol, 
        NULL, 0, WSA_FLAG_OVERLAPPED);

    return hSocket;
}


SOCKET CSocketFactory::CreateConnectedSocket(const char* lpConnAddr, unsigned short usPort)
{
	SOCKADDR sockAddr;
	if (!SetAddr(&sockAddr, lpConnAddr, usPort))
	{
		return INVALID_SOCKET;
	}

	SOCKET connectedSocket = CreateSocket();
	if (INVALID_SOCKET != connectedSocket)
	{
        if (SOCKET_ERROR == connect(connectedSocket, &sockAddr, m_nAddressLen))
	    {
            closesocket(connectedSocket);
            connectedSocket = INVALID_SOCKET;
        }
    }

	return connectedSocket;
}


SOCKET CSocketFactory::CreateBindedSocket(const char* lpBindAddr, unsigned short usPort)
{
	SOCKADDR sockAddr;
	if (!SetAddr(&sockAddr, lpBindAddr, usPort))
	{
		return INVALID_SOCKET;
	}	

	SOCKET bindedSocket = CreateSocket();
	if(INVALID_SOCKET != bindedSocket)
	{
	    BOOL bReuseAddr	= TRUE;	    
	    if (SOCKET_ERROR == setsockopt(bindedSocket, SOL_SOCKET, SO_REUSEADDR, 
            (char *)&bReuseAddr, sizeof(bReuseAddr)))
        {
            closesocket(bindedSocket);
            bindedSocket = INVALID_SOCKET;
        }
        else
        {
            if(SOCKET_ERROR == bind(bindedSocket, &sockAddr, m_nAddressLen))
            {
                closesocket(bindedSocket);
                bindedSocket = INVALID_SOCKET;
            }
        }
    }

	return bindedSocket;
}

SOCKET CSocketFactory::CreateListenSocket(const char* lpListenAddr, unsigned short usPort, int nBackLog)
{    
    nBackLog = min(nBackLog, MAX_BACKLOG);

	// bind server to Server address
	SOCKET sdListen = CreateBindedSocket(lpListenAddr, usPort);
	if (INVALID_SOCKET != sdListen) 
	{
	    // start listening	    
        if (SOCKET_ERROR == listen(sdListen, nBackLog))
	    {
		    closesocket(sdListen);
		    sdListen = INVALID_SOCKET;
	    }
    }

    return sdListen;
}


bool CINETFamilyFactory::SetAddr(LPSOCKADDR lpSA, const char* szAddr, unsigned short usPort)
{		
	int	nAddrLen = sizeof(SOCKADDR_IN);
	
	memset(lpSA, 0, nAddrLen);
    
    LPSOCKADDR_IN lpSA_in = reinterpret_cast<LPSOCKADDR_IN>(lpSA);

    if (0 != szAddr) 
	{
		lpSA_in->sin_family				= m_nSockFamily;
		lpSA_in->sin_addr.S_un.S_addr	= inet_addr(szAddr);
		lpSA_in->sin_port				= htons(usPort);

		if(0 == lpSA_in->sin_addr.S_un.S_addr)
		{
			return false;
		}
	} 
	else 
	{
		lpSA_in->sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	}	

    lpSA_in->sin_family = m_nSockFamily;
	lpSA_in->sin_port = htons(usPort);
	return true;
}


bool CINETFamilyFactory::GetNetworkInfo(char *Address_Out, int nMaxBufferSize)
{
    const int MAX_HOSTNAME_BUFFER = 256;
	char szHostName[MAX_HOSTNAME_BUFFER];
 	if(!gethostname(szHostName, MAX_HOSTNAME_BUFFER - 1))
	{
        szHostName[MAX_HOSTNAME_BUFFER - 1] = '\0';

		PHOSTENT pHost = gethostbyname(szHostName);
		if(NULL == pHost)
		{
			return false;
		}

		IN_ADDR Addr = {0,};
		int Count = 0;
		for(Count = 0;pHost->h_addr_list[Count]; ++Count)
		{
			memcpy(&Addr, pHost->h_addr_list[Count], sizeof(IN_ADDR));
			strncpy(Address_Out, inet_ntoa(Addr), nMaxBufferSize - 1);
            Address_Out[nMaxBufferSize - 1] = '\0';

			// 가상 아이피 확인
			// 10.0.0.0		~ 10.255.255.255
			// 172.16.0.0	~ 172.31.255.255
			// 192.168.0.0	~ 192.168.255.255
			if((unsigned char)10 == Addr.S_un.S_un_b.s_b1)
			{
				continue;
			}
			else if((unsigned char)172 == Addr.S_un.S_un_b.s_b1)
			{
				if(Addr.S_un.S_un_b.s_b2 >= (unsigned char)16 && Addr.S_un.S_un_b.s_b2 <= (unsigned char)31)
				{
					continue;
				}
			}
			else if((unsigned char)192 == Addr.S_un.S_un_b.s_b1)
			{
				if((unsigned char)168 == Addr.S_un.S_un_b.s_b2)
				{
					continue;
				}
			}

			return true;
		}

		if(0 != Count)
		{
            return true;
		}
	}

	return false;
}


bool CTCPFactory::SetLinger(SOCKET hSocket, bool bOn, unsigned short usTimeOut)
{
    linger socketLinger;

    socketLinger.l_onoff = bOn ? 1 : 0;
	socketLinger.l_linger = usTimeOut;
	if(SOCKET_ERROR == setsockopt(hSocket, SOL_SOCKET, SO_LINGER, 
        (char *)&socketLinger, sizeof(linger)))
	{		
		return false;
	}

    return true;
}

/*
bool CIPXFamilyFactory::SetAddr(LPSOCKADDR lpSA, char *szAddr, unsigned short usPort)
{
	if(0 == szAddr)	{	return false;	}

	// get the offset for node number / network number separator
	LPSTR lpszNodeNum = strchr(szAddr, '.');
	if (0 == lpszNodeNum) 
	{		
		return false;
	}

	++lpszNodeNum;

    LPSOCKADDR_IPX lpSA_ipx = reinterpret_cast<LPSOCKADDR_IPX>(lpSA);
	lpSA_ipx->sa_family = m_nSockFamily;

    Math::Convert::AcToHe(szAddr,(char*)&lpSA_ipx->sa_netnum, 4);
	Math::Convert::AcToHe(lpszNodeNum,(char*)&lpSA_ipx->sa_nodenum, 6);
	lpSA_ipx->sa_socket = htons(usPort);

	return true;
}


bool CIPXFamilyFactory::GetNetworkInfo(char *Address_Out, int nMaxBufferSize)
{

	return true;
}
*/

