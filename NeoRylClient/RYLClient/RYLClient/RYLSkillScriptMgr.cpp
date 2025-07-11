#include <Utility/math/Math.h>
#include "RYLSkillScriptMgr.h"
#include "RYLStringTable.h"

extern "C"
{
	#include "../LUA/include/lua.h"
	#include "../LUA/include/lualib.h"
	#include "../LUA/include/lauxlib.h"
}

#include "RYLGameData.h"
#include "RYLSkillDescDlg.h"
#include "GMMemory.h"

CRYLSkillScriptMgr*	CRYLSkillScriptMgr::ms_pInstance = NULL ;

std::map< unsigned long, LPRYLSKILLNODE >	CRYLSkillScriptMgr::m_mapSkillNode ;
LPRYLSKILLNODE								CRYLSkillScriptMgr::m_lpTmpSkillNode = NULL ;

static const luaL_reg SkillTooltip_methods[] = {
	{ "SkillBegin",			CRYLSkillScriptMgr::SkillTooltip_SkillBegin },
	{ "SkillEnd",			CRYLSkillScriptMgr::SkillTooltip_SkillEnd },
	{ "SkillGradeName",		CRYLSkillScriptMgr::SkillTooltip_SkillGradeName },
	{ "SkillGrade",			CRYLSkillScriptMgr::SkillTooltip_SkillGrade },
	{ "SkillType",			CRYLSkillScriptMgr::SkillTooltip_SkillType },
	{ "SkillDesc",			CRYLSkillScriptMgr::SkillTooltip_SkillDesc },
	{ 0, 0 }
} ;

static const luaL_reg SkillTooltip_meta[] = {
	/*
	{ "__gc",       SkillTooltip_gc },
	{ "__tostring", SkillTooltip_tostring },
	{ "__add",      SkillTooltip_add },
	{ "__sub",		SkillTooltip_sub },
	*/
	{ 0, 0 }
};
/*
static unsigned long BiToHe(char cBin)
{
	if((cBin >= '0') && (cBin <= '9')) 
    {
		return (cBin - '0');
	}
    else if((cBin >= 'A') && (cBin <= 'F'))
    {
		return (cBin - 'A' + 0xA);
	}
    if((cBin >= 'a') && (cBin <= 'f'))
    {
		return (cBin -'a' + 0xA);
	}

	return cBin;
}

static unsigned long StrToHex32( const char *szSrc )
{
	const char*     pStart	= szSrc + 2;
	WORD        	sHex	= 0;

	for (int i=0; i<2; i++) 
    {
		WORD	s1	= BiToHe(*pStart++);
		s1 <<= (8*(1-i)+4);
		
		WORD	s2	= BiToHe(*pStart++);
		s2 <<= (8*(1-i));

		WORD	sRet =  s1  + s2;
		sHex += sRet;
	}

	return sHex;
}

static unsigned long ConvertDextoHex( const char* szSrc )
{
	return (0 == strncmp(szSrc, "0x", 2)) ? StrToHex32(szSrc) : _ttol(szSrc);	
}
*/

CRYLSkillScriptMgr* CRYLSkillScriptMgr::Instance()
{
	assert( 0 != ms_pInstance && "Not initialized CRYLSkillScript Manager instance !!" ) ;
	return ms_pInstance ;
}

CRYLSkillScriptMgr::CRYLSkillScriptMgr() 
{
	assert( 0 == ms_pInstance && "Already exist CRYLSkillScript Manager instance !!" ) ;
	m_lpTmpSkillNode = NULL ;
	m_LUA			 = NULL ;

	ms_pInstance = this ;

	LuaMethodRegister() ;
}

CRYLSkillScriptMgr::~CRYLSkillScriptMgr() 
{
	if ( m_LUA )
	{
		lua_close( m_LUA ) ;
	}

	ClearScriptData() ;
}

void		CRYLSkillScriptMgr::LuaMethodRegister()
{
	m_LUA = lua_open() ;

	luaopen_base( m_LUA ) ;
	luaopen_table( m_LUA ) ;
	luaopen_io( m_LUA ) ;
	luaopen_string( m_LUA ) ;
	luaopen_math( m_LUA ) ;
	luaopen_debug( m_LUA ) ;

	SkillMethodRegister() ;
}

