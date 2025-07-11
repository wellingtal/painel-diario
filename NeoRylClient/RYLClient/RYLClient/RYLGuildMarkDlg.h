//=================================================================
// Name : RYLGuildMarkDlg.h
// Data : 2003. 01. 12
// Desc : ��� ��ũ ���̾�α�
//=================================================================
#ifndef __RYL_GUILDMARKDLG_H__
#define __RYL_GUILDMARKDLG_H__

#include "RYLDialog.h"
#include <vector>

namespace RYLGUILDMARKDLG
{
	const DWORD		CHANGEMARK_BUTTON			= 0x00000000 ;
	const DWORD		CANCEL_BUTTON				= 0x00000001 ;
	const DWORD		GUILDMARK_LISTBOX			= 0x00000002 ;
} ;

class CRYLButton ;
class CRYLListBox ;
class CRYLSpriteEx ;
class CRYLLabel ;
class CRYLSprite ;
class CRYLBitmap ;
class CTexture ;

typedef struct __MARKINFOEX
{
	TCHAR		szFileName[ MAX_PATH ] ;
	CRYLSprite*	pSprite ;
	CTexture*	pTex ;
	CRYLBitmap* pBMP ;

	__MARKINFOEX();
	~__MARKINFOEX();

} MARKINFOEX, *LPMARKINFOEX ;


class CRYLGuildMarkDlg : public CRYLDialog
{
public :

	CRYLLabel*			m_pMarkChangeLabel ;		// ��帶ũ ����
	CRYLSpriteEx*		m_pGuildMarkSprite ;		// ��� ��ũ ��������Ʈ
	CRYLListBox*		m_pMarkListBox ;			// ��� ��ũ �����̸�
	CRYLButton*			m_pChangeButton ;			// ��� ��ũ ���� ��ư
	CRYLButton*			m_pCancelButton ;			// ��� ��ư
		
	INT			m_SpriteIndex ;
	INT			m_iCurrentMarkSize ;

	std::vector< LPMARKINFOEX >		m_vecMARK ;

	unsigned long		m_dwGID ;					// ��ũ�� �ٲ� ����� ���̵�


public :

	CRYLGuildMarkDlg() ;
	virtual ~CRYLGuildMarkDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	GetSystemDirectoryBitmap() ;
	VOID	ClearVector() ;
	VOID	SelectGuildMark() ;
	VOID	ChangeGuildMarkButton() ;

	void	SetGID( unsigned long dwGID )	{ m_dwGID = dwGID;	}
} ;

#endif //__RYL_GUILDMARKDLG_H__
