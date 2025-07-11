
#include "IMCodeUnits.h"
#include <stdlib.h>
#include <map>
#include "RelocTable.h"
#include "SymbolTable.h"



///////////////////////////////////////////////////////////////////////////////////
//	전역변수

static char				szBuffer[128];
static const char *		g_szArrRegString[] = { "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI", "NONE" };
static const char *		g_szArrConditionString[] = { "G", "GE", "L", "LE", "E", "NE" };
static CSymbolTable *	g_pSymTable = NULL;
static FUNCTABLE *		g_pFuncTable = NULL;
static CRelocTable *	g_pRelocTable = NULL;

byte arrReg1[] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38 };
byte arrReg2[] = { 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78 };
byte arrReg3[] = { 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8 };
byte arrReg4[] = { 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8 };

///////////////////////////////////////////////////////////////////////////////////
//

void	SetSymbolTable( CSymbolTable * p )
{
	g_pSymTable = p;
}

void	SetFuncTable( FUNCTABLE * p )
{
	g_pFuncTable = p;
}

void	SetRelocTable( CRelocTable * p )
{
	g_pRelocTable = p;
}

///////////////////////////////////////////////////////////////////////////////////
//

union Conversion
{
	float	floatValue;
	int		intValue;
	byte	byteValue[4];

	Conversion()
		: intValue( 0 )
	{}
	Conversion( int v )
		: intValue( v )
	{}
	Conversion( float f )
		: floatValue( f )
	{}
	Conversion( CONST constID, int offset )		
	{
		eDataType type = g_pSymTable->GetTypeOfConst( int(constID) );
		const char * szValue = g_pSymTable->GetNameOfConst( int(constID) );
		
		switch( type )
		{
		case T_BOOL:
				if( strcmp( szValue, "true" ) == 0 )
					intValue = 1;
				else
					intValue = 0;
				break;
		case T_INT:
			{
				intValue = strtol( szValue, NULL, 0 );
				break;
			}
		case T_FLOAT:
				floatValue = (float)atof( szValue );
				break;
		case T_STRING:
				g_pRelocTable->AddConstString( offset );
				intValue = g_pSymTable->GetOffsetOfConst( int(constID ) );
				break;
		}
	}

	void	Set( byte * p )
	{
		p[0] = byteValue[0];
		p[1] = byteValue[1];
		p[2] = byteValue[2];
		p[3] = byteValue[3];
	}
};

///////////////////////////////////////////////////////////////////////////////////
//

int		Code_RM32_Offset( byte * p, byte opByte, int opDigit, int funcID, long addr, int VarOffset )
{
	p[0] = opByte;
	
	if( funcID == 0 )
	{
		p[1] = arrReg1[opDigit] + 5;
		Conversion value( VarOffset );
		value.Set( p + 2 );
		g_pRelocTable->AddGlobalVar( addr + 2 );				//disp32
		return 6;
	}
	else
	{
		if( VarOffset & 0xffffff00 )
		{
			p[1] = arrReg3[opDigit] + 5;						//disp32[EBP]
			Conversion value( VarOffset );
			value.Set( p + 2 );
			return 6;
		}
		else
		{
			p[1] = arrReg2[opDigit] + 5;
			p[2] = VarOffset & 0xff;							//disp8[EBP]
			return 3;
		}
	}
}

inline 
int		Code_RM32( byte * p, byte opByte, int opDigit, int funcID, int varID, long addr )
{
	int offset = g_pSymTable->GetOffsetOfVar( varID );
	return Code_RM32_Offset( p, opByte, opDigit, funcID, addr, offset );
}

long	Code_RM32_CodeSize_Offset( int funcID, int VarOffset )
{
	if( funcID == 0 || VarOffset & 0xffffff00 )
		return 6;
	else
		return 3;	
}

inline
long	Code_RM32_CodeSize( int funcID, int varID )
{
	int offset = g_pSymTable->GetOffsetOfVar( int(varID) );
	return Code_RM32_CodeSize_Offset( funcID, offset );	
}

///////////////////////////////////////////////////////////////////////////////////
//	COP_call

COP_call::COP_call( FUNC funcID, int line )
	: m_funcID( funcID ), m_iLine( line )
{}

int		COP_call::ToMachineCode( byte * p )
{
	FUNCTABLE::iterator result = g_pFuncTable->find( int(m_funcID) );
	if( result == g_pFuncTable->end() )
	{
		WarningMessage2( m_iLine, "정의되지 않은 함수를 호출하려 시도했습니다. : %s"
							, g_pSymTable->GetTypeStringOfFunc( int(m_funcID) ) );
	}

	p[0] = 0xe8;
	Conversion value( result->second - (m_lAddress+5) );
	value.Set( p + 1 );

	return 5;
}

long	COP_call::Addressing( long addr )
{ 
	m_lAddress = addr;
	return addr + 5;
}

void	COP_call::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "CALL\t\t%s\n", g_pSymTable->GetNameOfFunc( int(m_funcID) ) );
}





///////////////////////////////////////////////////////////////////////////////////
//	COP_mov

COP_mov::COP_mov( FUNC funcID, VAR varID, eRegister reg )
	: m_Type( 0 ), m_funcID( funcID ), m_leftVarID( varID ), m_midArrayIndex( 0 ), m_rightReg( reg )
{}
COP_mov::COP_mov( eRegister reg, FUNC funcID, VAR varID )
	: m_Type( 1 ), m_funcID( funcID ), m_leftReg( reg ), m_midArrayIndex( 0 ), m_rightVarID( varID )
{}
COP_mov::COP_mov( eRegister reg, CONST constID )
	: m_Type( 2 ), m_funcID( 0 ), m_leftReg( reg ), m_midArrayIndex( 0 ), m_rightConstID( constID )
{}
COP_mov::COP_mov( FUNC funcID, VAR varID, float valuef )
	: m_Type( 3 ), m_funcID( funcID ), m_leftVarID( varID ), m_midArrayIndex( 0 ), m_rightValuef( valuef )
{}
COP_mov::COP_mov( eRegister lreg, eRegister rreg )
	: m_Type( 4 ), m_funcID( 0 ), m_leftReg( lreg ), m_midArrayIndex( 0 ), m_rightReg( rreg )
{}
COP_mov::COP_mov( eRegister lreg, int value )
	: m_Type( 5 ), m_funcID( 0 ), m_leftReg( lreg ), m_midArrayIndex( 0 ), m_rightValue( value )
{}
COP_mov::COP_mov( FUNC funcID, VAR varID, int arrayIndex, eRegister rreg )
	: m_Type( 6 ), m_funcID( funcID ), m_leftVarID( varID ), m_midArrayIndex( arrayIndex ), m_rightReg( rreg )
{}
COP_mov::COP_mov( eRegister lreg, FUNC funcID, VAR varID, eRegister indexReg )
	: m_Type( 7 ), m_funcID( funcID ), m_leftReg( lreg ), m_midIndexReg( indexReg ), m_rightVarID( varID )
{}

