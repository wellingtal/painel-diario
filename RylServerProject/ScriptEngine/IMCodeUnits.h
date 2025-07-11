
#ifndef _IMCodeUnits_H_
#define _IMCodeUnits_H_

#include "BaseDef.h"
#include "Message.h"
#include "STL.h"

class CSymbolTable;
class CRelocTable;
typedef map<int, long>		FUNCTABLE;


///////////////////////////////////////////////////////////////////////////////////
//

void	SetSymbolTable( CSymbolTable * );
void	SetFuncTable( FUNCTABLE * );
void	SetRelocTable( CRelocTable * );


///////////////////////////////////////////////////////////////////////////////////
//

typedef unsigned char byte;

enum eRegister
{
	EAX = 0,
	ECX,
	EDX,
	EBX,
	ESP,
	EBP,
	ESI,
	EDI = 7,
	NONE = 8
};

enum eCondition
{
	G = 0,	//Greater
	GE,		//Greater or Equal
	L,		//Less
	LE,		//Less or Equal
	E,		//Equal
	NE = 5	//Not Equal
};

typedef struct _VAR_* VAR;
typedef struct _CONST_* CONST;
typedef struct _FUNC_* FUNC;



///////////////////////////////////////////////////////////////////////////////////
//

class IOPCode
{
public:
	long	m_lAddress;

public:
	virtual ~IOPCode()	{}									//ToMachineCode ȣ�� ���� Addressing�� �ݵ�� �Ǿ� �־�� �Ѵ�.
	virtual int		ToMachineCode( byte * ) = 0;			//�Ѱܹ��� �ּҿ� ���� �ڵ带 ���� index�� ������Ų��.
	virtual long	Addressing( long ) = 0;					//���� �ּҸ� �ް�, ���� instruction�� �ּҸ� �����Ѵ�.
	virtual void	Show( MESSAGE_FUNCTION2 ) = 0;
};


///////////////////////////////////////////////////////////////////////////////////
//

