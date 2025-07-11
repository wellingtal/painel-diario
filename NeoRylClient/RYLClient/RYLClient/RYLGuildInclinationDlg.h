//=================================================================
// Name : RYLGuildInclinationDlg.h
// Data : 2003. 01. 12
// Desc : ��� ��ũ ���̾�α�
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
	unsigned char		m_cNation;							// ����� ����
	unsigned char		m_cInclinationType;					// ���õ� ��� ����


	CRYLGuildInfoPanel*	m_pInfoPanel;
	CRYLLabel*			m_pInclinationChangeLabel;			// ��� ���� ���� �ؽ�Ʈ
	CRYLLabel*			m_pInclinationListLabel;			// ��� ���� ����Ʈ �ؽ�Ʈ

	CRYLStringGrid*		m_pInclinationStringGrid ;			// ���� ����Ʈ


	CRYLLabel*			m_pInclinationDescLabel;			// ���� ���� �ؽ�Ʈ
	CRYLLabel*			m_pInclinationDescValueLabel1;		// ���� ���� �� �ؽ�Ʈ	line 1
	CRYLLabel*			m_pInclinationDescValueLabel2;		// ���� ���� �� �ؽ�Ʈ	line 2
	CRYLLabel*			m_pInclinationDescValueLabel3;		// ���� ���� �� �ؽ�Ʈ	line 3
	CRYLLabel*			m_pInclinationDescValueLabel4;		// ���� ���� �� �ؽ�Ʈ	line 4

	CRYLLabel*			m_pInclinationAbleLabel;			// ��� ���� ���� �ؽ�Ʈ
	CRYLLabel*			m_pInclinationAbleValueLabel;		// ��� ���� ���� �� �ؽ�Ʈ

	CRYLLabel*			m_pInclinationNationLabel;			// ��� ���� �ؽ�Ʈ
	CRYLLabel*			m_pInclinationNationValueLabel;		// ��� ���� �� �ؽ�Ʈ

	CRYLLabel*			m_pInclinationPeaceLabel;			// ��� ����/��ȭ ���� �ؽ�Ʈ
	CRYLLabel*			m_pInclinationPeaceValueLabel;		// ��� ����/��ȭ ���� �� �ؽ�Ʈ

	CRYLLabel*			m_pInclinationCautionLabel1;		// ���� �ؽ�Ʈ	line 1
	CRYLLabel*			m_pInclinationCautionLabel2;		// ���� �ؽ�Ʈ	line 2

	CRYLButton*			m_pInclinationOKButton ;			// ���� ���� Ȯ�� ��ư
	CRYLButton*			m_pInclinationCancelButton ;		// ���� ���� ��� ��ư





	VOID				UpdateStringGrid();
	VOID				SetInclination();					// ���� ���õ� �������� ���� ���÷�
	VOID				UpdateLabel( unsigned char type );						// ���� ���õ� �������� ���� ���� ü���� 


	static unsigned char		Inclination2List( unsigned char type );			// ���� Ÿ��(��Ʈ��) -> ����Ʈ Ÿ��(������)
	static unsigned char		List2Inclination( unsigned char type );			// ����Ʈ Ÿ��(������) -> ���� Ÿ��(��Ʈ��)


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
