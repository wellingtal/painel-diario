// ItemInstance.cpp: implementation of the CItemInstance class.
//
//////////////////////////////////////////////////////////////////////

#include "resource.h"
#include "ItemInstance.h"
#include "ItemStorage.h"
#include "WinInput.h"
#include "GUITooltipItem.h"
#include "GUITooltipManager.h"
#include "FrameTimer.h"

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Skill/SkillMgr.h>

#include "RYLGameData.h"
#include "RYLTimer.h"
#include "RYLSpriteEx.h"
#include "GMMemory.h"


Item::CSkillItem::CSkillItem()
:	CItem(m_ItemInfoInstance)
{
	m_cMaxDurability = 0;
}

bool Item::CSkillItem::SetSkillInfo(const Skill::ProtoType *lpSkillInfo)
{
	m_ItemInfoInstance.m_SpriteData = lpSkillInfo->m_SpriteInfo;
	strcpy(m_ItemInfoInstance.m_StringData.m_szItemDescribe, lpSkillInfo->m_szSkillDescribe);

	return true;
}


CItemInstance::CItemInstance()
{
	m_lpItemBase = NULL;
	m_lpSprite = NULL;
	m_lpDSprite = NULL;
	m_lpTooltip = NULL;

	m_lCoolDownTime = 0;
	m_lCoolDownTimeCount = 0;

	m_bSkillEnable = TRUE;
	m_bClassSkill = FALSE;
	m_bSkill = FALSE;

	m_bStoreItem = FALSE;
	m_bTradeItem = FALSE;
	m_bMedalBuyItem = FALSE;
	m_bSkillCouponBuyItem = FALSE;

	m_bShowTooltip = TRUE;
	m_bCanUsed = TRUE;
	m_cCountCooldown = 0;
	m_wGrade = 0;

	m_wPutX = 0;
	m_wPutY = 0;

	for (int i = 0; i < 10; i++)
	{
		m_pCoolDownVertices[i].Diffuse.c = 0xFFFFFFFF;
		m_pCoolDownVertices[i].Diffuse.a = 0x80;
		m_pCoolDownVertices[i].Specular.c = 0;
		m_pCoolDownVertices[i].w = 0.1f;
		m_pCoolDownVertices[i].v.z = 0.1f;
		m_pCoolDownVertices[i].tu = 0.0f;
		m_pCoolDownVertices[i].tv = 0.0f;
	}

	m_wCoolDownIndex[ 0] = 0;	m_wCoolDownIndex[ 1] = 1;	m_wCoolDownIndex[ 2] = 9; 
	m_wCoolDownIndex[ 3] = 1;	m_wCoolDownIndex[ 4] = 2;	m_wCoolDownIndex[ 5] = 9; 
	m_wCoolDownIndex[ 6] = 2;	m_wCoolDownIndex[ 7] = 3;	m_wCoolDownIndex[ 8] = 9; 
	m_wCoolDownIndex[ 9] = 3;	m_wCoolDownIndex[10] = 4;	m_wCoolDownIndex[11] = 9; 
	m_wCoolDownIndex[12] = 4;	m_wCoolDownIndex[13] = 5;	m_wCoolDownIndex[14] = 9; 
	m_wCoolDownIndex[15] = 5;	m_wCoolDownIndex[16] = 6;	m_wCoolDownIndex[17] = 9; 
	m_wCoolDownIndex[18] = 6;	m_wCoolDownIndex[19] = 7;	m_wCoolDownIndex[20] = 9; 
	m_wCoolDownIndex[21] = 7;	m_wCoolDownIndex[22] = 8;	m_wCoolDownIndex[23] = 9; 
}

CItemInstance::~CItemInstance()
{
	DELETE_ITEM(m_lpItemBase);

	if (m_lpSprite) { delete m_lpSprite; m_lpSprite = NULL; }
	if (m_lpDSprite) { delete m_lpDSprite; m_lpDSprite = NULL; }

	if (m_lpTooltip) { delete m_lpTooltip; m_lpTooltip = NULL; }

	for ( std::vector<CRYLSpriteEx*>::iterator it = m_vecSpriteEx.begin() ; it != m_vecSpriteEx.end() ; ++it )
	{
		GM_DELETE( ( *it ) ) ;
	}
}

