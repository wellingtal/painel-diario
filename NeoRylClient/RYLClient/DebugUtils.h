#ifndef _DEBUG_UTILS_H_
#define _DEBUG_UTILS_H_

#include <winsock2.h>
#include <windows.h>
#include <tchar.h>

#include <Pattern/Singleton.h>

namespace DbgUtils
{
	class   CConvertErrorToText;
	void    SetProgramName(PTSTR pszOutBuffer, int nBufferSize, PTSTR pszProgamName = NULL);	
};


class DbgUtils::CConvertErrorToText : public CStaticSingleton<DbgUtils::CConvertErrorToText>
{
public:

	DWORD GetErrorTextBuffer(LPTSTR lpMessage, DWORD dwBufferLen, DWORD dwLastError);
    
private:

    CConvertErrorToText();
    ~CConvertErrorToText();
    friend class CStaticSingleton<DbgUtils::CConvertErrorToText>;

	HMODULE	m_hNetMsg;
};

#endif