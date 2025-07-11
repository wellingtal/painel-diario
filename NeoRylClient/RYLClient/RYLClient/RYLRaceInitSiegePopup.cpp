
#include <WinSock2.h>

#include "RYLRaceBase.h"
#include "RYLRaceBaseDefine.h"

#include "RYLStringTable.h"

#include "RYLNetWorkData.h"
#include "GMMemory.h"

VOID CRYLRaceBase::InitPopupEmblem( PopupMenu* pmAddMenu ) 
{
// CASTLE_TODO : 팝업 메뉴 막아두기

//	pmAddMenu->m_dwPopupKind = POPUPMENU_TAXCONTROL;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2500]);
//	m_lstPopupMenu.push_back(*pmAddMenu);
//
//	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLERIGHTSETTING;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2501]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);
//
//	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEEMBLEMUPGRADE;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2502]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);
//
//	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEEMBLEMUPGRADEEXPLANATION;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2503]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupGate( PopupMenu* pmAddMenu ) 		
{
// CASTLE_TODO : 팝엄 메뉴 막아두기

//	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEOPEN;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2505]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);
//
//	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLECLOSE;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2506]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);
//
//	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEDEFENCE;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2507]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);
//
//	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEUPGRADE;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2508]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);
//
//	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEREPAIR;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2509]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);
//
//	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLERECOVERY;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2510]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupBackDoor( PopupMenu* pmAddMenu ) 
{
	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEIN;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2511]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEOUT;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2512]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupCamp( PopupMenu* pmAddMenu ) 
{
	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPUPGRADE ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2513]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::CAMP_UPGRADE))
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_CHANGESHOP ;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2996]) ;
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_CHANGEMINING ;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2995]) ;
		m_lstPopupMenu.push_back(*pmAddMenu);
	}
		
	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPREPAIR;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2514]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPRIGHTSETTING;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2515]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPDESTROY;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2516]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPEXPLANATION;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2517]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupCampMining( PopupMenu* pmAddMenu ) 
{
	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPUPGRADE ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3062]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CHANGEBASE ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2997]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CHANGESHOP ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2996]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_MININGCAMPMENU ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3409]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPREPAIR;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3063]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPRIGHTSETTING;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3064]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPDESTROY;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3065]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_MININGCAMPEXPLANATION;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3066]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		pmAddMenu->m_dwPopupKind = POPUPMENU_SHOWTAXINFO;
//		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3862]) ;
//		m_lstPopupMenu.push_back(*pmAddMenu);
//	}
}

VOID CRYLRaceBase::InitPopupWorldWeapon( PopupMenu* pmAddMenu )
{
	/*
	0. 업그레이드
	1. 무기 장전
	2. 무기 발사
	3. 수리
	4. 권한 설정
	5. 파괴
	6. 도움말
	*/

	pmAddMenu->m_dwPopupKind = POPUPMENU_WEAPONUPGRADE;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3424] );
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_WEAPONLOAD;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3410] );
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_WEAPONLAUNCH;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3411] );
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_WEAPONREPAIR;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3412] );
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_WEAPONRIGHTSETTING;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3413] );
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_WEAPONDESTROY;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3414] );
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_WEAPONEXPLANATION;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3415] );
	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupCampShop( PopupMenu* pmAddMenu ) 
{
	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPUPGRADE ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3067]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CHANGEBASE ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2997]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CHANGEMINING ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2995]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPSHOPMENU ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3420]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPSHOPADMIN ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3193]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPREPAIR;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3068]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPRIGHTSETTING;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3069]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPDESTROY;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3070]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CAMPSHOPEXPLANATION;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3071]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		pmAddMenu->m_dwPopupKind = POPUPMENU_SHOWTAXINFO;
//		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3862]) ;
//		m_lstPopupMenu.push_back(*pmAddMenu);
//	}
}

VOID CRYLRaceBase::InitPopupCastleArmsNPC( PopupMenu* pmAddMenu ) 
{
	

// CASTLE_TODO : 팝업 메뉴 막아두기

//	pmAddMenu->m_dwPopupKind = POPUPMENU_GUARDDEVELOP ;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2519]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);

	//pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEDEFENCEARMSMENU;
	//strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2518]) ;
	//m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_SHORTDEFENCEARMSDEVELOP ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2520]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

//	pmAddMenu->m_dwPopupKind = POPUPMENU_LONGDEFENCEARMSDEVELOP ;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2521]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupCastleGuard( PopupMenu* pmAddMenu ) 
{
	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEGUARDREPAIR ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2522]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

// CASTLE_TODO : 팝업 메뉴 막아두기

//	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEGUARDUPGRADE ;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2523]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_CASTLEGUARDDESTROY ;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2524]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupShortRangeCastleArms( PopupMenu* pmAddMenu ) 
{
	pmAddMenu->m_dwPopupKind = POPUPMENU_SHORTCASTLEARMSRIDE;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2525]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_SHORTCASTLEARMSREPAIR;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2522]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

// CASTLE_TODO : 팝업 메뉴 막아두기

//	pmAddMenu->m_dwPopupKind = POPUPMENU_SHORTCASTLEARMSUPGRADE;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2523]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_SHORTCASTLEARMSDESTROY;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2524]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupLongRangeCastleArms( PopupMenu* pmAddMenu ) 
{
	pmAddMenu->m_dwPopupKind = POPUPMENU_LONGCASTLEARMSRIDE;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2525]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_LONGCASTLEARMSREPAIR;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2522]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

// CASTLE_TODO : 팝업 메뉴 막아두기

//	pmAddMenu->m_dwPopupKind = POPUPMENU_LONGCASTLEARMSUPGRADE;
//	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2523]) ;
//	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_LONGCASTLEARMSDESTROY;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2524]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupShortRangeSiegeArms( PopupMenu* pmAddMenu ) 
{
	pmAddMenu->m_dwPopupKind = POPUPMENU_SHORTSIEGEARMSRIDE;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2525]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_SHORTSIEGEARMSREPAIR;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2522]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_SHORTSIEGEARMSSTARTKITRECOVERY;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2526]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupLongRangeSiegeArms( PopupMenu* pmAddMenu ) 
{
	pmAddMenu->m_dwPopupKind = POPUPMENU_LONGSIEGEARMSRIDE;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2525]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_LONGSIEGEARMSREPAIR;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2522]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_LONGSIEGEARMSSTARTKITRECOVERY;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2526]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);
}

VOID CRYLRaceBase::InitPopupAirShip( PopupMenu* pmAddMenu ) 
{
	pmAddMenu->m_dwPopupKind = POPUPMENU_DROPSHIPRIDE;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2525]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_DROPSHIPREPAIR;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2522]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);

	pmAddMenu->m_dwPopupKind = POPUPMENU_DROPSHIPRECOVERY;
	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2526]) ;
	m_lstPopupMenu.push_back(*pmAddMenu);
}

