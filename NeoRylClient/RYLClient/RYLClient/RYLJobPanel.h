#ifndef __RYL_JOBPANEL_H__
#define __RYL_JOBPANEL_H__

#include "GMUICustomPanel.h"
#include <d3d8.h>
#include <vector>

class CRYLLabel ;
class CRYLButton ;
class CItemInstance ;

namespace RYLJOBPANEL
{
	const DWORD			PREVJOB_BUTTON		= 0x00000000 ;
	const DWORD			NEXTJOB_BUTTON		= 0x00000001 ;
} ;

class CRYLJobPanel : public CGMUICustomPanel
{
private :
	enum Const
	{
		MAX_SLOT_PER_PAGE		= 9,			// 페이지 당 최대 슬롯 수
	};

	CRYLButton*								m_pPrevJobButton ;
	CRYLButton*								m_pNextJobButton ;
	CRYLLabel*								m_pJobPointLabel ;// 잡 포인트
	CRYLLabel*								m_pJobPointValueLabel ;// 잡 포인트 값

	CRYLLabel*								m_pComingSoonLabel;

	unsigned long							m_dwSlotHeightMargin ;
	unsigned long							m_dwStartSlot ;

public:
	CRYLJobPanel(CGMUICustomDlg *pParentDlg);
	virtual ~CRYLJobPanel();

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	void	InitJob() ;
	void	PrevJobSlotPage() ;
	void	NextJobSlotPage() ;
	void	JobSlotAlign() ; 
	void    UpdateJobSlotPosition() ;
};

#endif //__RYL_JOBPANEL_H__