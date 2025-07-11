
#include "SyntaxTree.h"
#include "SymbolTable.h"
#include "lex.h"
#include <assert.h>
#include <set>

///////////////////////////////////////////////////////////////////////////////
// ��������

CSyntaxTree *			g_pSynTree = 0;
CSymbolTable *			g_pSymbolTable = 0;
char					szBuffer[4096];


///////////////////////////////////////////////////////////////////////////////
// Parser �Լ�
void yyerror( char *msg )
{
	CompilerMessage( msg );
}

extern "C" int yywrap(void) {
  return 1;
}


////////////////////////////////////////////////////////////////////////////////
// ��� �Լ�

SNode::SNode( int line, eSyntaxType type, SNode * pNode1, SNode * pNode2, SNode * pNode3 )
	: m_iLine( line ), m_eType( type ), m_SymbolID( 0 ), m_eReturnType( T_VOID )
{
	m_ArrPtrChilds[0] = pNode1;
	m_ArrPtrChilds[1] = pNode2;
	m_ArrPtrChilds[2] = pNode3;
}

const char * SyntaxTypeName[] = 
{
	"TYPE_STATEMENT_LIST",
	"TYPE_EMPTY_STATEMENT",
	"TYPE_ERROR_STATEMENT",
	"TYPE_EXPRESSION",
	"TYPE_IF_STATEMENT",
	"TYPE_IF_ELSE_STATEMENT",
	"TYPE_SWITCH_STATEMENT",
	"TYPE_SWITCH_CASES",
	"TYPE_CASE_ONE",
	"TYPE_DEFAULT",
	"TYPE_FOR_STATEMENT",
	"TYPE_WHILE_STATEMENT",
	"TYPE_FOR_EXPRESSION",
	"TYPE_BREAK_STATEMENT",
	"TYPE_CONTINUE_STATEMENT",
	"TYPE_RETURN_STATEMENT",
	"TYPE_DECLARATION",
	"TYPE_SPECIFIER_VOID",
	"TYPE_SPECIFIER_INT",
	"TYPE_SPECIFIER_FLOAT",
	"TYPE_SPECIFIER_BOOL",
	"TYPE_SPECIFIER_STRING",
	"TYPE_VARIABLE",
	"TYPE_DECLARATOR_LIST",
	"TYPE_INIT_DECLARATION",
	"TYPE_NORMAL_DECLARATION",
	"TYPE_ARRAY_INITIALIZE",
	"TYPE_ARRAY_INITIALIZE2",
	"TYPE_ARRAY_DECLARATION",
	"TYPE_FUNCTION_DECLARATION",
	"TYPE_INITIALIZER_LIST",
	"TYPE_ARGUMENT_DECLARATION_LIST",
	"TYPE_ARGUMENT_DECLARATION",
	"TYPE_FUNCTION_DEFINITION",
	"TYPE_CONSTANT_EXPRESSION",
	"TYPE_EXPRESSION_LIST",
	"TYPE_ASSIGNMENT_EXPESSION",
	"TYPE_OR_EXPRESSION",
	"TYPE_AND_EXPRESSION",
	"TYPE_NOT_EXPRESSION",
	"TYPE_EQUALITY_EXPRESSION",
	"TYPE_NOTEQAUL_EXPRESSION",
	"TYPE_LESSTHAN_EXPRESSION",
	"TYPE_MORETHAN_EXPRESSION",
	"TYPE_LESSTHANOREQUAL_EXPRESSION",
	"TYPE_MORETHANOREQUAL_EXPRESSION",
	"TYPE_ADDITION_EXPRESSION",
	"TYPE_SUBTRACTION_EXPRESSION",
	"TYPE_MULTIPLICATION_EXPRESSION",
	"TYPE_DIVISION_EXPRESSION",
	"TYPE_REMINDER_EXPRESSION",
	"TYPE_COMPOUND_ADDITION",
	"TYPE_COMPOUND_SUBTRACTION",
	"TYPE_COMPOUND_MULTIPLICATION",
	"TYPE_COMPOUND_DIVISION",
	"TYPE_COMPOUND_REMINDER",
	"TYPE_PREFIXINCREMENT",
	"TYPE_PREFIXDECREMENT",
	"TYPE_ARRAY_INDEXING",
	"TYPE_FUNCTION_CALL",
	"TYPE_POSTFIXINCREMENT",
	"TYPE_POSTFIXDECREMENT"
};

