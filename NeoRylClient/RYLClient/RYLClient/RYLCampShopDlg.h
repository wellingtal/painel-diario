#ifndef __RYL_CAMPSHOPDLG_H__
#define __RYL_CAMPSHOPDLG_H__

#pragma once

#include "RYLLabel.h"
#include "RYLStallDlg.h"

class CRYLButton;

class CRYLCampShopDlg : public CRYLStallDlg
{
private:

	enum Const
	{
		CONTAINER_WIDTH		= 10,
		CONTAINER_HEIGHT	= 8
	};

	CRYLButton*		m_pChangeTaxButton;			// 세율 변경 버튼
	CRYLButton*		m_pRepairButton;			// 수리하기 버튼
	CRYLButton*		m_pRepairAllButton;			// 모두 수리 버튼

	CRYLLabel*		m_pCampShopLabel;			// 길드 요새 상점 라벨
	CRYLLabel*		m_pTaxLabel;				// 세율 라벨
	CRYLLabel*		m_pTaxValueLabel;			// 세율 값
	CRYLLabel*		m_pShopOwnerLabel;			// 상점 주인 라벨
	CRYLLabel*		m_pShopOwnerValueLabel;		// 상점 주인 값
	CRYLLabel*		m_pCurrentVisitor;			// 현재 입장객 라벨

	unsigned long	m_dwSellResult;				// 판매 확인 메세지 박스 결과

	unsigned long	m_dwChangeTaxResult;		// 세율 변경 메세지 박스 결과
	unsigned long	m_dwChangeTaxValue;			// 세율 변경 메세지 박스 입력값

	unsigned long	m_dwBuyResult;				// 구매 확인 메세지 박스 결과
	unsigned long	m_dwBuyNum;					// 구매시 아이템 갯수
	CItemInstance*	m_pBuyItem;					// 구매하고자 하는 아이템

	unsigned long	m_dwRegisterResult;			// 등록 확인 메세지 박스 결과
	unsigned long	m_dwRegisterPrice;			// 등록시 아이템의 개당 가격
	unsigned char	m_cRegisterIndex;			// 아이템을 등록하고자 하는 위치

	unsigned char	m_cTax;						// 세율
	unsigned long	m_dwCampID;					// 길드 요새 아이디

	bool			m_bAdminMode;				// 관리 모드 (아이템 등록/취소, 세율 변경 가능)

	list<CItemInstance *>	m_lstItem;												// 판매중인 아이템 리스트
	__int64					m_aryItemIndex[CONTAINER_HEIGHT][CONTAINER_WIDTH];		// 판매중인 아이템의 인덱스 배열


public:

	enum ControlIndex
	{
		CHATTING_EDIT		= 0,
		CHANGETAX_BUTTON	= 1,
		REPAIR_BUTTON		= 2,
		REPAIRALL_BUTTON	= 3
	};

	CRYLCampShopDlg();
	virtual ~CRYLCampShopDlg();

	virtual VOID Show(CGMUIObject* Sender);
	virtual VOID Hide(CGMUIObject* Sender);

	virtual VOID GMUIPaint(INT xx, INT yy);

	virtual HRESULT Update(BOOL &bClick, BOOL &bEdge);
	virtual HRESULT	Render(LPDIRECT3DDEVICE8 lpD3DDevice)		{ return S_OK;	}
	virtual HRESULT	FinalRender(LPDIRECT3DDEVICE8 lpD3DDevice)	{ return S_OK;	}

	virtual bool SendChat(char* szChatMessage);
	virtual void AddItemList(CItemInstance* lpItem, bool bSound);

	static VOID	DlgClick(CGMUIObject* Sender, INT X, INT Y);
	static VOID DlgKeyDown(CGMUIObject* Sender, WORD key, INT Shift);

	const char* GetOwnerName(void)			{ return m_pShopOwnerValueLabel->GetCaption();	}

	inline unsigned char GetTax(void);
	void ChangeTaxMessageBox(void);
	void ChangeTax(unsigned char cTax);

	void SetCamp(unsigned long dwCampID)	{ m_dwCampID = dwCampID;		}
	void SetAdminMode(bool bAdminMode)		{ m_bAdminMode = bAdminMode;	}
	bool IsAdminMode(void)					{ return m_bAdminMode;			}

	bool Enter(unsigned long dwOwnerID);

	CItemInstance* GetItem(Item::ItemPos& srcPos);
	bool RemoveItem(CItemInstance* lpItem);


private:

	void InitCtrl(void);
	void FinalCtrl(void);

	bool ProcessMessageBoxResult(void);
	void ProcessSell(void);
	bool ProcessBuy(CItemInstance* lpItem);
	void ProcessRegister(void);
	void ProcessCancel(CItemInstance* lpItem);

	bool CheckItemIndex(CItemInstance *lpItem, unsigned char cX, unsigned char cY);
};

unsigned char CRYLCampShopDlg::GetTax(void)
{
	if (TRUE == GetVisible() && false == m_bAdminMode)
	{
		return m_cTax;
	}

	return 0;
}

#endif