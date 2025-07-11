#ifndef _DUMMY_GAME_HANDLER_H_
#define _DUMMY_GAME_HANDLER_H_

#pragma once

#include <Network/ClientNetwork/ClientEventHandler.h>

// forward delc.
class CCharacterData;
class CSession;

class CDummyGameHandler : public ClientNet::CClientEventHandler
{
public:

    CDummyGameHandler(CCharacterData& characterData);
    ~CDummyGameHandler();

private:

    int OnOpen(int iErrorCode);
    int OnClose();
    int OnDispatch(PktBase* lpPktBase, INET_Addr& peerAddr);

    CCharacterData& m_CharacterData;
};


#endif