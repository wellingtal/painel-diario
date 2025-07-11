//=================================================================
// Name : RYLGameScene.h
// Data : 2003. 11. 13
// Desc : Game Scene
//			- public : CRYLSceneObject
//=================================================================

#ifndef __RYL_GAMESCENE_H__
#define __RYL_GAMESCENE_H__

#include "RYLSceneObject.h"

namespace RYLMAINWINDOW
{
	const DWORD		QUICKSLOT_WINDOW			= ( 1 << 0 ) ;
	const DWORD		STATUS_WINDOW				= ( 1 << 1 ) ;
	const DWORD		MINIMAP_WINDOW				= ( 1 << 2 ) ;
	const DWORD		ENCHANT_WINDOW				= ( 1 << 3 ) ;
	const DWORD		SIEGETIME_WINDOW			= ( 1 << 4 ) ;

	const DWORD		ALL_WINDOW					= QUICKSLOT_WINDOW | STATUS_WINDOW | MINIMAP_WINDOW | ENCHANT_WINDOW | SIEGETIME_WINDOW ;
} ;

class CRYLDialog ;
class CRYLImage ;

class CRYLGameScene : public CRYLSceneObject
{
public :

	CRYLGameScene( INT iID ) ;
	virtual ~CRYLGameScene() ;

