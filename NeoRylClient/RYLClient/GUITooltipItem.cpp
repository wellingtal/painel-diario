// GUITooltipItem.cpp: implementation of the CGUITooltipItem class.
//
//////////////////////////////////////////////////////////////////////

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>

#include <Skill/SkillMgr.h>
#include <Creature/Character/CharacterClass.h>

#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLGameData.h"
#include "RYLNetWorkData.h"

#include "WinInput.h"
#include "GUITextEdit.h"
#include "GUITooltipItem.h"
#include "GMImageDib.h"
#include "ItemStorage.h"
#include "ItemInstance.h"

#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLStringGrid.h"
#include "RYLStringTable.h"
#include "RYLPicture.h"
#include "RYLSpriteList.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLCastle.h"
#include "RYLCastleManager.h"

#include "RYLTradeDlg.h"
#include "RYLNewTradeDlg.h"
#include "RYLCampShopDlg.h"
#include "RYLSkillDescDlg.h"
#include "RYLAbilityDescDlg.h"
#include "GMMemory.h"
#include "RYLSkillScriptMgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUITooltipItem::CGUITooltipItem()
{
	m_lpItem = NULL;
	m_pTooltipImg = NULL ;
    m_pTooltipList = NULL ;
}

CGUITooltipItem::~CGUITooltipItem()
{
    DestroyTooltip();
}

BOOL CGUITooltipItem::GetIsTooltip(void)
{
	if (m_lpItem)
		return TRUE;
	else
		return FALSE;
}

void CGUITooltipItem::DestroyTooltip(void)
{
	GM_DELETE( m_pTooltipImg ) ;
    GM_DELETE( m_pTooltipList ) ;
}

void CGUITooltipItem::Render(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	if (m_lpItem->m_bSkill)
	{
		RenderSkill(lpD3DDevice);
	} 
	else
	{
		switch (m_lpItem->GetItemType())
		{
			case Item::ItemType::SKILL_BOOK:
				RenderBook(lpD3DDevice);
				break;

			case Item::ItemType::EXP_BOOK:
			case Item::ItemType::CASH_BOOK:
			case Item::ItemType::CASH_ITEM:
			case Item::ItemType::ETC:
			case Item::ItemType::ORE:
			case Item::ItemType::POTION:
			case Item::ItemType::ARROW:
			case Item::ItemType::BOLT:
			case Item::ItemType::AMMO:
			case Item::ItemType::PORTAL:
			case Item::ItemType::EVENT_LOTTERY:
			case Item::ItemType::GEM:
			case Item::ItemType::GEM_SELL:
			case Item::ItemType::CAMP_KIT:
			case Item::ItemType::SHORT_RANGE_ARMS_KIT:
			case Item::ItemType::LONG_RANGE_ARMS_KIT:
			case Item::ItemType::AIRSHIP_KIT:
			case Item::ItemType::WORLDWEAPON_K_KIT:
			case Item::ItemType::WORLDWEAPON_M_KIT:
			case Item::ItemType::MINERAL_KIT:
			case Item::ItemType::FIRE_CRACKER:
			case Item::ItemType::QUEST_ITEM:
			case Item::ItemType::FIXED_PORTAL:
			case Item::ItemType::SKILL_ITEM:
				RenderEtc(lpD3DDevice);
				break;

			case Item::ItemType::RUNE:
				RenderRune(lpD3DDevice);
				break;

			default:
				RenderEquip(lpD3DDevice);
				break;
		}
	}
}
void CGUITooltipItem::RenderRune(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
	short			SizeX = ptMousePos->x + 7;
	short			SizeY = ptMousePos->y + 11;
	short			sLength, sLine;
	const unsigned short*			attribute = NULL;
	CRYLGameData*	pGame = CRYLGameData::Instance();


	GetRuneTooltipPos(SizeX, SizeY, sLength, sLine);
	
	const Item::ItemInfo* itemInfo = Item::CItemMgr::GetInstance().GetItemInfo(m_lpItem->GetProtoTypeID() );

	char strText[MAX_PATH];
	int Count = 0;
	unsigned long	dwLength, dwStart;
	RECT rcRect;

	unsigned long dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
	int nValue;
	
	if(itemInfo)	
	{
		attribute = itemInfo->m_EquipAttribute.m_usAttributeValue;
		pGame->RenderRect(lpD3DDevice, SizeX, SizeY, SizeX + sLength + 10, SizeY + sLine * 16 + 6, 0x00000000, 0x99);


		// -------------------------------------------------------------------------------
		// 아이템 명

		::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
		g_TextEdit.DrawText(m_lpItem->GetItemInfo().m_SpriteData.m_szName, &rcRect, DT_VCENTER | DT_CENTER, D3DCOLOR_RGBA(255, 255, 255, 255));
		++Count;

		/*
		// -------------------------------------------------------------------------------
		// 아이템 분류

		::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
		g_TextEdit.DrawText(m_lpItem->m_lpItemBase->GetItemTypeName(), &rcRect, DT_VCENTER | DT_CENTER, D3DCOLOR_RGBA(255, 255, 255, 255));
		++Count;	
		*/

		// -------------------------------------------------------------------------------
		// 가격

		if (pGame->m_lpInterface->GetIsTrade())
		{
			if (m_lpItem->m_bTradeItem)
			{
				if (m_lpItem->m_bMedalBuyItem)
				{
					sprintf(strText, CRYLStringTable::m_strString[862], g_TextEdit.MakePrintGold(m_lpItem->m_lpItemBase->GetBuyMedalPrice()));
				} 
				else
				{
					unsigned long dwBuyPrice = m_lpItem->m_lpItemBase->GetBuyPrice();

					NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
					if ( pNpcNode && pNpcNode->m_bBelongToCastle )
					{
						CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
						if ( lpCastle )
						{
							dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
						}
					}

					sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(dwBuyPrice));
				}
			} 
			else
			{
				sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(m_lpItem->m_lpItemBase->GetSellPrice()));
			}

			::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
			g_TextEdit.DrawText(strText, &rcRect, DT_VCENTER | DT_CENTER, D3DCOLOR_RGBA(255, 255, 255, 255));
			++Count;
		}

		// -------------------------------------------------------------------------------
		// 제한
		nValue = itemInfo->m_UseLimit.m_wLimitValue ;
		if( nValue > 0 )
		{
			switch( itemInfo->m_UseLimit.m_cLimitStatus )
			{
			case Skill::StatusLimit::STR:
				sprintf(strText, CRYLStringTable::m_strString[37], nValue + 20);
				if (pGame->m_csStatus.m_Info.STR - 20 < nValue) dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
				break;

			case Skill::StatusLimit::DEX:
				sprintf(strText, CRYLStringTable::m_strString[38], nValue + 20);
				if (pGame->m_csStatus.m_Info.DEX - 20 < nValue) dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
				break;

			case Skill::StatusLimit::CON:
				sprintf(strText, CRYLStringTable::m_strString[39], nValue + 20);
				if (pGame->m_csStatus.m_Info.CON - 20 < nValue) dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
				break;

			case Skill::StatusLimit::INT:
				sprintf(strText, CRYLStringTable::m_strString[40], nValue + 20);
				if (pGame->m_csStatus.m_Info.INT - 20 < nValue) dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
				break;

			case Skill::StatusLimit::WIS:
				sprintf(strText, CRYLStringTable::m_strString[41], nValue + 20);
				if (pGame->m_csStatus.m_Info.WIS - 20 < nValue) dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
				break;


			case Skill::StatusLimit::LEVEL:
				sprintf(strText, CRYLStringTable::m_strString[3557], nValue);
				if (pGame->m_csStatus.m_Info.Level < nValue) dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
				break;

			default:
				sprintf(strText,"");
				break;

			}
			::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
			g_TextEdit.DrawText(strText, &rcRect, DT_VCENTER | DT_CENTER, dwColor);
			dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
			++Count;
		}

		for(int nIndex = 0; nIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++nIndex)
		{
			if ( attribute[ Item::Attribute::Type(nIndex)] )
			{
				switch (nIndex)
				{
				case Item::Attribute::MIN_DAMAGE:	strcpy(strText, CRYLStringTable::m_strString[3555]);		break;
				case Item::Attribute::MAX_DAMAGE:	strcpy(strText, CRYLStringTable::m_strString[3556]);		break;
				case Item::Attribute::ARMOR:		strcpy(strText, CRYLStringTable::m_strString[48]);		break;
				case Item::Attribute::HIT_RATE:		strcpy(strText, CRYLStringTable::m_strString[50]);		break;
				case Item::Attribute::EVADE:		strcpy(strText, CRYLStringTable::m_strString[51]);		break;
				case Item::Attribute::CRITICAL:		strcpy(strText, CRYLStringTable::m_strString[49]);		break;
				case Item::Attribute::BLOCK:		strcpy(strText, CRYLStringTable::m_strString[53]);		break;
				case Item::Attribute::MAGIC_POWER:	strcpy(strText, CRYLStringTable::m_strString[59]);		break;
				case Item::Attribute::MAGIC_RESIST:	strcpy(strText, CRYLStringTable::m_strString[52]);		break;
				case Item::Attribute::LUCKCHANCE:	strcpy(strText, CRYLStringTable::m_strString[239]);		break;
				case Item::Attribute::COOLDOWN:		strcpy(strText, CRYLStringTable::m_strString[3549]);	break;
				case Item::Attribute::MAX_HP:		strcpy(strText, CRYLStringTable::m_strString[54]);		break;
				case Item::Attribute::HP_REGEN:		strcpy(strText, CRYLStringTable::m_strString[56]);		break;
				case Item::Attribute::MAX_MP:		strcpy(strText, CRYLStringTable::m_strString[55]);		break;
				case Item::Attribute::MP_REGEN:		strcpy(strText, CRYLStringTable::m_strString[57]);		break;
				case Item::Attribute::SPEED:		strcpy(strText, CRYLStringTable::m_strString[58]);		break;
				case Item::Attribute::SKILL_POINT:	strcpy(strText, CRYLStringTable::m_strString[3550]);	break;

				case Item::Attribute::FROST:		strcpy(strText, CRYLStringTable::m_strString[3551]);	break;
				case Item::Attribute::FIRE:			strcpy(strText, CRYLStringTable::m_strString[3552]);	break;
				case Item::Attribute::ELECTRO:		strcpy(strText, CRYLStringTable::m_strString[3553]);	break;
				case Item::Attribute::DARKNESS:		strcpy(strText, CRYLStringTable::m_strString[5200]);	break;
				// TEST ALEX
//				case Item::Attribute::ADD_STR:		strcpy(strText, CRYLStringTable::m_strString[5200]);	break;
				default:	continue;
				}

				char strTemp[100], a[10];
				strcpy(strTemp, strText);
				if (nIndex == Item::Attribute::MAX_HP || nIndex == Item::Attribute::MAX_MP)
				{
					sprintf(a, "%d", attribute[ Item::Attribute::Type(nIndex)] * 10);
				} 
				else
				{
					sprintf(a, "%d", attribute[ Item::Attribute::Type(nIndex)]);
				}

				strcat(strTemp, a);
				dwLength = g_TextEdit.GetStringLength(strTemp);
				dwStart = ((sLength + 10) / 2) - (dwLength / 2);

				dwLength = g_TextEdit.GetStringLength(strText);
				::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
				dwStart = rcRect.right;
				g_TextEdit.DrawText(strText, &rcRect, DT_VCENTER | DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));

				dwLength = g_TextEdit.GetStringLength(a);
				::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3, dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
				dwStart = rcRect.right;
				g_TextEdit.DrawText( a, &rcRect, DT_VCENTER | DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
				++Count;
			}


		}
	}


}

void CGUITooltipItem::RenderBook( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
	short SizeX = ptMousePos->x + 7;
	short SizeY = ptMousePos->y + 11;
	short sLength, sLine;
	
	CRYLGameData*	pGame = CRYLGameData::Instance();

	GetBookTooltipPos(SizeX, SizeY, sLength, sLine);
	
	RECT rt ;
	::SetRect( &rt, SizeX, SizeY, SizeX + sLength + 20, SizeY + sLine * 16 + 26 ) ;
	
    if ( m_pTooltipImg && m_pTooltipList )
	{
        // 이미지 높이나 너비가 바뀌었으면 다시 삭제후 다시 Create, 아니면 그냥 Render

        if (rt.right - rt.left == m_pTooltipList->GetWidth() &&
            rt.bottom - rt.top == m_pTooltipList->GetHeight())
        {
            RECT drawrt = rt;
            drawrt.left -= 10 ;
		    drawrt.top  -= 20 ;
		    
            m_pTooltipImg->DoDraw( NULL, drawrt, 0 ) ;
		    MakeSkillBookDesc( drawrt.right - drawrt.left ) ;
		    m_pTooltipList->DrawItems( drawrt.left - 10, drawrt.top + 3 ) ;
        }
        else
        {            
		    CreateTooltip(rt);
        }
	}
	else
	{        
		CreateTooltip(rt);
	}
}

