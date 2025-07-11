
#ifndef _IntermediateCode_H_
#define _IntermediateCode_H_

#include "STL.h"
#include "BaseDef.h"
#include "Message.h"

///////////////////////////////////////////////////////////////////////////////////
//

class IOPCode;
class CSyntaxTree;
class CSymbolTable;
class CRelocTable;

class CIntermediateCode
{
public:
	typedef list<IOPCode*>		IMCODES;
	typedef map<int, IMCODES*>	FUNCTIONS;
	typedef map<int, long>		FUNCTABLE;


protected:
	IMCODES *		m_pCodes;
	FUNCTIONS *		m_pFunctions;
	FUNCTABLE *		m_pFuncTable;


protected:
	void	DestroyIMCodes( IMCODES * );

public:
	CIntermediateCode();
	~CIntermediateCode();

	void	Create( CSyntaxTree & );
	void	Destroy();

	void	Show( MESSAGE_FUNCTION2, CSymbolTable * pSymbolTable );
	long	Addressing( long addr );													//past end의 addr을 리턴함.
	int		ToMachineCode( void *, CRelocTable * );
	
	FUNCTABLE &	GetFuncTable()		{ return *m_pFuncTable; }
};

///////////////////////////////////////////////////////////////////////////////////
//

#endif