void	SNode::Show( int level, MESSAGE_FUNCTION2 Print, CSymbolTable * pSymbolTable )
{
	const char * DataTypeString[] = { "void", "bool", "int", "float", "string" };

	int nl = level;

	if (!this)  return;
	if( m_eType != TYPE_STATEMENT_LIST )
	{
		Print( itoa( m_iLine, szBuffer, 10 ) );
		Print( ": " );
		for ( int i = 0; i < level; i++ )
			Print( "   " );

		Print( SyntaxTypeName[m_eType] );

		switch( m_eType )
		{
		case TYPE_CONSTANT_EXPRESSION :
			Print( "  ( " );			
			Print( pSymbolTable->GetNameOfConst( m_SymbolID ) );
			Print( " : " );
			Print( DataTypeString[ pSymbolTable->GetTypeOfConst( m_SymbolID ) ] );
			Print( " )" );
			break;

		case TYPE_INIT_DECLARATION :
		case TYPE_NORMAL_DECLARATION :
		case TYPE_ARRAY_INITIALIZE :
		case TYPE_ARRAY_INITIALIZE2 :
		case TYPE_ARRAY_DECLARATION :
		case TYPE_ARGUMENT_DECLARATION :
		case TYPE_VARIABLE :
		case TYPE_PREFIXINCREMENT :
		case TYPE_PREFIXDECREMENT :
		case TYPE_POSTFIXINCREMENT :
		case TYPE_POSTFIXDECREMENT :
		case TYPE_COMPOUND_ADDITION :
		case TYPE_COMPOUND_SUBTRACTION :
		case TYPE_COMPOUND_MULTIPLICATION :
		case TYPE_COMPOUND_DIVISION :
		case TYPE_COMPOUND_REMINDER :			
			Print( "  ( " );			
			Print( pSymbolTable->GetNameOfVar( m_SymbolID ) );
			Print( " : " );
			Print( DataTypeString[ pSymbolTable->GetTypeOfVar( m_SymbolID ) ] );
			Print( " )" );
			break;
		}
		nl++;
		Print("\n");
	}
	
	for( int i = 0;  m_ArrPtrChilds[i] != 0 && i < 3; i++ )
		m_ArrPtrChilds[i]->Show( nl, Print, pSymbolTable );
}

