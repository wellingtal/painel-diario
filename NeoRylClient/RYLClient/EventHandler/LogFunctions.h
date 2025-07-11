#ifndef _GAME_CLIENT_LOG_FUNCTIONS_H_
#define _GAME_CLIENT_LOG_FUNCTIONS_H_

// 전방 참조
struct sockaddr_in;

namespace GameClientLog
{
    void LogAddress(unsigned long dwID, unsigned long dwCharID, const sockaddr_in& PublicAddr, const sockaddr_in& PrivateAddr);
};

#endif