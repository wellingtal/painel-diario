
#include "RYLGameData.h"
#include "RYLClientMain.h"

#include <Creature/EnemyCheck.h>
#include <Creature/Monster/MonsterMgr.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>

#include "Vertex.h"
#include "FrameTimer.h"

#include "GUITextEdit.h"

#include "RYLCreature.h"
#include "RYLCharacterDataManager.h"

#include "RYLStringGrid.h"
#include "RYLStringTable.h"
#include "RYLTimer.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLCastleManager.h"
#include "RYLCastle.h"

#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"

#include "RYLRaceBase.h"
#include "RYLLabel.h"
#include "RYLPicture.h"
#include "GMFontTexture.h"

#include "RYLNetworkData.h"

#include "RYLGameTimeManager.h"
#include "GMMemory.h"

void CRYLRaceBase::RenderStreetStallButton(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, char *strText, short wPosX, short wPosY)
{
	RECT rcRect;

	unsigned long dwLength = g_TextEdit.GetStringLength(strText);
	SetRect(&rcRect, wPosX - (dwLength / 2 + 21), wPosY, wPosX + (dwLength / 2 + 23), wPosY + 30);

	if (strcmp(strText, pCreature->GetCharStreetStallNameLabel()->GetCaption()))
	{
		pCreature->DeleteSignpostImg();
	}

	if (pCreature->GetSignpostImg())
		pCreature->GetSignpostImg()->DoDraw(NULL, rcRect, 0);
	else
		pCreature->MakeSignpostImg(strText);
	
	CRYLLabel* Label = pCreature->GetCharStreetStallNameLabel();
	Label->SetCaption(strText);
	Label->SetAlignRect(rcRect);
	RECT rt = Label->SetAlign(CENTER);
	Label->DoDraw(NULL, rt, 0);
}

void CRYLRaceBase::RenderChrButton(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, char *strText, short wPosX, short wPosY)
{
	RECT rcRect;

	unsigned long dwLength = g_TextEdit.GetStringLength(strText);
	SetRect(&rcRect, wPosX - (dwLength / 2 + 21), wPosY, wPosX + (dwLength / 2 + 23), wPosY + 30);

	if (strcmp(strText, pCreature->GetCharNameLabel()->GetCaption()))
	{
		pCreature->DeleteSignpostImg();
	}

	if (pCreature->GetSignpostImg())
		pCreature->GetSignpostImg()->DoDraw(NULL, rcRect, 0);
	else
		pCreature->MakeSignpostImg(strText);
	
	CRYLLabel* Label = pCreature->GetCharNameLabel();
	Label->SetCaption(strText);
	Label->SetAlignRect(rcRect);
	RECT rt = Label->SetAlign(CENTER);
	Label->DoDraw(NULL, rt, 0);
}

void CRYLRaceBase::RenderChrButton(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, unsigned long dwUID, short wPosX, short wPosY, unsigned long dwColor)
{
	RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwUID ) ;
	if ( !pTempCreature )
		return;

	CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() ) ;
	if ( !pTempData )
		return;

	static char szName[ MAX_PATH ], szJob[ MAX_PATH ], szTemp[ MAX_PATH ] ;
	int nNameLen, nJobNameLen ;
	RECT rcRect, rcName, rcJobName ;
	BOOL bHasJobName = FALSE ;
	char* lpToken = NULL ;
	strcpy( szTemp, pTempData->m_strName.c_str() ) ; 
		
//	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::JAPAN )
	if( g_ClientMain.m_iLanguage == 2)
	{
		WCHAR	wszWideString[ MAX_PATH ] ;
		char	szBreak[ MAX_PATH ] ;

		MultiByteToWideChar( CP_ACP, 0, pTempData->m_strName.c_str(), strlen(pTempData->m_strName.c_str()) + 1, wszWideString, MAX_PATH ) ;

		WCHAR*   wtoken = wcstok( wszWideString, L"\\" ) ;
		if ( NULL != wtoken )
		{
			int nLen = wcslen( wtoken ) ;
			WideCharToMultiByte( CP_ACP, 0, wtoken, nLen + 1, szBreak, MAX_PATH, NULL, NULL ) ;
			memcpy( szName, szBreak, nLen * 2 + 1 ) ;
			
			wtoken = wcstok( NULL, L"\\" ) ;
			if ( NULL != wtoken )
			{
				bHasJobName = TRUE;

				nLen = wcslen( wtoken ) ;
				WideCharToMultiByte( CP_ACP, 0, wtoken, nLen + 1, szBreak, MAX_PATH, NULL, NULL ) ;
				memcpy( szJob, szBreak, nLen * 2 + 1 ) ;

				nNameLen = CRYLStringTable::GetStringWidth( szName ) ;
				nJobNameLen = CRYLStringTable::GetStringWidth( szJob ) ;
				int nLargeLen = (nNameLen > nJobNameLen) ? nNameLen : nJobNameLen ;

				SetRect( &rcRect, wPosX - (nLargeLen / 2 + 21), wPosY, wPosX + (nLargeLen / 2 + 23), wPosY + 48 ) ;
				SetRect( &rcName, wPosX - (nLargeLen / 2 + 21), wPosY, wPosX + (nLargeLen / 2 + 23), wPosY + 30 ) ;
				SetRect( &rcJobName, wPosX - (nLargeLen / 2 + 21), wPosY + 18, wPosX + (nLargeLen / 2 + 23), wPosY + 48 ) ;
			}
			else
			{
				nNameLen = CRYLStringTable::GetStringWidth( pTempData->m_strName.c_str() ) ;
				SetRect( &rcRect, wPosX - (nNameLen / 2 + 21), wPosY, wPosX + (nNameLen / 2 + 23), wPosY + 30 ) ;
			}
		}
		else
		{
			// 이름 설정이 잘못되었습니다. 이름이 NULL 이거나 잘못된 경우이다.
			return;
		}
	}
	else
	{
		lpToken = strtok( szTemp, "\\" ) ;
		if ( NULL != lpToken )
		{
			strcpy( szName, lpToken ) ;
			nNameLen = CRYLStringTable::GetStringWidth( szName ) ;

			lpToken = strtok( NULL, "\\" ) ;
			if ( NULL != lpToken )
			{
				bHasJobName = TRUE ;

				strcpy( szJob, lpToken ) ;
				nJobNameLen = CRYLStringTable::GetStringWidth( szJob ) ;
				int nLargeLen = ( nNameLen > nJobNameLen ) ? nNameLen : nJobNameLen ;

				SetRect( &rcRect, wPosX - (nLargeLen / 2 + 21), wPosY, wPosX + (nLargeLen / 2 + 23), wPosY + 48 ) ;
				SetRect( &rcName, wPosX - (nLargeLen / 2 + 21), wPosY, wPosX + (nLargeLen / 2 + 23), wPosY + 30 ) ;
				SetRect( &rcJobName, wPosX - (nLargeLen / 2 + 21), wPosY + 18, wPosX + (nLargeLen / 2 + 23), wPosY + 48 ) ;
			}
			else
			{
				nNameLen = CRYLStringTable::GetStringWidth( pTempData->m_strName.c_str() ) ;
				SetRect( &rcRect, wPosX - (nNameLen / 2 + 21), wPosY, wPosX + (nNameLen / 2 + 23), wPosY + 30 ) ;
			}
		}
		else
		{
			// 이름 설정이 잘못되었습니다. 이름이 NULL 이거나 잘못된 경우이다.
			return ;
		}
	}

	if ( pTempCreature->GetSignpostImg() )
		pTempCreature->GetSignpostImg()->DoDraw( NULL, rcRect, 0 ) ;
	else
		pTempCreature->MakeSignpostImg( const_cast<char* >( pTempData->m_strName.c_str() ) ) ;

	if ( bHasJobName )
	{
		CRYLLabel* lpLabel = pCreature->GetCharNameLabel() ;
		lpLabel->SetCaption( szName ) ;
		lpLabel->SetAlignRect( rcName ) ;
		lpLabel->GetFontTexture()->SetFontColor( dwColor ) ;
		RECT rt = lpLabel->SetAlign( CENTER ) ;
		lpLabel->DoDraw( NULL, rt, 0 ) ;

		CRYLLabel* lpJobLabel = pCreature->GetCharJobNameLabel() ;
		lpJobLabel->SetCaption( szJob ) ;
		lpJobLabel->SetAlignRect( rcJobName ) ;
		lpJobLabel->GetFontTexture()->SetFontColor( dwColor ) ;
		rt = lpJobLabel->SetAlign( CENTER ) ;
		lpJobLabel->DoDraw( NULL, rt, 0 ) ;
	}
	else
	{
		CRYLLabel* lpLabel = pCreature->GetCharNameLabel() ;
		lpLabel->SetCaption( (char*)pTempData->m_strName.c_str() ) ;
		lpLabel->SetAlignRect( rcRect ) ;
		lpLabel->GetFontTexture()->SetFontColor( dwColor ) ;
		RECT rt = lpLabel->SetAlign( CENTER ) ;
		lpLabel->DoDraw( NULL, rt, 0 ) ;
	}
}

