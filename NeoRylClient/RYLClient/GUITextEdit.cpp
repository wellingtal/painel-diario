// GUITextEdit.cpp: implementation of the CGUITextEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "RYLClientMain.h"
#include "RYLObjectControl.h"
#include "GUITextEdit.h"
#include <string>
#include "BaseGraphicsLayer.h"
#include <imm.h>

#include "RYLNetworkData.h"
#include "SoundMgr.h"
#include "GMMemory.h"

CGUITextEdit g_TextEdit;
WNDPROC OldEditProc;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUITextEdit::CGUITextEdit()
{
	m_lpText = NULL;
	m_hEditBox = NULL;
	m_lCarretPosition = 0;
	m_dwStartChat = 0;
	m_dwStartParty = 0;
	m_dwStartGuild = 0;
	m_dwStartStall = 0;
	m_dwEditMode = EDITMODE_NONE;
	m_dwChatMode = CHATMODE_CHAT;
}

CGUITextEdit::~CGUITextEdit()
{
	deque<Message *>::iterator it;
	for(it = m_deqChatMessage.begin(); it != m_deqChatMessage.end(); it++)
	{
		delete (*it);
	}
	m_deqChatMessage.clear();

	for(it = m_deqPartyMessage.begin(); it != m_deqPartyMessage.end(); it++)
	{
		delete (*it);
	}
	m_deqPartyMessage.clear();

	for(it = m_deqGuildMessage.begin(); it != m_deqGuildMessage.end(); it++)
	{
		delete (*it);
	}
	m_deqGuildMessage.clear();

	for(it = m_deqStallMessage.begin(); it != m_deqStallMessage.end(); it++)
	{
		delete (*it);
	}
	m_deqStallMessage.clear();

	if(m_hEditBox)
	{
		SetWindowLong(m_hEditBox, GWL_WNDPROC, (long)OldEditProc); 
		DestroyWindow(m_hEditBox);
		m_hEditBox = NULL;
	}
	if(m_lpText) { m_lpText->Release(); m_lpText = NULL; }
}

unsigned long CGUITextEdit::GetStringLength(const char *strString)
{
	unsigned long i, len = strlen(strString), countx = 0;
	for(i = 0; i < len; i++)
	{
		if(strString[i] & 0x80) { countx += m_dwTextSize[128]; i++; } else { countx += m_dwTextSize[strString[i]]; }
	}

	return countx;
}

LRESULT APIENTRY EditProc(HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_GETDLGCODE)
		return DLGC_WANTALLKEYS;

 	switch(uMsg)
	{
		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_RETURN:
					if(g_TextEdit.m_dwEditMode == EDITMODE_CHAT)
					{
						::SetFocus(g_ClientMain.m_hWnd);
					} else if(g_TextEdit.m_dwEditMode == EDITMODE_CURPASS)
					{
						g_TextEdit.m_dwEditMode = EDITMODE_NEWPASS;
						g_TextEdit.InitMessage();
					} else if(g_TextEdit.m_dwEditMode == EDITMODE_NEWPASS)
					{
						g_TextEdit.m_dwEditMode = EDITMODE_CURPASS;
						g_TextEdit.InitMessage();
					} else if(g_TextEdit.m_dwEditMode == EDITMODE_LOGINID)
					{
						g_TextEdit.m_dwEditMode = EDITMODE_LOGINPASS;
						g_TextEdit.InitEnglishMode();
						g_TextEdit.InitMessage();
					}/* else if(g_TextEdit.m_dwEditMode == EDITMODE_LOGINPASS)
					{
						g_TextEdit.m_dwEditMode = EDITMODE_LOGINID;
						g_TextEdit.InitEnglishMode();
						g_TextEdit.InitMessage();
					}*/
					return 0;

				case VK_TAB:
					if(g_TextEdit.m_dwEditMode == EDITMODE_LOGINID)
					{
						g_TextEdit.m_dwEditMode = EDITMODE_LOGINPASS;
						g_TextEdit.InitEnglishMode();
						g_TextEdit.InitMessage();
					} else if(g_TextEdit.m_dwEditMode == EDITMODE_LOGINPASS)
					{
						g_TextEdit.m_dwEditMode = EDITMODE_LOGINID;
						g_TextEdit.InitEnglishMode();
						g_TextEdit.InitMessage();
					}
					return 0;

				case VK_ESCAPE:
					return 0;
			}
			break;

		case WM_CHAR:
			switch(wParam)
			{
				case VK_TAB:
				case VK_RETURN:
				case VK_ESCAPE:
					return 0;
			}
			break;
	}

	return CallWindowProc(OldEditProc, hWnd, uMsg, wParam, lParam);
}

