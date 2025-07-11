#include "stdafx.h"
#include "DBDataMgr.h"

namespace DBAgent
{
namespace DataStorage
{

const char* g_szDBDataErrorString[MAX_DATA_ERROR] = 
{
    "DATA_SUCCEEDED",               //! 성공
    "DATA_SERVER_ERROR",            //! 메모리 할당 오류, 맵 삽입 오류 등
    "DATA_ALREADY_LOGIN_ERROR",     //! 이미 로그인 해 있음
    "DATA_ALREADY_LOGOUT_ERROR"     //! 이미 로그아웃 해 있음    
};

}
}
