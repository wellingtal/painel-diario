#include "stdafx.h"
#include "ConsoleCMDFactory.h"
#include <BaseLibrary/Utility/Math/Math.h>

CConsoleCMDFactory::StringCMD::StringCMD(const TCHAR* szCommand, CConsoleCommand* lpCMD)
:   m_szCommand(szCommand), m_lpCMD(lpCMD), 
    m_dwHashValue(Math::HashFunc::sdbmHash(reinterpret_cast<const unsigned char*>(szCommand)))
{
    
}

 

CConsoleCMDFactory::CConsoleCMDFactory()
{


}

CConsoleCMDFactory::~CConsoleCMDFactory()
{
    for(CMDVector::iterator itr = m_CMDVector.begin();
        itr != m_CMDVector.end(); ++itr)
    {
        delete itr->m_lpCMD;
    }

    m_CMDVector.clear();
}


bool CConsoleCMDFactory::AddCommand(const TCHAR* szCommand, CConsoleCommand* lpConsoleCommand)
{
    if(0 == szCommand, 0 == lpConsoleCommand)
    {
        return false;    
    }

    m_CMDVector.push_back(StringCMD(szCommand, lpConsoleCommand));
    return true;
}


// Desc : 주는 문자열로 커맨드 생성.
CConsoleCommand* CConsoleCMDFactory::Create(const TCHAR* szCommand, size_t nCommandLength)
{
    // 첫번째 WhiteSpace가 나오는 지점까지의 거리를 구한다.
    size_t nIndex = 0;
    for (; nIndex < nCommandLength; ++nIndex)
    {
        if (TEXT('\0') == szCommand[nIndex] || TEXT(' ') == szCommand[nIndex] || 
			TEXT('\t') == szCommand[nIndex] || TEXT('\n') == szCommand[nIndex])
        {
            break;
        }
    }

    const int MAX_BUFFER = 256;
    char szCommandOnly[MAX_BUFFER];

    if(nIndex < MAX_BUFFER)
    {
        memcpy(szCommandOnly, szCommand, nIndex);
        szCommandOnly[nIndex] = 0;

        for (CMDVector::iterator itr = m_CMDVector.begin();
            itr != m_CMDVector.end(); ++itr)
        {
            StringCMD& stringCMD = *itr;
     
            if (0 == _tcscmp(stringCMD.m_szCommand, szCommandOnly))
            {
                return stringCMD.m_lpCMD->Clone(szCommand, nCommandLength);
            }
        }
    }
   
    return NULL;
}