void CGUITooltipItem::CreateTooltip(const RECT& rt) 
{
    DestroyTooltip();

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	unsigned short	wWidth    = rt.right - rt.left ;
	unsigned short  wHeight   = rt.bottom - rt.top ;
		
	m_pTooltipImg = new CRYLPicture( NULL ) ;
	m_pTooltipImg->SetLeft( rt.left ) ;
	m_pTooltipImg->SetTop( rt.top ) ;

	// Bounus Str Image
	CRYLSpriteList::Instance()->CreateGMImage( wWidth, wHeight, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	pImageDIB->MakeFrameRect( 0, 0, wWidth, wHeight, 0xffa88a60, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pTooltipImg->AttachImageTexture( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;

	m_pTooltipList = new CRYLStringGrid( NULL, FALSE ) ;
	m_pTooltipList->SetLeft( rt.left ) ;
	m_pTooltipList->SetTop( rt.top ) ;
	m_pTooltipList->SetWidth( wWidth ) ;
	m_pTooltipList->SetHeight( wHeight ) ;
	m_pTooltipList->SetColCount( 4 ) ;

	m_pTooltipList->SetItemMerginX( 5 ) ;
	m_pTooltipList->SetItemMerginY( 10 ) ;
	m_pTooltipList->SetItemHeight( 16 ) ;
}

void CGUITooltipItem::MakeSkillBookDesc( unsigned short wWidth ) 
{
	char strText[MAX_PATH];
	CRYLGameData*	pGame = CRYLGameData::Instance();

	Item::CUseItem *lpItem = Item::CUseItem::DowncastToUseItem(m_lpItem->m_lpItemBase);
	if (!lpItem) return;

	unsigned short wLockCount = lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount;
	const Skill::ProtoType* lpSkillBase = g_SkillMgr.GetSkillProtoType(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID), *lpSkill;
	if (NULL == lpSkillBase) { return; }

	lpSkill = &lpSkillBase[wLockCount];
	
	m_pTooltipList->ClearItem() ;

	// -------------------------------------------------------------------------------
	// 아이템 명

	sprintf(strText, CRYLStringTable::m_strString[95], lpSkill->m_SpriteInfo.m_szName, lpSkillBase->m_SpriteInfo.m_szName, wLockCount + 1);
	m_pTooltipList->AddItem( 0, strText, wWidth, 0xffffffff ) ;
	
	// -------------------------------------------------------------------------------
	// 아이템 분류

	m_pTooltipList->AddItem( 0, const_cast< char* >( lpItem->GetItemTypeName() ), wWidth, 0xffffffff ) ;
	
	// -------------------------------------------------------------------------------
	// 가격

	if (TRUE == pGame->m_lpInterface->GetIsTrade())
	{
		DWORD dwColor = 0xFFFFFFFF;
		if (TRUE == m_lpItem->m_bTradeItem)
		{
			if (TRUE == m_lpItem->m_bMedalBuyItem)
			{
				sprintf(strText, CRYLStringTable::m_strString[862], g_TextEdit.MakePrintGold(lpItem->GetBuyMedalPrice()));
			} 
			else
			{
				unsigned long dwBuyPrice = lpItem->GetBuyPrice();

				NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
				if ( pNpcNode && pNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
					if ( lpCastle )
					{
						dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
					}
				}

				sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(dwBuyPrice));

				if ( m_lpItem->m_bStoreItem )
				{
					// edith 2009.05.25 개인상점 아이템 툴팁 가격 색 설정
					// 스토어에서 파는 아이템
					if( dwBuyPrice < 100000)
						dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
					else if( dwBuyPrice < 1000000)
						dwColor = D3DCOLOR_RGBA(255, 255, 128, 255);
					else if( dwBuyPrice < 10000000 )
						dwColor = D3DCOLOR_RGBA(255, 255, 0, 255);
					else if( dwBuyPrice < 100000000 )
						dwColor = D3DCOLOR_RGBA(255, 192, 0, 255);
					else
						dwColor = D3DCOLOR_RGBA(255, 128, 0, 255);
				}
			}
		} 
		else
		{
			unsigned long dwSellPrice  = lpItem->GetSellPrice();
			sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold( dwSellPrice ));

			if ( m_lpItem->m_bStoreItem )
			{
				// edith 2009.05.25 개인상점 아이템 툴팁 가격 색 설정
				// 스토어에서 파는 아이템
				if( dwSellPrice < 100000)
					dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
				else if( dwSellPrice < 1000000)
					dwColor = D3DCOLOR_RGBA(255, 255, 128, 255);
				else if( dwSellPrice < 10000000 )
					dwColor = D3DCOLOR_RGBA(255, 255, 0, 255);
				else if( dwSellPrice < 100000000 )
					dwColor = D3DCOLOR_RGBA(255, 192, 0, 255);
				else
					dwColor = D3DCOLOR_RGBA(255, 128, 0, 255);
			}
		}

		m_pTooltipList->AddItem( 0, strText, wWidth, dwColor ) ;		
	}

	// -------------------------------------------------------------------------------
	// 스테이터스 제한
	if(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID >= 0x8000)
	{
		SKILLSLOT *lpSkillSlot = pGame->m_csStatus.GetSkillSlot(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID);
		if (lpSkillSlot && wLockCount < lpSkillSlot->SKILLINFO.cLockCount)
		{
			m_pTooltipList->AddItem( 0, CRYLStringTable::m_strString[93], wWidth, 0xff0000ff ) ;		
		} 
		else if (lpSkillSlot && wLockCount == lpSkillSlot->SKILLINFO.cLockCount && lpSkillSlot->SKILLINFO.cSkillLevel == 6)
		{
			m_pTooltipList->AddItem( 0, CRYLStringTable::m_strString[94], wWidth, 0xff0000ff ) ;		
		} 
		else
		{
			for (int i = 0; i < Skill::ProtoType::MAX_LIMIT_NUM; i++)
			{
				if (lpSkill->m_StatusLimitType[i])
				{
					unsigned long dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
					int nValue = 0;

					if (lpSkillSlot)
					{
						if (wLockCount == lpSkillSlot->SKILLINFO.cLockCount)
						{
							nValue = (wLockCount * CSkillMgr::MAX_SKILL_LEVEL + lpSkillSlot->SKILLINFO.cSkillLevel) * 
								lpSkill->m_StatusLimitValue[i];
						} 
						else
						{
							nValue = (wLockCount * CSkillMgr::MAX_SKILL_LEVEL) * lpSkill->m_StatusLimitValue[i];
						}
					} 
					else
					{
						nValue = (wLockCount * CSkillMgr::MAX_SKILL_LEVEL) * lpSkill->m_StatusLimitValue[i];
					}

					switch (lpSkill->m_StatusLimitType[i])
					{
						case Skill::StatusLimit::STR:
							sprintf(strText, CRYLStringTable::m_strString[37], nValue + 20);
							if (pGame->m_csStatus.m_Info.STR - 20 < nValue) 
								dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
							break;

						case Skill::StatusLimit::DEX:
							sprintf(strText, CRYLStringTable::m_strString[38], nValue + 20);
							if (pGame->m_csStatus.m_Info.DEX - 20 < nValue) 
								dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
							break;

						case Skill::StatusLimit::CON:
							sprintf(strText, CRYLStringTable::m_strString[39], nValue + 20);
							if (pGame->m_csStatus.m_Info.CON - 20 < nValue) 
								dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
							break;

						case Skill::StatusLimit::INT:
							sprintf(strText, CRYLStringTable::m_strString[40], nValue + 20);
							if (pGame->m_csStatus.m_Info.INT - 20 < nValue) 
								dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
							break;

						case Skill::StatusLimit::WIS:
							sprintf(strText, CRYLStringTable::m_strString[41], nValue + 20);
							if (pGame->m_csStatus.m_Info.WIS - 20 < nValue) 
								dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
							break;

						case Skill::StatusLimit::LEVEL:
							sprintf(strText, CRYLStringTable::m_strString[3557], lpSkill->m_StatusLimitValue[i]);
							if (pGame->m_csStatus.m_Info.Level < lpSkill->m_StatusLimitValue[i]) 
								dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
							break;
					}

					m_pTooltipList->AddItem( 0, strText, wWidth, dwColor ) ;		
				}
			}
		}

		unsigned char cClass[6]		= { CClass::Fighter, CClass::Rogue, CClass::Mage, CClass::Acolyte,  CClass::Combatant,  CClass::Officiator };
		unsigned char cSubClass[14]	= { CClass::Defender, CClass::Warrior, CClass::Assassin, CClass::Archer,  CClass::Sorcerer,  CClass::Enchanter,
										CClass::Priest, CClass::Cleric, CClass::Templar, CClass::Attacker, CClass::Gunner, CClass::RuneOff, 
										CClass::LifeOff, CClass::ShadowOff };

		unsigned long dwClassLimit = lpItem->GetItemInfo().m_UseLimit.m_dwClassLimit;
		unsigned long dwCnt	= 0;
			
		strcpy(strText, CRYLStringTable::m_strString[98]);
		
		// 휴먼 체크.
		for(unsigned char cIndex = 0; cIndex < 4; cIndex++)
		{
			if(ClassCheck(dwClassLimit, cClass[cIndex]))
			{		
				if(dwCnt)
				{
					strcat(strText, ", ");
				}

				strcat(strText, CRYLStringTable::m_strString[451+cIndex]);
				dwCnt++;
			}
			else
			{
				for(unsigned char cSubIndex = 0; cSubIndex < 2; cSubIndex++)
				{
					if(ClassCheck(dwClassLimit, cSubClass[cIndex*2+cSubIndex]))
					{
						if(dwCnt)
						{
							strcat(strText, ", ");
						}

						strcat(strText, CRYLStringTable::m_strString[455+cIndex*2+cSubIndex]);
						dwCnt++;
					}
				}
			}
		}

		// 아칸 체크.
		for(unsigned char cIndex = 4; cIndex < 6; cIndex++)
		{
			if(ClassCheck(dwClassLimit, cClass[cIndex]))
			{		
				if(dwCnt)
				{
					strcat(strText, ", ");
				}

				strcat(strText, CRYLStringTable::m_strString[467+(cIndex-4)]);
				dwCnt++;
			}
			else
			{
				for(unsigned char cSubIndex = 0; cSubIndex < 3; cSubIndex++)
				{
					if(ClassCheck(dwClassLimit, cSubClass[8+(cIndex-4)*3+cSubIndex]))
					{
						if(dwCnt)
						{
							strcat(strText, ", ");
						}

						strcat(strText, CRYLStringTable::m_strString[469+(cIndex-4)*3+cSubIndex]);
						dwCnt++;
					}
				}
			}
		}

		if(dwCnt)
		{
			m_pTooltipList->AddItem( 0, strText, wWidth, 0xffffffff ) ;		
		}

		switch (lpSkill->m_eSkillType)
		{
			case Skill::Type::PASSIVE:				
				strcpy(strText, CRYLStringTable::m_strString[75]);
				break;

			case Skill::Type::INSTANCE:
				strcpy(strText, CRYLStringTable::m_strString[76]);
				break;

			case Skill::Type::CAST:
				strcpy(strText, CRYLStringTable::m_strString[77]);
				break;

			case Skill::Type::CHANT:
				strcpy(strText, CRYLStringTable::m_strString[78]);
				break;

			case Skill::Type::GATHER:
				strcpy(strText, CRYLStringTable::m_strString[76]);
				break;
		}
		m_pTooltipList->AddItem( 0, strText, wWidth, 0xffffffff ) ;		

		switch (lpSkill->m_eTargetType)
		{
			case Skill::Target::DEAD_ENEMY:				strcpy(strText, CRYLStringTable::m_strString[81]);		break;
			case Skill::Target::DEAD_FRIEND:			strcpy(strText, CRYLStringTable::m_strString[82]);		break;
			case Skill::Target::ENEMY:					strcpy(strText, CRYLStringTable::m_strString[83]);		break;
			case Skill::Target::ENEMY_OBJECT:			strcpy(strText, CRYLStringTable::m_strString[84]);		break;
			case Skill::Target::FRIEND:					strcpy(strText, CRYLStringTable::m_strString[85]);		break;
			case Skill::Target::FRIEND_EXCEPT_SELF:		strcpy(strText, CRYLStringTable::m_strString[86]);		break;
			case Skill::Target::FRIEND_OBJECT:			strcpy(strText, CRYLStringTable::m_strString[87]);		break;
			case Skill::Target::LINE_ENEMY:				strcpy(strText, CRYLStringTable::m_strString[88]);		break;
			case Skill::Target::MELEE:					strcpy(strText, CRYLStringTable::m_strString[89]);		break;
			case Skill::Target::PARTY:					strcpy(strText, CRYLStringTable::m_strString[90]);		break;
			case Skill::Target::SUMMON:					strcpy(strText, CRYLStringTable::m_strString[863]);		break;

			default:									strcpy(strText, "");									break;
		}
		if (strcmp(strText, ""))
		{
			m_pTooltipList->AddItem( 0, strText, wWidth, 0xffffffff ) ;		
		}

		if (lpSkill->m_fMaxRange)
		{
			sprintf(strText, CRYLStringTable::m_strString[91], lpSkill->m_fMaxRange);
			m_pTooltipList->AddItem( 0, strText, wWidth, 0xffffffff ) ;		
		}

		if (lpSkill->m_fEffectExtent)
		{
			sprintf(strText, CRYLStringTable::m_strString[92], lpSkill->m_fEffectExtent);
		} 
		else
		{
			strcpy(strText, CRYLStringTable::m_strString[99]);
		}

		m_pTooltipList->AddItem( 0, strText, wWidth, 0xffffffff ) ;
	}
	else 
	{
		sprintf(strText, CRYLStringTable::m_strString[4325], (int)(lpSkill->m_fMinRange));
		m_pTooltipList->AddItem( 0, strText, wWidth, 0xffffffff ) ;		
	}

	m_pTooltipList->AddItem( 0, " ", wWidth, 0xffffffff ) ;	

	if (strcmp(lpItem->GetItemInfo().GetItemDescribe(), ""))
	{
//		if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::JAPAN )
		if( g_ClientMain.m_iLanguage == 2)
		{
			WCHAR*	strWideString = NULL;
			WCHAR	seps[] = L"\\" ;
			WCHAR*	token;			
			char	strTemp[1024] = {0, };
			char	strText[MAX_PATH] = {0, };

			int nLen = MultiByteToWideChar( CP_ACP, 0, lpItem->GetItemInfo().GetItemDescribe(), lstrlen(lpItem->GetItemInfo().GetItemDescribe()), NULL, NULL);
			strWideString = SysAllocStringLen(NULL, nLen);
			MultiByteToWideChar( CP_ACP, 0, lpItem->GetItemInfo().GetItemDescribe(), strlen(lpItem->GetItemInfo().GetItemDescribe()), strWideString, nLen ) ;

			token = wcstok( strWideString, seps ) ;			

			while( token != NULL )
			{
				nLen = wcslen( token ) ;

				WideCharToMultiByte(CP_ACP, 0, token, nLen+1, strTemp, 1024, NULL, NULL);

//				memcpy(strText, strTemp, nLen*2+1);				

				m_pTooltipList->AddItem( 0, strTemp, wWidth, 0xffffffff ) ;		

				token = wcstok( NULL, seps ) ;
			}

			SysFreeString(strWideString);
		}
		else
		{
			char strText[MAX_PATH], *token, spes[] = "\\";
			strcpy(strText, lpItem->GetItemInfo().GetItemDescribe());
			token = strtok(strText, spes);
			while(token != NULL)
			{
				m_pTooltipList->AddItem( 0, token, wWidth, 0xffffffff ) ;		
				token = strtok(NULL, spes);
			}
		}		
	}
}

unsigned long CGUITooltipItem::GetGradeColor(const Item::Attribute::Type eType)
{
	Item::EquipType::Grade eGrade = Item::EquipType::MAX_GRADE;

	Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(m_lpItem->m_lpItemBase);
	if (NULL != lpEquip)
	{
		unsigned char cType = 0;

		if (Item::Attribute::MAX_ATTRIBUTE_NUM == eType)
		{			
//			// 아이템의 그레이드 표기
//			for (unsigned char cIndex = 0; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
//			{
//				if (true == lpEquip->CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
//				{
//					return D3DCOLOR_RGBA(255, 255, 0, 255);
//				}
//			}

			eGrade = lpEquip->GetItemGrade().m_eItemGrade;
		}
		else
		{
//			// 속성별 그레이드 표기
//			if (true == lpEquip->CheckAttributeLimit(eType, cType))
//			{
//				return D3DCOLOR_RGBA(255, 255, 0, 255);
//			}
//
			eGrade = lpEquip->GetItemGrade().m_aryAttributeGrade[eType];
		}
	}

	return GetGradeColor(eGrade);
}

unsigned long CGUITooltipItem::GetGradeColor(const Item::EquipType::Grade eGrade)
{
	switch (eGrade)
	{
		/*
		case Item::EquipType::S_GRADE:		return D3DCOLOR_RGBA(255, 255, 0, 255);
		case Item::EquipType::AAA_GRADE:	return D3DCOLOR_RGBA(255, 204, 0, 255);
		case Item::EquipType::AA_GRADE:		return D3DCOLOR_RGBA(255, 153, 0, 255);
		case Item::EquipType::A_GRADE:		return D3DCOLOR_RGBA(255, 255, 153, 255);
		case Item::EquipType::B_GRADE:		return D3DCOLOR_RGBA(0, 255, 255, 255);
		case Item::EquipType::C_GRADE:		return D3DCOLOR_RGBA(0, 255, 0, 255);
		case Item::EquipType::D_GRADE:		return D3DCOLOR_RGBA(204, 153, 255, 255);
		case Item::EquipType::F_GRADE:		return D3DCOLOR_RGBA(255, 255, 255, 255);
		case Item::EquipType::X_GRADE:		return D3DCOLOR_RGBA(255, 255, 255, 255);
		*/
		case Item::EquipType::S_GRADE:		return D3DCOLOR_RGBA(255, 255, 0, 255);
		case Item::EquipType::AAA_GRADE:	return D3DCOLOR_RGBA(255, 255, 0, 255);
		case Item::EquipType::AA_GRADE:		return D3DCOLOR_RGBA(255, 204, 0, 255);
		case Item::EquipType::A_GRADE:		return D3DCOLOR_RGBA(255, 153, 0, 255);
		case Item::EquipType::B_GRADE:		return D3DCOLOR_RGBA(255, 255, 153, 255);
		case Item::EquipType::C_GRADE:		return D3DCOLOR_RGBA(0, 255, 255, 255);
		case Item::EquipType::D_GRADE:		return D3DCOLOR_RGBA(0, 255, 0, 255);
		case Item::EquipType::F_GRADE:		return D3DCOLOR_RGBA(204, 153, 255, 255);
		case Item::EquipType::X_GRADE:		return D3DCOLOR_RGBA(255, 255, 255, 255);



	}

	return D3DCOLOR_RGBA(255, 255, 255, 255);
}

void CGUITooltipItem::RenderEquip( LPDIRECT3DDEVICE8 lpD3DDevice )
{
	POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
	short			SizeX = ptMousePos->x + 7;
	short			SizeY = ptMousePos->y + 11;
	short			sLength, sLine, sLineSpace;
	
	GetEquipTooltipPos(SizeX, SizeY, sLength, sLine);

	CRYLGameScene*	    pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLNewTradeDlg*	pTradeDlg	= ( CRYLNewTradeDlg* )pScene->GetNewTradeDlg();
	BOOL				bBlackMarket = pTradeDlg->IsBlackMarket();
	BOOL				bMedalMarket = pTradeDlg->IsMedalMarket();
	RECT				rt ;

	const Item::ItemInfo* lpBase = Item::CItemMgr::GetInstance().GetItemInfo(m_lpItem->GetProtoTypeID());
	if (NULL == lpBase) 
		return ; 
	
	Item::CEquipment *lpItem = Item::CEquipment::DowncastToEquipment(m_lpItem->m_lpItemBase);
	if (NULL == lpItem) 
		return ; 
	unsigned long runeID = 0;
	runeID = lpItem->GetRuneSocket(0) ;

	unsigned long dwSocketSize = 0;
	unsigned long dwRuneSocketSize = 0;
	if (0 < lpItem->GetMaxSocketNum() && lpItem->GetMaxSocketNum() <= 4)
	{
		dwSocketSize = 32;
	}
	else if (4 < lpItem->GetMaxSocketNum() && lpItem->GetMaxSocketNum() <= 12)
	{
		dwSocketSize = 96;
	}

	if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::RUNE))
	{
		if( lpItem->GetMaxRuneSocket() > 0 )
		{
			dwRuneSocketSize  = 32;
		}
	}

	sLineSpace = 16;

	if (TRUE == m_lpItem->m_bTradeItem && (bBlackMarket || bMedalMarket))
	{
		::SetRect( &rt, SizeX, SizeY, SizeX + sLength + 10, SizeY + sLine * sLineSpace + 30 + dwRuneSocketSize ) ;
	} 
	else
	{
		::SetRect( &rt, SizeX, SizeY, SizeX + sLength + 10, SizeY + sLine * sLineSpace + 30 + dwSocketSize + dwRuneSocketSize ) ;
	}
	
	if ( m_pTooltipImg && m_pTooltipList )
	{
        // 이미지 높이나 너비가 바뀌었으면 다시 삭제후 다시 Create, 아니면 그냥 Render

        if (rt.right - rt.left == m_pTooltipList->GetWidth() &&
            rt.bottom - rt.top == m_pTooltipList->GetHeight())
        {
            RECT drawrt = rt;
            drawrt.left -= 10 ;
		    drawrt.top  -= 20 ;
		    
            m_pTooltipImg->DoDraw( NULL, drawrt, 0 ) ;
		    MakeEquipDesc( drawrt.right - drawrt.left, lpD3DDevice ) ;
		    m_pTooltipList->DrawItems( drawrt.left - 10, drawrt.top + 3 ) ;
        }
        else
        {            
		    CreateTooltip(rt);
        }
	}
	else
	{        
		CreateTooltip(rt);
	}    
}

