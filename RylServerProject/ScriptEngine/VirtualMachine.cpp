
#include "VirtualMachine.h"
#include "IntermediateCode.h"
#include "SymbolTable.h"
#include "RelocTable.h"

#include <map>
#include <string>
#include <stdarg.h>
#include <fstream>
#include <set>
#include "GMMemory.h"
typedef unsigned char byte;

static const int	xor_key_value = 0x82fac623;

///////////////////////////////////////////////////////////////////////////////////
//

CVirtualMachine::ALLOCATED *	pAllocatedPtrs = NULL;

void		RegisterAllocatedMemory( void * p )
{
	pAllocatedPtrs->insert( p );
//	CompilerMessage2( "[[[Register Allocated Memory : %d]]]", int(p) );
}

void	UnregisterAllocatedMemory( void * p )
{
	pAllocatedPtrs->erase( p );
//	CompilerMessage2( "[[[Unregister Allocated Memory : %d]]]", int(p) );
}

///////////////////////////////////////////////////////////////////////////////////
//

ScriptFunc::ScriptFunc( void * pfunc, long type )
	: pFunc( pfunc ), Type( type )
{}

CVirtualMachine::CVirtualMachine()
	: m_pBuffer( NULL )	
	, m_pGlobalVars( NULL )
	, m_pStringBuffer( NULL )
	, m_pCodeBuffer( NULL )
	, m_iCodeSize( 0 )
	, m_pFunctionMap( new FUNCMAP )
	, m_pRelocation( new CRelocTable )
	, m_bRelocated( false )
	, m_pAllocatedPtrs( new ALLOCATED )
	, m_pSysVarBuffer( new char[32] )
{
	pAllocatedPtrs = m_pAllocatedPtrs;
}

CVirtualMachine::~CVirtualMachine()
{
	Destroy();
	delete m_pFunctionMap;
	delete m_pRelocation;
	delete m_pAllocatedPtrs;
	delete [] m_pSysVarBuffer;
}

///////////////////////////////////////////////////////////////////////////////////
//

void	Data_XOR( char * pDataBuf, unsigned size, int keyValue )
{
	int * pIntData = (int*)pDataBuf;
	int IntDataSize = size/sizeof(int);

	for( int i = 0; i < IntDataSize; i++ )
	{
		pIntData[i] = pIntData[i] ^ keyValue;
	}
}

///////////////////////////////////////////////////////////////////////////////////
//

void	File_XOR( const char * szSrcFilename, const char * szDstFilename, int keyValue )
{
	fstream infile( szSrcFilename, ios_base::in | ios_base::binary );

	if( !infile.is_open() )
	{
		ErrorMessage2( "ȭ���� �� �� �����ϴ�.(At File_XOR) : %s", szSrcFilename );
	}

	fstream outfile( szDstFilename, ios_base::out | ios_base::binary );

	if( !outfile.is_open() )
	{
		ErrorMessage2( "ȭ���� �� �� �����ϴ�.(At File_XOR) : %s", szDstFilename );
	}

	infile.seekg( 0, ios_base::end );
	unsigned filesize = infile.tellg();
	infile.seekg( 0, ios_base::beg );

	char * pDataBuf = new char[ filesize ];

	infile.read( pDataBuf, filesize );

	Data_XOR( pDataBuf, filesize, keyValue );

	outfile.write( pDataBuf, filesize );

	delete [] pDataBuf;
}

///////////////////////////////////////////////////////////////////////////////////
//

