#include "stdafx.h"
#include "SysTray.h"

#include <shellapi.h>
#include <stdio.h>


// constructor and destructor
CSysTray::CSysTray(HWND hWnd, HINSTANCE hInstance)
:	m_hWnd(hWnd), m_hInstance(hInstance), m_uIconCount(0)
{
	memset(&m_IconData, 0, sizeof(NOTIFYICONDATA));
}

CSysTray::~CSysTray()
{
	RemoveIcon();
}


// add item
BOOL CSysTray::AddIcon(const char *lpToolTip, HICON hIcon, UINT uID)
{
	BOOL res;

	if(0 != m_IconData.cbSize)
	{
		RemoveIcon();
	}
	
	m_IconData.cbSize			= sizeof(NOTIFYICONDATA); 
	m_IconData.hWnd				= m_hWnd;
	m_IconData.uID				= uID;
	m_IconData.uFlags			= NIF_MESSAGE|NIF_ICON|NIF_TIP; 
	m_IconData.hIcon			= hIcon;
	m_IconData.uCallbackMessage = GetSysTrayNotifyMsg(); 

	if (lpToolTip)
	{
		_snprintf(m_IconData.szTip, 64, "%s", lpToolTip);
	}
	else
	{
		_snprintf(m_IconData.szTip, 64, "Temp Application");
	}

	res = Shell_NotifyIcon(NIM_ADD, &m_IconData); 

	if(res)
	{
		m_uIconCount++;
	}

	return res;
}


// remove item
BOOL CSysTray::RemoveIcon(void)
{
    BOOL res = Shell_NotifyIcon(NIM_DELETE, &m_IconData);

    //decrement the counter
	if (res)
	{
		if(m_IconData.hIcon)
		{
			DestroyIcon(m_IconData.hIcon);
		}

		--m_uIconCount;
	
		memset(&m_IconData, 0, sizeof(NOTIFYICONDATA));
	}

    return res;
}

BOOL CSysTray::Update()
{
	if(0 != m_IconData.cbSize)
	{
		return Shell_NotifyIcon(NIM_MODIFY, &m_IconData);
	}

	return FALSE;
}

// show popup menu
void CSysTray::ShowPopupMenu(HWND hWnd, HMENU hMenu)
{
    POINT mouse;
    GetCursorPos(&mouse);

	SetForegroundWindow(hWnd);
    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, mouse.x, mouse.y, 0, hWnd, NULL);
	SetForegroundWindow(hWnd);
}

const UINT CSysTray::GetSysTrayNotifyMsg()
{
	static const UINT s_MsgID = RegisterWindowMessage(TEXT("CSysTrayNotifyMessage"));
	return s_MsgID;
}