int		COP_mov::ToMachineCode( byte * p )
{
	switch( m_Type )
	{
	case 0:		//89 / r MOV r/m32,r32 Move r32 to r/m32
	case 6:
		{
				int VarOffset = g_pSymTable->GetOffsetOfVar( int(m_leftVarID) ) + (m_midArrayIndex*4);
				return Code_RM32_Offset( p, 0x89, m_rightReg, int(m_funcID), m_lAddress, VarOffset );
		}
	case 1:		//8B / r MOV r32,r/m32 Move r/m32 to r32
				return Code_RM32( p, 0x8b, m_leftReg, int(m_funcID), int(m_rightVarID), m_lAddress );

	case 2:		//B8+ rd MOV r32,imm32 Move imm32 to r32
		{
				p[0] = 0xb8 + m_leftReg;

				Conversion value( m_rightConstID, m_lAddress + 1 );
				value.Set( p + 1 );
				return 5;
		}
	case 3:		//C7 / 0 MOV r/m32,imm32 Move imm32 to r/m32
		{
				int codeSize = Code_RM32( p, 0xc7, 0, int(m_funcID), int(m_leftVarID), m_lAddress );
				Conversion value( m_rightValuef );
				value.Set( p + codeSize );
				return codeSize + 4;
		}
	case 4:		//89 / r MOV r/m32,r32 Move r32 to r/m32
		{
				p[0] = 0x89;
				p[1] = arrReg4[m_rightReg] + m_leftReg;
				return 2;
		}
	case 5:		//C7 / 0 MOV r/m32,imm32 Move imm32 to r/m32
		{
				p[0] = 0xc7;
				p[1] = 0xc0 + m_leftReg;
				Conversion value( m_rightValue );
				value.Set( p + 2 );
				return 6;
		}
	case 7:
		{		
				//IMUL	m_midIndexReg, -4
				//6B / r ib IMUL r32,imm8 doubleword register ￢ doubleword register * signextended immediate byte
				p[0] = 0x6b;
				p[1] = 0xc0 + (m_midIndexReg * 9);
				p[2] = 0x04;

				int offset = g_pSymTable->GetOffsetOfVar( int(m_rightVarID) );
				if( m_funcID == 0 )
				{
					//8B / r MOV r32,r/m32 Move r/m32 to r32
					p[3] = 0x8b;
					p[4] = arrReg3[m_leftReg] + m_midIndexReg;
					Conversion value( offset );
					value.Set( p + 5 );
					g_pRelocTable->AddGlobalVar( m_lAddress + 5 );
					return 9;
				}
				else
				{
					//ADD	m_midIndexReg, EBP
					//03 / r ADD r32,r/m32 Add r/m32 to r32
					p[3] = 0x03;
					p[4] = arrReg4[m_midIndexReg] + 5;

					//8B / r MOV r32,r/m32 Move r/m32 to r32
					p[5] = 0x8b;
					p[6] = arrReg3[m_leftReg] + m_midIndexReg;
					Conversion value( offset );
					value.Set( p + 7 );
					return 11;
				}
				break;
		}
	}
	return 0;
}

long	COP_mov::Addressing( long addr )
{ 
	m_lAddress = addr;

	switch( m_Type )
	{
	case 0:
	case 6:
		{
			int offset = g_pSymTable->GetOffsetOfVar( int(m_leftVarID) ) + (m_midArrayIndex*4);
			return addr + Code_RM32_CodeSize_Offset( int(m_funcID), offset );
		}
	case 1:	
		{
			int offset = g_pSymTable->GetOffsetOfVar( int(m_rightVarID) ) + (m_midArrayIndex*4);
			return addr + Code_RM32_CodeSize_Offset( int(m_funcID), offset );
		}
	case 2:
			return addr + 5;
	case 3:
		{
			int offset = g_pSymTable->GetOffsetOfVar( int(m_leftVarID) ) + (m_midArrayIndex*4);
			return addr + 4 + Code_RM32_CodeSize_Offset( int(m_funcID), offset );
		}
	case 4:
			return addr + 2;
	case 5:
			return addr + 6;
	case 7:
			return addr + ( m_funcID == 0 ? 9 : 11 );
	}

	return 0;
}

void	COP_mov::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "MOV\t\t" );

	switch( m_Type )
	{
	case 0:
		Print( "%s, %s\n", g_pSymTable->GetNameOfVar( int(m_leftVarID) )
						, g_szArrRegString[m_rightReg] );
		break;
	case 1:
		Print( "%s, %s\n", g_szArrRegString[m_leftReg]
						, g_pSymTable->GetNameOfVar( int(m_rightVarID) ) );
		break;
	case 2:
		Print( "%s, %s\n", g_szArrRegString[m_leftReg]
						, g_pSymTable->GetNameOfConst( int(m_rightConstID) ) );
		break;
	case 3:
		Print( "%s, %f\n", g_pSymTable->GetNameOfVar( int(m_leftVarID) )
						, m_rightValuef );
		break;
	case 4:
		Print( "%s, %s\n", g_szArrRegString[m_leftReg]
						, g_szArrRegString[m_rightReg] );
		break;
	case 5:
		Print( "%s, %d\n", g_szArrRegString[m_leftReg]
						, m_rightValue );
		break;
	case 6:
		Print( "%s[%d], %s\n", g_pSymTable->GetNameOfVar( int(m_leftVarID) )
						, m_midArrayIndex, g_szArrRegString[m_rightReg] );
		break;
	case 7:
		Print( "%s, %s[%s]\n", g_szArrRegString[m_leftReg]
						, g_pSymTable->GetNameOfVar( int(m_rightVarID) )
						, g_szArrRegString[m_midIndexReg] );
		break;
	}		
}





