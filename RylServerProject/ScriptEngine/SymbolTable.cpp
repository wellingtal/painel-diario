
#include "SymbolTable.h"

#include <assert.h>
#include <map>
#include <string>
#include <list>

///////////////////////////////////////////////////////////////////////////////////
//	전역변수

const char * DataTypeString[] = { "void", "bool", "int", "float", "string" };

///////////////////////////////////////////////////////////////////////////////////
//

SFuncType::SFuncType()
	: m_data( 0 )
{
}

SFuncType::SFuncType( unsigned long data )
	: m_data( data )
{
}

int			SFuncType::GetArgCount()
{
	int i = 0;

	for( ; i < 7; i++ )
	{
		if( GetArgType( i ) == T_VOID )
			break;
	}
	return i;
}

const char*	SFuncType::ToString( const char * szFuncName )
{
	static char szBuffer[128];

	int n = sprintf( szBuffer, "%s %s(", DataTypeString[GetReturnType()], szFuncName );

	char * pBuffer = szBuffer + n;

	eDataType argType = GetArgType( 0 );
	if( argType != T_VOID )
	{
		n = sprintf( pBuffer, " %s", DataTypeString[ argType ] );
		pBuffer += n;

		for( int i = 1; i < 7; i++ )
		{
			argType = GetArgType( i );
			if( argType == T_VOID )
				break;
			n = sprintf( pBuffer, ", %s", DataTypeString[ argType ] );
			pBuffer += n;	
		}
		strcpy( pBuffer, " " );
		pBuffer += 1;
	}

	strcpy( pBuffer, ")" );

	return szBuffer;
}

struct ConstInfo
{
	eDataType	m_dataType;
	int			m_iOffset;

	ConstInfo()
		: m_dataType( T_VOID ), m_iOffset( 0 )
	{}
	ConstInfo( eDataType type )
		: m_dataType( type ), m_iOffset( 0 )
	{}
	ConstInfo( eDataType type, int offset )
		: m_dataType( type ), m_iOffset( offset )
	{}
};


struct VarInfo
{
	eDataType	m_dataType;
	int			m_iCount;			//변수의 개수.  일반 변수는 1이고 배열일 경우 배열 개수.
	int			m_iOffset;

	VarInfo()
		: m_dataType( T_VOID ), m_iCount( 0 ), m_iOffset( 0 )
	{}
	VarInfo( eDataType dataType, int count )
		: m_dataType( dataType ), m_iCount( count ), m_iOffset( 0 )
	{}
	VarInfo( eDataType dataType, int count, int offset )
		: m_dataType( dataType ), m_iCount( count ), m_iOffset( offset )
	{}

};

typedef CSymbolTable::VAR_CONTAINER		VAR_CONTAINER;
typedef VAR_CONTAINER::value_type		Value_Type;

typedef struct SLocalVarInfo
{
	int				m_iFuncID;
	VAR_CONTAINER	m_ArgContainer;
	VAR_CONTAINER	m_VarContainer;

	SLocalVarInfo()
		: m_iFuncID( 0 )
	{}

} LOCALVARINFO;


///////////////////////////////////////////////////////////////////////////////////
//


CSymbolTable::CSymbolTable()
	: m_pConstants( new CONST_CONTAINER )
	, m_pGlobalVar( new VAR_CONTAINER )
	, m_pLocalVars( new LOCALVAR_CONTAINER )
	, m_pCurNameSpace( m_pGlobalVar )
	, m_pNameSpace2( NULL )
	, m_pFunctions( new FUNC_CONTAINER )
	, m_eCurrentType( T_VOID )
	, m_pCurrentLocalVar( NULL )
	, m_iVarOffset( 0 )
	, m_iVarOffsetGlobal( 0 )
	, m_iOffsetFactor( 4 )
	, m_iStringOffset( 0 )
{
}
CSymbolTable::~CSymbolTable()
{
	delete m_pConstants;
	delete m_pGlobalVar;
	delete m_pLocalVars;
	delete m_pFunctions;
}

void CSymbolTable::Create()
{
}

void CSymbolTable::Destroy()
{
	m_pConstants->clear();
	m_pGlobalVar->clear();
	m_pLocalVars->clear();
	m_pFunctions->clear();
	m_eCurrentType = T_VOID;
}

int			CSymbolTable::AddConst( const char * szName, eDataType type )
{
	int finded = FindConst( szName );
	
	if( finded != 0 )
		return finded;

	pair< CONST_CONTAINER::iterator, bool > result;

	if( type == T_STRING )
	{
		result = m_pConstants->insert( CONST_CONTAINER::value_type( szName, ConstInfo( type, m_iStringOffset ) ) );
		m_iStringOffset += strlen( szName ) + 1;
	}
	else
		result = m_pConstants->insert( CONST_CONTAINER::value_type( szName, type ) );

	return int(&(*result.first));
}

