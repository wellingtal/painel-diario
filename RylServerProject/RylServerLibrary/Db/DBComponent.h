///////////////////////////////////////////////////////////////////////////////////////////////
//
// CDB Component
//
//////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CDBComponent
#define _CDBComponent

#include <DB/OLEDB.h>
#include "DBDefine.h"

//#define _CHECK_DBOBJECT_PERFORMANCE	

#ifdef _CHECK_DBOBJECT_PERFORMANCE
	#define DBOBJECT_PERFORMANCE_CHECK(x) x
#else 
	#define DBOBJECT_PERFORMANCE_CHECK(x) (void*)0
#endif


//////////////////////////////////////////////////////////////////////////////////////////////
//
// DB Componet
//
///////////////////////////////////////////////////////////////////////////////////////////////
class CDBComponent : public OleDB
{
public:

    enum DBClass 
    { 
        Class_AuthDB    = 1, 
        Class_GameDB    = 2,
        Class_KeeperDB  = 3,
		Class_AdminDB	= 4
    };

	enum 
    { 
        MAX_QUERY_LENGTH = 32768, 
        QUERY_BUFFER_LEN = 1024 
    };

    CDBComponent();
    virtual ~CDBComponent();

    bool Connect(DBClass DBClass_In);
    bool Connect(const TCHAR* szDBServerName, const TCHAR* szDBName, 
        const TCHAR* szDBAccount, const TCHAR* szDBPass, 
        OleDB::ConnType connType = OleDB::ConnType_MSSQL);

    // Åø °ü·Ã
    bool Select(const TCHAR *Query_In, void** lpLoginTable_Out, int Size_In, 
        int StartNum_In, int RowNum_In, int *GetRowNum_Out);

    TCHAR* GetQueryBuffer() { return m_Query; }
    const int GetQueryBufferLen() { return MAX_QUERY_LENGTH; }

private:

	TCHAR m_Query[MAX_QUERY_LENGTH];
};


//////////////////////////////////////////////////////////////////////////////////////////////
//
// DB Object Singleon
//
///////////////////////////////////////////////////////////////////////////////////////////////
class CDBSingleObject : public CDBComponent
{
public:
    CDBSingleObject(void) { }
    virtual ~CDBSingleObject(void) { }

    static CDBSingleObject& GetInstance();
};

#endif