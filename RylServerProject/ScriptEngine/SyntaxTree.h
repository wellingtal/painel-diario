
#ifndef _SyntaxTree_H_
#define _SyntaxTree_H_

#include "BaseDef.h"
#include "Message.h"
#include "STL.h"


#define MAXCHILD 3


class CSymbol;
class CSyntaxTree;
class CSymbolTable;

struct _iobuf;
typedef struct _iobuf FILE;


enum eSyntaxType
{
	TYPE_STATEMENT_LIST = 0,
	TYPE_EMPTY_STATEMENT,
	TYPE_ERROR_STATEMENT,
	TYPE_EXPRESSION,
	TYPE_IF_STATEMENT,
	TYPE_IF_ELSE_STATEMENT,
	TYPE_SWITCH_STATEMENT,
	TYPE_SWITCH_CASES,
	TYPE_CASE_ONE,
	TYPE_DEFAULT,
	TYPE_FOR_STATEMENT,
	TYPE_WHILE_STATEMENT,
	TYPE_FOR_EXPRESSION,
	TYPE_BREAK_STATEMENT,
	TYPE_CONTINUE_STATEMENT,
	TYPE_RETURN_STATEMENT,
	TYPE_DECLARATION,
	TYPE_SPECIFIER_VOID,
	TYPE_SPECIFIER_INT,
	TYPE_SPECIFIER_FLOAT,
	TYPE_SPECIFIER_BOOL,
	TYPE_SPECIFIER_STRING,
	TYPE_VARIABLE,
	TYPE_DECLARATOR_LIST,
	TYPE_INIT_DECLARATION,
	TYPE_NORMAL_DECLARATION,
	TYPE_ARRAY_INITIALIZE,
	TYPE_ARRAY_INITIALIZE2,
	TYPE_ARRAY_DECLARATION,
	TYPE_FUNCTION_DECLARATION,
	TYPE_INITIALIZER_LIST,
	TYPE_ARGUMENT_DECLARATION_LIST,
	TYPE_ARGUMENT_DECLARATION,
	TYPE_FUNCTION_DEFINITION,
	TYPE_CONSTANT_EXPRESSION,
	TYPE_EXPRESSION_LIST,
	TYPE_ASSIGNMENT_EXPRESSION,
	TYPE_OR_EXPRESSION,
	TYPE_AND_EXPRESSION,
	TYPE_NOT_EXPRESSION,
	TYPE_EQUALITY_EXPRESSION,
	TYPE_NOTEQAUL_EXPRESSION,
	TYPE_LESSTHAN_EXPRESSION,
	TYPE_MORETHAN_EXPRESSION,
	TYPE_LESSTHANOREQUAL_EXPRESSION,
	TYPE_MORETHANOREQUAL_EXPRESSION,
	TYPE_ADDITION_EXPRESSION,
	TYPE_SUBTRACTION_EXPRESSION,
	TYPE_MULTIPLICATION_EXPRESSION,
	TYPE_DIVISION_EXPRESSION,
	TYPE_REMINDER_EXPRESSION,
	TYPE_COMPOUND_ADDITION,
	TYPE_COMPOUND_SUBTRACTION,
	TYPE_COMPOUND_MULTIPLICATION,
	TYPE_COMPOUND_DIVISION,
	TYPE_COMPOUND_REMINDER,
	TYPE_PREFIXINCREMENT,
	TYPE_PREFIXDECREMENT,
	TYPE_ARRAY_INDEXING,
	TYPE_FUNCTION_CALL,
	TYPE_POSTFIXINCREMENT,
	TYPE_POSTFIXDECREMENT = 61,
};

///////////////////////////////////////////////////////////////////////////////////
//

struct SNode
{
	int				m_iLine;
	eSyntaxType		m_eType;
	SNode *			m_ArrPtrChilds[MAXCHILD];
	int				m_SymbolID;
	eDataType		m_eReturnType;

public:
	SNode( int, eSyntaxType, SNode *, SNode *, SNode * );
	void	Show( int, MESSAGE_FUNCTION2, CSymbolTable * );
	bool	SemanticCheck( CSymbolTable * );
};

class CSyntaxTree
{
protected:
	SNode *			m_pRoot;
	CSymbolTable *	m_pSymbolTable;

	typedef set<void *>				PTRSTORAGE;

	PTRSTORAGE *	m_pPtrStorage;


public:
	CSyntaxTree();
	~CSyntaxTree();

	void	Create( const char * );									//특정 스크립트 화일을 읽어서 Syntax트리를 구성한다.
	void	Create( FILE * );
	void	Destroy();

	int		Insert( int line, eSyntaxType, int childID1 = 0, int childID2 = 0, int childID3 = 0 );
	int		Insert( int line, int symbolID, eSyntaxType, int childID1 = 0, int childID2 = 0, int childID3 = 0 );
	void	SetRoot( int nodeID );
	void	Show( MESSAGE_FUNCTION2 );

	CSymbolTable *	GetSymbolTable()		{ return m_pSymbolTable; }

	SNode *	GetRoot()						{ return m_pRoot; }
};

///////////////////////////////////////////////////////////////////////////////////

#endif
