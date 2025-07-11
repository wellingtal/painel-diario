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

// Singleton Ŀ�ǵ� �ν��Ͻ�. ( �ΰ� Ŀ�ǵ尡 ���� ��쿡 ���. )
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

// �ܼ� Ŀ�ǵ� ���丮.
class CConsoleCMDFactory
{
public:

    enum
    {
        MAX_CMD_BUFFER = 512        // �ִ� Ŀ�ǵ� ���� ũ��.
    };

    CConsoleCMDFactory();
    virtual ~CConsoleCMDFactory();

    // Desc : Ŀ�ǵ带 �߰��Ѵ�. szCommand�� �ݵ�� ���ڿ� ���ͷ� ����̿��߸� �Ѵ�.    
    bool AddCommand(const TCHAR* szCommand, CConsoleCommand* lpConsoleCommand);

    // Desc : Ŀ�ǵ尡 ���Ŀ��� �ʿ��ϸ�, ������ ������ ���� ��.    
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