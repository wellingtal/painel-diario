// GUIMessageBox.h: interface for the CRYLMessageBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIMESSAGEBOX_H__F81A1529_1603_4C04_94BF_02E10D1898E7__INCLUDED_)
#define AFX_GUIMESSAGEBOX_H__F81A1529_1603_4C04_94BF_02E10D1898E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vertex.h>
#include <vector>
#include "RYLSprite.h"
#include "RYLDialog.h"

class CRYLListBox ;
class CRYLButton ;
class CRYLButton ;
class CRYLButton ;
class CRYLButton ;
class CRYLButton ;
class CRYLButton ;
class CRYLButton ;
class CRYLEdit ;
class CRYLEdit ;

using namespace std;

#define MB_NORMAL		0x00000000
#define MB_EXIT			0x00000001
#define MB_CONFIRM		0x00000002
#define MB_CANCEL		0x00000004
#define MB_YES			0x00000008
#define MB_NO			0x00000010
#define MB_PREV			0x00000020
#define MB_NEXT			0x00000040
#define MB_NUMBER		0x00000080
#define MB_STRING		0x00000100
#define MB_ARRAYCONFIRM	0x00000200
#define MB_THEATER		0x00000400
#define MB_STRINGEX		0x00000800

namespace GUIMESSAGEBOX
{
	const DWORD		NORMAL		= 0x00000000 ;
	const DWORD		EXIT		= 0x00000001 ;
	const DWORD		CONFIRM		= 0x00000002 ;
	const DWORD		CANCEL		= 0x00000003 ;
	const DWORD		YES			= 0x00000004 ;
	const DWORD		NO			= 0x00000005 ;
	const DWORD		PREV		= 0x00000006 ;
	const DWORD		NEXT		= 0x00000007 ;
	
	const DWORD		NUMBER		= 0x00000008 ;
	const DWORD		STRING		= 0x00000009 ;
} ;

// ==============================================
// by Ichabod
// 추가 데이타

#define MB_SUPPLEMENTDATA	0x00001000

class CRYLMessageBox : public CRYLDialog
{
public :

	CRYLListBox*	m_pDataListBox ;
	CRYLButton*		m_pExitButton ;
	CRYLButton*		m_pPrevButton ;
	CRYLButton*		m_pNextButton ;
	CRYLButton*		m_pConfirmButton ;
	CRYLButton*		m_pYesButton ;
	CRYLButton*		m_pCancelButton ;
	CRYLButton*		m_pNoButton ;
	CRYLEdit*		m_pNumberEdit ;
	CRYLEdit*		m_pStringEdit ;

	INT				m_iHeight ;
	BOOL			m_bAble ;

	typedef struct WORDNode
	{
		unsigned long m_dwNum;
		BOOL m_bFirstLink;
		vector<char *> m_lstMessage;
	} WORDNode, *LPWORDNode;

public:


	unsigned long*	m_lpResultValue;
	TLVertex		m_Vertexes[16];
	unsigned short	m_wIndex[54];
	unsigned long*	m_lpValue;
	char*			m_lpstrValue;
	CRYLSprite		m_spNumber;

	DWORD			m_dwDelayTick;

	vector<LPWORDNode>	m_lstMessage;
	CRYLMessageBox*		m_lpParent;
	long				m_lTick;

	float			m_fPosX;
	float			m_fPosY;
	RECT			m_rcRect;
	vector<char *>	m_lstWord;
	long			m_lMsgPage;

	BOOL			m_bWordBreak;
	BOOL			m_bShowEx;
	unsigned long	m_dwFlag;

	BOOL			m_bModal;

	BOOL			m_bEnableEnter;
	BOOL			m_bEnableBack;

	BOOL			m_bTimerDisable;

	CHAR			m_szSupplementData[ 4 ][ 128 ] ;


	CRYLMessageBox();
	virtual ~CRYLMessageBox();

	void Create( const char *strMessage, 
			        unsigned long dwFlag = MB_NORMAL, 
					float posX = 248.0f, float posY = 200.0f);

	void CreateEx( const char *strMessage = NULL, 
					const char *szTitle1	= NULL,
					const char *szData1	= NULL,
					const char *szTitle2	= NULL,
					const char *szData2	= NULL,
					unsigned long dwFlag = MB_NORMAL, 
					float posX = 248.0f, float posY = 200.0f) ;

	void Create( unsigned long dwFlag = MB_NORMAL, float posX = 248.0f, float posY = 200.0f);
	void Create( char* pszYes, char* pszNo, const char *strMessage, unsigned long dwFlag = MB_NORMAL, float posX = 248.0f, float posY = 200.0f ) ;
	BOOL ShowDialog(unsigned long dwChrID, unsigned long dwPage);

	void Destroy();

	void SetWordBreak(BOOL bWordBreak) { m_bWordBreak = bWordBreak; }
	void SetEnableEnter(BOOL bEnableEnter) { m_bEnableEnter = bEnableEnter; }
	void SetEnableBack(BOOL bEnableBack) { m_bEnableBack = bEnableBack; }
	void SetResult(unsigned long *lpValue) { m_lpResultValue = lpValue; }
	void SetValue(unsigned long *lpValue) { m_lpValue = lpValue; }
	void SetValue(char *lpstrValue) { m_lpstrValue = lpstrValue; }
	void SetText(const char *strMessage);
	void SetPosition(float fPosX, float fPosY);						//x, y는 메시지 박스의 left top이 아닌 정가운데점.
	void Show(BOOL bShow) { m_bShowEx = bShow; }
	void SetTimer(long lTick) { m_lTick = lTick; }
	void AddWord(char *strWord);

	void SetTimerDisable(BOOL bChk);

	void SetModal(BOOL bModal)
	{
		m_bModal = bModal;
	}

	BOOL UpdateEx(float fTick, BOOL &bClick, BOOL bAble);

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) { return S_OK ; };

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 

	VOID CreateDialogImage() ;
	VOID CreateControl( DWORD dwFlag, char* pszYesName = NULL, char* pszNoName = NULL ) ;

private:

	void RestoreFocus(void);
};

#endif // !defined(AFX_GUIMESSAGEBOX_H__F81A1529_1603_4C04_94BF_02E10D1898E7__INCLUDED_)
