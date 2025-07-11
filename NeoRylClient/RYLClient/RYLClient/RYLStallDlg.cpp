
#include "ItemInstance.h"
#include "SoundMgr.h"

#include "RYLClientMain.h"

#include "RYLStallDlg.h"

#include "RYLListBox.h"
#include "GMMemory.h"

CRYLStallDlg::CRYLStallDlg()
:	m_pChattingEdit(NULL), m_pChattingListBox(NULL), m_pVisitorListBox(NULL), 
	m_dwOwnerID(0)
{
}

CRYLStallDlg::~CRYLStallDlg()
{
}

void CRYLStallDlg::AddItemList(CItemInstance* lpItem, bool bSound)
{
	if (true == bSound)
	{
		if (strcmp(lpItem->m_lpItemBase->GetItemInfo().m_StringData.m_szEffectSoundName, "NONE"))
		{
			char szTemp[MAX_PATH]; 
			sprintf(szTemp, "%s\\Sound\\Item\\%s", 
				g_ClientMain.m_strClientPath, lpItem->m_lpItemBase->GetItemInfo().m_StringData.m_szEffectSoundName);

			CSound* pSound; 
			CSoundMgr::_GetInstance()->GetSound(pSound, szTemp); 
			pSound->Play(); 
		}
	}
}

void CRYLStallDlg::AddVisitor(unsigned long dwVisitorID, const char* szVisitorName)
{
	VisitorNode node(dwVisitorID, szVisitorName);

	int nIndex = 0;
	for (; nIndex < PktStIInfo::MAX_CUSTOMER_NUM; ++nIndex)
	{
		if (dwVisitorID == m_aryVisitorList[nIndex].m_dwCharID)
		{
			return;
		}

		if (0 == m_aryVisitorList[nIndex].m_dwCharID)
		{
			break;
		}
	}

	if (nIndex != PktStIInfo::MAX_CUSTOMER_NUM)
	{
		m_aryVisitorList[nIndex] = node;
		m_pVisitorListBox->AddItem((LPSTR)node.m_szCharName, 0xff64ff64, 0xff64ff64, 0xffffffff);
	}
}

void CRYLStallDlg::DeleteVisitor(unsigned long dwCharID) 
{
	for (int nIndex = 0; nIndex < PktStIInfo::MAX_CUSTOMER_NUM; ++nIndex)
	{
		if (dwCharID == m_aryVisitorList[nIndex].m_dwCharID)
		{
			int nDeleteIndex = nIndex;
			for (; nDeleteIndex < PktStIInfo::MAX_CUSTOMER_NUM - 1; ++nDeleteIndex)
			{
				m_aryVisitorList[nDeleteIndex] = m_aryVisitorList[nDeleteIndex + 1] ;
			}

			m_aryVisitorList[nDeleteIndex] = VisitorNode();
			m_pVisitorListBox->DeleteItem(nIndex);
			break;
		}
	}
}

void CRYLStallDlg::AddChatMessage(char* lpdzMsg, unsigned long dwColor) 
{
	if ( m_pChattingListBox->Count () > 100 )
	{
		m_pChattingListBox->DeleteItem( 0 ) ;
	}

	m_pChattingListBox->AddItem( lpdzMsg, dwColor, 0xFFFFFFFF, 0x00FFFFFF ) ;
	m_pChattingListBox->SetItemIndex( m_pChattingListBox->Count() ) ;
}

