// BaseGUI.cpp: implementation of the CBaseGUI class.
//
//////////////////////////////////////////////////////////////////////

//#include <crtdbg.h>
#include "RYLClientMain.h"
#include "Creature\NPC\NPCList.h"
#include "GUITextEdit.h"
#include "WinInput.h"
#include "BaseGUI.h"
#include "GUIMessageBox.h"
#include "FrameTimer.h"

#include "Skill\SkillMgr.h"
#include "SkillSystem.h"

#include "RenderOption.h"

#include "HumanGuildMain.h"

#include <dxutil.h>
#include <SectorSoundMap.h>
#include <BGMController.h>

#include <Network/Packet/PacketStruct/GuildPacket.h>
#include <Network/ClientSocket/Send/SendAttack.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/ClientSocket/Send/SendEtc.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendMove.h>
#include <Network/ClientSocket/Send/SendLoginout.h>
#include <Network/ClientSocket/Send/SendQuest.h>

#include "HumanGuildInfo.h"

//==========================================================
#include "RYLStringTable.h"
#include "RYLGameData.h"
#include "RYLNetWorkData.h"
#include "RYLCommunityData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseGUI::CBaseGUI()
{
	m_bShowHelp = FALSE;

	CTexture::SetPath(INTERFACETEXTUREPATH);
	m_lpChrButtonTexture = new CTexture;
	m_lpChrButtonTexture->Load("namebox.dds");

	m_lpPopupTexture = new CTexture;
	m_lpPopupTexture->Load("msgbox.dds");

	m_lpSocket = new CTexture;
	m_lpSocket->Load("slot.dds");

	m_lpCommonWindow = new CTexture;
	m_lpCommonWindow->Load("clw.dds");

	m_lpChantIcon = new CTexture;
	m_lpChantIcon->Load("Chanticon01.dds");

	m_lpHPGauge = new CSprite;
	m_lpHPGauge->Create(0, 0, 1, 32, 82, 37, m_lpCommonWindow);

	m_lpMarkIcon = new CTexture;
	m_lpMarkIcon->Load("RankingIcon.dds");

	// 인간
	m_lpMark[0] = new CSprite;
	m_lpMark[0]->Create(0, 0, 0, 0, 12, 12, m_lpMarkIcon);

	m_lpMark[1] = new CSprite;
	m_lpMark[1]->Create(0, 0, 13, 0, 25, 12, m_lpMarkIcon);

	m_lpMark[2] = new CSprite;
	m_lpMark[2]->Create(0, 0, 26, 0, 38, 12, m_lpMarkIcon);

	m_lpMark[3] = new CSprite;
	m_lpMark[3]->Create(0, 0, 39, 0, 51, 12, m_lpMarkIcon);

	m_lpMark[4] = new CSprite;
	m_lpMark[4]->Create(0, 0, 52, 0, 64, 12, m_lpMarkIcon);

	// 아칸
	m_lpMark[5] = new CSprite;
	m_lpMark[5]->Create(0, 0, 0, 13, 12, 25, m_lpMarkIcon);

	m_lpMark[6] = new CSprite;
	m_lpMark[6]->Create(0, 0, 13, 13, 25, 25, m_lpMarkIcon);

	m_lpMark[7] = new CSprite;
	m_lpMark[7]->Create(0, 0, 26, 13, 38, 25, m_lpMarkIcon);

	m_lpMark[8] = new CSprite;
	m_lpMark[8]->Create(0, 0, 39, 13, 51, 25, m_lpMarkIcon);

	m_lpMark[9] = new CSprite;
	m_lpMark[9]->Create(0, 0, 52, 13, 64, 25, m_lpMarkIcon);

	m_bPopupShow = FALSE;
	CRYLGameData::Instance()->m_dwStartQuickSlot = 0;

	m_dwPartyCmdResult = 0;
	m_dwExchangeCmdResult = 0;
	m_dwDuelCmdResult = 0;
	m_dwPartyDuelCmdResult = 0;

	m_bQuickWeaponExchange = FALSE;
	m_bWeaponExchageAble = TRUE;
	m_bShowSkillError = FALSE;

	m_lpSkillGaugeFrame = NULL;
	m_lpSkillGauge[0] = NULL;
	m_lpSkillGauge[1] = NULL;
	m_lpSkillGauge[2] = NULL;
	m_lpSkillGauge[3] = NULL;
	m_lpSkillGauge[4] = NULL;

	m_lpTarget[0] = NULL;
	m_lpTarget[1] = NULL;

	m_lpQuiver = NULL;

	for(unsigned long i = 0; i < 44; i++)
	{
		m_lpChant[i] = NULL;
	}

	m_lpChant[0] = new CSprite;		// 배틀송
	m_lpChant[0]->Create(0, 0, 0, 0, 16, 16, m_lpChantIcon);

	m_lpChant[1] = new CSprite;		// 메인터넌스 챈트
	m_lpChant[1]->Create(0, 0, 16, 0, 32, 16, m_lpChantIcon);

	m_lpChant[2] = new CSprite;		// 액셀러레이션 챈트
	m_lpChant[2]->Create(0, 0, 32, 0, 48, 16, m_lpChantIcon);

	m_lpChant[3] = new CSprite;		// 라이프 오라
	m_lpChant[3]->Create(0, 0, 48, 0, 64, 16, m_lpChantIcon);


	m_lpChant[14] = new CSprite;		// 블레이즈
	m_lpChant[14]->Create(0, 0, 16, 80, 32, 96, m_lpChantIcon);

	m_lpChant[15] = new CSprite;		// 차징
	m_lpChant[15]->Create(0, 0, 0, 80, 16, 96, m_lpChantIcon);

	m_lpChant[16] = new CSprite;		// 스텔스
	m_lpChant[16]->Create(0, 0, 0, 16, 16, 32, m_lpChantIcon);

	m_lpChant[17] = new CSprite;		// 마나쉘
	m_lpChant[17]->Create(0, 0, 16, 16, 32, 32, m_lpChantIcon);

	m_lpChant[18] = new CSprite;		// 인커리지
	m_lpChant[18]->Create(0, 0, 32, 16, 48, 32, m_lpChantIcon);

	m_lpChant[19] = new CSprite;		// 인챈트웨폰
	m_lpChant[19]->Create(0, 0, 48, 16, 64, 32, m_lpChantIcon);

	m_lpChant[20] = new CSprite;		// 브라이트 아머
	m_lpChant[20]->Create(0, 0, 64, 16, 80, 32, m_lpChantIcon);

	m_lpChant[21] = new CSprite;		// 하든스킨
	m_lpChant[21]->Create(0, 0, 80, 16, 96, 32, m_lpChantIcon);

	m_lpChant[22] = new CSprite;		// 플랙서빌러티
	m_lpChant[22]->Create(0, 0, 96, 16, 112, 32, m_lpChantIcon);

	m_lpChant[23] = new CSprite;		// 가드
	m_lpChant[23]->Create(0, 0, 112, 16, 128, 32, m_lpChantIcon);


	m_lpChant[24] = new CSprite;		// 홀드
	m_lpChant[24]->Create(0, 0, 0, 32, 16, 48, m_lpChantIcon);

	m_lpChant[25] = new CSprite;		// 스턴
	m_lpChant[25]->Create(0, 0, 16, 32, 32, 48, m_lpChantIcon);

	m_lpChant[26] = new CSprite;		// 프로즌
	m_lpChant[26]->Create(0, 0, 32, 32, 48, 48, m_lpChantIcon);

	m_lpChant[27] = new CSprite;		// 포이즌
	m_lpChant[27]->Create(0, 0, 48, 32, 64, 48, m_lpChantIcon);

	m_lpChant[28] = new CSprite;		// 로어스트랭스
	m_lpChant[28]->Create(0, 0, 64, 32, 80, 48, m_lpChantIcon);

	m_lpChant[29] = new CSprite;		// 무적
	m_lpChant[29]->Create(0, 0, 112, 32, 128, 48, m_lpChantIcon);

	m_lpChant[32] = new CSprite;		// 엘리트 레벨 -1
	m_lpChant[32]->Create(0, 0, 0, 64, 16, 80, m_lpChantIcon);
	m_lpChant[33] = new CSprite;		// 엘리트 레벨 -2
	m_lpChant[33]->Create(0, 0, 16, 64, 32, 80, m_lpChantIcon);
	m_lpChant[34] = new CSprite;		// 엘리트 레벨 -3
	m_lpChant[34]->Create(0, 0, 32, 64, 48, 80, m_lpChantIcon);
	m_lpChant[35] = new CSprite;		// 엘리트 레벨 -4
	m_lpChant[35]->Create(0, 0, 48, 64, 64, 80, m_lpChantIcon);
	m_lpChant[36] = new CSprite;		// 엘리트 레벨 -5
	m_lpChant[36]->Create(0, 0, 64, 64, 80, 80, m_lpChantIcon);
	m_lpChant[37] = new CSprite;		// 엘리트 레벨 +1
	m_lpChant[37]->Create(0, 0, 0, 48, 16, 64, m_lpChantIcon);
	m_lpChant[38] = new CSprite;		// 엘리트 레벨 +2
	m_lpChant[38]->Create(0, 0, 16, 48, 32, 64, m_lpChantIcon);
	m_lpChant[39] = new CSprite;		// 엘리트 레벨 +3
	m_lpChant[39]->Create(0, 0, 32, 48, 48, 64, m_lpChantIcon);
	m_lpChant[40] = new CSprite;		// 엘리트 레벨 +4
	m_lpChant[40]->Create(0, 0, 48, 48, 64, 64, m_lpChantIcon);
	m_lpChant[41] = new CSprite;		// 엘리트 레벨 +5
	m_lpChant[41]->Create(0, 0, 64, 48, 80, 64, m_lpChantIcon);

	m_lpChant[42] = new CSprite;		// 피스 모드
	m_lpChant[42]->Create(0, 0, 80, 48, 96, 64, m_lpChantIcon);
	m_lpChant[43] = new CSprite;		// 전투 모드
	m_lpChant[43]->Create(0, 0, 96, 48, 112, 64, m_lpChantIcon);
}

