#ifndef _CONSOLE_COMMAND_FACTORY_H_
#define _CONSOLE_COMMAND_FACTORY_H_

#pragma once

#include <winsock2.h>
#include <windows.h>

#include <Pattern/Command.h>

#include <vector>

class CConsoleCommand : public CCommand
{
protected:
    
    virtual CConsoleCommand* Clone(const TCHAR* szCommand, size_t nCommandLength) = 0;
    friend class CConsoleCMDFactory;
};

// Singleton 커맨드 인스턴스. ( 부가 커맨드가 없는 경우에 사용. )
template<typename Derived>
class CConsoleCMDSingleton : public CConsoleCommand
{
public:
	inline virtual CConsoleCommand* Clone(const TCHAR* szCommand, size_t nCommandLength);
	virtual bool Destroy() { return true; }
};

template<typename Derived>
inline CConsoleCommand* CConsoleCMDSingleton<Derived>::Clone(const TCHAR* szCommand, size_t nCommandLength)
{
    static Derived Instance;
    return &Instance;
}

// 콘솔 커맨드 팩토리.
class CConsoleCMDFactory
{
public:

    enum
    {
        MAX_CMD_BUFFER = 512        // 최대 커맨드 버퍼 크기.
    };

    CConsoleCMDFactory();
    virtual ~CConsoleCMDFactory();

    // Desc : 커맨드를 추가한다. szCommand는 반드시 문자열 리터럴 상수이여야만 한다.    
    bool AddCommand(const TCHAR* szCommand, CConsoleCommand* lpConsoleCommand);

    // Desc : 커맨드가 차후에도 필요하면, 별도로 저장해 놓을 것.    
    CConsoleCommand* Create(const TCHAR* szCommand, size_t nCommandLength);

protected:

    struct StringCMD
    {
        unsigned long       m_dwHashValue;
        const TCHAR*        m_szCommand;        
        CConsoleCommand*    m_lpCMD;
        
        StringCMD(const TCHAR* szCommand, CConsoleCommand* lpCMD);            
    };

    typedef std::vector<StringCMD> CMDVector;
    CMDVector                      m_CMDVector;
};


#endif