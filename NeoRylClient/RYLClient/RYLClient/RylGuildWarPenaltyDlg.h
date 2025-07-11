//================================================================
//
//	Name : RYLSiegeTimeDlg.h 
//	Desc : 2005.7.4
//  Date : 무방비 페널티.
//
//================================================================
#ifndef __RYL_GUILDWARPENALTYDLG_H__
#define __RYL_GUILDWARPENALTYDLG_H__

#include "RYLDialog.h"
#include <Community/Guild/GuildConstants.h>

class CRYLLabel;
class CRYLProgressBar;

class CRYLGuildWarPenaltyDlg : public CRYLDialog
{
private:

	CRYLLabel*			m_pCaption;	
	
	CRYLLabel*			m_pText[4];	

	CRYLProgressBar*	m_pProgress;

	unsigned long		m_dwTick;

	char				m_szGuildName[Guild::MAX_GUILD_NAME_LEN];

	bool				m_bPenalty;

	unsigned char		m_cZone;

public:

	CRYLGuildWarPenaltyDlg();
	virtual ~CRYLGuildWarPenaltyDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice );
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice );
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge );

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y );	

public:

	void		SetPenalty(char* szGuildName);			

public :

	// Signton

	static CRYLGuildWarPenaltyDlg*	s_pInstance ;
	static CRYLGuildWarPenaltyDlg*	Instance()				{ return s_pInstance; }
} ;

#endif // __RYL_GUILDWARPENALTYDLG_H__