CBaseGUI::~CBaseGUI()
{
	unsigned long i;
	for(i = 0; i < 44; i++)
	{
		if(m_lpChant[i]) { delete m_lpChant[i]; m_lpChant[i] = NULL; }
	}

	if(m_lpSkillGaugeFrame) { delete m_lpSkillGaugeFrame; m_lpSkillGaugeFrame = NULL; }

	if(m_lpSkillGauge[0]) { delete m_lpSkillGauge[0]; m_lpSkillGauge[0] = NULL; }
	if(m_lpSkillGauge[1]) { delete m_lpSkillGauge[1]; m_lpSkillGauge[1] = NULL; }
	if(m_lpSkillGauge[2]) { delete m_lpSkillGauge[2]; m_lpSkillGauge[2] = NULL; }
	if(m_lpSkillGauge[3]) { delete m_lpSkillGauge[3]; m_lpSkillGauge[3] = NULL; }
	if(m_lpSkillGauge[4]) { delete m_lpSkillGauge[4]; m_lpSkillGauge[4] = NULL; }

	if(m_lpTarget[0]) { delete m_lpTarget[0]; m_lpTarget[0] = NULL; }
	if(m_lpTarget[1]) { delete m_lpTarget[1]; m_lpTarget[1] = NULL; }
	if(m_lpQuiver) { delete m_lpQuiver; m_lpQuiver = NULL; }

	if(m_lpSocket) { delete m_lpSocket; m_lpSocket = NULL; }

	delete m_lpMarkIcon;

	for(i = 0; i < 10; i++)
	{
		if(m_lpMark[i]) { delete m_lpMark[i]; m_lpMark[i] = NULL; }
	}

	delete m_lpChrButtonTexture;
	delete m_lpPopupTexture;
	delete m_lpCommonWindow;
	delete m_lpHPGauge;
	delete m_lpChantIcon;

	DestroyAllObject();
}

void CBaseGUI::DestroyAllObject(void)
{
	{
		vector<CTexture *>::iterator it;
		for(it = m_lstTexture.begin(); it != m_lstTexture.end(); it++)
		{
			delete (*it);
		}
		m_lstTexture.clear();
	}

	{
		vector<CGUIWindow *>::iterator it;
		for(it = m_lstWindow.begin(); it != m_lstWindow.end(); it++)
		{
			delete (*it);
		}
		m_lstWindow.clear();
	}
}

void CBaseGUI::RenderChrButton(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned long dwUID, short wPosX, short wPosY, unsigned long dwColor)
{
	RECT rcRect;

	CCreature *lpCreature = g_CharacterData.GetCreature(dwUID);
	unsigned long dwLength = g_TextEdit.GetStringLength(lpCreature->m_strName);
	SetRect(&rcRect, wPosX - (dwLength / 2 + 21), wPosY, wPosX + (dwLength / 2 + 23), wPosY + 30);

	TLVertex Vertex[8];
	Vertex[0].Diffuse.c = Vertex[1].Diffuse.c = Vertex[2].Diffuse.c = Vertex[3].Diffuse.c = 0xFFFFFFFF;
	Vertex[4].Diffuse.c = Vertex[5].Diffuse.c = Vertex[6].Diffuse.c = Vertex[7].Diffuse.c = 0xFFFFFFFF;
	Vertex[0].Specular.c = Vertex[1].Specular.c = Vertex[2].Specular.c = Vertex[3].Specular.c = 0;
	Vertex[4].Specular.c = Vertex[5].Specular.c = Vertex[6].Specular.c = Vertex[7].Specular.c = 0;
	Vertex[0].w = Vertex[1].w = Vertex[2].w = Vertex[3].w = 0.1f;
	Vertex[4].w = Vertex[5].w = Vertex[6].w = Vertex[7].w = 0.1f;
	Vertex[0].v.z = Vertex[1].v.z = Vertex[2].v.z = Vertex[3].v.z = 0.1f;
	Vertex[4].v.z = Vertex[5].v.z = Vertex[6].v.z = Vertex[7].v.z = 0.1f;

	Vertex[0].v.x = Vertex[1].v.x = rcRect.left - 0.5f;
	Vertex[2].v.x = Vertex[3].v.x = rcRect.left + 20 - 0.5f;
	Vertex[4].v.x = Vertex[5].v.x = rcRect.right - 23 - 0.5f;
	Vertex[6].v.x = Vertex[7].v.x = rcRect.right - 0.5f;

	Vertex[0].v.y = Vertex[2].v.y = Vertex[4].v.y = Vertex[6].v.y = rcRect.top - 0.5f;
	Vertex[1].v.y = Vertex[3].v.y = Vertex[5].v.y = Vertex[7].v.y = rcRect.bottom - 0.5f;
	
	Vertex[0].tu = Vertex[1].tu = 0.0f;
	Vertex[2].tu = Vertex[3].tu = 20.0f / 64.0f;
	Vertex[4].tu = Vertex[5].tu = 40.0f / 64.0f;
	Vertex[6].tu = Vertex[7].tu = 63.0f / 64.0f;

	Vertex[0].tv = Vertex[2].tv = Vertex[4].tv = Vertex[6].tv = 0.0f;
	Vertex[1].tv = Vertex[3].tv = Vertex[5].tv = Vertex[7].tv = 32.0f / 64.0f;

	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(1, NULL);
	lpD3DDevice->SetTexture(0, m_lpChrButtonTexture->GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 6, Vertex, sizeof(TLVertex));

	g_TextEdit.DrawText(lpCreature->m_strName, &rcRect, DT_CENTER | DT_VCENTER, dwColor);
//	g_TextEdit.DrawText(lpCreature->m_strName, &rcRect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(255, 202, 132, 255));
}

void CBaseGUI::RenderChrButton(LPDIRECT3DDEVICE8 lpD3DDevice, char *strText, short wPosX, short wPosY)
{
	RECT rcRect;

	unsigned long dwLength = g_TextEdit.GetStringLength(strText);
	SetRect(&rcRect, wPosX - (dwLength / 2 + 21), wPosY, wPosX + (dwLength / 2 + 23), wPosY + 30);

	TLVertex Vertex[8];
	Vertex[0].Diffuse.c = Vertex[1].Diffuse.c = Vertex[2].Diffuse.c = Vertex[3].Diffuse.c = 0xFFFFFFFF;
	Vertex[4].Diffuse.c = Vertex[5].Diffuse.c = Vertex[6].Diffuse.c = Vertex[7].Diffuse.c = 0xFFFFFFFF;
	Vertex[0].Specular.c = Vertex[1].Specular.c = Vertex[2].Specular.c = Vertex[3].Specular.c = 0;
	Vertex[4].Specular.c = Vertex[5].Specular.c = Vertex[6].Specular.c = Vertex[7].Specular.c = 0;
	Vertex[0].w = Vertex[1].w = Vertex[2].w = Vertex[3].w = 0.1f;
	Vertex[4].w = Vertex[5].w = Vertex[6].w = Vertex[7].w = 0.1f;
	Vertex[0].v.z = Vertex[1].v.z = Vertex[2].v.z = Vertex[3].v.z = 0.1f;
	Vertex[4].v.z = Vertex[5].v.z = Vertex[6].v.z = Vertex[7].v.z = 0.1f;

	Vertex[0].v.x = Vertex[1].v.x = rcRect.left - 0.5f;
	Vertex[2].v.x = Vertex[3].v.x = rcRect.left + 20 - 0.5f;
	Vertex[4].v.x = Vertex[5].v.x = rcRect.right - 23 - 0.5f;
	Vertex[6].v.x = Vertex[7].v.x = rcRect.right - 0.5f;

	Vertex[0].v.y = Vertex[2].v.y = Vertex[4].v.y = Vertex[6].v.y = rcRect.top - 0.5f;
	Vertex[1].v.y = Vertex[3].v.y = Vertex[5].v.y = Vertex[7].v.y = rcRect.bottom - 0.5f;
	
	Vertex[0].tu = Vertex[1].tu = 0.0f;
	Vertex[2].tu = Vertex[3].tu = 20.0f / 64.0f;
	Vertex[4].tu = Vertex[5].tu = 40.0f / 64.0f;
	Vertex[6].tu = Vertex[7].tu = 63.0f / 64.0f;

	Vertex[0].tv = Vertex[2].tv = Vertex[4].tv = Vertex[6].tv = 0.5f;
	Vertex[1].tv = Vertex[3].tv = Vertex[5].tv = Vertex[7].tv = 1.0f;

	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(1, NULL);
	lpD3DDevice->SetTexture(0, m_lpChrButtonTexture->GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 6, Vertex, sizeof(TLVertex));

	g_TextEdit.DrawText(strText, &rcRect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255));
}