///////////////////////////////////////////////////////////////////////////////////
//	COP_nop

int		COP_nop::ToMachineCode( byte * p )
{
	p[0] = 0x90;
	return 1;
}

long	COP_nop::Addressing( long addr )
{ 
	m_lAddress = addr;
	return addr + 1;
}

void	COP_nop::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "NOP\n" );
}





///////////////////////////////////////////////////////////////////////////////////
//	COP_add

COP_add::COP_add( eRegister reg, int value )
	: m_Type( 0 ), m_leftReg( reg ), m_iValue( value )
{}
COP_add::COP_add( eRegister lreg, eRegister rreg )
	: m_Type( 1 ), m_leftReg( lreg ), m_rightReg( rreg )
{}

int		COP_add::ToMachineCode( byte * p )
{
	switch( m_Type )
	{
	case 0:
		{
			//81 /0 id ADD r/m32,imm32 Add imm32 to r/m32
			p[0] = 0x81;
			p[1] = 0xc0 + m_leftReg;
			
			Conversion value( m_iValue );
			value.Set( p + 2 );
			return 6;
		}
	case 1:
			//03 / r ADD r32,r/m32 Add r/m32 to r32
			p[0] = 0x03;
			p[1] = arrReg4[m_leftReg] + m_rightReg;
			return 2;
	}
	return 0;
}

long	COP_add::Addressing( long addr )
{ 
	m_lAddress = addr;
	switch( m_Type )
	{
	case 0:	return addr + 6;
	case 1: return addr + 2;
	}
	return 0;
}

void	COP_add::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "ADD\t\t" );
	switch( m_Type )
	{
	case 0:		Print( "%s, %d\n", g_szArrRegString[m_leftReg], m_iValue );
				break;
	case 1:		Print( "%s, %s\n", g_szArrRegString[m_leftReg], g_szArrRegString[m_rightReg] );
				break;
	}		
}





///////////////////////////////////////////////////////////////////////////////////
//	COP_sub


COP_sub::COP_sub( eRegister reg, int value )
	: m_Type( 0 ), m_leftReg( reg ), m_iValue( value )
{}
COP_sub::COP_sub( eRegister lreg, eRegister rreg )
	: m_Type( 1 ), m_leftReg( lreg ), m_rightReg( rreg )
{}

int		COP_sub::ToMachineCode( byte * p )
{
	switch( m_Type )
	{
	case 0:
		{
			//81 /5 id SUB r/m32,imm32 Subtract imm32 from r/m32
			p[0] = 0x81;
			p[1] = 0xe8 + m_leftReg;

			Conversion value( m_iValue );
			value.Set( p + 2 );
			return 6;
		}
	case 1:
			//2B / r SUB r32,r/m32 Subtract r/m32 from r32
			p[0] = 0x2b;
			p[1] = arrReg4[m_leftReg] + m_rightReg;
			return 2;
	}
	return 0;
}
long	COP_sub::Addressing( long addr )
{ 
	m_lAddress = addr;

	switch( m_Type )
	{
	case 0:
		return addr + 6;
	case 1:
		return addr + 2;
	}
	return 0;
}
void	COP_sub::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "SUB\t\t" );
	switch( m_Type )
	{
	case 0:
		Print( "%s, %d\n", g_szArrRegString[m_leftReg], m_iValue );
		break;
	case 1:
		Print( "%s, %s\n", g_szArrRegString[m_leftReg], g_szArrRegString[m_rightReg] );
		break;
	}
}




///////////////////////////////////////////////////////////////////////////////////
//	COP_imul	

COP_imul::COP_imul( eRegister lreg, eRegister rreg )
	: m_leftReg( lreg ), m_rightReg( rreg )
{}
int		COP_imul::ToMachineCode( byte * p )
{
	//0F AF / r IMUL r32,r/m32 doubleword register <- doubleword register * r/m doubleword
	p[0] = 0x0f;
	p[1] = 0xaf;
	p[2] = arrReg4[m_leftReg] + m_rightReg;

	return 3;
}
long	COP_imul::Addressing( long addr )
{ 
	m_lAddress = addr;
	return addr + 3;
}
void	COP_imul::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "IMUL\t\t" );
	Print( "%s, %s\n", g_szArrRegString[m_leftReg], g_szArrRegString[m_rightReg] );
}




///////////////////////////////////////////////////////////////////////////////////
//	COP_idiv


COP_idiv::COP_idiv( eRegister rreg )
	: m_rightReg( rreg )
{
}
int		COP_idiv::ToMachineCode( byte * p )
{
	//99 CDQ EDX:EAX ￢ sign-extend of EAX
	p[0] = 0x99;

	//F7 /7 IDIV r/m32 Signed divide EDX:EAX (where EDX must contain sign-extension of EAX) 
											//by r/m doubleword. (Results:EAX=Quotient, EDX=Remainder)
	p[1] = 0xf7;
	p[2] = 0xf8 + m_rightReg;
	return 3;
}
long	COP_idiv::Addressing( long addr )
{ 
	m_lAddress = addr;
	return addr + 3;
}
void	COP_idiv::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "IDIV\t\t" );
	Print( "%s\n", g_szArrRegString[m_rightReg] );
}





///////////////////////////////////////////////////////////////////////////////////
//	COP_push

COP_push::COP_push( eRegister reg )
	: m_Reg( reg )
{}

int		COP_push::ToMachineCode( byte * p )
{
	p[0] = 0x50 + m_Reg;
	return 1;
}

long	COP_push::Addressing( long addr )
{ 
	m_lAddress = addr;
	return addr + 1;
}

void	COP_push::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "PUSH\t\t%s\n", g_szArrRegString[m_Reg] );
}


///////////////////////////////////////////////////////////////////////////////////
//	COP_pop

COP_pop::COP_pop( eRegister reg )
	: m_Reg( reg )
{}


int		COP_pop::ToMachineCode( byte * p )
{
	p[0] = 0x58 + m_Reg;		//58+ rd POP r32 Pop top of stack into r32; increment stack pointer
	return 1;
}

long	COP_pop::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + 1;
}

void	COP_pop::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "POP\t\t%s\n", g_szArrRegString[m_Reg] );
}



