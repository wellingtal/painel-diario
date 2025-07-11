//================================================================
//
//	Name : RYLCastleEmblemRightDlg.h 
//	Desc : 성 상징물 권한 다이얼로그
//  Date : 2004. 05. 27
//
//================================================================
#ifndef __RYL_CASTLEEMBLEMRIGHTDLG_H__
#define __RYL_CASTLEEMBLEMRIGHTDLG_H__

#include "RYLDialog.h"
#include <Network/Packet/PacketStruct/CastlePacket.h>

namespace RYLCASTLEEMBLEMRIGHTDLG
{
	const DWORD		EMBLEMUSEDGENERAL_CHECKBUTTON			= 0x00000000 ;// [일반길드원]성 소유 상징물 이용
	const DWORD		EMBLEMUSEDADMIN_CHECKBUTTON				= 0x00000001 ;// [중간관리자]성 소유 상징물 이용

	const DWORD		CASTLEDOORMGRGENERAL_CHECKBUTTON		= 0x00000002 ;// [일반길드원]성문관리
	const DWORD		CASTLEDOORMGRADMIN_CHECKBUTTON			= 0x00000003 ;// [중간관리자]성문관리

	const DWORD		CASTLEDOORUSEDGENERAL_CHECKBUTTON		= 0x00000004 ;// [일반길드원]성문이용
	const DWORD		CASTLEDOORUSEDADMIN_CHECKBUTTON			= 0x00000005 ;// [중간관리자]성문이용

	const DWORD		ARMSMGRGENERAL_CHECKBUTTON				= 0x00000006 ;// [일반길드원]병기관리
	const DWORD		ARMSMGRADMIN_CHECKBUTTON				= 0x00000007 ;// [중간관리자]병기관리

	const DWORD		ARMSMGRUSEDGENERAL_CHECKBUTTON			= 0x00000008 ;// [일반길드원]병기이용
	const DWORD		ARMSMGRUSEDADMIN_CHECKBUTTON			= 0x00000009 ;// [중간관리자]병기이용

	const DWORD		OK_BUTTON								= 0x00000011 ;// 확인버튼
	const DWORD		CANCEL_BUTTON							= 0x00000012 ;// 취소버튼
}

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

class CRYLCastleEmblemRightDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pGeneralAdminRightTitleLable ;			// 성의 관리 권한설정
	
	CRYLLabel*			m_pEmbleUsedLable ;							// 성 소유 상징물 이용
	CRYLLabel*			m_pGeneralLabel1 ;							// 일반 길드원
	CRYLLabel*			m_pMiddleAdminLable1 ;						// 중간 관리자
	CRYLCheckBox*		m_pEmbleUsedGeneralChkBox ;					// [일반길드원]성 소유 상징물 이용
	CRYLCheckBox*		m_pEmbleUsedMiddleAdminChkBox ;				// [중간관리자]성 소유 상징물 이용
	
	CRYLLabel*			m_pCastleDoorMgrLable ;						// 성문 관리
	CRYLLabel*			m_pGeneralLabel2 ;							// 일반 길드원
	CRYLLabel*			m_pMiddleAdminLable2 ;						// 중간 관리자
	CRYLCheckBox*		m_pCastleDoorMgrGeneralChkBox ;				// [일반길드원]성문 관리
	CRYLCheckBox*		m_pCastleDoorMgrMiddleAdminChkBox ;			// [중간관리자]성문 관리

	CRYLLabel*			m_pCastleDoorUsedLable ;					// 성문 이용
	CRYLLabel*			m_pGeneralLabel3 ;							// 일반 길드원
	CRYLLabel*			m_pMiddleAdminLable3 ;						// 중간 관리자
	CRYLCheckBox*		m_pCastleDoorUsedGeneralChkBox ;			// [일반길드원]성문 이용
	CRYLCheckBox*		m_pCastleDoorUsedMiddleAdminChkBox ;		// [중간관리자]성문 이용

	CRYLLabel*			m_pArmsMgrLable ;							// 병기 관리
	CRYLLabel*			m_pGeneralLabel4 ;							// 일반 길드원
	CRYLLabel*			m_pMiddleAdminLable4 ;						// 중간 관리자
	CRYLCheckBox*		m_pArmsMgrGeneralChkBox ;					// [일반길드원]병기 관리
	CRYLCheckBox*		m_pArmsMgrMiddleAdminChkBox ;				// [중간관리자]병기 관리

	CRYLLabel*			m_pArmsUsedLable ;							// 병기 이용
	CRYLLabel*			m_pGeneralLabel5 ;							// 일반 길드원
	CRYLLabel*			m_pMiddleAdminLable5 ;						// 중간 관리자
	CRYLCheckBox*		m_pArmsUsedGeneralChkBox ;					// [일반길드원]병기 이용
	CRYLCheckBox*		m_pArmsUsedMiddleAdminChkBox ;				// [중간관리자]병기 이용

	CRYLButton*			m_pOkButton ;								// 확인 버튼
	CRYLButton*			m_pCancelButton	;							// 취소 버튼

	DWORD				m_dwCastleID ;								// 성 ID
	
public :
	CRYLCastleEmblemRightDlg() ;
	virtual ~CRYLCastleEmblemRightDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	VOID	InitValue() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	SetCastleRight() ;
	VOID	SetCastleID( unsigned long dwCastleID ) ;
	CastleRight			m_CastleRight ;
} ;

#endif // __RYL_CASTLEEMBLEMRIGHTDLG_H__	