#ifndef __RYLSKILLSCRIPTMGR_H__
#define __RYLSKILLSCRIPTMGR_H__

#include <map>
#include <vector>
#include <string>
#include <windows.h>

/*
extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}
*/

#define SKILL_TOOLTIP "SkillTooltip"

typedef struct __SKILLNODE 
{
	std::vector<std::string>	s_vecSkillTooltip ;	// ��ų ����
	std::vector<std::string>	s_vecSkillGrade ;	// ��ų �迭
	std::vector<std::string>	s_vecSkillDesc ;	// ��ų ����
	std::string					s_strSkillType ;	// ��ų Ÿ��
	std::string					s_strSkillGradeName;// ��ų �迭 �̸�
	unsigned long				s_dwSkillID ;		// ��ų ID
} RYLSKILLNODE, *LPRYLSKILLNODE ;

enum __SKILLDESC
{
	SKILL_DESC,
	SKILL_TIP
} ;

struct lua_State ;

class CRYLSkillScriptMgr
{
private :
	static std::map< unsigned long, LPRYLSKILLNODE >	m_mapSkillNode ;
	static LPRYLSKILLNODE								m_lpTmpSkillNode ;
	lua_State*											m_LUA ;
	
public :
	CRYLSkillScriptMgr() ;
	~CRYLSkillScriptMgr() ;
	lua_State*	GetLUA()			{ return m_LUA ; }

	void		UpdateScriptFlag() ;
	bool		SkillScriptLoad( const char* pszFileName ) ;		
	RYLSKILLNODE*	FindSkillNode( unsigned long dwSkillID ) ;
	
	static int  SkillTooltip_SkillBegin( lua_State *L ) ;
	static int  SkillTooltip_SkillEnd( lua_State *L ) ;
	static int  SkillTooltip_SkillGradeName( lua_State *L ) ;
	static int  SkillTooltip_SkillGrade( lua_State *L ) ;
	static int  SkillTooltip_SkillType( lua_State *L ) ;
	static int  SkillTooltip_SkillDesc( lua_State *L ) ;

	bool		CallSkillDesc( unsigned long dwSkillID, unsigned short wLevel, LPRYLSKILLNODE pSkillNode ) ;

	static CRYLSkillScriptMgr* Instance() ;

private :
	void 		LuaMethodRegister(); 
	int			SkillMethodRegister() ;
	void		ClearScriptData() ;
	
	static RYLSKILLNODE* pushSkillTooltip( lua_State* L ) ;
	static void	ProcessScriptString( char* pszData, unsigned long dwType, LPRYLSKILLNODE pSkillNode = NULL ) ;
	static std::string		CRYLSkillScriptMgr::SkipPointNumber( LPSTR lpStr ) ;

	static CRYLSkillScriptMgr* ms_pInstance ;
} ;

#define g_pSkillScriptMgr	CRYLSkillScriptMgr::Instance

#endif // __RYLSKILLSCRIPTMGR_H__