void CRYLRaceBase::RenderPopUp(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelect );
	
	if ( pTempCreature )
	{
		if(pTempCreature->GetRideEnable())
		{
			return;
		}
	}

	// 자신이 병기에 탑승한 경우 Skip.
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

	if(pSelfCreature)
	{
		if(EnemyCheck::IsSiegeArms(pSelfCreature->GetObjectType()) || EnemyCheck::IsCastleArms(pSelfCreature->GetObjectType()))		
		{
			return;
		}
	}

	TLVertex		Vertex[16];
	unsigned short	Index[54];
	unsigned long	i;
	CRYLGameData*	pGame = CRYLGameData::Instance();

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

	for (i = 0; i < 16; i++)
	{
		Vertex[i].Diffuse.c = 0xFFFFFFFF;
		Vertex[i].Specular.c = 0;
		Vertex[i].w = 0.1f;
		Vertex[i].v.z = 0.1f;
	}

	for (i = 0; i < 4; i++)
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

	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(0, m_lpPopupTexture->GetTexture());
	lpD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 16, 18, Index, D3DFMT_INDEX16, Vertex, sizeof(TLVertex));

	if (m_dwPopupSelect != 0xFFFFFFFF)
	{
		pGame->RenderRect(lpD3DDevice,
			m_nPopupWindowX + 10, m_nPopupWindowY + 12 + m_dwPopupSelect * 18,
			m_nPopupWindowX + 20 + m_dwPopupWindowLength, m_nPopupWindowY + 30 + m_dwPopupSelect * 18,
			0x80fff9c2, 0x78);
	}

	CRYLStringGrid	popupMenuList(NULL, FALSE);

	popupMenuList.SetLeft(WindowX);
	popupMenuList.SetTop(WindowY);
	popupMenuList.SetWidth(m_dwPopupWindowLength);
	popupMenuList.SetHeight(m_lstPopupMenu.size() * 18);
	popupMenuList.SetColCount(m_lstPopupMenu.size());

	popupMenuList.SetItemHeight(18);

	for (vector<PopupMenu>::iterator it = m_lstPopupMenu.begin(); it != m_lstPopupMenu.end(); ++it)
	{
        popupMenuList.AddItem(0, it->m_strPopupStr, m_dwPopupWindowLength, it->m_dwColor);
	}

	popupMenuList.DrawItems(WindowX + 15, WindowY + 13);
}

void CRYLRaceBase::RenderHPBar(LPDIRECT3DDEVICE8 lpD3DDevice, CharDataNode* pData, 
							   short wPosX, short wPosY, unsigned long ulBackColor, unsigned long ulHPColor)
//void CRYLRaceBase::DrawHealthbar(LPDIRECT3DDEVICE8 lpD3DDevice, CharDataNode* pData, short wPosX, short wPosY,unsigned long amount, unsigned long ulBackColor, unsigned long ulMinHPColor, unsigned long ulHPColor, unsigned long direction, unsigned long showback, unsigned long showborder)
{
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	CRYLGameData*	pGame = CRYLGameData::Instance();

	if (pData->m_ulMaxHP != 0)
	{	
		// 체력 게이햨E바
		// by avalansa
		float fHPRate = ((float)pData->m_lCurrHP / (float)pData->m_ulMaxHP) * 104.0f; // was 68.0f
		RECT rcRect = { wPosX - 50, wPosY + 24, wPosX + 52, wPosY + 34 }; // was wPosX - 34, wPosY + 30, wPosX + 34, wPosY + 34

		if (rcRect.left > g_ClientMain.m_BaseGraphicLayer.m_lScreenSx) 
			return;	

		if (rcRect.left < 0) 
			rcRect.left = 0;

		if (rcRect.right < 0) 
			return;	

		if (rcRect.right > g_ClientMain.m_BaseGraphicLayer.m_lScreenSx) 
			rcRect.right = g_ClientMain.m_BaseGraphicLayer.m_lScreenSx;

		if (rcRect.top > g_ClientMain.m_BaseGraphicLayer.m_lScreenSy) 
			return;

		if (rcRect.top < 0) 
			rcRect.top = 0;

		if (rcRect.bottom < 0) 
			return;	

		if (rcRect.bottom > g_ClientMain.m_BaseGraphicLayer.m_lScreenSy) 
			rcRect.bottom = g_ClientMain.m_BaseGraphicLayer.m_lScreenSy;

	//	pGame->RenderRect(lpD3DDevice, rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, ulHPColor, 0xFF555555);
		pGame->RenderRect(lpD3DDevice, rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, ulHPColor, 0xFF353535);

		SetRect(&rcRect, wPosX - 50, wPosY + 24, wPosX - 52 + fHPRate, wPosY + 34 ); // was wPosX - 34, wPosY + 30, wPosX - 34 + fHPRate, wPosY + 34

		if (rcRect.left > g_ClientMain.m_BaseGraphicLayer.m_lScreenSx) 
			return;	

		if (rcRect.left < 0) 
			rcRect.left = 0;

		if (rcRect.right < 0) 
			return;	

		if (rcRect.right > g_ClientMain.m_BaseGraphicLayer.m_lScreenSx) 
			rcRect.right = g_ClientMain.m_BaseGraphicLayer.m_lScreenSx;

		if (rcRect.top > g_ClientMain.m_BaseGraphicLayer.m_lScreenSy) 
			return;	

		if (rcRect.top < 0) 
			rcRect.top = 0;

		if (rcRect.bottom < 0) 
			return;	

		if (rcRect.bottom > g_ClientMain.m_BaseGraphicLayer.m_lScreenSy) 
			rcRect.bottom = g_ClientMain.m_BaseGraphicLayer.m_lScreenSy;

	//	pGame->RenderRect(lpD3DDevice, rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, ulBackColor, 0xFF999999); // backcol
		pGame->RenderRect(lpD3DDevice, rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, ulBackColor, 0xFF999999);

		m_lpHPGauge->m_nPutX = wPosX - 50; // was -41
		m_lpHPGauge->m_nPutY = wPosY + 23; // was +29
		m_lpHPGauge->Render(lpD3DDevice);
	}
}


