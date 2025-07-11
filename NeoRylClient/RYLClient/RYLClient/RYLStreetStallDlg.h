//================================================================
//
//	Name : RYLStreetStallDlg.h 
//	Desc : 노점상 다이얼로그
//  Date : 2004. 1. 3
//
//================================================================
#ifndef __RYL_STREETSTALLDLG_H__
#define __RYL_STREETSTALLDLG_H__

#include "RYLStallDlg.h"

class CRYLLabel ;
class CItemInstance ;

namespace RYLSTREETSTALLDLG
{
	const unsigned long				CHATTING_EDIT = 0x00000000 ;	
} ;

class CRYLStreetStallDlg : public CRYLStallDlg
{
private :

	CHAR			m_strMarketName[ 32 ] ;
	CHAR			m_strOwner[ CHAR_INFOST::MAX_NAME_LEN ] ;

	CRYLLabel*		m_pStreetStallLabel ;		// 노점상
	CRYLLabel*		m_pStorekeeperLabel ;		// 상점주인
	CRYLLabel*		m_pStorekeeperNameLabel ;	// 상점주인 값
	CRYLLabel*		m_pCurrentVisitor ;			// 현재입장객

	unsigned long	m_dwNameResult ;
	unsigned long	m_dwRegistItemResult ;
	unsigned long	m_dwRegistItemValue ;
	unsigned short	m_wItemIndex ;
	unsigned long	m_dwBuyResult ;
	unsigned long	m_dwItemNumber ;

	CItemInstance*	m_lpBuyItem ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;


public :
	
	CRYLStreetStallDlg() ;
	virtual ~CRYLStreetStallDlg() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	virtual bool SendChat(char* szChatMessage);
	virtual void AddItemList(CItemInstance* lpItem, bool bSound);

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 

	VOID	InitValue() ;

	BOOL	SelectItem() ;
	VOID	InitStreetStall( unsigned long dwChrID, LPSTR lpstrStreetName ) ;
	VOID	DeleteTradeItem();
	VOID	RemoveBuyItem(CItemInstance* lpItem) ;
		
	VOID	UpdateMessageBox() ;
} ;

#endif // __RYL_STREETSTALLDLG_H__