void CGUITooltipItem::MakeEquipDesc( unsigned short wWidth, LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	POINT*			ptMousePos	= g_DeviceInput.GetMouseLocal();
	short			SizeX		= ptMousePos->x + 7;
	short			SizeY		= ptMousePos->y + 11;

	short			sLength, sLine;
	unsigned long	dwLength, dwStart;
	char			strGrade[5];
	short attribute[ Item::Attribute::MAX_ATTRIBUTE_NUM ];
	CRYLGameData*	pGame = CRYLGameData::Instance();

	GetEquipTooltipPos(SizeX, SizeY, sLength, sLine);

	CRYLGameScene*	    pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLNewTradeDlg*	pTradeDlg	= ( CRYLNewTradeDlg* )pScene->GetNewTradeDlg();

	BOOL bBlackMarket = pTradeDlg->IsBlackMarket();
	BOOL bMedalMarket = pTradeDlg->IsMedalMarket();

	const Item::ItemInfo* lpBase = Item::CItemMgr::GetInstance().GetItemInfo(m_lpItem->GetProtoTypeID());
	if (NULL == lpBase) { return; }
	Item::CEquipment *lpItem = Item::CEquipment::DowncastToEquipment(m_lpItem->m_lpItemBase);
	if (NULL == lpItem) { return; }

	unsigned long runeID = 0;
	runeID = lpItem->GetRuneSocket(0) ;

	lpItem->GetSubRuneAttribute( attribute );


	unsigned long dwSocketSize = 0;
	unsigned long dwRuneSocketSize = 0;
	if (0 < lpItem->GetMaxSocketNum() && lpItem->GetMaxSocketNum() <= 4)
	{
		dwSocketSize = 32;
	}
	else if (4 < lpItem->GetMaxSocketNum() && lpItem->GetMaxSocketNum() <= 12)
	{
		dwSocketSize = 96;
	}

	if( lpItem->GetMaxRuneSocket() > 0 )
	{
		dwRuneSocketSize  = 32;
	}
	
	m_pTooltipList->ClearItem() ;

	// -------------------------------------------------------------------------------
	// 아이템 명

	bool bIsNonGradeItem = false;
	Item::Grade::GradeInfo gradeInfo = lpItem->GetItemGrade();
	unsigned long dwItemColor = GetGradeColor(Item::Attribute::MAX_ATTRIBUTE_NUM);
	char strRuneName[ 100 ];
	unsigned long dwRuneColor;
	unsigned long dwRuneLength;

	strcpy( strRuneName, "" );
	if( runeID )
	{
		const Item::ItemInfo *lpPT;
		if( lpItem->GetRuneSocket(1) )
		{
			int nRune1, nRune2;
			// 이중 장착룬
			lpPT = g_ItemMgr.GetItemInfo( runeID );
			if( lpPT )
			{
				nRune1 = Item::EquipType::Grade(lpPT->m_DetailData.m_cGrade);
			}
			lpPT = g_ItemMgr.GetItemInfo( lpItem->GetRuneSocket(1) );
			if( lpPT )
			{
				nRune2 = Item::EquipType::Grade(lpPT->m_DetailData.m_cGrade);
			}
			dwRuneColor = GetGradeColor( static_cast<Item::EquipType::Grade>(min( nRune1, nRune2 )) ) ;
			strcpy( strRuneName, CRYLStringTable::m_strString[3562]);
		}
		else
		{
			lpPT = g_ItemMgr.GetItemInfo( runeID );
			if( lpPT )
			{
				dwRuneColor = GetGradeColor( Item::EquipType::Grade(lpPT->m_DetailData.m_cGrade) );
				strcpy( strRuneName, lpPT->m_SpriteData.m_szName );
			}
		}
	}


	switch ( gradeInfo.m_eItemGrade )
	{
		/*
		case Item::EquipType::S_GRADE:		strcpy( strGrade, " S" );		break;
		case Item::EquipType::AAA_GRADE:	strcpy( strGrade, " AAA" );		break;
		case Item::EquipType::AA_GRADE:		strcpy( strGrade, " AA" );		break;
		case Item::EquipType::A_GRADE:		strcpy( strGrade, " A" );		break;
		case Item::EquipType::B_GRADE:		strcpy( strGrade, " B" );		break;
		case Item::EquipType::C_GRADE:		strcpy( strGrade, " C" );		break;
		case Item::EquipType::D_GRADE:		strcpy( strGrade, " D" );		break;
		case Item::EquipType::F_GRADE:		strcpy( strGrade, " F" );		break;
		*/
		case Item::EquipType::S_GRADE:		strcpy( strGrade, " S" );		break;
		case Item::EquipType::AAA_GRADE:	strcpy( strGrade, " S" );		break;
		case Item::EquipType::AA_GRADE:		strcpy( strGrade, " AAA" );		break;
		case Item::EquipType::A_GRADE:		strcpy( strGrade, " AA" );		break;
		case Item::EquipType::B_GRADE:		strcpy( strGrade, " A" );		break;
		case Item::EquipType::C_GRADE:		strcpy( strGrade, " B" );		break;
		case Item::EquipType::D_GRADE:		strcpy( strGrade, " C" );		break;
		case Item::EquipType::F_GRADE:		strcpy( strGrade, " D" );		break;
		case Item::EquipType::X_GRADE:
		{
			strcpy( strGrade, "");
			gradeInfo.m_cPlus = 0;
			dwItemColor = D3DCOLOR_RGBA(255, 255, 255, 255);
			bIsNonGradeItem = true;
			break;
		}
	}

	// 그레이드 표기를 하지 않는 종류의 아이템 ( 흰색 표기 )
	switch ( m_lpItem->GetItemType() )
	{
		// edith 2008.01.14 아이템그레이드를 표시하지 않는 아이템
		case Item::ItemType::SHIRT :
		case Item::ItemType::TUNIC :
		case Item::ItemType::ARROW :
		case Item::ItemType::BOLT :
		case Item::ItemType::AMMO :
		{
			strcpy( strGrade,"" );
			gradeInfo.m_cPlus = 0;
			dwItemColor = D3DCOLOR_RGBA(255, 255, 255, 255);
			bIsNonGradeItem = true;
			break;
		}
	}

	RECT rcRect;
	char strText[100];
	int i, Count = 0;
	::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);

	if (TRUE == m_lpItem->m_bTradeItem && (bBlackMarket || bMedalMarket))
	{
		dwItemColor = D3DCOLOR_RGBA(255, 255, 255, 255);
		m_pTooltipList->AddItem( 0, const_cast<char*>(lpItem->GetItemInfo().m_SpriteData.m_szName), wWidth, dwItemColor ) ;
		++Count ;
	} 
	else
	{
		char strItemName[MAX_PATH];

		if(lpItem->GetSeasonRecord() == 3)
			sprintf(strItemName, "%s %s", CRYLStringTable::m_strString[4301], lpItem->GetItemInfo().m_SpriteData.m_szName );
		else if(lpItem->GetSeasonRecord() == 2)
			sprintf(strItemName, "%s %s", CRYLStringTable::m_strString[238], lpItem->GetItemInfo().m_SpriteData.m_szName );
		else
			strcpy( strItemName, lpItem->GetItemInfo().m_SpriteData.m_szName );

		strcat( strItemName, strGrade );

		// edith 2008.01.16 +++++ 로 붙는걸. (+5) 로 변경.
//		if(gradeInfo.m_cPlus > 0)
//		{
//			char strTemp[32];
		//	sprintf(strTemp, "(+%d)", gradeInfo.m_cPlus);
		//	strcat( strItemName, strTemp);
//		}
//----
		for ( i = 0; i < gradeInfo.m_cPlus; i++ )
		{
			strcat( strItemName, "+");
		}
//-----

		if( strcmp( strRuneName, "" ) )
		{
			strcat(strRuneName, " ");
			sprintf(strText, "%s%s", strRuneName, strItemName);
			dwLength = g_TextEdit.GetStringLength(strText);
			dwStart = ( ( wWidth - dwLength ) / 2 ) - 15 ;
			dwRuneLength = g_TextEdit.GetStringLength(strRuneName);
			::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwRuneLength, SizeY + Count * 16 + 16 + 3);
			m_pTooltipList->AddItem( 0, strRuneName, dwRuneLength, dwRuneColor, rcRect.left ) ;
			//dwStart = rcRect.right ;
			dwLength = g_TextEdit.GetStringLength(strItemName);
			//::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
			m_pTooltipList->AddItem( 1, strItemName, dwLength, dwItemColor, rcRect.left + dwRuneLength) ;
		}
		else
		{
			m_pTooltipList->AddItem( 0, strItemName, wWidth, dwItemColor ) ;
		}
		++Count ;
	}

	// -------------------------------------------------------------------------------
	// 보상 판매 여부

	if (0 == lpItem->GetSeasonRecord() && m_lpItem->GetIsEnableAddOption() && (FALSE == bBlackMarket || FALSE == bMedalMarket))
	{
		::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 0, CRYLStringTable::m_strString[2777], wWidth, 0xffffffff ) ;
		++Count ;
	}

	// -------------------------------------------------------------------------------
	// 아이템 분류

	::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
	m_pTooltipList->AddItem( 0, const_cast<char*>(lpItem->GetItemTypeName()), wWidth, 0xffffffff ) ;
	++Count ;

	// -------------------------------------------------------------------------------
	// 가격

	if (TRUE == pGame->m_lpInterface->GetIsTrade())
	{
		DWORD dwColor = 0xFFFFFFFF;
		if (TRUE == m_lpItem->m_bTradeItem)
		{
			if (TRUE == bBlackMarket)
			{
				unsigned long dwBuyBlackPrice = lpItem->GetBuyBlackPrice();

				NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
				if ( pNpcNode && pNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
					if ( lpCastle )
					{
						dwBuyBlackPrice += static_cast<unsigned long>( dwBuyBlackPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
					}
				}

				sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(dwBuyBlackPrice));
			} 
			else if (m_lpItem->m_bMedalBuyItem)
			{
				sprintf(strText, CRYLStringTable::m_strString[862], g_TextEdit.MakePrintGold(lpItem->GetBuyMedalPrice()));
			} 
			else
			{
				unsigned long dwBuyPrice = lpItem->GetBuyPrice();
				NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
				if ( pNpcNode && pNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
					if ( lpCastle )
					{
						dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
					}
				}

				sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(dwBuyPrice));

				if ( m_lpItem->m_bStoreItem )
				{
					// edith 2009.05.25 개인상점 아이템 툴팁 가격 색 설정
					// 스토어에서 파는 아이템
					if( dwBuyPrice < 100000)
						dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
					else if( dwBuyPrice < 1000000)
						dwColor = D3DCOLOR_RGBA(255, 255, 128, 255);
					else if( dwBuyPrice < 10000000 )
						dwColor = D3DCOLOR_RGBA(255, 255, 0, 255);
					else if( dwBuyPrice < 100000000 )
						dwColor = D3DCOLOR_RGBA(255, 192, 0, 255);
					else
						dwColor = D3DCOLOR_RGBA(255, 128, 0, 255);
				}
			}
		} 
		else
		{
			unsigned long dwSellPrice = lpItem->GetSellPrice();
			sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold( dwSellPrice ));

			if ( m_lpItem->m_bStoreItem )
			{
				// edith 2009.05.25 개인상점 아이템 툴팁 가격 색 설정
				// 스토어에서 파는 아이템
				if( dwSellPrice < 100000)
					dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
				else if( dwSellPrice < 1000000)
					dwColor = D3DCOLOR_RGBA(255, 255, 128, 255);
				else if( dwSellPrice < 10000000 )
					dwColor = D3DCOLOR_RGBA(255, 255, 0, 255);
				else if( dwSellPrice < 100000000 )
					dwColor = D3DCOLOR_RGBA(255, 192, 0, 255);
				else
					dwColor = D3DCOLOR_RGBA(255, 128, 0, 255);
			}
		}

		::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 0, strText, wWidth, dwColor ) ;
		++Count ;
	}

	bool bLimit = false;

	// -------------------------------------------------------------------------------
	// 클래스 제한

	enum ClassLimitConst
	{
		MAX_LINE_NUM	= 3,	// 클래스 제한 표기 최대 라인수
		MAX_INDEX_NUM	= 3		// 한 줄당 클래스 제한 표기 최대 갯수
	};

	unsigned long dwClass[MAX_LINE_NUM * MAX_INDEX_NUM] = { 0, };
	unsigned long count = 0;

	// 종족을 판별하는 if가 추가되야한다.
	if (lpItem->GetItemInfo().m_UseLimit.m_dwClassLimit != 0x00FF0FFF && 
		lpItem->GetItemInfo().m_UseLimit.m_dwClassLimit != 0x00000FFF && 
		lpItem->GetItemInfo().m_UseLimit.m_dwClassLimit != 0x00FF0000)
	{
		bool bCanClass = false;
		for (int i = 1; i < CClass::MAX_CLASS; i++)
		{
			if (lpItem->GetItemInfo().m_UseLimit.m_dwClassLimit & (0x00000001 << (i - 1)))
			{
				dwClass[count] = i;
				count++;

				if (pGame->m_csStatus.m_Info.Class == i) 
				{
					bCanClass = true;
				}
			}
		}

		unsigned long dwColor = 
			(true == bCanClass) ? D3DCOLOR_RGBA(248, 200, 112, 255) : D3DCOLOR_RGBA(255, 0, 0, 255);

		if(!bCanClass)
		{
			bLimit = true;
		}

		for (int nLine = 0; nLine < MAX_LINE_NUM; ++nLine)
		{
			int nFirstIndex = nLine * MAX_INDEX_NUM;
			if (dwClass[nFirstIndex])
			{
				sprintf(strText, "%s", pGame->GetClassName(dwClass[nFirstIndex]));
			}

			for (int nNextIndex = nLine * MAX_INDEX_NUM + 1; nNextIndex < (nLine + 1) * MAX_INDEX_NUM; ++nNextIndex)
			{
				if (dwClass[nNextIndex])
				{
					strcat(strText, ", ");
					strcat(strText, pGame->GetClassName(dwClass[nNextIndex]));
				}
			}

			if (dwClass[nFirstIndex])
			{
				::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
				m_pTooltipList->AddItem( 0, strText, wWidth, dwColor ) ;
				++Count ;
			}
		}
	}	
	// -------------------------------------------------------------------------------
	// 스테이터스 제한

	const unsigned char cLimitStatus = lpItem->GetItemInfo().m_UseLimit.m_cLimitStatus;
	const unsigned short wLimitValue = lpItem->GetItemInfo().m_UseLimit.m_wLimitValue;

	unsigned short wMyStatus = 0;

	if ( Item::StatusLimit::NONE != cLimitStatus && 0 != wLimitValue )
	{
		unsigned short wDownLevel = 0;

		switch ( cLimitStatus )
		{
			case Item::StatusLimit::STR:	
				wMyStatus = pGame->m_csStatus.GetSTR();	
				sprintf(strText, CRYLStringTable::m_strString[37], wLimitValue);	
				break;

			case Item::StatusLimit::DEX:	
				wMyStatus = pGame->m_csStatus.GetDEX();	
				sprintf(strText, CRYLStringTable::m_strString[38], wLimitValue);	
				break;

			case Item::StatusLimit::CON:	
				wMyStatus = pGame->m_csStatus.GetCON();	
				sprintf(strText, CRYLStringTable::m_strString[39], wLimitValue);	
				break;

			case Item::StatusLimit::INT:	
				wMyStatus = pGame->m_csStatus.GetINT();	
				sprintf(strText, CRYLStringTable::m_strString[40], wLimitValue);	
				break;

			case Item::StatusLimit::WIS:	
				wMyStatus = pGame->m_csStatus.GetWIS();	
				sprintf(strText, CRYLStringTable::m_strString[41], wLimitValue);	
				break;
			
			case Item::StatusLimit::LEVEL:
				if(lpItem->GetSeasonRecord() == 3)
					wDownLevel = lpItem->GetCoreLevel();

				wDownLevel += pGame->m_csStatus.GetAbilityValue(Skill::Type::AB_EQUIP_LEVELDOWN);

				wMyStatus = pGame->m_csStatus.m_Info.Level;
				sprintf(strText, CRYLStringTable::m_strString[3557], wLimitValue-wDownLevel);
				break;
		}

		unsigned long dwColor = 
			( wMyStatus < (wLimitValue-wDownLevel) ) ? D3DCOLOR_RGBA(255, 0, 0, 255) : D3DCOLOR_RGBA(255, 255, 153, 255);

		if(wMyStatus < (wLimitValue-wDownLevel))
		{
			bLimit = true;
		}

		::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 0, strText, wWidth, dwColor ) ;
		++Count ;
	}

	// -------------------------------------------------------------------------------
	// 룬 스테이터스 제한
	if( runeID )
	{		
		unsigned long dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
		int nValue = lpItem->GetRuneEquipLimitLevel();
		if( nValue > 0 )
		{
			sprintf(strText, CRYLStringTable::m_strString[3557], nValue);
			if (pGame->m_csStatus.m_Info.Level < nValue) dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);

			if (pGame->m_csStatus.m_Info.Level < nValue) 
			{
				bLimit = true;
			}			

			::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
			m_pTooltipList->AddItem( 0, strText, wWidth, dwColor ) ;
			++Count;
		}
	}

	// -------------------------------------------------------------------------------
	// 장착 제한 레벨

	const unsigned short wLowLimitLevel		= lpItem->GetItemInfo().m_DetailData.m_wLowLimitLevel;
	const unsigned short wHighLimitLevel	= lpItem->GetItemInfo().m_DetailData.m_wHighLimitLevel;

	if(wLowLimitLevel || wHighLimitLevel)
	{		
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

		if( pSelfCreature )
		{			
			CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );

			if( pSelfData )
			{
				unsigned long dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);

				char szLimit[128] = {0, };
				sprintf(szLimit, CRYLStringTable::m_strString[3904]);

				if(bLimit || (wLowLimitLevel<=pSelfData->m_cLevel && pSelfData->m_cLevel>=wHighLimitLevel))
				{
					dwColor = D3DCOLOR_RGBA(255, 0, 0, 255);
					sprintf(szLimit, CRYLStringTable::m_strString[3905]);
				}

				if(wLowLimitLevel)
				{					
					sprintf(strText, CRYLStringTable::m_strString[3902], wLowLimitLevel);

					::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
					m_pTooltipList->AddItem( 0, strText, wWidth, dwColor ) ;
					++Count ;
				}

				if(wHighLimitLevel)
				{
					sprintf(strText, CRYLStringTable::m_strString[3903], wHighLimitLevel);

					::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
					m_pTooltipList->AddItem( 0, strText, wWidth, dwColor ) ;
					++Count ;
				}

				::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
				m_pTooltipList->AddItem( 0, szLimit, wWidth, dwColor ) ;
				++Count ;				
			}
		}
	}



	// -------------------------------------------------------------------------------
	// 내구도/최대내구도

	if (m_lpItem->m_bTradeItem && (bBlackMarket || bMedalMarket)) { return; }

	if (m_lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_dwFlags & Item::DetailData::STACKABLE)
	{
		sprintf(strText, CRYLStringTable::m_strString[60], lpItem->GetNumOrDurability(), lpItem->GetMaxNumOrDurability());
		dwLength = g_TextEdit.GetStringLength(strText);
		dwStart = ( ( wWidth - dwLength ) / 2 ) - 15 ;
		::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 0, strText, wWidth, 0xffffffff ) ;
		++Count ;
	}
	else if (m_lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_cItemType != Item::ItemType::RING &&	
			 m_lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_cItemType != Item::ItemType::NECKLACE &&
			 m_lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_cItemType != Item::ItemType::RIDE &&
			 m_lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_cItemType != Item::ItemType::TRANSFORM)
	{
		sprintf(strText, CRYLStringTable::m_strString[42], lpItem->GetNumOrDurability(), lpItem->GetMaxNumOrDurability());
		dwLength = g_TextEdit.GetStringLength(strText);
		dwStart = ( ( wWidth - dwLength ) / 2 ) - 15 ;

		// 내구도
		strcpy(strText, CRYLStringTable::m_strString[43]);
		dwLength = g_TextEdit.GetStringLength(strText);
		::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 0, strText, dwLength, 0xffffffff, rcRect.left ) ;
		dwStart = rcRect.right;
		
		// min value
		sprintf(strText, "%d ", lpItem->GetNumOrDurability());
		dwLength = g_TextEdit.GetStringLength(strText);
		::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3, dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
		
		if ( lpItem->GetNumOrDurability() < lpItem->GetMaxNumOrDurability() / 4 )
			m_pTooltipList->AddItem( 1, strText, dwLength, 0xffff0000, rcRect.left ) ;
		else if ( lpItem->GetNumOrDurability() < lpItem->GetMaxNumOrDurability() / 2 )
			m_pTooltipList->AddItem( 1, strText, dwLength, 0xffffff00, rcRect.left ) ;
		else if ( lpItem->GetNumOrDurability() < lpItem->GetMaxNumOrDurability() * 3 / 4 )
			m_pTooltipList->AddItem( 1, strText, dwLength, 0xff00ff00, rcRect.left ) ;
		else
			m_pTooltipList->AddItem( 1, strText, dwLength, 0xffffffff, rcRect.left ) ;

		dwStart = rcRect.right;
		
		strcpy(strText, "/ ");
		dwLength = g_TextEdit.GetStringLength(strText);
		::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3, dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 2, strText, dwLength, 0xffffffff, rcRect.left ) ;
		dwStart = rcRect.right;

		sprintf(strText, "%d", lpItem->GetMaxNumOrDurability());
		dwLength = g_TextEdit.GetStringLength(strText);
		::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3, dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 3, strText, dwLength, 0xffffffff, rcRect.left ) ;
		dwStart = rcRect.right;
		++Count ;
	}
	
	// -------------------------------------------------------------------------------
	// 아이템 스테이터스 전반

	unsigned short sMinDamage = 0, sMaxDamage = 0, sDefense = 0;
	for (i = 0; i < Item::Attribute::MAX_ATTRIBUTE_NUM; i++)
	{
		if (attribute[ Item::Attribute::Type(i)])
		{
			switch (i)
			{
				case Item::Attribute::MIN_DAMAGE:
					sMinDamage = attribute[ Item::Attribute::Type(i)];
					break;

				case Item::Attribute::MAX_DAMAGE:
					sMaxDamage = attribute[ Item::Attribute::Type(i)];
					break;

				case Item::Attribute::ARMOR:
					sDefense = attribute[ Item::Attribute::Type(i)];
					break;
			}
		}
	}

	if (sMinDamage || sMaxDamage)
	{
		if (m_lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_cItemType == Item::ItemType::RING ||
			m_lpItem->m_lpItemBase->GetItemInfo().m_DetailData.m_cItemType == Item::ItemType::NECKLACE)
		{
			if (sMinDamage)
			{
				sprintf(strText, "%s%d", CRYLStringTable::m_strString[3555], sMinDamage);
				dwLength = g_TextEdit.GetStringLength(strText);
				dwStart = ( ( wWidth - dwLength ) / 2 ) - 15 ;
				::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
				m_pTooltipList->AddItem( 0, strText, wWidth, 0xffffffff ) ;
				++Count ;
			}

			if (sMaxDamage)
			{
				sprintf(strText, "%s%d", CRYLStringTable::m_strString[3556], sMaxDamage);
				dwLength = g_TextEdit.GetStringLength(strText);
				dwStart = ( ( wWidth - dwLength ) / 2 ) - 15 ;
				::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
				m_pTooltipList->AddItem( 0, strText, wWidth, 0xffffffff ) ;
				++Count ;
			}
		}
		else
		{
			sprintf(strText, CRYLStringTable::m_strString[45], sMinDamage, sMaxDamage);
			dwLength = g_TextEdit.GetStringLength(strText);
			dwStart = ( ( wWidth - dwLength ) / 2 ) - 15 ;
			strcpy(strText, CRYLStringTable::m_strString[46]);
			dwLength = g_TextEdit.GetStringLength(strText);
			::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
			m_pTooltipList->AddItem( 0, strText, dwLength, 0xffffffff, rcRect.left ) ;
			dwStart = rcRect.right;

			sprintf(strText, "%d ", sMinDamage);
			dwLength = g_TextEdit.GetStringLength(strText);
			::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3, dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
			dwItemColor = (false == bIsNonGradeItem) ? 
				GetGradeColor(Item::Attribute::MIN_DAMAGE) : D3DCOLOR_RGBA(255, 255, 255, 255);
			m_pTooltipList->AddItem( 1, strText, dwLength, dwItemColor, rcRect.left ) ;
			dwStart = rcRect.right;

			strcpy(strText, "~ ");
			dwLength = g_TextEdit.GetStringLength(strText);
			::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3, dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
			m_pTooltipList->AddItem( 2, strText, dwLength, 0xffffffff, rcRect.left ) ;
			dwStart = rcRect.right;

			sprintf(strText, "%d", sMaxDamage);
			dwLength = g_TextEdit.GetStringLength(strText);
			::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3, dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
			dwItemColor = (false == bIsNonGradeItem) ? 
				GetGradeColor(Item::Attribute::MAX_DAMAGE) : D3DCOLOR_RGBA(255, 255, 255, 255);
			m_pTooltipList->AddItem( 3, strText, dwLength, dwItemColor, rcRect.left ) ;
			dwStart = rcRect.right;
			++Count ;
		}
	}

	if (sDefense)
	{
		sprintf(strText, CRYLStringTable::m_strString[47], sDefense);
		dwLength = g_TextEdit.GetStringLength(strText);
		dwStart = ( ( wWidth - dwLength ) / 2 ) - 15 ;
		strcpy(strText, CRYLStringTable::m_strString[48]);
		dwLength = g_TextEdit.GetStringLength(strText);
		::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 0, strText, dwLength, 0xffffffff, rcRect.left ) ;
		dwStart = rcRect.right;

		sprintf(strText, "%d", sDefense);
		dwLength = g_TextEdit.GetStringLength(strText);
		::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3, dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
		dwItemColor = (false == bIsNonGradeItem) ? 
			GetGradeColor(Item::Attribute::ARMOR) : D3DCOLOR_RGBA(255, 255, 255, 255);
		m_pTooltipList->AddItem( 1, strText, dwLength, dwItemColor, rcRect.left ) ;
		dwStart = rcRect.right;
		++Count ;
	}

	//////////////////////////////
	/*
		if (sMinDamage)
	{
		sprintf(strText, CRYLStringTable::m_strString[45], sMinDamage);
		dwLength = g_TextEdit.GetStringLength(strText);
		dwStart = ( ( wWidth - dwLength ) / 2 ) - 15 ;
		strcpy(strText, CRYLStringTable::m_strString[46]);
		dwLength = g_TextEdit.GetStringLength(strText);
		::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 0, strText, dwLength, 0xffffffff, rcRect.left ) ;
		dwStart = rcRect.right;

		sprintf(strText, "%d", sMinDamage);
		dwLength = g_TextEdit.GetStringLength(strText);
		::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3, dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
		dwItemColor = (false == bIsNonGradeItem) ? 
			GetGradeColor(Item::Attribute::MIN_DAMAGE) : D3DCOLOR_RGBA(255, 255, 255, 255);
		m_pTooltipList->AddItem( 1, strText, dwLength, dwItemColor, rcRect.left ) ;
		dwStart = rcRect.right;
		++Count ;
	}
	*/
	//////////////////////////////

	unsigned long dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);   /////////////////////////////////

	for (i = 0; i < Item::Attribute::MAX_ATTRIBUTE_NUM; ++i)
	{
		if ( attribute[ Item::Attribute::Type(i)] )
		{
			switch (i)
			{
				case Item::Attribute::CRITICAL:		strcpy(strText, CRYLStringTable::m_strString[49]);		break;
				case Item::Attribute::HIT_RATE:		strcpy(strText, CRYLStringTable::m_strString[50]);		break;
				case Item::Attribute::EVADE:		strcpy(strText, CRYLStringTable::m_strString[51]);		break;
				case Item::Attribute::MAGIC_RESIST:	strcpy(strText, CRYLStringTable::m_strString[52]);		break;
				case Item::Attribute::LUCKCHANCE:	strcpy(strText, CRYLStringTable::m_strString[239]);		break;
				case Item::Attribute::BLOCK:		strcpy(strText, CRYLStringTable::m_strString[53]);		break;
				case Item::Attribute::MAX_HP:		strcpy(strText, CRYLStringTable::m_strString[54]);		break;
				case Item::Attribute::MAX_MP:		strcpy(strText, CRYLStringTable::m_strString[55]);		break;
				case Item::Attribute::HP_REGEN:		strcpy(strText, CRYLStringTable::m_strString[56]);		break;
				case Item::Attribute::MP_REGEN:		strcpy(strText, CRYLStringTable::m_strString[57]);		break;
				case Item::Attribute::SPEED:		strcpy(strText, CRYLStringTable::m_strString[58]);		break;
				case Item::Attribute::MAGIC_POWER:	strcpy(strText, CRYLStringTable::m_strString[59]);		break;
				case Item::Attribute::COOLDOWN:		strcpy(strText, CRYLStringTable::m_strString[3549]);	break;
				case Item::Attribute::SKILL_POINT:	strcpy(strText, CRYLStringTable::m_strString[3550]);	break;
				case Item::Attribute::FROST:		strcpy(strText, CRYLStringTable::m_strString[3551]);	break;
				case Item::Attribute::FIRE:			strcpy(strText, CRYLStringTable::m_strString[3552]);	break;
				case Item::Attribute::ELECTRO:		strcpy(strText, CRYLStringTable::m_strString[3553]);	break;
				case Item::Attribute::DARKNESS:		strcpy(strText, CRYLStringTable::m_strString[5200]), dwColor = D3DCOLOR_RGBA(238, 168, 16, 255);	break;
				// TEST ALEX
//				case Item::Attribute::ADD_STR:		strcpy(strText, CRYLStringTable::m_strString[5200]);	break;

				default:	continue;
			}

			char strTemp[100], a[10];
			strcpy(strTemp, strText);
			if (i == Item::Attribute::MAX_HP || i == Item::Attribute::MAX_MP)
			{
				sprintf(a, "%d", attribute[ Item::Attribute::Type(i)] * 10);
			} 
			else
			{
				sprintf(a, "%d", attribute[ Item::Attribute::Type(i)]);
			}

			strcat(strTemp, a);
			dwLength = g_TextEdit.GetStringLength(strTemp);
			dwStart = ( ( wWidth - dwLength ) / 2 ) - 15 ;

			dwLength = g_TextEdit.GetStringLength(strText);
			::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3, SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
			m_pTooltipList->AddItem( 0, strText, dwLength, 0xffffffff, rcRect.left ) ;
			dwStart = rcRect.right;
			
			dwLength = g_TextEdit.GetStringLength(a);
			::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3, dwStart + dwLength, SizeY + Count * 16 + 16 + 3);
			dwItemColor = (false == bIsNonGradeItem) ? 
				GetGradeColor(static_cast<Item::Attribute::Type>(i)) : D3DCOLOR_RGBA(255, 255, 255, 255);
			m_pTooltipList->AddItem( 1, a, dwLength, dwItemColor, rcRect.left ) ;
			dwStart = rcRect.right;
			++Count ;
		}
	}

	// 제련단계
	if (lpItem->GetUpgradeLevel() > 0)
	{
		char strUpgrade[MAX_PATH];
		sprintf(strUpgrade, CRYLStringTable::m_strString[2785], lpItem->GetUpgradeLevel());
		::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 0, strUpgrade, wWidth, 0xffffffff ) ;
		++Count ;
	}

	// 각성 레벨 표시
	if(lpItem->GetSeasonRecord() >= 3 && lpItem->GetCoreLevel() > 0)
	{
		char strUpgrade[MAX_PATH];
		sprintf(strUpgrade, CRYLStringTable::m_strString[4302], lpItem->GetCoreLevel());
		::SetRect(&rcRect, SizeX + 5, SizeY + Count * 16 + 3, SizeX + 5 + sLength, SizeY + Count * 16 + 16 + 3);
		m_pTooltipList->AddItem( 0, strUpgrade, wWidth, 0xffffffff ) ;
		++Count ;
	}


	if (lpItem->GetMaxSocketNum())
	{
		if (lpItem->GetMaxSocketNum() < 4)
		{
			dwStart = ( wWidth / 2) - ((lpItem->GetMaxSocketNum() * 32) / 2) - 15 ;
		} 
		else
		{
			dwStart = ( wWidth / 2) - 64 - 15 ;
		}

		TLVertex pVertices[4];
		pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
		pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
		pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
		pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

		pVertices[0].tu = pVertices[1].tu = 0.0f;
		pVertices[2].tu = pVertices[3].tu = 1.0f;
		pVertices[0].tv = pVertices[2].tv = 0.0f;
		pVertices[1].tv = pVertices[3].tv = 1.0f;

		lpD3DDevice->SetVertexShader(NULL);
		lpD3DDevice->SetVertexShader(TLVERTEXFVF);
		lpD3DDevice->SetTexture(0, pGame->m_lpInterface->m_lpSocket->GetTexture());
		lpD3DDevice->SetTexture(1, NULL);

		for (i = 0; i < lpItem->GetMaxSocketNum(); i++)
		{
			pVertices[0].v.x = pVertices[1].v.x = SizeX + dwStart + (i % 4) * 32 - 0.5f;
			pVertices[2].v.x = pVertices[3].v.x = SizeX + dwStart + (i % 4) * 32 + 32 - 0.5f;
			pVertices[0].v.y = pVertices[2].v.y = SizeY + Count * 16 + 3 + (i / 4) * 32 - 0.5f;
			pVertices[1].v.y = pVertices[3].v.y = SizeY + Count * 16 + 3 + (i / 4) * 32 + 32 - 0.5f;

			lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
		}

		const Item::ItemInfo *lpPT;
		unsigned long count = 0;
		unsigned long SpriteSizeX;
		unsigned long SpriteSizeY;
		for (i = 0; i < 12; i++)
		{
			if (lpItem->GetSocket(i))
			{
				lpPT = g_ItemMgr.GetItemInfo(lpItem->GetSocket(i) + 1900);
				if (lpPT)
				{
					SpriteSizeX = (lpPT->m_SpriteData.m_nSpriteMaxX - lpPT->m_SpriteData.m_nSpriteMinX);
					SpriteSizeY = (lpPT->m_SpriteData.m_nSpriteMaxY - lpPT->m_SpriteData.m_nSpriteMinY);

					pVertices[0].v.x = pVertices[1].v.x = SizeX + 16 - (SpriteSizeX / 2) + dwStart + (count % 4) * 32 - 0.5f;
					pVertices[2].v.x = pVertices[3].v.x = SizeX + 16 - (SpriteSizeX / 2) + dwStart + (count % 4) * 32 + SpriteSizeX - 0.5f;
					pVertices[0].v.y = pVertices[2].v.y = SizeY + 16 - (SpriteSizeY / 2) + Count * 16 + 3 + (count / 4) * 32 - 0.5f;
					pVertices[1].v.y = pVertices[3].v.y = SizeY + 16 - (SpriteSizeY / 2) + Count * 16 + 3 + (count / 4) * 32 + SpriteSizeY - 0.5f;

					pVertices[0].tu = pVertices[1].tu = lpPT->m_SpriteData.m_nSpriteMinX / 256.0f;
					pVertices[2].tu = pVertices[3].tu = lpPT->m_SpriteData.m_nSpriteMaxX / 256.0f;
					pVertices[0].tv = pVertices[2].tv = lpPT->m_SpriteData.m_nSpriteMinY / 256.0f;
					pVertices[1].tv = pVertices[3].tv = lpPT->m_SpriteData.m_nSpriteMaxY / 256.0f;

					lpD3DDevice->SetTexture(0, ((CTexture *)g_ItemStorage.GetItemTexture(lpPT->m_SpriteData.m_szSpriteName))->GetTexture());
					lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
					count++;
				}
			}
		}
	}

	if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::RUNE))
	{
		// 룬소켓
		int nRuneSocketNum = 0;
		nRuneSocketNum = lpItem->GetMaxRuneSocket();

		if (nRuneSocketNum)
		{
			dwStart = ( wWidth / 2) - ((nRuneSocketNum * 32) / 2) - 15 ;

			TLVertex pVertices[4];
			pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
			pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
			pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
			pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

			pVertices[0].tu = pVertices[1].tu = 0.0f;
			pVertices[2].tu = pVertices[3].tu = 1.0f;
			pVertices[0].tv = pVertices[2].tv = 0.0f;
			pVertices[1].tv = pVertices[3].tv = 1.0f;

			lpD3DDevice->SetVertexShader(NULL);
			lpD3DDevice->SetVertexShader(TLVERTEXFVF);
			lpD3DDevice->SetTexture(0, pGame->m_lpInterface->m_lpSocket->GetTexture());
			lpD3DDevice->SetTexture(1, NULL);

			for (i = 0; i < nRuneSocketNum; i++)
			{
				pVertices[0].v.x = pVertices[1].v.x = SizeX + dwStart + (i % 4) * 32 - 0.5f;
				pVertices[2].v.x = pVertices[3].v.x = SizeX + dwStart + (i % 4) * 32 + 32 - 0.5f;
				pVertices[0].v.y = pVertices[2].v.y = SizeY + Count * 16 + 3 + dwSocketSize - 0.5f;
				pVertices[1].v.y = pVertices[3].v.y = SizeY + Count * 16 + 3 + dwSocketSize + 32 - 0.5f;

				lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
			}

			const Item::ItemInfo *lpPT;
			unsigned long count = 0;
			unsigned long SpriteSizeX;
			unsigned long SpriteSizeY;
			
			for (i = 0; i < 2; i++)
			{
				if ( (runeID = lpItem->GetRuneSocket( i ) ) )
				{
					lpPT = g_ItemMgr.GetItemInfo( runeID );
					if (lpPT)
					{
						SpriteSizeX = (lpPT->m_SpriteData.m_nSpriteMaxX - lpPT->m_SpriteData.m_nSpriteMinX);
						SpriteSizeY = (lpPT->m_SpriteData.m_nSpriteMaxY - lpPT->m_SpriteData.m_nSpriteMinY);


						pVertices[0].v.x = pVertices[1].v.x = SizeX + 16 - (SpriteSizeX / 2) + dwStart + (count % 4) * 32 - 0.5f;
						pVertices[2].v.x = pVertices[3].v.x = SizeX + 16 - (SpriteSizeX / 2) + dwStart + (count % 4) * 32 + SpriteSizeX - 0.5f;
						pVertices[0].v.y = pVertices[2].v.y = SizeY + 16 - (SpriteSizeY / 2) + Count * 16 + 3 + dwSocketSize - 0.5f;
						pVertices[1].v.y = pVertices[3].v.y = SizeY + 16 - (SpriteSizeY / 2) + Count * 16 + 3 + dwSocketSize + SpriteSizeY - 0.5f;

						pVertices[0].tu = pVertices[1].tu = lpPT->m_SpriteData.m_nSpriteMinX / 256.0f;
						pVertices[2].tu = pVertices[3].tu = lpPT->m_SpriteData.m_nSpriteMaxX / 256.0f;
						pVertices[0].tv = pVertices[2].tv = lpPT->m_SpriteData.m_nSpriteMinY / 256.0f;
						pVertices[1].tv = pVertices[3].tv = lpPT->m_SpriteData.m_nSpriteMaxY / 256.0f;

						lpD3DDevice->SetTexture(0, ((CTexture *)g_ItemStorage.GetItemTexture(lpPT->m_SpriteData.m_szSpriteName))->GetTexture());
						lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
						count++;
					}
				}
			}

			unsigned long dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);   /////////////////////////////////

			if ( (runeID = lpItem->GetRuneSocket( 0 ) ) )
			{
				const unsigned short*			attribute1 = NULL;
				const unsigned short*			attribute2 = NULL;
				unsigned long runeID2 = 0;

				lpPT = g_ItemMgr.GetItemInfo( runeID );
				attribute1 = lpPT->m_EquipAttribute.m_usAttributeValue;

				if( ( runeID2 = lpItem->GetRuneSocket( 1 ) ) )
				{
					const Item::ItemInfo *lpPT2;
					lpPT2 = g_ItemMgr.GetItemInfo( runeID2 );
					attribute2 = lpPT2->m_EquipAttribute.m_usAttributeValue;
				}

				unsigned short usValue = 0;

				for(int nIndex = 0; nIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++nIndex)
				{

					if( attribute2 )
						usValue = attribute1[ Item::Attribute::Type(nIndex)] + attribute2[ Item::Attribute::Type(nIndex)];
					else
						usValue = attribute1[ Item::Attribute::Type(nIndex)] ;

					if ( usValue )
					{
						switch (nIndex)
						{
							case Item::Attribute::MIN_DAMAGE:	strcpy(strText, CRYLStringTable::m_strString[3555]);	break;
							case Item::Attribute::MAX_DAMAGE:	strcpy(strText, CRYLStringTable::m_strString[3556]);	break;
							case Item::Attribute::ARMOR:		strcpy(strText, CRYLStringTable::m_strString[48]);		break;
							case Item::Attribute::HIT_RATE:		strcpy(strText, CRYLStringTable::m_strString[50]);		break;
							case Item::Attribute::EVADE:		strcpy(strText, CRYLStringTable::m_strString[51]);		break;
							case Item::Attribute::CRITICAL:		strcpy(strText, CRYLStringTable::m_strString[49]);		break;
							case Item::Attribute::BLOCK:		strcpy(strText, CRYLStringTable::m_strString[53]);		break;
							case Item::Attribute::MAGIC_POWER:	strcpy(strText, CRYLStringTable::m_strString[59]);		break;
							case Item::Attribute::MAGIC_RESIST:	strcpy(strText, CRYLStringTable::m_strString[52]);		break;
							case Item::Attribute::LUCKCHANCE:	strcpy(strText, CRYLStringTable::m_strString[239]);		break;								
							case Item::Attribute::COOLDOWN:		strcpy(strText, CRYLStringTable::m_strString[3549]);	break;
							case Item::Attribute::MAX_HP:		strcpy(strText, CRYLStringTable::m_strString[54]);		break;
							case Item::Attribute::HP_REGEN:		strcpy(strText, CRYLStringTable::m_strString[56]);		break;
							case Item::Attribute::MAX_MP:		strcpy(strText, CRYLStringTable::m_strString[55]);		break;
							case Item::Attribute::MP_REGEN:		strcpy(strText, CRYLStringTable::m_strString[57]);		break;
							case Item::Attribute::SPEED:		strcpy(strText, CRYLStringTable::m_strString[58]);		break;
							case Item::Attribute::SKILL_POINT:	strcpy(strText, CRYLStringTable::m_strString[3550]);	break;

							case Item::Attribute::FROST:		strcpy(strText, CRYLStringTable::m_strString[3551]);	break;
							case Item::Attribute::FIRE:			strcpy(strText, CRYLStringTable::m_strString[3552]);	break;
							case Item::Attribute::ELECTRO:		strcpy(strText, CRYLStringTable::m_strString[3553]);	break;
							case Item::Attribute::DARKNESS:		strcpy(strText, CRYLStringTable::m_strString[5200]), dwColor = D3DCOLOR_RGBA(238, 168, 16, 255);	break;
							// TEST ALEX
	//			            case Item::Attribute::ADD_STR:		strcpy(strText, CRYLStringTable::m_strString[3554]);	break;
							default:	continue;
						}

						char strTemp[100], a[10];
						strcpy(strTemp, strText);
						if (nIndex == Item::Attribute::MAX_HP || nIndex == Item::Attribute::MAX_MP)
						{
							sprintf(a, "%d", usValue * 10 );
						} 
						else
						{
							sprintf(a, "%d", usValue );
						}

						strcat(strTemp, a);
						dwLength = g_TextEdit.GetStringLength(strTemp);
						dwStart = ((sLength + 10) / 2) - (dwLength / 2);

						dwLength = g_TextEdit.GetStringLength(strText);
						::SetRect(&rcRect, SizeX + dwStart, SizeY + Count * 16 + 3 + dwSocketSize + 32, 
											SizeX + dwStart + dwLength, SizeY + Count * 16 + 16 + 3 + dwSocketSize + 32);
						dwStart = rcRect.right;
						g_TextEdit.DrawText(strText, &rcRect, DT_VCENTER | DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));

						dwLength = g_TextEdit.GetStringLength(a);
						::SetRect(&rcRect, dwStart, SizeY + Count * 16 + 3 + dwSocketSize + 32, 
											dwStart + dwLength, SizeY + Count * 16 + 16 + 3 + dwSocketSize + 32);
						dwStart = rcRect.right;
						g_TextEdit.DrawText( a, &rcRect, DT_VCENTER | DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
						++Count;
					}
				}
			}
		}	
	}
}

