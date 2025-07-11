#include "stdafx.h"
#include "ConsoleWindow.h"

#include <windowsx.h>
#include <Pattern/Command.h>
#include "ConsoleCMDFactory.h"

const TCHAR*        WINCONSOLE_CLASSNAME        = TEXT("RowConsoleWindow");//was RowConsoleWindow hayzohar
const TCHAR*		WINCONSOLE_FONTNAME			= TEXT("Arial");

const int			WINCONSOLE_FONTSIZE			= 10;				// font height임. ^^;;
const unsigned int	WINCONSOLE_WIDTH			= 480;
const unsigned int	WINCONSOLE_HEIGHT			= 640;

const unsigned int	WINCONSOLE_INFO_HEIGHT		= 450;
const unsigned int	WINCONSOLE_INPUT_HEIGHT		= 24;
const unsigned int	WINCONSOLE_OUTPUT_HEIGHT	= WINCONSOLE_HEIGHT - WINCONSOLE_INFO_HEIGHT - WINCONSOLE_INPUT_HEIGHT-30;
const unsigned int	WINCONSOLE_X				= (GetSystemMetrics(SM_CXSCREEN) - WINCONSOLE_WIDTH - 20);
const unsigned int	WINCONSOLE_Y				= (20);

const unsigned int	MAX_OUTPUT_COUNT			= 50;

const UINT      CConsoleWindow::ms_PrintOutputMsg	= RegisterWindowMessage(TEXT("ConsoleWindowCustomPrintOutput"));
const UINT      CConsoleWindow::ms_PrintInfoMsg		= RegisterWindowMessage(TEXT("ConsoleWindowCustomPrintInfo"));
const TCHAR*    CConsoleWindow::ms_this			    = TEXT("RowConsoleWindow");// was RowConsoleWindow hayzohar


CConsoleWindow::CConsoleWindow(HINSTANCE hInstance, HWND hParentWnd,
                               CConsoleCMDFactory& CMDFactory, CCommandProcess& CMDProcess)
:	m_hParentWnd(hParentWnd), m_hWnd(0), 
    m_hWndInfo(0), m_hWndInput(0), m_hWndOutput(0),
	m_hFont(0), m_fOldProc(0), m_hInstance(hInstance), 
    m_CMDFactory(CMDFactory), m_CMDProcess(CMDProcess),
	m_MsgPool(MAX_MESSAGE_SIZE)
{
    memset(m_szWindowName, 0, sizeof(char) * MAX_WINDOW_NAME);
}


CConsoleWindow::~CConsoleWindow()
{
	Destroy();
}