///////////////////////////////////////////////////////////////////////////////////
//	COP_jmp

COP_jmp::COP_jmp()
	: m_pJumpTarget( 0 )
{
}
COP_jmp::COP_jmp( IOPCode * pJmpTarget )
	: m_pJumpTarget( pJmpTarget )
{
}

COP_jmp::~COP_jmp()
{
}

int		COP_jmp::ToMachineCode( byte * p )
{
	//E9 cd JMP rel32 Jump near, relative, displacement relative to next instruction
	p[0] = 0xe9;
	Conversion i = m_pJumpTarget->m_lAddress - ( m_lAddress + 5 );
	i.Set( p + 1 );
	return 5;
}

long	COP_jmp::Addressing( long addr )
{ 
	m_lAddress = addr;
	return addr + 5;
}

void	COP_jmp::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "JMP\t\t%X\n", m_pJumpTarget->m_lAddress );
}





///////////////////////////////////////////////////////////////////////////////////
//	COP_jcc

COP_jcc::COP_jcc( eCondition condition )
	: m_Condition( condition )
{}

int		COP_jcc::ToMachineCode( byte * p )
{
	p[0] = 0x0f;

	Conversion i = m_pJumpTarget->m_lAddress - ( m_lAddress + 6 );
	i.Set( p + 2 );

	//모두 relative 점프임
	switch( m_Condition )
	{
	case G:
		//0F 8F cw/cd JG rel16/32 Jump near if greater (ZF=0 and SF=OF)
		p[1] = 0x8f;
		break;
	case GE:
		//0F 8D cw/cd JGE rel16/32 Jump near if greater or equal (SF=OF)
		p[1] = 0x8d;
		break;
	case L:
		//0F 8C cw/cd JL rel16/32 Jump near if less (SF<>OF)
		p[1] = 0x8c;
		break;
	case LE:
		//0F 8E cw/cd JLE rel16/32 Jump near if less or equal (ZF=1 or SF<>OF)
		p[1] = 0x8e;
		break;
	case E:
		//0F 84 cw/cd JE rel16/32 Jump near if equal (ZF=1)
		p[1] = 0x84;
		break;
	case NE:
		//0F 85 cw/cd JNE rel16/32 Jump near if not equal (ZF=0)
		p[1] = 0x85;
		break;
	}
	return 6;
}

long	COP_jcc::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + 6;
}

void	COP_jcc::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "J%s\t\t%X\n", g_szArrConditionString[m_Condition], m_pJumpTarget->m_lAddress );
}




///////////////////////////////////////////////////////////////////////////////////
//	COP_jmpmark

COP_jmpmark::COP_jmpmark()
	: m_pJumpSrc( 0 )
{}

COP_jmpmark::COP_jmpmark( COP_jmp * pOPjmp )
	: m_pJumpSrc( pOPjmp )
{
	pOPjmp->m_pJumpTarget = this;
}

int		COP_jmpmark::ToMachineCode( byte * )
{
	//코드를 만들지 않음.
	return 0;
}
long	COP_jmpmark::Addressing( long addr )
{
	m_lAddress = addr;
	//주소를 증가시키지 않음으로써 다음 instruction과 같은 주소를 가짐.
	return addr;
}
void	COP_jmpmark::Show( MESSAGE_FUNCTION2 Print )
{
	if( m_pJumpSrc )
		Print( "JMPMARK\t\t%X\n", m_pJumpSrc->m_lAddress );
	else
		Print( "JMPMARK\t\t\n" );
}




///////////////////////////////////////////////////////////////////////////////////
//	COP_cmp

COP_cmp::COP_cmp( FUNC funcID, VAR lvarID, CONST rconstID )
	: m_Type( 0 ), m_funcID( funcID ), m_leftVarID( lvarID ), m_rightConstID( rconstID )
{}

COP_cmp::COP_cmp( eRegister lreg, byte value )
	: m_Type( 1 ), m_funcID( 0 ), m_leftReg( lreg ), m_rightValue( value )
{}
		
COP_cmp::COP_cmp( eRegister lreg, eRegister rreg )
	: m_Type( 2 ), m_funcID( 0 ), m_leftReg( lreg ), m_rightReg( rreg )
{}

int		COP_cmp::ToMachineCode( byte * p )
{
	switch( m_Type )
	{
	case 0:
		{
			p[0] = 0x81;

			int offset = g_pSymTable->GetOffsetOfVar( int(m_leftVarID) );

			if( m_funcID == 0 )
			{
				p[1] = 0x3d;
				Conversion value( offset );
				value.Set( p + 2 );
				g_pRelocTable->AddGlobalVar( m_lAddress + 2 );
				Conversion value2( m_rightConstID, m_lAddress + 6 );
				value2.Set( p + 6 );
				return 10;
			}
			else
			{
				if( offset & 0xffffff00 )
				{
					p[1] = 0xbd;
					Conversion value( offset );
					value.Set( p + 2 );
					Conversion value2( m_rightConstID, m_lAddress + 6 );
					value2.Set( p + 6 );
					return 10;
				}
				else
				{
					p[1] = 0x7d;
					p[2] = offset & 0xff;
					Conversion value2( m_rightConstID, m_lAddress + 3 );
					value2.Set( p + 3 );
					return 7;
				}
			}
			break;
		}		
	case 1:
		//83 /7 ib CMP r/m32,imm8 Compare imm8 with r/m32
		p[0] = 0x83;
		p[1] = 0xf8 + m_leftReg;
		p[2] = m_rightValue;
		return 3;
	case 2:
		//3B / r CMP r32,r/m32 Compare r/m32 with r32
		p[0] = 0x3b;
		p[1] = arrReg4[m_leftReg] + m_rightReg;
		return 2;
	}
	return 0;
}

long	COP_cmp::Addressing( long addr )
{ 
	m_lAddress = addr;

	switch( m_Type )
	{
	case 0:		
		{	
				int offset = g_pSymTable->GetOffsetOfVar( int(m_leftVarID) );	
				if( m_funcID == 0 || offset & 0xffffff00 )
					return addr + 10;
				else
					return addr + 7;
				break;
		}
	case 1:		return addr + 3;
	case 2:		return addr + 2;
	}
	return 0;
}

