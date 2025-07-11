//================================================================
//
//	Name : RYLIntegServerVaultDlg.h 
//	Desc : 통합서버 창고 선택
//
//================================================================
#ifndef __RYLINTEGSERVERSELVAULT_H__
#define __RYLINTEGSERVERSELVAULT_H__

#include "RYLDialog.h"
#include <Db/DBdefine.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>

#include "RYLLabel.h"
#include "RYLCheckBox.h"

#define VAULT_NUM 7

namespace RYLINTEGSERVERSELVAULT
{
	const DWORD		CONFIRM_BUTTON				= 0x00000000 ;
	const DWORD		PREV_BUTTON					= 0x00000001 ;
	const DWORD		VAULT_CKBOX					= 0x00000002 ;
	
} ; 

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

typedef struct __IS_VAULTINFO
{
	CRYLLabel*		pServerNameLabel ;
	CRYLLabel*		pUsedNumLabel ;
	CRYLLabel*		pGoldLabel ;
	CRYLCheckBox*	pSelVaultButton ;
	unsigned long	dwSelectVaultServer ;
	
	__IS_VAULTINFO()
	{
		pServerNameLabel	= NULL ;
		pUsedNumLabel		= NULL ;
		pGoldLabel			= NULL ;
		pSelVaultButton		= NULL ;
		dwSelectVaultServer = 0 ;
	}

	~__IS_VAULTINFO()
	{
		GM_DELETE( pServerNameLabel ) ;
		GM_DELETE( pUsedNumLabel ) ;
		GM_DELETE( pGoldLabel ) ;
		GM_DELETE( pSelVaultButton ) ;
	}

}IS_VAULTINFO, *LPIS_VAULTINFO ;

class CRYLIntegServerSelVaultDlg : public CRYLDialog
{
public:
	CRYLLabel*		m_pTitleLabel ;				// 통합서버 창고선택
	CRYLLabel*		m_pSelVaultLabel ;			// 창고를 선택하여주십시요
	CRYLButton*		m_pPrevStepButton ;			// 이전단계로
	CRYLButton*		m_pArrayButton ;			// 선택완료

	UnifiedStoreInfo	m_UnifiedStoreInfo[PktUnifiedCharInfo::MAX_STORE_INFO] ;
	unsigned long		m_dwStoreInfoNum_Out ;

	IS_VAULTINFO	m_VaultSlot[ VAULT_NUM ] ;		// 창고 슬롯
	IS_VAULTINFO*	m_lpTmpVaultSlot ;
	unsigned long		m_dwSelectVaultServer ;

	bool				m_bVaultEnable ;
	unsigned short		m_sSelIndex ;
	
	unsigned char       m_cAgentServerType;
    unsigned char       m_cFirstLogin;
			
public :
	CRYLIntegServerSelVaultDlg() ;
	virtual ~CRYLIntegServerSelVaultDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	unsigned char GetAgentServerType() const { return m_cAgentServerType; }
    unsigned char GetFirstLogin() const { return m_cFirstLogin; }

	void SetVaultInfo( UnifiedStoreInfo* StoreInfo, unsigned long dwStoreInfoNum_Out ) ;
	void SetUnifiedStatus ( unsigned char cAgentServerType, unsigned char cFirstLogin ) ;

	void UpdateDlg() ;
	void CheckButton( int iIndex ) ;
	bool GetVaultEnable()				{ return m_bVaultEnable ; }
	void SetVaultEnable( bool bVault )	{ m_bVaultEnable = bVault ; }
} ;

#endif //__RYLINTEGSERVERSELVAULT_H__