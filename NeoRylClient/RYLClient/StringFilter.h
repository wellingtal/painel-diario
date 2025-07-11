// StringFilter.h: interface for the CStringFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGFILTER_H__033A0568_375A_41FD_8370_CEDE222BE28B__INCLUDED_)
#define AFX_STRINGFILTER_H__033A0568_375A_41FD_8370_CEDE222BE28B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStringFilter  
{
	public:
		bool Load(const char *strScriptFile);
		unsigned long m_dwStringCount;
		WCHAR m_strString[2000][15];
		char m_strSearched[32];

		bool Filter(char *strTarget);

		bool ConvertFilter(char *strTarget);
		CStringFilter();
		~CStringFilter();
};

extern CStringFilter g_StringFilter;
#endif // !defined(AFX_STRINGFILTER_H__033A0568_375A_41FD_8370_CEDE222BE28B__INCLUDED_)