void			CVirtualMachine::Create( const char * szFilename )
{
	ifstream file( szFilename, ios::binary | ios::in );

	if( !file.is_open() )
		ErrorMessage2( "������ ȭ���� ã�� �� �����ϴ�. (At CVirtualMachine::Create) : %s", szFilename );

	file.seekg( 0, ios_base::end );
	unsigned filesize = file.tellg();
	file.seekg( 0, ios_base::beg );

	char * pBuf = new char[filesize];

	int	xor_key_valueT = xor_key_value^0x601f1ac4;

	file.read( pBuf, filesize );
	Data_XOR( pBuf, filesize, xor_key_valueT );

	Create( pBuf, filesize );

	delete [] pBuf;
	file.close();
/*
	Destroy();
	ifstream file( szFilename, ios::binary | ios::in );

	if( !file.is_open() )
		ErrorMessage2( "������ ȭ���� ã�� �� �����ϴ�. (At CVirtualMachine::Create) : %s", szFilename );

	m_pRelocation->Create( file );

	int GlobalVarBufferSize, StringBufferSize, TotalBufferSize;

	file.read( (char*)&GlobalVarBufferSize, sizeof(int) );
	file.read( (char*)&StringBufferSize, sizeof(int) );
	file.read( (char*)&m_iCodeSize, sizeof(int) );

	TotalBufferSize = GlobalVarBufferSize + StringBufferSize + m_iCodeSize;

	m_pGlobalVars = m_pBuffer = new char[TotalBufferSize];
	m_pStringBuffer = ((byte*)m_pGlobalVars) + GlobalVarBufferSize;
	m_pCodeBuffer = ((byte*)m_pStringBuffer) + StringBufferSize;

	memset( m_pGlobalVars, 0, GlobalVarBufferSize );
	file.read( (char*)m_pStringBuffer, StringBufferSize );
	file.read( (char*)m_pCodeBuffer, m_iCodeSize );

	long FuncMapSize;
	file.read( (char*)&FuncMapSize, sizeof(long) );

	string str;
	char endChar;
	long type, offset;

	for( int i = 0; i < FuncMapSize; i++ )
	{
		file >> str;
		file.read( &endChar, 1 );
		file.read( (char*)&type, sizeof(long) );
		file.read( (char*)&offset, sizeof(long) );
		offset += long( m_pCodeBuffer );
		m_pFunctionMap->insert( FUNCMAP::value_type( str, FUNCINFO( type, (void*)offset ) ) );
	}

	file.read( (char*)m_pSysVarOffset, sizeof(int)*4 );
	file.close();

	m_pRelocation->Relocate( m_pGlobalVars, m_pStringBuffer, m_pCodeBuffer );
	m_bRelocated = true;
	SetSysVars();
*/
}

#define READ_INT( p, var )	{ var = *((int*)p); p += sizeof(int); }
#define READ_LONG( p, var )	{ var = *((long*)p); p += sizeof(long); }
#define READ_CHAR( p, var )	{ var = *p; p += sizeof(char); }

const char *	ReadLine( const char * szBuf, char * szOut, char endChar = '\0' )
{
	const char * p = szBuf;
	char * pDst = szOut;

	while( *p != endChar )
	{
		*pDst = *p;

		p++, pDst++;
	}

	*pDst = '\0';

	return p + 1;
}

void			CVirtualMachine::Create( const void * pDataBuf, unsigned DataSize )
{
	Destroy();

	const char * pData = (const char*)m_pRelocation->Create( pDataBuf, DataSize );

	int GlobalVarBufferSize, StringBufferSize, TotalBufferSize;

	READ_INT( pData, GlobalVarBufferSize );
	READ_INT( pData, StringBufferSize );
	READ_INT( pData, m_iCodeSize );

	TotalBufferSize = GlobalVarBufferSize + StringBufferSize + m_iCodeSize;

	m_pGlobalVars = m_pBuffer = new char[TotalBufferSize];
	m_pStringBuffer = ((byte*)m_pGlobalVars) + GlobalVarBufferSize;
	m_pCodeBuffer = ((byte*)m_pStringBuffer) + StringBufferSize;

	memset( m_pGlobalVars, 0, GlobalVarBufferSize );
	memcpy( m_pStringBuffer, pData, StringBufferSize );
	pData += StringBufferSize;
	memcpy( m_pCodeBuffer, pData, m_iCodeSize );
	pData += m_iCodeSize;

	long FuncMapSize;
	READ_LONG( pData, FuncMapSize );

	long type, offset;

	for( int i = 0; i < FuncMapSize; i++ )
	{
		char szStr[256];
		pData = ReadLine( pData, szStr, '\n' );

		READ_LONG( pData, type );
		READ_LONG( pData, offset );

		offset += long( m_pCodeBuffer );
		m_pFunctionMap->insert( FUNCMAP::value_type( szStr, FUNCINFO( type, (void*)offset ) ) );
	}

	memcpy( m_pSysVarOffset, pData, sizeof(int)*4 );
	pData += sizeof(int)*4;

	m_pRelocation->Relocate( m_pGlobalVars, m_pStringBuffer, m_pCodeBuffer );
	m_bRelocated = true;
	SetSysVars();
}

