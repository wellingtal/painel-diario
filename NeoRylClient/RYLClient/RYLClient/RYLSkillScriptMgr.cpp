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
// Desc : Skill Node ���� ������
// Val :  - int : ��ų ID
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
// Desc : Skil Node �� �� ����
//=============================================
int  CRYLSkillScriptMgr::SkillTooltip_SkillEnd( lua_State *L ) 
{
	m_lpTmpSkillNode = NULL ;
	return 1 ;
}

//=============================================
// Desc : ��ų �迭 �̸�
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
// Desc : ��ų �迭
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
// Desc : ��ų Ÿ��
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
// Desc : ��ų ����
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
	
	// ����.
	pGame->m_csStatus.StatusUpdate( STATUS_UPDATE_BASE, true, true ) ;

	lua_getglobal( m_LUA, "SetValue1"); // "func1"�� ���� ������ ������ �����ϴ�. 
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.STR ) ;					// 0 : ĳ������ STR��
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.DEX ) ;					// 1 : ĳ������ DEX��
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.CON ) ;					// 2 : ĳ������ CON��
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.INT ) ;					// 3 : ĳ������ INT��
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.WIS ) ;					// 4 : ĳ������ WIS��
	lua_pushnumber( m_LUA, pGame->m_csStatus.m_Info.Level ) ;				// 5 : ĳ������ ���� ��
	lua_pushnumber( m_LUA, lRightMinDamage ) ;								// 6 : ĳ���Ͱ� �����տ� ������ ���⸦ �ֵθ� �� �ּ� ������
	lua_pushnumber( m_LUA, lRightMaxDamage ) ;								// 7 : ĳ���Ͱ� �����տ� ������ ���⸦ �ֵθ� �� �ִ� ������
	lua_pushnumber( m_LUA, lLeftMinDamage ) ;								// 8 : ĳ���Ͱ� �޼տ� ������ ���⸦ �ֵθ� �� �ּ� ������
	lua_pushnumber( m_LUA, lLeftMaxDamage ) ;								// 9 : ĳ���Ͱ� �޼տ� ������ ���⸦ �ֵθ� �� �ִ� ������
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetArmor() ) ;					// 10 : ĳ������ ����
	lua_pushnumber( m_LUA, wRightHitRate ) ;								// 11 : ĳ���Ͱ� �����տ� ������ ���⸦ �ֵθ� �� ����
	lua_pushnumber( m_LUA, wLeftHitRate ) ;									// 12 : ĳ���Ͱ� �޼տ� ������ ���⸦ �ֵθ� �� ����
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetEvade() ) ;					// 13 : ĳ������ ȸ��
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMaxHP() ) ;					// 14 : ĳ������ �ִ� HP
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMaxMP() ) ;					// 15 : ĳ������ �ִ� MP
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetHPRegen() ) ;				// 16 : ĳ������ HP ȸ����
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMPRegen() ) ;				// 17 : ĳ������ MP ȸ����
	lua_pushnumber( m_LUA, wRightCritical ) ;								// 18 : ĳ���Ͱ� �����տ� ������ ���⸦ �ֵθ� �� ũ��Ƽ��
	lua_pushnumber( m_LUA, wLeftCritical ) ;								// 19 : ĳ���Ͱ� �޼տ� ������ ���⸦ �ֵθ� �� ũ��Ƽ��
	lua_call( m_LUA, 20, 1);         // "func1"�� 2���� ���ڸ� ������ ������ �� �� ����� ������ �ֻ�ܿ� ���´�. 
	lua_pop( m_LUA, 1 ) ;


	lua_getglobal( m_LUA, "SetValue2"); // "func1"�� ���� ������ ������ �����ϴ�. 

	lua_pushnumber( m_LUA, pGame->m_csStatus.GetBlock()  ) ;				// 20 : ĳ������ ���
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetSpeed() ) ;					// 21 : ĳ������ �̵��ӵ�
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetCoolDownRevision() ) ;		// 22 : ĳ������ ��ٿ� ����
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetSkillPoint() ) ;			// 23 : ĳ������ ��ų���ε�
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMagicPower() ) ;			// 24 : ĳ������ ������
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMagicResist() ) ;			// 25 : ĳ������ ���� ���׷�
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetDefenceRate() ) ;			// 26 : ĳ������ ���% (int)((99 * ���� / 4 ) / ( ���� / 4 + 50 ) * 100 ) / 100
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetCriticalMagnification() ) ;	// 27 : ĳ������ ũ��Ƽ�ù��� : 1.5 * ũ��Ƽ�� / 200
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetCriticalRate() ) ;			// 28 : ĳ������ ũ��Ƽ��% : (int)((99* ũ��Ƽ��) / (ũ��Ƽ�� + 50) * 100 ) / 100
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetBlockRate() ) ;				// 29 : ĳ������ ���� % ( int )( ( 99 * ��� / 2 ) / ( ��� / 2 + 50 ) * 100 ) / 100
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetSpeedRate() ) ;				// 30 : ĳ������ �ӵ�����% : (int )((99 * �ӵ� ) / ( �ӵ� + 50 ) * 250 ) / 100
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetCoolDownRevisionRate() ) ;	// 31 : ĳ������ ��ٿ�� % : 100 - (int)(( 99 * ��ٿ�� ) / ( ��ٿ�� + 50 ) * 50 ) / 100
	lua_pushnumber( m_LUA, pGame->m_csStatus.GetMagicResistRate() ) ;		// 32 : ��������% : ( int)((99 * �������� / 2) / (�������� / 2 + 50 ) * 100 ) / 100
	lua_pushnumber( m_LUA, CRYLSkillDescDlg::ms_cSkillStep ) ;				// 33 : ĳ���Ͱ� ���� ������ �ִ� ��ų ID ��ų ���� �ܰ�( 0~4)
	lua_pushnumber( m_LUA, CRYLSkillDescDlg::ms_cSkillLevel ) ;				// 34 : ĳ���Ͱ� ���� ������ �ִ� ��ų ID ��ų ���� ����( 0~6 )
	lua_call( m_LUA, 15, 1);         // "func1"�� 2���� ���ڸ� ������ ������ �� �� ����� ������ �ֻ�ܿ� ���´�. 
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