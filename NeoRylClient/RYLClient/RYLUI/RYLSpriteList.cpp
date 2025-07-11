#include "RYLClientMain.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "BaseDataDefine.h"
#include "RenderOption.h"
#include "BaseGraphicsLayer.h"
#include "GMMemory.h"

CRYLSpriteList*		CRYLSpriteList::s_pInstance = NULL ;

CRYLSpriteList::CRYLSpriteList() 
{
	s_pInstance = this ;
}

CRYLSpriteList::~CRYLSpriteList() 
{
}

HRESULT		CRYLSpriteList::CreateInstance()
{
//	GMASSERT( !s_pInstance, "CRYLSpriteList::s_pInstance Failed" ) ;
	s_pInstance = new CRYLSpriteList();
//	GMASSERT( s_pInstance, "CRYLSpriteList::s_pInstance Failed" ) ;

	return S_OK ;
}

HRESULT		CRYLSpriteList::DeleteInstance()
{
//	GMASSERT( s_pInstance, "CRYLSpriteList::s_pInstance Failed" ) ;
	GM_DELETE( s_pInstance ) ;

	return S_OK ;
}

HRESULT		CRYLSpriteList::InitSprite() 
{
	CHAR buff[ MAX_PATH ] ;
 	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\MainGui.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\MainGui02.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\MainGui03.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_MAINGUI03_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\MiniGui.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_MINIGUI_256x256 ) ) )
		return E_FAIL ;
	

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Compo.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) )
		return E_FAIL ;
	
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\button01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_BUTTON_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Inven_h_01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_INVEN_HUMAN01_256x256 ) ) )
		return E_FAIL ;
	
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Inven_a_01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_INVEN_AKHAN01_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Inven_h_02.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_INVEN_HUMAN02_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Inven_h_03.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_INVEN_HUMAN03_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\SkillSlot.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Stall01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_STREET_STALL01_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Stall02.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_STREET_STALL02_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Stall03.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_STREET_STALL03_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\QuestSlot.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_QUESTSLOT_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\bsmith.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_BLACKSMITH_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Chatwin_s.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CHATSMALL_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Chatwin_l.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CHATLARGE_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Rank01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RANKING01_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Rank02.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RANKING02_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Rank03.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RANKING03_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Exchg.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_EXCHANGE_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\GuildCompo.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_GUILD_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Login.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_256x256 ) ) )
		return E_FAIL ;

	// add by shadow
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Compo2.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_COMPONENT02_256x256 ) ) )
		return E_FAIL ;

	// add by shadow
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Status.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_STATUS_256x256 ) ) )
		return E_FAIL ;

	// add by shadow
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\NewLogin.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_NEWLOGIN_256x256 ) ) )
		return E_FAIL ;
	
    //---------------------------------------------------------------
	// 회사 로고

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Logo01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGO01 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Logo02.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGO02 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Logo03.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGO03 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Logo04_08_12.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGO04_08_12 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Logo05.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGO05 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Logo06.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGO06 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Logo07.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGO07 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Logo09_10.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGO09_10 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Logo11.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGO11 ) ) )
		return E_FAIL ;

	//--------------------------------------------------------------
	// 로그인 화면
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;//hz wasnt here
	strcat( buff, "\\LoginMain.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_MAIN ) ) )
		return E_FAIL ;//wasnt here 

	/*strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\LoginMain01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_MAIN01 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\LoginMain02.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_MAIN02 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\LoginMain03.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_MAIN03 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\LoginMain04_08_12.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_MAIN04_08_12 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\LoginMain05.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_MAIN05 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\LoginMain06.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_MAIN06 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\LoginMain07.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_MAIN07 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\LoginMain09_10.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_MAIN09_10 ) ) )
		return E_FAIL ;
	
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\LoginMain11.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOGIN_MAIN11 ) ) )
		return E_FAIL ;
*/
	//-------------------------------------------------------------
	// WORK_LIST 2.2 NationSelectScene 이름 수정
	// Race Select

	/*strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\sz01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RACE01 ) ) )
		return E_FAIL ;*/

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\NSBG.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RACE01 ) ) )
		return E_FAIL ;

	/*strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\sz02.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RACE02 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\sz03.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RACE03 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\sz04_08_12.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RACE04_08_12 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\sz05.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RACE05 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\sz06.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RACE06 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\sz07.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RACE07 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\sz09_10.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RACE09_10 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\sz11.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_RACE11 ) ) )
		return E_FAIL ;
*/
	//---------------------------------------------------------------
	// WORK_LIST 2.2 NationSelectScene 이름 수정
	// Check Box

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\NSH_1.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_HUMAN_RACE01_01 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\NSH_2.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_HUMAN_RACE01_02 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\NSA_1.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_AKHAN_RACE01_01 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\NSA_2.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_AKHAN_RACE01_02 ) ) )
		return E_FAIL ;


	/*strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\cslct_h01_01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_HUMAN_RACE01_01 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\cslct_h01_02.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_HUMAN_RACE01_02 ) ) )
		return E_FAIL ;*/

	/*strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\cslct_h02_01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_HUMAN_RACE02_01 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\cslct_h02_02.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_HUMAN_RACE02_02 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\cslct_h02_03.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_HUMAN_RACE02_03 ) ) )
		return E_FAIL ;*/

	//strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	//strcat( buff, "\\cslct_a01_01.gmi" ) ; 
	//if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_AKHAN_RACE01_01 ) ) )
	//	return E_FAIL ;

	//strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	//strcat( buff, "\\cslct_a01_02.gmi" ) ; 
	//if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_AKHAN_RACE01_02 ) ) )
	//	return E_FAIL ;

	/*strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\cslct_a02_01.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_AKHAN_RACE02_01 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\cslct_a02_02.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_AKHAN_RACE02_02 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\cslct_a02_03.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_AKHAN_RACE02_03 ) ) )
		return E_FAIL ;*/

	//---------------------------------------------------------------
	// WORK_LIST 2.3 계정 국적 변경 UI 추가
	// 계정 국적 관련

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\nation_kar.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_NATION_KARTERANT_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\nation_mer.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_NATION_MERKADIA_256x256 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\nation_pira.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_NATION_PIRATE_256x256 ) ) )
		return E_FAIL ;

	//---------------------------------------------------------------
	// 미니파티, 소환수 관련

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\SimplePnS.gmi" ) ; 
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_MINIPARTY_256x256 ) ) )
		return E_FAIL ;

    // Battle Ground 서버 선택/진입 창 UI
    strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
    strcat( buff, "\\Chslct.gmi" ) ;
    if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) ) )
		return E_FAIL ;

    strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
    strcat( buff, "\\Bgui01.gmi" ) ;
    if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_STATUS_256x256 ) ) )
		return E_FAIL ;

    // Battle Ground 서버군 게임 결과창 UI
    strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
    strcat( buff, "\\Bgui02.gmi" ) ;
    if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_RESULT_256x128 ) ) )
		return E_FAIL ;

    // Battle Ground 석상점령전 라지맵용 석상 아이콘
    strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
    strcat( buff, "\\Bgui03.gmi" ) ;
    if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_BATTLEGROUND_STONE_ICON_246x64 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
    strcat( buff, "\\Bar.gmi" ) ;
    if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_STATUS_BAR_256x256 ) ) )
		return E_FAIL ;

	// LOHAN UI AVALANSA
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
    strcat( buff, "\\Lohan.gmi" ) ;
    if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_LOHAN_256x256 ) ) )
		return E_FAIL ;
	
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
    strcat( buff, "\\IconCompo.gmi" ) ;
    if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CHANNEL ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
    strcat( buff, "\\Camp_k.gmi" ) ;
    if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_KARTERANT ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Camp_m.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_MERKADIA ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Camp_t.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_THIRD ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Camp_a.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_ATTACKED ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Camp_Simbol1.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_SIMBOL1 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Camp_Simbol2.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_SIMBOL2 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Camp_Simbol3.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_SIMBOL3 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\Camp_Simbol4.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_SIMBOL4 ) ) )
		return E_FAIL ;

	// alternative skill 
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\SkillSlot_alter.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_ALTERNATIVE_SKILL ) ) )
		return E_FAIL ;

	// 길드 요새 상점
	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\tshop01.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_SHOP01 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\tshop02.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_SHOP02 ) ) )
		return E_FAIL ;

	strcpy( buff, NEOINTERFACETEXTUREPATH ) ;
	strcat( buff, "\\tshop03.gmi" ) ;
	if ( FAILED( AddSprite( buff, ERYLSPRITENAME::RYL_CAMP_SHOP03 ) ) )
		return E_FAIL ;

	return S_OK ;
}