bool SNode::SemanticCheck( CSymbolTable * pSymbolTable )
{
	static eDataType tempType, tempType2;

	switch( m_eType )
	{
	case TYPE_ARRAY_INDEXING :
			//ù��° int�϶��� ��ȿ�ϰ�, �ɺ��� Ÿ�Կ� ���� Ÿ���� ������.
			if( m_ArrPtrChilds[0]->m_eReturnType != T_INT )
				return false;
			else
				m_eReturnType = pSymbolTable->GetTypeOfVar( m_SymbolID );
			break;

	case TYPE_EXPRESSION :
			
			m_eReturnType = m_ArrPtrChilds[0]->m_eReturnType;
			break;


	case TYPE_ARRAY_DECLARATION :
			//ù��° ���ϵ尡 int�϶��� ��ȿ�ϰ�, �ɺ� Ÿ�Կ� ���� Ÿ���� ������.
			if( m_ArrPtrChilds[0]->m_eReturnType != T_INT )
				return false;
			else
				m_eReturnType = pSymbolTable->GetTypeOfVar( m_SymbolID );
			break;

	case TYPE_ARRAY_INITIALIZE2 :
			//�ɺ� Ÿ�԰� �ι��� ���ϵ��� Ÿ���� ������ �˻��ϰ� ���� �� �ɺ� Ÿ���� ����
			//ù��° ���ϵ�� int���̾�� ��.
			m_eReturnType = pSymbolTable->GetTypeOfVar( m_SymbolID );
			if( m_ArrPtrChilds[0]->m_eReturnType != T_INT )
				return false;
			if( m_eReturnType != m_ArrPtrChilds[1]->m_eReturnType )
				return false;
			break;

	case TYPE_ARRAY_INITIALIZE :
			//�ɺ� Ÿ�԰� ù��° ���ϵ��� Ÿ���� ���ƾ� ��.
			m_eReturnType = pSymbolTable->GetTypeOfVar( m_SymbolID );
			if( m_eReturnType != m_ArrPtrChilds[0]->m_eReturnType )
				return false;
			break;

	case TYPE_INIT_DECLARATION :
	case TYPE_ASSIGNMENT_EXPRESSION :
			//�ɺ� Ÿ�԰� ù��° ���ϵ��� Ÿ���� ������ �˻��ϰ� ���� �� �ɺ� Ÿ���� ����
			m_eReturnType = pSymbolTable->GetTypeOfVar( m_SymbolID );

			if( m_eReturnType == T_VOID )
				return false;

			if( m_eReturnType != m_ArrPtrChilds[0]->m_eReturnType )
			{
				if( m_eReturnType != T_STRING )
					return false;
			}
			break;


	case TYPE_INITIALIZER_LIST :
				
			m_eReturnType = m_ArrPtrChilds[0]->m_eReturnType;

			if( m_eReturnType == T_STRING )
			{
				if( m_ArrPtrChilds[1]->m_eReturnType == T_VOID )
					return false;
			}
			else if( m_ArrPtrChilds[1]->m_eReturnType == T_STRING )
			{
				if( m_eReturnType == T_VOID )
					return false;
				m_eReturnType = T_STRING;
			}
			else
			{
				if( m_eReturnType != m_ArrPtrChilds[1]->m_eReturnType )
					break;
			}
			break;


	case TYPE_DECLARATOR_LIST :
			//ù��° ���ϵ�� �ι�° ���ϵ��� Ÿ���� ������ �˻��ϰ� ���� �� ù��° ���ϵ� Ÿ�� ����
			if( m_ArrPtrChilds[0]->m_eReturnType != m_ArrPtrChilds[1]->m_eReturnType )
				return false;
			else
				m_eReturnType = m_ArrPtrChilds[0]->m_eReturnType;
			break;


	case TYPE_ADDITION_EXPRESSION :

			m_eReturnType = m_ArrPtrChilds[0]->m_eReturnType;

			if( m_eReturnType == T_STRING )
			{
				if( m_ArrPtrChilds[1]->m_eReturnType == T_VOID )
					return false;
			}
			else if( m_ArrPtrChilds[1]->m_eReturnType == T_STRING )
			{
				if( m_eReturnType == T_VOID )
					return false;
				m_eReturnType = T_STRING;
			}
			else
			{
				if( m_eReturnType != m_ArrPtrChilds[1]->m_eReturnType )
					return false;
			}
			break;

	case TYPE_SUBTRACTION_EXPRESSION :
	case TYPE_MULTIPLICATION_EXPRESSION :
	case TYPE_DIVISION_EXPRESSION :

			//ù��° ���ϵ�� �ι�° ���ϵ尡 �ٸ� ���� ��ȿ, int�� float���� �ƴ� ��쿡�� ��ȿ
			if( m_ArrPtrChilds[0]->m_eReturnType != m_ArrPtrChilds[1]->m_eReturnType )
			{
				return false;
			}
			m_eReturnType = m_ArrPtrChilds[0]->m_eReturnType;
			if( m_eReturnType != T_INT && m_eReturnType != T_FLOAT )
				return false;
			break;
			

	case TYPE_REMINDER_EXPRESSION :

			//ù��° ���ϵ�� �ι�° ���ϵ尡 �ٸ� ���� ��ȿ, int���� �ƴ� ��쿡�� ��ȿ
			if( m_ArrPtrChilds[0]->m_eReturnType != m_ArrPtrChilds[1]->m_eReturnType )
			{
				return false;
				break;
			}
			m_eReturnType = m_ArrPtrChilds[0]->m_eReturnType;
			if( m_eReturnType != T_INT )
				return false;
			break;


	case TYPE_PREFIXINCREMENT :
	case TYPE_PREFIXDECREMENT :
	case TYPE_POSTFIXINCREMENT :
	case TYPE_POSTFIXDECREMENT :

			//int�� float���� �ƴ� ��쿡�� ��ȿ.	
			m_eReturnType = pSymbolTable->GetTypeOfVar( m_SymbolID );
			if( m_eReturnType != T_INT && m_eReturnType != T_FLOAT )
				return false;
			break;

	
	case TYPE_NORMAL_DECLARATION :
	case TYPE_CONSTANT_EXPRESSION :
	case TYPE_VARIABLE :
			//�ɺ��� Ÿ���� �� ����� Ÿ���� ����
			m_eReturnType = pSymbolTable->GetTypeOfVar( m_SymbolID );

			break;

	case TYPE_NOT_EXPRESSION :
			//ù��° ���ϵ尡 BOOL���� ��쿡�� ��ȿ��.
			if( m_ArrPtrChilds[0]->m_eReturnType != T_BOOL )
				return false;
			else
				m_eReturnType = T_BOOL;
			break;

	case TYPE_OR_EXPRESSION :
	case TYPE_AND_EXPRESSION :

			//ù��° ���ϵ�� �ι�° ���ϵ尡 ��� bool���� ������ ��ȿ��.
			m_eReturnType = T_BOOL;

			if( m_ArrPtrChilds[0]->m_eReturnType != T_BOOL || m_ArrPtrChilds[1]->m_eReturnType != T_BOOL )
			{
				return false;
			}
			
			break;

	case TYPE_EQUALITY_EXPRESSION :
	case TYPE_NOTEQAUL_EXPRESSION :

			//���ʰ� �������� Ÿ���� ���ƾ� �ϸ�, void�� ��� �ȵ�.
			tempType = m_ArrPtrChilds[0]->m_eReturnType;
			if( tempType != m_ArrPtrChilds[1]->m_eReturnType || tempType == T_VOID )
				return false;
			else
				m_eReturnType = T_BOOL;
			break;	

	case TYPE_LESSTHAN_EXPRESSION :
	case TYPE_MORETHAN_EXPRESSION :
	case TYPE_LESSTHANOREQUAL_EXPRESSION :
	case TYPE_MORETHANOREQUAL_EXPRESSION :

			tempType = m_ArrPtrChilds[0]->m_eReturnType;
			//ù ��° ���ϵ�� �� ��° ���ϵ尡 �ٸ� ���� ��ȿ, int, float���� �ƴ� ��쿡�� ����
			if( tempType != m_ArrPtrChilds[1]->m_eReturnType )
				return false;
			if( tempType != T_INT && tempType != T_FLOAT )
				return false;
			m_eReturnType = T_BOOL;
			break;

	case TYPE_COMPOUND_ADDITION :

			//�ɺ��� Ÿ���� �� ����� Ÿ���� ����
			m_eReturnType = pSymbolTable->GetTypeOfVar( m_SymbolID );

			//int�� float, string���� �ƴ� ��� ��ȿ
			if( m_eReturnType != T_INT && m_eReturnType != T_FLOAT && m_eReturnType != T_STRING )
				return false;
			
			//�ɺ� Ÿ�԰� ù��° ���ϵ尡 �ٸ� ���� �ɺ� Ÿ���� string�� ��쿡�� ��ȿ
			if( m_eReturnType != m_ArrPtrChilds[0]->m_eReturnType )
			{
				if( m_eReturnType != T_STRING )
					return false;
				else
				{
					if( m_ArrPtrChilds[0]->m_eReturnType == T_VOID )
						return false;
				}
			}

			break;

	case TYPE_COMPOUND_SUBTRACTION :
	case TYPE_COMPOUND_MULTIPLICATION :
	case TYPE_COMPOUND_DIVISION :

			//�ɺ��� Ÿ���� �� ����� Ÿ���� ����
			m_eReturnType = pSymbolTable->GetTypeOfVar( m_SymbolID );

			//�ɺ� Ÿ�԰� ù��° ���ϵ尡 �ٸ� ���� ��ȿ, int�� float���� �ƴ� ��쿡�� ��ȿ
			if( m_eReturnType != m_ArrPtrChilds[0]->m_eReturnType )
			{
				return false;
			}
			if( m_eReturnType != T_INT && m_eReturnType != T_FLOAT )
				return false;
			break;

	case TYPE_COMPOUND_REMINDER :

			//�ɺ��� Ÿ���� �� ����� Ÿ���� ����
			m_eReturnType = pSymbolTable->GetTypeOfVar( m_SymbolID );

			//�ɺ� Ÿ�԰� ù��° ���ϵ尡 �ٸ� ���� ��ȿ, int���� �ƴ� ��쿡�� ��ȿ
			if( m_eReturnType != m_ArrPtrChilds[0]->m_eReturnType )
			{
				return false;
			}
			if( m_eReturnType != T_INT )
				return false;
			break;

	case TYPE_FUNCTION_CALL :
		{
			SFuncType type = pSymbolTable->GetTypeOfFunc( m_SymbolID );
			m_eReturnType = type.GetReturnType();
			break;
		}
	case TYPE_SWITCH_STATEMENT :
		{
			if( m_ArrPtrChilds[0]->m_eReturnType != T_INT )
				return false;

			m_eReturnType = T_VOID;
			break;
		}
	case TYPE_CASE_ONE :
		{
			if( m_ArrPtrChilds[0]->m_eReturnType != T_INT )
				return false;

			m_eReturnType = T_VOID;
			break;
		}
	default :

			m_eReturnType = T_VOID;
			break;
	}

	return true;
}

