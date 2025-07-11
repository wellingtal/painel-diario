#ifndef _GAMA_INET_ADDR_
#define _GAMA_INET_ADDR_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>

class INET_Addr
{
public:

    INET_Addr() : m_iAddrLen(0) { memset(&m_SockAddr, 0, sizeof(SOCKADDR)); }
    INET_Addr(const char* addr, unsigned short port) { set_addr(addr, port); }
    INET_Addr(IN_ADDR addr, unsigned short port) { set_addr(addr, port); }
    INET_Addr(sockaddr& addr, int size) { set_addr(addr, size); }
    ~INET_Addr() { }

	void clear() { m_iAddrLen = 0; memset(&m_SockAddr, 0, sizeof(SOCKADDR)); }

    void set_addr(const char* addr, const unsigned short port);
    void set_addr(const IN_ADDR addr, const unsigned short port);
    void set_addr(const sockaddr& addr, const int size);
    
    // Winsock함수 사용을 위해서 const를 사용하지 않았음.
    sockaddr& get_addr() { return m_SockAddr; }

    const sockaddr_in& get_addr_in() const { return reinterpret_cast<const sockaddr_in&>(m_SockAddr); }
	unsigned short get_port_in() const { return ntohs(reinterpret_cast<const sockaddr_in&>(m_SockAddr).sin_port); }
	const char* get_addr_string() const { return inet_ntoa(reinterpret_cast<const sockaddr_in&>(m_SockAddr).sin_addr); }

    int  get_size() const { return m_iAddrLen;  }

    int* get_size_ptr()   { return &m_iAddrLen; }
    void set_size(int n)  { m_iAddrLen = n; }

	static int get_addr_buffer_size() { return sizeof(sockaddr); }

private:

    sockaddr    m_SockAddr;
    int         m_iAddrLen;    
};


#endif