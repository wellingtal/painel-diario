
#include "IMCodeGen.h"
#include "IMCodeUnits.h"
#include "SymbolTable.h"
#include "SyntaxTree.h"
#include <assert.h>
#include <list>
#include <map>


///////////////////////////////////////////////////////////////////////////////////
//	��� ����

#define	MARK_BREAK		(IOPCode*)0
#define MARK_CONTINUE	(IOPCode*)1
#define MARK_RETURN		(IOPCode*)2



///////////////////////////////////////////////////////////////////////////////////
//	��������

static IMCODES *		g_pIMCodes = NULL;
static CSyntaxTree *	g_pSyntaxTree = NULL;
static CSymbolTable *	g_pSymbolTable = NULL;
static FUNCTIONS *		g_pFunctions = NULL;
static int				g_iCurrentFuncID = 0;

int		SWITCHTEMPVAR = 0;
int		FLOATUNIT = 0;
int		FLOATTEMP = 0;
int		STRINGTRUE = 0;
int		STRINGFALSE = 0;
int		CONVERSIONBUFFER = 0;


///////////////////////////////////////////////////////////////////////////////////
//

void	GenerateCode( SNode * pNode, eRegister reg );


///////////////////////////////////////////////////////////////////////////////////
//

void	HandleMARK( IMCODES::iterator start, IMCODES::iterator end, IOPCode * pJumpSpot, IOPCode * Mark )
{
	while( start != end )
	{
		if( *start == Mark )
		{
			*start = new COP_jmp( pJumpSpot );
		}
		start++;
	}
}

void	GenCode_DataTypeToString( eDataType type, eRegister reg )
{
	switch( type )
	{
	case T_INT :	//int�� ���ڿ��� ��ȯ
				g_pIMCodes->push_back( new COP_itoa( reg, VAR(CONVERSIONBUFFER) ) );
				g_pIMCodes->push_back( new COP_mov( reg, FUNC(0), VAR(CONVERSIONBUFFER) ) );
				break;

	case T_FLOAT :	//float�� ���ڿ��� ��ȯ
				g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), reg ) );
				g_pIMCodes->push_back( new COP_fld( FUNC(0), VAR(FLOATTEMP) ) );
				g_pIMCodes->push_back( new COP_fstp( FUNC(0), VAR(FLOATTEMP), true ) );
				g_pIMCodes->push_back( new COP_ftoa( reg, VAR(FLOATTEMP), VAR(CONVERSIONBUFFER) ) );
				g_pIMCodes->push_back( new COP_mov( reg, FUNC(0), VAR(CONVERSIONBUFFER) ) );
				break;

	case T_BOOL :	//bool�� ���ڿ��� ��ȯ
		{
				g_pIMCodes->push_back( new COP_cmp( reg, 0 ) );
				//false�� �� ����
				COP_jcc * pJmpSrc1 = new COP_jcc( E );
				g_pIMCodes->push_back( pJmpSrc1 );
				//"true"�� �ְ� ������ ����
				g_pIMCodes->push_back( new COP_mov( reg, FUNC(0), VAR(STRINGTRUE) ) );
				COP_jmp * pJmpSrc2 = new COP_jmp;
				g_pIMCodes->push_back( pJmpSrc2 );
				//"false"�� ����.
				g_pIMCodes->push_back( new COP_jmpmark( pJmpSrc1 ) );
				g_pIMCodes->push_back( new COP_mov( reg, FUNC(0), VAR(STRINGFALSE) ) );
				//��
				g_pIMCodes->push_back( new COP_jmpmark( pJmpSrc2 ) );
				break;
		}
	case T_STRING :	//��ȯ�� �ʿ䰡 �����Ƿ� ����
				break;

	default :		//�̿��� Ÿ���� ���� �� ����.
				assert( false );
	}
}

void	GenCode_Assert( SNode * pNode, eRegister )
{
	assert( false );
}

void	GenCode_Empty( SNode * pNode, eRegister  )
{
}

void	GenCode_Error_Statement( SNode * pNode, eRegister  )
{
	ErrorMessage( pNode->m_iLine, "Error Statement" );
}

void	GenCode_If_Statement( SNode * pNode, eRegister  )
{
	//1. ��
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	//2. 0�̸� 4�� ����
	g_pIMCodes->push_back( new COP_cmp( ECX, 0 ) );
	COP_jcc * pJmpSrc1 = new COP_jcc( E );
	g_pIMCodes->push_back( pJmpSrc1 );
	//3. ���๮
	GenerateCode( pNode->m_ArrPtrChilds[1], NONE );
	//4. if ��
	g_pIMCodes->push_back( new COP_jmpmark( pJmpSrc1 ) );
}

void	GenCode_If_Else_Statement( SNode * pNode, eRegister  )
{
	//1. ��
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );							//������ �κ�
	//2. 0�̸� 4�� ����
	g_pIMCodes->push_back( new COP_cmp( ECX, 0 ) );
	COP_jcc * pJmpSrc1 = new COP_jcc( E );
	g_pIMCodes->push_back( pJmpSrc1 );
	//3. ���๮, 5�� ����
	GenerateCode( pNode->m_ArrPtrChilds[1], NONE );							//then part
	COP_jmp * pJmpSrc2 = new COP_jmp;
	g_pIMCodes->push_back( pJmpSrc2 );
	//4. else ����
	g_pIMCodes->push_back( new COP_jmpmark( pJmpSrc1 ) );
	GenerateCode( pNode->m_ArrPtrChilds[2], NONE );							//else part
	//5. endif
	g_pIMCodes->push_back( new COP_jmpmark( pJmpSrc2 ) );					//endif
}

