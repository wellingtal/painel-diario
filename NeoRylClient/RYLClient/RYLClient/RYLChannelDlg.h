//================================================================
//
//	Name : RYLChannelDlg.h 
//	Desc : ü�� ���� ���̾�α�
//  Date : 2004. 06. 08
//
//================================================================
#ifndef __RYL_CHANNELDLG_H__
#define __RYL_CHANNELDLG_H__

#include "RYLDialog.h"

namespace RYLCHANNELDLG
{
	const DWORD		CHANNEL_LISTBOX			= 0x00000000 ;
	const DWORD		OK_BUTTON				= 0x00000001 ;
} ;

class CRYLButton ;
class CRYLListBox ;
class CRYLLabel ;
class CRYLImage ;
class CRYLPicture ;

class CRYLChannelDlg : public CRYLDialog
{
public:
	CRYLListBox*		m_pChannelListBox ;				// ä�� 1
	CRYLLabel*			m_pChannelSelectLabel1 ;		// ä�μ���
	CRYLLabel*			m_pChannelSelectLabel2 ;		// ä���� �����Ͽ� �ֽʽÿ�.
	CRYLPicture*		m_pChannelPicture[ 5 ] ;		// ä�� 1
	
	CRYLImage*			m_pChannelGoodImage ;				// ��ȣ
	CRYLImage*			m_pChannelNormalImage ;			// ����
	CRYLImage*			m_pChannelConfusionImage ;		// ȥ��
	
	CRYLButton*			m_pOKBtn ;						// OK Button

public :
	CRYLChannelDlg() ;
	virtual ~CRYLChannelDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	InitChannel() ;
} ;

#endif //__RYL_CHANNELDLG_H__