void CGUITooltipItem::RenderSkill(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	// edith 2009.10.26 어빌리티 툴팁
	// 어빌리티 툴팁을 화면에 렌더링한다.
	// 소셜모션도 어빌리티 툴팁을 이용한다.
	if ( m_lpItem->GetSkillType() == Skill::Type::ABILITY || 
		m_lpItem->GetSkillType() == Skill::Type::ACTION )
	{
		unsigned short wSkillID = m_lpItem->GetProtoTypeID() ;
		const Skill::ProtoType *lpSkillBase = g_SkillMgr.GetSkillProtoType(wSkillID), *lpSkill;
		CRYLAbilityDescDlg::SetSameAbilityTooltip( wSkillID ) ;

		if ( g_pAbilityDescDlg )
		{
			unsigned char cSkillStep = m_lpItem->GetMaxDurability() ;  
			unsigned char cSkillLevel = m_lpItem->GetNowDurability() ;
			unsigned char cWeaponType = CRYLGameData::Instance()->m_csStatus.GetWeaponType() ;
			if ( !CRYLAbilityDescDlg::GetSameAbilityTooltip( cSkillStep, cSkillLevel, cWeaponType ) )
			{
				lpSkill = &lpSkillBase[m_lpItem->GetMaxDurability()];

	//			if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::JAPAN )
				if( g_ClientMain.m_iLanguage == 2)
				{
					WCHAR*	strWideString = NULL;
					WCHAR	seps[] = L"\\" ;
					WCHAR*	token;
					char	strSkill[1024];				

					int nLen = MultiByteToWideChar( CP_ACP, 0, lpSkill->m_szSkillDescribe, lstrlen(lpSkill->m_szSkillDescribe), NULL, NULL);
					strWideString = SysAllocStringLen(NULL, nLen);
					MultiByteToWideChar( CP_ACP, 0, lpSkill->m_szSkillDescribe, strlen(lpSkill->m_szSkillDescribe), strWideString, nLen ) ;

					token = wcstok( strWideString, seps ) ;

					nLen = wcslen( token ) ;

					WideCharToMultiByte(CP_ACP, 0, token, nLen + 1, strSkill, 1024, NULL, NULL);

					g_pAbilityDescDlg->SetDetailLabelCaption( const_cast<LPSTR>( strSkill ) ) ;				
			
					SysFreeString(strWideString);
				}
				else
				{
					char strText[MAX_PATH], *token, spes[] = "\\";

					strcpy(strText, lpSkill->m_szSkillDescribe);
					token = strtok(strText, spes);
					g_pAbilityDescDlg->SetDetailLabelCaption( const_cast<LPSTR>( token ) ) ;
				}

				// 스킬 스트립트Ex 루아파일에서 새로 추가된 스킬 스크립트가 정의되어 있어야
				// 스킬 툴팁이나온다.
				// 스킬 ID, 현재 스킬 단계, 현재 스킬 레벨
				LPRYLSKILLNODE  lpSkillNode = g_pSkillScriptMgr()->FindSkillNode( wSkillID ) ;

				if ( lpSkillNode )
				{
					g_pAbilityDescDlg->RestoreAbilityUI( wSkillID, cSkillStep, m_lpItem->GetNowDurability(), cWeaponType ) ;
					g_pAbilityDescDlg->SetVisible( TRUE ) ;
				}
			}
			else if ( g_pAbilityDescDlg )
			{
				LPRYLSKILLNODE  lpSkillNode = g_pSkillScriptMgr()->FindSkillNode( wSkillID ) ;

				if ( lpSkillNode )
				{
					g_pAbilityDescDlg->SetVisible( TRUE ) ;
				}			
			}

			if ( g_pAbilityDescDlg->GetVisible() )
			{
				int Count = 0;

				const unsigned long dwTopMargin = 50;		// 아이콘을 안 그리면 18이 된다.

				if (lpSkillBase)
				{
					lpSkill = &lpSkillBase[m_lpItem->GetMaxDurability()];

					TLVertex pVertices[4];
					pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
					pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
					pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
					pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

					unsigned long SpriteSizeX = (lpSkill->m_SpriteInfo.m_nSpriteMaxX - lpSkill->m_SpriteInfo.m_nSpriteMinX);
					unsigned long SpriteSizeY = (lpSkill->m_SpriteInfo.m_nSpriteMaxY - lpSkill->m_SpriteInfo.m_nSpriteMinY);

					int iLeft = g_pAbilityDescDlg->GetLeft() + 50 ;
					int iTop  = g_pAbilityDescDlg->GetTop() + 61 ;

					pVertices[0].v.x = pVertices[1].v.x = iLeft - (SpriteSizeX / 2) - 0.5f;
					pVertices[2].v.x = pVertices[3].v.x = iLeft - (SpriteSizeX / 2) + 37 - 0.5f;
					pVertices[0].v.y = pVertices[2].v.y = iTop - 0.5f;
					pVertices[1].v.y = pVertices[3].v.y = iTop + 37 - 0.5f;

					pVertices[0].tu = pVertices[1].tu = lpSkill->m_SpriteInfo.m_nSpriteMinX / 256.0f;
					pVertices[2].tu = pVertices[3].tu = lpSkill->m_SpriteInfo.m_nSpriteMaxX / 256.0f;
					pVertices[0].tv = pVertices[2].tv = lpSkill->m_SpriteInfo.m_nSpriteMinY / 256.0f;
					pVertices[1].tv = pVertices[3].tv = lpSkill->m_SpriteInfo.m_nSpriteMaxY / 256.0f;

					lpD3DDevice->SetVertexShader(NULL);
					lpD3DDevice->SetVertexShader(TLVERTEXFVF);
					lpD3DDevice->SetTexture(0, ((CTexture *)g_ItemStorage.GetSkillTexture(lpSkill->m_SpriteInfo.m_szSpriteName))->GetTexture());
					lpD3DDevice->SetTexture(1, NULL);
					lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
				}
			}
		}


		return ;
	}

	unsigned short wSkillID = m_lpItem->GetProtoTypeID() ;
	const Skill::ProtoType *lpSkillBase = g_SkillMgr.GetSkillProtoType(wSkillID), *lpSkill;
	CRYLSkillDescDlg::SetSameSkillTooltip( wSkillID ) ;
	bool bChild = false ;		// 얼터너티브 스킬중에 차일드 스킬을 보일때
	
	if ( g_pSkillDescDlg )
	{
		unsigned char cSkillStep = m_lpItem->GetMaxDurability() ;  
		unsigned char cSkillLevel = m_lpItem->GetNowDurability() ;
		unsigned char cWeaponType = CRYLGameData::Instance()->m_csStatus.GetWeaponType() ;
		if ( !CRYLSkillDescDlg::GetSameSkillTooltip( cSkillStep, cSkillLevel, cWeaponType )  || bChild )
		{
			lpSkill = &lpSkillBase[m_lpItem->GetMaxDurability()];

//			if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::JAPAN )
			if( g_ClientMain.m_iLanguage == 2)
			{
				WCHAR*	strWideString = NULL;
				WCHAR	seps[] = L"\\" ;
				WCHAR*	token;
				char	strSkill[1024];				

				int nLen = MultiByteToWideChar( CP_ACP, 0, lpSkill->m_szSkillDescribe, lstrlen(lpSkill->m_szSkillDescribe), NULL, NULL);
				strWideString = SysAllocStringLen(NULL, nLen);
				MultiByteToWideChar( CP_ACP, 0, lpSkill->m_szSkillDescribe, strlen(lpSkill->m_szSkillDescribe), strWideString, nLen ) ;

				token = wcstok( strWideString, seps ) ;

				nLen = wcslen( token ) ;

				WideCharToMultiByte(CP_ACP, 0, token, nLen + 1, strSkill, 1024, NULL, NULL);

				g_pSkillDescDlg->SetDetailLabelCaption( const_cast<LPSTR>( strSkill ) ) ;				
		
				SysFreeString(strWideString);
			}
			else
			{
				char strText[MAX_PATH], *token, spes[] = "\\";

				strcpy(strText, lpSkill->m_szSkillDescribe);
				token = strtok(strText, spes);
				g_pSkillDescDlg->SetDetailLabelCaption( const_cast<LPSTR>( token ) ) ;
			}

			// 스킬 ID, 현재 스킬 단계, 현재 스킬 레벨
			LPRYLSKILLNODE  lpSkillNode = g_pSkillScriptMgr()->FindSkillNode( wSkillID ) ;

			if ( lpSkillNode )
			{
				g_pSkillDescDlg->RestoreSkillUI( wSkillID, cSkillStep, m_lpItem->GetNowDurability(), cWeaponType ) ;
				g_pSkillDescDlg->SetVisible( TRUE ) ;
			}
		}
		else if ( g_pSkillDescDlg )
		{
			LPRYLSKILLNODE  lpSkillNode = g_pSkillScriptMgr()->FindSkillNode( wSkillID ) ;

			if ( lpSkillNode )
			{
				g_pSkillDescDlg->SetVisible( TRUE ) ;
			}			
		}

		if ( g_pSkillDescDlg->GetVisible() )
		{
			int Count = 0;

			const unsigned long dwTopMargin = 50;		// 아이콘을 안 그리면 18이 된다.

			if (lpSkillBase)
			{
				lpSkill = &lpSkillBase[m_lpItem->GetMaxDurability()];

				TLVertex pVertices[4];
				pVertices[0].Diffuse.c = pVertices[1].Diffuse.c = pVertices[2].Diffuse.c = pVertices[3].Diffuse.c = 0xFFFFFFFF;
				pVertices[0].Specular.c = pVertices[1].Specular.c = pVertices[2].Specular.c = pVertices[3].Specular.c = 0;
				pVertices[0].w = pVertices[1].w = pVertices[2].w = pVertices[3].w = 0.1f;
				pVertices[0].v.z = pVertices[1].v.z = pVertices[2].v.z = pVertices[3].v.z = 0.1f;

				unsigned long SpriteSizeX = (lpSkill->m_SpriteInfo.m_nSpriteMaxX - lpSkill->m_SpriteInfo.m_nSpriteMinX);
				unsigned long SpriteSizeY = (lpSkill->m_SpriteInfo.m_nSpriteMaxY - lpSkill->m_SpriteInfo.m_nSpriteMinY);

				int iLeft = g_pSkillDescDlg->GetLeft() + 50 ;
				int iTop  = g_pSkillDescDlg->GetTop() + 61 ;

				pVertices[0].v.x = pVertices[1].v.x = iLeft - (SpriteSizeX / 2) - 0.5f;
				pVertices[2].v.x = pVertices[3].v.x = iLeft - (SpriteSizeX / 2) + 37 - 0.5f;
				pVertices[0].v.y = pVertices[2].v.y = iTop - 0.5f;
				pVertices[1].v.y = pVertices[3].v.y = iTop + 37 - 0.5f;

				pVertices[0].tu = pVertices[1].tu = lpSkill->m_SpriteInfo.m_nSpriteMinX / 256.0f;
				pVertices[2].tu = pVertices[3].tu = lpSkill->m_SpriteInfo.m_nSpriteMaxX / 256.0f;
				pVertices[0].tv = pVertices[2].tv = lpSkill->m_SpriteInfo.m_nSpriteMinY / 256.0f;
				pVertices[1].tv = pVertices[3].tv = lpSkill->m_SpriteInfo.m_nSpriteMaxY / 256.0f;

				lpD3DDevice->SetVertexShader(NULL);
				lpD3DDevice->SetVertexShader(TLVERTEXFVF);
				lpD3DDevice->SetTexture(0, ((CTexture *)g_ItemStorage.GetSkillTexture(lpSkill->m_SpriteInfo.m_szSpriteName))->GetTexture());
				lpD3DDevice->SetTexture(1, NULL);
				lpD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(TLVertex));
			}
		}
	}
}

