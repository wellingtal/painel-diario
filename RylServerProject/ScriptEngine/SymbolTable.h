
#ifndef _SymbolTable_H_
#define _SymbolTable_H_

#include "STL.h"
#include "BaseDef.h"
#include "Message.h"

#define SYSVAR_SWITCHTEMP	"<SwitchTempVar>"
#define SYSVAR_FLOATUNIT	"<FloatUnit>"
#define SYSVAR_FLOATTEMP	"<FloatTemp>"
#define SYSVAR_TRUE			"<True>"
#define SYSVAR_FALSE		"<False>"
#define SYSVAR_CONVBUFFER	"<ConvBuffer>"

///////////////////////////////////////////////////////////////////////////////////
//	함수의 인자는 7개를 넘어선 안된다.

struct SFuncType
{
public:
	unsigned long	m_data;

public:
	SFuncType();
	SFuncType( unsigned long );

	void		SetArgType( int nth, eDataType arg )				{ m_data |= ( 0x0000000f & arg ) << (nth*4); }
	void		SetReturnType( eDataType retType )					{ m_data |= ( 0x0000000f & retType ) << 28; }
	eDataType	GetArgType( int nth )								{ return (eDataType)(m_data >> (nth*4) & 0x0000000f); }
	eDataType	GetReturnType()										{ return (eDataType)(m_data >> 28 & 0x0000000f); }
	int			GetArgCount();
	const char*	ToString( const char * szFuncName );

	bool		operator==( const SFuncType & rhs )					{ return (m_data & 0x0fffffff) == (rhs.m_data & 0x0fffffff); }
	bool		operator!=( const SFuncType & rhs )					{ return (m_data & 0x0fffffff) != (rhs.m_data & 0x0fffffff); }
};

///////////////////////////////////////////////////////////////////////////////////
//

class CSymbolTable
{
public:
	typedef map< int, int >	OFFSETMAP;
	typedef unsigned char byte;
	typedef struct ConstInfo			CONSTINFO;
	typedef map<string,CONSTINFO>		CONST_CONTAINER;
	typedef struct VarInfo				VARINFO;
	typedef map<string,VARINFO>			VAR_CONTAINER;
	typedef struct SLocalVarInfo		LOCALVARINFO;
	typedef list<LOCALVARINFO>			LOCALVAR_CONTAINER;
	typedef pair<SFuncType, bool>		FUNCINFO;
	typedef multimap<string,FUNCINFO>	FUNC_CONTAINER;


protected:
	CONST_CONTAINER *	m_pConstants;
	VAR_CONTAINER *		m_pGlobalVar;
	LOCALVAR_CONTAINER*	m_pLocalVars;
	VAR_CONTAINER *		m_pCurNameSpace;
	VAR_CONTAINER *		m_pNameSpace2;									//CurNameSpace와 쌍을 이룸( CurNameSpace가 ArgContainer일 때 이것은 VarContainer, VarContainer일 때 ArgContainer )
	FUNC_CONTAINER *	m_pFunctions;

	LOCALVARINFO *		m_pCurrentLocalVar;

	eDataType			m_eCurrentType;

	int					m_iVarOffset, m_iVarOffsetGlobal;
	int					m_iOffsetFactor;
	int					m_iStringOffset;

public:
	CSymbolTable();
	~CSymbolTable();

	void		Create();
	void		Destroy();

	int			AddConst( const char * szName, eDataType type );
	int			AddArrVar( const char * szName, eDataType, int nArray );
	int			AddArrVar( const char * szName, int nArray )				{ return AddArrVar( szName, m_eCurrentType, nArray ); }
	int			AddVar( const char * szName, eDataType type )				{ return AddArrVar( szName, type, 1 ); }
	int			AddVar( const char * szName )								{ return AddVar( szName, m_eCurrentType ); }
	int			AddFunc( bool hasDef, const char * szName, SFuncType );
	void		SetCurrentType( eDataType type )							{ m_eCurrentType = type; }
	eDataType	GetCurrentType()											{ return m_eCurrentType; }
	int			FindConst( const char * szName );
	int			FindVar( const char * szName );
	int			FindFunc( const char * szname, SFuncType );

	void		BeginLocalNameSpace();
	void		EndLocalNameSpace( int );									//인자로는 함수의 ID를 넘겨준다.
	void		EndArgument();

	const char*	GetNameOfConst( int );
	eDataType	GetTypeOfConst( int );
	int			GetOffsetOfConst( int );

	const char*	GetNameOfVar( int );
	eDataType	GetTypeOfVar( int );
	int			GetCountOfVar( int );
	int			GetOffsetOfVar( int );										//지역 변수일 경우 EBP를 기준으로한 오프셋을 리턴함.
																			//전역 변수일 경우 시작 주소에 대한 오프셋을 리턴한다.
	const char*	GetNameOfFunc( int );
	const char* GetTypeStringOfFunc( int );
	SFuncType	GetTypeOfFunc( int );
	bool		IsFuncDefined( int );

	int			GetLocalVarSize( int );
	int			GetGlobalVarSize();
	int			GetStringBufferSize();

	void		StringBuffer( void * );

	void		Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////

#endif