void CGUITextEdit::Create(HWND hWnd, LPDIRECT3DDEVICE8 lpD3DDevice)
{
	m_dwTextSize[0] = 0;		m_dwTextSize[1] = 6;		m_dwTextSize[2] = 6;		m_dwTextSize[3] = 6;
	m_dwTextSize[4] = 0;		m_dwTextSize[5] = 0;		m_dwTextSize[6] = 0;		m_dwTextSize[7] = 0;
	m_dwTextSize[8] = 0;		m_dwTextSize[9] = 0;		m_dwTextSize[10] = 0;		m_dwTextSize[11] = 0;
	m_dwTextSize[12] = 0;		m_dwTextSize[13] = 0;		m_dwTextSize[14] = 0;		m_dwTextSize[15] = 0;
	m_dwTextSize[16] = 0;		m_dwTextSize[17] = 0;		m_dwTextSize[18] = 0;		m_dwTextSize[19] = 0;
	m_dwTextSize[20] = 0;		m_dwTextSize[21] = 0;		m_dwTextSize[22] = 0;		m_dwTextSize[23] = 0;
	m_dwTextSize[24] = 0;		m_dwTextSize[25] = 0;		m_dwTextSize[26] = 0;		m_dwTextSize[27] = 0;
	m_dwTextSize[28] = 0;		m_dwTextSize[29] = 0;		m_dwTextSize[30] = 0;		m_dwTextSize[31] = 0;

	m_dwTextSize[32] = 4;		m_dwTextSize[33] = 4;		m_dwTextSize[34] = 4;		m_dwTextSize[35] = 6;
	m_dwTextSize[36] = 6;		m_dwTextSize[37] = 10;		m_dwTextSize[38] = 8;		m_dwTextSize[39] = 4;
	m_dwTextSize[40] = 5;		m_dwTextSize[41] = 5;		m_dwTextSize[42] = 6;		m_dwTextSize[43] = 6;
	m_dwTextSize[44] = 4;		m_dwTextSize[45] = 6;		m_dwTextSize[46] = 4;		m_dwTextSize[47] = 6;
	m_dwTextSize[48] = 6;		m_dwTextSize[49] = 6;		m_dwTextSize[50] = 6;		m_dwTextSize[51] = 6;
	m_dwTextSize[52] = 6;		m_dwTextSize[53] = 6;		m_dwTextSize[54] = 6;		m_dwTextSize[55] = 6;
	m_dwTextSize[56] = 6;		m_dwTextSize[57] = 6;		m_dwTextSize[58] = 4;		m_dwTextSize[59] = 4;
	m_dwTextSize[60] = 8;		m_dwTextSize[61] = 6;		m_dwTextSize[62] = 8;		m_dwTextSize[63] = 6;
	m_dwTextSize[64] = 12;		m_dwTextSize[65] = 8;		m_dwTextSize[66] = 8;		m_dwTextSize[67] = 9;
	m_dwTextSize[68] = 8;		m_dwTextSize[69] = 8;		m_dwTextSize[70] = 7;		m_dwTextSize[71] = 9;
	m_dwTextSize[72] = 8;		m_dwTextSize[73] = 3;		m_dwTextSize[74] = 4;		m_dwTextSize[75] = 8;
	m_dwTextSize[76] = 7;		m_dwTextSize[77] = 11;		m_dwTextSize[78] = 9;		m_dwTextSize[79] = 9;
	m_dwTextSize[80] = 8;		m_dwTextSize[81] = 9;		m_dwTextSize[82] = 8;		m_dwTextSize[83] = 8;
	m_dwTextSize[84] = 8;		m_dwTextSize[85] = 8;		m_dwTextSize[86] = 8;		m_dwTextSize[87] = 10;
	m_dwTextSize[88] = 8;		m_dwTextSize[89] = 8;		m_dwTextSize[90] = 8;		m_dwTextSize[91] = 6;
	m_dwTextSize[92] = 11;		m_dwTextSize[93] = 6;		m_dwTextSize[94] = 6;		m_dwTextSize[95] = 6;
	m_dwTextSize[96] = 4;		m_dwTextSize[97] = 7;		m_dwTextSize[98] = 7;		m_dwTextSize[99] = 7;
	m_dwTextSize[100] = 7;		m_dwTextSize[101] = 7;		m_dwTextSize[102] = 3;		m_dwTextSize[103] = 7;
	m_dwTextSize[104] = 7;		m_dwTextSize[105] = 3;		m_dwTextSize[106] = 3;		m_dwTextSize[107] = 6;
	m_dwTextSize[108] = 3;		m_dwTextSize[109] = 11;		m_dwTextSize[110] = 7;		m_dwTextSize[111] = 7;
	m_dwTextSize[112] = 7;		m_dwTextSize[113] = 7;		m_dwTextSize[114] = 4;		m_dwTextSize[115] = 7;
	m_dwTextSize[116] = 3;		m_dwTextSize[117] = 7;		m_dwTextSize[118] = 6;		m_dwTextSize[119] = 10;
	m_dwTextSize[120] = 7;		m_dwTextSize[121] = 7;		m_dwTextSize[122] = 7;		m_dwTextSize[123] = 6;
	m_dwTextSize[124] = 6;		m_dwTextSize[125] = 6;		m_dwTextSize[126] = 9;		m_dwTextSize[127] = 6;
	m_dwTextSize[128] = 12;

	HFONT hFont;
	hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	GetObject(hFont, sizeof(LOGFONT), (void*)&m_LogFont);
	strcpy(m_LogFont.lfFaceName, "돋음");
	D3DXCreateFontIndirect(BaseGraphicsLayer::GetDevice(), &m_LogFont, &m_lpText);

	m_hEditBox = CreateWindow(TEXT("edit"), NULL, WS_CHILD | ES_LEFT | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
				0, 0, 0, 0, hWnd, (HMENU)1, g_ClientMain.m_hInstance, NULL);
	OldEditProc = (WNDPROC)SetWindowLong(m_hEditBox, GWL_WNDPROC, (long)EditProc);
}

