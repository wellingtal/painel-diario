#ifndef _PARSE_UTILS_H_
#define _PARSE_UTILS_H_

#define CHECK_PACKET_SIZE(x) x

// ��Ŷ ���� �˻�.
#define CHECK_FIXED_PACKETSIZE(lpPktBase, nPacketSize, GameClientDispatch) \
    if((nPacketSize) != (lpPktBase)->GetLen()) \
    { \
        (GameClientDispatch).LogErrorPacket("���̰� �ٸ� ��Ŷ�� ���Խ��ϴ�. ������ ���� ������ �� �ֽ��ϴ�.", (lpPktBase)->GetCmd()); \
        return false; \
    }

// ĳ���� ������ �˻�
#define CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, pktCmd) \
    if(NULL == lpCharacter) \
    { \
        (GameClientDispatch).LogErrorPacket("ĳ���� �����Ͱ� NULL�Դϴ�.", pktCmd); \
        return false; \
    }

#endif