void	GenCode_Switch_Statement( SNode * pNode, eRegister  )
{
	//1.��
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR( SWITCHTEMPVAR ), ECX ) );		//������� �ӽ� ������ ����ְ�
	//2.��� case���� ����
	SNode * pNodeTemp = pNode->m_ArrPtrChilds[1];
	SNode * pNodeTemp2 = pNodeTemp;

	g_pIMCodes->push_back( new COP_nop );
	IMCODES::iterator CmpEnd = g_pIMCodes->end();
	CmpEnd--;

	while( pNodeTemp != NULL )
	{
		if( pNodeTemp->m_eType == TYPE_SWITCH_CASES )
			pNodeTemp2 = pNodeTemp->m_ArrPtrChilds[0];
		else if( pNodeTemp->m_eType == TYPE_CASE_ONE )
			pNodeTemp2 = pNodeTemp;
		else
			break;

		COP_cmp * p = new COP_cmp( FUNC(0), VAR( SWITCHTEMPVAR ), CONST( pNodeTemp2->m_ArrPtrChilds[0]->m_SymbolID ) );
		//2-1.��(������ �� �ڿ��� ����)
		g_pIMCodes->insert( CmpEnd, p );
		//2-2.������ case statement�� ����
		COP_jcc * pJmpSrc = new COP_jcc( E );
		g_pIMCodes->insert( CmpEnd, pJmpSrc );
		//2-3.case statement �߰�
		g_pIMCodes->push_back( new COP_jmpmark( pJmpSrc ) );
		GenerateCode( pNodeTemp2->m_ArrPtrChilds[1], NONE );	

		pNodeTemp = pNodeTemp->m_ArrPtrChilds[1];
	}
	//3.default�� ����(������ �� �ڿ��� ����)
	COP_jmp * pJmpSrc1 = new COP_jmp;
	g_pIMCodes->insert( CmpEnd, pJmpSrc1 );
	//4.default �߰�
	g_pIMCodes->push_back( new COP_jmpmark( pJmpSrc1 ) );
	GenerateCode( pNode->m_ArrPtrChilds[2], NONE );
	//5.end switch
	COP_jmpmark * pEndSwitch = new COP_jmpmark;
	g_pIMCodes->push_back( pEndSwitch );

	//6.start switch���� end switch���� ���� break mark�� ã�Ƽ� jump to end switch�� ��ü
	HandleMARK( CmpEnd, g_pIMCodes->end(), pEndSwitch, MARK_BREAK );
}

void	GenCode_FirstChild_Generate( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], reg );
}

void	GenCode_SecondChild_Generate( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[1], reg );							//ù��° �ڽ��� ����
}

void	GenCode_TwoChild_Generate( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], reg );
	GenerateCode( pNode->m_ArrPtrChilds[1], reg );
}

void	GenCode_For_Statement( SNode * pNode, eRegister  )
{
	//1. for_init ����( ó�� �ѹ��� ����� )
	if( pNode->m_ArrPtrChilds[0]->m_ArrPtrChilds[0] != NULL )
		GenerateCode( pNode->m_ArrPtrChilds[0]->m_ArrPtrChilds[0], NONE );
	//2. ������ �κ��� �ǳʶ�
	COP_jmp * pJmpSrc1 = new COP_jmp;
	g_pIMCodes->push_back( pJmpSrc1 );
	//3. for�� ������ �κ� ����
	COP_jmp * pJmpBack = new COP_jmp;
	COP_jmpmark * pJmpBackMark = new COP_jmpmark( pJmpBack );
	g_pIMCodes->push_back( pJmpBackMark );
	if( pNode->m_ArrPtrChilds[0] != NULL )
		GenerateCode( pNode->m_ArrPtrChilds[0]->m_ArrPtrChilds[2], NONE );
	g_pIMCodes->push_back( new COP_jmpmark( pJmpSrc1 ) );
	//4. for�� �� ����
	COP_jcc * pJmpToEnd;
	if( pNode->m_ArrPtrChilds[0]->m_ArrPtrChilds[1] != NULL )
	{
		GenerateCode( pNode->m_ArrPtrChilds[0]->m_ArrPtrChilds[1], ECX );
		//5. ������ ������ �� end if�� ����
		g_pIMCodes->push_back( new COP_cmp( ECX, 0 ) );
		pJmpToEnd = new COP_jcc( E );
		g_pIMCodes->push_back( pJmpToEnd );
	}
	
	IMCODES::iterator StatementStart = g_pIMCodes->end();
	StatementStart--;
	//6. statement ����
	GenerateCode( pNode->m_ArrPtrChilds[1], NONE );
	//7. 3���� ����
	g_pIMCodes->push_back( pJmpBack );

	//8. end for
	COP_jmpmark * pEndFor;
	if( pNode->m_ArrPtrChilds[0]->m_ArrPtrChilds[1] != NULL )
		pEndFor = new COP_jmpmark( pJmpToEnd );
	else
		pEndFor = new COP_jmpmark();
	g_pIMCodes->push_back( pEndFor );

	//break��ɵ� ó��
	HandleMARK( StatementStart, g_pIMCodes->end(), pEndFor, MARK_BREAK );
	//continue��ɵ� ó��
	HandleMARK( StatementStart, g_pIMCodes->end(), pJmpBackMark, MARK_CONTINUE );
}

void	GenCode_While_Statement( SNode * pNode, eRegister  )
{
	//0. Begin While
	COP_jmp * pJmpBack = new COP_jmp;
	COP_jmpmark * pJmpBackMark = new COP_jmpmark( pJmpBack );
	g_pIMCodes->push_back( pJmpBackMark );
	//1. ���� �˻�
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	//2. 0 (false)�̸� end While�� ����
	g_pIMCodes->push_back( new COP_cmp( ECX, 0 ) );
	COP_jcc * pJmpToEnd = new COP_jcc( E );
	g_pIMCodes->push_back( pJmpToEnd );
	//3. begin statement
	IMCODES::iterator beginStatement = g_pIMCodes->end();
	beginStatement--;
	//4. statement ����
	GenerateCode( pNode->m_ArrPtrChilds[1], NONE );
	//5. begin While�� ����
	g_pIMCodes->push_back( pJmpBack );
	//6. End While
	COP_jmpmark * pEndWhile = new COP_jmpmark( pJmpToEnd );
	g_pIMCodes->push_back( pEndWhile );

	//break ó��
	HandleMARK( beginStatement, g_pIMCodes->end(), pEndWhile, MARK_BREAK );
	//continue ó��
	HandleMARK( beginStatement, g_pIMCodes->end(), pJmpBackMark, MARK_CONTINUE );
}