void CGUITooltipItem::GetRuneTooltipPos(short &sPosX, short &sPosY, short &sLength, short &sLine)
{
	char			strText[100];
	long			dwLength;
	CRYLGameData*	pGame = CRYLGameData::Instance();
	const unsigned short*			attribute = NULL;

	const Item::ItemInfo* itemInfo = Item::CItemMgr::GetInstance().GetItemInfo(m_lpItem->GetProtoTypeID() );
	if( !itemInfo )
		return;

	sLength = dwLength = g_TextEdit.GetStringLength(m_lpItem->GetItemInfo().m_SpriteData.m_szName);
	sLine = 1;

	if (pGame->m_lpInterface->GetIsTrade())
	{
		if (m_lpItem->m_bTradeItem)
		{
			if (m_lpItem->m_bMedalBuyItem)
			{
				sprintf(strText, CRYLStringTable::m_strString[862], g_TextEdit.MakePrintGold(m_lpItem->m_lpItemBase->GetBuyMedalPrice()));
			} 
			else
			{
				unsigned long dwBuyPrice = m_lpItem->m_lpItemBase->GetBuyPrice();

				NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
				if ( pNpcNode && pNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
					if ( lpCastle )
					{
						dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
					}
				}

				sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(dwBuyPrice));
			}
		} 
		else
		{
			sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(m_lpItem->m_lpItemBase->GetSellPrice()));
		}

		dwLength = g_TextEdit.GetStringLength(strText);
		if (sLength < dwLength) sLength = dwLength;
		sLine++;
	}

	// -------------------------------------------------------------------------------
	// 제한

	unsigned long dwValue;
	dwValue = itemInfo->m_UseLimit.m_wLimitValue ;
	if( dwValue > 0 )
	{
		switch( itemInfo->m_UseLimit.m_cLimitStatus )
		{
		case Skill::StatusLimit::STR:
			sprintf(strText, CRYLStringTable::m_strString[37], dwValue + 20);
			break;

		case Skill::StatusLimit::DEX:
			sprintf(strText, CRYLStringTable::m_strString[38], dwValue + 20);
			break;

		case Skill::StatusLimit::CON:
			sprintf(strText, CRYLStringTable::m_strString[39], dwValue + 20);
			break;

		case Skill::StatusLimit::INT:
			sprintf(strText, CRYLStringTable::m_strString[40], dwValue + 20);
			break;

		case Skill::StatusLimit::WIS:
			sprintf(strText, CRYLStringTable::m_strString[41], dwValue + 20);
			break;


		case Skill::StatusLimit::LEVEL:
			sprintf(strText, CRYLStringTable::m_strString[3557], dwValue);
			break;

		default:
			sprintf(strText,"");
			break;
		}

		dwLength = g_TextEdit.GetStringLength(strText);
		if (sLength < dwLength) sLength = dwLength;
		sLine++;
	}

	unsigned long dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);   /////////////////////////////////

	attribute = itemInfo->m_EquipAttribute.m_usAttributeValue;

	for(int nIndex = 0; nIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++nIndex)
	{
		if ( attribute[ Item::Attribute::Type(nIndex)] )
		{
			switch (nIndex)
			{
			case Item::Attribute::MIN_DAMAGE:	strcpy(strText, CRYLStringTable::m_strString[3555]);		break;
			case Item::Attribute::MAX_DAMAGE:	strcpy(strText, CRYLStringTable::m_strString[3556]);		break;
			case Item::Attribute::ARMOR:		strcpy(strText, CRYLStringTable::m_strString[48]);		break;
			case Item::Attribute::HIT_RATE:		strcpy(strText, CRYLStringTable::m_strString[50]);		break;
			case Item::Attribute::EVADE:		strcpy(strText, CRYLStringTable::m_strString[51]);		break;
			case Item::Attribute::CRITICAL:		strcpy(strText, CRYLStringTable::m_strString[49]);		break;
			case Item::Attribute::BLOCK:		strcpy(strText, CRYLStringTable::m_strString[53]);		break;
			case Item::Attribute::MAGIC_POWER:	strcpy(strText, CRYLStringTable::m_strString[59]);		break;
			case Item::Attribute::MAGIC_RESIST:	strcpy(strText, CRYLStringTable::m_strString[52]);		break;
			case Item::Attribute::LUCKCHANCE:	strcpy(strText, CRYLStringTable::m_strString[239]);		break;

			case Item::Attribute::COOLDOWN:		strcpy(strText, CRYLStringTable::m_strString[3549]);	break;
			case Item::Attribute::MAX_HP:		strcpy(strText, CRYLStringTable::m_strString[54]);		break;
			case Item::Attribute::HP_REGEN:		strcpy(strText, CRYLStringTable::m_strString[56]);		break;
			case Item::Attribute::MAX_MP:		strcpy(strText, CRYLStringTable::m_strString[55]);		break;
			case Item::Attribute::MP_REGEN:		strcpy(strText, CRYLStringTable::m_strString[57]);		break;
			case Item::Attribute::SPEED:		strcpy(strText, CRYLStringTable::m_strString[58]);		break;
			case Item::Attribute::SKILL_POINT:	strcpy(strText, CRYLStringTable::m_strString[3550]);	break;

			case Item::Attribute::FROST:		strcpy(strText, CRYLStringTable::m_strString[3551]);	break;
			case Item::Attribute::FIRE:			strcpy(strText, CRYLStringTable::m_strString[3552]);	break;
			case Item::Attribute::ELECTRO:		strcpy(strText, CRYLStringTable::m_strString[3553]);	break;
		case Item::Attribute::DARKNESS:		strcpy(strText, CRYLStringTable::m_strString[5200]), dwColor = D3DCOLOR_RGBA(238, 168, 16, 255);	break;
			// TEST ALEX
//		    case Item::Attribute::ADD_STR:		strcpy(strText, CRYLStringTable::m_strString[3554]);	break;
			default:	continue;
			}

			char strTemp[100], a[10];
			strcpy(strTemp, strText);
			if (nIndex == Item::Attribute::MAX_HP || nIndex == Item::Attribute::MAX_MP)
			{
				sprintf(a, "%d", attribute[ Item::Attribute::Type(nIndex)] * 10);
			} 
			else
			{
				sprintf(a, "%d", attribute[ Item::Attribute::Type(nIndex)]);
			}

			strcat(strTemp, a);
			dwLength = g_TextEdit.GetStringLength(strTemp) + 10;
			if (sLength < dwLength) sLength = dwLength;
			sLine++;

		}
	}

	if (sPosY + (sLine * 16 + 6) > g_ClientMain.m_BaseGraphicLayer.m_lScreenSy )
	{
		sPosY = g_ClientMain.m_BaseGraphicLayer.m_lScreenSy -  (sLine * 16 + 6) ;
	}
}