void			CVirtualMachine::Create( CIntermediateCode & IMCode, CSymbolTable & SymbolTable )
{
	Destroy();

	m_iCodeSize = IMCode.Addressing( 0 );

	int GlobalVarsSize = SymbolTable.GetGlobalVarSize();
	int StringBufferSize = SymbolTable.GetStringBufferSize();
	int BufSize = GlobalVarsSize + StringBufferSize + m_iCodeSize;

	m_pGlobalVars = m_pBuffer = new char[ BufSize ];

	memset( m_pGlobalVars, 0, GlobalVarsSize );

	m_pStringBuffer = (char*)m_pBuffer + GlobalVarsSize;
	SymbolTable.StringBuffer( m_pStringBuffer );

	m_pCodeBuffer = (char*)m_pStringBuffer + StringBufferSize;

	if( IMCode.ToMachineCode( m_pCodeBuffer, m_pRelocation ) != m_iCodeSize )
		ScriptSystemError( "���� �ڵ� ������� ���� �ڵ� ����� �ٸ��ϴ�.( at CVirtualMachine::Create )" );

	//�������̼�
	m_pRelocation->Relocate( m_pGlobalVars, m_pStringBuffer, m_pCodeBuffer );
	m_bRelocated = true;

	//�Լ� ���̺� �����
	typedef CIntermediateCode::FUNCTABLE	FUNCTABLE;
	FUNCTABLE & funcTable = IMCode.GetFuncTable();
	
	byte * pCode = (byte*)m_pCodeBuffer;

	for( FUNCTABLE::iterator i = funcTable.begin(); i != funcTable.end(); i++ )
	{
		const char * pFuncName = SymbolTable.GetNameOfFunc( i->first );
		SFuncType FuncType = SymbolTable.GetTypeOfFunc( i->first );
		void * pFunc = pCode + i->second;

		m_pFunctionMap->insert( FUNCMAP::value_type( pFuncName, FUNCINFO( FuncType, pFunc ) ) );
	}

	m_pSysVarOffset[0] = SymbolTable.GetOffsetOfVar( SymbolTable.FindVar( SYSVAR_FLOATUNIT ) );
	m_pSysVarOffset[1] = SymbolTable.GetOffsetOfVar( SymbolTable.FindVar( SYSVAR_TRUE ) );
	m_pSysVarOffset[2] = SymbolTable.GetOffsetOfVar( SymbolTable.FindVar( SYSVAR_FALSE ) );
	m_pSysVarOffset[3] = SymbolTable.GetOffsetOfVar( SymbolTable.FindVar( SYSVAR_CONVBUFFER ) );
	SetSysVars();
}

void			CVirtualMachine::SetSysVars()
{
	int * pVarBuffer = (int*)m_pGlobalVars;

	float floatunit = 1.0f;

	pVarBuffer[ m_pSysVarOffset[0]/4 ] = *((int*)&floatunit);	//0x3f800000
	pVarBuffer[ m_pSysVarOffset[1]/4 ] = (int)strcpy( m_pSysVarBuffer, "true" );
	pVarBuffer[ m_pSysVarOffset[2]/4 ] = (int)strcpy( m_pSysVarBuffer + 5, "false" );
	pVarBuffer[ m_pSysVarOffset[3]/4 ] = int(m_pSysVarBuffer + 11);
}