void	GenCode_Break_Statement( SNode * pNode, eRegister  )
{	
	g_pIMCodes->push_back( MARK_BREAK );								//while, for, switch���� break����� ó���� �� �ֵ��� ǥ�ø� ���ܵд�.
}
void	GenCode_Continue_Statement( SNode * pNode, eRegister  )
{
	g_pIMCodes->push_back( MARK_CONTINUE );								//while, for���� continue����� ó���� �� �ֵ��� ǥ�ø� ���ܵд�.
}

void	GenCode_Return_Statement( SNode * pNode, eRegister  )
{
	if( pNode->m_ArrPtrChilds[0] != 0 )									//������ ���� ���� ��쿡�� EAX�� �־��ش�.
		GenerateCode( pNode->m_ArrPtrChilds[0], EAX );
	g_pIMCodes->push_back( MARK_RETURN );
}

void	GenCode_Variable( SNode * pNode, eRegister reg )
{
	if( reg != NONE )
		g_pIMCodes->push_back( new COP_mov( reg, FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
}

void	GenCode_Init_Declaration( SNode * pNode, eRegister )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );

	if( pNode->m_eReturnType == T_STRING )
	{
		//���������� ���� ���ڿ��� ��ȯ.
		GenCode_DataTypeToString( pNode->m_ArrPtrChilds[0]->m_eReturnType, ECX );

		g_pIMCodes->push_back( new COP_strlen( ECX ) );
		g_pIMCodes->push_back( new COP_push( ECX ) );
		g_pIMCodes->push_back( new COP_add( EAX, 1 ) );
		g_pIMCodes->push_back( new COP_malloc( EAX ) );							//�޸𸮸� ���� �Ҵ�.
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID), EAX ) );
		g_pIMCodes->push_back( new COP_pop( ECX ) );
		g_pIMCodes->push_back( new COP_strcpy( EAX, ECX ) );					//���ڿ� ����
	}
	else
	{
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), ECX ) );
	}

}

void	GenCode_Normal_Declaration( SNode * pNode, eRegister )
{
	if( pNode->m_eReturnType == T_STRING )
	{
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID), float(0x0) ) );
	}
}

void	GenCode_Array_Init( SNode * pNode, int childIndex )							//GenCode_Array_Initialize1, 2���� ���̴� �Լ�
{
	SNode * pNodeTemp = pNode->m_ArrPtrChilds[childIndex];
	SNode * pNodeTemp2 = NULL;
	int i = 0, arrSize = g_pSymbolTable->GetCountOfVar( pNode->m_SymbolID );

	while( pNodeTemp != NULL )
	{
		if( pNodeTemp->m_eType == TYPE_INITIALIZER_LIST )
			pNodeTemp2 = pNodeTemp->m_ArrPtrChilds[0];
		else
			pNodeTemp2 = pNodeTemp;

		GenerateCode( pNodeTemp2, ECX );
		
		if( pNode->m_eReturnType == T_STRING )
		{
			//���������� ���� ���ڿ��� ��ȯ.
			GenCode_DataTypeToString( pNodeTemp2->m_eReturnType, ECX );

			g_pIMCodes->push_back( new COP_strlen( ECX ) );
			g_pIMCodes->push_back( new COP_push( ECX ) );
			g_pIMCodes->push_back( new COP_add( EAX, 1 ) );
			g_pIMCodes->push_back( new COP_malloc( EAX ) );							//�޸𸮸� ���� �Ҵ�.
			g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID), i, EAX ) );
			g_pIMCodes->push_back( new COP_pop( ECX ) );
			g_pIMCodes->push_back( new COP_strcpy( EAX, ECX ) );					//���ڿ� ����
		}
		else
			g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), i, ECX ) );

		if( pNodeTemp->m_eType == TYPE_INITIALIZER_LIST )
		{
			if( ++i >= arrSize )
				break;
			pNodeTemp = pNodeTemp->m_ArrPtrChilds[1];
		}
		else
			break;		
	}
}

void	GenCode_Array_Initialize( SNode * pNode, eRegister  )
{
	GenCode_Array_Init( pNode, 0 );
}

void	GenCode_Array_Initialize2( SNode * pNode, eRegister  )
{
	GenCode_Array_Init( pNode, 1 );
}

void	GenCode_Function_Declaration( SNode * pNode, eRegister )
{
	IMCODES * pNewFunc = new IMCODES;
	if( g_pFunctions->insert( FUNCTIONS::value_type( pNode->m_SymbolID, pNewFunc ) ).second == false )
		ErrorMessage( pNode->m_iLine, "���� �Լ��� �� �� ������� �õ��߽��ϴ�." );

	SFuncType type = g_pSymbolTable->GetTypeOfFunc( pNode->m_SymbolID );
	int emptyCodeSize = type.GetArgCount() * 3 + 5;

	pNewFunc->push_back( new COP_push( EBP ) );
	pNewFunc->push_back( new COP_mov( EBP, ESP ) );

	for( int i = 0; i < emptyCodeSize; i++ )
		pNewFunc->push_back( new COP_nop );

	pNewFunc->push_back( new COP_mov( ESP, EBP ) );
	pNewFunc->push_back( new COP_pop( EBP ) );
	pNewFunc->push_back( new COP_ret );
}