void CGUITooltipItem::GetEquipTooltipPos(short &sPosX, short &sPosY, short &sLength, short &sLine)
{
	char			strText[100];
	long			dwLength;
	int				i;
	short attribute[ Item::Attribute::MAX_ATTRIBUTE_NUM ];
	CRYLGameData*	pGame = CRYLGameData::Instance();

	Item::CEquipment *lpItem = Item::CEquipment::DowncastToEquipment(m_lpItem->m_lpItemBase);
	if (!lpItem) return;

	lpItem->GetSubRuneAttribute( attribute );
	unsigned long runeID = 0;
	runeID = lpItem->GetRuneSocket(0) ;

	char			strGrade[20];
	char strRuneName[ 100 ];
	unsigned long dwRuneLength = 0;

	strcpy( strRuneName, "" );
	if( runeID )
	{
		const Item::ItemInfo *lpPT;
		if( lpItem->GetRuneSocket(1) )
		{
			strcpy( strRuneName, CRYLStringTable::m_strString[3562] );
		}
		else
		{
			lpPT = g_ItemMgr.GetItemInfo( runeID );
			if( lpPT )
			{
				strcpy( strRuneName, lpPT->m_SpriteData.m_szName );
			}
		}
	}
	if( strcmp( strRuneName, "" ) )
	{
		strcat( strRuneName, " " );
		dwRuneLength = g_TextEdit.GetStringLength( strRuneName );
	}


	Item::Grade::GradeInfo gradeInfo = lpItem->GetItemGrade();

	CRYLGameScene*	    pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLNewTradeDlg*	pTradeDlg	= ( CRYLNewTradeDlg* )pScene->GetNewTradeDlg();

	BOOL bBlackMarket = pTradeDlg->IsBlackMarket();
	BOOL bMedalMarket = pTradeDlg->IsMedalMarket();

	switch ( gradeInfo.m_eItemGrade )
	{
	/*	
		case Item::EquipType::S_GRADE:		strcpy( strGrade, " S" );		break;
		case Item::EquipType::AAA_GRADE:	strcpy( strGrade, " AAA" );		break;
		case Item::EquipType::AA_GRADE:		strcpy( strGrade, " AA" );		break;
		case Item::EquipType::A_GRADE:		strcpy( strGrade, " A" );		break;
		case Item::EquipType::B_GRADE:		strcpy( strGrade, " B" );		break;
		case Item::EquipType::C_GRADE:		strcpy( strGrade, " C" );		break;
		case Item::EquipType::D_GRADE:		strcpy( strGrade, " D" );		break;
		case Item::EquipType::F_GRADE:		strcpy( strGrade, " F" );		break;
		case Item::EquipType::X_GRADE:		strcpy( strGrade, "" );			break;
	*/
		case Item::EquipType::S_GRADE:		strcpy( strGrade, " S" );		break;
		case Item::EquipType::AAA_GRADE:	strcpy( strGrade, " S" );		break;
		case Item::EquipType::AA_GRADE:		strcpy( strGrade, " AAA" );		break;
		case Item::EquipType::A_GRADE:		strcpy( strGrade, " AA" );		break;
		case Item::EquipType::B_GRADE:		strcpy( strGrade, " A" );		break;
		case Item::EquipType::C_GRADE:		strcpy( strGrade, " B" );		break;
		case Item::EquipType::D_GRADE:		strcpy( strGrade, " C" );		break;
		case Item::EquipType::F_GRADE:		strcpy( strGrade, " D" );		break;
		case Item::EquipType::X_GRADE:		strcpy( strGrade, "" );			break;
	}

	for ( i = 0; i < gradeInfo.m_cPlus; i++ )
		strcat( strGrade, "+");
	// edith 2008.01.16 +++++ 로 붙는걸. (+5) 로 변경.
//	if(gradeInfo.m_cPlus > 0)
//	{
//		char strTemp[32];
//		sprintf(strTemp, "(+%d)", gradeInfo.m_cPlus);
//		strcat( strGrade, strTemp);
//	}

	// 행운이 붙는지 확인
	if(lpItem->GetSeasonRecord() == 3)
		dwRuneLength += (g_TextEdit.GetStringLength( CRYLStringTable::m_strString[4301] )+10);
	else if(lpItem->GetSeasonRecord() == 2)
		dwRuneLength += (g_TextEdit.GetStringLength( CRYLStringTable::m_strString[238] )+10);

	if (m_lpItem->m_bTradeItem && (bBlackMarket || bMedalMarket))
	{
		sLength = dwLength = g_TextEdit.GetStringLength(m_lpItem->GetItemInfo().m_SpriteData.m_szName);
		sLine = 1;
	} 
	else
	{
		sLength = dwLength = dwRuneLength + g_TextEdit.GetStringLength(m_lpItem->GetItemInfo().m_SpriteData.m_szName) +  g_TextEdit.GetStringLength( strGrade );
		sLine = 1;
	}

	// 보상 판매 여부
	if (0 == lpItem->GetSeasonRecord() && m_lpItem->GetIsEnableAddOption() && (FALSE == bBlackMarket || FALSE == bMedalMarket))
	{
		dwLength = g_TextEdit.GetStringLength(CRYLStringTable::m_strString[2777]);
		if (sLength < dwLength) sLength = dwLength;
		++sLine;
	}

	// 아이템 분류
	dwLength = g_TextEdit.GetStringLength(lpItem->GetItemTypeName());
	if (sLength < dwLength) sLength = dwLength;
	++sLine;

	// 가격
	if (pGame->m_lpInterface->GetIsTrade())
	{
		if (m_lpItem->m_bTradeItem)
		{
			if (bBlackMarket)
			{
				unsigned long dwBuyBlackPrice = lpItem->GetBuyBlackPrice();

				NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
				if ( pNpcNode && pNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
					if ( lpCastle )
					{
						dwBuyBlackPrice += static_cast<unsigned long>( dwBuyBlackPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
					}
				}

				sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(dwBuyBlackPrice));
			} 
			else if (m_lpItem->m_bMedalBuyItem)
			{
				sprintf(strText, CRYLStringTable::m_strString[862], g_TextEdit.MakePrintGold(lpItem->GetBuyMedalPrice()));
			} 
			else
			{
				unsigned long dwBuyPrice = lpItem->GetBuyPrice();
				NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
				if ( pNpcNode && pNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
					if ( lpCastle )
					{
						dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
					}
				}

				sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(dwBuyPrice));
			}
		} 
		else
		{
			sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(lpItem->GetSellPrice()));
		}

		dwLength = g_TextEdit.GetStringLength(strText);
		if (sLength < dwLength) sLength = dwLength;
		sLine++;
	}

	const Item::UseLimit *lpLimit = &lpItem->GetItemInfo().m_UseLimit;

	unsigned long dwClass[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, count = 0;
	// 종족을 판별하는 if가 추가되야한다.
	if (lpLimit->m_dwClassLimit != 0x00FF0FFF && lpLimit->m_dwClassLimit != 0x00000FFF && lpLimit->m_dwClassLimit != 0x00FF0000)
	{
		for (int i = 1; i <= 24; i++)
		{
			if (lpLimit->m_dwClassLimit & (0x00000001 << (i - 1)))
			{
				dwClass[count] = i;
				count++;
			}
		}

		if (dwClass[0])
		{
			sprintf(strText, "%s", pGame->GetClassName(dwClass[0]));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
			sLine++;
		}
		if (dwClass[1])
		{
			strcat(strText, ", ");
			strcat(strText, pGame->GetClassName(dwClass[1]));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
		}
		if (dwClass[2])
		{
			strcat(strText, ", ");
			strcat(strText, pGame->GetClassName(dwClass[2]));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
		}
		if (dwClass[3])
		{
			sprintf(strText, "%s", pGame->GetClassName(dwClass[3]));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
			sLine++;
		}
		if (dwClass[4])
		{
			strcat(strText, ", ");
			strcat(strText, pGame->GetClassName(dwClass[4]));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
		}
		if (dwClass[5])
		{
			strcat(strText, ", ");
			strcat(strText, pGame->GetClassName(dwClass[5]));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
		}
		if (dwClass[6])
		{
			sprintf(strText, "%s", pGame->GetClassName(dwClass[6]));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
			sLine++;
		}
		if (dwClass[7])
		{
			strcat(strText, ", ");
			strcat(strText, pGame->GetClassName(dwClass[7]));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
		}
		if (dwClass[8])
		{
			strcat(strText, ", ");
			strcat(strText, pGame->GetClassName(dwClass[8]));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
		}

	}

	if ( lpLimit->m_wLimitValue )
	{
		strcpy(strText, "");

		switch ( lpLimit->m_cLimitStatus )
		{
			case Item::StatusLimit::NONE:
				break;

			case Item::StatusLimit::STR:
				sprintf(strText, CRYLStringTable::m_strString[37], lpLimit->m_wLimitValue);
				break;

			case Item::StatusLimit::DEX:
				sprintf(strText, CRYLStringTable::m_strString[38], lpLimit->m_wLimitValue);
				break;

			case Item::StatusLimit::CON:
				sprintf(strText, CRYLStringTable::m_strString[39], lpLimit->m_wLimitValue);
				break;

			case Item::StatusLimit::INT:
				sprintf(strText, CRYLStringTable::m_strString[40], lpLimit->m_wLimitValue);
				break;

			case Item::StatusLimit::WIS:
				sprintf(strText, CRYLStringTable::m_strString[41], lpLimit->m_wLimitValue);
				break;

			case Item::StatusLimit::LEVEL:
				sprintf(strText, CRYLStringTable::m_strString[3557], lpLimit->m_wLimitValue);
				break;
		}

		dwLength = g_TextEdit.GetStringLength(strText);
		if ( sLength < dwLength ) 
			sLength = dwLength;

		if(dwLength)
			sLine++;
	}

	// -------------------------------------------------------------------------------
	// 룬 스테이터스 제한
	if( runeID )
	{
		unsigned long dwValue ;
		dwValue = lpItem->GetRuneEquipLimitLevel() ;
		if( dwValue > 0 )
		{
			sprintf(strText, CRYLStringTable::m_strString[3557], dwValue);

			dwLength = g_TextEdit.GetStringLength(strText);
			if ( sLength < dwLength ) 
				sLength = dwLength;
			sLine++;
		}
	}

	if (m_lpItem->m_bTradeItem && (bBlackMarket || bMedalMarket)) 
	{
		return;
	}

	// edith 2008.02.20 툴팁에 내구도 표시부분
	if (m_lpItem->GetItemInfo().m_DetailData.m_dwFlags & Item::DetailData::STACKABLE)
	{
		sprintf(strText, CRYLStringTable::m_strString[60], m_lpItem->GetNowDurability(), m_lpItem->GetMaxDurability());
	}
	else
	{
		sprintf(strText, CRYLStringTable::m_strString[42], m_lpItem->GetNowDurability(), m_lpItem->GetMaxDurability());
	}

	if (m_lpItem->GetItemInfo().m_DetailData.m_cItemType != Item::ItemType::RING &&
		m_lpItem->GetItemInfo().m_DetailData.m_cItemType != Item::ItemType::NECKLACE &&
		m_lpItem->GetItemInfo().m_DetailData.m_cItemType != Item::ItemType::RIDE &&
		m_lpItem->GetItemInfo().m_DetailData.m_cItemType != Item::ItemType::TRANSFORM)
	{
		dwLength = g_TextEdit.GetStringLength(strText);
		if (sLength < dwLength) sLength = dwLength;
		sLine++;
	}

	unsigned short sMinDamage = 0, sMaxDamage = 0, sDefense = 0;
	for (i = 0; i < Item::Attribute::MAX_ATTRIBUTE_NUM; i++)
	{
		if ( attribute[ Item::Attribute::Type(i)] )
		{
			switch (i)
			{
				case Item::Attribute::MIN_DAMAGE:
					sMinDamage = attribute[ Item::Attribute::Type(i)];
					break;

				case Item::Attribute::MAX_DAMAGE:
					sMaxDamage = attribute[ Item::Attribute::Type(i)];
					break;

				case Item::Attribute::ARMOR:
					sDefense = attribute[ Item::Attribute::Type(i)];
					break;
			}
		}
	}

	if (sMinDamage || sMaxDamage)
	{
		if (m_lpItem->GetItemInfo().m_DetailData.m_cItemType == Item::ItemType::RING ||
			m_lpItem->GetItemInfo().m_DetailData.m_cItemType == Item::ItemType::NECKLACE)
		{
			if (sMinDamage)
			{
				sprintf(strText, "%s%d", CRYLStringTable::m_strString[3555], sMinDamage);
				dwLength = g_TextEdit.GetStringLength(strText);
				if (sLength < dwLength) sLength = dwLength;
				sLine++;
			}

			if (sMaxDamage)
			{
				sprintf(strText, "%s%d", CRYLStringTable::m_strString[3556], sMaxDamage);
				dwLength = g_TextEdit.GetStringLength(strText);
				if (sLength < dwLength) sLength = dwLength;
				sLine++;
			}
		}
		else
		{
			sprintf(strText, CRYLStringTable::m_strString[45], sMinDamage, sMaxDamage);
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
			sLine++;
		}
	}

	if (sDefense)
	{
		sprintf(strText, CRYLStringTable::m_strString[47], sDefense);
		dwLength = g_TextEdit.GetStringLength(strText);
		if (sLength < dwLength) sLength = dwLength;
		sLine++;
	}

	for (i = 0; i < Item::Attribute::MAX_ATTRIBUTE_NUM; i++)
	{
		if ( attribute[ Item::Attribute::Type(i)] )
		{
			switch (i)
			{
				case Item::Attribute::CRITICAL:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[49], strText, attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::HIT_RATE:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[50], strText, attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::EVADE:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[51], strText, attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::MAGIC_RESIST:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[52], strText, attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::BLOCK:
					sprintf(strText, "%s%3.1f%%", CRYLStringTable::m_strString[53], strText, attribute[ Item::Attribute::Type(i)] / 2.0f);
					break;

				case Item::Attribute::MAX_HP:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[54], strText, attribute[ Item::Attribute::Type(i)] * 10);
					break;

				case Item::Attribute::MAX_MP:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[55], strText, attribute[ Item::Attribute::Type(i)] * 10);
					break;

				case Item::Attribute::HP_REGEN:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[56], strText, attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::MP_REGEN:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[57], strText, attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::SPEED:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[58], strText, attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::MAGIC_POWER:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[59], strText, attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::LUCKCHANCE:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[239], strText, attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::COOLDOWN:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[3549], attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::SKILL_POINT:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[3550], attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::FROST:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[3551], attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::FIRE:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[3552], attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::ELECTRO:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[3553], attribute[ Item::Attribute::Type(i)] );
					break;

				case Item::Attribute::DARKNESS:
					sprintf(strText, "%s%d", CRYLStringTable::m_strString[5200], attribute[ Item::Attribute::Type(i)] ); // was 3554
					break;
					// test alex
			//	case Item::Attribute::ADD_STR:
			//		sprintf(strText, "%s%d", CRYLStringTable::m_strString[5200], attribute[ Item::Attribute::Type(i)] );
			//		break;

				default:
					continue;
			}
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
			sLine++;
		}
	}
		
	// 제련단계
	if (lpItem->GetUpgradeLevel() > 0)
	{
		char strUpgrade[MAX_PATH];
		sprintf(strUpgrade, CRYLStringTable::m_strString[2785], lpItem->GetUpgradeLevel());
		dwLength = g_TextEdit.GetStringLength(strUpgrade);
		if (sLength < dwLength) sLength = dwLength;
		sLine++;
	}

	// 각성 레벨 표시
	if(lpItem->GetSeasonRecord() >= 3 && lpItem->GetCoreLevel() > 0)
	{
		char strUpgrade[MAX_PATH];
		sprintf(strUpgrade, CRYLStringTable::m_strString[4302], lpItem->GetUpgradeLevel());
		dwLength = g_TextEdit.GetStringLength(strUpgrade);
		if (sLength < dwLength) sLength = dwLength;
		sLine++;
	}

	if (lpItem->GetMaxSocketNum() >= 4)
	{
		dwLength = 32 * 4;
	} else
	{
		dwLength = 32 * lpItem->GetMaxSocketNum();
	}
	if (sLength < dwLength) sLength = dwLength;

	if (sPosX + (sLength + 10) > g_ClientMain.m_BaseGraphicLayer.m_lScreenSx)
	{
		sPosX = g_ClientMain.m_BaseGraphicLayer.m_lScreenSx - (sLength + 10);
	}

	long lSocketSize = 0;
	if (0 < lpItem->GetMaxSocketNum() && lpItem->GetMaxSocketNum() <= 4)
		lSocketSize = 32;
	else if (4 < lpItem->GetMaxSocketNum() && lpItem->GetMaxSocketNum() <= 8)
		lSocketSize = 64;

	if( lpItem->GetMaxRuneSocket() > 0 )
	{
		lSocketSize += 32;
	}

	if ( (runeID = lpItem->GetRuneSocket( 0 ) ) )
	{
		const unsigned short*			attribute1 = NULL;
		const unsigned short*			attribute2 = NULL;
		unsigned long runeID2 = 0;
		const Item::ItemInfo *lpPT;
		lpPT = g_ItemMgr.GetItemInfo( runeID );
		attribute1 = lpPT->m_EquipAttribute.m_usAttributeValue;

		if( ( runeID2 = lpItem->GetRuneSocket( 1 ) ) )
		{
			const Item::ItemInfo *lpPT2;
			lpPT2 = g_ItemMgr.GetItemInfo( runeID2 );
			attribute2 = lpPT2->m_EquipAttribute.m_usAttributeValue;
		}

		unsigned long dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);   /////////////////////////////////

		unsigned short usValue = 0;

		for(int nIndex = 0; nIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++nIndex)
		{

			if( attribute2 )
				usValue = attribute1[ Item::Attribute::Type(nIndex)] + attribute2[ Item::Attribute::Type(nIndex)];
			else
				usValue = attribute1[ Item::Attribute::Type(nIndex)] ;

			if ( usValue )
			{
				switch (nIndex)
				{
				case Item::Attribute::MIN_DAMAGE:	strcpy(strText, CRYLStringTable::m_strString[3555]);		break;
				case Item::Attribute::MAX_DAMAGE:	strcpy(strText, CRYLStringTable::m_strString[3556]);		break;
				case Item::Attribute::ARMOR:		strcpy(strText, CRYLStringTable::m_strString[48]);		break;
				case Item::Attribute::HIT_RATE:		strcpy(strText, CRYLStringTable::m_strString[50]);		break;
				case Item::Attribute::EVADE:		strcpy(strText, CRYLStringTable::m_strString[51]);		break;
				case Item::Attribute::CRITICAL:		strcpy(strText, CRYLStringTable::m_strString[49]);		break;
				case Item::Attribute::BLOCK:		strcpy(strText, CRYLStringTable::m_strString[53]);		break;
				case Item::Attribute::MAGIC_POWER:	strcpy(strText, CRYLStringTable::m_strString[59]);		break;
				case Item::Attribute::MAGIC_RESIST:	strcpy(strText, CRYLStringTable::m_strString[52]);		break;
				case Item::Attribute::LUCKCHANCE:	strcpy(strText, CRYLStringTable::m_strString[239]);		break;
				case Item::Attribute::COOLDOWN:		strcpy(strText, CRYLStringTable::m_strString[3549]);	break;
				case Item::Attribute::MAX_HP:		strcpy(strText, CRYLStringTable::m_strString[54]);		break;
				case Item::Attribute::HP_REGEN:		strcpy(strText, CRYLStringTable::m_strString[56]);		break;
				case Item::Attribute::MAX_MP:		strcpy(strText, CRYLStringTable::m_strString[55]);		break;
				case Item::Attribute::MP_REGEN:		strcpy(strText, CRYLStringTable::m_strString[57]);		break;
				case Item::Attribute::SPEED:		strcpy(strText, CRYLStringTable::m_strString[58]);		break;
				case Item::Attribute::SKILL_POINT:	strcpy(strText, CRYLStringTable::m_strString[3550]);	break;

				case Item::Attribute::FROST:		strcpy(strText, CRYLStringTable::m_strString[3551]);	break;
				case Item::Attribute::FIRE:			strcpy(strText, CRYLStringTable::m_strString[3552]);	break;
				case Item::Attribute::ELECTRO:		strcpy(strText, CRYLStringTable::m_strString[3553]);	break;
				case Item::Attribute::DARKNESS:		strcpy(strText, CRYLStringTable::m_strString[5200]), dwColor = D3DCOLOR_RGBA(238, 168, 16, 255);	break;
				// TEST ALEX
		  //      case Item::Attribute::ADD_STR:		strcpy(strText, CRYLStringTable::m_strString[3554]);	break;
				default:	continue;
				}

				char strTemp[100], a[10];
				strcpy(strTemp, strText);
				if (nIndex == Item::Attribute::MAX_HP || nIndex == Item::Attribute::MAX_MP)
				{
					sprintf(a, "%d", usValue * 10 );
				} 
				else
				{
					sprintf(a, "%d", usValue );
				}

				strcat(strTemp, a);
				dwLength = g_TextEdit.GetStringLength(strTemp) + 10;
				if (sLength < dwLength) sLength = dwLength;
				sLine++;			
			}
		}
	}


	if (sPosY + ((sLine * 16 + 6) + lSocketSize) > g_ClientMain.m_BaseGraphicLayer.m_lScreenSy)
	{
		sPosY = g_ClientMain.m_BaseGraphicLayer.m_lScreenSy - ((sLine * 16 + 6) + lSocketSize);
	}
}