Skill::ProtoType* CItemInstance::Render(LPDIRECT3DDEVICE8 lpD3DDevice, bool bAlterSkill )
{
	Skill::ProtoType*	pAltSkill = NULL ;

	if(!m_lpSprite )
	{
		return NULL;
	}

	if (!m_lpSprite->m_lpTexture)
	{
		if (m_bSkill)
		{
			// edith 2008.01.21 퀵슬롯 Desable 아이콘 작업 추가작업을 해줘야한다.
			m_lpSprite->m_lpTexture = g_ItemStorage.GetSkillTexture(GetItemInfo().m_SpriteData.m_szSpriteName, true);

			if (!m_lpDSprite->m_lpTexture)
			{
				// edith 2008.01.21 퀵슬롯 Desable 아이콘 작업 추가작업을 해줘야한다.
				// desable 텍스쳐
				m_lpDSprite->m_lpTexture = g_ItemStorage.GetSkillTexture(GetItemInfo().m_SpriteData.m_szSpriteName, false);
			}
		} 
		else
		{
			m_lpSprite->m_lpTexture = g_ItemStorage.GetItemTexture(GetItemInfo().m_SpriteData.m_szSpriteName);
		}
	}

	CRYLSprite*	lpSprite = m_lpSprite;
	if(m_bSkill && !m_bSkillEnable)
	{
		// 스킬이고 스킬이 Deable 되어있음
		lpSprite = m_lpDSprite;
	}

	if ( m_bSkill && bAlterSkill )
	{
		unsigned short wSkillID = GetProtoTypeID( bAlterSkill ) ;
		const Skill::ProtoType *lpSkillBase = g_SkillMgr.GetSkillProtoType(wSkillID);

		if (lpSkillBase)
		{
			TLVertex pVertices[4];
			pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
			pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
			pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
			pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

			unsigned long SpriteSizeX = (lpSkillBase->m_SpriteInfo.m_nSpriteMaxX - lpSkillBase->m_SpriteInfo.m_nSpriteMinX);
			unsigned long SpriteSizeY = (lpSkillBase->m_SpriteInfo.m_nSpriteMaxY - lpSkillBase->m_SpriteInfo.m_nSpriteMinY);
			pVertices[0].v.x = pVertices[1].v.x = m_wPutX - 0.5f;
			pVertices[2].v.x = pVertices[3].v.x = m_wPutX + 24 - 0.5f;
			pVertices[0].v.y = pVertices[2].v.y = m_wPutY  - 0.5f;
			pVertices[1].v.y = pVertices[3].v.y = m_wPutY + 24 - 0.5f;

			pVertices[0].tu = pVertices[1].tu = lpSkillBase->m_SpriteInfo.m_nSpriteMinX / 256.0f;
			pVertices[2].tu = pVertices[3].tu = lpSkillBase->m_SpriteInfo.m_nSpriteMaxX / 256.0f;
			pVertices[0].tv = pVertices[2].tv = lpSkillBase->m_SpriteInfo.m_nSpriteMinY / 256.0f;
			pVertices[1].tv = pVertices[3].tv = lpSkillBase->m_SpriteInfo.m_nSpriteMaxY / 256.0f;

			lpD3DDevice->SetVertexShader(NULL);
			lpD3DDevice->SetVertexShader(TLVERTEXFVF);
			lpD3DDevice->SetTexture(0, ((CTexture *)g_ItemStorage.GetSkillTexture(lpSkillBase->m_SpriteInfo.m_szSpriteName))->GetTexture());
			lpD3DDevice->SetTexture(1, NULL);
			lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));

			pAltSkill = const_cast< Skill::ProtoType* >( lpSkillBase ) ;
		} 
		else
		{
			lpSprite->Render(lpD3DDevice, 0xFF, m_wPutX, m_wPutY);
		}
	} 
	else
	{
		lpSprite->Render(lpD3DDevice, 0xFF, m_wPutX, m_wPutY);
	}

	if (m_lpItemBase->GetEnableStack() && GetNowDurability() > 1)
	{
		unsigned short wPutX, wPutY;
		if (GetItemPos() == TakeType::TS_QSLOT)
		{
			wPutX = m_wPutX + (lpSprite->GetSizeX() / 2) + 11 * GetItemInfo().m_DetailData.m_cXSize - 9;
			wPutY = m_wPutY + (lpSprite->GetSizeY() / 2) + 16 * GetItemInfo().m_DetailData.m_cYSize - 12;
		} else
		{
			wPutX = m_wPutX + (lpSprite->GetSizeX() / 2) + 13 * GetItemInfo().m_DetailData.m_cXSize - 9;
			wPutY = m_wPutY + (lpSprite->GetSizeY() / 2) + 13 * GetItemInfo().m_DetailData.m_cYSize - 12;
		}

		CRYLGameData::Instance()->RenderStackNumber(lpD3DDevice, wPutX, wPutY, GetNowDurability());
	}

	if (m_lCoolDownTimeCount > 0)
	{
		if (GetItemPos() == TakeType::TS_QSLOT)
		{
			for ( INT i = 0 ; i < 10 ; i ++ )
			{
				m_pCoolDownVertices[ i ].Diffuse.c = 0x8000FFFF ;
			}

			INT iHRange = 13, iRange = 27 ;

			int p = m_lCoolDownTime / 8;
			int m = m_lCoolDownTimeCount / p;
			int n = m_lCoolDownTimeCount % p;
			int jj = iHRange * n / p;
			
			int nPosX = (m_wPutX + (lpSprite->GetSizeX() / 2.0f) - iHRange);
			int nPosY = (m_wPutY + (lpSprite->GetSizeY() / 2.0f) - iHRange);
			switch(m)
			{
				case 0:
					m_pCoolDownVertices[7].v.x = nPosX + iHRange - jj - 0.5f;
					m_pCoolDownVertices[7].v.y = nPosY - 0.5f;
					break;

				case 1:
					m_pCoolDownVertices[6].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[6].v.y = nPosY + jj - 0.5f;

					m_pCoolDownVertices[7].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[7].v.y = nPosY - 0.5f;
					break;

				case 2:
					m_pCoolDownVertices[5].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[5].v.y = nPosY + iHRange + jj - 0.5f;

					m_pCoolDownVertices[6].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[6].v.y = nPosY + iHRange - 0.5f;
					m_pCoolDownVertices[7].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[7].v.y = nPosY - 0.5f;
					break;

				case 3:
					m_pCoolDownVertices[4].v.x = nPosX + jj - 0.5f;
					m_pCoolDownVertices[4].v.y = nPosY + iRange - 0.5f;

					m_pCoolDownVertices[5].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[5].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[6].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[6].v.y = nPosY + iHRange - 0.5f;
					m_pCoolDownVertices[7].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[7].v.y = nPosY - 0.5f;
					break;

				case 4:
					m_pCoolDownVertices[3].v.x = nPosX + iHRange + jj - 0.5f;
					m_pCoolDownVertices[3].v.y = nPosY + iRange - 0.5f;

					m_pCoolDownVertices[4].v.x = nPosX + iHRange - 0.5f;
					m_pCoolDownVertices[4].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[5].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[5].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[6].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[6].v.y = nPosY + iHRange - 0.5f;
					m_pCoolDownVertices[7].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[7].v.y = nPosY - 0.5f;
					break;

				case 5:
					m_pCoolDownVertices[2].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[2].v.y = nPosY + iRange - jj - 0.5f;

					m_pCoolDownVertices[3].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[3].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[4].v.x = nPosX + iHRange - 0.5f;
					m_pCoolDownVertices[4].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[5].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[5].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[6].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[6].v.y = nPosY + iHRange - 0.5f;
					m_pCoolDownVertices[7].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[7].v.y = nPosY - 0.5f;
					break;

				case 6:
					m_pCoolDownVertices[1].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[1].v.y = nPosY + iHRange - jj - 0.5f;

					m_pCoolDownVertices[2].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[2].v.y = nPosY + iHRange - 0.5f;
					m_pCoolDownVertices[3].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[3].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[4].v.x = nPosX + iHRange - 0.5f;
					m_pCoolDownVertices[4].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[5].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[5].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[6].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[6].v.y = nPosY + iHRange - 0.5f;
					m_pCoolDownVertices[7].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[7].v.y = nPosY - 0.5f;
					break;

				case 7:
					m_pCoolDownVertices[0].v.x = nPosX + iRange - jj - 0.5f;
					m_pCoolDownVertices[0].v.y = nPosY - 0.5f;

					m_pCoolDownVertices[1].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[1].v.y = nPosY - 0.5f;
					m_pCoolDownVertices[2].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[2].v.y = nPosY + iHRange - 0.5f;
					m_pCoolDownVertices[3].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[3].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[4].v.x = nPosX + iHRange - 0.5f;
					m_pCoolDownVertices[4].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[5].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[5].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[6].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[6].v.y = nPosY + iHRange - 0.5f;
					m_pCoolDownVertices[7].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[7].v.y = nPosY - 0.5f;
					break;

				case 8:
					m_pCoolDownVertices[0].v.x = nPosX + iHRange - 0.5f;
					m_pCoolDownVertices[0].v.y = nPosY - 0.5f;
					m_pCoolDownVertices[1].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[1].v.y = nPosY - 0.5f;
					m_pCoolDownVertices[2].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[2].v.y = nPosY + iHRange - 0.5f;
					m_pCoolDownVertices[3].v.x = nPosX + iRange - 0.5f;
					m_pCoolDownVertices[3].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[4].v.x = nPosX + iHRange - 0.5f;
					m_pCoolDownVertices[4].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[5].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[5].v.y = nPosY + iRange - 0.5f;
					m_pCoolDownVertices[6].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[6].v.y = nPosY + iHRange - 0.5f;
					m_pCoolDownVertices[7].v.x = nPosX - 0.5f;
					m_pCoolDownVertices[7].v.y = nPosY - 0.5f;
					break;
			}

			m_pCoolDownVertices[8].v.x = nPosX + iHRange - 0.5f;
			m_pCoolDownVertices[8].v.y = nPosY - 0.5f;
			m_pCoolDownVertices[9].v.x = nPosX + iHRange - 0.5f;
			m_pCoolDownVertices[9].v.y = nPosY + iHRange - 0.5f;

			lpD3DDevice->SetVertexShader(NULL);
			lpD3DDevice->SetVertexShader(TLVERTEXFVF);
			lpD3DDevice->SetTexture(0, NULL);
			lpD3DDevice->SetTexture(1, NULL);
			long pIndex = 0, pPri = 8;
			if (m < 8)
			{
				pIndex = 7 - m;
				pPri = m + 1;
			}

			lpD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, pIndex, 10 - pIndex, pPri, &m_wCoolDownIndex[pIndex * 3], D3DFMT_INDEX16, m_pCoolDownVertices, sizeof(TLVertex));
		}/* else
		{
		}*/
	}

	if (m_bShowTooltip && m_lpTooltip && IsOverMouse())
	{
		g_TooltipManager.m_ttRenderTarget = m_lpTooltip;
	}

	return pAltSkill ;
}

