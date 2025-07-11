#ifndef __RYL_STALLDLG_H__
#define __RYL_STALLDLG_H__

#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include "RYLDialog.h"

class CRYLEdit;
class CRYLListBox;

class CItemInstance;

class CRYLStallDlg : public CRYLDialog
{
protected:

	struct VisitorNode
	{
		unsigned long	m_dwCharID;
		char			m_szCharName[CHAR_INFOST::MAX_NAME_LEN];

		VisitorNode()
			:	m_dwCharID(0)
		{
			std::fill_n(m_szCharName, int(CHAR_INFOST::MAX_NAME_LEN), 0);
		}

		VisitorNode(unsigned long dwCharID, const char* szCharName)
			:	m_dwCharID(dwCharID)
		{
			std::copy(szCharName, szCharName + CHAR_INFOST::MAX_NAME_LEN, m_szCharName);
		}
	};

	CRYLEdit*		m_pChattingEdit;			// 채팅 박스
	CRYLListBox*	m_pChattingListBox;			// 채팅 리스트박스
	CRYLListBox*	m_pVisitorListBox;			// 입장객 리스트박스

	unsigned long	m_dwOwnerID;				// 상점 주인 아이디

	VisitorNode		m_aryVisitorList[PktStIInfo::MAX_CUSTOMER_NUM];		// 입장객 리스트


public :

	CRYLStallDlg();
	virtual ~CRYLStallDlg();

	virtual VOID Show(CGMUIObject* Sender) = 0;
	virtual VOID Hide(CGMUIObject* Sender) = 0;

	virtual VOID GMUIPaint(INT xx, INT yy) = 0;

	virtual	HRESULT		Render(LPDIRECT3DDEVICE8 lpD3DDevice) = 0;
	virtual HRESULT		FinalRender(LPDIRECT3DDEVICE8 lpD3DDevice) = 0;
	virtual HRESULT		Update(BOOL &bClick, BOOL &bEdge) = 0;

	virtual bool SendChat(char* szChatMessage) = 0;
	virtual void AddItemList(CItemInstance* lpItem, bool bSound);

	void AddVisitor(unsigned long dwVisitorID, const char* szVisitorName);
	void DeleteVisitor(unsigned long dwCharID);

	void AddChatMessage(char* lpdzMsg, unsigned long dwColor);

	unsigned long GetOwnerID(void)			{ return m_dwOwnerID;			}
	CRYLEdit* GetChattingEdit(void)			{ return m_pChattingEdit;		}
	CRYLListBox* GetChattingListBox(void)	{ return m_pChattingListBox;	}
};

#endif 