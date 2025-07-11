#include <winsock2.h>
#include "RYLAuthoritySettingDlg.h"
#include "RYLCheckBox.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "WinInput.h"
#include "RYLSpriteEx.h"
#include "RYLSprite.h"
#include "Texture.h"

#include "BaseDataDefine.h"
#include "RYLNetworkData.h"
#include "GMImageDib.h"
#include "RYLImage.h"	

#include <Network/ClientSocket/Send/SendCommunity.h>
#include <RenderOption.h>
#include "RYLStringTable.h"
#include "GMMemory.h"
CRYLAuthoritySettingDlg*	g_pAuthorityDlg = NULL ;

CRYLAuthoritySettingDlg::CRYLAuthoritySettingDlg() : CRYLDialog()
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 256, 375, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	for ( int i = 0 ; i < 73 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 365, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 6, 19, 245,  1, 0xff959595, 0x80000000,  1 ) ;
	pImageDIB->MakeFrameRect( 6, 166, 245,  1, 0xff959595, 0x80000000,  1 ) ;
	pImageDIB->MakeFrameRect( 6, 273, 245,  1, 0xff959595, 0x80000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	
	SetWidth( 256 ) ;
	SetHeight( 375 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pAuthorityDlg = this ;	
	InitCtrl() ;

	m_ucTitle =  Guild::COMMON ;
}

CRYLAuthoritySettingDlg::~CRYLAuthoritySettingDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLAuthoritySettingDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	
	// �Ϲ�/�����ڱ���
	m_pGSAuthorityLabel = new CRYLLabel( this ) ;
	m_pGSAuthorityLabel->SetLeft( 8 ) ;
	m_pGSAuthorityLabel->SetTop( 21 ) ;
	m_pGSAuthorityLabel->SetFontColor (0xffffffff);
	m_pGSAuthorityLabel->SetAutoSize( TRUE ) ;
	m_pGSAuthorityLabel->SetCaption( CRYLStringTable::m_strString[ 1990 ] ) ;
	AttachGMUIControl( m_pGSAuthorityLabel ) ;

	// �Ϲ�
	m_pGeneralLabel = new CRYLLabel( this ) ;
	m_pGeneralLabel->SetLeft( 182 ) ;
	m_pGeneralLabel->SetTop( 21 ) ;
	m_pGeneralLabel->SetFontColor (0xffffffff);
	m_pGeneralLabel->SetAutoSize( TRUE ) ;
	m_pGeneralLabel->SetCaption( CRYLStringTable::m_strString[ 1991 ] ) ;
	AttachGMUIControl( m_pGeneralLabel ) ;

	// ������
	m_pAdminLabel = new CRYLLabel( this ) ;
	m_pAdminLabel->SetLeft( 214 ) ;
	m_pAdminLabel->SetTop( 21 ) ;
	m_pAdminLabel->SetFontColor (0xffffffff);
	m_pAdminLabel->SetAutoSize( TRUE ) ;
	m_pAdminLabel->SetCaption( CRYLStringTable::m_strString[ 1992 ] ) ;
	AttachGMUIControl( m_pAdminLabel ) ;

	// ���â�� ��ǰ�ֱ�
	m_pPutStoreHouseLabel = new CRYLLabel( this ) ;
	m_pPutStoreHouseLabel->SetLeft( 8 ) ;
	m_pPutStoreHouseLabel->SetTop( 39 ) ;
	m_pPutStoreHouseLabel->SetFontColor (0xffffffff);
	m_pPutStoreHouseLabel->SetAutoSize( TRUE ) ;
	m_pPutStoreHouseLabel->SetCaption( CRYLStringTable::m_strString[ 1993 ] ) ;
	AttachGMUIControl( m_pPutStoreHouseLabel ) ;

	// [�Ϲ�]���â�� ��ǰ�ֱ�
	m_pPutStoreHouseGeneral = new CRYLCheckBox( this ) ;
	m_pPutStoreHouseGeneral->SetLeft( 185 ) ;
	m_pPutStoreHouseGeneral->SetTop( 39 ) ;
	m_pPutStoreHouseGeneral->SetInstance( RYLAUTHORITYDLG::PUTSTOREHOUSEGENERAL_CHECKBUTTON ) ;
	m_pPutStoreHouseGeneral->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPutStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pPutStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pPutStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPutStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPutStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pPutStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pPutStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pPutStoreHouseGeneral->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pPutStoreHouseGeneral ) ;

	// [������]���â�� ��ǰ�ֱ�
	m_pPutStoreHouseAdmin = new CRYLCheckBox( this ) ;
	m_pPutStoreHouseAdmin->SetLeft( 224 ) ;
	m_pPutStoreHouseAdmin->SetTop( 39 ) ;
	m_pPutStoreHouseAdmin->SetInstance( RYLAUTHORITYDLG::PUTSTOREHOUSEADMIN_CHECKBUTTON ) ;
	m_pPutStoreHouseAdmin->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPutStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pPutStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pPutStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPutStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pPutStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pPutStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pPutStoreHouseAdmin->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pPutStoreHouseAdmin ) ;

	// ���â�� ��ǰã��
	m_pGetStoreHouseLabel = new CRYLLabel( this ) ;
	m_pGetStoreHouseLabel->SetLeft( 8 ) ;
	m_pGetStoreHouseLabel->SetTop( 59 ) ;
	m_pGetStoreHouseLabel->SetFontColor (0xffffffff);
	m_pGetStoreHouseLabel->SetAutoSize( TRUE ) ;
	m_pGetStoreHouseLabel->SetCaption( CRYLStringTable::m_strString[ 1994 ] ) ;
	AttachGMUIControl( m_pGetStoreHouseLabel ) ;

	// [�Ϲ�]���â�� ��ǰã��
	m_pGetStoreHouseGeneral = new CRYLCheckBox( this ) ;
	m_pGetStoreHouseGeneral->SetLeft( 185 ) ;
	m_pGetStoreHouseGeneral->SetTop( 59 ) ;
	m_pGetStoreHouseGeneral->SetInstance( RYLAUTHORITYDLG::GETSTOREHOUSEGENERAL_CHECKBUTTON ) ;
	m_pGetStoreHouseGeneral->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pGetStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pGetStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pGetStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pGetStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pGetStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pGetStoreHouseGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pGetStoreHouseGeneral->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pGetStoreHouseGeneral ) ;

	// [������]���â�� ��ǰã��
	m_pGetStoreHouseAdmin = new CRYLCheckBox( this ) ;
	m_pGetStoreHouseAdmin->SetLeft( 224 ) ;
	m_pGetStoreHouseAdmin->SetTop( 59 ) ;
	m_pGetStoreHouseAdmin->SetInstance( RYLAUTHORITYDLG::GETSTOREHOUSEADMIN_CHECKBUTTON ) ;
	m_pGetStoreHouseAdmin->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pGetStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pGetStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pGetStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pGetStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pGetStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pGetStoreHouseAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pGetStoreHouseAdmin->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pGetStoreHouseAdmin ) ;
	
	// ���â�� ���/���
	m_pUseSafeLabel = new CRYLLabel( this ) ;
	m_pUseSafeLabel->SetLeft( 8 ) ;
	m_pUseSafeLabel->SetTop( 78 ) ;
	m_pUseSafeLabel->SetFontColor (0xffffffff);
	m_pUseSafeLabel->SetAutoSize( TRUE ) ;
	m_pUseSafeLabel->SetCaption( CRYLStringTable::m_strString[ 1995 ] ) ;
	AttachGMUIControl( m_pUseSafeLabel ) ;
	

	// [�Ϲ�]���â�� ���/���
	m_pUseSafeGeneral = new CRYLCheckBox( this ) ;
	m_pUseSafeGeneral->SetLeft( 185 ) ;
	m_pUseSafeGeneral->SetTop( 78 ) ;
	m_pUseSafeGeneral->SetInstance( RYLAUTHORITYDLG::USESAFEGENERAL_CHECKBUTTON ) ;
	m_pUseSafeGeneral->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pUseSafeGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pUseSafeGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pUseSafeGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pUseSafeGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pUseSafeGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pUseSafeGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pUseSafeGeneral->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pUseSafeGeneral ) ;

	// [������]���â�� ���/���
	m_pUseSafeAdmin = new CRYLCheckBox( this ) ;
	m_pUseSafeAdmin->SetLeft( 224 ) ;
	m_pUseSafeAdmin->SetTop( 78 ) ;
	m_pUseSafeAdmin->SetInstance( RYLAUTHORITYDLG::USESAFEADMIN_CHECKBUTTON ) ;
	m_pUseSafeAdmin->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pUseSafeAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pUseSafeAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pUseSafeAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pUseSafeAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pUseSafeAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pUseSafeAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pUseSafeAdmin->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pUseSafeAdmin ) ;
	
	// ���� �ʴ�
	m_pInviteMemberLabel = new CRYLLabel( this ) ;
	m_pInviteMemberLabel->SetLeft( 8 ) ;
	m_pInviteMemberLabel->SetTop( 97 ) ;
	m_pInviteMemberLabel->SetFontColor (0xffffffff);
	m_pInviteMemberLabel->SetAutoSize( TRUE ) ;
	m_pInviteMemberLabel->SetCaption( CRYLStringTable::m_strString[ 1996 ] ) ;
	AttachGMUIControl( m_pInviteMemberLabel ) ;
	
	// [�Ϲ�]���� �ʴ�
	m_pInviteMemberGeneral = new CRYLCheckBox( this ) ;
	m_pInviteMemberGeneral->SetLeft( 185 ) ;
	m_pInviteMemberGeneral->SetTop( 97 ) ;
	m_pInviteMemberGeneral->SetInstance( RYLAUTHORITYDLG::INVITEMEMVERGENERAL_CHECKBUTTON ) ;
	m_pInviteMemberGeneral->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pInviteMemberGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pInviteMemberGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pInviteMemberGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pInviteMemberGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pInviteMemberGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pInviteMemberGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pInviteMemberGeneral->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pInviteMemberGeneral ) ;

	// [������]���� �ʴ�
	m_pInviteMemberAdmin = new CRYLCheckBox( this ) ;
	m_pInviteMemberAdmin->SetLeft( 224 ) ;
	m_pInviteMemberAdmin->SetTop( 97 ) ;
	m_pInviteMemberAdmin->SetInstance( RYLAUTHORITYDLG::INVITEMEMVERADMIN_CHECKBUTTON ) ;
	m_pInviteMemberAdmin->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pInviteMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pInviteMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pInviteMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pInviteMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pInviteMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pInviteMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pInviteMemberAdmin->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pInviteMemberAdmin ) ;

	// ���� �㰡
	m_pPermitJoinLabel = new CRYLLabel( this ) ;
	m_pPermitJoinLabel->SetLeft( 8 ) ;
	m_pPermitJoinLabel->SetTop( 116 ) ;
	m_pPermitJoinLabel->SetFontColor (0xffffffff);
	m_pPermitJoinLabel->SetAutoSize( TRUE ) ;
	m_pPermitJoinLabel->SetCaption( CRYLStringTable::m_strString[ 1997 ] ) ;
	AttachGMUIControl( m_pPermitJoinLabel ) ;
		
	// [�Ϲ�]���� �㰡
	m_pPermitJoinGeneral = new CRYLCheckBox( this ) ;
	m_pPermitJoinGeneral->SetLeft( 185 ) ;
	m_pPermitJoinGeneral->SetTop( 116 ) ;
	m_pPermitJoinGeneral->SetInstance( RYLAUTHORITYDLG::PERMITJOINGENERAL_CHECKBUTTON ) ;
	m_pPermitJoinGeneral->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPermitJoinGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pPermitJoinGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pPermitJoinGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPermitJoinGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pPermitJoinGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pPermitJoinGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pPermitJoinGeneral->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pPermitJoinGeneral ) ;

	// [������]���� �㰡
	m_pPermitJoinAdmin = new CRYLCheckBox( this ) ;
	m_pPermitJoinAdmin->SetLeft( 224 ) ;
	m_pPermitJoinAdmin->SetTop( 116 ) ;
	m_pPermitJoinAdmin->SetInstance( RYLAUTHORITYDLG::PERMITJOINADMIN_CHECKBUTTON ) ;
	m_pPermitJoinAdmin->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPermitJoinAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pPermitJoinAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pPermitJoinAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pPermitJoinAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pPermitJoinAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pPermitJoinAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pPermitJoinAdmin->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pPermitJoinAdmin ) ;

	// ���â��/��й�ȣ����
	m_pChangePasswordLabel = new CRYLLabel( this ) ;
	m_pChangePasswordLabel->SetLeft( 8 ) ;
	m_pChangePasswordLabel->SetTop( 135 ) ;
	m_pChangePasswordLabel->SetFontColor (0xffffffff);
	m_pChangePasswordLabel->SetAutoSize( TRUE ) ;
	m_pChangePasswordLabel->SetCaption( CRYLStringTable::m_strString[ 1998 ] ) ;
	AttachGMUIControl( m_pChangePasswordLabel ) ;

	// [�Ϲ�]���â��/��й�ȣ����
	m_pChangePasswordGeneral = new CRYLCheckBox( this ) ;
	m_pChangePasswordGeneral->SetLeft( 185 ) ;
	m_pChangePasswordGeneral->SetTop( 135 ) ;
	m_pChangePasswordGeneral->SetInstance( RYLAUTHORITYDLG::CHANGEPASSWORDGENERAL_CHECKBUTTON ) ;
	m_pChangePasswordGeneral->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pChangePasswordGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pChangePasswordGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pChangePasswordGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pChangePasswordGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pChangePasswordGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pChangePasswordGeneral->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pChangePasswordGeneral->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pChangePasswordGeneral ) ;

	// [������]���â��/��й�ȣ����
	m_pChangePasswordAdmin = new CRYLCheckBox( this ) ;
	m_pChangePasswordAdmin->SetLeft( 224 ) ;
	m_pChangePasswordAdmin->SetTop( 135 ) ;
	m_pChangePasswordAdmin->SetInstance( RYLAUTHORITYDLG::CHANGEPASSWORDADMIN_CHECKBUTTON ) ;
	m_pChangePasswordAdmin->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pChangePasswordAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pChangePasswordAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pChangePasswordAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pChangePasswordAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pChangePasswordAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pChangePasswordAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pChangePasswordAdmin->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pChangePasswordAdmin ) ;

	// �����ڰ�������
	m_pAdminSpecificLabel = new CRYLLabel( this ) ;
	m_pAdminSpecificLabel->SetLeft( 8 ) ;
	m_pAdminSpecificLabel->SetTop( 168 ) ;
	m_pAdminSpecificLabel->SetFontColor (0xffffffff);
	m_pAdminSpecificLabel->SetAutoSize( TRUE ) ;
	m_pAdminSpecificLabel->SetCaption( CRYLStringTable::m_strString[ 1999 ] ) ;
	AttachGMUIControl( m_pAdminSpecificLabel ) ;

	// ������ ��������
	m_pRegulateTexLabel = new CRYLLabel( this ) ;
	m_pRegulateTexLabel->SetLeft( 8 ) ;
	m_pRegulateTexLabel->SetTop( 185 ) ;
	m_pRegulateTexLabel->SetFontColor (0xffffffff);
	m_pRegulateTexLabel->SetAutoSize( TRUE ) ;
	m_pRegulateTexLabel->SetCaption( CRYLStringTable::m_strString[ 2002 ] ) ;
	AttachGMUIControl( m_pRegulateTexLabel ) ;

	// [������]������ ��������
	m_pRegulateTexAdmin = new CRYLCheckBox( this ) ;
	m_pRegulateTexAdmin->SetLeft( 224 ) ;
	m_pRegulateTexAdmin->SetTop( 185 ) ;
	m_pRegulateTexAdmin->SetInstance( RYLAUTHORITYDLG::REGULATETEXADMIN_CHECKBUTTON ) ;
	m_pRegulateTexAdmin->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pRegulateTexAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pRegulateTexAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pRegulateTexAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pRegulateTexAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pRegulateTexAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pRegulateTexAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pRegulateTexAdmin->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pRegulateTexAdmin ) ;

	// ���� ���� ����
	m_pKickMemberLabel = new CRYLLabel( this ) ;
	m_pKickMemberLabel->SetLeft( 8 ) ;
	m_pKickMemberLabel->SetTop( 204 ) ;
	m_pKickMemberLabel->SetFontColor (0xffffffff);
	m_pKickMemberLabel->SetAutoSize( TRUE ) ;
	m_pKickMemberLabel->SetCaption( CRYLStringTable::m_strString[ 2003 ] ) ;
	AttachGMUIControl( m_pKickMemberLabel ) ;

	// [������]���� ���� ����
	m_pKickMemberAdmin = new CRYLCheckBox( this ) ;
	m_pKickMemberAdmin->SetLeft( 224 ) ;
	m_pKickMemberAdmin->SetTop( 204 ) ;
	m_pKickMemberAdmin->SetInstance( RYLAUTHORITYDLG::KICKMEMBERADMIN_CHECKBUTTON ) ;
	m_pKickMemberAdmin->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pKickMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pKickMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pKickMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pKickMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pKickMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pKickMemberAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pKickMemberAdmin->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pKickMemberAdmin ) ;
	
	// ������ �ۼ�����
	m_pBoardAdminLabel = new CRYLLabel( this ) ;
	m_pBoardAdminLabel->SetLeft( 8 ) ;
	m_pBoardAdminLabel->SetTop( 223 ) ;
	m_pBoardAdminLabel->SetFontColor (0xffffffff);
	m_pBoardAdminLabel->SetAutoSize( TRUE ) ;
	m_pBoardAdminLabel->SetCaption( CRYLStringTable::m_strString[ 2004 ] ) ;
	AttachGMUIControl( m_pBoardAdminLabel ) ;

	// [������]������ �ۼ�����
	m_pBoardAdminAdmin = new CRYLCheckBox( this ) ;
	m_pBoardAdminAdmin->SetLeft( 224 ) ;
	m_pBoardAdminAdmin->SetTop( 223 ) ;
	m_pBoardAdminAdmin->SetInstance( RYLAUTHORITYDLG::BOARDADMINADMIN_CHECKBUTTON ) ;
	m_pBoardAdminAdmin->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pBoardAdminAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pBoardAdminAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pBoardAdminAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pBoardAdminAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pBoardAdminAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pBoardAdminAdmin->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pBoardAdminAdmin->SetChecked( TRUE ) ;
	AttachGMUIControl( m_pBoardAdminAdmin ) ;

	// ��帶���� ��������
	m_pGuildMasterRightLabel = new CRYLLabel( this ) ;
	m_pGuildMasterRightLabel->SetLeft( 8 ) ;
	m_pGuildMasterRightLabel->SetTop( 275 ) ;
	m_pGuildMasterRightLabel->SetFontColor (0xffffffff);
	m_pGuildMasterRightLabel->SetAutoSize( TRUE ) ;
	m_pGuildMasterRightLabel->SetCaption( CRYLStringTable::m_strString[ 2005 ] ) ;
	AttachGMUIControl( m_pGuildMasterRightLabel ) ;

	// ��帶ũ/�Ը���
	m_pMarknLevelLabel = new CRYLLabel( this ) ;
	m_pMarknLevelLabel->SetLeft( 8 ) ;
	m_pMarknLevelLabel->SetTop( 292 ) ;
	m_pMarknLevelLabel->SetFontColor (0xffffffff);
	m_pMarknLevelLabel->SetAutoSize( TRUE ) ;
	m_pMarknLevelLabel->SetCaption( CRYLStringTable::m_strString[ 2006 ] ) ;
	AttachGMUIControl( m_pMarknLevelLabel ) ;

	// �߰������� �Ӹ� / ����
	m_pMiddleAdminLabel = new CRYLLabel( this ) ;
	m_pMiddleAdminLabel->SetLeft( 8 ) ;
	m_pMiddleAdminLabel->SetTop( 311 ) ;
	m_pMiddleAdminLabel->SetFontColor (0xffffffff);
	m_pMiddleAdminLabel->SetAutoSize( TRUE ) ;
	m_pMiddleAdminLabel->SetCaption( CRYLStringTable::m_strString[ 2007 ] ) ;
	AttachGMUIControl( m_pMiddleAdminLabel ) ;

	// ����� ���
	m_pGoldDivieLabel = new CRYLLabel( this ) ;
	m_pGoldDivieLabel->SetLeft( 8 ) ;
	m_pGoldDivieLabel->SetTop( 330 ) ;
	m_pGoldDivieLabel->SetFontColor (0xffffffff);
	m_pGoldDivieLabel->SetAutoSize( TRUE ) ;
	m_pGoldDivieLabel->SetCaption( CRYLStringTable::m_strString[ 2008 ] ) ;
	AttachGMUIControl( m_pGoldDivieLabel ) ;

	// ����ħ����
	m_pGuildCourseLabel = new CRYLLabel( this ) ;
	m_pGuildCourseLabel->SetLeft( 8 ) ;
	m_pGuildCourseLabel->SetTop( 349 ) ;
	m_pGuildCourseLabel->SetFontColor (0xffffffff);
	m_pGuildCourseLabel->SetAutoSize( TRUE ) ;
	m_pGuildCourseLabel->SetCaption( CRYLStringTable::m_strString[ 2009 ] ) ;
	AttachGMUIControl( m_pGuildCourseLabel ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	// ���� ��ư
	m_pApplyButton = new CRYLButton( this ) ;
	m_pApplyButton->SetLeft( 185 ) ;
	m_pApplyButton->SetTop( 355 ) ;
	m_pApplyButton->SetCaption( CRYLStringTable::m_strString[ 2010 ] ) ;
	m_pApplyButton->SetFontColor( 0xffffffff) ;
	m_pApplyButton->SetColor( 0xffffffff) ;
	m_pApplyButton->OnClickEvent = DlgClick ;
	m_pApplyButton->SetInstance( RYLAUTHORITYDLG::APPLY_BUTTON ) ;
	m_pApplyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pApplyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pApplyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pApplyButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pApplyButton ) ;
}

