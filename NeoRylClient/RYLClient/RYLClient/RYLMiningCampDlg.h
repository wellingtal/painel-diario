//=================================================================
//
// Name : RYLMiningCampDlg.h
// Data : 2004. 12. 14
// Desc : Mining Camp Dialog Class, 채굴기 
//
//=================================================================
#ifndef __RYL_MININGCAMPDLG_H__
#define __RYL_MININGCAMPDLG_H__

class CRYLButton ;
class CRYLLabel ;
class CRYLCheckBox ;

namespace RYLMININGCAMPDLG
{
	const DWORD		PREV_BUTTON				= 0x00000000 ;
	const DWORD		NEXT_BUTTON				= 0x00000001 ;
	const DWORD		ON_BUTTON				= 0x00000002 ;
	const DWORD		OFF_BUTTON				= 0x00000003 ;
	const DWORD		GET_BUTTON_0			= 0x00000004 ;
	const DWORD		GET_BUTTON_1			= 0x00000005 ;
	const DWORD		GET_BUTTON_2			= 0x00000006 ;
	const DWORD		GET_BUTTON_3			= 0x00000007 ;
	const DWORD		GET_BUTTON_4			= 0x00000008 ;
	const DWORD		GET_BUTTON_5			= 0x00000009 ;
	const DWORD		GET_BUTTON_6			= 0x0000000a ;
	const DWORD		GET_BUTTON_7			= 0x0000000b ;

} ;



 

class CRYLMiningCampDlg : public CRYLDialog
{
private : 

	CRYLButton*			m_pPrevButton ;							// 페이즈 이동 버튼
	CRYLButton*			m_pNextButton ;							// 페이즈 이동 버튼
	CRYLButton*			m_pOnButton ;							// 채굴기 켜기 버튼
	CRYLButton*			m_pOffButton ;							// 채굴기 끄기 버튼
	CRYLButton*			m_pGetMineButton[8] ;					// 광물 획득 버튼

	CRYLLabel*			m_pDoneLabel ;							// 완료 라벨
	CRYLLabel*			m_pPhaseLabel ;							// 페이즈 라벨
	CRYLLabel*			m_pMineNameLabel[8] ;					// 광물 이름
	CRYLLabel*			m_pMineCountLabel[8] ;					// 광물 수량
	CRYLLabel*			m_pTitleLabel ;							// 광물의 매입량 확인 ( 타이틀 )
	std::vector< CItemInstance* >	m_vecMineIcon ;				// 광물 아이콘
	std::vector< int >	m_vecMineCnt;							// 광물 갯수

	int					m_nCurPhase ;							// phase 
	int					m_nMaxPhase ;							// 페이즈 맥스 값.
	
	DWORD				m_dwSplitResult ;
	DWORD				m_dwSplitValue ;
	DWORD				m_dwSplitIndex ;
	DWORD				m_dwCampID ;							// 캠프 아이디
	DWORD				m_dwRemainTime	;						// 완료까지 남은 날
protected:
	VOID				NextPhase();
	VOID				PrevPhase();
	VOID				SetPhaseLabel();
	VOID				GetMine( int index );					//광물 획득 버튼 ( 1~8 )
	VOID				MiningOn();								//채굴기 켜기
	VOID				MiningOff();							//캐굴기 끄기
	VOID				AddItem( unsigned short id, unsigned short amount );				
	VOID				ClearItem();

public :
	CRYLMiningCampDlg() ;
	virtual ~CRYLMiningCampDlg() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;

	
	VOID	SetCamp( DWORD dwCampID ){ m_dwCampID = dwCampID; };
	VOID	InitMineral();
	VOID	SetMineral( unsigned long dwCampID, unsigned char cState, unsigned long dwRemainTime, unsigned char cNum, CampMineralInfo* lpMineralList );
	VOID	SetCampOnOff( unsigned long dwCampID, unsigned char cState );
	BOOL	RemoveItem( unsigned short id, unsigned short amountleft );

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;
	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };
	HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; };


	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

} ;

#endif //__RYL_MININGCAMPDLG_H__