void CRYLRaceBase::RenderChrName(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, short wPosX, short wPosY)
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	RECT rcRect;

	unsigned long	dwEnemyType = CRYLGameData::GetClientEnemyType(pCreature->GetCharID());
	unsigned long	dwIdentity = CRYLGameData::GetClientIdentity(pCreature->GetCharID());
	unsigned long	dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
	char*			strText = NULL, strSummonText[MAX_PATH];

	CharDataNode*   pData = RYLCharacterDataManager::Instance()->GetCharData(pCreature->GetCharID());
	if (!pData)
		return;

	RYLCreature* pMasterCreature = NULL;
	if (dwIdentity == Creature::CT_SUMMON)
	{
		// 마스터의 이름을 달아준다.
		pMasterCreature = RYLCreatureManager::Instance()->GetCreature(pData->m_ulSummonMasterCharID);
		if (pMasterCreature)
		{
			CharDataNode* pMasterData = RYLCharacterDataManager::Instance()->GetCharData(pMasterCreature->GetCharID());
			if (!pMasterData)
				return;

			if (pMasterData->m_strName.empty())
			{
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				if (pGame->m_cAccountNation != pMasterData->m_cNation &&
					!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
				{
					sprintf(strSummonText, CRYLStringTable::m_strString[818], CRYLStringTable::m_strString[472], pData->m_strName.c_str());
				}
				else
				{
					sprintf(strSummonText, CRYLStringTable::m_strString[818], pMasterData->m_strName.c_str(), pData->m_strName.c_str());
				}

				strText = strSummonText;
			} 
		} 

		// 색깔은 PC와 동일하다.
		dwIdentity = Creature::CT_PC;
	}

	// 이름의 색깔을 결정
	switch (dwIdentity)
	{
		case Creature::CT_PC:
		{
			switch (dwEnemyType)
			{
				case EnemyCheck::EC_ENEMY:			dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);		break;
				case EnemyCheck::EC_NEUTRAL:		dwColor = D3DCOLOR_RGBA(148, 150, 148, 255);	break;
				case EnemyCheck::EC_FRIEND:			dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);	break;
			}

			// NULL 이 아니면 소환수이다.
			if (NULL == strText)
			{
				// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
				RYLCreature* pTempCreature = pMasterCreature ? pMasterCreature : pCreature;
				if (pTempCreature)
				{
					CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData(pCreature->GetCharID());
/*					
					// edith 2008.06.16 국적이 틀린 적 이름표시로 수정.
					if (pTempData && pGame->m_cAccountNation && pGame->m_cAccountNation != pTempData->m_cNation &&
						!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
					{
						strcpy(strSummonText, CRYLStringTable::GetJobName( pTempData->m_usClass ));
						strText = strSummonText;
					}
*/
					strcpy(strSummonText, pTempData->m_strName.c_str());
					strText = strSummonText;
				}
			}

			RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
			RYLCreature* pTempCreature = pMasterCreature ? pMasterCreature : pCreature;

			if (pSelfCreature && pTempCreature)
			{
				CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
				CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData(pCreature->GetCharID());

				if (pSelfData && pTempData)
				{
					// 파티원
					if (pSelfData->m_ulPartyID && pSelfData->m_ulPartyID == pTempData->m_ulPartyID)
					{
						// 파티 색상 보다 적 색상이 우선한다.!!
						if ( EnemyCheck::EC_ENEMY != dwEnemyType )
						{
							// edith 2009.05.25 파티원 네임태그 색상
							dwColor = D3DCOLOR_RGBA(128, 255, 0, 255);
						}
						break;
					}

					// 길드원
					if (pSelfData->m_ulGuildID && !(pSelfData->m_ulGuildID & Guild::MEMBER_WAIT_BIT))
					{
						if (pSelfData->m_ulGuildID == pTempData->m_ulGuildID)
						{
							// 길드 색상 보다 적 색상이 우선한다.!!
							if ( EnemyCheck::EC_ENEMY != dwEnemyType )
							{
								if(!CRYLGameTimeManager::Instance()->IsGuildWarTime())
								{							
									if(pSelfCreature->GetTactics() || pTempCreature->GetTactics())
									{
										dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
										break;
									}
								}
								
								dwColor = D3DCOLOR_RGBA(255, 204, 0, 255);
							}

							break;
						}
					}
				}
			}

			break;
		}

		// NPC 판넬은 다른 함수에서 제어한다.
		case Creature::CT_NPC:	break; 

		case Creature::CT_MONSTER:
		{
			if (EnemyCheck::EC_ENEMY == dwEnemyType)
			{
				const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType(pCreature->GetCharID() & 0x0000FFFF);
				if (NULL != lpMonster)
				{
					switch (lpMonster->m_MonsterInfo.m_cSkillPattern)
					{
						case MonsterInfo::PATTERN_NAMED:	dwColor = D3DCOLOR_RGBA(255, 0, 255, 255);	break;
						case MonsterInfo::PATTERN_CHIEF:	dwColor = D3DCOLOR_RGBA(0, 210, 255, 255);	break;
						case MonsterInfo::PATTERN_BOSS:		dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);		break;
						case MonsterInfo::PATTERN_GUARD:	dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);		break;
						case MonsterInfo::PATTERN_GATHER:	return;

						default:
						{
							//Minotaurs 레벨별로 몬스터 이름색
							int i = lpMonster->m_CreatureStatus.m_nLevel - ((int)pGame->m_csStatus.m_Info.Level);

   							if( i == 0)  
								dwColor = D3DCOLOR_RGBA(255, 242, 76, 200);		// 노랑
							else if(0 < i && i <= 3 )
								dwColor = D3DCOLOR_RGBA(255, 183, 66, 255);		// 자주
   							else if(i > 3)  
								dwColor = D3DCOLOR_RGBA(255, 70, 70, 255);		// 빨강
							else if(i < 0  && i >= -2  )
								dwColor = D3DCOLOR_RGBA(70, 200, 70, 255);		// 녹색
							else 
								dwColor = D3DCOLOR_RGBA(164, 164, 164, 255);	// 회색
						}
						break;
					}
				}
				else
				{
					dwColor = D3DCOLOR_RGBA(255, 154, 206, 255);	
				}
			}

			if (EnemyCheck::EC_NEUTRAL == dwEnemyType)
			{
				dwColor = D3DCOLOR_RGBA(148, 150, 148, 255);
			}
			break;
		}

		case Creature::CT_STRUCT:
		{
			if (EnemyCheck::EC_ENEMY == dwEnemyType)
			{
				const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType(pCreature->GetCharID() & 0x0000FFFF);
				if (NULL != lpMonster)
					dwColor = D3DCOLOR_RGBA(255, 70, 70, 255);		// 빨강
				else
					dwColor = D3DCOLOR_RGBA(255, 154, 206, 255);	
			}

			if (EnemyCheck::EC_NEUTRAL == dwEnemyType)
			{
				dwColor = D3DCOLOR_RGBA(148, 150, 148, 255);
			}
			break;
		}
	}

	if (NULL == strText)
	{
		strText = const_cast<char*>(pData->m_strName.c_str());
	}

	unsigned long dwLength = g_TextEdit.GetStringLength(strText);
	SetRect(&rcRect, wPosX - (dwLength / 2 + 3), wPosY, wPosX + (dwLength / 2 + 3), wPosY + 30);
	
	CRYLLabel* lpLabel = pCreature->GetCharNameLabel();
	lpLabel->SetCaption(strText);
	lpLabel->SetAlignRect(rcRect);
	lpLabel->GetFontTexture()->SetFontColor(dwColor);
	RECT rt = lpLabel->SetAlign(CENTER);
	lpLabel->DoDraw(NULL, rt, 0);

	// ---------------------------------------------------------------------------------------------------

	CRYLGameScene*	pScene = (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
	CRYLGuildFrameDlg* pDlg = (CRYLGuildFrameDlg*)pScene->GetGuildFrameDlg();
	CRYLGuildInfoPanel* pPanel = (CRYLGuildInfoPanel*)pDlg->GetGuildInfoPanel();

	// 명성에 따른 계급장
	unsigned long dwClass = 0;
	unsigned long dwFame = pData->m_ulFame;

	if (0 < dwFame && dwFame <= 2000)				{ dwClass = 1; 	} 
	else if (2000 < dwFame && dwFame <= 10000)		{ dwClass = 2; 	} 
	else if (10000 < dwFame && dwFame <= 25000)		{ dwClass = 3; 	} 
	else if (25000 < dwFame && dwFame <= 50000)		{ dwClass = 4; 	} 
	else if (50000 < dwFame && dwFame <= 75000)		{ dwClass = 5; 	} 
	else if (75000 < dwFame && dwFame <= 100000)	{ dwClass = 6; 	} 
	else if (100000 < dwFame && dwFame <= 150000)	{ dwClass = 7; 	} 
	else if (150000 < dwFame && dwFame <= 250000)	{ dwClass = 8; 	} 
	else if (250000 < dwFame && dwFame <= 500000)	{ dwClass = 9; 	} 
	else if (500000 < dwFame)						{ dwClass = 10;	} 

	if (dwClass)
	{
		// 국가를 파악한다.
		unsigned char cNation   = Creature::STATELESS;
		unsigned long dwGID     = pData->m_ulGuildID;
		if (0 != dwGID && !(dwGID & Guild::MEMBER_WAIT_BIT))
		{
			LPGUILDLARGEINFOEX  pMyGuildInfo = pPanel->FindGuild(dwGID);
			if (NULL != pMyGuildInfo)
			{
				cNation = pMyGuildInfo->m_cInclination;
			}
		}
		else
		{
			switch (pData->m_cRace)
			{
				case CClass::RaceType::HUMAN :	cNation = Creature::KARTERANT;	break;
				case CClass::RaceType::AKHAN :	cNation = Creature::MERKADIA;	break;
				default :						cNation = Creature::STATELESS;	break;
			}
		}

		// 출력할 계급장을 결정한다.
		unsigned char cIndex = MAX_RANKING_SPRITE;
		switch (cNation)
		{
			case Creature::KARTERANT:	cIndex = (dwClass - 1);			break;	
			case Creature::MERKADIA:	cIndex = (dwClass - 1) + 10;	break;	
			case Creature::ALMIGHTY_PIRATE:		cIndex = (dwClass - 1) + 20;	break;	
		}

		// 계급장 출력
		if (MAX_RANKING_SPRITE != cIndex)
		{
			rcRect.top += 8;

			m_lpMark[cIndex]->SetPosition(0, 0);
			m_lpMark[cIndex]->Render(lpD3DDevice, 0xFF, rcRect.right, rcRect.top);
		}
	}

	// 용병 마크.
	if (pData->m_ulGuildID)
	{
		// 길드 마크 출력
		/*if ((SERVER_ID::STONE_WAR1 > CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone > SERVER_ID::STONE_WAR3) ||
			((SERVER_ID::STONE_WAR1 <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3) && pGame->m_cAccountNation == pData->m_cNation))*/
		{
			CRYLSprite* pSprite = pPanel->GetGuildSprite(pData->m_ulGuildID);

			if(pSprite)
			{
				if(pCreature->GetTactics()==Guild::TACTICS)
				{
					if(CRYLGameTimeManager::Instance()->IsGuildWarTime())
					{
						m_lpTacticsMark->SetPosition(0, 0);				
						m_lpTacticsMark->Render(lpD3DDevice, 0xFF, (wPosX - (dwLength / 2 + 3)) - 29, wPosY + 5);						
						goto lb_Mark;
					}
				}
				else
				{
lb_Mark:
					if (pSprite)
					{
						pSprite->SetPosition(0, 0);
						pSprite->Render(lpD3DDevice, 0xFF, (wPosX - (dwLength / 2 + 3)) - 20, wPosY + 8);


					}
				}			
			}			
		}
	}	
		
	// edith 2009.06.20 공헌훈장 렌더링 (별 렌더링)
	// 국가 공헌 훈장.
	unsigned char cRealmPoint = pCreature->GetRealmPoint();
	if(cRealmPoint)
	{
		for(unsigned char cIndex = 0; cIndex < cRealmPoint; cIndex++)
		{			
			m_lpRealmPoint->SetPosition(0, 0);
			m_lpRealmPoint->Render(lpD3DDevice, 0xFF, wPosX - 8 - (cRealmPoint-1) * 8 + cIndex * 16, wPosY-15);
		}
	}
}