void CGUITextEdit::SetEditLength(WPARAM wParam)
{
	m_dwLimitLength = (unsigned long)wParam;
	PostMessage(m_hEditBox, EM_LIMITTEXT, wParam, 0);
}

void CGUITextEdit::InitMessage(void)
{
	m_lCarretPosition = 0;
	strcpy(m_strMessage, "");
	::SetWindowText(m_hEditBox, "");
	HIMC hImc = ImmGetContext(m_hEditBox);
	ImmSetCompositionString(hImc, SCS_SETSTR, "", 0, "", 0);
	ImmReleaseContext(m_hEditBox, hImc);
}

void CGUITextEdit::AddNormalChatMessage(char *strMessage, unsigned long dwSender, unsigned long dwCommand)
{
	bool bAdd = false;
	int i;
	unsigned long dwColor = GetChatColor(dwCommand);

	if(dwSender != CRYLNetworkData::Instance()->m_dwMyChrID)
	{
        RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwSender );
        if( pTempCreature )
        {
            CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
            if( pTempData )
            {
                pTempData->m_strChatMessage = strMessage;
            }

            pTempCreature->m_fChatTimer = 8000.0f;
        }
	}

	int len = strlen(strMessage), count = 0, countx = 0, start = 0;
	for(i = 0; i < len; i++)
	{
		if(strMessage[i] & 0x80)
		{
			countx += m_dwTextSize[128];
			count += 2;
			i++;
			if(countx >= 471) // 483 - 12 하나의 최고 폭
			{
				if(m_deqChatMessage.size() >= MAX_CHATMESSAGE)
				{
					delete (*m_deqChatMessage.begin());
					m_deqChatMessage.pop_front();
				}

				Message *lpMessage;
				lpMessage = new Message;
				memcpy(lpMessage->strMessage, &strMessage[start], count);
				lpMessage->strMessage[count] = '\0';
				lpMessage->dwCommand = dwCommand;
				lpMessage->fTimer = 8000.0f;
				lpMessage->dwColor = dwColor;
				m_deqChatMessage.push_back(lpMessage);

				if(m_deqChatMessage.size() > MAX_DRAWCHAT)
				{
					m_dwStartChat = m_deqChatMessage.size() - MAX_DRAWCHAT;
				}
				countx = count = 0;
				start = i + 1;
			}
		} else
		{
			countx += m_dwTextSize[strMessage[i]];
			count++;
			if(countx >= 475) // 483 - 8 하나의 최고 폭
			{
				if(m_deqChatMessage.size() >= MAX_CHATMESSAGE)
				{
					delete (*m_deqChatMessage.begin());
					m_deqChatMessage.pop_front();
				}

				Message *lpMessage;
				lpMessage = new Message;
				memcpy(lpMessage->strMessage, &strMessage[start], count);
				lpMessage->strMessage[count] = '\0';
				lpMessage->dwCommand = dwCommand;
				lpMessage->fTimer = 8000.0f;
				lpMessage->dwColor = dwColor;
				m_deqChatMessage.push_back(lpMessage);

				if(m_deqChatMessage.size() > MAX_DRAWCHAT)
				{
					m_dwStartChat = m_deqChatMessage.size() - MAX_DRAWCHAT;
				}
				countx = count = 0;
				start = i + 1;
			}
		}
	}
	if(start <= len)
	{
		if(m_deqChatMessage.size() >= MAX_CHATMESSAGE)
		{
			delete (*m_deqChatMessage.begin());
			m_deqChatMessage.pop_front();
		}

		Message *lpMessage;
		lpMessage = new Message;
		memcpy(lpMessage->strMessage, &strMessage[start], count);
		lpMessage->strMessage[count] = '\0';
		lpMessage->dwCommand = dwCommand;
		lpMessage->fTimer = 8000.0f;
		lpMessage->dwColor = dwColor;
		m_deqChatMessage.push_back(lpMessage);

		if(m_deqChatMessage.size() > MAX_DRAWCHAT)
		{
			m_dwStartChat = m_deqChatMessage.size() - MAX_DRAWCHAT;
		}
	}
}

