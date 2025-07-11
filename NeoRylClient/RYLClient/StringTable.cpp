// StringTable.cpp: implementation of the CStringTable class.
//
//////////////////////////////////////////////////////////////////////

#include "StringTable.h"
#include "windows.h"
#include "ScriptEngine.h"
#include <string>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringTable g_StringTable;

CStringTable::CStringTable()
{
	m_dwStringCount = 0;
}

CStringTable::~CStringTable()
{

}

static void ScriptErrorMessage(const char *msg)
{
	MessageBox(NULL, msg, "Script Error", MB_OK);
}

static void AddString(const char *strMessage)
{
	if(strMessage)
	{
		strcpy(g_StringTable.m_strString[g_StringTable.m_dwStringCount], strMessage);
		g_StringTable.m_dwStringCount++;
	}
}

static void AddStringCount(int nCount, const char *strMessage)
{
	if(strMessage)
	{
		strcpy(g_StringTable.m_strString[nCount], strMessage);
		g_StringTable.m_dwStringCount = nCount + 1;
	}
}

bool CStringTable::Load(const char *strScriptFile)
{
	_SE_SetMessageFunction(ScriptErrorMessage);
	SCRIPT Script = _SE_Create(strScriptFile);
	if(!Script) return false;

	_SE_RegisterFunction(Script, AddString, T_VOID, "AddString", T_STRING, 0);
	_SE_RegisterFunction(Script, AddStringCount, T_VOID, "AddString", T_INT, T_STRING, 0);

	_SE_Execute(Script);

	_SE_Destroy(Script);

	return true;
}