int 		CRYLSkillScriptMgr::SkillMethodRegister()
{
	luaL_openlib( m_LUA, SKILL_TOOLTIP, SkillTooltip_methods, 0 ) ;	/* create methods table,
																	   add it to the globals */
	luaL_newmetatable( m_LUA, SKILL_TOOLTIP ) ;						/* create metatable for Foo,
																	   and add it to the Lua registry */
	luaL_openlib( m_LUA, 0, SkillTooltip_meta, 0 ) ;				/* fill metatable */
	
	lua_pushliteral( m_LUA, "__index" ) ;
	lua_pushvalue( m_LUA, -3 ) ;									/* dup methods table*/
	lua_rawset( m_LUA, -3 ) ;										/* metatable.__index = methods */
	
	lua_pushliteral( m_LUA, "__metatable" ) ;
	lua_pushvalue( m_LUA, -3 ) ;									/* dup methods table*/
	lua_rawset( m_LUA, -3 ) ;										/* hide metatable:
																	   metatable.__metatable = methods */
	lua_pop( m_LUA, 1 ) ;											/* drop metatable */

	return 1;														/* return methods on the stack */
}

void		CRYLSkillScriptMgr::ClearScriptData()
{
	m_lpTmpSkillNode = NULL  ;

	if ( !CRYLSkillScriptMgr::m_mapSkillNode.empty() )
	{
		std::map< unsigned long, LPRYLSKILLNODE >::iterator		mapiter = CRYLSkillScriptMgr::m_mapSkillNode.begin() ;
		std::vector<std::string>::iterator						veciter ;
		while( mapiter != CRYLSkillScriptMgr::m_mapSkillNode.end() )
		{
			veciter = ( mapiter->second )->s_vecSkillTooltip.begin() ;
			while( veciter != ( mapiter->second )->s_vecSkillTooltip.end() )
			{
				( *veciter ).clear() ;
				++veciter ;
			}

			veciter = ( mapiter->second )->s_vecSkillGrade.begin() ;
			while( veciter != ( mapiter->second )->s_vecSkillGrade.end() )
			{
				( *veciter ).clear() ;
				++veciter ;
			}

			veciter = ( mapiter->second )->s_vecSkillDesc.begin() ;
			while( veciter != ( mapiter->second )->s_vecSkillDesc.end() )
			{
				( *veciter ).clear() ;
				++veciter ;
			}

			( mapiter->second )->s_strSkillType.clear() ;
			( mapiter->second )->s_strSkillGradeName.clear() ;

			GM_DELETE( mapiter->second ) ;

			++ mapiter ;
		}

		CRYLSkillScriptMgr::m_mapSkillNode.clear() ;	
	}
}

bool		CRYLSkillScriptMgr::SkillScriptLoad( const char* pszFileName ) 
{
	ClearScriptData() ;

	lua_dofile( m_LUA, pszFileName ) ;

	lua_getglobal( m_LUA, "SetWeaponType" ) ;
	int luaResult = lua_pcall( m_LUA, 0, 1, 0 ) ;
	lua_pop( m_LUA, 1 ) ;

	return true ;
}

//=============================================
// Desc : Skill Node 시작 블럭설정
// Val :  - int : 스킬 ID
//=============================================
int			CRYLSkillScriptMgr::SkillTooltip_SkillBegin( lua_State *L )
{
	unsigned long dwSkillID = 0L ;
	const char* pszStr = luaL_optstring( L, 1, 0 ) ;
	if ( pszStr )
	{
		//dwSkillID = ConvertDextoHex( pszStr ) ;
		dwSkillID = Math::Convert::Atos( pszStr ) ;
	}

	//m_lpTmpSkillNode = pushSkillTooltip( L ) ;
	m_lpTmpSkillNode = new RYLSKILLNODE ;
	m_lpTmpSkillNode->s_dwSkillID = dwSkillID ;

	CRYLSkillScriptMgr::m_mapSkillNode.insert( std::pair< unsigned long, LPRYLSKILLNODE>( dwSkillID, m_lpTmpSkillNode ) ) ;

	return 1 ;
}

//=============================================
// Desc : Skil Node 끝 블럭 설정
//=============================================
int  CRYLSkillScriptMgr::SkillTooltip_SkillEnd( lua_State *L ) 
{
	m_lpTmpSkillNode = NULL ;
	return 1 ;
}