VOID	CRYLAuthoritySettingDlg::FinalCtrl ()
{
	GM_DELETE( m_pGSAuthorityLabel ) ;			// �Ϲ�/�����ڱ���
	GM_DELETE( m_pGeneralLabel ) ;				// �Ϲ�
	GM_DELETE( m_pAdminLabel ) ;				// �Ϲݰ�����

	GM_DELETE( m_pPutStoreHouseLabel ) ;		// ���â�� ��ǰ�ֱ�
	GM_DELETE( m_pPutStoreHouseGeneral ) ;		// [�Ϲ�]���â�� ��ǰ�ֱ�
	GM_DELETE( m_pPutStoreHouseAdmin ) ;		// [������]���â�� ��ǰ�ֱ�

	GM_DELETE( m_pGetStoreHouseLabel ) ;		// ���â�� ��ǰã��
	GM_DELETE( m_pGetStoreHouseGeneral ) ;		// [�Ϲ�]���â�� ��ǰã��
	GM_DELETE( m_pGetStoreHouseAdmin ) ;		// [������]���â�� ��ǰã��

	GM_DELETE( m_pUseSafeLabel ) ;				// ���â�� ���/���
	GM_DELETE( m_pUseSafeGeneral ) ;				// [�Ϲ�]���â�� ���/���
	GM_DELETE( m_pUseSafeAdmin ) ;				// [������]���â�� ���/���

	GM_DELETE( m_pInviteMemberLabel ) ;			// ���� �ʴ�
	GM_DELETE( m_pInviteMemberGeneral ) ;		// [�Ϲ�]���� �ʴ�
	GM_DELETE( m_pInviteMemberAdmin) ;			// [������]���� �ʴ�

	GM_DELETE( m_pPermitJoinLabel ) ;			// ���� �㰡
	GM_DELETE( m_pPermitJoinGeneral ) ;			// [�Ϲ�]���� �㰡
	GM_DELETE( m_pPermitJoinAdmin ) ;			// [������]���� �㰡

	GM_DELETE( m_pChangePasswordLabel ) ;		// ���â��/��й�ȣ����
	GM_DELETE( m_pChangePasswordGeneral ) ;		// [�Ϲ�]���â��/��й�ȣ����
	GM_DELETE( m_pChangePasswordAdmin ) ;		// [������]���â��/��й�ȣ����

	GM_DELETE( m_pAdminSpecificLabel ) ;		// �����ڰ�������
	GM_DELETE( m_pRegulateTexLabel ) ;			// ������ ��������
	GM_DELETE( m_pRegulateTexAdmin ) ;			// [������]������ ��������
	
	GM_DELETE( m_pKickMemberLabel ) ;			// ���� ���� ����
	GM_DELETE( m_pKickMemberAdmin ) ;			// [������]���� ���� ����
	
	GM_DELETE( m_pBoardAdminLabel ) ;			// ������ �ۼ�����
	GM_DELETE( m_pBoardAdminAdmin ) ;			// [������]������ �ۼ�����

	GM_DELETE( m_pGuildMasterRightLabel ) ;		// ��帶���� ��������
	GM_DELETE( m_pMarknLevelLabel ) ;			// ��帶ũ / �Ը���
	GM_DELETE( m_pMiddleAdminLabel ) ;			// �߰������� �Ӹ� / ����
	GM_DELETE( m_pGoldDivieLabel ) ;			// ����� ���
	GM_DELETE( m_pGuildCourseLabel ) ;			// ����ħ����

	GM_DELETE( m_pApplyButton ) ;				// ���� ��ư
}