void CBaseGUI::RenderPopUp(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	TLVertex Vertex[16];
	unsigned short Index[54];
	unsigned long i;

	Index[ 0] = 0;		Index[ 1] = 1;		Index[ 2] = 4;
	Index[ 3] = 1;		Index[ 4] = 5;		Index[ 5] = 4;
	Index[ 6] = 1;		Index[ 7] = 2;		Index[ 8] = 5;
	Index[ 9] = 2;		Index[10] = 6;		Index[11] = 5;
	Index[12] = 2;		Index[13] = 3;		Index[14] = 6;
	Index[15] = 3;		Index[16] = 7;		Index[17] = 6;
	Index[18] = 4;		Index[19] = 5;		Index[20] = 8;
	Index[21] = 5;		Index[22] = 9;		Index[23] = 8;
	Index[24] = 5;		Index[25] = 6;		Index[26] = 9;
	Index[27] = 6;		Index[28] = 10;		Index[29] = 9;
	Index[30] = 6;		Index[31] = 7;		Index[32] = 10;
	Index[33] = 7;		Index[34] = 11;		Index[35] = 10;
	Index[36] = 8;		Index[37] = 9;		Index[38] = 12;
	Index[39] = 9;		Index[40] = 13;		Index[41] = 12;
	Index[42] = 9;		Index[43] = 10;		Index[44] = 13;
	Index[45] = 10;		Index[46] = 14;		Index[47] = 13;
	Index[48] = 10;		Index[49] = 11;		Index[50] = 14;
	Index[51] = 11;		Index[52] = 15;		Index[53] = 14;

	for(i = 0 ; i < 16; i++)
	{
		Vertex[i].Diffuse.c = 0xFFFFFFFF;
		Vertex[i].Specular.c = 0;
		Vertex[i].w = 0.1f;
		Vertex[i].v.z = 0.1f;
	}

	for(i = 0; i < 4; i++)
	{
		Vertex[0 + i * 4].tu = 0.0f;
		Vertex[1 + i * 4].tu = 20 / 64.0f;
		Vertex[2 + i * 4].tu = 44 / 64.0f;
		Vertex[3 + i * 4].tu = 1.0f;

		Vertex[0 + i].tv = 0.0f;
		Vertex[4 + i].tv = 20 / 64.0f;
		Vertex[8 + i].tv = 44 / 64.0f;
		Vertex[12 + i].tv = 1.0f;
	}

	float WindowX = m_nPopupWindowX - 0.5f;
	float WindowY = m_nPopupWindowY - 0.5f;

	Vertex[ 0].v.x = (float)WindowX;								Vertex[ 0].v.y = (float)WindowY;
	Vertex[ 1].v.x = (float)(WindowX + 20);							Vertex[ 1].v.y = (float)WindowY;
	Vertex[ 2].v.x = (float)(WindowX + 12 + m_dwPopupWindowLength);	Vertex[ 2].v.y = (float)WindowY;
	Vertex[ 3].v.x = (float)(WindowX + 32 + m_dwPopupWindowLength);	Vertex[ 3].v.y = (float)WindowY;
	Vertex[ 4].v.x = (float)WindowX;								Vertex[ 4].v.y = (float)(WindowY + 20);
	Vertex[ 5].v.x = (float)(WindowX + 20);							Vertex[ 5].v.y = (float)(WindowY + 20);
	Vertex[ 6].v.x = (float)(WindowX + 12 + m_dwPopupWindowLength);	Vertex[ 6].v.y = (float)(WindowY + 20);
	Vertex[ 7].v.x = (float)(WindowX + 32 + m_dwPopupWindowLength);	Vertex[ 7].v.y = (float)(WindowY + 20);
	Vertex[ 8].v.x = (float)WindowX;								Vertex[ 8].v.y = (float)(WindowY + 4 + m_lstPopupMenu.size() * 18);
	Vertex[ 9].v.x = (float)(WindowX + 20);							Vertex[ 9].v.y = (float)(WindowY + 4 + m_lstPopupMenu.size() * 18);
	Vertex[10].v.x = (float)(WindowX + 12 + m_dwPopupWindowLength);	Vertex[10].v.y = (float)(WindowY + 4 + m_lstPopupMenu.size() * 18);
	Vertex[11].v.x = (float)(WindowX + 32 + m_dwPopupWindowLength);	Vertex[11].v.y = (float)(WindowY + 4 + m_lstPopupMenu.size() * 18);
	Vertex[12].v.x = (float)WindowX;								Vertex[12].v.y = (float)(WindowY + 24 + m_lstPopupMenu.size() * 18);
	Vertex[13].v.x = (float)(WindowX + 20);							Vertex[13].v.y = (float)(WindowY + 24 + m_lstPopupMenu.size() * 18);
	Vertex[14].v.x = (float)(WindowX + 12 + m_dwPopupWindowLength);	Vertex[14].v.y = (float)(WindowY + 24 + m_lstPopupMenu.size() * 18);
	Vertex[15].v.x = (float)(WindowX + 32 + m_dwPopupWindowLength);	Vertex[15].v.y = (float)(WindowY + 24 + m_lstPopupMenu.size() * 18);

	lpD3DDevice->SetTexture(0, m_lpPopupTexture->GetTexture());
	lpD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 16, 18, Index, D3DFMT_INDEX16, Vertex, sizeof(TLVertex));

	if(m_dwPopupSelect != 0xFFFFFFFF)
	{
		CRYLGameData::Instance()->m_lpCommonInterface->RenderRect(lpD3DDevice,
			m_nPopupWindowX + 10, m_nPopupWindowY + 12 + m_dwPopupSelect * 18,
			m_nPopupWindowX + 20 + m_dwPopupWindowLength, m_nPopupWindowY + 30 + m_dwPopupSelect * 18,
			0x80fff9c2, 0x78);
	}

	RECT rect;
	for(i = 0; i < m_lstPopupMenu.size(); i++)
	{
		SetRect(&rect, WindowX + 15, WindowY + 13 + i * 18, WindowX + 15 + m_dwPopupWindowLength, WindowY + 31 + i * 18);
		g_TextEdit.DrawText(m_lstPopupMenu[i].m_strPopupStr, &rect, DT_CENTER | DT_VCENTER, m_lstPopupMenu[i].m_dwColor);
	}
}