void CGUITextEdit::AddPartyChatMessage(char *strMessage, unsigned long dwSender, unsigned long dwCommand)
{
	int i, len = strlen(strMessage), count = 0, countx = 0, start = 0;
	unsigned long dwColor = GetChatColor(dwCommand);

	for(i = 0; i < len; i++)
	{
		if(strMessage[i] & 0x80)
		{
			countx += m_dwTextSize[128];
			count += 2;
			i++;
			if(countx >= 471) // 483 - 12 하나의 최고 폭
			{
				if(m_deqPartyMessage.size() >= MAX_CHATMESSAGE)
				{
					delete (*m_deqPartyMessage.begin());
					m_deqPartyMessage.pop_front();
				}

				Message *lpMessage;
				lpMessage = new Message;
				memcpy(lpMessage->strMessage, &strMessage[start], count);
				lpMessage->strMessage[count] = '\0';
				lpMessage->dwCommand = dwCommand;
				lpMessage->fTimer = 8000.0f;
				lpMessage->dwColor = dwColor;
				m_deqPartyMessage.push_back(lpMessage);

				if(m_deqPartyMessage.size() > MAX_DRAWCHAT)
				{
					m_dwStartParty = m_deqPartyMessage.size() - MAX_DRAWCHAT;
				}
				countx = count = 0;
				start = i + 1;
			}
		} else
		{
			countx += m_dwTextSize[strMessage[i]];
			count++;
			if(countx >= 475) // 483 - 8 하나의 최고 폭
			{
				if(m_deqPartyMessage.size() >= MAX_CHATMESSAGE)
				{
					delete (*m_deqPartyMessage.begin());
					m_deqPartyMessage.pop_front();
				}

				Message *lpMessage;
				lpMessage = new Message;
				memcpy(lpMessage->strMessage, &strMessage[start], count);
				lpMessage->strMessage[count] = '\0';
				lpMessage->dwCommand = dwCommand;
				lpMessage->fTimer = 8000.0f;
				lpMessage->dwColor = dwColor;
				m_deqPartyMessage.push_back(lpMessage);

				if(m_deqPartyMessage.size() > MAX_DRAWCHAT)
				{
					m_dwStartParty = m_deqPartyMessage.size() - MAX_DRAWCHAT;
				}
				countx = count = 0;
				start = i + 1;
			}
		}
	}
	if(start <= len)
	{
		if(m_deqPartyMessage.size() >= MAX_CHATMESSAGE)
		{
			delete (*m_deqPartyMessage.begin());
			m_deqPartyMessage.pop_front();
		}

		Message *lpMessage;
		lpMessage = new Message;
		memcpy(lpMessage->strMessage, &strMessage[start], count);
		lpMessage->strMessage[count] = '\0';
		lpMessage->dwCommand = dwCommand;
		lpMessage->fTimer = 8000.0f;
		lpMessage->dwColor = dwColor;
		m_deqPartyMessage.push_back(lpMessage);

		if(m_deqPartyMessage.size() > MAX_DRAWCHAT)
		{
			m_dwStartParty = m_deqPartyMessage.size() - MAX_DRAWCHAT;
		}
	}
}

