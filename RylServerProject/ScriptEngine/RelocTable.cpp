
#include "RelocTable.h"

#include <vector>
#include <fstream>
#include <stdio.h>

#include <windows.h>

#include "VirtualMachine.h"


CRelocTable::CRelocTable()
	: m_pGlobalVarTable( new RELOCTABLE )
	, m_pStringTable( new RELOCTABLE )
	, m_pFuncRelocTable( new FUNCRELOCTABLE )
{
}

CRelocTable::~CRelocTable()
{
	delete m_pGlobalVarTable;
	delete m_pStringTable;
	delete m_pFuncRelocTable;
}
	
void	CRelocTable::Create( ifstream & file )
{
	Destroy();

	int GlobalVarSize;
	int StringBufferSize;
	int FuncBufferSize;
	file.read( (char*)&GlobalVarSize, sizeof(int) );
	file.read( (char*)&StringBufferSize, sizeof(int) );
	file.read( (char*)&FuncBufferSize, sizeof(int) );

	int Temp;
	for( int i = 0; i < GlobalVarSize; i++ )
	{
		file.read( (char*)&Temp, sizeof(int) );
		m_pGlobalVarTable->push_back( Temp );
	}

	for( int i = 0; i < StringBufferSize; i++ )
	{
		file.read( (char*)&Temp, sizeof(int) );
		m_pStringTable->push_back( Temp );
	}

	for( int i = 0; i < FuncBufferSize; i++ )
	{
		eStdFunc Func;
		file.read( (char*)&Func, sizeof(Func) );
		file.read( (char*)&Temp, sizeof(int) );
		m_pFuncRelocTable->push_back( FUNCRELOC( Func, Temp ) );
	}
}

#define READ_INT( p, var )	{ var = *((int*)p); p += sizeof(int); }
#define READ_STDFUNCTYPE( p, var ) { var = *((eStdFunc*)p); p+= sizeof(eStdFunc); }

const void *	CRelocTable::Create( const void * pDataBuf, unsigned dataSize )
{
	Destroy();

	const char * pBuf = (const char *)pDataBuf;
	
	int GlobalVarSize, StringBufferSize, FuncBufferSize, temp;

	READ_INT( pBuf, GlobalVarSize );
	READ_INT( pBuf, StringBufferSize );
	READ_INT( pBuf, FuncBufferSize );

	for( int i = 0; i < GlobalVarSize; i++ )
	{
		READ_INT( pBuf, temp );
		m_pGlobalVarTable->push_back( temp );
	}

	for( int i = 0; i < StringBufferSize; i++ )
	{
		READ_INT( pBuf, temp );
		m_pStringTable->push_back( temp );
	}

	for( int i = 0; i < FuncBufferSize; i++ )
	{
		eStdFunc Func;
		READ_STDFUNCTYPE( pBuf, Func );
		READ_INT( pBuf, temp );
		m_pFuncRelocTable->push_back( FUNCRELOC( Func, temp ) );
	}

	return pBuf;
}


void	CRelocTable::Destroy()
{
	m_pGlobalVarTable->clear();
	m_pStringTable->clear();
}


void	CRelocTable::Save( ofstream & file )
{
	int GlobalVarSize = m_pGlobalVarTable->size();
	int StringBufferSize = m_pStringTable->size();
	int FuncBufferSize = m_pFuncRelocTable->size();
	file.write( (const char*)&GlobalVarSize, sizeof(int) );
	file.write( (const char*)&StringBufferSize, sizeof(int) );
	file.write( (const char*)&FuncBufferSize, sizeof(int) );

	for( int i = 0; i < GlobalVarSize; i++ )
	{
		int offset = m_pGlobalVarTable->at( i );
		file.write( (const char*)&offset, sizeof(int) );
	}

	for( int i = 0; i < StringBufferSize; i++ )
	{
		int offset = m_pStringTable->at( i );
		file.write( (const char*)&offset, sizeof(int) );
	}

	for( int i = 0; i < FuncBufferSize; i++ )
	{
		FUNCRELOC relocFunc = m_pFuncRelocTable->at( i );
		file.write( (const char*)&relocFunc.first, sizeof(relocFunc.first) );
		file.write( (const char*)&relocFunc.second, sizeof(relocFunc.second) );
	}
}

void	CRelocTable::AddGlobalVar( int offset )
{
	m_pGlobalVarTable->push_back( offset );
}

void	CRelocTable::AddConstString( int offset )
{
	m_pStringTable->push_back( offset );
}

void	CRelocTable::AddFuncBind( eStdFunc func, int offset )
{
	m_pFuncRelocTable->push_back( FUNCRELOC( func, offset ) );
}

void	CRelocTable::Relocate( void * pGlobalVarBuffer, void * pStringBuffer, void * pCodeBuffer )
{
	void * ArrFuncPtr[] = { itoa, gcvt, malloc, free, strcpy, strcmp, 
							strlen, RegisterAllocatedMemory, UnregisterAllocatedMemory };

	char * pCode = (char*)pCodeBuffer;

	for( RELOCTABLE::iterator i = m_pGlobalVarTable->begin(); i != m_pGlobalVarTable->end(); i++ )
	{
		long * pValue = (long*)(pCode + *i);
		*pValue += long(pGlobalVarBuffer);
	}

	for( RELOCTABLE::iterator j = m_pStringTable->begin(); j != m_pStringTable->end(); j++ )
	{
		long * pValue = (long*)(pCode + *j);
		*pValue += long(pStringBuffer);
	}

	for( FUNCRELOCTABLE::iterator k = m_pFuncRelocTable->begin(); k != m_pFuncRelocTable->end(); k++ )
	{
		long * pValue = (long*)(pCode + k->second);
		*pValue = (long) ArrFuncPtr[k->first];
	}
}

void	CRelocTable::Unlocate( void * pGlobalVarBuffer, void * pStringBuffer, void * pCodeBuffer )
{
	char * pCode = (char*)pCodeBuffer;

	for( RELOCTABLE::iterator i = m_pGlobalVarTable->begin(); i != m_pGlobalVarTable->end(); i++ )
	{
		long * pValue = (long*)(pCode + *i);
		*pValue -= long(pGlobalVarBuffer);
	}

	for( RELOCTABLE::iterator i = m_pStringTable->begin(); i != m_pStringTable->end(); i++ )
	{
		long * pValue = (long*)(pCode + *i);
		*pValue -= long(pStringBuffer);
	}
	for( FUNCRELOCTABLE::iterator k = m_pFuncRelocTable->begin(); k != m_pFuncRelocTable->end(); k++ )
	{
		long * pValue = (long*)(pCode + k->second);
		*pValue = 0;
	}
}
