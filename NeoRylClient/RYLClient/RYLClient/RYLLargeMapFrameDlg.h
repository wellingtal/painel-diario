//================================================================
//
//	Name : RYLLargeMapFrameDlg.h 
//	Desc : ���� �� ���̾�α�
//  Date : 2004. 04. 20
//
//================================================================
#ifndef __RYL_LARGEMAPFRAMEDLG_H__
#define __RYL_LARGEMAPFRAMEDLG_H__

#define RESPAWNPOS_MAX   100

#include "RYLDialog.h"
#include <Network/Packet/PacketStruct/CastlePacket.h>

class CRYLTabButton ;
class CRYLDialog ;
class CGMUICustomPanel ;
class CRYLSpriteEx ;
class CRYLSprite ;

struct QuestInstance ;

namespace RYLLARGEMAPFRAMEDLG
{
	const DWORD		TAB_BUTTON							= 0x00000000 ;
	const DWORD		LARGEMAP_PANEL						= 0x00000001 ;
//	const DWORD		LARGEMAPMONSTERDISTRIBUTE_PANEL		= 0x00000002 ;  ////////

	const DWORD		RESPAWN_BUTTON						= 0x00000002 ;
} ;

class CRYLLargeMapFrameDlg : public CRYLDialog
{
public:

	enum
	{
		MAX_ATTACK_FLAG		= 7,
		ATTACK_TEXT_TIME	= 10000
	};

	QuestInstance*		m_lpQuestTarget;

private :
	CRYLTabButton*		m_pTabButton ;
	CGMUICustomPanel*	m_pLargeMapPanel ;
	CGMUICustomPanel*	m_pLargeMapCampDistributePanel ;

//		CGMUICustomPanel*	m_pLargeMapMobDistributePanel ;  /////////
	
	CRYLSpriteEx*		m_pMyPosSprite ;
	CRYLSpriteEx*		m_pQuestSprite[4] ;
	CRYLSprite*			m_pPartySprite ;

	CRYLButton*			m_pSpawnButton[ RESPAWNPOS_MAX ] ;			// ������ ��ư

	BOOL				m_bPortal;
	BOOL				m_bClearPortal;

	struct StatueMapInfo
	{		
		StatueDisplayInfo	m_sStatue;					// ���� ����.

		// ���� ó�� ���� ����.

		unsigned char		m_cAlpha;
		unsigned long		m_dwTick;
		unsigned char		m_bAttackFlag;		
	};

	typedef std::map<unsigned long, StatueMapInfo*> StetueMap;

	StetueMap			m_mapStatue;

	BOOL				m_bStatue;

	// �ؽ�Ʈ ����� ���� ����.
	unsigned long		m_dwTextTick[2];

public :
	CRYLLargeMapFrameDlg() ;
	virtual ~CRYLLargeMapFrameDlg() ;

	VOID InitCtrl() ;
	VOID FinalCtrl() ;


	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge ) ;

	static VOID	DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;

	CRYLTabButton*		GetTabButton()				{ return m_pTabButton ;						}
	CGMUICustomPanel*	GetLargeMapDlg()			{ return m_pLargeMapPanel;					}
//	CGMUICustomPanel*   GetMobDistributePanel()		{ return m_pLargeMapMobDistributePanel ;	}  ///////////
	CGMUICustomPanel*   GetCampDistributePanel()	{ return m_pLargeMapCampDistributePanel ;	}

	VOID				SetShowLargeMap( BOOL bShowLargeMap ) ;
	BOOL				GetShowLargeMap();
	VOID				LoadLargeMapTexture() ;
	VOID				HideControl() ;
	VOID				ShowLargeMap( USHORT& pWaitNum_Out ) ;
	VOID                SetStoneState( unsigned char cIndex, unsigned wKID, unsigned char cState );
	BOOL				GetRespawnEnable();
	VOID				ShowQuestTarget( QuestInstance* lpQuest ) ;
	VOID				PlayerLevelChanged();

	VOID				SetFocusPanel( int iFocus ) ;
	
	VOID				MakeRespawnButton() ;
	VOID				ClearRespawnButton() ;
	void				SetDisableRespawn();						// ������ ��ư ��� Disable

	// Portal ���� �Լ�
	void				SetPortal(BOOL bPortal);
	BOOL				GetPortal();
	void				SetClearPortal(BOOL bClearPortal);
	BOOL				GetClearPortal();

	VOID				SetTabButtonName(BOOL bStatue);

	void				SetStatueTextTick(unsigned char cIndex, unsigned long dwTick)	{ m_dwTextTick[cIndex] = dwTick;	}
	unsigned long		GetStatueTextTick(unsigned char cIndex)							{ return m_dwTextTick[cIndex];		}
	
	const StatueDisplayInfo*	GetStatue(unsigned long dwIndex);									// �ش� �ε����� ���� ����ü�� �����Ѵ�.
	void						Update(unsigned long dwIndex, StatueDisplayInfo* sStatueInfo);		// �ش� �ε����� ���� ������ ������Ʈ �Ѵ�.

	VOID	SetStatue(bool bStatue) { m_bStatue = bStatue;	}
	BOOL	GetStatue()				{ return m_bStatue;		}
} ;

#endif