VOID		CRYLSpriteList::ClearVector() 
{
	if( !this ) return;	// by Belldandy : CreateInstance()도 못하고 뻑났을 때...

	if ( !m_vecSprite.empty() )
	{
		for ( std::vector< LPSPRITEINFO >::iterator it = m_vecSprite.begin(); it != m_vecSprite.end() ; ++ it ) 
		{
			GM_DELETE( ( *it )->m_pImage ) ;
			GM_DELETE( ( *it ) ) ;
		}

		m_vecSprite.clear() ;
	}
}

HRESULT		CRYLSpriteList::AddSprite( LPCTSTR lpszFile, INT iInstance ) 
{
	LPSPRITEINFO		lpSprite ;
	lpSprite = new RYLSPRITEINFO ; 
	lpSprite->m_iInstance = iInstance ;
	lpSprite->m_pImage = new CGMImageDib ;
	if ( lpSprite->m_pImage->LoadFromFile( ( CHAR* )lpszFile ) == -1 )
	{
		MessageBox( g_ClientMain.m_hWnd, lpszFile, "Error", MB_OK ) ;
		return E_FAIL ;
	}

	m_vecSprite.push_back( lpSprite ) ;

	return S_OK ;
}

HRESULT			CRYLSpriteList::DelSprite( INT iInstance ) 
{
	if ( (int)m_vecSprite.size() < iInstance )
		return E_FAIL ;

	std::vector< LPSPRITEINFO >::iterator it = 	m_vecSprite.begin() + iInstance;

	if ( ( *it ) )
	{
		GM_DELETE( ( *it ) ) ;
		m_vecSprite.erase( it ) ;

		return S_OK ;
	}

	return E_FAIL ;
}