void CRYLRaceBase::RenderHead(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if (!pSelfCreature)
		return;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData(pSelfCreature->GetCharID());
	if (!pSelfData)
		return;

	vector<unsigned long> vecCharID;
	RYLCreatureManager::Instance()->GetCharIDArray(vecCharID);

	std::vector<unsigned long>::reverse_iterator rpos = vecCharID.rbegin();
	std::vector<unsigned long>::reverse_iterator rend = vecCharID.rend();

	for (; rpos != rend; ++rpos)
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature(*rpos); 
		if (NULL == pTempCreature)
			continue;

		// Hide 캐릭터 그리햨E않콅E
		if (pTempCreature->GetEffect()->GetChantStatus(Skill::SpellID::Hide) && pTempCreature != pSelfCreature)
			continue;

		if (pTempCreature->GetRideEnable()) 
			continue;

		if (pTempCreature->m_fChatTimer)
		{
			pTempCreature->m_fChatTimer -= CFrameTimer::GetUpdateTimer(GRYLTimer.GetShowInterfaceTimerID()) * 2.0f;

			if (pTempCreature->m_fChatTimer <= 0.0f)
				pTempCreature->m_fChatTimer = 0.0f;
		}

		if (pTempCreature->GetIsUsed())
		{
			vector3 vecResult, vecDamageResult;
			float fW;

			vector3 vecPos = *(pTempCreature->GetNamePosition());
			BaseGraphicsLayer::TransformVector(vecPos, vecResult, fW);
			if (vecResult.z < 0.0f || vecResult.z > 1.0f) continue;

			vector3 vecDamagePos = *(pTempCreature->GetPosition());
			vecDamagePos.y += 150.0f;
			BaseGraphicsLayer::TransformVector(vecDamagePos, vecDamageResult, fW);
			if (vecDamageResult.z < 0.0f || vecDamageResult.z > 1.0f) continue;

			unsigned long dwIdentity = CRYLGameData::GetClientIdentity(pTempCreature->GetCharID());

			// 아군 석상은 NPC로 처리합니다. (메달 상점 이퓖E가능)
			if (Creature::CT_STRUCT == dwIdentity)
			{
				const CMonsterMgr::MonsterProtoType* lpMonster = g_MonsterMgr.GetMonsterProtoType(pTempCreature->GetCharID() & 0x0000FFFF);
				if (lpMonster)
				{
					if ((1 == lpMonster->m_MonsterInfo.m_cNation && CClass::RaceType::HUMAN == CRYLGameData::Instance()->m_dwClientRace) ||
						(2 == lpMonster->m_MonsterInfo.m_cNation && CClass::RaceType::AKHAN == CRYLGameData::Instance()->m_dwClientRace))
					{
						dwIdentity = Creature::CT_NPC;
					}
				}
			}

			if (Creature::CT_NPC == dwIdentity)
			{
				RenderNPC(lpD3DDevice, pTempCreature, vecResult, vecDamageResult);
			}
			else if (Creature::CT_SIEGE_OBJECT == dwIdentity)
			{
				if (pTempCreature->GetObjectType() == Siege::CAMP ||
					 pTempCreature->GetObjectType() == Siege::MINING_CAMP ||
					 pTempCreature->GetObjectType() == Siege::CAMP_SHOP ||
					 pTempCreature->GetObjectType() == Siege::KARTERANT_WEAPON ||
					 pTempCreature->GetObjectType() == Siege::MERKADIA_WEAPON
					)
				{
					vector3 vecTemp = *(pSelfCreature->GetPosition()) - *(pTempCreature->GetPosition());
					vecTemp.y = 0.0f;

					// 32m 이퍊E멀리 있는 베이스 캠프의 이름 판넬은 그리햨E않는다.
					if (vecTemp.GetLens() > 3200.0f)
						continue;
				}

				RenderSiege(lpD3DDevice, pTempCreature, vecResult, vecDamageResult);
			}
			else
			{
				// 경비병은 RenderMonster 루틴에서 처리한다.
				if (Creature::CT_MONSTER == dwIdentity)
				{
					const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType(pTempCreature->GetCharID() & 0x0000FFFF);
					if (NULL != lpMonster && MonsterInfo::PATTERN_GUARD == lpMonster->m_MonsterInfo.m_cSkillPattern)
					{
						RenderMonster(lpD3DDevice, pTempCreature, vecResult, vecDamageResult);
						continue;
					}
				}

				// 소환 몬스터때문에 아군은 RenderPC로, 적국 PC는 RenderMonster로...
				if (EnemyCheck::EC_ENEMY == CRYLGameData::GetClientEnemyType(pTempCreature->GetCharID()))
				{
					RenderMonster(lpD3DDevice, pTempCreature, vecResult, vecDamageResult);
				}
				else
				{
					RenderPC(lpD3DDevice, pTempCreature, vecResult, vecDamageResult);
				}
			}
		}
	}
}