void	GenCode_Function_Definition( SNode * pNode, eRegister  )
{
	IMCODES * pNewFunc = new IMCODES;
	if( g_pFunctions->insert( FUNCTIONS::value_type( pNode->m_SymbolID, pNewFunc ) ).second == false )
		ErrorMessage( pNode->m_iLine, "���� �Լ��� �� �� ������� �õ��߽��ϴ�." );

	swap( pNewFunc, g_pIMCodes );										//GenerateCode�� ���� ���� �Լ� �ڵ� �����̳ʿ� �ڵ带 �����ϵ���..
	g_iCurrentFuncID = pNode->m_SymbolID;

	//�Լ� �Ӹ�
	g_pIMCodes->push_back( new COP_push( EBP ) );
	g_pIMCodes->push_back( new COP_mov( EBP, ESP ) );

	int varSize = g_pSymbolTable->GetLocalVarSize( pNode->m_SymbolID );
	if( varSize )
		g_pIMCodes->push_back( new COP_sub( ESP, varSize ) );
	
	//�Լ��� ���� ũ�⸦ ���� ����� �־�� �Ѵ�.

	IMCODES::iterator beginFunc = g_pIMCodes->end();
	beginFunc--;
	if( pNode->m_ArrPtrChilds[0] != NULL )
		GenerateCode( pNode->m_ArrPtrChilds[0], NONE );
	GenerateCode( pNode->m_ArrPtrChilds[1], NONE );

	IMCODES::iterator endFunc = g_pIMCodes->end();

	//�Լ� ����
	COP_jmpmark * returnSpot = new COP_jmpmark;
	g_pIMCodes->push_back( returnSpot );
	g_pIMCodes->push_back( new COP_mov( ESP, EBP ) );
	g_pIMCodes->push_back( new COP_pop( EBP ) );
	g_pIMCodes->push_back( new COP_ret );

	HandleMARK( beginFunc, endFunc, returnSpot, MARK_RETURN );

	swap( pNewFunc, g_pIMCodes );
	g_iCurrentFuncID = 0;
}

void	GenCode_Constant_Expression( SNode * pNode, eRegister reg )
{
	g_pIMCodes->push_back( new COP_mov( reg, CONST( pNode->m_SymbolID ) ) );
}

void	GenCode_Assignment_Expression( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );

	if( pNode->m_eReturnType == T_STRING )
	{		
		//���������� ���� ���ڿ��� ��ȯ.
		GenCode_DataTypeToString( pNode->m_ArrPtrChilds[0]->m_eReturnType, ECX );

		g_pIMCodes->push_back( new COP_strlen( ECX ) );
		g_pIMCodes->push_back( new COP_push( ECX ) );
		g_pIMCodes->push_back( new COP_add( EAX, 1 ) );
		g_pIMCodes->push_back( new COP_malloc( EAX ) );							//�޸𸮸� ���� �Ҵ�.
		g_pIMCodes->push_back( new COP_mov( EBX, EAX ) );
		//���� ���۸� �����.
		g_pIMCodes->push_back( new COP_free( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID), EBX ) );
		g_pIMCodes->push_back( new COP_pop( ECX ) );
		g_pIMCodes->push_back( new COP_strcpy( EBX, ECX ) );					//���ڿ� ����
		if( reg != NONE && reg != EAX )
			g_pIMCodes->push_back( new COP_mov( reg, EAX ) );
	}
	else
	{
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), ECX ) );
		if( reg != NONE && reg != ECX )
			g_pIMCodes->push_back( new COP_mov( reg, ECX ) );
	}
}

void	GenCode_OR_Expression( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	g_pIMCodes->push_back( new COP_cmp( ECX, 1 ) );
	//ù��° ������ ���̸� ������� ���� �ִ� ������ ����
	COP_jcc * jmpSrc = new COP_jcc( E );
	g_pIMCodes->push_back( jmpSrc );
	GenerateCode( pNode->m_ArrPtrChilds[1], EDX );
	g_pIMCodes->push_back( new COP_cmp( EDX, 1 ) );
	//�ι�° ������ ���̸� ������� ���� �ִ� ������ ����
	COP_jcc * jmpSrc2 = new COP_jcc( E );
	g_pIMCodes->push_back( jmpSrc2 );
	//�� �� �����̸� ������� ������ �ְ� ������ ����
	g_pIMCodes->push_back( new COP_mov( reg, 0 ) );
	COP_jmp * jmpSrc3 = new COP_jmp;
	g_pIMCodes->push_back( jmpSrc3 );
	g_pIMCodes->push_back( new COP_jmpmark( jmpSrc ) );
	g_pIMCodes->push_back( new COP_jmpmark( jmpSrc2 ) );
	//������� ���� �ְ� ����
	g_pIMCodes->push_back( new COP_mov( reg, 1 ) );
	g_pIMCodes->push_back( new COP_jmpmark( jmpSrc3 ) );
}

void	GenCode_AND_Expression( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	g_pIMCodes->push_back( new COP_cmp( ECX, 0 ) );
	//ù��° ������ �����̸� ������� ������ �ִ� ������ ����
	COP_jcc * jmpSrc = new COP_jcc( E );
	g_pIMCodes->push_back( jmpSrc );
	GenerateCode( pNode->m_ArrPtrChilds[1], EDX );
	g_pIMCodes->push_back( new COP_cmp( EDX, 0 ) );
	//�ι�° ������ �����̸� ������� ������ �ִ� ������ ����
	COP_jcc * jmpSrc2 = new COP_jcc( E );
	g_pIMCodes->push_back( jmpSrc2 );
	//�� �� ���̸� ������� ���� �ְ� ������ ����
	g_pIMCodes->push_back( new COP_mov( reg, 1 ) );
	COP_jmp * jmpSrc3 = new COP_jmp;
	g_pIMCodes->push_back( jmpSrc3 );
	g_pIMCodes->push_back( new COP_jmpmark( jmpSrc ) );
	g_pIMCodes->push_back( new COP_jmpmark( jmpSrc2 ) );
	//������� ������ �ְ� ����
	g_pIMCodes->push_back( new COP_mov( reg, 0 ) );
	g_pIMCodes->push_back( new COP_jmpmark( jmpSrc3 ) );
}