VOID	CRYLAuthoritySettingDlg::Show( CGMUIObject* Sender )
{
	SetAuthorityValue( &m_GRight, Guild::NONE ) ;
}
	
VOID	CRYLAuthoritySettingDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID	CRYLAuthoritySettingDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLAuthoritySettingDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if(g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if(g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}

	if ( m_ucTitle == Guild::MASTER )
	{
		if ( m_pPutStoreHouseGeneral->GetChecked() ) 		// [�Ϲ�]���â�� ��ǰ�ֱ�
			m_pPutStoreHouseAdmin->SetChecked( TRUE ) ;			// [������]���â�� ��ǰ�ֱ�

		if ( m_pGetStoreHouseGeneral->GetChecked() )		// [�Ϲ�]���â�� ��ǰã��
			m_pGetStoreHouseAdmin->SetChecked( TRUE ) ;			// [������]���â�� ��ǰã��

		if ( m_pUseSafeGeneral->GetChecked() )				// [�Ϲ�]���â�� ���/���
			m_pUseSafeAdmin->SetChecked( TRUE ) ;				// [������]���â�� ���/���

		if ( m_pInviteMemberGeneral->GetChecked() )		// [�Ϲ�]���� �ʴ�
			m_pInviteMemberAdmin->SetChecked( TRUE ) ;			// [������]���� �ʴ�

		if ( m_pPermitJoinGeneral->GetChecked() )			// [�Ϲ�]���� �㰡
			m_pPermitJoinAdmin->SetChecked( TRUE ) ;			// [������]���� �㰡

		if ( m_pChangePasswordGeneral->GetChecked() )		// [�Ϲ�]���â��/��й�ȣ����
			m_pChangePasswordAdmin->SetChecked( TRUE ) ;		// [������]���â��/��й�ȣ����
	}

	return S_OK ;
}

VOID	CRYLAuthoritySettingDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pAuthorityDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLAUTHORITYDLG::APPLY_BUTTON :
			{
				g_pAuthorityDlg->SetGuildRight() ;
				SendPacket::CharGuildRight( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, g_pAuthorityDlg->m_GRight ) ;
				g_pAuthorityDlg->SetVisible( FALSE ) ;
			}
			break ;

	}
}

VOID	CRYLAuthoritySettingDlg::SetAuthorityValue( GuildRight* pRight, UCHAR ucTitle ) 
{
	if ( ucTitle != Guild::NONE )
		m_ucTitle = ucTitle ;

	memcpy( &m_GRight, pRight, sizeof( GuildRight ) ) ;

	if ( m_ucTitle == Guild::MASTER )
	{
		m_pPutStoreHouseGeneral->SetEnable( TRUE ) ;		// [�Ϲ�]���â�� ��ǰ�ֱ�
		m_pPutStoreHouseAdmin->SetEnable( TRUE ) ;			// [������]���â�� ��ǰ�ֱ�
		m_pGetStoreHouseGeneral->SetEnable( TRUE ) ;		// [�Ϲ�]���â�� ��ǰã��
		m_pGetStoreHouseAdmin->SetEnable( TRUE ) ;			// [������]���â�� ��ǰã��
		m_pUseSafeGeneral->SetEnable( TRUE ) ;				// [�Ϲ�]���â�� ���/���
		m_pUseSafeAdmin->SetEnable( TRUE ) ;				// [������]���â�� ���/���
		m_pInviteMemberGeneral->SetEnable( TRUE ) ;			// [�Ϲ�]���� �ʴ�
		m_pInviteMemberAdmin->SetEnable( TRUE );			// [������]���� �ʴ�
		m_pPermitJoinGeneral->SetEnable( TRUE ) ;			// [�Ϲ�]���� �㰡
		m_pPermitJoinAdmin->SetEnable( TRUE ) ;				// [������]���� �㰡
		m_pChangePasswordGeneral->SetEnable( TRUE ) ;		// [�Ϲ�]���â��/��й�ȣ����
		m_pChangePasswordAdmin->SetEnable( TRUE ) ;			// [������]���â��/��й�ȣ����
		m_pRegulateTexAdmin->SetEnable( TRUE ) ;			// [������]������ ��������
		m_pKickMemberAdmin->SetEnable( TRUE ) ;				// [������]���� ���� ����
		m_pBoardAdminAdmin->SetEnable( TRUE ) ;				// [������]������ �ۼ�����

		m_pApplyButton->SetEnable( TRUE ) ;
	}
	else if ( m_ucTitle >= Guild::MIDDLE_ADMIN ) 
	{
		m_pApplyButton->SetEnable( FALSE ) ;
	}

	// ���â�� ��ǰ�ֱ�
	switch ( m_GRight.m_aryRight[ GuildRight::PUT_STOREHOUSE ] ) 
	{
	case Guild::COMMON :
		m_pPutStoreHouseGeneral->SetChecked( TRUE ) ; 
		m_pPutStoreHouseAdmin->SetChecked( TRUE ) ; 
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pPutStoreHouseGeneral->SetDisableEmptyCheck( TRUE ) ; 
			m_pPutStoreHouseAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MIDDLE_ADMIN :
		m_pPutStoreHouseGeneral->SetChecked( FALSE ) ; 
		m_pPutStoreHouseAdmin->SetChecked( TRUE ) ; 
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pPutStoreHouseGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pPutStoreHouseAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MASTER :
		m_pPutStoreHouseGeneral->SetChecked( FALSE ) ; 
		m_pPutStoreHouseAdmin->SetChecked( FALSE ) ; 
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pPutStoreHouseGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pPutStoreHouseAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	}
	
	// ���â�� ��ǰã��
	switch ( m_GRight.m_aryRight[ GuildRight::GET_STOREHOUSE ] ) 
	{
	case Guild::COMMON :
		m_pGetStoreHouseGeneral->SetChecked( TRUE ) ;
		m_pGetStoreHouseAdmin->SetChecked( TRUE ) ;	
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pGetStoreHouseGeneral->SetDisableEmptyCheck( TRUE ) ; 
			m_pGetStoreHouseAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MIDDLE_ADMIN :
		m_pGetStoreHouseGeneral->SetChecked( FALSE ) ;
		m_pGetStoreHouseAdmin->SetChecked( TRUE ) ;	
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pGetStoreHouseGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pGetStoreHouseAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MASTER :
		m_pGetStoreHouseGeneral->SetChecked( FALSE ) ;
		m_pGetStoreHouseAdmin->SetChecked( FALSE ) ;	
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pGetStoreHouseGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pGetStoreHouseAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	}

	// ���â�� ���/���
	switch ( m_GRight.m_aryRight[ GuildRight::USE_SAFE ] ) 
	{
	case Guild::COMMON :
		m_pUseSafeGeneral->SetChecked( TRUE ) ;	
		m_pUseSafeAdmin->SetChecked( TRUE ) ;	
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pUseSafeGeneral->SetDisableEmptyCheck( TRUE ) ; 
			m_pUseSafeAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MIDDLE_ADMIN :
		m_pUseSafeGeneral->SetChecked( FALSE ) ;	
		m_pUseSafeAdmin->SetChecked( TRUE ) ;	
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pUseSafeGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pUseSafeAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MASTER :
		m_pUseSafeGeneral->SetChecked( FALSE ) ;	
		m_pUseSafeAdmin->SetChecked( FALSE ) ;	
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pUseSafeGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pUseSafeAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	}


	// ���� �ʴ�
	switch ( m_GRight.m_aryRight[ GuildRight::INVITE_MEMBER ] ) 
	{
	case Guild::COMMON :
		m_pInviteMemberGeneral->SetChecked( TRUE ) ;
		m_pInviteMemberAdmin->SetChecked( TRUE ) ;	
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pInviteMemberGeneral->SetDisableEmptyCheck( TRUE ) ; 
			m_pInviteMemberAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MIDDLE_ADMIN :
		m_pInviteMemberGeneral->SetChecked( FALSE ) ;
		m_pInviteMemberAdmin->SetChecked( TRUE ) ;	
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pInviteMemberGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pInviteMemberAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MASTER :
		m_pInviteMemberGeneral->SetChecked( FALSE ) ;
		m_pInviteMemberAdmin->SetChecked( FALSE ) ;	
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pInviteMemberGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pInviteMemberAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	}
	
	// ���������㰡 
	switch ( m_GRight.m_aryRight[ GuildRight::PERMIT_JOIN ] ) 
	{
	case Guild::COMMON :
		m_pPermitJoinGeneral->SetChecked( TRUE ) ;
		m_pPermitJoinAdmin->SetChecked( TRUE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pPermitJoinGeneral->SetDisableEmptyCheck( TRUE ) ; 
			m_pPermitJoinAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MIDDLE_ADMIN :
		m_pPermitJoinGeneral->SetChecked( FALSE ) ;
		m_pPermitJoinAdmin->SetChecked( TRUE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pPermitJoinGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pPermitJoinAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MASTER :
		m_pPermitJoinGeneral->SetChecked( FALSE ) ;
		m_pPermitJoinAdmin->SetChecked( FALSE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pPermitJoinGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pPermitJoinAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	}

	// ���â�� / ��й�ȣ���� 
	switch ( m_GRight.m_aryRight[ GuildRight::CHANGE_PASSWORD ] ) 
	{
	case Guild::COMMON :
		m_pChangePasswordGeneral->SetChecked( TRUE ) ;
		m_pChangePasswordAdmin->SetChecked( TRUE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pChangePasswordGeneral->SetDisableEmptyCheck( TRUE ) ; 
			m_pChangePasswordAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MIDDLE_ADMIN :
		m_pChangePasswordGeneral->SetChecked( FALSE ) ;
		m_pChangePasswordAdmin->SetChecked( TRUE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pChangePasswordGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pChangePasswordAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MASTER :
		m_pChangePasswordGeneral->SetChecked( FALSE ) ;
		m_pChangePasswordAdmin->SetChecked( FALSE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pChangePasswordGeneral->SetDisableEmptyCheck( FALSE ) ; 
			m_pChangePasswordAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	}
	
	// ������ ��������
	switch ( m_GRight.m_aryRight[ GuildRight::REGULATE_TEX ] ) 
	{
	case Guild::COMMON :
		m_pRegulateTexAdmin->SetChecked( FALSE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pRegulateTexAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	case Guild::MIDDLE_ADMIN :
		m_pRegulateTexAdmin->SetChecked( TRUE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pRegulateTexAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MASTER :
		m_pRegulateTexAdmin->SetChecked( FALSE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pRegulateTexAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	}

	// ���� ��������
	switch ( m_GRight.m_aryRight[ GuildRight::KICK_MEMBER ] ) 
	{
	case Guild::COMMON :
		m_pKickMemberAdmin->SetChecked( FALSE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pKickMemberAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	case Guild::MIDDLE_ADMIN :
		m_pKickMemberAdmin->SetChecked( TRUE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pKickMemberAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MASTER :
		m_pKickMemberAdmin->SetChecked( FALSE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pKickMemberAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	}
	
	// ������ �ۼ�����
	switch ( m_GRight.m_aryRight[ GuildRight::BOARD_ADMIN ] ) 
	{
	case Guild::COMMON :
		m_pBoardAdminAdmin->SetChecked( FALSE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pBoardAdminAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	case Guild::MIDDLE_ADMIN :
		m_pBoardAdminAdmin->SetChecked( TRUE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pBoardAdminAdmin->SetDisableEmptyCheck( TRUE ) ; 
		}
		break ;
	case Guild::MASTER :
		m_pBoardAdminAdmin->SetChecked( FALSE ) ;
		if ( m_ucTitle != Guild::MASTER )
		{
			m_pBoardAdminAdmin->SetDisableEmptyCheck( FALSE ) ; 
		}
		break ;
	}
}
VOID	CRYLAuthoritySettingDlg::SetGuildRight() 
{
	INT iIndex = 0 ;
	std::fill_n(m_GRight.m_aryRight, int(MAX_RIGHT_SIZE), Guild::NONE);

	if ( ( m_pPutStoreHouseGeneral->GetChecked() == TRUE ) && ( m_pPutStoreHouseAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::PUT_STOREHOUSE ] = Guild::COMMON ;
	else if ( ( m_pPutStoreHouseGeneral->GetChecked() == FALSE ) && ( m_pPutStoreHouseAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::PUT_STOREHOUSE ] = Guild::MIDDLE_ADMIN ;
	else if ( ( m_pPutStoreHouseGeneral->GetChecked() == FALSE ) && ( m_pPutStoreHouseAdmin->GetChecked() == FALSE ) )
		m_GRight.m_aryRight[ GuildRight::PUT_STOREHOUSE ] = Guild::MASTER ;

	if ( ( m_pGetStoreHouseGeneral->GetChecked() == TRUE ) && ( m_pGetStoreHouseAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::GET_STOREHOUSE ] = Guild::COMMON ;
	else if ( ( m_pGetStoreHouseGeneral->GetChecked() == FALSE ) && ( m_pGetStoreHouseAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::GET_STOREHOUSE ] = Guild::MIDDLE_ADMIN ;
	else if ( ( m_pGetStoreHouseGeneral->GetChecked() == FALSE ) && ( m_pGetStoreHouseAdmin->GetChecked() == FALSE ) )
		m_GRight.m_aryRight[ GuildRight::GET_STOREHOUSE ] = Guild::MASTER ;

	if ( ( m_pUseSafeGeneral->GetChecked() == TRUE ) && ( m_pUseSafeAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::USE_SAFE ] = Guild::COMMON ;
	else if ( ( m_pUseSafeGeneral->GetChecked() == FALSE ) && ( m_pUseSafeAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::USE_SAFE ] = Guild::MIDDLE_ADMIN ;
	else if ( ( m_pUseSafeGeneral->GetChecked() == FALSE ) && ( m_pUseSafeAdmin->GetChecked() == FALSE ) )
		m_GRight.m_aryRight[ GuildRight::USE_SAFE ] = Guild::MASTER ;

	if ( ( m_pInviteMemberGeneral->GetChecked() == TRUE ) && ( m_pInviteMemberAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::INVITE_MEMBER ] = Guild::COMMON ;
	else if ( ( m_pInviteMemberGeneral->GetChecked() == FALSE ) && ( m_pInviteMemberAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::INVITE_MEMBER ] = Guild::MIDDLE_ADMIN ;
	else if ( ( m_pInviteMemberGeneral->GetChecked() == FALSE ) && ( m_pInviteMemberAdmin->GetChecked() == FALSE ) )
		m_GRight.m_aryRight[ GuildRight::INVITE_MEMBER ] = Guild::MASTER ;

	if ( ( m_pPermitJoinGeneral->GetChecked() == TRUE ) && ( m_pPermitJoinAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::PERMIT_JOIN ] = Guild::COMMON ;
	else if ( ( m_pPermitJoinGeneral->GetChecked() == FALSE ) && ( m_pPermitJoinAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::PERMIT_JOIN ] = Guild::MIDDLE_ADMIN ;
	else if ( ( m_pPermitJoinGeneral->GetChecked() == FALSE ) && ( m_pPermitJoinAdmin->GetChecked() == FALSE ) )
		m_GRight.m_aryRight[ GuildRight::PERMIT_JOIN ] = Guild::MASTER ;

 	if ( ( m_pChangePasswordGeneral->GetChecked() == TRUE ) && ( m_pChangePasswordAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::CHANGE_PASSWORD ] = Guild::COMMON ;
	else if ( ( m_pChangePasswordGeneral->GetChecked() == FALSE ) && ( m_pChangePasswordAdmin->GetChecked() == TRUE ) )
		m_GRight.m_aryRight[ GuildRight::CHANGE_PASSWORD ] = Guild::MIDDLE_ADMIN ;
	else if ( ( m_pChangePasswordGeneral->GetChecked() == FALSE ) && ( m_pChangePasswordAdmin->GetChecked() == FALSE ) )
		m_GRight.m_aryRight[ GuildRight::CHANGE_PASSWORD ] = Guild::MASTER ;

	// ������ ��������
	if ( m_pRegulateTexAdmin->GetChecked() == TRUE )
		m_GRight.m_aryRight[ GuildRight::REGULATE_TEX ] = Guild::MIDDLE_ADMIN ;
	if ( m_pRegulateTexAdmin->GetChecked() == FALSE )
		m_GRight.m_aryRight[ GuildRight::REGULATE_TEX ] = Guild::MASTER ;

	// ���� ��������
	if ( m_pKickMemberAdmin->GetChecked() == TRUE )
		m_GRight.m_aryRight[ GuildRight::KICK_MEMBER ] = Guild::MIDDLE_ADMIN ;
	if ( m_pKickMemberAdmin->GetChecked() == FALSE )
		m_GRight.m_aryRight[ GuildRight::KICK_MEMBER ] = Guild::MASTER ;

	// ������ �ۼ�����
	if ( m_pBoardAdminAdmin->GetChecked() == TRUE )
		m_GRight.m_aryRight[ GuildRight::BOARD_ADMIN ] = Guild::MIDDLE_ADMIN ;
	if ( m_pBoardAdminAdmin->GetChecked() == FALSE )
		m_GRight.m_aryRight[ GuildRight::BOARD_ADMIN ] = Guild::MASTER ;
}

VOID		CRYLAuthoritySettingDlg::SetGuildRight( GuildRight GRight ) 
{
	m_GRight = GRight ; 
}