void CGUITooltipItem::GetBookTooltipPos(short &sPosX, short &sPosY, short &sLength, short &sLine)
{
	char			strText[100];
	long			dwLength;
	CRYLGameData*	pGame = CRYLGameData::Instance();

	Item::CUseItem *lpItem = Item::CUseItem::DowncastToUseItem(m_lpItem->m_lpItemBase);
	if (!lpItem) return;

	unsigned short wLockCount = lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount;
	const Skill::ProtoType *lpSkillBase = g_SkillMgr.GetSkillProtoType(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID), *lpSkill;

	if (lpSkillBase)
	{
		lpSkill = &lpSkillBase[wLockCount];

		sprintf(strText, CRYLStringTable::m_strString[95], lpSkill->m_SpriteInfo.m_szName, lpSkillBase->m_SpriteInfo.m_szName, wLockCount + 1);
		sLength = dwLength = g_TextEdit.GetStringLength(strText);
		sLine = 1;

		++sLine;

		if (pGame->m_lpInterface->GetIsTrade())
		{
			if (m_lpItem->m_bTradeItem)
			{
				if (m_lpItem->m_bMedalBuyItem)
				{
					sprintf(strText, CRYLStringTable::m_strString[862], g_TextEdit.MakePrintGold(lpItem->GetBuyMedalPrice()));
				} 
				else
				{
					unsigned long dwBuyPrice = m_lpItem->m_lpItemBase->GetBuyPrice();

					NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
					if ( pNpcNode && pNpcNode->m_bBelongToCastle )
					{
						CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
						if ( lpCastle )
						{
							dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
						}
					}

					sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(dwBuyPrice));
				}
			} else
				sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(m_lpItem->m_lpItemBase->GetSellPrice()));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
			sLine++;
		}

		if(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID >= 0x8000)
		{
			SKILLSLOT *lpSkillSlot = pGame->m_csStatus.GetSkillSlot(lpItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID);
			if (lpSkillSlot && wLockCount < lpSkillSlot->SKILLINFO.cLockCount)
			{
				strcpy(strText, CRYLStringTable::m_strString[93]);
				dwLength = g_TextEdit.GetStringLength(strText);
				if (sLength < dwLength) sLength = dwLength;
				sLine++;
			} 
			else if (lpSkillSlot && wLockCount == lpSkillSlot->SKILLINFO.cLockCount && lpSkillSlot->SKILLINFO.cSkillLevel == 6)
			{
				strcpy(strText, CRYLStringTable::m_strString[94]);
				dwLength = g_TextEdit.GetStringLength(strText);
				if (sLength < dwLength) sLength = dwLength;
				sLine++;
			} 
			else
			{
				for (int i = 0; i < Skill::ProtoType::MAX_LIMIT_NUM; i++)
				{
					if (lpSkill->m_StatusLimitType[i])
					{
						unsigned long dwValue;
						if (lpSkillSlot)
						{
							if (wLockCount == lpSkillSlot->SKILLINFO.cLockCount)
							{
								dwValue = (wLockCount * CSkillMgr::MAX_SKILL_LEVEL + lpSkillSlot->SKILLINFO.cSkillLevel) * 
									lpSkill->m_StatusLimitValue[i];
							} 
							else
							{
								dwValue = (wLockCount * CSkillMgr::MAX_SKILL_LEVEL) * lpSkill->m_StatusLimitValue[i];
							}
						} 
						else
						{
							dwValue = (wLockCount * CSkillMgr::MAX_SKILL_LEVEL) * lpSkill->m_StatusLimitValue[i];
						}

						switch (lpSkill->m_StatusLimitType[i])
						{
							case Skill::StatusLimit::STR:
								sprintf(strText, CRYLStringTable::m_strString[37], dwValue + 20);
								break;

							case Skill::StatusLimit::DEX:
								sprintf(strText, CRYLStringTable::m_strString[38], dwValue + 20);
								break;

							case Skill::StatusLimit::CON:
								sprintf(strText, CRYLStringTable::m_strString[39], dwValue + 20);
								break;

							case Skill::StatusLimit::INT:
								sprintf(strText, CRYLStringTable::m_strString[40], dwValue + 20);
								break;

							case Skill::StatusLimit::WIS:
								sprintf(strText, CRYLStringTable::m_strString[41], dwValue + 20);
								break;

							case Skill::StatusLimit::LEVEL:
								sprintf(strText, CRYLStringTable::m_strString[3557], lpSkill->m_StatusLimitValue[i]);
								break;
						}
						dwLength = g_TextEdit.GetStringLength(strText);
						if (sLength < dwLength) sLength = dwLength;
						sLine++;
					}
				}
			}
			
			unsigned char cClass[6]		= { CClass::Fighter, CClass::Rogue, CClass::Mage, CClass::Acolyte,  CClass::Combatant,  CClass::Officiator };
			unsigned char cSubClass[14]	= { CClass::Defender, CClass::Warrior, CClass::Assassin, CClass::Archer,  CClass::Sorcerer,  CClass::Enchanter,
				CClass::Priest, CClass::Cleric, CClass::Templar, CClass::Attacker, CClass::Gunner, CClass::RuneOff, 
				CClass::LifeOff, CClass::ShadowOff };

			unsigned long dwClassLimit = lpItem->GetItemInfo().m_UseLimit.m_dwClassLimit;
			unsigned long dwCnt	= 0;

			strcpy(strText, CRYLStringTable::m_strString[98]);

			// 휴먼 체크.		
			for(unsigned char cIndex = 0; cIndex < 4; cIndex++)
			{
				if(ClassCheck(dwClassLimit, cClass[cIndex]))
				{		
					if(dwCnt)
					{
						strcat(strText, ", ");
					}

					strcat(strText, CRYLStringTable::m_strString[451+cIndex]);
					dwCnt++;
				}
				else
				{
					for(unsigned char cSubIndex = 0; cSubIndex < 2; cSubIndex++)
					{
						if(ClassCheck(dwClassLimit, cSubClass[cIndex*2+cSubIndex]))
						{
							if(dwCnt)
							{
								strcat(strText, ", ");
							}

							strcat(strText, CRYLStringTable::m_strString[455+cIndex*2+cSubIndex]);
							dwCnt++;
						}
					}
				}
			}

			// 아칸 체크.
			for(unsigned char cIndex = 4; cIndex < 6; cIndex++)
			{
				if(ClassCheck(dwClassLimit, cClass[cIndex]))
				{		
					if(dwCnt)
					{
						strcat(strText, ", ");
					}

					strcat(strText, CRYLStringTable::m_strString[467+(cIndex-4)]);
					dwCnt++;
				}
				else
				{
					for(unsigned char cSubIndex = 0; cSubIndex < 3; cSubIndex++)
					{
						if(ClassCheck(dwClassLimit, cSubClass[8+(cIndex-4)*3+cSubIndex]))
						{
							if(dwCnt)
							{
								strcat(strText, ", ");
							}

							strcat(strText, CRYLStringTable::m_strString[469+(cIndex-4)*3+cSubIndex]);
							dwCnt++;
						}
					}
				}
			}
		
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
			sLine++;

			switch (lpSkill->m_eSkillType)
			{
				case Skill::Type::PASSIVE:				// 책에 써 있는 스킬의 종류
					strcpy(strText, CRYLStringTable::m_strString[75]);
					break;

				case Skill::Type::INSTANCE:
					strcpy(strText, CRYLStringTable::m_strString[76]);
					break;

				case Skill::Type::CAST:
					strcpy(strText, CRYLStringTable::m_strString[77]);
					break;

				case Skill::Type::CHANT:
					strcpy(strText, CRYLStringTable::m_strString[78]);
					break;

				case Skill::Type::GATHER:
					strcpy(strText, CRYLStringTable::m_strString[76]);
					break;
			}

			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
			sLine++;

			switch (lpSkill->m_eTargetType)
			{
				case Skill::Target::DEAD_ENEMY:
					strcpy(strText, CRYLStringTable::m_strString[81]);
					break;

				case Skill::Target::DEAD_FRIEND:
					strcpy(strText, CRYLStringTable::m_strString[82]);
					break;

				case Skill::Target::ENEMY:
					strcpy(strText, CRYLStringTable::m_strString[83]);
					break;

				case Skill::Target::ENEMY_OBJECT:
					strcpy(strText, CRYLStringTable::m_strString[84]);
					break;

				case Skill::Target::FRIEND:
					strcpy(strText, CRYLStringTable::m_strString[85]);
					break;

				case Skill::Target::FRIEND_EXCEPT_SELF:
					strcpy(strText, CRYLStringTable::m_strString[86]);
					break;

				case Skill::Target::FRIEND_OBJECT:
					strcpy(strText, CRYLStringTable::m_strString[87]);
					break;

				case Skill::Target::LINE_ENEMY:
					strcpy(strText, CRYLStringTable::m_strString[88]);
					break;

				case Skill::Target::MELEE:
					strcpy(strText, CRYLStringTable::m_strString[89]);
					break;

				case Skill::Target::PARTY:
					strcpy(strText, CRYLStringTable::m_strString[90]);
					break;

				case Skill::Target::SUMMON:
					strcpy(strText, CRYLStringTable::m_strString[863]);
					break;

				default:
					strcpy(strText, "");
					break;
			}
			if (strcmp(strText, ""))
			{
				dwLength = g_TextEdit.GetStringLength(strText);
				if (sLength < dwLength) sLength = dwLength;
				sLine++;
			}

			if (lpSkill->m_fMaxRange)
			{
				sprintf(strText, CRYLStringTable::m_strString[91], lpSkill->m_fMaxRange);
				dwLength = g_TextEdit.GetStringLength(strText);
				if (sLength < dwLength) sLength = dwLength;
				sLine++;
			}

			if (lpSkill->m_fEffectExtent)
			{
				sprintf(strText, CRYLStringTable::m_strString[92], lpSkill->m_fEffectExtent);
			} else
			{
				strcpy(strText, CRYLStringTable::m_strString[99]);
			}
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
			sLine++;
		}
		else 
		{
			sprintf(strText, CRYLStringTable::m_strString[4325], (int)(lpSkill->m_fMinRange));
			dwLength = g_TextEdit.GetStringLength(strText);
			if (sLength < dwLength) sLength = dwLength;
			sLine++;
		}

		sLine++;

		if (strcmp(m_lpItem->GetItemInfo().GetItemDescribe(), ""))
		{
			if( g_ClientMain.m_iLanguage == 2)
			{
				WCHAR*	strWideString = NULL;
				WCHAR	seps[] = L"\\" ;
				WCHAR*	token;
				char	strBreak[1024];
				char*	strMessageTemp = NULL ;

				strcpy(strBreak, m_lpItem->GetItemInfo().GetItemDescribe());

				int nLen = MultiByteToWideChar( CP_ACP, 0, strBreak, lstrlen(strBreak), NULL, NULL);
				strWideString = SysAllocStringLen(NULL, nLen);
				MultiByteToWideChar( CP_ACP, 0, strBreak, strlen(strBreak), strWideString, nLen ) ;

				token = wcstok( strWideString, seps ) ;

				long lLength;
				while( token != NULL )
				{
					nLen = wcslen( token ) ;

					WideCharToMultiByte(CP_ACP, 0, token, nLen + 1, strBreak, 1024, NULL, NULL);

					lLength = (long)g_TextEdit.GetStringLength(strBreak);
					if (sLength < lLength) sLength = lLength;
									
					token = wcstok(NULL, seps);
					sLine++;
				}

				SysFreeString(strWideString);
			}
			else
			{
				char strText[MAX_PATH], *token, spes[] = "\\";
				strcpy(strText, m_lpItem->GetItemInfo().GetItemDescribe());
				token = strtok(strText, spes);
				long lLength;
				while(token != NULL)
				{
					lLength = (long)g_TextEdit.GetStringLength(token);
					if (sLength < lLength) sLength = lLength;
					token = strtok(NULL, spes);
					sLine++;
				}
			}
		}
	}

	if (sPosX + (sLength + 10) > g_ClientMain.m_BaseGraphicLayer.m_lScreenSx)
	{
		sPosX = g_ClientMain.m_BaseGraphicLayer.m_lScreenSx - (sLength + 10);
	}
	if (sPosY + (sLine * 16 + 6) > g_ClientMain.m_BaseGraphicLayer.m_lScreenSy)
	{
		sPosY = g_ClientMain.m_BaseGraphicLayer.m_lScreenSy - (sLine * 16 + 6);
	}
}