void	COP_cmp::Show( MESSAGE_FUNCTION2 Print )
{
	switch( m_Type )
	{
	case 0:
		Print( "CMP\t\t%s, %s\n", g_pSymTable->GetNameOfVar( int(m_leftVarID) )
						, g_pSymTable->GetNameOfConst( int(m_rightConstID) ) );
		break;
	case 1:
		Print( "CMP\t\t%s, %d\n", g_szArrRegString[m_leftReg], m_rightValue );
		break;
	case 2:
		Print( "CMP\t\t%s, %s\n", g_szArrRegString[m_leftReg], g_szArrRegString[m_rightReg] );
		break;
	}
	
}




///////////////////////////////////////////////////////////////////////////////////
//	COP_ret

int		COP_ret::ToMachineCode( byte * p )
{
	//C3 RET Near return to calling procedure
	p[0] = 0xc3;
	return 1;
}

long	COP_ret::Addressing( long addr )
{ 
	m_lAddress = addr;
	return addr + 1;
}

void	COP_ret::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "RET\n" );
}




///////////////////////////////////////////////////////////////////////////////////
//	COP_xor

COP_xor::COP_xor( eRegister lreg, eRegister rreg )
	: m_leftReg( lreg ), m_rightReg( rreg )
{}

int		COP_xor::ToMachineCode( byte * p )
{
	//33 / r XOR r32,r/m32 r8 XOR r/m8
	p[0] = 0x33;
	p[1] = arrReg4[m_leftReg] + m_rightReg;
	return 2;
}

long	COP_xor::Addressing( long addr )
{ 
	m_lAddress = addr;
	return addr + 2;
}

void	COP_xor::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "XOR\t\t%s, %s\n", g_szArrRegString[m_leftReg], g_szArrRegString[m_rightReg] );
}




///////////////////////////////////////////////////////////////////////////////////
//	COP_setcc

COP_setcc::COP_setcc( eCondition condition, eRegister lreg )
	: m_Condition( condition ), m_Reg( lreg )
{
	if( m_Reg > EBX )
		ScriptSystemError( "setcc 명령에 EAX, ECX, EDX, EBX외의 레지스터를 지정했습니다." );
}

int		COP_setcc::ToMachineCode( byte * p )
{
	//xor reg, reg를 쓰면 EFLAG 레지스터 값을 바꿔버리므로 쓰면 안됨.
	//B8+ rd MOV r32,imm32 Move imm32 to r32
	p[0] = 0xb8 + m_Reg;
	Conversion value( 0 );
	value.Set( p + 1 );


	//setcc	 r/m8
	p[5] = 0x0f;

	switch( m_Condition )
	{
	case G:		//0F 9F SETG r/m8 Set byte if greater (ZF=0 and SF=OF)
			p[6] = 0x9f;
			break;
	case GE:	//0F 9D SETGE r/m8 Set byte if greater or equal (SF=OF)
			p[6] = 0x9d;
			break;
	case L:		//0F 9C SETL r/m8 Set byte if less (SF<>OF)
			p[6] = 0x9c;
			break;
	case LE:	//0F 9E SETLE r/m8 Set byte if less or equal (ZF=1 or SF<>OF)
			p[6] = 0x9e;
			break;
	case E:		//0F 94 SETE r/m8 Set byte if equal (ZF=1)
			p[6] = 0x94;
			break;
	case NE:	//0F 95 SETNE r/m8 Set byte if not equal (ZF=0)
			p[6] = 0x95;
			break;
	}

	p[7] = 0xc0 + m_Reg;

	return 8;
}

long	COP_setcc::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + 8;
}

void	COP_setcc::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "SET%s\t\t%s\n", g_szArrConditionString[m_Condition], g_szArrRegString[m_Reg] );
}

///////////////////////////////////////////////////////////////////////////////////
//	COP_int3

int		COP_int3::ToMachineCode( byte * p )
{
	p[0] = 0xcc;
	return 1;
}

long	COP_int3::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + 1;
}

void	COP_int3::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "INT\t\t3\n" );
}

///////////////////////////////////////////////////////////////////////////////////
//


COP_fld::COP_fld( FUNC funcID, VAR varID )
	: m_funcID( funcID ), m_varID( varID )
{}
	
int		COP_fld::ToMachineCode( byte * p )
{
	//D9 /0 FLD m32real Push m32real onto the FPU register stack.
	return Code_RM32( p, 0xd9, 0, int(m_funcID), int(m_varID), m_lAddress );
}

long	COP_fld::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + Code_RM32_CodeSize( int(m_funcID), int(m_varID) );
}

void	COP_fld::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "FLD\t\t%s\n", g_pSymTable->GetNameOfVar( int(m_varID) ) );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_fadd::COP_fadd( FUNC funcID, VAR varID )
	: m_funcID( funcID ), m_varID( varID )
{}

int		COP_fadd::ToMachineCode( byte * p )
{
	//D8 /0 FADD m32 real Add m32real to ST(0) and store result in ST(0)
	return Code_RM32( p, 0xd8, 0, int(m_funcID), int(m_varID), m_lAddress );
}

long	COP_fadd::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + Code_RM32_CodeSize( int(m_funcID), int(m_varID) );
}

void	COP_fadd::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "FADD\t\t%s\n", g_pSymTable->GetNameOfVar( int(m_varID) ) );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_fsub::COP_fsub( FUNC funcID, VAR varID )
	: m_funcID( funcID ), m_varID( varID )
{}

int		COP_fsub::ToMachineCode( byte * p )
{
	//D8 /4 FSUB m32real Subtract m32real from ST(0) and store result in ST(0)
	return Code_RM32( p, 0xd8, 4, int(m_funcID), int(m_varID), m_lAddress );
}

long	COP_fsub::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + Code_RM32_CodeSize( int(m_funcID), int(m_varID) );
}

void	COP_fsub::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "FSUB\t\t%s\n", g_pSymTable->GetNameOfVar( int(m_varID) ) );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_fmul::COP_fmul( FUNC funcID, VAR varID )
	: m_funcID( funcID ), m_varID( varID )
{}

int		COP_fmul::ToMachineCode( byte * p )
{
	//D8 /1 FMUL m32real Multiply ST(0) by m32real and store result in ST(0)
	return Code_RM32( p, 0xd8, 1, int(m_funcID), int(m_varID), m_lAddress );
}