void CItemInstance::RenderEnableChant(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	if(!m_lpSprite )
		return;

	for ( INT i = 0 ; i < 10 ; i ++ )
	{
		m_pCoolDownVertices[ i ].Diffuse.c = 0x80FFFF00 ;
	}

	INT iHRange = 13, iRange = 27 ;
	int nPosX = (m_wPutX + (m_lpSprite->GetSizeX() / 2.0f) - iHRange);
	int nPosY = (m_wPutY + (m_lpSprite->GetSizeY() / 2.0f) - iHRange);


	m_pCoolDownVertices[0].v.x = nPosX + iHRange - 0.5f;
	m_pCoolDownVertices[0].v.y = nPosY - 0.5f;
	m_pCoolDownVertices[1].v.x = nPosX + iRange - 0.5f;
	m_pCoolDownVertices[1].v.y = nPosY - 0.5f;
	m_pCoolDownVertices[2].v.x = nPosX + iRange - 0.5f;
	m_pCoolDownVertices[2].v.y = nPosY + iHRange - 0.5f;
	m_pCoolDownVertices[3].v.x = nPosX + iRange - 0.5f;
	m_pCoolDownVertices[3].v.y = nPosY + iRange - 0.5f;
	m_pCoolDownVertices[4].v.x = nPosX + iHRange - 0.5f;
	m_pCoolDownVertices[4].v.y = nPosY + iRange - 0.5f;
	m_pCoolDownVertices[5].v.x = nPosX - 0.5f;
	m_pCoolDownVertices[5].v.y = nPosY + iRange - 0.5f;
	m_pCoolDownVertices[6].v.x = nPosX - 0.5f;
	m_pCoolDownVertices[6].v.y = nPosY + iHRange - 0.5f;
	m_pCoolDownVertices[7].v.x = nPosX - 0.5f;
	m_pCoolDownVertices[7].v.y = nPosY - 0.5f;
	m_pCoolDownVertices[8].v.x = nPosX + iHRange - 0.5f;
	m_pCoolDownVertices[8].v.y = nPosY - 0.5f;
	m_pCoolDownVertices[9].v.x = nPosX + iHRange - 0.5f;
	m_pCoolDownVertices[9].v.y = nPosY + iHRange - 0.5f;

	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetVertexShader(TLVERTEXFVF);
	lpD3DDevice->SetTexture(0, NULL);
	lpD3DDevice->SetTexture(1, NULL);
	long pIndex = 0, pPri = 8;
	lpD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, pIndex, 10 - pIndex, pPri, &m_wCoolDownIndex[pIndex * 3], D3DFMT_INDEX16, m_pCoolDownVertices, sizeof(TLVertex));
}

