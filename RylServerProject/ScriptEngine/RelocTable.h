
#ifndef _RelocTable_H_
#define _RelocTable_H_

#include "STL.h"

///////////////////////////////////////////////////////////////////////////////////
//

enum eStdFunc
{
	FUNC_ITOA = 0,
	FUNC_GCVT,
	FUNC_MALLOC,
	FUNC_FREE,
	FUNC_STRCPY,
	FUNC_STRCMP,
	FUNC_STRLEN,
	FUNC_REGISTERALLOCATEDMEMORY,
	FUNC_UNREGISTERALLOCATEDMEMORY = 8
};

class CRelocTable
{
	typedef vector<int>			RELOCTABLE;
	typedef pair<eStdFunc, int>	FUNCRELOC;
	typedef vector<FUNCRELOC>	FUNCRELOCTABLE;
	
	RELOCTABLE	*			m_pGlobalVarTable;
	RELOCTABLE	*			m_pStringTable;
	FUNCRELOCTABLE *		m_pFuncRelocTable;

public:
	CRelocTable();
	~CRelocTable();
	
	void	Create( ifstream & );
	const void *	Create( const void * pDataBuf, unsigned dataSize );
	void	Destroy();
	void	Save( ofstream & );
	void	AddGlobalVar( int offset );
	void	AddConstString( int offset );
	void	AddFuncBind( eStdFunc, int offset );
	void	Relocate( void * pGlobalVarBuffer, void * pStringBuffer, void * pCodeBuffer );
	void	Unlocate( void * pGlobalVarBuffer, void * pStringBuffer, void * pCodeBuffer );
};


///////////////////////////////////////////////////////////////////////////////////

#endif