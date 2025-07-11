// GUITooltipButton.cpp: implementation of the CGUITooltipButton class.
//
//////////////////////////////////////////////////////////////////////
#include "RYLClientMain.h"
#include "GUITextEdit.h"
#include "WinInput.h"
#include "GUITooltipButton.h"
#include <string>

//================================================================
#include "RYLGameData.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUITooltipButton::CGUITooltipButton()
{

}

CGUITooltipButton::~CGUITooltipButton()
{
	DestroyTooltip();
}

void CGUITooltipButton::InitTooltip(const char *strTooltip)
{
	char seps[] = "\\";
	char *token;
	char strTemp[MAX_PATH * 4];
	char *strTempTooltip;

	DestroyTooltip();

	if(!strcmp(strTooltip, "")) { m_bShow = FALSE; return; }

	strcpy(strTemp, strTooltip);
	token = strtok(strTemp, seps);

	m_dwMaxLength = 0;
	unsigned long dwLength;
	for(;;)
	{
		if(!token) break;

		strTempTooltip = new char[strlen(token) + 1];
		memcpy(strTempTooltip, token, strlen(token));
		strTempTooltip[strlen(token)] = '\0';
		dwLength = g_TextEdit.GetStringLength(strTempTooltip) + 4;
		if(dwLength >= m_dwMaxLength) m_dwMaxLength = dwLength;
		m_lstTooltip.push_back(strTempTooltip);
		token = strtok(NULL, seps);
	}
}

BOOL CGUITooltipButton::GetIsTooltip(void)
{
	if(m_lstTooltip.empty())
		return FALSE;
	else
		return TRUE;
}

void CGUITooltipButton::DestroyTooltip(void)
{
	vector<char *>::iterator it;
	char *strDelete;

	for(it = m_lstTooltip.begin(); it != m_lstTooltip.end(); it++)
	{
		if((*it))
		{
			strDelete = (*it);
			delete[] strDelete;
			strDelete = NULL;
		}
	}
	m_lstTooltip.clear();

}

void CGUITooltipButton::Render(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
	unsigned short SizeX = ptMousePos->x + 7;
	unsigned short SizeY = ptMousePos->y + 11;
	SizeY += 16;
	if(SizeX + m_dwMaxLength + 6 > (unsigned long)BaseGraphicsLayer::m_lScreenSx) { SizeX -= (SizeX + m_dwMaxLength + 6) - BaseGraphicsLayer::m_lScreenSx; }
	if(SizeY + m_lstTooltip.size() * 18 + 6 > (unsigned long)BaseGraphicsLayer::m_lScreenSy) { SizeY -= (SizeY + m_lstTooltip.size() * 18 + 6) - BaseGraphicsLayer::m_lScreenSy; }
	CRYLGameData::Instance()->RenderRect(lpD3DDevice, SizeX, SizeY, SizeX + m_dwMaxLength + 6, SizeY + m_lstTooltip.size() * 18 + 6, 0x00000000, 0x99);

	RECT rcRect;
	unsigned long i;
	for(i = 0; i < m_lstTooltip.size(); i++)
	{
		::SetRect(&rcRect, SizeX + 3, SizeY + i * 18 + 3, SizeX + m_dwMaxLength, SizeY + i * 18 + 18 + 3);
		g_TextEdit.DrawText(m_lstTooltip[i], &rcRect, m_dwAlign | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255));
	}
}
