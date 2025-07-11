#ifndef _DBAGENT_SERVER_GAME_DISPATCH_H_
#define _DBAGENT_SERVER_GAME_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/MultiDispatchStorage.h>

#include <Item/ItemSerialMgr.h>

// forward decl.
struct PktSL;
struct PktBase;
struct PktUK;
struct PktAdmin;

namespace DBAgent
{
    class CGameDispatch : public CRylServerDispatch
    {
    public:

        static CMultiDispatch& GetDispatchTable();

        enum Const
        {
			// edith 2008.03.04 CGameDispatch 수정
            MAX_PACKET_DISPATCH_PER_PULSE   = 100,
            MAX_STREAM_BUFFER_SIZE          = 16000
        };

        CGameDispatch(CSession& Session);
        virtual ~CGameDispatch();

        virtual void Connected();
        virtual void Disconnected();
        virtual bool DispatchPacket(PktBase* lpPktBase);
        
        void IncCharNum(int nRace);
        void DecCharNum(int nRace);

        int GetCharNum(int nRace) const;    // 항상 실 인원수보다 한명이 많다.
        int GetCharNum() const;             // 항상 실 인원수보다 한명이 많다.

        unsigned long GetServerID() const { return m_dwServerID; }

    private:

        bool ServerLogin(PktBase* lpPktBase);
        bool ServerLogout(PktBase* lpPktBase);

        bool UserKillAck(PktBase* lpPktBase);
        bool CharAdminCmd(PktBase* lpPktBase);
        bool GiveItemToTempInven(PktBase* lpPktBase);
        bool ChangeName(PktBase* lpPktBase);

        Item::CItemSerialMgr    m_GameItemSerialMgr;

        // 인원수는 항상 한명이 많게 관리하고, 보여줄때만 1씩 빼서 보여준다.
        // 그 이유는 채널의 존재 여부를 인원수가 0인지 아닌지로 파악하기 때문이다.
        int m_nHumanNum;    
        int m_nAkhanNum;        

        unsigned long m_dwServerID;
    };

    
    // 전체 캐릭터 수를 얻어오는 함수자이다. CMultiDispatch::Process 에 사용한다.
    class CGetTotalCount
    {
    public:

        CGetTotalCount(unsigned short& usHumanCount, unsigned short& usAkhanCount, 
            unsigned char& cChannelCount);
        bool operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch);

    private:

        unsigned short& m_usHumanCount;
        unsigned short& m_usAkhanCount;
        unsigned char&  m_cChannelCount;
    };

    // 존당 캐릭터 수 퍼센티지를 얻어오는 함수자이다. CMultiDispatch::Process 에 사용한다.
    class CUserPercentageInZone
    {
    public:

        CUserPercentageInZone(unsigned short* lpChannelUserNum,
            unsigned char cZone, unsigned char cChannelNum);
        bool operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch);

    private:

        int             m_nMaxUserNum;
        unsigned short* m_lpChannelUserNum;
        unsigned char   m_cZone;
        unsigned char   m_cChannelNum;
    };

    // 특정 존의 모든 채널에 패킷을 보낸다.
    class CSendPacketToZone
    {
    public:

        CSendPacketToZone(const char* szData, unsigned long dwDataLen, 
            unsigned char cPacketCmd, unsigned char cZone);
        bool operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch);

    private:

        const char*     m_szData;
        unsigned long   m_dwDataLen;
        unsigned char   m_cPacketCmd;
        unsigned char   m_cZone;
    };

}

#endif