BOOL CItemInstance::IsOverMouse(void)
{
	if (g_DeviceInput.InRect(m_wPutX, m_wPutY, m_wPutX + m_lpSprite->GetSizeX(), m_wPutY + m_lpSprite->GetSizeY()))
	{
		return TRUE;
	}

	return FALSE;
}

int CItemInstance::SetItemInfo(const char* lpSerializedItem_In, size_t& nParseLength_InOut)
{
	int nDBSize = nParseLength_InOut;
	m_lpItemBase = Item::CItemFactory::GetInstance().CreateItem(lpSerializedItem_In, nParseLength_InOut);
	if (m_lpItemBase)
	{
		m_ipPreIndex = m_lpItemBase->GetPos();
		m_dwItemUID = m_lpItemBase->GetUID();
		m_lpSprite = new CRYLSprite;
		m_lpSprite->Create( 0, 0, GetItemInfo().m_SpriteData.m_nSpriteMinX,
							GetItemInfo().m_SpriteData.m_nSpriteMinY,
							GetItemInfo().m_SpriteData.m_nSpriteMaxX,
							GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL ) ;

		std::vector<CRYLSpriteEx*>			m_vecSpriteEx ;

		m_lpTooltip = new CGUITooltipItem;
		((CGUITooltipItem *)m_lpTooltip)->SetItem(this);

		if (GetItemType() == Item::ItemType::POTION)
		{
			m_lCoolDownTime = 14000;

			Item::CUseItem *lpItem = Item::CUseItem::DowncastToUseItem(m_lpItemBase);
			if (lpItem)
			{
				const Skill::ProtoType *lpSkillBase = g_SkillMgr.GetSkillProtoType(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID), *lpSkill;
				if (lpSkillBase)
				{
					lpSkill = &lpSkillBase[lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount];
					m_lCoolDownTime = lpSkill->m_dwCoolDownTime;
				}
			}

			m_lCoolDownTimeCount = CRYLGameData::Instance()->m_csStatus.GetPotionCoolTime(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID);
		}
	}

	return nDBSize;
}

