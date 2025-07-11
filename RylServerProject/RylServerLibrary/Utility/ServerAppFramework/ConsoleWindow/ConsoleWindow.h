#ifndef _CONSOLE_WINDOW_H_
#define _CONSOLE_WINDOW_H_

#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <Thread/Lock.h>
#include <boost/pool/pool.hpp>

// 전방 참조
class CCommandProcess;
class CConsoleCMDFactory;

class CConsoleWindow
{
public:

    enum Const
    {
        MAX_MESSAGE_SIZE = 8192,
        MAX_WINDOW_NAME  = 128
    };

	CConsoleWindow(HINSTANCE hInstance, HWND hParentWnd,
        CConsoleCMDFactory& CMDFactory, CCommandProcess& CMDProcess);
	virtual ~CConsoleWindow();

	bool Initialize(const char* szWindowName);
	bool Destroy();

    CCommandProcess& GetCMDProcess() { return m_CMDProcess; }
    CConsoleCMDFactory& GetConsoleCMDFactory() { return m_CMDFactory; }

	void PrintOutput(const TCHAR* lpFormat, ...);    
    void PrintInfo(const TCHAR* lpFormat, ...);
	
private:

    void CreateCommand();

	// static members
	static LRESULT CALLBACK	ConsoleWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK InputWindowProc(HWND hWnd, WORD msg, WORD wParam, LONG lParam);

	static const UINT   ms_PrintOutputMsg;
	static const UINT   ms_PrintInfoMsg;
    static const TCHAR* ms_this;

    typedef CCSLock     ConsoleWindowLock;

	ConsoleWindowLock   m_ConsoleWindowLock;
    CACHE_PAD(Padding, sizeof(ConsoleWindowLock));
    
    HWND        m_hParentWnd;
    HWND		m_hWnd;
	HWND		m_hWndInfo;
	HWND		m_hWndInput;
	HWND		m_hWndOutput;

	HFONT		m_hFont;
	HINSTANCE	m_hInstance;
	LONG_PTR	m_fOldProc;

    char        m_szWindowName[MAX_WINDOW_NAME];

    CCommandProcess&	m_CMDProcess;
    CConsoleCMDFactory& m_CMDFactory;

	boost::pool<>		m_MsgPool;
};

#endif