void	GenCode_NOT_Expression( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	g_pIMCodes->push_back( new COP_cmp( ECX, 0 ) );
	if( reg != NONE )
		g_pIMCodes->push_back( new COP_setcc( E, reg ) );
}

void	GenCode_Equality_Expression( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	GenerateCode( pNode->m_ArrPtrChilds[1], EDX );

	if( pNode->m_ArrPtrChilds[0]->m_eReturnType == T_STRING )
	{
		g_pIMCodes->push_back( new COP_strcmp( ECX, EDX ) );
		g_pIMCodes->push_back( new COP_cmp( EAX, 0 ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( E, reg ) );
	}
	else
	{
		g_pIMCodes->push_back( new COP_cmp( ECX, EDX ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( E, reg ) );
	}
}

void	GenCode_NotEqual_Expression( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	GenerateCode( pNode->m_ArrPtrChilds[1], EDX );

	if( pNode->m_ArrPtrChilds[0]->m_eReturnType == T_STRING )
	{
		g_pIMCodes->push_back( new COP_strcmp( ECX, EDX ) );
		g_pIMCodes->push_back( new COP_cmp( EAX, 0 ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( NE, reg ) );
	}
	else
	{
		g_pIMCodes->push_back( new COP_cmp( ECX, EDX ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( NE, reg ) );
	}
}

void	GenCode_LessThan_Expression( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	GenerateCode( pNode->m_ArrPtrChilds[1], EDX );

	if( pNode->m_ArrPtrChilds[0]->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_cmp( ECX, EDX ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( L, reg ) );
	}
	else	//float
	{
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fld( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), EDX ) );
		g_pIMCodes->push_back( new COP_fcomp( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fnstsw_ax );
		g_pIMCodes->push_back( new COP_test_ah( 1 ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( NE, reg ) );
	}
}

void	GenCode_MoreThan_Expression( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	GenerateCode( pNode->m_ArrPtrChilds[1], EDX );

	if( pNode->m_ArrPtrChilds[0]->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_cmp( ECX, EDX ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( G, reg ) );
	}
	else
	{
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fld( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), EDX ) );
		g_pIMCodes->push_back( new COP_fcomp( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fnstsw_ax );
		g_pIMCodes->push_back( new COP_test_ah( 65 ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( E, reg ) );
	}
}

void	GenCode_LessThanEqual_Expression( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	GenerateCode( pNode->m_ArrPtrChilds[1], EDX );

	if( pNode->m_ArrPtrChilds[0]->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_cmp( ECX, EDX ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( LE, reg ) );	
	}
	else
	{
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fld( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), EDX ) );
		g_pIMCodes->push_back( new COP_fcomp( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fnstsw_ax );
		g_pIMCodes->push_back( new COP_test_ah( 65 ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( NE, reg ) );
	}
}

void	GenCode_MoreThanEqual_Expression( SNode * pNode, eRegister reg )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	GenerateCode( pNode->m_ArrPtrChilds[1], EDX );

	if( pNode->m_ArrPtrChilds[0]->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_cmp( ECX, EDX ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( GE, reg ) );
	}
	else
	{
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fld( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), EDX ) );
		g_pIMCodes->push_back( new COP_fcomp( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fnstsw_ax );
		g_pIMCodes->push_back( new COP_test_ah( 1 ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_setcc( E, reg ) );
	}
}



void	GenCode_Addition_Expression( SNode * pNode, eRegister reg  )
{
	if( pNode->m_eReturnType == T_INT )
	{
		GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
		GenerateCode( pNode->m_ArrPtrChilds[1], EDX );

		g_pIMCodes->push_back( new COP_add( ECX, EDX ) );
		if( reg != NONE && reg != ECX )
			g_pIMCodes->push_back( new COP_mov( reg, ECX ) );
	}
	else if( pNode->m_eReturnType == T_FLOAT )
	{
		GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
		GenerateCode( pNode->m_ArrPtrChilds[1], EDX );

		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fld( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), EDX ) );
		g_pIMCodes->push_back( new COP_fadd( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(0), VAR(FLOATTEMP) ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, FUNC(0), VAR(FLOATTEMP) ) );		
	}
	else	//string
	{
		g_pIMCodes->push_back( new COP_push( ESI ) );

		GenerateCode( pNode->m_ArrPtrChilds[0], EAX );
		GenCode_DataTypeToString( pNode->m_ArrPtrChilds[0]->m_eReturnType, EAX );
		g_pIMCodes->push_back( new COP_push( EAX ) );

		//���ڿ� ���� ���
		g_pIMCodes->push_back( new COP_strlen( EAX ) );
		g_pIMCodes->push_back( new COP_mov( EDI, EAX ) );

		GenerateCode( pNode->m_ArrPtrChilds[1], EBX );
		GenCode_DataTypeToString( pNode->m_ArrPtrChilds[1]->m_eReturnType, EBX );

		g_pIMCodes->push_back( new COP_strlen( EBX ) );
		g_pIMCodes->push_back( new COP_add( EAX, EDI ) );
		g_pIMCodes->push_back( new COP_add( EAX, 1 ) );
		//�޸𸮸� ���� �Ҵ�
		g_pIMCodes->push_back( new COP_malloc( EAX ) );
		//���ڿ� ����
		g_pIMCodes->push_back( new COP_pop( ESI ) );
		g_pIMCodes->push_back( new COP_strcpy( EAX, ESI ) );
		g_pIMCodes->push_back( new COP_mov( ESI, EAX ) );
		g_pIMCodes->push_back( new COP_add( EAX, EDI ) );		
		g_pIMCodes->push_back( new COP_strcpy( EAX, EBX ) );
		//���� �Ҵ�� �޸� �ּҸ� ��� �������Ϳ� ����
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, ESI ) );

		g_pIMCodes->push_back( new COP_pop( ESI ) );
	}
}

