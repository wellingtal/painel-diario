#pragma once

#include <vector>

typedef struct IPBAND
{
	DWORD	start;
	DWORD	end;
}*LPIPBAND;

class NFIPSec
{
public:
	NFIPSec(void);
	~NFIPSec(void);

public:
	void LoadAllowIP(WCHAR* lpIPFileName);
	void LoadBlockIP(WCHAR* lpIPFileName);
	void LoadAllowIPZ(WCHAR* lpIPFileName);
	void LoadBlockIPZ(WCHAR* lpIPFileName);

	int SerializeOut(int iType, int iPos, int iCount, char* lpBuffer_Out);
	void SerializeIn(int iType, int iCount, char* lpBuffer_In);

	inline void ClearAllowIP() { m_vecAllowIP.clear(); }
	inline void ClearBlockIP() { m_vecBlockIP.clear(); }

	BOOL IsAliveIP(const char * strIP);
	BOOL IsAliveIP(unsigned long dwIP);

	BOOL CheckAllowIP(const char * strIP);
	BOOL CheckAllowIP(unsigned long dwIP);

	BOOL CheckBlockIP(const char * strIP);
	BOOL CheckBlockIP(unsigned long dwIP);

	inline int GetAllowIPCount() { return (int)m_vecAllowIP.size(); }
	inline int GetBlockIPCount() { return (int)m_vecBlockIP.size(); }

private:
	std::vector<IPBAND> m_vecAllowIP;
	std::vector<IPBAND> m_vecBlockIP;

};
