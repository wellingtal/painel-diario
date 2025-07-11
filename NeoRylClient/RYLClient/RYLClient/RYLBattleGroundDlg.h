//=================================================================
// Name : RYLBattleGroundDlg.h
// Data : 2003. 01. 08
// Desc : ��Ʋ�׶��� ���̾�α�
//=================================================================
#ifndef __RYL_BATTLEGROUNDDLG_H__
#define __RYL_BATTLEGROUNDDLG_H__

#include "RYLDialog.h"
#include "..\\ItemInstance.h"
#include <Network/Packet/PacketStruct/CharStatusPacket.h>

namespace RYLBATTLEGROUNDDLG
{
	const DWORD		CHANGEPOS_BUTTON					= 0x00000000 ;
} ;

class CRYLButton ;
class CRYLLabel ;
class CRYLProgressBar ;

typedef struct __STONESTATUERESPAWNINFOEX
{
	unsigned short usTurn_Out ;
	unsigned short usWaitNum_Out ;
	unsigned short usLeftTime_Out ;
	unsigned short usHumanNum_Out ;
	unsigned short usAkhanNum_Out ;
}STONESTATUERESPAWNINFOEX, *LPSTONESTATUERESPAWNINFOEX ;

class CRYLBattleGroundDlg : public CRYLDialog
{
public :
	CRYLLabel*			m_pWaitingRankingLabel ;			// ������
	CRYLLabel*			m_pWaitingRankingValueLabel ;		// ������ ��
	CRYLLabel*			m_pCurrentHumanLabel ;				// �����޸�
	CRYLLabel*			m_pCurrentHumanValueLabel ;			// �����޸� ��
	CRYLLabel*			m_pCurrentAkhanLabel ;				// �����ĭ
	CRYLLabel*			m_pCurrentAkhanValueLabel ;			// �����ĭ ��
	CRYLLabel*			m_pWaitingTimeLabel ;				// ���ð�
	CRYLLabel*			m_pWaitingTimeValueLabel ;			// ���ð� ��
	CRYLLabel*			m_pProgressStateLabel ;				// �����Ȳ
	CRYLLabel*			m_pProgressStateValueLabel ;		// �����Ȳ ��
	CRYLProgressBar*	m_pProgressStateProgrss ;			// �����Ȳ ���α׷�����
	CRYLButton*			m_pChangePositionButton ;			// ��ġ ����

	FLOAT				m_fProgressTime ; 
	LONG				m_lScreenFade ;
	ULONG				m_dwFadeMode ;
	BOOL				m_bShow ;
	BOOL				m_bRespawn;
	BOOL				m_bRespawnWait ;
	BOOL				m_bWarQuit ;

	LPSTONESTATUERESPAWNINFOEX		m_pStoneStatueRespawn ;
			
public :
	CRYLBattleGroundDlg() ;
	virtual ~CRYLBattleGroundDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )		 { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	VOID	SetMyRanking( LPSTR szMyRanking, LPSTR szTotalRanking ) ;	
	VOID	SetLeftoverTime( LPSTR szLeftoverTime )	;	
	VOID	SetCurrentHumanNum( LPSTR szCurrentHumanNum ) ;	
	VOID	SetCurrentAkhanNum( LPSTR szCurrentAkhanNum ) ;

	VOID	SetStoneStatueRespawnInfo( LPSTONESTATUERESPAWNINFOEX	pStoneStatueRespawn ) ;

	VOID    SetRespawnWait( BOOL bWait ) { m_bRespawnWait = bWait ; } ;
	BOOL	GetRespawnWait()			 { return m_bRespawnWait ; }
	VOID	SetWarQuit( BOOL bQuit )     { m_bWarQuit = bQuit ; } ;
	VOID	InitRespawnPoint() ;
} ;

#endif //__RYL_BATTLEGROUNDDLG_H__
