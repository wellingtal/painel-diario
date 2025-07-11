
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
	void *			m_pGlobalVars;							//���� ���� ����
	void *			m_pStringBuffer;						//m_pBuffer���� Const���۰� ���۵Ǵ� ����
	void *			m_pCodeBuffer;							//m_pBuffer���� Code���۰� ���۵Ǵ� ����
	int				m_iCodeSize;
	FUNCMAP *		m_pFunctionMap;							//Ŀ���� �Լ��� �̸�, ���ڿ� �Լ� ������ ���� ���� ���̺�
	CRelocTable *	m_pRelocation;							//Relocation Table

	int				m_pSysVarOffset[4];						//�ý��� �����鿡 ���� ������ ����.
	char *			m_pSysVarBuffer;
	bool			m_bRelocated;

	ALLOCATED *		m_pAllocatedPtrs;

protected:
	void			SetSysVars();

public:
	CVirtualMachine();	
	~CVirtualMachine();

	void			Create( const char * szFilename );
	void			Create( const void * pDataBuf, unsigned DataSize );		//�޸� �����κ��� ����
	void			Create( CIntermediateCode &, CSymbolTable & );
	void			Destroy();
	bool			Save( const char * szFilename );
	void			Execute();

	//������ ���� T_VOID�� �� �־��־�� ��!!!
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