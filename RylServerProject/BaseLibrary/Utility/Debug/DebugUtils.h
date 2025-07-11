#ifndef _DEBUG_UTILS_H_
#define _DEBUG_UTILS_H_

#include <tchar.h>

namespace DbgUtils
{
	class CConvertErrorToText;
	void SetProgramName(TCHAR* pszOutBuffer, int nBufferSize, TCHAR* pszProgamName = 0);
};

class DbgUtils::CConvertErrorToText
{
public:

	unsigned long GetErrorTextBuffer(TCHAR* lpMessage, 
		unsigned long dwBufferLen, unsigned long dwLastError);
    
	static CConvertErrorToText& GetInstance();

private:

    CConvertErrorToText();
    ~CConvertErrorToText();

	HMODULE	m_hNetMsg;
};

#endif