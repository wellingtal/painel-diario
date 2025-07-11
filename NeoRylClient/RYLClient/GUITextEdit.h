// GUITextEdit.h: interface for the CGUITextEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUITEXTEDIT_H__5B84E354_3FE6_43BF_BAA4_9C5AF226EB9F__INCLUDED_)
#define AFX_GUITEXTEDIT_H__5B84E354_3FE6_43BF_BAA4_9C5AF226EB9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include <d3dx8.h>
#include <deque>

#define MAX_CHATMESSAGE			100
#define MAX_DRAWCHAT			5
#define MAX_DRAWSTALLCHAT		10

#define EDITMODE_NONE			0
#define EDITMODE_CREATECHAR		1
#define EDITMODE_CHAT			2
#define EDITMODE_NUMBER			3
#define EDITMODE_WHISPER		4
#define EDITMODE_DEPOSITPASS	5
#define EDITMODE_CURPASS		6
#define EDITMODE_NEWPASS		7
#define EDITMODE_STALL			8
#define EDITMODE_LOGINID		9
#define EDITMODE_LOGINPASS		10

#define CHATMODE_CHAT		0
#define CHATMODE_PARTY		1
#define CHATMODE_GUILD		3	

using namespace std;

class CGUITextEdit  
{
	protected:
		unsigned long m_dwTextSize[129];
		char m_strMessage[MAX_PATH];
		unsigned long m_dwLimitLength;
		long m_lCarretPosition;
		LOGFONT m_LogFont;
		HWND m_hEditBox;

	public:
		void InitEnglishMode(void);
		char * MakeWordBreak(const char *strString, unsigned long dwWidth);
		char * MakePrintGold(unsigned long dwGold);
		typedef struct Message
		{
			unsigned long dwCommand;
			float fTimer;
			char strMessage[MAX_PATH];
			unsigned long dwColor;
		} Message;

		void AddNotice(char *strMessage, unsigned short wCmd);
		unsigned long GetChatColor(unsigned long dwCommand);
		deque<Message *> m_deqChatMessage;
		deque<Message *> m_deqPartyMessage;
		deque<Message *> m_deqGuildMessage;
		deque<Message *> m_deqNoticeMessage;
		deque<Message *> m_deqStallMessage;

		unsigned long m_dwChatMode;
		unsigned long m_dwStartChat;
		unsigned long m_dwStartParty;
		unsigned long m_dwStartGuild;
		unsigned long m_dwStartStall;
		LPD3DXFONT m_lpText;
		unsigned long m_dwEditMode;
		
		CGUITextEdit();
		~CGUITextEdit();

		void DeleteStallMessage(void);
		void Create(HWND hWnd, LPDIRECT3DDEVICE8 lpD3DDevice);
		void AddNormalChatMessage(char *strMessage, unsigned long dwSender, unsigned long dwCommand);
		void AddPartyChatMessage(char *strMessage, unsigned long dwSender, unsigned long dwCommand);
		void AddGuildChatMessage(char *strMessage, unsigned long dwSender, unsigned long dwCommand);
		void AddMessage(char *strMessage, unsigned long dwSender, unsigned long dwCommand);
		unsigned long GetStringLength(const char *strString);
		void SetEditLength(WPARAM wParam);
		void InitMessage(void);
		void SetWindowText(void)
		{
			char strMessage[256];
			int length = ::GetWindowTextLength(m_hEditBox);
			if(length)
			{
				if(length < 255)
				{
					::GetWindowText(m_hEditBox, strMessage, 256);
					strcpy(m_strMessage, strMessage);
				} else
				{
					::SetWindowText(m_hEditBox, NULL);
					strcpy(m_strMessage, "");
				}
			} else
				strcpy(m_strMessage, "");
			//::GetWindowText(m_hEditBox, m_strMessage, MAX_PATH);
		}
		void SetWindowText(char *strString) { ::SetWindowText(m_hEditBox, strString); }
		char *GetMessage(void) { return m_strMessage; }
		void SetFocus(void) { ::SetFocus(m_hEditBox); }
		HWND GetHwnd(void) { return m_hEditBox; }
		BOOL IsFocus(void) { if(GetFocus() == m_hEditBox) return TRUE; else return FALSE; }
		void DrawText(const char *pString, RECT *pRect, unsigned long Format, D3DCOLOR Color) ;

		VOID GetRenderStateEx() ;
};

extern CGUITextEdit g_TextEdit;

#endif // !defined(AFX_GUITEXTEDIT_H__5B84E354_3FE6_43BF_BAA4_9C5AF226EB9F__INCLUDED_)