int			CSymbolTable::AddArrVar( const char * szName, eDataType type, int nArray )
{
	pair< VAR_CONTAINER::iterator, bool > result;

	if( m_pCurrentLocalVar == NULL )
	{
		result = m_pCurNameSpace->insert( VAR_CONTAINER::value_type( szName, VARINFO( type, nArray, m_iVarOffset ) ) );
		m_iVarOffset += m_iOffsetFactor * nArray;
	}
	else
	{
		m_iVarOffset += m_iOffsetFactor * nArray;
		result = m_pCurNameSpace->insert( VAR_CONTAINER::value_type( szName, VARINFO( type, nArray, m_iVarOffset ) ) );
	}

	if( !result.second )
		return 0;

	return int(&(*result.first));
}

int			CSymbolTable::AddFunc( bool hasDef, const char * szName, SFuncType type )
{
	typedef FUNC_CONTAINER::value_type value_type;

	int finded = FindFunc( szName, type );

	if( hasDef )
	{
		if( finded != 0 )
		{
			if( IsFuncDefined( finded ) )
			{
				return 0; 
			}
			else
			{
				value_type * pValue = (value_type*)finded;
				pValue->second.second = true;
				return finded;
			}
		}
		else
		{
			FUNC_CONTAINER::iterator result = m_pFunctions->insert( FUNC_CONTAINER::value_type( szName, FUNCINFO( type, true ) ) );
			return int(&(*result));
		}
	}
	else
	{
		if( finded == 0 )
		{
			FUNC_CONTAINER::iterator result = m_pFunctions->insert( FUNC_CONTAINER::value_type( szName, FUNCINFO( type, false ) ) );
			return int(&(*result));
		}
		else
			return finded;
	}
}

int			CSymbolTable::FindConst( const char * szName )
{
	CONST_CONTAINER::iterator result = m_pConstants->find( szName );
	if( result == m_pConstants->end() )
		return 0;

	return int(&(*result));
}

int			CSymbolTable::FindVar( const char * szName )
{
	VAR_CONTAINER::iterator result = m_pCurNameSpace->find( szName );
	if( result == m_pCurNameSpace->end() )
	{
		if( m_pNameSpace2 )
		{
			result = m_pNameSpace2->find( szName );
			if( result == m_pNameSpace2->end() )
			{
				if( m_pCurNameSpace != m_pGlobalVar )
				{
					result = m_pGlobalVar->find( szName );
					if( result == m_pGlobalVar->end() )
						return 0;
				}
				else
					return 0;
			}
		}
		else
		{
			return 0;
		}
	}

	return int(&(*result));
}

int			CSymbolTable::FindFunc( const char * szName, SFuncType type )
{
	typedef FUNC_CONTAINER::iterator iterator;

	pair< iterator, iterator > result = m_pFunctions->equal_range( szName );
			
	while( result.first != result.second )
	{
		if( result.first->second.first == type )
			return int(&(*result.first));
		result.first++;
	}

	return 0;
}

const char*	CSymbolTable::GetNameOfConst( int symbolID )
{
	typedef CONST_CONTAINER::value_type value_type;

	value_type * pValue = (value_type*)symbolID;
	return pValue->first.c_str();
}

eDataType	CSymbolTable::GetTypeOfConst( int symbolID )
{
	typedef CONST_CONTAINER::value_type value_type;	

	value_type * pValue = (value_type*)symbolID;
	return pValue->second.m_dataType;
}

int			CSymbolTable::GetOffsetOfConst( int symbolID )
{
	typedef CONST_CONTAINER::value_type value_type;	

	value_type * pValue = (value_type*)symbolID;
	return pValue->second.m_iOffset;
}

const char*	CSymbolTable::GetNameOfVar( int symbolID )
{
	typedef VAR_CONTAINER::value_type value_type;

	value_type * pValue = (value_type*)symbolID;
	return pValue->first.c_str();
}

eDataType	CSymbolTable::GetTypeOfVar( int symbolID )
{
	typedef VAR_CONTAINER::value_type value_type;

	value_type * pValue = (value_type*)symbolID;
	return pValue->second.m_dataType;
}

int			CSymbolTable::GetCountOfVar( int symbolID )
{
	typedef VAR_CONTAINER::value_type value_type;

	value_type * pValue = (value_type*)symbolID;
	return pValue->second.m_iCount;
}

int			CSymbolTable::GetOffsetOfVar( int varID )
{
	typedef VAR_CONTAINER::value_type	value_type;

	value_type * pValue = (value_type*)varID;
	return pValue->second.m_iOffset;
}


const char*	CSymbolTable::GetNameOfFunc( int symbolID )
{
	typedef FUNC_CONTAINER::value_type value_type;

	value_type * pValue = (value_type*)symbolID;
	return pValue->first.c_str();
}

const char* CSymbolTable::GetTypeStringOfFunc( int symbolID )
{
	SFuncType type = GetTypeOfFunc( symbolID );

	return type.ToString( GetNameOfFunc( symbolID ) );
}

SFuncType	CSymbolTable::GetTypeOfFunc( int symbolID )
{
	typedef FUNC_CONTAINER::value_type value_type;

	value_type * pValue = (value_type*)symbolID;
	return pValue->second.first;
}