int CItemInstance::SetItemInfofromID(unsigned short wProtoTypeID, Item::ItemPos &pItemIndex)
{
	m_lpItemBase = Item::CItemFactory::GetInstance().CreateItem(wProtoTypeID);
	if (m_lpItemBase)
	{
		m_lpItemBase->GetPos().SetPos(pItemIndex.m_cIndex);

		SetPos(pItemIndex.m_cIndex);
		SetItemPos(pItemIndex.m_cPos);
		m_ipPreIndex = m_lpItemBase->GetPos();

		m_lpSprite = new CRYLSprite;
		m_lpSprite->Create(0, 0, GetItemInfo().m_SpriteData.m_nSpriteMinX,
			GetItemInfo().m_SpriteData.m_nSpriteMinY,
			GetItemInfo().m_SpriteData.m_nSpriteMaxX,
			GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL);
		m_lpTooltip = new CGUITooltipItem;
		((CGUITooltipItem *)m_lpTooltip)->SetItem(this);

		if (GetItemType() == Item::ItemType::POTION)
		{
			m_lCoolDownTime = 14000;

			Item::CUseItem *lpItem = Item::CUseItem::DowncastToUseItem(m_lpItemBase);
			if (lpItem)
			{
				const Skill::ProtoType *lpSkillBase = g_SkillMgr.GetSkillProtoType(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID), *lpSkill;
				if (lpSkillBase)
				{
					lpSkill = &lpSkillBase[lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount];
					m_lCoolDownTime = lpSkill->m_dwCoolDownTime;
				}
			}

			m_lCoolDownTimeCount = CRYLGameData::Instance()->m_csStatus.GetPotionCoolTime(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID);
		}

		return 1;
	}

	return 0;
}

int CItemInstance::SetItemInfofromItem( Item::CItem* pBaseItem, Item::ItemPos &pItemIndex)
{
	if (pBaseItem)
	{
		char Buffer[Item::MAX_ITEM_SIZE];
		unsigned int dwItemSize = Item::MAX_ITEM_SIZE;
		int size = 0;
		size = sizeof(Item::ItemData);
		size = sizeof(Item::EquipmentInfo);
		size = Item::EquipmentInfo::MAX_SOCKET_AND_ATTRIBUTE_SIZE;
		size = Item::EquipmentInfo::MAX_SOCKET_NUM;
		size = Item::EquipmentInfo::MAX_ATTRIBUTE_NUM;
		size = sizeof(Item::ItemAttribute);


		pBaseItem->SerializeOut(Buffer, dwItemSize);
		if (dwItemSize)
		{
			m_lpItemBase = Item::CItemFactory::GetInstance().CreateItem(Buffer, dwItemSize);
		}
	} 
	else
	{
		m_lpItemBase = NULL;
	}

	if (m_lpItemBase)
	{
		m_lpItemBase->SetPos(pItemIndex.m_cIndex);
		m_lpItemBase->SetItemPos(pItemIndex.m_cPos);
		m_ipPreIndex = m_lpItemBase->GetPos();
		m_dwItemUID = m_lpItemBase->GetUID();

		m_lpSprite = new CRYLSprite;
		m_lpSprite->Create(0, 0, GetItemInfo().m_SpriteData.m_nSpriteMinX,
			GetItemInfo().m_SpriteData.m_nSpriteMinY,
			GetItemInfo().m_SpriteData.m_nSpriteMaxX,
			GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL);
		m_lpTooltip = new CGUITooltipItem;
		((CGUITooltipItem *)m_lpTooltip)->SetItem(this);

		if (GetItemType() == Item::ItemType::POTION)
		{
			m_lCoolDownTime = 14000;

			Item::CUseItem *lpItem = Item::CUseItem::DowncastToUseItem(m_lpItemBase);
			if (lpItem)
			{
				const Skill::ProtoType *lpSkillBase = g_SkillMgr.GetSkillProtoType(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID), *lpSkill;
				if (lpSkillBase)
				{
					lpSkill = &lpSkillBase[lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount];
					m_lCoolDownTime = lpSkill->m_dwCoolDownTime;
				}
			}

			m_lCoolDownTimeCount = CRYLGameData::Instance()->m_csStatus.GetPotionCoolTime(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID);
		}

		return 1;
	}

	return 0;
}

