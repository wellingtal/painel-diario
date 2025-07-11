//================================================================
//
//	Name : RYLSiegeTimeDlg.h 
//	Desc : 2005.7.4
//  Date : 저장 상태 확인 창.
//
//================================================================
#ifndef __RYL_SIEGETIMEDLG_H__
#define __RYL_SIEGETIMEDLG_H__

#include "RYLDialog.h"

class CRYLButton;
class CRYLLabel;

namespace RYLSIEGETIMEDLG
{
	const DWORD		GUILDWAR_JOIN_BUTTON	= 0x00000000;
	const DWORD		REALMWAR_JOIN_BUTTON	= 0x00000001;
};

class CRYLSiegeTimeDlg : public CRYLDialog
{
private:

	CRYLLabel*			m_pCaption;

	// 길드 전쟁 관련 UI.
	
	CRYLLabel*			m_pGuildLabel;
	CRYLButton*			m_pGuildButton;

	CRYLLabel*			m_pGuildTimeLabel;
	
	
	// 국가 전쟁 관련 UI.
	
	CRYLLabel*			m_pRealmLabel;
	CRYLButton*			m_pRealmButton;

	CRYLLabel*			m_pRealmTimeLabel;

	unsigned long		m_dwRealmResult;
	unsigned long		m_dwGuildResult;

	unsigned char		m_cRealmType;
	unsigned char		m_cGuildType;

public:

	CRYLSiegeTimeDlg(INT nLeft, INT nTop);
	virtual ~CRYLSiegeTimeDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge );

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y );	

public:

	void		SetGuildType(unsigned char cType);
	void		SetRealmType(unsigned char cType);

public :

	// Signton

//	static CRYLSiegeTimeDlg*	s_pInstance ;
//	static CRYLSiegeTimeDlg*	Instance()				{ return s_pInstance; }
} ;

#endif // __RYL_SIEGETIMEDLG_H__