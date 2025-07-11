#ifndef _MY_SYSTRAY_H_
#define _MY_SYSTRAY_H_

// 2002년 9월 3일
// 변동호
#pragma once

#include <winsock2.h>
#include <windows.h>
#include <shellapi.h>


class CSysTray
{
private:

	NOTIFYICONDATA	m_IconData;

	HWND		m_hWnd;
    HINSTANCE	m_hInstance;
	UINT		m_uIconCount;
	
public:

	// constructor and destructor
	CSysTray(HWND hWnd, HINSTANCE hInstance);
	~CSysTray();

	BOOL AddIcon(const char *lpToolTip, HICON hIcon, UINT uID);
    BOOL RemoveIcon();
	BOOL Update();
	
    void ShowPopupMenu(HWND hWnd, HMENU hMenu);

	static const UINT GetSysTrayNotifyMsg();
};

#endif