#ifndef _FIELD_SERVER_CLIENT_DISPATCH_H_
#define _FIELD_SERVER_CLIENT_DISPATCH_H_

#include <Network/Dispatch/GameClient/GameClientDispatch.h>


class CFieldGameClientDispatch : public CGameClientDispatch
{
public:

    CFieldGameClientDispatch(CSession& Session);	
};

#endif