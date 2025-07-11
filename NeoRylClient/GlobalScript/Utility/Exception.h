#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_


/*
// ��ü �Լ� Exception ���
Nave::Exception::EnableException(true);
// Dump ����
Nave::Exception::EnableSaveDump(true);

*/ 

#include <string>
#include <ostream>

void MiniDump(const char* filename, PEXCEPTION_POINTERS ep);

class Exception
{
private:
	enum {
		BufferSize=1024
	};
	char buffer[BufferSize];
	void set(const char* message);

public:
	Exception(const char* message);
	Exception(const std::string& message);

	virtual ~Exception(void);

	virtual const char* What() const;

public:
	enum MiniDumpTypes 
	{ 
		NormalDump=0, DataSegs, HeapDump 
	};

	static char DefaultDumpFileName[512];
	static MiniDumpTypes MiniDumpType;
	static bool ExceptionSaveDump;
	static bool ExecptionAutoThrow;

	static LPTOP_LEVEL_EXCEPTION_FILTER lpPreviousFilter;

	static void SetDumpType(MiniDumpTypes type);
	static void EnableSaveDump(bool enable = true);
	static void SetDumpName(char* strDumpName) { strcpy(DefaultDumpFileName, strDumpName); }

	// EnableUnhandledExceptioinFilter �̰� ����Ұ�� 
	// try~catch�� ������ ���� ������ ���ܰ� �߻��Ҷ� ���α׷��� Ư���ൿ��
	// �ϰ� ����ɼ� �ִٷ� �����Ҽ��ִ�.
	// EnableSaveDump �� ������ ���� ������ ������, dmp�� ����� ������ 
	// �����Ѵ�.
	// ����� ������ ����� ��Ȱ�� �Ѵ�.
	// ���߿� �����鼭 Ư�� ���ø����̼��� ���� �ϵ��� ó���Ҽ� �ִ�.
	static void EnableUnhandledExceptioinFilterEx(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter, bool enable = true);
	static void EnableUnhandledExceptioinFilter(bool enable = true);

	// ������ Dump������ ����� ���� ���
	static void MiniDump(const char* filename);
};

#endif