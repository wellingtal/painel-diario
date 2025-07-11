//================================================================
//
//	Name : RYLIntegServerSelChar.h 
//	Desc : ���ռ��� ĳ���� ����
//
//================================================================
#ifndef __RYLINTEGSERVERSELCHAR_H__
#define __RYLINTEGSERVERSELCHAR_H__

#include "RYLDialog.h"
#include <Db/DBdefine.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>

namespace RYLINTEGSERVERSELCHAR
{
	const DWORD		SEVER_LIST				= 0x00000000 ;
	const DWORD		NEXTSTEP_BUTTON			= 0x00000001 ;
	const DWORD		OLD_SLOT_BUTTON			= 0x00000002 ;
	const DWORD		NEW_SLOT_BUTTON			= 0x00000007 ;
	const DWORD		RACE_COMBOBOX_BTN		= 0x00000011 ;
	const DWORD		RACE_COMBOBOX_MENU		= 0x00000012 ;
} ;

class CRYLComboBox ;
class CRYLListBox ;
class CRYLButton ;
class CRYLLabel ;

#define SELECT_CHAR_SLOT 5

typedef struct __IS_CHARINFO
{
	CRYLLabel*		pCharNameLabel ;
	CRYLLabel*		pCharClassLabel ;
	CRYLLabel*		pCharLevelLabel ;
	CRYLButton*		pSelCharButton ;
	bool			bEnable ;
	__IS_CHARINFO()
	{
		pCharNameLabel	= NULL ;
		pCharClassLabel = NULL ;
		pCharLevelLabel = NULL ;
		pSelCharButton  = NULL ;
		bEnable			= false ;
	}
}IS_CHARINFO, *LPIS_CHARINFO ;

typedef struct __SORT_DATA
{
	char	szName[ 32 ] ;
	char	szClass[ 32 ] ;
	char	szLevel[ 16 ] ;
	bool	bEnable ;
	int		x, y ;
	unsigned long dwCID ;
	unsigned char sClass ;
	unsigned char cLevel ;
    unsigned char cOldServerGroupID;

	__SORT_DATA()
	{
		szName[ 0 ] = '\0' ;
		szClass[ 0 ] = '\0' ;
		szLevel[ 0 ] = '\0' ;
		bEnable = false ;
		cOldServerGroupID = x = y = dwCID = sClass = 0 ;
	}
}SORT_DATA, *LPSORT_DATA ;

class CRYLIntegServerSelCharDlg : public CRYLDialog
{
public:

	CRYLListBox*	m_pServerList ;				// ���� ����Ʈ
	CRYLComboBox*	m_pRaceCBBox ;				// ���� �޺��ڽ�
	CRYLLabel*		m_pTitleLabel ;				// ���ռ��� ĳ���ͼ���
	CRYLLabel*		m_pOldCharLabel ;			// ���� ĳ���ͼ���
	CRYLLabel*		m_pRaceLabel ;				// ���� ����
	CRYLLabel*		m_pNationLabel ;			// ���� ����
	CRYLLabel*		m_pIntegCharLabel ;			// ���õ� ���ռ����� ĳ����
	CRYLLabel*		m_pDesc1Label ;				// *�ϳ��� �������� �޸�ĳ���Ϳ� ��ĭ ĳ���͸�
	CRYLLabel*		m_pDesc2Label ;				// ���ÿ� �����Ͻ� �� �����ϴ�.
	CRYLLabel*		m_pSelectCharcLabel ;		// ĳ���ͼ���Ƚ��
	CRYLLabel*		m_pRemainCharTransferCountLabel ;// ĳ���ͼ���Ƚ�� ��
	CRYLButton*		m_pNextStepButton ;			// ��������

	USER_INFO			m_UserInfo ;
    UnifiedCharData     m_UnifiedCharData[PktUnifiedCharInfo::MAX_CHAR_DATA] ;
	unsigned long		m_dwCharDataNum_Out ;
	SORT_DATA			m_SortData[ 10 ][ SELECT_CHAR_SLOT ] ;
	SORT_DATA			m_SelectChar[ SELECT_CHAR_SLOT ] ;

	IS_CHARINFO		m_OldCharSlot[ SELECT_CHAR_SLOT ] ;		// ���� ĳ���� ����
	IS_CHARINFO		m_SelCharSlot[ SELECT_CHAR_SLOT ] ;		// ������ ĳ���� ����
	unsigned short		m_sUnifiedNation ;
	bool				m_bFirstLoginEnable ;
	unsigned char		m_cRestrictedPart1ToPart2Level ;
	char				m_cSelMaxCharCount ;
	char				m_cRemainCharTransferCount ;
			
public :
	CRYLIntegServerSelCharDlg() ;
	virtual ~CRYLIntegServerSelCharDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	void SetCharInfo( USER_INFO* UserInfo, UnifiedCharData* UnifiedCharData, 
					  unsigned long dwCharDataNum_Out, unsigned char cRestrictedPart1ToPart2Level ) ;
	void UpdateDlg() ;
	void SelectServer() ;
	void ResetServerCharInfo() ;
	void ResetServerList() ;
	void SelectChar( int iIndex ) ;
	void CancelChar( int iIndex ) ;
	USER_INFO& GetUserInfo()		{ return m_UserInfo ; }
	bool CheckNation( int iIndex ) ;
	void SetFirstLoginEnable( bool bFirst )				{ m_bFirstLoginEnable = bFirst ; }
	bool GetFirstLoginEnable()	 						{ return m_bFirstLoginEnable ; }
	bool GetEnableCharSlot() ;
	void SetCurrentNation() ;
	void SetRemainCharTransferCount( unsigned char cVal ) { m_cRemainCharTransferCount = m_cSelMaxCharCount = cVal ; }
	unsigned char GetRemainCharTransferCount()			{ return m_cRemainCharTransferCount ; }
	bool UpdateRemainCharTransferCount() ;
} ;

#endif //__RYLINTEGSERVERSELCHAR_H__