void CGUITextEdit::AddGuildChatMessage(char *strMessage, unsigned long dwSender, unsigned long dwCommand)
{
	int i, len = strlen(strMessage), count = 0, countx = 0, start = 0;
	unsigned long dwColor = GetChatColor(dwCommand);

	for(i = 0; i < len; i++)
	{
		if(strMessage[i] & 0x80)
		{
			countx += m_dwTextSize[128];
			count += 2;
			i++;
			if(countx >= 471) // 483 - 12 하나의 최고 폭
			{
				if(m_deqGuildMessage.size() >= MAX_CHATMESSAGE)
				{
					delete (*m_deqGuildMessage.begin());
					m_deqGuildMessage.pop_front();
				}

				Message *lpMessage;
				lpMessage = new Message;
				memcpy(lpMessage->strMessage, &strMessage[start], count);
				lpMessage->strMessage[count] = '\0';
				lpMessage->dwCommand = dwCommand;
				lpMessage->fTimer = 8000.0f;
				lpMessage->dwColor = dwColor;
				m_deqGuildMessage.push_back(lpMessage);

				if(m_deqGuildMessage.size() > MAX_DRAWCHAT)
				{
					m_dwStartGuild = m_deqGuildMessage.size() - MAX_DRAWCHAT;
				}
				countx = count = 0;
				start = i + 1;
			}
		} else
		{
			countx += m_dwTextSize[strMessage[i]];
			count++;
			if(countx >= 475) // 483 - 8 하나의 최고 폭
			{
				if(m_deqGuildMessage.size() >= MAX_CHATMESSAGE)
				{
					delete (*m_deqGuildMessage.begin());
					m_deqGuildMessage.pop_front();
				}

				Message *lpMessage;
				lpMessage = new Message;
				memcpy(lpMessage->strMessage, &strMessage[start], count);
				lpMessage->strMessage[count] = '\0';
				lpMessage->dwCommand = dwCommand;
				lpMessage->fTimer = 8000.0f;
				lpMessage->dwColor = dwColor;
				m_deqGuildMessage.push_back(lpMessage);

				if(m_deqGuildMessage.size() > MAX_DRAWCHAT)
				{
					m_dwStartGuild = m_deqGuildMessage.size() - MAX_DRAWCHAT;
				}
				countx = count = 0;
				start = i + 1;
			}
		}
	}
	if(start <= len)
	{
		if(m_deqGuildMessage.size() >= MAX_CHATMESSAGE)
		{
			delete (*m_deqGuildMessage.begin());
			m_deqGuildMessage.pop_front();
		}

		Message *lpMessage;
		lpMessage = new Message;
		memcpy(lpMessage->strMessage, &strMessage[start], count);
		lpMessage->strMessage[count] = '\0';
		lpMessage->dwCommand = dwCommand;
		lpMessage->fTimer = 8000.0f;
		lpMessage->dwColor = dwColor;
		m_deqGuildMessage.push_back(lpMessage);

		if(m_deqGuildMessage.size() > MAX_DRAWCHAT)
		{
			m_dwStartGuild = m_deqGuildMessage.size() - MAX_DRAWCHAT;
		}
	}
}

void CGUITextEdit::DeleteStallMessage(void)
{
	deque<Message *>::iterator it;
	for(it = m_deqStallMessage.begin(); it != m_deqStallMessage.end(); it++)
	{
		delete (*it);
	}
	m_deqStallMessage.clear();

	m_dwStartStall = 0;
}

