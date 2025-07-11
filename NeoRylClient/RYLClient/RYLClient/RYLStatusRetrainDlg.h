//=================================================================
// Name : RYLStatusRetrainDlg.h
// Data : 2003. 01. 08
// Desc : �������ͽ� ���Ʒ�/��й� ���̾�α�
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
		DLG_RETRAIN			= 0,		// ���� ���Ʒ�
		DLG_REDSTRIBUTION	= 1			// ���� ��й�
	};

	enum Const
	{
		MAX_RETRAIN_STATE	= 800			// ���Ʒ� ������ �ִ� IP ��
	};


private :

	// STR ����Ʈ ����,���� ��ư
	CRYLLabel*			m_pStrLabel ;
	CRYLLabel*			m_pStrValueLabel ;
	CRYLButton*			m_pStrPointIncBtn ;
	CRYLButton*			m_pStrPointDecBtn ;

	// DEX ����Ʈ ����,���� ��ư
	CRYLLabel*			m_pDexLabel ;
	CRYLLabel*			m_pDexValueLabel ;
	CRYLButton*			m_pDexPointIncBtn ;
	CRYLButton*			m_pDexPointDecBtn ;

	// CON ����Ʈ ����,���� ��ư
	CRYLLabel*			m_pConLabel ;
	CRYLLabel*			m_pConValueLabel ;
	CRYLButton*			m_pConPointIncBtn ;
	CRYLButton*			m_pConPointDecBtn ;

	// INT ����Ʈ ����,���� ��ư
	CRYLLabel*			m_pIntLabel ;
	CRYLLabel*			m_pIntValueLabel ;
	CRYLButton*			m_pIntPointIncBtn ;
	CRYLButton*			m_pIntPointDecBtn ;

	// WIS ����Ʈ ����,���� ��ư
	CRYLLabel*			m_pWisLabel ;
	CRYLLabel*			m_pWisValueLabel ;
	CRYLButton*			m_pWisPointIncBtn ;
	CRYLButton*			m_pWisPointDecBtn ;

	// ���Ʒ� ����Ʈ
	CRYLLabel*			m_pRetrainLabel ;
	CRYLLabel*			m_pRetrainValueLabel ;

	// ���ʽ� ����Ʈ
	CRYLLabel*			m_pBonusPointLabel ;
	CRYLLabel*			m_pBonusPointValueLabel ;

	// Ok, Cancel
	CRYLButton*			m_pOkBtn ;
	CRYLButton*			m_pCancelBtn ;

	// ���ʽ� ����Ʈ �̹��� 
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
	bool				m_bAcceptMessageBox ;		// ���� Ȯ�� �޼��� �ڽ��� ���ִ°�?

	unsigned char		m_cDlgState ;				// ���̾�α��� �뵵


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


	// STR ����Ʈ ����,���� 
	VOID		StrPointInc() ;
	VOID		StrPointDec() ;
	
	// DEX ����Ʈ ����,���� 
	VOID		DexPointInc() ;
	VOID		DexPointDec() ;

	// CON ����Ʈ ����,���� 
	VOID		ConPointInc() ;
	VOID		ConPointDec() ;

	// INT ����Ʈ ����,���� 
	VOID		IntPointInc() ;
	VOID		IntPointDec() ;

	// WIS ����Ʈ ����,���� 
	VOID		WisPointInc() ;
	VOID		WisPointDec() ;

	VOID		Ok() ;
	VOID		Cancel() ;

	void		SetState( unsigned char cDlgState ) ;
} ;

#endif //__RYL_STATUSRETRAINDLG_H__
