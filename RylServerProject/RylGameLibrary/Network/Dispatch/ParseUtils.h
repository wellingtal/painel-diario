#ifndef _PARSE_UTILS_H_
#define _PARSE_UTILS_H_

#define CHECK_PACKET_SIZE(x) x

// 패킷 길이 검사.
#define CHECK_FIXED_PACKETSIZE(lpPktBase, nPacketSize, GameClientDispatch) \
    if((nPacketSize) != (lpPktBase)->GetLen()) \
    { \
        (GameClientDispatch).LogErrorPacket("길이가 다른 패킷이 들어왔습니다. 서버에 대한 공격일 수 있습니다.", (lpPktBase)->GetCmd()); \
        return false; \
    }

// 캐릭터 포인터 검사
#define CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, pktCmd) \
    if(NULL == lpCharacter) \
    { \
        (GameClientDispatch).LogErrorPacket("캐릭터 포인터가 NULL입니다.", pktCmd); \
        return false; \
    }

#endif