void CGUITextEdit::AddMessage(char *strMessage, unsigned long dwSender, unsigned long dwCommand)
{
	switch(m_dwChatMode)
	{
		case CHATMODE_CHAT:
			AddNormalChatMessage(strMessage, dwSender, dwCommand);
			break;

		case CHATMODE_PARTY:
			AddPartyChatMessage(strMessage, dwSender, dwCommand);
			break;

		case CHATMODE_GUILD:
			AddGuildChatMessage(strMessage, dwSender, dwCommand);
			break;
	}
}

unsigned long CGUITextEdit::GetChatColor(unsigned long dwCommand)
{
	switch(dwCommand)
	{
		case 1:		// 파티 채팅
			return D3DCOLOR_RGBA(141, 180, 244, 255);

		case 3:
			return D3DCOLOR_RGBA(255, 0, 255, 255);

		case 7:		// 외치기
			return D3DCOLOR_RGBA(53, 218, 193, 255);

		case 8:		// 운영자 일반 대화
			return D3DCOLOR_RGBA(220, 220, 220, 255);

		case 9:		// 운영자 외치기
			return D3DCOLOR_RGBA(255, 128, 64, 255);

		case 10:	// 귓속말
			return D3DCOLOR_RGBA(255, 128, 239, 255);

		case 128:
			return D3DCOLOR_RGBA(100, 255, 100, 255);

		case 129:
			return D3DCOLOR_RGBA(255, 0, 0, 255);

		case 130:	// NPC 대화
			return D3DCOLOR_RGBA(252, 238, 92, 255);

		case 131:
			return D3DCOLOR_RGBA(255, 120, 0, 255);
		
		// by ichabod
		// 길드 관련 폰트
		case 200 :	// 길드 가입권유 및 신청시
			return D3DCOLOR_RGBA(50, 128, 100, 255);
		case 201 : // 승낙
			return D3DCOLOR_RGBA(0, 255, 255, 255);
		case 202 : // 권한 설정
			return D3DCOLOR_RGBA(0, 150, 150, 255);
		
		default:
			return D3DCOLOR_RGBA(255, 255, 255, 255);
	}
}

void CGUITextEdit::AddNotice(char *strMessage, unsigned short wCmd)
{
	int i, len = strlen(strMessage), count = 0, countx = 0, start = 0;
	unsigned long dwColor;
	
	switch(wCmd)
	{
		case 0:
			dwColor = D3DCOLOR_RGBA(255, 0, 255, 255);

			{
				/*
				CSoundManager &sm = CSoundManager::GetInstance();
				char Temp[MAX_PATH];
				sprintf(Temp, "%s\\Sound\\gong.mnd", g_ClientMain.m_strClientPath);
				ISoundObject &sb = sm.GetBuffer(Temp, false);
				sb.Play(false);
				*/
				char Temp[MAX_PATH]; 
				sprintf(Temp, "%s\\Sound\\gong.mnd", g_ClientMain.m_strClientPath);
				CSound* pSound; 
				CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
				pSound->Play(); 
			}
			break;

		case 1:
			dwColor = D3DCOLOR_RGBA(129, 255, 129, 255);
			break;
	}

	for(i = 0; i < len; i++)
	{
		if(strMessage[i] & 0x80)
		{
			countx += m_dwTextSize[128];
			count += 2;
			i++;
			if(countx >= 471) // 483 - 12 하나의 최고 폭
			{
				if(m_deqNoticeMessage.size() >= 3)
				{
					delete (*m_deqNoticeMessage.begin());
					m_deqNoticeMessage.pop_front();
				}

				Message *lpMessage;
				lpMessage = new Message;
				memcpy(lpMessage->strMessage, &strMessage[start], count);
				lpMessage->strMessage[count] = '\0';
				lpMessage->dwCommand = 255;
				lpMessage->fTimer = 10000;
				lpMessage->dwColor = dwColor;
				m_deqNoticeMessage.push_back(lpMessage);

				countx = count = 0;
				start = i + 1;
			}
		} else
		{
			countx += m_dwTextSize[strMessage[i]];
			count++;
			if(countx >= 475) // 483 - 8 하나의 최고 폭
			{
				if(m_deqNoticeMessage.size() >= 3)
				{
					delete (*m_deqNoticeMessage.begin());
					m_deqNoticeMessage.pop_front();
				}

				Message *lpMessage;
				lpMessage = new Message;
				memcpy(lpMessage->strMessage, &strMessage[start], count);
				lpMessage->strMessage[count] = '\0';
				lpMessage->dwCommand = 255;
				lpMessage->fTimer = 10000;
				lpMessage->dwColor = dwColor;
				m_deqNoticeMessage.push_back(lpMessage);

				countx = count = 0;
				start = i + 1;
			}
		}
	}

	if(start <= len)
	{
		if(m_deqNoticeMessage.size() >= 3)
		{
			delete (*m_deqNoticeMessage.begin());
			m_deqNoticeMessage.pop_front();
		}

		Message *lpMessage;
		lpMessage = new Message;
		memcpy(lpMessage->strMessage, &strMessage[start], count);
		lpMessage->strMessage[count] = '\0';
		lpMessage->dwCommand = 255;
		lpMessage->fTimer = 10000;
		lpMessage->dwColor = dwColor;
		m_deqNoticeMessage.push_back(lpMessage);
	}
}

