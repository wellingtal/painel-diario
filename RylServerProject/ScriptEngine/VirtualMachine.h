
#ifndef _VirtualMachine_H_
#define _VirtualMachine_H_

#include "STL.h"
#include "BaseDef.h"

class CIntermediateCode;
class CSymbolTable;
struct SFuncType;
class CRelocTable;
typedef char *  va_list;

///////////////////////////////////////////////////////////////////////////////////
//

void	RegisterAllocatedMemory( void * );
void	UnregisterAllocatedMemory( void * );

///////////////////////////////////////////////////////////////////////////////////
//

class CVirtualMachine
{
public:
	typedef pair<SFuncType, void*>		FUNCINFO;
	typedef	multimap<string, FUNCINFO>	FUNCMAP;
	typedef set<void*>					ALLOCATED;

protected:
	void *			m_pBuffer;
	void *			m_pGlobalVars;							//전역 변수 버퍼
	void *			m_pStringBuffer;						//m_pBuffer에서 Const버퍼가 시작되는 지점
	void *			m_pCodeBuffer;							//m_pBuffer에서 Code버퍼가 시작되는 지점
	int				m_iCodeSize;
	FUNCMAP *		m_pFunctionMap;							//커스텀 함수의 이름, 인자와 함수 오프셋 간의 매핑 테이블
	CRelocTable *	m_pRelocation;							//Relocation Table

	int				m_pSysVarOffset[4];						//시스템 변수들에 대한 오프셋 값들.
	char *			m_pSysVarBuffer;
	bool			m_bRelocated;

	ALLOCATED *		m_pAllocatedPtrs;

protected:
	void			SetSysVars();

public:
	CVirtualMachine();	
	~CVirtualMachine();

	void			Create( const char * szFilename );
	void			Create( const void * pDataBuf, unsigned DataSize );		//메모리 블럭으로부터 생성
	void			Create( CIntermediateCode &, CSymbolTable & );
	void			Destroy();
	bool			Save( const char * szFilename );
	void			Execute();

	//인자의 끝에 T_VOID를 꼭 넣어주어야 함!!!
	void			RegisterFunction( ANY_FUNCTION, eDataType returnType, const char *, ... );
	void			RegisterFunction( ANY_FUNCTION, eDataType, const char *, va_list );
	ScriptFunc		GetScriptFunction( eDataType returnType, const char *, ... );
	ScriptFunc		GetScriptFunction( eDataType, const char *, va_list );

	void *			CallScriptFunction( ScriptFunc );
	void *			CallScriptFunction( ScriptFunc, AnyData );
	void *			CallScriptFunction( ScriptFunc, AnyData, AnyData );
	void *			CallScriptFunction( ScriptFunc, AnyData, AnyData, AnyData );
	void *			CallScriptFunction( ScriptFunc, AnyData, AnyData, AnyData, AnyData );
	void *			CallScriptFunction( ScriptFunc, AnyData args[], int nArgs );
//	int				CallScriptFunction( ScriptFunc, va_list );
//	int				CallScriptFunction( ScriptFunc, ... );
};

///////////////////////////////////////////////////////////////////////////////////

#endif