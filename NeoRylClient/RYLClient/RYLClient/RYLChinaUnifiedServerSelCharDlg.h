//================================================================
//
//	Name : RYLChinaUnifiedServerSelCharDlg.h 
//	Desc : 중국, 대만 통합서버 캐릭터 선택
//
//================================================================
#ifndef __RYL_CHINA_UNIFIEDSERVER_SELCHARDLG_H__
#define __RYL_CHINA_UNIFIEDSERVER_SELCHARDLG_H__

#include "RYLDialog.h"
#include "RYLIntegServerSelCharDlg.h"

namespace RYLCHINASELCHARDLG
{
	const DWORD		SEVER_LIST				= 0x00000000 ;
	const DWORD		SELECT_BUTTON			= 0x00000001 ;
} ;

class CRYLListBox ;
class CRYLButton ;
class CRYLLabel ;

typedef struct __IS_CHINA_CHARINFO
{
	CRYLLabel*		pCharNameLabel ;
	CRYLLabel*		pCharClassLabel ;
	CRYLLabel*		pCharLevelLabel ;
}IS_CHINA_CHARINFO, *LPIS_CHINA_CHARINFO ;

typedef struct __IS_CHINA_VAULTINFO
{
	char			szGold[ 32 ] ;
	unsigned char	cUsedTab ;
	
	__IS_CHINA_VAULTINFO()
	{
		szGold[ 0 ]   = '\0' ;
		cUsedTab = 0 ;
	}

}IS_CHINA_VAULTINFO, *LPIS_CHINA_VAULTINFO ;

class CRYLChinaUnifiedServerSelCharDlg : public CRYLDialog
{
public:
	IS_CHINA_CHARINFO	m_OldCharSlot[ 5 ] ;		// 기존 캐릭터 슬롯
	IS_CHINA_VAULTINFO	m_VaultSlot[ 7 ] ;			// 창고 정보

	CRYLListBox*		m_pServerList ;				// 서버 리스트
	CRYLLabel*			m_pTitleLabel ;				// 통합서버 캐릭터선택
	CRYLLabel*			m_pOldCharLabel ;			// 기존 캐릭터선택
	CRYLButton*			m_pSelectButton ;
	CRYLLabel*			m_pVaultLabel ;
	CRYLLabel*			m_pVaultGoldLabel ;

	CRYLLabel*			m_pVaultTebLabel ;
	CRYLLabel*			m_pVaultUsedLabel ;

	unsigned char       m_cAgentServerType;
    unsigned char       m_cFirstLogin;
	
	USER_INFO			m_UserInfo ;
	UnifiedCharData     m_UnifiedCharData[PktUnifiedCharInfo::MAX_CHAR_DATA] ;
	unsigned long		m_dwCharDataNum_Out ;
	unsigned char		m_cRestrictedPart1ToPart2Level ;

	SORT_DATA			m_SortData[ 10 ][ SELECT_CHAR_SLOT ] ;

	UnifiedStoreInfo	m_UnifiedStoreInfo[PktUnifiedCharInfo::MAX_STORE_INFO] ;
	unsigned long		m_dwStoreInfoNum_Out ;

	unsigned long		m_dwFastLoginSlot ;

public :
	CRYLChinaUnifiedServerSelCharDlg() ;
	virtual ~CRYLChinaUnifiedServerSelCharDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	void SetVaultInfo( UnifiedStoreInfo* StoreInfo, unsigned long dwStoreInfoNum_Out ) ;
	void SetUnifiedStatus ( unsigned char cAgentServerType, unsigned char cFirstLogin ) ;
	void SetCharInfo( USER_INFO* UserInfo, UnifiedCharData* UnifiedCharData, 
					  unsigned long dwCharDataNum_Out ) ;

	void UpdateCharDlg() ;
	void UpdateVaultDlg() ;
	void SelectServer() ;
	void ResetServerCharInfo() ;
	const char* GetSlotName() ;
	void SendUnifiedCharSelect() ;
	bool LoadFastLoginSlot() ; 
	void SaveFastLoginSlot() ; 
} ;

#endif //__RYL_CHINA_UNIFIEDSERVER_SELCHARDLG_H__