char *CGUITextEdit::MakePrintGold(unsigned long dwGold)
{
	static char strGold[16];
	char strTemp[16];
	sprintf(strTemp, "%u", dwGold);
	ZeroMemory(strGold, sizeof(strGold));
	unsigned long dwLen = strlen(strTemp);
	unsigned long dwTemp1 = dwLen / 3;
	unsigned long dwTemp2 = dwLen % 3;
	if(!dwTemp2 && dwTemp1) dwTemp1--;
	unsigned long dwCount = 0, dwDotCount = 0;
	for(unsigned long i = dwLen + dwTemp1; i > 0; i--)
	{
		dwDotCount++;
		if((dwDotCount % 4) == 0)
		{
			dwDotCount = 0;
			strGold[i - 1] = ',';
		} else
		{
			strGold[i - 1] = strTemp[dwLen - dwCount - 1];
			dwCount++;
		}
	}

	return strGold;
}

char *CGUITextEdit::MakeWordBreak(const char *strString, unsigned long dwWidth)
{
	static char strWordBreak[16000];
	WCHAR strWideString[16000];
	int nBufferSize = 16000;
	int nWideStringLen;
	long i;

	MultiByteToWideChar(CP_ACP, 0, strString, strlen(strString) + 1, strWideString, nBufferSize);
	nWideStringLen = wcslen(strWideString);

	unsigned long dwStringWidth = 0;
//	unsigned long dwStartPos = 0;
	unsigned long dwSrcPos = 0;
	unsigned long dwDstPos = 0;

	WCHAR strWideBreak[256];
	char strBreak[256];

	strcpy(strWordBreak, "");

	for(i = 0; i < nWideStringLen; i++)
	{
		if((dwSrcPos <= (unsigned long)i) && (i < 16000))
		{
			if(strWideString[i] == 0x005c)		// "\"
			{
				ZeroMemory(strWideBreak, sizeof(strWideBreak));
				wcsncpy(strWideBreak, &strWideString[dwSrcPos], i - dwSrcPos);

				if(strWideString[i + 1] == 0x005c) i += 2; else ++i;

				if(strWideString[i] < 128)
					dwStringWidth = m_dwTextSize[strWideString[i]];
				else
					dwStringWidth = m_dwTextSize[128];

				dwSrcPos = i;
				WideCharToMultiByte(CP_ACP, 0, strWideBreak, wcslen(strWideBreak) + 1, strBreak, 256, NULL, NULL);

				strcpy(&strWordBreak[dwDstPos], strBreak);
				dwDstPos = strlen(strWordBreak);
				strWordBreak[dwDstPos] = '\\';	dwDstPos++;

				continue;
			} else if(strWideString[i] < 128)
			{
				dwStringWidth += m_dwTextSize[strWideString[i]];
			} else
			{
				dwStringWidth += m_dwTextSize[128];
			}

			if(dwStringWidth > dwWidth)
			{
				i--;
				ZeroMemory(strWideBreak, sizeof(strWideBreak));
				if(strWideString[i] == ' ')
				{
					i--;
					wcsncpy(strWideBreak, &strWideString[dwSrcPos], i - dwSrcPos + 1);
					i += 2;

					if(strWideString[i] < 128)
						dwStringWidth = m_dwTextSize[strWideString[i]];
					else
						dwStringWidth = m_dwTextSize[128];
				} else
				{
					wcsncpy(strWideBreak, &strWideString[dwSrcPos], i - dwSrcPos + 1);
					i++;

					if(strWideString[i] < 128)
						dwStringWidth = m_dwTextSize[strWideString[i]];
					else
						dwStringWidth = m_dwTextSize[128];
				}

				dwSrcPos = i;
				WideCharToMultiByte(CP_ACP, 0, strWideBreak, wcslen(strWideBreak) + 1, strBreak, 256, NULL, NULL);

				strcpy(&strWordBreak[dwDstPos], strBreak);
				dwDstPos = strlen(strWordBreak);
				strWordBreak[dwDstPos] = '\\';	dwDstPos++;
			}
		} else
		{
			break;
		}
	}

	if(dwSrcPos < (unsigned long)nWideStringLen)
	{
		if((nWideStringLen < 16000))
		{
			ZeroMemory(strWideBreak, sizeof(strWideBreak));
			if(strWideString[dwSrcPos] == ' ')
			{
				if(dwSrcPos + 1 < (unsigned long)nWideStringLen)
				{
					dwSrcPos++;
				}
				wcsncpy(strWideBreak, &strWideString[dwSrcPos], nWideStringLen - dwSrcPos + 1);
			} else
			{
				wcsncpy(strWideBreak, &strWideString[dwSrcPos], nWideStringLen - dwSrcPos + 1);
			}

			WideCharToMultiByte(CP_ACP, 0, strWideBreak, wcslen(strWideBreak) + 1, strBreak, 256, NULL, NULL);

			strcpy(&strWordBreak[dwDstPos], strBreak);
		}
	}

	return strWordBreak;
}