bool CConsoleWindow::Initialize(const char* szWindowName)
{
    ConsoleWindowLock::Syncronize sync(m_ConsoleWindowLock);

    if(0 != m_hWnd)
    {
        // 이미 생성되어 있으면, 더 생성하지 않는다.
        return true;
    }

    strncpy(m_szWindowName, szWindowName, MAX_WINDOW_NAME - 1);
    m_szWindowName[MAX_WINDOW_NAME - 1] = 0;

	WNDCLASS wc;

	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= (HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.hCursor			= LoadCursor(0, IDC_ARROW);
	wc.hIcon			= LoadIcon(0, IDI_APPLICATION);
	wc.hInstance		= m_hInstance;
	wc.lpfnWndProc		= CConsoleWindow::ConsoleWindowProc;
	wc.lpszClassName	= WINCONSOLE_CLASSNAME;
	wc.lpszMenuName		= 0;
	wc.style			= CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wc))
	{
		return false;
	}

	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, WINCONSOLE_CLASSNAME, m_szWindowName, WS_VISIBLE, 
		WINCONSOLE_X, WINCONSOLE_Y, WINCONSOLE_WIDTH, WINCONSOLE_HEIGHT, m_hParentWnd, 0, m_hInstance, 0);

    if(0 == m_hWnd)
    {
        return false;
    }

    if(FALSE == SetProp(m_hWnd, ms_this, reinterpret_cast<HANDLE>(this)))
    {
        return false;
    }

	m_hWndInfo = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "information ====================================", 
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_READONLY | ES_MULTILINE,
		0, 0, WINCONSOLE_WIDTH - 10, WINCONSOLE_INFO_HEIGHT, m_hWnd, 0, m_hInstance, 0);

    if(0 == m_hWndInfo)
    {
        return false;
    }

	m_hWndOutput = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOSEL, 
		0, WINCONSOLE_INFO_HEIGHT, WINCONSOLE_WIDTH - 10, WINCONSOLE_OUTPUT_HEIGHT,
		m_hWnd, 0, m_hInstance, 0);

    if(0 == m_hWndOutput)
    {
        return false;
    }

	m_hWndInput = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE, 0, 
		WINCONSOLE_INFO_HEIGHT + WINCONSOLE_OUTPUT_HEIGHT-10, WINCONSOLE_WIDTH - 10, 
		WINCONSOLE_INPUT_HEIGHT, m_hWnd, 0, m_hInstance, 0);

    if(0 == m_hWndInput)
    {
        return false;
    }

    m_fOldProc = (LONG_PTR)SetWindowLongPtr(m_hWndInput, GWLP_WNDPROC, 
        (LONG)((LONG_PTR)CConsoleWindow::InputWindowProc));

    if(FALSE == SetProp(m_hWndInput, ms_this, reinterpret_cast<HANDLE>(this)))
    {
        return false;
    }

    //subclass the edit control		
	if ((m_hFont = CreateFont(-(WINCONSOLE_FONTSIZE), 0, // Height, Font-width
								0, 0,		// Angle
								FW_NORMAL,	// Weight
								FALSE,		// Italic
								FALSE,		// Underline
								FALSE,		// ...
								DEFAULT_CHARSET,
								OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY,			// DEFAULT_PITCH | FF_DONTCARE,
								FIXED_PITCH | FF_DONTCARE,
								WINCONSOLE_FONTNAME)) == 0)
	{
		return false;
	}

	// 비동기방식으로 메시지 큐에 저장하기 위해서 SendMessage 를 PostMessage 로 바꿈. 2002.09.07
	PostMessage(m_hWndInfo,	  WM_SETFONT, (WPARAM)m_hFont, (LPARAM)MAKELPARAM(FALSE, 0));
	PostMessage(m_hWndOutput, WM_SETFONT, (WPARAM)m_hFont, (LPARAM)MAKELPARAM(FALSE, 0));
	PostMessage(m_hWndInput,  WM_SETFONT, (WPARAM)m_hFont, (LPARAM)MAKELPARAM(FALSE, 0));

	return true;
}


#define SAFE_DESTROY_WND(hWnd) if(0 != (hWnd)) { DestroyWindow((hWnd)); (hWnd) = 0; }

bool CConsoleWindow::Destroy()
{
    ConsoleWindowLock::Syncronize sync(m_ConsoleWindowLock);

    if (m_hFont) 
	{
        DeleteObject((HGDIOBJ)m_hFont);
        m_hFont = 0;
    }

    SAFE_DESTROY_WND(m_hWndInfo);
    SAFE_DESTROY_WND(m_hWndInput);
    SAFE_DESTROY_WND(m_hWndOutput);
    SAFE_DESTROY_WND(m_hWnd);

	UnregisterClass(WINCONSOLE_CLASSNAME, m_hInstance);
	return true;
}

void CConsoleWindow::PrintOutput(const TCHAR* lpFormat, ...)
{	
    ConsoleWindowLock::Syncronize sync(m_ConsoleWindowLock);

    if(0 != m_hWnd)
    {
		TCHAR* szText = reinterpret_cast<TCHAR*>(m_MsgPool.malloc());
		if(0 != szText)
		{
            va_list	arguments;
            va_start(arguments, lpFormat);
            int nLength = _vsntprintf(szText, MAX_MESSAGE_SIZE - 1, lpFormat, arguments);
            va_end(arguments);
			
			if(0 < nLength)
			{
				szText[nLength] = 0;
				PostMessage(m_hWnd, ms_PrintOutputMsg, (WPARAM)szText, 0);
			}
		}
    }
}