void			CVirtualMachine::Destroy()
{
	delete [] m_pBuffer;
	m_pBuffer = m_pGlobalVars = m_pStringBuffer = m_pCodeBuffer = NULL;
	m_iCodeSize = 0;
	m_pFunctionMap->clear();
	m_pRelocation->Destroy();

	for( ALLOCATED::iterator i = m_pAllocatedPtrs->begin(); i != m_pAllocatedPtrs->end(); i++ )
	{
		free( *i );
//		CompilerMessage2( "[[[free : %d]]]", int(*i) );
	}
	m_pAllocatedPtrs->clear();
}

///////////////////////////////////////////////////////////////////////////////////
//	ȭ�� ����
//	1. ���� ���� Relocation ����(int)
//	2. ���ڿ� ��� Relocation ����(int)
//	3. ���� ���� Relocation Table
//	4. ���ڿ� ��� Relocation Table
//	5. �������� ���� ũ��(int)
//	6. ���ڿ� ���� ũ��(int)
//	7. �ڵ� ���� ũ��(int)
//	8. ���ڿ� ����
//	9. �ڵ� ����
//	10. �Լ��� ������
//	11. < ���ڿ�, �Լ� Ÿ��(long), ������(long) > * ������
///////////////////////////////////////////////////////////////////////////////////

bool			CVirtualMachine::Save( const char * szFilename )
{
	if( m_pBuffer == NULL || m_pGlobalVars == NULL || m_pStringBuffer == NULL || m_pCodeBuffer == NULL )
		return false;

	ofstream file( szFilename, ios::binary | ios::out );

	if( !file.is_open() )
		ErrorMessage2( "������ ȭ�Ϸ� ������ �� �� �����ϴ�.(At CVirtualMachine::Save) : %s", szFilename );

	if( m_bRelocated )
	{
		m_pRelocation->Unlocate( m_pGlobalVars, m_pStringBuffer, m_pCodeBuffer );
		m_bRelocated = false;
	}

	m_pRelocation->Save( file );

	int GlobalVarBufferSize = ((byte*)m_pStringBuffer) - ((byte*)m_pGlobalVars);
	int	StringBufferSize = ((byte*)m_pCodeBuffer) - ((byte*)m_pStringBuffer);

	file.write( (const char*)&GlobalVarBufferSize, sizeof(int) );
	file.write( (const char*)&StringBufferSize, sizeof(int) );
	file.write( (const char*)&m_iCodeSize, sizeof(int) );

	file.write( (const char*)m_pStringBuffer, StringBufferSize );
	file.write( (const char*)m_pCodeBuffer, m_iCodeSize );

	int FuncMapSize = m_pFunctionMap->size();
	file.write( (const char*)&FuncMapSize, sizeof(int) );

	for( FUNCMAP::iterator it = m_pFunctionMap->begin(); it != m_pFunctionMap->end(); it++ )
	{
		file << it->first << '\n';
		file.write( (const char*)&it->second.first.m_data, sizeof(long) );
		long offset = long( it->second.second ) - long(m_pCodeBuffer);
		file.write( (const char*)&offset, sizeof(long) );
	}

	file.write( (const char*)m_pSysVarOffset, sizeof(int)*4 );

	file.close();

	m_pRelocation->Relocate( m_pGlobalVars, m_pStringBuffer, m_pCodeBuffer );

	char tempFilename[256];
	sprintf( tempFilename, "%s+", szFilename );

	int	xor_key_valueT = xor_key_value^0x601f1ac4;

	File_XOR( szFilename, tempFilename, xor_key_valueT );

	remove( szFilename );
	rename( tempFilename, szFilename );

	return true;
}

