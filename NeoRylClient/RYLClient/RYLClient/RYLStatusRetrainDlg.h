//=================================================================
// Name : RYLStatusRetrainDlg.h
// Data : 2003. 01. 08
// Desc : 스테이터스 재훈련/재분배 다이얼로그
//=================================================================
#ifndef __RYL_STATUSRETRAINDLG_H__
#define __RYL_STATUSRETRAINDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"
#include <Network/Packet/PacketStruct/CharStatusPacket.h>

namespace RYLSTATUSRETRAINDLG
{
	const DWORD		STRPOINTINC_BUTTON			= 0x00000000 ;
	const DWORD		STRPOINTDEC_BUTTON			= 0x00000001 ;

	const DWORD		DEXPOINTINC_BUTTON			= 0x00000002 ;
	const DWORD		DEXPOINTDEC_BUTTON			= 0x00000003 ;

	const DWORD		CONPOINTINC_BUTTON			= 0x00000004 ;
	const DWORD		CONPOINTDEC_BUTTON			= 0x00000005 ;

	const DWORD		INTPOINTINC_BUTTON			= 0x00000006 ;
	const DWORD		INTPOINTDEC_BUTTON			= 0x00000007 ;

	const DWORD		WISPOINTINC_BUTTON			= 0x00000008 ;
	const DWORD		WISPOINTDEC_BUTTON			= 0x00000009 ;

	const DWORD		OK_BUTTON					= 0x00000010 ;
	const DWORD		CANCEL_BUTTON				= 0x00000011 ;
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLPicture ;

class CRYLStatusRetrainDlg : public CRYLDialog
{
public :

	enum DlgState
	{
		DLG_RETRAIN			= 0,		// 스탯 재훈련
		DLG_REDSTRIBUTION	= 1			// 스탯 재분배
	};

	enum Const
	{
		MAX_RETRAIN_STATE	= 800			// 재훈련 가능한 최대 IP 수
	};


private :

	// STR 포인트 증가,감소 버튼
	CRYLLabel*			m_pStrLabel ;
	CRYLLabel*			m_pStrValueLabel ;
	CRYLButton*			m_pStrPointIncBtn ;
	CRYLButton*			m_pStrPointDecBtn ;

	// DEX 포인트 증가,감소 버튼
	CRYLLabel*			m_pDexLabel ;
	CRYLLabel*			m_pDexValueLabel ;
	CRYLButton*			m_pDexPointIncBtn ;
	CRYLButton*			m_pDexPointDecBtn ;

	// CON 포인트 증가,감소 버튼
	CRYLLabel*			m_pConLabel ;
	CRYLLabel*			m_pConValueLabel ;
	CRYLButton*			m_pConPointIncBtn ;
	CRYLButton*			m_pConPointDecBtn ;

	// INT 포인트 증가,감소 버튼
	CRYLLabel*			m_pIntLabel ;
	CRYLLabel*			m_pIntValueLabel ;
	CRYLButton*			m_pIntPointIncBtn ;
	CRYLButton*			m_pIntPointDecBtn ;

	// WIS 포인트 증가,감소 버튼
	CRYLLabel*			m_pWisLabel ;
	CRYLLabel*			m_pWisValueLabel ;
	CRYLButton*			m_pWisPointIncBtn ;
	CRYLButton*			m_pWisPointDecBtn ;

	// 재훈련 포인트
	CRYLLabel*			m_pRetrainLabel ;
	CRYLLabel*			m_pRetrainValueLabel ;

	// 보너스 포인트
	CRYLLabel*			m_pBonusPointLabel ;
	CRYLLabel*			m_pBonusPointValueLabel ;

	// Ok, Cancel
	CRYLButton*			m_pOkBtn ;
	CRYLButton*			m_pCancelBtn ;

	// 보너스 포인트 이미지 
	CRYLPicture*		m_pBonusSTRPicture ;
	CRYLPicture*		m_pBonusDEXPicture ;
	CRYLPicture*		m_pBonusCONPicture ;
	CRYLPicture*		m_pBonusINTPicture ;
	CRYLPicture*		m_pBonusWISPicture ;
		
	ChState				m_csInitState ;
	ChState				m_csBasicState ;
	ChState				m_csBaseState ;

	CItemInstance*		m_lpStatusRetrainItem ;
	unsigned long		m_dwStatusInitResult ;
	long				m_lRetrainIP ;
	bool				m_bAcceptMessageBox ;		// 최종 확인 메세지 박스가 떠있는가?

	unsigned char		m_cDlgState ;				// 다이얼로그의 용도


public :

	CRYLStatusRetrainDlg() ;
	virtual ~CRYLStatusRetrainDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;


	// STR 포인트 증가,감소 
	VOID		StrPointInc() ;
	VOID		StrPointDec() ;
	
	// DEX 포인트 증가,감소 
	VOID		DexPointInc() ;
	VOID		DexPointDec() ;

	// CON 포인트 증가,감소 
	VOID		ConPointInc() ;
	VOID		ConPointDec() ;

	// INT 포인트 증가,감소 
	VOID		IntPointInc() ;
	VOID		IntPointDec() ;

	// WIS 포인트 증가,감소 
	VOID		WisPointInc() ;
	VOID		WisPointDec() ;

	VOID		Ok() ;
	VOID		Cancel() ;

	void		SetState( unsigned char cDlgState ) ;
} ;

#endif //__RYL_STATUSRETRAINDLG_H__
