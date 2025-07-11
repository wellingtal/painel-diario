
#include "IntermediateCode.h"
#include "IMCodeGen.h"
#include "IMCodeUnits.h"
#include "SymbolTable.h"
#include <list>
#include <map>
#include "RelocTable.h"
#include <stdlib.h>
#include <string>


///////////////////////////////////////////////////////////////////////////////////
//

CIntermediateCode::CIntermediateCode()
	: m_pCodes( new IMCODES )
	, m_pFunctions( new FUNCTIONS )
	, m_pFuncTable( new FUNCTABLE )
{
}

CIntermediateCode::~CIntermediateCode()
{
	Destroy();
	delete m_pCodes;
	delete m_pFunctions;
	delete m_pFuncTable;
}

void	CIntermediateCode::Create( CSyntaxTree & rSyntaxTree )
{
	GenerateCode( rSyntaxTree, *m_pCodes, *m_pFunctions );
}

void	CIntermediateCode::DestroyIMCodes( IMCODES * pIMCodes )
{
	for( IMCODES::iterator i = pIMCodes->begin(); i != pIMCodes->end(); i++ )
	{
		delete *i;
	}
	pIMCodes->clear();	
}

void	CIntermediateCode::Destroy()
{
	for( FUNCTIONS::iterator i = m_pFunctions->begin(); i != m_pFunctions->end(); i++ )
	{
		DestroyIMCodes( i->second );
		delete i->second;
	}
	m_pFunctions->clear();
	DestroyIMCodes( m_pCodes );
}

void	CIntermediateCode::Show( MESSAGE_FUNCTION2 Print, CSymbolTable * pSymbolTable )
{
	const char * DataTypeString[] = { "void", "bool", "int", "float", "string" };

	SetSymbolTable( pSymbolTable );

	Print( "<<< void Main() >>>\n" );

	for( IMCODES::iterator i = m_pCodes->begin(); i != m_pCodes->end(); i++ )
	{
		IOPCode * pCode = (*i);
		Print( "%X :\t", pCode->m_lAddress );
		pCode->Show( Print );
	}

	for( FUNCTIONS::iterator j = m_pFunctions->begin(); j != m_pFunctions->end(); j++ )
	{
		SFuncType type = pSymbolTable->GetTypeOfFunc( j->first );
		Print( "<<< %s %s(", DataTypeString[type.GetReturnType()], pSymbolTable->GetNameOfFunc( j->first ) );

		if( type.GetArgType( 0 ) != T_VOID )
		{
			Print( " " );
			Print( DataTypeString[ type.GetArgType( 0 ) ] );
			for( int ith = 1; ith < 8; ith++ )
			{
				eDataType dataType = type.GetArgType( ith );
				if( dataType == T_VOID )
					break;
				Print( ", %s", DataTypeString[dataType] );
			}
			Print( " " );
		}
		Print( ") >>>\n" );


		IMCODES * pFuncCode = j->second;
		for( IMCODES::iterator i = pFuncCode->begin(); i != pFuncCode->end(); i++ )
		{
			Print( "%X :\t", (*i)->m_lAddress );
			(*i)->Show( Print );
		}
	}
}

long	CIntermediateCode::Addressing( long addr )
{
	for( IMCODES::iterator i = m_pCodes->begin(); i != m_pCodes->end(); i++ )
	{
		addr = (*i)->Addressing( addr );
	}

	for( FUNCTIONS::iterator j = m_pFunctions->begin(); j != m_pFunctions->end(); j++ )
	{
		IMCODES * pFuncCode = j->second;
		for( IMCODES::iterator i = pFuncCode->begin(); i != pFuncCode->end(); i++ )
		{
			addr = (*i)->Addressing( addr );
		}
	}
	return addr;
}

int		CIntermediateCode::ToMachineCode( void * pBuf, CRelocTable * pRelocTable )
{
	Addressing( 0 );

	for( FUNCTIONS::iterator k = m_pFunctions->begin(); k != m_pFunctions->end(); k++ )
	{
		int funcID = k->first;
		int offset = k->second->front()->m_lAddress;
		m_pFuncTable->insert( FUNCTABLE::value_type( funcID, offset ) );
	}

	SetFuncTable( m_pFuncTable );
	SetRelocTable( pRelocTable );

	byte * pBuffer = (byte*)pBuf;
	int TotalSize = 0;

	IMCODES::iterator i = m_pCodes->begin();

	for( ; i != m_pCodes->end(); i++ )
	{
		int size = (*i)->ToMachineCode( pBuffer );
		pBuffer += size;
		TotalSize += size;
	}

	for( FUNCTIONS::iterator j = m_pFunctions->begin(); j != m_pFunctions->end(); j++ )
	{
		IMCODES * pFuncCode = j->second;
		for( i = pFuncCode->begin(); i != pFuncCode->end(); i++ )
		{
			int size = (*i)->ToMachineCode( pBuffer );
			pBuffer += size;
			TotalSize += size;
		}
	}

	return TotalSize;
}

///////////////////////////////////////////////////////////////////////////////////
//