void __declspec(safebuffers) CVirtualMachine::Execute()
{
	void * pCodeBuffer = m_pCodeBuffer;
	DWORD szBuffer;
	HANDLE hProcess;
	_MEMORY_BASIC_INFORMATION * mbi;
	DWORD * oldProtect;
	void *tmpAddr;

	hProcess = GetCurrentProcess();
	tmpAddr = VirtualAllocEx(hProcess,NULL,this->m_iCodeSize,MEM_COMMIT + MEM_RESERVE,64);
	memcpy(tmpAddr,pCodeBuffer,this->m_iCodeSize);
	VirtualQueryEx(hProcess,this->m_pCodeBuffer,mbi,sizeof(_MEMORY_BASIC_INFORMATION));	
	oldProtect = addressof(mbi->Protect);
	VirtualProtectEx(hProcess,this->m_pCodeBuffer,this->m_iCodeSize,64,oldProtect);
	memcpy(pCodeBuffer,tmpAddr,this->m_iCodeSize);
	VirtualFreeEx(hProcess,tmpAddr,NULL,MEM_RELEASE);	
	__asm 	call	pCodeBuffer;	

}

SFuncType		MakeFuncType( eDataType returnType, va_list args )
{
	SFuncType type;

	type.SetReturnType( returnType );

	for( int i = 0; i < 7; i++ )
	{	
		eDataType dataType = va_arg( args, eDataType );
		
		if( dataType == T_VOID )
			break;

		type.SetArgType( i, dataType );
	}

	return type;
}

void *			GetFuncPtr( CVirtualMachine::FUNCMAP & funcMap, const char * szFuncName, SFuncType funcType )
{
	typedef CVirtualMachine::FUNCMAP::iterator iterator;

	pair< iterator, iterator > result = funcMap.equal_range( szFuncName );

	while( result.first != result.second )
	{
		SFuncType ftype = result.first->second.first;
		if( ftype == funcType )
		{
			return result.first->second.second;
		}
		result.first++;
	}	
	return NULL;
}

union long_byte
{
	long	longValue;
	char	byteValue[4];
};

///////////////////////////////////////////////////////////////////////////////////
//Native�Լ��� ��ũ��Ʈ�� ���ε� �ϴ� ���
//
//���� ��� : �Լ��� �� �� ��ġ�� �Ѵ�. ( ���� �ӵ��� �� �� �������� �� �����ϴ�. )
//		1-1. ���� �ְ�, ���ǰ� ���� �Լ��鸸 �Լ� �ڵ忡�� ó�� �κп� 5����Ʈ �̻��� ����(nop)�� �����д�.
//		1-2. RegisterFunction�� ȣ����� �� 1-1���� ������ ���� �κп� call <native function>�� ret�� ����ִ´�.
//		�󸶳� ������?  ��� �Լ��� nop�� 2�� �̻� �߰��ǰ�, native call�� ���� ȣ���� �ȴ�.
//		Empty Function�� �����Ѵ�.
//
//�ٸ� ��� : ����Ϸ��� �Լ��� ȣ���ϴ� ��� �κ��� �Լ� �ּҸ� �ٲ۴�.( ���� �ӵ��� ������ �����ϴ�. )
//		2-1. ��� �Լ� ȣ�� �κ��� Relocation Tableó�� call table�� �����صд�.
//		2-2. RegisterFunction�� ȣ��Ǿ��� �� call table�� �����ؼ� call�ϰ� �ִ� �κ��� ���� �ٲ۴�.
//		call table��?  map< string, pair< SFuncType, vector<int> > >
///////////////////////////////////////////////////////////////////////////////////


void			CVirtualMachine::RegisterFunction( ANY_FUNCTION FuncPtr, eDataType returnType, const char * szFuncName, ... )
{
	va_list args;
	va_start( args, szFuncName );
	
	RegisterFunction( FuncPtr, returnType, szFuncName, args );
}

