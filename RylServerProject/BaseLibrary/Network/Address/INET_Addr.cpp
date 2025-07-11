#include "stdafx.h"
#include "INET_Addr.h"
#include "GMMemory.h"

void INET_Addr::set_addr(const char* addr, const unsigned short port)
{
    memset(&m_SockAddr, 0, sizeof(sockaddr));
    sockaddr_in& addr_in = *reinterpret_cast<sockaddr_in*>(&m_SockAddr);
    
    addr_in.sin_family      = AF_INET;
    addr_in.sin_addr.s_addr	= (NULL == addr) ? htonl(INADDR_ANY) : inet_addr(addr);
    addr_in.sin_port		= (0	== port) ? 0 : htons(port);

    m_iAddrLen = sizeof(sockaddr_in);
}


void INET_Addr::set_addr(const sockaddr& addr, const int size)
{   
    m_SockAddr = addr;
    m_iAddrLen = size;
}


void INET_Addr::set_addr(const IN_ADDR addr, const unsigned short port)
{
    memset(&m_SockAddr, 0, sizeof(sockaddr));
    sockaddr_in& addr_in = *reinterpret_cast<sockaddr_in*>(&m_SockAddr);

    addr_in.sin_family  = AF_INET;
    addr_in.sin_addr    = addr;
    addr_in.sin_port	= (0	== port) ? 0 : htons(port);

    m_iAddrLen = sizeof(sockaddr_in);
}

