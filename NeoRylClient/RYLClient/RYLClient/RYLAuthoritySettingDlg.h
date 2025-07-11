//=================================================================
// Name : RYLAuthoritySettingDlg.h
// Data : 2004. 01. 10
// Desc : 권한 설정 다이얼로그
//=================================================================
#ifndef __RYL_AUTHORITYSETTINGDLG_H__
#define __RYL_AUTHORITYSETTINGDLG_H__

#include "RYLDialog.h"
#include <Network/Packet/PacketStruct/GuildPacket.h>

namespace RYLAUTHORITYDLG
{
	const DWORD		PUTSTOREHOUSEGENERAL_CHECKBUTTON			= 0x00000000 ;// [일반]길드창고 물품넣기
	const DWORD		PUTSTOREHOUSEADMIN_CHECKBUTTON				= 0x00000001 ;// [관리자]길드창고 물품넣기
	const DWORD		GETSTOREHOUSEGENERAL_CHECKBUTTON			= 0x00000002 ;// [일반]길드창고 물품찾기
	const DWORD		GETSTOREHOUSEADMIN_CHECKBUTTON				= 0x00000003 ;// [관리자]길드창고 물품찾기
	const DWORD		USESAFEGENERAL_CHECKBUTTON					= 0x00000004 ;// [일반]길드창고 출금/사용
	const DWORD		USESAFEADMIN_CHECKBUTTON					= 0x00000005 ;// [관리자]길드창고 출금/사용
	const DWORD		INVITEMEMVERGENERAL_CHECKBUTTON				= 0x00000006 ;// [일반]길드원 초대
	const DWORD		INVITEMEMVERADMIN_CHECKBUTTON				= 0x00000007 ;// [관리자]길드원 초대
	const DWORD		PERMITJOINGENERAL_CHECKBUTTON				= 0x00000008 ;// [일반]가입 허가
	const DWORD		PERMITJOINADMIN_CHECKBUTTON					= 0x00000009 ;// [관리자]가입 허가
	const DWORD		CHANGEPASSWORDGENERAL_CHECKBUTTON			= 0x00000010 ;// [일반]길드창고/비밀번호변경
	const DWORD		CHANGEPASSWORDADMIN_CHECKBUTTON				= 0x00000011 ;// [관리자]길드창고/비밀번호변경
	const DWORD		SETUPRANKADMIKN_CHECKBUTTON					= 0x00000012 ;// [관리자]서열정렬
	const DWORD		REGULATETEXADMIN_CHECKBUTTON				= 0x00000013 ;// [관리자]점령지 세율조절
	const DWORD		KICKMEMBERADMIN_CHECKBUTTON					= 0x00000014 ;// [관리자]길드원 강제 축출
	const DWORD		BOARDADMINADMIN_CHECKBUTTON					= 0x00000015 ;// [관리자]길드공지 작성권한

	const DWORD		APPLY_BUTTON								= 0x00000016 ;// 설정적용
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

class CRYLAuthoritySettingDlg : public CRYLDialog
{
private:
	CRYLLabel*			m_pGSAuthorityLabel ;			// 일반/관리자권한
	CRYLLabel*			m_pGeneralLabel ;				// 일반
	CRYLLabel*			m_pAdminLabel ;					// 일반관리자

	CRYLLabel*			m_pPutStoreHouseLabel ;			// 길드창고 물품넣기
	CRYLCheckBox*		m_pPutStoreHouseGeneral ;		// [일반]길드창고 물품넣기
	CRYLCheckBox*		m_pPutStoreHouseAdmin ;			// [관리자]길드창고 물품넣기

	CRYLLabel*			m_pGetStoreHouseLabel ;			// 길드창고 물품찾기
	CRYLCheckBox*		m_pGetStoreHouseGeneral ;		// [일반]길드창고 물품찾기
	CRYLCheckBox*		m_pGetStoreHouseAdmin ;			// [관리자]길드창고 물품찾기

	CRYLLabel*			m_pUseSafeLabel ;				// 길드창고 출금/사용
	CRYLCheckBox*		m_pUseSafeGeneral ;				// [일반]길드창고 출금/사용
	CRYLCheckBox*		m_pUseSafeAdmin ;				// [관리자]길드창고 출금/사용

	CRYLLabel*			m_pInviteMemberLabel ;			// 길드원 초대
	CRYLCheckBox*		m_pInviteMemberGeneral ;		// [일반]길드원 초대
	CRYLCheckBox*		m_pInviteMemberAdmin;			// [관리자]길드원 초대

	CRYLLabel*			m_pPermitJoinLabel ;			// 가입 허가
	CRYLCheckBox*		m_pPermitJoinGeneral ;			// [일반]가입 허가
	CRYLCheckBox*		m_pPermitJoinAdmin ;			// [관리자]가입 허가

	CRYLLabel*			m_pChangePasswordLabel ;		// 길드창고/비밀번호변경
	CRYLCheckBox*		m_pChangePasswordGeneral ;		// [일반]길드창고/비밀번호변경
	CRYLCheckBox*		m_pChangePasswordAdmin ;		// [관리자]길드창고/비밀번호변경

	CRYLLabel*			m_pAdminSpecificLabel ;			// 관리자고유권한
	
	CRYLLabel*			m_pRegulateTexLabel ;			// 점령지 세율조절
	CRYLCheckBox*		m_pRegulateTexAdmin ;			// [관리자]점령지 세율조절
	
	CRYLLabel*			m_pKickMemberLabel ;			// 길드원 강제 축출
	CRYLCheckBox*		m_pKickMemberAdmin ;			// [관리자]길드원 강제 축출
	
	CRYLLabel*			m_pBoardAdminLabel ;			// 길드공지 작성권한
	CRYLCheckBox*		m_pBoardAdminAdmin ;			// [관리자]길드공지 작성권한

	CRYLLabel*			m_pGuildMasterRightLabel ;		// 길드마스터 고유권한
	CRYLLabel*			m_pMarknLevelLabel ;			// 길드마크 / 규모설정
	CRYLLabel*			m_pMiddleAdminLabel ;			// 중간관리자 임명 / 해임
	CRYLLabel*			m_pGoldDivieLabel ;				// 길드골드 배분
	CRYLLabel*			m_pGuildCourseLabel ;			// 길드방침변경

	CRYLButton*			m_pApplyButton ;				// 적용 버튼

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