//=============================================
// Desc : 스킬 계열 이름
//=============================================
int  CRYLSkillScriptMgr::SkillTooltip_SkillGradeName( lua_State *L ) 
{
	const char* pszStr = luaL_optstring( L, 1, 0 ) ;
	char* pStr = new char[ strlen( pszStr ) + 1 ] ;
	strcpy( pStr, pszStr ) ;
	m_lpTmpSkillNode->s_strSkillGradeName = pStr ;
	delete [] pStr ;
	pStr = NULL ;

	return 1 ;
}

//=============================================
// Desc : 스킬 계열
//=============================================
int  CRYLSkillScriptMgr::SkillTooltip_SkillGrade( lua_State *L ) 
{
	const char* pszStr = luaL_optstring( L, 1, 0 ) ;
	char* pStr = new char[ strlen( pszStr ) + 1 ] ;
	strcpy( pStr, pszStr ) ;
	std::string str = pStr ;
	m_lpTmpSkillNode->s_vecSkillGrade.push_back( str ) ;
	delete [] pStr ;

	return 1 ;
}

//=============================================
// Desc : 스킬 타입
//=============================================
int  CRYLSkillScriptMgr::SkillTooltip_SkillType( lua_State *L ) 
{
	const char* pszStr = luaL_optstring( L, 1, 0 ) ;
	char* pStr = new char[ strlen( pszStr ) + 1 ] ;
	strcpy( pStr, pszStr ) ;
	m_lpTmpSkillNode->s_strSkillType = pStr ;
	delete [] pStr ;

	return 1 ;
}

//=============================================
// Desc : 스킬 설명
//=============================================
int  CRYLSkillScriptMgr::SkillTooltip_SkillDesc( lua_State *L ) 
{
	const char* pszStr = luaL_optstring( L, 1, 0 ) ;
	char* pStr = new char[ strlen( pszStr ) + 1 ] ;
	strcpy( pStr, pszStr ) ;
	CRYLSkillScriptMgr::ProcessScriptString( pStr, SKILL_DESC ) ;
	delete [] pStr ;

	return 1 ;
}

RYLSKILLNODE* CRYLSkillScriptMgr::pushSkillTooltip( lua_State* L )
{
	RYLSKILLNODE* lpSkill = static_cast< RYLSKILLNODE* >( lua_newuserdata( L, sizeof( RYLSKILLNODE ) ) ) ;
	luaL_getmetatable( L, SKILL_TOOLTIP ) ;
	lua_setmetatable( L, -2 ) ;

	return lpSkill ;
}

void	CRYLSkillScriptMgr::ProcessScriptString( char* pszData, unsigned long dwType, LPRYLSKILLNODE pSkillNode ) 
{
	char *strMessageTemp, *token, seps[] = "\\";

	switch ( dwType )
	{
		case SKILL_DESC :
			strMessageTemp = CRYLStringTable::MakeWordBreak(pszData, 213) ;
			break ;

		case SKILL_TIP :
			strMessageTemp = pszData ;
			break ;
	}
	
	token = strtok(strMessageTemp, seps);

	while(token != NULL)
	{
		std::string str ;
		
		str = SkipPointNumber( token ) ;
		
		switch( dwType )
		{
		case SKILL_DESC :
			m_lpTmpSkillNode->s_vecSkillDesc.push_back( str ) ;
			break ;

		case SKILL_TIP :
			pSkillNode->s_vecSkillTooltip.push_back( str ) ;
			break ;
		}
		
		token = strtok(NULL, seps);
	}
}

std::string		CRYLSkillScriptMgr::SkipPointNumber( LPSTR lpStr )
{
	std::string	   strResult ;
	LPSTR lpTmpStr = lpStr ;
	char  szResultStr[ MAX_PATH ] ;

	int  iSrcIdx = 0 ; 
	int  iDestIdx = 0 ;
	int  len = strlen( lpTmpStr ) ;

	if ( lpTmpStr[ 1 ] == '@' )
	{
		int iPtUnder = atoi( &lpTmpStr[ 0 ] ) ;
		for ( int i = 2 ; i < len ; i ++ )
		{
			if ( lpTmpStr[ i ] == '.' )
			{
				if ( iPtUnder != 0 )
				{
					for ( int j = 0 ; j <= iPtUnder ; j ++, i ++ )
					{
						szResultStr[ iDestIdx ] = lpTmpStr[ i ] ;
						++ iDestIdx ;
					}
				}

				++ i ;
				while( true )
				{
					if ( lpTmpStr[ i ] >= 0x30 && lpTmpStr[ i ] <= 0x39 )
					{
						++ i ;
					}
					else
					{
						-- i ;
						break ;
					}
				}
			}
			else
			{
				szResultStr[ iDestIdx ] = lpTmpStr[ i ] ;
				++ iDestIdx ;
			}
		}
	}
	else
	{
		return lpTmpStr ;
	}
	
	szResultStr[ iDestIdx ] = '\0' ;
	strResult = szResultStr ;
	return strResult ;
}

