#include "stdafx.h"
#include "DBDataMgr.h"

namespace DBAgent
{
namespace DataStorage
{

const char* g_szDBDataErrorString[MAX_DATA_ERROR] = 
{
    "DATA_SUCCEEDED",               //! ����
    "DATA_SERVER_ERROR",            //! �޸� �Ҵ� ����, �� ���� ���� ��
    "DATA_ALREADY_LOGIN_ERROR",     //! �̹� �α��� �� ����
    "DATA_ALREADY_LOGOUT_ERROR"     //! �̹� �α׾ƿ� �� ����    
};

}
}