bool		CSymbolTable::IsFuncDefined( int symbolID )
{
	typedef FUNC_CONTAINER::value_type value_type;

	value_type * pValue = (value_type*)symbolID;
	return pValue->second.second;
}

void		CSymbolTable::BeginLocalNameSpace()
{
	m_pLocalVars->push_back( LOCALVARINFO() );
	m_pCurrentLocalVar = &m_pLocalVars->back();
	m_pCurNameSpace = &m_pCurrentLocalVar->m_ArgContainer;
	m_pNameSpace2 = &m_pCurrentLocalVar->m_VarContainer;

	m_iVarOffsetGlobal = m_iVarOffset;
	m_iVarOffset = 4;
	m_iOffsetFactor = 4;
}

void		CSymbolTable::EndLocalNameSpace( int FuncID )
{
	m_pCurNameSpace = m_pGlobalVar;
	m_pNameSpace2 = NULL;
	m_pCurrentLocalVar = NULL;

	if( FuncID )
	{
		m_pLocalVars->back().m_iFuncID = FuncID;
	}
	else
		m_pLocalVars->pop_back();

	m_iVarOffset = m_iVarOffsetGlobal;
	m_iOffsetFactor = 4;
}

void		CSymbolTable::EndArgument()
{
	if( m_pCurrentLocalVar )
	{
		m_pCurNameSpace = &m_pCurrentLocalVar->m_VarContainer;
		m_pNameSpace2 = &m_pCurrentLocalVar->m_ArgContainer;

		m_iVarOffset = 0;
		m_iOffsetFactor = -4;
	}
}

int			CSymbolTable::GetLocalVarSize( int FuncID )
{
	int size = 0;

	VAR_CONTAINER * pNameSpace = NULL;

	for( LOCALVAR_CONTAINER::iterator i = m_pLocalVars->begin(); i != m_pLocalVars->end(); i++ )
	{
		if( i->m_iFuncID == FuncID )
		{
			pNameSpace = &(i->m_VarContainer);
			break;
		}
	}

	if( pNameSpace == NULL )
	{
		ScriptSystemError( "지정한 이름 공간을 찾을 수 없습니다. (CSymbolTable::GetLocalVarSize)" );
		return 0;
	}

	for( VAR_CONTAINER::iterator j = pNameSpace->begin(); j != pNameSpace->end(); j++ )
	{
		size += j->second.m_iCount * 4;
	}

	return size;
}

int			CSymbolTable::GetGlobalVarSize()
{
	int size = 0;

	for( VAR_CONTAINER::iterator i = m_pGlobalVar->begin(); i != m_pGlobalVar->end(); i++ )
	{
		size += i->second.m_iCount * 4;
	}

	return size;
}

int			CSymbolTable::GetStringBufferSize()
{
	return m_iStringOffset;
}

void		CSymbolTable::StringBuffer( void * pBuffer )
{
	char * pBuf = (char*)pBuffer;

	for( CONST_CONTAINER::iterator i = m_pConstants->begin(); i != m_pConstants->end(); i++ )
	{
		if( i->second.m_dataType == T_STRING )
		{
			strcpy( pBuf + i->second.m_iOffset, i->first.c_str() );
		}
	}
}

void		ShowVariables( CSymbolTable::VAR_CONTAINER & Container, MESSAGE_FUNCTION2 Print )
{
	for( CSymbolTable::VAR_CONTAINER::iterator i = Container.begin(); i!= Container.end(); i++ )
	{
		Print( i->first.c_str() );
		if( i->second.m_iCount != 1 )
		{
			Print( "[%d]", i->second.m_iCount );
		}
		Print( " : %s  <%d>\n", DataTypeString[ i->second.m_dataType ], i->second.m_iOffset );
	}
}

void		CSymbolTable::Show( MESSAGE_FUNCTION2 Print )
{
	const char * DataTypeString[] = { "void", "bool", "int", "float", "string" };

	Print( "--Constants--\n" );
	for( CONST_CONTAINER::iterator i = m_pConstants->begin(); i!= m_pConstants->end(); i++ )
	{
		Print( "%s : %s <%d>\n", i->first.c_str(), DataTypeString[ i->second.m_dataType ], i->second.m_iOffset );
	}
	Print( "--Global Variables--\n" );
	ShowVariables( *m_pGlobalVar, Print );
	Print( "--Local Variables--\n" );
	for( LOCALVAR_CONTAINER::iterator l = m_pLocalVars->begin(); l != m_pLocalVars->end(); l++ )
	{
		Print( "<%s>\n", this->GetNameOfFunc( l->m_iFuncID ) );
		ShowVariables( l->m_ArgContainer, Print );
		ShowVariables( l->m_VarContainer, Print );
	}
	Print( "--Functions--\n" );

	for( FUNC_CONTAINER::iterator k = m_pFunctions->begin(); k != m_pFunctions->end(); k++ )
	{
		Print( GetTypeStringOfFunc( int(&(*k)) ) );
		Print( "\n" );
	}
}

///////////////////////////////////////////////////////////////////////////////////
//