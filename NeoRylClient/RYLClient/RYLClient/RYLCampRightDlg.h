//================================================================
//
//	Name : RYLCamprightDlg.h 
//	Desc : 진지 권한 다이얼로그
//  Date : 2004. 06. 01
//
//================================================================
#ifndef __RYL_CAMPRIGHTDLG_H__
#define __RYL_CAMPRIGHTDLG_H__

#include "RYLDialog.h"
#include <Network/Packet/PacketStruct/CastlePacket.h>

namespace RYLCAMPRIGHTDLG
{
	const DWORD		CAMPCONTROLGENERAL_CHECKBUTTON			= 0x00000000 ;// [일반길드원]진지 관리
	const DWORD		CAMPCONTROLADMIN_CHECKBUTTON			= 0x00000001 ;// [중간관리자]진지 관리
					
	const DWORD		CAMPDESTORYGRGENERAL_CHECKBUTTON		= 0x00000002 ;// [일반길드원]진지 파괴
	const DWORD		CAMPDESTORYADMIN_CHECKBUTTON			= 0x00000003 ;// [중간관리자]진지 파괴

	const DWORD		OK_BUTTON								= 0x00000005 ;// 확인버튼
	const DWORD		CANCEL_BUTTON							= 0x00000006 ;// 취소버튼
}

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

class CRYLCampRightDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pCampRightTitleLable ;					// 진지 권한 설정
	
	CRYLLabel*			m_pCampControlLable ;						// 진지 관리
	CRYLLabel*			m_pGeneralLabel1 ;							// 일반 길드원
	CRYLLabel*			m_pMiddleAdminLable1 ;						// 중간 관리자
	CRYLCheckBox*		m_pCampControlGeneralChkBox ;				// [일반길드원]진지 관리
	CRYLCheckBox*		m_pCampControlMiddleAdminChkBox ;			// [중간관리자]진지 관리

	/*
	CRYLLabel*			m_pCampDestoryLable ;						// 진지 파괴
	CRYLLabel*			m_pGeneralLabel2 ;							// 일반 길드원
	CRYLLabel*			m_pMiddleAdminLable2 ;						// 중간 관리자
	CRYLCheckBox*		m_pCampDestoryGeneralChkBox ;				// [일반길드원]진지 파괴
	CRYLCheckBox*		m_pCampDestoryMiddleAdminChkBox ;			// [중간관리자]진지 파괴
	*/

	CRYLButton*			m_pOkButton ;								// 확인 버튼
	CRYLButton*			m_pCancelButton	;							// 취소 버튼

	DWORD				m_dwCampID ;
	BOOL				m_bWorldWeapon ;							// 월드웨폰 다이얼로그 변환
	
protected:
	VOID				ChangeDlg();

public :
	CRYLCampRightDlg() ;
	virtual ~CRYLCampRightDlg() ;

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
	
	VOID	SetCampRight() ;
	VOID	SetCampID( DWORD dwCampID ) ;
	VOID	SetWorldWeaponDlg( BOOL bWeapon ){ m_bWorldWeapon = bWeapon ; };
	CampRight			m_CampRight ;
} ;

#endif // __RYL_CAMPRIGHTDLG_H__