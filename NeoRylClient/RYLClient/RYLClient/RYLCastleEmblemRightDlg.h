//================================================================
//
//	Name : RYLCastleEmblemRightDlg.h 
//	Desc : �� ��¡�� ���� ���̾�α�
//  Date : 2004. 05. 27
//
//================================================================
#ifndef __RYL_CASTLEEMBLEMRIGHTDLG_H__
#define __RYL_CASTLEEMBLEMRIGHTDLG_H__

#include "RYLDialog.h"
#include <Network/Packet/PacketStruct/CastlePacket.h>

namespace RYLCASTLEEMBLEMRIGHTDLG
{
	const DWORD		EMBLEMUSEDGENERAL_CHECKBUTTON			= 0x00000000 ;// [�Ϲݱ���]�� ���� ��¡�� �̿�
	const DWORD		EMBLEMUSEDADMIN_CHECKBUTTON				= 0x00000001 ;// [�߰�������]�� ���� ��¡�� �̿�

	const DWORD		CASTLEDOORMGRGENERAL_CHECKBUTTON		= 0x00000002 ;// [�Ϲݱ���]��������
	const DWORD		CASTLEDOORMGRADMIN_CHECKBUTTON			= 0x00000003 ;// [�߰�������]��������

	const DWORD		CASTLEDOORUSEDGENERAL_CHECKBUTTON		= 0x00000004 ;// [�Ϲݱ���]�����̿�
	const DWORD		CASTLEDOORUSEDADMIN_CHECKBUTTON			= 0x00000005 ;// [�߰�������]�����̿�

	const DWORD		ARMSMGRGENERAL_CHECKBUTTON				= 0x00000006 ;// [�Ϲݱ���]�������
	const DWORD		ARMSMGRADMIN_CHECKBUTTON				= 0x00000007 ;// [�߰�������]�������

	const DWORD		ARMSMGRUSEDGENERAL_CHECKBUTTON			= 0x00000008 ;// [�Ϲݱ���]�����̿�
	const DWORD		ARMSMGRUSEDADMIN_CHECKBUTTON			= 0x00000009 ;// [�߰�������]�����̿�

	const DWORD		OK_BUTTON								= 0x00000011 ;// Ȯ�ι�ư
	const DWORD		CANCEL_BUTTON							= 0x00000012 ;// ��ҹ�ư
}

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

class CRYLCastleEmblemRightDlg : public CRYLDialog
{
private :
	CRYLLabel*			m_pGeneralAdminRightTitleLable ;			// ���� ���� ���Ѽ���
	
	CRYLLabel*			m_pEmbleUsedLable ;							// �� ���� ��¡�� �̿�
	CRYLLabel*			m_pGeneralLabel1 ;							// �Ϲ� ����
	CRYLLabel*			m_pMiddleAdminLable1 ;						// �߰� ������
	CRYLCheckBox*		m_pEmbleUsedGeneralChkBox ;					// [�Ϲݱ���]�� ���� ��¡�� �̿�
	CRYLCheckBox*		m_pEmbleUsedMiddleAdminChkBox ;				// [�߰�������]�� ���� ��¡�� �̿�
	
	CRYLLabel*			m_pCastleDoorMgrLable ;						// ���� ����
	CRYLLabel*			m_pGeneralLabel2 ;							// �Ϲ� ����
	CRYLLabel*			m_pMiddleAdminLable2 ;						// �߰� ������
	CRYLCheckBox*		m_pCastleDoorMgrGeneralChkBox ;				// [�Ϲݱ���]���� ����
	CRYLCheckBox*		m_pCastleDoorMgrMiddleAdminChkBox ;			// [�߰�������]���� ����

	CRYLLabel*			m_pCastleDoorUsedLable ;					// ���� �̿�
	CRYLLabel*			m_pGeneralLabel3 ;							// �Ϲ� ����
	CRYLLabel*			m_pMiddleAdminLable3 ;						// �߰� ������
	CRYLCheckBox*		m_pCastleDoorUsedGeneralChkBox ;			// [�Ϲݱ���]���� �̿�
	CRYLCheckBox*		m_pCastleDoorUsedMiddleAdminChkBox ;		// [�߰�������]���� �̿�

	CRYLLabel*			m_pArmsMgrLable ;							// ���� ����
	CRYLLabel*			m_pGeneralLabel4 ;							// �Ϲ� ����
	CRYLLabel*			m_pMiddleAdminLable4 ;						// �߰� ������
	CRYLCheckBox*		m_pArmsMgrGeneralChkBox ;					// [�Ϲݱ���]���� ����
	CRYLCheckBox*		m_pArmsMgrMiddleAdminChkBox ;				// [�߰�������]���� ����

	CRYLLabel*			m_pArmsUsedLable ;							// ���� �̿�
	CRYLLabel*			m_pGeneralLabel5 ;							// �Ϲ� ����
	CRYLLabel*			m_pMiddleAdminLable5 ;						// �߰� ������
	CRYLCheckBox*		m_pArmsUsedGeneralChkBox ;					// [�Ϲݱ���]���� �̿�
	CRYLCheckBox*		m_pArmsUsedMiddleAdminChkBox ;				// [�߰�������]���� �̿�

	CRYLButton*			m_pOkButton ;								// Ȯ�� ��ư
	CRYLButton*			m_pCancelButton	;							// ��� ��ư

	DWORD				m_dwCastleID ;								// �� ID
	
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