void CRYLRaceBase::RenderPC(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, vector3 vecResult, vector3 vecDamageResult)
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	CharDataNode*   pData = RYLCharacterDataManager::Instance()->GetCharData(pCreature->GetCharID());
	if (!pData)
		return;

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if (!pSelfCreature)
		return;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData(pSelfCreature->GetCharID());
	if (!pSelfData)
		return;

	unsigned long etType = CRYLGameData::GetClientEnemyType(pCreature->GetCharID());

	if (false == pGame->m_bMovieVersion)
	{
		if (!pCreature->GetIsDead())
		{
			if (pCreature->m_fChatTimer)
			{
				RECT rcRect = { vecResult.x - 150, vecResult.y - 50, vecResult.x + 150, vecResult.y };
								
//				if(CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::JAPAN)
				if( g_ClientMain.m_iLanguage == 2)
				{
					char*	strMessageTemp;
					WCHAR	strWideString[MAX_PATH];
					char	szToken[MAX_PATH];

					strMessageTemp = new char[ strlen(pData->m_strChatMessage.c_str()) + 100 ];
					strcpy(strMessageTemp, CRYLStringTable::MakeWordBreak(pData->m_strChatMessage.c_str(), 300));
										
					MultiByteToWideChar(CP_ACP, 0, strMessageTemp, strlen(strMessageTemp) + 1,
						strWideString, MAX_PATH);			

					WCHAR* wtoken = wcstok(strWideString, L"\\");			

					CRYLLabel* lpLabel = pCreature->GetCharChatMsgLabel();

					for (int nCount = 0; nCount < 3; nCount ++)
					{
						lpLabel[ nCount ].SetCaption("");
					}

					int nCount = 0;

					while(wtoken != NULL)
					{
						int nLen = wcslen(wtoken);

						WideCharToMultiByte(CP_ACP, 0, wtoken, nLen + 1, szToken, MAX_PATH, NULL, NULL);

						lpLabel[ nCount ].SetCaption(szToken);
						lpLabel[ nCount ].SetAlignRect(rcRect);
						lpLabel[ nCount ].GetFontTexture()->SetFontColor(0xffffffff);
						RECT rt = lpLabel[ nCount ].SetAlign(CENTER);
						lpLabel[ nCount ].DoDraw(NULL, rt, 0);
						++ nCount;
						rcRect.top += 28;

						wtoken = wcstok(NULL, L"\\");

						if(nCount==3)
						{
							break;
						}
					}

					delete[] strMessageTemp;
				}
				else
				{
					char *strMessageTemp, *token, seps[] = "\\";

					strMessageTemp = new char[ strlen(pData->m_strChatMessage.c_str()) + 100 ];
					strcpy(strMessageTemp, CRYLStringTable::MakeWordBreak(pData->m_strChatMessage.c_str(), 300));
					token = strtok(strMessageTemp, seps);

					CRYLLabel* lpLabel = pCreature->GetCharChatMsgLabel();

					for (int nCount = 0; nCount < 3; nCount ++)
					{
						lpLabel[ nCount ].SetCaption("");
					}

					int nCount = 0;

					while(token != NULL)
					{
						lpLabel[ nCount ].SetCaption(token);
						lpLabel[ nCount ].SetAlignRect(rcRect);
						lpLabel[ nCount ].GetFontTexture()->SetFontColor(0xffffffff);
						RECT rt = lpLabel[ nCount ].SetAlign(CENTER);
						lpLabel[ nCount ].DoDraw(NULL, rt, 0);
						++ nCount;
						rcRect.top += 28;

						token = strtok(NULL, seps);

						if(nCount==3)
						{
							break;
						}
					}

					delete[] strMessageTemp;
				}				
			}

			BOOL bShow = FALSE;
			if (pGame->m_csStatus.CheckPartyMember(pCreature->GetCharID()))
			{
				if (pGame->m_coOption.m_dwCharName & 1)
				{
					bShow = TRUE;
				}
			}
			else
			{
				switch (etType)
				{
					case EnemyCheck::EC_FRIEND:
					{
						if (pGame->m_coOption.m_dwCharName & 2)
						{
							bShow = TRUE;
						}
						break;
					}

					case EnemyCheck::EC_NEUTRAL:
					{
						if (pGame->m_coOption.m_dwCharName & 4)
						{
							bShow = TRUE;
						}
						break;
					}

					case EnemyCheck::EC_ENEMY:
					{
						if (pGame->m_coOption.m_dwCharName & 8)
						{
							bShow = TRUE;
						}
						break;
					}
				}
			}

			if (pCreature->GetEffect()->GetChantStatus(Skill::SpellID::Stealth) && pData->m_cNation != pSelfData->m_cNation)
			{
				bShow = FALSE;
			}

			if (bShow && (VIEW_PC_NAME == pGame->GetViewNameMode() || VIEW_ALL_NAME == pGame->GetViewNameMode() || pCreature->GetViewName() || 
				(m_dwNormalTargetChrID == pCreature->GetCharID() || m_dwSpecialTargetChrID == pCreature->GetCharID() || m_dwRangedWeaponTargetChrID == pCreature->GetCharID())))
			{
				RenderChrName(lpD3DDevice, pCreature, vecResult.x, vecResult.y);

				if (g_ClientMain.m_wAdminMode > SUPPORTER)
				{
				//	RenderHPBar(lpD3DDevice, pData, vecResult.x, vecResult.y, 0xFFFFFF55, 0xFF302D65);
					RenderHPBar(lpD3DDevice, pData, vecResult.x, vecResult.y, 0xFF00FF00, 0xFFDADADC);
				}
			}

			// edith 2010.01.23 종족이 다르면 상점이름을 다른이름으로 대체하기
			if (!pData->m_strStreetStallName.empty())
			{
				if(pData->m_cNation != pSelfData->m_cNation)
				{
					if(pData->m_cNation == Creature::KARTERANT)
						RenderStreetStallButton(lpD3DDevice, pCreature, CRYLStringTable::m_strString[ 4411 ], vecResult.x, vecResult.y - 25);
					else
						RenderStreetStallButton(lpD3DDevice, pCreature, CRYLStringTable::m_strString[ 4412 ], vecResult.x, vecResult.y - 25);
				}
				else
					RenderStreetStallButton(lpD3DDevice, pCreature, const_cast<char*>(pData->m_strStreetStallName.c_str()), vecResult.x, vecResult.y - 25);
			}
		}
	}

	if (EnemyCheck::EC_ENEMY == etType)
	{
		pCreature->GetDamageEffect()->RenderDamage(lpD3DDevice, pGame->m_bMovieVersion, vecDamageResult, RYLDamageEffect::ENEMY_DAMAGE);
	}
	else
	{
		pCreature->GetDamageEffect()->RenderDamage(lpD3DDevice, pGame->m_bMovieVersion, vecDamageResult, RYLDamageEffect::FRIENDLY_DAMAGE);
	}
}