BOOL CItemInstance::UpdateCoolDownTime()
{
	if (!m_cCountCooldown)
	{
		float fUpdate = CFrameTimer::GetUpdateTimer(GRYLTimer.GetShowInterfaceTimerID());
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if ( !pSelfCreature )
            return false;

        if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Frozen ) )
		{
			m_lCoolDownTimeCount -= fUpdate * 2.0f; // m_lCoolDownTimeCount -= fUpdate; hayzohar
		}
		else
		{
			m_lCoolDownTimeCount -= fUpdate * 2.0f; //m_lCoolDownTimeCount -= fUpdate * 2.0f; hayzohar
		}

		if (m_lCoolDownTimeCount <= 0)
		{
			m_lCoolDownTimeCount = 0;
			m_wGrade = 0;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CItemInstance::SetSkillInfo(unsigned short wSkillID, unsigned char cGrade, unsigned char cLevel, unsigned char cPos, unsigned char cIndex, long lCoolDown)
{
	const Skill::ProtoType *lpSkillProtoType = g_SkillMgr.GetSkillProtoType(wSkillID);

	if (lpSkillProtoType)
	{
		lpSkillProtoType = &lpSkillProtoType[cGrade];

		if (lpSkillProtoType)
		{
			m_bSkill = TRUE;
			m_wPreProtoTypeID = wSkillID;

			if (lpSkillProtoType->m_bIsClassSkill) m_bClassSkill = TRUE; else m_bClassSkill = FALSE;

			m_lpItemBase = new Item::CSkillItem;

			SetItemPos(cPos);
			SetPos(cIndex);
			m_ipPreIndex = m_lpItemBase->GetPos();

			((Item::CSkillItem *)m_lpItemBase)->SetSkillInfo(lpSkillProtoType);
			((Item::CSkillItem *)m_lpItemBase)->SetProtoTypeID(wSkillID);
			((Item::CSkillItem *)m_lpItemBase)->SetMaxNumOrDurability(cGrade);
			SetNowDurability(cLevel);

			// 이디스 추가
			m_wGrade = cGrade;

			m_lCoolDownTime = lpSkillProtoType->m_dwCoolDownTime;
			m_lCoolDownTimeCount = lCoolDown;

			m_lpSprite = new CRYLSprite;
			m_lpSprite->Create(0, 0, GetItemInfo().m_SpriteData.m_nSpriteMinX,
				GetItemInfo().m_SpriteData.m_nSpriteMinY,
				GetItemInfo().m_SpriteData.m_nSpriteMaxX,
				GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL);

			m_lpDSprite = new CRYLSprite;
			m_lpDSprite->Create(0, 0, GetItemInfo().m_SpriteData.m_nSpriteMinX,
				GetItemInfo().m_SpriteData.m_nSpriteMinY,
				GetItemInfo().m_SpriteData.m_nSpriteMaxX,
				GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL);


			m_lpTooltip = new CGUITooltipItem;
			((CGUITooltipItem *)m_lpTooltip)->SetItem(this);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CItemInstance::SetSkillInfo(CItemInstance *lpSkill)
{
	if (lpSkill)
	{
		m_bSkill = TRUE;
		m_bClassSkill = lpSkill->m_bClassSkill;
		m_wPreProtoTypeID = lpSkill->m_lpItemBase->GetPrototypeID();

		unsigned long wSkillID = m_wPreProtoTypeID;
		const Skill::ProtoType *lpSkillProtoType = g_SkillMgr.GetSkillProtoType(wSkillID);
		if (lpSkillProtoType)
		{
			lpSkillProtoType = &lpSkillProtoType[lpSkill->GetMaxDurability()];

			if (lpSkillProtoType)
			{
				m_lpItemBase = new Item::CSkillItem;

				SetPos(lpSkill->m_lpItemBase->GetPos());
				m_ipPreIndex = m_lpItemBase->GetPos();

				((Item::CSkillItem *)m_lpItemBase)->SetSkillInfo(lpSkillProtoType);
				((Item::CSkillItem *)m_lpItemBase)->SetProtoTypeID(wSkillID);
				((Item::CSkillItem *)m_lpItemBase)->SetMaxNumOrDurability(lpSkill->GetMaxDurability());
				SetNowDurability(lpSkill->GetNowDurability());

				m_lCoolDownTime = lpSkill->m_lCoolDownTime;
				m_lCoolDownTimeCount = lpSkill->m_lCoolDownTimeCount;
				m_cCountCooldown = lpSkill->m_cCountCooldown;
				m_wGrade = lpSkill->m_wGrade;

				m_lpSprite = new CRYLSprite;
				m_lpSprite->Create(0, 0, GetItemInfo().m_SpriteData.m_nSpriteMinX,
					GetItemInfo().m_SpriteData.m_nSpriteMinY,
					GetItemInfo().m_SpriteData.m_nSpriteMaxX,
					GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL);

				m_lpDSprite = new CRYLSprite;
				m_lpDSprite->Create(0, 0, GetItemInfo().m_SpriteData.m_nSpriteMinX,
					GetItemInfo().m_SpriteData.m_nSpriteMinY,
					GetItemInfo().m_SpriteData.m_nSpriteMaxX,
					GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL);

				m_lpTooltip = new CGUITooltipItem;
				((CGUITooltipItem *)m_lpTooltip)->SetItem(this);

				return TRUE;
			}
		}
	}

	return FALSE;
}

bool CItemInstance::GetIsEnableUpgrade(void)
{
	const unsigned char cItemType = GetItemType();

	if (cItemType == Item::ItemType::CON_ARMOUR || cItemType == Item::ItemType::DEX_ARMOUR ||
		cItemType == Item::ItemType::ONEHANDED_AXE || cItemType == Item::ItemType::ONEHANDED_BLUNT || cItemType == Item::ItemType::BOW ||
		cItemType == Item::ItemType::CROSSBOW || cItemType == Item::ItemType::DAGGER ||
		cItemType == Item::ItemType::STAFF || cItemType == Item::ItemType::ONEHANDED_SWORD ||
		cItemType == Item::ItemType::TWOHANDED_AXE || cItemType == Item::ItemType::TWOHANDED_BLUNT ||
		cItemType == Item::ItemType::TWOHANDED_SWORD || cItemType == Item::ItemType::SHIELD ||

		cItemType == Item::ItemType::CON_BODY || cItemType == Item::ItemType::DEX_BODY || cItemType == Item::ItemType::COM_BLUNT ||
		cItemType == Item::ItemType::COM_SWORD || cItemType == Item::ItemType::OPP_HAMMER ||
		cItemType == Item::ItemType::OPP_AXE || cItemType == Item::ItemType::OPP_SLUSHER ||
		cItemType == Item::ItemType::OPP_SYTHE || cItemType == Item::ItemType::OPP_TALON ||
		cItemType == Item::ItemType::SKILL_A_GUARD || cItemType == Item::ItemType::SKILL_A_ATTACK ||
		cItemType == Item::ItemType::SKILL_A_GUN || cItemType == Item::ItemType::SKILL_A_KNIFE  || /// chnage by alex finis code
////		
		// edith 2008.01.14 edith 업그레이드 가능한 아이템 추가
		cItemType == Item::ItemType::CON_HELM || cItemType == Item::ItemType::DEX_HELM || 
		cItemType == Item::ItemType::CON_HEAD || cItemType == Item::ItemType::DEX_HEAD ||

		cItemType == Item::ItemType::CON_GLOVE || cItemType == Item::ItemType::DEX_GLOVE ||
		cItemType == Item::ItemType::CON_BOOTS || cItemType == Item::ItemType::DEX_BOOTS ||
		cItemType == Item::ItemType::CON_PELVIS || cItemType == Item::ItemType::DEX_PELVIS ||
		cItemType == Item::ItemType::CON_PROTECT_A || cItemType == Item::ItemType::DEX_PROTECT_A )
///
	{
		return true;
	}

	return false;
}

bool CItemInstance::GetIsEnableAddOption(void)
{
	// edith 2008.01.14 위에 업그레이드 추가로 인해서 주석처리됨.
	if (true == GetIsEnableUpgrade() 
		 || 
		GetItemType() == Item::ItemType::CON_HELM || 
		GetItemType() == Item::ItemType::DEX_HELM || 
		GetItemType() == Item::ItemType::CON_HEAD || 
		GetItemType() == Item::ItemType::DEX_HEAD 
		)
	{
		return true;
	}

	return false;
}

void CItemInstance::StartCoolDown(unsigned short wSkillGrade, unsigned char cCountCooldown, unsigned short wSkillID, unsigned int iCurCooldown )
{
	if (m_bSkill)
	{
		const Skill::ProtoType *lpSkillProtoType = NULL ;
		if ( wSkillID )
			lpSkillProtoType = g_SkillMgr.GetSkillProtoType( wSkillID ) ;
		else
			lpSkillProtoType = g_SkillMgr.GetSkillProtoType( GetProtoTypeID() ) ;			

		if (lpSkillProtoType)
		{
			lpSkillProtoType = &lpSkillProtoType[wSkillGrade];
			float fMulti = CRYLGameData::Instance()->m_csStatus.GetCoolDownRevisionRate() / 100.0f;
			m_lCoolDownTime = lpSkillProtoType->m_dwCoolDownTime * fMulti;
		}

		m_lCoolDownTimeCount = m_lCoolDownTime;
		m_cCountCooldown = cCountCooldown;
		m_wGrade = wSkillGrade;
	} 
	else
	{
		if(iCurCooldown == 0 || (unsigned int)(m_lCoolDownTime) < iCurCooldown)
			m_lCoolDownTimeCount = m_lCoolDownTime;
		else 
			m_lCoolDownTimeCount = iCurCooldown;

		m_cCountCooldown = cCountCooldown;
		m_wGrade = wSkillGrade;
	}
}

void CItemInstance::ClearCoolDown()
{
	m_lCoolDownTimeCount = 0;
	m_cCountCooldown = 0;
	m_wGrade = 0;
}

unsigned char CItemInstance::GetSkillType()
{
	if (m_bSkill)
	{
		const Skill::ProtoType *lpSkillProtoType = g_SkillMgr.GetSkillProtoType(m_lpItemBase->GetPrototypeID());

		if (lpSkillProtoType) return lpSkillProtoType->m_eSkillType;
	}

	return 0;
}

unsigned char CItemInstance::GetSkillLockCount(void)
{
	if (TRUE == m_bSkill)
	{
		return ((Item::CSkillItem *)m_lpItemBase)->GetMaxNumOrDurability();
	}
	else
	{
		if (Item::ItemType::SKILL_ITEM == m_lpItemBase->GetItemInfo().m_DetailData.m_cItemType)
		{
			return m_lpItemBase->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount;
		}
	}

	return 0;
}

unsigned short CItemInstance::GetSkillIDbyChantID(unsigned char cChant)
{
	if (m_bSkill)
	{
		const Skill::ProtoType *lpSkillProtoType = g_SkillMgr.GetSkillProtoType(m_lpItemBase->GetPrototypeID());
		if (lpSkillProtoType)
		{
			lpSkillProtoType = &lpSkillProtoType[GetMaxDurability()];
			if (lpSkillProtoType)
			{
				if (lpSkillProtoType->m_cEndCoolDown == cChant)
				{
					return m_lpItemBase->GetPrototypeID();
				}
			}
		}
	}

	return 0;
}

int CItemInstance::SetItemInfo(CItemInstance *lpItem, Item::ItemPos pItemPos)
{
	if (lpItem)
	{
		char Buffer[Item::MAX_ITEM_SIZE];
		unsigned int dwItemSize = Item::MAX_ITEM_SIZE;
		lpItem->m_lpItemBase->SerializeOut(Buffer, dwItemSize);
		if (dwItemSize)
		{
			m_lpItemBase = Item::CItemFactory::GetInstance().CreateItem(Buffer, dwItemSize);
		}
	} 
	else
	{
		m_lpItemBase = NULL;
	}

	if (m_lpItemBase)
	{
		m_lpItemBase->SetPos(pItemPos.m_cIndex);
		m_lpItemBase->SetItemPos(pItemPos.m_cPos);
		m_ipPreIndex = m_lpItemBase->GetPos();
		m_dwItemUID = m_lpItemBase->GetUID();

		m_lpSprite = new CRYLSprite;
		m_lpSprite->Create(0, 0, GetItemInfo().m_SpriteData.m_nSpriteMinX,
			GetItemInfo().m_SpriteData.m_nSpriteMinY,
			GetItemInfo().m_SpriteData.m_nSpriteMaxX,
			GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL);


		m_lpTooltip = new CGUITooltipItem;
		((CGUITooltipItem *)m_lpTooltip)->SetItem(this);

		if (GetItemType() == Item::ItemType::POTION)
		{
			m_lCoolDownTime = 14000;

			Item::CUseItem *lpItem = Item::CUseItem::DowncastToUseItem(m_lpItemBase);
			if (lpItem)
			{
				const Skill::ProtoType *lpSkillBase = g_SkillMgr.GetSkillProtoType(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID), *lpSkill;
				if (lpSkillBase)
				{
					lpSkill = &lpSkillBase[lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount];
					m_lCoolDownTime = lpSkill->m_dwCoolDownTime;
				}
			}

			m_lCoolDownTimeCount = CRYLGameData::Instance()->m_csStatus.GetPotionCoolTime(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID);
		}

		return 1;
	}

	return 0;
}

unsigned short CItemInstance::GetProtoTypeID( bool bAlterSkill )
{
	if (m_bSkill)
	{
		return GetSkillID();
	} 
	else
	{
		return m_lpItemBase->GetPrototypeID();
	}

	return 0;
}

unsigned short CItemInstance::GetSkillID( bool bAlterSkill )
{
	if (TRUE == m_bSkill)
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

		if ( NULL != pSelfCreature && true == pSelfCreature->GetStillCasting() )
		{
			return m_wPreProtoTypeID;
		} 
		else
		{
			if ( true == bAlterSkill )
			{
				const Skill::ProtoType* lpSkillBase = g_SkillMgr.GetSkillProtoType(m_lpItemBase->GetPrototypeID());

				// 얼터너티브 스킬
				if ( 0 != lpSkillBase->m_usChildSkill )			
				{
					m_wPreProtoTypeID = lpSkillBase->m_usChildSkill;
					return m_wPreProtoTypeID;
				}
			}
			else
			{
				m_wPreProtoTypeID = m_lpItemBase->GetPrototypeID();
				return m_wPreProtoTypeID;
			}
		}
	} 
	else
	{
		switch (m_lpItemBase->GetItemInfo().m_DetailData.m_cItemType)
		{
			case Item::ItemType::SKILL_BOOK:
			case Item::ItemType::SKILL_ITEM:
				return m_lpItemBase->GetItemInfo().m_UseItemInfo.m_usSkill_ID;
		}
	}

	return 0;
}