BOOL CBaseGUI::UpdatePopup(void)
{
	switch(m_dwKindPos)
	{
		case 0:
			{
				vector3 vecResult, vecChrPos;
				float fW;
				CCreature *lpCreature = g_CharacterData.GetCreature(m_dwPopupSelectChrID);
				if(!lpCreature)
				{
					m_bPopupShow = FALSE;
					m_dwPopupSelect = 0xFFFFFFFF;

					return TRUE;
				}

				vecChrPos = lpCreature->m_vecPos;
				vecChrPos.y += 180.0f;
				BaseGraphicsLayer::TransformVector(vecChrPos, vecResult, fW);
				if(vecResult.z > 1.0f)
				{
					m_bPopupShow = FALSE;
					return FALSE;
				}

				m_nPopupWindowX = vecResult.x + 50.0f - 0.5;
				m_nPopupWindowY = vecResult.y - 0.5f;
			}
			break;

		case 1:
			{
				POINT *ptMousePos = g_DeviceInput.GetMousePosition();
				m_nPopupWindowX = ptMousePos->x;
				m_nPopupWindowY = ptMousePos->y;
			}
			break;
	}

	m_dwPopupSelect = 0xFFFFFFFF;
	for(unsigned long i = 0; i < m_lstPopupMenu.size(); i++)
	{
		if(g_DeviceInput.InRect(m_nPopupWindowX + 10, m_nPopupWindowY + 13 + i * 18, m_nPopupWindowX + 20 + m_dwPopupWindowLength, m_nPopupWindowY + 31 + i * 18))
		{
			m_dwPopupSelect = i;
			if(g_DeviceInput.GetIsLeftMouseDown())
			{
				switch(m_lstPopupMenu[m_dwPopupSelect].m_dwPopupKind)
				{
					case POPUPMENU_EXCHANGE:
						if(!GRYLCommunityDataInstance()->GetIsRefuse(m_dwPopupSelectChrID))
						{
							CRYLNetworkData::SendExchangeCmd(m_dwPopupSelectChrID, ClientConstants::ExchangeCmd_Propose);
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[16], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						} else
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[810], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						}
						break;

					case POPUPMENU_DUEL:
						if(!GRYLCommunityDataInstance()->GetIsRefuse(m_dwPopupSelectChrID))
						{
							m_dwDuelCmdResult = 0;
							m_dwDuelCmdType = 100;
							m_dwDuelCmdSenderID = m_dwPopupSelectChrID;
							char strText[200];
							CCreature *lpCreature = g_CharacterData.GetCreature(m_dwPopupSelectChrID);
							if(lpCreature)
							{
								sprintf(strText, CRYLStringTable::m_strString[17],
									lpCreature->m_strName, lpCreature->m_cLevel,
									CRYLGameData::Instance()->m_lpCommonInterface->GetClassName(lpCreature->m_wClass));
								CGUIMessageBox *lpMessageBox;
								lpMessageBox = new CGUIMessageBox;
								lpMessageBox->Create(strText, MB_EXIT | MB_YES | MB_NO);
								lpMessageBox->SetResult(&m_dwDuelCmdResult);
							}
						} else
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[810], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						}
						break;

					case POPUPMENU_PARTYINVATE:
						if(!GRYLCommunityDataInstance()->GetIsRefuse(m_dwPopupSelectChrID))
						{
							CRYLNetworkData::SendPartyCmd(m_dwPopupSelectChrID, ClientConstants::PartyCmd_Invite);
							g_TextEdit.AddNormalChatMessage(CRYLStringTable::m_strString[10], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						} else
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[810], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						}
						break;

					case POPUPMENU_STREETSTALL:
						CRYLNetworkData::SendStallEnter(m_dwPopupSelectChrID);
						break;

					case POPUPMENU_LEADER:
						SendPacket::CharPartyCmd(g_GameSession, m_dwPopupSelectChrID, CRYLNetworkData::Instance()->m_dwMyChrID,
							CRYLGameData::Instance()->m_csStatus.m_PartyInfo.m_dwPartyID, ClientConstants::PartyCmd_Transfer);
						g_TextEdit.AddNormalChatMessage(CRYLStringTable::m_strString[11], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						break;

					case POPUPMENU_VANISH:
						SendPacket::CharPartyCmd(g_GameSession, m_dwPopupSelectChrID, CRYLNetworkData::Instance()->m_dwMyChrID,
							CRYLGameData::Instance()->m_csStatus.m_PartyInfo.m_dwPartyID, ClientConstants::PartyCmd_Banish);
						g_TextEdit.AddNormalChatMessage(CRYLStringTable::m_strString[12], CRYLNetworkData::Instance()->m_dwMyChrID, 128);
						break;

					case POPUPMENU_DIALOG:
						{
							CGUIMessageBox *lpMessageBox;
							lpMessageBox = new CGUIMessageBox;
							if(!lpMessageBox->ShowDialog(m_dwPopupSelectChrID, 1))
							{
								delete lpMessageBox;
							}
						}
						break;

					case POPUPMENU_SAVEPOS:
						{
							POS pPos;
							pPos.fPointX = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos.x / 100.0f;
							pPos.fPointY = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos.y / 100.0f;
							pPos.fPointZ = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos.z / 100.0f;
							SendPacket::CharBindPos(g_GameSession, m_dwPopupSelectChrID, 0, &pPos, 0);
						}
						break;

					case POPUPMENU_TRADE:
						InitTrade(m_dwKindPopup, m_dwPopupSelectChrID);
						break;

					case POPUPMENU_UPGRADE:
						InitItemUpgrade(m_dwPopupSelectChrID);
						break;

					case POPUPMENU_UNLOCK:
						InitSkillUnLock(m_dwPopupSelectChrID);
						break;

					case POPUPMENU_STATUSRETRAIN:
						InitStatusRetrain();
						break;

					case POPUPMENU_JOBCHANGE:
						InitClassChange(m_dwPopupSelectChrID);
						break;

					case POPUPMENU_ZONEMOVE:
						{
							NPCNode *lpNode = g_NPCList.GetNPCNode(m_dwPopupSelectChrID);
							if(lpNode)
							{
								LPWarpNode lpZone = lpNode->GetWarpNode(0);
								if(lpZone)
								{
									if(g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bPeaceMode &&
									   ((CRYLGameData::Instance()->m_csStatus.m_Info.Nationality == NATION_HUMAN && lpZone->m_wZoneNumber == 5) ||
									    (CRYLGameData::Instance()->m_csStatus.m_Info.Nationality == NATION_AKHAN && lpZone->m_wZoneNumber == 4) ||
										lpZone->m_wZoneNumber == 3 || lpZone->m_wZoneNumber == 6))
									{
										CGUIMessageBox *lpMessageBox;
										lpMessageBox = new CGUIMessageBox;
										lpMessageBox->SetWordBreak(TRUE);
										lpMessageBox->Create(CRYLStringTable::m_strString[807]);
									} else if((CRYLGameData::Instance()->m_csStatus.m_Info.Nationality == NATION_HUMAN && lpZone->m_wZoneNumber == 2) ||
											  (CRYLGameData::Instance()->m_csStatus.m_Info.Nationality == NATION_AKHAN && lpZone->m_wZoneNumber == 1))
									{
										CGUIMessageBox *lpMessageBox;
										lpMessageBox = new CGUIMessageBox;
										lpMessageBox->Create(CRYLStringTable::m_strString[801]);
									} else
									{
										POS pSpawnPoint;
										pSpawnPoint.fPointX = lpZone->m_fPosX;
										pSpawnPoint.fPointY = lpZone->m_fPosY;
										pSpawnPoint.fPointZ = lpZone->m_fPosZ;
										SendPacket::CharMoveZone(g_GameSession, lpZone->m_wZoneNumber, pSpawnPoint);
									}
								}
							}
						}
						break;

					case POPUPMENU_DIALOG2:
						{
							CGUIMessageBox *lpMessageBox;
							lpMessageBox = new CGUIMessageBox;
							if(!lpMessageBox->ShowDialog(m_dwPopupSelectChrID, 2))
							{
								delete lpMessageBox;
							}
						}
						break;

					case POPUPMENU_DEPOSIT:
						InitDeposit();
						break;

					case POPUPMENU_PARTYDUEL:
						if(!GRYLCommunityDataInstance()->GetIsRefuse(m_dwPopupSelectChrID))
						{
							SendPacket::CharDuelCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_dwPopupSelectChrID, ClientConstants::DuelCmd_RequestPartyInfo);
						} else
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[810], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						}
/*							m_dwPartyDuelCmdResult = 0;
							m_dwPartyDuelCmdType = 100;
							m_dwPartyDuelCmdSenderID = m_dwPopupSelectChrID;
							char strText[200];
							CCreature *lpCreature = g_CharacterData.GetCreature(m_dwPopupSelectChrID);
							if(lpCreature)
							{
								sprintf(strText, CRYLStringTable::m_strString[17],
									lpCreature->m_strName, lpCreature->m_cLevel,
									CRYLGameData::Instance()->m_lpCommonInterface->GetClassName(lpCreature->m_wClass));
								CGUIMessageBox *lpMessageBox;
								lpMessageBox = new CGUIMessageBox;
								lpMessageBox->Create(strText, MB_EXIT | MB_YES | MB_NO);
								lpMessageBox->SetResult(&m_dwDuelCmdResult);
							}
						}/* else
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[18], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						}*/
						break;

					case POPUPMENU_QUEST:
						// 퀘스트 창이 뜬다.
						InitQuest(m_dwPopupSelectChrID, m_lstPopupMenu[m_dwPopupSelect].m_dwQuestID);
						break;

					case POPUPMENU_FRIEND:
						{
							CCreature *lpCreature = g_CharacterData.GetCreature(m_dwPopupSelectChrID);
							if(lpCreature)
							{
								SendPacket::CharFriendAddRequest(g_GameSession, lpCreature->m_strName, PktFriendAddReq::ADD_FRIEND_REQ);
							}
						}
						break;

					case POPUPMENU_BAN:
						{
							CCreature *lpCreature = g_CharacterData.GetCreature(m_dwPopupSelectChrID);
							if(lpCreature)
							{
								SendPacket::CharFriendAddRequest(g_GameSession, lpCreature->m_strName, PktFriendAddReq::BAN_FRIEND_REQ);
							}
						}
						break;

					case POPUPMENU_PEACE:
						{
							CGUIMessageBox *lpMessageBox = new CGUIMessageBox;
							if(g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bPeaceMode)
							{
								lpMessageBox->SetWordBreak(FALSE);
								lpMessageBox->Create(CRYLStringTable::m_strString[808], MB_YES | MB_NO | MB_EXIT);
								lpMessageBox->SetResult(&CRYLCommunityData::Instance()->m_dwPeaceModeResult);

								CRYLCommunityData::Instance()->m_dwPeaceModeResult = 0;
								CRYLCommunityData::Instance()->m_dwPeaceMode = 0;
							} else
							{
								lpMessageBox->SetWordBreak(FALSE);
								lpMessageBox->Create(CRYLStringTable::m_strString[809], MB_YES | MB_NO | MB_EXIT);
								lpMessageBox->SetResult(&CRYLCommunityData::Instance()->m_dwPeaceModeResult);

								CRYLCommunityData::Instance()->m_dwPeaceModeResult = 0;
								CRYLCommunityData::Instance()->m_dwPeaceMode = 1;
							}
						}
						break;

					case POPUPMENU_FOLLOW:
						{
							g_CharacterData.m_bAutoFollowMode = TRUE;
							g_CharacterData.m_dwAutoFollowChrID = m_dwPopupSelectChrID;
						}
						break;

					case POPUPMENU_STATUSCLEAN:
						{
							if(CRYLGameData::Instance()->m_csStatus.m_Info.Chance > 0)
							{
								InitStatusInit();
							} else
							{
								g_TextEdit.AddMessage(CRYLStringTable::m_strString[811], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							}
						}
						break;

					case POPUPMENU_CLASSCLEAN:
						{
							CGUIMessageBox *lpMessageBox = new CGUIMessageBox;
							if(CRYLGameData::Instance()->m_csStatus.m_Info.Class == 21)
							{
								lpMessageBox->Create(CRYLStringTable::m_strString[812], MB_YES | MB_NO | MB_EXIT);
								lpMessageBox->SetResult(&CRYLGameData::Instance()->m_dwClassCleanResult);

								CRYLGameData::Instance()->m_dwClassCleanResult = 0;
							} else
							{
								lpMessageBox->Create(CRYLStringTable::m_strString[813]);
							}
						}
						break;

					//==============================================================================
					// by ichabod
					// desc : 길드 관련
					case POPUPMENU_GUILDINVITE :
						{
							SendPacket::CharGuildCmd( g_GameSession, CRYLGameData::Instance()->m_csStatus.m_Info.Guild, m_dwPopupSelectChrID, CRYLNetworkData::Instance()->m_dwMyChrID, PktGuildCmd::GC_INVITE ) ;
						}
						break ;

					case POPUPMENU_MAKEGUILD :
						{
							((CHumanGuildMain *)CRYLGameData::Instance()->m_lpInterface->m_lstWindow[WINDOW_GUILDMAIN])->CheckGuildStipulation() ;
						}
						break ;
				}
				m_bPopupShow = FALSE;
				m_dwPopupSelect = 0xFFFFFFFF;

				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

void CBaseGUI::NPCSpawn()
{
	vector3 vecSelf = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos;
	vecSelf.y = 0.0f;
	vector3 vecLength = vecSelf - g_CharacterData.m_vecPrePosition;
	float fLength = vecLength.GetLens();
	if(fLength > 500.0f)
	{
		g_CharacterData.m_vecPrePosition = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos;
		g_CharacterData.m_vecPrePosition.y = 0.0f;
		float fMinX = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos.x - 5000.0f;
		float fMaxX = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos.x + 5000.0f;
		float fMinZ = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos.z - 5000.0f;
		float fMaxZ = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos.z + 5000.0f;

		NPCNode *lpNPCNode;
		vector3 vecPos;
		for(unsigned long i = 0; i < g_NPCList.GetNpcNum(); i++)
		{
			lpNPCNode = g_NPCList.GetNPCNodeByOrder(i);
			vecPos.x = lpNPCNode->m_fPosX * 100.0f;
			vecPos.y = lpNPCNode->m_fPosY * 100.0f;
			vecPos.z = lpNPCNode->m_fPosZ * 100.0f;
			if(fMinX <= vecPos.x && vecPos.x <= fMaxX && fMinZ <= vecPos.z && vecPos.z <= fMaxZ)
			{	// NPC 추가
				if(strcmp(lpNPCNode->m_strNpcSkin, "None"))
				{
					g_CharacterData.AddCharacter(lpNPCNode->m_dwUID, lpNPCNode->m_strNpcName, lpNPCNode->m_strNpcSkin, vecPos, lpNPCNode->m_fDirection);
				}
			}
		}
	}

	static POINT ptOldSector = { 0, 0 };
	POINT ptSector, ptItem;
	vector3 vecSelfChrPos;

	g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_lpChrModel->GetPosition(vecSelfChrPos);
	ptSector.x =  vecSelfChrPos.x/ 3200.0f;
	ptSector.y =  vecSelfChrPos.z/ 3200.0f;

	

	//if(ptOldSector.x != ptSector.x || ptOldSector.y != ptSector.y)
	if(!CRYLGameData::Instance()->m_lpSceneManager->m_pCameraScript->ISPlay())
	{
		{
			list<LPFieldInstance>::iterator it;
			for(it = CRYLGameData::Instance()->m_lstFieldItem.begin(); it != CRYLGameData::Instance()->m_lstFieldItem.end();)
			{
				ptItem.x = (*it)->m_vecPos.x / 3200.0f;
				ptItem.y = (*it)->m_vecPos.z / 3200.0f;

				if(ptSector.x - 1 <= ptItem.x && ptItem.x <= ptSector.x + 1 && ptSector.y - 1 <= ptItem.y && ptItem.y <= ptSector.y + 1)
				{
					it++;
				} else
				{
					LPFieldInstance lpInstance = (*it);
					CRYLGameData::Instance()->m_lpSceneManager->DeleteInstanceObject(lpInstance->m_dwInstanceID);
					it = CRYLGameData::Instance()->m_lstFieldItem.erase(it);
					delete lpInstance;
				}
			}
		}

		{
			for(int i = g_CharacterData.m_lstCharData.num-1 ; i>=1  ;i--)
			{
				//if(strcmp(g_CharacterData.m_lstCharData[i]->m_strGCMDSName,"")==0)
				//	continue;
				ptItem.x = g_CharacterData.m_lstCharData[i]->m_vecPos.x / 3200.0f;
				ptItem.y = g_CharacterData.m_lstCharData[i]->m_vecPos.z / 3200.0f;

				if((ptSector.x - 1 <= ptItem.x && ptItem.x <= ptSector.x + 1 && ptSector.y - 1 <= ptItem.y && ptItem.y <= ptSector.y + 1) ||
					CRYLGameData::Instance()->m_csStatus.CheckPartyMember(g_CharacterData.m_lstCharData[i]->m_dwChrID))
				{
					//i++;
				} else
				{
//					char strText[100];
					if(ptItem.x==0 && ptItem.y==0)
						continue;
					if( CRYLGameData::GetClientType(g_CharacterData.m_lstCharData[i]->m_dwChrID) == CT_PC)
					{
						m_lstDeleteChr.push_back(g_CharacterData.m_lstCharData[i]->m_dwChrID);

//						sprintf(strText, "0x%08x님을 리스트에서 지웠습니다.", g_CharacterData.m_lstCharData[i]->m_dwChrID);
//						g_TextEdit.AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
//						SendPacket::DeleteFromAddressList(g_GameSession, ClientConstants::ListType_Client,
//							g_CharacterData.m_lstCharData[i]->m_dwChrID);
//						g_CharacterData.UnLoadChr(g_CharacterData.m_lstCharData[i]->m_dwChrID);
					} else
						g_CharacterData.UnLoadChr(g_CharacterData.m_lstCharData[i]->m_dwChrID);

//					sprintf(strText, "0x%08x을 지웠습니다.", g_CharacterData.m_lstCharData[i]->m_dwChrID);
//					g_TextEdit.AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				}
			}
		}
	}
	ptOldSector.x=ptSector.x;
	ptOldSector.y=ptSector.y;
	
}

BOOL CBaseGUI::SelectNPC(unsigned long dwTargetID, BOOL bButton)
{
	if(g_CharacterData.m_bCharacterTargetNear || g_DeviceInput.GetIsLeftMouseDown() || (!g_TextEdit.IsFocus() && g_DeviceInput.KeyDown('E')))
	{
		if(g_CharacterData.m_bCharacterTargetNear)
		{
			g_CharacterData.m_bCharacterTargetNear = FALSE;
			g_CharacterData.m_dwNormalTargetID = 0xFFFFFFFF;
		}

		switch( CRYLGameData::GetClientType(dwTargetID, TRUE))
		{
			case CT_PC:
				{
					CCreature *lpCreature = g_CharacterData.GetCreature(dwTargetID);
					if(lpCreature)
					{
						if(g_DeviceInput.GetIsLeftMouseDown())
						{
							if(bButton)
							{
								if(strcmp(lpCreature->m_strStreetStallName, ""))
								{
									SendPacket::CharStallEnter(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, lpCreature->m_dwChrID);
									return TRUE;
								}
							}

							if(!g_DeviceInput.KeyHold(VK_SHIFT)) break;
						}
						if(g_DeviceInput.KeyDown('E')) break;
						if(lpCreature->m_cNation != g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_cNation) break;
						if(InitPopup(POPUP_PC, dwTargetID, 0)) return TRUE;
					}
				}
				break;

			case CT_NPC:
				{
					NPCNode *lpNode = g_NPCList.GetNPCNode(dwTargetID);
					if(lpNode)
					{
						if(CRYLGameData::Instance()->m_csStatus.m_lstQuest.size())
						{
							unsigned long dwTriggerCount;
							LPQuestInstance lpQuest = NULL;
							LPTriggerNode lpTrigger = CRYLGameData::Instance()->m_csStatus.GetTriggertoTalk(lpQuest, &dwTriggerCount, dwTargetID);
							if(lpTrigger)
							{
								SendPacket::CharOperateTrigger(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
									(unsigned short)lpQuest->m_dwQuestID, (unsigned char)lpQuest->m_dwPresentPhase,
									(unsigned char)dwTriggerCount, (unsigned char)lpQuest->m_lstTriggerCount[dwTriggerCount]);

								return TRUE;
							}
						}

						if(InitPopup(lpNode->m_dwJob, dwTargetID, 0))
						{
							char strTemp[MAX_PATH];
							sprintf(strTemp, "%s : %s", lpNode->m_strNpcName, lpNode->GetGreeting());
							g_TextEdit.AddNormalChatMessage(strTemp, lpNode->m_dwUID, 130);

							return TRUE;
						}
					}
				}
				break;
		}
	}

	return FALSE;
}

void CBaseGUI::RenderSkillGauge(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	if(g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting)
	{
		unsigned short wSkill = CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetProtoTypeID();	// 시전중인 스킬ID

		const Skill::ProtoType *lpSkillTypeTemp = g_SkillMgr.GetSkillProtoType(wSkill);
		const Skill::ProtoType *lpSkillType = NULL;
		if(lpSkillTypeTemp) lpSkillType = &lpSkillTypeTemp[g_CharacterData.m_dwCastingSkillGrade];

		if(lpSkillType)
		{
			char strTemp[10];
			RECT rcRect;

			unsigned long dwLevelTick = lpSkillType->m_LevelTick * ((500 - ((CRYLGameData::Instance()->m_csStatus.m_Info.DEX + CRYLGameData::Instance()->m_csStatus.m_lAddDEX) - 20)) / 500.0f);
			int level;
			if(dwLevelTick)
			{
				level = g_CharacterData.m_dwCastingSkillTick / dwLevelTick;
			} else
			{
				if(lpSkillType->m_LevelTick)
				{
					level = g_CharacterData.m_dwCastingSkillTick / lpSkillType->m_LevelTick;
				} else
				{
					g_CharacterData.m_bSkillExecuted = FALSE;
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwSkillComboCount = 0;
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bSkillCombo = FALSE;

					char a[MAX_PATH];
					strcpy(a, CRYLStringTable::m_strString[19]);
					CGUIMessageBox *lpMessageBox;
					lpMessageBox = new CGUIMessageBox;
					lpMessageBox->Create(a);

					return;
				}
			}

			m_lpSkillGauge[g_CharacterData.m_dwCastingSkillGrade]->Render(lpD3DDevice);
			m_lpSkillGaugeFrame->Render(lpD3DDevice);
			SetRect(&rcRect, 271, 16, 280, 27);
			sprintf(strTemp, "%d", level);
			g_TextEdit.DrawText(strTemp, &rcRect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255));
		}
	}
}

void CBaseGUI::RenderChrName(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned long dwUID, short wPosX, short wPosY)
{
	RECT rcRect;

	CCreature *lpCreature = g_CharacterData.GetCreature(dwUID);
	unsigned long dwLength = g_TextEdit.GetStringLength(lpCreature->m_strName);
	SetRect(&rcRect, wPosX - (dwLength / 2 + 21), wPosY, wPosX + (dwLength / 2 + 23), wPosY + 30);

	rcRect.left++;	rcRect.right++;	rcRect.top++;	rcRect.bottom++;
	g_TextEdit.DrawText(lpCreature->m_strName, &rcRect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255));
	rcRect.left--;	rcRect.right--;	rcRect.top--;	rcRect.bottom--;
	unsigned long dwType = CRYLGameData::GetClientType(dwUID, TRUE);
	unsigned long dwType1 = CRYLGameData::GetClientType(dwUID);
	
	/*	
	*/

	switch(dwType)
	{
		case CT_MONSTER:
			if(dwType1 == CT_PC)
			{
				if(g_CharacterData.GetIsDuelTarget(dwUID))
				{
					g_TextEdit.DrawText(lpCreature->m_strName, &rcRect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(255, 0, 255, 255));
				} else
				{
					g_TextEdit.DrawText(lpCreature->m_strName, &rcRect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(255, 0, 0, 255));
				}
			} else
			{
				g_TextEdit.DrawText(lpCreature->m_strName, &rcRect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, 255));
			}
			break;

		case CT_PC:
			g_TextEdit.DrawText(lpCreature->m_strName, &rcRect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(188, 184, 255, 255));
			break;
	}
}