void	GenCode_Subtraction_Expression( SNode * pNode, eRegister reg  )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	GenerateCode( pNode->m_ArrPtrChilds[1], EDX );

	if( pNode->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_sub( ECX, EDX ) );
		if( reg != NONE && reg != ECX )
			g_pIMCodes->push_back( new COP_mov( reg, ECX ) );
	}
	else
	{
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fld( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), EDX ) );
		g_pIMCodes->push_back( new COP_fsub( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(0), VAR(FLOATTEMP) ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, FUNC(0), VAR(FLOATTEMP) ) );
	}
}

void	GenCode_Multiplication_Expression( SNode * pNode, eRegister reg  )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	GenerateCode( pNode->m_ArrPtrChilds[1], EDX );

	if( pNode->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_imul( ECX, EDX ) );
		if( reg != NONE && reg != ECX )
			g_pIMCodes->push_back( new COP_mov( reg, ECX ) );
	}
	else
	{
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fld( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), EDX ) );
		g_pIMCodes->push_back( new COP_fmul( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(0), VAR(FLOATTEMP) ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, FUNC(0), VAR(FLOATTEMP) ) );
	}
}

void	GenCode_Division_Expression( SNode * pNode, eRegister reg  )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], EAX );
	GenerateCode( pNode->m_ArrPtrChilds[1], ECX );

	if( pNode->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_idiv( ECX ) );
		if( reg != NONE && reg != EAX )
			g_pIMCodes->push_back( new COP_mov( reg, EAX ) );
	}
	else	//float
	{
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), EAX ) );
		g_pIMCodes->push_back( new COP_fld( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fdiv( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(0), VAR(FLOATTEMP) ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, FUNC(0), VAR(FLOATTEMP) ) );
	}
}

void	GenCode_Reminder_Expression( SNode * pNode, eRegister reg  )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], EAX );
	GenerateCode( pNode->m_ArrPtrChilds[1], ECX );
	g_pIMCodes->push_back( new COP_idiv( ECX ) );
	if( reg != NONE && reg != EDX )
		g_pIMCodes->push_back( new COP_mov( reg, EDX ) );
}

void	GenCode_Compound_Addition( SNode * pNode, eRegister reg  )
{	
	if( pNode->m_eReturnType == T_INT )
	{
		GenerateCode( pNode->m_ArrPtrChilds[0], ECX );

		g_pIMCodes->push_back( new COP_mov( EDX, FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ) ) );
		g_pIMCodes->push_back( new COP_add( EDX, ECX ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), EDX ) );
		if( reg != NONE && reg != EDX )
			g_pIMCodes->push_back( new COP_mov( reg, EDX ) );
	}
	else if( pNode->m_eReturnType == T_FLOAT )
	{
		GenerateCode( pNode->m_ArrPtrChilds[0], ECX );

		g_pIMCodes->push_back( new COP_fld( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fadd( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
	}
	else	//string
	{
		//���ڿ� ���� ���
		g_pIMCodes->push_back( new COP_strlen( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_mov( EBX, EAX ) );

		GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
		//������ Ÿ�Կ� ���� ���ڿ��� ��ȯ
		GenCode_DataTypeToString( pNode->m_ArrPtrChilds[0]->m_eReturnType, ECX );

		g_pIMCodes->push_back( new COP_strlen( ECX ) );
		g_pIMCodes->push_back( new COP_push( ECX ) );
		g_pIMCodes->push_back( new COP_add( EAX, EBX ) );
		g_pIMCodes->push_back( new COP_add( EAX, 1 ) );
		//�޸𸮸� ���� �Ҵ�
		g_pIMCodes->push_back( new COP_malloc( EAX ) );		
		//���ڿ� ����
		g_pIMCodes->push_back( new COP_strcpy( EAX, FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );		
		g_pIMCodes->push_back( new COP_pop( ECX ) );
		g_pIMCodes->push_back( new COP_push( EAX ) );
		g_pIMCodes->push_back( new COP_add( EAX, EBX ) );		
		g_pIMCodes->push_back( new COP_strcpy( EAX, ECX ) );
		//���� ���۸� ����� ���� �Ҵ�� �޸� �ּҸ� ��� ������ ����
		g_pIMCodes->push_back( new COP_free( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_pop( EAX ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID), EAX ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, EAX ) );
	}
}

void	GenCode_Compound_Subtraction( SNode * pNode, eRegister reg  )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );

	if( pNode->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_mov( EDX, FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ) ) );
		g_pIMCodes->push_back( new COP_sub( EDX, ECX ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), EDX ) );
		if( reg != NONE && reg != EDX )
			g_pIMCodes->push_back( new COP_mov( reg, EDX ) );
	}
	else
	{
		g_pIMCodes->push_back( new COP_fld( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fsub( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
	}
}

void	GenCode_Compound_Multiplication( SNode * pNode, eRegister reg  )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );

	if( pNode->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_mov( EDX, FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ) ) );
		g_pIMCodes->push_back( new COP_imul( EDX, ECX ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), EDX ) );
		if( reg != NONE && reg != EDX )
			g_pIMCodes->push_back( new COP_mov( reg, EDX ) );
	}
	else	//float
	{
		g_pIMCodes->push_back( new COP_fld( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fmul( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
	}
}

void	GenCode_Compound_Division( SNode * pNode, eRegister reg  )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );

	if( pNode->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_mov( EAX, FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ) ) );
		g_pIMCodes->push_back( new COP_idiv( ECX ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), EAX ) );
		if( reg != NONE && reg != EAX )
			g_pIMCodes->push_back( new COP_mov( reg, EAX ) );
	}
	else	//float
	{
		g_pIMCodes->push_back( new COP_fld( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(0), VAR(FLOATTEMP), ECX ) );
		g_pIMCodes->push_back( new COP_fdiv( FUNC(0), VAR(FLOATTEMP) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
	}
}

void	GenCode_Compound_Reminder( SNode * pNode, eRegister reg  )
{
	GenerateCode( pNode->m_ArrPtrChilds[0], ECX );
	g_pIMCodes->push_back( new COP_mov( EAX, FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ) ) );
	g_pIMCodes->push_back( new COP_idiv( ECX ) );
	g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), EDX ) );
	if( reg != NONE && reg != EDX )
		g_pIMCodes->push_back( new COP_mov( reg, EDX ) );	
}

