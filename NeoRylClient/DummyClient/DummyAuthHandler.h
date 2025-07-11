#ifndef _DUMMY_AUTH_HANDLER_H_
#define _DUMMY_AUTH_HANDLER_H_

#pragma once

#include <Network/ClientNetwork/ClientEventHandler.h>

// forward delc.
class CCharacterData;
class CSession;

class CDummyAuthHandler : public ClientNet::CClientEventHandler
{
public:

    CDummyAuthHandler(CCharacterData& characterData);
    ~CDummyAuthHandler();

private:

    int OnOpen(int iErrorCode);
    int OnClose();
    int OnDispatch(PktBase* lpPktBase, INET_Addr& peerAddr);
  
    CCharacterData& m_CharacterData;
};


#endif