long	COP_fmul::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + Code_RM32_CodeSize( int(m_funcID), int(m_varID) );
}

void	COP_fmul::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "FMUL\t\t%s\n", g_pSymTable->GetNameOfVar( int(m_varID) ) );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_fdiv::COP_fdiv( FUNC funcID, VAR varID )
	: m_funcID( funcID), m_varID( varID )
{}

int		COP_fdiv::ToMachineCode( byte * p )
{
	//D8 /6 FDIV m32real Divide ST(0) by m32real and store result in ST(0)
	return Code_RM32( p, 0xd8, 6, int(m_funcID), int(m_varID), m_lAddress );
}

long	COP_fdiv::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + Code_RM32_CodeSize( int(m_funcID), int(m_varID) );
}

void	COP_fdiv::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "FDIV\t\t%s\n", g_pSymTable->GetNameOfVar( int(m_varID) ) );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_fstp::COP_fstp( FUNC funcID, VAR varID, bool bDouble )
	: m_funcID( funcID ), m_varID( varID ), m_bDouble( bDouble )
{}

int		COP_fstp::ToMachineCode( byte * p )
{
	if( m_bDouble )
	{
		//DD /3 FSTP m64real Copy ST(0) to m64real and pop register stack
		int offset = g_pSymTable->GetOffsetOfVar( int(m_varID) );
		return Code_RM32_Offset( p, 0xdd, 3, int(m_funcID), m_lAddress, offset - 4 );
	}
	else
	{
		//D9 /3 FSTP m32real Copy ST(0) to m32real and pop register stack
		return Code_RM32( p, 0xd9, 3, int(m_funcID), int(m_varID), m_lAddress );
	}
}

long	COP_fstp::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + Code_RM32_CodeSize( int(m_funcID), int(m_varID) );
}

void	COP_fstp::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "FSTP\t\t%s\n", g_pSymTable->GetNameOfVar( int(m_varID) ) );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_fcomp::COP_fcomp( FUNC funcID, VAR varID )
	: m_funcID( funcID ), m_varID( varID )
{}

int		COP_fcomp::ToMachineCode( byte * p )
{
	//D8 /3 FCOMP m32real Compare ST(0) with m32real and pop register stack.
	return Code_RM32( p, 0xd8, 3, int(m_funcID), int(m_varID), m_lAddress );
}

long	COP_fcomp::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + Code_RM32_CodeSize( int(m_funcID), int(m_varID) );
}

void	COP_fcomp::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "FCOMP\t\t%s\n", g_pSymTable->GetNameOfVar( int(m_varID) ) );
}


///////////////////////////////////////////////////////////////////////////////////
//

int		COP_fnstsw_ax::ToMachineCode( byte * p )
{
	p[0] = 0xdf;
	p[1] = 0xe0;
	return 2;
}

long	COP_fnstsw_ax::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + 2;
}

void	COP_fnstsw_ax::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "FNSTSW\t\tAX\n" );
}

///////////////////////////////////////////////////////////////////////////////////
//


COP_test_ah::COP_test_ah( byte Value8 )
	: m_Value8( Value8 )
{}

int		COP_test_ah::ToMachineCode( byte * p )
{
	//F6 /0 ib TEST r/m8,imm8 AND imm8 with r/m8; set SF, ZF, PF according to result
	p[0] = 0xf6;
	p[1] = 0xc4;
	p[2] = m_Value8;
	return 3;
}

long	COP_test_ah::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + 3;
}

void	COP_test_ah::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "TEST\t\tAH, %d\n", m_Value8 );
}


///////////////////////////////////////////////////////////////////////////////////
//

COP_itoa::COP_itoa( eRegister leftReg, VAR stringBuffer )
	: m_leftReg( leftReg ), m_StringBuffer( stringBuffer )
{}

int		COP_itoa::ToMachineCode( byte * p )
{
	//itoa의 세번째 인자(10) push
	p[0] = 0x6a;				//6A PUSH imm8 Push imm8
	p[1] = 0x0a;

	//itoa의 두번째 인자(문자열 버퍼) push
	p[2] = 0xff;				//FF /6 PUSH r/m32 Push r/m32
	p[3] = 0x35;

	if( g_pSymTable->GetTypeOfVar( int(m_StringBuffer) ) != T_STRING )
		ScriptSystemError( "이것은 String Buffer가 아니자나??!! ( COP_itoa::ToMachineCode() )" );

	int StrBufOffset = g_pSymTable->GetOffsetOfVar( int(m_StringBuffer) );
	Conversion value( StrBufOffset );
	g_pRelocTable->AddGlobalVar( m_lAddress + 4 );
	value.Set( p + 4 );

	//itoa의 첫번째 인자(정수) push
	p[8] = 0x50 + m_leftReg;	//50+ rd PUSH r32 Push r32

	//call itoa
	p[9] = 0xb8 + m_leftReg;	//B8+ rd MOV r32,imm32 Move imm32 to r32
	g_pRelocTable->AddFuncBind( FUNC_ITOA, m_lAddress + 10 );
	Conversion value2( 0 );
	value.Set( p + 10 );

	p[14] = 0xff;				//FF /2 CALL r/m32 Call near, absolute indirect, address given in r/m32
	p[15] = 0xd0 + m_leftReg;

	//add	esp, 12
	p[16] = 0x83;				//83 /0 ib ADD r/m32,imm8 Add sign-extended imm8 to r/m32
	p[17] = 0xc4;
	p[18] = 0x0c;

	return 19;
}

long	COP_itoa::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + 19;
}

void	COP_itoa::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "ITOA\t\t%s, %s\n", g_szArrRegString[m_leftReg]
							, g_pSymTable->GetNameOfConst( int(m_StringBuffer) ) );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_ftoa::COP_ftoa( eRegister freeReg, VAR DoubleVar, VAR stringBuffer )
	: m_freeReg( freeReg ), m_DoubleVar( DoubleVar ), m_StringBuffer( stringBuffer )
{}

