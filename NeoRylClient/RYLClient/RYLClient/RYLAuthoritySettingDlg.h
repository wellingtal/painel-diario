//=================================================================
// Name : RYLAuthoritySettingDlg.h
// Data : 2004. 01. 10
// Desc : ���� ���� ���̾�α�
//=================================================================
#ifndef __RYL_AUTHORITYSETTINGDLG_H__
#define __RYL_AUTHORITYSETTINGDLG_H__

#include "RYLDialog.h"
#include <Network/Packet/PacketStruct/GuildPacket.h>

namespace RYLAUTHORITYDLG
{
	const DWORD		PUTSTOREHOUSEGENERAL_CHECKBUTTON			= 0x00000000 ;// [�Ϲ�]���â�� ��ǰ�ֱ�
	const DWORD		PUTSTOREHOUSEADMIN_CHECKBUTTON				= 0x00000001 ;// [������]���â�� ��ǰ�ֱ�
	const DWORD		GETSTOREHOUSEGENERAL_CHECKBUTTON			= 0x00000002 ;// [�Ϲ�]���â�� ��ǰã��
	const DWORD		GETSTOREHOUSEADMIN_CHECKBUTTON				= 0x00000003 ;// [������]���â�� ��ǰã��
	const DWORD		USESAFEGENERAL_CHECKBUTTON					= 0x00000004 ;// [�Ϲ�]���â�� ���/���
	const DWORD		USESAFEADMIN_CHECKBUTTON					= 0x00000005 ;// [������]���â�� ���/���
	const DWORD		INVITEMEMVERGENERAL_CHECKBUTTON				= 0x00000006 ;// [�Ϲ�]���� �ʴ�
	const DWORD		INVITEMEMVERADMIN_CHECKBUTTON				= 0x00000007 ;// [������]���� �ʴ�
	const DWORD		PERMITJOINGENERAL_CHECKBUTTON				= 0x00000008 ;// [�Ϲ�]���� �㰡
	const DWORD		PERMITJOINADMIN_CHECKBUTTON					= 0x00000009 ;// [������]���� �㰡
	const DWORD		CHANGEPASSWORDGENERAL_CHECKBUTTON			= 0x00000010 ;// [�Ϲ�]���â��/��й�ȣ����
	const DWORD		CHANGEPASSWORDADMIN_CHECKBUTTON				= 0x00000011 ;// [������]���â��/��й�ȣ����
	const DWORD		SETUPRANKADMIKN_CHECKBUTTON					= 0x00000012 ;// [������]��������
	const DWORD		REGULATETEXADMIN_CHECKBUTTON				= 0x00000013 ;// [������]������ ��������
	const DWORD		KICKMEMBERADMIN_CHECKBUTTON					= 0x00000014 ;// [������]���� ���� ����
	const DWORD		BOARDADMINADMIN_CHECKBUTTON					= 0x00000015 ;// [������]������ �ۼ�����

	const DWORD		APPLY_BUTTON								= 0x00000016 ;// ��������
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

class CRYLAuthoritySettingDlg : public CRYLDialog
{
private:
	CRYLLabel*			m_pGSAuthorityLabel ;			// �Ϲ�/�����ڱ���
	CRYLLabel*			m_pGeneralLabel ;				// �Ϲ�
	CRYLLabel*			m_pAdminLabel ;					// �Ϲݰ�����

	CRYLLabel*			m_pPutStoreHouseLabel ;			// ���â�� ��ǰ�ֱ�
	CRYLCheckBox*		m_pPutStoreHouseGeneral ;		// [�Ϲ�]���â�� ��ǰ�ֱ�
	CRYLCheckBox*		m_pPutStoreHouseAdmin ;			// [������]���â�� ��ǰ�ֱ�

	CRYLLabel*			m_pGetStoreHouseLabel ;			// ���â�� ��ǰã��
	CRYLCheckBox*		m_pGetStoreHouseGeneral ;		// [�Ϲ�]���â�� ��ǰã��
	CRYLCheckBox*		m_pGetStoreHouseAdmin ;			// [������]���â�� ��ǰã��

	CRYLLabel*			m_pUseSafeLabel ;				// ���â�� ���/���
	CRYLCheckBox*		m_pUseSafeGeneral ;				// [�Ϲ�]���â�� ���/���
	CRYLCheckBox*		m_pUseSafeAdmin ;				// [������]���â�� ���/���

	CRYLLabel*			m_pInviteMemberLabel ;			// ���� �ʴ�
	CRYLCheckBox*		m_pInviteMemberGeneral ;		// [�Ϲ�]���� �ʴ�
	CRYLCheckBox*		m_pInviteMemberAdmin;			// [������]���� �ʴ�

	CRYLLabel*			m_pPermitJoinLabel ;			// ���� �㰡
	CRYLCheckBox*		m_pPermitJoinGeneral ;			// [�Ϲ�]���� �㰡
	CRYLCheckBox*		m_pPermitJoinAdmin ;			// [������]���� �㰡

	CRYLLabel*			m_pChangePasswordLabel ;		// ���â��/��й�ȣ����
	CRYLCheckBox*		m_pChangePasswordGeneral ;		// [�Ϲ�]���â��/��й�ȣ����
	CRYLCheckBox*		m_pChangePasswordAdmin ;		// [������]���â��/��й�ȣ����

	CRYLLabel*			m_pAdminSpecificLabel ;			// �����ڰ�������
	
	CRYLLabel*			m_pRegulateTexLabel ;			// ������ ��������
	CRYLCheckBox*		m_pRegulateTexAdmin ;			// [������]������ ��������
	
	CRYLLabel*			m_pKickMemberLabel ;			// ���� ���� ����
	CRYLCheckBox*		m_pKickMemberAdmin ;			// [������]���� ���� ����
	
	CRYLLabel*			m_pBoardAdminLabel ;			// ������ �ۼ�����
	CRYLCheckBox*		m_pBoardAdminAdmin ;			// [������]������ �ۼ�����

	CRYLLabel*			m_pGuildMasterRightLabel ;		// ��帶���� ��������
	CRYLLabel*			m_pMarknLevelLabel ;			// ��帶ũ / �Ը���
	CRYLLabel*			m_pMiddleAdminLabel ;			// �߰������� �Ӹ� / ����
	CRYLLabel*			m_pGoldDivieLabel ;				// ����� ���
	CRYLLabel*			m_pGuildCourseLabel ;			// ����ħ����

	CRYLButton*			m_pApplyButton ;				// ���� ��ư

	UCHAR				m_ucTitle ;
public :
	GuildRight			m_GRight ;

		
public :
	CRYLAuthoritySettingDlg() ;
	virtual ~CRYLAuthoritySettingDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; } ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	SetAuthorityValue( GuildRight* pRight, UCHAR ucTitle ) ;
	VOID	SetGuildRight() ;

	GuildRight*	GetGuildRight()							{ return &m_GRight ; }
	VOID		SetGuildRight( GuildRight GRight ) ;

	UCHAR		GetTitle()								{ return m_ucTitle ; }
	VOID		SetTitle( UCHAR ucTitle )				{ m_ucTitle =  ucTitle ; }
} ;

#endif //__RYL_AUTHORITYSETTINGDLG_H__