void CRYLRaceBase::RenderNPC(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, vector3 vecResult, vector3 vecDamageResult )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData(pCreature->GetCharID());
	if (!pData)
		return;

	const unsigned long dwEnemyType = pGame->GetClientEnemyType(pCreature->GetCharID());

	if (!pCreature->GetIsDead() && !pGame->m_bMovieVersion)
	{
		unsigned long dwColor = D3DCOLOR_RGBA(148, 150, 148, 255);
		
		if (EnemyCheck::EC_FRIEND == dwEnemyType)
		{
			dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);

			NPCNode *pNode = g_NPCList.GetNPCNode(pCreature->GetCharID());
			if (pNode)
			{
				// 수행할 것이 있는 퀘스트가 있는지를 검사해서 이름을 초록색으로 해주는 루틴
#ifdef __NEWNPCSCRIPT
				if (pNode->IsExistQuest())
#else
				if (pNode->m_lstQuest.size())
#endif
				{
					//  렌홀드 성안에 NPC는 수성측에서만 사용할 수 있다.
					bool bTownIDCheck = false;

					if(pNode->m_dwTownOrNameID==0x70000001)
					{					
						CRYLCastle*	lpCastle = CRYLCastleManager::Instance()->GetCurrentCastle();

						if(lpCastle)
						{
							if(lpCastle->GetNation()!=CRYLGameData::Instance()->m_cAccountNation )
							{													
								bTownIDCheck = true;
							}
						}											
					}

					if(!bTownIDCheck)
					{
						unsigned short aryComplete[1000] = { 0, };
						unsigned short aryQuest[20] = { 0, };
						unsigned long iNPC = 0;

						list<LPQuestInstance>::iterator it;

						for(it = pGame->m_csStatus.m_lstCompleteQuest.begin(); 
							it != pGame->m_csStatus.m_lstCompleteQuest.end(); ++it, ++iNPC)
						{
							aryComplete[ iNPC ] = (*it)->m_dwQuestID;
						}

						iNPC = 0; 
						for(it = pGame->m_csStatus.m_lstQuest.begin(); 
							it != pGame->m_csStatus.m_lstQuest.end(); ++it, ++iNPC)
						{
							aryQuest[ iNPC ] = (*it)->m_dwQuestID;
						}

						const CHAR_INFOST* pInfo = &pGame->m_csStatus.m_Info;
						size_t dwCompleteQuestSize = pGame->m_csStatus.m_lstCompleteQuest.size();

						pNode->SearchQuestList(pInfo->Level, pInfo->Fame, pInfo->Class, 
							pGame->m_cAccountNation, aryComplete, dwCompleteQuestSize, aryQuest);

						// 하나라도 수행가능한 퀘스트가 있다면..
						if (aryQuest[0] != 0xFFFF)		
						{   
							for (int j = 0; j < 20; j++)
							{
								bool pass = false;
								const char* msg = NULL;
								if (aryQuest[j] == 0xFFFF)
									break;

								for(it = pGame->m_csStatus.m_lstQuest.begin(); 
									it != pGame->m_csStatus.m_lstQuest.end(); it++)
								{
									if (aryQuest[j] == (*it)->m_dwQuestID)
										pass = true;
								}
								if (pass)
									continue;

								msg = pNode->GetHelpWordsForQuestID(aryQuest[j]);
								if (msg)
								{
									RECT rcRect = { vecResult.x - 150, vecResult.y - 50, vecResult.x + 150, vecResult.y };
									
									char *strMessageTemp, *token, seps[] = "\\";
									
									strMessageTemp = new char[ strlen(msg) + 100 ];
									strcpy(strMessageTemp, CRYLStringTable::MakeWordBreak(msg, 300));
									token = strtok(strMessageTemp, seps);

									CRYLLabel* lpLabel = pCreature->GetCharChatMsgLabel();

									for (int nCount = 0; nCount < 3; nCount ++)
									{
										lpLabel[ nCount ].SetCaption("");
									}
								
									int nCount = 0;

									while(token != NULL)
									{
										CRYLLabel* lpLabel = pCreature->GetCharChatMsgLabel();
										lpLabel[ nCount ].SetCaption(token);
										lpLabel[ nCount ].SetAlignRect(rcRect);
										lpLabel[ nCount ].GetFontTexture()->SetFontColor(dwColor);
										RECT rt = lpLabel[ nCount ].SetAlign(CENTER);
										lpLabel[ nCount ].DoDraw(NULL, rt, 0);
										++ nCount;
										rcRect.top += 28;

										token = strtok(NULL, seps);
									}

									delete[] strMessageTemp;
									break;
								}
							}

							dwColor = D3DCOLOR_RGBA(0, 255, 0, 255);

							///// Minotaurs 퀘스트가 있으면 느낌표 띄우기
							std::map<unsigned long,int>::iterator itr = m_mapQuestExclamation.find(pCreature->GetCharID());
							if(itr == m_mapQuestExclamation.end())
							{

								std::map<unsigned long,int>::iterator itr2 = m_mapQuestQuestion.find(pCreature->GetCharID());
								if( itr2 != m_mapQuestQuestion.end() )
								{
									CSceneManager::DeleteInstanceObject(itr2->second);
									m_mapQuestQuestion.erase(itr2);
								}

								vector3 vec = *pCreature->GetPosition();
								
								vec.y += 250;
								//int a = CSceneManager::AddInstanceObject("exclamation mark.R3S",vec,pCreature->GetDirection());
								int a = CSceneManager::AddInstanceObject("Scroll.R3S",vec,pCreature->GetDirection());
								m_mapQuestExclamation.insert(make_pair(pCreature->GetCharID(),a));	
							}else
							{
								CSceneManager::InstanceObjectRotation(itr->second,0.03f);
							}
							
						
						}
						else
						{
							///// 퀘스트가 없으면 지우기
							std::map<unsigned long,int>::iterator itr = m_mapQuestExclamation.find(pCreature->GetCharID());
							if(itr != m_mapQuestExclamation.end())
							{
								CSceneManager::DeleteInstanceObject(itr->second);
								m_mapQuestExclamation.erase(itr);	
								
							}
						}
					}					
				}
			}

			// 현재 수행중인 퀘스트 중에서 TriggerTalk와 TriggerLevelTalk, TriggerFameTalk의 NPC이면 녹색으로 출력
			bool bPass = false;
			if (!pGame->m_csStatus.m_lstQuest.empty())
			{
				
				unsigned long dwTriggerCount; 
				list<LPQuestInstance>::iterator it;

				for (it = pGame->m_csStatus.m_lstQuest.begin(); it != pGame->m_csStatus.m_lstQuest.end(); it++)
				{
					LPQuestInstance pInfo = (*it);
					if(pInfo->m_lpPhaseScript == NULL)
						continue;

					for (dwTriggerCount = 0; dwTriggerCount < pInfo->m_lpPhaseScript->m_lstTrigger.size(); dwTriggerCount++)
					{
						if (pInfo->m_lstTriggerCount[dwTriggerCount] > 0)
						{
							if (pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerKind == TRIGGER_TALK ||
								pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerKind == TRIGGER_FAME_TALK ||
								pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerKind == TRIGGER_LEVEL_TALK ||
								pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerKind == TRIGGER_ABILITY_TALK)
							{
								DWORD dwAA,dwBB;
								dwAA = pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerID;
								dwBB = pCreature->GetCharID();
								unsigned long dwTempValue = pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerID;
								if (pInfo->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwTriggerID == pCreature->GetCharID())
								{
									dwColor = D3DCOLOR_RGBA(0, 255, 0, 255);
									bPass = true;

									///// Minotaurs 진행중인 퀘스트가 있으면 물음표 띄우기
									std::map<unsigned long,int>::iterator itr = m_mapQuestExclamation.find(pCreature->GetCharID());
									std::map<unsigned long,int>::iterator itr2 = m_mapQuestQuestion.find(pCreature->GetCharID());
									if(itr == m_mapQuestExclamation.end() && itr2 == m_mapQuestQuestion.end() )
									{
										vector3 vec = *pCreature->GetPosition();
										vec.y += 80;
										int a = CSceneManager::AddInstanceObject("question mark _1.R3S",vec,pCreature->GetDirection());
										
										m_mapQuestQuestion.insert(make_pair(pCreature->GetCharID(),a));	
									}else if(itr == m_mapQuestExclamation.end() && itr2 != m_mapQuestQuestion.end() )
									{
										CSceneManager::InstanceObjectRotation(itr2->second,0.03f);
									}
									break;
								}
							}	
						}
					}
					if (bPass)
					{
						break;
					}
				}
			}	
			if(!bPass)		///// 퀘스트가 없으면 지우기
			{
				std::map<unsigned long,int>::iterator itr = m_mapQuestQuestion.find(pCreature->GetCharID());
				if(itr != m_mapQuestQuestion.end())
				{
					CSceneManager::DeleteInstanceObject(itr->second);
					m_mapQuestQuestion.erase(itr);	
				}
				
			}
		}

		

		BOOL	bShow = FALSE;
		if (pGame->m_csStatus.CheckPartyMember(pCreature->GetCharID()))
		{
			if (pGame->m_coOption.m_dwCharName & 1)
			{
				bShow = TRUE;
			}
		}
		else
		{
			switch (dwEnemyType)
			{
				case EnemyCheck::EC_FRIEND:
				{
					if (pGame->m_coOption.m_dwCharName & 2)
					{
						bShow = TRUE;
					}
					break;
				}

				case EnemyCheck::EC_NEUTRAL:
				{
					if (pGame->m_coOption.m_dwCharName & 4)
					{
						bShow = TRUE;
					}
					break;
				}

				case EnemyCheck::EC_ENEMY:
				{
					if (pGame->m_coOption.m_dwCharName & 8)
					{
						bShow = TRUE;
					}
					break;
				}
			}
		}

		if (TRUE == bShow)
		{
			if (VIEW_OTHER_NAME == pGame->GetViewNameMode() || VIEW_ALL_NAME == pGame->GetViewNameMode() || 
				pCreature->GetViewName() || pData->m_ulMaxHP != 0 || 
				(m_dwNormalTargetChrID == pCreature->GetCharID() || 
				m_dwSpecialTargetChrID == pCreature->GetCharID() || 
				m_dwRangedWeaponTargetChrID == pCreature->GetCharID()))
			{
				RenderChrButton(lpD3DDevice, pCreature, pCreature->GetCharID(), vecResult.x, vecResult.y, dwColor);
				
				if (pData->m_nChrPattern == 8)		
				{	
					// 체력 게이지바
					//RenderHPBar(lpD3DDevice, pData, vecResult.x, vecResult.y, 0xFF2AE955, 0xFF302D65);
					  RenderHPBar(lpD3DDevice, pData, vecResult.x, vecResult.y, 0xFF00FF00, 0xFFDADADC);
				}
			}
		}

		if (EnemyCheck::EC_ENEMY == dwEnemyType)
		{
			pCreature->GetDamageEffect()->RenderDamage(lpD3DDevice, pGame->m_bMovieVersion, vecDamageResult, RYLDamageEffect::ENEMY_DAMAGE);
		}
		else
		{
			pCreature->GetDamageEffect()->RenderDamage(lpD3DDevice, pGame->m_bMovieVersion, vecDamageResult, RYLDamageEffect::FRIENDLY_DAMAGE);
		}
	}
}