bool		CRYLSkillScriptMgr::CallSkillDesc( unsigned long dwSkillID, unsigned short wLevel, LPRYLSKILLNODE pSkillNode ) 
{
	char szBuff[ 64 ] ;
	sprintf( szBuff, "SkillToolTipDesc_0x%x_%d", dwSkillID, wLevel ) ;

	unsigned char ucType = CRYLGameData::Instance()->m_csStatus.GetWeaponType() ;
	
	lua_getglobal( m_LUA, szBuff ) ;
	lua_pushnumber( m_LUA, ucType ) ;
	
	int luaResult = lua_pcall( m_LUA, 1, 1, 0 ) ;
	/*
	switch ( luaResult )
	{
	case LUA_ERRRUN :
		MessageBox( NULL, "a runtime error", "error", MB_OK ) ;
		break ;
	case LUA_ERRMEM :
		MessageBox( NULL, "memory allocation error", "error", MB_OK ) ;
		break ;
	case LUA_ERRERR :
		MessageBox( NULL, "error while running the error handler function", "error", MB_OK ) ;
		break ;
	}
	*/
	const char* pszStr = lua_tostring( m_LUA, -1 ) ;
	lua_pop( m_LUA, 1 ) ;

	char *pTmpString = new char[ strlen( pszStr ) + 1 ] ;
	strcpy( pTmpString, pszStr ) ;
	ProcessScriptString( pTmpString, SKILL_TIP, pSkillNode ) ;
	delete [] pTmpString ;
	
	return true ;
}