	virtual HRESULT			SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual	HRESULT			Render( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT			FinalRender(  LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT			Update() ;
	
	VOID					InitResourceByRace() ;
	virtual HRESULT			InitResourceObject() ;
	virtual HRESULT			DeleteResourceObject() ;

	virtual HRESULT			FinalCleanup() ;
	virtual HRESULT			MsgProc( HWND hWnd, UINT msg, WPARAM, LPARAM lParam ) ;

	virtual VOID			SceneShow() ;
	virtual VOID			SceneHide() ;

	VOID					GameSceneDelete() ;
	VOID					DeleteSystemDlg() ;

	VOID					GUIVisible( BOOL bVisible ) ;
	VOID					MovieMode( BOOL bVisible  = FALSE ) ;
	
	CRYLDialog*				GetQuickSlotDlg()				{ return m_RYLQuickSlotDlg ;				}
	CRYLDialog*				GetStatusDlg()					{ return m_RYLStatusDlg ;					}
	CRYLDialog*				GetStatusDlg2()					{ return m_RYLStatusDlg2 ;					}//hz guigui
	CRYLDialog*				GetStatusDlg3()					{ return m_RYLStatusDlg3 ;					}//hz guigui
	CRYLDialog*				GetStatusAndQuestFrameDlg()		{ return m_RYLStatusAndQuestFrameDlg ;		}
	CRYLDialog*				GetSocietyFrameDlg()			{ return m_RYLSocietyFrameDlg ;				}
	CRYLDialog*				GetGuildFrameDlg()				{ return m_RYLGuildFrameDlg ;				}
	CRYLDialog*				GetHumanInventoryFrameDlg()		{ return m_RYLHumanInventoryFrameDlg ;		} 
    CRYLDialog*             GetNewTradeDlg()                { return m_RYLNewTradeDlg ;					}
	CRYLDialog*				GetStreetStallDlg()				{ return m_RYLStreetStallDlg ;				}
	CRYLDialog*				GetQuestOpeningDlg()			{ return m_RYLQuestOpeningDlg ;				}
	CRYLDialog*				GetQuestDetailDataDlg()			{ return m_RYLQuestDetailDataDlg ;			}
	CRYLDialog*				GetBlacksmithDlg()				{ return m_RYLBlacksmithDlg ;				}
	CRYLDialog*				GetChattingDlg()				{ return m_RYLChattingDlg ;					}
	CRYLDialog*				GetStatusRetrainDlg()			{ return m_RYLStatusRetrainDlg ;			}
	CRYLDialog*				GetRankingDlg()					{ return m_RYLRankingDlg;					}
	CRYLDialog*				GetBattleGroundDlg()			{ return m_RYLBattleGroundDlg ;				}
	CRYLDialog*				GetLargeMapDlg()				{ return m_RYLLargeMapFrameDlg ;			}				
	CRYLDialog*				GetHelpDlg()					{ return m_RYLHelpFrameDlg ;				}				
	CRYLDialog*				GetExchangeDlg()				{ return m_RYLExchangeDlg ;					}
	CRYLDialog*				GetMiniMapDlg()					{ return m_RYLMiniMapDlg ;					}
	CRYLDialog*				GetJobChangeDlg()				{ return m_RYLJobChangeDlg ;				}
	CRYLDialog*				GetSystemDlg()					{ return m_RYLSystemDlg ; 					}
	CRYLDialog*				GetOptionDlg()					{ return m_RYLOptionDlg ; 					}
	CRYLDialog*				GetVaultDlg()					{ return m_RYLVaultDlg ;					}
	CRYLDialog*				GetVaultPasswordDlg()			{ return m_RYLVaultPasswordDlg ;			}
	CRYLDialog*				GetVaultCertificationdDlg()		{ return m_RYLVaultCertificationdDlg ;		}
	CRYLDialog*				GetMiniPartyDlg()				{ return m_RYLMiniPartyDlg ;				}
	CRYLDialog*				GetSummonDlg()					{ return m_RYLSummonDlg ;					}
	CRYLDialog*				GetEnchantDlg()					{ return m_RYLEnchantDlg ;					}
    CRYLDialog*				GetBattleGroundPrevResultDlg()	{ return m_RYLBattleGroundPrevResultDlg;	}
    CRYLDialog*             GetBattleGroundStatusDlg()      { return m_RYLBattleGroundStatusDlg;		}
    CRYLDialog*             GetBattleGroundGameStatusDlg()  { return m_RYLBattleGroundGameStatusDlg;	}
    CRYLDialog*             GetBattleGroundChangeMoney()    { return m_RYLBattleGroundChangeMoneyDlg;	}
    CRYLDialog*             GetBattleGroundResultDlg()      { return m_RYLBattleGroundResultDlg;		}
    CRYLDialog*             GetGMReportDlg()				{ return m_RYLGMReportDlg;					}
	CRYLDialog*				GetChangeNameDlg()				{ return m_RYLChangeNameDlg;				}

	CRYLDialog*				GetSiegeTimeDlg()				{ return m_RYLSiegeTimeDlg;					}

	CRYLDialog*				GetJoystickConfigureDlg()		{ return m_RYLJoystickConfigureDlg;			}

	// 리밸런싱, 아이템 관련 다이얼로그
	CRYLDialog*				GetItemReparationDlg()			{ return m_RYLItemReparationDlg;			}
	CRYLDialog*				GetItemCompositionDlg()			{ return m_RYLItemCompositionDlg;			}
	CRYLDialog*				GetTempInventory()				{ return m_RYLTempInventoryDlg;				}

	// 공성관련 다이얼로그
	CRYLDialog*				GetCastleStatusFrameDlg()		{ return m_RYLCastleStatusFrameDlg ;		}
	CRYLDialog*				GetCastleMineralDlg()			{ return m_RYLCastleMineralDlg ;			}
	CRYLDialog*				GetCastleEmblemRightDlg()		{ return m_RYLCastleEmblemRightDlg ;		}
	CRYLDialog*				GetCastleDoorblockDlg()			{ return m_RYLCastleDoorblockDlg ;			}
	CRYLDialog*				GetArmsCreatMenuDlg()			{ return m_RYLArmsCreatMenuDlg ; 			}
	CRYLDialog*				GetEmblemUpgradeDlg()			{ return m_RYLEmblemUpgradeDlg ; 			}
	CRYLDialog*				GetCampRightDlg()				{ return m_RYLCampRightDlg ;				}
	CRYLDialog*				GetSiegeArmsCreatingDlg()		{ return m_RYLSiegeArmsCreatingDlg ;		}
	CRYLDialog*				GetCampUpgradeDlg()				{ return m_RYLCampUpgradeDlg ;				}
	CRYLDialog*				GetCampRepairDlg()				{ return m_RYLCampRepairDlg ;				}
	CRYLDialog*				GetCampCreatingDlg()			{ return m_RYLCampCreatingDlg ; 			}
	CRYLDialog*				GetCampShowTaxInfoDlg()			{ return m_RYLCampShowTaxInfoDlg ;			}
	CRYLDialog*				GetMiningAmountDlg()			{ return m_RYLMiningAmountDlg ; 			}
	CRYLDialog*				GetMiningCampDlg()				{ return m_RYLMiningCampDlg ;				}
	CRYLDialog*				GetMiningAmountProcessDlg()		{ return m_RYLMiningAmountProcessDlg ;		}
	CRYLDialog*				GetCampChangeDlg()				{ return m_RYLCampChangeDlg ;				}
	CRYLDialog*				GetWorldWeaponLoadDlg()			{ return m_RYLWorldWeaponLoadDlg ;			}
	CRYLDialog*				GetCampShopDlg()				{ return m_RYLCampShopDlg ;					}
	CRYLDialog*				GetArmsHPDlg()					{ return m_RYLArmsHPDlg ;					}
	CRYLDialog*				GetWorldWeaponLaunchDlg()		{ return m_RYLWorldWeaponLaunchDlg ;		}

	CRYLDialog*				GetGuildWarPenaltyDlg()			{ return m_RYLGuildWarPenaltyDlg ;			}

	CRYLDialog*				GetDialogControlDlg()			{ return m_RYLDialogControlDlg;				}

	CRYLDialog*				GetRespawnDlg()					{ return m_RYLRespawnDlg ;					}
	CRYLDialog*				GetBattleWaitDlg()				{ return m_RYLBattleWaitDlg ;				}
	// 스킬 툴팁
	CRYLDialog*				GetSkillTooltipDlg()			{ return m_RYLSkillTooltipDlg ;				}
	// 어빌리티 툴팁
	CRYLDialog*				GetAbilityTooltipDlg()			{ return m_RYLAbilityTooltipDlg ;				}

	VOID					SetVisibleInfo( DWORD dwDlgType, BOOL bVisible ) ;
	BOOL					GetVisibleInfo( DWORD dwDlgType ) ;

	VOID					SaveMainWindowPosToReg() ;

private:

	VOID					LoadMainWindowPosFromReg() ;
	
	struct RYLMainWindowInfo
	{
		INT		m_nDlgControlLeft, m_nDlgControlTop ;
		INT		m_nQuickSlotLeft, m_nQuickSlotTop ;
		INT		m_nStatusLeft, m_nStatusTop, m_nStatusExtensionLevel ;
		INT		m_nEnchantLeft, m_nEnchantTop ;
		INT		m_nMapLeft, m_nMapTop, m_nMapType ;
		INT		m_nChatLeft, m_nChatTop ;
		INT		m_nSiegeLeft, m_nSiegeTop;
		INT		m_nChatDlgType ;
		DWORD	m_dwVisibleFlag ;
		BOOL	m_bDlgControlVertical, m_bQuickSlotVertical ;
	} ;

	RYLMainWindowInfo		m_MainWindowInfo ;

	// Rodin : vector나 배열을 이용해서 관리할 수 있지 않을까요?

	CRYLDialog*		m_RYLStatusAndQuestFrameDlg ;
	CRYLDialog*		m_RYLSocietyFrameDlg ;
	CRYLDialog*		m_RYLGuildFrameDlg ;
	CRYLDialog*		m_RYLHumanInventoryFrameDlg ;
	CRYLDialog*		m_RYLStatusDlg ;
	CRYLDialog*		m_RYLStatusDlg2 ;//hz guigui
	CRYLDialog*		m_RYLStatusDlg3 ;//hz guigui

	CRYLDialog*		m_RYLDialogControlDlg ;
	CRYLDialog*		m_RYLQuickSlotDlg ;
    CRYLDialog*     m_RYLNewTradeDlg ;
	CRYLDialog*		m_RYLStreetStallDlg ;
	CRYLDialog*		m_RYLMiniMapDlg ;
	CRYLDialog*		m_RYLQuestOpeningDlg ;
	CRYLDialog*		m_RYLQuestDetailDataDlg ;
	CRYLDialog*		m_RYLBlacksmithDlg ;
	CRYLDialog*		m_RYLChattingDlg ;
	CRYLDialog*		m_RYLStatusRetrainDlg ;
	CRYLDialog*		m_RYLRankingDlg ;
	CRYLDialog*		m_RYLBattleGroundDlg ;
	CRYLDialog*		m_RYLLargeMapFrameDlg ;
	CRYLDialog*		m_RYLHelpFrameDlg ;
	CRYLDialog*		m_RYLExchangeDlg ;
	CRYLDialog*		m_RYLJobChangeDlg ;
	CRYLDialog*		m_RYLSystemDlg ;
	CRYLDialog*		m_RYLOptionDlg ;
	CRYLDialog*		m_RYLVaultDlg ;
	CRYLDialog*		m_RYLVaultPasswordDlg ;
	CRYLDialog*		m_RYLVaultCertificationdDlg ;
	CRYLDialog*		m_RYLMiniPartyDlg ;
	CRYLDialog*		m_RYLSummonDlg ;
	CRYLDialog*		m_RYLEnchantDlg ;
    CRYLDialog*     m_RYLBattleGroundPrevResultDlg;
    CRYLDialog*     m_RYLBattleGroundStatusDlg;
    CRYLDialog*     m_RYLBattleGroundGameStatusDlg;
	CRYLDialog*     m_RYLBattleGroundChangeMoneyDlg;
    CRYLDialog*     m_RYLBattleGroundResultDlg;
    CRYLDialog*     m_RYLGMReportDlg;
	CRYLDialog*		m_RYLChangeNameDlg;

	CRYLDialog*		m_RYLSiegeTimeDlg;

	CRYLDialog*		m_RYLJoystickConfigureDlg;

	CRYLDialog*		m_RYLGuildWarPenaltyDlg;

	// 2004. 6. 23 
	// 리밸런싱, 아이템 관련 다이얼로그
	CRYLDialog*		m_RYLItemReparationDlg ;				// 아이템 보상 판매 다이얼로그
	CRYLDialog*		m_RYLItemCompositionDlg ;				// 아이템 옵션 이식 다이얼로그
	CRYLDialog*		m_RYLTempInventoryDlg ;					// 임시 인벤토리 다이얼로그

	//--------------------------------------------------
	// 공성관련 다이얼로그

	CRYLDialog*		m_RYLCastleStatusFrameDlg ;
	CRYLDialog*		m_RYLCastleMineralDlg ;
	CRYLDialog*		m_RYLCastleEmblemRightDlg ;
	CRYLDialog*		m_RYLCastleDoorblockDlg ;
	CRYLDialog*		m_RYLArmsCreatMenuDlg ;
	CRYLDialog*		m_RYLEmblemUpgradeDlg ;
	CRYLDialog*		m_RYLCampRightDlg ;
	CRYLDialog*		m_RYLSiegeArmsCreatingDlg ;
	CRYLDialog*		m_RYLCampUpgradeDlg ;
	CRYLDialog*		m_RYLCampRepairDlg ;
	CRYLDialog*		m_RYLCampCreatingDlg ;
	CRYLDialog*		m_RYLCampShowTaxInfoDlg ;
	CRYLDialog*		m_RYLMiningAmountDlg ;
	CRYLDialog*		m_RYLMiningCampDlg ;
	CRYLDialog*		m_RYLMiningAmountProcessDlg ;
	CRYLDialog*		m_RYLCampChangeDlg ;
	CRYLDialog*		m_RYLWorldWeaponLaunchDlg;
	CRYLDialog*		m_RYLWorldWeaponLoadDlg;
	CRYLDialog*		m_RYLCampShopDlg;
	CRYLDialog*		m_RYLArmsHPDlg;

	CRYLDialog*		m_RYLRespawnDlg ;
	CRYLDialog*		m_RYLBattleWaitDlg ; 

	//--------------------------------------------------
	// 스킬 툴팁

	CRYLDialog*		m_RYLSkillTooltipDlg ;
	CRYLDialog*		m_RYLAbilityTooltipDlg ;

	// Movie Mode
	CRYLImage*		m_pUpMovieBarImage ;
	CRYLImage*		m_pDownMovieBarImage ;
	int				m_f2ndBarSY ;
	int				m_iBarHeight ;


	// ---------------------------------------------------------
	// 무비 모드가 되기 전 UI 상태를 기록하기 위한 변수들

	BOOL 	m_bRYLStatusAndQuestFrameDlg;	
	BOOL 	m_bRYLSocietyFrameDlg;
	BOOL 	m_bRYLGuildFrameDlg;
	BOOL 	m_bRYLHumanInventoryFrameDlg; 
	BOOL 	m_bRYLStatusDlg;
	BOOL 	m_bRYLStatusDlg2;//hz guigui
	BOOL 	m_bRYLStatusDlg3;//hz guigui
	BOOL 	m_bRYLDialogControlDlg;		
	BOOL 	m_bRYLQuickSlotDlg;			
    BOOL 	m_bRYLNewTradeDlg;
	BOOL 	m_bRYLStreetStallDlg;			
	BOOL 	m_bRYLMiniMapDlg;				
	BOOL 	m_bRYLQuestOpeningDlg;		
	BOOL 	m_bRYLQuestDetailDataDlg;		
	BOOL 	m_bRYLBlacksmithDlg;			
	BOOL 	m_bRYLStatusRetrainDlg;		
	BOOL 	m_bRYLRankingDlg;				
	BOOL 	m_bRYLBattleGroundDlg;		
	BOOL 	m_bRYLLargeMapFrameDlg;
	BOOL 	m_bRYLHelpFrameDlg;
	BOOL 	m_bRYLExchangeDlg;			
	BOOL 	m_bRYLJobChangeDlg;			
	BOOL 	m_bRYLOptionDlg;				
	BOOL 	m_bRYLVaultDlg;				
	BOOL 	m_bRYLVaultPasswordDlg;		
	BOOL 	m_bRYLVaultCertificationdDlg; 
	BOOL 	m_bRYLMiniPartyDlg;			
	BOOL 	m_bRYLSummonDlg;				
	BOOL 	m_bRYLSystemDlg;
	BOOL 	m_bRYLChattingDlg;
	BOOL 	m_bRYLEnchantDlg;
	BOOL 	m_bRYLBattleGroundPrevResultDlg;
    BOOL 	m_bRYLBattleGroundStatusDlg;
    BOOL 	m_bRYLBattleGroundGameStatusDlg;
    BOOL 	m_bRYLBattleGroundChangeMoneyDlg;
    BOOL 	m_bRYLBattleGroundResultDlg;
    BOOL 	m_bRYLGMReportDlg;
	BOOL 	m_bRYLItemReparationDlg;
	BOOL 	m_bRYLItemCompositionDlg;
	BOOL 	m_bRYLTempInventoryDlg;
	BOOL	m_bRYLCastleStatusFrameDlg;
	BOOL	m_bRYLCastleMineralDlg;
	BOOL 	m_bRYLCastleEmblemRightDlg;
    BOOL 	m_bRYLTaxrateDlg;
	BOOL 	m_bRYLCastleDoorblockDlg;
	BOOL 	m_bRYLTaxpresentDlg;
	BOOL 	m_bRYLArmsCreatMenuDlg;
	BOOL 	m_bRYLEmblemUpgradeDlg;
	BOOL 	m_bRYLCampRightDlg;
	BOOL 	m_bRYLSiegeArmsCreatingDlg;
	BOOL 	m_bRYLCampUpgradeDlg;
	BOOL	m_bRYLCampRepairDlg;
	BOOL 	m_bRYLCampCreatingDlg;
	BOOL	m_bRYLCampShowTaxInfoDlg;
	BOOL	m_bRYLMiningAmountDlg ;
	BOOL	m_bRYLMiningCampDlg ;
	BOOL	m_bRYLMiningAmountProcessDlg ;
	BOOL	m_bRYLCampChangeDlg ;
	BOOL 	m_bRYLRespawnDlg;
	BOOL 	m_bRYLBattleWaitDlg;
	BOOL 	m_bRYLSkillTooltipDlg;
	BOOL	m_bRYLAbilityTooltipDlg;
	BOOL	m_bRYLChangeNameDlg;
	BOOL	m_bRYLWorldWeaponLaunchDlg;
	BOOL	m_bRYLWorldWeaponLoadDlg;
	BOOL	m_bRYLCampShopDlg;
	BOOL	m_bRYLArmsHPDlg;
	BOOL	m_bRYLSiegeTimeDlg;
	BOOL	m_bRYLGuildWarPenaltyDlg;
	BOOL	m_bRYLJoystickConfigureDlg;

	// ---------------------------------------------------------
} ;

#endif //__RYL_GAMESCENE_H__
