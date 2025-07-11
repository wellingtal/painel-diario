// StringTable.h: interface for the CStringTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGTABLE_H__2DBB753F_DB17_431A_9655_D9D8692F16EB__INCLUDED_)
#define AFX_STRINGTABLE_H__2DBB753F_DB17_431A_9655_D9D8692F16EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStringTable
{
	public:
		bool Load(const char *strScriptFile);
		unsigned long m_dwStringCount;
		char m_strString[2000][512];

/*		char m_strLoginError[5][100];
		char m_strCreateError[7][100];
		char m_strGameError[11][100];
		char m_strTooltip[29][100];
		char m_strItem[20][100];
		char m_strStatus[14][100];
		char m_strPopup[7][100];
		char m_strParty[18][100];
		char m_strChat[30][100];
		char m_strMonster[40][100];
		char m_strMessage[10][100];*/

		CStringTable();
		~CStringTable();
};

extern CStringTable g_StringTable;

#endif // !defined(AFX_STRINGTABLE_H__2DBB753F_DB17_431A_9655_D9D8692F16EB__INCLUDED_)