void			CVirtualMachine::RegisterFunction( ANY_FUNCTION FuncPtr, eDataType returnType, const char * szFuncName, va_list args )
{
	SFuncType funcType = MakeFuncType( returnType, args );

	byte * pFunc = (byte*)GetFuncPtr( *m_pFunctionMap, szFuncName, funcType );
	
	if( pFunc == NULL )
		ErrorMessage2( "�Լ� ��Ͽ� �����߽��ϴ�. �Լ��� ã�� �� �������ϴ�. : %s", funcType.ToString( szFuncName ) );

	if( pFunc[3] != 0x90 )												//nop�� �ƴ� ��
		ScriptSystemError( "RegisterFunction Error!!( at CVirtualMachine::RegisterFunction )" );

	int nArg = funcType.GetArgCount();
	pFunc += 3;

	for( int i = 0; i < nArg; i++ )
	{
		*(pFunc++) = 0xff;
		*(pFunc++) = 0x75;
		*(pFunc++) = 0xff & (8+4*i);
	}

	*(pFunc++) = 0xe8;
	long_byte FuncOffset;
	FuncOffset.longValue = long(FuncPtr) - long(pFunc+4);

	*(pFunc++) = FuncOffset.byteValue[0];
	*(pFunc++) = FuncOffset.byteValue[1];
	*(pFunc++) = FuncOffset.byteValue[2];
	*(pFunc++) = FuncOffset.byteValue[3];
}

ScriptFunc		CVirtualMachine::GetScriptFunction( eDataType returnType, const char * szFuncName, ... )
{
	va_list args;
	va_start( args, szFuncName );
	return GetScriptFunction( returnType, szFuncName, args );
}

ScriptFunc		CVirtualMachine::GetScriptFunction( eDataType returnType, const char * szFuncName, va_list args )
{
	SFuncType funcType = MakeFuncType( returnType, args );

	void * pFunc = GetFuncPtr( *m_pFunctionMap, szFuncName, funcType );
	if( pFunc == NULL )
		ErrorMessage2( "��ũ��Ʈ �Լ��� �����µ� �����߽��ϴ�. �Լ��� ã�� �� �������ϴ�. : %s", funcType.ToString( szFuncName ) );

	return ScriptFunc( pFunc, funcType.m_data );
}

void *  			CVirtualMachine::CallScriptFunction( ScriptFunc Func )
{
	__asm	call	Func.pFunc;
}

void * 				CVirtualMachine::CallScriptFunction( ScriptFunc Func, AnyData arg1 )
{
	__asm
	{
		push	arg1;
		call	Func.pFunc;
		add		esp, 4;
	}
}

void * 				CVirtualMachine::CallScriptFunction( ScriptFunc Func, AnyData arg1, AnyData arg2 )
{
	__asm
	{
		push	arg2;
		push	arg1;
		call	Func.pFunc;
		add		esp, 8;
	}
}

void * 			CVirtualMachine::CallScriptFunction( ScriptFunc Func, AnyData arg1, AnyData arg2, AnyData arg3 )
{
	__asm
	{
		push	arg3;
		push	arg2;
		push	arg1;
		call	Func.pFunc;
		add		esp, 12;
	}
}

void * 			CVirtualMachine::CallScriptFunction( ScriptFunc Func, AnyData arg1, AnyData arg2, AnyData arg3, AnyData arg4 )
{
	__asm
	{
		push	arg4;
		push	arg3;
		push	arg2;
		push	arg1;
		call	Func.pFunc;
		add		esp, 16;
	}
}

void * 				CVirtualMachine::CallScriptFunction( ScriptFunc Func, AnyData args[], int nArgs )
{
	for( int i = nArgs - 1; i >= 0; i-- )
		__asm	push	args[i];

	__asm	call	Func.pFunc;

	nArgs *= 4;

	__asm	add		esp, nArgs;
}

/*

int				CVirtualMachine::CallScriptFunction( ScriptFunc Func, va_list args )
{
	int nArgs = SFuncType( Func.Type ).GetArgCount();

	for( int i = 0; i < nArgs; i++ )
	{
		AnyData data = va_arg( args, AnyData );
		__asm	push	data;
	}

	void * pFunc = Func.pFunc;

	int n = nArgs * 4;

	__asm
	{
		call	pFunc;
		add		esp, n;
	}
}

int				CVirtualMachine::CallScriptFunction( ScriptFunc Func, ... )
{
	va_list args;
	va_start( args, Func );

	return CallScriptFunction( Func, args );
}

*/