void CBaseGUI::PickItem(DWORD nSelectItem)
{
	Item::ItemPos pItemIndex;
	LPFieldInstance lpInstance = CRYLGameData::Instance()->GetFieldObject(nSelectItem);
	if(lpInstance)
	{
		vector3 vecItem = vector3(lpInstance->m_vecPos.x, 0.0f, lpInstance->m_vecPos.z);
		vector3 vecSelf = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos;
		vecSelf.y = 0.0f;
		vecSelf -= vecItem;
		if(vecSelf.GetLens() < 300.0f)
		{
			unsigned char cIndexNum;
			if(!(lpInstance->m_dwTypeID & FieldObject::Gold))
			{
				cIndexNum = CRYLGameData::Instance()->m_csStatus.GetAbleInven(lpInstance, pItemIndex);
			} else
			{
				cIndexNum = 1;
				pItemIndex.m_cIndex = 0;
				pItemIndex.m_cPos = 0;
			}

			if(cIndexNum)
			{
				CRYLNetworkData::SendFieldObjectPickUp(lpInstance->m_dwOID, pItemIndex);
			} else
			{
				g_TextEdit.AddMessage(CRYLStringTable::m_strString[13], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
			}

			return;
		}
	}
}

void CBaseGUI::RenderCrossHair(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned long dwMode)
{
	TLVertex pVertices[4];
	pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
	pVertices[0].Diffuse.a = pVertices[1].Diffuse.a = pVertices[2].Diffuse.a = pVertices[3].Diffuse.a = 0xFF;
	pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
	pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
	pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

	pVertices[0].v.x = pVertices[1].v.x = 393 - 0.5f;
	pVertices[2].v.x = pVertices[3].v.x = 407 - 0.5f;
	pVertices[0].v.y = pVertices[2].v.y = 243 - 0.5f;
	pVertices[1].v.y = pVertices[3].v.y = 257 - 0.5f;

	pVertices[0].tu = pVertices[1].tu = (1 + dwMode * 15) / 128.0f - 0.0005f;
	pVertices[2].tu = pVertices[3].tu = (1 + dwMode * 15 + 14) / 128.0f - 0.0005f;
	pVertices[0].tv = pVertices[2].tv = 41 / 64.0f - 0.0005f;
	pVertices[1].tv = pVertices[3].tv = 55 / 64.0f - 0.0005f;

	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(1, NULL);
	lpD3DDevice->SetTexture(0, m_lpCommonWindow->GetTexture());
	lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
}

void CBaseGUI::ProcessSkill()
{
	// 스킬 발동
	CRYLGameData::CheckTargetforSkill(m_dwSpecialTargetChrID, FALSE);

	if(CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected)
	{
		if(g_DeviceInput.KeyUp(VK_CONTROL) && m_dwSpecialTargetChrID != CRYLNetworkData::Instance()->m_dwMyChrID)
		{
			g_CharacterData.m_bSkillExecuted = FALSE;
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;

			return;
		}

		switch(CRYLGameData::Instance()->m_csStatus.CheckUseSkill())
		{
			case 1:
				g_CharacterData.m_bSkillExecuted = FALSE;
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
				if(!m_bShowSkillError)
				{
					g_TextEdit.AddMessage(CRYLStringTable::m_strString[814], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					m_bShowSkillError = TRUE;
				}
				return;

			case 2:
				g_CharacterData.m_bSkillExecuted = FALSE;
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
				if(!m_bShowSkillError)
				{
					g_TextEdit.AddMessage(CRYLStringTable::m_strString[143], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					m_bShowSkillError = TRUE;
				}
				return;

			case 3:
				g_CharacterData.m_bSkillExecuted = FALSE;
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
				if(!m_bShowSkillError)
				{
					g_TextEdit.AddMessage(CRYLStringTable::m_strString[815], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					m_bShowSkillError = TRUE;
				}
				return;

			case 4:
				g_CharacterData.m_bSkillExecuted = FALSE;
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
				if(!m_bShowSkillError)
				{
					g_TextEdit.AddMessage("패시브 스킬은 쓰지 못합니다.", CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					m_bShowSkillError = TRUE;
				}
				return;
		}

		if(CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetSkillIDbyChantID(17))
		{
			CCreature *lpCreature = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter];
			char strText[MAX_PATH];
			BOOL bError = FALSE;

			if(lpCreature->GetChantFlag(13))
			{
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[816]);
			} else if(lpCreature->GetChantFlag(25))
			{// 홀드
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[802]);
			} else if(lpCreature->GetChantFlag(26))
			{// 스턴
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[803]);
			} else if(lpCreature->GetChantFlag(27))
			{// 프로즌
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[804]);
			} else if(lpCreature->GetChantFlag(28))
			{// 포이즌
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[805]);
			} else if(lpCreature->GetChantFlag(29))
			{// 로우스트렝스
				bError = TRUE;
				strcpy(strText, CRYLStringTable::m_strString[806]);
			}

			if(bError)
			{
				g_CharacterData.m_bSkillExecuted = FALSE;
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
				if(!m_bShowSkillError)
				{
					g_TextEdit.AddMessage(strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
					m_bShowSkillError = TRUE;
				}
				return;
			}
		}

		const Skill::ProtoType *lpSkillType = g_SkillMgr.GetSkillProtoType(CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetProtoTypeID());
		if(lpSkillType == NULL)
		{
			g_CharacterData.m_bSkillExecuted = FALSE;
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
			if(!m_bShowSkillError)
			{
				g_TextEdit.AddMessage(CRYLStringTable::m_strString[144], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				m_bShowSkillError = TRUE;
			}

			return;
		}

		unsigned long dwFunction;
		if( CRYLGameData::CheckStillCasting(dwFunction))
		{
			if(dwFunction != 0xFFFFFFFF) m_dwSpecialTargetChrID = dwFunction;
		}

		switch(lpSkillType->m_eTargetType)
		{
			case Skill::Target::DEAD_ENEMY:
			case Skill::Target::FRIEND:
			case Skill::Target::ENEMY:
			case Skill::Target::DEAD_FRIEND:
			case Skill::Target::FRIEND_OBJECT:
			case Skill::Target::ENEMY_OBJECT:
			case Skill::Target::PARTY:
			case Skill::Target::FRIEND_EXCEPT_SELF:
				if(lpSkillType->m_EffectDistance != 0.0f && m_dwSpecialTargetChrID == 0xFFFFFFFF)
				{
					g_CharacterData.m_bSkillExecuted = FALSE;
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
					if(!m_bShowSkillError)
					{
						g_TextEdit.AddMessage(CRYLStringTable::m_strString[145], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}

					return;
				}
				break;
		}

		if(lpSkillType[CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetMaxDurability()].m_StartMP + lpSkillType->m_LevelMP > CRYLGameData::Instance()->m_csStatus.m_Info.MP)
		{
			g_CharacterData.m_bSkillExecuted = FALSE;
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
			if(!m_bShowSkillError)
			{
				g_TextEdit.AddMessage(CRYLStringTable::m_strString[146], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
				m_bShowSkillError = TRUE;
			}

			return;
		}

		BOOL bUseLimit = TRUE;
		switch(lpSkillType->m_eUseLimit)
		{
			case Skill::UseLimit::FOR_FIGHTER:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::SWORD ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::BLUNT ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::AXE ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::TWOHANDED_BLUNT ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::TWOHANDED_AXE ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::TWOHANDED_SWORD)
					{
						bUseLimit = FALSE;
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[147], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::FOR_FIGHTER_TWOHANDED:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::TWOHANDED_BLUNT ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::TWOHANDED_AXE ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::TWOHANDED_SWORD)
					{
						bUseLimit = FALSE;
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[800], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::WITH_DAGGER:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::DAGGER)
					{
						bUseLimit = FALSE;
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[148], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::FOR_ALL_ONEHAND:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::SWORD ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::BLUNT ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::AXE)
					{
						bUseLimit = FALSE;
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[149], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::RANGED:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::BOW ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[11 + CRYLGameData::Instance()->m_csStatus.m_cWeaponPos]->GetItemType() == Item::ItemType::CROSSBOW)
					{
						bUseLimit = FALSE;
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[150], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::NONE:
				bUseLimit = FALSE;
				break;

			case Skill::UseLimit::WITH_WEAPON:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->GetItemType() == Item::ItemType::COM_BLUNT ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->GetItemType() == Item::ItemType::COM_SWORD ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->GetItemType() == Item::ItemType::OPP_AXE ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->GetItemType() == Item::ItemType::OPP_HAMMER ||
						CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->GetItemType() == Item::ItemType::OPP_SLUSHER)
					{
						bUseLimit = FALSE;
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[149], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::WITH_CLOW:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::WEAPON_HAND1]->GetItemType() == Item::ItemType::OPP_TALON)
					{
						bUseLimit = FALSE;
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[500], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				}
				break;

			case Skill::UseLimit::LEFT_ARM:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
				{
					bUseLimit = FALSE;
				} else
				{
					if(!m_bShowSkillError)
					{
						g_TextEdit.AddMessage(CRYLStringTable::m_strString[501], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}
				}
				break;

			case Skill::UseLimit::GUARD_ARM:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetItemType() == Item::ItemType::SKILL_A_GUARD)
					{
						bUseLimit = FALSE;
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[502], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				} else
				{
					if(!m_bShowSkillError)
					{
						g_TextEdit.AddMessage(CRYLStringTable::m_strString[502], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}
				}
				break;

			case Skill::UseLimit::ATTACK_ARM:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetItemType() == Item::ItemType::SKILL_A_ATTACK)
					{
						bUseLimit = FALSE;
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[503], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				} else
				{
					if(!m_bShowSkillError)
					{
						g_TextEdit.AddMessage(CRYLStringTable::m_strString[503], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}
				}
				break;

			case Skill::UseLimit::GUN_ARM:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetItemType() == Item::ItemType::SKILL_A_GUN)
					{
						unsigned short wAmmo = 4003;
						CItemInstance *lpItem = CRYLGameData::Instance()->m_csStatus.GetInventoryItemfromID(wAmmo);
						if(lpItem)
							bUseLimit = FALSE;
						else
						{
							if(!m_bShowSkillError)
							{
								g_TextEdit.AddMessage(CRYLStringTable::m_strString[504], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
								m_bShowSkillError = TRUE;
							}
						}
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[505], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				} else
				{
					if(!m_bShowSkillError)
					{
						g_TextEdit.AddMessage(CRYLStringTable::m_strString[505], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}
				}
				break;

			case Skill::UseLimit::KNIFE_ARM:
				if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1])
				{
					if(CRYLGameData::Instance()->m_csStatus.m_lstEquipment[Item::EquipmentPos::SHIELD_HAND1]->GetItemType() == Item::ItemType::SKILL_A_KNIFE)
					{
						bUseLimit = FALSE;
					} else
					{
						if(!m_bShowSkillError)
						{
							g_TextEdit.AddMessage(CRYLStringTable::m_strString[506], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
							m_bShowSkillError = TRUE;
						}
					}
				} else
				{
					if(!m_bShowSkillError)
					{
						g_TextEdit.AddMessage(CRYLStringTable::m_strString[506], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
						m_bShowSkillError = TRUE;
					}
				}
				break;
		}

		if(bUseLimit)
		{
			g_CharacterData.m_bSkillExecuted = FALSE;
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;

			return;
		}

		unsigned long dwLowerAni = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwLowerChrAction;
		if(dwLowerAni != CA_CASTING && dwLowerAni != CA_AIMEDSHOT1 && dwLowerAni != CA_AIMEDSHOT2)
		{
			if(lpSkillType->m_usSkill_ID == 0x8803 || lpSkillType->m_usSkill_ID == 0x9506)				// 에임드 샷
			{
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->BeginCast("AIMED_SHOT1");
			} else
			{
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->BeginCast("CASTING");
			}
		}

		if(Skill::Type::INSTANCE == lpSkillType->m_eSkillType)
		{
			g_CharacterData.m_bSkillExecuted = FALSE;
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
			if(g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bFlying && !strcmp(lpSkillType->m_szCastingFileName, "SHOT"))
			{
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->EndSkill("JUMP_SHOT");
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwSkillComboCount = 0;
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bSkillCombo = FALSE;

				// 스킬을 집어넣는다.
				CSkillNode *lpSkillNode;
				unsigned long dwLock = CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetMaxDurability();
				unsigned long dwLevel = CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetNowDurability();
				CRYLGameData::Instance()->m_csStatus.CheckUseSkillforStatus(dwLock, dwLevel);
				lpSkillNode = new CSkillNode;
				lpSkillNode->Create(m_dwSpecialTargetChrID, 0, CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetProtoTypeID(), dwLock, dwLevel);

				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;

				m_bShowSkillError = TRUE;
			} else if(lpSkillType->m_bGauge)
			{
				lpSkillType = &lpSkillType[g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwSkillComboCount];
				if(strcmp(lpSkillType->m_szCastingFileName, "NULL"))
				{
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->EndSkill((char *)lpSkillType->m_szCastingFileName);
				}

				// 스킬을 집어넣는다.
				unsigned long dwGrade, dwLevel;
				dwGrade = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwSkillComboCount;
				if(CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetMaxDurability() == g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwSkillComboCount)
				{
					dwLevel = CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetNowDurability();
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwSkillComboCount = 0;
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bSkillCombo = FALSE;
				} else
				{
					dwLevel = 6;
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwSkillComboCount++;
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bSkillCombo = TRUE;
				}

				CSkillNode *lpSkillNode;
				lpSkillNode = new CSkillNode;
				lpSkillNode->Create(m_dwSpecialTargetChrID, 0, CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetProtoTypeID(), dwGrade, dwLevel);

				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;

				m_bShowSkillError = TRUE;
			} else
			{
				if(strcmp(lpSkillType->m_szCastingFileName, "NULL"))
				{
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->EndSkill((char *)lpSkillType->m_szCastingFileName);
				}
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwSkillComboCount = 0;
				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bSkillCombo = FALSE;

				// 스킬을 집어넣는다.
				CSkillNode *lpSkillNode;
				lpSkillNode = new CSkillNode;
				unsigned long dwLock = CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetMaxDurability();
				unsigned long dwLevel = CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetNowDurability();
				CRYLGameData::Instance()->m_csStatus.CheckUseSkillforStatus(dwLock, dwLevel);
				lpSkillNode->Create(m_dwSpecialTargetChrID, 0, CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetProtoTypeID(), dwLock, dwLevel);

				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;

				m_bShowSkillError = TRUE;
			}
		} else if(g_CharacterData.m_bSkillExecuted)
		{
			g_CharacterData.m_bSkillExecuted = FALSE;
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwSkillComboCount = 0;
			g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bSkillCombo = FALSE;

			if(g_CharacterData.GetCreature(m_dwSpecialTargetChrID))
			{
				if(strcmp(lpSkillType->m_szCastingFileName, "NULL"))
				{
					g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->EndSkill((char *)lpSkillType->m_szCastingFileName);
				}

				// 스킬을 집어넣는다.
				CSkillNode *lpSkillNode;
				lpSkillNode = new CSkillNode;
				lpSkillNode->Create(m_dwSpecialTargetChrID, 0,
					CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetProtoTypeID(),
					g_CharacterData.m_dwCastingSkillGrade, g_CharacterData.m_dwCastingSkillTick);

				g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
			} else
			{
				float fTime = CSceneManager::GetWeatherTime();
				if(( fTime < 16.0f) && (fTime > 9.0f))
					CCameraControl::SetFadeOut(1.0f,1.0f,30.0f);
			}

			m_bShowSkillError = TRUE;
		}
	} else
	{
		g_CharacterData.m_bSkillExecuted = FALSE;
		g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->CancelSkill();
		g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bStillCasting = FALSE;
		g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_dwSkillComboCount = 0;
		g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bSkillCombo = FALSE;
	}
}

BOOL CBaseGUI::InitZone(unsigned long dwZoneNumber)
{
	CRYLGameData::Instance()->m_dwClientState = GAME_LOADING;

	CRYLGameData::Instance()->m_lpSceneManager->GetCamera()->EndPlay();	

	char strPath[MAX_PATH], szAmbienceFile[MAX_PATH], szBGMFile[MAX_PATH], Temp[MAX_PATH];

	CRYLGameData::Instance()->m_lpSceneManager->m_ViewerMode = 1;

	sprintf(strPath, "%s\\NPCScript.mcf", g_ClientMain.m_strClientPath);
	g_NPCList.Load(strPath, dwZoneNumber);

	float fDayTime;
	if(dwZoneNumber == 100)
	{
		fDayTime=17.0f;
	} else
	{
		fDayTime=13.0f;
	}

	CRYLGameData::Instance()->m_lpSceneManager->SetWeatherTime(fDayTime);

	switch(dwZoneNumber)
	{
		case 1:
			sprintf(szAmbienceFile, "%s\\Sound\\am\\Ambience.snm", g_ClientMain.m_strClientPath);
			sprintf( szBGMFile, "%s\\Sound\\am\\Human.trg", g_ClientMain.m_strClientPath );
			sprintf(Temp, "%s\\sound\\am\\hudas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone1.z3s", g_ClientMain.m_strClientPath);

			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataUnload();
			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone1");			
			CRYLGameData::Instance()->m_lpSceneManager->DeleteAllData();
			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataLoad(CRenderOption::m_strBaseGraphicsDataPath);
			CRYLGameData::Instance()->m_lpSceneManager->m_MapStorage.LoadGF3(strPath);
			CRYLGameData::Instance()->m_lpSceneManager->SetMapSize(vector3(47313.0f,0.0f,18492.0f),vector3(338671.0f,0.0f,306341.0f));
			CRYLGameData::Instance()->ResetTerrain();
			break;

		case 2:
			sprintf(szAmbienceFile, "%s\\Sound\\am\\Akhan.snm", g_ClientMain.m_strClientPath);
			sprintf( szBGMFile, "%s\\Sound\\am\\Akhan.trg", g_ClientMain.m_strClientPath );
			sprintf(Temp, "%s\\sound\\am\\akdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone2.z3s", g_ClientMain.m_strClientPath);

			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataUnload();
			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone2");			
			CRYLGameData::Instance()->m_lpSceneManager->DeleteAllData();
			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataLoad(CRenderOption::m_strBaseGraphicsDataPath);
			CRYLGameData::Instance()->m_lpSceneManager->m_MapStorage.LoadGF3(strPath);
			CRYLGameData::Instance()->m_lpSceneManager->SetMapSize(vector3(31507.0f,0.0f,31507.0f),vector3(378084.0f,0.0f,378084.0f));
			CRYLGameData::Instance()->ResetTerrain();
			break;

		case 3:
			sprintf(szAmbienceFile, "%s\\Sound\\am\\bg.snm", g_ClientMain.m_strClientPath);
			sprintf( szBGMFile, "%s\\Sound\\am\\bg.trg", g_ClientMain.m_strClientPath );
			sprintf(Temp, "%s\\sound\\am\\bgdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone3.z3s", g_ClientMain.m_strClientPath);

			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataUnload();
			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone3");
			CRYLGameData::Instance()->m_lpSceneManager->DeleteAllData();
			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataLoad(CRenderOption::m_strBaseGraphicsDataPath);			
			CRYLGameData::Instance()->m_lpSceneManager->m_MapStorage.LoadGF3(strPath);
			CRYLGameData::Instance()->m_lpSceneManager->SetMapSize(vector3(59181.0f,0.0f,57580.0f),vector3(222636.0f,0.0f,224901.0f));
			CRYLGameData::Instance()->ResetTerrain();
			break;

		case 4:
//			sprintf(szAmbienceFile, "%s\\Sound\\am\\bg.snm", g_ClientMain.m_strClientPath);
//			sprintf( szBGMFile, "%s\\Sound\\am\\bg.trg", g_ClientMain.m_strClientPath );
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			sprintf(Temp, "%s\\sound\\am\\hsnjdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone4.z3s", g_ClientMain.m_strClientPath);

			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataUnload();
			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone4");
			CRYLGameData::Instance()->m_lpSceneManager->DeleteAllData();
			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataLoad(CRenderOption::m_strBaseGraphicsDataPath);			
			CRYLGameData::Instance()->m_lpSceneManager->m_MapStorage.LoadGF3(strPath);
			CRYLGameData::Instance()->m_lpSceneManager->SetMapSize(vector3(48430.0f,0.0f,45270.0f),vector3(270453.0f,0.0f,270536.0f));
			CRYLGameData::Instance()->ResetTerrain();
			break;

		case 5:
//			sprintf(szAmbienceFile, "%s\\Sound\\am\\bg.snm", g_ClientMain.m_strClientPath);
//			sprintf( szBGMFile, "%s\\Sound\\am\\bg.trg", g_ClientMain.m_strClientPath );
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			sprintf(Temp, "%s\\sound\\am\\aapjdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone5.z3s", g_ClientMain.m_strClientPath);

			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataUnload();
			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone5");
			CRYLGameData::Instance()->m_lpSceneManager->DeleteAllData();
			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataLoad(CRenderOption::m_strBaseGraphicsDataPath);			
			CRYLGameData::Instance()->m_lpSceneManager->m_MapStorage.LoadGF3(strPath);
			CRYLGameData::Instance()->m_lpSceneManager->SetMapSize(vector3(45000.0f,0.0f,45000.0f),vector3(332583.0f,0.0f,334642.0f));
			CRYLGameData::Instance()->ResetTerrain();
			break;

		case 6:
			sprintf(szAmbienceFile, "%s\\Sound\\am\\bg.snm", g_ClientMain.m_strClientPath);
			sprintf( szBGMFile, "%s\\Sound\\am\\bg.trg", g_ClientMain.m_strClientPath );
			sprintf(Temp, "%s\\sound\\am\\bgdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone3.z3s", g_ClientMain.m_strClientPath);

			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataUnload();
			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone3");
			CRYLGameData::Instance()->m_lpSceneManager->DeleteAllData();
			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataLoad(CRenderOption::m_strBaseGraphicsDataPath);			
			CRYLGameData::Instance()->m_lpSceneManager->m_MapStorage.LoadGF3(strPath);
			CRYLGameData::Instance()->m_lpSceneManager->SetMapSize(vector3(59181.0f,0.0f,57580.0f),vector3(222636.0f,0.0f,224901.0f));
			CRYLGameData::Instance()->ResetTerrain();
			break;

		case 8:
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			sprintf(Temp, "%s\\sound\\am\\hsnjdas.ogg", g_ClientMain.m_strClientPath);
			sprintf(strPath, "%s\\Zone8.z3s", g_ClientMain.m_strClientPath);

			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataUnload();
			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone8");			
			CRYLGameData::Instance()->m_lpSceneManager->DeleteAllData();
			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataLoad(CRenderOption::m_strBaseGraphicsDataPath);
			CRYLGameData::Instance()->m_lpSceneManager->m_MapStorage.LoadGF3(strPath);
			CRYLGameData::Instance()->ResetTerrain();
			break;

		case 100:
//			sprintf(szAmbienceFile, "%s\\Sound\\am\\bg.snm", g_ClientMain.m_strClientPath);
//			sprintf( szBGMFile, "%s\\Sound\\am\\bg.trg", g_ClientMain.m_strClientPath );
//			sprintf(Temp, "%s\\sound\\am\\arena.ogg", g_ClientMain.m_strClientPath);
			strcpy(szAmbienceFile, "");
			strcpy(szBGMFile, "");
			strcpy(Temp, "");
			sprintf(strPath, "%s\\Zone100.z3s", g_ClientMain.m_strClientPath);

			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataUnload();
			CRYLGameData::Instance()->m_lpSceneManager->DeleteAllData();
			strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone100");
			CRYLGameData::Instance()->m_lpSceneManager->BaseGraphicsDataLoad(CRenderOption::m_strBaseGraphicsDataPath);
		

			CRYLGameData::Instance()->m_lpSceneManager->m_MapStorage.LoadGF3(strPath);
//			CRYLGameData::Instance()->m_lpSceneManager->SetMapSize(vector3(59181.0f,0.0f,57580.0f),vector3(222636.0f,0.0f,224901.0f));
			CRYLGameData::Instance()->ResetTerrain();
			break;

		default:
			return FALSE;
	}

	CRYLGameData::Instance()->m_lpSceneManager->SetWeatherTimeAdder(0.000f);
	// 9.30 추가
	CRYLGameData::Instance()->m_lpSceneManager->HeightPosInit();

	//------------------사운드 처리-----------------------//
/*	try
	{
		CSectorSoundMap & sndMap = CSectorSoundMap::GetInstance(g_ClientMain.m_hWnd, 0x02);

		if(strcmp(szAmbienceFile, ""))
		{
			FILE * fpAmb = fopen(szAmbienceFile, "r");													//화일이 있는지 확인하고
			if(fpAmb != NULL)
			{
				fclose(fpAmb);
				sndMap.Create(szAmbienceFile);
			}
		} else
		{
			sndMap.Destroy();
		}

		CBGMController & BGMC = CBGMController::GetInstance( sndMap.GetDS() );
		if(strcmp(szBGMFile, ""))
		{
			FILE * fpBGM = fopen( szBGMFile, "r" );
			if( fpBGM != NULL )
			{
				fclose( fpBGM );
				BGMC.Create( szBGMFile );
			}
		} else
		{
			BGMC.Destroy();
		}

		if(strcmp(Temp, ""))
		{
			CSoundManager &sm = CSoundManager::GetInstance();
			g_ClientMain.m_lpBackGroundMusic = &sm.GetBuffer(Temp, false, true);
			g_ClientMain.m_lpBackGroundMusic->Play(true);
		} else
		{
			if(g_ClientMain.m_lpBackGroundMusic)
			{
				try
				{
					g_ClientMain.m_lpBackGroundMusic->Destroy();
					g_ClientMain.m_lpBackGroundMusic = NULL;
				} catch(exception & e)
				{
					e = 0;
					g_ClientMain.m_lpBackGroundMusic = NULL;
				}
			}
		}
	}
	catch(exception & e)
	{
		MessageBox(NULL, e.what(), " ", MB_OK);
	}*/
	//---------------------------------------------------//

	return TRUE;
}

void CBaseGUI::RenderChant(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	unsigned long dwFirst = 0, dwSecond = 0, dwThird = 0;

	for(unsigned long i = 0; i < 32; i++)
	{
		if(CRYLGameData::Instance()->m_csStatus.m_aryEnchantLevel[i])
		{
			switch(i)
			{
				case 0:
				case 1:
				case 2:
				case 3:
					if(m_lpChant[i]) m_lpChant[i]->Render(lpD3DDevice, 0xFF, 12 + dwFirst * 16, 389);
					dwFirst++;
					break;

				case 14:
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 29:
					if(m_lpChant[i]) m_lpChant[i]->Render(lpD3DDevice, 0xFF, 12 + dwSecond * 16, 406);
					dwSecond++;
					break;

				case 24:
				case 25:
				case 26:
				case 27:
				case 28:
					if(m_lpChant[i]) m_lpChant[i]->Render(lpD3DDevice, 0xFF, 12 + dwThird * 16, 423);
					dwThird++;
					break;
			}
		}
	}

	if(CRYLGameData::Instance()->m_cEliteNation != 0xFF && CRYLGameData::Instance()->m_cEliteLevel > 0)
	{
		if(CRYLGameData::Instance()->m_cEliteNation == CRYLGameData::Instance()->m_dwClientNation)
		{
			if(m_lpChant[37 + (CRYLGameData::Instance()->m_cEliteLevel - 1)])
			{
				m_lpChant[37 + (CRYLGameData::Instance()->m_cEliteLevel - 1)]->Render(lpD3DDevice, 0xFF, 44, 440);
			}
		} else
		{
			if(m_lpChant[32 + (CRYLGameData::Instance()->m_cEliteLevel - 1)])
			{
				m_lpChant[32 + (CRYLGameData::Instance()->m_cEliteLevel - 1)]->Render(lpD3DDevice, 0xFF, 44, 440);
			}
		}
	}

	if(g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_bPeaceMode)
	{
		m_lpChant[42]->Render(lpD3DDevice, 0xFF, 60, 440);
	} else
	{
		m_lpChant[43]->Render(lpD3DDevice, 0xFF, 60, 440);
	}
}

void CBaseGUI::RenderChrName(LPDIRECT3DDEVICE8 lpD3DDevice, CCreature *lpCreature, short wPosX, short wPosY)
{
	RECT rcRect;

	unsigned long dwType = CRYLGameData::GetClientType(lpCreature->m_dwChrID, TRUE);
	unsigned long dwType1 = CRYLGameData::GetClientType(lpCreature->m_dwChrID);
	unsigned long dwColor;
	char *strText = NULL, strSummonText[MAX_PATH];

	switch(dwType)
	{
		case CT_MONSTER:
			if(dwType1 == CT_PC)
			{
				if(g_CharacterData.GetIsDuelTarget(lpCreature->m_dwChrID))
				{
					dwColor = D3DCOLOR_RGBA(255, 0, 255, 255);
				} else
				{
					strText = CRYLGameData::Instance()->m_lpCommonInterface->GetClassName(lpCreature->m_wClass);
					dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
				}
				if(strText == NULL) strText = lpCreature->m_strName;
			} else
			{
				if(lpCreature->m_dwChrID & 0x20000000)
				{
					CCreature *lpMasterCreature = g_CharacterData.GetCreature(lpCreature->m_dwSummonMasterChrID);
					if(lpMasterCreature)
					{
						if(strcmp(lpMasterCreature->m_strName, ""))
						{
							sprintf(strSummonText, "%s의 %s", lpMasterCreature->m_strName, lpCreature->m_strName);
							strText = strSummonText;
						} else
						{
							strText = lpCreature->m_strName;
						}
					} else
					{
						strText = lpCreature->m_strName;
					}
				} else
				{
					strText = lpCreature->m_strName;
				}
				dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
			}
			break;

		case CT_PC:
			strText = lpCreature->m_strName;
			dwColor = D3DCOLOR_RGBA(188, 184, 255, 255);
			break;
	}

	unsigned long dwLength = g_TextEdit.GetStringLength(strText);
	SetRect(&rcRect, wPosX - (dwLength / 2 + 3), wPosY, wPosX + (dwLength / 2 + 3), wPosY + 30);
	rcRect.left++;	rcRect.right++;	rcRect.top++;	rcRect.bottom++;
	g_TextEdit.DrawText(strText, &rcRect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255));
	rcRect.left--;	rcRect.right--;	rcRect.top--;	rcRect.bottom--;
	g_TextEdit.DrawText(strText, &rcRect, DT_CENTER | DT_VCENTER, dwColor);

	unsigned long dwClass = 0;
	unsigned long dwFame = lpCreature->m_dwFame;

	if( 1 <= dwFame && dwFame <= 2000 )
	{
		dwClass = 1;
	} else if( 2001 <= dwFame && dwFame <= 10000 )
	{
		dwClass = 2;
	} else if( 10001 <= dwFame && dwFame <= 25000 )
	{
		dwClass = 3;
	} else if( 25001 <= dwFame && dwFame <= 50000 )
	{
		dwClass = 4;
	} else if( 50001 <= dwFame )
	{
		dwClass = 5;
	}

	if(dwClass)
	{
		rcRect.top += 8;
		m_lpMark[(dwClass - 1) + (lpCreature->m_cNation * 5)]->Render(lpD3DDevice, 0xFF, rcRect.right, rcRect.top);
	}

	CSprite* pSprite = ((CHumanGuildInfo *)CRYLGameData::Instance()->m_lpInterface->m_lstWindow[WINDOW_GUILDINFO])->GetGuildSprite( lpCreature->m_dwGuildID ) ;

	if ( pSprite )
	{
		pSprite->SetPosition( 0, 0 ) ;
		pSprite->Render( lpD3DDevice, 0xFF, ( wPosX - (dwLength / 2 + 3) ) - 20, wPosY + 8 ) ;
	}
	/*
	if ( lpCreature->m_pGuildMarkSprite )
		lpCreature->m_pGuildMarkSprite->Render( lpD3DDevice, 0xFF, rcRect.right - 18, rcRect.top ) ;
	*/

}