void CConsoleWindow::PrintInfo(const TCHAR* lpFormat, ...)
{
    ConsoleWindowLock::Syncronize sync(m_ConsoleWindowLock);

    if(0 != m_hWnd)
    {
		TCHAR* szText = reinterpret_cast<TCHAR*>(m_MsgPool.malloc());
		if(0 != szText)
		{
            va_list	arguments;
            va_start(arguments, lpFormat);
            int nLength = _vsntprintf(szText, MAX_MESSAGE_SIZE - 1, lpFormat, arguments);
            va_end(arguments);

            if(0 < nLength)
            {
                szText[nLength] = 0;
                PostMessage(m_hWnd, ms_PrintInfoMsg, (WPARAM)szText, 0);
			}
		}
    }
}


LRESULT CALLBACK CConsoleWindow::ConsoleWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CConsoleWindow* lpConsoleWindow = 
        reinterpret_cast<CConsoleWindow*>(GetProp(hWnd, ms_this));

	if(0 == lpConsoleWindow )
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	if(msg == ms_PrintOutputMsg && 0 != wParam)
	{        
		TCHAR* szText = reinterpret_cast<TCHAR*>(wParam);

		HWND hWndOutput = lpConsoleWindow->m_hWndOutput;

        int nEndIndex = ListBox_GetCount(hWndOutput);
		if(nEndIndex > MAX_OUTPUT_COUNT) 
		{		
			ListBox_DeleteString(hWndOutput, 0);
		}

		ListBox_AddString(hWndOutput, szText);
		ListBox_SetTopIndex(hWndOutput, nEndIndex - 1);

		ConsoleWindowLock::Syncronize sync(lpConsoleWindow->m_ConsoleWindowLock);
        lpConsoleWindow->m_MsgPool.free(szText);
	}
	else if (msg == ms_PrintInfoMsg && 0 != wParam)
	{
		TCHAR* szText = reinterpret_cast<TCHAR*>(wParam);
		
		HWND hWndInfo = lpConsoleWindow->m_hWndInfo;

		SetWindowText(hWndInfo, szText);
	    UpdateWindow(hWndInfo);
		
        ConsoleWindowLock::Syncronize sync(lpConsoleWindow->m_ConsoleWindowLock);
        lpConsoleWindow->m_MsgPool.free(szText);
	}
	else
	{
        switch (msg) 
		{
		case WM_CLOSE:
			SetWindowLongPtr(lpConsoleWindow->m_hWndInput, GWLP_WNDPROC, (LONG)lpConsoleWindow->m_fOldProc);
			PostQuitMessage(0);
			break;

		case WM_CTLCOLORLISTBOX :
			SetBkColor((HDC) wParam, GetSysColor(COLOR_BTNFACE));
			return (LRESULT) GetSysColorBrush(COLOR_BTNFACE);

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}

	return 0;
}


LRESULT CALLBACK CConsoleWindow::InputWindowProc(HWND hWnd, WORD msg, WORD wParam, LONG lParam)
{
    CConsoleWindow* lpConsoleWindow = 
        reinterpret_cast<CConsoleWindow*>(GetProp(hWnd, ms_this));

	if(0 == lpConsoleWindow || 0 == lpConsoleWindow->m_fOldProc)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	else if (WM_CHAR == msg)
	{
		if (VK_RETURN == wParam) 
		{
            lpConsoleWindow->CreateCommand();
    		return 0;
		}
	}

	return (LONG)CallWindowProc((WNDPROC)lpConsoleWindow->m_fOldProc, hWnd, msg, wParam, lParam);	
}


void CConsoleWindow::CreateCommand()
{
    TCHAR szCommand[CConsoleCMDFactory::MAX_CMD_BUFFER];

    int nLength = GetWindowText(m_hWndInput, szCommand, CConsoleCMDFactory::MAX_CMD_BUFFER);
    if(0 < nLength)
    {
        CConsoleCommand* lpCommand = m_CMDFactory.Create(szCommand, nLength);
        if(0 != lpCommand)
        {
            PrintOutput(szCommand);
            m_CMDProcess.Add(lpCommand);
        }
        else
        {			
            PrintOutput(TEXT("Can't recognize command. Please reinput."));
        }    
    }

    SetWindowText(m_hWndInput, "");
}