void		CRYLSkillScriptMgr::UpdateScriptFlag() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	pGame->m_csStatus.StatusUpdate( STATUS_UPDATE_BASE, false, false ) ;
	long lLeftMinDamage = pGame->m_csStatus.GetMinDamage() ;
	long lLeftMaxDamage = pGame->m_csStatus.GetMaxDamage() ;
	short wLeftHitRate   = pGame->m_csStatus.GetHitRate() ;
	short wLeftCritical  = pGame->m_csStatus.GetCritical() ;

	pGame->m_csStatus.StatusUpdate( STATUS_UPDATE_BASE, true, false );
	long lRightMinDamage = pGame->m_csStatus.GetMinDamage() ;
	long lRightMaxDamage = pGame->m_csStatus.GetMaxDamage() ;
	short wRightHitRate   = pGame->m_csStatus.GetHitRate() ;
	short wRightCritical  = pGame->m_csStatus.GetCritical() ;
	
	// 복원.
	pGame->m_csStatus.StatusUpdate( STATUS_UPDATE_BASE, true, true ) ;

	lua_getglobal( m_LUA, "SetValue1"); // "func1"을 얻어와 스택의 맨위에 놓습니다. 
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.STR ) ;					// 0 : 캐릭터의 STR값
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.DEX ) ;					// 1 : 캐릭터의 DEX값
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.CON ) ;					// 2 : 캐릭터의 CON값
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.INT ) ;					// 3 : 캐릭터의 INT값
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.WIS ) ;					// 4 : 캐릭터의 WIS값
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.Level ) ;				// 5 : 캐릭터의 레벨 값
	lua_pushnumber( m_LUA, lRightMinDamage ) ;								// 6 : 캐릭터가 오른손에 장착한 무기를 휘두를 때 최소 데미지
	lua_pushnumber( m_LUA, lRightMaxDamage ) ;								// 7 : 캐릭터가 오른손에 장착한 무기를 휘두를 떄 최대 데미지
	lua_pushnumber( m_LUA, lLeftMinDamage ) ;								// 8 : 캐릭터가 왼손에 장착한 무기를 휘두를 때 최소 데미지
	lua_pushnumber( m_LUA, lLeftMaxDamage ) ;								// 9 : 캐릭터가 왼손에 장착한 무기를 휘두를 때 최대 데미지
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetArmor() ) ;					// 10 : 캐릭터의 방어력
	lua_pushnumber( m_LUA, wRightHitRate ) ;								// 11 : 캐릭터가 오른손에 장착한 무기를 휘두를 때 명중
	lua_pushnumber( m_LUA, wLeftHitRate ) ;									// 12 : 캐릭터가 왼손에 장착한 무기를 휘두를 때 명중
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetEvade() ) ;					// 13 : 캐릭터의 회피
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMaxHP() ) ;					// 14 : 캐릭터의 최대 HP
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMaxMP() ) ;					// 15 : 캐릭터의 최대 MP
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetHPRegen() ) ;				// 16 : 캐릭터의 HP 회복량
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMPRegen() ) ;				// 17 : 캐릭터의 MP 회복량
	lua_pushnumber( m_LUA, wRightCritical ) ;								// 18 : 캐릭터가 오른손에 장착한 무기를 휘두를 때 크리티컬
	lua_pushnumber( m_LUA, wLeftCritical ) ;								// 19 : 캐릭터가 왼손에 장착한 무기를 휘두를 때 크리티컬
	lua_call( m_LUA, 20, 1);         // "func1"에 2개의 인자를 가지고 실행한 후 그 결과를 스택의 최상단에 놓는다. 
	lua_pop( m_LUA, 1 ) ;


	lua_getglobal( m_LUA, "SetValue2"); // "func1"을 얻어와 스택의 맨위에 놓습니다. 

	lua_pushnumber( m_LUA, pGame->m_csStatus.GetBlock()  ) ;				// 20 : 캐릭터의 블록
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetSpeed() ) ;					// 21 : 캐릭터의 이동속도
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetCoolDownRevision() ) ;		// 22 : 캐릭터의 쿨다운 보정
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetSkillPoint() ) ;			// 23 : 캐릭터의 스킬포인드
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMagicPower() ) ;			// 24 : 캐릭터의 마법력
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMagicResist() ) ;			// 25 : 캐릭터의 마법 저항력
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetDefenceRate() ) ;			// 26 : 캐릭터의 방어% (int)((99 * 방어력 / 4 ) / ( 방어력 / 4 + 50 ) * 100 ) / 100
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetCriticalMagnification() ) ;	// 27 : 캐릭터의 크리티컬배율 : 1.5 * 크리티컬 / 200
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetCriticalRate() ) ;			// 28 : 캐릭터의 크리티컬% : (int)((99* 크리티컬) / (크리티컬 + 50) * 100 ) / 100
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetBlockRate() ) ;				// 29 : 캐릭터의 블럭률 % ( int )( ( 99 * 블록 / 2 ) / ( 블록 / 2 + 50 ) * 100 ) / 100
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetSpeedRate() ) ;				// 30 : 캐릭터의 속도배율% : (int )((99 * 속도 ) / ( 속도 + 50 ) * 250 ) / 100
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetCoolDownRevisionRate() ) ;	// 31 : 캐릭터의 쿨다운보정 % : 100 - (int)(( 99 * 쿨다운보정 ) / ( 쿨다운보정 + 50 ) * 50 ) / 100
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMagicResistRate() ) ;		// 32 : 마법저항% : ( int)((99 * 마법저항 / 2) / (마법저항 / 2 + 50 ) * 100 ) / 100
	lua_pushnumber( m_LUA, CRYLSkillDescDlg::ms_cSkillStep ) ;				// 33 : 캐릭터가 현재 가지고 있는 스킬 ID 스킬 습득 단계( 0~4)
	lua_pushnumber( m_LUA, CRYLSkillDescDlg::ms_cSkillLevel ) ;				// 34 : 캐릭터가 현재 가지고 있는 스킬 ID 스킬 습들 레벨( 0~6 )
	lua_call( m_LUA, 15, 1);         // "func1"에 2개의 인자를 가지고 실행한 후 그 결과를 스택의 최상단에 놓는다. 
	lua_pop( m_LUA, 1 ) ;
}

LPRYLSKILLNODE	CRYLSkillScriptMgr::FindSkillNode( unsigned long dwSkillID ) 
{
	std::map< unsigned long, LPRYLSKILLNODE >::iterator		iter = m_mapSkillNode.find( dwSkillID ) ;

	if ( iter != m_mapSkillNode.end() )
	{
		return ( iter->second ) ;
	}

	return NULL ;
}