int		COP_ftoa::ToMachineCode( byte * p )
{
	int n = 0;

	//gvct의 세 번째 인자(문자열 버퍼) push
	p[n++] = 0xff;				//FF /6 PUSH r/m32 Push r/m32
	p[n++] = 0x35;

	if( g_pSymTable->GetTypeOfVar( int(m_StringBuffer) ) != T_STRING )
		ScriptSystemError( "이것은 String Buffer가 아니자나??!! ( COP_ftoa::ToMachineCode() )" );
	
	int StrBufOffset = g_pSymTable->GetOffsetOfVar( int(m_StringBuffer) );
	Conversion value( StrBufOffset );
	g_pRelocTable->AddGlobalVar( m_lAddress + n );
	value.Set( p + n );
	n += 4;

	//gvct의 두 번째 인자(유효 숫자) push
	p[n++] = 0x6a;				//6A PUSH imm8 Push imm8
	p[n++] = 0x08;				//유효 숫자를 15로 설정.

	//gvct의 첫 번째 인자(실수값) push	-- double을 인자로 받으므로 두번 push함.
	//FF /6 PUSH r/m32 Push r/m32
	n += Code_RM32( p + n, 0xff, 6, 0, int(m_DoubleVar), m_lAddress + n );
	int offset = g_pSymTable->GetOffsetOfVar( int(m_DoubleVar) );
	n += Code_RM32_Offset( p + n, 0xff, 6, 0, m_lAddress + n, offset - 4 );

	//call gvct
	p[n++] = 0xb8 + m_freeReg;	//B8+ rd MOV r32,imm32 Move imm32 to r32
	g_pRelocTable->AddFuncBind( FUNC_GCVT, m_lAddress + n );
	Conversion value2( 0 );
	value2.Set( p + n );
	n += 4;

	p[n++] = 0xff;				//FF /2 CALL r/m32 Call near, absolute indirect, address given in r/m32
	p[n++] = 0xd0 + m_freeReg;

	//add	esp, 16
	p[n++] = 0x83;				//83 /0 ib ADD r/m32,imm8 Add sign-extended imm8 to r/m32
	p[n++] = 0xc4;
	p[n++] = 0x10;

	return n;
}

long	COP_ftoa::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + Code_RM32_CodeSize( 0, int(m_DoubleVar) ) * 2 + 18;
}

void	COP_ftoa::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "FTOA\t\t%s, %s\n", g_pSymTable->GetNameOfVar( int(m_DoubleVar) )
							, g_pSymTable->GetNameOfVar( int(m_StringBuffer) ) );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_malloc::COP_malloc( eRegister reg )
	: m_Reg( reg )
{}

int		COP_malloc::ToMachineCode( byte * p )
{
	for( int i = 0; i < 24; i++ )
	{
		p[i] = 0x90;
	}

	//malloc의 첫번째 인자(할당할 양) push
	p[0] = 0x50 + m_Reg;		//50+ rd PUSH r32 Push r32

	//call malloc
	p[1] = 0xb8 + m_Reg;		//B8+ rd MOV r32,imm32 Move imm32 to r32
	g_pRelocTable->AddFuncBind( FUNC_MALLOC, m_lAddress + 2 );
	Conversion value( 0 );
	value.Set( p + 2 );

	p[6] = 0xff;				//FF /2 CALL r/m32 Call near, absolute indirect, address given in r/m32
	p[7] = 0xd0 + m_Reg;

 	//add	esp, 4
	p[8] = 0x83;				//83 /0 ib ADD r/m32,imm8 Add sign-extended imm8 to r/m32
	p[9] = 0xc4;
	p[10] = 0x04;
	
	p[11] = 0x50 + EAX;

	//RegisterAllocatedMemory 호출
	p[12] = 0x50 + ESI;			//push ESI
	p[13] = 0x50 + EAX;			//push EAX

	p[14] = 0xb8 + ESI;			//B8+ rd MOV r32,imm32 Move imm32 to r32
	g_pRelocTable->AddFuncBind( FUNC_REGISTERALLOCATEDMEMORY, m_lAddress + 15 );

	p[19] = 0xff;				//FF /2 CALL r/m32 Call near, absolute indirect, address given in r/m32
	p[20] = 0xd0 + ESI;

	p[21] = 0x83;				//83 /0 ib ADD r/m32,imm8 Add sign-extended imm8 to r/m32
	p[22] = 0xc4;				//add esp, 4
	p[23] = 0x04;

	p[24] = 0x58 + ESI;			//pop ESI
	p[25] = 0x58 + EAX;			//58+ rd POP r32 Pop top of stack into r32; increment stack pointer

	return 26;
}

long	COP_malloc::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + 26;
}

void	COP_malloc::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "MALLOC\t\t%s\n", g_szArrRegString[m_Reg] );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_free::COP_free( FUNC funcID, VAR varID )
	: m_funcID( funcID ), m_varID( varID )
{}

int		COP_free::ToMachineCode( byte * p )
{
	//free의 첫번째 인자(메모리 주소) push	//FF /6 PUSH r/m32 Push r/m32
	int n = Code_RM32( p, 0xff, 6, int(m_funcID), int(m_varID), m_lAddress );

	//call	free
	p[n++] = 0xb8 + ECX;		//B8+ rd MOV r32,imm32 Move imm32 to r32
	g_pRelocTable->AddFuncBind( FUNC_FREE, m_lAddress + n );
	Conversion value( 0 );
	value.Set( p + n );
	n += 4;

	p[n++] = 0xff;				//FF /2 CALL r/m32 Call near, absolute indirect, address given in r/m32
	p[n++] = 0xd0 + ECX;

 	//add	esp, 4
	p[n++] = 0x83;				//83 /0 ib ADD r/m32,imm8 Add sign-extended imm8 to r/m32
	p[n++] = 0xc4;
	p[n++] = 0x04;

	//UnregisterAllocatedMemory 호출
	n += Code_RM32( p + n, 0xff, 6, int(m_funcID), int(m_varID), m_lAddress + n );
	
	p[n++] = 0xb8 + ECX;		//B8+ rd MOV r32,imm32 Move imm32 to r32
	g_pRelocTable->AddFuncBind( FUNC_UNREGISTERALLOCATEDMEMORY, m_lAddress + n );
	n += 4;

	p[n++] = 0xff;				//FF /2 CALL r/m32 Call near, absolute indirect, address given in r/m32
	p[n++] = 0xd0 + ECX;

	p[n++] = 0x83;				//83 /0 ib ADD r/m32,imm8 Add sign-extended imm8 to r/m32
	p[n++] = 0xc4;
	p[n++] = 0x04;

	return n;
}