void CGUITooltipItem::GetEtcTooltipPos(short &sPosX, short &sPosY, short &sLength, short &sLine)
{
	char			strText[100];
	long			dwLength;
	CRYLGameData*	pGame = CRYLGameData::Instance();

	sLength = dwLength = g_TextEdit.GetStringLength(m_lpItem->GetItemInfo().m_SpriteData.m_szName);
	sLine = 1;

	dwLength = g_TextEdit.GetStringLength(m_lpItem->m_lpItemBase->GetItemTypeName());
	if (sLength < dwLength) sLength = dwLength;
	sLine++;

	unsigned long dwClassLimit = m_lpItem->GetItemInfo().m_UseLimit.m_dwClassLimit;

	if( dwClassLimit != 0)
	{
		unsigned char cClass[6]		= { CClass::Fighter, CClass::Rogue, CClass::Mage, CClass::Acolyte,  CClass::Combatant,  CClass::Officiator };
		unsigned char cSubClass[14]	= { CClass::Defender, CClass::Warrior, CClass::Assassin, CClass::Archer,  CClass::Sorcerer,  CClass::Enchanter,
			CClass::Priest, CClass::Cleric, CClass::Templar, CClass::Attacker, CClass::Gunner, CClass::RuneOff, 
			CClass::LifeOff, CClass::ShadowOff };

		unsigned long dwClassLimit = m_lpItem->GetItemInfo().m_UseLimit.m_dwClassLimit;
		unsigned long dwCnt	= 0;

		strcpy(strText, CRYLStringTable::m_strString[98]);

		// 휴먼 체크.		
		for(unsigned char cIndex = 0; cIndex < 4; cIndex++)
		{
			if(ClassCheck(dwClassLimit, cClass[cIndex]))
			{		
				if(dwCnt)
				{
					strcat(strText, ", ");
				}

				strcat(strText, CRYLStringTable::m_strString[451+cIndex]);
				dwCnt++;
			}
			else
			{
				for(unsigned char cSubIndex = 0; cSubIndex < 2; cSubIndex++)
				{
					if(ClassCheck(dwClassLimit, cSubClass[cIndex*2+cSubIndex]))
					{
						if(dwCnt)
						{
							strcat(strText, ", ");
						}

						strcat(strText, CRYLStringTable::m_strString[455+cIndex*2+cSubIndex]);
						dwCnt++;
					}
				}
			}
		}

		// 아칸 체크.
		for(unsigned char cIndex = 4; cIndex < 6; cIndex++)
		{
			if(ClassCheck(dwClassLimit, cClass[cIndex]))
			{		
				if(dwCnt)
				{
					strcat(strText, ", ");
				}

				strcat(strText, CRYLStringTable::m_strString[467+(cIndex-4)]);
				dwCnt++;
			}
			else
			{
				for(unsigned char cSubIndex = 0; cSubIndex < 3; cSubIndex++)
				{
					if(ClassCheck(dwClassLimit, cSubClass[8+(cIndex-4)*3+cSubIndex]))
					{
						if(dwCnt)
						{
							strcat(strText, ", ");
						}

						strcat(strText, CRYLStringTable::m_strString[469+(cIndex-4)*3+cSubIndex]);
						dwCnt++;
					}
				}
			}
		}
		
		dwLength = g_TextEdit.GetStringLength(strText);
		if (sLength < dwLength) sLength = dwLength;
		sLine++;
	}

	if (pGame->m_lpInterface->GetIsTrade())
	{
		if (m_lpItem->m_bTradeItem)
		{
			if (m_lpItem->m_bMedalBuyItem)
			{
				sprintf(strText, CRYLStringTable::m_strString[862], g_TextEdit.MakePrintGold(m_lpItem->m_lpItemBase->GetBuyMedalPrice()));
			} 
			else
			{
				unsigned long dwBuyPrice = m_lpItem->m_lpItemBase->GetBuyPrice();

				NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
				if ( pNpcNode && pNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
					if ( lpCastle )
					{
						dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
					}
				}

				sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(dwBuyPrice));
			}
		} 
		else
		{
			sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(m_lpItem->m_lpItemBase->GetSellPrice()));
		}

		dwLength = g_TextEdit.GetStringLength(strText);
		if (sLength < dwLength) sLength = dwLength;
		sLine++;
	}

	if (strcmp(m_lpItem->GetItemInfo().GetItemDescribe(), ""))
	{
		if( g_ClientMain.m_iLanguage == 2)
		{
			WCHAR*	strWideString = NULL;
			WCHAR	seps[] = L"\\" ;
			WCHAR*	token;
			char	strBreak[1024];
			char*	strMessageTemp = NULL ;

			strcpy(strBreak, m_lpItem->GetItemInfo().GetItemDescribe());

			int nLen = MultiByteToWideChar( CP_ACP, 0, strBreak, lstrlen(strBreak), NULL, NULL);
			strWideString = SysAllocStringLen(NULL, nLen);
			MultiByteToWideChar( CP_ACP, 0, strBreak, strlen(strBreak), strWideString, nLen ) ;

			token = wcstok( strWideString, seps ) ;

			long lLength;
			while( token != NULL )
			{
				nLen = wcslen( token ) ;

				WideCharToMultiByte(CP_ACP, 0, token, nLen + 1, strBreak, 1024, NULL, NULL);

				lLength = (long)g_TextEdit.GetStringLength(strBreak);
				if (sLength < lLength) sLength = lLength;
								
				token = wcstok(NULL, seps);
				sLine++;
			}

			SysFreeString(strWideString);
		}
		else
		{
			char strText[MAX_PATH], *token, spes[] = "\\";
			strcpy(strText, m_lpItem->GetItemInfo().GetItemDescribe());
			token = strtok(strText, spes);
			long lLength;
			while(token != NULL)
			{
				lLength = (long)g_TextEdit.GetStringLength(token);
				if (sLength < lLength) sLength = lLength;
				token = strtok(NULL, spes);
				sLine++;
			}
		}
	}

	if (sPosX + (sLength + 10) > g_ClientMain.m_BaseGraphicLayer.m_lScreenSx)
	{
		sPosX = g_ClientMain.m_BaseGraphicLayer.m_lScreenSx - (sLength + 10);
	}
	if (sPosY + (sLine * 16 + 6) > g_ClientMain.m_BaseGraphicLayer.m_lScreenSy)
	{
		sPosY = g_ClientMain.m_BaseGraphicLayer.m_lScreenSy - (sLine * 16 + 6);
	}
}

void CGUITooltipItem::RenderEtc(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
	short			SizeX = ptMousePos->x + 7;
	short			SizeY = ptMousePos->y + 11;
	short			sLength, sLine;
	
	GetEtcTooltipPos(SizeX, SizeY, sLength, sLine);
	
	RECT rt;
	::SetRect( &rt, SizeX, SizeY, SizeX + sLength + 20, SizeY + sLine * 16 + 26 ) ;
	
	if ( m_pTooltipImg && m_pTooltipList )
	{
        // 이미지 높이나 너비가 바뀌었으면 다시 삭제후 다시 Create, 아니면 그냥 Render

        if (rt.right - rt.left == m_pTooltipList->GetWidth() &&
            rt.bottom - rt.top == m_pTooltipList->GetHeight())
        {
            RECT drawrt = rt;
            drawrt.left -= 10 ;
		    drawrt.top  -= 20 ;
		    
            m_pTooltipImg->DoDraw( NULL, drawrt, 0 ) ;
		    MakeEtcDesc( drawrt.right - drawrt.left ) ;
		    m_pTooltipList->DrawItems( drawrt.left - 10, drawrt.top + 3 ) ;
        }
        else
        {            
		    CreateTooltip(rt);
        }
	}
	else
	{        
		CreateTooltip(rt);
	}
}

void CGUITooltipItem::MakeEtcDesc( unsigned short wWidth ) 
{
	CRYLGameData*	pGame = CRYLGameData::Instance();
	char strText[MAX_PATH];

	m_pTooltipList->ClearItem() ;

	// -------------------------------------------------------------------------------
	// 아이템 명

	m_pTooltipList->AddItem( 0, const_cast<char*>(m_lpItem->GetItemInfo().m_SpriteData.m_szName), wWidth, 0xffffffff ) ;

	// -------------------------------------------------------------------------------
	// 아이템 분류

	m_pTooltipList->AddItem( 0, const_cast<char*>(m_lpItem->m_lpItemBase->GetItemTypeName()), wWidth, 0xffffffff ) ;


	// -------------------------------------------------------------------------------
	// 사용제한.
	unsigned long dwClassLimit = m_lpItem->GetItemInfo().m_UseLimit.m_dwClassLimit;
	unsigned long dwCnt	= 0;

	if( dwClassLimit != 0)
	{
		unsigned char cClass[6]		= { CClass::Fighter, CClass::Rogue, CClass::Mage, CClass::Acolyte,  CClass::Combatant,  CClass::Officiator };
		unsigned char cSubClass[14]	= { CClass::Defender, CClass::Warrior, CClass::Assassin, CClass::Archer,  CClass::Sorcerer,  CClass::Enchanter,
										CClass::Priest, CClass::Cleric, CClass::Templar, CClass::Attacker, CClass::Gunner, CClass::RuneOff, 
										CClass::LifeOff, CClass::ShadowOff };

		strcpy(strText, CRYLStringTable::m_strString[98]);
		
		// 휴먼 체크.
		for(unsigned char cIndex = 0; cIndex < 4; cIndex++)
		{
			if(ClassCheck(dwClassLimit, cClass[cIndex]))
			{		
				if(dwCnt)
				{
					strcat(strText, ", ");
				}

				strcat(strText, CRYLStringTable::m_strString[451+cIndex]);
				dwCnt++;
			}
			else
			{
				for(unsigned char cSubIndex = 0; cSubIndex < 2; cSubIndex++)
				{
					if(ClassCheck(dwClassLimit, cSubClass[cIndex*2+cSubIndex]))
					{
						if(dwCnt)
						{
							strcat(strText, ", ");
						}

						strcat(strText, CRYLStringTable::m_strString[455+cIndex*2+cSubIndex]);
						dwCnt++;
					}
				}
			}
		}

		// 아칸 체크.
		for(unsigned char cIndex = 4; cIndex < 6; cIndex++)
		{
			if(ClassCheck(dwClassLimit, cClass[cIndex]))
			{		
				if(dwCnt)
				{
					strcat(strText, ", ");
				}

				strcat(strText, CRYLStringTable::m_strString[467+(cIndex-4)]);
				dwCnt++;
			}
			else
			{
				for(unsigned char cSubIndex = 0; cSubIndex < 3; cSubIndex++)
				{
					if(ClassCheck(dwClassLimit, cSubClass[8+(cIndex-4)*3+cSubIndex]))
					{
						if(dwCnt)
						{
							strcat(strText, ", ");
						}

						strcat(strText, CRYLStringTable::m_strString[469+(cIndex-4)*3+cSubIndex]);
						dwCnt++;
					}
				}
			}
		}

		if(dwCnt)
		{
			enum ClassLimitConst
			{
				MAX_LINE_NUM	= 3,	// 클래스 제한 표기 최대 라인수
				MAX_INDEX_NUM	= 3		// 한 줄당 클래스 제한 표기 최대 갯수
			};

			bool bCanClass = false;
			for (int i = 1; i < CClass::MAX_CLASS; i++)
			{
				if (m_lpItem->GetItemInfo().m_UseLimit.m_dwClassLimit & (0x00000001 << (i - 1)))
				{
					if (pGame->m_csStatus.m_Info.Class == i) 
					{
						bCanClass = true;
					}
				}
			}

			unsigned long dwColor = 
				(true == bCanClass) ? D3DCOLOR_RGBA(248, 200, 112, 255) : D3DCOLOR_RGBA(255, 0, 0, 255);

			m_pTooltipList->AddItem( 0, strText, wWidth, dwColor ) ;		
		}
	}

	// -------------------------------------------------------------------------------
	// 가격

	if (pGame->m_lpInterface->GetIsTrade())
	{
		DWORD dwColor = 0xFFFFFFFF;
		if (m_lpItem->m_bTradeItem)
		{
			if (m_lpItem->m_bMedalBuyItem)
			{
				sprintf(strText, CRYLStringTable::m_strString[862], g_TextEdit.MakePrintGold(m_lpItem->m_lpItemBase->GetBuyMedalPrice()));
			} 
			else
			{
				unsigned long dwBuyPrice = m_lpItem->m_lpItemBase->GetBuyPrice();

				NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
				if ( pNpcNode && pNpcNode->m_bBelongToCastle )
				{
					CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
					if ( lpCastle )
					{
						dwBuyPrice += static_cast<unsigned long>( dwBuyPrice * ( lpCastle->GetTax(Castle::TRADE_TAX) / 100.f ) );
					}
				}

				sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold(dwBuyPrice));

				if ( m_lpItem->m_bStoreItem )
				{
					// edith 2009.05.25 개인상점 아이템 툴팁 가격 색 설정
					// 스토어에서 파는 아이템
					if( dwBuyPrice < 100000)
						dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
					else if( dwBuyPrice < 1000000)
						dwColor = D3DCOLOR_RGBA(255, 255, 128, 255);
					else if( dwBuyPrice < 10000000 )
						dwColor = D3DCOLOR_RGBA(255, 255, 0, 255);
					else if( dwBuyPrice < 100000000 )
						dwColor = D3DCOLOR_RGBA(255, 192, 0, 255);
					else
						dwColor = D3DCOLOR_RGBA(255, 128, 0, 255);
				}
			}
		} 
		else
		{
			unsigned long dwSellPrice = m_lpItem->m_lpItemBase->GetSellPrice();
			sprintf(strText, CRYLStringTable::m_strString[35], g_TextEdit.MakePrintGold( dwSellPrice ));

			if ( m_lpItem->m_bStoreItem )
			{
				// edith 2009.05.25 개인상점 아이템 툴팁 가격 색 설정
				// 스토어에서 파는 아이템
				if( dwSellPrice < 100000)
					dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
				else if( dwSellPrice < 1000000)
					dwColor = D3DCOLOR_RGBA(255, 255, 128, 255);
				else if( dwSellPrice < 10000000 )
					dwColor = D3DCOLOR_RGBA(255, 255, 0, 255);
				else if( dwSellPrice < 100000000 )
					dwColor = D3DCOLOR_RGBA(255, 192, 0, 255);
				else
					dwColor = D3DCOLOR_RGBA(255, 128, 0, 255);
			}
		}
	

		m_pTooltipList->AddItem( 0, strText, wWidth, dwColor ) ;
	}

	// -------------------------------------------------------------------------------
	// 아이템 설명 전반

	if (strcmp(m_lpItem->GetItemInfo().GetItemDescribe(), ""))
	{
	//	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::JAPAN )
		if( g_ClientMain.m_iLanguage == 2)
		{
			WCHAR*	strWideString = NULL;
			WCHAR	seps[] = L"\\" ;
			WCHAR*	token;
			char	strBreak[1024];
			char*	strMessageTemp = NULL ;

			strcpy(strBreak, m_lpItem->GetItemInfo().GetItemDescribe());

			int nLen = MultiByteToWideChar( CP_ACP, 0, strBreak, lstrlen(strBreak), NULL, NULL);
			strWideString = SysAllocStringLen(NULL, nLen);
			MultiByteToWideChar( CP_ACP, 0, strBreak, strlen(strBreak), strWideString, nLen ) ;

			token = wcstok( strWideString, seps ) ;

			while( token != NULL )
			{
				nLen = wcslen( token ) ;

				WideCharToMultiByte(CP_ACP, 0, token, nLen + 1, strBreak, 1024, NULL, NULL);

				m_pTooltipList->AddItem( 0, strBreak, wWidth, 0xffffffff ) ;
								
				token = wcstok(NULL, seps);
			}

			SysFreeString(strWideString);
		}
		else
		{
			char strText[MAX_PATH], *token, spes[] = "\\";
			strcpy(strText, m_lpItem->GetItemInfo().GetItemDescribe());
			token = strtok(strText, spes);
			while(token != NULL)
			{
				m_pTooltipList->AddItem( 0, token, wWidth, 0xffffffff ) ;
				token = strtok(NULL, spes);
			}
		}
	}
}

bool CGUITooltipItem::ClassCheck(unsigned long dwClassType, unsigned char cClass)
{
	if((dwClassType & (0x00000001 << (cClass - 1)))) 
	{
		return true;
	}

	return false;
}
