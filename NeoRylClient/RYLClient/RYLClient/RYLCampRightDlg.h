//================================================================
//
//	Name : RYLCamprightDlg.h 
//	Desc : ���� ���� ���̾�α�
//  Date : 2004. 06. 01
//
//================================================================
#ifndef __RYL_CAMPRIGHTDLG_H__
#define __RYL_CAMPRIGHTDLG_H__

#include "RYLDialog.h"
#include <Network/Packet/PacketStruct/CastlePacket.h>

namespace RYLCAMPRIGHTDLG
{
	const DWORD		CAMPCONTROLGENERAL_CHECKBUTTON			= 0x00000000 ;// [�Ϲݱ���]���� ����
	const DWORD		CAMPCONTROLADMIN_CHECKBUTTON			= 0x00000001 ;// [�߰�������]���� ����
					
	const DWORD		CAMPDESTORYGRGENERAL_CHECKBUTTON		= 0x00000002 ;// [�Ϲݱ���]���� �ı�
	const DWORD		CAMPDESTORYADMIN_CHECKBUTTON			= 0x00000003 ;// [�߰�������]���� �ı�

	const DWORD		OK_BUTTON								= 0x00000005 ;// Ȯ�ι�ư
	const DWORD		CANCEL_BUTTON							= 0x00000006 ;// ��ҹ�ư
}

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

class CRYLCampRightDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pCampRightTitleLable ;					// ���� ���� ����
	
	CRYLLabel*			m_pCampControlLable ;						// ���� ����
	CRYLLabel*			m_pGeneralLabel1 ;							// �Ϲ� ����
	CRYLLabel*			m_pMiddleAdminLable1 ;						// �߰� ������
	CRYLCheckBox*		m_pCampControlGeneralChkBox ;				// [�Ϲݱ���]���� ����
	CRYLCheckBox*		m_pCampControlMiddleAdminChkBox ;			// [�߰�������]���� ����

	/*
	CRYLLabel*			m_pCampDestoryLable ;						// ���� �ı�
	CRYLLabel*			m_pGeneralLabel2 ;							// �Ϲ� ����
	CRYLLabel*			m_pMiddleAdminLable2 ;						// �߰� ������
	CRYLCheckBox*		m_pCampDestoryGeneralChkBox ;				// [�Ϲݱ���]���� �ı�
	CRYLCheckBox*		m_pCampDestoryMiddleAdminChkBox ;			// [�߰�������]���� �ı�
	*/

	CRYLButton*			m_pOkButton ;								// Ȯ�� ��ư
	CRYLButton*			m_pCancelButton	;							// ��� ��ư

	DWORD				m_dwCampID ;
	BOOL				m_bWorldWeapon ;							// ������� ���̾�α� ��ȯ
	
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