long	COP_free::Addressing( long addr )
{
	int n = Code_RM32_CodeSize( int(m_funcID), int(m_varID) );
	m_lAddress = addr;
	return addr + ((n + 10)*2);
}

void	COP_free::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "FREE\t\t%s\n", g_pSymTable->GetNameOfVar( int(m_varID) ) );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_strcpy::COP_strcpy( eRegister lreg, eRegister rreg )
	: m_type( 0 ), m_leftReg( lreg ), m_rightReg( rreg )
{}

COP_strcpy::COP_strcpy( eRegister lreg, FUNC funcID, VAR varID )
	: m_type( 1 ), m_leftReg( lreg ), m_funcID( funcID ), m_varID( varID )
{}

int		COP_strcpy::ToMachineCode( byte * p )
{
	int n = 0;
	//strcpy의 두번째 인자(원본 문자열주소) push
	if( m_type == 0 )
	{
		//50+ rd PUSH r32 Push r32	
		p[n++] = 0x50 + m_rightReg;			
	}
	else
	{
		//FF /6 PUSH r/m32 Push r/m32
		n += Code_RM32( p, 0xff, 6, int(m_funcID), int(m_varID), m_lAddress );
	}
	
	//strcpy의 첫번째 인자(대상 문자열 주소) push
	p[n++] = 0x50 + m_leftReg;

	//call	strcpy
	p[n++] = 0xb8 + m_leftReg;	//B8+ rd MOV r32,imm32 Move imm32 to r32
	g_pRelocTable->AddFuncBind( FUNC_STRCPY, m_lAddress + n );
	Conversion value( 0 );
	value.Set( p + n );
	n += 4;

	p[n++] = 0xff;				//FF /2 CALL r/m32 Call near, absolute indirect, address given in r/m32
	p[n++] = 0xd0 + m_leftReg;

 	//add	esp, 8
	p[n++] = 0x83;				//83 /0 ib ADD r/m32,imm8 Add sign-extended imm8 to r/m32
	p[n++] = 0xc4;
	p[n++] = 0x08;


	return n;
}

long	COP_strcpy::Addressing( long addr )
{
	m_lAddress = addr;
	if( m_type == 0 )
		return addr + 12;
	else
		return addr + Code_RM32_CodeSize( int(m_funcID), int(m_varID) ) + 11;
}

void	COP_strcpy::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "STRCPY\t\t%s,", g_szArrRegString[m_leftReg] );
	if( m_type == 0 )
		Print( " %s\n", g_szArrRegString[m_rightReg] );
	else
		Print( " %s\n", g_pSymTable->GetNameOfVar( int(m_varID) ) );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_strcmp::COP_strcmp( eRegister lreg, eRegister rreg )
	: m_leftReg( lreg ), m_rightReg( rreg )
{}

int		COP_strcmp::ToMachineCode( byte * p )
{
	//strcmp의 두번째 인자 push
	p[0] = 0x50 + m_rightReg;					//50+ rd PUSH r32 Push r32	
	//strcmp의 첫번째 인자 push
	p[1] = 0x50 + m_leftReg;					//50+ rd PUSH r32 Push r32	

	//call strcmp
	p[2] = 0xb8 + m_leftReg;					//B8+ rd MOV r32,imm32 Move imm32 to r32
	g_pRelocTable->AddFuncBind( FUNC_STRCMP, m_lAddress + 3 );
	Conversion value( 0 );
	value.Set( p + 3 );

	p[7] = 0xff;								//FF /2 CALL r/m32 Call near, absolute indirect, address given in r/m32
	p[8] = 0xd0 + m_leftReg;

 	//add	esp, 8
	p[9] = 0x83;								//83 /0 ib ADD r/m32,imm8 Add sign-extended imm8 to r/m32
	p[10] = 0xc4;
	p[11] = 0x08;	

	return 12;
}

long	COP_strcmp::Addressing( long addr )
{
	m_lAddress = addr;
	return addr + 12;
}

void	COP_strcmp::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "STRCMP\t\t%s, %s\n", g_szArrRegString[m_leftReg], g_szArrRegString[m_rightReg] );
}

///////////////////////////////////////////////////////////////////////////////////
//

COP_strlen::COP_strlen( eRegister reg )
	: m_type( 0 ), m_Reg( reg )
{}

COP_strlen::COP_strlen( FUNC funcID, VAR varID )
	: m_type( 1 ), m_funcID( funcID ), m_varID( varID )
{}

int		COP_strlen::ToMachineCode( byte * p )
{
	//strcpy의 첫번째 인자(문자열의 주소) push
	int n = 0;
	if( m_type == 0 )
	{
		//50+ rd PUSH r32 Push r32	
		p[n++] = 0x50 + m_Reg;
	}
	else
	{
		//FF /6 PUSH r/m32 Push r/m32
		n += Code_RM32( p, 0xff, 6, int(m_funcID), int(m_varID), m_lAddress );
	}

	//call strlen
	p[n++] = 0xb8 + EAX;		//B8+ rd MOV r32,imm32 Move imm32 to r32
	g_pRelocTable->AddFuncBind( FUNC_STRLEN, m_lAddress + n );
	Conversion value( 0 );
	value.Set( p + n );
	n += 4;

	p[n++] = 0xff;				//FF /2 CALL r/m32 Call near, absolute indirect, address given in r/m32
	p[n++] = 0xd0 + EAX;

 	//add	esp, 4
	p[n++] = 0x83;				//83 /0 ib ADD r/m32,imm8 Add sign-extended imm8 to r/m32
	p[n++] = 0xc4;
	p[n++] = 0x04;

	return n;
}

long	COP_strlen::Addressing( long addr )
{
	m_lAddress = addr;
	if( m_type == 0 )
		return addr + 11;
	else
		return addr + Code_RM32_CodeSize( int(m_funcID), int(m_varID) ) + 10;
}

void	COP_strlen::Show( MESSAGE_FUNCTION2 Print )
{
	Print( "STRLEN\t\t" );
	if( m_type == 0 )
		Print( "%s\n", g_szArrRegString[m_Reg] );
	else
		Print( "%s\n", g_pSymTable->GetNameOfVar( int(m_varID) ) );
}

///////////////////////////////////////////////////////////////////////////////////