void CGUITextEdit::InitEnglishMode()
{
	HIMC hImc = ImmGetContext(m_hEditBox);
	ImmSetConversionStatus(hImc, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
	ImmReleaseContext(m_hEditBox, hImc);
}

VOID CGUITextEdit::GetRenderStateEx()
{
	DWORD		dwState ;
	CHAR		Buff[ MAX_PATH ] ;

	OutputDebugString( "RenderState\n" ) ;
	for ( int i = 7 ; i <= 60 ; i ++ )
	{
		BaseGraphicsLayer::GetDevice()->GetRenderState( (D3DRENDERSTATETYPE)i, &dwState ) ;
		sprintf( Buff, "%d = %d\n", i, dwState ) ;
		OutputDebugString( Buff ) ;
	}
	
	for ( int i = 128 ; i <= 173 ; i ++ )
	{
		BaseGraphicsLayer::GetDevice()->GetRenderState( (D3DRENDERSTATETYPE)i, &dwState ) ;
		sprintf( Buff, "%d = %d\n", i, dwState ) ;
		OutputDebugString( Buff ) ;
	}
	
	OutputDebugString( "TextureStageState #0\n" ) ;
	for ( int i = 1 ; i <= 28 ; i ++ )
	{
		BaseGraphicsLayer::GetDevice()->GetTextureStageState( 0, (D3DTEXTURESTAGESTATETYPE)i, &dwState ) ;
		sprintf( Buff, "%d = %d\n", i, dwState ) ;
		OutputDebugString( Buff ) ;
	}

	OutputDebugString( "TextureStageState #1\n" ) ;
	for ( int i = 1 ; i <= 28 ; i ++ )
	{
		BaseGraphicsLayer::GetDevice()->GetTextureStageState( 1, (D3DTEXTURESTAGESTATETYPE)i, &dwState ) ;
		sprintf( Buff, "%d = %d\n", i, dwState ) ;
		OutputDebugString( Buff ) ;
	}
}

void CGUITextEdit::DrawText(const char *pString, RECT *pRect, unsigned long Format, D3DCOLOR Color)
{
	if ( m_lpText )
	{
		m_lpText->Begin();
		m_lpText->DrawTextA(pString, -1, pRect, Format, Color);
		m_lpText->End();
	}
}