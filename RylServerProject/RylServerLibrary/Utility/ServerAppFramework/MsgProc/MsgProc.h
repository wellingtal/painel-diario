#ifndef _GM_NETWORK_MSG_PROC_
#define _GM_NETWORK_MSG_PROC_

#include <winsock2.h>
#include <windows.h>
#include <map>

class CMsgProc
{
public:
	virtual ~CMsgProc() = 0;
	virtual LRESULT operator () (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
};

inline CMsgProc::~CMsgProc() { }

class CMsgProcessMgr
{
public:

	CMsgProcessMgr();
	~CMsgProcessMgr();

	bool Register(UINT uMsg, CMsgProc* lpMsgProcessProc);
	bool Remove(UINT uMsg);

	CMsgProc* FindMsgProc(UINT uMsg);

	void Clear();

private:

	typedef std::map<UINT, CMsgProc*> MessageProcessMap;

	MessageProcessMap	m_MessageProcessMap;
};

#endif