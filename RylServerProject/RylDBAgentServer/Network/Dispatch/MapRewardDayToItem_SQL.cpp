#include "MapRewardDayToItem.h"
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <fstream>
#include <sstream>

std::string GetConnectionStringFromINI() {
    std::ifstream file("Config/dbconfig.ini");
    std::string line, server, db, user, pass;
    while (std::getline(file, line)) {
        if (line.find("Server=") != std::string::npos) server = line.substr(7);
        if (line.find("Database=") != std::string::npos) db = line.substr(9);
        if (line.find("User=") != std::string::npos) user = line.substr(5);
        if (line.find("Password=") != std::string::npos) pass = line.substr(9);
    }
    std::ostringstream conn;
    conn << "DRIVER={SQL Server};SERVER=" << server
         << ";DATABASE=" << db
         << ";UID=" << user
         << ";PWD=" << pass;
    return conn.str();
}

int FallbackRewardItem(int day) {
    switch (day) {
        case 1: return 9002;
        case 2: return 9004;
        case 3: return 9008;
        case 4: return 9010;
        case 5: return 9012;
        case 6: return 9014;
        case 7: return 9016;
        default: return 0;
    }
}

int MapRewardDayToItem(int day) {
    SQLHENV hEnv = NULL;
    SQLHDBC hDbc = NULL;
    SQLHSTMT hStmt = NULL;
    SQLRETURN ret;
    int prototypeID = 0;

    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return FallbackRewardItem(day);

    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return FallbackRewardItem(day);

    std::string connStr = GetConnectionStringFromINI();
    SQLCHAR outStr[1024];
    SQLSMALLINT outStrLen;
    ret = SQLDriverConnect(hDbc, NULL, (SQLCHAR*)connStr.c_str(), SQL_NTS, outStr, sizeof(outStr), &outStrLen, SQL_DRIVER_COMPLETE);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return FallbackRewardItem(day);

    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return FallbackRewardItem(day);

    std::string query = "SELECT TOP 1 PrototypeID FROM Product WHERE Sort = " + std::to_string(day);
    ret = SQLExecDirect(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER id;
        SQLLEN indicator;
        if (SQLFetch(hStmt) == SQL_SUCCESS) {
            SQLGetData(hStmt, 1, SQL_C_SLONG, &id, 0, &indicator);
            prototypeID = id;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    return (prototypeID > 0) ? prototypeID : FallbackRewardItem(day);
}