void CRYLRaceBase::RenderMonster(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, vector3 vecResult, vector3 vecDamageResult)
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData(pCreature->GetCharID());
	if (!pData)
		return;

	unsigned long etType = CRYLGameData::GetClientEnemyType(pCreature->GetCharID());

	if (false == pGame->m_bMovieVersion && !pCreature->GetEffect()->GetChantStatus(Skill::SpellID::Stealth))
	{
		if (!pCreature->GetIsDead())
		{
			BOOL bShow = TRUE;

			// 중립이나 적군의 스킬을 보지 않을 때
			if (!(pGame->m_coOption.m_dwCharName & 4) || !(pGame->m_coOption.m_dwCharName & 8))		
			{
				if (CRYLGameData::GetClientIdentity(pCreature->GetCharID()) == Creature::CT_PC)		
				{
					if (!g_CharacterData.GetIsDuelTarget(pCreature->GetCharID()))
					{
						if (etType == EnemyCheck::EC_ENEMY)
						{
							if (!(pGame->m_coOption.m_dwCharName & 8))
							{
								bShow = FALSE;
							}
						}
						else if (etType == EnemyCheck::EC_NEUTRAL)
						{
							if (!(pGame->m_coOption.m_dwCharName & 4))
							{
								bShow = FALSE;
							}
						}
					}
				}
				else					
				{
					if (etType == EnemyCheck::EC_ENEMY)
					{
						if (!(pGame->m_coOption.m_dwCharName & 8))
						{
							bShow = FALSE;
						}
					}
					else if (etType == EnemyCheck::EC_NEUTRAL)
					{
						if (!(pGame->m_coOption.m_dwCharName & 4))
						{
							bShow = FALSE;
						}
					}
				}
			}

			if (TRUE == bShow)
			{
				if (VIEW_OTHER_NAME == pGame->GetViewNameMode() || VIEW_ALL_NAME == pGame->GetViewNameMode() || pCreature->GetViewName() || pData->m_ulMaxHP != 0 || 
					(m_dwNormalTargetChrID == pCreature->GetCharID() || m_dwSpecialTargetChrID == pCreature->GetCharID() || m_dwRangedWeaponTargetChrID == pCreature->GetCharID()))
				{
					RenderChrName(lpD3DDevice, pCreature, vecResult.x, vecResult.y);
					//RenderHPBar(lpD3DDevice, pData, vecResult.x, vecResult.y, 0xFF2AE955, 0xFF302D65);
					  RenderHPBar(lpD3DDevice, pData, vecResult.x, vecResult.y, 0xFF00FF00, 0xFFDADADC);
				}
			}
		}
	}

	if (EnemyCheck::EC_ENEMY == etType)
	{
		pCreature->GetDamageEffect()->RenderDamage(lpD3DDevice, pGame->m_bMovieVersion, vecDamageResult, RYLDamageEffect::ENEMY_DAMAGE);
	}
	else
	{
		pCreature->GetDamageEffect()->RenderDamage(lpD3DDevice, pGame->m_bMovieVersion, vecDamageResult, RYLDamageEffect::FRIENDLY_DAMAGE);
	}
}

