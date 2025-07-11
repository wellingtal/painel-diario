
#ifndef __RYL_INVENTORYPANEL_H__
#define __RYL_INVENTORYPANEL_H__

#pragma once

#include <Item/ItemConstants.h>
#include <Item/ItemStructure.h>

#include "GMUICustomPanel.h"

class CRYLInventoryPanel : public CGMUICustomPanel
{
public:

	CRYLInventoryPanel(CGMUICustomDlg *pParentDlg);
	virtual ~CRYLInventoryPanel(void);

	static bool UsePotion(CItemInstance* lpItem);
	static bool UsePortal(CItemInstance* lpItem);


protected:

	void RenderRectBGColor(LPDIRECT3DDEVICE8 lpD3DDevice, Item::EquipmentPos::Type eIndex, 
		int nLeft, int nTop, int nRight, int nBottom);

	bool IsSelectableFifthItem(unsigned short wSkillID);
	bool SetEquip(Item::EquipmentPos::Type eIndex);
	void SetInvenIndex(unsigned long dwIndex)		{ m_dwBackInven = dwIndex;	}

	bool UseItem(CItemInstance* lpItem);

	bool UseQuestItem(CItemInstance* lpItem);
	bool UseSkillBook(CItemInstance* lpItem);
	
	bool UseCashItem(CItemInstance* lpItem);
	bool UseLottery(CItemInstance* lpItem);
	bool UseFireCracker(CItemInstance* lpItem);

	bool UseCampKit(CItemInstance* lpItem);
	bool UseMineralKit(CItemInstance* lpItem);
	bool UseWorldWeaponKit(CItemInstance* lpItem, unsigned char cNation);
	bool UseStartKit(CItemInstance *lpItem);

	void MessageInstallRune(CItemInstance* lpExistItem, bool bCurrentEquip);
	void MessageDestructionRune(CItemInstance* lpExistItem);

	bool ProcessMessageBoxResult(void);
	bool ProcessItemOverlapEvent(__int64 ddItem, Item::ItemPos itemPos);

	unsigned long	m_dwBackInven;

	bool firstAbility;//cant learn skill after learn ability
	unsigned long	m_dwSplitResult;
	unsigned long	m_dwSplitValue; 
	unsigned long	m_dwRepairResult;
	unsigned long	m_dwRuneResult;
	unsigned long	m_dwRuneDestuctionResult;
	unsigned long	m_dwSocketResult;

	CItemInstance*	m_lpSplitItem;
	CItemInstance*	m_lpRepairItem;
	CItemInstance*	m_lpRuneExistItem;	
	CItemInstance*	m_lpSocketItem;	
};

#endif