CSyntaxTree::CSyntaxTree()
	: m_pRoot( 0 )
	, m_pSymbolTable( new CSymbolTable )
	, m_pPtrStorage( new PTRSTORAGE )
{
}

CSyntaxTree::~CSyntaxTree()
{
	Destroy();

	delete m_pSymbolTable;
	delete m_pPtrStorage;
}

void CSyntaxTree::Create( const char * szFileName )
{
	FILE * fp = fopen( szFileName, "rt" );
	if( fp == NULL )
		return;
	yylloc.first_line = 1;

	Create( fp );
}

void CSyntaxTree::Create( FILE * fp )
{
	yyin = fp;
	g_pSynTree = this;
	g_pSymbolTable = m_pSymbolTable;
	g_pSymbolTable->Create();
	yylloc.first_line = 1;

	yyparse();
}

void CSyntaxTree::Destroy()
{
	typedef PTRSTORAGE::iterator	ITERATOR;

	for( ITERATOR i = m_pPtrStorage->begin(); i != m_pPtrStorage->end(); i++ )
	{
		delete (*i);
	}
	m_pPtrStorage->clear();
	
	m_pSymbolTable->Destroy();
}

int CSyntaxTree::Insert( int line, eSyntaxType SynType, int childID1, int childID2, int childID3 )
{
	SNode * pNode = new SNode( line, SynType, (SNode*)childID1, (SNode*)childID2, (SNode*)childID3 );

	assert( m_pPtrStorage->insert( pNode ).second );

	if( pNode->SemanticCheck( m_pSymbolTable ) == false )
		ErrorMessage( pNode->m_iLine, "Ÿ���� ���� �ʽ��ϴ�~\n" );

	return (int)pNode;
}
int	CSyntaxTree::Insert( int line, int symbolID, eSyntaxType SynType, int childID1, int childID2, int childID3 )
{
	SNode * pNode = new SNode( line, SynType, (SNode*)childID1, (SNode*)childID2, (SNode*)childID3 );

	assert( m_pPtrStorage->insert( pNode ).second );

	pNode->m_SymbolID = symbolID;

	if( pNode->SemanticCheck( m_pSymbolTable ) == false )
		CompilerMessage2( "Error(line %d) : Ÿ���� ���� �ʽ��ϴ�~\n", pNode->m_iLine );

	return (int)pNode;
}
void CSyntaxTree::SetRoot( int nodeID )
{
	if( nodeID != 0 )
	{
		if( m_pRoot != NULL )
		{
			Destroy();
		}
		m_pRoot = (SNode*)nodeID;
	}
}

void CSyntaxTree::Show( MESSAGE_FUNCTION2 Print )
{
	if( m_pRoot != NULL )
	{
		m_pRoot->Show( 0, Print, m_pSymbolTable );
	}
}
