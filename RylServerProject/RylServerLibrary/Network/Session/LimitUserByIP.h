#ifndef _GM_NETWORK_LIMIT_SESSION_BY_IP_
#define _GM_NETWORK_LIMIT_SESSION_BY_IP_

#include <vector>

#include <Thread/Lock.h>
#include <Network/Listener/Listener.h>

class CLimitUserByIP : public CValidateConnection
{
public:

	enum AllowMode_t
	{
		ALLOW_ALL,
		DENY_ALL,
		ALLOW_SOME,
	};

	CLimitUserByIP(const char* szFileName);
	virtual ~CLimitUserByIP();
	virtual bool operator () (INET_Addr& localAddr, INET_Addr& remoteAddr);

	bool LoadAllowIPList(const char* szFileName);
	void ClearIPList();

	void OperateMode(AllowMode_t eAllowMode);
	AllowMode_t OperateMode();

    void ReserveAllowIP(size_t nAllowIPNum) { m_AllowIPList.reserve(nAllowIPNum); }

    bool AddAllowIP(unsigned long dwIP);
    bool AddAllowIP(INET_Addr& address);
    bool AddAllowIP(in_addr addr);

    bool RemoveAllowIP(unsigned long dwIP);
    bool RemoveAllowIP(INET_Addr& address);
    bool RemoveAddAllowIP(in_addr addr);

private:

	typedef CCSLock						LimitLock;
	typedef std::vector<unsigned long>  IPList;

	LimitLock	m_LimitLock;
	IPList		m_AllowIPList;
	AllowMode_t	m_eAllowMode;
};


#endif