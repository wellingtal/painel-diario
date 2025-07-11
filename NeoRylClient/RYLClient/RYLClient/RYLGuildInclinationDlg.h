//=================================================================
// Name : RYLGuildInclinationDlg.h
// Data : 2003. 01. 12
// Desc : 길드 마크 다이얼로그
//=================================================================
#ifndef __RYL_GUILDINCLINATIONDLG_H__
#define __RYL_GUILDINCLINATIONDLG_H__

#include "RYLDialog.h"
#include <Community/Guild/GuildConstants.h>

unsigned char	GUILDINCLINATIONTYPE[];

class CRYLButton ;
class CRYLListBox ;
class CRYLSpriteEx ;
class CRYLLabel ;
class CRYLSprite ;
class CRYLBitmap ;
class CTexture ;
class CRYLStringGrid ;
class CRYLGuildInfoPanel;


namespace RYLGUILDINCLINATIONDLG
{
	const DWORD		MEMBER_STRINGGRID					= 0x00000000 ;
	const DWORD		INCLINATIONOK_BUTTON				= 0x00000001 ;
	const DWORD		INCLINATIONCANCEL_BUTTON			= 0x00000002 ;

	enum	INCLINATION
	{
		HUMAN_KARTERANT_WAR			= Guild::RACE_HUMAN | Guild::NATION_KARTERANT | Guild::MODE_WAR ,
		HUMAN_MERKADIA_WAR			= Guild::RACE_HUMAN | Guild::NATION_MERKADIA  | Guild::MODE_WAR ,
		HUMAN_NATION_THIRD_WAR		= Guild::RACE_HUMAN | Guild::NATION_THIRD		| Guild::MODE_WAR ,
		HUMAN_NATION_THIRD_PEACE	= Guild::RACE_HUMAN | Guild::NATION_THIRD		| Guild::MODE_PEACE ,

		AKHAN_KARTERANT_WAR			= Guild::RACE_AKHAN | Guild::NATION_KARTERANT | Guild::MODE_WAR ,
		AKHAN_MERKADIA_WAR			= Guild::RACE_AKHAN | Guild::NATION_MERKADIA  | Guild::MODE_WAR ,
		AKHAN_NATION_THIRD_WAR		= Guild::RACE_AKHAN | Guild::NATION_THIRD		| Guild::MODE_WAR ,
		AKHAN_NATION_THIRD_PEACE	= Guild::RACE_AKHAN | Guild::NATION_THIRD		| Guild::MODE_PEACE ,

		BOTH_KARTERANT_WAR			= Guild::RACE_BOTH  | Guild::NATION_KARTERANT | Guild::MODE_WAR ,
		BOTH_MERKADIA_WAR			= Guild::RACE_BOTH  | Guild::NATION_MERKADIA  | Guild::MODE_WAR ,
		BOTH_NATION_THIRD_WAR		= Guild::RACE_BOTH  | Guild::NATION_THIRD		| Guild::MODE_WAR ,
		BOTH_NATION_THIRD_PEACE		= Guild::RACE_BOTH  | Guild::NATION_THIRD		| Guild::MODE_PEACE 
	};
} ;


class CRYLGuildInclinationDlg : public CRYLDialog
{
protected :
	unsigned char		m_cNation;							// 길드장 종족
	unsigned char		m_cInclinationType;					// 선택된 길드 성향


	CRYLGuildInfoPanel*	m_pInfoPanel;
	CRYLLabel*			m_pInclinationChangeLabel;			// 길드 성향 변경 텍스트
	CRYLLabel*			m_pInclinationListLabel;			// 길드 성향 리스트 텍스트

	CRYLStringGrid*		m_pInclinationStringGrid ;			// 성향 리스트


	CRYLLabel*			m_pInclinationDescLabel;			// 성향 설명 텍스트
	CRYLLabel*			m_pInclinationDescValueLabel1;		// 성향 설명 값 텍스트	line 1
	CRYLLabel*			m_pInclinationDescValueLabel2;		// 성향 설명 값 텍스트	line 2
	CRYLLabel*			m_pInclinationDescValueLabel3;		// 성향 설명 값 텍스트	line 3
	CRYLLabel*			m_pInclinationDescValueLabel4;		// 성향 설명 값 텍스트	line 4

	CRYLLabel*			m_pInclinationAbleLabel;			// 길드 가입 가능 텍스트
	CRYLLabel*			m_pInclinationAbleValueLabel;		// 길드 가입 가능 값 텍스트

	CRYLLabel*			m_pInclinationNationLabel;			// 길드 국적 텍스트
	CRYLLabel*			m_pInclinationNationValueLabel;		// 길드 국적 값 텍스트

	CRYLLabel*			m_pInclinationPeaceLabel;			// 길드 전쟁/평화 성향 텍스트
	CRYLLabel*			m_pInclinationPeaceValueLabel;		// 길드 전쟁/평화 성향 값 텍스트

	CRYLLabel*			m_pInclinationCautionLabel1;		// 주의 텍스트	line 1
	CRYLLabel*			m_pInclinationCautionLabel2;		// 주의 텍스트	line 2

	CRYLButton*			m_pInclinationOKButton ;			// 성향 변경 확인 버튼
	CRYLButton*			m_pInclinationCancelButton ;		// 성향 변경 취소 버튼





	VOID				UpdateStringGrid();
	VOID				SetInclination();					// 현재 선택된 성향으로 성향 디스플레
	VOID				UpdateLabel( unsigned char type );						// 현재 선택된 성향으로 설명 문구 체인지 


	static unsigned char		Inclination2List( unsigned char type );			// 성향 타입(비트값) -> 리스트 타입(순차적)
	static unsigned char		List2Inclination( unsigned char type );			// 리스트 타입(순차적) -> 성향 타입(비트값)


public :
	CRYLGuildInclinationDlg( CRYLGuildInfoPanel * pInfoPanel ) ;
	virtual ~CRYLGuildInclinationDlg() ;

	VOID				InitCtrl() ;
	VOID				FinalCtrl() ;
	virtual VOID		GMUIPaint( INT xx, INT yy ) ;
	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;




	VOID				SetInclinationType( unsigned char type );
	unsigned char		GetInclinationType(){ return m_cInclinationType; };






	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID	DlgMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ); 
	static VOID	SetNation( unsigned char cNation );

} ;

#endif //__RYL_GUILDINCLINATIONDLG_H__
