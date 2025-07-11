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

	CRYLButton*		m_pChangeTaxButton;			// ���� ���� ��ư
	CRYLButton*		m_pRepairButton;			// �����ϱ� ��ư
	CRYLButton*		m_pRepairAllButton;			// ��� ���� ��ư

	CRYLLabel*		m_pCampShopLabel;			// ��� ��� ���� ��
	CRYLLabel*		m_pTaxLabel;				// ���� ��
	CRYLLabel*		m_pTaxValueLabel;			// ���� ��
	CRYLLabel*		m_pShopOwnerLabel;			// ���� ���� ��
	CRYLLabel*		m_pShopOwnerValueLabel;		// ���� ���� ��
	CRYLLabel*		m_pCurrentVisitor;			// ���� ���尴 ��

	unsigned long	m_dwSellResult;				// �Ǹ� Ȯ�� �޼��� �ڽ� ���

	unsigned long	m_dwChangeTaxResult;		// ���� ���� �޼��� �ڽ� ���
	unsigned long	m_dwChangeTaxValue;			// ���� ���� �޼��� �ڽ� �Է°�

	unsigned long	m_dwBuyResult;				// ���� Ȯ�� �޼��� �ڽ� ���
	unsigned long	m_dwBuyNum;					// ���Ž� ������ ����
	CItemInstance*	m_pBuyItem;					// �����ϰ��� �ϴ� ������

	unsigned long	m_dwRegisterResult;			// ��� Ȯ�� �޼��� �ڽ� ���
	unsigned long	m_dwRegisterPrice;			// ��Ͻ� �������� ���� ����
	unsigned char	m_cRegisterIndex;			// �������� ����ϰ��� �ϴ� ��ġ

	unsigned char	m_cTax;						// ����
	unsigned long	m_dwCampID;					// ��� ��� ���̵�

	bool			m_bAdminMode;				// ���� ��� (������ ���/���, ���� ���� ����)

	list<CItemInstance *>	m_lstItem;												// �Ǹ����� ������ ����Ʈ
	__int64					m_aryItemIndex[CONTAINER_HEIGHT][CONTAINER_WIDTH];		// �Ǹ����� �������� �ε��� �迭


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