CGMImageDib*	CRYLSpriteList::FindSprite( INT iInstance ) 
{
	if ( m_vecSprite[ iInstance ]->m_pImage )
		return m_vecSprite[ iInstance ]->m_pImage ;

	return NULL ;
}

CRYLImage*	CRYLSpriteList::GetImage( LPRECT lpRect, INT iInstance )
{
	CGMImageDib* pdestDib;
	CRYLImage* pImage;

	INT		iWidth  = lpRect->right - lpRect->left ;
	INT		iHeight = lpRect->bottom - lpRect->top ; 

	pdestDib = new CGMImageDib() ;
	pdestDib->SetSize( iWidth, iHeight, FALSE, EAIF_32Bit ) ;

	pdestDib->ClearbyColor( 0x00000000 ) ;
	pdestDib->Draw( -lpRect->left, -lpRect->top, FindSprite( iInstance ) ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pdestDib ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	GM_DELETE( pdestDib ) ;
	
	return pImage ;
}

VOID	CRYLSpriteList::CreateGMImage( INT iWidth, INT iHeight, CGMImageDib** ppImage )
{
	CGMImageDib* pGMImage = NULL ;
	pGMImage = new CGMImageDib() ;
	
	//=====================================================
	// Low texture도 32bit로 생성
	if ( BaseGraphicsLayer::m_iGraphicCards == 0 || 
		 BaseGraphicsLayer::m_iGraphicCards == 1 )
	{
		pGMImage->SetSize( iWidth, iHeight, FALSE, EAIF_16Bit ) ;
	}
	else
	{
		pGMImage->SetSize( iWidth, iHeight, FALSE, EAIF_32Bit ) ;		
	}
		
	*ppImage = pGMImage ;
}

LPRECT	CRYLSpriteList::GetDefaultButtonDisableRect( INT iFontSize )
{
	switch( iFontSize )
	{
	case 7 : SetRect( &m_rtDisableButton, 96, 195, 263, 228 ) ;	break ;
	case 6 : SetRect( &m_rtDisableButton, 0, 148, 92, 173 ) ;	break ;
	case 5 : SetRect( &m_rtDisableButton, 0, 0, 78, 25 ) ;		break ;
	case 4 : SetRect( &m_rtDisableButton, 78, 0, 144, 25 ) ;	break ;
	case 3 : SetRect( &m_rtDisableButton, 144, 0, 199, 25 ) ;	break ;
	case 2 : SetRect( &m_rtDisableButton, 199, 0, 243, 25 ) ;	break ;
	}
	return &m_rtDisableButton ;	
}

LPRECT	CRYLSpriteList::GetDefaultButtonNormalRect( INT iFontSize )
{
	switch( iFontSize )
	{
	case 13 : SetRect( &m_rtNormalButton, 0, 252, 173, 284 ) ;	break ; // new button by shadow  LOGIN
	case 12 : SetRect( &m_rtNormalButton, 0, 338, 173, 370 ) ;	break ; // new button by shadow  CANCEL
	case 11 : SetRect( &m_rtNormalButton, 0, 422, 156, 454 ) ; break ; // new button by shadow  REGISTER
    case 10 : SetRect( &m_rtNormalButton, 245, 0, 375, 100 ) ;	break ; // new button by shadow  FACEBOOK
	case 9 : SetRect( &m_rtNormalButton, 265, 55, 326, 95 ) ;	break ; // new button by shadow  Vault
	case 8 : SetRect( &m_rtNormalButton, 335, 54, 396, 94 ) ;	break ; // new button by shadow REPAIR
	case 7 : SetRect( &m_rtNormalButton, 405, 55, 466, 95 ) ;	break ; // new button by shadow NAME
	case 6 : SetRect( &m_rtNormalButton, 0, 173, 92, 198 ) ;	break ;
	case 5 : SetRect( &m_rtNormalButton, 0, 25, 78, 50 ) ;		break ;
	case 4 : SetRect( &m_rtNormalButton, 78, 25, 144, 50 ) ;	break ;
	case 3 : SetRect( &m_rtNormalButton, 144, 25, 199, 50 ) ;	break ;
	case 2 : SetRect( &m_rtNormalButton, 199, 25, 243, 50 ) ;	break ;
	}
	return &m_rtNormalButton ;	
}

LPRECT	CRYLSpriteList::GetDefaultButtonUpRect( INT iFontSize )
{
	switch( iFontSize )
	{
	case 13 : SetRect( &m_rtUpButton, 0, 295, 173, 327 ) ;	    break ; // new button by shadow  LOGIN
	case 12 : SetRect( &m_rtUpButton, 0, 381, 173, 413 ) ;	    break ; // new button by shadow  CANCEL
	case 11 : SetRect( &m_rtUpButton, 0, 464, 156, 496 ) ;	break ; // new button by shadow  REGISTER
	case 10 : SetRect( &m_rtUpButton, 373, 0, 503, 100) ;	    break ; // new button by shadow  FACEBOOK
	case 9 : SetRect( &m_rtUpButton, 265, 55, 326, 95 ) ;	    break ; // new button by shadow  Vault
	case 8 : SetRect( &m_rtUpButton, 335, 54, 396, 94 ) ;	    break ; // new button by shadow REPAIR
	case 7 : SetRect( &m_rtUpButton, 405, 55, 466, 95 ) ;	    break ; // new button by shadow NAME
	case 6 : SetRect( &m_rtUpButton, 0, 198, 92, 223 ) ;	    break ;
	case 5 : SetRect( &m_rtUpButton, 0, 50, 78, 75 ) ;		    break ;
	case 4 : SetRect( &m_rtUpButton, 78, 50, 144, 75 ) ;	    break ;
	case 3 : SetRect( &m_rtUpButton, 144, 50, 199, 75 ) ;	    break ;
	case 2 : SetRect( &m_rtUpButton, 199, 50, 243, 75 ) ;	    break ;
	}
	return &m_rtUpButton ;	
}

LPRECT	CRYLSpriteList::GetDefaultButtonDownRect( INT iFontSize )
{
	switch( iFontSize )
	{
	case 13 : SetRect( &m_rtDownButton, 0, 295, 173, 327 ) ;	break ; // new button by shadow  LOGIN
	case 12 : SetRect( &m_rtDownButton, 0, 381, 173, 413 ) ;	break ; // new button by shadow  CANCEL
	case 11 : SetRect( &m_rtDownButton, 0, 464, 156, 496 ) ;	break ; // new button by shadow  REGISTER
	case 10 : SetRect( &m_rtDownButton, 373, 0, 503, 100 ) ;	break ; // new button by shadow  FACEBOOK
	case 9 : SetRect( &m_rtDownButton, 265, 99, 326, 139 ) ;	break ; // new button by shadow  Vault
	case 8 : SetRect( &m_rtDownButton, 335, 99, 396, 139 ) ;	break ; // new button by shadow REPAIR
	case 7 : SetRect( &m_rtDownButton, 405, 99, 466, 139 ) ;	break ; // new button by shadow NAME
	case 6 : SetRect( &m_rtDownButton, 0, 223, 92, 248 ) ;	    break ;
	case 5 : SetRect( &m_rtDownButton, 0, 75, 78, 100 ) ;		break ;
	case 4 : SetRect( &m_rtDownButton, 78, 75, 144, 100 ) ;		break ;
	case 3 : SetRect( &m_rtDownButton, 144, 75, 199, 100 ) ;	break ;
	case 2 : SetRect( &m_rtDownButton, 199, 75, 243, 100 ) ;	break ;
	}
	return &m_rtDownButton ;	
}