void CRYLRaceBase::RenderSiege(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, vector3 vecResult, vector3 vecDamageResult)
{
	char			strNameText[MAX_PATH];
	CRYLGameData*	pGame = CRYLGameData::Instance();

	RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if (!pSelfCreature)
		return;

	CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
	if (!pSelfData)
		return;

	CharDataNode*   pData = RYLCharacterDataManager::Instance()->GetCharData(pCreature->GetCharID());
	if (!pData)
		return;

	unsigned long etType = CRYLGameData::GetClientEnemyType(pCreature->GetCharID());

	if (!pGame->m_bMovieVersion)
	{
		if (pCreature->m_fChatTimer)
		{
			RECT rcRect = { vecResult.x - 150, vecResult.y - 30, vecResult.x + 150, vecResult.y + 30 };
			rcRect.left++; rcRect.right++; rcRect.top++; rcRect.bottom++;
			g_TextEdit.DrawText(pData->m_strChatMessage.c_str(), &rcRect, DT_CENTER | DT_WORDBREAK, D3DCOLOR_RGBA(0, 0, 0, 255));
			rcRect.left--; rcRect.right--; rcRect.top--; rcRect.bottom--;
			g_TextEdit.DrawText(pData->m_strChatMessage.c_str(), &rcRect, DT_CENTER | DT_WORDBREAK, D3DCOLOR_RGBA(255, 255, 255, 255));
		} 
		else
		{
			BOOL bShow = FALSE;
			if (pGame->m_csStatus.CheckPartyMember(pCreature->GetCharID()))
			{
				if (pGame->m_coOption.m_dwCharName & 1)
				{
					bShow = TRUE;
				}
			}
			else
			{
				switch (etType)
				{
					case EnemyCheck::EC_FRIEND:
					{
						if (pGame->m_coOption.m_dwCharName & 2)
						{
							bShow = TRUE;
						}
						break;
					}

					case EnemyCheck::EC_NEUTRAL:
					{
						if (pGame->m_coOption.m_dwCharName & 4)
						{
							bShow = TRUE;
						}
						break;
					}

					case EnemyCheck::EC_ENEMY:
					{
						if (pGame->m_coOption.m_dwCharName & 8)
						{
							bShow = TRUE;
						}
						break;
					}
				}
			}

			if (pCreature->GetEffect()->GetChantStatus(Skill::SpellID::Stealth) && pData->m_cNation != pSelfData->m_cNation)
			{
				bShow = FALSE;
			}

			if (pCreature->GetObjectType())
			{
				WORD wRealObjType, wObjectType;
				wObjectType = pCreature->GetObjectType();
				unsigned char cState = pCreature->GetState();
				unsigned char cSubState = pCreature->GetSubState();
				unsigned char cUpgradeType = pCreature->GetUpgradeType();
				unsigned char cUpgradeStep = pCreature->GetUpgradeStep();

				if (wObjectType == EMBLEM)
				{
					wRealObjType = wObjectType + (cState  * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM) + 
						(cUpgradeType * Siege::UPGRADE_KIND_NUM) + cUpgradeStep;
				}
				else 
				{
					wRealObjType = wObjectType + (cState  * Siege::UPGRADE_KIND_NUM) + cUpgradeStep; 
				}

				const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType(wRealObjType);

				if (lpMonster)
				{
					CRYLGameScene*	pScene = (CRYLGameScene *)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
					CRYLGuildFrameDlg* pDlg = (CRYLGuildFrameDlg*)pScene->GetGuildFrameDlg();
					CRYLGuildInfoPanel* pPanel = (CRYLGuildInfoPanel*)pDlg->GetGuildInfoPanel();

					// 성은 길드의 이름을 달아준다.
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle(pCreature->GetCastleID());
					LPGUILDLARGEINFOEX lpGuildInfo = pPanel->FindGuild(pData->m_ulGuildID);

					switch (wObjectType)
					{
						// '길드이름'의 상징물
						case Siege::EMBLEM:						
						{
							// 길드의 이름을 달아준다.
							if (lpGuildInfo)
							{
								if (strcmp(lpGuildInfo->m_szName, ""))
								{
									sprintf(strNameText, CRYLStringTable::m_strString[ 2641 ], lpGuildInfo->m_szName, lpMonster->m_MonsterInfo.m_strName);
								}
							}
							else if (lpCastle && strcmp(lpCastle->GetCastleName(), ""))
							{
								sprintf(strNameText, CRYLStringTable::m_strString[ 2642 ], lpCastle->GetCastleName(), lpMonster->m_MonsterInfo.m_strName);
							}
							else
							{
								sprintf(strNameText, "%s", lpMonster->m_MonsterInfo.m_strName);
							}
							break;
						}

						// '성이름'의 성문 UP '길드이름'의 성
						case Siege::GATE:						
						{
							if (lpCastle && lpGuildInfo)
							{
								if (strcmp(lpCastle->GetCastleName(), "") && strcmp(lpGuildInfo->m_szName, ""))
								{
									sprintf(strNameText, CRYLStringTable::m_strString[ 2643 ], 
										lpCastle->GetCastleName(), lpMonster->m_MonsterInfo.m_strName, lpGuildInfo->m_szName);
								}
							}
							else if (lpCastle && strcmp(lpCastle->GetCastleName(), ""))
							{
								sprintf(strNameText, CRYLStringTable::m_strString[ 2642 ], lpCastle->GetCastleName(), lpMonster->m_MonsterInfo.m_strName);
							}
							else
							{
								sprintf(strNameText, "%s", lpMonster->m_MonsterInfo.m_strName);
							}
							break;
						}

						// '길드이름' 성의 뒷문
						case Siege::BACKDOOR:					
						{
							// 길드의 이름을 달아준다.
							if (lpGuildInfo)
							{
								if (strcmp(lpGuildInfo->m_szName, ""))
								{
									sprintf(strNameText, CRYLStringTable::m_strString[ 2642 ], lpGuildInfo->m_szName, lpMonster->m_MonsterInfo.m_strName);
								}
							}
							else if (lpCastle && strcmp(lpCastle->GetCastleName(), ""))
							{
								sprintf(strNameText, CRYLStringTable::m_strString[ 2642 ], lpCastle->GetCastleName(), lpMonster->m_MonsterInfo.m_strName);
							}
							else
							{
								sprintf(strNameText, "%s", lpMonster->m_MonsterInfo.m_strName);
							}
							break;
						}

						case Siege::CASTLE_ARMS_NPC:			// 병기 관리 NPC
						{
							strcpy(strNameText, lpMonster->m_MonsterInfo.m_strName);
							break;
						}

						case Siege::SHORT_RANGE_CASTLE_ARMS:	// 근거리 수성 병기
						case Siege::LONG_RANGE_CASTLE_ARMS:		// 원거리 수성 병기
						case Siege::GUARD:						// 가드
						{
							// '소유주이름' + 병기 이름
							RYLCreature* lpOwner = RYLCreatureManager::Instance()->GetCreature( pCreature->GetOwnerID() ) ;
							if ( lpOwner )
							{
								CharDataNode* lpOwnerData = RYLCharacterDataManager::Instance()->GetCharData( lpOwner->GetCharID() ) ;
								if ( lpOwnerData )
								{
									sprintf( strNameText, CRYLStringTable::m_strString[ 4081 ], lpOwnerData->m_strName.c_str(), lpMonster->m_MonsterInfo.m_strName ) ;
								}
								else
								{
									sprintf( strNameText, "%s", lpMonster->m_MonsterInfo.m_strName ) ;
								}
							}
							else
							{
								sprintf( strNameText, "%s", lpMonster->m_MonsterInfo.m_strName ) ;
							}
							break;
						}

						// '길드이름'의 진햨E
						case Siege::CAMP:						
						case Siege::MINING_CAMP:
						case Siege::CAMP_SHOP:
						case Siege::KARTERANT_WEAPON:
						case Siege::MERKADIA_WEAPON:
						{
							// 길드의 이름을 달아준다.
							if (lpGuildInfo)
							{
								if (strcmp(lpGuildInfo->m_szName, ""))
								{
									sprintf(strNameText, CRYLStringTable::m_strString[ 2641 ], lpGuildInfo->m_szName, lpMonster->m_MonsterInfo.m_strName);
								}
							}
							else
							{
								sprintf(strNameText, CRYLStringTable::m_strString[ 2644 ], lpMonster->m_MonsterInfo.m_strName);
							}

							// 월탛E웨?E무콅E장픸E?표시
							if (Siege::KARTERANT_WEAPON == wObjectType || Siege::MERKADIA_WEAPON == wObjectType)
							{
								switch (cSubState)
								{
									case Siege::WEAPON_EMPTY :
									case Siege::WEAPON_READY :
										break;

									case Siege::WEAPON_CHARGE :
									{
										strcat(strNameText, CRYLStringTable::m_strString[ 3465 ]);
									}
									break;

									case Siege::WEAPON_FIRE :
									{
										strcat(strNameText, CRYLStringTable::m_strString[ 3466 ]);
									}
									break;
								}
							}

							break;
						}

						case Siege::SHORT_RANGE_SIEGE_ARMS:		// '마스터'의 병콅EUP
						case Siege::LONG_RANGE_SIEGE_ARMS:		// '마스터'의 병콅EUP
						case Siege::AIRSHIP:					// '마스터'의 병콅EUP
						{
							// 마스터의 이름을 달아준다.
							RYLCreature* pMasterCreature = RYLCreatureManager::Instance()->GetCreature(pCreature->GetOwnerID());
							if (pMasterCreature)
							{
								CharDataNode* pMasterData = RYLCharacterDataManager::Instance()->GetCharData(pMasterCreature->GetCharID());
								if (pMasterData && !pMasterData->m_strName.empty())
								{
									sprintf(strNameText, CRYLStringTable::m_strString[ 4070 ], 
										pMasterData->m_strName.c_str(), lpMonster->m_MonsterInfo.m_strName);
								} 
							}
							else
							{
								sprintf(strNameText, CRYLStringTable::m_strString[ 2644 ], lpMonster->m_MonsterInfo.m_strName);
							}

							break;
						}
					}

					if (!pGame->m_bMovieVersion)
					{
						if (VIEW_OTHER_NAME == pGame->GetViewNameMode() || VIEW_ALL_NAME == pGame->GetViewNameMode() || pCreature->GetViewName() || 
							(m_dwNormalTargetChrID == pCreature->GetCharID() || m_dwSpecialTargetChrID == pCreature->GetCharID() || m_dwRangedWeaponTargetChrID == pCreature->GetCharID()))
						{
							RenderChrButton(lpD3DDevice, pCreature, (char*)strNameText, vecResult.x, vecResult.y - 20);
						}
					}
				}
			}
		}
	}

	if (!pGame->m_bMovieVersion)
	{
		if (VIEW_OTHER_NAME == pGame->GetViewNameMode() || VIEW_ALL_NAME == pGame->GetViewNameMode() || pCreature->GetViewName() || 
			(m_dwNormalTargetChrID == pCreature->GetCharID() || m_dwSpecialTargetChrID == pCreature->GetCharID() || m_dwRangedWeaponTargetChrID == pCreature->GetCharID()))
		{
			if(pCreature->GetObjectType()!=GATE && pCreature->GetObjectType()!=CASTLE_ARMS_NPC)
			{
				//RenderHPBar(lpD3DDevice, pData, vecResult.x, vecResult.y, 0xFFFFFF55, 0xFF302D65);
				  RenderHPBar(lpD3DDevice, pData, vecResult.x, vecResult.y, 0xFF00FF00, 0xFFDADADC);
			}
			
		}
	}

	if (EnemyCheck::EC_ENEMY == etType)
	{
		pCreature->GetDamageEffect()->RenderDamage(lpD3DDevice, pGame->m_bMovieVersion, vecDamageResult, RYLDamageEffect::ENEMY_DAMAGE);
	}
	else
	{
		pCreature->GetDamageEffect()->RenderDamage(lpD3DDevice, pGame->m_bMovieVersion, vecDamageResult, RYLDamageEffect::FRIENDLY_DAMAGE);
	}
}