void	GenCode_PrefixIncrement( SNode * pNode, eRegister reg  )
{
	if( pNode->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_mov( ECX, FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_add( ECX, 1 ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), ECX ) );
		if( reg != NONE && reg != ECX )
			g_pIMCodes->push_back( new COP_mov( reg, ECX ) );
	}
	else
	{
		const char * str = g_pSymbolTable->GetNameOfVar( pNode->m_SymbolID );
		g_pIMCodes->push_back( new COP_fld( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_fadd( FUNC(0), VAR(FLOATUNIT) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
	}
}

void	GenCode_PrefixDecrement( SNode * pNode, eRegister reg  )
{
	if( pNode->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_mov( ECX, FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ) ) );
		g_pIMCodes->push_back( new COP_sub( ECX, 1 ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), ECX ) );
		if( reg != NONE && reg != ECX )
			g_pIMCodes->push_back( new COP_mov( reg, ECX ) );
	}
	else	//float�� ��
	{
		g_pIMCodes->push_back( new COP_fld( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_fsub( FUNC(0), VAR(FLOATUNIT) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		if( reg != NONE )
			g_pIMCodes->push_back( new COP_mov( reg, FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
	}
}

void	GenCode_Array_Indexing( SNode * pNode, eRegister reg )
{
	g_pIMCodes->push_back( new COP_push( ESI ) );

	eRegister StoreReg = reg == ECX ? EDX : ECX;
	GenerateCode( pNode->m_ArrPtrChilds[0], StoreReg );
	if( reg != NONE )
		g_pIMCodes->push_back( new COP_mov( reg, FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID), StoreReg ) );

	g_pIMCodes->push_back( new COP_pop( ESI ) );
}

void	GenCode_Function_Call( SNode * pNode, eRegister reg )
{
	//���ڵ��� ��� ���ÿ� push�Ѵ�.
	SNode * pNodeTemp = pNode->m_ArrPtrChilds[0];
	int i = 0;

	while( pNodeTemp != NULL )
	{
		if( pNodeTemp->m_eType == TYPE_EXPRESSION_LIST )
		{
			GenerateCode( pNodeTemp->m_ArrPtrChilds[0], ECX );
			g_pIMCodes->push_back( new COP_push( ECX ) );
			i++;
			pNodeTemp = pNodeTemp->m_ArrPtrChilds[1];
		}
		else
		{
			GenerateCode( pNodeTemp, ECX );
			g_pIMCodes->push_back( new COP_push( ECX ) );
			i++;
			break;
		}		
	}

	g_pIMCodes->push_back( new COP_call( FUNC(pNode->m_SymbolID), pNode->m_iLine ) );
	//���ÿ� �־��� ���ڵ��� ��� pop�Ѵ�.( Stack Pointer�� �ű����ν�.. )
	g_pIMCodes->push_back( new COP_add( ESP, i*4 ) );
	if( reg != NONE && reg != EAX )
		g_pIMCodes->push_back( new COP_mov( reg, EAX ) );

}

void	GenCode_PostfixIncrement( SNode * pNode, eRegister reg  )
{
	if( reg != NONE )
		g_pIMCodes->push_back( new COP_mov( reg, FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ) ) );

	if( pNode->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_push( ESI ) );

		assert( reg != ESI );
		g_pIMCodes->push_back( new COP_mov( ESI, FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ) ) );
		g_pIMCodes->push_back( new COP_add( ESI, 1 ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), ESI ) );

		g_pIMCodes->push_back( new COP_pop( ESI ) );
	}
	else	//float �� ��
	{
		g_pIMCodes->push_back( new COP_fld( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_fadd( FUNC(0), VAR(FLOATUNIT) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
	}
}

void	GenCode_PostfixDecrement( SNode * pNode, eRegister reg  )
{
	if( reg != NONE )
		g_pIMCodes->push_back( new COP_mov( reg, FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ) ) );

	if( pNode->m_eReturnType == T_INT )
	{
		g_pIMCodes->push_back( new COP_push( ESI ) );

		assert( reg != ESI );
		g_pIMCodes->push_back( new COP_mov( ESI, FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ) ) );
		g_pIMCodes->push_back( new COP_sub( ESI, 1 ) );
		g_pIMCodes->push_back( new COP_mov( FUNC(g_iCurrentFuncID), VAR( pNode->m_SymbolID ), ESI ) );

		g_pIMCodes->push_back( new COP_pop( ESI ) );
	}
	else	//float�� ���
	{
		g_pIMCodes->push_back( new COP_fld( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
		g_pIMCodes->push_back( new COP_fsub( FUNC(0), VAR(FLOATUNIT) ) );
		g_pIMCodes->push_back( new COP_fstp( FUNC(g_iCurrentFuncID), VAR(pNode->m_SymbolID) ) );
	}
}

///////////////////////////////////////////////////////////////////////////////////
//

typedef void (*GENCODE)( SNode * pNode, eRegister );

GENCODE		g_GenCodeFunctions[] = {

	GenCode_TwoChild_Generate,			//TYPE_STATEMENT_LIST
	GenCode_Empty,
	GenCode_Error_Statement,
	GenCode_FirstChild_Generate,		//TYPE_EXPRESSION
	GenCode_If_Statement,
	GenCode_If_Else_Statement,
	GenCode_Switch_Statement,
	GenCode_Assert,						//TYPE_SWITCH_CASES
	GenCode_Assert,						//TYPE_CASE_ONE
	GenCode_FirstChild_Generate,		//TYPE_DEFAULT
	GenCode_For_Statement,
	GenCode_While_Statement,
	GenCode_Assert,						//TYPE_FOR_EXPRESSION
	GenCode_Break_Statement,
	GenCode_Continue_Statement,
	GenCode_Return_Statement,
	GenCode_SecondChild_Generate,
	GenCode_Assert,						//TYPE_SPECIFIER_VOID
	GenCode_Assert,						//TYPE_SPECIFIER_INT
	GenCode_Assert,						//TYPE_SPECIFIER_FLOAT
	GenCode_Assert,						//TYPE_SPECIFIER_BOOL
	GenCode_Assert,						//TYPE_SPECIFIER_STRING
	GenCode_Variable,
	GenCode_TwoChild_Generate,			//TYPE_DECLARATOR_LIST
	GenCode_Init_Declaration,
	GenCode_Normal_Declaration,
	GenCode_Array_Initialize,
	GenCode_Array_Initialize2,
	GenCode_Empty,
	GenCode_Function_Declaration,
	GenCode_TwoChild_Generate,			//TYPE_INITIALIZER_LIST
	GenCode_TwoChild_Generate,			//TYPE_ARGUMENT_DECLARATION_LIST
	GenCode_Empty,
	GenCode_Function_Definition,
	GenCode_Constant_Expression,
	GenCode_TwoChild_Generate,			//TYPE_EXPRESSION_LIST
	GenCode_Assignment_Expression,
	GenCode_OR_Expression,
	GenCode_AND_Expression,
	GenCode_NOT_Expression,
	GenCode_Equality_Expression,
	GenCode_NotEqual_Expression,
	GenCode_LessThan_Expression,
	GenCode_MoreThan_Expression,
	GenCode_LessThanEqual_Expression,
	GenCode_MoreThanEqual_Expression,
	GenCode_Addition_Expression,
	GenCode_Subtraction_Expression,
	GenCode_Multiplication_Expression,
	GenCode_Division_Expression,
	GenCode_Reminder_Expression,
	GenCode_Compound_Addition,
	GenCode_Compound_Subtraction,
	GenCode_Compound_Multiplication,
	GenCode_Compound_Division,
	GenCode_Compound_Reminder,
	GenCode_PrefixIncrement,
	GenCode_PrefixDecrement,
	GenCode_Array_Indexing,
	GenCode_Function_Call,
	GenCode_PostfixIncrement,
	GenCode_PostfixDecrement
};

void	GenerateCode( SNode * pNode, eRegister reg )
{
	(g_GenCodeFunctions[pNode->m_eType])( pNode, reg );
}

void	GenerateCode( CSyntaxTree & rSyntaxTree, IMCODES & rIMCodes, FUNCTIONS & rFunctions )
{
	g_pSyntaxTree = &rSyntaxTree;
	g_pSymbolTable = rSyntaxTree.GetSymbolTable();
	g_pIMCodes = &rIMCodes;
	g_pFunctions = &rFunctions;

	//��ũ��Ʈ �ý��� ������ �����.
	SWITCHTEMPVAR = g_pSymbolTable->AddVar( SYSVAR_SWITCHTEMP, T_INT );
	assert( SWITCHTEMPVAR != 0 );
	FLOATUNIT = g_pSymbolTable->AddVar( SYSVAR_FLOATUNIT, T_FLOAT );
	assert( FLOATUNIT != 0 );
	FLOATTEMP = g_pSymbolTable->AddArrVar( SYSVAR_FLOATTEMP, T_FLOAT, 2 );
	assert( FLOATTEMP != 0 );
	STRINGTRUE = g_pSymbolTable->AddVar( SYSVAR_TRUE, T_STRING );
	assert( STRINGTRUE != 0 );
	STRINGFALSE = g_pSymbolTable->AddVar( SYSVAR_FALSE, T_STRING );
	assert( STRINGFALSE != 0 );
	CONVERSIONBUFFER = g_pSymbolTable->AddVar( SYSVAR_CONVBUFFER, T_STRING );
	assert( CONVERSIONBUFFER != 0 );

	//�ڵ� ����.
	GenerateCode( rSyntaxTree.GetRoot(), NONE );

	//ó������ ���� break, continue���� ó���Ѵ�. 
	for( IMCODES::iterator i = g_pIMCodes->begin(); i != g_pIMCodes->end(); i++ )
	{
		if( *i == MARK_BREAK || *i == MARK_CONTINUE )
			*i = new COP_nop;
	}

	IMCODES::iterator begin = g_pIMCodes->begin();
	//main �Լ� �Ӹ�
	for( int j = 0; j < 3; j++ )
		g_pIMCodes->insert( begin, new COP_nop );

	g_pIMCodes->insert( begin, new COP_push( EBP ) );
	g_pIMCodes->insert( begin, new COP_mov( EBP, ESP ) );
	
	//main �Լ� ����
	COP_jmpmark * returnSpot = new COP_jmpmark;
	g_pIMCodes->push_back( returnSpot );
	g_pIMCodes->push_back( new COP_mov( ESP, EBP ) );
	g_pIMCodes->push_back( new COP_pop( EBP ) );
	g_pIMCodes->push_back( new COP_ret );

	for( int j = 0; j < 3; j++ )
		g_pIMCodes->push_back( new COP_int3 );

	HandleMARK( begin, g_pIMCodes->end(), returnSpot, MARK_RETURN );

	g_pSyntaxTree = NULL;
	g_pSymbolTable = NULL;
	g_pIMCodes = NULL;
	g_pFunctions = NULL;
}