class COP_call : public IOPCode
{
	FUNC	m_funcID;
	int		m_iLine;

public:
	COP_call( FUNC, int line );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_mov : public IOPCode
{
	byte			m_Type;
	FUNC			m_funcID;

	union
	{
		VAR			m_leftVarID;
		eRegister	m_leftReg;
	};

	union
	{
		int			m_midArrayIndex;
		eRegister	m_midIndexReg;
	};

	union
	{
		eRegister	m_rightReg;
		CONST		m_rightConstID;
		VAR			m_rightVarID;
		int			m_rightValue;
		float		m_rightValuef;
	};
public:
	COP_mov( FUNC, VAR, eRegister reg );
	COP_mov( eRegister reg, FUNC, VAR );
	COP_mov( eRegister reg, CONST constID );
	COP_mov( FUNC, VAR, float valuef );
	COP_mov( eRegister lreg, eRegister rreg );
	COP_mov( eRegister lreg, int value );
	COP_mov( FUNC, VAR, int arrayIndex, eRegister rreg );
	COP_mov( eRegister lreg, FUNC, VAR, eRegister indexReg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_nop : public IOPCode
{
public:
	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_add : public IOPCode
{
	byte		m_Type;
	eRegister	m_leftReg;
	union
	{
		int			m_iValue;
		eRegister	m_rightReg;
	};

public:
	COP_add( eRegister reg, int value );
	COP_add( eRegister lreg, eRegister rreg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_sub : public IOPCode
{
	byte		m_Type;
	eRegister	m_leftReg;
	union
	{
		eRegister	m_rightReg;
		int			m_iValue;
	};
public:
	COP_sub( eRegister reg, int value );
	COP_sub( eRegister lreg, eRegister rreg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_imul : public IOPCode
{
	eRegister	m_leftReg;
	eRegister	m_rightReg;

public:
	COP_imul( eRegister lreg, eRegister rreg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_idiv : public IOPCode
{
	eRegister	m_rightReg;

public:
	COP_idiv( eRegister rreg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_push : public IOPCode
{
	eRegister	m_Reg;

public:
	COP_push( eRegister reg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_pop : public IOPCode
{
	eRegister	m_Reg;

public:
	COP_pop( eRegister reg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_jmp : public IOPCode
{
public:
	IOPCode *	m_pJumpTarget;

public:
	COP_jmp();
	COP_jmp( IOPCode * pJmpTarget );
	virtual ~COP_jmp();

	virtual int		ToMachineCode( byte * );
	virtual long	Addressing( long addr );
	virtual void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_jcc : public COP_jmp
{
	eCondition		m_Condition;

public:
	COP_jcc( eCondition condition );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_jmpmark : public IOPCode
{
public:
	IOPCode *	m_pJumpSrc;
public:
	COP_jmpmark();
	COP_jmpmark( COP_jmp * pOPjmp );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_cmp : public IOPCode
{
	byte	m_Type;
	FUNC	m_funcID;	
	
	union
	{
		VAR			m_leftVarID;
		eRegister	m_leftReg;
	};

	union
	{
		byte		m_rightValue;
		CONST		m_rightConstID;
		eRegister	m_rightReg;
	};

public:
	COP_cmp( FUNC, VAR lvarID, CONST rconstID );
	COP_cmp( eRegister lreg, byte value );	
	COP_cmp( eRegister lreg, eRegister rreg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_ret : public IOPCode
{
public:
	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

class COP_xor : public IOPCode
{
	eRegister	m_leftReg;
	eRegister	m_rightReg;

public:
	COP_xor( eRegister lreg, eRegister rreg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_setcc : public IOPCode
{
	eCondition	m_Condition;
	eRegister	m_Reg;

public:
	COP_setcc( eCondition condition, eRegister lreg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );

};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_int3 : public IOPCode
{

public:
	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_fld : public IOPCode
{
	FUNC	m_funcID;
	VAR		m_varID;
public:
	COP_fld( FUNC, VAR );
	
	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_fadd : public IOPCode
{
	FUNC	m_funcID;
	VAR		m_varID;
public:
	COP_fadd( FUNC, VAR );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_fsub : public IOPCode
{
	FUNC	m_funcID;
	VAR		m_varID;
public:
	COP_fsub( FUNC, VAR );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_fmul : public IOPCode
{
	FUNC	m_funcID;
	VAR		m_varID;
public:
	COP_fmul( FUNC, VAR );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_fdiv : public IOPCode
{
	FUNC	m_funcID;
	VAR		m_varID;
public:
	COP_fdiv( FUNC, VAR );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_fstp : public IOPCode
{
	FUNC	m_funcID;
	VAR		m_varID;
	bool	m_bDouble;
public:
	COP_fstp( FUNC, VAR, bool = false );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_fcomp : public IOPCode
{
	FUNC	m_funcID;
	VAR		m_varID;
public:
	COP_fcomp( FUNC, VAR );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_fnstsw_ax : public IOPCode
{
public:
	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_test_ah : public IOPCode
{
	byte		m_Value8;
public:
	COP_test_ah( byte );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_itoa : public IOPCode
{
	eRegister		m_leftReg;
	FUNC			m_funcID;
	VAR 			m_StringBuffer;

public:
	COP_itoa( eRegister, VAR );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_ftoa : public IOPCode
{
	eRegister		m_freeReg;
	VAR				m_DoubleVar;
	VAR				m_StringBuffer;

public:
	COP_ftoa( eRegister, VAR, VAR );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

//�Ҵ�� �޸��� �ּҰ� EAX�� ����.
class COP_malloc : public IOPCode
{
	eRegister	m_Reg;

public:
	COP_malloc( eRegister );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_free : public IOPCode
{
	FUNC	m_funcID;
	VAR		m_varID;

public:
	COP_free( FUNC, VAR );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

class COP_strcpy : public IOPCode
{
	int			m_type;
	eRegister	m_leftReg;
	FUNC		m_funcID;
	union
	{
		eRegister	m_rightReg;
		VAR			m_varID;
	};

public:
	COP_strcpy( eRegister, eRegister );
	COP_strcpy( eRegister, FUNC, VAR );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

//ptr[leftReg]�� ptr[rightReg]�� ���ڿ��� ���ؼ� ������� eax�� �ִ´�.
class COP_strcmp : public IOPCode
{
	eRegister	m_leftReg;
	eRegister	m_rightReg;

public:
	COP_strcmp( eRegister lreg, eRegister rreg );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////
//

//ptr [rightReg] �� �ִ� ���ڿ��� ���̸� ���� leftReg�� �ִ´�.
class COP_strlen : public IOPCode
{
	int			m_type;
	FUNC		m_funcID;
	union
	{
		eRegister	m_Reg;
		VAR			m_varID;
	};

public:
	COP_strlen( eRegister reg );
	COP_strlen( FUNC, VAR );

	int		ToMachineCode( byte * );
	long	Addressing( long addr );
	void	Show( MESSAGE_FUNCTION2 Print );
};

///////////////////////////////////////////////////////////////////////////////////

#endif